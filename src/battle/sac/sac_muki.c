#include "battle/sac/sac_muki.h"
#include "battle/sac/sac_common.h"

#include "battle/battle.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_audience.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include <dolphin/gx/GXTransform.h>
#include "driver/effdrv.h"
#include "eff/eff_starstone.h"
#include "eff/eff_hit.h"
#include "eff/n64/eff_stamp_n64.h"
#include "pmario_sound.h"
#include "party/party_motion.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "system.h"
#include <string.h>
#include <dolphin/mtx.h>
#include "math_ext.h"
#include "matching.h"

// Data objects defined later in this file or in other units, declared before their first use.
extern s32 sac_start_camera_evt[];
extern s32 sac_mario_item_on_evt[];
extern s32 sac_start_stardust[];
extern s32 sac_mario_item_on_end_evt[];
extern s32 sac_mario_back_home[];
extern s32 marioAttackEvent_Muki[];

#pragma pool_data on // the original addresses its .rodata through one base register

// The Muki action-command work area, at _battleWorkPointer + 0x1F4C. No header declares it.
typedef struct MukiEntry {
    s32 type;             // 0x00
    s32 field_04;         // 0x04
    s32 mode;             // 0x08
    Vec pos;              // 0x0C
    Vec scale;            // 0x18
    f32 field_24;         // 0x24
    f32 rot;              // 0x28
    f32 field_2C;         // 0x2C
    EffectEntry* effect;  // 0x30
    u8 alpha;             // 0x34
    u8 field_35[0x38 - 0x35];
} MukiEntry;

typedef struct MukiPoint {
    s32 type;          // 0x00
    s32 field_04;      // 0x04
    Vec pos;           // 0x08
    f32 field_14;      // 0x14
    f32 field_18;      // 0x18
    f32 field_1C;      // 0x1C
    MukiEntry* field_20; // 0x20, but MukiEntry is declared above? no
} MukiPoint;

typedef struct MukiWork {
    s32 field_00;                    // 0x000
    s32 state;                       // 0x004
    s32 field_08;                    // 0x008
    s32 field_0C;                    // 0x00C
    s32 field_10;                    // 0x010
    f32 field_14;                    // 0x014
    f32 field_18;                    // 0x018
    f32 field_1C;                    // 0x01C
    f32 field_20;                    // 0x020
    s32 field_24;                    // 0x024
    s32 field_28;                    // 0x028
    f32 field_2C;                    // 0x02C
    f32 field_30;                    // 0x030
    s32 field_34;                    // 0x034
    s32 field_38;                    // 0x038
    s32 field_3C;                    // 0x03C
    s32 field_40;                    // 0x040
    Vec field_44;                    // 0x044
    Vec field_50;                    // 0x050
    Vec field_5C;                    // 0x05C
    Vec field_68;                    // 0x068
    GXColor field_74;                // 0x074
    MukiPoint points[9];             // 0x078
    MukiEntry entries[9];            // 0x1BC
    s32 field_3B4;                   // 0x3B4
    s32 field_3B8;                   // 0x3B8
    f32 posX;                        // 0x3BC
    f32 posY;                        // 0x3C0
    f32 posZ;                        // 0x3C4
    f32 startX;                      // 0x3C8
    f32 startY;                      // 0x3CC
    f32 startZ;                      // 0x3D0
    f32 endX;                        // 0x3D4
    f32 endY;                        // 0x3D8
    f32 endZ;                        // 0x3DC
    u8 field_3E0[0x3EC - 0x3E0];     // 0x3E0
    Vec scale;                       // 0x3EC
    Vec scale2;                      // 0x3F8
    EffectEntry* starStone;          // 0x404
    u8 weapon[0xC0];                 // 0x408
} MukiWork;                          // size 0x4C8

static BattleWeapon weapon = {
    "btl_wn_sac_mukimuki_body",               // name
    0x019A,                                   // icon
    ITEM_NULL,                                // item
    "msg_mukimuki_body",                      // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    3,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02100070,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_sac_4",                              // acHelpMsg
    0x00020008,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_Muki,                    // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

u32 nice_event_pow[8] = {0x0005005B, (u32)btlevtcmd_ACSuccessEffect, 0xFE363C80, 0xFFFFFF9C, 0x000000AF, 0, 2, 1};

static MukiWork* get_ptr(void);
static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall);
static s32 start_game(EventEntry* event, BOOL isFirstCall);
static s32 set_weapon(EventEntry* event, BOOL isFirstCall);
static s32 wait_game_end(EventEntry* event, BOOL isFirstCall);
static void disp_3D_alpha(CameraId cameraId, void* param);
static void disp_3D(CameraId cameraId, void* param);
static void disp_2D(CameraId cameraId, void* param);
static void main_star(void);
static void main_base(void);

// .text:0x908 | 0x8024905C | size: 0x134
#pragma dont_inline on
// Prototypes for the forward references the original function order needs.
static void main_base(void);
static void main_star(void);
static void disp_2D(CameraId cameraId, void* param);
static void disp_3D(CameraId cameraId, void* param);
static void disp_3D_alpha(CameraId cameraId, void* param);
static MukiWork* get_ptr(void);
static s32 set_weapon(EventEntry* event, BOOL isFirstCall);
static s32 start_game(EventEntry* event, BOOL isFirstCall);
static s32 wait_game_end(EventEntry* event, BOOL isFirstCall);
static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall);

