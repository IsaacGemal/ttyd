/*
 * Warning, inlining hell
 */
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "sdk/DEMOInit.h"
#include "himo.h"
#include "driver/mapdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include "math_trig.h"
#include "math_ext.h"
#define fabs(x) __fabs(x)

#include <stdio.h>

#include <string.h>

// Data objects defined later in this file or in other units, declared before their first use.
extern GXColor dat_80419a2c;
extern GXColor dat_80419a30;
extern u32 zero_80419a50;
extern const char str_80419A54[];
extern const char str_80419A5C[];

#define TESTHEAP_SIZE (u32)(1.5 * 1024 * 1024)
#define PI 3.14159265358979323846264338327950288419716939937510f

extern GlobalWork* gp;

// The header's AnimationPoseData padding at 0x1A0 is 4 bytes short: the original
// binary has groups at 0x1A8 and mpAnims at 0x1AC. Work around it locally.
#define ANIM_POSE_DATA_GROUPS(data) (*(AnimPoseGroup**)((u8*)(data) + 0x1A8))
#define ANIM_POSE_DATA_ANIMS(data) (*(AnimTableEntry**)((u8*)(data) + 0x1AC))

//.data
u32 _vivihimoData[0x300] ATTRIBUTE_ALIGN(32) = {
    0x00000000, 0x3F4CCCCD, 0x3F99999A, 0x3F4CCCCD, 0x00000000, 0x00000000, 0x3F4CCCCD, 0x3F333333,
    0x3F000000, 0x3F333333, 0x3ECCCCCD, 0x3F333333, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x3E4CCCCD, 0x3F800000, 0x00000000, 0x00000000,
    0x3E4CCCCD, 0x3F4CCCCD, 0x00000000, 0x3F4CCCCD, 0x00000000, 0x3F4CCCCD, 0x00000000, 0x00000000,
    0x00000000, 0x3F4CCCCD, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x3F4CCCCD, 0x00000000, 0x3F4CCCCD, 0x00000000, 0x3F4CCCCD, 0x3F800000, 0x3F800000,
    0x00000000, 0x3F4CCCCD, 0x3F666666, 0x3F333333, 0x3F19999A, 0x3F333333, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
    0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000,
};

MoamoaChainEntry g_moamoa_chain_tbl[47] = {
    { "joint_00", "jointdai1_A01" },
    { "jointdai1_A01", "jointdai1_A02" },
    { "jointdai1_A02", "jointdai1_A03" },
    { "jointdai1_A03", "jointdai1_A04" },
    { "jointdai1_A04", "jointdai1_A05" },
    { "joint_00", "jointdai1_B01" },
    { "jointdai1_B01", "jointdai1_B02" },
    { "jointdai1_B02", "jointdai1_B03" },
    { "jointdai1_B03", "jointdai1_B04" },
    { "jointdai1_B04", "jointdai1_B05" },
    { "joint_00", "jointdai1_C01" },
    { "jointdai1_C01", "jointdai1_C02" },
    { "jointdai1_C02", "jointdai1_C03" },
    { "jointdai1_C03", "jointdai1_C04" },
    { "jointdai1_C04", "jointdai1_C05" },
    { "joint_00", "jointdai2_01" },
    { "jointdai2_01", "jointdai2_02" },
    { "jointdai2_02", "jointdai2_03" },
    { "jointdai2_03", "jointdai2_04" },
    { "jointdai2_04", "jointdai2_05" },
    { "jointdai2_05", "jointdai2_06" },
    { "jointdai2_06", "jointdai2_07" },
    { "jointdai2_07", "jointdai2_upper01" },
    { "jointdai2_upper01", "jointdai2_upper02" },
    { "jointdai2_upper02", "jointdai2_upper03" },
    { "jointdai2_upper03", "jointdai2_upper04" },
    { "jointdai2_07", "jointdai2_lower01" },
    { "jointdai2_lower01", "jointdai2_lower02" },
    { "jointdai2_lower02", "jointdai2_lower03" },
    { "jointdai2_lower03", "jointdai2_lower04" },
    { "joint_00", "jointasi_A01" },
    { "jointasi_A01", "jointasi_A02" },
    { "jointasi_A02", "jointasi_A03" },
    { "jointasi_A03", "jointasi_A04" },
    { "joint_00", "jointasi_B01" },
    { "jointasi_B01", "jointasi_B02" },
    { "jointasi_B02", "jointasi_B03" },
    { "jointasi_B03", "jointasi_B04" },
    { "joint_00", "jointasi_C01" },
    { "jointasi_C01", "jointasi_C02" },
    { "jointasi_C02", "jointasi_C03" },
    { "jointasi_C03", "jointasi_C04" },
    { "joint_00", "jointasi_D01" },
    { "jointasi_D01", "jointasi_D02" },
    { "jointasi_D02", "jointasi_D03" },
    { "jointasi_D03", "jointasi_D04" },
    { "joint_00", "joint_core" },
}; // filled in at runtime from a/vivian.bin
__declspec(section ".rodata") Vec vec3_802c2db0[1] = { { 0.0f, 0.0f, 0.0f } };
__declspec(section ".rodata") Vec vec3_802c2dbc[1] = { { 0.0f, 0.0f, 0.0f } };
__declspec(section ".rodata") Vec vec3_802c2dc8[1] = { { 0.0f, 0.0f, 0.0f } };
__declspec(section ".rodata") Vec vec3_802c2dd4[1] = { { 0.0f, 0.0f, 0.0f } };

// An ordinary, immediately generated helper pools the unsigned-int->float conversion double
// before the deferred functions' strings. Nothing calls it; the linker strips the body.
#ifdef MUST_MATCH
#pragma defer_codegen off
static f32 animdrv_pool_i2f_mask(u32 v) {
    return (f32)v;
}
#pragma defer_codegen reset
#endif

//.bss
static AnimationWork work;
f32 tanfTbl[0xB2];

//.sdata
s32 vivihimoTailGroupNo[4] ATTRIBUTE_ALIGN(32) = { 0, 2, -1, -1 };
void* vivihimoData[4] = {
    NULL,
    &_vivihimoData,
    (u8*)_vivihimoData + 0x400,
    (u8*)_vivihimoData + 0x800,
};
static AnimationWork* wp = &work;
s32 _dummy = -1;
static s32 tbl[4] = { 2, 1, 3, 0 };

//.sbss
u32 anim_ext_tex_size;
Mtx mtx; // TODO: make sure it's put in .sbss
s32 g_modeling_mtx_lv;
Mtx* g_modeling_mtx;
Mtx g_modeling_mtx_stack[32];

// .sdata2 constants this unit references by name in retail
extern const f32 float_0_80419a48;
extern const f32 float_1_80419a44;
extern const f32 float_0p04_80419ac0;
extern const f32 float_0p5_80419a64;
extern const f32 float_8_80419a70;
extern const f32 float_25p74_80419a74;
extern const f32 float_0p14881_80419a78;
extern const f32 float_150_80419a7c;
extern const f32 float_215_80419a80;
extern const f32 float_0p25_80419a6c;
extern const f32 float_0p75_80419a68;
extern const f64 double_to_int_mask_802c2de0;
extern const f64 double_to_int_802c2df0;
extern const f32 float_neg1_80419a8c;
extern const f32 float_deg2rad_80419a4c;
extern const f32 float_neg0p5_80419a88;
extern const f32 float_neg2_80419ab8;
extern const f32 float_neg3_80419abc;
extern const f32 float_neg90_80419a90;
extern const f32 float_rad2deg_80419a94;
extern const f32 float_360_80419a98;
extern const f32 float_neg360_80419a9c;
extern const f32 float_90_80419aa0;
extern const f32 float_270_80419aa4;
extern const f32 float_neg270_80419aa8;
extern const f32 float_180_80419aac;
extern const f32 float_neg180_80419ab0;
extern const f32 float_0p1_80419ab4;


// local declarations

AnimationWork* animGetPtr(void) {
    return wp;
}

OSTime animTimeGetTime(BOOL globalTime) {
    if (globalTime == FALSE) {
        return OSTicksToMilliseconds(gp->renderFieldTime);
    }
    return OSTicksToMilliseconds(gp->renderTime);
}

void initTestHeap(void) {
    if (!wp->testHeap) { // allocate 1.5MiB test heap
        wp->testHeap = __memAlloc(HEAP_DEFAULT, TESTHEAP_SIZE);
    }
    wp->testAlloc = (u32)wp->testHeap;
}

// heavily inlined
void* testAlloc(u32 size) {
    void* alloc = (void*)wp->testAlloc;

    if (size & 31) {
        size += 32 - (size & 31);
    }
    wp->testAlloc += size;
    if ((u32)wp->testAlloc >= (u32)wp->testHeap + TESTHEAP_SIZE) { // need to allocate more
        animPoseRefresh();
        alloc = testAlloc(size);
    }
    return alloc;
}

void animInit(void) {
    int i;
    FileEntry* file;

    wp->totalFiles = 64; // TODO: #define these?
    wp->totalTextures = 64;
    wp->totalPoses = 256;
    wp->mFloatScratchRP = 1024;
    wp->mFloatScratchWP = 0;
    wp->files = __memAlloc(HEAP_DEFAULT, sizeof(AnimationPoseFile) * wp->totalFiles);
    wp->textures = __memAlloc(HEAP_DEFAULT, sizeof(AnimationTextureFile) * wp->totalTextures);
    wp->poses = __memAlloc(HEAP_DEFAULT, sizeof(AnimationPose) * wp->totalPoses);
    wp->mbUseFloatScratch = 0;
    wp->mpFloatScratch = __memAlloc(HEAP_DEFAULT, sizeof(Vec) * wp->mFloatScratchRP);

    wp->mpTexObjs[0] = NULL;
    wp->mpTexObjs[1] = NULL;
    wp->mpCurTexObj = NULL;

    wp->mpTexDatas[0] = NULL;
    wp->mpTexDatas[1] = NULL;
    wp->mpCurTexData = NULL;

    wp->paperMtxPtr[0] = NULL;
    wp->paperMtxPtr[1] = NULL;
    wp->paperMtxPtr[2] = NULL;

    wp->field_0xD8 = 1;
    wp->field_0xDC = 1;

    wp->testHeap = NULL;
    wp->inBattle = FALSE;
    initTestHeap(); // inlined

    for (i = 0; i < 0xB2; i++) {
        tanfTbl[i] = (f32)tan((i - 0x59) * (3.141592f / 180.0f));
    }
    file = fileAllocf(0, "a/vivian.bin");
    if (file) {
        memcpy_as4(vivihimoData[1], *file->data, sizeof(_vivihimoData));
        memcpy_as4(vivihimoTailGroupNo, (void*)((u32)*file->data + sizeof(_vivihimoData)), sizeof(vivihimoTailGroupNo));
        fileFree(file);
    }
    fileSetCurrentArchiveType(0);
    wp->mp_ag2tg = fileAllocf(0, "a/ag2tg.bin");
}

void animMain(void) {
    dispEntry(CAMERA_OFFSCREEN, 0.0f, 1, animPaperPoseDisp, NULL);
}

inline AnimationPoseData* animPoseGetBaseData(AnimationPose* pose) {
    return *wp->files[pose->fileId].handle->data;
}

#define animPoseGetBaseData(pose) (*(AnimationPoseData**)(wp->files[(pose)->fileId].handle->data))

// heavily inlined, TODO helper function for that AnimPoseData* data, copy of animPoseGetAnimBaseDataPtr with AnimPose*
// arg
void animPose_AllocBuffer(AnimationPose* pose) {
    pose->mpBufferVtxPos = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferPosNum);
    pose->mpVtxArrayPos = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferPosNum);

    pose->mpBufferVtxNrm = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferNrmNum);
    pose->mpVtxArrayNrm = testAlloc(sizeof(Vec) * animPoseGetBaseData(pose)->mBufferNrmNum);

    pose->mpBufferGroupVisibility = testAlloc(sizeof(u8) * animPoseGetBaseData(pose)->mBufferVisibilityNum);
    pose->mpGroupVisibility = pose->mpBufferGroupVisibility;

    pose->mpBufferNode = testAlloc(sizeof(AnimPoseNode*) * animPoseGetBaseData(pose)->mBufferNodeNum);
    pose->mpNodes = testAlloc(sizeof(AnimPoseNode*) * animPoseGetBaseData(pose)->mBufferNodeNum);

    pose->mpBufferTexAnimEntries = testAlloc(sizeof(AnimTexMtx) * animPoseGetBaseData(pose)->mBufferTexAnimNum);
    pose->mpTexAnimEntries = pose->mpBufferTexAnimEntries;

    pose->lastFrameZero = -1;
}

// heavily inlined
void animPoseRefresh(void) {
    int i;

    initTestHeap(); // inline
    for (i = 0; i < wp->totalPoses; i++) {
        AnimationPose* pose = &wp->poses[i]; // inline
        if (pose->flags) {
            // TODO: cleanup in rewrite?
            if ((!wp->inBattle || pose->type != 0) && (wp->inBattle || pose->type != 1)) {
                animPose_AllocBuffer(pose);
                pose->lastFrameZero = -1; // TODO: redundant?
            }
        }
    }
}

void animPoseBattleInit(void) {
    wp->inBattle = TRUE;
    animPoseRefresh();
}

// Finds the first inactive file slot, writing its index through id. The early return
// inside the loop is what gives the found-exit the target's `bne next; b found` layout.
static inline AnimationPoseFile* animPoseFindFreeFile(s32* id) {
    AnimationPoseFile* f;

    for (*id = 0; *id < wp->totalFiles; (*id)++) {
        f = &wp->files[*id];
        if (!f->active) {
            return f;
        }
    }
    *id = -1;
    return NULL;
}

// Finds the first inactive texture slot, writing its index through id. Same early-return
// shape as the file search above.
static inline AnimationTextureFile* animPoseFindFreeTexture(s32* id) {
    AnimationTextureFile* t;

    for (*id = 0; *id < wp->totalTextures; (*id)++) {
        t = &wp->textures[*id];
        if ((u32)t->active == 0) {
            return t;
        }
    }
    *id = -1;
    return NULL;
}

// The file- and texture-slot free loops use the early-return helpers above for the target's
// `bne next; b found` found-exit layout; the name-search loops use fileId/texId as counters.
// NONMATCHING (98.32%): register allocation only, plus five induction-offset initialisations
// (retail `mr offset, index`, ours `li offset, 0`); the instruction count matches.
s32 animPoseEntry(const char* animName, s32 type) {
    AnimationPose *pose;
    AnimationPoseData* data;
    AnimationTextureFile *texture;
    AnimationPoseFile *file;
    AnimationPose *temp;
    s32 poseId;
    s32 fileId;
    s32 texId;
    s32 check;
    char textureName[256];

    switch (type) {
        case 0:
            fileSetCurrentArchiveType(1);
            break;
        case 1:
            fileSetCurrentArchiveType(2);
            break;
        case 2:
            fileSetCurrentArchiveType(0);
            break;
    }

    for (poseId = 0; poseId < wp->totalPoses; poseId++) {
        temp = &wp->poses[poseId];
        if (!temp->flags) {
            pose = temp;
            goto label_1;
        }
    }
    pose = NULL;
    poseId = -1;
label_1:
    pose->flags |= 1;
    pose->typeFlags = 0;
    pose->references = 0;

    check = wp->totalFiles;
    for (fileId = 0; fileId < check; fileId++) {
        file = &wp->files[fileId];
        if (file->active) {
            data = *file->handle->data;
            if (!strcmp(data->fileName, animName)) {
                file->references++;
                goto label_5;
            }
        }
    }

    file = animPoseFindFreeFile(&fileId);
label_2:
    file->active = TRUE;
    file->references++;
    file->handle = fileAllocf(5, str_80419A54, str_80419A5C, animName);

    if (!file->handle) {
        fileId = -2;
        file->active = FALSE;
        file->references--;
    } else {
        data = *file->handle->data;
        if (!data) {
            fileFree(file->handle);
            fileId = -2;
            file->active = FALSE;
            file->references--;
        } else {
            sprintf(textureName, "%s/%s-", str_80419A5C, data->textureName);
            check = wp->totalTextures;
            for (texId = 0; texId < check; texId++) {
                texture = &wp->textures[texId];
                if ((u32)texture->active != 0) {
                    if (texture->handle) {
                        if (!strcmp(texture->handle->filename, textureName)) {
                            texture->references++;
                            goto label_4;
                        }
                    }
                }
            }

            texture = animPoseFindFreeTexture(&texId);
        label_3:
            texture->active = TRUE;
            texture->references++;
            texture->handle = fileAlloc(textureName, 4);
        label_4:
            file->textureId = texId;
        }
    }
label_5:
    pose->fileId = fileId;
    if (pose->fileId == -2) {
        pose->flags = 0;
        return -2;
    }
    pose->animId = 0;
    pose->localTime = animTimeGetTime(type); // inline
    pose->lastFrameZero = -1;
    pose->lastFrameTime = float_0_80419a48;
    pose->field_0x80 = 0;
    pose->loopTime = float_0_80419a48;
    pose->field_0x70 = float_0_80419a48;
    pose->rotationY = float_0_80419a48;
    pose->field_0x7C = float_0_80419a48;
    pose->type = type;
    pose->paperId = -1;
    animPose_AllocBuffer(pose);
    pose->gxCallback = NULL;
    pose->disableDraw = 0;
    pose->materialFlag = 0;
    pose->materialLightFlag = 0;
    pose->materialColor = dat_80419a2c;
    pose->materialColor2 = dat_80419a30;
    pose->field_0xFC = float_1_80419a44;
    pose->field_0xF8 = float_1_80419a44;
    pose->field_0x100 = float_0_80419a48;
    pose->localTimeRate = float_1_80419a44;
    MTXIdentity(pose->matrix);
    if (!strcmp(animName, "tst_vivi")) {
        pose->vivianType = 1;
        pose->vivianGroupNo = &vivihimoTailGroupNo[0];
    } else if (!strcmp(animName, "c_vivian")) {
        pose->vivianType = 1;
        pose->vivianGroupNo = &vivihimoTailGroupNo[1];
    } else if (!strcmp(animName, "c_maririn")) {
        pose->vivianType = 2;
        pose->vivianGroupNo = &vivihimoTailGroupNo[2];
    } else if (!strcmp(animName, "c_majyorin")) {
        pose->vivianType = 3;
        pose->vivianGroupNo = &vivihimoTailGroupNo[3];
    } else { // if none match
        pose->vivianType = 0;
        pose->vivianGroupNo = &_dummy;
    }
    pose->dispCallback = NULL;
    pose->dispUserDataCallback = NULL;
    return poseId;
}

// Searches wp->poses for a paper pose of `type` whose animation file name is animName. Shared by
// animPaperPoseEntry and both loops of animPoseSetPaperAnimGroup; the original evidently factored it
// out, and reproducing the helper boundary is what gives the callers the target's register allocation.
static inline s32 animPaperPoseSearchGroup(s32 type, const char* animName) {
    AnimationPose* pose;
    s32 i;

    for (i = 0; i < wp->totalPoses; i++) {
        pose = &wp->poses[i];
        if ((pose->flags & 1) && (pose->typeFlags & 1) && (pose->typeFlags & 2) && pose->type == type) {
            if (!strcmp(animPoseGetBaseData(pose)->fileName, animName)) {
                break;
            }
        }
    }
    if (i == wp->totalPoses) {
        i = -1;
    }
    return i;
}

s32 animPaperPoseEntry(const char* animName, s32 group) {
    s32 i = animPaperPoseSearchGroup(group, animName);

    if (i >= 0) {
        ((AnimationPose*)((u8*)wp->poses + i * 0x170))->references += 1;
        return i;
    }
    {
        s32 id = animPoseEntry(animName, group);

        if (id == -2) {
            return id;
        }
        {
            AnimationPose* pose = &wp->poses[id];

            pose->typeFlags = 3;
            pose->references = 0;
            pose->flags |= 1;
        }
        return id;
    }
}

