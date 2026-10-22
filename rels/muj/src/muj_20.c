#include "muj_20.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_light.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_nannpc.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "driver/mapdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_funemizu.h"
#include "eff/n64/eff_water_damage_n64.h"
#include "eff/eff_mahojin.h"
#include "eff/eff_mist.h"
#include "eff/eff_splash.h"
#include "manager/evtmgr_cmd.h"
#include "manager/filemgr.h"
#include "win/win_main.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "system.h"
#include "muj_10.h"
#include <dolphin/gx/GXTev.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include <stdlib.h>
#include <string.h>

// marcoship work, private to this unit (layout read from the original's asm).
typedef struct MarcoShip {
    f32 angle;        // 0x0, swing angle; marcoship_yure_func hands it to the script each frame
    f32 angleStep;    // 0x4, per-frame change of angle, sign flips at each end
    f32 angleLimit;   // 0x8, swing amplitude
    s32 settled;      // 0xC, set once the swing has returned to rest; marcoship_stop_wait_func waits on it
    s32 state;        // 0x10, 0 swing, 1 pause at an end, 2 swing back to zero (marcoship_stop_func), 3 at rest
    u8 pad_0x14[4];   // 0x14
    OSTime stateTime; // 0x18, renderTime when the current state began
} MarcoShip;

// logo work, private to this unit (layout read from the original's asm).
typedef struct LogoWork {
    FileEntry* tpl;  // 0x0, mariost.tpl
    f32 offsetY;     // 0x4, added to the logo's y: -50 to 0 while fading in, 0 to 100 while fading out
    s32 alpha;       // 0x8, 0-255, the logo's vertex alpha
    s32 state;       // 0xC
    s32 timer;       // 0x10, fade step counter; advanced twice per frame, once per intplGetValue call
} LogoWork;

#pragma pool_data off

#pragma force_active on
extern s32 muj_npc_buruburu[];
extern s32 muj_npc_fuwafuwa[];
extern s32 muj_npc_guruguru[];
extern s32 muj_ship_puka[];
static void draw(CameraId cameraId, void* param);
static void pressStartGX(LogoWork* work, s32 alpha, f32 x, f32 y);
static USER_FUNC(fune_eff);
static USER_FUNC(alpha_set_func);
static USER_FUNC(alpha_init_func);
static void alpha_tev(MapTevCallback* param);

// .bss work pointers for the alpha/tev callbacks and map animation rates.
MarcoShip* marcoship;
f32 mapAnimRate[4];
static u8* _ap;

