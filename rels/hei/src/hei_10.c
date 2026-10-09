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

// Inline degree/radian sine and cosine approximations (same polynomials used by
// the npc blur path). Assigning the coefficients to non-const locals first
// (rather than using the literals directly) stops MWCC rematerialising them in
// every branch, which recovers most of both callers.
static inline f32 heiSinf(f32 rad) {
    f32 c0, c1, c2, pi;
    f32 f0, f1, sinv;

    c0 = 0.00761f;
    c1 = 0.16605f;
    c2 = 1.0f;
    pi = 3.1415927f;
    sinv = c0;
    if (rad <= pi) {
        if (rad <= 1.5707964f) {
            f0 = rad * rad;
            sinv = sinv * f0 - c1;
            sinv = sinv * f0 + c2;
            sinv = sinv * rad;
        } else {
            f0 = rad - 1.5707964f;
            f1 = 1.5707964f - f0;
            f0 = f1 * f1;
            sinv = sinv * f0 - c1;
            sinv = sinv * f0 + c2;
            sinv = sinv * f1;
        }
    } else if (rad < 4.712389f) {
        f1 = rad - pi;
        f0 = f1 * f1;
        sinv = sinv * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * f1;
        sinv = -sinv;
    } else {
        f0 = rad - 4.712389f;
        f1 = 1.5707964f - f0;
        f0 = f1 * f1;
        sinv = sinv * f0 - c1;
        sinv = sinv * f0 + c2;
        sinv = sinv * f1;
        sinv = -sinv;
    }
    return sinv;
}

static inline f32 heiCosf(f32 rad) {
    f32 c0, c1, c2, pi;
    f32 f0, cosv;

    c0 = 0.03705f;
    c1 = 0.4967f;
    c2 = 1.0f;
    pi = 3.1415927f;
    cosv = c0;
    if (rad <= pi) {
        if (rad <= 1.5707964f) {
            f0 = rad * rad;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
        } else {
            f0 = rad - 1.5707964f;
            f0 = 1.5707964f - f0;
            f0 = f0 * f0;
            cosv = cosv * f0 - c1;
            cosv = cosv * f0 + c2;
            cosv = -cosv;
        }
    } else if (rad < 4.712389f) {
        f0 = rad - pi;
        f0 = f0 * f0;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
        cosv = -cosv;
    } else {
        f0 = rad - 4.712389f;
        f0 = 1.5707964f - f0;
        f0 = f0 * f0;
        cosv = cosv * f0 - c1;
        cosv = cosv * f0 + c2;
    }
    return cosv;
}

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
static f32 randf(f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;

    return r * (max - min) + min;
}

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

