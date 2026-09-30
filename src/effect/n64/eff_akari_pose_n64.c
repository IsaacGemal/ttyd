#include "eff/n64/eff_akari_pose_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <math.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// Building the zero Vec through this inline (rather than `position = vec3_...;` and then assigning
// the fields) reproduces the original's schedule: the zero words are materialised first, overwritten
// in place, then copied to the second Vec.
static inline Vec akariMakeVec(f32 x, f32 y, f32 z) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = x;
    temp.y = y;
    temp.z = z;
    return temp;
}

// .sdata2 float constants (named after their original addresses).
extern const f32 float_deg2rad_8041e8d0;
extern const f32 float_0p03125_8041e8d4;
extern const f32 float_0_8041e8d8;
extern const f32 float_0p1_8041e8dc;
extern const f32 float_0p2_8041e8e0;
extern const f32 float_360_8041e8e4;
extern const f32 float_6p2832_8041e8e8;
extern const f32 float_16_8041e8ec;
extern const f32 float_45_8041e8f0;
extern const f32 float_1_8041e8f4;
extern const f32 float_0p5_8041e8f8;
extern const f32 float_neg1_8041e8fc;

// .rodata string and the pooled int->float conversion double (named after their original addresses).
const char str_802FA1D0[16] = "AkariPoseN64";
// The compiler only references this by name if it is non-const; a const definition is constant-folded
// to an anonymous @NNN literal, which no longer matches the original `lfd ... double_to_int_802fa1c8`.
f64 double_to_int_802fa1c8 = 4503601774854144.0;

// .sdata sequence-length table indexed by the effect type (local to this file in the original).
static s32 max_seq_num[2] = {0x18, 0x0C};

// .sbss2 scratch GXColor the original builds its TEV colour from.
extern GXColor unk_80423098;

// .data vertex array, local to this file (0x38 bytes).
static u32 size16x16_tex32x32_vtx[0x38 / 4] = {
    0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008, 0xFFF80000, 0x04000000, 0x000000FF,
    0x00080008, 0x00000400, 0x04000000, 0x00FFFFF8, 0x00080000, 0x00000400, 0x000000FF,
};

// .data per-frame scale tables, local to this file in the original.
static f32 scale_data[24] = {
    0.9f, 1.0f, 1.0f, 0.9f, 0.85f, 0.8f, 0.75f, 0.7f, 0.65f, 0.6f, 0.55f, 0.5f,
    0.45f, 0.4f, 0.35f, 0.3f, 0.25f, 0.2f, 0.15f, 0.1f, 0.05f, 0.03f, 0.02f, 0.01f,
};
static f32 scale_data2[24] = {
    0.9f, 1.0f, 1.0f, 0.9f, 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
};

