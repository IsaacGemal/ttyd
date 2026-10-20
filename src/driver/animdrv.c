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

#define TESTHEAP_SIZE (u32)(1.5 * 1024 * 1024)
#define PI 3.14159265358979323846264338327950288419716939937510f

extern GlobalWork* gp;

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
f32 tanfTbl[0xB2];
static AnimationWork work;

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

//.sbss (MWCC lays these out last-declared-first)
Mtx* g_modeling_mtx;
s32 g_modeling_mtx_lv;
Mtx g_modeling_mtx_stack[32];

// .sdata2 constants this unit references by name in retail


// local declarations
static const char* animGetAg2tgName(const char* name);

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
    file = fileAllocf(FILE_TYPE_RAW, "a/vivian.bin");
    if (file) {
        memcpy_as4(vivihimoData[1], *file->data, sizeof(_vivihimoData));
        memcpy_as4(vivihimoTailGroupNo, (void*)((u32)*file->data + sizeof(_vivihimoData)), sizeof(vivihimoTailGroupNo));
        fileFree(file);
    }
    fileSetCurrentArchiveType(0);
    wp->mp_ag2tg = fileAllocf(FILE_TYPE_RAW, "a/ag2tg.bin");
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

static inline s32 animPoseFindFreePose(AnimationPose** out) {
    s32 i;

    for (i = 0; i < wp->totalPoses; i++) {
        AnimationPose* p = &wp->poses[i];
        if (!p->flags) {
            *out = p;
            return i;
        }
    }
    *out = NULL;
    return -1;
}

// Finds the first inactive file slot, writing its index through id. The early return
// inside the loop is what gives the found-exit the target's `bne next; b found` layout.
static inline s32 animPoseFindFreeFile(AnimationPoseFile** out) {
    s32 i;

    for (i = 0; i < wp->totalFiles; i++) {
        AnimationPoseFile* f = &wp->files[i];
        if (!f->active) {
            *out = f;
            return i;
        }
    }
    *out = NULL;
    return -1;
}

// Finds the first inactive texture slot, writing its index through id. Same early-return
// shape as the file search above.
static inline s32 animPoseFindFreeTexture(AnimationTextureFile** out) {
    s32 i;

    for (i = 0; i < wp->totalTextures; i++) {
        AnimationTextureFile* t = &wp->textures[i];
        if ((u32)t->active == 0) {
            *out = t;
            return i;
        }
    }
    *out = NULL;
    return -1;
}

// Returns the slot of the texture archive `name`, loading it into a free slot if no loaded
// texture has that name.
static inline s32 animPoseLoadTexture(const char* name) {
    AnimationTextureFile* texture;
    s32 texId;
    s32 count = wp->totalTextures;

    for (texId = 0; texId < count; texId++) {
        AnimationTextureFile* t = &wp->textures[texId];
        if ((u32)t->active != 0) {
            if (t->handle) {
                if (!strcmp(t->handle->filename, name)) {
                    t->references++;
                    return texId;
                }
            }
        }
    }
    texId = animPoseFindFreeTexture(&texture);
    texture->active = TRUE;
    texture->references++;
    texture->handle = fileAlloc(name, FILE_TYPE_TPL);
    return texId;
}

// Returns the slot of the animation archive `animName` (loading it and its textures if needed),
// or -2 when it cannot be loaded.
static inline s32 animPoseLoadFile(const char* animName) {
    AnimationPoseFile* file;
    AnimationPoseData* data;
    s32 fileId;
    s32 count = wp->totalFiles;
    char textureName[256];

    for (fileId = 0; fileId < count; fileId++) {
        AnimationPoseFile* f = &wp->files[fileId];
        if (f->active) {
            data = *f->handle->data;
            if (!strcmp(data->fileName, animName)) {
                f->references++;
                return fileId;
            }
        }
    }
    fileId = animPoseFindFreeFile(&file);
    file->active = TRUE;
    file->references++;
    file->handle = fileAllocf(FILE_TYPE_ANIMPOSE, "%s/%s", "a", animName);
    if (!file->handle) {
        file->active = FALSE;
        file->references--;
        return -2;
    }
    data = *file->handle->data;
    if (!data) {
        fileFree(file->handle);
        file->active = FALSE;
        file->references--;
        return -2;
    }
    sprintf(textureName, "%s/%s-", "a", data->textureName);
    file->textureId = animPoseLoadTexture(textureName);
    return fileId;
}