s32 main_muki(EventEntry* event, BOOL isFirstCall) {
    MukiWork* work = get_ptr();
    s32* args = event->args;
    s32 i;

    if (isFirstCall) {
        memset(work, 0, 0x4C8);
        work->field_10 = evtGetValue(event, args[0]);
        for (i = 0; i < 9; i++) {
            f32 zero = 0.0f;
            work->entries[i].effect = effStarStoneEntry(9, zero, -1000.0f, zero,
                                                        1.0f);
        }
    }
    main_base();
    main_cursor();
    for (i = 0; i < 9; i++) {
        main_point(i);
    }
    for (i = 0; i < 9; i++) {
        main_icon(i);
    }
    main_star();
    dispEntry(CAMERA_2D, 901.0f, 1, disp_2D, NULL);
    dispEntry(CAMERA_3D, 0.0f, 1, disp_3D, NULL);
    dispEntry(CAMERA_3D, 0.0f, 2, disp_3D_alpha, NULL);
    return 0;
}
#pragma dont_inline reset

// .text:0x1D0C | 0x80248BD8 | size: 0x484
#pragma dont_inline on
static void main_base(void) {
    MukiWork* work = get_ptr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    BattleUnitPart* part = BtlUnit_GetPartsPtr(mario, BtlUnit_GetBodyPartsId(mario));
    s32 i;
    s32 count;
    f32 f;
    f32 diff;

    if (work->field_24 > 0) {
        work->field_24--;
    }
    if (work->field_28 > 0) {
        work->field_28--;
    }

    switch (work->state) {
    case 0:
        break;
    case 1:
        work->state = 2;
        work->field_18 = 0.0f;
        work->field_14 = 0.0f;
        work->field_20 = 0.0f;
        work->field_1C = 0.0f;
        work->field_2C = -400.0f;
        work->field_30 = 90.0f;
        work->field_0C = 0x383;
        for (i = 0; i < 9; i++) {
            work->points[i].type = 1;
        }
        /* fallthrough */
    case 2:
        count = 0;
        for (i = 0; i < 9; i++) {
            if (work->points[i].type == 4) {
                count++;
            }
            else {
                break;
            }
        }
        if (count == 9) {
            work->state = 3;
            work->field_08 = 0;
        }
        break;
    case 3:
        work->field_08++;
        work->field_2C = intplGetValue(4, work->field_08, 0x1E, -400.0f, -120.0f);
        if (work->field_08 >= 0x1E) {
            work->state = 4;
            work->field_08 = 0;
            work->field_38 = 1;
            for (i = 0; i < 9; i++) {
                work->points[i].type = 5;
            }
        }
        break;
    case 4:
        if (work->field_00 & 1) {
            work->field_34++;
            if (work->field_34 == 0x1E) {
                BtlUnit_SetAnim(part, "M_X_1");
            }
        }
        work->field_0C--;
        if (work->field_0C <= 0) {
            work->state = 5;
            work->field_08 = 0;
            work->field_38 = 7;
            BtlUnit_SetAnim(part, "M_S_1");
            for (i = 0; i < 9; i++) {
                work->points[i].type = 6;
                work->entries[i].type = 9;
            }
            count = (s32)(work->field_14 + work->field_1C);
            if (count >= 1) {
                if (count == 1) count = 2;
                else if (count == 2) count = 3;
                else if (count == 3) count = 4;
                else if (count == 4) count = 5;
                else if (count >= 5) count = 6;
                evtEntry(nice_event_pow, 0, 0x20)->lwData[0] = count;
            }
        }
        break;
    case 5:
        count = 0;
        for (i = 0; i < 9; i++) {
            if (work->points[i].type == 8) {
                count++;
            }
            else {
                break;
            }
        }
        if (count == 9) {
            work->state = 6;
            work->field_08 = 0;
        }
        break;
    case 6:
        work->field_08++;
        if (work->field_08 >= 0x1E) {
            work->state = 7;
            work->field_08 = 0;
        }
        break;
    case 7:
        break;
    }

    diff = work->field_14 - work->field_18;
    if (0.0f != diff) {
        if (diff > 0.0f) {
            f = 0.1f;
            {
                f32 prod = f * diff;
                f = (prod < f) ? f : prod;
            }
            work->field_18 += f;
            if (work->field_14 - work->field_18 < 0.0f) {
                work->field_18 = work->field_14;
            }
        }
        else {
            f = 0.1f * diff;
            if (f > -0.1f) {
                f = 0.1f;
            }
            else {
                f = -0.1f * diff;
            }
            work->field_18 -= f;
            if (work->field_14 - work->field_18 > 0.0f) {
                work->field_18 = work->field_14;
            }
        }
    }

    diff = work->field_1C - work->field_20;
    if (0.0f != diff) {
        if (diff > 0.0f) {
            f = 0.1f;
            {
                f32 prod = f * diff;
                f = (prod < f) ? f : prod;
            }
            work->field_20 += f;
            if (work->field_1C - work->field_20 < 0.0f) {
                work->field_20 = work->field_1C;
            }
        }
        else {
            f = 0.1f * diff;
            if (f > -0.1f) {
                f = 0.1f;
            }
            else {
                f = -0.1f * diff;
            }
            work->field_20 -= f;
            if (work->field_1C - work->field_20 > 0.0f) {
                work->field_20 = work->field_1C;
            }
        }
    }
}
#pragma dont_inline reset

