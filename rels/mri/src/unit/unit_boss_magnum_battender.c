#include "unit_boss_magnum_battender.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_batten.h"
#include "eff/n64/eff_akari_charge_n64.h"
#include "eff/n64/eff_hokori_n64.h"
#include "manager/evtmgr_cmd.h"
#include "math_trig.h"
#include "memory.h"
#include <dolphin/mtx.h>

#pragma pool_data off

#pragma force_active on
static USER_FUNC(waku_main);
static USER_FUNC(arm_check);
static USER_FUNC(eff_dead);
static USER_FUNC(eff_dead_delete);
static USER_FUNC(arm_init);
static USER_FUNC(arm_init2);
static USER_FUNC(arm_main);
static USER_FUNC(arm_end);
static EffectEntry* eff_charge;
static MagnumBattenderWork* work[2];
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[2];
static BattleUnitKindPart parts[3];
static s32 init_event[92];
static s32 waku_event[23];
static s32 damage_event[111];
static s32 dead_event[234];
static s32 entry_event[78];
static s32 attack_event[150];
s32 stamp_attack_event[352];
static s32 shake_attack_event[324];
static s32 arm_bunri_event[199];
static s32 arm_return_event[151];
static s32 wait_event[6];
BattleUnitKind unit_boss_rocket_punch;
static BattleVulnerableStatus regist_r;
static BattleDataEntry data_table_r[1];
static BattleUnitKindPart parts_r[1];
static s32 init_event_r[191];
static s32 arm_event[59];
static s32 arm_event_end[35];
static s32 damage_event_r[30];
static s32 attack_event_r[368];
static s32 wait_event_r[18];