// clang-format off
extend_data(40,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(41,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

USER_FUNC(urouro_func) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x1 = (f32)evtGetValue(event, args[1]);
    f32 z1 = (f32)evtGetValue(event, args[2]);
    f32 x2 = (f32)evtGetValue(event, args[3]);
    f32 z2 = (f32)evtGetValue(event, args[4]);
    s32 flag = evtGetValue(event, args[5]);
    s32 duration = evtGetValue(event, args[6]);
    s32 mode = evtGetValue(event, args[7]);
    NpcEntry* npc = evtNpcNameToPtr(event, name);
    s32 elapsed;
    f32 x;
    f32 z;

    if (isFirstCall) {
        event->savedTime = gp->renderTime;
    }
    elapsed = ((s32)gp->renderTime - (s32)event->savedTime) / (OS_TIMER_CLOCK / 1000);
    // The (s32) casts on elapsed make MWCC materialise the division in r4 and copy it to r25 across
    // the calls; the plain form computes straight into r25 (same idiom as muj_12's muj_item_move_3d_position).
    if (elapsed < duration) {
        x = intplGetValue(mode, (s32)elapsed, duration, x1, x2);
        z = intplGetValue(mode, (s32)elapsed, duration, z1, z2);
        if (flag == 0) {
            if (npc->position.x > x) {
                npc->direction = 270.0f;
            } else {
                npc->direction = 90.0f;
            }
        }
        npc->prevPosition.x = x;
        npc->position.x = x;
        npc->prevPosition.z = z;
        npc->position.z = z;
        return EVT_RETURN_BLOCK;
    } else {
        x = x2;
        z = z2;
        if (flag == 0) {
            if (npc->position.x > x) {
                npc->direction = 270.0f;
            } else {
                npc->direction = 90.0f;
            }
        }
        npc->prevPosition.x = x;
        npc->position.x = x;
        npc->prevPosition.z = z;
        npc->position.z = z;
        return EVT_RETURN_DONE;
    }
}

EVT_BEGIN(hiru_yoru_event)
    CALL(evt_map_playanim, STRING("hiru_yoru"), 0, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_map_set_playrate, STRING("hiru_yoru"), FLOAT(4.0f))
        WAIT_MS(750)
        CALL(evt_map_set_playrate, STRING("hiru_yoru"), FLOAT(3.4f))
        WAIT_MS(200)
        CALL(evt_map_set_playrate, STRING("hiru_yoru"), FLOAT(2.8f))
        WAIT_MS(100)
        CALL(evt_map_set_playrate, STRING("hiru_yoru"), FLOAT(1.5f))
    END_INLINE_EVENT
    LOOP(0)
        CALL(evt_map_checkanim, STRING("hiru_yoru"), LW(0), LW(1))
        IF_INT_EQUAL(LW(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(yoru_hiru_event)
    CALL(evt_map_playanim, STRING("yoru_hiru"), 0, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_map_set_playrate, STRING("yoru_hiru"), FLOAT(4.0f))
        WAIT_MS(750)
        CALL(evt_map_set_playrate, STRING("yoru_hiru"), FLOAT(3.4f))
        WAIT_MS(100)
        CALL(evt_map_set_playrate, STRING("yoru_hiru"), FLOAT(2.8f))
        WAIT_MS(100)
        CALL(evt_map_set_playrate, STRING("yoru_hiru"), FLOAT(1.5f))
    END_INLINE_EVENT
    LOOP(0)
        CALL(evt_map_checkanim, STRING("yoru_hiru"), LW(0), LW(1))
        IF_INT_EQUAL(LW(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

USER_FUNC(urouro_fuwafuwa_func) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x1 = (f32)evtGetValue(event, args[1]);
    f32 y1 = (f32)evtGetValue(event, args[2]);
    f32 z1 = (f32)evtGetValue(event, args[3]);
    f32 x2 = (f32)evtGetValue(event, args[4]);
    f32 y2 = (f32)evtGetValue(event, args[5]);
    f32 z2 = (f32)evtGetValue(event, args[6]);
    s32 flag = evtGetValue(event, args[7]);
    s32 duration = evtGetValue(event, args[8]);
    s32 mode = evtGetValue(event, args[9]);
    NpcEntry* npc = evtNpcNameToPtr(event, name);
    f32 tbl[32] = {
        0.0f,  0.5f,  1.0f,  1.5f,  2.0f,  2.2f,  2.5f,  2.7f,
        2.7f,  2.7f,  2.5f,  2.2f,  2.0f,  1.5f,  1.0f,  0.5f,
        -0.0f, -0.5f, -1.0f, -1.5f, -2.0f, -2.2f, -2.5f, -2.7f,
        -2.7f, -2.7f, -2.5f, -2.2f, -2.0f, -1.5f, -1.0f, -0.5f,
    };
    s32 elapsed;

    if (isFirstCall) {
        event->savedTime = gp->renderTime;
    }
    elapsed = ((s32)gp->renderTime - (s32)event->savedTime) / (OS_TIMER_CLOCK / 1000);
    if (elapsed < duration) {
        x1 = intplGetValue(mode, elapsed, duration, x1, x2);
        y1 = intplGetValue(mode, elapsed, duration, y1, y2);
        z1 = intplGetValue(mode, elapsed, duration, z1, z2);
        if (flag == 0) {
            if (npc->position.x > x1) {
                npc->direction = 270.0f;
            } else {
                npc->direction = 90.0f;
            }
        }
        {
            f32 tempY = y1 + tbl[sysMsec2Frame(elapsed) & 0x1F];
            Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
            temp.x = x1;
            temp.y = tempY;
            temp.z = z1;
            npc->prevPosition = temp;
        }
        npc->position = npc->prevPosition;
        return EVT_RETURN_BLOCK;
    } else {
        if (flag == 0) {
            if (npc->position.x > x2) {
                npc->direction = 270.0f;
            } else {
                npc->direction = 90.0f;
            }
        }
        {
            Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
            temp.x = x2;
            temp.y = y2;
            temp.z = z2;
            npc->prevPosition = temp;
        }
        npc->position = npc->prevPosition;
        return EVT_RETURN_DONE;
    }
}

USER_FUNC(marcoship_yure_init_func) {
    MarcoShip* ship = _mapAlloc(0x20);

    marcoship = ship;
    ship->angle = 0.0f;
    ship->angleStep = 0.02f;
    ship->angleLimit = 1.5f;
    ship->settled = 0;
    ship->state = 0;
    ship->stateTime = 0;
    return EVT_RETURN_DONE;
}

USER_FUNC(marcoship_yure_func) {
    MarcoShip* ship = marcoship;
    s32* args = event->args;
    f32 zero = 0.0f;
    s32 elapsed;

    switch (ship->state) {
    case 0:
        if (ship->angleStep > zero) {
            if (ship->angle > ship->angleLimit) {
                ship->angleStep *= -1.0f;
                ship->state = 1;
                ship->stateTime = gp->renderTime;
                break;
            }
        } else if (ship->angle < -ship->angleLimit) {
            ship->angleStep *= -1.0f;
            ship->state = 1;
            ship->stateTime = gp->renderTime;
            break;
        }
        ship->angle += ship->angleStep;
        break;
    case 1:
        elapsed = OSTicksToMilliseconds((u32)gp->renderTime - (u32)ship->stateTime);
        if (elapsed > 500) {
            ship->state = 0;
        }
        break;
    case 2:
        if (ship->angleStep > zero) {
            if (ship->angle > zero) {
                ship->state = 3;
                break;
            }
        } else if (ship->angle < zero) {
            ship->state = 3;
            break;
        }
        ship->angle += ship->angleStep;
        break;
    case 3:
        ship->angle = zero;
        ship->settled = 1;
        break;
    }
    evtSetFloat(event, args[0], ship->angle);
    evtSetValue(event, args[1], ship->settled);
    return EVT_RETURN_DONE;
}

USER_FUNC(marcoship_stop_func) {
    marcoship->state = 2;
    return EVT_RETURN_DONE;
}

USER_FUNC(marcoship_stop_wait_func) {
    return (marcoship->settled != 0) ? EVT_RETURN_DONE : 0;
}

USER_FUNC(evt_map_anim_rate) {
    f32* rate = mapAnimRate;
    f32 diff;

    if (isFirstCall) {
        rate[0] = 1.0f;
        rate[2] = 1.0f;
        rate[1] = 1.0f;
        rate[3] = 1.0f;
    }
    diff = rate[1] - rate[0];
    if (diff >= 0.005f) {
        diff = 0.005f;
    } else if (diff <= -0.005f) {
        diff = -0.005f;
    }
    rate[0] += diff;
    diff = rate[3] - rate[2];
    if (diff >= 0.005f) {
        diff = 0.005f;
    } else if (diff <= -0.005f) {
        diff = -0.005f;
    }
    rate[2] += diff;
    mapSetPlayRate("fune", rate[0]);
    mapSetPlayRate("fune_01", rate[0]);
    mapSetPlayRate("nami", rate[2]);
    mapSetPlayRate("nami_02", rate[2]);
    return EVT_RETURN_BLOCK;
}

// Matching note: the loop-invariant float literals are named locals so MWCC hoists them into
// f25-f31; `(f32)r` is computed once into `rf` before the second `irand`; the radius add is
// one `irand` with the 330 default loaded before the `d < 70` branch; and the declaration order
// `work, i, entry, r` is what lands i, entry and the entry offset in r29/r28/r27.
USER_FUNC(evt_set_fune_anim_rate) {
    mapAnimRate[1] = evtGetFloat(event, event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_set_nami_anim_rate) {
    mapAnimRate[3] = evtGetFloat(event, event->args[0]);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(urouro)
    CALL(evt_npc_get_position, LW(0), LW(11), LW(12), LW(13))
    LOOP(0)
        CALL(urouro_func, LW(0), LW(11), LW(13), LW(1), LW(3), LW(4), LW(5), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_npc_get_position, LW(0), LW(11), LW(12), LW(13))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_NPC_ESCAPE1"), LW(11), LW(12), LW(13), 0)
        END_INLINE_EVENT
        CALL(urouro_func, LW(0), LW(1), LW(3), LW(11), LW(13), LW(4), LW(5), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_npc_get_position, LW(0), LW(11), LW(12), LW(13))
            CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_NPC_ESCAPE1"), LW(11), LW(12), LW(13), 0)
        END_INLINE_EVENT
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(urouro_fuwafuwa)
    CALL(evt_npc_get_position, LW(0), LW(11), LW(12), LW(13))
    LOOP(0)
        CALL(urouro_fuwafuwa_func, LW(0), LW(11), LW(12), LW(13), LW(1), LW(2), LW(3), LW(4), LW(5), 0)
        CALL(urouro_fuwafuwa_func, LW(0), LW(1), LW(2), LW(3), LW(11), LW(12), LW(13), LW(4), LW(5), 0)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(buruburu)
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_sub_random, 200, LW(10))
    ADD(LW(10), 100)
    WAIT_MS(LW(10))
    CALL(muj_npc_buruburu, LW(0), 0, FLOAT(1.0f), 10, 0)
    RETURN
EVT_END()

EVT_BEGIN(fuwafuwa)
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    LOOP(0)
        CALL(muj_npc_fuwafuwa, LW(0), 0, 10000)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(kirimomi)
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 0, 0, 360, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_npc_set_ry, LW(10), LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(marco_ship_yure)
    CALL(marcoship_yure_init_func)
    CALL(evt_hit_bind_mapobj, STRING("A_a1"), STRING("S_a1"))
    LOOP(0)
        CALL(marcoship_yure_func, LW(0), LW(1))
        CALL(evt_mapobj_rotate, 1, STRING("S_a1"), 0, 0, LW(0))
        IF_INT_EQUAL(LW(1), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(moya_effect01)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_l_01"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_r_01"), 1)
    CALL(evt_sub_intpl_msec_init, 11, 1500, 0, 7000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 0)
        SUBTRACT(LW(2), LW(0))
        CALL(evt_mapobj_trans, 1, STRING("S_hai_l_01"), LW(0), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_hai_r_01"), LW(2), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(moya_effect02)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_l_02"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_r_02"), 1)
    CALL(evt_sub_intpl_msec_init, 11, 1500, 0, 7000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 0)
        SUBTRACT(LW(2), LW(0))
        CALL(evt_mapobj_trans, 1, STRING("S_hai_l_02"), LW(0), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_hai_r_02"), LW(2), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(moya_effect03)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_l_03"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_r_03"), 1)
    CALL(evt_sub_intpl_msec_init, 11, 1500, 0, 7000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 0)
        SUBTRACT(LW(2), LW(0))
        CALL(evt_mapobj_trans, 1, STRING("S_hai_l_03"), LW(0), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_hai_r_03"), LW(2), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(moya_effect04)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_l_04"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_r_04"), 1)
    CALL(evt_sub_intpl_msec_init, 11, 1500, 0, 7000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 0)
        SUBTRACT(LW(2), LW(0))
        CALL(evt_mapobj_trans, 1, STRING("S_hai_l_04"), LW(0), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_hai_r_04"), LW(2), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(moya_effect05)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_l_05"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hai_r_05"), 1)
    CALL(evt_sub_intpl_msec_init, 11, 1500, 0, 7000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_VALUE(LW(2), 0)
        SUBTRACT(LW(2), LW(0))
        CALL(evt_mapobj_trans, 1, STRING("S_hai_l_05"), LW(0), 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_hai_r_05"), LW(2), 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static void alpha_tev(MapTevCallback* param) {
    s32 stage = param->tevStage;
    u8* ap = _ap;

    GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColor(GX_TEVREG0, (GXColor){ 0, 0, 0, ap[0] });
    GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
    param->tevStage++;
}

static USER_FUNC(alpha_init_func) {
    _ap = _mapAlloc(1);
    _ap[0] = 0xFF;
    return EVT_RETURN_DONE;
}

static USER_FUNC(alpha_set_func) {
    _ap[0] = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(elmos1000tai_init) {
    NanNpcWork* work;
    s32 i;
    NanNpcEntry* entry;
    s32 r;
    f32 c90 = 90.0f;
    f32 c6p2832 = 6.2831855f;
    f32 c360 = 360.0f;
    f32 c0p9 = 0.9f;
    f32 c100 = 100.0f;
    f32 c0p5 = 0.5f;

    work = nanNPCWork;
    work->flags |= 1;
    for (i = 0; i < work->count; i++) {
        s32 d;
        s32 r2;
        f32 s;
    f32 angle;
        f32 v;
        f32 c;
        f32 rf;
        entry = &work->entries[i];
        entry->pose->flags |= 4;
        entry->color.a = 0;
        entry->facingAngle = c90;
        entry->field_0x70 = i;
        if (i % 3 != 0) {
            r = irand(140) + 110;
        } else {
            r = irand(360);
        }
        rf = (f32)r;
        entry->radius = 250;
        d = abs(180 - r);
        r2 = 330;
        if (d < 70) {
            r2 = (70 - d) * 1000 / 70 + 330;
        }
        entry->radius += irand(r2);
        angle = c6p2832 * rf / c360;
        v = (f32)entry->radius;
        s = (f32)sin(angle);
        c = (f32)cos(angle);
        entry->position.x = c0p9 * (v * s);
        entry->position.y = c100;
        entry->position.z = c0p5 * (v * c) - c100;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(elmos1000tai_start) {
    s32 i;
    NanNpcEntry* entry;
    NanNpcWork* work;

    work = nanNPCWork;
    for (i = 0; i < work->count; i++) {
        entry = &work->entries[i];
        entry->callback = elmos1000tai_seq00;
        entry->state = 0;
    }
    return EVT_RETURN_DONE;
}

void elmos1000tai_seq00(struct NanNpcEntry* entry) {
    s32 elapsed;

    switch (entry->state) {
    case 0:
        entry->startTime = gp->renderTime;
        entry->timer = irand(1000) + entry->radius * 3;
        entry->state = 1;
        // fallthrough
    case 1:
        elapsed = ((s32)gp->renderTime - (s32)entry->startTime) / (OS_TIMER_CLOCK / 1000);
        if (elapsed >= entry->timer) {
            entry->startTime = gp->renderTime;
            entry->timer = 500;
            entry->state = 2;
        }
        break;
    case 2:
        elapsed = ((s32)gp->renderTime - (s32)entry->startTime) / (OS_TIMER_CLOCK / 1000);
        if (elapsed < entry->timer) {
            entry->color.a = (u8)intplGetValue(0, elapsed, entry->timer, 0.0f, 255.0f);
        } else {
            entry->state = 3;
        }
        break;
    case 3:
        break;
    }
}

USER_FUNC(mist_func) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);
    EffectEntry* effect;
    s32 done = 0;
    s32 elapsed;

    if (isFirstCall) {
        x += (f32)(irand(150) - 75);
        y += (f32)(irand(150) - 75);
        z += (f32)(irand(150) - 75);
        event->userdata[1] = (s32)effMistEntry(0, 0, x, y, z, 0.0f);
        event->userdata[0] = 0;
    }
    effect = (EffectEntry*)event->userdata[1];
    switch (event->userdata[0]) {
    case 0:
        event->savedTime = gp->renderTime;
        event->userdata[2] = irand(1000) + 3000;
        event->userdata[0] = 1;
        break;
    case 1:
        elapsed = ((s32)gp->renderTime - (s32)event->savedTime) / (OS_TIMER_CLOCK / 1000);
        if (elapsed > event->userdata[2]) {
            event->userdata[0] = 2;
        }
        break;
    case 2:
        done = 1;
        break;
    }
    if (done == 0) {
        return EVT_RETURN_BLOCK;
    }
    effSoftDelete(effect);
    return EVT_RETURN_DONE;
}

USER_FUNC(marcoship_shizumu_func) {
    f32 value = evtGetFloat(event, event->args[0]);

    mapObjTranslate("S_a1", 0.0f, value, 0.0f);
    return EVT_RETURN_DONE;
}

USER_FUNC(splash_eff) {
    s32* args = event->args;
    f32 x = evtGetFloat(event, args[0]);
    f32 y = evtGetFloat(event, args[1]);
    f32 z = evtGetFloat(event, args[2]);

    effSplashEntry(0, x, y, z, 3.0f);
    return EVT_RETURN_DONE;
}

USER_FUNC(kaizoku_fune_eff) {
    Vec pos;
    EffectEntry* eff;
    f32 x;
    f32 z;
    f32 dist;
    f32 angle;
    f32 dir;

    if (isFirstCall) {
        event->userdata[0] = (s32)effFunemizuEntry(0, 0.0f, -1000.0f, 0.0f, 0.0f, 0.0f);
        mapObjGetPos("S_b", &pos);
        event->userdataf[2] = pos.x;
        *(f32*)&event->field_84 = pos.z;
    }
    eff = (EffectEntry*)event->userdata[0];
    x = event->userdataf[2];
    z = *(f32*)&event->field_84;
    mapObjGetPos("S_b", &pos);
    event->userdataf[2] = pos.x;
    *(f32*)&event->field_84 = pos.z;
    dist = distABf(x, z, pos.x, pos.z);
    angle = angleABf(x, z, pos.x, pos.z);
    dir = (angle < 180.0f) ? 1.0f : -1.0f;
    ((EffFunemizuData*)eff->userdata)->scale = 5.0f;
    ((EffFunemizuData*)eff->userdata)->x = 140.0f * dir + pos.x;
    ((EffFunemizuData*)eff->userdata)->y = pos.y - 160.0f;
    ((EffFunemizuData*)eff->userdata)->z = 40.0f + pos.z;
    ((EffFunemizuData*)eff->userdata)->speed = dist;
    if (angle < 180.0f) {
        ((EffFunemizuData*)eff->userdata)->yaw = 135.0f + angle;
    } else {
        ((EffFunemizuData*)eff->userdata)->yaw = 45.0f + angle;
    }
    return EVT_RETURN_BLOCK;
}

#ifdef MUST_MATCH
// Match-only: calling the sum through this inline makes MWCC keep 85.0f in f0 and pos.x in f1
// (the plain `85.0f + pos.x` picks f1 for the constant). See #else for the natural form.
static inline f32 fune_eff_add_x(f32 a, f32 b) {
    return a + b;
}
#endif

USER_FUNC(fune_eff) {
    Vec pos;
    EffectEntry* eff1;
    EffectEntry* eff2;
    f32 x;
    f32 z;
    f32 dist;
    f32 angle;

    if (isFirstCall) {
        event->userdata[0] = (s32)effFunemizuEntry(0, 0.0f, -1000.0f, 0.0f, 0.0f, 0.0f);
        event->userdata[1] = (s32)effFunemizuEntry(2, 0.0f, -1000.0f, 0.0f, 0.0f, 0.0f);
        mapObjGetPos("S_fune_m", &pos);
        event->userdataf[2] = pos.x;
        *(f32*)&event->field_84 = pos.z;
    }
    eff1 = (EffectEntry*)event->userdata[0];
    eff2 = (EffectEntry*)event->userdata[1];
    x = event->userdataf[2];
    z = *(f32*)&event->field_84;
    mapObjGetPos("S_fune_m", &pos);
    event->userdataf[2] = pos.x;
    *(f32*)&event->field_84 = pos.z;
    dist = distABf(x, z, pos.x, pos.z);
    angle = angleABf(x, z, pos.x, pos.z);
    (void)angle;
    if (evtGetValue(event, GSW(0)) == 2) {
        dist = 2.0f;
    }
    ((EffFunemizuData*)eff1->userdata)->scale = 3.0f;
#ifdef MUST_MATCH
    ((EffFunemizuData*)eff1->userdata)->x = fune_eff_add_x(pos.x, 85.0f);
#else
    ((EffFunemizuData*)eff1->userdata)->x = 85.0f + pos.x;
#endif
    ((EffFunemizuData*)eff1->userdata)->y = pos.y - 51.0f;
    ((EffFunemizuData*)eff1->userdata)->z = 30.0f + pos.z;
    ((EffFunemizuData*)eff1->userdata)->speed = dist;
    ((EffFunemizuData*)eff1->userdata)->yaw = 225.0f;
    ((EffFunemizuData*)eff2->userdata)->x = pos.x - 15.0f;
    ((EffFunemizuData*)eff2->userdata)->y = pos.y - 51.0f;
    ((EffFunemizuData*)eff2->userdata)->z = 90.0f + pos.z;
    ((EffFunemizuData*)eff2->userdata)->speed = dist;
    ((EffFunemizuData*)eff2->userdata)->yaw = 180.0f;
    if (dist > 1.0f) {
        if (gp->retraceCount % 15 == 0) {
            effWaterDamageN64Entry(5, 30, pos.x - 150.0f, pos.y - 50.0f, pos.z, 2.0f);
        }
    }
    return EVT_RETURN_BLOCK;
}

EVT_BEGIN(nami_alpha)
    CALL(alpha_init_func)
    CALL(evt_map_set_tevcallback, 1, PTR(alpha_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_umi_l_siro_b"), 16777216)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_umi_l_siro_b"), 256)
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(alpha_set_func, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(moonout_effect)
    CALL(alpha_init_func)
    CALL(evt_map_set_tevcallback, 1, PTR(alpha_tev))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_hai02"), 16777216)
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_hai03_moon"), 16777216)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_hai02"), 256)
    CALL(evt_mapobj_flag_onoff, 0, 0, STRING("S_hai03_moon"), 256)
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, 1500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(alpha_set_func, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(moya_effect)
    SPAWN_EVENT(moya_effect01)
    SPAWN_EVENT(moya_effect02)
    SPAWN_EVENT(moya_effect03)
    SPAWN_EVENT(moya_effect04)
    SPAWN_EVENT(moya_effect05)
    CALL(evt_map_playanim, STRING("onryou"), 0, 0)
    SPAWN_EVENT(moonout_effect)
    RETURN
EVT_END()

EVT_BEGIN(_kakushuku)
    CALL(evt_sub_intpl_msec_init, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_FLOAT_VALUE(LW(2), LW(0))
        DIVIDE_FLOAT(LW(2), FLOAT(100.0f))
        CALL(evt_npc_set_scale, STRING("エルモスA"), LW(2), LW(2), LW(2))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(elmos_a_kakushuku)
    SET_VALUE(LW(0), 100)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 150)
    SPAWN_EVENT(_kakushuku)
    WAIT_MS(500)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 100)
    SET_VALUE(LW(2), 150)
    SPAWN_EVENT(_kakushuku)
    LOOP(0)
        SET_VALUE(LW(0), 100)
        SET_VALUE(LW(1), 150)
        SET_VALUE(LW(2), 100)
        SPAWN_EVENT(_kakushuku)
        WAIT_MS(100)
        SET_VALUE(LW(0), 150)
        SET_VALUE(LW(1), 100)
        SET_VALUE(LW(2), 100)
        SPAWN_EVENT(_kakushuku)
        WAIT_MS(100)
        IF_INT_EQUAL(GF(10), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_VALUE(GF(11), 0)
    RETURN
EVT_END()

EVT_BEGIN(elmos_a_kaku)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 100)
    SET_VALUE(LW(2), 150)
    CALL_CHILD(_kakushuku)
    RETURN
EVT_END()

EVT_BEGIN(elmos_a_shuku)
    SET_VALUE(LW(0), 100)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 150)
    CALL_CHILD(_kakushuku)
    RETURN
EVT_END()

// TODO: type
static u32 ext_entry_data[63] = {
    (u32)"c_elmos", (u32)"S_1", 0x00000000, (u32)"c_elmos",
    (u32)"S_1", 0x3F800000, (u32)"c_elmos", (u32)"S_1",
    0x40000000, (u32)"c_elmos", (u32)"S_1", 0x40400000,
    (u32)"c_elmos", (u32)"S_1", 0x40800000, (u32)"c_elmos",
    (u32)"S_1", 0x40A00000, (u32)"c_elmos", (u32)"S_1",
    0x40C00000, (u32)"c_elmos", (u32)"S_1", 0x40E00000,
    (u32)"c_elmos", (u32)"S_1", 0x41000000, (u32)"c_elmos",
    (u32)"S_1", 0x41100000, (u32)"c_elmos", (u32)"S_1",
    0x41200000, (u32)"c_elmos", (u32)"S_1", 0x41300000,
    (u32)"c_elmos", (u32)"S_1", 0x41400000, (u32)"c_elmos",
    (u32)"S_1", 0x41500000, (u32)"c_elmos", (u32)"S_1",
    0x41600000, (u32)"c_elmos", (u32)"S_1", 0x41700000,
    (u32)"c_elmos", (u32)"S_1", 0x41800000, (u32)"c_elmos",
    (u32)"S_1", 0x41880000, (u32)"c_elmos", (u32)"S_1",
    0x41900000, (u32)"c_elmos", (u32)"S_1", 0x41980000,
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 elmos_animtbl[11] = {
    0x00000001, 0x00020003, 0x00040005, 0x00060007,
    0x00080009, 0x000A000B, 0x000C000D, 0x000E000F,
    0x00100011, 0x00120013, 0xFFFF0000,
};

static EVT_BEGIN(nannpc_mode_setup)
    CALL(evt_nannpc_init, PTR(ext_entry_data), 0, 0, 0, 1000)
    CALL(evt_nannpc_set_subfunc, 1, PTR(nannpc_ext_main_sub_fast))
    SET_VALUE(LW(0), 0)
    LOOP(1000)
        CALL(evt_nannpc_entry, LW(0))
        CALL(evt_nannpc_set_func, LW(0), 0)
        CALL(evt_nannpc_set_animtbl, LW(0), PTR(elmos_animtbl))
        ADD(LW(0), 1)
    END_LOOP
    CALL(elmos1000tai_init)
    RETURN
EVT_END()

// TODO: type
u32 mist_pos[91] = {
    0xFFFFFE70, 0x0000001E, 0xFFFFFF38, 0xFFFFFE70,
    0x0000001E, 0xFFFFFE70, 0xFFFFFE70, 0x0000001E,
    0xFFFFFDA8, 0xFFFFFE70, 0x0000001E, 0xFFFFFCE0,
    0xFFFFFE70, 0x0000001E, 0xFFFFFC18, 0xFFFFFE70,
    0x0000001E, 0xFFFFFB50, 0xFFFFFF38, 0x0000001E,
    0xFFFFFF38, 0xFFFFFF38, 0x0000001E, 0xFFFFFE70,
    0xFFFFFF38, 0x0000001E, 0xFFFFFDA8, 0xFFFFFF38,
    0x0000001E, 0xFFFFFCE0, 0xFFFFFF38, 0x0000001E,
    0xFFFFFC18, 0xFFFFFF38, 0x0000001E, 0xFFFFFB50,
    0x00000000, 0x0000001E, 0xFFFFFF38, 0x00000000,
    0x0000001E, 0xFFFFFE70, 0x00000000, 0x0000001E,
    0xFFFFFDA8, 0x00000000, 0x0000001E, 0xFFFFFCE0,
    0x00000000, 0x0000001E, 0xFFFFFC18, 0x00000000,
    0x0000001E, 0xFFFFFB50, 0x000000C8, 0x0000001E,
    0xFFFFFF38, 0x000000C8, 0x0000001E, 0xFFFFFE70,
    0x000000C8, 0x0000001E, 0xFFFFFDA8, 0x000000C8,
    0x0000001E, 0xFFFFFCE0, 0x000000C8, 0x0000001E,
    0xFFFFFC18, 0x000000C8, 0x0000001E, 0xFFFFFB50,
    0x00000190, 0x0000001E, 0xFFFFFF38, 0x00000190,
    0x0000001E, 0xFFFFFE70, 0x00000190, 0x0000001E,
    0xFFFFFDA8, 0x00000190, 0x0000001E, 0xFFFFFCE0,
    0x00000190, 0x0000001E, 0xFFFFFC18, 0x00000190,
    0x0000001E, 0xFFFFFB50, 0xFFFFD8F0,
};

EVT_BEGIN(mist_setup)
    LOOP(0)
        SET_VALUE(LW(10), 0)
        LOOP(0)
            SET_INT_READ_ADDR(PTR(mist_pos))
            READ_INT_INDEXED(LW(0), LW(10))
            IF_INT_EQUAL(LW(0), -10000)
                BREAK_LOOP
            END_IF
            ADD(LW(10), 1)
            READ_INT_INDEXED(LW(1), LW(10))
            ADD(LW(10), 1)
            READ_INT_INDEXED(LW(2), LW(10))
            ADD(LW(10), 1)
            BEGIN_INLINE_EVENT
                CALL(mist_func, LW(0), LW(1), LW(2))
            END_INLINE_EVENT
        END_LOOP
        WAIT_MS(2000)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(marcoship_shizumu)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_SHIP_QUAKE1"), LW(10))
    SET_FLOAT_VALUE(LW(2), FLOAT(0.0f))
    CALL(evt_hit_bind_mapobj, STRING("A_a1"), STRING("S_a1"))
    CALL(evt_sub_intpl_msec_init, 1, 0, -300, 8000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(marcoship_shizumu_func, LW(0))
        SET_FLOAT_VALUE(LW(3), LW(0))
        SUBTRACT_FLOAT(LW(3), LW(2))
        CALL(evt_npc_get_position, STRING("マルコ"), LW(4), LW(5), LW(6))
        ADD_FLOAT(LW(5), LW(3))
        CALL(evt_npc_set_position, STRING("マルコ"), LW(4), LW(5), LW(6))
        CALL(evt_npc_get_position, STRING("ガラの悪い水夫"), LW(4), LW(5), LW(6))
        ADD_FLOAT(LW(5), LW(3))
        CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), LW(4), LW(5), LW(6))
        CALL(evt_npc_get_position, STRING("オドオド水夫"), LW(4), LW(5), LW(6))
        ADD_FLOAT(LW(5), LW(3))
        CALL(evt_npc_set_position, STRING("オドオド水夫"), LW(4), LW(5), LW(6))
        CALL(evt_npc_get_position, STRING("水夫A"), LW(4), LW(5), LW(6))
        ADD_FLOAT(LW(5), LW(3))
        CALL(evt_npc_set_position, STRING("水夫A"), LW(4), LW(5), LW(6))
        CALL(evt_npc_get_position, STRING("水夫ボム兵"), LW(4), LW(5), LW(6))
        ADD_FLOAT(LW(5), LW(3))
        CALL(evt_npc_set_position, STRING("水夫ボム兵"), LW(4), LW(5), LW(6))
        CALL(evt_npc_get_position, STRING("店員"), LW(4), LW(5), LW(6))
        ADD_FLOAT(LW(5), LW(3))
        CALL(evt_npc_set_position, STRING("店員"), LW(4), LW(5), LW(6))
        CALL(evt_npc_get_position, STRING("宿店員"), LW(4), LW(5), LW(6))
        ADD_FLOAT(LW(5), LW(3))
        CALL(evt_npc_set_position, STRING("宿店員"), LW(4), LW(5), LW(6))
        CALL(evt_hit_bind_update, STRING("A_a1"))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        SET_FLOAT_VALUE(LW(2), LW(0))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_snd_sfxoff, LW(10))
    RETURN
EVT_END()

EVT_BEGIN(onryo_event)
    BEGIN_INLINE_EVENT
        CALL(evt_map_anim_rate)
    END_INLINE_EVENT
    SPAWN_EVENT(marco_ship_yure)
    SPAWN_EVENT(nannpc_mode_setup)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_b"), 1)
    CALL(evt_hitobj_onoff, STRING("A_b"), 1, 0)
    IF_INT_EQUAL(GSW(0), 230)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_mario_cont_onoff, 0)
        CALL(evt_party_cont_onoff, 0, 0)
        CALL(evt_mario_set_pos, 5, 90, -100)
        CALL(evt_party_set_pos, 0, 0, -1000, 0)
        GOTO(100)
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_cam3d_evt_set, 0, 600, 2155, 0, 350, -108, 0, 11)
    CALL(evt_map_playanim, STRING("yoru_hiru"), 2, 0)
    CALL(evt_seq_wait, 2)
    WAIT_MS(4000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_00"), 0, 0)
    CALL_CHILD(hiru_yoru_event)
    WAIT_MS(3000)
    CALL_CHILD(yoru_hiru_event)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_28"), 0, 0)
    CALL_CHILD(hiru_yoru_event)
    WAIT_MS(3000)
    CALL_CHILD(yoru_hiru_event)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_29"), 0, 0)
    CALL_CHILD(hiru_yoru_event)
    WAIT_MS(1000)
    SET_VALUE(GSW(0), 230)
    LABEL(100)
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 5, 90, -100)
    CALL(evt_mario_set_dir, 90, 0, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_a1"), 2)
    CALL(evt_npc_set_position, STRING("マルコ"), 116, 116, -100)
    CALL(evt_npc_set_ry, STRING("マルコ"), 90)
    CALL(evt_npc_set_position, STRING("オドオド水夫"), 25, 90, -163)
    CALL(evt_npc_set_ry, STRING("オドオド水夫"), 90)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), 21, 90, -55)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
    CALL(evt_npc_set_position, STRING("サンダース"), -11, 90, -163)
    CALL(evt_npc_set_ry, STRING("サンダース"), 90)
    CALL(evt_npc_set_position, STRING("めがね水夫"), -15, 90, -40)
    CALL(evt_npc_set_ry, STRING("めがね水夫"), 90)
    CALL(evt_npc_set_position, STRING("水夫A"), -65, 114, -145)
    CALL(evt_npc_set_ry, STRING("水夫A"), 90)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫A"), 1024)
    CALL(evt_npc_set_stay_anim, STRING("水夫A"))
    CALL(evt_npc_set_position, STRING("水夫ボム兵"), -65, 114, -44)
    CALL(evt_npc_set_ry, STRING("水夫ボム兵"), 90)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫ボム兵"), 1024)
    CALL(evt_npc_set_stay_anim, STRING("水夫ボム兵"))
    CALL(evt_npc_set_position, STRING("店員"), -115, 163, -153)
    CALL(evt_npc_set_ry, STRING("店員"), 90)
    CALL(evt_npc_flag_onoff, 0, STRING("店員"), 1024)
    CALL(evt_npc_set_stay_anim, STRING("店員"))
    CALL(evt_npc_set_position, STRING("宿店員"), -119, 163, -62)
    CALL(evt_npc_set_ry, STRING("宿店員"), 90)
    CALL(evt_npc_flag_onoff, 0, STRING("宿店員"), 1024)
    CALL(evt_npc_set_stay_anim, STRING("宿店員"))
    CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("マルコ"), 1073743392)
    CALL(evt_npc_flag_onoff, 1, STRING("マルコ"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("オドオド水夫"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("ガラの悪い水夫"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("サンダース"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("めがね水夫"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫A"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫ボム兵"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("店員"), 32768)
    CALL(evt_npc_flag_onoff, 1, STRING("宿店員"), 32768)
    CALL(evt_npc_sound_data_set, STRING("マルコ"), STRING("SFX_STG5_MARCO_MOVE1L"), STRING("SFX_STG5_MARCO_MOVE1R"), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(evt_cam3d_evt_set, -10, 340, 558, -11, 131, -108, 0, 11)
    WAIT_MS(1000)
    CALL(evt_npc_flag_onoff, 0, STRING("マルコ"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("オドオド水夫"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("ガラの悪い水夫"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("サンダース"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("めがね水夫"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫A"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("水夫ボム兵"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("店員"), 32768)
    CALL(evt_npc_flag_onoff, 0, STRING("宿店員"), 32768)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_sea_01"), 0, STRING("マルコ"))
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 480, 352, 214, 58, 150, -96, 0, 11)
    WAIT_MS(1500)
    CALL(evt_npc_set_ry, STRING("マルコ"), 270)
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("マルコ"), 107, -100, 0, FLOAT(60.0f), 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_02"), 0, STRING("マルコ"))
    CALL(evt_cam3d_evt_set, 0, 300, 985, 0, 175, -108, 0, 11)
    CALL(evt_snd_bgmoff, 26624)
    SET_VALUE(GW(0), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("マルコ"), 512)
    CALL(evt_npc_set_anim, STRING("マルコ"), STRING("S_2"))
    BEGIN_INLINE_EVENT
        CALL(evt_msg_print, 0, STRING("small_01"), 0, STRING("マルコ"))
        LOOP(0)
            IF_INT_EQUAL(GW(0), 1)
                BREAK_LOOP
            END_IF
            CALL(evt_msg_repeat)
        END_LOOP
    END_INLINE_EVENT
    CALL(evt_set_fune_anim_rate, FLOAT(0.7f))
    CALL(evt_set_nami_anim_rate, FLOAT(0.7f))
    WAIT_MS(1000)
    WAIT_MS(1500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_03"), 0, STRING("ガラの悪い水夫"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_04"), 0, STRING("オドオド水夫"))
    CALL(marcoship_stop_func)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmoff_f, 512, 2000)
        WAIT_MS(2000)
        CALL(evt_snd_bgmon_f, 768, STRING("BGM_EVT_STG5_GHOST1"), 2000)
        CALL(evt_snd_envoff, 18432)
    END_INLINE_EVENT
    CALL(evt_set_fune_anim_rate, FLOAT(0.4f))
    CALL(evt_set_nami_anim_rate, FLOAT(0.4f))
    CALL(evt_msg_print_add, 0, STRING("stg5_sea_04_01"))
    CALL(evt_cam3d_evt_set, 0, 295, 370, 0, 145, -108, 0, 11)
    SPAWN_EVENT(nami_alpha)
    CALL(evt_set_fune_anim_rate, FLOAT(0.0f))
    CALL(evt_set_nami_anim_rate, FLOAT(0.1f))
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("S_fne_nami"), 1)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_05"), 0, STRING("ガラの悪い水夫"))
    WAIT_MS(1000)
    SET_VALUE(GW(0), 1)
    CALL(evt_npc_flag_onoff, 1, STRING("マルコ"), 512)
    CALL(evt_msg_close, STRING("small_01"))
    CALL(evt_npc_set_anim, STRING("マルコ"), STRING("T_1"))
    WAIT_MS(2000)
    CALL(evt_npc_set_anim, STRING("マルコ"), STRING("S_1"))
    CALL(marcoship_stop_wait_func)
    WAIT_MS(500)
    CALL(evt_npc_move_position, STRING("マルコ"), 68, -71, 0, FLOAT(120.0f), 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_06"), 0, STRING("マルコ"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_07"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_cam3d_evt_set, -50, 470, 588, -50, 390, -107, 500, 11)
    WAIT_MS(500)
    CALL(evt_npc_set_anim, STRING("ガラの悪い水夫"), STRING("R_1"))
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -12, 90, -80)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 270)
    CALL(evt_npc_glide_position, STRING("ガラの悪い水夫"), -52, 315, -75, 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_npc_set_anim, STRING("ガラの悪い水夫"), STRING("S_1"))
    WAIT_MS(500)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 270)
    CALL(evt_npc_set_anim, STRING("ガラの悪い水夫"), STRING("R_1"))
    CALL(evt_npc_glide_position, STRING("ガラの悪い水夫"), -78, 315, -79, 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_npc_set_anim, STRING("ガラの悪い水夫"), STRING("S_1"))
    WAIT_MS(1500)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -23, 315, -79)
    CALL(evt_npc_set_rotate, STRING("ガラの悪い水夫"), 0, 180, 0)
    WAIT_MS(1200)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -78, 315, -79)
    CALL(evt_npc_set_rotate, STRING("ガラの悪い水夫"), 0, 0, 0)
    WAIT_MS(1800)
    CALL(evt_npc_set_position, STRING("ガラの悪い水夫"), -23, 315, -79)
    CALL(evt_npc_set_rotate, STRING("ガラの悪い水夫"), 0, 180, 0)
    WAIT_MS(1000)
    CALL(evt_npc_set_position, STRING("エルモスA"), -45, 336, -73)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_APPEAR1_2"), -45, 336, -73, 0)
    CALL(evt_npc_set_ry, STRING("エルモスA"), 90)
    WAIT_MS(1000)
    CALL(evt_npc_get_position, STRING("エルモスA"), LW(10), LW(11), LW(12))
    SET_VALUE(LW(13), LW(10))
    SET_VALUE(LW(14), LW(11))
    SET_VALUE(LW(15), LW(12))
    ADD(LW(13), 5)
    ADD(LW(14), -5)
    ADD(LW(15), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_MOVE1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(13), LW(14), LW(15), 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(10), LW(11), LW(12), 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_MOVE1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(13), LW(14), LW(15), 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(10), LW(11), LW(12), 0, FLOAT(120.0f), 0, 0, 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_08"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_npc_set_position, STRING("エルモスA"), 0, -1000, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 0, 180, 0, 200)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_npc_set_rotate, STRING("ガラの悪い水夫"), 0, LW(0), 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    WAIT_MS(200)
    SPAWN_EVENT(moya_effect)
    WAIT_MS(1500)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ガラの悪い水夫"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_NPC_QUESTION_MARK1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 1, 0, LW(0), LW(1), LW(2), 30, 0, 90)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
    WAIT_MS(500)
    CALL(evt_npc_set_position, STRING("エルモスA"), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_APPEAR1_2"), LW(10), LW(11), LW(12), 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_MOVE1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(13), LW(14), LW(15), 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(10), LW(11), LW(12), 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_MOVE1"), LW(10), LW(11), LW(12), 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(13), LW(14), LW(15), 0, FLOAT(120.0f), 0, 0, 0)
    CALL(evt_npc_glide_position, STRING("エルモスA"), LW(10), LW(11), LW(12), 0, FLOAT(120.0f), 0, 0, 0)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_09"), 0, STRING("ガラの悪い水夫"))
    WAIT_MS(1000)
    CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 270)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, -50, 412, 340, -50, 360, -108, 100, 11)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("ガラの悪い水夫"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_NPC_BIKKURI_MARK1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 0, 0, LW(0), LW(1), LW(2), 30, 0, 60)
    END_INLINE_EVENT
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_DANGER2"))
    END_INLINE_EVENT
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        LOOP(5)
            CALL(evt_cam3d_evt_set, -50, 412, 340, -50, 360, -108, 100, 11)
            WAIT_MS(100)
            CALL(evt_cam3d_evt_set, -50, 423, 438, -50, 360, -108, 100, 11)
            WAIT_MS(100)
        END_LOOP
        CALL(evt_cam3d_evt_set, -50, 412, 340, -50, 360, -108, 100, 11)
        WAIT_MS(100)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    CALL(evt_msg_print, 0, STRING("stg5_sea_11"), 0, STRING("ガラの悪い水夫"))
    WAIT_WHILE(GF(0))
    CALL(evt_npc_set_anim, STRING("ガラの悪い水夫"), STRING("R_1"))
    CALL(muj_npc_guruguru, STRING("ガラの悪い水夫"), -52, -90, 0, FLOAT(-12.0f), 3000)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 270)
        CALL(evt_npc_glide_position, STRING("ガラの悪い水夫"), -52, 315, -70, 0, FLOAT(180.0f), 0, 0, 0)
        CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
        CALL(evt_npc_glide_position, STRING("ガラの悪い水夫"), -43, 90, -72, 0, FLOAT(180.0f), 0, 0, 0)
        CALL(evt_npc_set_ry, STRING("ガラの悪い水夫"), 90)
        CALL(evt_npc_glide_position, STRING("ガラの悪い水夫"), 20, 90, -55, 0, FLOAT(180.0f), 0, 0, 0)
        CALL(evt_npc_jump_position_nohit, STRING("ガラの悪い水夫"), 20, 90, -55, 100, 30)
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, -10, 340, 558, -11, 131, -108, 1000, 11)
    WAIT_WHILE(GF(0))
    CALL(evt_msg_print, 0, STRING("stg5_sea_12"), 0, STRING("ガラの悪い水夫"))
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_sea_13"), 0, STRING("マルコ"))
    WAIT_MS(500)
    SPAWN_EVENT(mist_setup)
    CALL(evt_npc_set_position, STRING("エルモスA"), 168, 160, -84)
    CALL(evt_npc_set_position, STRING("エルモスB"), 105, 100, -200)
    CALL(evt_npc_set_position, STRING("エルモスC"), 10, 110, -200)
    CALL(evt_npc_set_position, STRING("エルモスD"), -70, 105, -200)
    CALL(evt_npc_set_position, STRING("エルモスE"), 125, 105, 0)
    CALL(evt_npc_set_position, STRING("エルモスF"), 10, 100, 0)
    CALL(evt_npc_set_position, STRING("エルモスG"), -80, 110, 0)
    CALL(evt_npc_set_ry, STRING("エルモスA"), 270)
    CALL(evt_npc_set_ry, STRING("エルモスB"), 270)
    CALL(evt_npc_set_ry, STRING("エルモスC"), 270)
    CALL(evt_npc_set_ry, STRING("エルモスD"), 90)
    CALL(evt_npc_set_ry, STRING("エルモスE"), 270)
    CALL(evt_npc_set_ry, STRING("エルモスF"), 270)
    CALL(evt_npc_set_ry, STRING("エルモスG"), 90)
    CALL(evt_npc_set_color, STRING("エルモスA"), 255, 255, 255, 0)
    CALL(evt_npc_set_color, STRING("エルモスB"), 255, 255, 255, 0)
    CALL(evt_npc_set_color, STRING("エルモスC"), 255, 255, 255, 0)
    CALL(evt_npc_set_color, STRING("エルモスD"), 255, 255, 255, 0)
    CALL(evt_npc_set_color, STRING("エルモスE"), 255, 255, 255, 0)
    CALL(evt_npc_set_color, STRING("エルモスF"), 255, 255, 255, 0)
    CALL(evt_npc_set_color, STRING("エルモスG"), 255, 255, 255, 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_ELMOS_APPEAR1_2"), 0)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_npc_set_color, STRING("エルモスA"), 255, 255, 255, LW(0))
        CALL(evt_npc_set_color, STRING("エルモスB"), 255, 255, 255, LW(0))
        CALL(evt_npc_set_color, STRING("エルモスC"), 255, 255, 255, LW(0))
        CALL(evt_npc_set_color, STRING("エルモスD"), 255, 255, 255, LW(0))
        CALL(evt_npc_set_color, STRING("エルモスE"), 255, 255, 255, LW(0))
        CALL(evt_npc_set_color, STRING("エルモスF"), 255, 255, 255, LW(0))
        CALL(evt_npc_set_color, STRING("エルモスG"), 255, 255, 255, LW(0))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_MS(500)
    SET_VALUE(LW(0), STRING("エルモスA"))
    SPAWN_EVENT_GET_ID(PTR(fuwafuwa), GW(1))
    SET_VALUE(LW(0), STRING("エルモスB"))
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -20)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(5), 850)
    SPAWN_EVENT(urouro_fuwafuwa)
    SET_VALUE(LW(0), STRING("エルモスC"))
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -25)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(5), 1000)
    SPAWN_EVENT(urouro_fuwafuwa)
    SET_VALUE(LW(0), STRING("エルモスD"))
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -22)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(5), 900)
    SPAWN_EVENT(urouro_fuwafuwa)
    SET_VALUE(LW(0), STRING("エルモスE"))
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -30)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(5), 1050)
    SPAWN_EVENT(urouro_fuwafuwa)
    SET_VALUE(LW(0), STRING("エルモスF"))
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -20)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(5), 950)
    SPAWN_EVENT(urouro_fuwafuwa)
    SET_VALUE(LW(0), STRING("エルモスG"))
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -24)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(5), 900)
    SPAWN_EVENT(urouro_fuwafuwa)
    CALL(evt_cam3d_evt_set, 2, 700, 1614, -1, 261, -108, 0, 11)
    CALL(evt_npc_set_position, STRING("マルコ"), 116, 116, -100)
    WAIT_MS(500)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_ELMOS_VOICE1"), 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_KORTESS_VOICE1"), 0)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_14"), 0, 0)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 0, 358, 572, -1, 145, -108, 0, 11)
    WAIT_MS(1000)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_15"), 0, 0)
    WAIT_MS(500)
    SET_VALUE(LW(0), STRING("オドオド水夫"))
    CALL(evt_npc_set_anim, LW(0), STRING("R_2"))
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -20)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 300)
    SPAWN_EVENT(urouro)
    SET_VALUE(LW(0), STRING("ガラの悪い水夫"))
    CALL(evt_npc_set_run_anim, LW(0))
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -25)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 330)
    SPAWN_EVENT(urouro)
    SET_VALUE(LW(0), STRING("サンダース"))
    CALL(evt_npc_set_run_anim, LW(0))
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -30)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 350)
    SPAWN_EVENT_GET_ID(PTR(urouro), GW(2))
    SET_VALUE(LW(0), STRING("めがね水夫"))
    CALL(evt_npc_set_run_anim, LW(0))
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -22)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 340)
    SPAWN_EVENT_GET_ID(PTR(urouro), GW(3))
    SET_VALUE(LW(0), STRING("店員"))
    CALL(evt_npc_set_run_anim, LW(0))
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -22)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 300)
    SPAWN_EVENT(urouro)
    SET_VALUE(LW(0), STRING("宿店員"))
    CALL(evt_npc_set_run_anim, LW(0))
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -24)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 340)
    SPAWN_EVENT(urouro)
    SET_VALUE(LW(0), STRING("水夫A"))
    SPAWN_EVENT(buruburu)
    SET_VALUE(LW(0), STRING("水夫ボム兵"))
    SPAWN_EVENT(buruburu)
    CALL(evt_npc_flag_onoff, 0, STRING("マルコ"), 1536)
    CALL(evt_npc_set_anim, STRING("マルコ"), STRING("O_1"))
    CALL(evt_mario_set_pose, STRING("M_I_O"))
    CALL(evt_npc_flag_onoff, 0, STRING("ガラの悪い水夫"), 512)
    CALL(evt_npc_flag_onoff, 0, STRING("オドオド水夫"), 512)
    CALL(evt_msg_print, 0, STRING("stg5_sea_16"), 0, STRING("ガラの悪い水夫"))
    CALL(evt_msg_print, 0, STRING("stg5_sea_17"), 0, STRING("オドオド水夫"))
    CALL(evt_cam3d_evt_set, 115, 300, 323, 115, 165, -105, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_sea_18"), 0, STRING("マルコ"))
    CALL(elmos1000tai_start)
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL(evt_snd_sfxon, STRING("SFX_STG5_ELMOS_APPEAR2"), 0)
    END_INLINE_EVENT
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_KORTESS_VOICE1"), 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_19"), 0, 0)
    DELETE_EVENT(GW(1))
    SET_VALUE(GF(10), 1)
    SET_VALUE(GF(11), 1)
    SPAWN_EVENT(elmos_a_kakushuku)
    WAIT_MS(500)
    CALL(evt_npc_set_position, STRING("エルモスA"), 80, 126, -100)
    CALL(evt_npc_set_ry, STRING("エルモスA"), 90)
    WAIT_MS(500)
    WAIT_MS(1000)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_get_position, STRING("マルコ"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_NPC_BIKKURI_MARK1"), LW(0), LW(1), LW(2), 0)
        ADD(LW(1), 20)
        CALL(evt_eff_fukidashi, 2, STRING(""), 0, 0, 0, LW(0), LW(1), LW(2), 30, 0, 90)
    END_INLINE_EVENT
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_sea_20"), 0, STRING("マルコ"))
    CALL(evt_npc_set_anim, STRING("マルコ"), STRING("B_1"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_sea_21"), 0, STRING("マルコ"))
    SET_VALUE(GF(10), 0)
    WAIT_WHILE(GF(11))
    BEGIN_INLINE_EVENT
        CALL(evt_snd_sfxon, STRING("SFX_STG5_SHIP_QUAKE1"), LW(10))
        CALL(evt_cam_shake, 4, FLOAT(0.002f), FLOAT(0.0f), 200)
        CALL(evt_cam_shake, 4, FLOAT(0.004f), FLOAT(0.0f), 200)
        CALL(evt_cam_shake, 4, FLOAT(0.006f), FLOAT(0.0f), 200)
        CALL(evt_cam_shake, 4, FLOAT(0.008f), FLOAT(0.0f), 200)
        CALL(evt_snd_sfxoff, LW(10))
        CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 0)
    END_INLINE_EVENT
    CALL_CHILD(elmos_a_shuku)
    CALL(evt_npc_set_position, STRING("エルモスA"), 168, 160, -84)
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_ELMOS_APPEAR1"), 168, 160, -84, 0)
    CALL(evt_npc_set_ry, STRING("エルモスA"), 270)
    WAIT_MS(300)
    CALL_CHILD(elmos_a_kaku)
    SET_VALUE(LW(0), STRING("エルモスA"))
    SPAWN_EVENT(fuwafuwa)
    SET_VALUE(LW(0), STRING("マルコ"))
    CALL(evt_npc_set_anim, LW(0), STRING("R_2"))
    CALL(evt_npc_flag_onoff, 0, LW(0), 1024)
    CALL(evt_npc_flag_onoff, 1, LW(0), 1073741856)
    CALL(evt_npc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(1), -20)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 340)
    SPAWN_EVENT_GET_ID(PTR(urouro), GW(0))
    CALL(evt_cam3d_evt_set, 0, 358, 572, -1, 145, -108, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_sea_22"), 0, STRING("サンダース"))
    CALL(evt_msg_print, 0, STRING("stg5_sea_23"), 0, STRING("サンダース"))
    CALL(evt_msg_print, 0, STRING("stg5_sea_24"), 0, STRING("サンダース"))
    DELETE_EVENT(GW(2))
    SET_VALUE(LW(10), STRING("サンダース"))
    SPAWN_EVENT(kirimomi)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_npc_get_position, STRING("サンダース"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_NPC_FALL1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("サンダース"), -20, -100, -300, 600, 100)
    CALL(evt_eff, 0, STRING("splash"), 0, -20, 30, -150, FLOAT(1.5f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_25"), 0, STRING("めがね水夫"))
    DELETE_EVENT(GW(3))
    SET_VALUE(LW(10), STRING("めがね水夫"))
    SPAWN_EVENT(kirimomi)
    BEGIN_INLINE_EVENT
        WAIT_MS(400)
        CALL(evt_npc_get_position, STRING("めがね水夫"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_NPC_FALL1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_npc_jump_position_nohit, STRING("めがね水夫"), -30, -100, 100, 600, 100)
    CALL(evt_eff, 0, STRING("splash"), 0, -30, 30, 0, FLOAT(1.5f), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_msg_print, 0, STRING("stg5_sea_26"), 0, STRING("マルコ"))
    BEGIN_INLINE_EVENT
        DELETE_EVENT(GW(0))
        CALL(evt_npc_set_ry, STRING("マルコ"), 270)
        CALL(evt_npc_jump_position_nohit, STRING("マルコ"), 20, 90, -90, 500, 50)
    END_INLINE_EVENT
    WAIT_MS(500)
    CALL(evt_mapobj_get_position, STRING("S_a1"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_SHIP_SINK1"), LW(0), LW(1), LW(2), LW(10))
    SPAWN_EVENT(marcoship_shizumu)
    WAIT_MS(4500)
    CALL(evt_fade_entry, 2, 500, 0, 0, 0)
    CALL(evt_fade_end_wait)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(evt_snd_bgmoff, 512)
    SET_VALUE(GSW(0), 231)
    CALL(evt_fade_set_mapchange_type, 0, 10, 0, 9, 300)
    CALL(evt_bero_mapchange, STRING("muj_00"), STRING(""))
    RETURN
EVT_END()

EVT_BEGIN(muj_20_koopa_evt)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_npc_set_position, STRING("飛クッパ"), 0, 500, -700)
    CALL(evt_npc_set_rotate, STRING("飛クッパ"), 0, 0, 180)
    CALL(evt_npc_set_anim, STRING("飛クッパ"), STRING("HKPA_O_3"))
    CALL(evt_cam3d_evt_set, 0, 322, 480, 0, 218, -113, 0, 11)
    CALL(evt_seq_wait, 2)
    WAIT_MS(2000)
    SET_VALUE(GF(0), 0)
    CALL(evt_snd_sfxon, STRING("SFX_KUPPA_AIRSHIP_FALL1"), GW(0))
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_get_position, STRING("飛クッパ"), LW(0), LW(1), LW(2))
            IF_INT_LESS_OR_EQUAL(LW(1), 50)
                CALL(evt_snd_sfxoff, GW(0))
                CALL(evt_snd_sfxon, STRING("SFX_KUPPA_AIRSHIP_DOWN_SEA1"), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_npc_get_position, STRING("飛クッパ"), LW(0), LW(1), LW(2))
            IF_INT_LESS_OR_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(splash_eff, LW(0), LW(1), LW(2))
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 1, 500, -100, 1000)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_npc_set_position, STRING("飛クッパ"), 0, LW(0), -700)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    SET_VALUE(GF(0), 1)
    WAIT_MS(2000)
    SET_VALUE(GSW(0), 216)
    CALL(evt_bero_mapchange, STRING("kpa_03"), STRING(""))
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(kaizoku_hata)
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 11, -4000, 4000, 2500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(evt_mapobj_rotate, 1, STRING("S_b_02_hata"), 0, LW(0), 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(500)
        CALL(evt_sub_intpl_msec_init, 11, 4000, -4000, 2500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(evt_mapobj_rotate, 1, STRING("S_b_02_hata"), 0, LW(0), 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(500)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(gor_muj_ship)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    CALL(evt_cam3d_evt_set, 0, 600, 2155, 0, 350, -108, 0, 11)
    CALL(evt_map_playanim, STRING("yoru_hiru"), 2, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a1"), 1)
    CALL(evt_mapobj_trans, 1, STRING("S_b"), -1000, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_b"), 0, 0, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), -1000, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_umi_l_siro_b"), 0, 0, 0)
    WAIT_FRAMES(1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_umi_l_siro_b"), 1)
    BEGIN_INLINE_EVENT
        CALL(kaizoku_fune_eff)
    END_INLINE_EVENT
    CALL(evt_seq_wait, 2)
    SPAWN_EVENT(kaizoku_hata)
    BEGIN_INLINE_EVENT
        WAIT_MS(3500)
        CALL_CHILD(hiru_yoru_event)
        WAIT_MS(3500)
        CALL_CHILD(yoru_hiru_event)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_key_get_buttontrg, 0, LW(0))
            IF_INT_EQUAL(LW(0), 256)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_bero_mapchange, STRING("muj_00"), STRING(""))
    END_INLINE_EVENT
    SET_RAW_VALUE(LW(0), GW(1))
    SET_FLOAT_VALUE(LW(1), FLOAT(5.0f))
    SPAWN_EVENT(muj_ship_puka)
    CALL(evt_sub_intpl_msec_init, 0, -1000, 1000, 15000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, STRING("S_b"), LW(0), GW(1), 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_b"), 0, 0, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), LW(0), 0, 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_umi_l_siro_b"), 0, 0, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_bero_mapchange, STRING("muj_00"), STRING(""))
    RETURN
EVT_END()

static EVT_BEGIN(muj_gor_ship)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_party_cont_onoff, 1, 0)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_party_set_pos, 1, 0, -1000, 0)
    CALL(evt_cam3d_evt_set, 0, 600, 2155, 0, 350, -108, 0, 11)
    CALL(evt_map_playanim, STRING("yoru_hiru"), 2, 0)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a1"), 1)
    CALL(evt_mapobj_trans, 1, STRING("S_b"), -1000, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_b"), 0, 180, 0)
    CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), -1000, 0, 0)
    CALL(evt_mapobj_rotate, 1, STRING("S_umi_l_siro_b"), 0, 180, 0)
    WAIT_FRAMES(1)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_umi_l_siro_b"), 1)
    BEGIN_INLINE_EVENT
        CALL(kaizoku_fune_eff)
    END_INLINE_EVENT
    CALL(evt_seq_wait, 2)
    SPAWN_EVENT(kaizoku_hata)
    BEGIN_INLINE_EVENT
        WAIT_MS(3500)
        CALL_CHILD(hiru_yoru_event)
        WAIT_MS(3500)
        CALL_CHILD(yoru_hiru_event)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_key_get_buttontrg, 0, LW(0))
            IF_INT_EQUAL(LW(0), 256)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_bero_mapchange, STRING("gor_00"), STRING(""))
    END_INLINE_EVENT
    SET_RAW_VALUE(LW(0), GW(1))
    SET_FLOAT_VALUE(LW(1), FLOAT(5.0f))
    SPAWN_EVENT(muj_ship_puka)
    CALL(evt_sub_intpl_msec_init, 0, 1000, -1000, 15000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, STRING("S_b"), LW(0), GW(1), 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_b"), 0, 180, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), LW(0), 0, 0)
        CALL(evt_mapobj_rotate, 1, STRING("S_umi_l_siro_b"), 0, 180, 0)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_bero_mapchange, STRING("gor_00"), STRING(""))
    RETURN