// animEffectAsync
BOOL animEffectAsync(const char* animName, u32 group) {
    s32 fileId;
    s32 other;
    void** data;
    u8* entry;
    u8* result;
    s32 offset;
    s32 size;

    switch (group) {
        case 0:
            fileSetCurrentArchiveType(1);
            break;
        case 1:
            fileSetCurrentArchiveType(2);
            break;
        case 2:
            fileSetCurrentArchiveType(0);
            break;
    }
    fileId = fileAsyncf(5, NULL, str_80419A54, str_80419A5C, animName);
    data = wp->mp_ag2tg->data;
    size = (s32)data[1];
    entry = (u8*)data[0];
    for (offset = 0; offset < size; offset += 0x20) {
        if (!strcmp((char*)entry, animName)) {
            result = entry + 0x20;
            goto found;
        }
        entry += 0x20;
    }
    result = (u8*)&zero_80419a50;
found:
    other = fileAsyncf(4, NULL, "%s/%s-", str_80419A5C, result);

    if (fileId == -1) {
        return TRUE;
    }
    if ((u32)fileId == 0) {
        return FALSE;
    }
    if (other == -1) {
        return TRUE;
    }
#ifdef MUST_MATCH
    // Natural `return other != 0;` compiles to the branchless (x | -x) >> 31 form; the
    // original keeps the explicit 0/1 blocks, which a goto preserves.
    if ((u32)other != 0) {
        goto ret_true;
    }
    return FALSE;
ret_true:
    return TRUE;
#else
    return other != 0;
#endif
}

void animPosePeraOn(s32 poseId) {
    wp->poses[poseId].flags |= 8;
}

void animPosePeraOff(s32 poseId) {
    wp->poses[poseId].flags &= ~8;
}

void animPosePaperPeraOn(s32 poseId) {
    AnimationPose *src, *dst;

    src = animPoseGetAnimPosePtr(poseId);
    dst = &wp->poses[src->paperId];
    dst->flags |= 8;
    dst->field_0x70 = src->field_0x70;
    dst->rotationY = src->rotationY;
    dst->field_0x7C = src->field_0x7C;
}

void animPosePaperPeraOff(s32 poseId) {
    AnimationPose* src;

    src = animPoseGetAnimPosePtr(poseId);
    wp->poses[src->paperId].flags &= ~8;
}

void animPoseSetLocalTimeRate(s32 poseId, f32 localTimeRate) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->localTimeRate = localTimeRate;
}

// TODO: get animTimeGetTime to inline here
void animPoseSetLocalTime(s32 poseId, f32 localTimeFrames) {
    AnimationPose *pose, *pera;
    OSTime time;

    pose = &wp->poses[poseId];
    time = (localTimeFrames * 16.666666f);
    pose->localTime = animTimeGetTime(pose->type) - time;
    pose->mLocalTimeInit = pose->localTime;
    if (pose->paperId != -1) {
        pera = &wp->poses[pose->paperId];
        if (!(pera->typeFlags & 2)) {
            pera->localTime = pose->localTime;
        }
    }
}

void animPoseSetStartTime(s32 poseId, OSTime startTime) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->localTime = startTime;
}

// Forward declaration: the shared search helper is defined below, next to animPoseSetEffectAnim.
static inline s32 animPoseSearchAnim(AnimationPose* pose, const char* animName);

void animPoseSetAnim(s32 poseId, const char* animName, BOOL reset) {
    AnimationPose* pose = &wp->poses[poseId];
    s32 animId = animPoseSearchAnim(pose, animName);

    if (!reset && animId == pose->animId) {
        return;
    }
    pose->animId = animId;
    if (pose->paperId != -1 && (pose->flags & 2)) {
        pose->flags |= 4;
    }
    pose->localTime = animTimeGetTime(pose->type);
    pose->lastFrameZero = -1;
    pose->lastFrameTime = float_0_80419a48;
    pose->field_0x80 = 0;
    pose->loopTime = float_0_80419a48;
}

s32 animPaperPoseGetId(const char* animName, s32 group) {
    AnimationPose* pose;
    AnimationPoseData* data;
    s32 i;

    for (i = 0; i < wp->totalPoses; i++) {
        pose = &wp->poses[i];
        if ((pose->flags & 1) && (pose->typeFlags & 1) && (pose->typeFlags & 2) && pose->type == group) {
            data = *wp->files[pose->fileId].handle->data;
            if (!strcmp(data->fileName, animName)) {
                break;
            }
        }
    }
    if (i == wp->totalPoses) {
        i = -1;
    }
    return i;
}

// NONMATCHING (98.83%): the two search loops call animPaperPoseSearchGroup, so only the callee-saved
// colouring differs (target pose r27/someflag r30/i r29/type r28/offset r26; here r30/r29/r28/r27/r26).
void animPoseSetPaperAnimGroup(s32 poseId, const char* animName, BOOL someflag) {
    s32 poseOffset = poseId * 0x170;
    s32 type;
    s32 i;
    s32 j;
    AnimationPose* pose = &wp->poses[poseId];

    if (animName == NULL) {
        pose->paperId = -1;
        ((AnimationPose*)((u8*)wp->poses + poseOffset))->materialLightFlag &= ~1;
        return;
    }
    type = pose->type;
    i = animPaperPoseSearchGroup(type, animName);
    pose->paperId = i;
    if (pose->paperId < 0) {
        j = animPaperPoseSearchGroup(2, animName);
        pose->paperId = j;
    }
    *(s32*)((u8*)pose + 0x94) = -1;
    pose->mLocalTimeInit = animTimeGetTime(pose->type);
    if (someflag) {
        pose->flags |= 2;
        pose->flags |= 4;
    } else {
        pose->flags &= ~2;
        pose->flags &= ~4;
    }
    ((AnimationPose*)((u8*)wp->poses + poseOffset))->materialLightFlag |= 1;
}
// Match-only: the target's search exits through an early `return`, which MWCC compiles to the
// `bne <increment>; b <found>` branch pair (a plain `break` gives one `beq <found>`). Taking the poseId
// and computing the pose/paper/data/top inside the helper is what gives the caller the target's
// callee-saved colouring (i r30, pose r31, paper materialised so the fileId load folds).
#ifdef MUST_MATCH
static inline s32 animPoseSearchAnimPaper(s32 poseId, const char* animName) {
    s32 i = (s32)animName;
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPose* paper = &wp->poses[pose->paperId];
    AnimationPoseData* data = animPoseGetBaseData(paper);
    s32 top = data->mAnimCount;

    if (i < 0) {
        for (i = 0; i < top; i++) {
            if (!strcmp(ANIM_POSE_DATA_ANIMS(data)[i].name, animName)) {
                return i;
            }
        }
        goto Zero;
    }
    if (i >= top) {
        goto Zero;
    }
    return i;
Zero:
    i = 0;
Found:
    return i;
}
#else
static inline s32 animPoseSearchAnimPaper(s32 poseId, const char* animName) {
    s32 i = (s32)animName;
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPose* paper = &wp->poses[pose->paperId];
    AnimationPoseData* data = animPoseGetBaseData(paper);
    s32 top = data->mAnimCount;

    if (i < 0) {
        for (i = 0; i < top; i++) {
            if (!strcmp(ANIM_POSE_DATA_ANIMS(data)[i].name, animName)) {
                break;
            }
        }
    }
    if (i >= top) {
        i = 0;
    }
    return i;
}
#endif

void animPoseSetPaperAnim(s32 poseId, const char* animName) {
    AnimationPose* pose = &wp->poses[poseId];
    s32 animId = animPoseSearchAnimPaper(poseId, animName);

    *(s32*)((u8*)pose + 0x94) = animId;
    pose->mLocalTimeInit = animTimeGetTime(pose->type);
    if (pose->paperId != -1 && !(((AnimationPose*)((u8*)wp->poses + pose->paperId * 0x170))->typeFlags & 2)) {
        animPoseMain(pose->paperId);
    }
}

// Inline form of the paper-pose release block shared with animPoseSetEffect; the original evidently
// inlined it too.
static inline void animPoseReleasePaper(AnimationPose* paper) {
    AnimationPoseFile* file;
    AnimationTextureFile* texture;

    paper->references--;
    if (paper->references < 0) {
        file = &wp->files[paper->fileId];
        file->references--;
        if (file->references <= 0) {
            file->active = 0;
            texture = &wp->textures[file->textureId];
            texture->references--;
            if (texture->references <= 0) {
                fileFree(texture->handle);
                texture->active = 0;
            }
            fileFree(file->handle);
        }
        paper->flags = 0;
    }
}

// The effect pose's own entry, inlined: its early return for a failed entry is the original's `bne; b` layout.
static inline s32 animEffectPoseEntry(const char* animName, s32 type) {
    s32 id = animPoseEntry(animName, type);
    AnimationPose* p;

    if (id == -2) {
        return id;
    }
    p = &wp->poses[id];
    p->typeFlags = 1;
    p->references = 0;
    p->flags |= 1;
    return id;
}

// NONMATCHING (99.34%): the paper-release block is the shared inline animPoseReleasePaper above and the new pose's
// entry is animEffectPoseEntry; only the pose/poseOffset callee-saved pair is swapped (target pose r31, offset r30).
void animPoseSetEffect(s32 poseId, const char* animName, BOOL someflag) {
    AnimationPose* pose = &wp->poses[poseId];
    s32 poseOffset;

    if (pose->paperId != -1) {
        if (animName != NULL) {
            AnimationPose* paper = &wp->poses[pose->paperId];
            FileEntry* handle = *(FileEntry**)((u8*)wp->files + paper->fileId * 0x10 + 8);

            if (!strcmp((*(AnimationPoseData**)(handle->data))->fileName, animName)) {
                s32 f = (pose->flags & 2) ? 1 : 0;

                if (f == someflag) {
                    return;
                }
            }
        }
        {
            AnimationPose* paper = &wp->poses[pose->paperId];
            animPoseReleasePaper(paper);
        }
    }
    poseOffset = poseId * 0x170;
    if (animName == NULL) {
        pose->paperId = -1;
        ((AnimationPose*)((u8*)wp->poses + poseOffset))->materialLightFlag &= ~1;
        return;
    }
    pose->paperId = animEffectPoseEntry(animName, pose->type);
    *(s32*)((u8*)pose + 0x94) = -1;
    pose->mLocalTimeInit = animTimeGetTime(pose->type);
    if (someflag) {
        pose->flags |= 2;
        pose->flags |= 4;
    } else {
        pose->flags &= ~2;
    }
    ((AnimationPose*)((u8*)wp->poses + poseOffset))->materialLightFlag |= 1;
}

// Searches ANIM_POSE_DATA_ANIMS(data) for animName when i is negative (i is the string's address used
// as a sentinel), then clamps the result into [0, top). MUST_MATCH keeps the target's exact control flow;
// the natural form is in the #else. Computing data/top from the passed pose inside the helper (rather
// than passing them in) is what gives the callers the target's callee-saved colouring.
#ifdef MUST_MATCH
static inline s32 animPoseSearchAnim(AnimationPose* pose, const char* animName) {
    s32 i = (s32)animName;
    AnimationPoseData* data = animPoseGetBaseData(pose);
    s32 top = data->mAnimCount;

    if (i < 0) {
        for (i = 0; i < top; i++) {
            if (!strcmp(ANIM_POSE_DATA_ANIMS(data)[i].name, animName)) {
                return i;
            }
        }
        goto Zero;
    }
    if (i >= top) {
        goto Zero;
    }
    return i;
Zero:
    i = 0;
Found:
    return i;
}
#else
static inline s32 animPoseSearchAnim(AnimationPose* pose, const char* animName) {
    s32 i = (s32)animName;
    AnimationPoseData* data = animPoseGetBaseData(pose);
    s32 top = data->mAnimCount;

    if (i < 0) {
        for (i = 0; i < top; i++) {
            if (!strcmp(ANIM_POSE_DATA_ANIMS(data)[i].name, animName)) {
                break;
            }
        }
    }
    if (i >= top) {
        i = 0;
    }
    return i;
}
#endif

void animPoseSetEffectAnim(s32 poseId, const char* animName, BOOL reset) {
    AnimationPose* pose = &wp->poses[poseId];
    s32 i;

    if (pose->paperId == -1) {
        return;
    }
    pose = &wp->poses[pose->paperId];
    i = animPoseSearchAnim(pose, animName);
    if (!reset && i == pose->animId) {
        return;
    }
    pose->animId = i;
    if (pose->paperId != -1 && (pose->flags & 2)) {
        pose->flags |= 4;
    }
    pose->localTime = animTimeGetTime(pose->type);
    pose->lastFrameZero = -1;
    pose->lastFrameTime = float_0_80419a48;
    pose->field_0x80 = 0;
    pose->loopTime = float_0_80419a48;
}

void animPoseSetGXFunc(s32 poseId, void (*callback)(s32 wXluStage), BOOL disableDraw) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->gxCallback = callback;
    pose->disableDraw = disableDraw;
}

f32 animPoseGetRadius(s32 poseId) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return (f32)*(s32*)((u8*)data + 0xC8) * 2.0f;
}

f32 animPoseGetHeight(s32 poseId) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return (f32)*(s32*)((u8*)data + 0xCC) * 2.0f;
}

f32 animPoseGetLoopTimes(s32 id) {
    AnimationPose* pose = &wp->poses[id];
    return pose->loopTime;
}

BOOL animPoseGetPeraEnd(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];

    return !(pose->flags & 8) ? TRUE : fabs(pose->field_0x7C - pose->rotationY) < 10.0;
}
void animPoseSetMaterialFlagOn(s32 poseId, u32 mask) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialFlag |= mask;
}

void animPoseSetMaterialFlagOff(s32 poseId, u32 mask) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialFlag &= ~mask;
}

void animPoseSetMaterialLightFlagOn(s32 poseId, u32 mask) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialLightFlag |= mask;
}

void animPoseSetMaterialLightFlagOff(s32 poseId, u32 mask) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialLightFlag &= ~mask;
}

void animPoseSetMaterialEvtColor(s32 poseId, GXColor color) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->materialColor = color;
}

u32 animPoseGetMaterialFlag(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    return pose->materialFlag;
}

u32 animPoseGetMaterialLightFlag(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    return pose->materialLightFlag;
}

u32 animPoseGetMaterialEvtColor(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    return *(u32*)&pose->materialColor;
}

// animPoseMain
#include <dolphin/os/OSFastCast.h>
// AnimationPose's 0x20-0x3C block and AnimationPoseData's 0x190/0x1A0/0x1A4 words are opaque
// padding in include/driver/animdrv.h, so they are reached by raw offset here. The animation
// record (AnimTableEntry::data, the header's opaque `AnimData`) keeps its frame and delta
// tables behind the same padding; a frame record is 0x2C bytes.
#define ANIM_DATA_CLIP(a)         (*(s32**)((u8*)(a) + 0x24))
#define ANIM_DATA_FRAMES(a)       (*(u8**)((u8*)(a) + 0x28))
#define ANIM_DATA_FRAME_COUNT(a)  (*(s32*)((u8*)(a) + 0x8))
#define ANIM_DATA_FRAMES2(a)      (*(u8**)((u8*)(a) + 0x40))
#define ANIM_DATA_FRAME2_COUNT(a) (*(s32*)((u8*)(a) + 0x20))
#define ANIM_DATA_POS(a)          (*(u8**)((u8*)(a) + 0x2C))
#define ANIM_DATA_NRM(a)          (*(u8**)((u8*)(a) + 0x30))
#define ANIM_DATA_TEX(a)          (*(u8**)((u8*)(a) + 0x34))
#define ANIM_DATA_VIS(a)          (*(u8**)((u8*)(a) + 0x38))
#define ANIM_DATA_NODE(a)         (*(u8**)((u8*)(a) + 0x3C))

#define ANIM_FRAME_TIME(f)       (*(f32*)((u8*)(f) + 0x0))
#define ANIM_FRAME_POS_OFF(f)    (*(s32*)((u8*)(f) + 0x4))
#define ANIM_FRAME_POS_COUNT(f)  (*(s32*)((u8*)(f) + 0x8))
#define ANIM_FRAME_NRM_OFF(f)    (*(s32*)((u8*)(f) + 0xC))
#define ANIM_FRAME_NRM_COUNT(f)  (*(s32*)((u8*)(f) + 0x10))
#define ANIM_FRAME_TEX_OFF(f)    (*(s32*)((u8*)(f) + 0x14))
#define ANIM_FRAME_TEX_COUNT(f)  (*(s32*)((u8*)(f) + 0x18))
#define ANIM_FRAME_VIS_OFF(f)    (*(s32*)((u8*)(f) + 0x1C))
#define ANIM_FRAME_VIS_COUNT(f)  (*(s32*)((u8*)(f) + 0x20))
#define ANIM_FRAME_NODE_OFF(f)   (*(s32*)((u8*)(f) + 0x24))
#define ANIM_FRAME_NODE_COUNT(f) (*(s32*)((u8*)(f) + 0x28))

#define ANIM_FRAME_SIZE 0x2C
#define APOSE_F(p, off) (*(f32*)((u8*)(p) + (off)))
#define APOSE_S(p, off) (*(s32*)((u8*)(p) + (off)))

// One position/normal delta run: [u8 index][s8 dx][s8 dy][s8 dz]; the index walks the
// destination array from the previous record.
static inline void animPoseAddVecDelta(Vec* dst, u8* src, s32 size, f32 scale) {
    u8* end = src + size;
    while (src < end) {
        dst += *src;
        {
            f32 dx = __OSs8tof32((s8*)src + 1);
            f32 dy = __OSs8tof32((s8*)src + 2);
            f32 dz = __OSs8tof32((s8*)src + 3);
            dst->x += scale * dx;
            dst->y += scale * dy;
            dst->z += scale * dz;
        }
        src += 4;
    }
}

static inline void animPoseAddVecDeltaRange(Vec* dst, u8* src, u8* end, f32 scale) {
    while (src < end) {
        dst += *src;
        {
            f32 dx = __OSs8tof32((s8*)src + 1);
            f32 dy = __OSs8tof32((s8*)src + 2);
            f32 dz = __OSs8tof32((s8*)src + 3);
            dst->x += dx * scale;
            dst->y += dy * scale;
            dst->z += dz * scale;
        }
        src += 4;
    }
}

// Visibility delta run: [u8 index][u8 delta].
static inline void animPoseAddVisDelta(u8* src, u8* dst, s32 size) {
    u8* end = src + size;
    while (src < end) {
        dst += src[0];
        *dst += src[1];
        src += 2;
    }
}

// Node scalar delta run: [u8 index][s8 delta][u8][u8].
static inline void animPoseAddNodeDelta(f32* dst, u8* src, s32 size, f32 scale) {
    u8* end = src + size;
    while (src < end) {
        dst += src[0];
        *dst += scale * __OSs8tof32((s8*)src + 1);
        src += 4;
    }
}

// Texture-animation delta run: [u8 index][u8 delta][f32 s][f32 t], index in 0x18-byte entries.
static inline void animPoseAddTexDelta(AnimTexMtx* dst, u8* src, s32 size) {
    u8* end = src + size;
    while (src < end) {
        dst = (AnimTexMtx*)((u8*)dst + src[0] * 0x18);
        dst->mTexBindIdx += src[1];
        dst->mTransS += *(f32*)(src + 4);
        dst->mTransT += *(f32*)(src + 8);
        src += 0xC;
    }
}

// Applies one frame's five delta channels to the pose buffers. Retail inlines this at both
// update paths (reset and catch-up). It is a macro rather than a helper function because the
// combined expansion exceeds the default auto-inline size, and raising #pragma inline_max_size
// for it leaks into the earlier functions in this file (testAlloc/animPoseRefresh stop matching).
#define ANIM_POSE_ADD_FRAME(pose, anim, frame, scale)                                               \
    do {                                                                                            \
        animPoseAddVecDelta((pose)->mpBufferVtxPos,                                                 \
                            ANIM_DATA_POS(anim) + ANIM_FRAME_POS_OFF(frame) * 4,                    \
                            ANIM_FRAME_POS_COUNT(frame) * 4, scale);                                \
        animPoseAddVecDelta((pose)->mpBufferVtxNrm,                                                 \
                            ANIM_DATA_NRM(anim) + ANIM_FRAME_NRM_OFF(frame) * 4,                    \
                            ANIM_FRAME_NRM_COUNT(frame) * 4, scale);                                \
        animPoseAddVisDelta(ANIM_DATA_VIS(anim) + ANIM_FRAME_VIS_OFF(frame) * 2,                    \
                            (pose)->mpBufferGroupVisibility,                                        \
                            ANIM_FRAME_VIS_COUNT(frame) * 2);                                       \
        animPoseAddNodeDelta((f32*)(pose)->mpBufferNode,                                            \
                             ANIM_DATA_NODE(anim) + ANIM_FRAME_NODE_OFF(frame) * 4,                 \
                             ANIM_FRAME_NODE_COUNT(frame) * 4, scale);                              \
        animPoseAddTexDelta((pose)->mpBufferTexAnimEntries,                                         \
                            ANIM_DATA_TEX(anim) + ANIM_FRAME_TEX_OFF(frame) * 0xC,                  \
                            ANIM_FRAME_TEX_COUNT(frame) * 0xC);                                     \
    } while (0)

