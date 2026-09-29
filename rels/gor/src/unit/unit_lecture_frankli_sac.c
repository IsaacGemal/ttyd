#include "unit_lecture_frankli_sac.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "evt/evt_cmd.h"
#include "evt/evt_msg.h"
#include "forcedata.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(lecture_init);
static USER_FUNC(_battle_set_lecture_unit_id);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[1];
static BattleUnitKindPart parts[1];
static s32 init_event[61];
static s32 phase_event[105];
s32 actend_event[538];
static s32 damage_event[75];
static s32 wait_event[6];

// clang-format off
extend_data(80,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(81,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_lecture_frankli_sac = {
    UNIT_TUTORIAL_FRANKLY_B3,   // mUnitType
    "btl_un_frankli",           // name
    10,                         // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    1,                          // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    0,                          // bonusCoinRate
    0,                          // baseCoin
    100,                        // field_0x13
    9,                          // field_0x14
    28,                         // width
    24,                         // height
    { 5, 25 },                  // statusIconOffset
    { 0.0f, 14.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 30.0f, 0.0f },      // talkTogeBaseOffset
    { 14.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 14.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 14.0f, 15.6f, 0.0f },     // kissHitOffset
    { 0.0f, 0.0f, 0.0f },       // cutBaseOffset
    0.0f,                       // cutWidth
    0.0f,                       // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    -1,                         // swallowRate
    0,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_ENM_KURI_DAMAGED1",    // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x02000000,                 // attributeFlags
    &regist,                    // vulnStatus
    1,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

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

static BattleDispPose pose_table[5] = {
    { 0x1B, "D_1" },
    { 0x1C, "S_1" },
    { 0x1F, "D_2" },
    { 0x27, "D_2" },
    { 0x45, "S_1" },
};

static BattleDataEntry data_table[1] = {
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                     // partNum
        "btl_un_frankli",      // partName
        "c_kuri_h",            // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 25.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // field_0x30
        30,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00880009,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
};

// TODO: write _set_actend_event (global, 0x70 bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(_test_event)
    CALL(btlevtcmd_SetTalkPose, -6, STRING("T_1"))
    CALL(btlevtcmd_SetStayPose, -6, STRING("S_1"))
    CALL(evt_msg_print, 2, STRING("mac_0_b_t3_000"), 0, -2)
    CALL(_set_actend_event, -3, 0)
    RETURN
EVT_END()

EVT_BEGIN(stop_wait_event)
    CALL(btlevtcmd_StopWaitEvent, -3)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -3, 1)
    CALL(btlevtcmd_StopWaitEvent, -4)
    CALL(btlevtcmd_GetBodyId, -4, LW(0))
    CALL(btlevtcmd_AnimeChangePoseFromTable, -4, LW(0))
    RETURN
EVT_END()

// TODO: write lecture_init (static, 0xf8 bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it

// TODO: write _battle_set_lecture_unit_id (static, 0x40 bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(init_event)
    CALL(lecture_init)
    CALL(btlevtcmd_onoff_battleflag, 1, 4096)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    CALL(_battle_set_lecture_unit_id, LW(0))
    CALL(_set_actend_event, -3, PTR(actend_event))
    CALL(_set_actend_event, -4, PTR(actend_event))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_SetMaxMoveCount, -2, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// TODO: write spot_square (global, 0x1b8 bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it

// TODO: write blinkAP (global, 0x28 bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_get_turn, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL_CHILD(stop_wait_event)
            CALL(btlevtcmd_SetTalkPose, -2, STRING("T_1"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("S_1"))
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(evt_msg_print, 2, STRING("mac_0_b_t3_000"), 0, -2)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
            WAIT_FRAMES(60)
            CALL(blinkAP)
            CALL(spot_square, -105, 172, 206, 128, 20, 120)
            WAIT_FRAMES(30)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            WAIT_MS(500)
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(evt_btl_camera_set_moveto, 0, 92, 94, 320, 92, 44, 13, 1, 11)
            WAIT_MS(500)
            CALL(evt_msg_print, 2, STRING("mac_0_b_t3_001"), 0, -2)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(btlevtcmd_onoff_battleflag, 1, 67108864)
            CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
            GOTO(99)
        END_IF
    END_IF
    LABEL(99)
    RETURN
EVT_END()

// TODO: write aud_attack_end (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it

// TODO: write aud_attack_init (global, 0x2c bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it

// TODO: write hammer_on (global, 0x80 bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it

// TODO: write aud_dec (global, 0x28 bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it

// TODO: write aud_inc (global, 0x28 bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it

// TODO: write aud_joy (global, 0x24 bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it

// TODO: write get_ac_result (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it

// TODO: write get_actkind (global, 0x38 bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(actend_event)
    CALL_CHILD(stop_wait_event)
    CALL(get_actkind, LW(0))
    IF_INT_EQUAL(LW(0), 4)
        CALL(btlevtcmd_GetUnitWork, -6, 0, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            WAIT_MS(1000)
            CALL_CHILD(stop_wait_event)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            WAIT_MS(500)
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(evt_btl_camera_set_moveto, 0, 92, 94, 320, 92, 44, 13, 1, 11)
            WAIT_MS(500)
            CALL(evt_msg_print, 2, STRING("mac_0_b_t3_003"), 0, -6)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
            WAIT_FRAMES(60)
            CALL(blinkAP)
            CALL(spot_square, -105, 172, 206, 128, 20, 120)
            WAIT_FRAMES(30)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            WAIT_MS(500)
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(evt_btl_camera_set_moveto, 0, 92, 94, 320, 92, 44, 13, 1, 11)
            WAIT_MS(500)
            CALL(evt_msg_print, 2, STRING("mac_0_b_t3_004"), 0, -6)
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(evt_btl_camera_set_moveto, 0, 0, 40, 590, 0, 15, 60, 30, 11)
            WAIT_FRAMES(30)
            CALL(aud_joy)
            WAIT_FRAMES(60)
            CALL(aud_joy)
            WAIT_FRAMES(30)
            WAIT_MS(500)
            CALL(evt_btl_camera_set_mode, 0, 3)
            CALL(evt_btl_camera_set_moveto, 0, 92, 94, 320, 92, 44, 13, 1, 11)
            WAIT_MS(30)
            CALL(evt_msg_print, 2, STRING("mac_0_b_t3_005"), 0, -6)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(btlevtcmd_onoff_battleflag, 0, 67108864)
            CALL(btlevtcmd_onoff_battleflag, 1, 268435456)
            CALL(btlevtcmd_SetUnitWork, -6, 0, 2)
            GOTO(99)
        END_IF
    END_IF
    IF_INT_NOT_EQUAL(LW(0), 9)
        CALL(btlevtcmd_GetUnitWork, -6, 0, LW(0))
        IF_INT_EQUAL(LW(0), 2)
            WAIT_MS(200)
            CALL_CHILD(stop_wait_event)
            CALL(get_ac_result, LW(0))
            IF_INT_EQUAL(LW(0), 1)
                CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
                IF_INT_EQUAL(LW(0), 0)
                    WAIT_MS(2000)
                    CALL(btlevtcmd_StatusWindowOnOff, 0)
                    CALL(evt_msg_print, 2, STRING("mac_0_b_t3_006"), 0, -6)
                    CALL(btlevtcmd_StatusWindowOnOff, 1)
                    CALL(evt_btl_camera_set_mode, 0, 0)
                    WAIT_FRAMES(60)
                    CALL(blinkAP)
                    CALL(spot_square, -105, 172, 206, 128, 20, 120)
                    WAIT_FRAMES(30)
                    CALL(btlevtcmd_AddUnitWork, -6, 1, 1)
                    GOTO(99)
                END_IF
                IF_INT_EQUAL(LW(0), 1)
                    CALL(aud_inc)
                    WAIT_MS(2000)
                    CALL(btlevtcmd_StatusWindowOnOff, 0)
                    CALL(evt_msg_print, 2, STRING("mac_0_b_t3_007"), 0, -6)
                    CALL(evt_btl_camera_set_mode, 0, 0)
                    WAIT_FRAMES(60)
                    CALL(spot_square, -295, -145, -175, -215, 20, 120)
                    WAIT_FRAMES(30)
                    CALL(btlevtcmd_StatusWindowOnOff, 1)
                    CALL(btlevtcmd_AddUnitWork, -6, 1, 1)
                    GOTO(99)
                END_IF
                IF_INT_EQUAL(LW(0), 2)
                    CALL(btlevtcmd_StatusWindowOnOff, 0)
                    CALL(evt_msg_print, 2, STRING("mac_0_b_t3_013"), 0, -6)
                    CALL(evt_btl_camera_set_mode, 0, 3)
                    CALL(evt_btl_camera_set_moveto, 0, 92, 94, 320, 92, 44, 13, 30, 11)
                    WAIT_FRAMES(30)
                    CALL(evt_msg_print, 2, STRING("mac_0_b_t3_014"), 0, -6)
                    CALL(evt_btl_camera_set_mode, 0, 0)
                    WAIT_MS(1600)
                    CALL(hammer_on)
                    WAIT_MS(1000)
                    CALL(evt_msg_print, 2, STRING("mac_0_b_t3_014_1"), 0, -6)
                    CALL(evt_btl_camera_set_mode, 0, 3)
                    CALL(evt_btl_camera_set_moveto, 0, -30, 29, 499, -30, -3, 5, 30, 11)
                    WAIT_FRAMES(120)
                    CALL(evt_msg_toge, 1, 0, 0, 0)
                    CALL(evt_msg_print, 2, STRING("mac_0_b_t3_014_2"), 0, -6)
                    CALL(evt_btl_camera_set_mode, 0, 0)
                    CALL(evt_btl_camera_set_moveSpeedLv, 0, 3)
                    WAIT_FRAMES(30)
                    CALL(evt_msg_print, 2, STRING("mac_0_b_t3_014_3"), 0, -6)
                    CALL(btlevtcmd_SetUnitWork, -6, 0, 3)
                    CALL(aud_attack_init)
                    CALL(btlevtcmd_onoff_battleflag, 0, 268435456)
                    CALL(btlevtcmd_onoff_battleflag, 1, 8388608)
                    GOTO(99)
                END_IF
            ELSE
                CALL(btlevtcmd_GetUnitWork, -6, 2, LW(0))
                IF_INT_EQUAL(LW(0), 0)
                    CALL(btlevtcmd_StatusWindowOnOff, 0)
                    CALL(evt_msg_print, 2, STRING("mac_0_b_t3_008"), 0, -6)
                    CALL(btlevtcmd_StatusWindowOnOff, 1)
                    CALL(evt_btl_camera_set_mode, 0, 0)
                    CALL(btlevtcmd_AddUnitWork, -6, 2, 1)
                    GOTO(99)
                END_IF
                IF_INT_EQUAL(LW(0), 1)
                    CALL(aud_dec)
                    WAIT_MS(750)
                    CALL(btlevtcmd_StatusWindowOnOff, 0)
                    CALL(evt_msg_print, 2, STRING("mac_0_b_t3_009"), 0, -6)
                    CALL(btlevtcmd_StatusWindowOnOff, 1)
                    CALL(evt_btl_camera_set_mode, 0, 0)
                    CALL(btlevtcmd_AddUnitWork, -6, 2, 1)
                    GOTO(99)
                END_IF
                IF_INT_EQUAL(LW(0), 2)
                    CALL(btlevtcmd_StatusWindowOnOff, 0)
                    CALL(evt_msg_print, 2, STRING("mac_0_b_t3_010"), 0, -6)
                    CALL(btlevtcmd_StatusWindowOnOff, 1)
                    GOTO(99)
                END_IF
            END_IF
        END_IF
    END_IF
    CALL(btlevtcmd_GetUnitWork, -6, 0, LW(0))
    IF_INT_EQUAL(LW(0), 3)
        WAIT_MS(200)
        CALL(aud_attack_end)
        CALL_CHILD(stop_wait_event)
        CALL(evt_msg_print, 2, STRING("mac_0_b_t3_014_4"), 0, -6)
        CALL(evt_btl_camera_set_mode, 0, 3)
        CALL(evt_btl_camera_set_moveto, 0, 92, 94, 320, 92, 44, 13, 30, 11)
        WAIT_FRAMES(30)
        CALL(evt_msg_print, 2, STRING("mac_0_b_t3_015"), 0, -6)
        WAIT_MS(1000)
        CALL(btlevtcmd_onoff_battleflag, 1, 32)
        GOTO(99)
    END_IF
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 2)
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        IF_INT_EQUAL(LW(0), 2)
            WAIT_MS(1000)
            CALL(get_ac_result, LW(0))
            IF_INT_EQUAL(LW(0), 1)
                CALL(btlevtcmd_StatusWindowOnOff, 0)
                CALL(evt_msg_print, 2, STRING("mac_0_b_t3_011"), 0, -2)
                CALL(btlevtcmd_StatusWindowOnOff, 1)
                WAIT_FRAMES(60)
                CALL(spot_square, 198, 169, 294, 117, 20, 120)
                WAIT_FRAMES(30)
                CALL(btlevtcmd_StatusWindowOnOff, 0)
                CALL(evt_msg_print, 2, STRING("mac_0_b_t3_012"), 0, -2)
                CALL(btlevtcmd_StatusWindowOnOff, 1)
                WAIT_MS(500)
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

// TODO: type
u32 point[4] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 darkCount[1] = {
    0x00000000,
};

// TODO: type
u8 dark[1] = {
    0x00,
};

// TODO: write spot_square_disp (global, 0x390 bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it

// TODO: write Quads2D (global, 0xc4 bytes) here:
// its asm is in build/G8MJ01/gor/asm/unit/unit_lecture_frankli_sac.s; this is where the original has it, just before the first script that uses it
// clang-format on