s32 animPoseEntry(const char* animName, s32 type) {
    AnimationPose* pose;
    s32 poseId;

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

    poseId = animPoseFindFreePose(&pose);
    pose->flags |= 1;
    pose->typeFlags = 0;
    pose->references = 0;

    pose->fileId = animPoseLoadFile(animName);
    if (pose->fileId == -2) {
        pose->flags = 0;
        return -2;
    }
    pose->animId = 0;
    pose->localTime = animTimeGetTime(type); // inline
    pose->lastFrameZero = -1;
    pose->lastFrameTime = 0.0f;
    pose->evtValue = 0;
    pose->loopTime = 0.0f;
    pose->drawRotY = 0.0f;
    pose->rotationY = 0.0f;
    pose->targetRotY = 0.0f;
    pose->type = type;
    pose->paperId = -1;
    animPose_AllocBuffer(pose);
    pose->gxCallback = NULL;
    pose->disableDraw = 0;
    pose->materialFlag = 0;
    pose->materialLightFlag = 0;
    pose->materialColor = dat_80419a2c;
    pose->materialColor2 = dat_80419a30;
    pose->paperScaleY = 1.0f;
    pose->paperScaleX = 1.0f;
    pose->paperOffsetY = 0.0f;
    pose->localTimeRate = 1.0f;
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
    fileId = fileAsyncf(FILE_TYPE_ANIMPOSE, NULL, "%s/%s", "a", animName);
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
    result = (u8*)"";
found:
    other = fileAsyncf(FILE_TYPE_TPL, NULL, "%s/%s-", "a", result);

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
    dst->drawRotY = src->drawRotY;
    dst->rotationY = src->rotationY;
    dst->targetRotY = src->targetRotY;
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
    pose->lastFrameTime = 0.0f;
    pose->evtValue = 0;
    pose->loopTime = 0.0f;
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

void animPoseSetPaperAnimGroup(s32 poseId, const char* animName, BOOL someflag) {
    s32 poseOffset = poseId * 0x170;
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);

    if (animName == NULL) {
        pose->paperId = -1;
        ((AnimationPose*)((u8*)wp->poses + poseOffset))->materialLightFlag &= ~1;
        return;
    }
    pose->paperId = animPaperPoseSearchGroup(pose->type, animName);
    if (pose->paperId < 0) {
        pose->paperId = animPaperPoseSearchGroup(2, animName);
    }
    pose->paperAnim = (const char*)-1;
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
            if (!strcmp(data->mpAnims[i].name, animName)) {
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
            if (!strcmp(data->mpAnims[i].name, animName)) {
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

    pose->paperAnim = (const char*)animId;
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

// Deriving the pose pointer from poseOffset keeps the index multiply in one named local; computing it
// from poseId separately lets the front end make poseOffset its own temp that outranks the pose web.
void animPoseSetEffect(s32 poseId, const char* animName, BOOL someflag) {
    AnimationPose* pose;
    s32 poseOffset;

    poseOffset = poseId * 0x170;
    pose = (AnimationPose*)((u8*)wp->poses + poseOffset);

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
    if (animName == NULL) {
        pose->paperId = -1;
        ((AnimationPose*)((u8*)wp->poses + poseOffset))->materialLightFlag &= ~1;
        return;
    }
    pose->paperId = animEffectPoseEntry(animName, pose->type);
    pose->paperAnim = (const char*)-1;
    pose->mLocalTimeInit = animTimeGetTime(pose->type);
    if (someflag) {
        pose->flags |= 2;
        pose->flags |= 4;
    } else {
        pose->flags &= ~2;
    }
    ((AnimationPose*)((u8*)wp->poses + poseOffset))->materialLightFlag |= 1;
}

// Searches data->mpAnims for animName when i is negative (i is the string's address used
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
            if (!strcmp(data->mpAnims[i].name, animName)) {
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
            if (!strcmp(data->mpAnims[i].name, animName)) {
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
    pose->lastFrameTime = 0.0f;
    pose->evtValue = 0;
    pose->loopTime = 0.0f;
}

void animPoseSetGXFunc(s32 poseId, void (*callback)(s32 wXluStage), BOOL disableDraw) {
    AnimationPose* pose = &wp->poses[poseId];
    pose->gxCallback = callback;
    pose->disableDraw = disableDraw;
}

f32 animPoseGetRadius(s32 poseId) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return (f32)data->radius * 2.0f;
}

f32 animPoseGetHeight(s32 poseId) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return (f32)data->height * 2.0f;
}

f32 animPoseGetLoopTimes(s32 id) {
    AnimationPose* pose = &wp->poses[id];
    return pose->loopTime;
}

BOOL animPoseGetPeraEnd(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];

    return !(pose->flags & 8) ? TRUE : fabs(pose->targetRotY - pose->rotationY) < 10.0;
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
// The frame and delta tables of an animation record are walked by byte count: a frame record
// is 0x2C bytes, and the position, normal, texture, visibility and node deltas are indexed by
// the frame's byte offsets.

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

// The same runs given as [src, end): the reset path walks its frames with these.
static inline void animPoseAddVecDeltaRange(Vec* dst, u8* src, u8* end, f32 scale) {
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

static inline void animPoseAddNodeDeltaRange(f32* dst, u8* src, u8* end, f32 scale) {
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

// Applies one frame's five delta channels to the pose buffers (the catch-up path; the reset path
// spells the position, normal and node runs as [src, end) ranges, which numbers each run's end
// before its start and gives retail's r5/r7 there). It is a macro rather than a helper function because the
// combined expansion exceeds the default auto-inline size, and raising #pragma inline_max_size
// for it leaks into the earlier functions in this file (testAlloc/animPoseRefresh stop matching).
#define ANIM_POSE_ADD_FRAME(pose, anim, frame, scale)                                               \
    do {                                                                                            \
        animPoseAddVecDelta((pose)->mpBufferVtxPos,                                                 \
                            (anim)->posData + ANIM_FRAME_POS_OFF(frame) * 4,                        \
                            ANIM_FRAME_POS_COUNT(frame) * 4, scale);                                \
        animPoseAddVecDelta((pose)->mpBufferVtxNrm,                                                 \
                            (anim)->nrmData + ANIM_FRAME_NRM_OFF(frame) * 4,                        \
                            ANIM_FRAME_NRM_COUNT(frame) * 4, scale);                                \
        animPoseAddVisDelta((anim)->visData + ANIM_FRAME_VIS_OFF(frame) * 2,                        \
                            (pose)->mpBufferGroupVisibility,                                        \
                            ANIM_FRAME_VIS_COUNT(frame) * 2);                                       \
        animPoseAddNodeDelta((f32*)(pose)->mpBufferNode,                                            \
                             (anim)->nodeData + ANIM_FRAME_NODE_OFF(frame) * 4,                     \
                             ANIM_FRAME_NODE_COUNT(frame) * 4, scale);                              \
        animPoseAddTexDelta((pose)->mpBufferTexAnimEntries,                                         \
                            (anim)->texData + ANIM_FRAME_TEX_OFF(frame) * 0xC,                      \
                            ANIM_FRAME_TEX_COUNT(frame) * 0xC);                                     \
    } while (0)

// The unit's own .sdata2 constants referenced by name from the assembly below.

static inline f32 animAngleWrap(f32 a) {
    while (a < -360.0f) {
        a += 360.0f;
    }
    while (a >= 360.0f) {
        a -= 360.0f;
    }
    return a;
}

static inline f32 animGetCameraAngle(void) {
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    f32 raw = (f32)atan2(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z));
    f32 angle = animAngleWrap(57.29579f * raw);

    if (angle < 0.0f) {
        angle += 360.0f;
    }
    return angle;
}

// Cubic Hermite interpolation at t of a value p with end tangents m0/m1 (start value 0).
static inline f32 animHermite(f32 t, f32 p, f32 m0, f32 m1) {
    f32 t2 = t * t;
    f32 t3 = t * t2;
    f32 h0 = (-2.0f) * t3 + 3.0f * t2;
    f32 h1 = t3 - 2.0f * t2 + t;
    f32 h2 = t3 - t2;

    return h0 * p + h1 * m0 + h2 * m1;
}

// The four inlined delta loops make each update path large; retail inlines them in both.
#pragma inline_max_total_size(60000)
// Reconstruction notes. The whole recursive pose evaluator is reconstructed from the assembly.
// Source shapes that reproduce retail's operand order and register split: the
// two `0.0f == x` compares keep the constant first in `fcmpu`; the clamped value
// accumulates back into `start` (retail's clamp lives in start's f3) instead of a separate
// `clamped` variable; `end` is declared before `start` and loaded first, giving retail's f1/f3
// split; the reset loop's position/normal/node runs go through the [src, end) range helpers, each
// start pointer in its own block (r5 start / r7 end there; the size helpers' local `end` gives the
// catch-up loops' r7/r5), and the cubic `-2*t3 + 3*t2` term order matches retail's fmadds.
// The node blend is the animHermite helper: its t2/t3 locals number the squared and cubed terms
// the way retail colours them (f8/f7), and its named basis terms give retail's schedule.
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

    anim = data->mpAnims[pose->animId].data;

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

    canLoop = anim->clip[0];
    now = animTimeGetTime(pose->type);
    pose->animTime = (f32)(u64)(now - pose->localTime) * 60.0f / 1000.0f;
    pose->animTime *= pose->localTimeRate;

    time = pose->animTime;
    end = ((f32*)anim->clip)[2];
    start = ((f32*)anim->clip)[1];
    duration = end - start;

    if (0.0f == duration) {
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
    pose->clipTime = start;

    {
    f32 frameTime = pose->clipTime;
    {
    u8* fp = anim->frames;
    for (i = 0; i < anim->frameCount; i++) {
        if (*(f32*)fp > frameTime) {
            break;
        }
        fp += ANIM_FRAME_SIZE;
    }
    }
    pose->keyFrame1 = i - 1;
    pose->keyFrame2 = pose->keyFrame1 + 1;

    {
    f32 frameTime2 = pose->clipTime;
    u8* fp2 = anim->frames2;
    for (i = 0; i < anim->frame2Count; fp2 += 8, i++) {
        if (*(f32*)fp2 > frameTime2) {
            break;
        }
    }
    }
    pose->evtFrame1 = i - 1;
    pose->evtFrame2 = pose->evtFrame1 + 1;
    }
    if (!canLoop) {
        if (pose->keyFrame2 >= anim->frameCount) {
            pose->keyFrame2 = pose->keyFrame1;
        }
        if (pose->evtFrame2 >= anim->frame2Count) {
            pose->evtFrame2 = pose->evtFrame1;
        }
    }

    {
        s32* p = (s32*)anim->frames2;
        pose->evtValue = p[pose->evtFrame1 * 2 + 1];
    }

    {
        f32 num = pose->clipTime - *(f32*)(anim->frames + pose->keyFrame1 * ANIM_FRAME_SIZE);
        f32 den;
        if (canLoop) {
            if (pose->keyFrame2 >= anim->frameCount) {
                pose->keyFrame2 = 0;
                den = ((f32*)anim->clip)[2] - *(f32*)(anim->frames + pose->keyFrame1 * ANIM_FRAME_SIZE);
            } else {
                den = *(f32*)(anim->frames + pose->keyFrame2 * ANIM_FRAME_SIZE)
                    - *(f32*)(anim->frames + pose->keyFrame1 * ANIM_FRAME_SIZE);
            }
        } else {
            den = *(f32*)(anim->frames + pose->keyFrame2 * ANIM_FRAME_SIZE)
                - *(f32*)(anim->frames + pose->keyFrame1 * ANIM_FRAME_SIZE);
        }
        if (0.0f == den) {
            pose->keyAlpha = 0.0f;
        } else {
            pose->keyAlpha = num / den;
        }
    }

    frame1 = pose->keyFrame1;
    frame2 = pose->keyFrame2;
    alpha = pose->keyAlpha;
    posNum = data->mBufferPosNum;
    nrmNum = data->mBufferNrmNum;
    visNum = data->mBufferVisibilityNum;
    nodeNum = data->mBufferNodeNum;
    texNum = data->mBufferTexAnimNum;

    if (pose->lastFrameZero != frame1) {
        if (pose->lastFrameZero == -1 || pose->lastFrameZero > frame1) {
            u8* f = anim->frames;
            u8* fend = anim->frames + frame1 * ANIM_FRAME_SIZE + ANIM_FRAME_SIZE;
            if (posNum) {
                memcpy_as4(pose->mpBufferVtxPos, data->bufferVtxPos, posNum * 0xC);
            }
            if (nrmNum) {
                memcpy_as4(pose->mpBufferVtxNrm, data->nrmVtxBuffer, nrmNum * 0xC);
            }
            if (visNum) {
                memcpy(pose->mpBufferGroupVisibility, data->mBufferVisibility, visNum);
            }
            if (nodeNum) {
                memcpy_as4(pose->mpBufferNode, data->mBufferNode, nodeNum * 4);
            }
            if (texNum) {
                memcpy_as4(pose->mpBufferTexAnimEntries, data->mBufferTexAnim, texNum * 0x18);
            }
            if (pose->lastFrameZero == -1) {
                pose->lastFrameZero = 0;
            }
            for (; f < fend; f += ANIM_FRAME_SIZE) {
                {
                    u8* p = anim->posData + ANIM_FRAME_POS_OFF(f) * 4;
                    animPoseAddVecDeltaRange(pose->mpBufferVtxPos, p, p + ANIM_FRAME_POS_COUNT(f) * 4, 0.0625f);
                }
                {
                    u8* p = anim->nrmData + ANIM_FRAME_NRM_OFF(f) * 4;
                    animPoseAddVecDeltaRange(pose->mpBufferVtxNrm, p, p + ANIM_FRAME_NRM_COUNT(f) * 4, 0.0625f);
                }
                animPoseAddVisDelta(anim->visData + ANIM_FRAME_VIS_OFF(f) * 2, pose->mpBufferGroupVisibility,
                                    ANIM_FRAME_VIS_COUNT(f) * 2);
                {
                    u8* p = anim->nodeData + ANIM_FRAME_NODE_OFF(f) * 4;
                    animPoseAddNodeDeltaRange((f32*)pose->mpBufferNode, p, p + ANIM_FRAME_NODE_COUNT(f) * 4, 0.0625f);
                }
                animPoseAddTexDelta(pose->mpBufferTexAnimEntries, anim->texData + ANIM_FRAME_TEX_OFF(f) * 0xC,
                                    ANIM_FRAME_TEX_COUNT(f) * 0xC);
            }
        } else if (pose->lastFrameZero < frame1) {
            u8* f = anim->frames + (pose->lastFrameZero + 1) * ANIM_FRAME_SIZE;
            u8* fend = anim->frames + (frame1 + 1) * ANIM_FRAME_SIZE;
            for (; f < fend; f += ANIM_FRAME_SIZE) {
                ANIM_POSE_ADD_FRAME(pose, anim, f, 0.0625f);
            }
        }
    } else {
        if (pose->lastFrameTime > alpha) {
            pose->lastFrameTime = 0.0f;
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
        u8* key2 = anim->frames + frame2 * ANIM_FRAME_SIZE;
        f32 blend = 0.0625f * alpha;

        {
            u8* posSrc = anim->posData + ANIM_FRAME_POS_OFF(key2) * 4;
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
            u8* nrmSrc = anim->nrmData + ANIM_FRAME_NRM_OFF(key2) * 4;
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
            u8* src = anim->nodeData + ANIM_FRAME_NODE_OFF(key2) * 4;
            u8* srcEnd = src + ANIM_FRAME_NODE_COUNT(key2) * 4;
            void* nodeSrc = pose->mpBufferNode;
            f32* dst = (f32*)pose->mpNodes;
            f32 timedelta = *(f32*)(anim->frames + frame2 * ANIM_FRAME_SIZE)
                          - *(f32*)(anim->frames + frame1 * ANIM_FRAME_SIZE);

            if (nodeNum) {
                memcpy_as4(dst, nodeSrc, nodeNum * 4);
            }
            {
                while (src < srcEnd) {
                    s32 t1 = (s8)src[3];
                    dst += src[0];
                    if (t1 < 0x59 && t1 > -0x59) {
                        f32 step = 0.0625f * __OSs8tof32((s8*)src + 1);
                        f32 tan0 = tanfTbl[(s8)src[2] + 0x59];
                        f32 tan1 = tanfTbl[t1 + 0x59];

                        *dst += animHermite(alpha, step, timedelta * tan0, timedelta * tan1);
                    }
                    src += 4;
                }
            }
        }
    }

    pose->lastFrameZero = frame1;
    pose->lastFrameTime = alpha;

    if (pose->flags & 8) {
        f32 diff = animAngleWrap(animGetCameraAngle() - pose->rotationY);

        if ((diff > 90.0f && diff <= 270.0f)
            || (diff < (-90.0f) && diff >= (-270.0f))) {
            pose->facingSign = (-1.0f);
        } else {
            pose->facingSign = 1.0f;
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

// A texture entry's wrap flags override the descriptor's modes when non-negative: bit 2/3 mirror S/T,
// otherwise bit 0/1 selects repeat over clamp.
static inline void animSetMaterial_WrapMode(TEXDescriptor* desc, s32 wrapFlags, GXTexWrapMode* outS, GXTexWrapMode* outT) {
    GXTexWrapMode wrapS = desc->textureHeader->wrapS;
    GXTexWrapMode wrapT = desc->textureHeader->wrapT;

    if (wrapFlags >= 0) {
        wrapS = (wrapFlags & 4) ? GX_MIRROR : ((wrapFlags & 1) ? GX_REPEAT : GX_CLAMP);
        wrapT = (wrapFlags & 8) ? GX_MIRROR : ((wrapFlags & 2) ? GX_REPEAT : GX_CLAMP);
    }
    *outS = wrapS;
    *outT = wrapT;
}

#pragma opt_strength_reduction off // preserves the matrix loop's counter/flags/matrix-ID code generation
void animSetMaterial_Texture(s32 texCount, s32* texIdRemap, AnimPoseTexEntry* pTexEntries, AnimTexMtx* animEntries,
                             s32 texBindsCapacity, AnimTexBind* texBinds, AnimationTextureFile* texFile) {
    s32 i;
    AnimationPose* pose;
    AnimPoseTexEntry* texEntry;
    AnimTexBind* bind;
    s32 wrapFlags;
    s32 palId;
    f32(*mtx)[4];
    u32 flags;
    AnimationPoseData* poseData;
    AnimTexMtx* anim;
    s32 mipmap;
    TEXDescriptor* desc;
    s32 texId;
    s32 bindId;
    s32 frameExt;

    Mtx mRot, mTrans, mScale, mTmp;

    GXTexObj texObj;
    GXTlutObj tlutObj;

    GXTexWrapMode wrapT, wrapS;

    for (i = 0; i < texCount; i++) {
        int slot = texCount - i - 1;
        texId = texIdRemap[slot];

        texEntry = &pTexEntries[texId];
        bindId = texEntry->bindId;
        frameExt = animEntries[texId].mTexBindIdx;
        bindId += frameExt;

        if (bindId < 0 || bindId >= texBindsCapacity) {
            pose = wp->mpCurPose;
            poseData = *wp->files[pose->fileId].handle->data;
            bindId = 0;

            printf("Error: dispSetTevMode: \203x\201[\203X\203\202\203f\203\213%s \203A\203j\203\201 %s "
                   "\223\340\202\305 \203t\203\214\201[\203\200\203G\203N\203X\203e\203\223\203V\203\207\203\223\202\314\220"
                   "\335\222\350\202\252\215s\202\355\202\352\202\304\202\242\202\334\202\267\202\252\201A\212Y\223\226\202\267"
                   "\202\351\203e\203N\203X\203`\203\203\202\252\202\240\202\350\202\334\202\271\202\361 "
                   "\nframeExt\224\324\215\206 %d? \202\252\226\263\202\242\202\346\202\244\202\305\202\267\201B\203\202\203f"
                   "\203\213\202\334\202\275\202\315\203A\203j\203\201\202\311\217C\220\263\202\252\225K\227v\202\305\202\267\201B"
                   "textureIdx=%d\n",
                   poseData->fileName, poseData->mpAnims[pose->animId].name, frameExt, texEntry->bindId);
        }
        wrapFlags = texEntry->wrapFlags;
        bind = &texBinds[bindId];
        palId = bind->palId;
        if (bind->texType == 0) {
            desc = TEXGet((TEXPalette*)*texFile->handle->data, palId);

            if (desc->CLUTHeader) {
                desc = TEXGet((TEXPalette*)*texFile->handle->data, palId);
                animSetMaterial_WrapMode(desc, wrapFlags, &wrapS, &wrapT);
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
#ifdef MUST_MATCH // Mask in the destination to avoid an extra temporary/copy into GXInitTexObj's r8.
                    if (wrapFlags & 4) {
                        wrapS = GX_MIRROR;
                    } else {
                        wrapS = wrapFlags;
                        wrapS &= 1;
                    }
#else
                    wrapS = (wrapFlags & 4) ? GX_MIRROR : ((wrapFlags & 1) ? GX_REPEAT : GX_CLAMP);
#endif
                    wrapT = (wrapFlags & 8) ? GX_MIRROR : ((wrapFlags & 2) ? GX_REPEAT : GX_CLAMP);
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

    // Load each stage's animated texture matrix: rotate about the centre, translate, then scale.
    for (i = 0; i < texCount; i++) {
        f32 scaleS, scaleT, transS, transT, rotate;

#ifdef MUST_MATCH // `1u` keeps the original's `subf; subi 1; slwi` instead of folding it to `subf; slwi; subi 4`
        texId = texIdRemap[texCount - i - 1u];
#else
        texId = texIdRemap[texCount - i - 1];
#endif
        anim = &animEntries[texId];
        flags = 0;

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
                GXSetTexCoordGen2(i, GX_TG_MTX2x4, GX_TG_TEX0 + i, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
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
        GXSetTexCoordGen2(i, GX_TG_MTX2x4, GX_TG_TEX0 + i, GX_TEXMTX0 + (u32)(i * 3), GX_FALSE, GX_PTIDENTITY);
        GXLoadTexMtxImm(mtx, GX_TEXMTX0 + (u32)(i * 3), GX_MTX2x4);
    }
}
#pragma opt_strength_reduction reset

// Inlined into materialProc and renderProc (the name is the one in its error string). It reads the current
// pose data through wp, as its callers do; the texture-argument group is declared (texObj, texData, mtx).
static inline BOOL animSetMaterial_ChangeTexture(AnimPoseShapeDraw* draw, AnimationPose* pose, GXTexObj* texObj,
                                                 void** texData, Mtx mtx, s32 texType) {
    AnimTexBind* bind;
    s32 bindCount;
    AnimTexMtx* texAnim;
    AnimPoseTexEntry* texEntry;
    s32 texCount;
    s32 dataId, bindId, frameExt, idx, texBase;
    GXTexObj* smartObj;
    int i;

    bind = wp->mpCurPoseData->texBinds;
    bindCount = wp->mpCurPoseData->texBindCount;
    texAnim = pose->mpTexAnimEntries;
    texEntry = wp->mpCurPoseData->texData;
    texCount = draw->texCount;

    for (i = 0; i < texCount; i++) {
        idx = texCount - i - 1;
        dataId = draw->texDataIds[idx];
        frameExt = texAnim[dataId].mTexBindIdx;
        texBase = texEntry[dataId].bindId;
        bindId = texBase + frameExt;
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
void materialProc(s32 shapeId, s32 keyFrame1) {
    CameraEntry* camera;
    AnimationPoseData* data;
    AnimPoseShape* shapes;
    int i;
    s32 drawCount;
    AnimPoseShapeDraw *draws, *draw;
    s32 dispMode, drawId;
    s32 texCount;
    GXTexObj* texObj;
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
                    texChange1 = animSetMaterial_ChangeTexture(draw, wp->mpCurPose, wp->mpTexObjs[0], wp->mpTexDatas[0],
                                                               *wp->paperMtxPtr[0], 1);
                    check1 = texChange1;
                    if (!texChange1) {
                        check1 = animSetMaterial_ChangeTexture(draw, wp->mpCurPose, wp->mpTexObjs[1], wp->mpTexDatas[1],
                                                               *wp->paperMtxPtr[1], 2);
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
                            animSetMaterial_ChangeTexture(draw, wp->mpCurPose, wp->mpTexObjs[0], wp->mpTexDatas[0],
                                                          *wp->paperMtxPtr[0], 1);
                        }
                    }
                }
            }
        }
    }
}

// I'm so sorry, whoever gets tasked with matching this function
// note: I detect at least 8 inlined functions here.
// Reconstruction notes: the material setup and the vertex-format setup each read draw->texCount into their own
// local (matTexCount / texCount); one shared local becomes a renamed second-live-range temp and rotates
// texCount/i/draw/j out of r27..r30. The declaration order of the locals sets the rest of the callee-saved
// colouring. The ChangeTexture body is the inlined animSetMaterial_ChangeTexture (its group declared
// (texObj, texData, mtx) so MWCC's right-to-left argument evaluation loads mtx first, and an explicit
// `idx = texCount - i - 1` keeps retail's `subi ..., 1` instead of folding it into the array base). The
// index pointers and the u16 `count` are declared per draw-call loop; the `k1` loop counts down (retail's
// `ble` unroll guard); the multi-texture vertex loop is written inline with an `int slot` index (retail's
// `lbz -0x1(base)` fold) and indexes each texture's u32 entries by the vertex number `k`, which MWCC
// strength-reduces to retail's byte offset.
void renderProc(s32 shapeId, s32 keyFrame1) {
    int i;
    AnimPoseShapeDraw* draw;
    s32 texCount;
    s8* texIds;
    Mtx* mtx;
    BOOL check1;
    int k;
    s32 drawId;
    AnimPoseShape *shapes, *shape;
    Vec *posVtxPtr, *nrmVtxPtr;
    GXTexObj* texObj;
    BOOL texChange1, check2;
    AnimationPoseData* data;
    s32 drawCallCount;
    AnimPoseShapeDraw* draws;
    s32 posFirstVtx;
    int k1;
    AnimationPose* pose;
    s32 dispMode, posFirstId, nrmFirstId;
    GXTexObj* currTexObj;
    s32 nrmFirstVtx;
    GXColor* clr0VtxPtr;
    s32 clr0FirstVtx;
    BOOL texChange2;
    s32 clr0FirstId;
    s32* firstId;
    AnimDrawCallEntry* drawCall;
    int j;
    s32 firstDrawId;
    f32* buffer;
    CameraEntry* camera;
    s32 matTexCount;

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
                matTexCount = draw->texCount;
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
                    mapSetMaterialTev(matTexCount, draw->drawMode, wp->mpCurPose->materialFlag, *g_modeling_mtx);
                }
                if ((u32)wp->mpCurPose->typeFlags != 0) {
                    texChange1 = animSetMaterial_ChangeTexture(draw, wp->mpCurPose, wp->mpTexObjs[0],
                                                               wp->mpTexDatas[0], *wp->paperMtxPtr[0], 1);
                    check1 = texChange1;
                    if (!texChange1) {
                        texChange2 = animSetMaterial_ChangeTexture(draw, wp->mpCurPose, wp->mpTexObjs[1],
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
                            animSetMaterial_ChangeTexture(draw, pose, texObj, wp->mpTexDatas[0],
                                                          *wp->paperMtxPtr[0], 1);
                        }
                    }
                }

                texIds = (s8*)draw->texArrayIds;
                firstId = draw->firstIdTex;
                drawCall = &wp->mpCurPoseData->drawCalls[draw->firstDrawCall];
                texCount = draw->texCount;
                drawCallCount = draw->drawCallCount;
                clr0VtxPtr = (GXColor*)((u8*)wp->mpCurPoseData->clr0VtxBuffer + clr0VtxSize);
                posFirstId = draw->firstIdPos;
                nrmFirstId = draw->firstIdNrm;
                clr0FirstId = draw->firstIdClr0;

                if ((s32)wp->mbUseFloatScratch != 0) {
                    posVtxPtr = (Vec*)((u8*)wp->vtxArrayPos + posVtxSize);
                    nrmVtxPtr = (Vec*)((u8*)wp->vtxArrayNrm + nrmVtxSize);
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
                        u16 count;
                        u16 *posIndexPtr, *nrmIndexPtr, *clr0IndexPtr, *texIndexPtr;

                        data = wp->mpCurPoseData; // doubly inlined function here
                        count = drawCall->count;
                        // indices are stored as u32, has to do math to load the lower 2 bytes
                        posIndexPtr = (u16*)((u8*)data->posIndex + (drawCall->first + posFirstId) * 4 + 2);
                        nrmIndexPtr = (u16*)((u8*)data->nrmIndex + (drawCall->first + nrmFirstId) * 4 + 2);
                        clr0IndexPtr = (u16*)((u8*)data->clr0Index + (drawCall->first + clr0FirstId) * 4 + 2);
                        texIndexPtr = (u16*)((u8*)data->texIndex[texIds[0]] + (drawCall->first + firstId[texIds[0]]) * 4 + 2);
                        GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, count);
                        for (k1 = count; k1 > 0; k1--) { // unrolled, warning
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
                        u16 count;
                        u16 *posIndexPtr, *nrmIndexPtr, *clr0IndexPtr;
                        u8* texEntryPtr;
                        s32 index;
                        int l;
                        int slot;

                        firstDrawId = drawCall->first;
                        data = wp->mpCurPoseData; // doubly inlined function here
                        count = drawCall->count;
                        // indices are stored as u32, has to do math to load the lower 2 bytes
                        posIndexPtr = (u16*)((u8*)data->posIndex + (firstDrawId + posFirstId) * 4 + 2);
                        nrmIndexPtr = (u16*)((u8*)data->nrmIndex + (firstDrawId + nrmFirstId) * 4 + 2);
                        clr0IndexPtr = (u16*)((u8*)data->clr0Index + (firstDrawId + clr0FirstId) * 4 + 2);
                        GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, count);

                        for (k = 0; k < count; k++, posIndexPtr += 2, nrmIndexPtr += 2, clr0IndexPtr += 2) {
                            GXPosition1x16(*posIndexPtr);
                            GXNormal1x16(*nrmIndexPtr);
                            GXColor1x16(*clr0IndexPtr);
                            for (l = 0; l < texCount; l++) {
                                slot = texCount - l - 1;
                                index = texIds[slot];
                                // The header types texIndex as u16*, but retail loads a full word (lwzx) and
                                // stores its low half.
                                texEntryPtr = (u8*)wp->mpCurPoseData->texIndex[index];
                                GXTexCoord1x16(((u32*)texEntryPtr + k)[firstDrawId + firstId[index]]);
                            }
                        }
                    }
                }
            }
        }
    }
}

// dispProc
void dispProc(s32 parentGroupId, s32 groupId, s32 keyFrame1, s32 keyFrame2, f32 f1) {
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
    groups = data->groups;
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
                x = y = z = 1.0f;
            } else {
                AnimPoseGroup* parent = &groups[parentGroupId];
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
            Vec pos = vec3_802c2dc8[0];
            Vec* buffer = (Vec*)wp->mpCurPose->groupWorldPos->address;
            pos.x = (*g_modeling_mtx)[0][3];
            pos.y = (*g_modeling_mtx)[1][3];
            pos.z = (*g_modeling_mtx)[2][3];
            buffer[groupId] = pos;
        }
        if (wp->mpCurPose->flags & 0x80) {
            Mtx* dest = (Mtx*)wp->mpCurPose->groupWorldMtx->address;
            PSMTXCopy(*g_modeling_mtx, dest[groupId]);
        }
        if ((wp->mpCurPose->flags & 0x20) && groupId == *wp->mpCurPose->vivianGroupNo) {
            Mtx* mtx = g_modeling_mtx;
            f32 (*matrix)[4] = wp->mpCurPose->matrix;
            f32* params = (f32*)((u8*)vivihimoData[wp->mpCurPose->vivianType] + wp->mpCurPose->animId * 8);
            f32 v1 = 0.0f;
            f32 v2 = v1;
            int i, j;

            switch (wp->mpCurPose->vivianType) {
                case 1:
                    v1 = (-2.0f);
                    v2 = 0.5f;
                    break;
                case 2:
                    v1 = (-3.0f);
                    v2 = 0.5f;
                    break;
                case 3:
                    v1 = (-1.0f);
                    v2 = 0.5f;
                    break;
            }
            materialProc(group->shapeId, keyFrame1);
            vivihimo(*mtx, matrix, params[0], params[1], v1, v2);
            wp->mpCurPose->flags &= ~0x20;
            for (i = 0; i < 3; i++) {
                for (j = 0; j < 3; j++) {
                    matrix[i][j] += 0.04f * ((*mtx)[i][j] - matrix[i][j]);
                }
            }
        } else {
            if (wp->mpCurPose->dispCallback != NULL) {
                ((void (*)(void*, s32, Mtx))wp->mpCurPose->dispCallback)(wp->mpCurPose->dispUserDataCallback, groupId,
                                                                          *g_modeling_mtx);
            }
            renderProc(group->shapeId, keyFrame1);
            dispProc(groupId, group->childGroupId, keyFrame1, keyFrame2, f1);
        }
        g_modeling_mtx_lv--;
        g_modeling_mtx = &g_modeling_mtx_stack[g_modeling_mtx_lv];
    }
    dispProc(groupId, group->brotherGroupId, keyFrame1, keyFrame2, f1);
}