#pragma dont_inline on
void main_cursor(void) {
    MukiWork* work = get_ptr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    BattleUnitPart* part = BtlUnit_GetPartsPtr(mario, BtlUnit_GetBodyPartsId(mario));
    s32 audienceNum = BattleAudience_GetAudienceNum();
    MukiEntry* entry;
    f32 old;
    f32 x;
    f32 y;
    f32 z;

    switch (work->field_38) {
    case 0:
        break;
    case 1:
        if (work->points[4].type < 5) {
            break;
        }
        work->field_38 = 2;
        work->field_44 = work->points[4].pos;
        work->field_44.z += 5.0f;
        work->field_68 = (Vec){0.5f, 0.5f, 1.0f};
        work->field_40 = 4;
        work->field_74 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
        /* fallthrough */
    case 2:
        work->field_74 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
        if (work->field_34 >= 0x78) {
            BtlUnit_SetAnim(part, "M_I_Y");
            work->field_00 &= ~1;
            work->field_34 = 0;
        }
        if (keyGetButtonTrg(0) & 0x100) {
            work->field_38 = 5;
            entry = work->points[work->field_40].field_20;
            if (entry != NULL) {
                entry->type = 5;
                effHitEntry(0, 5, entry->pos.x, entry->pos.y, entry->pos.z, 1.0f);
                switch (entry->mode) {
                case 0:
                    old = work->field_14;
                    work->field_14 += 0.25f;
                    if (work->field_14 > 9.0f) {
                        work->field_14 = 9.0f;
                    }
                    if ((s32)work->field_14 > (s32)old) {
                        work->field_24 = 0x3C;
                        psndSFXOn("SFX_BTL_SAC_BODY6");
                        if (audienceNum >= 1 && audienceNum <= 0x31) {
                            BattleAudienceSoundCallKind(1);
                        }
                        if (audienceNum >= 0x32 && audienceNum <= 0x63) {
                            BattleAudienceSoundCallKind(1);
                            BattleAudienceSoundHandBeat();
                        }
                        if (audienceNum >= 0x64 && audienceNum <= 0x95) {
                            BattleAudienceSoundCallKind(2);
                        }
                        if (audienceNum >= 0x96) {
                            BattleAudienceSoundCallKind(2);
                            BattleAudienceSoundHandBeat();
                        }
                    }
                    psndSFXOn("SFX_BTL_SAC_BODY4");
                    break;
                case 1:
                    old = work->field_1C;
                    work->field_1C += 0.25f;
                    if (work->field_1C > 9.0f) {
                        work->field_1C = 9.0f;
                    }
                    if ((s32)work->field_1C > (s32)old) {
                        work->field_28 = 0x3C;
                        psndSFXOn("SFX_BTL_SAC_BODY6");
                        if (audienceNum >= 1 && audienceNum <= 0x31) {
                            BattleAudienceSoundCallKind(1);
                        }
                        if (audienceNum >= 0x32 && audienceNum <= 0x63) {
                            BattleAudienceSoundCallKind(1);
                            BattleAudienceSoundHandBeat();
                        }
                        if (audienceNum >= 0x64 && audienceNum <= 0x95) {
                            BattleAudienceSoundCallKind(2);
                        }
                        if (audienceNum >= 0x96) {
                            BattleAudienceSoundCallKind(2);
                            BattleAudienceSoundHandBeat();
                        }
                    }
                    psndSFXOn("SFX_BTL_SAC_BODY4");
                    break;
                case 2:
                    BtlUnit_SetAnim(part, "M_D_1");
                    BtlUnit_GetPos(mario, &x, &y, &z);
                    {
                        EffectEntry* eff =
                            effStampN64Entry(2, x, 20.0f + y, 10.0f + z);

                        ((effStampData*)eff->userdata)->tevReg0R = 0;
                        ((effStampData*)eff->userdata)->tevReg0G = 0xC8;
                        ((effStampData*)eff->userdata)->tevReg0B = 0;
                        ((effStampData*)eff->userdata)->tevReg1R = 0;
                        ((effStampData*)eff->userdata)->tevReg1G = 0x0A;
                        ((effStampData*)eff->userdata)->tevReg1B = 0;
                    }
                    work->field_34 = 0;
                    work->field_00 |= 1;
                    psndSFXOn("SFX_BTL_SAC_BODY5");
                    if (audienceNum >= 1 && audienceNum <= 0x31) {
                        BattleAudienceSoundBooingKind(1);
                    }
                    if (audienceNum >= 0x32 && audienceNum <= 0x63) {
                        BattleAudienceSoundBooingKind(1);
                    }
                    if (audienceNum >= 0x64 && audienceNum <= 0x95) {
                        BattleAudienceSoundBooingKind(2);
                    }
                    if (audienceNum >= 0x96) {
                        BattleAudienceSoundBooingKind(3);
                    }
                    break;
                }
            }
        }
        if (keyGetDirTrg(0) & 0x8000) {
            if (work->field_40 % 3 != 2) {
                work->field_38 = 3;
                work->field_40++;
            }
        }
        if (keyGetDirTrg(0) & 0x4000) {
            if (work->field_40 % 3 != 0) {
                work->field_38 = 3;
                work->field_40--;
            }
        }
        if (keyGetDirTrg(0) & 0x1000) {
            if (work->field_40 / 3 != 0) {
                work->field_38 = 3;
                work->field_40 -= 3;
            }
        }
        if (keyGetDirTrg(0) & 0x2000) {
            if (work->field_40 / 3 != 2) {
                work->field_38 = 3;
                work->field_40 += 3;
            }
        }
        break;
    case 3:
        work->field_38 = 4;
        work->field_3C = 0;
        work->field_50 = work->field_44;
        work->field_5C = work->points[work->field_40].pos;
        psndSFXOn("SFX_BTL_SAC_BODY3");
        /* fallthrough */
    case 4:
        work->field_3C++;
        work->field_74 = (GXColor){0x00, 0x00, 0xFF, 0xFF};
        if (!(work->field_00 & 1)) {
            work->field_44.x = intplGetValue(4, work->field_3C, 5, work->field_50.x, work->field_5C.x);
            work->field_44.y = intplGetValue(4, work->field_3C, 5, work->field_50.y, work->field_5C.y);
            if (work->field_3C >= 5) {
                work->field_38 = 2;
            }
        }
        else {
            work->field_44.x = intplGetValue(4, work->field_3C, 0x19, work->field_50.x, work->field_5C.x);
            work->field_44.y = intplGetValue(4, work->field_3C, 0x19, work->field_50.y, work->field_5C.y);
            if (work->field_3C >= 0x19) {
                work->field_38 = 2;
            }
        }
        break;
    case 5:
        work->field_38 = 6;
        work->field_3C = 0;
        /* fallthrough */
    case 6:
        work->field_3C++;
        work->field_74 = (GXColor){0xFF, 0x00, 0x00, 0xFF};
        if (work->field_3C >= 1) {
            work->field_38 = 2;
            work->field_3C = 0;
        }
        break;
    case 7:
        break;
    }
}
#pragma dont_inline reset