// clang-format off
extend_data(88,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(89,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boss_magnum_battender = {
    UNIT_MAGNUS,                // mUnitType
    "btl_un_magnum_battender",  // name
    30,                         // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    68,                         // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // escapeRate
    6,                          // renzokuSoftCap
    120,                        // width
    160,                        // height
    { 30, 100 },                // statusIconOffset
    { 0.0f, 80.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 30.0f, 115.0f, 0.0f },    // talkTogeBaseOffset
    { 60.0f, 0.0f, -10.0f },    // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // kagenukeOffsetX
    0.0f,                       // kagenukeOffsetZ
    { 52.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 30.0f, 110.0f, 20.0f },   // kissHitOffset
    { 0.0f, 80.0f, 0.0f },      // cutBaseOffset
    120.0f,                     // cutWidth
    160.0f,                     // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    -1,                         // swallowRate
    0,                          // swallowAttribute
    100,                        // shotDamageRate
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_MBTN_DAMAGED1",   // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0,                          // attributeFlags
    &regist,                    // vulnStatus
    3,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleUnitDefense defence = { { 1, 1, 1, 1, 1 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    0,   // sleepRate
    50,  // stopRate
    50,  // dizzyRate
    0,   // poisonRate
    50,  // confuseRate
    0,   // electricRate
    100, // burnRate
    30,  // freezeRate
    0,   // hugeRate
    0,   // tinyRate
    100, // attackUpRate
    0,   // attackDownRate
    100, // defenseUpRate
    90,  // defenseDownRate
    50,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    40,  // slowRate
    0,   // dodgyRate
    0,   // invisibleRate
    0,   // instaKillRate
};

static BattleDispPose pose_table[17] = {
    { 0x01, "MGN_S_1" },
    { 0x02, "MGN_2D_1" },
    { 0x09, "MGN_2D_1" },
    { 0x05, "MGN_X_1" },
    { 0x04, "MGN_X_1" },
    { 0x03, "MGN_X_1" },
    { 0x1C, "MGN_S_1" },
    { 0x1D, "MGN_Q_1" },
    { 0x1E, "MGN_Q_1" },
    { 0x1F, "MGN_S_1" },
    { 0x27, "MGN_D_1" },
    { 0x28, "MGN_W_1" },
    { 0x29, "MGN_W_2" },
    { 0x38, "MGN_S_1" },
    { 0x39, "MGN_S_1" },
    { 0x41, "MGN_T_1" },
    { 0x45, "MGN_S_1" },
};

static BattleDispPose pose_table_no_arm[16] = {
    { 0x01, "MGN_S_2" },
    { 0x02, "MGN_2D_2" },
    { 0x09, "MGN_2D_2" },
    { 0x05, "MGN_X_2" },
    { 0x04, "MGN_X_2" },
    { 0x03, "MGN_X_2" },
    { 0x1C, "MGN_S_2" },
    { 0x1D, "MGN_Q_2" },
    { 0x1E, "MGN_Q_2" },
    { 0x1F, "MGN_S_2" },
    { 0x27, "MGN_D_2" },
    { 0x28, "MGN_W_3" },
    { 0x38, "MGN_S_2" },
    { 0x39, "MGN_S_2" },
    { 0x41, "MGN_T_2" },
    { 0x45, "MGN_S_2" },
};

static BattleDataEntry data_table[2] = {
    { 0x31, dead_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[3] = {
    {
        1,                         // partNum
        "btl_un_magnum_battender", // partName
        "c_mb_robo_1b",            // modelName
        { 0.0f, 0.0f, 0.0f },      // positionOffset
        { 37.0f, 103.0f, 30.0f },  // hitBaseOffset
        { 37.0f, 113.0f, 61.0f },  // hitCursorBaseOffset
        106,                       // shadowSize
        174,                       // field_0x32
        255,                       // baseAlpha
        &defence,                  // defense
        &defence_attr,             // defenseAttr
        0x00600009,                // attributes
        0,                         // counterAttributes
        pose_table,                // poseTable
    },
    {
        2,                         // partNum
        "btl_un_magnum_battender", // partName
        "c_mb_robo_1b",            // modelName
        { 0.0f, 0.0f, 0.0f },      // positionOffset
        { 10.0f, 10.0f, 30.0f },   // hitBaseOffset
        { 10.0f, 20.0f, 61.0f },   // hitCursorBaseOffset
        106,                       // shadowSize
        174,                       // field_0x32
        255,                       // baseAlpha
        &defence,                  // defense
        &defence_attr,             // defenseAttr
        0x03100002,                // attributes
        0,                         // counterAttributes
        pose_table,                // poseTable
    },
    {
        3,                         // partNum
        "btl_un_magnum_battender", // partName
        "c_mb_robo_1b",            // modelName
        { 0.0f, 0.0f, 0.0f },      // positionOffset
        { 0.0f, 0.0f, 0.0f },      // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },      // hitCursorBaseOffset
        30,                        // shadowSize
        30,                        // field_0x32
        255,                       // baseAlpha
        &defence,                  // defense
        &defence_attr,             // defenseAttr
        0x12010000,                // attributes
        0,                         // counterAttributes
        pose_table,                // poseTable
    },
};

BattleWeapon weapon_batten_stamp_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    1,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20001000,                               // targetPropertyFlags
    0,                                        // element
    12,                                       // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x0000000C,                               // counterResistanceFlags
    0x80002000,                               // targetWeightingFlags
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

BattleWeapon weapon_batten_shake_attack = {
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
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000008,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x000010C7,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x00002000,                               // targetWeightingFlags
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

static BattleUnitSetup entry_rocket_punch[1] = {
    { &unit_boss_rocket_punch, 1, 0x04000004, { -1000.0f, -1000.0f, -1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

static USER_FUNC(waku_main) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    BattleUnitPart* part1;
    BattleUnitPart* part3;
    AnimationPose* pose;
    Vec* positions;
    s32 id;
    s32 poseId;
    s32 group;

    id = evtGetValue(event, *args++);
    animGetPtr();
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    part1 = BtlUnit_GetPartsPtr(unit, 1);
    part3 = BtlUnit_GetPartsPtr(unit, 3);
    poseId = part1->poseId;
    animPoseWorldPositionEvalOn(poseId);
    animPoseGetAnimBaseDataPtr(poseId);
    pose = animPoseGetAnimPosePtr(poseId);
    group = animPoseGetGroupIdx(poseId, "locator17");
    positions = pose->groupWorldPos->address;
    BtlUnit_SetPartsPos(part3, positions[group].x, positions[group].y, positions[group].z);
    {
        f32 yaw = camGetPtr(CAMERA_3D)->viewYaw;
        BtlUnit_SetPartsRotate(part3, 0.0f, -yaw, 0.0f);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(arm_check) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 value = evtGetValue(event, args[0]);
    s32 result = -1;
    s32 i;

    for (i = 0; i < 64; i++) {
        unit = wp->units[i];
        if (unit != NULL && (unit->currentKind == UNIT_X_FIST || unit->currentKind == UNIT_X_PUNCH)) {
            if (value == 0) {
                if (unit->work[0] == 0 || unit->work[0] == 2) {
                    result = i;
                    break;
                }
            }
            if (value == 1) {
                if (unit->work[0] == 1 || unit->work[0] == 3) {
                    result = i;
                    break;
                }
            }
        }
    }
    evtSetValue(event, args[1], result);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_OnUnitFlag, -2, 16777216)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(entry_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("MGN_L_1"))
    SPAWN_EVENT(waku_event)
    CALL(arm_init)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    CALL(btlevtcmd_SetHomePos, -2, 125, 0, 0)
    CALL(btlevtcmd_SetPos, -2, 125, 0, 0)
    CALL(btlevtcmd_SetBaseRotate, -2, 0, -55, 0)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_BOSS_MBTN_MOVE1"), STRING("SFX_BOSS_MBTN_MOVE1"), 26, 26, 26)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_BOSS_MBTN_MOVE1"), STRING("SFX_BOSS_MBTN_MOVE1"), 20, 20, 20)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(waku_event)
    LABEL(0)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    IF_INT_EQUAL(LW(0), -1)
        GOTO(99)
    END_IF
    CALL(waku_main, -2)
    GOTO(0)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_CheckDamagePattern, -2, 10, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnPartsAttribute, -2, 3, 16777216)
    END_IF
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
    CALL(btlevtcmd_GetUnitWork, -2, 6, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetHp, -2, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), 7)
            CALL(btlevtcmd_WaitAttackEnd)
            CALL(btlevtcmd_SetUnitWork, -2, 6, 1)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(evt_btl_camera_set_prilimit, 1)
            CALL(evt_btl_camera_set_mode, 1, 3)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            SUBTRACT(LW(0), 20)
            CALL(evt_btl_camera_set_moveto, 1, LW(0), 160, 600, LW(0), 110, -100, 20, 11)
            WAIT_FRAMES(20)
            CALL(btlevtcmd_SetTalkPoseType, -2, 65)
            CALL(btlevtcmd_SetStayPoseType, -2, 69)
            CALL(evt_msg_print, 2, STRING("stg2_mri_e24_15"), 0, -2)
            CALL(evt_btl_camera_set_prilimit, 0)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            WAIT_MS(500)
        END_IF
    END_IF
    RETURN
EVT_END()

static USER_FUNC(eff_dead) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);

    eff_charge = effAkariChargeN64Entry(0, 1000, x, y, z, 1.5f);
    return EVT_RETURN_DONE;
}

static USER_FUNC(eff_dead_delete) {
    effDelete(eff_charge);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_SetUnitWork, -2, 6, 1)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 70)
    ADD(LW(2), 10)
    CALL(eff_dead, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 20)
    CALL(evt_btl_camera_set_moveto, 1, LW(0), 160, 600, LW(0), 110, -100, 20, 11)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_SetTalkPoseType, -2, 39)
    CALL(btlevtcmd_SetStayPoseType, -2, 39)
    CALL(evt_msg_print, 2, STRING("stg2_mri_e24_16"), 0, -2)
    CALL(evt_btl_camera_set_prilimit, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    WAIT_MS(500)
    CALL(arm_check, 0, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        CALL(btlevtcmd_CheckDamageCode, LW(0), 512, LW(1))
        IF_INT_EQUAL(LW(1), 0)
            SET_VALUE(LW(10), LW(0))
            BEGIN_INLINE_EVENT
                CALL_CHILD(subsetevt_dead_core)
                CALL(arm_end, 0)
                CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 16777216)
                CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 33554432)
            END_INLINE_EVENT
        END_IF
    END_IF
    CALL(arm_check, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        CALL(btlevtcmd_CheckDamageCode, LW(0), 512, LW(1))
        IF_INT_EQUAL(LW(1), 0)
            SET_VALUE(LW(10), LW(0))
            BEGIN_INLINE_EVENT
                CALL_CHILD(subsetevt_dead_core)
                CALL(arm_end, 1)
                CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 16777216)
                CALL(btlevtcmd_OnPartsAttribute, LW(10), 1, 33554432)
            END_INLINE_EVENT
        END_IF
    END_IF
    SET_VALUE(LW(10), -2)
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetCoin, LW(10), LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, LW(10), LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL(btlevtcmd_GetPos, LW(10), LW(3), LW(1), LW(2))
    ADD(LW(2), 50)
    ADD(LW(1), 30)
    CALL(_add_star_point_disp_offset, LW(10), LW(3), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("star_point"), 0, LW(3), LW(1), LW(2), LW(0), 0, 0, 0, 0, 0, 0, 0)
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        LOOP(LW(0))
        END_LOOP
    END_IF
    WAIT_FRAMES(120)
    CALL(btlevtcmd_GetBackItem, LW(10))
    CALL(btlevtcmd_KillUnit, LW(10), 1)
    CALL(eff_dead_delete)
    RETURN
EVT_END()

static EVT_BEGIN(entry_event)
    WAIT_FRAMES(120)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SUBTRACT(LW(0), 20)
    CALL(evt_btl_camera_set_moveto, 1, LW(0), 160, 600, LW(0), 110, -100, 20, 11)
    WAIT_FRAMES(20)
    CALL(evt_msg_print, 2, STRING("stg2_mri_e24_09"), 0, -2)
    CALL(evt_btl_camera_set_moveto, 1, -102, 83, 334, -102, 59, 0, 20, 11)
    WAIT_FRAMES(20)
    CALL(evt_msg_print_battle_party, STRING("stg2_mri_e24_10"))
    CALL(evt_btl_camera_set_prilimit, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    WAIT_MS(500)
    CALL(btlevtcmd_GetBodyId, -4, LW(0))
    CALL(btlevtcmd_AnimeChangePoseFromTable, -4, LW(0))
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(arm_check, 0, LW(0))
        IF_INT_EQUAL(LW(0), -1)
            CALL(arm_check, 1, LW(0))
            IF_INT_EQUAL(LW(0), -1)
                CALL(btlevtcmd_AddUnitWork, -2, 1, -1)
                CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
                IF_INT_LESS_OR_EQUAL(LW(0), 0)
                    CALL_CHILD(arm_return_event)
                END_IF
            END_IF
        END_IF
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL_CHILD(stamp_attack_event)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_GetHp, -2, LW(0))
        IF_INT_GREATER(LW(0), 20)
            CALL_CHILD(stamp_attack_event)
            GOTO(99)
        ELSE
            CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL_CHILD(arm_bunri_event)
                GOTO(99)
            END_IF
            SET_VALUE(LW(0), 0)
            ADD(LW(0), 60)
            ADD(LW(0), 40)
            SUBTRACT(LW(0), 1)
            CALL(evt_sub_random, LW(0), LW(1))
            SET_VALUE(LW(0), 60)
            IF_INT_LESS(LW(1), LW(0))
                CALL_CHILD(stamp_attack_event)
                GOTO(99)
            ELSE
                CALL_CHILD(arm_bunri_event)
                GOTO(99)
            END_IF
        END_IF
    ELSE
        CALL_CHILD(shake_attack_event)
        GOTO(99)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(stamp_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_batten_stamp_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_batten_stamp_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 5, 1)
        CALL(btlevtcmd_StatusWindowOnOff, 0)
        CALL(evt_btl_camera_set_prilimit, 1)
        CALL(evt_btl_camera_set_mode, 1, 3)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 20)
        CALL(evt_btl_camera_set_moveto, 1, LW(0), 160, 600, LW(0), 110, -100, 20, 11)
        WAIT_FRAMES(20)
        CALL(evt_msg_print, 2, STRING("stg2_mri_e24_11"), 0, -2)
        CALL(evt_btl_camera_set_prilimit, 0)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_StatusWindowOnOff, 1)
        WAIT_MS(500)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_batten_stamp_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_batten_stamp_attack))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 100)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_MovePosition, -2, -50, 0, 0, 120, -1, 1)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    IF_INT_GREATER_OR_EQUAL(LW(0), -125)
        SET_VALUE(LW(15), 1)
    ELSE
        SET_VALUE(LW(15), 0)
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_LEG_UP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_1"))
    WAIT_MS(450)
    BEGIN_SIBLING_EVENT
        LOOP(3)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_LEG_PERPLEX1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_MS(333)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_2"))
    WAIT_MS(1000)
    IF_INT_EQUAL(LW(15), 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_3"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_4"))
    END_IF
    WAIT_MS(150)
    CALL(evt_btl_camera_shake_h, 0, 5, 0, 60, 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_stamp_attack), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(95)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_batten_stamp_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_stamp_attack), 256, LW(5))
    GOTO(95)
    LABEL(95)
    WAIT_MS(1000)
    IF_INT_EQUAL(LW(15), 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_5"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A2_6"))
    END_IF
    WAIT_MS(1000)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(200)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 41)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 60, -2, 0)
    RETURN