EVT_END()

EVT_BEGIN(anm_kaji)
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 11, 500, -500, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(evt_mapobj_rotate, 1, STRING("S_f_drin"), LW(0), 0, 0)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 11, -500, 500, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(evt_mapobj_rotate, 1, STRING("S_f_drin"), LW(0), 0, 0)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(anm_wakka)
    SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
    LOOP(0)
        SET_FLOAT_VALUE(LW(0), FLOAT(2.0f))
        MULTIPLY_FLOAT(LW(0), GW(10))
        ADD_FLOAT(LW(1), LW(0))
        CALL(evt_mapobj_rotate, 1, STRING("S_f_wa_l"), 0, 0, LW(1))
        CALL(evt_mapobj_rotate, 1, STRING("S_f_wa_r"), 0, 0, LW(1))
        IF_FLOAT_GREATER(LW(1), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(1), FLOAT(360.0f))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(anm_entotsu)
    SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
    LOOP(0)
        SET_FLOAT_VALUE(LW(0), FLOAT(2.0f))
        MULTIPLY_FLOAT(LW(0), GW(10))
        ADD_FLOAT(LW(1), LW(0))
        CALL(evt_mapobj_rotate, 1, STRING("S_f_en01"), 0, LW(1), 0)
        IF_FLOAT_GREATER(LW(1), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(1), FLOAT(360.0f))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(anm_neji)
    SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
    LOOP(0)
        SET_FLOAT_VALUE(LW(0), FLOAT(2.0f))
        MULTIPLY_FLOAT(LW(0), GW(10))
        ADD_FLOAT(LW(1), LW(0))
        CALL(evt_mapobj_rotate, 1, STRING("S_skr"), LW(1), 0, 0)
        IF_FLOAT_GREATER(LW(1), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(1), FLOAT(360.0f))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