// The unit's own .sdata2 constants referenced by name from the assembly below.
extern const f32 float_60_80419ac4;
extern const f32 float_1000_80419ac8;
extern const f32 float_0p0625_80419acc;
extern const f32 float_2_80419a84;
extern const f32 float_3_80419ad0;

// The four inlined delta loops make each update path large; retail inlines them in both.
#pragma inline_max_total_size(60000)
// Reconstruction notes. The whole recursive pose evaluator is reconstructed from the assembly;
// the bl sequence and immediates match (17 calls), so only register allocation and instruction
// scheduling remain. Source shapes that reproduce retail's operand order and register split: the
// two `float_0_80419a48 == x` compares keep the constant first in `fcmpu`; the clamped value
// accumulates back into `start` (retail's clamp lives in start's f3) instead of a separate
// `clamped` variable; `end` is declared before `start` and loaded first, giving retail's f1/f3
// split; the range delta helper multiplies `dx * scale` (the size helper's `scale * dx` is what
// retail's reset loop uses), and the cubic `-2*a3 + 3*a2` term order matches retail's fmadds.
// What is left: the reset loops take their source in r7 where retail uses r5, the node
// cubic-Hermite block schedules a2/a3 and the tanfTbl loads in another order and keeps its
// `angle` in f3 where retail uses f2, and the two `0.0f` clamp stores rematerialise
// `float_0_80419a48` where retail keeps it live in f0. Permuter work: no call, immediate or
// control-flow difference remains.
// NONMATCHING (98.53%): register allocation/scheduling only; all calls and immediates match.
void animPoseMain(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* data = animPoseGetBaseData(pose);
    AnimData* anim;
    Vec* vtxNrm;
    Vec* vtxPos;
    s32 canLoop;
    OSTime now;
    f32 time;
    f32 end;
    f32 start;
    f32 duration;
    s32 i;
    s32 frame2;
    s32 frame1;
    f32 alpha;
    s32 posNum, nrmNum, visNum, nodeNum, texNum;

    anim = ANIM_POSE_DATA_ANIMS(data)[pose->animId].data;

    OSInitFastCast();

    if (*(s32*)&wp->mbUseFloatScratch) {
        vtxPos = wp->vtxArrayPos;
        vtxNrm = wp->vtxArrayNrm;
    } else {
        vtxPos = pose->mpVtxArrayPos;
        vtxNrm = pose->mpVtxArrayNrm;
    }

    if (pose->paperId != -1 && !(((AnimationPose*)((u8*)wp->poses + pose->paperId * 0x170))->typeFlags & 2)) {
        animPoseMain(pose->paperId);
    }

    if (anim == NULL) {
        return;
    }

    canLoop = ANIM_DATA_CLIP(anim)[0];
    now = animTimeGetTime(pose->type);
    APOSE_F(pose, 0x20) = (f32)(u64)(now - pose->localTime) * float_60_80419ac4 / float_1000_80419ac8;
    APOSE_F(pose, 0x20) *= pose->localTimeRate;

    time = APOSE_F(pose, 0x20);
    end = ((f32*)ANIM_DATA_CLIP(anim))[2];
    start = ((f32*)ANIM_DATA_CLIP(anim))[1];
    duration = end - start;

    if (float_0_80419a48 == duration) {
        pose->loopTime = time - start;
    } else if (canLoop) {
        f32 t = time - start;
        if (t < 0.0f) {
            pose->loopTime = 0.0f;
        } else {
            pose->loopTime = t / duration;
            start = time - (f32)(s32)pose->loopTime * duration;
        }
    } else {
        if (time < start) {
            pose->loopTime = 0.0f;
        } else if (time > end) {
            pose->loopTime = (time - start) / duration;
            start = end;
        } else {
            pose->loopTime = (time - start) / duration;
            start = time;
        }
    }
    APOSE_F(pose, 0x24) = start;

    {
    f32 frameTime = APOSE_F(pose, 0x24);
    {
    u8* fp = ANIM_DATA_FRAMES(anim);
    for (i = 0; i < ANIM_DATA_FRAME_COUNT(anim); i++) {
        if (*(f32*)fp > frameTime) {
            break;
        }
        fp += ANIM_FRAME_SIZE;
    }
    }
    APOSE_S(pose, 0x28) = i - 1;
    APOSE_S(pose, 0x2C) = APOSE_S(pose, 0x28) + 1;

    {
    f32 frameTime2 = APOSE_F(pose, 0x24);
    u8* fp2 = ANIM_DATA_FRAMES2(anim);
    for (i = 0; i < ANIM_DATA_FRAME2_COUNT(anim); fp2 += 8, i++) {
        if (*(f32*)fp2 > frameTime2) {
            break;
        }
    }
    }
    APOSE_S(pose, 0x34) = i - 1;
    APOSE_S(pose, 0x38) = APOSE_S(pose, 0x34) + 1;
    }
    if (!canLoop) {
        if (APOSE_S(pose, 0x2C) >= ANIM_DATA_FRAME_COUNT(anim)) {
            APOSE_S(pose, 0x2C) = APOSE_S(pose, 0x28);
        }
        if (APOSE_S(pose, 0x38) >= ANIM_DATA_FRAME2_COUNT(anim)) {
            APOSE_S(pose, 0x38) = APOSE_S(pose, 0x34);
        }
    }

    {
        s32* p = (s32*)ANIM_DATA_FRAMES2(anim);
        pose->field_0x80 = p[APOSE_S(pose, 0x34) * 2 + 1];
    }

    {
        f32 num = APOSE_F(pose, 0x24) - *(f32*)(ANIM_DATA_FRAMES(anim) + APOSE_S(pose, 0x28) * ANIM_FRAME_SIZE);
        f32 den;
        if (canLoop) {
            if (APOSE_S(pose, 0x2C) >= ANIM_DATA_FRAME_COUNT(anim)) {
                APOSE_S(pose, 0x2C) = 0;
                den = ((f32*)ANIM_DATA_CLIP(anim))[2] - *(f32*)(ANIM_DATA_FRAMES(anim) + APOSE_S(pose, 0x28) * ANIM_FRAME_SIZE);
            } else {
                den = *(f32*)(ANIM_DATA_FRAMES(anim) + APOSE_S(pose, 0x2C) * ANIM_FRAME_SIZE)
                    - *(f32*)(ANIM_DATA_FRAMES(anim) + APOSE_S(pose, 0x28) * ANIM_FRAME_SIZE);
            }
        } else {
            den = *(f32*)(ANIM_DATA_FRAMES(anim) + APOSE_S(pose, 0x2C) * ANIM_FRAME_SIZE)
                - *(f32*)(ANIM_DATA_FRAMES(anim) + APOSE_S(pose, 0x28) * ANIM_FRAME_SIZE);
        }
        if (float_0_80419a48 == den) {
            APOSE_F(pose, 0x30) = float_0_80419a48;
        } else {
            APOSE_F(pose, 0x30) = num / den;
        }
    }

    frame1 = APOSE_S(pose, 0x28);
    frame2 = APOSE_S(pose, 0x2C);
    alpha = APOSE_F(pose, 0x30);
    posNum = data->mBufferPosNum;
    nrmNum = data->mBufferNrmNum;
    visNum = data->mBufferVisibilityNum;
    nodeNum = data->mBufferNodeNum;
    texNum = data->mBufferTexAnimNum;

    if (pose->lastFrameZero != frame1) {
        if (pose->lastFrameZero == -1 || pose->lastFrameZero > frame1) {
            u8* f = ANIM_DATA_FRAMES(anim);
            u8* fend = ANIM_DATA_FRAMES(anim) + frame1 * ANIM_FRAME_SIZE + ANIM_FRAME_SIZE;
            if (posNum) {
                memcpy_as4(pose->mpBufferVtxPos, data->bufferVtxPos, posNum * 0xC);
            }
            if (nrmNum) {
                memcpy_as4(pose->mpBufferVtxNrm, data->nrmVtxBuffer, nrmNum * 0xC);
            }
            if (visNum) {
                memcpy(pose->mpBufferGroupVisibility, *(void**)((u8*)data + 0x1A0), visNum);
            }
            if (nodeNum) {
                memcpy_as4(pose->mpBufferNode, *(void**)((u8*)data + 0x1A4), nodeNum * 4);
            }
            if (texNum) {
                memcpy_as4(pose->mpBufferTexAnimEntries, *(void**)((u8*)data + 0x190), texNum * 0x18);
            }
            if (pose->lastFrameZero == -1) {
                pose->lastFrameZero = 0;
            }
            for (; f < fend; f += ANIM_FRAME_SIZE) {
                ANIM_POSE_ADD_FRAME(pose, anim, f, float_0p0625_80419acc);
            }
        } else if (pose->lastFrameZero < frame1) {
            u8* f = ANIM_DATA_FRAMES(anim) + (pose->lastFrameZero + 1) * ANIM_FRAME_SIZE;
            u8* fend = ANIM_DATA_FRAMES(anim) + (frame1 + 1) * ANIM_FRAME_SIZE;
            for (; f < fend; f += ANIM_FRAME_SIZE) {
                ANIM_POSE_ADD_FRAME(pose, anim, f, float_0p0625_80419acc);
            }
        }
    } else {
        if (pose->lastFrameTime > alpha) {
            pose->lastFrameTime = float_0_80419a48;
        }
    }

    if (frame1 == frame2) {
        if (posNum) {
            memcpy_as4(vtxPos, pose->mpBufferVtxPos, posNum * 0xC);
        }
        if (nrmNum) {
            memcpy_as4(vtxNrm, pose->mpBufferVtxNrm, nrmNum * 0xC);
        }
        if (nodeNum) {
            memcpy_as4(pose->mpNodes, pose->mpBufferNode, nodeNum * 4);
        }
    } else if (frame1 <= frame2) {
        u8* key2 = ANIM_DATA_FRAMES(anim) + frame2 * ANIM_FRAME_SIZE;
        f32 blend = float_0p0625_80419acc * alpha;

        {
            u8* posSrc = ANIM_DATA_POS(anim) + ANIM_FRAME_POS_OFF(key2) * 4;
            u8* posEnd = posSrc + ANIM_FRAME_POS_COUNT(key2) * 4;
            Vec* d = vtxPos;
            Vec* vc = pose->mpBufferVtxPos;
            if (posNum) {
                memcpy_as4(d, vc, posNum * 0xC);
            }
            while (posSrc < posEnd) {
                d += *posSrc;
                {
                    f32 dx = __OSs8tof32((s8*)posSrc + 1);
                    f32 dy = __OSs8tof32((s8*)posSrc + 2);
                    f32 dz = __OSs8tof32((s8*)posSrc + 3);
                    d->x += dx * blend;
                    d->y += dy * blend;
                    d->z += dz * blend;
                }
                posSrc += 4;
            }
        }

        {
            u8* nrmSrc = ANIM_DATA_NRM(anim) + ANIM_FRAME_NRM_OFF(key2) * 4;
            u8* nrmEnd = nrmSrc + ANIM_FRAME_NRM_COUNT(key2) * 4;
            Vec* d = vtxNrm;
            Vec* nc = pose->mpBufferVtxNrm;
            if (nrmNum) {
                memcpy_as4(d, nc, nrmNum * 0xC);
            }
            while (nrmSrc < nrmEnd) {
                d += *nrmSrc;
                {
                    f32 dx = __OSs8tof32((s8*)nrmSrc + 1);
                    f32 dy = __OSs8tof32((s8*)nrmSrc + 2);
                    f32 dz = __OSs8tof32((s8*)nrmSrc + 3);
                    d->x += dx * blend;
                    d->y += dy * blend;
                    d->z += dz * blend;
                }
                nrmSrc += 4;
            }
        }

        {
            u8* src = ANIM_DATA_NODE(anim) + ANIM_FRAME_NODE_OFF(key2) * 4;
            u8* srcEnd = src + ANIM_FRAME_NODE_COUNT(key2) * 4;
            void* nodeSrc = pose->mpBufferNode;
            f32* dst = (f32*)pose->mpNodes;
            f32 timedelta = *(f32*)(ANIM_DATA_FRAMES(anim) + frame2 * ANIM_FRAME_SIZE)
                          - *(f32*)(ANIM_DATA_FRAMES(anim) + frame1 * ANIM_FRAME_SIZE);

            if (nodeNum) {
                memcpy_as4(dst, nodeSrc, nodeNum * 4);
            }
            {
                f32 a2 = alpha * alpha;
                while (src < srcEnd) {
                    f32 a3 = alpha * a2;
                    s32 t1 = (s8)src[3];
                    dst += src[0];
                    if (t1 < 0x59 && t1 > -0x59) {
                        f32 step = float_0p0625_80419acc * __OSs8tof32((s8*)src + 1);
                        f32 tan0 = tanfTbl[(s8)src[2] + 0x59];
                        s32 i1 = t1 + 0x59;
                        f32 tan1 = tanfTbl[i1];
                        *dst += (float_neg2_80419ab8 * a3 + float_3_80419ad0 * a2) * step
                              + (a3 - float_2_80419a84 * a2 + alpha) * (timedelta * tan0)
                              + (a3 - a2) * (timedelta * tan1);
                    }
                    src += 4;
                }
            }
        }
    }

    pose->lastFrameZero = frame1;
    pose->lastFrameTime = alpha;

    if (pose->flags & 8) {
        CameraEntry* cam = camGetPtr(CAMERA_3D);
        f32 rad = (f32)atan2(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z));
        f32 angle = float_rad2deg_80419a94 * rad;
        f32 diff;

        while (angle < float_neg360_80419a9c) {
            angle += float_360_80419a98;
        }
        while (angle >= float_360_80419a98) {
            angle -= float_360_80419a98;
        }
        if (angle < float_0_80419a48) {
            angle += float_360_80419a98;
        }

        diff = angle - pose->rotationY;
        while (diff < float_neg360_80419a9c) {
            diff += float_360_80419a98;
        }
        while (diff >= float_360_80419a98) {
            diff -= float_360_80419a98;
        }

        if ((diff > float_90_80419aa0 && diff <= float_270_80419aa4)
            || (diff < float_neg90_80419a90 && diff >= float_neg270_80419aa8)) {
            pose->field_0x78 = float_neg1_80419a8c;
        } else {
            pose->field_0x78 = float_1_80419a44;
        }
    }
}
#pragma inline_max_size reset
#pragma inline_max_total_size reset

// pushGXModelMtx_TransformNode__
// Matches retail. Local declaration order (translation, scale, rotation, then the four
// centre/translation groups) fixes the f14-f31 allocation; the outer centre zero tests are
// `!=` (constant-first fcmpu) while translation/rotation use truthiness (value-first).
void pushGXModelMtx_TransformNode__(void* node) {
    AnimPoseNode* n = node;
    Mtx mTrans, mScale, mRotX, mRot, t1, t2, t3, t4, t5, t6;
    f32 (*result)[4];
    u32 flags = 0;
    f32 tx = n->mTranslation.x, ty = n->mTranslation.y, tz = n->mTranslation.z;
    f32 sx = n->mScale.x, sy = n->mScale.y, sz = n->mScale.z;
    f32 rotX = n->mRot1.x * 2.0f, rotY = n->mRot1.y * 2.0f, rotZ = n->mRot1.z * 2.0f;
    f32 rcx = n->mRotationCenter.x, rcy = n->mRotationCenter.y, rcz = n->mRotationCenter.z;
    f32 scx = n->mScaleCenter.x, scy = n->mScaleCenter.y, scz = n->mScaleCenter.z;
    f32 rtx = n->mRotationTranslation.x, rty = n->mRotationTranslation.y, rtz = n->mRotationTranslation.z;
    f32 stx = n->mScaleTranslation.x, sty = n->mScaleTranslation.y, stz = n->mScaleTranslation.z;

    if (tx || ty || tz) {
        PSMTXTrans(mTrans, tx, ty, tz);
        flags |= 1;
    }
    if (sx != 1.0f || sy != 1.0f || sz != 1.0f) {
        PSMTXScale(mScale, sx, sy, sz);
        flags |= 2;
    }
    if (scx != 0.0f || scy != 0.0f || scz != 0.0f) {
        if (flags & 2) {
            PSMTXTrans(t2, -scx, -scy, -scz);
            PSMTXTrans(t1, scx, scy, scz);
            PSMTXTrans(t3, stx, sty, stz);
            PSMTXConcat(t3, t1, mRot);
            PSMTXConcat(mRot, mScale, mRot);
            PSMTXConcat(mRot, t2, mScale);
        } else if (stx || sty || stz) {
            PSMTXTrans(mScale, stx, sty, stz);
            flags |= 2;
        }
    }
    PSMTXIdentity(mRotX);
    if (rotZ) {
        PSMTXRotRad(mRot, 'z', MTXDegToRad(rotZ));
        PSMTXConcat(mRotX, mRot, mRotX);
        flags |= 4;
    }
    if (rotY) {
        PSMTXRotRad(mRot, 'y', MTXDegToRad(rotY));
        PSMTXConcat(mRotX, mRot, mRotX);
        flags |= 4;
    }
    if (rotX) {
        PSMTXRotRad(mRot, 'x', MTXDegToRad(rotX));
        PSMTXConcat(mRotX, mRot, mRotX);
        flags |= 4;
    }
    if (rcx != 0.0f || rcy != 0.0f || rcz != 0.0f) {
        if (flags & 4) {
            PSMTXTrans(t5, -rcx, -rcy, -rcz);
            PSMTXTrans(t4, rcx, rcy, rcz);
            PSMTXTrans(t6, rtx, rty, rtz);
            PSMTXConcat(t6, t4, mRot);
            PSMTXConcat(mRot, mRotX, mRotX);
            PSMTXConcat(mRotX, t5, mRotX);
        } else if (rtx || rty || rtz) {
            PSMTXTrans(mRotX, rtx, rty, rtz);
            flags |= 4;
        }
    }

    g_modeling_mtx_lv++;
    g_modeling_mtx = &g_modeling_mtx_stack[g_modeling_mtx_lv];
    switch (flags) {
        case 0:
            PSMTXIdentity(mRot);
            result = mRot;
            break;
        case 1:
            result = mTrans;
            break;
        case 2:
            result = mScale;
            break;
        case 3:
            PSMTXConcat(mTrans, mScale, mRot);
            result = mRot;
            break;
        case 4:
            result = mRotX;
            break;
        case 5:
            PSMTXConcat(mTrans, mRotX, mRot);
            result = mRot;
            break;
        case 6:
            PSMTXConcat(mRotX, mScale, mRot);
            result = mRot;
            break;
        case 7:
            PSMTXConcat(mTrans, mRotX, mRot);
            PSMTXConcat(mRot, mScale, mTrans);
            result = mTrans;
            break;
    }
    PSMTXConcat(g_modeling_mtx_stack[g_modeling_mtx_lv - 1], result, g_modeling_mtx_stack[g_modeling_mtx_lv]);
}