EVT_END()

USER_FUNC(eff_hokori) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    s32 a = evtGetValue(event, args[3]);
    EffectEntry* effect = effHokoriN64Entry(0, a, x, y, z);

    ((effHokoriData*)effect->userdata)->tevReg0R = 0xFF;
    ((effHokoriData*)effect->userdata)->tevReg0G = 0xFF;
    ((effHokoriData*)effect->userdata)->tevReg0B = 0xFF;
    ((effHokoriData*)effect->userdata)->tevReg1R = 0;
    ((effHokoriData*)effect->userdata)->tevReg1G = 0;
    ((effHokoriData*)effect->userdata)->tevReg1B = 0;
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(shake_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_batten_shake_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_batten_shake_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_batten_shake_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_batten_shake_attack))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A3_3"))
    WAIT_MS(900)
    CALL(btlevtcmd_CheckActStatus, -3, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckStatus, -3, 18, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_StopWaitEvent, -3)
            CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_I_O"))
        END_IF
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_QUAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_CAILING1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(evt_btl_camera_shake_h, 0, 7, 0, 30, 0)
    CALL(eff_hokori, 0, 250, 0, 80)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A3_4"))
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(20)
        CALL(eff_hokori, 100, 250, 0, 80)
        WAIT_FRAMES(20)
        CALL(eff_hokori, -100, 250, 0, 80)
        WAIT_FRAMES(20)
        CALL(eff_hokori, 50, 250, 0, 80)
        WAIT_FRAMES(20)
        CALL(eff_hokori, -100, 250, 0, 80)
        WAIT_FRAMES(20)
        CALL(evt_snd_sfxoff, LW(15))
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        LOOP(12)
            WAIT_FRAMES(10)
            ADD_FLOAT(LW(0), FLOAT(0.7f))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_QUAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(evt_btl_camera_shake_h, 0, LW(0), 0, 10, 0)
        END_LOOP
        CALL(evt_btl_camera_shake_h, 0, 6, 0, 60, 0)
    END_SIBLING_EVENT
    WAIT_FRAMES(120)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(6), 0)
    LABEL(10)
    IF_INT_EQUAL(LW(3), -1)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_shake_attack), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_CheckActStatus, -3, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckStatus, -3, 18, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_StopWaitEvent, -3)
            CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
        END_IF
    END_IF
    GOTO(93)
    LABEL(91)
    IF_INT_EQUAL(LW(6), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_batten_shake_attack))
        SET_VALUE(LW(6), 1)
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_shake_attack), 256, LW(5))
    GOTO(93)
    LABEL(93)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    GOTO(10)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(1000)
    RETURN