// .text:0x1528 | 0x802482F4 | size: 0x2AC
#pragma dont_inline on
void main_point(s32 index) {
    MukiWork* work = get_ptr();
    MukiPoint* point = &work->points[index];
    MukiEntry* entry;
    s32 i;
    f32 f;

    switch (point->type) {
    case 1:
        point->type = 2;
        point->field_04 = irand(0x1E) + 0x1E;
        {
            s32 row = index / 3;
            s32 col = index % 3;
            Vec pos = {0.0f, 0.0f, 20.0f};
            pos.x = (f32)(col * 0x30 - 0x30);
            pos.y = (f32)(0x7D - row * 0x30);
            point->pos = pos;
        }
        *(Vec*)&point->field_14 = (Vec){0.0f, 0.0f, 1.0f};
        point->field_20 = NULL;
        /* fallthrough */
    case 2:
        point->field_04--;
        if (point->field_04 <= 0x1E) {
            f = intplGetValue(0, 0x1E - point->field_04, 0x1E, 0.0f, 0.5f);
            point->field_18 = f;
            point->field_14 = f;
        }
        if (point->field_04 <= 0) {
            point->type = 3;
            point->field_04 = 0;
        }
        break;
    case 3:
        point->type = 4;
        point->field_04 = 0;
        *(Vec*)&point->field_14 = (Vec){0.5f, 0.5f, 1.0f};
        break;
    case 0:
    case 4:
    case 8:
        break;
    case 5:
        entry = point->field_20;
        if (entry != NULL) {
            if (entry->type == 0) {
                point->field_20 = NULL;
            }
        }
        else if (irand(0x41) == 0) {
            for (i = 0; i < 9; i++) {
                entry = &work->entries[i];
                if (entry->type == 0) {
                    entry->type = 1;
                    entry->pos = point->pos;
                    entry->pos.z = 0.0f;
                    point->field_20 = entry;
                    break;
                }
            }
        }
        break;
    case 6:
        point->type = 7;
        point->field_04 = irand(0x1E) + 0x1E;
        point->field_20 = NULL;
        /* fallthrough */
    case 7:
        point->field_04--;
        if (point->field_04 <= 0x1E) {
            f = intplGetValue(0, 0x1E - point->field_04, 0x1E, 0.5f, 0.0f);
            point->field_18 = f;
            point->field_14 = f;
        }
        if (point->field_04 <= 0) {
            point->type = 8;
            point->field_04 = 0;
        }
        break;
    }
}
#pragma dont_inline reset

// .text:0x12A0 | 0x8024806C | size: 0x288
#pragma dont_inline on
void main_icon(s32 index) {
    MukiWork* work = get_ptr();
    MukiEntry* entry = &work->entries[index];
    effStarStoneData* data;

    switch (entry->type) {
    case 0:
        entry->pos = (Vec){0.0f, -1000.0f, 0.0f};
        entry->alpha = 0xFF;
        break;
    case 1:
        entry->type = 2;
        entry->field_04 = 0;
        entry->mode = irand(3);
        entry->rot = 0.0f;
        entry->scale = (Vec){0.3f, 0.3f, 0.3f};
        /* fallthrough */
    case 2:
        entry->field_04++;
        entry->rot = intplGetValue(4, entry->field_04, 0x1E, 180.0f, 1080.0f);
        entry->alpha = (u8)intplGetValue(0, entry->field_04, 0x1E, 0.0f, 255.0f);
        if (entry->field_04 >= 0x1E) {
            entry->type = 7;
        }
        break;
    case 3:
        entry->type = 4;
        entry->field_04 = 0;
        entry->rot = 0.0f;
        /* fallthrough */
    case 4:
        entry->field_04++;
        entry->rot = intplGetValue(1, entry->field_04, 0x1E, 0.0f, 900.0f);
        entry->alpha = (u8)intplGetValue(0, entry->field_04, 0x1E, 255.0f, 0.0f);
        if (entry->field_04 >= 0x1E) {
            entry->type = 0;
        }
        break;
    case 5:
        entry->type = 6;
        /* fallthrough */
    case 6:
        entry->type = 0;
        break;
    case 7:
        entry->type = 8;
        entry->field_04 = 0;
        entry->rot = 0.0f;
        break;
    case 8:
        entry->field_04++;
        entry->alpha = 0xFF;
        if (entry->field_04 >= 0x5A) {
            entry->type = 3;
        }
        break;
    case 9:
        if (entry->alpha >= 8) {
            entry->alpha -= 8;
        }
        else {
            entry->alpha = 0;
        }
        break;
    }

    if (entry->effect != NULL) {
        data = entry->effect->userdata;
        data->x = entry->pos.x;
        data->y = entry->pos.y;
        data->z = entry->pos.z;
        data->tipRotY = entry->field_24;
        data->rotY = entry->rot;
        data->tipRotZ = entry->field_2C;
        data->scale = 9.0f * entry->scale.x;
        data->tevReg0A = entry->alpha;
    }
}
#pragma dont_inline reset