void animPoseDraw(s32 poseId, f32 x, f32 y, f32 z, f32 rotY, f32 scale, s32 xluMode) {
    Mtx matrix;

    PSMTXTrans(matrix, x, y, z);
    animPoseDrawMtx(poseId, matrix, xluMode, rotY, scale);
}



// _animPoseDrawMtx
// Brings a pose's vertex buffers up to date before drawing: paper poses evaluate into the shared
// float scratch buffer when it has room.
static inline void animPoseMainScratch(AnimationPose* pose) {
    if (pose->lastFrameZero == -1) {
        s32 poseId = (s32)((u32)((u8*)pose - (u8*)wp->poses) / sizeof(AnimationPose));

        if (pose->typeFlags & 2) {
            s32 posNum = animPoseGetBaseData(pose)->mBufferPosNum;
            s32 nrmNum = animPoseGetBaseData(pose)->mBufferNrmNum;
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
            animPoseMain(poseId);
            wp->mbUseFloatScratch = 0;
        } else {
            animPoseMain(poseId);
        }
    }
}

void _animPoseDrawMtx(AnimationPose* pose, Mtx matrix, s32 xluMode, f32 rotY, f32 scale) {
    AnimationPoseData* poseData;
    AnimData* animData;
    Mtx dstMtx;
    Mtx rotMtx;
    Mtx rotMtx2;
    Mtx scaleMtx;
    GXColor color0;
    GXColor color1;
    s32 keyFrame1;
    s32 keyFrame2;
    f32 keyAlpha;
    s32 stage;
    s32 useScratch;

    poseData = animPoseGetBaseData(pose);
    animData = poseData->mpAnims[pose->animId].data;
    keyFrame1 = pose->keyFrame1;
    keyFrame2 = pose->keyFrame2;
    keyAlpha = pose->keyAlpha;
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

    if (!(xluMode == 2 && (poseData->xluFlags & 1)) &&
        !(poseData->xluFlags & (1 << xluMode))) {
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
        f32 diff;

        rotY += (-90.0f);
        pose->drawRotY = rotY;
        diff = animAngleWrap(animGetCameraAngle() - pose->drawRotY);
        if ((diff > 90.0f && diff <= 270.0f) ||
            (diff < (-90.0f) && diff >= (-270.0f))) {
            pose->targetRotY = animAngleWrap(180.0f + animGetCameraAngle());
        } else {
            pose->targetRotY = animGetCameraAngle();
        }
        while (pose->targetRotY - pose->rotationY > 180.0f) {
            pose->rotationY += 360.0f;
        }
        while (pose->targetRotY - pose->rotationY < (-180.0f)) {
            pose->rotationY -= 360.0f;
        }
        if (pose->flags & 0x100) {
            pose->rotationY = pose->targetRotY;
        } else {
            pose->rotationY += 0.1f * (pose->targetRotY - pose->rotationY);
        }
        rotY = pose->rotationY;
        if (0.0f != rotY) {
            PSMTXRotRad(rotMtx, 'y', 0.017453292f * rotY);
            PSMTXConcat(matrix, rotMtx, dstMtx);
            matrix = dstMtx;
        }
        if (pose->facingSign == -1.0f) {
            PSMTXScale(rotMtx, 1.0f, 1.0f, -1.0f);
            PSMTXConcat(matrix, rotMtx, dstMtx);
            matrix = dstMtx;
        }
    } else {
        if (rotY) {
            PSMTXRotRad(rotMtx2, 'y', 0.017453292f * rotY);
            PSMTXConcat(matrix, rotMtx2, dstMtx);
            matrix = dstMtx;
        }
    }

    if (1.0f != scale) {
        PSMTXScale(scaleMtx, scale, scale, scale);
        PSMTXConcat(matrix, scaleMtx, dstMtx);
        matrix = dstMtx;
    }

    animPoseMainScratch(pose);

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
    dispProc(-1, poseData->groupCount - 1, keyFrame1, keyFrame2, keyAlpha);
}