// pushGXModelMtx_JointNode__
// Matches retail. The node fields are read as
// truthiness because that is the only form MWCC emits with the value first in `fcmpu`
// (`fcmpu f5, f0`, not `fcmpu f0, f5`), which retail uses for the zero tests.
void pushGXModelMtx_JointNode__(void* node, f32 x, f32 y, f32 z) {
    AnimPoseNode* n = node;
    Mtx mTrans, mScale, mJointScale, mRotX, mRot;
    f32 (*result)[4];
    u32 flags = 0;
    f32 transX = n->mTranslation.x, transY = n->mTranslation.y, transZ = n->mTranslation.z;
    f32 scaleX = n->mScale.x, scaleY = n->mScale.y, scaleZ = n->mScale.z;
    f32 rot1X = n->mRot1.x * 2.0f, rot1Y = n->mRot1.y * 2.0f, rot1Z = n->mRot1.z * 2.0f;
    f32 rot2X = n->mRot2.x, rot2Y = n->mRot2.y, rot2Z = n->mRot2.z;

    if (transX || transY || transZ) {
        PSMTXTrans(mTrans, transX, transY, transZ);
        flags |= 1;
    }
    // retail tests z == 1 here (likely an original bug, kept)
    if (x != 1.0f || y != 1.0f || z == 1.0f) {
        if (x && y && z) {
            PSMTXScale(mJointScale, 1.0f / x, 1.0f / y, 1.0f / z);
        } else {
            PSMTXScale(mJointScale, 1.0f, 1.0f, 1.0f);
        }
        flags |= 2;
    }
    // retail tests scaleZ == 1 here (likely an original bug, kept)
    if (scaleX != 1.0f || scaleY != 1.0f || scaleZ == 1.0f) {
        PSMTXScale(mScale, scaleX, scaleY, scaleZ);
        flags |= 4;
    }
    PSMTXIdentity(mRotX);
    if (rot2Z) {
        PSMTXRotRad(mRot, 'z', MTXDegToRad(rot2Z));
        PSMTXConcat(mRotX, mRot, mRotX);
        flags |= 8;
    }
    if (rot2Y) {
        PSMTXRotRad(mRot, 'y', MTXDegToRad(rot2Y));
        PSMTXConcat(mRotX, mRot, mRotX);
        flags |= 8;
    }
    if (rot2X) {
        PSMTXRotRad(mRot, 'x', MTXDegToRad(rot2X));
        PSMTXConcat(mRotX, mRot, mRotX);
        flags |= 8;
    }
    if (rot1Z) {
        PSMTXRotRad(mRot, 'z', MTXDegToRad(rot1Z));
        PSMTXConcat(mRotX, mRot, mRotX);
        flags |= 8;
    }
    if (rot1Y) {
        PSMTXRotRad(mRot, 'y', MTXDegToRad(rot1Y));
        PSMTXConcat(mRotX, mRot, mRotX);
        flags |= 8;
    }
    if (rot1X) {
        PSMTXRotRad(mRot, 'x', MTXDegToRad(rot1X));
        PSMTXConcat(mRotX, mRot, mRotX);
        flags |= 8;
    }

    g_modeling_mtx_lv++;
    g_modeling_mtx = &g_modeling_mtx_stack[g_modeling_mtx_lv];
    switch (flags) {
        case 0:
            PSMTXIdentity(mRot);
            result = mRot;
            break;
        case 1:
            result = mTrans;
            break;
        case 2:
            result = mJointScale;
            break;
        case 3:
            PSMTXConcat(mTrans, mJointScale, mRot);
            result = mRot;
            break;
        case 4:
            result = mScale;
            break;
        case 5:
            PSMTXConcat(mTrans, mScale, mRot);
            result = mRot;
            break;
        case 6:
            PSMTXConcat(mJointScale, mScale, mRot);
            result = mRot;
            break;
        case 7:
            PSMTXConcat(mTrans, mJointScale, mRot);
            PSMTXConcat(mRot, mScale, mTrans);
            result = mTrans;
            break;
        case 8:
            result = mRotX;
            break;
        case 9:
            PSMTXConcat(mTrans, mRotX, mRot);
            result = mRot;
            break;
        case 10:
            PSMTXConcat(mJointScale, mRotX, mRot);
            result = mRot;
            break;
        case 11:
            PSMTXConcat(mTrans, mJointScale, mRot);
            PSMTXConcat(mRot, mRotX, mTrans);
            result = mTrans;
            break;
        case 12:
            PSMTXConcat(mRotX, mScale, mRot);
            result = mRot;
            break;
        case 13:
            PSMTXConcat(mTrans, mRotX, mRot);
            PSMTXConcat(mRot, mScale, mTrans);
            result = mTrans;
            break;
        case 14:
            PSMTXConcat(mJointScale, mRotX, mRot);
            PSMTXConcat(mRot, mScale, mTrans);
            result = mTrans;
            break;
        case 15:
            PSMTXConcat(mTrans, mJointScale, mRot);
            PSMTXConcat(mRot, mRotX, mTrans);
            PSMTXConcat(mTrans, mScale, mRot);
            result = mRot;
            break;
    }
    PSMTXConcat(g_modeling_mtx_stack[g_modeling_mtx_lv - 1], result, g_modeling_mtx_stack[g_modeling_mtx_lv]);
}

