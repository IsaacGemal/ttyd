#include "muj.h"
#include "data/mapdata.h"
#include "driver/effdrv.h"
#include "eff/n64/eff_kameki_tornade_n64.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "party.h"
#include "party/party_motion.h"
#include "system.h"
#include "muj_00.h"
#include "muj_01.h"
#include "muj_02.h"
#include "muj_04.h"
#include "muj_05.h"
#include "muj_10.h"
#include "muj_12.h"
#include "muj_20.h"
#include "muj_21.h"

// psndSFXOnEx_3D has no shared prototype (retail callers disagree on the widths).
s32 psndSFXOnEx_3D(const char* name, Vec* position, u8 vol, u8 pan, u16 pitch, u16 flags);

#pragma pool_data off

#pragma force_active on
static USER_FUNC(tornade_alpha_func);

// clang-format off
extend_data(0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(2,
     f64, _three, 3,
     f64, _half, 0.5)

f32 muj_ship_puka_data[16] = {
    -0.0f, -0.5f, -1.0f, -1.5f, -2.0f, -2.2f, -2.5f, -2.7f,
    -2.7f, -2.7f, -2.5f, -2.2f, -2.0f, -1.5f, -1.0f, -0.5f,
};

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("muj_00", muj_00_init_evt);
    relSetEvtAddr("muj_01", muj_01_init_evt);
    relSetEvtAddr("muj_02", muj_02_init_evt);
    relSetEvtAddr("muj_03", muj_03_init_evt);
    relSetEvtAddr("muj_04", muj_04_init_evt);
    relSetEvtAddr("muj_05", muj_05_init_evt);
    relSetEvtAddr("muj_10", muj_10_init_evt);
    relSetEvtAddr("muj_11", muj_11_init_evt);
    relSetEvtAddr("muj_12", muj_12_init_evt);
    relSetEvtAddr("muj_20", muj_20_init_evt);
    relSetEvtAddr("muj_21", muj_21_init_evt);
    relSetBtlAddr("muj", btlsetup_muj_tbl, btl_muj_setup_no_tbl);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
}

void _unresolved(void) {
}

USER_FUNC(muj_ship_puka_func) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    f32 scale = evtGetFloat(event, args[1]);

    evtSetFloat(event, args[2], scale * muj_ship_puka_data[index & 0xF]);
    return EVT_RETURN_DONE;
}

USER_FUNC(muj_npc_guruguru) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    NpcEntry* npc = evtNpcNameToPtr(event, name);
    f32 targetX = evtGetFloat(event, args[1]);
    f32 targetZ = evtGetFloat(event, args[2]);
    s32 direction = evtGetValue(event, args[3]);
    f32 speed = evtGetFloat(event, args[4]);
    s32 threshold = evtGetValue(event, args[5]);
    s32 msec;
    f32 frame;
    f32 x;
    f32 y;
    f32 z;
    f32 angle;
    Vec temp;

    if (isFirstCall) {
        event->savedTime = gp->renderTime;
        event->userdataf[1] = angleABf(targetX, targetZ, npc->position.x, npc->position.z);
        event->userdataf[2] = distABf(targetX, targetZ, npc->position.x, npc->position.z);
    }
    msec = ((u32)gp->renderTime - (u32)event->savedTime) / ((__OSBusClock / 4) / 1000);
    frame = sysMsec2FrameFloat((f32)msec);
    angle = event->userdataf[1] + speed * (f32)(s32)(0.5f + frame);
    x = targetX;
    y = npc->position.y;
    z = targetZ;
    movePos(&x, &z, event->userdataf[2], angle);
    if (direction == 0) {
        if (npc->position.x > x) {
            npc->direction = 270.0f;
        } else {
            npc->direction = 90.0f;
        }
    }
    temp = (Vec){ x, y, z };
    npc->prevPosition = temp;
    npc->position = npc->prevPosition;
    if (npc->footstepAccum > (f32)npc->footstepStride / npc->moveSpeed) {
        if (npc->footstepToggle != 0) {
            if (npc->footstepSfxA != NULL) {
                psndSFXOnEx_3D(npc->footstepSfxA, &npc->position, 0xFF, 0xFF, 0, 8);
            }
        } else {
            if (npc->footstepSfxB != NULL) {
                psndSFXOnEx_3D(npc->footstepSfxB, &npc->position, 0xFF, 0xFF, 0, 8);
            }
        }
        npc->footstepAccum = 0.0f;
        npc->footstepToggle = (s8)(1 - npc->footstepToggle);
    }
    if (npc->footstepSfxB != NULL || npc->footstepToggle != 0) {
        npc->footstepAccum += 1000.0f * npc->moveDt;
    }
    if (msec < threshold) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_YIELD;
}

