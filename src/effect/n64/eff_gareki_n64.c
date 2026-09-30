#include "eff/n64/eff_gareki_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

// .sdata2 float constants (named after their original addresses).
extern const f32 float_neg0p1_8041f054;
extern const f32 float_0p8_8041f058;
extern const f32 float_neg0p2_8041f05c;
extern const f32 float_0p9_8041f060;
extern const f32 float_10_8041f064;
extern const f32 float_20_8041f068;
extern const f32 float_neg2_8041f06c;
extern const f32 float_0_8041f048;
extern const f32 float_0p25_8041f070;
extern const f32 float_2_8041f074;
extern const f32 float_1_8041f078;
extern const f32 float_0p1_8041f07c;

// .rodata / .data constants (named after their original addresses).
const f64 double_to_int_802fa6e0 = 4503601774854144.0;
extern const char str_802FA6E8[];

// trans_data is a run of (offsetX, offsetY) pairs; typed as a pair struct so the
// loop's induction variable strength-reduces to a pre-biased pointer.
typedef struct GarekiOffset {
    f32 x; // 0x0
    f32 y; // 0x4
} GarekiOffset;

extern GarekiOffset trans_data[];

// .data vertex array, local to this unit in the original.
static u32 stg__s_v[0x348 / 4] = {
	0xFFFAFFFA, 0x00000000, 0x0000FFFF, 0xFFFF0006, 0xFFFA0000, 0x04000000,
	0xFFFFFFFF, 0x00060006, 0x00000400, 0x0600FFFF, 0xFFFFFFFA, 0x00060000,
	0x00000600, 0xFFFFFFFF, 0xFFFA0006, 0x00000000, 0x0C00FFFF, 0xFFFF0006,
	0x00060000, 0x04000C00, 0xFFFFFFFF, 0x0006FFFA, 0x00000400, 0x0600FFFF,
	0xFFFFFFFA, 0xFFFA0000, 0x00000600, 0xFFFFFFFF, 0xFFFFFFFE, 0x00000000,
	0x0000FFFF, 0xFFFF0003, 0xFFFE0000, 0x01550000, 0xFFFFFFFF, 0xFFFF0004,
	0x00000000, 0x0300FFFF, 0xFFFFFFFF, 0x00040000, 0x00000900, 0xFFFFFFFF,
	0x0003FFFE, 0x00000155, 0x0600FFFF, 0xFFFFFFFF, 0xFFFE0000, 0x00000600,
	0xFFFFFFFF, 0x0001FFFC, 0x00000155, 0x0000FFFF, 0xFFFF0003, 0x00020000,
	0x02000300, 0xFFFFFFFF, 0xFFFD0002, 0x00000000, 0x0300FFFF, 0xFFFFFFFD,
	0x00020000, 0x00000900, 0xFFFFFFFF, 0x00030002, 0x00000200, 0x0900FFFF,
	0xFFFF0001, 0xFFFC0000, 0x01550600, 0xFFFFFFFF, 0xFFFCFFFD, 0x00000155,
	0x0000FFFF, 0xFFFF0000, 0xFFFD0000, 0x02AB0000, 0xFFFFFFFF, 0x00040003,
	0x00000400, 0x0300FFFF, 0xFFFFFFFE, 0x00030000, 0x02000300, 0xFFFFFFFF,
	0xFFFE0003, 0x00000200, 0x0900FFFF, 0xFFFF0004, 0x00030000, 0x04000900,
	0xFFFFFFFF, 0x0000FFFD, 0x000002AB, 0x0600FFFF, 0xFFFFFFFC, 0xFFFD0000,
	0x01550600, 0xFFFFFFFF, 0xFFFDFFFE, 0x000002AB, 0x0000FFFF, 0xFFFF0001,
	0xFFFE0000, 0x04000000, 0xFFFFFFFF, 0x00010004, 0x00000400, 0x0300FFFF,
	0xFFFF0001, 0x00040000, 0x04000900, 0xFFFFFFFF, 0x0001FFFE, 0x00000400,
	0x0600FFFF, 0xFFFFFFFD, 0xFFFE0000, 0x02AB0600, 0xFFFFFFFF, 0x0004FFFE,
	0x00000200, 0x0300FFFF, 0xFFFFFFFE, 0x00040000, 0x00000600, 0xFFFFFFFF,
	0xFFFEFFFE, 0x00000000, 0x0300FFFF, 0xFFFFFFFE, 0xFFFE0000, 0x00000900,
	0xFFFFFFFF, 0xFFFE0004, 0x00000000, 0x0C00FFFF, 0xFFFF0004, 0xFFFE0000,
	0x02000900, 0xFFFFFFFF, 0x0001FFFC, 0x00000200, 0x0300FFFF, 0xFFFF0003,
	0x00020000, 0x02AB0600, 0xFFFFFFFF, 0xFFFF0002, 0x00000155, 0x0600FFFF,
	0xFFFFFFFB, 0x00020000, 0x00000600, 0xFFFFFFFF, 0xFFFB0002, 0x00000000,
	0x0C00FFFF, 0xFFFFFFFF, 0x00020000, 0x01550C00, 0xFFFFFFFF, 0x00030002,
	0x000002AB, 0x0C00FFFF, 0xFFFF0001, 0xFFFC0000, 0x02000900, 0xFFFFFFFF,
	0x0003FFFE, 0x00000400, 0x0300FFFF, 0xFFFFFFFF, 0x00040000, 0x02AB0600,
	0xFFFFFFFF, 0xFFFDFFFE, 0x00000200, 0x0300FFFF, 0xFFFFFFFD, 0xFFFE0000,
	0x02000900, 0xFFFFFFFF, 0xFFFF0004, 0x000002AB, 0x0C00FFFF, 0xFFFF0003,
	0xFFFE0000, 0x04000900, 0xFFFFFFFF, 0x0001FFFC, 0x00000400, 0x0300FFFF,
	0xFFFF0001, 0x00020000, 0x04000600, 0xFFFFFFFF, 0xFFFD0002, 0x000002AB,
	0x0600FFFF, 0xFFFFFFFD, 0x00020000, 0x02AB0C00, 0xFFFFFFFF, 0x00010002,
	0x00000400, 0x0C00FFFF, 0xFFFF0001, 0xFFFC0000, 0x04000900, 0xFFFFFFFF,
};
static f32 vx_data[8] = { -1.0f, -0.5f, 0.5f, 1.0f, -1.0f, -0.5f, 0.5f, 1.0f };

