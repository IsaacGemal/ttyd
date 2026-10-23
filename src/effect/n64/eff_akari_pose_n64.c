#include "eff/n64/eff_akari_pose_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

#include "math_ext.h"
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

// .rodata string and the pooled int->float conversion double (named after their original addresses).
// The compiler only references this by name if it is non-const; a const definition is constant-folded
// to an anonymous @NNN literal, which no longer matches the original `lfd ... double_to_int_802fa1c8`.

// .sdata sequence-length table indexed by the effect type (local to this file in the original).
static s32 max_seq_num[2] = {0x18, 0x0C};

// .data per-frame scale tables, local to this file in the original.
static f32 scale_data[24] = {
    0.9f, 1.0f, 1.0f, 0.9f, 0.85f, 0.8f, 0.75f, 0.7f, 0.65f, 0.6f, 0.55f, 0.5f,
    0.45f, 0.4f, 0.35f, 0.3f, 0.25f, 0.2f, 0.15f, 0.1f, 0.05f, 0.03f, 0.02f, 0.01f,
};
static f32 scale_data2[24] = {
    0.9f, 1.0f, 1.0f, 0.9f, 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
};

// .data vertex array, local to this file (0x38 bytes).
static u32 size16x16_tex32x32_vtx[0x38 / 4] = {
    0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008, 0xFFF80000, 0x04000000, 0x000000FF,
    0x00080008, 0x00000400, 0x04000000, 0x00FFFFF8, 0x00080000, 0x00000400, 0x000000FF,
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
    f32 targetVx;      // 0x18
    f32 targetVy;      // 0x1C
    s32 seqIndex;      // 0x20
    f32 angle;         // 0x24
    u8 pad_0x28[0x8];  // 0x28
    s32 alpha;         // 0x30
    f32 scale;         // 0x34
} effAkariPosePart;

typedef struct effAkariPoseData {
    s32 type;                // 0x00
    f32 x;                   // 0x04
    f32 y;                   // 0x08
    f32 z;                   // 0x0C
    u8 pad_0x10[0x18];       // 0x10
    s32 life;                // 0x28
    s32 frame;               // 0x2C
    s32 alpha;               // 0x30
    f32 scale;               // 0x34
    effAkariPosePart part[]; // 0x38
} effAkariPoseData;

EffectEntry* effAkariPoseN64Entry(s32 type, s32 count, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    effAkariPoseData* data;
    effAkariPosePart* part;
    s32 i;
    f32 seqStep;
    s32 seqNum;

    effect = effEntry();
    effect->type = "AkariPoseN64";
    effect->count = count + 1;
    data = __memAlloc(HEAP_EFFECT, (count + 1) * 0x38);
    effect->userdata = data;
    effect->callback = effAkariPoseMain;
    effect->flags |= 2;

    data->type = type;
    data->frame = 0;
    if (lifetime <= 0) {
        data->life = 1000;
    } else {
        data->life = lifetime;
    }
    data->alpha = 0xff;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = scale;

    seqNum = max_seq_num[type];
    seqStep = seqNum / 2.0f;
    for (i = 1, part = data->part; i < count + 1; i++, part++) {
        part->seqIndex = (s32)(-1.0f - seqStep * (f32)(i & 1));
    }
    return effect;
}

