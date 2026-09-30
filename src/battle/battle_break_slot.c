#include "battle/battle_break_slot.h"

#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "battle/battle_camera.h"
#include "battle/battle_disp.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "manager/evtmgr.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "system.h"
#include <dolphin/gx.h>
#include <string.h>

typedef struct SlotCards8 {
    s32 v[8];
} SlotCards8;

typedef struct SlotCards5 {
    s32 v[5];
} SlotCards5;

struct BattleBreakSlotReel {
    s32 type;       // 0x00
    u32 flags;      // 0x04
    s32 state;      // 0x08
    s32 field_0C;   // 0x0C
    s32 value;      // 0x10
    s32* cards;     // 0x14
    s32 count;      // 0x18
    s32 index;      // 0x1C
    f32 x;          // 0x20
    f32 y;          // 0x24
    f32 z;          // 0x28
    f32 rotX;       // 0x2C
    f32 rotY;       // 0x30
    f32 rotZ;       // 0x34
    f32 scaleX;     // 0x38
    f32 scaleY;     // 0x3C
    f32 scaleZ;     // 0x40
    f32 field_44;   // 0x44
    f32 field_48;   // 0x48
    f32 field_4C;   // 0x4C
    f32 field_50;   // 0x50
    u8 field_54[4]; // 0x54
    u8 field_58;    // 0x58
    u8 field_59[3]; // 0x59
};

struct BattleBreakSlot {
    u32 flags;      // 0x00
    s32 state;      // 0x04
    s32 field_08;   // 0x08
    s32 counter;    // 0x0C
    s32 breakTurn;  // 0x10
    f32 field_14;   // 0x14
    BattleBreakSlotReel reels[3]; // 0x18
    s32 sound;      // 0x12C
    EventEntry* event; // 0x130
};


// The original emits real calls to BattleBreakSlotGetPtr/ReelGetPtr.
#pragma dont_inline on

const char str_802FD6C0[] = "recovery";
const char str_802FD6CC[] = "msg_slot_HP_max";
const char str_802FD6DC[] = "msg_slot_FP_max";
const char str_802FD6EC[] = "msg_slot_SP_max";
const char str_802FD6FC[] = "msg_slot_HPFPSP_max";
const char str_802FD710[] = "msg_slot_HPFPSP_half";

// The original has these constants in .sdata2 (const). Declared const, MWCC folds their values into its own
// pool and BattleBreakSlotDispReel drops from 68% to 64%, so the matching build keeps them writable.
#ifdef MUST_MATCH
#define SDATA2_CONST
#else
#define SDATA2_CONST const
#endif
SDATA2_CONST f32 zero_80420520 = 0.0f;
SDATA2_CONST u32 dat_80420524 = 0xFFFFFFD0;
SDATA2_CONST u32 dat_80420528 = 0xFFFFFF00;
SDATA2_CONST u32 dat_8042052c = 0xFFFFFF00;
SDATA2_CONST f32 float_226_80420530 = 226.0f;
SDATA2_CONST f32 float_42_80420534 = 42.0f;
SDATA2_CONST f32 float_122_80420538 = 122.0f;
SDATA2_CONST f32 float_0_8042053c = 0.0f;
SDATA2_CONST f32 float_deg2rad_80420540 = 0.017453292f;
SDATA2_CONST f32 float_neg32_80420544 = -32.0f;
SDATA2_CONST f32 float_neg128_80420548 = -128.0f;
SDATA2_CONST f32 float_490_8042054c = 490.0f;
SDATA2_CONST f32 float_neg1_80420550 = -1.0f;
SDATA2_CONST f32 float_0p2_80420554 = 0.2f;
SDATA2_CONST f32 float_0p15_80420558 = 0.15f;
SDATA2_CONST f32 float_32767_8042055c = 32767.0f;

u16 tex_id_list_small[] = { '+', '-', '/', '.', ',', 0 };
u16 tex_id_list_big[] = { '0', '2', '4', '3', '1', 0 };
const SlotCards8 R_slot_1_cards = { { 0, 1, 2, 1, 2, 0, 3, 4 } };
const SlotCards8 R_slot_2_cards = { { 3, 1, 0, 1, 4, 2, 0, 2 } };
const SlotCards5 R_slot_3_cards = { { 0, 1, 2, 3, 4 } };

void item_appear_force(BattleBreakSlotReel* reel, s32 id) {
    s32 i;

    for (i = 0; i < reel->count; i++) {
        if (reel->cards[i] == id) {
            reel->index = i - 1;
        }
        if (reel->index == -1) {
            reel->index = reel->count - 1;
        }
    }
}

void BattleBreakSlot_HideReel(void) {
    BattleBreakSlot* slot = BattleBreakSlotGetPtr();
    BattleBreakSlotReel* reel = BattleBreakSlotReelGetPtr(0);
    s32 i;

    for (i = 0; i < 3; i++) {
        reel->flags |= 2;
        reel++;
    }
    slot->counter = 0;
}

void BattleBreakSlot_DecBreakTurn(void) {
    BattleBreakSlot* slot = BattleBreakSlotGetPtr();

    slot->breakTurn--;
    if (slot->breakTurn < 0) {
        slot->breakTurn = 0;
    }
}

s32 BattleBreakSlot_GetBreakTurn(void) {
    return BattleBreakSlotGetPtr()->breakTurn;
}

s32 BattleBreakSlot_CheckReaction(void) {
    BattleBreakSlot* slot = BattleBreakSlotGetPtr();

    BattleBreakSlot_Start();
    if (slot->state >= 1 && slot->state <= 8) {
        return 1;
    }
    if (slot->event != NULL && evtCheckID(slot->event->eventId) != 0) {
        return 1;
    }
    btl_camera_set_mode(0, 0);
    btl_camera_set_moveSpeedLv(0, 2);
    BattleAudienceNumToTarget();
    slot->event = NULL;
    return 0;
}

void BattleBreakSlot_Start(void) {
    BattleBreakSlot* slot = BattleBreakSlotGetPtr();

    if (slot->counter >= 3) {
        BattleBreakSlotReel* reel;
        s32 i;

        slot->counter = 0;
        slot->state = 2;
        reel = BattleBreakSlotReelGetPtr(0);
        for (i = 0; i < 3; i++) {
            reel->state = 4;
            reel->field_0C = 0;
            reel++;
        }
    }
}

void BattleBreakSlot_PointInc(void) {
    u8* battle = (u8*)_battleWorkPointer + 0x20000;
    BattleBreakSlot* slot = BattleBreakSlotGetPtr();
    BattleBreakSlotReel* reel;
    PouchData* pouch;

    pouch = pouchGetPtr();
    if (pouch->mStarPowersObtained == 0) {
        return;
    }
    reel = BattleBreakSlotReelGetPtr(slot->counter);
    reel->state = 1;
    reel->field_0C = 0;
    slot->counter++;
    if (slot->counter > 3) {
        slot->counter = 3;
        return;
    }
    switch (reel->type) {
    case 0:
        reel->index = irand(reel->count) - 1;
        if (reel->index == -1) {
            reel->index = reel->count - 1;
        }
        reel->value = reel->cards[reel->index];
        if (*(u32*)(battle - 0x6F98) & 0x80000) {
            item_appear_force(reel, *(s32*)(battle - 0x6F84));
        }
        if (slot->flags & 1) {
            item_appear_force(reel, 3);
        }
        if (slot->flags & 4) {
            item_appear_force(reel, 3);
        }
        break;
    case 1:
        reel->index = irand(reel->count) - 1;
        if (reel->index == -1) {
            reel->index = reel->count - 1;
        }
        reel->value = reel->cards[reel->index];
        if (*(u32*)(battle - 0x6F98) & 0x80000) {
            item_appear_force(reel, *(s32*)(battle - 0x6F84));
        }
        if (slot->flags & 1) {
            item_appear_force(reel, 3);
        }
        if (slot->flags & 4) {
            item_appear_force(reel, 3);
        }
        break;
    case 2: {
        BattleBreakSlotReel* reel2;

        do {
            reel2 = BattleBreakSlotReelGetPtr(1);
            reel->index = irand(reel->count);
            reel->value = reel->cards[reel->index];
        } while (reel->value == reel2->value);
        break;
    }
    }
}

void BattleBreakSlot_End(void) {
	//stubbed in retail
}