typedef struct GarekiChild {
    f32 x;          // 0x00
    f32 y;          // 0x04
    f32 z;          // 0x08
    u8 pad_0C[4];   // 0x0C
    f32 vx;         // 0x10
    f32 vy;         // 0x14
    f32 offsetX;    // 0x18
    f32 offsetY;    // 0x1C
    u8 pad_20[0xC]; // 0x20
    f32 angleX;     // 0x2C
    u8 pad_30[4];   // 0x30
    f32 angleZ;     // 0x34
    u8 pad_38[8];   // 0x38
} GarekiChild; // 0x40

typedef struct GarekiData {
    Vec position;            // 0x00
    f32 groundY;             // 0x0C
    u8 pad_10[4];            // 0x10
    f32 vy;                  // 0x14
    u8 pad_18[8];            // 0x18
    s32 timer;               // 0x20
    s32 state;               // 0x24
    s32 alpha;               // 0x28
    f32 angleX;              // 0x2C
    f32 angleZ;              // 0x30
    f32 field_34;            // 0x34
    f32 angleVelX;           // 0x38
    f32 angleVelZ;           // 0x3C
    GarekiChild children[];  // 0x40
} GarekiData;

// Local copy of system.c's inline helper so it inlines here.
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

// NONMATCHING (99.86%): only the two loop-preheader pointer setups are scheduled in the
// opposite order (target: trans = &trans_data[1], then p = data->children; MWCC swaps them).
EffectEntry* effGarekiN64Entry(s32 type, f32 x, f32 y, f32 z, f32 groundY) {
    EffectEntry* effect;
    GarekiData* data;
    GarekiChild* p;
    GarekiOffset* trans;
    s32 i;

    effect = effEntry();
    effect->type = str_802FA6E8;
    effect->count = 9;
    data = __memAlloc(HEAP_EFFECT, 0x240);
    effect->userdata = data;
    effect->callback = effGarekiMain;
    data->state = 0;
    data->position.x = x;
    data->position.y = y;
    data->groundY = groundY;
    data->position.z = z;
    data->vy = float_neg2_8041f06c;
    data->angleX = float_0_8041f048;
    data->angleZ = float_0_8041f048;
    data->field_34 = (f32)(rand() % 60 - 30);
    data->alpha = 0xff;
    data->angleVelX = (f32)(rand() % 60 - 30);
    data->angleVelZ = float_0p25_8041f070 * data->angleVelX;
    data->timer = 20;

    p = data->children;
    trans = trans_data;
    for (i = 1; i < 9; i++, p++) {
        p->offsetX = trans[i - 1].x;
        p->offsetY = trans[i - 1].y;
        p->x = float_0_8041f048;
        p->y = float_0_8041f048;
        p->z = float_0_8041f048;
        p->vx = float_2_8041f074 * vx_data[i - 1];
        p->vy = float_2_8041f074 * ((f32)(rand() % 10) * float_0p1_8041f07c) + float_1_8041f078;
        p->angleX = float_0_8041f048;
        p->angleZ = float_0_8041f048;
    }
    return effect;
}