#pragma dont_inline on
static void main_star(void) {
    MukiWork* work = get_ptr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    effStarStoneData* data;
    f32 f;

    switch (work->field_3B4) {
    case 0:
        break;
    case 1:
        {
            f32 zero = 0.0f;

            work->field_3B4 = 2;
            {
                f32 neg = -1000.0f;

                work->field_3B8 = 0;
                work->starStone = effStarStoneEntry(2, zero, neg, zero, 1.0f);
            }
        }
        BtlUnit_GetPos(mario, &work->posX, &work->posY, &work->posZ);
        work->posY += mario->mSizeMultiplier * (f32)mario->mHeight + 37.0f;
        *(Vec*)&work->startX = *(Vec*)&work->posX;
        *(Vec*)&work->endX = *(Vec*)&work->posX;
        work->endY += 50.0f;
        work->endZ += -1.0f;
        work->scale = (Vec){0.0f, 0.0f, 0.0f};
        work->scale2 = (Vec){0.0f, 0.0f, 0.0f};
        /* fallthrough */
    case 2:
        work->field_3B8++;
        if (work->field_3B8 <= 0x64) {
            work->posX = intplGetValue(0, work->field_3B8, 0x64, work->startX, work->endX);
            work->posY = intplGetValue(0, work->field_3B8, 0x64, work->startY, work->endY);
            work->posZ = intplGetValue(0, work->field_3B8, 0x64, work->startZ, work->endZ);
            f = intplGetValue(0, work->field_3B8, 0x64, 0.0f, 2.0f);
            work->scale.z = f;
            work->scale.y = f;
            work->scale.x = f;
        }
        else {
            f32 two = 2.0f;

            work->posX = work->endX;
            work->posY = work->endY;
            work->posZ = work->endZ;
            work->scale.z = two;
            work->scale.y = two;
            work->scale.x = two;
        }
        work->scale2.y = intplGetValue(4, work->field_3B8, 0x78, 0.0f, 2160.0f);
        if (work->field_3B8 >= 0x78) {
            work->field_3B4 = 3;
            work->field_3B8 = 0;
            *(Vec*)&work->startX = *(Vec*)&work->posX;
            work->endY = 300.0f;
        }
        break;
    case 3:
        work->field_3B8++;
        work->posY = intplGetValue(1, work->field_3B8, 0x3C, work->startY, work->endY);
        if (work->field_3B8 >= 0x3C) {
            work->field_3B4 = 5;
            work->field_3B8 = 0;
        }
        break;
    case 4:
    case 5:
        break;
    }

    if (work->starStone != NULL) {
        f32 s = 1.5f;

        data = work->starStone->userdata;
        data->x = work->posX;
        data->y = work->posY;
        data->z = work->posZ;
        data->tipRotY = work->scale2.x;
        data->rotY = work->scale2.y;
        data->tipRotZ = work->scale2.z;
        data->scale = s * work->scale.x;
    }
}
#pragma dont_inline reset

// .text:0xF80 | 0x80247CE0 | size: 0x6C
#pragma dont_inline on
s32 end_muki(EventEntry* event, BOOL isFirstCall) {
    MukiWork* work = get_ptr();
    s32 off;
#ifdef MUST_MATCH
    // MUST_MATCH: the all-ones mask (a permuter find, Claude #3) gives the loop's induction variables the
    // original's registers; `(u32)off` alone doesn't.
    int mask;
#endif
    s32 i;
    MukiEntry* entry;

    effDelete(((effStarStoneData*)work->starStone->userdata)->dustEffect);
    effDelete(work->starStone);
#ifdef MUST_MATCH
    mask = -1;
#endif
    for (i = 0, off = 0; i < 9; i++, off += 0x38) {
#ifdef MUST_MATCH
        entry = (MukiEntry*)((u8*)work + (off & mask) + 0x1BC);
#else
        entry = &work->entries[i];
#endif
        effDelete(entry->effect);
    }
    return 2;
}
#pragma dont_inline reset

// Each gauge's screen point is built through an aggregate return, so the template copy lands in a
// stack temp and is then copied to the caller's Vec, as the original schedules it. `y` is the
// helper's own local, read after `v.x` is set: as an inline local it ranks below the `field_2C`
// load and takes f2, as in the original. MATCH_F32 keeps MWCC from propagating it into `v.y`.
static inline Vec mukiScreenVec0(MukiWork* work) {
    Vec v = {0.0f, 0.0f, 0.0f};
    f32 y;
    v.x = work->field_2C;
    y = MATCH_F32(work->field_30);
    v.y = y;
    v.z = 0.0f;
    return v;
}

static inline Vec mukiScreenVec1(MukiWork* work) {
    Vec v = {0.0f, 0.0f, 0.0f};
    f32 y;
    v.x = work->field_2C;
    y = MATCH_F32(work->field_30) - 30.0f;
    v.y = y;
    v.z = 0.0f;
    return v;
}

