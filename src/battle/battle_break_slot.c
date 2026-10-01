#include "battle/battle_break_slot.h"

#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "battle/battle_camera.h"
#include "battle/battle_disp.h"
#include "eff/eff_break.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "manager/evtmgr.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include "math_ext.h"
#include <stdlib.h>
#include <string.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u32 lbl_803AB118[2] = { 0xBF800000, 0x3F800000 };

extern s32 kinoko_effect[];
extern s32 flower_effect[];
extern s32 shine_effect[];
extern s32 doku_kinoko_effect[];
extern s32 star_effect[2];
extern s32 msg_hp_max[9];
extern s32 msg_fp_max[9];
extern s32 msg_star_max[9];
extern s32 msg_super_max[9];
extern s32 msg_fault[9];

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
    Vec pos;        // 0x20
    Vec rot;        // 0x2C
    Vec scale;      // 0x38
    Vec pivot;      // 0x44
    f32 field_50;   // 0x50
    f32 field_54;   // 0x54
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

#ifdef MUST_MATCH
// Returning the colour from an inline (rather than writing the channels in place) makes MWCC lay the
// local/outgoing GXColor pair out the way the original does; the direct build puts the two slots the
// other way round.
static inline GXColor slotColorA(u8 a) {
    GXColor c = *(GXColor*)&dat_80420528;

    c.a = a;
    return c;
}

static inline GXColor slotColorB(u8 a) {
    GXColor c = *(GXColor*)&dat_8042052c;

    c.a = a;
    return c;
}
#endif

#ifdef MUST_MATCH
// Returning the colour from an inline (rather than writing the channels in place) makes MWCC lay the
// local/outgoing GXColor pair out the way the original does; passing the alpha first is what makes the
// second call's scratch slots come out adjacent to its local (with the colour first the compiler groups
// the two by-value colours' locals together and the second call's copy lands 4 bytes high).
static inline GXColor slotColor(u8 a, GXColor c) {
    c.a = a;
    return c;
}
#endif