USER_FUNC(unk_muj_00004328) {
    Mtx m1;
    Mtx m2;
    Mtx m3;
    Vec pos;
    f32 x;
    f32 y;
    f32 z;
    f32 speed;
    f32 angle;
    x = evtGetFloat(event, GW(0));
    y = evtGetFloat(event, GW(1));
    z = evtGetFloat(event, GW(2));
    speed = evtGetFloat(event, LW(0));
    {
        Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
        temp.x = x;
        temp.y = y;
        temp.z = z;
        pos = temp;
    }
    angle = 5.0f * x / -6000.0f;
    PSMTXTrans(m1, 0.0f, -6000.0f, 0.0f);
    PSMTXRotRad(m2, 'z', 0.017453292f * angle);
    PSMTXConcat(m1, m2, m3);
    PSMTXTrans(m1, 0.0f, 6000.0f, 0.0f);
    PSMTXConcat(m3, m1, m3);
    PSMTXMultVec(m3, &pos, &pos);
    evtSetFloat(event, LW(7), pos.x);
    evtSetFloat(event, LW(8), pos.y);
    evtSetFloat(event, LW(9), pos.z);
    evtSetFloat(event, LW(0), speed + angle);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(anm_yure)
    CALL(evt_mapobj_trans, 1, STRING("S_fune_m"), GW(0), GW(1), GW(2))
    CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), GW(0), GW(1), GW(2))
    WAIT_MS(1500)
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_OPN_SHIP_MOVE1"), GW(0), GW(1), GW(2), GW(4))
    CALL(evt_snd_sfx_dist, GW(4), 60)
    LOOP(0)
        CALL(evt_sub_intpl_msec_init, 11, 500, -500, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(unk_muj_00004328)
            CALL(evt_mapobj_trans, 1, STRING("S_fune_m"), LW(7), LW(8), LW(9))
            CALL(evt_mapobj_rotate, 1, STRING("S_fune_m"), 0, 180, LW(0))
            CALL(evt_snd_sfx_pos, GW(4), LW(7), LW(8), LW(9))
            CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), LW(7), LW(8), LW(9))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_sub_intpl_msec_init, 11, -500, 500, 2000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            DIVIDE_FLOAT(LW(0), FLOAT(100.0f))
            CALL(unk_muj_00004328)
            CALL(evt_mapobj_trans, 1, STRING("S_fune_m"), LW(7), LW(8), LW(9))
            CALL(evt_mapobj_rotate, 1, STRING("S_fune_m"), 0, 180, LW(0))
            CALL(evt_snd_sfx_pos, GW(4), LW(7), LW(8), LW(9))
            CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), LW(7), LW(8), LW(9))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