void effGarekiMain(EffectEntry* effect) {
    GarekiData* data = effect->userdata;
    Vec position;
    s32 i;

    position = initializeVec(&data->position);

    if (data->state == 0) {
        data->vy += float_neg0p1_8041f054;
        data->position.y += data->vy;
        data->angleX += data->angleVelX;
        data->angleZ += data->angleVelZ;
        if (data->position.y <= data->groundY) {
            data->state = 1;
        }
    }

    if (data->state == 1) {
        data->timer--;
        if (data->timer < 0) {
            effDelete(effect);
            return;
        }
        if (data->timer < 10) {
            data->alpha = (s32)((f32)data->alpha * float_0p8_8041f058);
        }
        data++;
        for (i = 1; i < effect->count; i++, data++) {
            ((GarekiChild*)data)->vy += float_neg0p2_8041f05c;
            ((GarekiChild*)data)->vx *= float_0p9_8041f060;
            ((GarekiChild*)data)->x += ((GarekiChild*)data)->vx;
            ((GarekiChild*)data)->y += ((GarekiChild*)data)->vy;
            ((GarekiChild*)data)->angleX += float_10_8041f064;
            ((GarekiChild*)data)->angleZ += float_20_8041f068;
        }
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effGarekiDisp, effect);
}

// NOTE: the original's switch jump table is the local symbol jumptable_8039D3A0; mwcc emits it as an
// anonymous local @NNN (known pool-naming limitation), which objdiff reports as an arg_mismatch.
void gareki_dl(s32 type) {
    GXSetCullMode(GX_CULL_NONE);
    effSetVtxDescN64((effVtxArray*)stg__s_v);
    switch (type) {
    case 0:
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xC);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
        tri2(4, 5, 6, 0, 4, 6, 7, 0);
        break;
    case 1:
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(8, 9, 10, 8, 11, 12, 13, 8);
        break;
    case 2:
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(14, 15, 16, 14, 17, 18, 19, 14);
        break;
    case 3:
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xC);
        tri2(20, 21, 22, 20, 20, 22, 23, 20);
        tri2(24, 25, 26, 20, 24, 26, 27, 20);
        break;
    case 4:
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(28, 29, 30, 28, 31, 32, 33, 28);
        break;
    case 5:
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(34, 35, 36, 34, 37, 38, 39, 34);
        break;
    case 6:
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xC);
        tri2(40, 41, 42, 40, 40, 42, 43, 40);
        tri2(44, 45, 46, 40, 44, 46, 47, 40);
        break;
    case 7:
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(48, 49, 50, 48, 51, 52, 53, 48);
        break;
    case 8:
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(54, 55, 56, 54, 57, 58, 59, 54);
        break;
    }
}