EVT_END()

static EVT_BEGIN(arm_bunri_event)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_StatusWindowOnOff, 0)
        CALL(evt_btl_camera_set_prilimit, 1)
        CALL(evt_btl_camera_set_mode, 1, 3)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 20)
        CALL(evt_btl_camera_set_moveto, 1, LW(0), 160, 600, LW(0), 110, -100, 20, 11)
        WAIT_FRAMES(20)
        CALL(evt_msg_print, 2, STRING("stg2_mri_e24_12"), 0, -2)
        CALL(evt_btl_camera_set_prilimit, 0)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_StatusWindowOnOff, 1)
        WAIT_MS(500)
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_ACTIVE1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_1"))
    WAIT_MS(600)
    CALL(evt_snd_sfxoff, LW(15))
    WAIT_MS(200)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_STOP1"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_2"))
    WAIT_MS(500)
    CALL(evt_snd_sfxoff, LW(15))
    WAIT_MS(100)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_3"))
    WAIT_MS(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_IGNITE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_shake_w, 0, 3, 0, 20, 0)
    WAIT_MS(100)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_rocket_punch), 0)
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, 0)
    CALL(btlevtcmd_SetUnitWork, LW(3), 1, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 3, LW(3))
    CALL(btlevtcmd_AnimeChangePoseType, LW(3), 1, 69)
    CALL(btlevtcmd_SpawnUnit, LW(4), PTR(entry_rocket_punch), 0)
    CALL(btlevtcmd_SetUnitWork, LW(4), 0, 1)
    CALL(btlevtcmd_SetUnitWork, LW(4), 1, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 4, LW(4))
    CALL(btlevtcmd_AnimeChangePoseType, LW(4), 1, 69)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_no_arm))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    WAIT_MS(1500)
    RETURN
