#include "eff/eff_ultra_hammer.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"
#include "seq/seq_title.h"
#include "pmario_sound.h"
#include <demo/DEMOInit.h>
#include <dolphin/os.h>
#include <dolphin/base/PPCArch.h>
#include <dolphin/vi.h>
#include <string.h>
#include <stdio.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
// The code and the linker's only handle on the tables below is this anchor, so the compiler-emitted
// pairs and the arrays they reach are force-kept here.
#pragma force_active on
__declspec(section ".data") u8 lbl_803B70C0[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };
// The compiler emitted four { -1.0f, 1.0f } / { 1.0f, -1.0f } pairs between the anchor and
// wakka_vertex_tbl (the local names drop the compiler's $); the code reaches the tables that
// follow through lbl_803B70C0, so the pairs are what puts wakka_vertex_tbl at +0x40.
__declspec(section ".data") static u8 negone_one_374[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };
__declspec(section ".data") u8 lbl_803B70D0[8] = { 0x3F, 0x80, 0x00, 0x00, 0xBF, 0x80, 0x00, 0x00 };
__declspec(section ".data") static u8 one_negone_373[8] = { 0x3F, 0x80, 0x00, 0x00, 0xBF, 0x80, 0x00, 0x00 };
__declspec(section ".data") u8 lbl_803B70E0[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };
__declspec(section ".data") static u8 negone_one_360[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };
__declspec(section ".data") u8 lbl_803B70F0[8] = { 0x3F, 0x80, 0x00, 0x00, 0xBF, 0x80, 0x00, 0x00 };
__declspec(section ".data") static u8 one_negone_359[8] = { 0x3F, 0x80, 0x00, 0x00, 0xBF, 0x80, 0x00, 0x00 };
#pragma force_active reset


#ifdef __MWERKS__
#pragma force_active on
// Original .sdata order: these two 32-byte tables come first, then the twenty display lists.
s8 wakka_normal_tbl[32] = {
	0, 64, 0, // 0
};

u8 wakka_color0_tbl[32] = {
	0xFF, 0xFF, 0xFF, 0x7F, // 0
	0xFF, 0xFF, 0xFF, 0x00  // 1
};

