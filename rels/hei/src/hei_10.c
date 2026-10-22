#include "hei_10.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/extdrv.h"
#include "driver/npcdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_snd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "memory.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "system.h"
#include <stdlib.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
static USER_FUNC(npc_jump);
static void ext_dispent(void);
static void ext_main(void);
static void ext_shadow_disp(void);
static void ext_disp(CameraId cameraId, void* param);
static void ext_init(void);
static f32 randf(f32 min, f32 max);

static HeiExtWork* wp;

// clang-format off
extend_data(44,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(45,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

static BeroEntry bero_entry_data[4] = {
    { "w_bero", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "hei_09", "e_bero", 1, 1, 0, 0 },
    { "e_bero", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "hei_11", "w_bero", 1, 1, 0, 0 },
    { "dokan", 2, 0, 0x06, 0x186A0, 0, 0, -1, NULL, 0x0A, NULL, "hei_07", "dokan", 1, 1, 0, 0 },
    { NULL, 0, 0, 0, 0, 0, 0, 0, NULL, 0, NULL, NULL, NULL, 0, 0, 0, 0 },
};

static ExtEntry ext_data[21] = {
    { "c_chorobon", "CBN_W_1", 0.0f },
    { "c_chorobon", "CBN_W_1", 2.0f },
    { "c_chorobon", "CBN_W_1", 4.0f },
    { "c_chorobon", "CBN_W_1", 6.0f },
    { "c_chorobon", "CBN_W_1", 8.0f },
    { "c_chorobon", "CBN_W_1", 10.0f },
    { "c_chorobon", "CBN_W_1", 12.0f },
    { "c_chorobon", "CBN_W_1", 14.0f },
    { "c_chorobon", "CBN_W_1", 16.0f },
    { "c_chorobon", "CBN_W_1", 18.0f },
    { "c_chorobon", "CBN_W_1", 20.0f },
    { "c_chorobon", "CBN_W_1", 22.0f },
    { "c_chorobon", "CBN_W_1", 24.0f },
    { "c_chorobon", "CBN_W_1", 26.0f },
    { "c_chorobon", "CBN_W_1", 28.0f },
    { "c_chorobon", "CBN_W_1", 30.0f },
    { "c_chorobon", "CBN_W_1", 32.0f },
    { "c_chorobon", "CBN_W_1", 34.0f },
    { "c_chorobon", "CBN_W_1", 36.0f },
    { "c_chorobon", "CBN_W_1", 38.0f },
    { NULL, NULL, 0.0f },
};
// randf's scaling step, for the sites in ext_main that draw the rand() fraction ahead of its use.
static inline f32 randScale(f32 t, f32 min, f32 max) {
    return t * (max - min) + min;
}

// randf's first step, the rand() fraction in [0, 1].
static inline f32 frand(void) {
    f32 r = (f32)rand() / 32767.0f;

    return r;
}

static inline f32 randRange(f32 min, f32 max) {
    return max - min;
}

#include "rel/common/randf.h"

static void ext_init(void) {
    s32 i;
    ExtPose* pose;
    HeiExtEntry* entry;

    wp = _mapAlloc(0xC);
    memset(wp, 0, 0xC);
    wp->count = extGetPoseNum();
    wp->entries = _mapAlloc(wp->count * 0x30);
    memset(wp->entries, 0, wp->count * 0x30);
    pose = extGetPosePtr();
    entry = wp->entries;
    i = 0;
    while (i < wp->count) {
        pose->flags |= 7;
        PSMTXIdentity(*pose->matrix);
        (*pose->matrix)[0][3] = randf(-230.0f, 230.0f);
        (*pose->matrix)[1][3] = -15.0f;
        (*pose->matrix)[2][3] = randf(-100.0f, 100.0f);
        i++;
        pose++;
        entry->field_0x00 = 0;
        entry->state = 0;
        entry->field_0x04 = 0;
        entry++;
    }
}

static void ext_disp(CameraId cameraId, void* param) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

static void ext_shadow_disp(void) {
    static Mtx mt = {
        {8.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 8.0f, 0.0f, 0.5f},
        {0.0f, 0.0f, 8.0f, 0.0f},
    };

    extLoadShadowMtx(mt);
    extLoadShadowRenderMode();
    extLoadShadowVertex();
    extLoadShadowTev();
    extDrawShadow();
}

const Vec vec3_hei_00007a34 = {0.0f, 0.0f, 0.0f};
const Vec vec3_hei_00007a40 = {0.0f, 0.0f, 0.0f};
const Vec vec3_hei_00007a4c = {0.0f, 0.0f, 0.0f};

void chb_jump(ExtPose* pose, HeiExtEntry* entry, f32 x, f32 y, f32 z, f32 v1, f32 v2, s16 field24) {
    Vec pos = vec3_hei_00007a34;
    f32 dy;
    f32 dist;
    s32 frames;

    pos.x = x;
    pos.y = y;
    pos.z = z;
    entry->position = pos;
    entry->field_0x1C = v1;
    entry->field_0x20 = v2;
    dy = entry->position.y - (*pose->matrix)[1][3];
    dist = distABf((*pose->matrix)[0][3], (*pose->matrix)[2][3], entry->position.x, entry->position.z);
    angleABf((*pose->matrix)[0][3], (*pose->matrix)[2][3], entry->position.x, entry->position.z);
    frames = (s32)(dist / entry->field_0x1C);
    entry->field_0x18 = frames;
    entry->field_0x14 = entry->field_0x20 * entry->field_0x18 / 2.0f + dy / entry->field_0x18;
    entry->state = 0x64;
    entry->field_0x24 = field24;
}

static inline void chbJumpRand(ExtPose* pose, HeiExtEntry* entry, f32 x, f32 y, f32 z, f32 a, f32 lo1, f32 hi1, f32 b,
                               f32 lo2, f32 hi2, s16 state) {
    chb_jump(pose, entry, x, y, z, a * (hi1 - lo1) + lo1, b * (hi2 - lo2) + lo2, state);
}

static inline void chbJumpRand2(ExtPose* pose, HeiExtEntry* entry, f32 x, f32 y, f32 z, f32 k, f32 a, f32 lo1, f32 hi1,
                                f32 b, f32 lo2, f32 hi2, s16 state) {
    chb_jump(pose, entry, x, y, z, k * (a * (hi1 - lo1) + lo1), b * (hi2 - lo2) + lo2, state);
}

// Inline degree->radian used by ext_main (the original inlines pi*deg/180).
static inline f32 heiDeg2Rad(f32 deg) {
    return 3.1415927f * deg / 180.0f;
}
static inline f32 heiSin2(f32 angle) {
    f32 v = 0.00761f;
    f32 b = 0.16605f;
    f32 one = 1.0f;
    f32 u;
    if (angle <= 3.1415927f) {
        if (angle <= 1.5707964f) { u = angle * angle; v = v * u - b; v = v * u + one; v = v * angle; }
        else { angle = 1.5707964f - (angle - 1.5707964f); u = angle * angle; v = v * u - b; v = v * u + one; v = v * angle; }
    } else if (angle < 4.712389f) {
        angle = angle - 3.1415927f; u = angle * angle; v = v * u - b; v = v * u + one; v = v * angle; v = -v;
    } else {
        angle = 1.5707964f - (angle - 4.712389f); u = angle * angle; v = v * u - b; v = v * u + one; v = v * angle; v = -v;
    }
    return v;
}

static inline f32 heiCos2(f32 angle) {
    f32 v = 0.03705f;
    f32 b = 0.4967f;
    f32 one = 1.0f;
    f32 u;
    if (angle <= 3.1415927f) {
        if (angle <= 1.5707964f) { u = angle * angle; v = v * u - b; v = v * u + one; }
        else { angle = 1.5707964f - (angle - 1.5707964f); u = angle * angle; v = v * u - b; v = v * u + one; v = -v; }
    } else if (angle < 4.712389f) {
        angle = angle - 3.1415927f; u = angle * angle; v = v * u - b; v = v * u + one; v = -v;
    } else {
        angle = 1.5707964f - (angle - 4.712389f); u = angle * angle; v = v * u - b; v = v * u + one;
    }
    return v;
}

#pragma opt_propagation off // stop MWCC rematerialising the heiSin2/heiCos2 coefficients per branch
static void ext_main(void) {
#ifdef MUST_MATCH
    union { const char* p; u32 u; } sfx;
#else
    const char* sfx;
#endif
    MarioWork* mario;
    s32 i;
    ExtPose* pose;
    HeiExtEntry* entry;
    Vec sfxOut;
    Vec sfxPos;

    mario = marioGetPtr();
    pose = extGetPosePtr();
    entry = wp->entries;
#ifdef MUST_MATCH
    sfx.p = "SFX_STG1_CHORO_ESCAPE1";
#else
    sfx = "SFX_STG1_CHORO_ESCAPE1";
#endif

    for (i = 0; i < wp->count; i++, pose++, entry++) {
        switch (entry->state) {
        case 0: {
            f32 speed, degR, aR, bR, rad, py, pz, deg, px;

            degR = frand();
            speed = randf(4.0f, 7.0f);
            deg = randScale(degR, 0.0f, 360.0f);
            rad = heiDeg2Rad(deg);
            px = speed * heiSin2(rad) + (*pose->matrix)[0][3];
            py = (*pose->matrix)[1][3];
            pz = -(speed * heiCos2(rad) - (*pose->matrix)[2][3]);
            aR = frand();
            bR = frand();
            chbJumpRand(pose, entry, px, py, pz, aR, 0.3f, 0.5f, bR, 0.5f, 0.8f, entry->state);
        } break;
        case 1: {
            f32 pz, speed, rad, py, dist, a, px, b, dir;

            dist = distABf(mario->position.x, mario->position.z, (*pose->matrix)[0][3], (*pose->matrix)[2][3]);
            if (dist < 800.0f) {
            a = frand();
            dir = angleABf(mario->position.x, mario->position.z, (*pose->matrix)[0][3], (*pose->matrix)[2][3]) +
                  (a * randRange(-45.0f, 45.0f) + -45.0f);
            speed = randf(15.0f, 20.0f);
            rad = heiDeg2Rad(dir);
            px = speed * heiSin2(rad) + (*pose->matrix)[0][3];
            py = (*pose->matrix)[1][3];
            pz = -(speed * heiCos2(rad) - (*pose->matrix)[2][3]);
            a = frand();
            b = frand();
            chbJumpRand2(pose, entry, px, py, pz, 2.0f, a, 1.0f, 1.2f, b, 0.8f, 0.8f, entry->state);
            if (rand() % 150 == 0) {
                sfxPos = vec3_hei_00007a40;

                sfxPos.x = (*pose->matrix)[0][3];
                sfxPos.y = (*pose->matrix)[1][3];
                sfxPos.z = (*pose->matrix)[2][3];
#ifdef MUST_MATCH
                sfxOut = sfxPos;
                psndSFXOn_3D((const char*)(u32)sfx.p, &sfxOut);
#else
                psndSFXOn_3D(sfx, &sfxPos);
#endif
            }
            } else {
                entry->state = 99;
            }
        } break;
        case 0x63:
            break;
        case 0x64: {
            f32 angle;
            f32 rad;
            f32 sinv;
            f32 cosv;

            (*pose->matrix)[1][3] += entry->field_0x14;
            entry->field_0x14 -= entry->field_0x20;
            angle = angleABf((*pose->matrix)[0][3], (*pose->matrix)[2][3], entry->position.x, entry->position.z);
            rad = 3.1415927f * angle / 180.0f;
            sinv = heiSin2(rad);
            (*pose->matrix)[0][3] += entry->field_0x1C * sinv;
            cosv = heiCos2(rad);
            (*pose->matrix)[2][3] += entry->field_0x1C * -cosv;
            if (--entry->field_0x18 < 0) {
                (*pose->matrix)[0][3] = entry->position.x;
                (*pose->matrix)[1][3] = entry->position.y;
                (*pose->matrix)[2][3] = entry->position.z;
                entry->state = entry->field_0x24;
            }
        } break;
        default:
            break;
        }
        switch (entry->state) {
        case 1:
            break;
        case 0:
            if ((*pose->matrix)[0][3] > 230.0f) {
                (*pose->matrix)[0][3] = 230.0f;
            }
            if ((*pose->matrix)[0][3] < -230.0f) {
                (*pose->matrix)[0][3] = -230.0f;
            }
            if ((*pose->matrix)[2][3] > 100.0f) {
                (*pose->matrix)[2][3] = 100.0f;
            }
            if ((*pose->matrix)[2][3] < -100.0f) {
                (*pose->matrix)[2][3] = -100.0f;
            }
            break;
        }
        if (gp->retraceCount & 1) {
            pose->texId = (gp->retraceCount + i) % 20;
        }
    }
}
#pragma opt_propagation reset
static void ext_dispent(void) {
    dispEntry(4, 0.0f, 1, ext_disp, NULL);
}

USER_FUNC(evt_chorobon_escape) {
    s32 i;
    HeiExtEntry* entry;

    extGetPosePtr();
    entry = wp->entries;
    for (i = 0; i < wp->count; i++) {
        entry->state = 1;
        entry->field_0x24 = 1;
        entry++;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_chorobon_wait) {
    s32 i;
    HeiExtEntry* entry;

    extGetPosePtr();
    entry = wp->entries;
    for (i = 0; i < wp->count; i++) {
        if (entry->state != 99) {
            return EVT_RETURN_BLOCK;
        }
        entry++;
    }
    extReset();
    return EVT_RETURN_DONE;
}

const Vec vec3_hei_00007a58 = {0.0f, 0.0f, 0.0f};
const Vec vec3_hei_00007a64 = {0.0f, 0.0f, 0.0f};

// Hops an npc to (x, y, z) over a frame count: userdata holds the frames left, the vertical velocity and the speed.
#pragma opt_propagation off // stop MWCC rematerialising the heiSin2/heiCos2 coefficients per branch
static USER_FUNC(npc_jump) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    f32 speed = evtGetFloat(event, args[4]);
    f32 grav = evtGetFloat(event, args[5]);
    s32 ms = evtGetValue(event, args[6]);
    f32 frames = 60.0f * (f32)ms / 1000.0f;
    NpcEntry* npc = npcNameToPtr(name);
    f32 vy;
    f32 rad;

    if (isFirstCall) {
        Vec to;
        Vec from;
        f32 dist;
        f32 dy;

        from = vec3_hei_00007a64;
        from.x = npc->position.x;
        from.z = npc->position.z;
        to = vec3_hei_00007a58;
        to.x = x;
        to.z = z;
        dist = PSVECDistance(&to, &from);
        dy = y - npc->position.y;
        if (0.0f == frames) {
            frames = dist / speed;
        } else {
            speed = dist / frames;
        }
        event->userdata[0] = *(s32*)&frames;
        vy = dy / frames + grav * frames / 2.0f;
        event->userdata[1] = *(s32*)&vy;
        event->userdata[2] = *(s32*)&speed;
    }
    frames = event->userdataf[0];
    vy = event->userdataf[1];
    speed = event->userdataf[2];
    rad = 6.2831855f * angleABf(npc->position.x, npc->position.z, x, z) / 360.0f;
    npc->position.x = speed * heiSin2(rad) + npc->position.x;
    npc->position.z = -(speed * heiCos2(rad) - npc->position.z);
    npc->position.y += vy;
    vy -= grav;
    frames -= 1.0f;
    if (frames < 0.0f) {
        Vec land = {0.0f, 0.0f, 0.0f};

        land.x = x;
        land.y = y;
        land.z = z;
        npc->position = land;
        return EVT_RETURN_YIELD;
    }
    event->userdata[0] = *(s32*)&frames;
    event->userdata[1] = *(s32*)&vy;
    return EVT_RETURN_BLOCK;
}
#pragma opt_propagation reset
USER_FUNC(wait_anim_frame) {
    f32 frame = evtGetFloat(event, event->args[0]);
    AnimationPose* pose = animPoseGetAnimPosePtr(npcNameToPtr("ゴールドチョロボン")->poseId);

    if (pose->clipTime < frame) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(wait_anim_end) {
    NpcEntry* npc = npcNameToPtr("ゴールドチョロボン");

    if (isFirstCall) {
        event->lwData[15] = (s32)animPoseGetLoopTimes(npc->poseId);
    }
    if (event->lwData[15] == (s32)animPoseGetLoopTimes(npc->poseId)) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(golden_se)
    LOOP(0)
        CALL(wait_anim_frame, 35)
        CALL(evt_npc_get_position, STRING("ゴールドチョロボン"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_CHORO_MOVE1"), LW(0), LW(1), LW(2), 0)
        CALL(wait_anim_frame, 50)
        CALL(evt_npc_get_position, STRING("ゴールドチョロボン"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_CHORO_G_WAIT2"), LW(0), LW(1), LW(2), 0)
        CALL(wait_anim_frame, 95)
        CALL(evt_npc_get_position, STRING("ゴールドチョロボン"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_CHORO_G_WAIT1"), LW(0), LW(1), LW(2), 0)
        CALL(wait_anim_end)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_golden)
    CALL(evt_npc_entry, STRING("dummy"), STRING("c_chorobon"))
    CALL(evt_npc_set_position, STRING("dummy"), 0, -1000, 0)
    SPAWN_EVENT_GET_ID(PTR(golden_se), LW(15))
    WAIT_FRAMES(10)
    CALL(evt_mario_wait_movable)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_FIND1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_eff_fukidashi, 0, STRING("eff0"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_eff_fukidashi, 3, STRING("eff1"), 0, 0, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(evt_cam3d_evt_set, 0, 58, 227, 0, 5, -68, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg1_hei_42"), 0, STRING("ゴールドチョロボン"))
    WAIT_MS(300)
    CALL(evt_cam3d_event_from_road, 0, -15, 0, 0, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5))
    CALL(evt_cam3d_evt_set, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), 500, 11)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_off, 1000, 11)
    DELETE_EVENT(LW(15))
    CALL(evt_npc_set_anim, STRING("ゴールドチョロボン"), STRING("CBN_W_1"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    DIVIDE_FLOAT(LW(0), FLOAT(4.0f))
    SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
    CALL(evt_npc_flag_onoff, 1, STRING("ゴールドチョロボン"), 131088)
    LOOP(3)
        ADD_FLOAT(LW(3), LW(0))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_CHORO_MOVE1"), LW(3), LW(1), LW(2), 0)
        CALL(evt_npc_jump_position, STRING("ゴールドチョロボン"), LW(3), LW(1), LW(2), 300, 0, FLOAT(30.0f), 0)
    END_LOOP
    CALL(evt_npc_flag_onoff, 1, STRING("ゴールドチョロボン"), 1073741856)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    ADD(LW(1), 30)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_SURPRISED2_2"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_dispflag_onoff, 1, 2)
    CALL(evt_mario_set_pose, STRING("M_N_7"))
    WAIT_MS(500)
    CALL(evt_npc_get_position, STRING("ゴールドチョロボン"), LW(3), LW(4), LW(5))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG1_CHORO_MOVE1"), LW(3), LW(4), LW(5), 0)
    CALL(evt_npc_flag_onoff, 0, STRING("ゴールドチョロボン"), 131088)
    CALL(npc_jump, STRING("ゴールドチョロボン"), LW(0), LW(1), LW(2), FLOAT(3.5f), FLOAT(2.0f), 0)
    CALL(evt_npc_battle_start, STRING("ゴールドチョロボン"))
    CALL(evt_npc_wait_battle_end)
    CALL(evt_mario_dispflag_onoff, 0, 2)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_TORIDE1"))
    CALL(evt_npc_set_position, STRING("ゴールドチョロボン"), 0, -15, 0)
    CALL(evt_ext_entry, 500, PTR(ext_data), PTR(ext_init), PTR(ext_main), PTR(ext_dispent))
    CALL(evt_msg_print, 0, STRING("stg1_hei_44"), 0, STRING("ゴールドチョロボン"))
    WAIT_MS(300)
    CALL(evt_chorobon_escape)
    CALL(evt_npc_set_position, STRING("ゴールドチョロボン"), 0, -1000, 0)
    SET_VALUE(GSW(0), 32)
    CALL(evt_mario_key_onoff, 1)
    BEGIN_INLINE_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
        SET_FLOAT_VALUE(LW(1), FLOAT(0.0f))
        SET_FLOAT_VALUE(LW(2), FLOAT(500.0f))
        DIVIDE_FLOAT(LW(2), FLOAT(40.0f))
        SET_FLOAT_VALUE(LW(3), FLOAT(0.0f))
        CALL(evt_npc_flag_onoff, 0, STRING("ゴールドチョロボン"), 131088)
        LOOP(40)
            ADD_FLOAT(LW(3), LW(2))
            CALL(npc_jump, STRING("ゴールドチョロボン"), LW(0), LW(1), LW(3), FLOAT(0.8f), FLOAT(1.0f), 0)
        END_LOOP
        CALL(evt_npc_set_position, STRING("ゴールドチョロボン"), 0, -1000, 0)
    END_INLINE_EVENT
    CALL(evt_chorobon_wait)
    RETURN
EVT_END()

EVT_BEGIN(hei_10_init_evt)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG1_TORIDE1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG1_HEI3"))
    CALL(evt_snd_set_rev_mode, 1)
    CALL(evt_mobj_badgeblk, STRING("m"), 100, 46, -75, 241, 0, GSWF(1796), 1)
    CALL(evt_npc_set_position, STRING("ゴールドチョロボン"), 0, -1000, 0)
    IF_INT_EQUAL(GSWF(1774), 1)
        IF_INT_EQUAL(GSWF(1775), 1)
            IF_INT_LESS(GSW(0), 32)
                CALL(evt_npc_set_position, STRING("ゴールドチョロボン"), 0, -15, 0)
                CALL(evt_npc_change_fbat_mode, 1)
                SPAWN_EVENT(evt_golden)
            END_IF
        END_IF
    END_IF
    CALL(evt_map_playanim, STRING("mizu"), 1, 0)
    CALL(evt_map_set_tevcallback, 1, PTR(hei_test_water_tev))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("mizu"), 16777216)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("mizu"), 256)
    RETURN
EVT_END()
// clang-format on