// NONMATCHING (97.89%): every instruction/constant/offset matches; what is left is register colouring
// (loop2 texMtxId/flags one off) plus the wrapS ternary materialising through r3 (`mr`) and the loop2
// `texMtxId` init (`mr r19,r21` in the target vs a `li` here).
void animSetMaterial_Texture(s32 texCount, s32* texIdRemap, AnimPoseTexEntry* pTexEntries, AnimTexMtx* animEntries,
                             s32 texBindsCapacity, AnimTexBind* texBinds, AnimationTextureFile* texFile) {
    Mtx mRot, mTrans, mScale, mTmp;
    GXTexObj texObj;
    GXTlutObj tlutObj;
    s32 i;
    s32 texMtxId;
    s32 j;
    GXTexWrapMode wrapT, wrapS;
    s32 mipmap;

    for (i = 0; i < texCount; i++) {
#ifdef MUST_MATCH // `1u` keeps the original's `subf; subi 1; slwi` instead of folding it to `subf; slwi; subi 4`
        s32 texId = texIdRemap[texCount - i - 1u];
#else
        s32 texId = texIdRemap[texCount - i - 1];
#endif
        AnimPoseTexEntry* texEntry;
        s32 frameExt, bindId;
        s32 wrapFlags;
        AnimTexBind* bind;
        s32 palId;

        texEntry = &pTexEntries[texId];
        bindId = texEntry->bindId;
        frameExt = animEntries[texId].mTexBindIdx;
        bindId += frameExt;

        if (bindId < 0 || bindId >= texBindsCapacity) {
            AnimationPose* pose = wp->mpCurPose;
            AnimationPoseData* poseData = *wp->files[pose->fileId].handle->data;
            bindId = 0;

            printf("Error: dispSetTevMode: \203x\201[\203X\203\202\203f\203\213%s \203A\203j\203\201 %s "
                   "\223\340\202\305 \203t\203\214\201[\203\200\203G\203N\203X\203e\203\223\203V\203\207\203\223\202\314\220"
                   "\335\222\350\202\252\215s\202\355\202\352\202\304\202\242\202\334\202\267\202\252\201A\212Y\223\226\202\267"
                   "\202\351\203e\203N\203X\203`\203\203\202\252\202\240\202\350\202\334\202\271\202\361 "
                   "\nframeExt\224\324\215\206 %d? \202\252\226\263\202\242\202\346\202\244\202\305\202\267\201B\203\202\203f"
                   "\203\213\202\334\202\275\202\315\203A\203j\203\201\202\311\217C\220\263\202\252\225K\227v\202\305\202\267\201B"
                   "textureIdx=%d\n",
                   poseData->fileName, ANIM_POSE_DATA_ANIMS(poseData)[pose->animId].name, frameExt, texEntry->bindId);
        }
        wrapFlags = texEntry->wrapFlags;
        bind = &texBinds[bindId];
        palId = *(s32*)&bind->field_0x0[4]; // field_0x0[4..7] holds the texture palette id; not named in AnimTexBind
        if (bind->texType == 0) {
            TEXDescriptor* desc = TEXGet((TEXPalette*)*texFile->handle->data, palId);

            if (desc->CLUTHeader) {
                desc = TEXGet((TEXPalette*)*texFile->handle->data, palId);
                wrapS = desc->textureHeader->wrapS;
                wrapT = desc->textureHeader->wrapT;
                if (wrapFlags >= 0) {
                    wrapS = (wrapFlags & 4) ? 2 : (wrapFlags & 1);
                    if (wrapFlags & 8) {
                        wrapT = 2;
                    } else {
                        wrapT = (wrapFlags >> 1) & 1;
                    }
                }
                mipmap = (desc->textureHeader->minLOD != desc->textureHeader->maxLOD);
                GXInitTlutObj(&tlutObj, desc->CLUTHeader->data, desc->CLUTHeader->format,
                              desc->CLUTHeader->numEntries);
                GXInitTexObjCI(&texObj, desc->textureHeader->data, desc->textureHeader->width,
                               desc->textureHeader->height, desc->textureHeader->format, wrapS, wrapT, mipmap, 0);
                GXInitTexObjLOD(&texObj, desc->textureHeader->minFilter, desc->textureHeader->magFilter,
                                (f32)desc->textureHeader->minLOD, (f32)desc->textureHeader->maxLOD,
                                desc->textureHeader->LODBias, GX_FALSE, desc->textureHeader->edgeLODEnable,
                                GX_ANISO_1);
                GXLoadTlut(&tlutObj, 0);
                GXLoadTexObj(&texObj, i);
            } else {
                desc = TEXGet((TEXPalette*)*texFile->handle->data, palId);
                wrapS = desc->textureHeader->wrapS;
                wrapT = desc->textureHeader->wrapT;
                if (wrapFlags >= 0) {
                    wrapS = (wrapFlags & 4) ? 2 : (wrapFlags & 1);
                    if (wrapFlags & 8) {
                        wrapT = 2;
                    } else {
                        wrapT = (wrapFlags >> 1) & 1;
                    }
                }
                GXInitTexObj(&texObj, desc->textureHeader->data, desc->textureHeader->width,
                             desc->textureHeader->height, desc->textureHeader->format, wrapS, wrapT,
                             (desc->textureHeader->minLOD != desc->textureHeader->maxLOD));
                GXInitTexObjLOD(&texObj, desc->textureHeader->minFilter, desc->textureHeader->magFilter,
                                (f32)desc->textureHeader->minLOD, (f32)desc->textureHeader->maxLOD,
                                desc->textureHeader->LODBias, GX_FALSE, desc->textureHeader->edgeLODEnable,
                                GX_ANISO_1);
                GXLoadTexObj(&texObj, i);
            }
        }
    }

    texMtxId = 0;
    for (j = 0; j < texCount; j++, texMtxId += 3) {
#ifdef MUST_MATCH // `1u` keeps the original's `subf; subi 1; slwi` instead of folding it to `subf; slwi; subi 4`
        s32 texId = texIdRemap[texCount - j - 1u];
#else
        s32 texId = texIdRemap[texCount - j - 1];
#endif
        AnimTexMtx* anim = &animEntries[texId];
        f32 scaleS, scaleT, transS, transT, rotate;
        u32 flags = 0;
        f32(*mtx)[4];

        rotate = anim->mRotate;
        scaleS = anim->mScaleS;
        scaleT = anim->mScaleT;
        transS = anim->mTransS;
        transT = anim->mTransT;
        if (rotate != 0.0f) {
            flags |= 1;
            PSMTXTrans(mTmp, 0.5f, 0.5f, 0.0f);
            PSMTXRotRad(mRot, 'z', MTXDegToRad(-rotate));
            PSMTXConcat(mTmp, mRot, mTrans);
            PSMTXTrans(mTmp, -0.5f, -0.5f, 0.0f);
            PSMTXConcat(mTrans, mTmp, mRot);
        }
        if (transS != 0.0f || transT != 0.0f || scaleT != 0.0f) {
            flags |= 2;
            PSMTXTrans(mTrans, transS, (1.0f - transT) - scaleT, 0.0f);
        }
        if (scaleS != 1.0f || scaleT != 1.0f) {
            flags |= 4;
            PSMTXScale(mScale, scaleS, scaleT, 1.0f);
        }
        switch (flags) {
            case 0:
                GXSetTexCoordGen2(j, GX_TG_MTX2x4, GX_TG_TEX0 + j, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
                continue;
            case 1:
                mtx = mRot;
                break;
            case 2:
                mtx = mTrans;
                break;
            case 3:
                PSMTXConcat(mRot, mTrans, mTmp);
                mtx = mTmp;
                break;
            case 4:
                mtx = mScale;
                break;
            case 5:
                PSMTXConcat(mRot, mScale, mTmp);
                mtx = mTmp;
                break;
            case 6:
                PSMTXConcat(mTrans, mScale, mTmp);
                mtx = mTmp;
                break;
            case 7:
                PSMTXConcat(mRot, mTrans, mTmp);
                PSMTXConcat(mTmp, mScale, mTrans);
                mtx = mTrans;
                break;
        }
        GXSetTexCoordGen2(j, GX_TG_MTX2x4, GX_TG_TEX0 + j, GX_TEXMTX0 + texMtxId, GX_FALSE, GX_PTIDENTITY);
        GXLoadTexMtxImm(mtx, GX_TEXMTX0 + texMtxId, GX_MTX2x4);
    }
}

inline BOOL animSetMaterial_ChangeTexture(AnimationPoseData* data, AnimPoseShapeDraw* draw, AnimationPose* pose,
                                          Mtx mtx, void** texData, GXTexObj* texObj, s32 texType) {
    AnimPoseTexEntry* texEntry;
    AnimTexMtx* texAnim;
    AnimTexBind* bind;
    s32 bindCount, texCount;
    s32 dataId, bindId, frameExt;
    GXTexObj* smartObj;
    int i;

    bind = data->texBinds;
    bindCount = data->texBindCount;
    texAnim = pose->mpTexAnimEntries;
    texEntry = data->texData;
    texCount = draw->texCount;

    for (i = 0; i < texCount; i++) {
        dataId = draw->texDataIds[texCount - i - 1];
        frameExt = texAnim[dataId].mTexBindIdx;
        bindId = texEntry[dataId].bindId + frameExt;
        if (bindId < 0 || bindId >= bindCount) {
            bindId = 0;
            //"Error: animSetMaterial_ChangeTexture: テクスチャがありません frameExt=%d\n"
            printf("Error: animSetMaterial_ChangeTexture: "
                   "\x83\x65\x83\x4E\x83\x58\x83\x60\x83\x83\x82\xAA\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1 "
                   "frameExt=%d\n",
                   frameExt);
        }
        if (bind[bindId].texType == texType) {
            if (texObj) {
                smartObj = smartTexObj(texObj, texData);
                GXLoadTexObj(smartObj, i);
                if (mtx) {
                    GXLoadTexMtxImm(mtx, GX_TEXMTX0 + (u32)(i * 3), GX_MTX2x4);
                    GXSetTexCoordGen(i, GX_TG_MTX2x4, GX_TG_TEX0 + i, GX_TEXMTX0 + (u32)(i * 3));
                } else {
                    GXSetTexCoordGen(i, GX_TG_MTX2x4, GX_TG_TEX0 + i, GX_IDENTITY);
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

// materialProc-local copy of animSetMaterial_ChangeTexture. The texture-argument group is declared
// (texObj, texData, mtx), retail's argument order for this inlining; the explicit `idx` keeps retail's
// separate `subi ..., 1` (MWCC folds an inline `count - i - 1` into the array base otherwise); and the
// interleaved local declaration order below (bind, bindCount, texAnim, texEntry, texCount) is what makes
// MWCC colour the cached values r23/r24/r25/r26 the way retail does.
static inline BOOL materialProc_ChangeTexture(AnimationPoseData* data, AnimPoseShapeDraw* draw, AnimationPose* pose,
                                              GXTexObj* texObj, void** texData, Mtx mtx, s32 texType) {
    AnimTexBind* bind;
    s32 bindCount;
    AnimTexMtx* texAnim;
    AnimPoseTexEntry* texEntry;
    s32 texCount;
    s32 dataId, bindId, frameExt, idx;
    GXTexObj* smartObj;
    int i;

    bind = data->texBinds;
    bindCount = data->texBindCount;
    texAnim = pose->mpTexAnimEntries;
    texEntry = data->texData;
    texCount = draw->texCount;

    for (i = 0; i < texCount; i++) {
        idx = texCount - i - 1;
        dataId = draw->texDataIds[idx];
        frameExt = texAnim[dataId].mTexBindIdx;
        bindId = texEntry[dataId].bindId + frameExt;
        if (bindId < 0 || bindId >= bindCount) {
            bindId = 0;
            //"Error: animSetMaterial_ChangeTexture: テクスチャがありません frameExt=%d\n"
            printf("Error: animSetMaterial_ChangeTexture: "
                   "\x83\x65\x83\x4E\x83\x58\x83\x60\x83\x83\x82\xAA\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1 "
                   "frameExt=%d\n",
                   frameExt);
        }
        if (bind[bindId].texType == texType) {
            if (texObj) {
                smartObj = smartTexObj(texObj, texData);
                GXLoadTexObj(smartObj, i);
                if (mtx) {
                    GXLoadTexMtxImm(mtx, GX_TEXMTX0 + (u32)(i * 3), GX_MTX2x4);
                    GXSetTexCoordGen(i, GX_TG_MTX2x4, GX_TG_TEX0 + i, GX_TEXMTX0 + (u32)(i * 3));
                } else {
                    GXSetTexCoordGen(i, GX_TG_MTX2x4, GX_TG_TEX0 + i, GX_IDENTITY);
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}
// materialProc
// The call/constant/byte-offset structure and the three inline ChangeTexture bodies all match. Caching each
// call's `wp->mpCurPoseData` in a local (`d1`/`d2`/`d3` below) puts every block's `wp` in retail's register
// (`r3`/`r4`/`r3`), and the flags&0x10 block tests `wp->mpTexObjs[0]`/`wp->paperMtxPtr[0]` directly (a
// local there made MWCC give texObj/mtx r26/r27 instead of retail's r27/r26).
// What still differs is pure register colouring: the typeFlags and flags&0x10 blocks keep pose in r4 and
// data in r5 (retail pose r5, data r4), and the three `bindId = texEntry[dataId].bindId + frameExt` adds
// have their operands commuted.
// NONMATCHING (99.77%): register allocation only; see the two points above.
void materialProc(s32 shapeId, s32 field_0x28) {
    CameraEntry* camera;
    AnimationPoseData* data;
    AnimPoseShape* shapes;
    int i;
    s32 drawCount;
    AnimPoseShapeDraw *draws, *draw;
    s32 dispMode, drawId;
    s32 texCount;
    GXTexObj* texObj;
    Mtx* mtx; // unused now that the flags&0x10 block reads wp->paperMtxPtr[0] directly; kept for retail's
              // declaration shape
    BOOL texChange1, check1;
    GXTexObj* currTexObj;
    Vec lightPos, lightPos2;
    Mtx camMtx, invCamMtx;
    AnimPoseShape* shape;

    if (shapeId != -1) {
        camera = camGetCurPtr();
        MTXConcat(camera->view, *g_modeling_mtx, camMtx);
        GXLoadPosMtxImm(camMtx, 0);
        MTXInvXpose(camMtx, invCamMtx);
        GXLoadNrmMtxImm(invCamMtx, 0);
        GXSetCurrentMtx(0);

        data = wp->mpCurPoseData;
        shapes = data->shapes;
        draws = data->draws;
        shape = &shapes[shapeId];
        dispMode = shape->dispMode;
        drawId = shape->shapeDrawStart;
        drawCount = shape->shapeDrawCount;
        draw = &draws[drawId];

        if ((dispMode == 0 && wp->currDispMode == 2) || dispMode == wp->currDispMode ||
            wp->mpCurPose->materialColor.a != 0xFF || wp->mpCurPose->materialColor2.a != 0xFF) {
            GXSetCullMode(tbl[shape->cullMode]);
            for (i = 0; i < drawCount; i++, draw++) {
                texCount = draw->texCount;
                if ((u32)wp->mpCurPose->disableDraw == 0) {
                    lightPos = vec3_802c2db0[0];
                    MTXMultVec(*g_modeling_mtx, &lightPos, &lightPos);
                    lightPos2 = lightPos;
                    mapSetMaterialLight(wp->mpCurPose->materialLightFlag, lightPos2);
                    animSetMaterial_Texture(draw->texCount, draw->texDataIds, wp->mpCurPoseData->texData,
                                            wp->mpCurPose->mpTexAnimEntries, wp->mpCurPoseData->texBindCount,
                                            wp->mpCurPoseData->texBinds,
                                            &wp->textures[wp->files[wp->mpCurPose->fileId].textureId]);
                    mapSetMaterialLastStageBlend(wp->mpCurPose->materialFlag, wp->mpCurPose->materialColor,
                                                 wp->mpCurPose->materialColor2);
                    camera = camGetPtr(CAMERA_3D);
                    if (camGetCurPtr() == camera) {
                        mapSetMaterialFog();
                    }
                    mapSetMaterialTev(texCount, draw->drawMode, wp->mpCurPose->materialFlag, *g_modeling_mtx);
                }
                if ((u32)wp->mpCurPose->typeFlags != 0) {
                    AnimationPoseData* d1 = wp->mpCurPoseData;
                    texChange1 = materialProc_ChangeTexture(d1, draw, wp->mpCurPose, wp->mpTexObjs[0],
                                                           wp->mpTexDatas[0], *wp->paperMtxPtr[0], 1);
                    check1 = texChange1;
                    if (!texChange1) {
                        AnimationPoseData* d2 = wp->mpCurPoseData;
                        check1 = materialProc_ChangeTexture(d2, draw, wp->mpCurPose, wp->mpTexObjs[1],
                                                           wp->mpTexDatas[1], *wp->paperMtxPtr[1], 2);
                    }
                    if (check1 && !draw->drawMode) {
                        currTexObj = wp->mpCurTexObj;
                        if (currTexObj) {
                            smartTexObj(currTexObj, wp->mpCurTexData);
                            if (wp->currDispMode != 3) {
                                GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
                                GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
                                GXSetZCompLoc(GX_FALSE);
                                GXSetAlphaCompare(GX_GEQUAL, 0x80u, GX_AOP_OR, GX_NEVER, 0);
                            }
                            texObj = wp->mpTexObjs[texChange1 == 0];
                            if (texObj) {
                                GXLoadTexObj(texObj, GX_TEXMAP0);
                                GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE,
                                                  GX_PTIDENTITY);
                                // `!texChange1` stops MWCC CSE-ing the second `texChange1 == 0`; retail recomputes cntlzw
                                GXLoadTexMtxImm(*wp->paperMtxPtr[!texChange1], GX_TEXMTX0, GX_MTX2x4);
                            }
                            GXLoadTexObj(currTexObj, GX_TEXMAP1);
                            GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE,
                                              GX_PTIDENTITY);
                            GXLoadTexMtxImm(*wp->paperMtxPtr[2], GX_TEXMTX1, GX_MTX2x4);
                            mapSetMaterialTev(2, 12, wp->mpCurPose->materialFlag, *g_modeling_mtx);
                        }
                    }
                }

                if (wp->mpCurPose->flags & 0x10) {
                    if (wp->mpTexObjs[0]) {
                        if (wp->paperMtxPtr[0]) {
                            AnimationPoseData* d3 = wp->mpCurPoseData;
                            materialProc_ChangeTexture(d3, draw, wp->mpCurPose, wp->mpTexObjs[0], wp->mpTexDatas[0],
                                                       *wp->paperMtxPtr[0], 1);
                        }
                    }
                }
            }
        }
    }
}

// renderProc-local copy of animSetMaterial_ChangeTexture. The texture-argument group is declared
// (texObj, texData, mtx) so MWCC's right-to-left argument evaluation loads mtx first, then texData,
// then texObj, matching retail. The `idx` local keeps retail's separate `subi ..., 1` (MWCC folds
// an inline `count - i - 1` into the array base otherwise).
static inline BOOL renderProc_ChangeTexture(AnimationPoseData* data, AnimPoseShapeDraw* draw, AnimationPose* pose,
                                            GXTexObj* texObj, void** texData, Mtx mtx, s32 texType) {
    AnimTexBind* bind;
    AnimPoseTexEntry* texEntry;
    AnimTexMtx* texAnim;
    s32 bindCount, texCount;
    s32 dataId, bindId, frameExt, idx;
    GXTexObj* smartObj;
    int i;

    bind = data->texBinds;
    bindCount = data->texBindCount;
    texAnim = pose->mpTexAnimEntries;
    texEntry = data->texData;
    texCount = draw->texCount;

    for (i = 0; i < texCount; i++) {
        idx = texCount - i - 1;
        dataId = draw->texDataIds[idx];
        frameExt = texAnim[dataId].mTexBindIdx;
        bindId = texEntry[dataId].bindId + frameExt;
        if (bindId < 0 || bindId >= bindCount) {
            bindId = 0;
            //"Error: animSetMaterial_ChangeTexture: テクスチャがありません frameExt=%d\n"
            printf("Error: animSetMaterial_ChangeTexture: "
                   "\x83\x65\x83\x4E\x83\x58\x83\x60\x83\x83\x82\xAA\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1 "
                   "frameExt=%d\n",
                   frameExt);
        }
        if (bind[bindId].texType == texType) {
            if (texObj) {
                smartObj = smartTexObj(texObj, texData);
                GXLoadTexObj(smartObj, i);
                if (mtx) {
                    GXLoadTexMtxImm(mtx, GX_TEXMTX0 + (u32)(i * 3), GX_MTX2x4);
                    GXSetTexCoordGen(i, GX_TG_MTX2x4, GX_TG_TEX0 + i, GX_TEXMTX0 + (u32)(i * 3));
                } else {
                    GXSetTexCoordGen(i, GX_TG_MTX2x4, GX_TG_TEX0 + i, GX_IDENTITY);
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

// I'm so sorry, whoever gets tasked with matching this function
// note: I detect at least 8 inlined functions here.
// All calls (64), constants and byte offsets match; the inner `l` vertex loop's `<` trip-count spelling makes the
// `.p` unroller emit retail's split `srwi`/`andi` remainder guard and its `ble` entry guard, and the `k1`
// `!=` loop keeps the split guard too. The ChangeTexture body is inlined through
// the renderProc_ChangeTexture helper above (group declared (texObj, texData, mtx) so MWCC's right-to-left
// argument evaluation loads mtx first, and an explicit `idx = texCount - i - 1` keeps retail's `subi ..., 1`
// instead of folding it into the array base).
// The remaining diffs are callee-saved register colouring: retail keeps the outer-loop `draw`/`i` in r28/r29
// and the draw-call scratch in r20..r27; this build lands them in r23/r22 and r15..r21. The non-register
// remainder is retail's folded `lbz -0x1(base)` for texIds[texCount-l-1] (this build emits `subi` + a
// byte-indexed `lbzx`) and one `ble`/`beq` unroll guard (the `k1` loop emits `beq` where retail has `ble`).
// NONMATCHING (96.55%): all calls, constants and offsets match; only callee-saved register colouring differs.
void renderProc(s32 shapeId, s32 field_0x28) {
    s32 dispMode, drawId;
    s32 texIndex;
    u16 *posIndexPtr, *nrmIndexPtr, *clr0IndexPtr, *texIndexPtr;
    u8* texEntryPtr;
    s32 texCount;
    GXTexObj* currTexObj;
    AnimPoseShapeDraw *draws, *draw;
    int i, j, k, k1, l;
    f32* buffer;
    s32* firstId;
    AnimDrawCallEntry* drawCall;
    Vec *nrmVtxPtr, *posVtxPtr;
    CameraEntry* camera;
    u16 count;
    BOOL texChange1, check1, texChange2, check2;
    s32 firstDrawId;
    s32 posFirstId, nrmFirstId, clr0FirstId;
    AnimationPoseData* data;
    AnimPoseShape *shapes, *shape;
    GXColor* clr0VtxPtr;
    s32 drawCallCount;
    s32 index;
    AnimationPose* pose;
    GXTexObj* texObj;
    s32 posFirstVtx, nrmFirstVtx, clr0FirstVtx;
    Mtx* mtx;
    s8* texIds;

    // stack variables
    s32 clr0VtxSize, posVtxSize, nrmVtxSize;
    s32 drawCount;
    Mtx camMtx, invCamMtx;
    f32* texVtxIndices[8];
    Vec lightPos, lightPos2; // TODO: rename? 2 is probably implicit stack copy

    if (shapeId != -1) {
        camera = camGetCurPtr();
        MTXConcat(camera->view, *g_modeling_mtx, camMtx);
        GXLoadPosMtxImm(camMtx, 0);
        MTXInvXpose(camMtx, invCamMtx);
        GXLoadNrmMtxImm(invCamMtx, 0);
        GXSetCurrentMtx(0);

        data = wp->mpCurPoseData;
        shapes = data->shapes;
        draws = data->draws;
        shape = &shapes[shapeId];
        dispMode = shape->dispMode;
        drawId = shape->shapeDrawStart;
        posFirstVtx = shape->vtxPos.first;
        nrmFirstVtx = shape->vtxNrm.first;
        clr0FirstVtx = shape->vtxClr0.first;
        drawCount = shape->shapeDrawCount;
        draw = &draws[drawId];

        if ((dispMode == 0 && wp->currDispMode == 2) || dispMode == wp->currDispMode ||
            wp->mpCurPose->materialColor.a != 0xFF || wp->mpCurPose->materialColor2.a != 0xFF) {
            GXSetCullMode(tbl[shape->cullMode]);
            data = wp->mpCurPoseData;
            posVtxSize = (s32)sizeof(Vec) * posFirstVtx;
            clr0VtxSize = (s32)sizeof(GXColor) * clr0FirstVtx;
            nrmVtxSize = (s32)sizeof(Vec) * nrmFirstVtx;
            buffer = data->texVtxBuffer;
            texVtxIndices[0] = (f32*)((u8*)buffer + (sizeof(f32) * 2) * shape->vtxTex[0].first);
            texVtxIndices[1] = (f32*)((u8*)buffer + (sizeof(f32) * 2) * shape->vtxTex[1].first);
            texVtxIndices[2] = (f32*)((u8*)buffer + (sizeof(f32) * 2) * shape->vtxTex[2].first);
            texVtxIndices[3] = (f32*)((u8*)buffer + (sizeof(f32) * 2) * shape->vtxTex[3].first);
            texVtxIndices[4] = (f32*)((u8*)buffer + (sizeof(f32) * 2) * shape->vtxTex[4].first);
            texVtxIndices[5] = (f32*)((u8*)buffer + (sizeof(f32) * 2) * shape->vtxTex[5].first);
            texVtxIndices[6] = (f32*)((u8*)buffer + (sizeof(f32) * 2) * shape->vtxTex[6].first);
            texVtxIndices[7] = (f32*)((u8*)buffer + (sizeof(f32) * 2) * shape->vtxTex[7].first);

            for (i = 0; i < drawCount; i++, draw++) {
                texCount = draw->texCount;
                if ((u32)wp->mpCurPose->disableDraw == 0) {
                    lightPos = vec3_802c2dbc[0];
                    MTXMultVec(*g_modeling_mtx, &lightPos, &lightPos);
                    lightPos2 = lightPos;
                    mapSetMaterialLight(wp->mpCurPose->materialLightFlag, lightPos2);
                    animSetMaterial_Texture(draw->texCount, draw->texDataIds, wp->mpCurPoseData->texData,
                                            wp->mpCurPose->mpTexAnimEntries, wp->mpCurPoseData->texBindCount,
                                            wp->mpCurPoseData->texBinds,
                                            &wp->textures[wp->files[wp->mpCurPose->fileId].textureId]);
                    mapSetMaterialLastStageBlend(wp->mpCurPose->materialFlag, wp->mpCurPose->materialColor,
                                                 wp->mpCurPose->materialColor2);
                    camera = camGetPtr(CAMERA_3D);
                    if (camGetCurPtr() == camera) {
                        mapSetMaterialFog();
                    }
                    mapSetMaterialTev(texCount, draw->drawMode, wp->mpCurPose->materialFlag, *g_modeling_mtx);
                }
                pose = wp->mpCurPose;
                if ((u32)pose->typeFlags != 0) {
                    AnimationPoseData* d1 = wp->mpCurPoseData;
                    texChange1 = renderProc_ChangeTexture(d1, draw, pose, wp->mpTexObjs[0],
                                                          wp->mpTexDatas[0], *wp->paperMtxPtr[0], 1);
                    check1 = texChange1;
                    if (!texChange1) {
                        texChange2 = renderProc_ChangeTexture(wp->mpCurPoseData, draw, pose, wp->mpTexObjs[1],
                                                              wp->mpTexDatas[1], *wp->paperMtxPtr[1], 2);
                        check1 = texChange2;
                        check2 = texChange2;
                    } else {
                        check2 = FALSE;
                    }

                    if (check1 && (texChange1 && (s32)wp->field_0xD8 == 0 || check2 && (s32)wp->field_0xDC == 0) &&
                        !draw->drawMode) {
                        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
                    }
                    if (check1 && !draw->drawMode) {
                        currTexObj = wp->mpCurTexObj;
                        if (currTexObj) {
                            smartTexObj(currTexObj, wp->mpCurTexData);
                            if (wp->currDispMode != 3) {
                                GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
                                GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
                                GXSetZCompLoc(GX_FALSE);
                                GXSetAlphaCompare(GX_GEQUAL, 0x80u, GX_AOP_OR, GX_NEVER, 0);
                            }
                            texObj = wp->mpTexObjs[texChange1 == 0];
                            if (texObj) {
                                GXLoadTexObj(texObj, GX_TEXMAP0);
                                GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE,
                                                  GX_PTIDENTITY);
                                GXLoadTexMtxImm(*wp->paperMtxPtr[!texChange1], GX_TEXMTX0, GX_MTX2x4);
                            }
                            GXLoadTexObj(currTexObj, GX_TEXMAP1);
                            GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE,
                                              GX_PTIDENTITY);
                            GXLoadTexMtxImm(*wp->paperMtxPtr[2], GX_TEXMTX1, GX_MTX2x4);
                            mapSetMaterialTev(2, 12, wp->mpCurPose->materialFlag, *g_modeling_mtx);
                        }
                    }
                }

                pose = wp->mpCurPose;
                if (pose->flags & 0x10) {
                    texObj = wp->mpTexObjs[0];
                    if (texObj) {
                        mtx = wp->paperMtxPtr[0];
                        if (mtx) {
                            renderProc_ChangeTexture(wp->mpCurPoseData, draw, pose, texObj, wp->mpTexDatas[0],
                                                     *mtx, 1);
                        }
                    }
                }

                texIds = (s8*)draw->texArrayIds;
                firstId = draw->firstIdTex;
                data = wp->mpCurPoseData; // inlined function here
                drawCall = &data->drawCalls[draw->firstDrawCall];
                texCount = draw->texCount;
                drawCallCount = draw->drawCallCount;
                clr0VtxPtr = (GXColor*)((u8*)data->clr0VtxBuffer + clr0VtxSize);
                posFirstId = draw->firstIdPos;
                nrmFirstId = draw->firstIdNrm;
                clr0FirstId = draw->firstIdClr0;

                if ((s32)wp->mbUseFloatScratch != 0) {
                    nrmVtxPtr = (Vec*)((u8*)wp->vtxArrayNrm + nrmVtxSize);
                    posVtxPtr = (Vec*)((u8*)wp->vtxArrayPos + posVtxSize);
                } else {
                    pose = wp->mpCurPose;
                    posVtxPtr = (Vec*)((u8*)pose->mpVtxArrayPos + posVtxSize);
                    nrmVtxPtr = (Vec*)((u8*)pose->mpVtxArrayNrm + nrmVtxSize);
                }
                GXClearVtxDesc();
                GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
                GXSetArray(GX_VA_POS, posVtxPtr, sizeof(Vec));
                GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
                GXSetArray(GX_VA_NRM, nrmVtxPtr, sizeof(Vec));
                GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
                GXSetArray(GX_VA_CLR0, clr0VtxPtr, sizeof(GXColor));

                if (texCount == 1) { // just use texIndex[0]
                    GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
                    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
                    GXSetArray(GX_VA_TEX0, texVtxIndices[texIds[0]], sizeof(f32) * 2);
                    for (j = 0; j < drawCallCount; j++, drawCall++) {
                        data = wp->mpCurPoseData; // doubly inlined function here
                        count = drawCall->count;
                        // indices are stored as u32, has to do math to load the lower 2 bytes
                        posIndexPtr = (u16*)((u8*)data->posIndex + (drawCall->first + posFirstId) * 4 + 2);
                        nrmIndexPtr = (u16*)((u8*)data->nrmIndex + (drawCall->first + nrmFirstId) * 4 + 2);
                        clr0IndexPtr = (u16*)((u8*)data->clr0Index + (drawCall->first + clr0FirstId) * 4 + 2);
                        texIndexPtr = (u16*)((u8*)data->texIndex[texIds[0]] + (drawCall->first + firstId[texIds[0]]) * 4 + 2);
                        GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, count);
                        for (k1 = 0; k1 != count; k1++) { // unrolled, warning
                            GXPosition1x16(*posIndexPtr);
                            posIndexPtr += 2;
                            GXNormal1x16(*nrmIndexPtr);
                            nrmIndexPtr += 2;
                            GXColor1x16(*clr0IndexPtr);
                            clr0IndexPtr += 2;
                            GXTexCoord1x16(*texIndexPtr);
                            texIndexPtr += 2;
                        }
                    }
                } else { // enumerate texIndex
                    for (j = 0; j < texCount; j++) {
                        GXSetVtxDesc(GX_VA_TEX0 + j, GX_INDEX16);
                        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0 + j, GX_TEX_ST, GX_F32, 0);
                        GXSetArray(GX_VA_TEX0 + j, texVtxIndices[texIds[texCount - j - 1]], sizeof(f32) * 2);
                    }
                    for (j = 0; j < drawCallCount; j++, drawCall++) {
                        firstDrawId = drawCall->first;
                        data = wp->mpCurPoseData; // doubly inlined function here
                        count = drawCall->count;
                        // indices are stored as u32, has to do math to load the lower 2 bytes
                        posIndexPtr = (u16*)((u8*)data->posIndex + (drawCall->first + posFirstId) * 4 + 2);
                        nrmIndexPtr = (u16*)((u8*)data->nrmIndex + (drawCall->first + nrmFirstId) * 4 + 2);
                        clr0IndexPtr = (u16*)((u8*)data->clr0Index + (drawCall->first + clr0FirstId) * 4 + 2);
                        GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, count);

                        for (k = 0, texIndex = 0; k < count; k++) {
                            GXPosition1x16(*posIndexPtr);
                            GXNormal1x16(*nrmIndexPtr);
                            GXColor1x16(*clr0IndexPtr);
                            for (l = 0; l < texCount; l++) {
                                index = texIds[texCount - l - 1];
                                // The header types texIndex as u16*, but retail loads a full word (lwzx) and
                                // stores its low half; the pointer is a byte base and texIndex is the byte offset
                                // into the entry, stepping 4 per vertex.
                                texEntryPtr = (u8*)wp->mpCurPoseData->texIndex[index];
                                GXTexCoord1x16(*(u32*)(texEntryPtr + (firstDrawId + firstId[index]) * 4 + texIndex));
                            }
                            texIndex += 4;
                            posIndexPtr += 2;
                            nrmIndexPtr += 2;
                            clr0IndexPtr += 2;
                        }
                    }
                }
            }
        }
    }
}

// dispProc
void dispProc(s32 parentGroupId, s32 groupId, s32 field_0x28, s32 field_0x2c, f32 f1) {
    AnimPoseGroup* group;
    AnimPoseGroup* groups;
    AnimationPoseData* data;
    f32* nodes;
    f32* node;
    f32 x, y, z;

    if (groupId == -1) {
        return;
    }
    data = wp->mpCurPoseData;
    groups = ANIM_POSE_DATA_GROUPS(data);
    group = &groups[groupId];
#ifdef MUST_MATCH
    // MWCC picks the original registers only when the sign extension is a cast on the loaded element; the
    // pointer form under #else is the natural equivalent (same exceptions, other registers).
    if ((s8)wp->mpCurPose->mpGroupVisibility[group->visibilityId] != 0) {
#else
    if (((s8*)wp->mpCurPose->mpGroupVisibility)[group->visibilityId] != 0) {
#endif
        nodes = (f32*)wp->mpCurPose->mpNodes;
        node = &nodes[group->nodeId];
        if (group->useSSC) {
            if (parentGroupId == -1) {
                x = y = z = float_1_80419a44;
            } else {
                AnimPoseGroup* parent = &groups[parentGroupId];
                if (!parent->useSSC) {
                    x = y = z = float_1_80419a44;
                } else {
                    x = nodes[parent->nodeId + 3];
                    y = nodes[parent->nodeId + 4];
                    z = nodes[parent->nodeId + 5];
                }
            }
            pushGXModelMtx_JointNode__(node, x, y, z);
        } else {
            pushGXModelMtx_TransformNode__(node);
        }

        if (wp->mpCurPose->flags & 0x40) {
            Vec pos = vec3_802c2dc8[0];
            Vec* buffer = (Vec*)(*(smartEntry**)((u8*)wp->mpCurPose + 0x15C))->address;
            pos.x = (*g_modeling_mtx)[0][3];
            pos.y = (*g_modeling_mtx)[1][3];
            pos.z = (*g_modeling_mtx)[2][3];
            buffer[groupId] = pos;
        }
        if (wp->mpCurPose->flags & 0x80) {
            Mtx* dest = (Mtx*)(*(smartEntry**)((u8*)wp->mpCurPose + 0x164))->address;
            PSMTXCopy(*g_modeling_mtx, dest[groupId]);
        }
        if ((wp->mpCurPose->flags & 0x20) && groupId == *wp->mpCurPose->vivianGroupNo) {
            Mtx* mtx = g_modeling_mtx;
            f32 (*matrix)[4] = wp->mpCurPose->matrix;
            f32* params = (f32*)((u8*)vivihimoData[wp->mpCurPose->vivianType] + wp->mpCurPose->animId * 8);
            f32 v1 = float_0_80419a48;
            f32 v2 = v1;
            int i, j;

            switch (wp->mpCurPose->vivianType) {
                case 1:
                    v1 = float_neg2_80419ab8;
                    v2 = float_0p5_80419a64;
                    break;
                case 2:
                    v1 = float_neg3_80419abc;
                    v2 = float_0p5_80419a64;
                    break;
                case 3:
                    v1 = float_neg1_80419a8c;
                    v2 = float_0p5_80419a64;
                    break;
            }
            materialProc(group->shapeId, field_0x28);
            vivihimo(*mtx, matrix, params[0], params[1], v1, v2);
            wp->mpCurPose->flags &= ~0x20;
            for (i = 0; i < 3; i++) {
                for (j = 0; j < 3; j++) {
                    matrix[i][j] += float_0p04_80419ac0 * ((*mtx)[i][j] - matrix[i][j]);
                }
            }
        } else {
            if (wp->mpCurPose->dispCallback != NULL) {
                ((void (*)(void*, s32, Mtx))wp->mpCurPose->dispCallback)(wp->mpCurPose->dispUserDataCallback, groupId,
                                                                          *g_modeling_mtx);
            }
            renderProc(group->shapeId, field_0x28);
            dispProc(groupId, group->childGroupId, field_0x28, field_0x2c, f1);
        }
        g_modeling_mtx_lv--;
        g_modeling_mtx = &g_modeling_mtx_stack[g_modeling_mtx_lv];
    }
    dispProc(groupId, group->brotherGroupId, field_0x28, field_0x2c, f1);
}

void animPoseDraw(s32 poseId, f32 x, f32 y, f32 z, f32 rotY, f32 scale, s32 xluMode) {
    Mtx matrix;

    PSMTXTrans(matrix, x, y, z);
    animPoseDrawMtx(poseId, matrix, xluMode, rotY, scale);
}


// The original inlines one camera-angle helper at all three sites; the macro keeps the
// target's frsp/lfs ordering.
#define ANIM_GET_ANGLE(angle)                                                                       \
    do {                                                                                              \
        CameraEntry* cam = camGetPtr(CAMERA_3D);                                                      \
        f32 raw = (f32)atan2(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z)); \
        angle = float_rad2deg_80419a94;                                                               \
        angle *= raw;                                                                                 \
        while (angle < float_neg360_80419a9c) {                                                       \
            angle += float_360_80419a98;                                                              \
        }                                                                                             \
        while (angle >= float_360_80419a98) {                                                         \
            angle -= float_360_80419a98;                                                              \
        }                                                                                             \
        if (angle < float_0_80419a48) {                                                               \
            angle += float_360_80419a98;                                                              \
        }                                                                                             \
    } while (0)

// _animPoseDrawMtx
// The three inlined copies of the camera-angle helper and the two caller normalisations
// read the file's named angle constants; MWCC must reload them at each loop rather than
// CSE one register across the loops.
#pragma opt_common_subs off // keeps the lastFrameZero block's GPR colouring
// Still different: the helper's first-loop constants land swapped (360 in f0, -360 in f1; the
// target has 360 in f1, -360 in f0), which also costs one extra 360 reload after the loops; the
// rotation normalisation (loop-2 360 in f4 vs f2; rotationY/diff in f2/f1 vs the target's f1/f4)
// and the lastFrameZero scratch (posNum/nrmNum/RP in r8/r0/r5 vs the target's r6/r5/r8) differ only
// in register colouring.
// NONMATCHING (98.61%): register colouring only (see above).
void _animPoseDrawMtx(AnimationPose* pose, Mtx matrix, s32 xluMode, f32 rotY, f32 scale) {
    AnimationPoseData* poseData;
    AnimData* animData;
    Mtx dstMtx;
    Mtx rotMtx;
    Mtx rotMtx2;
    Mtx scaleMtx;
    GXColor color0;
    GXColor color1;
    s32 field_0x28;
    s32 field_0x2c;
    f32 field_0x30;
    s32 stage;
    f32 neg1;
    s32 useScratch;

    poseData = animPoseGetBaseData(pose);
    animData = ANIM_POSE_DATA_ANIMS(poseData)[pose->animId].data;
    field_0x28 = *(s32*)((u8*)pose + 0x28);
    field_0x2c = *(s32*)((u8*)pose + 0x2C);
    field_0x30 = *(f32*)((u8*)pose + 0x30);
    if (animData == NULL) {
        return;
    }

    {
        s32 inBattle = wp->inBattle;

        if (inBattle) {
            if (pose->type == 0) {
                return;
            }
        }
        if (!inBattle) {
            if (pose->type == 1) {
                return;
            }
        }
    }

    if (!(xluMode == 2 && (*(u32*)poseData->field_0xC4 & 1)) &&
        !(*(u32*)poseData->field_0xC4 & (1 << xluMode))) {
        return;
    }

    stage = xluMode;
    color1 = pose->materialColor2;
    color0 = pose->materialColor;
    if (mapTestXLU(pose->materialFlag, &color0, &color1)) {
        stage = 3;
    }
    if (pose->gxCallback != NULL) {
        pose->gxCallback(stage);
    } else {
        switch (stage) {
            case 1:
                GXSetZMode(1, 3, 1);
                GXSetBlendMode(0, 4, 5, 0);
                GXSetZCompLoc(1);
                GXSetAlphaCompare(7, 0, 0, 7, 0);
                break;
            case 0:
            case 2:
                GXSetZMode(1, 3, 1);
                GXSetBlendMode(0, 4, 5, 0);
                GXSetZCompLoc(0);
                GXSetAlphaCompare(6, 0x80, 1, 0, 0);
                break;
            case 3:
                GXSetZMode(1, 3, 0);
                GXSetBlendMode(1, 4, 5, 0);
                GXSetZCompLoc(1);
                GXSetAlphaCompare(7, 0, 0, 7, 0);
                break;
        }
        wp->currDispMode = stage;
    }

    if (pose->flags & 0x8) {
        f32 angle;
        f32 diff;

        rotY += float_neg90_80419a90;
        pose->field_0x70 = rotY;
        ANIM_GET_ANGLE(diff);
        diff = diff - pose->field_0x70;
        while (diff < float_neg360_80419a9c) {
            diff += float_360_80419a98;
        }
        while (diff >= float_360_80419a98) {
            diff -= float_360_80419a98;
        }
        if ((diff > float_90_80419aa0 && diff <= float_270_80419aa4) ||
            (diff < float_neg90_80419a90 && diff >= float_neg270_80419aa8)) {
            ANIM_GET_ANGLE(angle);
            angle = float_180_80419aac + angle;
            while (angle < float_neg360_80419a9c) {
                angle += float_360_80419a98;
            }
            while (angle >= float_360_80419a98) {
                angle -= float_360_80419a98;
            }
            pose->field_0x7C = angle;
        } else {
            ANIM_GET_ANGLE(angle);
            pose->field_0x7C = angle;
        }
        while (pose->field_0x7C - pose->rotationY > float_180_80419aac) {
            pose->rotationY += float_360_80419a98;
        }
        while (pose->field_0x7C - pose->rotationY < float_neg180_80419ab0) {
            pose->rotationY -= float_360_80419a98;
        }
        if (pose->flags & 0x100) {
            pose->rotationY = pose->field_0x7C;
        } else {
            pose->rotationY = float_0p1_80419ab4 * (pose->field_0x7C - pose->rotationY) + pose->rotationY;
        }
        rotY = pose->rotationY;
        if (float_0_80419a48 != rotY) {
            PSMTXRotRad(rotMtx, 'y', float_deg2rad_80419a4c * rotY);
            PSMTXConcat(matrix, rotMtx, dstMtx);
            matrix = dstMtx;
        }
        neg1 = float_neg1_80419a8c;
        if (neg1 == pose->field_0x78) {
            PSMTXScale(rotMtx, float_1_80419a44, float_1_80419a44, neg1);
            PSMTXConcat(matrix, rotMtx, dstMtx);
            matrix = dstMtx;
        }
    } else {
        if (rotY != float_0_80419a48) {
            PSMTXRotRad(rotMtx2, 'y', float_deg2rad_80419a4c * rotY);
            PSMTXConcat(matrix, rotMtx2, dstMtx);
            matrix = dstMtx;
        }
    }

    if (float_1_80419a44 != scale) {
        PSMTXScale(scaleMtx, scale, scale, scale);
        PSMTXConcat(matrix, scaleMtx, dstMtx);
        matrix = dstMtx;
    }

    if (pose->lastFrameZero == -1) {
        s32 poseIndex = ((u32)pose - (u32)wp->poses) / sizeof(AnimationPose);

        if (pose->typeFlags & 2) {
            AnimationPoseData* data;
            s32 posNum;

            data = animPoseGetBaseData(pose);
            posNum = data->mBufferPosNum;
            if (posNum + (s32)data->mBufferNrmNum <= (s32)wp->mFloatScratchRP) {
                if ((s32)wp->mFloatScratchWP + posNum + (s32)data->mBufferNrmNum > (s32)wp->mFloatScratchRP) {
                    wp->mFloatScratchWP = 0;
                }
                wp->mbUseFloatScratch = 1;
                wp->vtxArrayPos = (Vec*)((u8*)wp->mpFloatScratch + wp->mFloatScratchWP * 12);
                wp->mFloatScratchWP += posNum;
                wp->vtxArrayNrm = (Vec*)((u8*)wp->mpFloatScratch + wp->mFloatScratchWP * 12);
                wp->mFloatScratchWP += (s32)data->mBufferNrmNum;
            }
            animPoseMain(poseIndex);
            wp->mbUseFloatScratch = 0;
        } else {
            animPoseMain(poseIndex);
        }
    }

    g_modeling_mtx_lv = 0;
    g_modeling_mtx = g_modeling_mtx_stack;
    PSMTXCopy(matrix, *g_modeling_mtx);
    wp->mpCurPose = pose;
    wp->mpCurAnimData = animData;
    wp->mpCurPoseData = poseData;
    useScratch = (s32)wp->mbUseFloatScratch;
    if (useScratch) {
        DCFlushRange(wp->vtxArrayPos, animPoseGetBaseData(pose)->mBufferPosNum * 12);
        DCFlushRange(wp->vtxArrayNrm, animPoseGetBaseData(pose)->mBufferNrmNum * 12);
    } else {
        DCFlushRange(pose->mpVtxArrayPos, animPoseGetBaseData(pose)->mBufferPosNum * 12);
        DCFlushRange(pose->mpVtxArrayNrm, animPoseGetBaseData(pose)->mBufferNrmNum * 12);
    }
    GXInvalidateVtxCache();
    dispProc(-1, poseData->groupCount - 1, field_0x28, field_0x2c, field_0x30);
}
#pragma opt_common_subs reset

// Searches ANIM_POSE_DATA_ANIMS(data) for `name` when i is negative (i is the string's address used as
// a sentinel), then clamps the result into [0, top). Used by animPoseDrawMtx below. The `return` inside the loop makes MWCC emit the target's
// `bne <increment>; b <found>` branch shape, indexing by i makes it emit the target's `offset = i`
// copy, and the shared `goto zero` tail makes both zero exits one `li`; a plain `goto`/`break`
// collapses the branch into a single `beq` and drops the copy. animPoseSearchAnim (above) cannot be
// used here: its `i = 0` fallback adds a redundant re-test.
static inline s32 animPoseSearchAnimIdx(AnimationPoseData* data, const char* name, s32 i, s32 top) {
    if (i < 0) {
        for (i = 0; i < top; i++) {
            if (!strcmp(ANIM_POSE_DATA_ANIMS(data)[i].name, name)) {
                return i;
            }
        }
        goto zero;
    }
    if (i < top) {
        return i;
    }
zero:
    return 0;
}
// dont_inline keeps animPoseDrawMtx a real bl from animPoseDraw (retail emits one).
#pragma dont_inline on
// All logic/constants/offsets match and the animId search now uses the target's branch shape, so every
// remaining diff is callee-saved register allocation. The target colours the long-lived webs
// pose/paperPose/pMtx/xluMode/matrix/poseData to r31/r30/r29/r28/r27/r26; this build puts xluMode in r29
// and drops pMtx/poseData to r23/r24, so the whole function is a permutation of the same registers. The
// local declaration order below was searched to get the typeFlags block's posNum/nrmNum and the animArg
// block's top/found right (it moved the score 98.81 -> 99.24%); no order found reaches the target's
// colouring. No pragma helps. The inner shadow variables (pp/ppd) reproduce the target's non-CSEd
// rematerialisation of the paper pose inside the animArg block (outer r30, inner r29).
// NONMATCHING (99.24%): register allocation only (60 instructions, all register renames).
void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale) {
    AnimationPose* pose = &wp->poses[poseId];
    s32 top;
    s32 nrmNum;
    s32 animArg;
    Mtx mtxE;
    Mtx mtxD;
    Mtx mtxC;
    Mtx mtxB;
    Mtx mtxA;
    f32(*pMtx)[4];
    s32 found;
    AnimationPoseData* paperPoseData;
    AnimationPoseData* poseData;
    s32 posNum;
    AnimationPose* paperPose;

    if (pose->paperId != -1) {
        if ((pose->flags & 0xC0) != 0) {
            poseData = *wp->files[pose->fileId].handle->data;
            pMtx = matrix;
            if (!(pose->flags & 8) && rotY != float_0_80419a48) {
                PSMTXRotRad(mtxB, 'y', float_deg2rad_80419a4c * rotY);
                PSMTXConcat(matrix, mtxB, mtxA);
                pMtx = mtxA;
            }
            if (float_1_80419a44 != scale) {
                PSMTXScale(mtxC, scale, scale, scale);
                PSMTXConcat(pMtx, mtxC, mtxA);
                pMtx = mtxA;
            }
            g_modeling_mtx_lv = 0;
            g_modeling_mtx = g_modeling_mtx_stack;
            PSMTXCopy(pMtx, g_modeling_mtx_stack[0]);
            wp->mpCurPose = pose;
            wp->mpCurPoseData = poseData;
            evalProc(-1, poseData->groupCount - 1);
        }

        paperPose = &wp->poses[pose->paperId];
        paperPoseData = *wp->files[paperPose->fileId].handle->data;
        if (!(xluMode == 2 && (*(u32*)paperPoseData->field_0xC4 & 1))) {
            if (!(*(u32*)paperPoseData->field_0xC4 & (1 << xluMode))) {
                return;
            }
        }

        if (paperPose->typeFlags & 2) {
            posNum = paperPoseData->mBufferPosNum;
            nrmNum = paperPoseData->mBufferNrmNum;
            if (posNum + nrmNum <= (s32)wp->mFloatScratchRP) {
                if ((s32)(wp->mFloatScratchWP + posNum + nrmNum) > (s32)wp->mFloatScratchRP) {
                    wp->mFloatScratchWP = 0;
                }
                wp->mbUseFloatScratch = 1;
                wp->vtxArrayPos = &wp->mpFloatScratch[wp->mFloatScratchWP];
                wp->mFloatScratchWP += posNum;
                wp->vtxArrayNrm = &wp->mpFloatScratch[wp->mFloatScratchWP];
                wp->mFloatScratchWP += nrmNum;
            }
            animArg = *(s32*)pose->field_0x94;
            if (animArg != -1) {
                {
                    AnimationPose* pp = &wp->poses[pose->paperId];
                    AnimationPoseData* ppd = *wp->files[pp->fileId].handle->data;
                    top = ppd->mAnimCount;
                    found = animPoseSearchAnimIdx(ppd, (const char*)animArg, animArg, top);
                    if (found != pp->animId) {
                        pp->animId = found;
                        if (pp->paperId != -1 && (pp->flags & 2)) {
                            pp->flags |= 4;
                        }
                        pp->localTime = animTimeGetTime(pp->type);
                        pp->lastFrameZero = -1;
                        pp->lastFrameTime = 0.0f;
                        pp->field_0x80 = 0;
                        pp->loopTime = 0.0f;
                    }
                }
            }
            paperPose->localTime = animTimeGetTime(paperPose->type);
            paperPose->lastFrameZero = -1;
            paperPose->lastFrameTime = 0.0f;
            paperPose->field_0x80 = 0;
            paperPose->loopTime = 0.0f;
            paperPose->localTime = pose->mLocalTimeInit;
            animPoseMain(pose->paperId);
        }

        {
            void* texData0 = *(void**)(pose->field_0x94 + 4);
            void* texData1 = *(void**)(pose->field_0x94 + 8);

            wp->mpTexObjs[0] = texData0 ? (GXTexObj*)(pose->field_0x94 + 0xC) : NULL;
            wp->mpTexObjs[1] = texData1 ? (GXTexObj*)(pose->field_0x94 + 0x2C) : NULL;
            wp->mpCurTexObj = NULL;
            wp->mpTexDatas[0] = texData0;
            wp->mpTexDatas[1] = texData1;
            wp->mpCurTexData = NULL;
            wp->field_0xD8 = 1;
            wp->field_0xDC = 1;
        }

        paperPose->materialFlag = pose->materialFlag;
        paperPose->materialLightFlag = pose->materialLightFlag;
        paperPose->materialColor = pose->materialColor;
        paperPose->materialColor2 = pose->materialColor2;

        if (pose->flags & 2) {
            PSMTXScale(mtxE, *(f32*)(pose->field_0x104 + 8), *(f32*)(pose->field_0x104 + 0xC), float_1_80419a44);
            PSMTXTrans(mtxD, *(f32*)pose->field_0x104, *(f32*)(pose->field_0x104 + 4), float_0_80419a48);
            PSMTXConcat(mtxE, mtxD, mtxE);
            animSetPaperTexMtx(mtxE, NULL, NULL);
            PSMTXScale(mtxE, pose->field_0xF8, pose->field_0xFC, pose->field_0xF8);
            if (wp->inBattle) {
                PSMTXTrans(mtxD, float_0_80419a48, 2.0f * pose->field_0x100, float_0_80419a48);
            } else {
                PSMTXTrans(mtxD, float_0_80419a48, pose->field_0x100, float_0_80419a48);
            }
            PSMTXConcat(mtxD, mtxE, mtxE);
            PSMTXConcat(matrix, mtxE, mtx);
            matrix = mtx;
        } else {
            PSMTXScale(mtxE, *(f32*)(pose->field_0x104 + 8), *(f32*)(pose->field_0x104 + 0xC), float_1_80419a44);
            PSMTXTrans(mtxD, *(f32*)pose->field_0x104, *(f32*)(pose->field_0x104 + 4), float_0_80419a48);
            PSMTXConcat(mtxE, mtxD, mtxE);
            PSMTXTrans(mtxD, float_0p5_80419a64, float_1_80419a44, float_0_80419a48);
            PSMTXConcat(mtxE, mtxD, mtxE);
            PSMTXScale(mtxD, float_1_80419a44 / pose->field_0xF8, float_1_80419a44 / pose->field_0xFC, float_1_80419a44);
            PSMTXConcat(mtxE, mtxD, mtxE);
            PSMTXTrans(mtxD, float_neg0p5_80419a88, float_neg1_80419a8c, float_0_80419a48);
            PSMTXConcat(mtxE, mtxD, mtxE);
            animSetPaperTexMtx(mtxE, NULL, NULL);
        }
    } else {
        poseData = *wp->files[pose->fileId].handle->data;
        if (!(xluMode == 2 && (*(u32*)poseData->field_0xC4 & 1))) {
            if (!(*(u32*)poseData->field_0xC4 & (1 << xluMode))) {
                return;
            }
        }
    }

    if (pose->paperId != -1) {
        _animPoseDrawMtx(paperPose, matrix, xluMode, rotY, scale);
        wp->paperMtxPtr[0] = NULL;
        wp->paperMtxPtr[1] = NULL;
        wp->paperMtxPtr[2] = NULL;
        if (paperPose->typeFlags & 2) {
            wp->mbUseFloatScratch = 0;
        }
    } else {
        _animPoseDrawMtx(pose, matrix, xluMode, rotY, scale);
    }
}
#pragma dont_inline reset

void animSetPaperTexObj(GXTexObj* a1, GXTexObj* a2, GXTexObj* a3, void* a4, void* a5, void* a6, s32 a7, s32 a8) {
    wp->mpTexObjs[0] = a4 ? a1 : NULL;
    wp->mpTexObjs[1] = a5 ? a2 : NULL;
    wp->mpCurTexObj = a6 ? a3 : NULL;
    wp->mpTexDatas[0] = a4;
    wp->mpTexDatas[1] = a5;
    wp->mpCurTexData = a6;
    wp->field_0xD8 = a7;
    wp->field_0xDC = a7;
}

// animPoseRelease
void animPoseRelease(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];

    if ((u32)pose->typeFlags != 0) {
        AnimationPoseFile* file;
        AnimationTextureFile* texture;
        pose->references--;
        if (pose->references < 0) {
            file = &wp->files[pose->fileId];
            file->references--;
            if (file->references <= 0) {
                file->active = 0;
                texture = &wp->textures[file->textureId];
                texture->references--;
                if (texture->references <= 0) {
                    fileFree(texture->handle);
                    texture->active = 0;
                }
                fileFree(file->handle);
            }
            pose->flags = 0;
        }
    } else {
        AnimationPoseFile* file;
        AnimationTextureFile* texture;
        if (pose->flags & 0x40) {
            smartFree(*(smartEntry**)((u8*)pose + 0x15C));
        }
        if (pose->flags & 0x80) {
            smartFree(*(smartEntry**)((u8*)pose + 0x164));
        }
        file = &wp->files[pose->fileId];
        file->references--;
        if (file->references <= 0) {
            file->active = 0;
            texture = &wp->textures[file->textureId];
            texture->references--;
            if (texture->references <= 0) {
                fileFree(texture->handle);
                texture->active = 0;
            }
            fileFree(file->handle);
        }
        pose->flags = 0;
    }
}

void animPaperPoseRelease(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationTextureFile* texture;
    AnimationPoseFile* file;

    pose->references--;
    if (pose->references < 0) {
        file = &wp->files[pose->fileId];
        file->references--;
        if (file->references <= 0) {
            file->active = 0;
            texture = &wp->textures[file->textureId];
            texture->references--;
            if (texture->references <= 0) {
                fileFree(texture->handle);
                texture->active = 0;
            }
            fileFree(file->handle);
        }
        pose->flags = 0;
    }
}

// animPoseAutoRelease
void animPoseAutoRelease(s32 type) {
    AnimationPose* pose;
    s32 i;

    for (i = 0; i < wp->totalPoses; i++) {
        pose = &wp->poses[i];
        if ((pose->flags & 1) && pose->type == type) {
            if ((u32)pose->typeFlags != 0) {
                AnimationPoseFile* file;
                AnimationTextureFile* texture;
                pose->references--;
                if (pose->references < 0) {
                    file = &wp->files[pose->fileId];
                    file->references--;
                    if (file->references <= 0) {
                        file->active = 0;
                        texture = &wp->textures[file->textureId];
                        texture->references--;
                        if (texture->references <= 0) {
                            fileFree(texture->handle);
                            texture->active = 0;
                        }
                        fileFree(file->handle);
                    }
                    pose->flags = 0;
                }
            } else {
                AnimationPoseFile* file;
                AnimationTextureFile* texture;
                if (pose->flags & 0x40) {
                    smartFree(*(smartEntry**)((u8*)pose + 0x15C));
                }
                if (pose->flags & 0x80) {
                    smartFree(*(smartEntry**)((u8*)pose + 0x164));
                }
                file = &wp->files[pose->fileId];
                file->references--;
                if (file->references <= 0) {
                    file->active = 0;
                    texture = &wp->textures[file->textureId];
                    texture->references--;
                    if (texture->references <= 0) {
                        fileFree(texture->handle);
                        texture->active = 0;
                    }
                    fileFree(file->handle);
                }
                pose->flags = 0;
            }
        }
    }
    if (type == 1) {
        wp->inBattle = FALSE;
    }
    animPoseRefresh();
}

void animPaperPoseDisp(CameraId camId, void* param) {
    s32 i;
    AnimationPose* pose;

    i = 0;
    pose = wp->poses;
    while (i < wp->totalPoses) {
        if ((pose->flags & 1) != 0 && pose->paperId != -1) {
            animPaperPoseDispSub(0, pose);
        }
        i++;
        pose++;
    }
}

// AnimData's paper fields (not declared in the header's truncated AnimData).
#define ANIMDATA_F44(d) (*(f32*)((u8*)(d) + 0x44))
#define ANIMDATA_F48(d) (*(f32*)((u8*)(d) + 0x48))
#define ANIMDATA_F50(d) (*(f32*)((u8*)(d) + 0x50))
#define ANIMDATA_F54(d) (*(f32*)((u8*)(d) + 0x54))
// AnimationPose fields the header keeps as opaque padding.
#define POSE_I0x98(p) (*(smartEntry**)((u8*)(p) + 0x98))
#define POSE_I0x9C(p) (*(s32*)((u8*)(p) + 0x9C))
#define POSE_TEXOBJ(p) (*(GXTexObj*)((u8*)(p) + 0xA0))
#define POSE_S0x114(p) (*(s16*)((u8*)(p) + 0x114))
#define POSE_S0x116(p) (*(s16*)((u8*)(p) + 0x116))
#define POSE_S0x118(p) (*(s16*)((u8*)(p) + 0x118))
#define POSE_S0x11A(p) (*(s16*)((u8*)(p) + 0x11A))
#define POSE_F0x104(p) (*(f32*)((u8*)(p) + 0x104))
#define POSE_F0x108(p) (*(f32*)((u8*)(p) + 0x108))
#define POSE_F0x10C(p) (*(f32*)((u8*)(p) + 0x10C))
#define POSE_F0x110(p) (*(f32*)((u8*)(p) + 0x110))

extern GXColor dat_80419a34;
extern GXColor dat_80419a38;

// Logic, offsets, constants, calls and branch layout match; what remains is the callee-saved GPR map.
// The original keeps pose in r30, animData in r29, materialFlag in r31, flags in r28 and
// materialLightFlag in r27, and reuses r27/r31/r28 for x/c/b (and r28/r29 for w/h) in the tail;
// this compiler instead assigns pose r28, flags r27, materialFlag r30, materialLightFlag r31,
// w r29 and h r30. Declaration order (all 23 locals, sampled), assignment order, block-scoped
// x/y/b/c, a local `p = pose` copy, `register`, every -O/opt_*/global_optimizer/scheduling/
// defer_codegen/peephole/optimize_for_size pragma, and GC compiler 2.0/2.5/2.6/2.7 all leave
// `pose` in r28 -- it is a pure graph-colouring tie-break and needs the permuter. The y-flip
// statements must be `x=...; y=...; c=x; if(x<y)c=y;` (c=x after y, not before) and the
// field_0x104 numerator is emitted as the negated `-(left - pose114)` form; the max is written
// with a temp under MUST_MATCH to stop the else branch coalescing c into x.
// NONMATCHING (97.50%): callee-saved GPR map (pose r30 vs r28 etc.) and the tail schedule it drives.
void animPaperPoseDispSub(s32 unused, AnimationPose* pose) {
    AnimData* animData;
    AnimationPoseData* data;
    GXColor color0;
    GXColor color1;
    Mtx mtxTrans;
    Mtx mtxSaved;
    f32 f31;
    f32 f30;
    u16 left;
    u16 top;
    u16 right;
    u16 bottom;
    s32 materialLightFlag;
    s32 flags;
    s32 materialFlag;
    s32 x;
    s32 y;
    s32 y2;
    s32 b;
    s32 c;
    u16 w;
    u16 h;
    f32 scale;

    f31 = float_8_80419a70;

    if ((pose->flags & 1) == 0) {
        return;
    }

    data = animPoseGetBaseData(pose);
    animData = ANIM_POSE_DATA_ANIMS(data)[pose->animId].data;

    f31 *= float_25p74_80419a74 / (ANIMDATA_F54(animData) - ANIMDATA_F48(animData));
    f30 = float_0p75_80419a68 * (f32)DEMOGetRenderModeObj()->efbHeight;

    sysWaitDrawSync();
    GXClearBoundingBox();

    PSMTXTrans(mtxTrans, float_0_80419a48,
               float_0p25_80419a6c * -(f32)DEMOGetRenderModeObj()->efbHeight - ANIMDATA_F48(animData) * f31,
               float_0_80419a48);

    materialFlag = pose->materialFlag;
    materialLightFlag = pose->materialLightFlag;
    flags = pose->flags;
    color0 = pose->materialColor;
    color1 = pose->materialColor2;
    pose->flags &= ~8;
    pose->materialColor = dat_80419a34;
    pose->materialColor2 = dat_80419a38;
    pose->materialLightFlag = 0;

    if (pose->vivianType != 0) {
        PSMTXCopy(pose->matrix, mtxSaved);
        PSMTXCopy(mtxTrans, pose->matrix);
    }

    _animPoseDrawMtx(pose, mtxTrans, 2, float_0_80419a48, f31);

    pose->flags = flags;
    pose->materialFlag = materialFlag;
    pose->materialColor = color0;
    pose->materialColor2 = color1;
    pose->materialLightFlag = materialLightFlag;
    if (pose->vivianType != 0) {
        PSMTXCopy(mtxSaved, pose->matrix);
    }

    sysWaitDrawSync();
    GXReadBoundingBox(&left, &top, &right, &bottom);

    if (left > right) {
        left = 0;
        top = 0;
        right = 0x10;
        bottom = 0x10;
    }

    if ((pose->flags & 2) && (pose->flags & 4) && gp->inBattle) {
        x = (s32)(ANIMDATA_F44(animData) * f31 + (f32)(DEMOGetRenderModeObj()->fbWidth / 2));
        y = (s32)(ANIMDATA_F50(animData) * f31 + (f32)(DEMOGetRenderModeObj()->fbWidth / 2));
        y2 = (s32)(f30 - f31 * (ANIMDATA_F54(animData) - ANIMDATA_F48(animData)));
        b = (s32)f30;
        if (x <= left && y >= right && y2 <= top && b >= bottom) {
            left = x;
            right = y;
            top = y2;
            bottom = b;
        }
    }

    if (left < 2) left = 2;
    if (top < 2) top = 2;
    if (right > DEMOGetRenderModeObj()->fbWidth - 2) right = DEMOGetRenderModeObj()->fbWidth - 2;
    if (bottom > DEMOGetRenderModeObj()->efbHeight - 2) bottom = DEMOGetRenderModeObj()->efbHeight - 2;

    if (pose->flags & 2) {
        if (pose->flags & 4) {
            x = (DEMOGetRenderModeObj()->fbWidth / 2) - left;
            y = right - (DEMOGetRenderModeObj()->fbWidth / 2);
#ifdef MUST_MATCH
            // match-only temp: the natural `c = x; if (x < y) c = y;` coalesces c into x in this branch
            { s32 t = x; if (t < y) t = y; c = t; }
#else
            c = x;
            if (x < y) c = y;
#endif
            x = (s32)(float_0p5_80419a64 * (f32)c);
            b = (s32)(float_0p14881_80419a78 * (f32)(bottom - top));
            POSE_S0x114(pose) = (DEMOGetRenderModeObj()->fbWidth / 2) - c - x;
            POSE_S0x116(pose) = (DEMOGetRenderModeObj()->fbWidth / 2) + x + c;
            scale = float_8_80419a70 / f31;
            POSE_S0x118(pose) = top - b;
            POSE_S0x11A(pose) = bottom + b;
            pose->field_0xF8 = scale * ((f32)(POSE_S0x116(pose) - POSE_S0x114(pose)) / float_150_80419a7c);
            pose->field_0xFC = scale * ((f32)(POSE_S0x11A(pose) - POSE_S0x118(pose)) / float_215_80419a80);
            pose->field_0x100 = ANIMDATA_F48(animData) + (f30 - (f32)bottom) / f31;
            pose->flags &= ~4;
        } else {
            x = (DEMOGetRenderModeObj()->fbWidth / 2) - left;
            y = right - (DEMOGetRenderModeObj()->fbWidth / 2);
#ifdef MUST_MATCH
            // match-only temp: the natural `c = x; if (x < y) c = y;` coalesces c into x in this branch
            { s32 t = x; if (t < y) t = y; c = t; }
#else
            c = x;
            if (x < y) c = y;
#endif
            x = (s32)(float_0p5_80419a64 * (f32)c);
            b = (s32)(float_0p14881_80419a78 * (f32)(bottom - top));
            POSE_S0x114(pose) = (DEMOGetRenderModeObj()->fbWidth / 2) - c - x;
            POSE_S0x116(pose) = (DEMOGetRenderModeObj()->fbWidth / 2) + x + c;
            scale = float_8_80419a70 / f31;
            POSE_S0x118(pose) = top - b;
            POSE_S0x11A(pose) = bottom + b;
            pose->field_0xF8 = scale * ((f32)(POSE_S0x116(pose) - POSE_S0x114(pose)) / float_150_80419a7c);
            pose->field_0xFC = scale * ((f32)(POSE_S0x11A(pose) - POSE_S0x118(pose)) / float_215_80419a80);
            pose->field_0x100 = ANIMDATA_F48(animData) + (f30 - (f32)bottom) / f31;
        }
    }

    left -= 2;
    left -= left & 1;
    top -= 2;
    top -= top & 1;
    right += 2;
    right += right & 1;
    bottom += 2;
    bottom += bottom & 1;
    w = right - left;
    h = bottom - top;
#ifdef MUST_MATCH
    // match-only negation: the original emits -(left - pose114), not pose114 - left
    POSE_F0x104(pose) = -((f32)left - (f32)POSE_S0x114(pose)) / (f32)(POSE_S0x116(pose) - POSE_S0x114(pose));
#else
    POSE_F0x104(pose) = ((f32)POSE_S0x114(pose) - (f32)left) / (f32)(POSE_S0x116(pose) - POSE_S0x114(pose));
#endif
    POSE_F0x108(pose) = ((f32)POSE_S0x118(pose) - (f32)top) / (f32)(POSE_S0x11A(pose) - POSE_S0x118(pose));
    POSE_F0x10C(pose) = (f32)(POSE_S0x116(pose) - POSE_S0x114(pose)) / (f32)w;
    POSE_F0x110(pose) = (f32)(POSE_S0x11A(pose) - POSE_S0x118(pose)) / (f32)h;
    GXSetTexCopySrc(left, top, w, h);
    GXSetTexCopyDst(w, h, 5, 0);
    POSE_I0x98(pose) = smartAlloc(GXGetTexBufferSize(w, h, 5, 0, 0), 3);
    GXCopyTex(*(void**)POSE_I0x98(pose), 1);
    GXPixModeSync();
    GXInitTexObj(&POSE_TEXOBJ(pose), *(void**)POSE_I0x98(pose), w, h, 5, 0, 0, 0);
    POSE_I0x9C(pose) = 0;
}

// animPoseDisp_MakeExtTexture

extern const f32 float_16p667_80419a60;
extern GXColor dat_80419a3c;
extern GXColor dat_80419a40;
// All logic/constants/offsets match (including every compare direction and signedness). What remains is
// callee-saved register colouring: `data` now lands in r18 as in the target, but the search loop's `top`
// and the compiler's 0x40-stride induction pointer have swapped registers (target top r26 / pointer r17;
// here top r17 / pointer r26; the induction pointer wants data-1 = r17, which top took first). The target
// also sets i=0 before loading gp and emits `offset = i` as `mr r28, r31`, where this build folds the
// known-zero i to `li r28, 0`. Declaring `BOOL inBattle;` before `s32 offset;`, and `data` before `top`,
// is what fixes the inBattle r26 / offset r28 split and puts data in r18. The `p` shadow (r28 in the
// target) reproduces the target's re-materialised pose in the animId-fixup block; the rendering tail
// matches byte-for-byte. A declaration-order sweep of the eight locals and every structural variant
// tried leave the ceiling at 98.35%; the top/pointer swap is a pure graph-colouring tie-break.
// NONMATCHING (98.35%): callee-saved GPR colouring in the two search loops only.
void animPoseDisp_MakeExtTexture(const char* name, f32 f1, void* field_0x4, void* texObj, void* base,
                                 f32 f2, s32* out0, s32* out1, s32* out2, s32* out3) {
    s32 i;
    BOOL inBattle;
    s32 offset;
    s32 animId;
    s32 found;
    AnimationPoseData* data;
    s32 top;
    AnimationPose* pose;
    AnimationPose savedPose;

    f32 f30;
    f32 f29;
    Mtx mtx;
    u16 left;
    u16 topEdge;
    u16 right;
    u16 bottom;
    u16 w;
    u16 h;
    s32 flags;
    GXColor color0;
    GXColor color1;
    s32 materialFlag;
    OSTime time;
    s32 cx;
    s32 cy;

    i = 0;
    offset = 0;
    inBattle = (gp->inBattle != 0);
    while (i < wp->totalPoses) {
        pose = (AnimationPose*)((u8*)wp->poses + offset);
        if ((pose->flags & 1) && pose->type == inBattle &&
            !strcmp(animPoseGetBaseData(pose)->fileName, name)) {
            break;
        }
        offset += 0x170;
        i++;
    }
    if (i == wp->totalPoses) {
        i = -1;
    }
    if (i < 0) {
        return;
    }
    pose = &wp->poses[i];
    savedPose = *pose;
    pose->paperId = -1;
    {
        // Shadow of the same address: the target materialises the pose again here (r28) and keeps
        // the outer copy (r30) live, i.e. the original did not share one `pose` variable. Using the
        // shadow reproduces the target's non-CSEd pair (see the animPoseDrawMtx note).
        AnimationPose* p = &wp->poses[i];
        data = animPoseGetBaseData(p);
        animId = (s32)field_0x4;
        top = data->mAnimCount;
        found = animPoseSearchAnimIdx(data, (const char*)field_0x4, animId, top);
        if (found != p->animId) {
            p->animId = found;
            if (p->paperId != -1 && (p->flags & 2)) {
                p->flags |= 4;
            }
            p->localTime = animTimeGetTime(p->type);
            p->lastFrameZero = -1;
            p->lastFrameTime = 0.0f;
            p->field_0x80 = 0;
            p->loopTime = 0.0f;
        }
    }
    pose->localTime = animTimeGetTime(pose->type);
    pose->lastFrameZero = -1;
    pose->lastFrameTime = 0.0f;
    pose->field_0x80 = 0;
    pose->loopTime = 0.0f;
    {
        // Shadow again: the target's `- time` reset runs on a re-materialised &wp->poses[i] (r29,
        // reusing the post-loop i*0x170) and its paperId fixup reads that copy. The shadow has to be
        // declared before `time =` so the compiler schedules the fmuls above it and the (OSTime)
        // conversion below it, as the target does; moving it after costs ~1.4%.
        AnimationPose* p2 = &wp->poses[i];
        time = (OSTime)(float_16p667_80419a60 * f1);
        p2->localTime = animTimeGetTime(p2->type) - time;
        p2->mLocalTimeInit = p2->localTime;
        if (p2->paperId != -1) {
            AnimationPose* pera = &wp->poses[p2->paperId];

            if (!(pera->typeFlags & 2)) {
                pera->localTime = p2->localTime;
            }
        }
    }
    animPoseMain(i);
    f30 = float_0p5_80419a64 * (f32)DEMOGetRenderModeObj()->fbWidth;
    f29 = float_0p75_80419a68 * (f32)DEMOGetRenderModeObj()->efbHeight;
    sysWaitDrawSync();
    GXClearBoundingBox();
    PSMTXTrans(mtx, float_0_80419a48, float_0p25_80419a6c * -(f32)DEMOGetRenderModeObj()->efbHeight, float_0_80419a48);
    flags = pose->flags;
    color0 = pose->materialColor;
    color1 = pose->materialColor2;
    materialFlag = pose->materialFlag;
    pose->flags &= ~8;
    pose->materialFlag = 0;
    pose->materialColor = dat_80419a3c;
    pose->materialColor2 = dat_80419a40;
    _animPoseDrawMtx(pose, mtx, 1, float_0_80419a48, f2);
    _animPoseDrawMtx(pose, mtx, 2, float_0_80419a48, f2);
    _animPoseDrawMtx(pose, mtx, 3, float_0_80419a48, f2);
    pose->materialColor = color0;
    pose->materialColor2 = color1;
    pose->flags = flags;
    pose->materialFlag = materialFlag;
    sysWaitDrawSync();
    GXReadBoundingBox(&left, &topEdge, &right, &bottom);
    if (left > right) {
        left = 0;
        topEdge = 0;
        right = 0x10;
        bottom = 0x10;
    }
    if (left < 2) {
        left = 2;
    }
    if (topEdge < 2) {
        topEdge = 2;
    }
    if (right > DEMOGetRenderModeObj()->fbWidth - 2) {
        right = DEMOGetRenderModeObj()->fbWidth - 2;
    }
    if (topEdge > DEMOGetRenderModeObj()->efbHeight - 2) {
        topEdge = DEMOGetRenderModeObj()->efbHeight - 2;
    }
    left -= left & 1;
    topEdge -= topEdge & 1;
    right += right & 1;
    bottom += bottom & 1;
    w = right - left;
    h = bottom - topEdge;
    GXSetTexCopySrc(left, topEdge, w, h);
    GXSetTexCopyDst(w, h, 5, 0);
    anim_ext_tex_size += GXGetTexBufferSize(w, h, 5, 0, 0);
    *(void**)base = smartAlloc(GXGetTexBufferSize(w, h, 5, 0, 0), 1);
    GXCopyTex(**(void***)base, 1);
    GXPixModeSync();
    GXInitTexObj(texObj, **(void***)base, w, h, 5, 0, 0, 0);
    cx = (s32)f30;
    cy = (s32)f29;
    *out0 = left - cx;
    *out1 = topEdge - cy;
    *out2 = right - cx;
    *out3 = bottom - cy;
    *pose = savedPose;
}

void animSetPaperTexMtx(Mtx mtx1, Mtx mtx2, Mtx mtx3) {
    if (mtx1 == NULL) {
        wp->paperMtxPtr[0] = NULL;
    } else {
        PSMTXCopy(mtx1, wp->paperMtx[0]);
        wp->paperMtxPtr[0] = &wp->paperMtx[0];
    }
    if (mtx2 == NULL) {
        wp->paperMtxPtr[1] = NULL;
    } else {
        PSMTXCopy(mtx2, wp->paperMtx[1]);
        wp->paperMtxPtr[1] = &wp->paperMtx[1];
    }
    if (mtx3 == NULL) {
        wp->paperMtxPtr[2] = NULL;
    } else {
        PSMTXCopy(mtx3, wp->paperMtx[2]);
        wp->paperMtxPtr[2] = &wp->paperMtx[2];
    }
}

BOOL animGroupBaseAsync(const char* name, s32 group, void* callback) {
    s32 fileId;
    s32 other;
    void** data;
    u8* entry;
    u8* result;
    s32 offset;
    s32 size;

    switch (group) {
        case 0:
            fileSetCurrentArchiveType(1);
            break;
        case 1:
            fileSetCurrentArchiveType(2);
            break;
        case 2:
            fileSetCurrentArchiveType(0);
            break;
    }
    fileId = fileAsyncf(5, callback, str_80419A54, str_80419A5C, name);
    data = wp->mp_ag2tg->data;
    size = (s32)data[1];
    entry = (u8*)data[0];
    for (offset = 0; offset < size; offset += 0x20) {
        if (!strcmp((char*)entry, name)) {
            result = entry + 0x20;
            goto found;
        }
        entry += 0x20;
    }
    result = (u8*)&zero_80419a50;
found:
    other = fileAsyncf(4, NULL, "%s/%s-", str_80419A5C, result);

    if (fileId == -1) {
        return TRUE;
    }
    if ((u32)fileId == 0) {
        return FALSE;
    }
    if (other == -1) {
        return TRUE;
    }
    return other != 0;
}

AnimationPose* animPoseGetAnimPosePtr(s32 poseId) {
    return &wp->poses[poseId];
}

AnimData* animPoseGetAnimDataPtr(s32 poseId) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);
    return ANIM_POSE_DATA_ANIMS(animPoseGetAnimBaseDataPtr(poseId))[pose->animId].data;
}

AnimationPoseData* animPoseGetAnimBaseDataPtr(s32 poseId) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);
    return *wp->files[pose->fileId].handle->data;
}

AnimTableEntry* animPoseGetCurrentAnim(s32 poseId) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return &ANIM_POSE_DATA_ANIMS(data)[animPoseGetAnimPosePtr(poseId)->animId];
}

BOOL animPoseTestXLU(s32 poseId) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return (*(u32*)data->field_0xC4 >> 3) & 1;
}