// The search inlined into animPoseDrawMtx and animPoseDisp_MakeExtTexture (with the rest of an
// animPoseSetAnim). It returns from inside the loop and shares one `goto zero` exit, and copies the
// id into i only after the negative test. animPoseSetAnim's own animPoseSearchAnim copies it first,
// which is the colouring those two functions have.
static inline s32 animPoseSearchAnimIdx(AnimationPose* pose, const char* name) {
    AnimationPoseData* data = animPoseGetBaseData(pose);
    s32 i;
    s32 top = data->mAnimCount;

    if ((s32)name < 0) {
        for (i = 0; i < top; i++) {
            if (!strcmp(data->mpAnims[i].name, name)) {
                return i;
            }
        }
        goto zero;
    }
    i = (s32)name;
    if (i >= top) {
        goto zero;
    }
    return i;
zero:
    return 0;
}

// Takes a paper pose's vertex buffers from the shared float scratch when it has room.
static inline void animPoseAllocFloatScratch(AnimationPoseData* data) {
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
}

// animPoseEvalMtx's body for a pose already in hand: animPoseDrawMtx's paper-pose evaluation pass.
// As a helper its pose data and matrix pointer number after the caller's locals, which gives the
// target's colouring of them (r26 and r29).
static inline void animPoseEvalMtxPose(AnimationPose* pose, Mtx matrix, f32 rotY, f32 scale) {
    AnimationPoseData* poseData = *wp->files[pose->fileId].handle->data;
    Mtx mtxA;
    Mtx mtxB;
    Mtx mtxC;

    if (!(pose->flags & 8) && rotY) {
        PSMTXRotRad(mtxB, 'y', 0.017453292f * rotY);
        PSMTXConcat(matrix, mtxB, mtxA);
        matrix = mtxA;
    }
    if (1.0f != scale) {
        PSMTXScale(mtxC, scale, scale, scale);
        PSMTXConcat(matrix, mtxC, mtxA);
        matrix = mtxA;
    }
    g_modeling_mtx_lv = 0;
    g_modeling_mtx = g_modeling_mtx_stack;
    PSMTXCopy(matrix, g_modeling_mtx_stack[0]);
    wp->mpCurPose = pose;
    wp->mpCurPoseData = poseData;
    evalProc(-1, poseData->groupCount - 1);
}
// dont_inline keeps animPoseDrawMtx a real bl from animPoseDraw (retail emits one).
#pragma dont_inline on
// The float-scratch allocation as a helper gives that block's volatile temps (wp r3, paperPoseData r5).
// The paper animation name is read from paperAnim for the test and again for the search: the
// CSE'd load's copy into the search's name parameter is coalesced, and that extra permanent
// neighbour keeps pp in the graph past xluMode (pp r29, xluMode r28). The inner `pp` shadow
// reproduces the target's non-CSEd re-materialisation of the paper pose inside that block.
void animPoseDrawMtx(s32 poseId, Mtx matrix, s32 xluMode, f32 rotY, f32 scale) {
    static Mtx mtx;
    AnimationPose* pose = &wp->poses[poseId];
    Mtx mtxE;
    Mtx mtxD;
    s32 found;
    AnimationPoseData* paperPoseData;
    AnimationPoseData* poseData;
    AnimationPose* paperPose;

    if (pose->paperId != -1) {
        if ((pose->flags & 0xC0) != 0) {
            animPoseEvalMtxPose(pose, matrix, rotY, scale);
        }

        paperPose = &wp->poses[pose->paperId];
        paperPoseData = *wp->files[paperPose->fileId].handle->data;
        if (!(xluMode == 2 && (paperPoseData->xluFlags & 1))) {
            if (!(paperPoseData->xluFlags & (1 << xluMode))) {
                return;
            }
        }

        if (paperPose->typeFlags & 2) {
            animPoseAllocFloatScratch(paperPoseData);
            if ((s32)pose->paperAnim != -1) {
                {
                    AnimationPose* pp = &wp->poses[pose->paperId];
                    found = animPoseSearchAnimIdx(pp, pose->paperAnim);
                    if (found != pp->animId) {
                        pp->animId = found;
                        if (pp->paperId != -1 && (pp->flags & 2)) {
                            pp->flags |= 4;
                        }
                        pp->localTime = animTimeGetTime(pp->type);
                        pp->lastFrameZero = -1;
                        pp->lastFrameTime = 0.0f;
                        pp->evtValue = 0;
                        pp->loopTime = 0.0f;
                    }
                }
            }
            paperPose->localTime = animTimeGetTime(paperPose->type);
            paperPose->lastFrameZero = -1;
            paperPose->lastFrameTime = 0.0f;
            paperPose->evtValue = 0;
            paperPose->loopTime = 0.0f;
            paperPose->localTime = pose->mLocalTimeInit;
            animPoseMain(pose->paperId);
        }

        {
            void* texData0 = pose->paperTexData[0];
            void* texData1 = pose->paperTexData[1];

            wp->mpTexObjs[0] = texData0 ? &pose->paperTexObj[0] : NULL;
            wp->mpTexObjs[1] = texData1 ? &pose->paperTexObj[1] : NULL;
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
            PSMTXScale(mtxE, pose->paperTexScale[0], pose->paperTexScale[1], 1.0f);
            PSMTXTrans(mtxD, pose->paperTexTrans[0], pose->paperTexTrans[1], 0.0f);
            PSMTXConcat(mtxE, mtxD, mtxE);
            animSetPaperTexMtx(mtxE, NULL, NULL);
            PSMTXScale(mtxE, pose->paperScaleX, pose->paperScaleY, pose->paperScaleX);
            if (wp->inBattle) {
                PSMTXTrans(mtxD, 0.0f, 2.0f * pose->paperOffsetY, 0.0f);
            } else {
                PSMTXTrans(mtxD, 0.0f, pose->paperOffsetY, 0.0f);
            }
            PSMTXConcat(mtxD, mtxE, mtxE);
            PSMTXConcat(matrix, mtxE, mtx);
            matrix = mtx;
        } else {
            PSMTXScale(mtxE, pose->paperTexScale[0], pose->paperTexScale[1], 1.0f);
            PSMTXTrans(mtxD, pose->paperTexTrans[0], pose->paperTexTrans[1], 0.0f);
            PSMTXConcat(mtxE, mtxD, mtxE);
            PSMTXTrans(mtxD, 0.5f, 1.0f, 0.0f);
            PSMTXConcat(mtxE, mtxD, mtxE);
            PSMTXScale(mtxD, 1.0f / pose->paperScaleX, 1.0f / pose->paperScaleY, 1.0f);
            PSMTXConcat(mtxE, mtxD, mtxE);
            PSMTXTrans(mtxD, (-0.5f), (-1.0f), 0.0f);
            PSMTXConcat(mtxE, mtxD, mtxE);
            animSetPaperTexMtx(mtxE, NULL, NULL);
        }
    } else {
        poseData = *wp->files[pose->fileId].handle->data;
        if (!(xluMode == 2 && (poseData->xluFlags & 1))) {
            if (!(poseData->xluFlags & (1 << xluMode))) {
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
            smartFree(pose->groupWorldPos);
        }
        if (pose->flags & 0x80) {
            smartFree(pose->groupWorldMtx);
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
                    smartFree(pose->groupWorldPos);
                }
                if (pose->flags & 0x80) {
                    smartFree(pose->groupWorldMtx);
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

extern GXColor dat_80419a34;
extern GXColor dat_80419a38;

void animPaperPoseDispSub(s32 unused, AnimationPose* param) {
#ifdef MUST_MATCH
    s32 max1; // Keep each bounds branch's maximum in its own local web.
    // An explicit cast preserves the local working pointer instead of folding it into the parameter.
    AnimationPose* pose = (AnimationPose*)param;
#else
    AnimationPose* pose = param;
#endif
    s32 x;
#ifdef MUST_MATCH
    s32 max2;
#endif
    AnimData* animData;
    s32 y;
    s32 margin1;
#ifdef MUST_MATCH
    s32 btmp; // Keep the second bounds branch's vertical margin separate from b.
#endif
    u16 w;
    s32 materialLightFlag;
    s32 flags;
    s32 y2;
    u16 h;
    s32 b;
    s32 materialFlag;

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
    s32 c;
    f32 scale;

    f31 = 8.0f;

    if ((pose->flags & 1) == 0) {
        return;
    }

    data = animPoseGetBaseData(pose);
    animData = data->mpAnims[pose->animId].data;

    f31 *= 25.74f / (ANIMDATA_F54(animData) - ANIMDATA_F48(animData));
    f30 = 0.75f * (f32)DEMOGetRenderModeObj()->efbHeight;

    sysWaitDrawSync();
    GXClearBoundingBox();

    PSMTXTrans(mtxTrans, 0.0f,
               0.25f * -(f32)DEMOGetRenderModeObj()->efbHeight - ANIMDATA_F48(animData) * f31,
               0.0f);

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

    _animPoseDrawMtx(pose, mtxTrans, 2, 0.0f, f31);

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
            max1 = x;
            if (max1 < y) max1 = y;
            c = max1;
#else
            c = x;
            if (x < y) c = y;
#endif
            x = (s32)(0.5f * (f32)c);
            margin1 = (s32)(0.14881f * (f32)(bottom - top));
            pose->paperLeft = (DEMOGetRenderModeObj()->fbWidth / 2) - c - x;
#ifdef MUST_MATCH // Preserve the original's addition order after common-subexpression elimination.
            pose->paperRight = (DEMOGetRenderModeObj()->fbWidth / 2) + c + x;
#else
            pose->paperRight = (DEMOGetRenderModeObj()->fbWidth / 2) + x + c;
#endif
            scale = 8.0f / f31;
            pose->paperTop = top - margin1;
            pose->paperBottom = bottom + margin1;
            pose->paperScaleX = scale * ((f32)(pose->paperRight - pose->paperLeft) / 150.0f);
            pose->paperScaleY = scale * ((f32)(pose->paperBottom - pose->paperTop) / 215.0f);
            pose->paperOffsetY = ANIMDATA_F48(animData) + (f30 - (f32)bottom) / f31;
            pose->flags &= ~4;
        }
    } else {
        x = (DEMOGetRenderModeObj()->fbWidth / 2) - left;
        y = right - (DEMOGetRenderModeObj()->fbWidth / 2);
#ifdef MUST_MATCH
        // match-only temp: the natural `c = x; if (x < y) c = y;` coalesces c into x in this branch
        max2 = x;
        if (max2 < y) max2 = y;
        c = max2;
#else
        c = x;
        if (x < y) c = y;
#endif
        x = (s32)(0.5f * (f32)c);
#ifdef MUST_MATCH
        btmp = (s32)(0.14881f * (f32)(bottom - top));
#else
        b = (s32)(0.14881f * (f32)(bottom - top));
#endif
        pose->paperLeft = (DEMOGetRenderModeObj()->fbWidth / 2) - c - x;
#ifdef MUST_MATCH // Preserve the original's addition order after common-subexpression elimination.
        pose->paperRight = (DEMOGetRenderModeObj()->fbWidth / 2) + c + x;
#else
        pose->paperRight = (DEMOGetRenderModeObj()->fbWidth / 2) + x + c;
#endif
        scale = 8.0f / f31;
#ifdef MUST_MATCH
        pose->paperTop = top - btmp;
        pose->paperBottom = bottom + btmp;
#else
        pose->paperTop = top - b;
        pose->paperBottom = bottom + b;
#endif
        pose->paperScaleX = scale * ((f32)(pose->paperRight - pose->paperLeft) / 150.0f);
        pose->paperScaleY = scale * ((f32)(pose->paperBottom - pose->paperTop) / 215.0f);
        pose->paperOffsetY = ANIMDATA_F48(animData) + (f30 - (f32)bottom) / f31;
    }

    left -= 2;
    top -= 2;
    right += 2;
    bottom += 2;
    left -= left & 1;
    top -= top & 1;
    right += right & 1;
    bottom += bottom & 1;
    w = right - left;
    h = bottom - top;
#ifdef MUST_MATCH
    // match-only negation: the original emits -(left - paperLeft), not paperLeft - left
    pose->paperTexTrans[0] = -((f32)left - (f32)pose->paperLeft) / (f32)(pose->paperRight - pose->paperLeft);
#else
    pose->paperTexTrans[0] = ((f32)pose->paperLeft - (f32)left) / (f32)(pose->paperRight - pose->paperLeft);
#endif
#ifdef MUST_MATCH
    // match-only negation: the original emits -(top - paperTop), not paperTop - top
    pose->paperTexTrans[1] = -((f32)top - (f32)pose->paperTop) / (f32)(pose->paperBottom - pose->paperTop);
#else
    pose->paperTexTrans[1] = ((f32)pose->paperTop - (f32)top) / (f32)(pose->paperBottom - pose->paperTop);
#endif
    pose->paperTexScale[0] = (f32)(pose->paperRight - pose->paperLeft) / (f32)w;
    pose->paperTexScale[1] = (f32)(pose->paperBottom - pose->paperTop) / (f32)h;
    GXSetTexCopySrc(left, top, w, h);
    GXSetTexCopyDst(w, h, 5, 0);
    pose->paperTexData[0] = smartAlloc(GXGetTexBufferSize(w, h, 5, 0, 0), 3);
    GXCopyTex(*(void**)pose->paperTexData[0], 1);
    GXPixModeSync();
    GXInitTexObj(&pose->paperTexObj[0], *(void**)pose->paperTexData[0], w, h, 5, 0, 0, 0);
    pose->paperTexData[1] = NULL;
}

static inline s32 animPoseSearchName(const char* name, s32 type) {
    s32 i;

    for (i = 0; i < wp->totalPoses; i++) {
        AnimationPose* pose = &wp->poses[i];
        if ((pose->flags & 1) && pose->type == type && !strcmp(animPoseGetBaseData(pose)->fileName, name)) {
            break;
        }
    }
    if (i == wp->totalPoses) {
        i = -1;
    }
    return i;
}

// animPoseDisp_MakeExtTexture

u32 anim_ext_tex_size;

extern GXColor dat_80419a3c;
extern GXColor dat_80419a40;
void animPoseDisp_MakeExtTexture(const char* name, f32 f1, void* animName, void* texObj, void* base,
                                 f32 f2, s32* out0, s32* out1, s32* out2, s32* out3) {
    s32 i;
    s32 found;
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

    i = animPoseSearchName(name, gp->inBattle != 0);
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
        found = animPoseSearchAnimIdx(p, (const char*)animName);
        if (found != p->animId) {
            p->animId = found;
            if (p->paperId != -1 && (p->flags & 2)) {
                p->flags |= 4;
            }
            p->localTime = animTimeGetTime(p->type);
            p->lastFrameZero = -1;
            p->lastFrameTime = 0.0f;
            p->evtValue = 0;
            p->loopTime = 0.0f;
        }
    }
    pose->localTime = animTimeGetTime(pose->type);
    pose->lastFrameZero = -1;
    pose->lastFrameTime = 0.0f;
    pose->evtValue = 0;
    pose->loopTime = 0.0f;
    {
        // Shadow again: the target's `- time` reset runs on a re-materialised &wp->poses[i] (r29,
        // reusing the post-loop i*0x170) and its paperId fixup reads that copy. The shadow has to be
        // declared before `time =` so the compiler schedules the fmuls above it and the (OSTime)
        // conversion below it, as the target does; moving it after costs ~1.4%.
        AnimationPose* p2 = &wp->poses[i];
        time = (OSTime)(16.666666f * f1);
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
    f30 = 0.5f * (f32)DEMOGetRenderModeObj()->fbWidth;
    f29 = 0.75f * (f32)DEMOGetRenderModeObj()->efbHeight;
    sysWaitDrawSync();
    GXClearBoundingBox();
    PSMTXTrans(mtx, 0.0f, 0.25f * -(f32)DEMOGetRenderModeObj()->efbHeight, 0.0f);
    flags = pose->flags;
    color0 = pose->materialColor;
    color1 = pose->materialColor2;
    materialFlag = pose->materialFlag;
    pose->flags &= ~8;
    pose->materialFlag = 0;
    pose->materialColor = dat_80419a3c;
    pose->materialColor2 = dat_80419a40;
    _animPoseDrawMtx(pose, mtx, 1, 0.0f, f2);
    _animPoseDrawMtx(pose, mtx, 2, 0.0f, f2);
    _animPoseDrawMtx(pose, mtx, 3, 0.0f, f2);
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
    fileId = fileAsyncf(FILE_TYPE_ANIMPOSE, callback, "%s/%s", "a", name);
    other = fileAsyncf(FILE_TYPE_TPL, NULL, "%s/%s-", "a", animGetAg2tgName(name));

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

static const char* animGetAg2tgName(const char* name) {
    void** data = wp->mp_ag2tg->data;
    s32 size = (s32)data[1];
    s32 offset;
    u8* entry = (u8*)data[0];

    for (offset = 0; offset < size; offset += 0x20) {
        if (!strcmp((char*)entry, name)) {
            return (const char*)(entry + 0x20);
        }
        entry += 0x20;
    }
    return "";
}

AnimationPose* animPoseGetAnimPosePtr(s32 poseId) {
    return &wp->poses[poseId];
}

AnimData* animPoseGetAnimDataPtr(s32 poseId) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);
    return animPoseGetAnimBaseDataPtr(poseId)->mpAnims[pose->animId].data;
}

AnimationPoseData* animPoseGetAnimBaseDataPtr(s32 poseId) {
    AnimationPose* pose = animPoseGetAnimPosePtr(poseId);
    return *wp->files[pose->fileId].handle->data;
}

AnimTableEntry* animPoseGetCurrentAnim(s32 poseId) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return &data->mpAnims[animPoseGetAnimPosePtr(poseId)->animId];
}