// The original reuses the header pointer as the parts cursor: `data++` steps it past the 0x38-byte header
// and the loop steps it once per part, so the part cursor is `data` cast to the part type. Keeping `data`
// live across the loop also colours it into the target's callee-saved r28 (with `i`/`type` in r27/r26); a
// separate `part = data->part` cursor leaves the pre-loop `data` in caller-saved r6 instead.
// Both type tests are written as `switch (type) { case 0: ...; break; default: ...; }` because that makes
// mwcc emit the target's `beq then; b else` layout; a plain `if (type == 0) ... else ...` comes out as
// `bne else` and mismatches two branches per test.
void effAkariPoseMain(EffectEntry* effect) {
    effAkariPoseData* data;
    Vec position;
    Vec position2;
    s32 i;
    s32 type;
    s32 frame;
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
        data->life = 0x10;
    }
    if (data->life < 1000) {
        data->life--;
    }
    data->frame++;
    if (data->life < 0) {
        effDelete(effect);
        return;
    }
    frame = data->frame;
    if (data->life < 0x10) {
        data->alpha = data->life << 4;
    }
    data++;
    switch (type) {
    case 0:
        velScale = 0.1f;
        lerpScale = 0.2f;
        break;
    default:
        velScale = 0.2f;
        lerpScale = velScale;
        break;
    }
    for (i = 1; i < effect->count; i++, data++) {
        effAkariPosePart* part = (effAkariPosePart*)data;
        part->seqIndex++;
        if (part->seqIndex >= max_seq_num[type]) {
            part->seqIndex = 0;
        }
        if (part->seqIndex < 0) {
            continue;
        }
        if (part->seqIndex == 0) {
            angle = 360.0f * (f32)i / (f32)(effect->count - 1);
            rad = 6.2831855f * angle / 360.0f;
            part->x = 16.0f * (f32)sin(rad);
            part->y = 16.0f * (f32)cos(rad);
            part->vx = part->x * velScale;
            part->vy = part->y * velScale;
            part->targetVx = 0.0f;
            part->targetVy = 0.0f;
            part->angle = -angle - 45.0f;
            part->scale = 1.0f;
        }
        switch (type) {
        case 0:
            part->alpha = a_data[part->seqIndex];
            part->scale = scale_data[part->seqIndex];
            break;
        default:
            part->alpha = a_data2[part->seqIndex];
            part->scale = scale_data2[part->seqIndex];
            break;
        }
        if ((frame & 1) == (i & 1)) {
            part->x += part->vx;
            part->y += part->vy;
        } else {
            part->x += 0.2f * part->vx;
            part->y += 0.2f * part->vy;
        }
        part->vx += lerpScale * (part->targetVx - part->vx);
        part->vy += lerpScale * (part->targetVy - part->vy);
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effAkariPoseDisp, effect);
}

// The original reuses the header pointer as the parts cursor: `data++` steps it past the 0x38-byte
// header and the loop steps it once per part, so the `part` cursor is `data` cast to the part type.
// With a separate `part = data->part` pointer mwcc schedules the preheader `addi r30,r30,0x38`
// before `li r31,1` (99.25%); reusing `data` reproduces the original's `li`-then-`addi` order.
void effAkariPoseDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effAkariPoseData* data;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx mtx;
    Mtx scaleMtx;
    Mtx rotMtx;
    Mtx viewMtx;
    s32 alpha;
    s32 i;

    cam = camGetPtr(camId);
    data = effect->userdata;
    alpha = data->alpha;
    PSMTXTrans(mtx, data->x, data->y, data->z);
    PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
    PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
    PSMTXConcat(mtx, rotMtx, mtx);
    PSMTXConcat(mtx, scaleMtx, mtx);
    PSMTXConcat(cam->view, mtx, viewMtx);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xff);
    GXSetTevOp(0, 3);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
    PSMTXScale(scaleMtx, 0.03125f, 0.03125f, 0.0f);
    GXLoadTexMtxImm(scaleMtx, 0x1e, 1);
    effGetTexObjN64(0x76, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);
    effSetVtxDescN64(size16x16_tex32x32_vtx);
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        effAkariPosePart* part = (effAkariPosePart*)data;
        if (part->seqIndex < 0) {
            continue;
        }
        PSMTXTrans(mtx, part->x, part->y, 0.0f);
        PSMTXRotRad(rotMtx, 'z', 0.017453292f * part->angle);
        PSMTXScale(scaleMtx, part->scale, part->scale, part->scale);
        PSMTXConcat(mtx, rotMtx, mtx);
        PSMTXConcat(mtx, scaleMtx, mtx);
        PSMTXConcat(viewMtx, mtx, mtx);
        GXLoadPosMtxImm(mtx, 0);
        GXSetCurrentMtx(0);
        GXSetTevColor(1, (GXColor){ 0, 0, 0, (alpha * part->alpha) >> 8 });
        GXBegin(0x90, 0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}

// 16 zero bytes that end the unit's .sdata: an all-zero object needs explicit_zero_data to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80412D70_sdata[4] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset
