#include "battle/battle.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "evt/evt_cmd.h"
#include "evt/evt_sub.h"
#include "battle/battle_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
static USER_FUNC(battleGetBarriernSatelliteInitPos);
static BattleUnitKindPart parts[6];
static BattleVulnerableStatus regist;
static BattleDispPose pose_table[17];
static BattleDataEntry data_table[1];
static s32 init_event[29];
static s32 damage_event[10];
static s32 unison_phase_event[5];
static s32 attack_event[5];
static s32 wait_event[56];

// clang-format off
extend_data(barriern_satellite_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(barriern_satellite_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind UNIT_DATA_SYM = {
    UNIT_MINI_YUX,               // mUnitType
    "btl_un_barriern_satellite", // name
    1,                           // maxHp
    0,                           // maxFp
    1,                           // mDangerHP
    1,                           // mPerilHP
    0,                           // mLevel
    0,                           // mBonusExp
    0,                           // bonusCoinRolls
    70,                          // bonusCoinRate
    0,                           // baseCoin
    50,                          // field_0x13
    9999,                        // field_0x14
    16,                          // width
    16,                          // height
    { 0, 8 },                    // statusIconOffset
    { 0.0f, 0.0f, 0.0f },        // centerOffset
    { -3, -6 },                  // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },        // talkTogeBaseOffset
    { 8.0f, 0.0f, -10.0f },      // field_0x3C
    { 0.0f, 0.0f, 0.0f },        // burnFlameOffset
    0.0f,                        // field_0x54
    0.0f,                        // field_0x58
    { 8.0f, 0.0f, 0.0f },        // bintaHitOffset
    { 5.0f, 6.0f, 0.0f },        // kissHitOffset
    { 0.0f, 0.0f, 0.0f },        // cutBaseOffset
    16.0f,                       // cutWidth
    16.0f,                       // cutHeight
    0,                           // turnOrder
    0,                           // turnOrderVariance
    0,                           // swallowRate
    2,                           // swallowAttribute
    100,                         // field_0x8C
    20,                          // itemStealParam
    { 0.0f, 0.0f, 0.0f },        // starPointDispOffset
    "SFX_ENM_BARRI_DAMAGED1",    // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",      // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",       // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",  // electricDamageSfxName
    0x00000004,                  // attributeFlags
    &regist,                     // vulnStatus
    6,                           // numParts
    parts,                       // mParts
    init_event,                  // initEvtScript
    data_table,                  // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleUnitKindPart parts[6] = {
    {
        1,                           // partNum
        "btl_un_barriern_satellite", // partName
        "c_baria",                   // modelName
        { 0.0f, 0.0f, 0.0f },        // positionOffset
        { 0.0f, 5.0f, 0.0f },        // hitBaseOffset
        { 0.0f, 10.0f, 0.0f },       // hitCursorBaseOffset
        20,                          // field_0x30
        30,                          // field_0x32
        255,                         // baseAlpha
        &defence,                    // defense
        &defence_attr,               // defenseAttr
        0x00600009,                  // attributes
        0,                           // counterAttributes
        pose_table,                  // poseTable
    },
    {
        2,                           // partNum
        "btl_un_barriern_satellite", // partName
        "c_baria",                   // modelName
        { 0.0f, 0.0f, 0.0f },        // positionOffset
        { 0.0f, 5.0f, 0.0f },        // hitBaseOffset
        { 0.0f, 10.0f, 0.0f },       // hitCursorBaseOffset
        20,                          // field_0x30
        30,                          // field_0x32
        255,                         // baseAlpha
        &defence,                    // defense
        &defence_attr,               // defenseAttr
        0x13010000,                  // attributes
        0,                           // counterAttributes
        pose_table,                  // poseTable
    },
    {
        3,                           // partNum
        "btl_un_barriern_satellite", // partName
        "c_baria",                   // modelName
        { 0.0f, 0.0f, 0.0f },        // positionOffset
        { 0.0f, 5.0f, 0.0f },        // hitBaseOffset
        { 0.0f, 10.0f, 0.0f },       // hitCursorBaseOffset
        20,                          // field_0x30
        30,                          // field_0x32
        255,                         // baseAlpha
        &defence,                    // defense
        &defence_attr,               // defenseAttr
        0x13010000,                  // attributes
        0,                           // counterAttributes
        pose_table,                  // poseTable
    },
    {
        4,                           // partNum
        "btl_un_barriern_satellite", // partName
        "c_baria",                   // modelName
        { 0.0f, 0.0f, 0.0f },        // positionOffset
        { 0.0f, 5.0f, 0.0f },        // hitBaseOffset
        { 0.0f, 10.0f, 0.0f },       // hitCursorBaseOffset
        20,                          // field_0x30
        30,                          // field_0x32
        255,                         // baseAlpha
        &defence,                    // defense
        &defence_attr,               // defenseAttr
        0x13010000,                  // attributes
        0,                           // counterAttributes
        pose_table,                  // poseTable
    },
    {
        5,                           // partNum
        "btl_un_barriern_satellite", // partName
        "c_baria",                   // modelName
        { 0.0f, 0.0f, 0.0f },        // positionOffset
        { 0.0f, 5.0f, 0.0f },        // hitBaseOffset
        { 0.0f, 10.0f, 0.0f },       // hitCursorBaseOffset
        20,                          // field_0x30
        30,                          // field_0x32
        255,                         // baseAlpha
        &defence,                    // defense
        &defence_attr,               // defenseAttr
        0x13010000,                  // attributes
        0,                           // counterAttributes
        pose_table,                  // poseTable
    },
    {
        6,                           // partNum
        "btl_un_barriern_satellite", // partName
        "c_baria",                   // modelName
        { 0.0f, 0.0f, 0.0f },        // positionOffset
        { 0.0f, 5.0f, 0.0f },        // hitBaseOffset
        { 0.0f, 10.0f, 0.0f },       // hitCursorBaseOffset
        20,                          // field_0x30
        30,                          // field_0x32
        255,                         // baseAlpha
        &defence,                    // defense
        &defence_attr,               // defenseAttr
        0x13010000,                  // attributes
        0,                           // counterAttributes
        pose_table,                  // poseTable
    },
};

static BattleVulnerableStatus regist = {
    80,  // sleepRate
    100, // stopRate
    100, // dizzyRate
    0,   // poisonRate
    90,  // confuseRate
    100, // electricRate
    0,   // burnRate
    0,   // freezeRate
    100, // hugeRate
    90,  // tinyRate
    100, // attackUpRate
    90,  // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    90,  // allergicRate
    0,   // frightRate
    100, // galeForceRate
    100, // fastRate
    90,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    100, // instaKillRate
};

static BattleDispPose pose_table[17] = {
    { 0x01, "N_2" },
    { 0x02, "Y_2" },
    { 0x09, "Y_2" },
    { 0x05, "K_2" },
    { 0x04, "X_2" },
    { 0x03, "X_2" },
    { 0x1C, "S_2" },
    { 0x1D, "Q_2" },
    { 0x1E, "Q_2" },
    { 0x1F, "S_2" },
    { 0x27, "D_2" },
    { 0x2A, "R_2" },
    { 0x29, "R_2" },
    { 0x28, "W_2" },
    { 0x38, "X_2" },
    { 0x39, "X_2" },
    { 0x45, "S_2" },
};

static BattleDataEntry data_table[1] = {
    { 0x00, NULL },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventUnisonPhase, -2, PTR(unison_phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_OnUnitFlag, -2, 3145728)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    RETURN
EVT_END()

static EVT_BEGIN(unison_phase_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    LABEL(0)
    CALL(evt_sub_random, 120, LW(0))
    ADD(LW(0), 30)
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckActStatus, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_CheckStatus, -2, 5, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_4"))
                WAIT_FRAMES(180)
                CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
            END_IF
        END_IF
    END_IF
    GOTO(0)
    RETURN
EVT_END()

// clang-format on
static USER_FUNC(battleGetBarriernSatelliteInitPos) {
    s32* args;
    BattleUnit* unit;
    s32 id;
    s32 n;
    s32 m;
    s32 i;
    s32 j;
    s32 indexX;
    s32 indexY;
    s32 indexZ;
    const Vec* p;
    BattleWork* wp;
    static const Vec VecData[] = {
        {25.f, 0.f, 0.f},   {25.f, 0.f, 0.f},   {-25.f, 0.f, 0.f}, {0.f, 25.f, 0.f}, {25.f, -25.f, 0.f},
        {-25.f, -25.f, 0.f}, {25.f, 25.f, 0.f}, {-25.f, 25.f, 0.f}, {25.f, -25.f, 0.f}, {-25.f, -25.f, 0.f},
    };

    args = event->args;
    wp = _battleWorkPointer;
    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    n = evtGetValue(event, args[1]);
    m = evtGetValue(event, args[2]);
    indexX = args[3];
    indexY = args[4];
    indexZ = args[5];
    p = VecData;
    for (i = 0; i < n; i++) {
        for (j = 0; j < i; j++) {
            p++;
        }
    }
    for (j = 0; j < m; j++) {
        p++;
    }
    evtSetValue(event, indexX, (s32)(p->x * unit->faceDirection));
    evtSetValue(event, indexY, (s32)p->y);
    evtSetValue(event, indexZ, (s32)p->z);
    return EVT_RETURN_DONE;
}