// .text:0x908 | 0x802475F4 | size: 0x6EC
// The by-value colour pair goes through a `GXColor argv[4]` filled in reverse index order, which
// gives the target's 0x6c,0x68,0x74,0x70 stores. `frac` is block-local so `left` coalesces into
// `sx`'s register. The gauge offsets `w`/`len` are locals: as literals MWCC puts the constant first
// in `sx + 32` and `frac * 80` and folds `sx - 32 - 8` across the calls; the locals keep the
// original's operand order. Block 1 reads the screen point in the first call's arguments, so `sx`
// is loaded straight into its callee-saved register.
#pragma dont_inline on
static void disp_2D(CameraId cameraId, void* param) {
    MukiWork* work = get_ptr();
    f32 w = 32.0f;
    f32 len = 80.0f;
    Mtx mtx;
    Vec sp;
    GXColor c;
    GXColor argv[4];
    f32 sy;
    f32 d;
    f32 sx;
    f32 left;
    u8 cr;
    u8 cg;
    u8 cb;
    u8 ca;
    s32 n;

    if (work->state >= 2 && work->state <= 7) {
        f32 frac;
        sp = mukiScreenVec0(work);
        btlGetScreenPoint(&sp, &sp);
        c = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
        PSMTXTrans(mtx, (sx = sp.x) - w - 8.0f, sy = sp.y, 0.0f);
        btlDispTexPlane2(mtx, 0x43, c, 0);
        PSMTXScale(mtx, 16.0f, 1.0f, 1.0f);
        PSMTXTransApply(mtx, mtx, sx, sy, 0.0f);
        btlDispTexPlane2(mtx, 0x44, c, 0);
        PSMTXScale(mtx, -1.0f, 1.0f, 1.0f);
        PSMTXTransApply(mtx, mtx, sx + w + 8.0f, sy, 0.0f);
        btlDispTexPlane2(mtx, 0x43, c, 0);

        left = sx - w - 8.0f;
        frac = work->field_18 - (f32)(s32)work->field_18;
        btlDispGXInit2DRasta();
        GXLoadPosMtxImm(camGetPtr(8)->view, 0);
        d = work->field_14 - work->field_18;
        if (0.0f != d) {
            cr = 0xFF;
            cg = 0xB4;
            cb = 0xB4;
            ca = 0xFF;
        }
        else {
            cr = 0xF0;
            cg = 0xA0;
            cb = 0xA0;
            ca = 0xFF;
        }
        btlDispGXQuads2DRasta(left, 8.0f + sy, frac * len + left,
                              sy - 8.0f, cr, cg, cb, ca);
        n = work->field_24 % 12;
        if (n >= 0 && n <= 5) {
            PSMTXScale(mtx, 0.5f, 0.5f, 0.5f);
            PSMTXTransApply(mtx, mtx, left - 30.0f, sy, 0.0f);
            argv[1] = (GXColor){0xE9, 0x4A, 0x3F, 0xFF};
            argv[0] = argv[1];
            argv[3] = (GXColor){0xFE, 0xE6, 0x3E, 0xFF};
            argv[2] = argv[3];
            btlDispTexPlane3(mtx, 0x45, argv[0], argv[1], argv[2], argv[3], 0);
            PSMTXScale(mtx, 0.7f, 0.7f, 0.7f);
            PSMTXTransApply(mtx, mtx, left - 35.0f, sy - 14.0f, 0.0f);
            iconDispGx2(mtx, 0x10, 0x1ED);
            PSMTXScale(mtx, 0.7f, 0.7f, 0.7f);
            PSMTXTransApply(mtx, mtx, left - 25.0f, sy - 14.0f, 0.0f);
            c = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
            iconNumberDispGx(mtx, (s32)work->field_18, 0, c);
        }
    }

    if (work->state >= 2 && work->state <= 7) {
        f32 frac;
        sp = mukiScreenVec1(work);
        btlGetScreenPoint(&sp, &sp);
        sx = sp.x;
        sy = sp.y;
        c = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
        PSMTXTrans(mtx, sx - w - 8.0f, sy, 0.0f);
        btlDispTexPlane2(mtx, 0x43, c, 0);
        PSMTXScale(mtx, 16.0f, 1.0f, 1.0f);
        PSMTXTransApply(mtx, mtx, sx, sy, 0.0f);
        btlDispTexPlane2(mtx, 0x44, c, 0);
        PSMTXScale(mtx, -1.0f, 1.0f, 1.0f);
        PSMTXTransApply(mtx, mtx, sx + w + 8.0f, sy, 0.0f);
        btlDispTexPlane2(mtx, 0x43, c, 0);

        left = sx - w - 8.0f;
        frac = work->field_20 - (f32)(s32)work->field_20;
        btlDispGXInit2DRasta();
        GXLoadPosMtxImm(camGetPtr(8)->view, 0);
        if (0.0f != d) {
            cr = 0xB4;
            cg = 0xB4;
            cb = 0xFF;
            ca = 0xFF;
        }
        else {
            cr = 0xA0;
            cg = 0xA0;
            cb = 0xF0;
            ca = 0xFF;
        }
        btlDispGXQuads2DRasta(left, 8.0f + sy, frac * len + left,
                              sy - 8.0f, cr, cg, cb, ca);
        n = work->field_28 % 12;
        if (n >= 0 && n <= 5) {
            PSMTXScale(mtx, 0.5f, 0.5f, 0.5f);
            PSMTXTransApply(mtx, mtx, left - 30.0f, sy, 0.0f);
            argv[1] = (GXColor){0x3F, 0x3F, 0xE9, 0xFF};
            argv[0] = argv[1];
            argv[3] = (GXColor){0x74, 0xC9, 0xFF, 0xFF};
            argv[2] = argv[3];
            btlDispTexPlane3(mtx, 0x45, argv[0], argv[1], argv[2], argv[3], 0);
            PSMTXScale(mtx, 0.7f, 0.7f, 0.7f);
            PSMTXTransApply(mtx, mtx, left - 35.0f, sy - 14.0f, 0.0f);
            iconDispGx2(mtx, 0x10, 0x1ED);
            PSMTXScale(mtx, 0.7f, 0.7f, 0.7f);
            PSMTXTransApply(mtx, mtx, left - 25.0f, sy - 14.0f, 0.0f);
            c = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
            iconNumberDispGx(mtx, (s32)work->field_20, 0, c);
        }
    }
}
#pragma dont_inline reset

