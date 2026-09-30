#include "unit_boss_moamoa.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/extdrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_npc.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "mario/mariost.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include <stdlib.h>
#include <string.h>

#pragma pool_data off

typedef struct MoamoaWorkPose {
    u16 field_0x0; // 0x0
    u8 pad_0x2[2];
    f32 field_0x4;     // 0x4
    f32 field_0x8;     // 0x8
    f32 field_0xC;     // 0xC
    s16 field_0x10;    // 0x10
    u8 pad_0x12[2];
    s32 field_0x14;    // 0x14
    OSTime field_0x18; // 0x18
    s16 groupIdx;      // 0x20
    s16 seq;           // 0x22
    f32 frame;         // 0x24
    void* animPose;    // 0x28
    s32 type;          // 0x2C
    void* field_0x30;  // 0x30
    f32 field_0x34[3]; // 0x34
    f32 field_0x40[3]; // 0x40
    u8 pad_0x4C[4];
} MoamoaWorkPose; // 0x50

typedef struct MoamoaWorkAnim {
    char field_0x0[0x10]; // 0x0
    char field_0x10[8];   // 0x10
    f32 field_0x18;       // 0x18
    s32 field_0x1C;       // 0x1C
    s32 field_0x20;       // 0x20
} MoamoaWorkAnim; // 0x24

typedef struct MoamoaWork {
    u8 pad_0x0[4];
    s32 countA;            // 0x4
    MoamoaWorkPose* poses; // 0x8
    s32 countB;            // 0xC
    MoamoaWorkAnim anims[32]; // 0x10
    s16 seqCounts[0x80];    // 0x490
    s16 groupCounts[0x100]; // 0x590
    void* field_0x790[5];   // 0x790
    s32 mainUnitId;         // 0x7A4
    f32 field_0x7A8[3];     // 0x7A8
    f32 field_0x7B4;        // 0x7B4
    f32 field_0x7B8;        // 0x7B8
    f32 field_0x7BC;        // 0x7BC
    f32 field_0x7C0;        // 0x7C0
} MoamoaWork; // 0x7C4

typedef struct MoamoaPoseArg {
    u8 pad_0x0[0x10];
    s16 field_0x10;   // 0x10
    u8 pad_0x12[2];   // 0x12
    s32 field_0x14;   // 0x14
    OSTime field_0x18; // 0x18
    u8 pad_0x20[0xC]; // 0x20
    s32 type;         // 0x2C
} MoamoaPoseArg;

static MoamoaWork* wp;

static u32 group_tbl[22];

#pragma force_active on
extern BattleUnitKind unit_boss_moamoa_mouth;
extern BattleUnitKind unit_boss_moamoa_tentacle_a;
extern BattleUnitKind unit_boss_moamoa_tentacle_b;
extern BattleUnitKind unit_boss_moamoa_tentacle_c;
static void ext_moamoa_main(void); // TODO: real signature
static void ext_moamoa_init(void); // TODO: real signature
static f32 randf(f32 min, f32 max);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[2];
static BattleUnitKindPart parts[1];
static s32 init_event[159];
static s32 damage_event[10];
static s32 dead_event[349];
static s32 phase_event[159];
static s32 attack_event[19];
static s32 wait_event[6];
static s32 evt_moamoa[151];
s32 MOAMOA_SET_BODYSCALE[4];