BOOL animPoseTestXLU(s32 poseId) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return (data->xluFlags >> 3) & 1;
}

// Runs the evalProc pass of animPoseDrawMtx on its own: positions the pose and refreshes its
// group matrices without drawing. Nothing in the game calls it (the linker strips it); generated
// here, it pools 0.0f and the degree factor ahead of animGroupBaseAsync's strings.
void animPoseEvalMtx(s32 poseId, Mtx matrix, f32 rotY, f32 scale) {
    AnimationPose* pose = &wp->poses[poseId];
    AnimationPoseData* poseData = *wp->files[pose->fileId].handle->data;
    f32(*pMtx)[4] = matrix;
    Mtx rotMtx;
    Mtx scaleMtx;
    Mtx dstMtx;

    if (!(pose->flags & 8) && rotY != 0.0f) {
        PSMTXRotRad(rotMtx, 'y', 0.017453292f * rotY);
        PSMTXConcat(matrix, rotMtx, dstMtx);
        pMtx = dstMtx;
    }
    if (1.0f != scale) {
        PSMTXScale(scaleMtx, scale, scale, scale);
        PSMTXConcat(pMtx, scaleMtx, dstMtx);
        pMtx = dstMtx;
    }
    g_modeling_mtx_lv = 0;
    g_modeling_mtx = g_modeling_mtx_stack;
    PSMTXCopy(pMtx, g_modeling_mtx_stack[0]);
    wp->mpCurPose = pose;
    wp->mpCurPoseData = poseData;
    evalProc(-1, poseData->groupCount - 1);
}