// evalProc
// NONMATCHING (99.75%): control flow and offsets match; the first block's data/groups registers are
// swapped (target data r4/groups r6, here data r6) and the vec3/PSMTX blocks schedule the pointer
// adds and mtx loads in a different order. No structural difference.
void evalProc(s32 parentGroupId, s32 groupId) {
    AnimPoseGroup* group;
    AnimPoseGroup* parent;
    AnimationPoseData* data;
    Vec pos;
    f32* nodes;
    f32 x, y, z;

    if (groupId == -1) {
        return;
    }
    data = wp->mpCurPoseData;
    group = &ANIM_POSE_DATA_GROUPS(data)[groupId];

    if (((s8*)wp->mpCurPose->mpGroupVisibility)[group->visibilityId] != 0) {
        f32* node;

        nodes = (f32*)wp->mpCurPose->mpNodes;
        node = &nodes[group->nodeId];
        if (group->useSSC) {
            if (parentGroupId == -1) {
                x = y = z = 1.0f;
            } else {
                parent = &ANIM_POSE_DATA_GROUPS(data)[parentGroupId];
                if (!parent->useSSC) {
                    x = y = z = 1.0f;
                } else {
                    x = nodes[parent->nodeId + 3];
                    y = nodes[parent->nodeId + 4];
                    z = nodes[parent->nodeId + 5];
                }
            }
            pushGXModelMtx_JointNode__(node, x, y, z);
        } else {
            pushGXModelMtx_TransformNode__(node);
        }

        if (wp->mpCurPose->flags & 0x40) {
            pos = vec3_802c2dd4[0];
            pos.x = (*g_modeling_mtx)[0][3];
            pos.y = (*g_modeling_mtx)[1][3];
            pos.z = (*g_modeling_mtx)[2][3];
            ((Vec*)(*(smartEntry**)((u8*)wp->mpCurPose + 0x15C))->address)[groupId] = pos;
        }
        if (wp->mpCurPose->flags & 0x80) {
            PSMTXCopy(*g_modeling_mtx, ((Mtx*)(*(smartEntry**)((u8*)wp->mpCurPose + 0x164))->address)[groupId]);
        }
        if (!(wp->mpCurPose->flags & 0x20) || groupId != *wp->mpCurPose->vivianGroupNo) {
            evalProc(groupId, group->childGroupId);
        }
        g_modeling_mtx_lv--;
        g_modeling_mtx = &g_modeling_mtx_stack[g_modeling_mtx_lv];
    }
    evalProc(groupId, group->brotherGroupId);
}