// clang-format off
extend_data(36,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(37,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boss_moamoa = {
    UNIT_SMORG,                 // mUnitType
    "btl_un_moamoa",            // name
    50,                         // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    78,                         // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // field_0x13
    6,                          // field_0x14
    160,                        // width
    80,                         // height
    { 0, 24 },                  // statusIconOffset
    { 0.0f, 0.0f, 0.0f },       // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 280.0f, 40.0f, 0.0f },    // talkTogeBaseOffset
    { 80.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    15.0f,                      // field_0x54
    10.0f,                      // field_0x58
    { 80.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 10.0f, 80.0f, 20.0f },    // kissHitOffset
    { 0.0f, 70.0f, 0.0f },      // cutBaseOffset
    160.0f,                     // cutWidth
    80.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    -1,                         // swallowRate
    0,                          // swallowAttribute
    0,                          // field_0x8C
    0,                          // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_MOA_G_DAMAGED1",  // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x00200000,                 // attributeFlags
    &regist,                    // vulnStatus
    1,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

BattleUnitDefense moamoa_defence = { { 2, 2, 2, 2, 2 } };

BattleUnitDefense moamoa_defence_no_parts = { { 0, 0, 0, 0, 0 } };

BattleUnitDefense moamoa2_defence = { { 1, 1, 1, 1, 1 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    0, // sleepRate
    0, // stopRate
    0, // dizzyRate
    0, // poisonRate
    0, // confuseRate
    0, // electricRate
    0, // burnRate
    0, // freezeRate
    0, // hugeRate
    0, // tinyRate
    0, // attackUpRate
    0, // attackDownRate
    0, // defenseUpRate
    0, // defenseDownRate
    0, // allergicRate
    0, // frightRate
    0, // galeForceRate
    0, // fastRate
    0, // slowRate
    0, // dodgyRate
    0, // invisibleRate
    0, // instaKillRate
};

BattleDispPose moamoa1_pose_table[4] = {
    { 0x1C, "S_1_1" },
    { 0x1F, "D_1_1" },
    { 0x27, "D_1_1" },
    { 0x45, "S_1_1" },
};

BattleDispPose moamoa1_weakness_pose_table[4] = {
    { 0x1C, "S_1_3" },
    { 0x1F, "D_1_3" },
    { 0x27, "D_1_3" },
    { 0x45, "S_1_3" },
};

static BattleDataEntry data_table[2] = {
    { 0x31, dead_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                       // partNum
        "btl_un_moamoa",         // partName
        "c_g_moamoa_b",          // modelName
        { 0.0f, 0.0f, 0.0f },    // positionOffset
        { -2.0f, 53.0f, 20.0f }, // hitBaseOffset
        { -4.0f, 63.0f, 20.0f }, // hitCursorBaseOffset
        20,                      // field_0x30
        30,                      // field_0x32
        255,                     // baseAlpha
        &moamoa_defence,         // defense
        &defence_attr,           // defenseAttr
        0x20000001,              // attributes
        0,                       // counterAttributes
        moamoa1_pose_table,      // poseTable
    },
};

BattleUnitSetup entry_boss_moamoa_tentacle_a[1] = {
    { &unit_boss_moamoa_tentacle_a, 1, 0x04000004, { 0.0f, -1000.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_boss_moamoa_tentacle_b[1] = {
    { &unit_boss_moamoa_tentacle_b, 1, 0x04000004, { 0.0f, -1000.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_boss_moamoa_tentacle_c[1] = {
    { &unit_boss_moamoa_tentacle_c, 1, 0x04000004, { 0.0f, -1000.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleUnitSetup entry_boss_moamoa_mouth[1] = {
    { &unit_boss_moamoa_mouth, 1, 0x04000004, { 0.0f, -1000.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleWeapon weapon_moamoa_tsunami_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
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
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

EVT_BEGIN(moamoa_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    IF_INT_NOT_EQUAL(LW(0), 2)
        SET_VALUE(LW(1), 30)
    ELSE
        SET_VALUE(LW(1), 1)
    END_IF
    CALL(evt_btl_camera_set_moveto, 0, 83, 144, 512, 83, 109, 0, LW(1), 11)
    IF_INT_EQUAL(LW(0), 1)
        WAIT_MS(600)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(mario_party_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    IF_INT_NOT_EQUAL(LW(0), 2)
        SET_VALUE(LW(1), 30)
    ELSE
        SET_VALUE(LW(1), 1)
    END_IF
    CALL(evt_btl_camera_set_moveto, 0, -120, 58, 257, -120, 40, 0, LW(1), 11)
    IF_INT_EQUAL(LW(0), 1)
        WAIT_MS(600)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 2)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_GetUnitId, -2, LW(4))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_boss_moamoa_tentacle_c), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    SET_VALUE(LW(5), 3)
    CALL(btlevtcmd_SetUnitWork, -2, LW(5), LW(3))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(4))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_boss_moamoa_tentacle_b), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    SET_VALUE(LW(5), 4)
    CALL(btlevtcmd_SetUnitWork, -2, LW(5), LW(3))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(4))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_boss_moamoa_tentacle_a), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    SET_VALUE(LW(5), 5)
    CALL(btlevtcmd_SetUnitWork, -2, LW(5), LW(3))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(4))
    SET_VALUE(LW(5), 6)
    CALL(btlevtcmd_SetUnitWork, -2, LW(5), -1)
    CALL_CHILD(evt_moamoa)
    SPAWN_EVENT(MOAMOA_SET_BODYSCALE)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

char* MOAMOA_TAIJOU[4] = {
    "S_1_2", NULL, "L_1_4", NULL,
};

USER_FUNC(boss_moamoa_check_zenei_kouei) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);
    s32 index = args[1];
    BattleUnit* unit = BattleGetUnitPtr(work, BattleTransID(event, value));
    f32 x, y, z;

    BtlUnit_GetPos(unit, &x, &y, &z);
    if (x >= -120.0f) {
        evtSetValue(event, index, 0);
    } else {
        evtSetValue(event, index, 1);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_moamoa_mainUnitIdClear) {
    wp->mainUnitId = -1;
    return EVT_RETURN_DONE;
}

static f32 randf(f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;
    f32 d = max - min;
    return r * d + min;
}

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 1)
        SET_VALUE(LW(0), 5)
        CALL(btlevtcmd_GetUnitWork, -2, LW(0), LW(1))
        IF_INT_NOT_EQUAL(LW(1), -1)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AnimeChangePose, LW(1), 1, STRING("R_1A"))
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AnimeWaitPlayComplete, LW(1), 1)
            CALL(btlevtcmd_KillUnit, LW(1), 1)
            CALL(btlevtcmd_SetUnitWork, -2, LW(0), -1)
        END_IF
        SET_VALUE(LW(0), 4)
        CALL(btlevtcmd_GetUnitWork, -2, LW(0), LW(1))
        IF_INT_NOT_EQUAL(LW(1), -1)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AnimeChangePose, LW(1), 1, STRING("R_1B"))
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AnimeWaitPlayComplete, LW(1), 1)
            CALL(btlevtcmd_KillUnit, LW(1), 1)
            CALL(btlevtcmd_SetUnitWork, -2, LW(0), -1)
        END_IF
        SET_VALUE(LW(0), 3)
        CALL(btlevtcmd_GetUnitWork, -2, LW(0), LW(1))
        IF_INT_NOT_EQUAL(LW(1), -1)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AnimeChangePose, LW(1), 1, STRING("R_1C"))
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AnimeWaitPlayComplete, LW(1), 1)
            CALL(btlevtcmd_KillUnit, LW(1), 1)
            CALL(btlevtcmd_SetUnitWork, -2, LW(0), -1)
        END_IF
    ELSE
        SET_VALUE(LW(0), 6)
        CALL(btlevtcmd_GetUnitWork, -2, LW(0), LW(1))
        IF_INT_NOT_EQUAL(LW(1), -1)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_DOWN2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AnimeChangePose, LW(1), 1, STRING("L_2_6"))
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AnimeWaitPlayComplete, LW(1), 1)
            CALL(btlevtcmd_KillUnit, LW(1), 1)
            CALL(btlevtcmd_SetUnitWork, -2, LW(0), -1)
        END_IF
    END_IF
    SET_INT_READ_ADDR(PTR(MOAMOA_TAIJOU))
    LOOP(0)
        READ_INT(LW(2))
        IF_INT_EQUAL(LW(2), 0)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    END_LOOP
    WAIT_MS(1000)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_LUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LOOP(0)
        READ_INT(LW(2))
        IF_INT_EQUAL(LW(2), 0)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    END_LOOP
    CALL(moamoa_guruguru)
    WAIT_MS(500)
    SET_VALUE(LW(10), -2)
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    ADD(LW(2), 10)
    CALL(evt_eff, 0, STRING("kemuri_test"), 4, LW(0), LW(1), LW(2), FLOAT(0.8f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetCoin, LW(10), LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, LW(10), LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL(btlevtcmd_GetPos, LW(10), LW(3), LW(1), LW(2))
    ADD(LW(2), 10)
    ADD(LW(1), 30)
    CALL(_add_star_point_disp_offset, LW(10), LW(3), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("star_point"), 0, LW(3), LW(1), LW(2), LW(0), 0, 0, 0, 0, 0, 0, 0)
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        LOOP(LW(0))
        END_LOOP
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_EXPLOSION1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(moamoa_bomb)
    CALL(evt_moamoa_mainUnitIdClear)
    CALL(btlevtcmd_GetBackItem, LW(10))
    CALL(btlevtcmd_CheckDamageCode, LW(10), 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_KillUnit, LW(10), 0)
    ELSE
        CALL(btlevtcmd_KillUnit, LW(10), 2)
    END_IF
    RETURN
EVT_END()

char* moamoa2_change_tbl[7] = {
    "L_2_1", "L_2_3", NULL, "L_2_4", "L_2_5", "S_2", NULL,
};

void moamoa_mainUnitIdClear(void) {
    wp->mainUnitId = -1;
}

EVT_BEGIN(moamoa_tentacl_on)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_RESTORE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("V_2"))
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_GetUnitId, -2, LW(4))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_boss_moamoa_tentacle_c), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    SET_VALUE(LW(5), 3)
    CALL(btlevtcmd_SetUnitWork, -2, LW(5), LW(3))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(4))
    SET_VALUE(LW(6), LW(3))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_boss_moamoa_tentacle_b), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    SET_VALUE(LW(5), 4)
    CALL(btlevtcmd_SetUnitWork, -2, LW(5), LW(3))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(4))
    SET_VALUE(LW(7), LW(3))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_boss_moamoa_tentacle_a), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    SET_VALUE(LW(5), 5)
    CALL(btlevtcmd_SetUnitWork, -2, LW(5), LW(3))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(4))
    CALL(moamoa1_on)
    CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(&moamoa_defence))
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 536870912)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(moamoa1_pose_table))
    CALL(btlevtcmd_OnPartsAttribute, LW(3), 1, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, LW(7), 1, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, LW(6), 1, 16777216)
    WAIT_FRAMES(3)
    CALL(btlevtcmd_OffPartsAttribute, LW(3), 1, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, LW(7), 1, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, LW(6), 1, 16777216)
    CALL(btlevtcmd_AnimeChangePose, LW(3), 1, STRING("B_1A"))
    CALL(btlevtcmd_AnimeChangePose, LW(7), 1, STRING("B_1B"))
    CALL(btlevtcmd_AnimeChangePose, LW(6), 1, STRING("B_1C"))
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AnimeWaitPlayComplete, LW(6), 1)
    RETURN