// i->f32 pools an anonymous 2^52 double instead of the original's named double_to_int_802fd930; the value
// and instructions are identical, only the pool label differs (not scored).
void BattleBreakSlotDispReel(CameraId camId, void* param) {
    s32 i;
    BattleBreakSlotReel* reel;
    Mtx mtxTrans;
    Mtx mtxScale;
    Mtx mtxPos;
    Mtx mtxNeg;
    Mtx mtxRot;
    Mtx mtxRotX;
    Mtx mtxRotY;
    Mtx mtxRotZ;
    Mtx mtxFinal;
    u16* texList;
    u16 tex;

    i = 0;
    {
        BattleBreakSlotReel* reel1 = BattleBreakSlotReelGetPtr(0);

        for (; i < 3; i++) {
            if ((reel1->flags & 1) && reel1->state >= 1 && reel1->state <= 4) {
                PSMTXTrans(mtxTrans, float_42_80420534 * (f32)i + float_226_80420530, float_122_80420538, float_0_8042053c);
                {
                    GXColor color = *(GXColor*)&dat_80420524;

                    iconDispGxCol(mtxTrans, 0x10, 0x1D4, color);
                }
            }
            reel1++;
        }
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
    i = 0;
    reel = BattleBreakSlotReelGetPtr(0);
    for (; i < 3; i++) {
        if (reel->flags & 1) {
            GXSetBlendMode(1, 4, 5, 0);
            GXSetZCompLoc(1);
            GXSetAlphaCompare(7, 0, 0, 7, 0);
            GXSetZMode(1, 3, 0);
            PSMTXTrans(mtxTrans, reel->pos.x, reel->pos.y, reel->pos.z);
            PSMTXScale(mtxScale, reel->scale.x, reel->scale.y, reel->scale.z);
            PSMTXRotRad(mtxRotX, 'x', float_deg2rad_80420540 * reel->rot.x);
            PSMTXRotRad(mtxRotY, 'y', float_deg2rad_80420540 * reel->rot.y);
            PSMTXRotRad(mtxRotZ, 'z', float_deg2rad_80420540 * reel->rot.z);
            PSMTXConcat(mtxRotX, mtxRotZ, mtxRot);
            PSMTXConcat(mtxRotY, mtxRot, mtxRot);
            PSMTXTrans(mtxPos, reel->pivot.x, reel->pivot.y, reel->pivot.z);
            PSMTXTrans(mtxNeg, -reel->pivot.x, -reel->pivot.y, -reel->pivot.z);
            PSMTXConcat(mtxRot, mtxPos, mtxFinal);
            PSMTXConcat(mtxNeg, mtxFinal, mtxFinal);
            PSMTXConcat(mtxScale, mtxFinal, mtxFinal);
            PSMTXConcat(mtxTrans, mtxFinal, mtxFinal);
            {
                u16 tex0 = texList[reel->value];
#ifdef MUST_MATCH
                btlDispTexPlane2(mtxFinal, tex0, slotColor(reel->field_58, *(GXColor*)&dat_80420528), 0);
#else
                GXColor c0;

                *(u32*)&c0 = dat_80420528;
                c0.a = reel->field_58;
                btlDispTexPlane2(mtxFinal, tex0, c0, 0);
#endif
            }
            if (reel->state <= 0xA && reel->state != 3 && reel->state != 4) {
                s32 idx = reel->index + 1;

                if (idx >= reel->count) {
                    idx = 0;
                }
                tex = texList[reel->cards[idx]];
                if (reel->state >= 1 && reel->state <= 4) {
                    PSMTXTrans(mtxTrans, float_0_8042053c, float_neg32_80420544, float_0_8042053c);
                } else {
                    PSMTXTrans(mtxTrans, float_0_8042053c, float_neg128_80420548, float_0_8042053c);
                }
                PSMTXConcat(mtxTrans, mtxFinal, mtxFinal);
                {
#ifdef MUST_MATCH
                    btlDispTexPlane2(mtxFinal, tex, slotColor(reel->field_58, *(GXColor*)&dat_8042052c), 0);
#else
                    GXColor c1;

                    *(u32*)&c1 = dat_8042052c;
                    c1.a = reel->field_58;
                    btlDispTexPlane2(mtxFinal, tex, c1, 0);
#endif
                }
            }
        }
        reel++;
    }
    GXSetScissor(0, 0, gp->fbWidth, gp->efbHeight);
}
void BattleBreakSlot_Disp(void) {
    dispEntry(CAMERA_2D, 490.0f, 0, BattleBreakSlotDispReel, NULL);
}

// NONMATCHING (98.50%): the .sdata2 float literals pool into anonymous slots, so objdiff still flags every
// float load against the original's named symbols (the score counts them as matched, so they are not the
// remaining gap). What still differs: the effect scripts are extern here, so each evtEntry loads its own
// address instead of using a .data base register (r27), which in turn leaves _battleWorkPointer in r4 instead
// of r5; the int-to-float double pools after Main's SFX strings in .rodata (0x2E0) instead of before them
// (0x270); pouchGetAP's s16 return gets an extsh the original doesn't have (mario_pouch.h declares s16; the
// target does srawi/addze on r3, so the original is s32); case 3's intplGetValue argument loads and case 9/10's
// field_50 arithmetic are scheduled with different registers. The gp->language test is now the unsigned
// compare the target has, and `0.0f != updown` reproduces its load order.
// NONMATCHING (98.5%): see the note above for what still differs.
void BattleBreakSlot_Main(void) {
    static f32 updown;
    BattleWork* wp = _battleWorkPointer;
    u8* battle = (u8*)wp + 0x20000;
    BattleAudience* audience = BattleAudienceBaseGetPtr();
    BattleBreakSlot* slot = BattleBreakSlotGetPtr();
    BattleBreakSlotReel* reel;
    s32 n;
    s32 i;
    s32 j;
    f32 v;

    BattleGetMarioPtr(wp);
    BattleGetPartyPtr(wp);
    switch (slot->state) {
    case 0:
        n = 0;
        for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
            if (reel->state == 2) {
                n++;
            }
            reel++;
        }
        if (n == 2 && BattleBreakSlotReelGetPtr(0)->value != BattleBreakSlotReelGetPtr(1)->value) {
            slot->state = 1;
            slot->field_08 = 0;
        }
        break;
    case 1:
        if (slot->field_08 == 60) {
            for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 2; i++) {
                reel->state = 3;
                reel->field_0C = 0;
                reel++;
            }
        }
        n = 0;
        for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
            if (reel->state == 0) {
                n++;
            }
            reel++;
        }
        if (n == 3) {
            slot->state = 8;
            slot->counter = 0;
        }
        slot->field_08++;
        break;
    case 2:
        n = 0;
        for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
            if (reel->state == 6 || reel->state == 10) {
                n++;
            }
            reel++;
        }
        if (n == 3) {
            slot->state = 3;
            slot->field_08 = 0;
            for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
                if (reel->state == 6) {
                    reel->state = 7;
                }
                reel++;
            }
            slot->sound = psndSFXOn("SFX_BTL_SLOT_START1");
        }
        break;
    case 3:
        n = 0;
        for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
            if (reel->state == 10) {
                n++;
            }
            reel++;
        }
        if (n == 3) {
            slot->state = 4;
            slot->field_08 = 0;
            psndSFXOff(slot->sound);
        }
        if ((keyGetButtonTrg(0) & 0x100) && slot->field_08 >= 15) {
            for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
                if (reel->state == 8) {
                    reel->state = 9;
                    reel->field_54 = -1.0f * (0.2f + (0.15f * rand()) / 32767.0f);
                    reel->field_54 = -1.0f;
                    break;
                }
                reel++;
            }
        }
        slot->field_08++;
        break;
    case 4:
        if (slot->field_08 == 20) {
            n = 0;
            for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 2; i++) {
                if (reel[0].value == reel[1].value) {
                    n++;
                }
                reel++;
            }
            if (n >= 2) {
                slot->state = 5;
                psndSFXOn("SFX_BTL_SLOT_SUCCESS1");
                for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
                    reel->state = 11;
                    reel->field_0C = 0;
                    reel++;
                }
            } else {
                slot->state = 7;
                psndSFXOn("SFX_BTL_SYS_BREAK_FAILRE1");
                for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
                    reel->state = 12;
                    reel->field_0C = 0;
                    reel++;
                }
            }
        }
        slot->field_08++;
        break;
    case 5:
        n = 0;
        for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
            if (reel->state == 0) {
                n++;
            }
            reel++;
        }
        if (n == 3) {
            slot->state = 6;
            slot->field_08 = 0;
        }
        break;
    case 6:
        if (slot->field_08 == 0) {
            if (BattleBreakSlotReelGetPtr(0)->value != 4) {
                effBreakEntry(0, 240, 0.0f, 80.0f, 0.0f);
            } else {
                slot->field_08 = 101;
                BattleAudienceSoundBooing();
            }
        }
        if (slot->field_08 == 20 || slot->field_08 == 40 || slot->field_08 == 60 || slot->field_08 == 80 ||
            slot->field_08 == 100 || slot->field_08 == 120) {
            psndSFXOn("SFX_BTL_SYS_BREAK1");
            btl_camera_shake_h(0, 10, 0, 5.0f, 0.0f);
        }
        if (slot->field_08 == 120) {
            BattleAudienceSoundCheer(240, 240);
        }
        if (slot->field_08 == 130) {
            switch (BattleBreakSlotReelGetPtr(0)->value) {
            case 0:
                evtEntry(kinoko_effect, 0, 0x20);
                slot->breakTurn = 4;
                slot->field_14 = 2.0f;
                break;
            case 1:
                evtEntry(flower_effect, 0, 0x20);
                slot->breakTurn = 4;
                slot->field_14 = 2.0f;
                break;
            case 2:
                evtEntry(star_effect, 0, 0x20);
                pouchSetAP(pouchGetMaxAP());
                psndSFXOn("SFX_HP_RECOVER_BIG1");
                slot->breakTurn = 4;
                slot->field_14 = 2.0f;
                break;
            case 3:
                evtEntry(shine_effect, 0, 0x20);
                pouchSetAP(pouchGetMaxAP());
                slot->breakTurn = 6;
                slot->field_14 = 3.0f;
                break;
            case 4:
                evtEntry(doku_kinoko_effect, 0, 0x20);
                pouchSetAP(pouchGetAP() / 2);
                break;
            }
        }
        if (slot->field_08 == 140) {
            switch (BattleBreakSlotReelGetPtr(0)->value) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                btl_camera_set_mode(0, 6);
                btl_camera_set_moveSpeedLv(0, 2);
                break;
            }
            if (_battleWorkPointer->flags & 0x100) {
                btl_camera_set_mode(0, 0);
            }
        }
        if (slot->field_08 == 180) {
            updown = 0.0f;
            switch (BattleBreakSlotReelGetPtr(0)->value) {
            case 0:
            case 1:
            case 2:
                updown = BattleAudienceAddAudienceNum(audience->mMaxAudience / 2);
                break;
            case 3:
                updown = BattleAudienceAddAudienceNum(audience->mMaxAudience);
                break;
            case 4:
                updown = BattleAudienceAddAudienceNum(-audience->mMaxAudience);
                break;
            }
            if (_battleWorkPointer->flags & 0x100) {
                updown = 0.0f;
            }
        }
        if (slot->field_08 == 209 && 0.0f != updown) {
            BattleAudienceSoundRun(0);
        }
        if (slot->field_08 == 210) {
            if (!(_battleWorkPointer->cameraWork.flags & 2)) {
                btl_camera_shake_h(0, 1000, 0, 2.0f, 2.0f);
            }
            for (j = 0; j < 200; j++) {
                if ((u8)BattleAudience_GetExist(j) == 1 &&
                    (BattleAudienceGetPtr(j)->status == 11 || BattleAudienceGetPtr(j)->status == 12)) {
                    slot->field_08--;
                    break;
                }
            }
            if (j == 200) {
                btl_camera_noshake(0);
                BattleAudienceSoundStop(13);
            }
        }
#ifdef MUST_MATCH
        // retail tests j here, the audience loop's counter, which is only assigned on a field_08 == 210 frame: on the
        // frame-220 call it reads an uninitialised local (rule 7: kept only under MUST_MATCH)
        if (slot->field_08 == 220 && j == 200) {
#else
        if (slot->field_08 == 220) {
#endif
            btl_camera_set_mode(0, 0);
            btl_camera_noshake(0);
        }
        if (slot->field_08 == 270) {
            BattleAudienceAddTargetNum(0.0f, updown);
            slot->state = 8;
            slot->flags &= ~4;
            if ((u32)gp->language == 0) {
                switch (BattleBreakSlotReelGetPtr(0)->value) {
                case 0:
                    slot->event = evtEntry(msg_hp_max, 0, 0x20);
                    break;
                case 1:
                    slot->event = evtEntry(msg_fp_max, 0, 0x20);
                    break;
                case 2:
                    slot->event = evtEntry(msg_star_max, 0, 0x20);
                    break;
                case 3:
                    slot->event = evtEntry(msg_super_max, 0, 0x20);
                    break;
                case 4:
                    slot->event = evtEntry(msg_fault, 0, 0x20);
                    break;
                }
            }
        }
        slot->field_08++;
        break;
    case 7:
        n = 0;
        for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++) {
            if (reel->state == 0) {
                n++;
            }
            reel++;
        }
        if (n == 3) {
            slot->state = 8;
        }
        break;
    case 8:
        slot->state = 0;
        break;
    }

    for (i = 0, reel = BattleBreakSlotReelGetPtr(0); i < 3; i++, reel++) {
        switch (reel->state) {
        case 0:
            reel->flags &= ~1;
            reel->field_58 = 0xFF;
            break;
        case 1:
            if (i != 2) {
                reel->flags |= 1;
            }
            reel->field_0C++;
            if (reel->field_0C <= 5) {
                v = intplGetValue(0, reel->field_0C, 5, 110.0f, 142.0f);
                reel->pos = (Vec){ 42.0f * i + 226.0f, v, 0.0f };
                reel->scale = (Vec){ 0.8f, 0.8f, 0.8f };
                reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
                reel->pivot = (Vec){ 0.0f, 0.0f, 0.0f };
                reel->field_58 = 0xFF;
            } else {
                v = intplGetValue(4, reel->field_0C - 5, 20, 142.0f, 174.0f);
                reel->pos = (Vec){ 42.0f * i + 226.0f, v, 0.0f };
                reel->scale = (Vec){ 0.8f, 0.8f, 0.8f };
                reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
                reel->pivot = (Vec){ 0.0f, 0.0f, 0.0f };
                reel->field_58 = 0xFF;
            }
            if (reel->field_0C >= 25) {
                reel->state = 2;
                reel->index++;
                if (reel->index >= reel->count) {
                    reel->index -= reel->count;
                }
                reel->value = reel->cards[reel->index];
                reel->pos = (Vec){ 42.0f * i + 226.0f, 142.0f, 0.0f };
            }
            break;
        case 2:
            reel->pos = (Vec){ 42.0f * i + 226.0f, 142.0f, 0.0f };
            reel->scale = (Vec){ 0.8f, 0.8f, 0.8f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            if (reel->flags & 2) {
                reel->flags &= ~2;
                reel->state = 3;
                reel->field_0C = 0;
            }
            break;
        case 3:
            v = intplGetValue(0, reel->field_0C, 20, 142.0f, 174.0f);
            reel->pos = (Vec){ 42.0f * i + 226.0f, v, 0.0f };
            reel->scale = (Vec){ 0.8f, 0.8f, 0.8f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            reel->field_0C++;
            if (reel->field_0C > 20) {
                reel->state = 0;
                reel->field_0C = 0;
                reel->flags &= ~1;
            }
            break;
        case 4:
            v = intplGetValue(0, reel->field_0C, 20, 142.0f, 174.0f);
            reel->pos = (Vec){ 42.0f * i + 226.0f, v, 0.0f };
            reel->scale = (Vec){ 0.8f, 0.8f, 0.8f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            reel->field_0C++;
            if (reel->field_0C > 20) {
                reel->state = 5;
                reel->pos = (Vec){ (f32)(i * 128 - 128), -128.0f, 0.0f };
                reel->field_0C = 0;
            }
            break;
        case 5:
            reel->flags |= 1;
            v = intplGetValue(0, reel->field_0C, 40, -128.0f, 0.0f);
            reel->pos = (Vec){ (f32)(i * 128 - 128), v, 0.0f };
            reel->scale = (Vec){ 1.0f, 1.0f, 1.0f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            reel->field_0C++;
            if (reel->field_0C > 40) {
                if (reel->type == 2) {
                    reel->state = 6;
                    reel->field_0C = 0;
                } else {
                    reel->state = 10;
                    reel->field_0C = 0;
                }
            }
            break;
        case 6:
            break;
        case 7:
            reel->field_50 = intplGetValue(0, reel->field_0C, 20, 0.0f, 22.5f);
            reel->pos = (Vec){ 128.0f * i + -128.0f, reel->pos.y + reel->field_50, 0.0f };
            reel->scale = (Vec){ 1.0f, 1.0f, 1.0f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            reel->pivot = (Vec){ 0.0f, 0.0f, 0.0f };
            if (reel->pos.y >= 128.0f) {
                reel->pos.y -= 128.0f;
                reel->index++;
                if (reel->index >= reel->count) {
                    reel->index = 0;
                }
                reel->value = reel->cards[reel->index];
            }
            reel->field_0C++;
            if (reel->field_0C > 20) {
                reel->state = 8;
                reel->field_0C = 0;
            }
            break;
        case 8:
            reel->field_50 = 22.5f;
            reel->pos = (Vec){ 128.0f * i + -128.0f, reel->pos.y + reel->field_50, 0.0f };
            reel->scale = (Vec){ 1.0f, 1.0f, 1.0f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            if (reel->pos.y >= 128.0f) {
                reel->pos.y -= 128.0f;
                reel->index++;
                if (reel->index >= reel->count) {
                    reel->index = 0;
                }
                reel->value = reel->cards[reel->index];
            }
            break;
        case 9:
            reel->field_50 += reel->field_54;
            if (reel->field_50 < 20.0f) {
                reel->field_50 = 20.0f;
            }
            reel->pos = (Vec){ 128.0f * i + -128.0f, reel->pos.y + reel->field_50, 0.0f };
            reel->scale = (Vec){ 1.0f, 1.0f, 1.0f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            if (reel->field_50 >= 0.0f && reel->pos.y >= 128.0f) {
                reel->pos.y -= 128.0f;
                reel->index++;
                if (reel->index == reel->count) {
                    reel->index = 0;
                }
                reel->value = reel->cards[reel->index];
                if ((audience->flags & 0x4000) && BattleBreakSlotReelGetPtr(0)->value == 4 &&
                    BattleBreakSlotReelGetPtr(1)->value == 4 && reel->value == 4) {
                    break;
                }
                if ((slot->flags & 2) && BattleBreakSlotReelGetPtr(0)->value == 4 &&
                    BattleBreakSlotReelGetPtr(1)->value == 4 && reel->value == 4) {
                    break;
                }
                if ((slot->flags & 1) && reel->value != 3) {
                    break;
                }
                if ((slot->flags & 4) && reel->value != 3) {
                    break;
                }
                if ((*(u32*)(battle - 0x6F98) & 0x80000) && reel->value != *(s32*)(battle - 0x6F84)) {
                    break;
                }
                reel->pos.y = 0.0f;
                reel->state = 10;
                reel->field_0C = 0;
                reel->field_54 = reel->field_50;
            }
            break;
        case 10:
            if (i == 2) {
                reel->field_0C++;
                reel->field_50 = intplGetValue(0, reel->field_0C, 20, reel->field_50, 0.0f);
                reel->pos.y = reel->field_50 * (f32)sin(3.141592f * (2.0f * reel->field_0C) / 8.0f);
            }
            break;
        case 11:
            reel->field_50 = 0.0f;
            reel->pos.y = 0.0f;
            v = intplGetValue(0, reel->field_0C, 60, 1.0f, 2.0f);
            reel->scale = (Vec){ v, v, 1.0f };
            reel->field_58 = intplGetValue(0, reel->field_0C, 60, 255.0f, 0.0f);
            reel->field_0C++;
            if (reel->field_0C > 60) {
                reel->state = 13;
                reel->flags &= ~1;
            }
            break;
        case 12:
            reel->field_50 = 0.0f;
            reel->pos.y = 0.0f;
            v = intplGetValue(0, reel->field_0C, 40, 0.0f, 128.0f);
            reel->pos = (Vec){ 128.0f * i + -128.0f, v, 0.0f };
            reel->scale = (Vec){ 1.0f, 1.0f, 1.0f };
            reel->rot = (Vec){ 0.0f, 0.0f, 0.0f };
            reel->field_0C++;
            if (reel->field_0C > 40) {
                reel->state = 13;
                reel->flags &= ~1;
            }
            break;
        case 13:
            reel->state = 0;
            break;
        }
    }
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
#ifdef MUST_MATCH
    i = 0;
    reel = BattleBreakSlotReelGetPtr(0);
    for (; i < 3; i++) {
#else
    reel = BattleBreakSlotReelGetPtr(0);
    for (i = 0; i < 3; i++) {
#endif
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