u8 wakka_dl_0_0[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	1, 0, 0, 2, // 0
	0, 0, 0, 3, // 1
	21, 0, 0, 1, // 2
	20, 0, 0, 0, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_1[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	2, 0, 0, 5, // 0
	1, 0, 0, 2, // 1
	22, 0, 0, 4, // 2
	21, 0, 0, 1, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_2[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	3, 0, 0, 7, // 0
	2, 0, 0, 5, // 1
	23, 0, 0, 6, // 2
	22, 0, 0, 4, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_3[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	39, 0, 0, 38, // 0
	20, 0, 0, 0, // 1
	19, 0, 0, 39, // 2
	0, 0, 0, 3, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_4[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	38, 0, 0, 36, // 0
	39, 0, 0, 38, // 1
	18, 0, 0, 37, // 2
	19, 0, 0, 39, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_5[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	37, 0, 0, 34, // 0
	38, 0, 0, 36, // 1
	17, 0, 0, 35, // 2
	18, 0, 0, 37, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_6[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	36, 0, 0, 32, // 0
	37, 0, 0, 34, // 1
	16, 0, 0, 33, // 2
	17, 0, 0, 35, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_7[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	35, 0, 0, 30, // 0
	36, 0, 0, 32, // 1
	15, 0, 0, 31, // 2
	16, 0, 0, 33, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_8[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	34, 0, 0, 28, // 0
	35, 0, 0, 30, // 1
	14, 0, 0, 29, // 2
	15, 0, 0, 31, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_9[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	33, 0, 0, 26, // 0
	34, 0, 0, 28, // 1
	13, 0, 0, 27, // 2
	14, 0, 0, 29, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_10[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	32, 0, 0, 24, // 0
	33, 0, 0, 26, // 1
	12, 0, 0, 25, // 2
	13, 0, 0, 27, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_11[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	31, 0, 0, 22, // 0
	32, 0, 0, 24, // 1
	11, 0, 0, 23, // 2
	12, 0, 0, 25, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_12[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	30, 0, 0, 20, // 0
	31, 0, 0, 22, // 1
	10, 0, 0, 21, // 2
	11, 0, 0, 23, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_13[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	29, 0, 0, 18, // 0
	30, 0, 0, 20, // 1
	9, 0, 0, 19, // 2
	10, 0, 0, 21, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_14[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	28, 0, 0, 16, // 0
	29, 0, 0, 18, // 1
	8, 0, 0, 17, // 2
	9, 0, 0, 19, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_15[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	27, 0, 0, 14, // 0
	28, 0, 0, 16, // 1
	7, 0, 0, 15, // 2
	8, 0, 0, 17, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_16[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	26, 0, 0, 12, // 0
	27, 0, 0, 14, // 1
	6, 0, 0, 13, // 2
	7, 0, 0, 15, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_17[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	25, 0, 0, 9, // 0
	26, 0, 0, 12, // 1
	5, 0, 0, 10, // 2
	6, 0, 0, 13, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_18[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	24, 0, 0, 8, // 0
	25, 0, 0, 9, // 1
	4, 0, 0, 11, // 2
	5, 0, 0, 10, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_19[] = {
	(GX_DRAW_TRIANGLE_STRIP | GX_VTXFMT0), // command
	0, 4, //number of verts
	41, 0, 1, 41, // 0
	3, 0, 0, 7, // 1
	40, 0, 1, 40, // 2
	23, 0, 0, 6, // 3
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP,
	GX_NOP, GX_NOP, GX_NOP, GX_NOP, GX_NOP, 
};

u8 wakka_dl_0_size_tbl[] = {
	1, // 0
	1, // 1
	1, // 2
	1, // 3
	1, // 4
	1, // 5
	1, // 6
	1, // 7
	1, // 8
	1, // 9
	1, // 10
	1, // 11
	1, // 12
	1, // 13
	1, // 14
	1, // 15
	1, // 16
	1, // 17
	1, // 18
	1, // 19
};

// Trailing zero word: the section ends at the original offset before the next unit's `.sdata`.
#pragma explicit_zero_data on
__declspec(section ".sdata") static u32 gap_07_80418194_sdata[1] = { 0 };
#pragma explicit_zero_data reset

// Explicit 128-element size: the original's object covers the 252 bytes of vertex data plus the
// two zero s16 that pad it to the 32-byte alignment.
s16 wakka_vertex_tbl[128] = {
	24347, 0, -8825, // 0
	20710, 0, -15961, // 1
	15047, 0, -21625, // 2
	7910, 0, -25261, // 3
	0, 0, -26514, // 4
	-7910, 0, -25261, // 5
	-15047, 0, -21625, // 6
	-20710, 0, -15961, // 7
	-24347, 0, -8825, // 8
	-25600, 0, -914, // 9
	-24347, 0, 6996, // 10
	-20710, 0, 14133, // 11
	-15047, 0, 19796, // 12
	-7910, 0, 23432, // 13
	0, 0, 24685, // 14
	7910, 0, 23432, // 15
	15047, 0, 19796, // 16
	20710, 0, 14133, // 17
	24347, 0, 6996, // 18
	25600, 0, -914, // 19
	12173, 0, -4869, // 20
	10355, 0, -8437, // 21
	7523, 0, -11269, // 22
	3955, 0, -13087, // 23
	0, 0, -13714, // 24
	-3955, 0, -13087, // 25
	-7523, 0, -11269, // 26
	-10355, 0, -8437, // 27
	-12173, 0, -4869, // 28
	-12800, 0, -914, // 29
	-12173, 0, 3041, // 30
	-10355, 0, 6609, // 31
	-7523, 0, 9441, // 32
	-3955, 0, 11259, // 33
	0, 0, 11885, // 34
	3955, 0, 11259, // 35
	7523, 0, 9441, // 36
	10355, 0, 6609, // 37
	12173, 0, 3041, // 38
	12800, 0, -914, // 39
	0, 0, -13714, // 40
	0, 0, -26514, // 41
};

s16 wakka_texcoord0_tbl[] = {
	24576, 0, // 0
	25600, 0, // 1
	25600, 8192, // 2
	24576, 8192, // 3
	26624, 0, // 4
	26624, 8192, // 5
	27648, 0, // 6
	27648, 8192, // 7
	8192, 0, // 8
	9216, 0, // 9
	9216, 8192, // 10
	8192, 8192, // 11
	10240, 0, // 12
	10240, 8192, // 13
	11264, 0, // 14
	11264, 8192, // 15
	12288, 0, // 16
	12288, 8192, // 17
	13312, 0, // 18
	13312, 8192, // 19
	14336, 0, // 20
	14336, 8192, // 21
	15360, 0, // 22
	15360, 8192, // 23
	16384, 0, // 24
	16384, 8192, // 25
	17408, 0, // 26
	17408, 8192, // 27
	18432, 0, // 28
	18432, 8192, // 29
	19456, 0, // 30
	19456, 8192, // 31
	20480, 0, // 32
	20480, 8192, // 33
	21504, 0, // 34
	21504, 8192, // 35
	22528, 0, // 36
	22528, 8192, // 37
	23552, 0, // 38
	23552, 8192, // 39
	28672, 0, // 40
	28672, 8192, // 41
};

void* wakka_dl_0_tbl[] = {
	wakka_dl_0_0, // 0
	wakka_dl_0_1, // 1
	wakka_dl_0_2, // 2
	wakka_dl_0_3, // 3
	wakka_dl_0_4, // 4
	wakka_dl_0_5, // 5
	wakka_dl_0_6, // 6
	wakka_dl_0_7, // 7
	wakka_dl_0_8, // 8
	wakka_dl_0_9, // 9
	wakka_dl_0_10, // 10
	wakka_dl_0_11, // 11
	wakka_dl_0_12, // 12
	wakka_dl_0_13, // 13
	wakka_dl_0_14, // 14
	wakka_dl_0_15, // 15
	wakka_dl_0_16, // 16
	wakka_dl_0_17, // 17
	wakka_dl_0_18, // 18
	wakka_dl_0_19, // 19
};
#else
u8 wakka_dl_0_0[];
u8 wakka_dl_0_1[];
u8 wakka_dl_0_2[];
u8 wakka_dl_0_3[];
u8 wakka_dl_0_4[];
u8 wakka_dl_0_5[];
u8 wakka_dl_0_6[];
u8 wakka_dl_0_7[];
u8 wakka_dl_0_8[];
u8 wakka_dl_0_9[];
u8 wakka_dl_0_10[];
u8 wakka_dl_0_11[];
u8 wakka_dl_0_12[];
u8 wakka_dl_0_13[];
u8 wakka_dl_0_14[];
u8 wakka_dl_0_15[];
u8 wakka_dl_0_16[];
u8 wakka_dl_0_17[];
u8 wakka_dl_0_18[];
u8 wakka_dl_0_19[];
void* wakka_dl_0_tbl[];
u8 wakka_dl_0_size_tbl[];
s16 wakka_vertex_tbl[];
s8 wakka_normal_tbl[];
u32 wakka_color0_tbl[];
s16 wakka_texcoord0_tbl[];
#endif
#pragma force_active reset

//-3.5 / 24 * i (up to 25) with some imprecision
f32 scroll_tbl[] = {
	0.0f, -0.14583334f, -0.29166669f, -0.43750003f, -0.58333337f,
	-0.72916669f, -0.87500006f, -1.0208334f, -1.1666667f, -1.3125f,
	-1.4583334f, -1.6041666f, -1.75f, -1.8958334f, -2.0416667f,
	-2.1875f, -2.3333333f, -2.4791667f, -2.625f, -2.7708333f,
	-2.9166667f, -3.0625f, -3.2083333f, -3.3541667f, -3.5f
};

// Trailing zero word: the section ends at the original offset (the next unit's jump tables follow).
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") static u32 gap_05_803B735C_data[1] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset


EffectEntry* effUltraHammerEntry(BOOL arg0, s32 counter, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect;
    EffUltraHammerData* data;
    s32 i;

    effect = effEntry();
    effect->type = "UltraHammer";
    effect->count = 4;
    data = __memAlloc(HEAP_EFFECT, sizeof(EffUltraHammerData) * 4);
    effect->userdata = data;
    effect->callback = effUltraHammerMain;
    data->flipX = arg0;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->scale = scale;
    if (counter <= 0) {
        data->counter = 1000;
    }
    else {
        data->counter = counter;
    }
    data->alpha = 0xFF;
    data->scroll = 0;
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        s32 updates[3] = {0, 30, 15};
        data->position.x = 0.0f;
        data->position.y = -4.0f * (f32)(i - 1);
        data->position.z = 0.0f;
        data->scale = 1.0f;
        data->update = updates[i - 1];
        data->scroll = 0;
        data->counter = 0;
    }
    return effect;
}

void effUltraHammerMain(EffectEntry* effect) {
    EffUltraHammerData* data;
    Vec position;
    int i;

    data = effect->userdata;
    position = initializeVec(&data->position);
    if (data->counter < 1000) {
        data->counter--;
    }
    if (data->counter < 16) {
        data->alpha = data->counter * 16;
    }
    if (data->counter < 0) {
        effDelete(effect);
        return;
    }
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->update != 0) {
            data->update--;
        } else {
            if (data->scroll < 25) {
                data->offsetX = scroll_tbl[data->scroll];
                data->scale = 3.5f;
                data->scroll += 1;
            } else {
                data->update = 15;
                data->scroll = 0;
            }
        }
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effUltraHammerDisp, effect);
}

// unknown function, is used for many functions to "initialize"/copy some value into the local stack, with an extra vec
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}


// Splitting the 20-entry display-list loop into this inline function gives it the original's
// callee-saved colouring for the loop (dlptr/size_tbl/j in r27/r28/r29); inlined at the call site
// the compiler instead colours them r28/r29/r27.
static inline void wakkaDrawDL(void) {
    u32 j;
    for (j = 0; j < 20; j++) {
        GXCallDisplayList(wakka_dl_0_tbl[j], wakka_dl_0_size_tbl[j] * 32);
    }
}

void effUltraHammerDisp(enum CameraId camId, void *param) {
    EffectEntry *effect;
    CameraEntry *temp_r31;
    EffUltraHammerData *data;
    int i;
    s32 flipX;
    Mtx spC0;
    Mtx sp90;
    Mtx sp60;
    Mtx sp30;
    GXTexObj sp10;
    
    effect = param;
    data = effect->userdata;
    temp_r31 = camGetPtr(camId);
    flipX = data->flipX;
    
    PSMTXTrans(spC0, data->position.x, data->position.y, data->position.z);
    PSMTXScale(sp60, data->scale, data->scale, data->scale);
    PSMTXRotRad(sp90, 'y', MTXDegToRad(-camGetPtr(camId)->viewYaw));
    PSMTXConcat(spC0, sp90, spC0);
    PSMTXConcat(spC0, sp60, sp30);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, 0, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(2);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 30, 0, 125);
    effGetTexObj(145, &sp10);
    GXLoadTexObj(&sp10, GX_TEXMAP0);
    GXSetCullMode(GX_CULL_NONE);
    GXSetTevColor(GX_TEVREG0, (GXColor){0xFF, 0xFF, 0xFF, data->alpha});
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->update == 0) {
            PSMTXTrans(spC0, data->position.x, data->position.y, data->position.z);
            PSMTXScale(sp60, 0.6f * data->scale, 0.6f * data->scale, 0.6f * data->scale);
            PSMTXConcat(spC0, sp60, spC0);
            if (flipX != 0) {
                PSMTXScale(sp60, -1.0f, 1.0f, 1.0f);
                PSMTXConcat(spC0, sp60, spC0);
            }
            PSMTXConcat(sp30, spC0, spC0);
            PSMTXConcat(temp_r31->view, spC0, spC0);
            GXLoadPosMtxImm(spC0, 0);
            GXSetCurrentMtx(0);
            PSMTXTrans(spC0, data->offsetX, data->offsetY, 0.0f);
            GXLoadTexMtxImm(spC0, 30, GX_MTX2x4);
            GXClearVtxDesc();
            GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_TEX_ST, GX_RGBA4, 10);
            GXSetArray(GX_VA_POS, wakka_vertex_tbl, 6);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_TEX_S, GX_RGB8, 6);
            GXSetArray(GX_VA_NRM, &wakka_normal_tbl, 3);
            GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_TEX_ST, GX_RGBA8, 0);
            GXSetArray(GX_VA_CLR0, &wakka_color0_tbl, 4);
            GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
            GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_RGBA4, 13);
            GXSetArray(GX_VA_TEX0, wakka_texcoord0_tbl, 4);
            wakkaDrawDL();
        }
    }
}