EVT_END()

// NONMATCHING (99.8%): body exact; MWCC picks a different temp register for `wp` (r4 vs the
// original's r5) in the two `wp->poses` loads (the `zero` constant takes the other).
USER_FUNC(moamoa_guruguru) {
    BattleUnit* unit;
    Vec vec;
    Mtx mtx;
    f32 count;
    f32 angle;
    s32 i;
    MoamoaWorkPose* pose;
    ExtPose* extPose;

    unit = BattleGetUnitPtr(_battleWorkPointer, wp->mainUnitId);
    *(Vec*)wp->field_0x7A8 = (Vec){ 0.0f, 0.0f, 0.0f };

    extPose = extGetPosePtr();
    pose = wp->poses;
    count = 0.0f;
    for (i = 0; i < wp->countA; i++, extPose++, pose++) {
        if (*(s32*)pose->field_0x30 != -1) {
            vec.x = (*extPose->matrix)[0][3];
            vec.y = (*extPose->matrix)[1][3];
            vec.z = (*extPose->matrix)[2][3];
            if (vec.y >= 0.0f) {
                pose->field_0x0 |= 1;
                count += 1.0f;
                PSVECAdd(&vec, (Vec*)wp->field_0x7A8, (Vec*)wp->field_0x7A8);
            }
        }
    }
    PSVECScale((Vec*)wp->field_0x7A8, (Vec*)wp->field_0x7A8, 1.0f / count);
    wp->field_0x7B4 = 0.0f;
    wp->field_0x7B8 = 2.0f;

    extPose = extGetPosePtr();
    angle = 0.0f;
    pose = wp->poses;
    for (i = 0; i < wp->countA; i++, extPose++, pose++) {
        if (pose->field_0x0 & 1) {
            pose->field_0x34[0] = (*extPose->matrix)[0][3];
            pose->field_0x34[1] = (*extPose->matrix)[1][3];
            pose->field_0x34[2] = (*extPose->matrix)[2][3];
            *(Vec*)pose->field_0x40 = (Vec){ 0.0f, 1.0f, 0.0f };
            PSMTXRotRad(mtx, 'Z', angle * 0.017453292f);
            PSMTXMultVec(mtx, (Vec*)pose->field_0x40, (Vec*)pose->field_0x40);
            angle += 22.4375f;
            if (angle >= 360.0f) {
                angle -= 360.0f;
            }
        }
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(moamoa_mouth_on)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MOA_G_ARM_RESTORE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(40)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("V_2"))
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_GetUnitId, -2, LW(4))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_boss_moamoa_mouth), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    SET_VALUE(LW(5), 6)
    CALL(btlevtcmd_SetUnitWork, -2, LW(5), LW(3))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(4))
    CALL(moamoa2_on)
    CALL(btlevtcmd_SetPartsDefenceTable, -2, 1, PTR(&moamoa2_defence))
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(moamoa1_pose_table))
    SET_INT_READ_ADDR(PTR(moamoa2_change_tbl))
    CALL(btlevtcmd_OnPartsAttribute, LW(3), 1, 16777216)
    WAIT_FRAMES(3)
    CALL(btlevtcmd_OffPartsAttribute, LW(3), 1, 16777216)
    LOOP(0)
        READ_INT(LW(2))
        IF_INT_EQUAL(LW(2), 0)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_AnimeChangePose, LW(3), 1, LW(2))
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AnimeWaitPlayComplete, LW(3), 1)
    END_LOOP
    WAIT_MS(1000)
    LOOP(0)
        READ_INT(LW(2))
        IF_INT_EQUAL(LW(2), 0)
            BREAK_LOOP
        END_IF
        CALL(btlevtcmd_AnimeChangePose, LW(3), 1, LW(2))
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AnimeWaitPlayComplete, LW(3), 1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(0)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 6, LW(0))
    IF_INT_EQUAL(LW(0), -1)
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        IF_INT_EQUAL(LW(0), -1)
            CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
            IF_INT_EQUAL(LW(0), -1)
                CALL(btlevtcmd_GetUnitWork, -2, 3, LW(0))
                IF_INT_EQUAL(LW(0), -1)
                    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(1))
                    IF_INT_NOT_EQUAL(LW(1), 0)
                        SUBTRACT(LW(1), 1)
                        CALL(btlevtcmd_SetUnitWork, -2, 1, LW(1))
                        IF_INT_EQUAL(LW(1), 0)
                            CALL(btlevtcmd_SetUnitWork, -2, 1, 2)
                            CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
                            SWITCH(LW(0))
                                CASE_EQUAL(0)
                                CALL(btlevtcmd_GetMaxHp, -2, LW(0))
                                MULTIPLY(LW(0), 50)
                                DIVIDE(LW(0), 100)
                                CALL(btlevtcmd_GetHp, -2, LW(1))
                                IF_INT_LESS_OR_EQUAL(LW(1), LW(0))
                                    CALL(btlevtcmd_SetUnitWork, -2, 2, 1)
                                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1_2"))
                                    WAIT_MS(1000)
                                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1_1"))
                                END_IF
                            END_SWITCH
                            CALL(btlevtcmd_GetUnitWork, -2, 2, LW(1))
                            IF_INT_NOT_EQUAL(LW(1), 1)
                                CALL_CHILD(moamoa_tentacl_on)
                            ELSE
                                CALL_CHILD(moamoa_mouth_on)
                            END_IF
                            CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
                            CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 0, 0)
                        END_IF
                    END_IF
                END_IF
            END_IF
        END_IF
    END_IF
    LABEL(0)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static MoamoaAnimData ext_moamoa_data[89] = {
    { "c_n_moa_a", "S_1", 0.0f },
    { "c_n_moa_a", "S_1", 2.0f },
    { "c_n_moa_a", "S_1", 4.0f },
    { "c_n_moa_a", "S_1", 6.0f },
    { "c_n_moa_a", "S_1", 8.0f },
    { "c_n_moa_a", "S_1", 10.0f },
    { "c_n_moa_a", "S_1", 12.0f },
    { "c_n_moa_a", "S_1", 14.0f },
    { "c_n_moa_a", "S_1", 16.0f },
    { "c_n_moa_a", "S_1", 18.0f },
    { "c_n_moa_a", "S_1", 20.0f },
    { "c_n_moa_a", "S_1", 22.0f },
    { "c_n_moa_a", "S_1", 24.0f },
    { "c_n_moa_a", "S_1", 26.0f },
    { "c_n_moa_a", "S_1", 28.0f },
    { "c_n_moa_a", "S_1", 30.0f },
    { "c_n_moa_a", "S_1", 32.0f },
    { "c_n_moa_a", "S_1", 34.0f },
    { "c_n_moa_a", "S_1", 36.0f },
    { "c_n_moa_a", "S_1", 38.0f },
    { "c_n_moa_a", "D_1", 0.0f },
    { "c_n_moa_a", "D_1", 2.0f },
    { "c_n_moa_a", "D_1", 4.0f },
    { "c_n_moa_a", "D_1", 6.0f },
    { "c_n_moa_a", "D_1", 8.0f },
    { "c_n_moa_a", "D_1", 10.0f },
    { "c_n_moa_a", "S_2", 0.0f },
    { "c_n_moa_a", "S_2", 2.0f },
    { "c_n_moa_a", "S_2", 4.0f },
    { "c_n_moa_a", "S_2", 6.0f },
    { "c_n_moa_a", "S_2", 8.0f },
    { "c_n_moa_a", "S_2", 10.0f },
    { "c_n_moa_a", "S_2", 12.0f },
    { "c_n_moa_a", "S_2", 14.0f },
    { "c_n_moa_a", "S_2", 16.0f },
    { "c_n_moa_a", "S_2", 18.0f },
    { "c_n_moa_a", "S_2", 20.0f },
    { "c_n_moa_a", "S_2", 22.0f },
    { "c_n_moa_a", "S_2", 24.0f },
    { "c_n_moa_a", "S_2", 26.0f },
    { "c_n_moa_a", "S_2", 28.0f },
    { "c_n_moa_a", "S_2", 30.0f },
    { "c_n_moa_a", "S_2", 32.0f },
    { "c_n_moa_a", "S_2", 34.0f },
    { "c_n_moa_a", "S_2", 36.0f },
    { "c_n_moa_a", "S_2", 38.0f },
    { "c_n_moa_b", "S_1", 0.0f },
    { "c_n_moa_b", "S_1", 2.0f },
    { "c_n_moa_b", "S_1", 4.0f },
    { "c_n_moa_b", "S_1", 6.0f },
    { "c_n_moa_b", "S_1", 8.0f },
    { "c_n_moa_b", "S_1", 10.0f },
    { "c_n_moa_b", "S_1", 12.0f },
    { "c_n_moa_b", "S_1", 14.0f },
    { "c_n_moa_b", "S_1", 16.0f },
    { "c_n_moa_b", "S_1", 18.0f },
    { "c_n_moa_b", "S_1", 20.0f },
    { "c_n_moa_b", "S_1", 22.0f },
    { "c_n_moa_b", "S_1", 24.0f },
    { "c_n_moa_b", "S_1", 26.0f },
    { "c_n_moa_b", "S_1", 28.0f },
    { "c_n_moa_b", "S_1", 30.0f },
    { "c_n_moa_b", "S_1", 32.0f },
    { "c_n_moa_b", "S_1", 34.0f },
    { "c_n_moa_b", "S_1", 36.0f },
    { "c_n_moa_b", "S_1", 38.0f },
    { "c_n_moa_b", "D_1", 0.0f },
    { "c_n_moa_b", "D_1", 2.0f },
    { "c_n_moa_b", "D_1", 4.0f },
    { "c_n_moa_b", "D_1", 6.0f },
    { "c_n_moa_b", "D_1", 8.0f },
    { "c_n_moa_b", "D_1", 10.0f },
    { "c_kinodebu", "G_1", 3.0f },
    { "c_kinodebu", "G_1", 11.0f },
    { "c_puku_c", "U_1", 1.0f },
    { "c_puku_c", "U_1", 6.0f },
    { "c_powan", "R_2", 3.0f },
    { "c_powan", "R_2", 6.0f },
    { "c_smoko", "G_1", 2.0f },
    { "c_smoko", "G_1", 6.0f },
    { "c_rose", "G_1", 4.0f },
    { "c_rose", "G_1", 8.0f },
    { "c_papab", "G_1", 1.0f },
    { "c_papab", "G_1", 5.0f },
    { "c_mamab", "G_1", 1.0f },
    { "c_mamab", "G_1", 5.0f },
    { "c_konari", "G_1", 1.0f },
    { "c_konari", "G_1", 5.0f },
    { 0, 0, 0.0f },
};