USER_FUNC(muj_npc_fuwafuwa) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    NpcEntry* npc = evtNpcNameToPtr(event, name);
    s32 mode = evtGetValue(event, args[1]);
    s32 threshold = evtGetValue(event, args[2]);
    f32 table[32] = {
        0.0f, 0.5f, 1.0f, 1.5f, 2.0f, 2.2f, 2.5f, 2.7f,
        2.7f, 2.7f, 2.5f, 2.2f, 2.0f, 1.5f, 1.0f, 0.5f,
        -0.0f, -0.5f, -1.0f, -1.5f, -2.0f, -2.2f, -2.5f, -2.7f,
        -2.7f, -2.7f, -2.5f, -2.2f, -2.0f, -1.5f, -1.0f, -0.5f,
    };
    s32 msec;
    s32 index;
    f32 frame;
    f32 x;
    f32 y;
    f32 z;
    Vec temp;

    if (isFirstCall) {
        event->savedTime = gp->renderTime;
        event->userdataf[1] = npc->position.x;
        event->userdataf[2] = npc->position.y;
        event->userdataf[3] = npc->position.z;
    }
    msec = ((u32)gp->renderTime - (u32)event->savedTime) / ((__OSBusClock / 4) / 1000);
    frame = sysMsec2FrameFloat((f32)msec);
    index = (s32)(0.5f + frame);
    x = event->userdataf[1];
    y = event->userdataf[2];
    z = event->userdataf[3];
    switch (mode) {
    case 0:
        y += table[index & 0x1F];
        break;
    }
    temp = (Vec){ x, y, z };
    npc->prevPosition = temp;
    npc->position = npc->prevPosition;
    if (msec < threshold) {
        return EVT_RETURN_BLOCK;
    }
    temp = (Vec){ event->userdataf[1], event->userdataf[2], event->userdataf[3] };
    npc->prevPosition = temp;
    npc->position = npc->prevPosition;
    return EVT_RETURN_YIELD;
}

USER_FUNC(muj_npc_buruburu) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    NpcEntry* npc = evtNpcNameToPtr(event, name);
    s32 axis = evtGetValue(event, args[1]);
    f32 speed = evtGetFloat(event, args[2]);
    s32 period = evtGetValue(event, args[3]);
    s32 threshold = evtGetValue(event, args[4]);
    PartyEntry* party = partyGetPtr(1);
    s32 msec;
    f32 delta;
    f32 value;
    Vec temp;

    if (isFirstCall) {
        event->savedTime = gp->renderTime;
        event->userdataf[0] = 0.0f;
        if (npc == (NpcEntry*)-2) {
            event->userdataf[1] = party->position.x;
            event->userdataf[2] = party->position.y;
            event->userdataf[3] = party->position.z;
        } else {
            event->userdataf[1] = npc->position.x;
            event->userdataf[2] = npc->position.y;
            event->userdataf[3] = npc->position.z;
        }
    }
    msec = ((u32)gp->renderTime - (u32)event->savedTime) / ((__OSBusClock / 4) / 1000);
    if (msec % (period * 2) >= period) {
        delta = speed;
    } else {
        delta = -speed;
    }
    value = delta - event->userdataf[0];
    switch (axis) {
    case 0:
        if (npc == (NpcEntry*)-2) {
            party->position.x += value;
        } else {
            npc->position.x += value;
            npc->prevPosition.x = npc->position.x;
        }
        break;
    case 1:
        if (npc == (NpcEntry*)-2) {
            party->position.y += value;
        } else {
            npc->position.y += value;
            npc->prevPosition.y = npc->position.y;
        }
        break;
    case 2:
        if (npc == (NpcEntry*)-2) {
            party->position.z += value;
        } else {
            npc->position.z += value;
            npc->prevPosition.z = npc->position.z;
        }
        break;
    }
    event->userdataf[0] = delta;
    if (threshold == 0) {
        return EVT_RETURN_BLOCK;
    }
    if (msec < threshold) {
        return EVT_RETURN_BLOCK;
    }
    if (npc == (NpcEntry*)-2) {
        temp = (Vec){ event->userdataf[1], event->userdataf[2], event->userdataf[3] };
        party->position = temp;
    } else {
        temp = (Vec){ event->userdataf[1], event->userdataf[2], event->userdataf[3] };
        npc->prevPosition = temp;
        npc->position = npc->prevPosition;
    }
    return EVT_RETURN_YIELD;
}


