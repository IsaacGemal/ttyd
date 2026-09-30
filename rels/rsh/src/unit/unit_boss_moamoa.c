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

// TODO: write moamoa_bomb (global, 0x3e0 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/unit/unit_boss_moamoa.s; this is where the original has it, just before the first script that uses it

// TODO: write moamoa_guruguru (global, 0x25c bytes) here:
// its asm is in build/G8MJ01/rsh/asm/unit/unit_boss_moamoa.s; this is where the original has it, just before the first script that uses it

// TODO: write evt_moamoa_mainUnitIdClear (global, 0x1c bytes) here:
USER_FUNC(evt_moamoa_mainUnitIdClear) {
    wp->mainUnitId = -1;
    return EVT_RETURN_DONE;
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

// TODO: write moamoa1_on (global, 0x200 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/unit/unit_boss_moamoa.s; this is where the original has it, just before the first script that uses it

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

// TODO: write moamoa2_on (global, 0x1ac bytes) here:
// NONMATCHING (78.48%): WIP reconstruction; see REPORT.md.
USER_FUNC(moamoa2_on) {
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, wp->mainUnitId);
    BattleUnitPart* part = BattleGetUnitPartsPtr(unit->work[6], 1);
    const Vec vec = {0.0f, -1000.0f, 0.0f};
    AnimationPose* pose;
    Vec* vtx;
    MoamoaWorkPose* wp2;
    s32 count;
    s32 i;
    s32 j;

    wp->field_0x790[4] = (void*)*(s32*)((u8*)part + 0x1C0);
    animPoseWorldPositionEvalOn((s32)wp->field_0x790[4]);
    pose = animPoseGetAnimPosePtr((s32)wp->field_0x790[4]);
    vtx = *(Vec**)((u8*)pose + 0x15C);
    count = *(s32*)((u8*)pose + 0x160);
    for (j = 0; j < count; j++) {
        vtx[j] = vec;
    }
    wp2 = wp->poses;
    for (i = 0; i < wp->countA; i++, wp2++) {
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

// TODO: type
static u32 ext_moamoa_data[267] = {
    (u32)"c_n_moa_a", (u32)"S_1", 0x00000000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x40000000, (u32)"c_n_moa_a", (u32)"S_1",
    0x40800000, (u32)"c_n_moa_a", (u32)"S_1", 0x40C00000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41000000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41200000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41400000, (u32)"c_n_moa_a", (u32)"S_1", 0x41600000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41800000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41900000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41A00000, (u32)"c_n_moa_a", (u32)"S_1", 0x41B00000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x41C00000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x41D00000, (u32)"c_n_moa_a", (u32)"S_1",
    0x41E00000, (u32)"c_n_moa_a", (u32)"S_1", 0x41F00000,
    (u32)"c_n_moa_a", (u32)"S_1", 0x42000000, (u32)"c_n_moa_a",
    (u32)"S_1", 0x42080000, (u32)"c_n_moa_a", (u32)"S_1",
    0x42100000, (u32)"c_n_moa_a", (u32)"S_1", 0x42180000,
    (u32)"c_n_moa_a", (u32)"D_1", 0x00000000, (u32)"c_n_moa_a",
    (u32)"D_1", 0x40000000, (u32)"c_n_moa_a", (u32)"D_1",
    0x40800000, (u32)"c_n_moa_a", (u32)"D_1", 0x40C00000,
    (u32)"c_n_moa_a", (u32)"D_1", 0x41000000, (u32)"c_n_moa_a",
    (u32)"D_1", 0x41200000, (u32)"c_n_moa_a", (u32)"S_2",
    0x00000000, (u32)"c_n_moa_a", (u32)"S_2", 0x40000000,
    (u32)"c_n_moa_a", (u32)"S_2", 0x40800000, (u32)"c_n_moa_a",
    (u32)"S_2", 0x40C00000, (u32)"c_n_moa_a", (u32)"S_2",
    0x41000000, (u32)"c_n_moa_a", (u32)"S_2", 0x41200000,
    (u32)"c_n_moa_a", (u32)"S_2", 0x41400000, (u32)"c_n_moa_a",
    (u32)"S_2", 0x41600000, (u32)"c_n_moa_a", (u32)"S_2",
    0x41800000, (u32)"c_n_moa_a", (u32)"S_2", 0x41900000,
    (u32)"c_n_moa_a", (u32)"S_2", 0x41A00000, (u32)"c_n_moa_a",
    (u32)"S_2", 0x41B00000, (u32)"c_n_moa_a", (u32)"S_2",
    0x41C00000, (u32)"c_n_moa_a", (u32)"S_2", 0x41D00000,
    (u32)"c_n_moa_a", (u32)"S_2", 0x41E00000, (u32)"c_n_moa_a",
    (u32)"S_2", 0x41F00000, (u32)"c_n_moa_a", (u32)"S_2",
    0x42000000, (u32)"c_n_moa_a", (u32)"S_2", 0x42080000,
    (u32)"c_n_moa_a", (u32)"S_2", 0x42100000, (u32)"c_n_moa_a",
    (u32)"S_2", 0x42180000, (u32)"c_n_moa_b", (u32)"S_1",
    0x00000000, (u32)"c_n_moa_b", (u32)"S_1", 0x40000000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x40800000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x40C00000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41000000, (u32)"c_n_moa_b", (u32)"S_1", 0x41200000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41400000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41600000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41800000, (u32)"c_n_moa_b", (u32)"S_1", 0x41900000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41A00000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41B00000, (u32)"c_n_moa_b", (u32)"S_1",
    0x41C00000, (u32)"c_n_moa_b", (u32)"S_1", 0x41D00000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x41E00000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x41F00000, (u32)"c_n_moa_b", (u32)"S_1",
    0x42000000, (u32)"c_n_moa_b", (u32)"S_1", 0x42080000,
    (u32)"c_n_moa_b", (u32)"S_1", 0x42100000, (u32)"c_n_moa_b",
    (u32)"S_1", 0x42180000, (u32)"c_n_moa_b", (u32)"D_1",
    0x00000000, (u32)"c_n_moa_b", (u32)"D_1", 0x40000000,
    (u32)"c_n_moa_b", (u32)"D_1", 0x40800000, (u32)"c_n_moa_b",
    (u32)"D_1", 0x40C00000, (u32)"c_n_moa_b", (u32)"D_1",
    0x41000000, (u32)"c_n_moa_b", (u32)"D_1", 0x41200000,
    (u32)"c_kinodebu", (u32)"G_1", 0x40400000, (u32)"c_kinodebu",
    (u32)"G_1", 0x41300000, (u32)"c_puku_c", (u32)"U_1",
    0x3F800000, (u32)"c_puku_c", (u32)"U_1", 0x40C00000,
    (u32)"c_powan", (u32)"R_2", 0x40400000, (u32)"c_powan",
    (u32)"R_2", 0x40C00000, (u32)"c_smoko", (u32)"G_1",
    0x40000000, (u32)"c_smoko", (u32)"G_1", 0x40C00000,
    (u32)"c_rose", (u32)"G_1", 0x40800000, (u32)"c_rose",
    (u32)"G_1", 0x41000000, (u32)"c_papab", (u32)"G_1",
    0x3F800000, (u32)"c_papab", (u32)"G_1", 0x40A00000,
    (u32)"c_mamab", (u32)"G_1", 0x3F800000, (u32)"c_mamab",
    (u32)"G_1", 0x40A00000, (u32)"c_konari", (u32)"G_1",
    0x3F800000, (u32)"c_konari", (u32)"G_1", 0x40A00000,
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: write ext_moamoa_main (static, 0x68c bytes) here:
// its asm is in build/G8MJ01/rsh/asm/unit/unit_boss_moamoa.s; this is where the original has it, just before the first script that uses it

// TODO: write ext_moamoa_init (static, 0x134 bytes) here:
// NONMATCHING (98.44%): body and control flow match; MWCC assigns the callee-saved registers in a
// different order than the original (ours: &wp=r30,i=r27,group_tbl=r31,j=r26,name=r25,sub=r24).
static void ext_moamoa_init(void) {
    ExtPose* extPose = extGetPosePtr();
    MoamoaWorkPose* pose = wp->poses;
    const char* name;
    const char* sub;
    MoamoaWorkAnim* anim;
    s32 i;
    s32 j;

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

// TODO: write moamoa_on (global, 0x6c4 bytes) here:
// its asm is in build/G8MJ01/rsh/asm/unit/unit_boss_moamoa.s; this is where the original has it, just before the first script that uses it

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

// TODO: write moamoa_set_bodyScale (global, 0xdc bytes) here:
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
    return 0;
}

EVT_BEGIN(MOAMOA_SET_BODYSCALE)
    CALL(moamoa_set_bodyScale)
    RETURN
EVT_END()

// TODO: write moamoa_draw (global, 0x34 bytes) here:
void moamoa_draw(void) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

// TODO: write moamoa_pose (global, 0xe0 bytes) here:
// NONMATCHING (90.89%): logic matches; MWCC assigns the callee-saved registers differently
// (ours: i=r27,name=r29,anim=r26 vs orig: i=r28,name=r26,anim=r29).
void moamoa_pose(void* poseArg) {
    MoamoaPoseArg* pose = poseArg;
    const char* name = (const char*)group_tbl[pose->type * 2];
    const char* sub = (const char*)group_tbl[pose->type * 2 + 1];
    s32 i = 0;
    MoamoaWorkAnim* anim = wp->anims;

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

// TODO: write moamoa_mainUnitIdClear (global, 0x18 bytes) here:
void moamoa_mainUnitIdClear(void) {
    wp->mainUnitId = -1;
}

// TODO: write randf (static, 0x7c bytes) here:
static f32 randf(f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;
    f32 d = max - min;
    return r * d + min;
}

// TODO: write boss_moamoa_check_zenei_kouei (global, 0xa8 bytes) here:
// NONMATCHING (97.5%): one extra `mr r0,r3` before passing evtGetValue()'s result to BattleTransID
// (ours `mr r0,r3; mr r4,r0`, orig `mr r4,r3`).
USER_FUNC(boss_moamoa_check_zenei_kouei) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);
    s32 index = args[1];
    s32 id = BattleTransID(event, value);
    BattleUnit* unit = BattleGetUnitPtr(work, id);
    f32 x, y, z;

    BtlUnit_GetPos(unit, &x, &y, &z);
    if (x >= -120.0f) {
        evtSetValue(event, index, 0);
    } else {
        evtSetValue(event, index, 1);
    }
    return EVT_RETURN_DONE;
}
// clang-format on