// evalProc
// The first block's register colouring needs the sign extension as a cast on the loaded element,
// as in dispProc; the pointer form under #else is the natural equivalent.
void evalProc(s32 parentGroupId, s32 groupId) {
    AnimPoseGroup* group;
    AnimPoseGroup* groups;
    AnimPoseGroup* parent;
    AnimationPoseData* data;
    Vec pos;
    f32* nodes;
    f32* node;
    f32 x, y, z;

    if (groupId == -1) {
        return;
    }
    data = wp->mpCurPoseData;
    groups = data->groups;
    group = &groups[groupId];

#ifdef MUST_MATCH
    if ((s8)wp->mpCurPose->mpGroupVisibility[group->visibilityId] != 0) {
#else
    if (((s8*)wp->mpCurPose->mpGroupVisibility)[group->visibilityId] != 0) {
#endif
        nodes = (f32*)wp->mpCurPose->mpNodes;
        node = &nodes[group->nodeId];
        if (group->useSSC) {
            if (parentGroupId == -1) {
                x = y = z = 1.0f;
            } else {
                parent = &groups[parentGroupId];
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
            ((Vec*)wp->mpCurPose->groupWorldPos->address)[groupId] = pos;
        }
        if (wp->mpCurPose->flags & 0x80) {
            PSMTXCopy(*g_modeling_mtx, ((Mtx*)wp->mpCurPose->groupWorldMtx->address)[groupId]);
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
    pose->groupWorldPosCount = data->groupCount;
    pose->groupWorldPos = smartAlloc(data->groupCount * sizeof(Vec), type);
}

void L_animPoseWorldPositionEvalOff(s32 poseId) {
    AnimationPose* pose = &wp->poses[poseId];
    if (pose->flags & 0x40) {
        pose->flags &= ~0x40;
        smartFree(pose->groupWorldPos);
        pose->groupWorldPos = NULL;
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
    pose->groupWorldMtxCount = data->groupCount;
    pose->groupWorldMtx = smartAlloc(data->groupCount * sizeof(Mtx), type);
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
// The 14 saved wp fields are declared in the target's colour order (r21/r20/r19 for
// mpCurPose/AnimData/PoseData, r30..r22 for the tex/data/mtx fields, r18/r17 for the two tail words);
// MWCC assigns those registers in declaration order, while the loads/restores stay in struct-field order.
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
    wp->mpCurAnimData = animPoseGetBaseData(pose)->mpAnims[pose->animId].data;
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

    animPoseMainScratch(pose);
    renderProc(shapeId, pose->keyFrame1);

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
    group = data->groups;
    for (i = 0; i < data->groupCount; i++, group++) {
        if (!strcmp(group->name, name)) {
            return i;
        }
    }
    return -1;
}

const char* animPoseGetGroupName(s32 poseId, s32 group) {
    AnimationPoseData* data = animPoseGetAnimBaseDataPtr(poseId);
    return data->groups[group].name;
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