EVT_BEGIN(muj_ship_puka)
    SET_VALUE(LW(10), 0)
    LOOP(0)
        CALL(muj_ship_puka_func, LW(10), LW(1), LW(11))
        SET_FLOAT_INDEXED(LW(0), LW(11))
        ADD(LW(10), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(korutesu_apper)
    CALL(evt_npc_get_position, STRING("コルテス"), LW(0), LW(1), LW(2))
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_KOR_APPEAR1"), LW(0), LW(1), LW(2), LW(10))
        WAIT_MS(1000)
        CALL(evt_snd_sfxoff, LW(10))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_KOR_APPEAR2"), LW(0), LW(1), LW(2), LW(10))
        WAIT_MS(2000)
        CALL(evt_snd_sfxoff, LW(10))
    END_INLINE_EVENT
    CALL(evt_eff64, STRING("tornade"), STRING("kameki_tornade_n64"), 0, LW(0), LW(1), LW(2), FLOAT(2.5f), 0, 0, 0, 0, 0, 0, 0)
    CALL(tornade_alpha_func, STRING("tornade"), FLOAT(0.0f))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, FLOAT(0.0f), FLOAT(100.0f), 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(tornade_alpha_func, STRING("tornade"), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(3000)
        CALL(evt_eff_softdelete, STRING("tornade"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(700)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(600)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(500)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(400)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(300)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(200)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(100)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(50)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(30)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(30)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
    END_INLINE_EVENT
    WAIT_MS(3000)
    RETURN
EVT_END()

EVT_BEGIN(korutesu_vanish)
    CALL(evt_npc_get_position, STRING("コルテス"), LW(0), LW(1), LW(2))
    CALL(evt_eff64, STRING("tornade"), STRING("kameki_tornade_n64"), 0, LW(0), LW(1), LW(2), FLOAT(2.5f), 0, 0, 0, 0, 0, 0, 0)
    CALL(tornade_alpha_func, STRING("tornade"), FLOAT(0.0f))
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, FLOAT(0.0f), FLOAT(100.0f), 1000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(tornade_alpha_func, STRING("tornade"), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_STG5_KOR_GOOUT1"), LW(10))
        WAIT_MS(3000)
        CALL(evt_snd_sfxoff, LW(10))
        CALL(evt_eff_softdelete, STRING("tornade"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(30)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(30)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(50)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(100)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(200)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(300)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(400)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(500)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(600)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(700)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
        CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 128)
        WAIT_MS(800)
        CALL(evt_npc_flag_onoff, 0, STRING("コルテス"), 128)
        WAIT_FRAMES(1)
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(muj_mario_pos_osreport)
    CALL(muj_mario_pos_osreport_func)
    RETURN
EVT_END()

static USER_FUNC(tornade_alpha_func) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 alpha = evtGetFloat(event, args[1]);
    EffectEntry* effect = effNameToPtr((const char*)name);
    effKamekiTornadeData* data = effect->userdata;

    data->alphaScale = alpha;
    return EVT_RETURN_DONE;
}

USER_FUNC(muj_mario_pos_osreport_func) {
    marioGetPtr();
    keyGetButtonTrg(0);
    return EVT_RETURN_BLOCK;
}
// clang-format on