// .sdata per-frame alpha tables, local to this file in the original.
static u8 a_data[24] = {
    0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
static u8 a_data2[24] = {
    0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

typedef struct effAkariPosePart {
    u8 pad_0x00[0x4];  // 0x00
    f32 x;             // 0x04
    f32 y;             // 0x08
    u8 pad_0x0C[0x4];  // 0x0C
    f32 vx;            // 0x10
    f32 vy;            // 0x14
    f32 field_0x18;    // 0x18
    f32 field_0x1c;    // 0x1C
    s32 field_0x20;    // 0x20
    f32 field_0x24;    // 0x24
    u8 pad_0x28[0x8];  // 0x28
    s32 field_0x30;    // 0x30
    f32 field_0x34;    // 0x34
} effAkariPosePart;

typedef struct effAkariPoseData {
    s32 type;                // 0x00
    f32 x;                   // 0x04
    f32 y;                   // 0x08
    f32 z;                   // 0x0C
    u8 pad_0x10[0x18];       // 0x10
    s32 field_0x28;          // 0x28
    s32 field_0x2c;          // 0x2C
    s32 field_0x30;          // 0x30
    f32 field_0x34;          // 0x34
    effAkariPosePart part[]; // 0x38
} effAkariPoseData;

EffectEntry* effAkariPoseN64Entry(s32 type, s32 count, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    effAkariPoseData* data;
    effAkariPosePart* part;
    s32 i;
    f32 seqStep;
#ifdef MUST_MATCH
    // Two statements instead of one: the original loads the 0.5 constant into f0 and the converted
    // seq length into f1; a single expression makes this compiler swap them.
    f32 seqF;
#endif

    effect = effEntry();
    effect->type = str_802FA1D0;
    effect->count = count + 1;
    data = __memAlloc(HEAP_EFFECT, (count + 1) * 0x38);
    effect->userdata = data;
    effect->callback = effAkariPoseMain;
    effect->flags |= 2;

    data->type = type;
    data->field_0x2c = 0;
    if (lifetime <= 0) {
        data->field_0x28 = 1000;
    } else {
        data->field_0x28 = lifetime;
    }
    data->field_0x30 = 0xff;
    data->x = x;
    data->y = y;
    data->z = z;
    data->field_0x34 = scale;

#ifdef MUST_MATCH
    seqF = (f32)max_seq_num[type];
    seqStep = seqF * float_0p5_8041e8f8;
#else
    seqStep = (f32)max_seq_num[type] * float_0p5_8041e8f8;
#endif
    for (i = 1, part = data->part; i < count + 1; i++, part++) {
        part->field_0x20 = (s32)(float_neg1_8041e8fc - seqStep * (f32)(i & 1));
    }
    return effect;
}

// Only register allocation of the `data` pointer differs. The original coalesces `data` into the loop
// pointer's callee-saved r28; mwcc (GC/2.6 and 1.3.x both) keeps the pre-loop `data` in caller-saved r6
// and the loop pointer in r28 (the live range is not split, it is just coloured r6), which also shifts
// `type`/`i` to r27/r26. The only other diff is that each `if (type == 0)` compiles to `bne else` here vs
// the target's `beq then; b else` (2 op_mismatch instructions). Every other instruction, GPR and FPR
// matches, including the sinusoid, the alpha/scale tables and dispCalcZ.
// NONMATCHING (98.43%): the `data` pointer register (r6 vs r28) and the two `if (type == 0)` branch layouts.
void effAkariPoseMain(EffectEntry* effect) {
    effAkariPoseData* data;
    effAkariPosePart* part;
    Vec position;
    Vec position2;
    s32 type;
    s32 i;
    s32 unk2c;
    f32 velScale;
    f32 lerpScale;
    f32 rad;
    f32 angle;

    data = effect->userdata;
    position = akariMakeVec(data->x, data->y, data->z);
    position2 = position;
    type = data->type;
    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->field_0x28 = 0x10;
    }
    if (data->field_0x28 < 1000) {
        data->field_0x28--;
    }
    data->field_0x2c++;
    if (data->field_0x28 < 0) {
        effDelete(effect);
        return;
    }
    unk2c = data->field_0x2c;
    if (data->field_0x28 < 0x10) {
        data->field_0x30 = data->field_0x28 << 4;
    }
    part = data->part;
    if (type == 0) {
        velScale = float_0p1_8041e8dc;
        lerpScale = float_0p2_8041e8e0;
    } else {
        velScale = float_0p2_8041e8e0;
        lerpScale = velScale;
    }
    for (i = 1; i < effect->count; i++, part++) {
        part->field_0x20++;
        if (part->field_0x20 >= max_seq_num[type]) {
            part->field_0x20 = 0;
        }
        if (part->field_0x20 < 0) {
            continue;
        }
        if (part->field_0x20 == 0) {
            angle = float_360_8041e8e4 * (f32)i / (f32)(effect->count - 1);
            rad = float_6p2832_8041e8e8 * angle / float_360_8041e8e4;
            part->x = float_16_8041e8ec * (f32)sin(rad);
            part->y = float_16_8041e8ec * (f32)cos(rad);
            part->vx = part->x * velScale;
            part->vy = part->y * velScale;
            part->field_0x18 = float_0_8041e8d8;
            part->field_0x1c = float_0_8041e8d8;
            part->field_0x24 = -angle - float_45_8041e8f0;
            part->field_0x34 = float_1_8041e8f4;
        }
        if (type == 0) {
            part->field_0x30 = a_data[part->field_0x20];
            part->field_0x34 = scale_data[part->field_0x20];
        } else {
            part->field_0x30 = a_data2[part->field_0x20];
            part->field_0x34 = scale_data2[part->field_0x20];
        }
        if ((unk2c & 1) == (i & 1)) {
            part->x += part->vx;
            part->y += part->vy;
        } else {
            part->x += float_0p2_8041e8e0 * part->vx;
            part->y += float_0p2_8041e8e0 * part->vy;
        }
        part->vx += lerpScale * (part->field_0x18 - part->vx);
        part->vy += lerpScale * (part->field_0x1c - part->vy);
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effAkariPoseDisp, effect);
}

#ifdef MUST_MATCH
static inline GXColor akariColor(u8 alpha) {
    GXColor c = unk_80423098;
    c.a = alpha;
    return c;
}
#endif

// The original emits `li r31,1` (i = 1) before `addi r30,r30,0x38` (part = data->part) in the
// loop preheader. mwcc (every version from GC/1.3 to GC/2.7 here) always schedules the pointer
// advance first, in every source shape tried (separate statements, for-header comma, array-index
// form, goto-labelled loop, integer-cast address). Turning scheduling off restores the order but
// reorders ~20 other instructions, so it cannot be used.
// The MUST_MATCH `akariColor` inline above fixes the GXColor stack slot the natural assignment
// produces (local at r1+0xc vs the target's r1+0x8; the same quirk as eff_booboo_n64.c).
// NONMATCHING (99.25%): only the preheader `li`/`addi` order differs.
void effAkariPoseDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effAkariPoseData* data;
    effAkariPosePart* part;
    CameraEntry* cam;
    GXTexObj texObj;
    GXColor color;
    Mtx mtx;
    Mtx scaleMtx;
    Mtx rotMtx;
    Mtx viewMtx;
    s32 alpha;
    s32 i;

    cam = camGetPtr(camId);
    data = effect->userdata;
    alpha = data->field_0x30;
    PSMTXTrans(mtx, data->x, data->y, data->z);
    PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041e8d0);
    PSMTXScale(scaleMtx, data->field_0x34, data->field_0x34, data->field_0x34);
    PSMTXConcat(mtx, rotMtx, mtx);
    PSMTXConcat(mtx, scaleMtx, mtx);
    PSMTXConcat(cam->view, mtx, viewMtx);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xff);
    GXSetTevOp(0, 3);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
    PSMTXScale(scaleMtx, float_0p03125_8041e8d4, float_0p03125_8041e8d4, float_0_8041e8d8);
    GXLoadTexMtxImm(scaleMtx, 0x1e, 1);
    effGetTexObjN64(0x76, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);
    effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);
    for (i = 1, part = data->part; i < effect->count; i++, part++) {
        if (part->field_0x20 < 0) {
            continue;
        }
        PSMTXTrans(mtx, part->x, part->y, float_0_8041e8d8);
        PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041e8d0 * part->field_0x24);
        PSMTXScale(scaleMtx, part->field_0x34, part->field_0x34, part->field_0x34);
        PSMTXConcat(mtx, rotMtx, mtx);
        PSMTXConcat(mtx, scaleMtx, mtx);
        PSMTXConcat(viewMtx, mtx, mtx);
        GXLoadPosMtxImm(mtx, 0);
        GXSetCurrentMtx(0);
#ifdef MUST_MATCH
        color = akariColor((alpha * part->field_0x30) >> 8);
#else
        color = unk_80423098;
        color.a = (alpha * part->field_0x30) >> 8;
#endif
        GXSetTevColor(1, color);
        GXBegin(0x90, 0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