void animPoseWorldPositionEvalOn(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* data = *wp->files[pose->fileId].handle->data;
    u8 type;

    if (pose->flags & 0x40) {
        return;
    }
    pose->flags |= 0x40;
    switch (pose->type) {
        case 0:
            type = 1;
            break;
        case 1:
            type = 2;
            break;
        case 2:
            type = 0;
            break;
    }
    *(s32*)((u8*)pose + 0x160) = data->groupCount;
    *(void**)((u8*)pose + 0x15C) = smartAlloc(data->groupCount * sizeof(Vec), type);
}

void L_animPoseWorldPositionEvalOff(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    if (pose->flags & 0x40) {
        pose->flags &= ~0x40;
        smartFree(*(smartEntry**)((u8*)pose + 0x15C));
        *(void**)((u8*)pose + 0x15C) = NULL;
    }
}

void animPoseWorldMatrixEvalOn(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* data = *wp->files[pose->fileId].handle->data;
    u8 type;

    if (pose->flags & 0x80) {
        return;
    }
    pose->flags |= 0x80;
    switch (pose->type) {
        case 0:
            type = 1;
            break;
        case 1:
            type = 2;
            break;
        case 2:
            type = 0;
            break;
    }
    *(s32*)((u8*)pose + 0x168) = data->groupCount;
    *(void**)((u8*)pose + 0x164) = smartAlloc(data->groupCount * sizeof(Mtx), type);
}