EVT_END()

static EVT_BEGIN(arm_return_event)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_6"))
    WAIT_MS(600)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_rocket_punch), 0)
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, 2)
    CALL(btlevtcmd_SetUnitWork, -2, 3, LW(3))
    CALL(btlevtcmd_AnimeChangePoseType, LW(3), 1, 69)
    CALL(btlevtcmd_SpawnUnit, LW(4), PTR(entry_rocket_punch), 0)
    CALL(btlevtcmd_SetUnitWork, LW(4), 0, 3)
    CALL(btlevtcmd_SetUnitWork, -2, 4, LW(4))
    CALL(btlevtcmd_AnimeChangePoseType, LW(4), 1, 69)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetPos, LW(3), 300, 40, -100)
        CALL(btlevtcmd_DivePosition, LW(3), 30, 100, -40, 30, -10, 0, 0, -1)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetPos, LW(4), 350, 40, 100)
    CALL(btlevtcmd_DivePosition, LW(4), 80, 100, 40, 30, -10, 0, 0, -1)
    CALL(arm_end, 0)
    CALL(arm_end, 1)
    CALL(btlevtcmd_KillUnit, LW(3), 0)
    CALL(btlevtcmd_KillUnit, LW(4), 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_IGNITE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_shake_w, 0, 3, 0, 20, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_7"))
    WAIT_MS(300)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_8"))
    WAIT_MS(600)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    WAIT_MS(500)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

BattleUnitKind unit_boss_rocket_punch = {
    UNIT_X_FIST,                // mUnitType
    "btl_un_rocket_punch",      // name
    2,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    0,                          // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // escapeRate
    10,                         // renzokuSoftCap
    50,                         // width
    34,                         // height
    { 0, 15 },                  // statusIconOffset
    { 0.0f, 0.0f, 0.0f },       // centerOffset
    { 0, -20 },                 // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 25.0f, 0.0f, -10.0f },    // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // kagenukeOffsetX
    0.0f,                       // kagenukeOffsetZ
    { 25.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 24.0f, 16.0f, 0.0f },     // kissHitOffset
    { -5.0f, 0.0f, 0.0f },      // cutBaseOffset
    50.0f,                      // cutWidth
    34.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    -1,                         // swallowRate
    0,                          // swallowAttribute
    100,                        // shotDamageRate
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_BOSS_MBTN_DAMAGED1",   // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x00000004,                 // attributeFlags
    &regist_r,                  // vulnStatus
    1,                          // numParts
    parts_r,                    // mParts
    init_event_r,               // initEvtScript
    data_table_r,               // dataTable
};

static BattleUnitDefense defence_r = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr_r = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist_r = {
    0,   // sleepRate
    80,  // stopRate
    80,  // dizzyRate
    100, // poisonRate
    80,  // confuseRate
    100, // electricRate
    100, // burnRate
    80,  // freezeRate
    0,   // hugeRate
    0,   // tinyRate
    100, // attackUpRate
    0,   // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    80,  // allergicRate
    0,   // frightRate
    100, // galeForceRate
    100, // fastRate
    80,  // slowRate
    0,   // dodgyRate
    0,   // invisibleRate
    0,   // instaKillRate
};

static BattleDispPose pose_table_r[14] = {
    { 0x01, "MGN_A1_9" },
    { 0x02, "MGN_A1_4" },
    { 0x09, "MGN_A1_4" },
    { 0x05, "MGN_A1_9" },
    { 0x04, "MGN_A1_9" },
    { 0x03, "MGN_A1_9" },
    { 0x1C, "MGN_A1_9" },
    { 0x1D, "MGN_A1_9" },
    { 0x1E, "MGN_A1_9" },
    { 0x1F, "MGN_A1_9" },
    { 0x27, "MGN_A1_9" },
    { 0x38, "MGN_A1_9" },
    { 0x39, "MGN_A1_9" },
    { 0x45, "MGN_A1_9" },
};

static BattleDataEntry data_table_r[1] = {
    { 0x00, NULL },
};

static BattleUnitKindPart parts_r[1] = {
    {
        1,                     // partNum
        "btl_un_rocket_punch", // partName
        "c_mb_robo_1b",        // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 10.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 10.0f, 0.0f }, // hitCursorBaseOffset
        106,                   // shadowSize
        174,                   // field_0x32
        255,                   // baseAlpha
        &defence_r,            // defense
        &defence_attr_r,       // defenseAttr
        0x00600009,            // attributes
        0,                     // counterAttributes
        pose_table_r,          // poseTable
    },
};

static BattleWeapon weapon_batten_rocket_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    2,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20001000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x00002004,                               // targetWeightingFlags
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

static USER_FUNC(arm_init) {
    s32 i;

    for (i = 0; i < 2; i++) {
        work[i] = BattleAlloc(0x24);
        work[i]->prevPos = work[i]->pos = (Vec){0.0f, 0.0f, 0.0f};
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(arm_init2) {
    s32 index = evtGetValue(event, event->args[0]);
    MagnumBattenderWork* w = work[index];

    w->effect = effBattenEntry(3, 1000, 0.0f, -1000.0f, 0.0f, 1.0f);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(init_event_r)
    CALL(btlevtcmd_OnUnitFlag, -2, 16777216)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_4"))
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event_r))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event_r))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event_r))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetBaseRotate, -2, 0, -90, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(arm_init2, 0)
    END_IF
    IF_INT_EQUAL(LW(0), 2)
        CALL(arm_init2, 0)
    END_IF
    IF_INT_EQUAL(LW(0), 1)
        CALL(arm_init2, 1)
    END_IF
    IF_INT_EQUAL(LW(0), 3)
        CALL(arm_init2, 1)
    END_IF
    SPAWN_EVENT(arm_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetPos, -2, 30, 100, -40)
        CALL(btlevtcmd_DivePosition, -2, -100, 150, -60, 20, 0, 4, 0, -1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_DivePosition, -2, 350, 110, -80, 30, 50, 0, 0, -1)
        CALL(btlevtcmd_DivePosition, -2, 30, 60, -40, 25, -30, 4, 0, -1)
        CALL(btlevtcmd_SetHomePos, -2, 30, 60, -40)
        CALL(btlevtcmd_StartWaitEvent, -2)
    END_IF
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_SetPos, -2, 80, 100, 40)
        CALL(btlevtcmd_DivePosition, -2, -50, 150, 60, 20, 0, 4, 0, -1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_DivePosition, -2, 300, 120, 80, 30, 50, 0, 0, -1)
        CALL(btlevtcmd_DivePosition, -2, 80, 60, 40, 30, -30, 4, 0, -1)
        CALL(btlevtcmd_SetHomePos, -2, 80, 60, 40)
        CALL(btlevtcmd_StartWaitEvent, -2)
    END_IF
    RETURN