// NONMATCHING (99.6%): no instruction differences; MWCC swaps the callee-saved registers for
// `pose` and `extPose` in the second loop (ours extPose=r29,pose=r30 vs orig extPose=r30,pose=r29).
USER_FUNC(moamoa_bomb) {


    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, wp->mainUnitId);
    ExtPose* extPose;
    s32 j;
    s32 count;
    s32 i;
    MoamoaWorkPose* pose;

    *(Vec*)wp->field_0x7A8 = (Vec){ 0.0f, 0.0f, 0.0f };

    if (isFirstCall) {
        extGetPosePtr();
        pose = wp->poses;
        for (i = 0; i < wp->countA; i++, pose++) {
            if (*(s32*)pose->field_0x30 != -1) {
                pose->field_0x0 &= ~1;
            }
        }
    }

    if (!(gp->retraceCount & 1)) {
        extPose = extGetPosePtr();
        count = 0;
        pose = wp->poses;
        for (j = 0; j < wp->countA; j++, pose++, extPose++) {
            if ((*extPose->matrix)[1][3] >= 0.0f) {
                if (!(pose->field_0x0 & 1)) {
                    pose->field_0x0 |= 1;
                    if (pose->field_0x0 & 1) {
                        pose->field_0x34[0] = randf(0.0f, 60.0f) - 30.0f;
                        pose->field_0x34[1] = randf(0.0f, 35.0f);
                        pose->field_0x34[2] = randf(0.0f, 30.0f) + 60.0f;
                        pose->field_0x40[0] = (*extPose->matrix)[0][3];
                        pose->field_0x40[1] = (*extPose->matrix)[1][3];
                        pose->field_0x40[2] = (*extPose->matrix)[2][3];
                        count++;
                    }
                }
            }
            if (count >= 10) {
                break;
            }
        }
    }

    extPose = extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->countA; i++, pose++, extPose++) {
        if (pose->field_0x0 & 1) {
            if (pose->field_0x40[1] >= -500.0f) {
                pose->field_0x40[0] += pose->field_0x34[0];
                pose->field_0x40[1] += pose->field_0x34[1];
                pose->field_0x40[2] += pose->field_0x34[2];
                pose->field_0x34[1] -= 1.0f;
            }
            (*extPose->matrix)[0][3] = pose->field_0x40[0];
            (*extPose->matrix)[1][3] = pose->field_0x40[1];
            (*extPose->matrix)[2][3] = pose->field_0x40[2];
        }
    }

    extPose = extGetPosePtr();
    count = 0;
    for (i = 0; i < wp->countA; i++, extPose++) {
        if ((*extPose->matrix)[1][3] >= -200.0f) {
            count++;
        }
    }
    if (count <= 9) {
        *(Vec*)wp->field_0x7A8 = (Vec){ 0.0f, -1000.0f, 0.0f };
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(moamoa1_on) {
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, wp->mainUnitId);
    MoamoaWorkPose* pose;
    s32 i;
    s32 j;

    for (i = 1; i < 4; i++) {
        BattleUnitPart* part;
        AnimationPose* anim;
        Vec* positions;

        switch (i) {
        case 1:
            part = BattleGetUnitPartsPtr(unit->work[3], 1);
            break;
        case 2:
            part = BattleGetUnitPartsPtr(unit->work[4], 1);
            break;
        case 3:
            part = BattleGetUnitPartsPtr(unit->work[5], 1);
            break;
        }
        wp->field_0x790[i] = (void*)*(s32*)part->field_0x1C0;
        animPoseWorldPositionEvalOn((s32)wp->field_0x790[i]);
        anim = animPoseGetAnimPosePtr((s32)wp->field_0x790[i]);
        positions = (Vec*)((smartEntry*)*(void**)((u8*)anim + 0x15C))->address;
        for (j = 0; j < *(s32*)((u8*)anim + 0x160); j++) {
            *positions++ = (Vec){ 0.0f, -1000.0f, 0.0f };
        }
    }

    for (i = 0, pose = wp->poses; i < wp->countA; i++, pose++) {
        if (pose->groupIdx < 5) {
            pose->animPose = wp->field_0x790[3];
            pose->field_0x30 = &unit->work[5];
        } else if (pose->groupIdx < 0xA) {
            pose->animPose = wp->field_0x790[2];
            pose->field_0x30 = &unit->work[4];
        } else if (pose->groupIdx < 0xF) {
            pose->animPose = wp->field_0x790[1];
            pose->field_0x30 = &unit->work[3];
        } else if (pose->groupIdx < 0x1E) {
            pose->animPose = wp->field_0x790[0];
            pose->field_0x30 = &wp->mainUnitId;
        } else {
            pose->animPose = wp->field_0x790[0];
            pose->field_0x30 = &wp->mainUnitId;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(moamoa2_on) {
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, wp->mainUnitId);
    BattleUnitPart* part = BattleGetUnitPartsPtr(unit->work[6], 1);
    AnimationPose* pose;
    Vec* positions;
    s32 i;
    MoamoaWorkPose* wp2;
    s32 j;

    wp->field_0x790[4] = (void*)*(s32*)part->field_0x1C0;
    animPoseWorldPositionEvalOn((s32)wp->field_0x790[4]);
    pose = animPoseGetAnimPosePtr((s32)wp->field_0x790[4]);
    positions = (Vec*)((smartEntry*)*(void**)((u8*)pose + 0x15C))->address;
    for (j = 0; j < *(s32*)((u8*)pose + 0x160); j++) {
        *positions++ = (Vec){ 0.0f, -1000.0f, 0.0f };
    }
    for (i = 0, wp2 = wp->poses; i < wp->countA; i++, wp2++) {
        if (wp2->groupIdx < 5) {
            wp2->animPose = wp->field_0x790[0];
            wp2->field_0x30 = &wp->mainUnitId;
        } else if (wp2->groupIdx < 0xA) {
            wp2->animPose = wp->field_0x790[0];
            wp2->field_0x30 = &wp->mainUnitId;
        } else if (wp2->groupIdx < 0xF) {
            wp2->animPose = wp->field_0x790[0];
            wp2->field_0x30 = &wp->mainUnitId;
        } else if (wp2->groupIdx < 0x1E) {
            wp2->animPose = wp->field_0x790[4];
            wp2->field_0x30 = &unit->work[6];
        } else {
            wp2->animPose = wp->field_0x790[0];
            wp2->field_0x30 = &wp->mainUnitId;
        }
    }
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(evt_moamoa)
    CALL(evt_npc_entry, STRING("moa_a"), STRING("c_n_moa_a"))
    CALL(evt_npc_entry, STRING("moa_b"), STRING("c_n_moa_b"))
    CALL(evt_npc_set_position, STRING("moa_a"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("moa_b"), 0, -1000, 0)
    CALL(evt_npc_entry, STRING("kinopio"), STRING("c_kinodebu"))
    CALL(evt_npc_entry, STRING("cook"), STRING("c_puku_c"))
    CALL(evt_npc_entry, STRING("powan"), STRING("c_powan"))
    CALL(evt_npc_entry, STRING("sarary"), STRING("c_smoko"))
    CALL(evt_npc_entry, STRING("rose"), STRING("c_rose"))
    CALL(evt_npc_entry, STRING("papa"), STRING("c_papab"))
    CALL(evt_npc_entry, STRING("mama"), STRING("c_mamab"))
    CALL(evt_npc_entry, STRING("konari"), STRING("c_konari"))
    CALL(evt_npc_set_position, STRING("kinopio"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("cook"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("powan"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("sarary"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("rose"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("papa"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("mama"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("konari"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("kinopio"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("cook"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("powan"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("sarary"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("rose"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("papa"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("mama"), 2048)
    CALL(evt_npc_flag_onoff, 1, STRING("konari"), 2048)
    CALL(moamoa_on)
    CALL(evt_ext_entry, 750, PTR(ext_moamoa_data), PTR(ext_moamoa_init), PTR(ext_moamoa_main), 0)
    RETURN
EVT_END()

static u32 group_tbl[22] = {
    (u32)"c_n_moa_a", (u32)"S_1", (u32)"c_n_moa_b", (u32)"S_1",
    (u32)"c_kinodebu", (u32)"G_1", (u32)"c_puku_c", (u32)"U_1",
    (u32)"c_powan", (u32)"R_2", (u32)"c_smoko", (u32)"G_1",
    (u32)"c_rose", (u32)"G_1", (u32)"c_papab", (u32)"G_1",
    (u32)"c_mamab", (u32)"G_1", (u32)"c_konari", (u32)"G_1",
    (u32)"c_n_moa_a", (u32)"S_2",
};

// NONMATCHING (99.89%): body exact apart from one strength-reduction register choice (the group-
// count loop keeps its counter in r30 in the original, r29 here).
USER_FUNC(moamoa_on) {
    s32 i;
    MoamoaWorkPose* pose;
    AnimationPose* animPose;
    Vec* positions;
    BattleUnitPart* part;
    MoamoaAnimData* data;
    s32 j;
    s32 lastAnim;
    char name[32];
    char animName[32];

    wp = __memAlloc(4, 0x7C4);
    memset(wp, 0, 0x7C4);
    wp->countA = 750;
    wp->poses = __memAlloc(4, wp->countA * 0x50);
    memset(wp->poses, 0, wp->countA * 0x50);

    pose = wp->poses;
    for (i = 0; i < wp->countA; i++, pose++) {
        Vec pos;

        pose->field_0x0 = 0;
        pos = (Vec){ 0.0f, 0.0f, 0.0f };
        pos.x = randf(-10.0f, 20.0f);
        pos.y = randf(-10.0f, 20.0f);
        pos.z = randf(-10.0f, 20.0f);
        *(Vec*)&pose->field_0x4 = pos;
    }

    wp->mainUnitId = BattleTransID(event, -2);
    BattleGetUnitPtr(_battleWorkPointer, wp->mainUnitId);
    part = BattleGetUnitPartsPtr(wp->mainUnitId, 1);
    wp->field_0x790[0] = (void*)*(s32*)part->field_0x1C0;
    animPoseWorldPositionEvalOn((s32)wp->field_0x790[0]);
    animPose = animPoseGetAnimPosePtr((s32)wp->field_0x790[0]);
    positions = (Vec*)((smartEntry*)*(void**)((u8*)animPose + 0x15C))->address;
    for (i = 0; i < *(s32*)((u8*)animPose + 0x160); i++) {
        *positions++ = (Vec){ 0.0f, -1000.0f, 0.0f };
    }

    for (i = 0; i < dat_80419a28; i++) {
        wp->groupCounts[i * 2] = animPoseGetGroupIdx((s32)wp->field_0x790[0], g_moamoa_chain_tbl[i].name);
        wp->groupCounts[i * 2 + 1] = animPoseGetGroupIdx((s32)wp->field_0x790[0], g_moamoa_chain_tbl[i].anim);
    }

    pose = wp->poses;
    for (i = 0; i < wp->countA; i++, pose++) {
        pose->groupIdx = i % dat_80419a28;
        pose->seq = wp->seqCounts[pose->groupIdx]++;
    }

    moamoa1_on(event, isFirstCall);

    pose = wp->poses;
    for (i = 0; i < wp->countA; i++, pose++) {
        pose->frame = (f32)pose->seq / (f32)wp->seqCounts[pose->groupIdx];
    }

    pose = wp->poses;
    for (i = 0; i < wp->countA; i++, pose++) {
        if (pose->groupIdx == 0 || pose->groupIdx == 5 || pose->groupIdx == 0xA ||
            pose->groupIdx == 0xF || pose->groupIdx == 0x2E) {
            pose->type = 1;
        }
        if (pose->seq == 0) {
            if (pose->groupIdx == 3) {
                pose->type = 2;
            }
            if (pose->groupIdx == 4) {
                pose->type = 3;
            }
            if (pose->groupIdx == 7) {
                pose->type = 4;
            }
            if (pose->groupIdx == 8) {
                pose->type = 5;
            }
            if (pose->groupIdx == 9) {
                pose->type = 6;
            }
            if (pose->groupIdx == 0xC) {
                pose->type = 7;
            }
            if (pose->groupIdx == 0xD) {
                pose->type = 8;
            }
            if (pose->groupIdx == 0xE) {
                pose->type = 9;
            }
        }
    }

    data = (MoamoaAnimData*)ext_moamoa_data;
    i = 0;
    strcpy(name, "");
    strcpy(animName, "");
    wp->countB = -1;
    do {
        if (strcmp(data->name1, name) != 0) {
            strcpy(name, data->name1);
            strcpy(animName, "");
        }
        if (strcmp(data->name2, animName) != 0) {
            strcpy(animName, data->name2);
            lastAnim = i;
            wp->countB++;
        }
        strcpy(wp->anims[wp->countB].field_0x0, name);
        strcpy(wp->anims[wp->countB].field_0x10, animName);
        wp->anims[wp->countB].field_0x1C = lastAnim;
        wp->anims[wp->countB].field_0x20 = i - lastAnim + 1;
        if (pose->type >= 2 && pose->type != 0xA) {
            wp->anims[wp->countB].field_0x18 = 12.0f;
        } else {
            wp->anims[wp->countB].field_0x18 = data->frame;
        }
        i++;
        data++;
    } while (data->name1 != NULL);
    wp->countB++;
    for (i = 0; i < wp->countB; i++) {
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(MOAMOA_SET_BODYSCALE)
    CALL(moamoa_set_bodyScale)
    RETURN
EVT_END()

void moamoa_pose(void* poseArg) {
    MoamoaPoseArg* pose = poseArg;
    MoamoaWorkAnim* anim;
    s32 i;
    const char* sub;
    const char* name;

    i = 0;
    anim = wp->anims;
    name = (const char*)group_tbl[pose->type * 2];
    sub = (const char*)group_tbl[pose->type * 2 + 1];
    for (; i < wp->countB; i++, anim++) {
        if (strcmp(anim->field_0x0, name) == 0 && strcmp(anim->field_0x10, sub) == 0) {
            break;
        }
    }
    if (i < wp->countB && pose->field_0x10 != i) {
        pose->field_0x10 = i;
        pose->field_0x18 = gp->renderTime;
    }
}

static void ext_moamoa_init(void) {
    s32 i;
    ExtPose* extPose = extGetPosePtr();
    MoamoaWorkPose* pose = wp->poses;
    const char* name;
    const char* sub;
    s32 j;
    MoamoaWorkAnim* anim;

    wp->field_0x7BC = 1.0f;
    wp->field_0x7C0 = 1.0f;
    for (i = 0; i < wp->countA; i++, extPose++, pose++) {
        extPose->flags |= 5;
        PSMTXIdentity(*extPose->matrix);
        name = (const char*)group_tbl[pose->type * 2];
        sub = (const char*)group_tbl[pose->type * 2 + 1];
        anim = wp->anims;
        for (j = 0; j < wp->countB; j++, anim++) {
            if (strcmp(anim->field_0x0, name) == 0 && strcmp(anim->field_0x10, sub) == 0) {
                break;
            }
        }
        if (j < wp->countB && pose->field_0x10 != j) {
            pose->field_0x10 = j;
            pose->field_0x18 = gp->renderTime;
        }
    }
}

// NONMATCHING (95.83%): body instruction-perfect except register allocation; the part loop keeps
// its counter/offsets in r22/r23/r21 in the original but r25/r22/r21 here, and the animation loop
// cycles pose/extPose/i by one register. `.text` size already matches (0x1bb4); 166 `.text` bytes
// and a few `.rodata` pool references still differ.
static void ext_moamoa_main(void) {
    MoamoaWorkPose* pose;
    ExtPose* extPose;
    MoamoaWorkAnim* anim;
    s32 i;
    AnimationPose* animPose;
    Vec* vtx;
    BattleUnit* unit;
    Vec tmp2[5];
    Vec tmp1[5];
    Vec tmp;
    Vec offset;
    Vec offsetR;
    Mtx mtx;
    s32 j;
    s32 p0;
    s32 p1;
    s32 frame;
    f32 f;

    extGetPosePtr();
    for (i = 0; i < wp->countA; i++) {
    }
    wp->field_0x7C0 += 0.1f * (wp->field_0x7BC - wp->field_0x7C0);

    extPose = extGetPosePtr();
    pose = wp->poses;
    for (i = 0; i < wp->countA; i++, extPose++, pose++) {
        anim = &wp->anims[pose->field_0x10];
        f = (f32)gp->framerate * (f32)OSTicksToMilliseconds((u64)(gp->renderTime - pose->field_0x18)) / 1000.0f / anim->field_0x18;
        if (f >= 1.0f) {
            pose->field_0x18 = gp->renderTime;
            f = 0.0f;
        }
        frame = (s32)((f32)anim->field_0x20 * f);
        if (frame >= anim->field_0x20) {
            frame = anim->field_0x20 - 1;
        }
        extPose->field_0x10 = anim->field_0x1C + frame;
    }

    if (wp->mainUnitId != -1) {
        unit = BattleGetUnitPtr(_battleWorkPointer, wp->mainUnitId);
        for (i = 0; i < 5; i++) {
            if (i != 0 && unit->work[i + 2] == -1) {
                continue;
            }
            animPose = animPoseGetAnimPosePtr((s32)wp->field_0x790[i]);
            j = animPoseGetGroupIdx((s32)wp->field_0x790[i], "joint_core");
            vtx = (Vec*)((smartEntry*)*(void**)((u8*)animPose + 0x15C))->address;
            tmp2[i] = vtx[j];
            PSVECSubtract(&tmp2[0], &tmp2[i], &tmp1[i]);
        }

        extPose = extGetPosePtr();
        pose = wp->poses;
        for (i = 0; i < wp->countA; i++, pose++, extPose++) {
            if (*(s32*)pose->field_0x30 == -1) {
                (*extPose->matrix)[0][3] = 0.0f;
                (*extPose->matrix)[1][3] = -1000.0f;
                (*extPose->matrix)[2][3] = 0.0f;
            } else {
                f32 t;
                f32 inv;

                animPose = animPoseGetAnimPosePtr((s32)pose->animPose);
                vtx = (Vec*)((smartEntry*)*(void**)((u8*)animPose + 0x15C))->address;
                p0 = wp->groupCounts[pose->groupIdx * 2];
                p1 = wp->groupCounts[pose->groupIdx * 2 + 1];
                t = pose->frame;
                inv = 1.0f - t;
                if (vtx[p0].y == -1000.0f || vtx[p1].y == -1000.0f) {
                    offset = (Vec){ 0.0f, -1000.0f, 0.0f };
                    extPose->flags &= ~1;
                } else {
                    PSVECScale(&vtx[p0], &offset, t);
                    PSVECScale(&vtx[p1], &tmp, inv);
                    PSVECAdd(&offset, &tmp, &offset);
                    PSVECAdd(&offset, (Vec*)&pose->field_0x4, &offset);
                    extPose->flags |= 1;
                }
                if (pose->field_0x30 != &wp->mainUnitId) {
                    for (j = 1; j < 5; j++) {
                        if (&unit->work[j + 2] == (s32*)pose->field_0x30) {
                            PSVECAdd(&offset, &tmp1[j], &offset);
                            break;
                        }
                    }
                }
                if (pose->groupIdx >= 0x2E) {
                    f = wp->field_0x7C0;
                    (*extPose->matrix)[1][1] = f;
                    (*extPose->matrix)[0][0] = f;
                }
                (*extPose->matrix)[0][3] += 0.5f * (offset.x - (*extPose->matrix)[0][3]);
                (*extPose->matrix)[1][3] += 0.5f * (offset.y - (*extPose->matrix)[1][3]);
                (*extPose->matrix)[2][3] += 0.5f * (offset.z - (*extPose->matrix)[2][3]);
            }
        }
    } else {
        wp->field_0x7B4 += 8.571428f;
        wp->field_0x7B8 -= 0.0044444f;
        if (wp->field_0x7B8 < 0.8f) {
            wp->field_0x7B8 = 0.8f;
        }
        if (wp->field_0x7B4 >= 360.0f) {
            wp->field_0x7B4 -= 360.0f;
        }
        extPose = extGetPosePtr();
        pose = wp->poses;
        for (i = 0; i < wp->countA; i++, pose++, extPose++) {
            if (pose->field_0x0 & 1) {
                PSVECSubtract((Vec*)pose->field_0x34, (Vec*)wp->field_0x7A8, &offsetR);
                PSMTXRotAxisRad(mtx, (Vec*)pose->field_0x40, 0.017453292f * wp->field_0x7B4);
                PSMTXMultVec(mtx, &offsetR, &offsetR);
                PSVECScale(&offsetR, &offsetR, wp->field_0x7B8);
                PSVECAdd(&offsetR, (Vec*)wp->field_0x7A8, &offsetR);
                (*extPose->matrix)[0][3] += 0.5f * (offsetR.x - (*extPose->matrix)[0][3]);
                (*extPose->matrix)[1][3] += 0.5f * (offsetR.y - (*extPose->matrix)[1][3]);
                (*extPose->matrix)[2][3] += 0.5f * (offsetR.z - (*extPose->matrix)[2][3]);
            }
        }
    }
    dispEntry(4, 0.0f, 1, (DispCallback)moamoa_draw, NULL);
}

void moamoa_draw(void) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

USER_FUNC(moamoa_set_bodyScale) {
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, -2));
    s32 num = 0;

    if (unit == NULL) {
        extReset();
        return EVT_RETURN_DONE;
    }

    if (unit->work[6] == -1) {
        num = 3;
    }
    if (unit->work[5] == -1) {
        num++;
    }
    if (unit->work[4] == -1) {
        num++;
    }
    if (unit->work[3] == -1) {
        num++;
    }
    wp->field_0x7BC = 0.333f * (num - 3) + 1.0f;
    return EVT_RETURN_BLOCK;
}
// clang-format on