void animPoseVivianMain(s32 poseId, Vec* pos) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->flags |= 0x20;
    pose->matrix[0][3] = pos->x;
    pose->matrix[1][3] = pos->y;
    pose->matrix[2][3] = pos->z;
}

s32 animPoseGetVivianType(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    return pose->vivianType;
}

void animPoseSetDispCallBack(s32 poseId, void* dispCallback, void* dispUserData) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->dispCallback = dispCallback;
    pose->dispUserDataCallback = dispUserData;
}

// animPoseDrawShape
// Structure matches and the 14 saved wp fields now get the target's exact colours (r21/r20/r19 for
// mpCurPose/AnimData/PoseData, r30..r22 for the tex/data/mtx fields, r18/r17 for the two tail words).
// MWCC assigns those registers in declaration order, so the saved fields are declared in that colour
// order while the loads/restores stay in struct-field order.
#pragma opt_common_subs off // needed: MWCC CSEs two `wp` loads into a register the target keeps separate
// Hoisting `mBufferPosNum`/`mBufferNrmNum` into locals keeps them in registers across the two
// `mFloatScratchWP` updates, which is what the target does (it removed the two duplicated loads).
// NONMATCHING (98.28%): the remaining differences are register colouring in the poseId division and
// the base-data block (`wp` r6/data r4 in the target vs r4/r6 here).
void animPoseDrawShape(s32 poseId, s32 shapeId) {
    AnimationPose* pose = &wp->poses[poseId];
    GXTexObj* prevTexObj0;
    GXTexObj* prevTexObj1;
    GXTexObj* prevCurTexObj;
    void* prevTexData0;
    void* prevTexData1;
    void* prevCurTexData;
    Mtx* prevPaperMtx0;
    Mtx* prevPaperMtx1;
    Mtx* prevPaperMtx2;
    AnimationPose* prevPose;
    AnimData* prevAnimData;
    AnimationPoseData* prevPoseData;
    u32 prevD8;
    u32 prevDC;

    prevPose = wp->mpCurPose;
    prevAnimData = wp->mpCurAnimData;
    prevPoseData = wp->mpCurPoseData;
    prevTexObj0 = wp->mpTexObjs[0];
    prevTexObj1 = wp->mpTexObjs[1];
    prevCurTexObj = wp->mpCurTexObj;
    prevTexData0 = wp->mpTexDatas[0];
    prevTexData1 = wp->mpTexDatas[1];
    prevCurTexData = wp->mpCurTexData;
    prevPaperMtx0 = wp->paperMtxPtr[0];
    prevPaperMtx1 = wp->paperMtxPtr[1];
    prevPaperMtx2 = wp->paperMtxPtr[2];
    prevD8 = wp->field_0xD8;
    prevDC = wp->field_0xDC;

    wp->mpCurPose = pose;
    wp->mpCurAnimData = ANIM_POSE_DATA_ANIMS(animPoseGetBaseData(pose))[pose->animId].data;
    wp->mpCurPoseData = animPoseGetBaseData(pose);

    wp->mpTexObjs[0] = NULL;
    wp->mpTexObjs[1] = NULL;
    wp->mpCurTexObj = NULL;
    wp->mpTexDatas[0] = NULL;
    wp->mpTexDatas[1] = NULL;
    wp->mpCurTexData = NULL;
    wp->paperMtxPtr[0] = NULL;
    wp->paperMtxPtr[1] = NULL;
    wp->paperMtxPtr[2] = NULL;
    wp->field_0xD8 = 0;
    wp->field_0xDC = 0;

    if (pose->lastFrameZero == -1) {
#ifdef MUST_MATCH
        // The original loses poseId to the pointer mulli and recovers it with an unsigned
        // division of the byte offset by sizeof(AnimationPose) (magic 0xb21642c9 >> 8).
        s32 shapePoseId = (s32)((u32)((u8*)pose - (u8*)wp->poses) / sizeof(AnimationPose));
#else
        s32 shapePoseId = poseId;
#endif
        if (pose->typeFlags & 2) {
            AnimationPoseData* data = animPoseGetBaseData(pose);
            s32 posNum = data->mBufferPosNum;
            s32 nrmNum = data->mBufferNrmNum;
            s32 total = posNum + nrmNum;

            if (total <= (s32)wp->mFloatScratchRP) {
                if ((s32)(wp->mFloatScratchWP + total) > (s32)wp->mFloatScratchRP) {
                    wp->mFloatScratchWP = 0;
                }
                wp->mbUseFloatScratch = 1;
                wp->vtxArrayPos = &wp->mpFloatScratch[wp->mFloatScratchWP];
                wp->mFloatScratchWP += posNum;
                wp->vtxArrayNrm = &wp->mpFloatScratch[wp->mFloatScratchWP];
                wp->mFloatScratchWP += nrmNum;
            }
            animPoseMain(shapePoseId);
            wp->mbUseFloatScratch = 0;
        } else {
            animPoseMain(shapePoseId);
        }
    }
    renderProc(shapeId, *(s32*)((u8*)pose + 0x28));

    wp->mpCurPose = prevPose;
    wp->mpCurAnimData = prevAnimData;
    wp->mpCurPoseData = prevPoseData;
    wp->mpTexObjs[0] = prevTexObj0;
    wp->mpTexObjs[1] = prevTexObj1;
    wp->mpCurTexObj = prevCurTexObj;
    wp->mpTexDatas[0] = prevTexData0;
    wp->mpTexDatas[1] = prevTexData1;
    wp->mpCurTexData = prevCurTexData;
    wp->paperMtxPtr[0] = prevPaperMtx0;
    wp->paperMtxPtr[1] = prevPaperMtx1;
    wp->paperMtxPtr[2] = prevPaperMtx2;
    wp->field_0xD8 = prevD8;
    wp->field_0xDC = prevDC;
}
#pragma opt_common_subs reset

s32 animPoseGetShapeIdx(s32 poseId, const char* name) {
    AnimPoseShape* shape;
    AnimationPoseData* data;
    s32 i;

    data = animPoseGetAnimBaseDataPtr(poseId);
    shape = data->shapes;
    for (i = 0; i < data->shapeCount; i++, shape++) {
        if (!strcmp(shape->name, name)) {
            return i;
        }
    }
    return -1;
}

s32 animPoseGetGroupIdx(s32 poseId, const char* name) {
    AnimPoseGroup* group;
    AnimationPoseData* data;
    s32 i;

    data = animPoseGetAnimBaseDataPtr(poseId);
    group = ANIM_POSE_DATA_GROUPS(data);
    for (i = 0; i < data->groupCount; i++, group++) {
        if (!strcmp(group->name, name)) {
            return i;
        }
    }
    return -1;
}

const char* animPoseGetGroupName(s32 poseId, s32 group) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return ANIM_POSE_DATA_GROUPS(data)[group].name;
}

// Named .sdata2 GXColor objects, defined after the code so the compiler keeps the loads.
#pragma force_active on // dat_80419a28 has no reference and must still be emitted
const s32 dat_80419a28 = 0x0000002f;
__declspec(section ".sdata2") GXColor dat_80419a2c = { 0xff, 0xff, 0xff, 0xff };
__declspec(section ".sdata2") GXColor dat_80419a30 = { 0xff, 0xff, 0xff, 0xff };
__declspec(section ".sdata2") GXColor dat_80419a34 = { 0xff, 0xff, 0xff, 0xff };
__declspec(section ".sdata2") GXColor dat_80419a38 = { 0xff, 0xff, 0xff, 0xff };
__declspec(section ".sdata2") GXColor dat_80419a3c = { 0xff, 0xff, 0xff, 0xff };
__declspec(section ".sdata2") GXColor dat_80419a40 = { 0xff, 0xff, 0xff, 0xff };
#pragma force_active reset

const f32 float_1_80419a44 = 1.0f;
#pragma explicit_zero_data on // emit float_0_80419a48 as 4 zero bytes instead of .sbss
const f32 float_0_80419a48 = 0.0f;
#pragma explicit_zero_data reset
const f32 float_deg2rad_80419a4c = 0.017453292f;
#pragma explicit_zero_data on // emit zero_80419a50 as 4 zero bytes instead of .sbss
__declspec(section ".sdata2") u32 zero_80419a50 = 0;
#pragma explicit_zero_data reset
__declspec(section ".sdata2") const char str_80419A54[] = "%s/%s";
__declspec(section ".sdata2") const char str_80419A5C[] = "a";
const f32 float_16p667_80419a60 = 16.666666f;
const f32 float_0p5_80419a64 = 0.5f;
const f32 float_0p75_80419a68 = 0.75f;
const f32 float_0p25_80419a6c = 0.25f;
const f32 float_8_80419a70 = 8.0f;
const f32 float_25p74_80419a74 = 25.74f;
const f32 float_0p14881_80419a78 = 0.14881f;
const f32 float_150_80419a7c = 150.f;
const f32 float_215_80419a80 = 215.0f;
const f32 float_2_80419a84 = 2.0f;
const f32 float_neg0p5_80419a88 = -0.5f;
const f32 float_neg1_80419a8c = -1.0f;
const f32 float_neg90_80419a90 = -90.f;
const f32 float_rad2deg_80419a94 = 57.29579f;
const f32 float_360_80419a98 = 360.f;
const f32 float_neg360_80419a9c = -360.f;
const f32 float_90_80419aa0 = 90.f;
const f32 float_270_80419aa4 = 270.f;
const f32 float_neg270_80419aa8 = -270.f;
const f32 float_180_80419aac = 180.f;
const f32 float_neg180_80419ab0 = -180.f;
const f32 float_0p1_80419ab4 = 0.1f;
const f32 float_neg2_80419ab8 = -2.0f;
const f32 float_neg3_80419abc = -3.0f;
const f32 float_0p04_80419ac0 = 0.04f;
const f32 float_60_80419ac4 = 60.f;
const f32 float_1000_80419ac8 = 1000.f;
const f32 float_0p0625_80419acc = 0.0625f;
const f32 float_3_80419ad0 = 3.0f;