EVT_END()

static USER_FUNC(arm_main) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    MagnumBattenderWork* w;
    BattenData* data;
    s32 id;
    Mtx mtx1;
    Mtx mtx2;
    Vec offset;
    Vec pos;
    f32 x;
    f32 y;
    f32 z;
    f32 rx;
    f32 ry;
    f32 rz;
    f32 bx;
    f32 by;
    f32 bz;

    id = evtGetValue(event, args[0]);
    w = work[evtGetValue(event, args[1])];
    if (w->effect == NULL) {
        return EVT_RETURN_DONE;
    }
    data = w->effect->userdata;
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    BtlUnit_GetPos(unit, &x, &y, &z);
    w->prevPos = w->pos;
    pos = (Vec){0.0f, 0.0f, 0.0f};
    pos.x = x;
    pos.y = y;
    pos.z = z;
    w->pos = pos;
    if (evtCheckID(unit->waitEventId) != 1) {
        f32 dx = w->pos.x - w->prevPos.x;
        f32 dy = w->pos.y - w->prevPos.y;

        if (dx != 0.0f) {
            w->targetRotZ = 360.0f * (f32)atan(dy / dx) / 6.283184f;
            w->rotZ = w->rotZ + (w->targetRotZ - w->rotZ) / 3.0f;
            BtlUnit_GetRotate(unit, &rx, &ry, &rz);
            BtlUnit_SetRotate(unit, rx, ry, w->rotZ);
        } else {
            w->targetRotZ = 0.0f;
        }
    }
    BtlUnit_GetBaseRotate(unit, &bx, &by, &bz);
    BtlUnit_GetRotate(unit, &rx, &ry, &rz);
    rx += bx;
    ry += by;
    rz += bz;
    PSMTXRotRad(mtx1, 'x', 1.5707964f);
    PSMTXRotRad(mtx2, 'y', MTXDegToRad(ry));
    PSMTXConcat(mtx2, mtx1, mtx1);
    PSMTXRotRad(mtx2, 'x', MTXDegToRad(rx));
    PSMTXConcat(mtx2, mtx1, mtx1);
    PSMTXRotRad(mtx2, 'z', MTXDegToRad(rz));
    PSMTXConcat(mtx2, mtx1, mtx1);
    PSMTXCopy(mtx1, data->matrix);
    offset = (Vec){0.0f, -18.0f, 0.0f};
    PSMTXMultVec(mtx1, &offset, &offset);
    data->position.x = w->pos.x + offset.x;
    data->position.y = w->pos.y + offset.y;
    data->position.z = w->pos.z + offset.z;
    return EVT_RETURN_DONE;
}