static void pressStartGX(LogoWork* work, s32 alpha, f32 x, f32 y) {
    CameraEntry* camera = camGetCurPtr();
    GXTexObj startText, gradient;
    Mtx v17, v16;

    TEXGetGXTexObjFromPalette(*work->tpl->data, &startText, 8);
    GXLoadTexObj(&startText, GX_TEXMAP0);
    TEXGetGXTexObjFromPalette(*work->tpl->data, &gradient, 9);
    GXLoadTexObj(&gradient, GX_TEXMAP1);
    GXSetNumTevStages(2);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
    GXSetTevColor(GX_TEVREG0, (GXColor){ 0x00, 0x00, 0x00, alpha });
    GXSetNumChans(0);
    GXSetNumTexGens(2);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 60);
    GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, 30);
    MTXScale(v16, 1.0f, 1.0f, 1.0f);
    MTXTrans(v17, (f32)gp->retraceCount / -200.0f, 0.0f, 0.0f);
    MTXConcat(v16, v17, v17);
    GXLoadTexMtxImm(v17, 30, GX_MTX2x4);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZMode(GX_DISABLE, GX_LEQUAL, GX_DISABLE);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    MTXTrans(v17, x, y, 0.0f);
    MTXScale(v16, (f32)GXGetTexObjWidth(&startText), (f32)GXGetTexObjHeight(&startText), 1.0f);
    MTXConcat(v17, v16, v17);
    MTXConcat(camera->view, v17, v17);
    GXLoadPosMtxImm(v17, 0);
    GXSetCurrentMtx(0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(-0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
}

static void draw(CameraId cameraId, void* param) {
    LogoWork* work = param;
    CameraEntry* camera;
    f32 viewport[GX_VIEWPORT_SZ];
    f32 projection[GX_PROJECTION_SZ];
    CameraEntry camBackup;

    GXGetViewportv(viewport);
    GXGetProjectionv(projection);
    camera = camGetPtr(CAMERA_2D);
    GXSetProjection(camGetPtr(CAMERA_2D)->projection, camera->projType);
    camBackup = *camGetCurPtr();
    *camGetCurPtr() = *camGetPtr(CAMERA_2D);
    winTexInit(*work->tpl->data);
    unk_80177008(5, (Vec){ 0.0f, 100.0f + work->offsetY, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f },
                 (GXColor){ 0xFF, 0xFF, 0xFF, work->alpha });
    *camGetCurPtr() = camBackup;
    GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
    GXSetProjectionv(projection);
}

USER_FUNC(logo) {
    LogoWork* work;

    if (isFirstCall) {
        work = _mapAlloc(0x14);
        memset(work, 0, 0x14);
        event->userdata[0] = (s32)work;
    }
    work = (LogoWork*)event->userdata[0];
    if (fileAsyncf(FILE_TYPE_TPL, NULL, "%s/mariost.tpl", getMarioStDvdRoot()) == 0) {
        return EVT_RETURN_BLOCK;
    }
    switch (work->state) {
    case 0:
        work->tpl = fileAllocf(FILE_TYPE_TPL, "%s/mariost.tpl", getMarioStDvdRoot());
        work->alpha = 0;
        work->offsetY = 0.0f;
        work->timer = 0;
        work->state++;
        break;
    case 1:
        if (work->timer < 180) {
            work->alpha = (s32)intplGetValue(11, ++work->timer, 180, 0.0f, 255.0f);
            work->offsetY = intplGetValue(11, ++work->timer, 180, -50.0f, 0.0f);
        } else if (gp->retraceCount % 15 == 0) {
            work->state++;
        }
        break;
    case 2:
        if (evtGetValue(event, GF(0)) != 0) {
            if (keyGetButtonTrg(0) & 0x1300) {
                work->timer = 0;
                work->state++;
                evtSetValue(event, GF(1), 1);
            }
        }
        break;
    case 3:
        if (work->timer < 120) {
            work->alpha = (s32)intplGetValue(11, ++work->timer, 120, 255.0f, 0.0f);
            work->offsetY = intplGetValue(11, ++work->timer, 120, 0.0f, 100.0f);
        } else {
            work->state++;
        }
        break;
    case 4:
        fileFree(work->tpl);
        work->tpl = NULL;
        _mapFree(work);
        evtSetValue(event, GF(2), 1);
        return EVT_RETURN_DONE;
    }
    if (work->tpl != NULL) {
        dispEntry(8, 1000.0f, 0, draw, work);
    }
    return EVT_RETURN_BLOCK;
}

static EVT_BEGIN(evt_prologue)
    WAIT_MS(500)
    CALL(evt_snd_bgmon, 512, STRING("BGM_OPENING1"))
    CALL(evt_snd_envon, 272, STRING("ENV_OPN_MUJ1"))
    WAIT_MS(3500)
    SET_VALUE(GF(0), 0)
    SET_VALUE(GF(1), 0)
    SET_VALUE(GF(2), 0)
    BEGIN_INLINE_EVENT
        CALL(logo)
    END_INLINE_EVENT
    SET_FLOAT_VALUE(GW(10), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(2), FLOAT(-1800.0f))
    CALL(evt_sub_intpl_msec_init, 11, -1800, 0, 10000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_FLOAT_VALUE(LW(3), LW(0))
        SUBTRACT_FLOAT(LW(3), LW(2))
        SET_FLOAT_VALUE(GW(10), LW(3))
        SET_FLOAT_VALUE(LW(2), LW(0))
        SET_FLOAT_VALUE(GW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        LOOP(5)
            WAIT_MS(1000)
            IF_INT_NOT_EQUAL(GF(1), 0)
                RETURN
            END_IF
        END_LOOP
        IF_INT_EQUAL(GF(1), 0)
            CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_SAILING1"))
        END_IF
    END_INLINE_EVENT
    LOOP(0)
        IF_INT_EQUAL(GF(1), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_snd_sfxon, STRING("SFX_PRESS_START1"), 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(4000)
        CALL(evt_snd_bgmoff_f, 512, 6000)
        CALL(evt_snd_envoff_f, 512, 6000)
        CALL(evt_fade_set_mapchange_type, 1, 17, 800, 16, 800)
        CALL(evt_bero_mapchange, STRING("sys_01"), STRING("prologue"))
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(5000)
        CALL(evt_snd_sfxoff, GW(4))
    END_INLINE_EVENT
    SET_FLOAT_VALUE(GW(10), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(2), FLOAT(0.0f))
    CALL(evt_sub_intpl_msec_init, 11, 0, 1800, 10000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        SET_FLOAT_VALUE(LW(3), LW(0))
        SUBTRACT_FLOAT(LW(3), LW(2))
        SET_FLOAT_VALUE(GW(10), LW(3))
        SET_FLOAT_VALUE(LW(2), LW(0))
        SET_FLOAT_VALUE(GW(0), LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    LOOP(0)
        IF_INT_EQUAL(GF(2), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_npc_entry)
    CALL(evt_npc_entry, STRING("コルテス"), STRING("c_korutesu3"))
    CALL(evt_npc_set_tribe, STRING("コルテス"), STRING("コルテス"))
    CALL(evt_npc_set_position, STRING("コルテス"), FLOAT(-830.0f), FLOAT(147.0f), FLOAT(-98.0f))
    CALL(evt_npc_set_ry, STRING("コルテス"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("コルテス"), 32768)
    RETURN
EVT_END()

EVT_BEGIN(fune_move)
    BEGIN_INLINE_EVENT
        CALL(kaizoku_fune_eff)
    END_INLINE_EVENT
    CALL(evt_mapobj_get_position, STRING("S_b"), LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_EVT_EPILOGUE_KOR_SHIP_MOVE1"), LW(0), LW(1), LW(2), LW(10))
    CALL(evt_snd_sfx_dist, LW(10), 30)
    SET_FLOAT_VALUE(LW(0), FLOAT(-700.0f))
    SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
    LOOP(0)
        ADD_FLOAT(LW(0), FLOAT(2.0f))
        CALL(evt_mapobj_trans, 1, STRING("S_b"), LW(0), 0, 0)
        CALL(evt_mapobj_get_position, STRING("S_b"), LW(3), LW(4), LW(5))
        CALL(evt_snd_sfx_pos, LW(10), LW(3), LW(4), LW(5))
        ADD_FLOAT(LW(1), FLOAT(1.0f))
        IF_INT_GREATER_OR_EQUAL(LW(1), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(1), FLOAT(360.0f))
        END_IF
        CALL(evt_sub_get_sincos, LW(1), LW(2), LW(3))
        MULTIPLY_FLOAT(LW(2), FLOAT(3.0f))
        CALL(evt_mapobj_rotate, 1, STRING("S_b"), 0, 0, LW(2))
        CALL(evt_mapobj_trans, 1, STRING("S_umi_l_siro_b"), LW(0), 0, 0)
        SET_FLOAT_VALUE(LW(2), LW(0))
        ADD_FLOAT(LW(2), FLOAT(130.0f))
        CALL(evt_npc_set_position, STRING("コルテス"), LW(2), 147, -98)
        SET_FLOAT_VALUE(LW(2), LW(0))
        ADD_FLOAT(LW(2), FLOAT(45.0f))
        CALL(evt_npc_set_position, STRING("サンダース"), LW(2), 90, -50)
        SET_FLOAT_VALUE(LW(2), LW(0))
        ADD_FLOAT(LW(2), FLOAT(17.0f))
        CALL(evt_npc_set_position, STRING("水夫A"), LW(2), 90, -169)
        SET_FLOAT_VALUE(LW(2), LW(0))
        ADD_FLOAT(LW(2), FLOAT(-106.0f))
        CALL(evt_npc_set_position, STRING("水夫ボム兵"), LW(2), 160, -18)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(epilogue_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_MUJ1"))
    CALL(evt_snd_bgm_scope, 0, 1)
    CALL(evt_snd_envon, 272, STRING("ENV_END_MUJ3"))
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_dispflag_onoff, 1, 1)
    CALL(evt_npc_set_position, STRING("サンダース"), FLOAT(-655.0f), FLOAT(90.0f), FLOAT(-50.0f))
    CALL(evt_npc_set_ry, STRING("サンダース"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("サンダース"), 32768)
    CALL(evt_npc_set_position, STRING("水夫A"), FLOAT(-683.0f), FLOAT(90.0f), FLOAT(-169.0f))
    CALL(evt_npc_set_ry, STRING("水夫A"), 90)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫A"), 32768)
    CALL(evt_npc_set_position, STRING("水夫ボム兵"), FLOAT(-806.0f), FLOAT(160.0f), FLOAT(-18.0f))
    CALL(evt_npc_set_ry, STRING("水夫ボム兵"), 270)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫ボム兵"), 32768)
    CALL(evt_cam3d_evt_set, 25, 305, 1091, 25, 150, -108, 0, 11)
    CALL(evt_seq_wait, 2)
    SPAWN_EVENT(fune_move)
    WAIT_MS(10000)
    CALL(evt_fade_set_mapchange_type, 1, 10, 900, 9, 900)
    CALL(evt_bero_mapchange, STRING("muj_01"), STRING(""))
    RETURN
EVT_END()


EVT_BEGIN(muj_20_init_evt)
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("fune_b_pointLight1"), STRING("S_b"), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("fune_b_pointLight2"), STRING("S_b"), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("fune_b_pointLight3"), STRING("S_b"), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("fune_b_pointLight4"), STRING("S_b"), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("fune_b_pointLight5"), STRING("S_b"), 1)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_light_bind_mapobj, STRING("fune_b_pointLight6"), STRING("S_b"), 1)
    END_INLINE_EVENT
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_l_01"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_l_02"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_l_03"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_l_04"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_l_05"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_r_01"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_r_02"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_r_03"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_r_04"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hai_r_05"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_fune_m"), 1)
    IF_INT_EQUAL(GSW(0), 215)
        CALL(evt_mario_set_mode, 2)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_b"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_umi_l_siro_b"), 1)
        CALL(evt_map_playanim, STRING("nami"), 1, 0)
        CALL(evt_map_playanim, STRING("nami_02"), 1, 0)
        CALL(evt_map_playanim, STRING("fune_01"), 1, 0)
        CALL(evt_map_playanim, STRING("kumo"), 1, 0)
        CALL(evt_map_playanim, STRING("yoru_hiru"), 2, 0)
        SPAWN_EVENT(muj_20_koopa_evt)
        RETURN
    END_IF
    CALL(evt_map_playanim, STRING("nami"), 1, 0)
    CALL(evt_map_playanim, STRING("nami_02"), 1, 0)
    CALL(evt_map_playanim, STRING("fune_01"), 1, 0)
    CALL(evt_map_playanim, STRING("kumo"), 1, 0)
    IF_INT_EQUAL(GSWF(3125), 1)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG5_GHOST_SHIP1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ1_1"))
        SPAWN_EVENT(gor_muj_ship)
    END_IF
    IF_INT_EQUAL(GSWF(1178), 1)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_STG5_GHOST_SHIP1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ1_1"))
        SPAWN_EVENT(muj_gor_ship)
    END_IF
    IF_INT_EQUAL(GSW(0), 229)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_SAILING1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ1_1"))
        SPAWN_EVENT(onryo_event)
    END_IF
    IF_INT_EQUAL(GSW(0), 230)
        CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_SAILING1"))
        CALL(evt_snd_envon, 272, STRING("ENV_STG5_MUJ1_1"))
        SPAWN_EVENT(onryo_event)
    END_IF
    CALL(evt_bero_get_entername, LW(0))
    IF_STR_EQUAL(LW(0), STRING("prologue"))
        CALL(evt_cam3d_evt_set, 0, 367, 2093, 0, 343, -115, 0, 11)
        CALL(evt_mario_cont_onoff, 0)
        CALL(evt_party_cont_onoff, 0, 0)
        CALL(evt_mario_set_pos, 0, -1000, 0)
        CALL(evt_party_set_pos, 0, 0, -1000, 0)
        CALL(evt_map_playanim, STRING("yoru_hiru"), 2, 0)
        CALL(evt_mapobj_trans, 1, STRING("S_hai03_sun"), 0, -260, 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_umi_l_siro_b"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_b"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_fune_m"), 1)
        BEGIN_INLINE_EVENT
            CALL(fune_eff)
        END_INLINE_EVENT
        SET_FLOAT_VALUE(GW(0), FLOAT(-2000.0f))
        SET_FLOAT_VALUE(GW(1), FLOAT(0.0f))
        SET_FLOAT_VALUE(GW(2), FLOAT(-3000.0f))
        SPAWN_EVENT(anm_kaji)
        SPAWN_EVENT(anm_wakka)
        SPAWN_EVENT(anm_entotsu)
        SPAWN_EVENT(anm_neji)
        SPAWN_EVENT(anm_yure)
        SPAWN_EVENT(evt_prologue)
    END_IF
    IF_INT_EQUAL(GSW(0), 402)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_a1"), 1)
        CALL(evt_hitobj_onoff, STRING("A_a1"), 1, 0)
        CALL(evt_map_playanim, STRING("yoru_hiru"), 2, 0)
        CALL(evt_mario_kill_party, 0)
        CALL_CHILD(epilogue_npc_entry)
        SPAWN_EVENT(epilogue_evt)
    END_IF
    RETURN
EVT_END()
// clang-format on