// NONMATCHING (68.41%): 3 instructions differ, 35 missing, 33 extra, plus register differences.
void BattleBreakSlotDispReel(CameraId camId, void* param) {
    GXColor color;
    Mtx mtxTrans;
    Mtx mtxScale;
    Mtx mtxPos;
    Mtx mtxNeg;
    Mtx mtxRot;
    Mtx mtxRotX;
    Mtx mtxRotY;
    Mtx mtxRotZ;
    Mtx mtxFinal;
    BattleBreakSlotReel* reel;
    u16* texList;
    s32 i;

    reel = BattleBreakSlotReelGetPtr(0);
    for (i = 0; i < 3; i++) {
        if ((reel->flags & 1) && reel->state >= 1 && reel->state <= 4) {
            PSMTXTrans(mtxTrans, float_42_80420534 * (f32)i + float_226_80420530, float_122_80420538, float_0_8042053c);
            *(u32*)&color = dat_80420524;
            iconDispGxCol(mtxTrans, 0x10, 0x1D4, color);
        }
        reel++;
    }
    reel = BattleBreakSlotReelGetPtr(0);
    if (reel->state >= 1 && reel->state <= 4) {
        GXSetScissor(0, 0x52, 0x260, 0x20);
        texList = tex_id_list_small;
    } else {
        texList = tex_id_list_big;
        if (reel->state != 0xB) {
            GXSetScissor(0, 0xB0, 0x260, 0x80);
        }
    }
    for (i = 0; i < 3; i++) {
        if (reel->flags & 1) {
            GXSetBlendMode(1, 4, 5, 0);
            GXSetZCompLoc(1);
            GXSetAlphaCompare(7, 0, 0, 7, 0);
            GXSetZMode(1, 3, 0);
            PSMTXTrans(mtxTrans, reel->x, reel->y, reel->z);
            PSMTXScale(mtxScale, reel->scaleX, reel->scaleY, reel->scaleZ);
            PSMTXRotRad(mtxRotX, 'x', float_deg2rad_80420540 * reel->rotX);
            PSMTXRotRad(mtxRotY, 'y', float_deg2rad_80420540 * reel->rotY);
            PSMTXRotRad(mtxRotZ, 'z', float_deg2rad_80420540 * reel->rotZ);
            PSMTXConcat(mtxRotX, mtxRotZ, mtxRot);
            PSMTXConcat(mtxRotY, mtxRot, mtxRot);
            PSMTXTrans(mtxPos, reel->field_44, reel->field_48, reel->field_4C);
            PSMTXTrans(mtxNeg, -reel->field_44, -reel->field_48, -reel->field_4C);
            PSMTXConcat(mtxRot, mtxPos, mtxFinal);
            PSMTXConcat(mtxNeg, mtxFinal, mtxFinal);
            PSMTXConcat(mtxScale, mtxFinal, mtxFinal);
            PSMTXConcat(mtxTrans, mtxFinal, mtxFinal);
            *(u32*)&color = dat_80420528;
            color.a = reel->field_58;
            btlDispTexPlane2(mtxFinal, texList[reel->value], color, 0);
            if (reel->state <= 0xA && reel->state != 3 && reel->state != 4) {
                s32 idx = reel->index + 1;

                if (idx >= reel->count) {
                    idx = 0;
                }
                if (reel->state >= 1 && reel->state <= 4) {
                    PSMTXTrans(mtxTrans, float_0_8042053c, float_neg32_80420544, float_0_8042053c);
                } else {
                    PSMTXTrans(mtxTrans, float_0_8042053c, float_neg128_80420548, float_0_8042053c);
                }
                PSMTXConcat(mtxTrans, mtxFinal, mtxFinal);
                *(u32*)&color = dat_8042052c;
                color.a = reel->field_58;
                btlDispTexPlane2(mtxFinal, texList[reel->cards[idx]], color, 0);
            }
        }
        reel++;
    }
    GXSetScissor(0, 0, gp->fbWidth, gp->efbHeight);
}

void BattleBreakSlot_Disp(void) {
    dispEntry(CAMERA_2D, 490.0f, 0, BattleBreakSlotDispReel, NULL);
}

// NONMATCHING (0.07%): empty placeholder, not decompiled yet.
void BattleBreakSlot_Main(void) {

}

void BattleBreakSlot_Init(void) {
    BattleBreakSlot* slot = BattleBreakSlotGetPtr();
    BattleBreakSlotReel* reel;
    SlotCards8 tmp1 = R_slot_1_cards;
    SlotCards8 tmp2 = R_slot_2_cards;
    SlotCards5 tmp3 = R_slot_3_cards;
    s32 i;

    memset(slot, 0, 0x134);
    slot->state = 0;
    i = 0;
    reel = BattleBreakSlotReelGetPtr(0);
    for (; i < 3; i++) {
        reel->type = i;
        reel->state = 0;
        reel++;
    }
    reel = BattleBreakSlotReelGetPtr(0);
    reel->count = 8;
    reel->cards = BattleAlloc(reel->count * 4);
    memcpy(reel->cards, &tmp1, reel->count * 4);
    reel = BattleBreakSlotReelGetPtr(1);
    reel->count = 8;
    reel->cards = BattleAlloc(reel->count * 4);
    memcpy(reel->cards, &tmp2, reel->count * 4);
    reel = BattleBreakSlotReelGetPtr(2);
    reel->count = 5;
    reel->cards = BattleAlloc(reel->count * 4);
    memcpy(reel->cards, &tmp3, reel->count * 4);
}
BattleBreakSlotReel* BattleBreakSlotReelGetPtr(s32 index) {
    return &BattleBreakSlotGetPtr()->reels[index];
}

BattleBreakSlot* BattleBreakSlotGetPtr(void) {
    return (BattleBreakSlot*)((u8*)_battleWorkPointer + 0x1616C);
}

#pragma dont_inline reset