// .text:0x530 | 0x802472FC | size: 0x2F8
#pragma dont_inline on
static void disp_3D(CameraId cameraId, void* param) {
    MukiWork* work = get_ptr();
    MukiPoint* point;
    Mtx m;
    s32 arr0[8] = {-8, 8, 8, 8, 8, -8, -8, -8};
    s32 arr1[8] = {-24, 24, 24, 24, 24, -24, -24, -24};
    s32 arr2[8] = {1, 1, -1, 1, -1, -1, 1, -1};
    GXColor color;
    s32 i;
    s32 t;

    for (i = 0; i < 9; i++) {
        point = &work->points[i];
        if (point->type >= 2 && point->type < 8) {
            for (t = 0; t < 4; t++) {
                PSMTXScale(m, (f32)arr2[t * 2] * point->field_14,
                           (f32)arr2[t * 2 + 1] * point->field_18, point->field_1C);
                PSMTXTransApply(m, m, point->pos.x, point->pos.y, point->pos.z);
                PSMTXTransApply(m, m, (f32)arr0[t * 2] * point->field_14,
                                (f32)arr0[t * 2 + 1] * point->field_18,
                                0.0f);
                color = (GXColor){0xDE, 0xDE, 0xDE, 0xFF};
                btlDispTexPlane2(m, 0x42, color, 0);
            }
        }
    }
    if (work->field_38 >= 2 && work->field_38 < 7) {
        for (t = 0; t < 4; t++) {
            PSMTXScale(m, (f32)arr2[t * 2] * work->field_68.x,
                       (f32)arr2[t * 2 + 1] * work->field_68.y, work->field_68.z);
            PSMTXTransApply(m, m, work->field_44.x, work->field_44.y, work->field_44.z);
            PSMTXTransApply(m, m, (f32)arr1[t * 2] * work->field_68.x,
                            (f32)arr1[t * 2 + 1] * work->field_68.y,
                            0.0f);
            color = work->field_74;
            btlDispTexPlane2(m, 0x41, color, 0);
        }
    }
}
#pragma dont_inline reset

#ifdef MUST_MATCH
// MUST_MATCH: building each colour through an inline that returns it makes MWCC keep the source
// colour in the original's stack slot and rank the colour/alpha webs the original's way. The alpha
// parameter must come first: with `(GXColor c, u8 a)` the case-2 word/alpha land in r0/r7, with
// `(u8 a, GXColor c)` they land in the original's r4/r0. The inline folds away.
static inline GXColor mukiAlphaColor(u8 a, GXColor c) { c.a = a; return c; }
#endif
// dont_inline so the original's own helpers stay real calls and its argument copies survive
#pragma dont_inline on
// case 0 keeps its own rotation angle so its f30/f31 webs are not merged with cases 1 and 2
// .text:0x1A4 | 0x80246F70 | size: 0x38C
static void disp_3D_alpha(CameraId cameraId, void* param) {
    f32 pi = 3.141592f;
    f32 angle0;
    MukiWork* work = get_ptr();
    MukiEntry* entry;
    Mtx mtx;
    GXColor cA;
    GXColor cB;
    GXColor argv[4];
    f32 f;
    f32 fc;
    f32 fs;
    s32 i;

    for (i = 0; i < 9; i++) {
        entry = &work->entries[i];
        if (entry->type >= 1 && entry->type <= 9) {
            switch (entry->mode) {
            case 0:
                PSMTXRotRad(mtx, 'y', 0.017453292f * entry->rot);
                PSMTXScaleApply(mtx, mtx, 2.0f * entry->scale.x,
                                2.0f * entry->scale.y, entry->scale.z);
                PSMTXTransApply(mtx, mtx, entry->pos.x, entry->pos.y, entry->pos.z);
                angle0 = 2.0f * entry->rot * pi / 360.0f;
                fc = (f32)cos(angle0);
                fs = (f32)sin(angle0);
                PSMTXTransApply(mtx, mtx, 15.0f * fs,
                                0.0f, 15.0f * fc);
#ifdef MUST_MATCH
                cA = mukiAlphaColor(entry->alpha, (GXColor){0xE9, 0x4A, 0x3F, 0x00});
                cB = mukiAlphaColor(entry->alpha, (GXColor){0xFE, 0xE6, 0x3E, 0x00});
#else
                cA = (GXColor){0xE9, 0x4A, 0x3F, 0x00};
                cA.a = entry->alpha;
                cB = (GXColor){0xFE, 0xE6, 0x3E, 0x00};
                cB.a = entry->alpha;
#endif
                // reverse-order fill, each colour's two copies written together, so MWCC emits
                // the original's grouped colour stores; the call then takes the array in order.
                argv[1] = cA; argv[0] = cA; argv[3] = cB; argv[2] = cB;
                btlDispTexPlane3(mtx, 0x45, argv[0], argv[1], argv[2], argv[3], 0);
                break;
            case 1:
                PSMTXRotRad(mtx, 'y', 0.017453292f * entry->rot);
                PSMTXScaleApply(mtx, mtx, 2.0f * entry->scale.x,
                                2.0f * entry->scale.y, entry->scale.z);
                PSMTXTransApply(mtx, mtx, entry->pos.x, entry->pos.y, entry->pos.z);
                f = 2.0f * entry->rot * pi / 360.0f;
                fc = (f32)cos(f);
                fs = (f32)sin(f);
                PSMTXTransApply(mtx, mtx, 15.0f * fs,
                                0.0f, 15.0f * fc);
#ifdef MUST_MATCH
                cA = mukiAlphaColor(entry->alpha, (GXColor){0x3F, 0x3F, 0xE9, 0x00});
                cB = mukiAlphaColor(entry->alpha, (GXColor){0x74, 0xC9, 0xFF, 0x00});
#else
                cA = (GXColor){0x3F, 0x3F, 0xE9, 0x00};
                cA.a = entry->alpha;
                cB = (GXColor){0x74, 0xC9, 0xFF, 0x00};
                cB.a = entry->alpha;
#endif
                argv[1] = cA; argv[0] = cA; argv[3] = cB; argv[2] = cB;
                btlDispTexPlane3(mtx, 0x45, argv[0], argv[1], argv[2], argv[3], 0);
                break;
            case 2:
                PSMTXRotRad(mtx, 'y', 0.017453292f * entry->rot);
                PSMTXScaleApply(mtx, mtx, entry->scale.x, entry->scale.y, entry->scale.z);
                PSMTXTransApply(mtx, mtx, entry->pos.x, entry->pos.y, entry->pos.z);
                f = 2.0f * entry->rot * pi / 360.0f;
                fc = (f32)cos(f);
                fs = (f32)sin(f);
                PSMTXTransApply(mtx, mtx, 15.0f * fs,
                                0.0f, 15.0f * fc);
#ifdef MUST_MATCH
                argv[0] = mukiAlphaColor(entry->alpha, (GXColor){0xFF, 0xFF, 0xFF, 0x00});
#else
                argv[0] = (GXColor){0xFF, 0xFF, 0xFF, 0x00};
                argv[0].a = entry->alpha;
#endif
                btlDispTexPlane2(mtx, 0x31, argv[0], 0);
                break;
            }
        }
    }
}
#pragma dont_inline reset