static USER_FUNC(arm_end) {
    MagnumBattenderWork* w = work[evtGetValue(event, event->args[0])];

    if (w->effect != NULL) {
        effDelete(w->effect);
        w->effect = NULL;
    }
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(arm_event)
    LABEL(0)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    IF_INT_EQUAL(LW(0), -1)
        CALL_CHILD(arm_event_end)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(arm_main, -2, 0)
    END_IF
    IF_INT_EQUAL(LW(0), 2)
        CALL(arm_main, -2, 0)
    END_IF
    IF_INT_EQUAL(LW(0), 1)
        CALL(arm_main, -2, 1)
    END_IF
    IF_INT_EQUAL(LW(0), 3)
        CALL(arm_main, -2, 1)
    END_IF
    GOTO(0)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(arm_event_end)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(arm_end, 0)
    END_IF
    IF_INT_EQUAL(LW(0), 2)
        CALL(arm_end, 0)
    END_IF
    IF_INT_EQUAL(LW(0), 1)
        CALL(arm_end, 1)
    END_IF
    IF_INT_EQUAL(LW(0), 3)
        CALL(arm_end, 1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(damage_event_r)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_CheckDamageCode, -2, 512, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckDamageCode, -2, 2048, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL_CHILD(arm_event_end)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(attack_event_r)
    CALL(btlevtcmd_AddUnitWork, -2, 1, -1)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_batten_rocket_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_batten_rocket_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(evt_sub_random, 1, LW(6))
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_batten_rocket_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_batten_rocket_attack))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MGN_A1_4"))
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_rocket_attack), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(95)
    LABEL(91)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_GetPos, -2, LW(10), LW(11), LW(12))
            IF_INT_LESS_OR_EQUAL(LW(10), LW(0))
                CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_batten_rocket_attack))
                CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_batten_rocket_attack), 256, LW(5))
                CALL(evt_btl_camera_shake_w, 0, 3, 0, 10, 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    GOTO(95)
    LABEL(95)
    IF_INT_EQUAL(LW(6), 0)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 110)
        SET_VALUE(LW(1), 120)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, 10, 4, 0, -1)
        WAIT_FRAMES(5)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), -20)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_DivePosition, -2, -250, 0, LW(2), 20, -10, 0, 0, -1)
    ELSE
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), -200)
        ADD(LW(1), 140)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 40, -80, 1, 0, -1)
    END_IF
    WAIT_MS(250)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(10))
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_SetPos, -2, 300, LW(1), -100)
    ELSE
        CALL(btlevtcmd_SetPos, -2, 300, LW(1), 100)
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_MBTN_PUNCH_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, 0, 4, 0, -1)
    LABEL(99)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event_r)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    LABEL(0)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

// clang-format on