void chb_jump(ExtPose* pose, HeiExtEntry* entry, s16 field24, f32 x, f32 y, f32 z, f32 v1, f32 v2) {
    Vec pos = { 0 };
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

// Inline degree->radian used by ext_main (the original inlines pi*deg/180).
static inline f32 heiDeg2Rad(f32 deg) {
    return 3.1415927f * deg / 180.0f;
}
// Structure, the switch dispatch, the case ordering, the clamp guard and the
// inlined sin/cos blocks match the original. Source spellings the original
// needs:
//  - case 1's range test is `if (dist < 800.0f) { ... } else { state = 99; }`
//    (an early-return would make MWCC emit the NaN-safe cror eq,gt,eq form);
//  - the trailing clamp guard is `switch (entry->state)` with `case 1: break;`
//    before `case 0:` so MWCC emits the original's two-compare range test;
//  - the jitter is `+ -45.0f` (loads float_neg45 and contracts to fmadds);
//  - case 0's `a` is `aR * 0.19999999f + 0.3f` for float_0p2_hei_00007bec,
//    and case 1's field_0x1C uses 0.20000005f (float_0p2_hei_00007c08);
//    plain 0.2f shifts .rodata;
//  - case 0 calls chb_jump, which MWCC inlines: the original inlines it in both cases (retail
//    reads chb_jump's zero template vec3_hei_00007a34 three times: chb_jump, case 0, case 1).
//    Case 1 still spells the body out (as a call, with its v1/v2 arguments, it scores 93.00 but
//    gets the three 7a34 references and the sfxPos template at 7a40 right; retail's 7a4c is
//    unreferenced). Case 1's `Vec pos = { 0 }` and `Vec sfxPos = { 0 }` keep their
//    initialisers, and its copy must sit in its own `{ }` block or MWCC sinks it;
//  - case 1's `field_0x20 = zero * b + 0.8f` needs a block-local `zero` so the
//    original's zero_hei_00007b88 load and fmadds survive (a literal folds);
//  - case 0x64 keeps `matrix[2][3] += field_0x1C * -cosv` (fneg + fmadds, not
//    fnmsubs) and updates matrix[0][3] before heiCosf;
//  - the inlined randf calls are unfolded by hand so the normalised
//    rand()/32767 survives the next rand() call in a callee-saved FPR;
//  - `oldState` is declared at function scope first;
//  - heiSinf/heiCosf reuse `rad` as the reduced angle and spell the reduction
//    `1.5707964f - (rad - 1.5707964f)`: that is what colours the 1p5708 pivot
//    into f1 and the accumulator seed into f6/f4 like the original;
//  - the loop header increments `pose++, entry++, i++`, not the original
//    `i++, pose++, entry++` (worth 0.02%).
// What is left: MWCC's allocator still keeps pz/aR swapped (f28/f29) through
// the case-0 rand calls, slots the case-0/case-1 Vec temporaries at 0x8/0x20
// (the original's 0x14/0x8), reads the case-0 `deg` fmadd
// with its operands the other way round, and spills the sfx address through r0
// (`mr r28, r0`). Allocator work, no source-shape fix left.
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

#pragma opt_propagation off // stop MWCC rematerialising the heiSinf/heiCosf coefficients per branch
extern const f32 float_360_hei_00007b94;
// NONMATCHING (93.69%): MWCC FPR/GPR allocation and instruction scheduling (notes above heiSin2).
static void ext_main(void) {
    s16 oldState;
    const char* sfx;
    MarioWork* mario;
    s32 i;
    ExtPose* pose;
    HeiExtEntry* entry;

    mario = marioGetPtr();
    pose = extGetPosePtr();
    entry = wp->entries;
    sfx = "SFX_STG1_CHORO_ESCAPE1";

    for (i = 0; i < wp->count; pose++, entry++, i++) {
        switch (entry->state) {
        case 0: {
#ifdef MUST_MATCH
            // The original's inlined randf(0,360) keeps the normalised
            // rand()/32767 in a callee-saved FPR across the following rand()
            // call, and folds the `*360+0` scaling in after it. Unfold by hand.
            f32 degR = (f32)rand() / 32767.0f;
            f32 speed = randf(4.0f, 7.0f);
            f32 zero = 0.0f;
            f32 deg = degR * float_360_hei_00007b94 + zero;
#else
            f32 deg = randf(0.0f, 360.0f);
            f32 speed = randf(4.0f, 7.0f);
#endif
            f32 rad = heiDeg2Rad(deg);
            f32 sinv = heiSinf(rad);
            f32 py = (*pose->matrix)[1][3];
            f32 px = speed * sinv + (*pose->matrix)[0][3];
            f32 cosv = heiCosf(rad);
            f32 pz = (*pose->matrix)[2][3] - speed * cosv;
#ifdef MUST_MATCH
            f32 aR = (f32)rand() / 32767.0f;
            f32 bR = (f32)rand() / 32767.0f;
            f32 a = aR * 0.19999999f + 0.3f;
            f32 b = bR * 0.3f + 0.5f;
#else
            f32 a = randf(0.3f, 0.5f);
            f32 b = randf(0.5f, 0.8f);
#endif
            chb_jump(pose, entry, entry->state, px, py, pz, a, b);
        } break;
        case 1: {
            f32 dist = distABf(mario->position.x, mario->position.z, (*pose->matrix)[0][3], (*pose->matrix)[2][3]);
            f32 r;
            f32 dir;
            f32 rad;
            f32 speed;
            f32 sinv;
            f32 py;
            f32 cosv;
            f32 pz;
            f32 a;
            f32 px;
            f32 b;
            f32 dy;
            s32 frames;

            if (dist < 800.0f) {
#ifdef MUST_MATCH
            // The original folds randf(0,1)'s "*1+0" away here; propagation is
            // off for the sin/cos coefficients, so spell the division out.
            r = (f32)rand() / 32767.0f;
#else
            r = randf(0.0f, 1.0f);
#endif
            // Spelling the jitter as `+ -45.0f` makes MWCC load float_neg45 and
            // contract to fmadds, matching the original's pool entry; the natural
            // `- 45.0f` instead pools float_45 and emits fmsubs.
            dir = angleABf(mario->position.x, mario->position.z, (*pose->matrix)[0][3], (*pose->matrix)[2][3]) + (r * 90.0f + -45.0f);
            speed = randf(15.0f, 20.0f);
            rad = heiDeg2Rad(dir);
            sinv = heiSinf(rad);
            px = speed * sinv + (*pose->matrix)[0][3];
            py = (*pose->matrix)[1][3];
            cosv = heiCosf(rad);
            pz = (*pose->matrix)[2][3] - speed * cosv;
#ifdef MUST_MATCH
            a = (f32)rand() / 32767.0f;
            b = (f32)rand() / 32767.0f;
#else
            a = randf(0.0f, 1.0f);
            b = randf(0.0f, 1.0f);
#endif
            {
                Vec pos = { 0 };

                pos.x = px;
                pos.y = py;
                pos.z = pz;
                oldState = entry->state;
                entry->position = pos;
            }
            entry->field_0x1C = 2.0f * (1.0f + 0.20000005f * a);
            {
                f32 zero = 0.0f;

                entry->field_0x20 = zero * b + 0.8f;
            }
            dy = entry->position.y - (*pose->matrix)[1][3];
            dist = distABf((*pose->matrix)[0][3], (*pose->matrix)[2][3], entry->position.x, entry->position.z);
            angleABf((*pose->matrix)[0][3], (*pose->matrix)[2][3], entry->position.x, entry->position.z);
            frames = (s32)(dist / entry->field_0x1C);
            entry->field_0x18 = frames;
            entry->field_0x14 = entry->field_0x20 * (f32)(s16)entry->field_0x18 / 2.0f + dy / (f32)(s16)entry->field_0x18;
            entry->state = 0x64;
            entry->field_0x24 = oldState;
            if (rand() % 150 == 0) {
#ifdef MUST_MATCH
                // The original builds the position then copies it to a second
                // Vec before the call (MWCC by-value argument copy).
                Vec sfxPos = { 0 };
                Vec sfxOut;
#else
                Vec sfxPos = { 0 };
#endif

                sfxPos.x = (*pose->matrix)[0][3];
                sfxPos.y = (*pose->matrix)[1][3];
                sfxPos.z = (*pose->matrix)[2][3];
#ifdef MUST_MATCH
                sfxOut = sfxPos;
                psndSFXOn_3D(sfx, &sfxOut);
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
            sinv = heiSinf(rad);
            (*pose->matrix)[0][3] += entry->field_0x1C * sinv;
            cosv = heiCosf(rad);
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
            pose->field_0x10 = (gp->retraceCount + i) % 20;
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
const Vec vec3_hei_00007a70 = {0.0f, 0.0f, 0.0f};

// Hops an npc to (x, y, z) over a frame count: userdata holds the frames left, the vertical velocity and the speed.
// The same 5 FPR diffs as rsh_05's copy (dy f2 vs retail f4, then the frames reload and 0.5 swap f1/f2).
// MWCC's parser puts the leaf operand of `grav * frames * 0.5f` on the left whichever way it is written, so
// the 0.5 load gets the lower vreg; rasim says retail numbered the product first.
// NONMATCHING (99.88%): FPR colouring of dy and the vy expression.
#pragma opt_lifetimes off
#pragma opt_propagation off // stop MWCC rematerialising the heiSinf/heiCosf coefficients per branch
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
        vy = dy / frames + grav * frames * 0.5f;
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
        Vec land;

        land = vec3_hei_00007a70;
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
#pragma opt_lifetimes reset
USER_FUNC(wait_anim_frame) {
    f32 frame = evtGetFloat(event, event->args[0]);
    AnimationPose* pose = animPoseGetAnimPosePtr(npcNameToPtr("ゴールドチョロボン")->poseId);

    if (((AnimPoseField24*)pose)->field_0x24 < frame) {
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
