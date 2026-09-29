#include "battle/battle.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "evt/evt_cmd.h"
#include "evt/evt_sub.h"
#include "battle/battle_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"

#pragma pool_data off

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
static USER_FUNC(battleGetBarriernSatelliteInitPos);
static u32 parts[114];
static u32 regist[6];
static u32 pose_table[34];
static u32 data_table[2];
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

// TODO: type
u32 UNIT_DATA_SYM[49] = {
    0x0000001E, (u32)"btl_un_barriern_satellite", 0x00010000, 0x01010000,
    0x00460032, 0x270F0010, 0x00100000, 0x00080000,
    0x00000000, 0x00000000, 0x00000000, 0xFFFDFFFA,
    0x00000000, 0x00000000, 0x00000000, 0x41000000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41000000,
    0x00000000, 0x00000000, 0x40A00000, 0x40C00000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x41800000, 0x41800000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_BARRI_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000004,
    (u32)&regist, 0x06000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
static u32 defence[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[114] = {
    0x00000001, (u32)"btl_un_barriern_satellite", (u32)"c_baria", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x40A00000,
    0x00000000, 0x00000000, 0x41200000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00600009, 0x00000000, (u32)&pose_table, 0x00000002,
    (u32)"btl_un_barriern_satellite", (u32)"c_baria", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x40A00000, 0x00000000,
    0x00000000, 0x41200000, 0x00000000, 0x0014001E,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x13010000,
    0x00000000, (u32)&pose_table, 0x00000003, (u32)"btl_un_barriern_satellite",
    (u32)"c_baria", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x40A00000, 0x00000000, 0x00000000,
    0x41200000, 0x00000000, 0x0014001E, 0x00FF0000,
    (u32)&defence, (u32)&defence_attr, 0x13010000, 0x00000000,
    (u32)&pose_table, 0x00000004, (u32)"btl_un_barriern_satellite", (u32)"c_baria",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x40A00000, 0x00000000, 0x00000000, 0x41200000,
    0x00000000, 0x0014001E, 0x00FF0000, (u32)&defence,
    (u32)&defence_attr, 0x13010000, 0x00000000, (u32)&pose_table,
    0x00000005, (u32)"btl_un_barriern_satellite", (u32)"c_baria", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x40A00000,
    0x00000000, 0x00000000, 0x41200000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x13010000, 0x00000000, (u32)&pose_table, 0x00000006,
    (u32)"btl_un_barriern_satellite", (u32)"c_baria", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x40A00000, 0x00000000,
    0x00000000, 0x41200000, 0x00000000, 0x0014001E,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x13010000,
    0x00000000, (u32)&pose_table,
};

// TODO: type
static u32 regist[6] = {
    0x50646400, 0x5A640000, 0x645A645A, 0x645F5A00,
    0x64645A64, 0x64640000,
};

// TODO: type
static u32 pose_table[34] = {
    0x00000001, (u32)"N_2", 0x00000002, (u32)"Y_2",
    0x00000009, (u32)"Y_2", 0x00000005, (u32)"K_2",
    0x00000004, (u32)"X_2", 0x00000003, (u32)"X_2",
    0x0000001C, (u32)"S_2", 0x0000001D, (u32)"Q_2",
    0x0000001E, (u32)"Q_2", 0x0000001F, (u32)"S_2",
    0x00000027, (u32)"D_2", 0x0000002A, (u32)"R_2",
    0x00000029, (u32)"R_2", 0x00000028, (u32)"W_2",
    0x00000038, (u32)"X_2", 0x00000039, (u32)"X_2",
    0x00000045, (u32)"S_2",
};

// TODO: type
static u32 data_table[2] = {
    0x00000000, 0x00000000,
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