// .text:0x198 | 0x80246F64 | size: 0xC
#pragma dont_inline on
static MukiWork* get_ptr(void) {
    return (MukiWork*)&_battleWorkPointer->actionCommands.field_0x2BC[0];
}
#pragma dont_inline reset

// .text:0xDC | 0x80246EA8 | size: 0xBC
#pragma dont_inline on
static s32 set_weapon(EventEntry* event, BOOL isFirstCall) {
    MukiWork* work;
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    work = get_ptr();
    memcpy(work->weapon, &weapon, 0xC0);
    switch (type) {
    case 0:
        work->weapon[0x96] = 0x64;
        work->weapon[0x97] = 3;
        work->weapon[0x98] = value;
        break;
    case 1:
        work->weapon[0x99] = 0x64;
        work->weapon[0x9A] = 3;
        work->weapon[0x9B] = value;
        break;
    }
    evtSetValue(event, args[2], (s32)work->weapon);
    return 2;
}
#pragma dont_inline reset

// .text:0xB0 | 0x80246E7C | size: 0x2C
#pragma dont_inline on
static s32 start_game(EventEntry* event, BOOL isFirstCall) {
    MukiWork* work = get_ptr();
    work->state = 1;
    return 2;
}
#pragma dont_inline reset

// .text:0x2C | 0x80246DF8 | size: 0x84
#pragma dont_inline on
static s32 wait_game_end(EventEntry* event, BOOL isFirstCall) {
    MukiWork* work = get_ptr();
    s32* args = event->args;

    if (work->state == 7) {
        evtSetValue(event, args[0], (s32)work->field_14);
        evtSetValue(event, args[1], (s32)work->field_1C);
        return 2;
    }
    return 0;
}
#pragma dont_inline reset

// The original never inlines its own helpers: every call to one is a real `bl`.
#pragma dont_inline on

// .text:0x0 | 0x80246DCC | size: 0x2C
static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall) {
    MukiWork* work = get_ptr();
    work->field_3B4 = 1;
    return 2;
}
#pragma dont_inline reset

EVT_BEGIN(marioAttackEvent_Muki)
    CALL(btlevtcmd_GetUnitId, -4, LW(14))
    CALL(btlevtcmd_GetBodyId, LW(14), LW(15))
    CALL(btlevtcmd_CheckStatus, LW(14), 27, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(14), -1)
    END_IF
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(0))
    CALL(btlevtcmd_reset_move_color_lv_all)
    BEGIN_SIBLING_EVENT
        CALL(main_muki, LW(14))
    END_SIBLING_EVENT
    CALL_CHILD(sac_start_camera_evt)
    WAIT_MS(500)
    CALL(sac_enemy_slide_go)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_START1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL_CHILD(sac_mario_item_on_evt)
    WAIT_MS(500)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_START2"), EVTDAT_POINTER_MAX, 0, LW(11))
    CALL(star_stone_appear)
    BEGIN_SIBLING_EVENT
        CALL_CHILD(sac_start_stardust)
    END_SIBLING_EVENT
    WAIT_FRAMES(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_START3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_snd_sfxoff, LW(11))
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(40)
    CALL_CHILD(sac_mario_item_on_end_evt)
    CALL_CHILD(sac_mario_back_home)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_I_Y"))
    WAIT_MS(1000)
    BEGIN_SIBLING_EVENT
        CALL(sac_handbeat)
    END_SIBLING_EVENT
    CALL(sac_ac_help_on)
    CALL(start_game)
    CALL(wait_game_end, LW(0), LW(1))
    CALL(sac_ac_help_off)
    CALL(sac_handbeat_end)
    WAIT_MS(500)
    SET_VALUE(LW(2), LW(0))
    ADD(LW(2), LW(1))
    IF_INT_NOT_EQUAL(LW(2), 0)
        CALL(evt_btl_camera_set_mode, 0, 7)
        CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
        CALL(evt_btl_camera_set_zoom, 0, 200)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(evt_btl_camera_set_zoomSpeedLv, 0, 2)
        WAIT_MS(1000)
        CALL(sac_wao)
        BEGIN_INLINE_EVENT
            WAIT_MS(1000)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(sac_enemy_slide_return)
            CALL(end_muki)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            WAIT_MS(400)
            IF_INT_NOT_EQUAL(LW(1), 0)
                CALL(set_weapon, 1, LW(1), LW(9))
                IF_INT_NOT_EQUAL(LW(14), -1)
                    CALL(btlevtcmd_CheckDamage, -2, LW(14), LW(15), LW(9), 256, LW(5))
                END_IF
                CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(9), 256, LW(5))
            END_IF
        END_INLINE_EVENT
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(set_weapon, 0, LW(0), LW(9))
            IF_INT_NOT_EQUAL(LW(14), -1)
                CALL(btlevtcmd_CheckDamage, -2, LW(14), LW(15), LW(9), 256, LW(5))
            END_IF
            CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(9), 256, LW(5))
            RETURN
        END_IF
    END_IF
    CALL(sac_enemy_slide_return)
    WAIT_MS(1000)
    CALL(end_muki)
    WAIT_MS(1000)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()
