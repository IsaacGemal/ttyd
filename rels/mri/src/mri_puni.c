#include "mri_puni.h"
#include "mri.h"
#include <string.h>
#include "memory.h"
#include "driver/camdrv.h"
#include "evt/evt_bero.h"
#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "evt/evt_seq.h"
#include "evt/evt_sub.h"
#include "forcedata.h"

#pragma pool_data off

#include "driver/icondrv.h"
#include "driver/animdrv.h"
#include "mario/mariost.h"
#include "driver/swdrv.h"
#include "math_ext.h"
#include "mri_02.h"
#include <stdlib.h>
#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include <dolphin/os.h>
#include "mario/mario.h"
#include "pmario_sound.h"
#include "system.h"
#include "eff/eff_puniballoon.h"
#include "driver/npcdrv.h"
#include "eff/eff_fukidashi.h"
#include "driver/envdrv.h"
#include "eff/eff_n64.h"
#include "punidrv.h"
#include "driver/dispdrv.h"
#include "mario/mario_party.h"
#include "party.h"
#include "party/party_cloud.h"
#include "manager/evtmgr_cmd.h"
#include <stdio.h>
#include "evt/evt_shop.h"
typedef struct MriPuniAnimSet {
    s16* unk0[10]; // 0x00
    s16 unk28[10]; // 0x28
    s16 unk3C[10]; // 0x3C
} MriPuniAnimSet;

typedef struct PuniFallTarget {
    s32 unk0;    // 0x0
    Vec vec;     // 0x4
    void* unk10; // 0x10 (actor pointer)
} PuniFallTarget; // 0x14

typedef struct PuniFallActor {
    s32 unk0;  // 0x0
    u32 flags; // 0x4
} PuniFallActor;

typedef struct PuniSearchEntry {
    s32 flags;                 // 0x0
    Vec pos;                   // 0x4
    u8 unk10[0x14 - 0x10];     // 0x10
} PuniSearchEntry; // 0x14

typedef struct PuniPaternData {
    u16* pat[10]; // 0x00
    s16 num[10];  // 0x28
    s16 end[10];  // 0x3C
} PuniPaternData;













#pragma force_active on
extern PuniMap* Punimap;
static f32 randf(f32 min, f32 max);
extern PuniTexInfo* Punitex;
extern PuniWork* Punigp;
extern PuniData* Punidat;


static inline f32 ymodeRamp(f32 c, f32 z, f32 base) {
    f32 t = (c - z) / 20.0f;

    if (t > 0.5f) {
        t = (t - 0.5f) / 0.5f;
    } else {
        t = 0.0f;
    }
    return 10.0f * t + base;
}

static inline s32 puni_rnd(s32 range) {
    s32 s;
    s32 v;

    Punigp->field_0x450 =
        Punigp->field_0x454 + 0x17340000 + Punigp->field_0x450 - 0x506f;
    Punigp->field_0x454 =
        Punigp->field_0x450 + 0x21270000 + Punigp->field_0x454 + 0x3f9c;
    s = Punigp->field_0x454;
    v = s;
    if (v < 0) {
        v = -v;
    }
    return v % range;
}

static inline s32 puniQrnd(s32 range) {
    s32 s;
    s32 v;


    Punigp->field_0x450 =
        Punigp->field_0x454 + 0x17340000 + Punigp->field_0x450 - 0x506f;
    Punigp->field_0x454 =
        Punigp->field_0x450 + 0x21270000 + Punigp->field_0x454 + 0x3f9c;
    s = Punigp->field_0x454;
    v = s;
    if (v < 0) {
        v = -v;
    }
    return v % range;
}

static inline void puniModeSearch_sub(s32 xx, s32 yy, f32* pc, f32* pd) {
    switch (Punigp->field_0x340) {
    case 0:
        break;
    case 1:
        puniMapdata_MRI_06_getpos(xx, yy, pc, pd);
        break;
    case 2:
        switch (xx) {
        case 1: *pc = -250.0f; break;
        case 2: *pc = -145.0f; break;
        case 3: *pc = 0.0f; break;
        case 4: *pc = 145.0f; break;
        case 5: *pc = 260.0f; break;
        default: *pc = -1000.0f; break;
        }
        switch (yy) {
        case 1: *pd = -120.0f; break;
        case 2: *pd = 0.0f; break;
        case 3: *pd = 105.0f; break;
        case 4: *pd = 160.0f; break;
        default: *pd = -1000.0f; break;
        }
        break;
    case 3:
        puniMapdata_MRI_16_getpos(xx, yy, pc, pd);
        break;
    }
}

static inline f32 puniRandf(f32 a, f32 b) {
    return (f32)rand() / 32767.0f * (b - a) + a;
}

static inline Vec puniMakeVec(f32 x, f32 y, f32 z) {
    Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

static inline void puniMapSetData(f32 unk0, f32 unk4, s32 unk8, s32 unkC, u32* data) {
    Punimap->field_0x0 = unk0;
    Punimap->field_0x4 = unk4;
    if (data == NULL) {
        Punimap->field_0x14 = 0;
        return;
    }
    Punimap->field_0x8 = unk8;
    Punimap->field_0xC = unkC;
    Punimap->field_0x14 = 1;
    Punimap->field_0x18 = data;
    Punimap->field_0x1C = 0;
    Punimap->field_0x10 = unk8 * unkC * 4;
    Punimap->field_0x20 = _mapAlloc(Punimap->field_0x10);
    memcpy(Punimap->field_0x18, data, Punimap->field_0x10);
}

static inline f32 puni_rndf(f32 min, f32 max) {
    return (f32)rand() / 32767.0f * (max - min) + min;
}

static inline s32 puni_sext16(s32 v) {
#ifdef MUST_MATCH
    if (v & 0x8000) {
        v |= 0xFFFF0000;
    }
#else
    v = (s16)v;
#endif
    return v;
}

static inline f32 puniNearest(f32 v) {
    f32 d = __fabs(v - Punigp->field_0x32C);
    f32 r = Punigp->field_0x32C;
    f32 t;

    t = __fabs(v - Punigp->field_0x330);
    if (d > t) {
        d = t;
        r = Punigp->field_0x330;
    }
    t = __fabs(v - Punigp->field_0x334);
    if (d > t) {
        d = t;
        r = Punigp->field_0x334;
    }
    t = __fabs(v - Punigp->field_0x338);
    if (d > t)
        r = Punigp->field_0x338;
    return r;
}

static inline BOOL puniHagureCheck(PuniData* punidat, Vec* pos, Vec* target) {
    Vec delta;
    Vec out;
    f32 dist;
    f32 nx, ny, nz;

    PSVECSubtract(target, pos, &delta);
    if (delta.x == 0.0f && delta.z == 0.0f) {
        punidat->field_0x12C = *pos;
        return TRUE;
    }
    PSVECNormalize(&delta, &delta);
    dist = PSVECMag(&delta);
    if (hitCheckFilter(NULL, &out.x, &out.y, &out.z, &dist, &nx, &ny, &nz, pos->x, pos->y, pos->z,
                       delta.x, delta.y, delta.z) == NULL) {
        punidat->field_0x12C = *target;
        return TRUE;
    }
    return FALSE;
}

static inline f32 puniExitNearest(f32 f) {
    f32 best = Punigp->field_0x32C;
    f32 dist = (f32)__fabs(f - best);
    f32 dist2 = (f32)__fabs(f - Punigp->field_0x330);

    if (dist > dist2) {
        dist = dist2;
        best = Punigp->field_0x330;
    }
    dist2 = (f32)__fabs(f - Punigp->field_0x334);
    if (dist > dist2) {
        dist = dist2;
        best = Punigp->field_0x334;
    }
    dist2 = (f32)__fabs(f - Punigp->field_0x338);
    if (dist > dist2) {
        best = Punigp->field_0x338;
    }
    return best;
}

// clang-format off
extend_data(76,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(77,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

u16 puni_nom_patern_data[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,    21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 0,  1,  2,  3,  4,  5,     6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,    31, 32, 33, 34,
    35, 36, 37, 38, 39, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,    16, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 51, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 65535,
};

u16 puni_wlk_patern_data[] = {
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 65535,
};

u16 puni_run_patern_data[] = { 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 65535 };

u16 puni_dmg_patern_data[] = { 72, 65535 };

u16 puni_dwn_patern_data[] = { 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 65535 };

u16 puni_rol_patern_data[] = { 83, 65535 };

u16 puni_hag_patern_data[] = { 0, 65535 };

u16 puni_jpu_patern_data[] = { 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 65534 };

u16 puni_jpd_patern_data[] = { 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 65534 };

u16 puni_wan_patern_data[] = { 104, 105, 106, 107, 108, 109, 110, 111, 65534 };

u16* puni_patern_table[] = { puni_nom_patern_data, puni_wlk_patern_data, puni_run_patern_data, puni_dmg_patern_data,
                             puni_dwn_patern_data, puni_rol_patern_data, puni_hag_patern_data, puni_jpu_patern_data,
                             puni_jpd_patern_data, puni_wan_patern_data };

u16 toge_nom_patern_data[25] = {
    84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 65535,
};

u16 toge_wlk_patern_data[] = { 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 65535 };

u16 toge_run_patern_data[] = { 108, 110, 112, 114, 116, 118, 65535 };

u16 toge_dmg_patern_data[] = { 120, 65535 };

u16 toge_dwn_patern_data[] = { 121, 122, 123, 124, 125, 126, 127, 128, 65535 };

u16 toge_rol_patern_data[] = { 129, 65535 };

u16 toge_hag_patern_data[] = { 65535 };

u16* toge_patern_table[] = { toge_nom_patern_data, toge_wlk_patern_data, toge_run_patern_data, toge_dmg_patern_data,
                             toge_dwn_patern_data, toge_rol_patern_data, toge_hag_patern_data, toge_hag_patern_data,
                             toge_hag_patern_data, toge_hag_patern_data };


// TODO: type
static u32 jumptable_00023FA0[7] = {
    (u32)((u8*)&puniSetYPosition + 0x280), (u32)((u8*)&puniSetYPosition + 0x50), (u32)((u8*)&puniSetYPosition + 0xcc), (u32)((u8*)&puniSetYPosition + 0x154),
    (u32)((u8*)&puniSetYPosition + 0x1e4), (u32)((u8*)&puniSetYPosition + 0x1ec), (u32)((u8*)&puniSetYPosition + 0x1f4),
};

// TODO: type
u32 puniMapdata_MRI_06[33] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000,
    0x0000FFFF, 0x00000000, 0x0000FFFF, 0x00000000,
    0x0000FFFF, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000,
    0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFFFFFF,
    0x0000FFFF, 0x00000000, 0x0000FFFF, 0x00000000,
    0x0000FFFF, 0x0000FFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFF0000,
};

// TODO: type
u32 puniMapdata_MRI_09[21] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0x20002000, 0x20002000, 0x2000FFFF, 0xFFFF2000,
    0x00000000, 0x00002000, 0xFFFFFFFF, 0x20000000,
    0x00000000, 0x2000FFFF, 0xFFFF2000, 0x00000000,
    0x00001000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF,
};

// TODO: type
u32 puniMapdata_MRI_09_01[21] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0x20002000, 0x00002000, 0x2000FFFF, 0xFFFF2000,
    0x00000000, 0x00002000, 0xFFFFFFFF, 0x20000000,
    0x00000000, 0x2000FFFF, 0xFFFF2000, 0x00000000,
    0x00001000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF,
};

// TODO: type
u32 puniMapdata_MRI_16[50] = {
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0x2000FFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0x2000FFFF, 0xFFFF2000,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF2000,
    0xFFFFFFFF, 0x20000300, 0x03000300, 0x03000300,
    0x03000300, 0x2000FFFF, 0xFFFF2000, 0x10001000,
    0x10001000, 0x10001000, 0x10002000, 0xFFFFFFFF,
    0x20000100, 0x01000100, 0x01000100, 0x01000100,
    0x2000FFFF, 0xFFFF2000, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFF2000, 0xFFFFFFFF, 0x2000FFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x2000FFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFF0000,
};


// TODO: type
static u32 jumptable_000241B0[12] = {
    (u32)((u8*)&puniMapdata_MRI_06_getpos + 0xd0), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x20), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x30), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x40),
    (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x50), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x60), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x70), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x80),
    (u32)((u8*)&puniMapdata_MRI_06_getpos + 0x90), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0xa0), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0xb0), (u32)((u8*)&puniMapdata_MRI_06_getpos + 0xc0),
};


// TODO: type
static u32 jumptable_000241E0[8] = {
    (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x14c), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0xdc), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0xec), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0xfc),
    (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x10c), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x11c), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x12c), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x13c),
};

// TODO: type
static u32 jumptable_00024200[10] = {
    (u32)((u8*)&puniMapdata_MRI_16_getpos + 0xb0), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x20), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x30), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x40),
    (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x50), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x60), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x70), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x80),
    (u32)((u8*)&puniMapdata_MRI_16_getpos + 0x90), (u32)((u8*)&puniMapdata_MRI_16_getpos + 0xa0),
};


// TODO: type
static u32 jumptable_00024228[11] = {
    (u32)((u8*)&puniModeWind + 0x40), (u32)((u8*)&puniModeWind + 0x1bc), (u32)((u8*)&puniModeWind + 0x318), (u32)((u8*)&puniModeWind + 0x440),
    (u32)((u8*)&puniModeWind + 0x470), (u32)((u8*)&puniModeWind + 0x6bc), (u32)((u8*)&puniModeWind + 0x6bc), (u32)((u8*)&puniModeWind + 0x6bc),
    (u32)((u8*)&puniModeWind + 0x6bc), (u32)((u8*)&puniModeWind + 0x6bc), (u32)((u8*)&puniModeWind + 0x574),
};


// TODO: type
static u32 jumptable_00024254[35] = {
    (u32)((u8*)&puniModeHagure + 0x38), (u32)((u8*)&puniModeHagure + 0x88), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218),
    (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218),
    (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0xf4), (u32)((u8*)&puniModeHagure + 0x218),
    (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218),
    (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218),
    (u32)((u8*)&puniModeHagure + 0x15c), (u32)((u8*)&puniModeHagure + 0x16c), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218),
    (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218),
    (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x218), (u32)((u8*)&puniModeHagure + 0x18c), (u32)((u8*)&puniModeHagure + 0x1fc),
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 shadow_dl[8] = {
    0x80000400, 0x00010102, 0x02030300, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 shadow_vtx[16] = {
    0xBF99999A, 0x00000000, 0xBF99999A, 0xBF99999A,
    0x00000000, 0x3F99999A, 0x3F99999A, 0x00000000,
    0x3F99999A, 0x3F99999A, 0x00000000, 0xBF99999A,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 shadow_coord[8] = {
    0x00000000, 0x3F800000, 0x00000000, 0x00000000,
    0x3F800000, 0x00000000, 0x3F800000, 0x3F800000,
};



// TODO: type
static u32 jumptable_00024390[30] = {
    (u32)((u8*)&unk_mri_00012d58 + 0x40), (u32)((u8*)&unk_mri_00012d58 + 0xb4), (u32)((u8*)&unk_mri_00012d58 + 0x1b4), (u32)((u8*)&unk_mri_00012d58 + 0x1c8),
    (u32)((u8*)&unk_mri_00012d58 + 0x1dc), (u32)((u8*)&unk_mri_00012d58 + 0x1f0), (u32)((u8*)&unk_mri_00012d58 + 0x204), (u32)((u8*)&unk_mri_00012d58 + 0x218),
    (u32)((u8*)&unk_mri_00012d58 + 0x4a8), (u32)((u8*)&unk_mri_00012d58 + 0x480), (u32)((u8*)&unk_mri_00012d58 + 0x494), (u32)((u8*)&unk_mri_00012d58 + 0x4bc),
    (u32)((u8*)&unk_mri_00012d58 + 0x4d0), (u32)((u8*)&unk_mri_00012d58 + 0x53c), (u32)((u8*)&unk_mri_00012d58 + 0xde0), (u32)((u8*)&unk_mri_00012d58 + 0x550),
    (u32)((u8*)&unk_mri_00012d58 + 0x804), (u32)((u8*)&unk_mri_00012d58 + 0xa68), (u32)((u8*)&unk_mri_00012d58 + 0x818), (u32)((u8*)&unk_mri_00012d58 + 0x900),
    (u32)((u8*)&unk_mri_00012d58 + 0x914), (u32)((u8*)&unk_mri_00012d58 + 0xa40), (u32)((u8*)&unk_mri_00012d58 + 0xa54), (u32)((u8*)&unk_mri_00012d58 + 0xa7c),
    (u32)((u8*)&unk_mri_00012d58 + 0xb44), (u32)((u8*)&unk_mri_00012d58 + 0xb58), (u32)((u8*)&unk_mri_00012d58 + 0xb6c), (u32)((u8*)&unk_mri_00012d58 + 0xbd4),
    (u32)((u8*)&unk_mri_00012d58 + 0xb80), (u32)((u8*)&unk_mri_00012d58 + 0xdc0),
};


// TODO: type
static u32 jumptable_00024408[30] = {
    (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0x40),
    (u32)((u8*)&fn_19_13A90 + 0x364), (u32)((u8*)&fn_19_13A90 + 0x900), (u32)((u8*)&fn_19_13A90 + 0x900), (u32)((u8*)&fn_19_13A90 + 0x40),
    (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0x628), (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0xc2c),
    (u32)((u8*)&fn_19_13A90 + 0xc2c), (u32)((u8*)&fn_19_13A90 + 0x900), (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0xc2c),
    (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0xc2c), (u32)((u8*)&fn_19_13A90 + 0xc2c), (u32)((u8*)&fn_19_13A90 + 0xc2c),
    (u32)((u8*)&fn_19_13A90 + 0xc2c), (u32)((u8*)&fn_19_13A90 + 0xc2c), (u32)((u8*)&fn_19_13A90 + 0xc2c), (u32)((u8*)&fn_19_13A90 + 0x40),
    (u32)((u8*)&fn_19_13A90 + 0x90c), (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0x40), (u32)((u8*)&fn_19_13A90 + 0x40),
    (u32)((u8*)&fn_19_13A90 + 0xc2c), (u32)((u8*)&fn_19_13A90 + 0x8e0),
};


EVT_BEGIN(puni_piders_check)
    CALL(evt_seq_wait, 2)
    LOOP(0)
        CALL(puni_piders_check_func)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(puni_disp_info)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 118)
        RETURN
    END_IF
    LOOP(0)
        CALL(puni_disp_info_sub, LW(0), LW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(mri_puninum_fadeout)
    WAIT_MS(1200)
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_puni_puninum_alpha, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mri_puninum_fadein)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 500)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_puni_puninum_alpha, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()






EVT_BEGIN(mri_puni_position_init)
    CALL(evt_puni_set_systemmode, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_puni_puninum_alpha, 0)
        WAIT_FRAMES(2)
        CALL(evt_bero_get_now_number, LW(10))
        CALL(evt_bero_get_info_kinddir, LW(10), LW(0), LW(1), LW(2))
        CALL(evt_bero_get_info_nextarea, LW(10), LW(3), LW(4))
        SET_VALUE(LF(0), 0)
        IF_INT_EQUAL(LW(0), 2)
            IF_INT_NOT_EQUAL(LW(3), 0)
                SET_VALUE(LF(0), 1)
            END_IF
        END_IF
        IF_INT_EQUAL(LF(0), 0)
            CALL(evt_puni_puninum_alpha, 255)
        END_IF
    END_INLINE_EVENT
    DEBUG_PUT_MSG(STRING("プニ関係：初期の実行中待ち"))
    CALL(evt_bero_exec_wait, 1)
    LOOP(0)
        CALL(evt_puni_set_systemmode, 1)
        DEBUG_PUT_MSG(STRING("プニ関係：座標セット待ち"))
        CALL(evt_bero_exec_wait, 2)
        DEBUG_PUT_MSG(STRING("プニ関係：小動物関係初期化"))
        DEBUG_PUT_REG(GSW(665))
        DEBUG_PUT_REG(GSW(666))
        CALL(evt_bero_get_now_number, LW(10))
        CALL(evt_bero_get_info_kinddir, LW(10), LW(0), LW(1), LW(2))
        CALL(evt_bero_get_info_nextarea, LW(10), LW(3), LW(4))
        IF_INT_EQUAL(LW(0), 2)
            IF_INT_NOT_EQUAL(LW(3), 0)
                SPAWN_EVENT(mri_puninum_fadein)
            END_IF
        END_IF
        CALL(evt_puni_setup, LW(0), LW(1), LW(2))
        CALL(evt_toge_setup, LW(6), LW(7), LW(8), LW(9))
        CALL(evt_puni_set_systemmode, 2)
        DEBUG_PUT_MSG(STRING("プニ関係：終了待ち"))
        CALL(evt_bero_exec_wait, 65536)
        CALL(evt_puni_set_systemmode, 3)
        DEBUG_PUT_MSG(STRING("プニ関係：次の実行中待ち"))
        CALL(evt_bero_exec_wait, 1)
        SET_VALUE(GSW(666), GSW(665))
        CALL(evt_bero_get_now_number, LW(10))
        CALL(evt_bero_get_info_kinddir, LW(10), LW(0), LW(1), LW(2))
        CALL(evt_bero_get_info_nextarea, LW(10), LW(3), LW(4))
        IF_INT_EQUAL(LW(0), 1)
            DEBUG_PUT_MSG(STRING("プニ関係：扉待ち"))
            CALL(evt_bero_exec_wait, 268435456)
            DEBUG_PUT_MSG(STRING("プニ関係：扉待ち終わり"))
            CALL(evt_bero_exec_onoff, 1, 268435456)
        END_IF
        IF_INT_EQUAL(LW(0), 2)
            IF_INT_NOT_EQUAL(LW(3), 0)
                SPAWN_EVENT(mri_puninum_fadeout)
            END_IF
        END_IF
        BEGIN_INLINE_EVENT
            DEBUG_PUT_MSG(STRING("プニ関係：エリアチェンジ直前待ち"))
            CALL(evt_bero_exec_wait, 8)
            DEBUG_PUT_MSG(STRING("プニ関係：プニ族トゲ族SE強制オフ"))
            CALL(evt_puni_force_se_off)
        END_INLINE_EVENT
        CALL(evt_puni_exit, LW(0), LW(1), LW(2), LW(3))
    END_LOOP
    RETURN
EVT_END()


EVT_BEGIN(mri_puni_main)
    SET_TYPE(0)
    CALL(evt_puni_main)
    RETURN
EVT_END()

EVT_BEGIN(mri_puni_init_dvd)
    CALL(evt_npc_entry, STRING("プニsp"), STRING("c_puni"))
    CALL(evt_npc_set_tribe, STRING("プニsp"), STRING("プニ族"))
    CALL(evt_npc_entry, STRING("トゲsp"), STRING("c_toge"))
    CALL(evt_npc_set_tribe, STRING("トゲsp"), STRING("トゲ族"))
    CALL(evt_npc_entry, STRING("ガイドsp"), STRING("c_pguide"))
    CALL(evt_npc_set_tribe, STRING("ガイドsp"), STRING("ガイド"))
    CALL(evt_npc_entry, STRING("ガイド妹sp"), STRING("c_psister"))
    CALL(evt_npc_set_tribe, STRING("ガイド妹sp"), STRING("ガイド妹"))
    CALL(evt_npc_entry, STRING("ちびトゲsp"), STRING("c_tchibi"))
    CALL(evt_npc_set_tribe, STRING("ちびトゲsp"), STRING("ちびトゲ"))
    CALL(evt_npc_entry, STRING("ライバルsp"), STRING("c_prival"))
    CALL(evt_npc_set_tribe, STRING("ライバルsp"), STRING("ライバル"))
    CALL(evt_npc_set_position, STRING("プニsp"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("トゲsp"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ガイドsp"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ガイド妹sp"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ちびトゲsp"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("ライバルsp"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("プニsp"), 1074266144)
    CALL(evt_npc_flag_onoff, 1, STRING("トゲsp"), 1074266144)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイドsp"), 1074266144)
    CALL(evt_npc_flag_onoff, 1, STRING("ガイド妹sp"), 1074266144)
    CALL(evt_npc_flag_onoff, 1, STRING("ちびトゲsp"), 1074266144)
    CALL(evt_npc_flag_onoff, 1, STRING("ライバルsp"), 1074266144)
    RETURN
EVT_END()







EVT_BEGIN(mri_puni_init)
    CALL_CHILD(mri_puni_init_dvd)
    SWITCH(GSW(0))
        CASE_LESS(88)
        RETURN
        CASE_GREATER_OR_EQUAL(403)
        RETURN
        CASE_EQUAL(400)
        CALL(evt_sub_get_mapname, LW(0))
        IF_STR_NOT_EQUAL(LW(0), STRING("mri_00"))
            RETURN
        END_IF
        SET_VALUE(LF(0), 0)
        CALL(evt_bero_get_entername, LW(0))
        IF_STR_EQUAL(LW(0), STRING("sekai_yami1"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_STR_EQUAL(LW(0), STRING("minnnanokoe"))
            SET_VALUE(LF(0), 1)
        END_IF
        IF_INT_EQUAL(LF(0), 0)
            RETURN
        END_IF
        CASE_BETWEEN(118, 399)
        CALL(evt_sub_get_mapname, LW(0))
        IF_STR_NOT_EQUAL(LW(0), STRING("mri_00"))
            RETURN
        END_IF
        IF_INT_EQUAL(GSWF(2884), 1)
            RETURN
        END_IF
        CASE_DEFAULT
    END_SWITCH
    CALL(evt_make_texbuf)
    CALL(evt_puni_init, 210, PTR(puni_puni_init), PTR(puni_puni_main), PTR(puni_puni_dispent))
    CALL(evt_puni_set_maze)
    SPAWN_EVENT(mri_puni_position_init)
    SPAWN_EVENT(mri_puni_main)
    RETURN
EVT_END()

EVT_BEGIN(mri_puni_init_none)
    CALL_CHILD(mri_puni_init_dvd)
    CALL(evt_make_texbuf)
    CALL(evt_puni_init, 210, PTR(puni_puni_init), PTR(puni_puni_main), PTR(puni_puni_dispent))
    CALL(evt_puni_set_maze)
    RETURN
EVT_END()

// TODO: h3$2957 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 h3$2957[2] = {
//     0x00, 0x00,
// };

// TODO: h2$2956 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 h2$2956[2] = {
//     0x00, 0x00,
// };

// TODO: h1$2955 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 h1$2955[2] = {
//     0x00, 0x00,
// };

// TODO: h0$2954 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 h0$2954[2] = {
//     0x00, 0x00,
// };

// TODO: m3$2953 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 m3$2953[2] = {
//     0x00, 0x00,
// };

// TODO: m2$2952 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 m2$2952[2] = {
//     0x00, 0x00,
// };

// TODO: m1$2951 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 m1$2951[2] = {
//     0x00, 0x00,
// };

// TODO: m0$2950 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 m0$2950[2] = {
//     0x00, 0x00,
// };

// TODO: find$2949 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 find$2949[2] = {
//     0x00, 0x00,
// };

// TODO: move$2948 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 move$2948[2] = {
//     0x00, 0x00,
// };

// TODO: takasa$2947 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u8 takasa$2947[2] = {
//     0x00, 0x00,
// };

// TODO: debug$2303 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 debug$2303[1] = {
//     0x00000000,
// };

PuniMap* Punimap;

PuniTexInfo* Punitex;

PuniWork* Punigp;

PuniData* Punidat;
PuniData* puniGetPunidat(void) {
    return Punidat;
}
PuniWork* puniGetPunigp(void) {
    return Punigp;
}
PuniTexInfo* puniGetTex(void) {
    return Punitex;
}
void evt_icon_disp_func(CameraId camId, void* param) {
    PuniWork* work = Punigp;
    PuniData* dat;
    s32 i;

    if (work->unk328 != 9 && work->unk328 != 12) {
        return;
    }

    for (i = 1; i < PUNI_DATA_MAX; i++) {
        dat = &Punidat[i];
        if ((dat->unk4 & 0x400) && !(dat->unk4 & 0x804000)) {
            Vec pos;
            Vec pos2;

            pos = dat->field_0x12C;
            if (dat->field_0x8 & 1) {
                (dat->field_0x121)++;
                if (*(s8*)((u8*)dat + 0x121) > 30) {
                    dat->field_0x121 = 0;
                    dat->field_0x8 &= ~1;
                }
            }
            if (dat->field_0x8 & 2) {
                pos2 = pos;
                puniSinCosRet(dat->field_0x116 + gp->retraceCount * 10, &pos.z, &pos.x, 9.0f, 9.0f);
                pos.y += 13.0f;
                iconDispGx(&pos, 0, 0x14D, 0.6f);

                puniSinCosRet(dat->field_0x116 + gp->retraceCount * 10 + 0xB4, &pos2.z, &pos2.x, 9.0f, 9.0f);
                pos2.y += 15.0f;
                iconDispGx(&pos2, 0, 0x14D, 0.5f);
            }
        }
    }
}
PuniTexInfo* _make_texbuf(PuniTexInfo* texbuf, s32 count, const char* a3, const char* a4, f32 a5) {
    s32 i;

    for (i = 0; i < count; i++) {
        texbuf->unk0 = _mapAlloc(0xC);
        texbuf->unk4 = _mapAlloc(0xC);
        strcpy(texbuf->unk0, a3);
        strcpy(texbuf->unk4, a4);
        texbuf->unk8 = a5;
        a5 += 1.0f;
        texbuf++;
    }
    return texbuf;
}
USER_FUNC(evt_make_texbuf) {
    PuniTexInfo* texbuf = Punitex;

    texbuf = Punitex;
    texbuf = _make_texbuf(texbuf, 40, "c_puni", "S_1", 0.0f);
    texbuf = _make_texbuf(texbuf, 12, "c_puni", "S_1", 97.0f);
    texbuf = _make_texbuf(texbuf, 20, "c_puni", "W_1", 0.0f);
    texbuf = _make_texbuf(texbuf, 1, "c_puni", "D_1", 0.0f);
    texbuf = _make_texbuf(texbuf, 10, "c_puni", "D_2", 0.0f);
    texbuf = _make_texbuf(texbuf, 1, "c_puni", "F_1", 0.0f);

    if (Punigp->unk328 == 9 || Punigp->unk328 == 12) {
        texbuf = _make_texbuf(texbuf, 24, "c_toge", "S_1", 0.0f);
        texbuf = _make_texbuf(texbuf, 12, "c_toge", "W_1", 0.0f);
        texbuf = _make_texbuf(texbuf, 1, "c_toge", "D_1", 0.0f);
        texbuf = _make_texbuf(texbuf, 8, "c_toge", "D_2", 0.0f);
        texbuf = _make_texbuf(texbuf, 1, "c_toge", "F_1", 0.0f);
    } else {
        texbuf = _make_texbuf(texbuf, 10, "c_puni", "J_1A", 0.0f);
        texbuf = _make_texbuf(texbuf, 10, "c_puni", "J_1B", 0.0f);
        texbuf = _make_texbuf(texbuf, 8, "c_puni", "B_1", 0.0f);
    }
    texbuf->unk0 = 0;
    texbuf->unk4 = 0;
    texbuf->unk8 = 0.0f;
    return EVT_RETURN_DONE;
}
void puniAnimCount(s16* anim, s16* count, s16* term) {
    s32 n = 0;
    s32 i = 0;
    s16 v;

    while (1) {
        v = anim[i];
        if (v == -1) {
            *term = v;
            break;
        }
        if (v == -2) {
            *term = v;
            break;
        }
        n++;
        i++;
    }
    *count = n;
}
static s32 puniAnimCountInline(s16** arr, s32 idx, s16* term, s32 count) {
    s16 v;
    s16* tbl = arr[idx];
    s32 n = 0;
    s32 j = count;
    while (1) {
        v = tbl[j];
        if (v == -1) {
            *term = v;
            break;
        }
        if (v == -2) {
            *term = v;
            break;
        }
        n++;
        j++;
    }
    return n;
}


// The inlined helper reproduces the original's `li r8,0; mr r7,r8` (j = count)
// and loads the table into r9 with the value in r10; the second loop is manual.
void puniAnimationInit(s16** a, s16** b) {
    PuniWork* work = Punigp;
    MriPuniAnimSet* set0 = (MriPuniAnimSet*)((u8*)work + 0x4);
    MriPuniAnimSet* set1 = (MriPuniAnimSet*)((u8*)work + 0x54);
    s32 i;

    for (i = 0; i < 10; i++) {
        s32 count;
        s32 j;
        s16* tbl;
        s16 v;

        set0->unk0[i] = a[i];
        count = puniAnimCountInline(a, i, &set0->unk3C[i], 0);
        set0->unk28[i] = count;

        set1->unk0[i] = b[i];
        tbl = b[i];
        count = 0;
        j = 0;
        while (1) {
            v = tbl[j];
            if (v == -1) {
                set1->unk3C[i] = v;
                break;
            }
            if (v == -2) {
                set1->unk3C[i] = v;
                break;
            }
            count++;
            j++;
        }
        set1->unk28[i] = count;
    }
}
void puniAnimationInit2(void) {
    char* p;

    p = (char*)Punigp + 0xA4;
    strcpy(p + 0x0, "S_1");
    strcpy(p + 0x10, "W_1");
    strcpy(p + 0x20, "R_1");
    strcpy(p + 0x30, "D_1");
    strcpy(p + 0x40, "D_2");
    strcpy(p + 0x50, "F_1");
    strcpy(p + 0x60, "Z_1");
    strcpy(p + 0x70, "J_1A");
    strcpy(p + 0x80, "J_1B");
    strcpy(p + 0x90, "B_1");
    p = (char*)Punigp + 0x144;
    strcpy(p + 0x0, "S_1");
    strcpy(p + 0x10, "W_1");
    strcpy(p + 0x20, "R_1");
    strcpy(p + 0x30, "D_1");
    strcpy(p + 0x40, "D_2");
    strcpy(p + 0x50, "F_1");
    strcpy(p + 0x60, "Z_1");
    strcpy(p + 0x70, "J_1A");
    strcpy(p + 0x80, "J_1B");
    strcpy(p + 0x90, "B_1");
    p = (char*)Punigp + 0x1E4;
    strcpy(p + 0x0, "S_1");
    strcpy(p + 0x10, "W_1");
    strcpy(p + 0x20, "R_1");
    strcpy(p + 0x30, "D_1");
    strcpy(p + 0x40, "D_2");
    strcpy(p + 0x50, "F_1");
    strcpy(p + 0x60, "Z_1");
    strcpy(p + 0x70, "J_1A");
    strcpy(p + 0x80, "J_1B");
    strcpy(p + 0x90, "B_1");
    p = (char*)Punigp + 0x284;
    strcpy(p + 0x0, "S_1");
    strcpy(p + 0x10, "W_1");
    strcpy(p + 0x20, "R_1");
    strcpy(p + 0x30, "D_2");
    strcpy(p + 0x40, "D_2");
    strcpy(p + 0x50, "F_1");
    strcpy(p + 0x60, "Z_1");
    strcpy(p + 0x70, "J_1A");
    strcpy(p + 0x80, "J_1B");
    strcpy(p + 0x90, "B_1");
}
void puniAnimation(PuniData* dat, PuniPose* pose) {
    NpcEntry* npc;
    char (*names)[16];
    s32 visible;

    if (dat->unk4 & 0x40000) {
        return;
    }

    if (dat->unk4 & 0x1E0) {
        s32 frame;

        if (dat->unk4 & 0x20) {
            npc = npcNameToPtr("ガイドsp");
            names = (char(*)[16])((u8*)Punigp + 0xA4);
        } else if (dat->unk4 & 0x40) {
            npc = npcNameToPtr("ガイド妹sp");
            names = (char(*)[16])((u8*)Punigp + 0x144);
        } else if (dat->unk4 & 0x80) {
            npc = npcNameToPtr("ちびトゲsp");
            names = (char(*)[16])((u8*)Punigp + 0x1E4);
        } else if (dat->unk4 & 0x100) {
            npc = npcNameToPtr("ライバルsp");
            names = (char(*)[16])((u8*)Punigp + 0x284);
        } else {
            npc = npcNameToPtr("ライバルsp");
            names = (char(*)[16])((u8*)Punigp + 0x284);
        }

        frame = (s32)dat->field_0x1A4;
        if (dat->unk4 & 0x01000000) {
            (dat->field_0x124)++;
        } else {
            if (dat->unkC != 2) {
                frame = dat->field_0x116;
            } else {
                frame = frame / 8;
                if (dat->field_0x11C == 0) {
                    frame += dat->field_0x116 * 40;
                }
            }
            dat->field_0x124 = gp->retraceCount + frame;
        }

        if (!(dat->unk4 & 0x20000)) {
            animPoseSetAnim(npc->poseId, names[dat->field_0x11C], 1);
            animPoseSetLocalTime(npc->poseId, (f32) * (s32*)((u8*)dat + 0x124));
        }

        visible = 0;
        if (dat->unk4 & 0x400) {
            visible = 1;
        }
        if (dat->unk4 & 0x800) {
            visible = 0;
        }
        if (visible) {
            npc->position.x = dat->field_0x12C.x + dat->field_0x144;
            npc->position.y = dat->field_0x12C.y + dat->field_0x148;
            npc->position.z = dat->field_0x12C.z + dat->field_0x14C;
        } else {
            npc->position.x = 0.0f;
            npc->position.y = -1000.0f;
            npc->position.z = 0.0f;
        }
        pose->matrix[0][0][3] = 0.0f;
        pose->matrix[0][1][3] = -1000.0f;
        pose->matrix[0][2][3] = 0.0f;
        npc->scale.x = dat->field_0x18C;
        npc->scale.y = dat->field_0x18C;
        npc->direction = dat->field_0x180;
        npc->rotation.z = dat->field_0x184;
        npc->rotationOffset.y = dat->field_0x188;
    } else {
        MriPuniAnimSet* set;
        s32 count;
        s32 frame;
        s16* anim;
        s16 term;
        s32 idx;

        set = (MriPuniAnimSet*)((u8*)Punigp + 0x4);
        if (dat->unk4 & 0x8) {
            set = (MriPuniAnimSet*)((u8*)Punigp + 0x4);
        }
        if (dat->unk4 & 0x10) {
            set = (MriPuniAnimSet*)((u8*)Punigp + 0x54);
        }
        anim = set->unk0[dat->field_0x11C];
        count = set->unk28[dat->field_0x11C];
        term = set->unk3C[dat->field_0x11C];
        frame = (s32)dat->field_0x1A4;

        if (dat->unk4 & 0x01000000) {
            idx = dat->field_0x124 % count;
            if (term == -2 && count <= dat->field_0x124) {
                idx = count - 1;
            }
            pose->field_0x1C = anim[idx];
            (dat->field_0x124)++;
        } else {
            if (dat->unkC != 2) {
                frame = dat->field_0x116;
            } else {
                frame = frame / 8;
                if (dat->field_0x11C == 0) {
                    frame += dat->field_0x116 * (count / 3);
                }
            }
            idx = (u32)(gp->retraceCount + frame) % (u32)count;
            pose->field_0x1C = anim[idx];
        }

        visible = 0;
        if (dat->unk4 & 0x400) {
            visible = 1;
        }
        if (dat->unk4 & 0x800) {
            visible = 0;
        }
        if (visible) {
            pose->matrix[0][0][3] = dat->field_0x12C.x + dat->field_0x144;
            pose->matrix[0][1][3] = dat->field_0x12C.y + dat->field_0x148;
            pose->matrix[0][2][3] = dat->field_0x12C.z + dat->field_0x14C;
        } else {
            pose->matrix[0][0][3] = 0.0f;
            pose->matrix[0][1][3] = -1000.0f;
            pose->matrix[0][2][3] = 0.0f;
        }
        pose->field_0x14 = dat->field_0x18C;
        pose->field_0x4 = -dat->field_0x180;
        pose->field_0x8 = dat->field_0x184;
        pose->field_0xC = dat->field_0x188;
    }

    dat->field_0x11E = dat->field_0x11C;
}
f32 _chukanhosei(f32 a, f32 b, f32 c) {
    f32 t = (20.0f + b - a) / 20.0f;

    if (t > 0.5f) {
        t = (t - 0.5f) / 0.5f;
    } else {
        t = 0.0f;
    }
    return 10.0f * t + c;
}
s32 _areacheck(f32 x, f32 z, f32 range, f32 cx, f32 cz) {
    s32 ret = 1;

    if (x - range > cx) {
        ret = 0;
    }
    if (x + range < cx) {
        ret = 0;
    }
    if (z - range > cz) {
        ret = 0;
    }
    if (z + range < cz) {
        ret = 0;
    }
    return ret;
}
void _ymode_mri_09(PuniData* dat) {
    f32 z;

    if (dat->field_0x12C.x <= 400.0f && dat->field_0x12C.x > 200.0f && dat->field_0x12C.y <= 100.0f &&
        dat->field_0x12C.y >= 0.0f) {
        z = dat->field_0x12C.z;
        if (z <= -55.0f) {
            dat->field_0x12C.y = 100.0f;
        } else if (z <= -35.0f) {
            dat->field_0x12C.y = ymodeRamp(-35.0f, z, 90.0f);
        } else if (z <= -15.0f) {
            dat->field_0x12C.y = ymodeRamp(-15.0f, z, 80.0f);
        } else if (z <= 5.0f) {
            dat->field_0x12C.y = ymodeRamp(5.0f, z, 70.0f);
        } else if (z <= 25.0f) {
            dat->field_0x12C.y = ymodeRamp(25.0f, z, 60.0f);
        } else if (z <= 45.0f) {
            dat->field_0x12C.y = ymodeRamp(45.0f, z, 50.0f);
        } else if (z <= 65.0f) {
            dat->field_0x12C.y = ymodeRamp(65.0f, z, 40.0f);
        } else if (z <= 85.0f) {
            dat->field_0x12C.y = ymodeRamp(85.0f, z, 30.0f);
        } else if (z <= 105.0f) {
            dat->field_0x12C.y = ymodeRamp(105.0f, z, 20.0f);
        } else if (z <= 125.0f) {
            dat->field_0x12C.y = ymodeRamp(125.0f, z, 10.0f);
        } else if (z <= 145.0f) {
            dat->field_0x12C.y = ymodeRamp(145.0f, z, 0.0f);
        } else {
            dat->field_0x12C.y = 0.0f;
        }
    }

    if (dat->field_0x12C.x >= -170.0f && dat->field_0x12C.x <= 170.0f && dat->field_0x12C.z >= -80.0f &&
        dat->field_0x12C.z <= 140.0f) {
        dat->field_0x12C.y = 0.0f;
    }
}
#define PUNI_16_Y_INTERP(xx, thresh, base)                                                                             \
    do {                                                                                                               \
        f32 t = ((thresh) - (xx)) / 20.0f;                                                                             \
        if (t > 0.5f) {                                                                                                \
            t = (t - 0.5f) / 0.5f;                                                                                     \
        } else {                                                                                                       \
            t = 0.0f;                                                                                                  \
        }                                                                                                              \
        puni->field_0x12C.y = 10.0f * t + (base);                                                                      \
    } while (0)
void _ymode_mri_16_00(PuniData* puni) {
    u32 entry = *(u32*)((u8*)puni + 0x8); // PuniData 0x8
    u32 flags;
    f32 x, z;

    *(u32*)((u8*)puni + 0x8) = entry & ~0x8; // PuniData 0x8

    if (puni->field_0x12C.y <= -200.0f && puni->field_0x12C.y >= -400.0f) {
        x = puni->field_0x12C.x;
        if (x <= 40.0f && x >= -210.0f && (z = puni->field_0x12C.z) <= -76.46314f && z >= -200.0f) {
            if (x <= -190.0f) {
                puni->field_0x12C.y = -320.0f;
            } else if (x <= -170.0f) {
                PUNI_16_Y_INTERP(x, -170.0f, -330.0f);
            } else if (x <= -150.0f) {
                PUNI_16_Y_INTERP(x, -150.0f, -340.0f);
            } else if (x <= -130.0f) {
                PUNI_16_Y_INTERP(x, -130.0f, -350.0f);
            } else if (x <= -110.0f) {
                PUNI_16_Y_INTERP(x, -110.0f, -360.0f);
            } else if (x <= -90.0f) {
                PUNI_16_Y_INTERP(x, -90.0f, -370.0f);
            } else if (x <= -70.0f) {
                PUNI_16_Y_INTERP(x, -70.0f, -380.0f);
            } else if (x <= -50.0f) {
                PUNI_16_Y_INTERP(x, -50.0f, -390.0f);
            } else if (x <= -30.0f) {
                PUNI_16_Y_INTERP(x, -30.0f, -400.0f);
            } else {
                puni->field_0x12C.y = -400.0f;
            }
            if (puni->field_0x12C.y < -320.0f && puni->field_0x12C.y > -400.0f) {
                *(u32*)((u8*)puni + 0x8) |= 0x8; // PuniData 0x8
            }
        }
        if (puni->field_0x12C.x <= 40.0f && puni->field_0x12C.x >= -200.0f && puni->field_0x12C.z > -76.46314f &&
            puni->field_0x12C.z < 200.0f) {
            puni->field_0x12C.y = -400.0f;
        }
    }

    flags = *(u32*)((u8*)puni + 0x8); // PuniData 0x8
    if (flags & 0x4) {
        return;
    }
    if (flags & 0x8) {
        return;
    }
    if (!(entry & 0x8)) {
        return;
    }
    if (!(puni->field_0x12C.z > -76.46314f)) {
        return;
    }
    if (!(puni->field_0x138.z <= -76.46314f)) {
        return;
    }

    *(Vec*)&puni->field_0x12C = puni->field_0x138;
    puni->field_0x12C.z -= 0.0002f;
}
void _ymode_mri_16_01(PuniData* puni) {
    u32 entry = *(u32*)((u8*)puni + 0x8); // PuniData 0x8
    u32 flags;
    f32 x, z;

    *(u32*)((u8*)puni + 0x8) = entry & ~0x8; // PuniData 0x8

    if (puni->field_0x12C.y <= 100.0f && puni->field_0x12C.y >= 0.0f) {
        if (puni->field_0x12C.x <= 40.0f && puni->field_0x12C.x >= -200.0f && puni->field_0x12C.z > -200.0f &&
            puni->field_0x12C.z < 200.0f) {
            puni->field_0x12C.y = 0.0f;
        }
        x = puni->field_0x12C.x;
        if (x <= 40.0f && x >= -210.0f && (z = puni->field_0x12C.z) <= 60.00006f && z >= -10.00012f) {
            if (x <= -190.0f) {
                puni->field_0x12C.y = 80.0f;
            } else if (x <= -170.0f) {
                PUNI_16_Y_INTERP(x, -170.0f, 70.0f);
            } else if (x <= -150.0f) {
                PUNI_16_Y_INTERP(x, -150.0f, 60.0f);
            } else if (x <= -130.0f) {
                PUNI_16_Y_INTERP(x, -130.0f, 50.0f);
            } else if (x <= -110.0f) {
                PUNI_16_Y_INTERP(x, -110.0f, 40.0f);
            } else if (x <= -90.0f) {
                PUNI_16_Y_INTERP(x, -90.0f, 30.0f);
            } else if (x <= -70.0f) {
                PUNI_16_Y_INTERP(x, -70.0f, 20.0f);
            } else if (x <= -50.0f) {
                PUNI_16_Y_INTERP(x, -50.0f, 10.0f);
            } else if (x <= -30.0f) {
                f32 base = 0.0f;
                PUNI_16_Y_INTERP(x, -30.0f, base);
            } else {
                puni->field_0x12C.y = 0.0f;
            }
            if (puni->field_0x12C.y < 80.0f && puni->field_0x12C.y > 0.0f) {
                *(u32*)((u8*)puni + 0x8) |= 0x8; // PuniData 0x8
            }
        }
    }

    flags = *(u32*)((u8*)puni + 0x8); // PuniData 0x8
    if (flags & 0x4) {
        return;
    }
    if (flags & 0x8) {
        return;
    }
    if (!(entry & 0x8)) {
        return;
    }

    if (puni->field_0x12C.z > 60.00006f && puni->field_0x138.z <= 60.00006f) {
        *(Vec*)&puni->field_0x12C = puni->field_0x138;
        puni->field_0x12C.z -= 0.0002f;
    }
    if (puni->field_0x12C.z < -10.00012f && puni->field_0x138.z >= -10.00012f) {
        *(Vec*)&puni->field_0x12C = puni->field_0x138;
        puni->field_0x12C.z += 0.0002f;
    }
}
void puniSetYPosition(PuniData* puni) {
    if ((puni->unk4 & 1) == 0) {
        return;
    }

    switch (Punigp->field_0x33C) {
    case 0:
        break;
    case 1: {
        f32 x, z;
        s32 ok = 1;

        puni->field_0x12C.y = 0.0f;
        x = puni->field_0x12C.x;
        z = puni->field_0x12C.z;
        if (-49.0f > x) {
            ok = 0;
        }
        if (49.0f < x) {
            ok = 0;
        }
        if (-69.0f > z) {
            ok = 0;
        }
        if (29.0f < z) {
            ok = 0;
        }
        if (ok) {
            puni->field_0x12C.y = getMri02Yuka();
        }
        break;
    }
    case 2:
        _ymode_mri_09(puni);
        if (swGet(0xB23) == 0) {
            f32 x, z;
            s32 ok = 1;

            x = puni->field_0x12C.x;
            z = puni->field_0x12C.z;
            if (-35.0f > x) {
                ok = 0;
            }
            if (35.0f < x) {
                ok = 0;
            }
            if (40.0f > z) {
                ok = 0;
            }
            if (110.0f < z) {
                ok = 0;
            }
            if (ok) {
                puni->field_0x12C.y = 6.0f;
            }
        }
        break;
    case 3:
        puni->field_0x12C.y = 0.0f;
        if (swGet(0xB2A) == 0) {
            f32 x, z;
            s32 ok = 1;

            x = puni->field_0x12C.x;
            z = puni->field_0x12C.z;
            if (-80.0f > x) {
                ok = 0;
            }
            if (-10.0f < x) {
                ok = 0;
            }
            if (-99.0f > z) {
                ok = 0;
            }
            if (-29.0f < z) {
                ok = 0;
            }
            if (ok) {
                puni->field_0x12C.y = 6.0f;
            }
        }
        break;
    case 4:
        _ymode_mri_16_00(puni);
        break;
    case 5:
        _ymode_mri_16_01(puni);
        break;
    case 6:
        puni->field_0x12C.y = 0.0f;
        if (swGet(0xB37) == 0) {
            f32 x, z;
            s32 ok = 1;

            x = puni->field_0x12C.x;
            z = puni->field_0x12C.z;
            if (-35.0f > x) {
                ok = 0;
            }
            if (35.0f < x) {
                ok = 0;
            }
            if (-70.0f > z) {
                ok = 0;
            }
            if (0.0f < z) {
                ok = 0;
            }
            if (ok) {
                puni->field_0x12C.y = 6.0f;
            }
        }
        break;
    }

    switch (Punigp->field_0x33C) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        if (11.0 < __fabsf(puni->field_0x138.y - puni->field_0x12C.y)) {
            puni->field_0x12C.y = puni->field_0x138.y;
        }
        break;
    }
}
void puniMapdata_MRI_06_getidx(s32* outX, s32* outZ, f64 x, f64 z) {
    if (x <= -420.0) {
        *outX = -1;
    } else if (x <= -350.0) {
        *outX = 1;
    } else if (x <= -250.0) {
        *outX = 2;
    } else if (x <= -200.0) {
        *outX = 3;
    } else if (x <= -100.0) {
        *outX = 4;
    } else if (x <= -50.0) {
        *outX = 5;
    } else if (x <= 50.0) {
        *outX = 6;
    } else if (x <= 100.0) {
        *outX = 7;
    } else if (x <= 200.0) {
        *outX = 8;
    } else if (x <= 250.0) {
        *outX = 9;
    } else if (x <= 350.0) {
        *outX = 10;
    } else if (x <= 400.0) {
        *outX = 11;
    } else {
        *outX = -1;
    }

    if (z <= -150.0) {
        *outZ = -1;
    } else if (z <= -50.0) {
        *outZ = 1;
    } else if (z <= 110.0) {
        *outZ = 2;
    } else if (z <= 162.0) {
        *outZ = 3;
    } else {
        *outZ = -1;
    }
}
void puniMapdata_MRI_06_getpos(s32 idx, s32 sub, f32* outX, f32* outZ) {
    switch (idx) {
    case 1:
        *outX = -375.0f;
        break;
    case 2:
        *outX = -300.0f;
        break;
    case 3:
        *outX = -225.0f;
        break;
    case 4:
        *outX = -150.0f;
        break;
    case 5:
        *outX = -75.0f;
        break;
    case 6:
        *outX = -0.0f;
        break;
    case 7:
        *outX = 75.0f;
        break;
    case 8:
        *outX = 150.0f;
        break;
    case 9:
        *outX = 225.0f;
        break;
    case 10:
        *outX = 300.0f;
        break;
    case 11:
        *outX = 375.0f;
        break;
    default:
        *outX = -1000.0f;
        break;
    }

    switch (sub) {
    case 1:
        *outZ = -100.0f;
        break;
    case 2:
        *outZ = 30.0f;
        break;
    case 3:
        *outZ = 135.0f;
        break;
    default:
        *outZ = -1000.0f;
        break;
    }
}
void puniMapdata_MRI_09_getidx(s32* outX, s32* outZ, f64 x, f64 z) {
    if (x <= -300.0) {
        *outX = -1;
    } else if (x <= -200.0) {
        *outX = 1;
    } else if (x <= -90.0) {
        *outX = 2;
    } else if (x <= 90.0) {
        *outX = 3;
    } else if (x <= 200.0) {
        *outX = 4;
    } else if (x <= 320.0) {
        *outX = 5;
    } else {
        *outX = -1;
    }

    if (z <= -150.0) {
        *outZ = -1;
    } else if (z <= -100.0) {
        *outZ = 1;
    } else if (z <= 100.0) {
        *outZ = 2;
    } else if (z <= 120.0) {
        *outZ = 3;
    } else if (z <= 200.0) {
        *outZ = 4;
    } else {
        *outZ = -1;
    }
}
void puniMapdata_MRI_09_getpos(s32 idx, s32 sub, f32* outX, f32* outZ) {
    switch (idx) {
    case 1:
        *outX = -250.0f;
        break;
    case 2:
        *outX = -145.0f;
        break;
    case 3:
        *outX = 0.0f;
        break;
    case 4:
        *outX = 145.0f;
        break;
    case 5:
        *outX = 260.0f;
        break;
    default:
        *outX = -1000.0f;
        break;
    }

    switch (sub) {
    case 1:
        *outZ = -120.0f;
        break;
    case 2:
        *outZ = 0.0f;
        break;
    case 3:
        *outZ = 105.0f;
        break;
    case 4:
        *outZ = 160.0f;
        break;
    default:
        *outZ = -1000.0f;
        break;
    }
}
void puniMapdata_MRI_16_getidx(s32* out1, s32* out2, f32 x, f32 z) {
    if (x <= -350.0) {
        *out1 = -1;
    } else if (x <= -200.0) {
        *out1 = 1;
    } else if (x <= -150.0) {
        *out1 = 2;
    } else if (x <= -100.0) {
        *out1 = 3;
    } else if (x <= -50.0) {
        *out1 = 4;
    } else if (x <= 0.0) {
        *out1 = 5;
    } else if (x <= 50.0) {
        *out1 = 6;
    } else if (x <= 100.0) {
        *out1 = 7;
    } else if (x <= 150.0) {
        *out1 = 8;
    } else if (x <= 350.0) {
        *out1 = 9;
    } else {
        *out1 = -1;
    }

    if (z <= -150.0) {
        *out2 = -1;
    } else if (z <= -80.0) {
        *out2 = 1;
    } else if (z <= -10.0) {
        *out2 = 2;
    } else if (z <= 60.0) {
        *out2 = 3;
    } else if (z <= 90.0) {
        *out2 = 4;
    } else if (z <= 120.0) {
        *out2 = 5;
    } else if (z <= 150.0) {
        *out2 = 6;
    } else if (z <= 180.0) {
        *out2 = 7;
    } else {
        *out2 = -1;
    }
}
void puniMapdata_MRI_16_getpos(s32 a, s32 b, f32* outX, f32* outZ) {
    switch (a) {
    case 1:
        *outX = -220.0f;
        break;
    case 2:
        *outX = -175.0f;
        break;
    case 3:
        *outX = -125.0f;
        break;
    case 4:
        *outX = -75.0f;
        break;
    case 5:
        *outX = -25.0f;
        break;
    case 6:
        *outX = 25.0f;
        break;
    case 7:
        *outX = 75.0f;
        break;
    case 8:
        *outX = 125.0f;
        break;
    case 9:
        *outX = 170.0f;
        break;
    default:
        *outX = -1000.0f;
        break;
    }

    switch (b) {
    case 1:
        *outZ = -120.0f;
        break;
    case 2:
        *outZ = -45.0f;
        break;
    case 3:
        *outZ = 25.0f;
        break;
    case 4:
        *outZ = 75.0f;
        break;
    case 5:
        *outZ = 105.0f;
        break;
    case 6:
        *outZ = 135.0f;
        break;
    case 7:
        *outZ = 165.0f;
        break;
    default:
        *outZ = -1000.0f;
        break;
    }
}
void puniMapPos2Blk(s32* out1, s32* out2, f32 x, f32 z) {
    switch (Punigp->field_0x340) {
    case 0:
        break;
    case 1:
        puniMapdata_MRI_06_getidx(out1, out2, x, z);
        break;
    case 2:
        puniMapdata_MRI_09_getidx(out1, out2, x, z);
        break;
    case 3:
        puniMapdata_MRI_16_getidx(out1, out2, x, z);
        break;
    }
}
void puniMapBlk2Pos(s32 a, s32 b, f32* outX, f32* outZ) {
    switch (Punigp->field_0x340) {
    case 0:
        break;
    case 1:
        puniMapdata_MRI_06_getpos(a, b, outX, outZ);
        break;
    case 2:
        switch (a) {
        case 1:
            *outX = -250.0f;
            break;
        case 2:
            *outX = -145.0f;
            break;
        case 3:
            *outX = 0.0f;
            break;
        case 4:
            *outX = 145.0f;
            break;
        case 5:
            *outX = 260.0f;
            break;
        default:
            *outX = -1000.0f;
            break;
        }
        switch (b) {
        case 1:
            *outZ = -120.0f;
            break;
        case 2:
            *outZ = 0.0f;
            break;
        case 3:
            *outZ = 105.0f;
            break;
        case 4:
            *outZ = 160.0f;
            break;
        default:
            *outZ = -1000.0f;
            break;
        }
        break;
    case 3:
        puniMapdata_MRI_16_getpos(a, b, outX, outZ);
        break;
    }
}
u16 puniMapGetInfo(s32 x, s32 y) {
    PuniMap* map = Punimap;
    u32 info = map->field_0x20[y * map->field_0x8 + x];

    if ((info & 0xFF) == 0)
        return info | 0xFFFF;
    return info;
}
void puniMapInit(void) {
    Punimap = _mapAlloc(0x24);
    memset(Punimap, 0, 0x24);
}
void puniMapDataSet(s32 w, s32 h, f32 a, f32 b, void* data) {
    Punimap->field_0x0 = a;
    Punimap->field_0x4 = b;
    if (data == NULL) {
        Punimap->field_0x14 = 0;
    } else {
        Punimap->field_0x8 = w;
        Punimap->field_0xC = h;
        Punimap->field_0x14 = 1;
        Punimap->field_0x18 = data;
        Punimap->field_0x1C = 0;
        Punimap->field_0x10 = w * 4 * h;
        Punimap->field_0x20 = _mapAlloc(Punimap->field_0x10);
        memcpy(Punimap->field_0x18, data, Punimap->field_0x10);
    }
}
// NONMATCHING (80.93%): switch + hoisted pos x/z + `q` neighbour pointer match;
// the loop's register allocation differs (target keeps a byte index and uses
// lhzx, we recompute the address) as does the saved-register set.
void puniMapFindbuff(Vec* pos) {
    s32 out1;
    s32 out2;
    u16 id;
    s32 changed;
    s32 i;
    s32 total;

    if (Punimap->field_0x14 == 0)
        return;
    if (Punimap->field_0x0 < pos->y || Punimap->field_0x4 > pos->y) {
        Punimap->field_0x1C = 0;
        return;
    }
    memcpy(Punimap->field_0x20, Punimap->field_0x18, Punimap->field_0x10);
    Punimap->field_0x1C = 1;
    {
        f32 x = pos->x;
        f32 z = pos->z;

        switch (Punigp->field_0x340) {
        case 0:
            break;
        case 1:
            puniMapdata_MRI_06_getidx(&out1, &out2, x, z);
            break;
        case 2:
            puniMapdata_MRI_09_getidx(&out1, &out2, x, z);
            break;
        case 3:
            puniMapdata_MRI_16_getidx(&out1, &out2, x, z);
            break;
        }
    }
    {
        s32 index = out1 + out2 * Punimap->field_0x8;

        if (index < 0)
            Punimap->field_0x1C = 0;
        if (Punimap->field_0x1C == 0)
            return;
        Punimap->field_0x20[index] |= 1;
    }
    id = 1;
    while (1) {
        changed = 0;
        total = Punimap->field_0x8 * Punimap->field_0xC;
        for (i = 0; i < total; i++) {
            u16* buf = Punimap->field_0x20;
            u16* q = &buf[i];
            u16 cur = buf[i];
            s32 hi;

            if (cur == 0xFFFF)
                continue;
            if ((cur & 0xFFF) != id)
                continue;
            {
                s32 w = Punimap->field_0x8;

                hi = cur & 0xF000;
                {
                    u16 nb = q[-w];
                    u8 low = nb & 0xFF;

                    if (low == 0) {
                        if (low < id) {
                            if (abs((nb & 0xF000) - hi) <= 0x1000) {
                                q[-w] = nb | (id + 1);
                                changed = 1;
                            }
                        }
                    }
                }
                {
                    u16 nb = q[w];
                    u8 low = nb & 0xFF;

                    if (low == 0) {
                        if (low < id) {
                            if (abs((nb & 0xF000) - hi) <= 0x1000) {
                                q[w] = nb | (id + 1);
                                changed = 1;
                            }
                        }
                    }
                }
                {
                    u16 nb = q[-1];
                    u8 low = nb & 0xFF;

                    if (low == 0) {
                        if (low < id) {
                            if (abs((nb & 0xF000) - hi) <= 0x1000) {
                                q[-1] = nb | (id + 1);
                                changed = 1;
                            }
                        }
                    }
                }
                {
                    u16 nb = q[1];
                    u8 low = nb & 0xFF;

                    if (low == 0) {
                        if (low < id) {
                            if (abs((nb & 0xF000) - hi) <= 0x1000) {
                                q[1] = nb | (id + 1);
                                changed = 1;
                            }
                        }
                    }
                }
            }
        }
        if (changed == 0)
            break;
        id++;
    }

}
void puniTargetSearch(PuniData* punidat) {
    s32 i;
    PuniTarget* target = (PuniTarget*)((u8*)Punigp + 0x374); // PuniWork 0x374, 10 entries of 0x14


    punidat->field_0x1B4 = -1;
    punidat->field_0x1B8 = 1e+06f;
    punidat->field_0x1BC = -1;
    punidat->field_0x1C0 = 1e+06f;
    for (i = 0; i < 10; i++, target++) {
        if (target->flags & 1) {
            f64 limit = 32.0;

            if (limit < __fabs(punidat->field_0x12C.y - target->position.y))
                continue;
            if (target->flags & 4) {
                f32 dist = PSVECSquareDistance(&punidat->field_0x12C, &target->position);
                if (punidat->field_0x1C0 > dist) {
                    punidat->field_0x1BC = i;
                    punidat->field_0x1C0 = dist;
                }
            }
            if (target->flags & 2) {
                f32 dist = PSVECSquareDistance(&punidat->field_0x12C, &target->position);
                if (punidat->field_0x1B8 > dist) {
                    punidat->field_0x1B4 = i;
                    punidat->field_0x1B8 = dist;
                }
            }
        }
    }
}
void puniswGetindex(u32 base, s32 index, s32* out, Vec* pos) {
    u8 b0, b1, b2, b3, b4, b5, b6;
    u32 off;

    off = base + index * 7;
    b0 = _swByteGet(off + 0x8F0D180);
    b1 = _swByteGet(off + 0x8F0D181);
    b2 = _swByteGet(off + 0x8F0D182);
    b3 = _swByteGet(off + 0x8F0D183);
    b4 = _swByteGet(off + 0x8F0D184);
    b5 = _swByteGet(off + 0x8F0D185);
    b6 = _swByteGet(off + 0x8F0D186);

    *out = b0;
    {
        s32 x = b1 + (b2 << 8);

        if (x & 0x8000)
            x |= 0xFFFF0000;
        {
            s32 y = b3 + (b4 << 8);

            if (y & 0x8000)
                y |= 0xFFFF0000;
            {
                s32 z = b5 + (b6 << 8);

                if (z & 0x8000)
                    z |= 0xFFFF0000;
                pos->x = x;
                pos->y = y;
                pos->z = z;
            }
        }
    }
}
// NONMATCHING (88.5%): register allocation and scheduling differ.
void puniswSetindex(u32 base, s32 index, s32* out, Vec* pos) {
    s32 x = (s32)pos->x;
    s32 y = (s32)pos->y;
    s32 z = (s32)pos->z;
    s32 xh = (x >> 8) & 0xFF;
    s32 yh = (y >> 8) & 0xFF;
    s32 zh = (z >> 8) & 0xFF;
    u32 off = base + index * 7;

    _swByteSet(off + 0x8F0D180, *out);
    _swByteSet(off + 0x8F0D181, x);
    _swByteSet(off + 0x8F0D182, xh);
    _swByteSet(off + 0x8F0D183, y);
    _swByteSet(off + 0x8F0D184, yh);
    _swByteSet(off + 0x8F0D185, z);
    _swByteSet(off + 0x8F0D186, zh);
}
void puniswTrapGetindex(s32 index, s32* out) {
    s32 byte;

    if (index >= 0x67) {
        *out = 0;
    } else {
        byte = swByteGet(index / 8 + 0x28C);
        *out = (byte >> (index % 8)) & 1;
    }
}
void puniswTrapSetindex(s32 index, s32* value) {
    if (index < 0x67) {
        s32 byteIndex = index / 8;
        s32 bit = index % 8;
        s32 byte = swByteGet(byteIndex + 0x28C);

        byte = byte & ((1 << bit) ^ 0xFF);
        byte = byte | (*value << bit);
        swByteSet(byteIndex + 0x28C, (u8)byte);
    }
}
static f32 randf(f32 min, f32 max) {
    f32 q = (f32)rand() / 32767.0f;

    return q * (max - min) + min;
}
s32 randi(s32 min, s32 max) {
    s32 r = rand() % (max - min + 1);

    return r + min;
}
void qrndseed(s32 seed) {
    Punigp->field_0x450 = seed + 0x3A3B01F9;
    Punigp->field_0x454 = 0x21708331;
}
s32 qrnd(s32 max) {
    s32 v;

    Punigp->field_0x450 += Punigp->field_0x454 + 0x17340000 - 0x506F;
    Punigp->field_0x454 += Punigp->field_0x450 + 0x21270000 + 0x3F9C;
    v = Punigp->field_0x454;
#ifdef MUST_MATCH
    // the cast keeps MWCC from folding this into its branchless abs idiom
    if (v < 0) {
        v = -(s32)(u32)v;
    }
#else
    if (v < 0) {
        v = -v;
    }
#endif
    return v % max;
}
void puniDeleteEffect(PuniData* puni) {
    EffectEntry* effect = (EffectEntry*)puni->field_0x218;

    if (effect != NULL) {
        effDelete(effect);
        puni->field_0x218 = 0;
    }
}
void _daime_change(PuniData* puni) {
    if ((PuniData*)puni->field_0x1D4 != NULL) {
        ((PuniData*)puni->field_0x1D4)->field_0x11A--;
        _daime_change((PuniData*)puni->field_0x1D4);
    }
    if ((PuniData*)puni->field_0x1D8 != NULL) {
        ((PuniData*)puni->field_0x1D8)->field_0x11A--;
        _daime_change((PuniData*)puni->field_0x1D8);
    }
}
void PuniDokuritsu(PuniData* puni) {
    PuniData* left;
    PuniData* right;
    PuniData* parent;

    puni->field_0x118 = -1;
    puni->unk4 &= ~0x00080000;
    puni->field_0x11A = 0;
    left = (PuniData*)puni->field_0x1D4;
    parent = (PuniData*)puni->field_0x1D0;
    right = (PuniData*)puni->field_0x1D8;
    puni->field_0x1D0 = 0;
    puni->field_0x1D4 = 0;
    puni->field_0x1D8 = 0;

    if (left != NULL) {
        left->field_0x11A--;
        _daime_change(left);
        left->field_0x1D0 = (s32)parent;
        if ((PuniData*)left->field_0x1D0 != NULL) {
            if ((PuniData*)parent->field_0x1D4 == puni) {
                parent->field_0x1D4 = (s32)left;
            } else if ((PuniData*)parent->field_0x1D8 == puni) {
                parent->field_0x1D8 = (s32)left;
            }
        }
        left->field_0x1D8 = (s32)right;
        if ((PuniData*)left->field_0x1D8 != NULL) {
            ((PuniData*)left->field_0x1D8)->field_0x1D0 = (s32)left;
        }
    } else if (right != NULL) {
        right->field_0x11A--;
        _daime_change(right);
        right->field_0x1D0 = (s32)parent;
        if ((PuniData*)right->field_0x1D0 != NULL) {
            if ((PuniData*)parent->field_0x1D4 == puni) {
                parent->field_0x1D4 = (s32)right;
            } else if ((PuniData*)parent->field_0x1D8 == puni) {
                parent->field_0x1D8 = (s32)right;
            }
        }
        right->field_0x1D4 = (s32)left;
        if ((PuniData*)right->field_0x1D4 != NULL) {
            ((PuniData*)right->field_0x1D4)->field_0x1D0 = (s32)right;
        }
    } else if (parent != NULL) {
        if ((PuniData*)parent->field_0x1D4 == puni) {
            parent->field_0x1D4 = 0;
        } else if ((PuniData*)parent->field_0x1D8 == puni) {
            parent->field_0x1D8 = 0;
        }
    }
}
PuniData* PuniSakanobori(PuniData* puni) {
    PuniData* next;

    while ((next = (PuniData*)puni->field_0x1D0) != NULL) {
        puni = next;
    }
    return puni;
}

// NONMATCHING (97.87%): the inlined recursion's register colouring is one register lower (we use r27-r31,
// the target r28-r31); the emitted instructions are otherwise identical.
void PuniChildHagure(PuniData* puni) {
    PuniData* child = (PuniData*)puni->field_0x1D4;

    if (child != NULL) {
        child->unkC = 0x10;
        puni->field_0x114 = 0;
        PuniChildHagure(child);
        PuniDokuritsu(child);
    }
    child = (PuniData*)puni->field_0x1D8;
    if (child != NULL) {
        child->unkC = 0x10;
        puni->field_0x114 = 0;
        PuniChildHagure(child);
        PuniDokuritsu(child);
    }
}
s32 searchParent(PuniData* puni) {
    s32 best = 10000;
    PuniData* parent;
    s32 i;
    PuniData* p;

    p = Punidat;
    for (i = 0; i < PUNI_DATA_MAX; i++, p++) {
        if (!(p->unk4 & 0x00080000) || (PuniData*)p->field_0x1D8 != NULL) {
            continue;
        }
        if ((PuniData*)p->field_0x1D4 != NULL && p->field_0x11A >= 2) {
            continue;
        }
        if (p == puni) {
            continue;
        }
        if ((s16)best <= p->field_0x11A) {
            continue;
        }
        parent = p;
        best = p->field_0x11A + 1;
    }
    if ((s16)best == 10000) {
        return 0;
    }
    puni->field_0x1D0 = (s32)parent;
    puni->field_0x11A = best;
    puni->field_0x118 = parent->field_0x118;
    if ((PuniData*)parent->field_0x1D4 == NULL) {
        parent->field_0x1D4 = (s32)puni;
    } else {
        parent->field_0x1D8 = (s32)puni;
    }
    return 1;
}
s32 searchMario(PuniData* puni) {
    return puni->field_0x1A4 < 150.0f;
}
s32 puniPositionCheck(PuniData* puni) {
    f32 top;
    f32 bottom;
    Vec pos;
    Vec normal;
    f32 hitZ;
    f32 hitY;
    f32 hitX;
    f32 hitNZ;
    f32 hitNY;
    f32 hitNX;
    f32 distance;
    s32 inBand = 0;
    PuniWork* gp = Punigp;

    top = gp->field_0x350;
    bottom = gp->field_0x354;
    if (-1000.0f != top && puni->field_0x12C.y < top && puni->field_0x12C.y > bottom) {
        inBand = 1;
    }
    top = gp->field_0x358;
    bottom = gp->field_0x35C;
    if (-1000.0f != top && puni->field_0x12C.y < top && puni->field_0x12C.y > bottom) {
        inBand = 1;
    }
    if (inBand == 0) {
        return 1;
    }
    pos = puni->field_0x12C;
    pos.y += 1.0f;
    normal = (Vec){0.0f, -1.0f, 0.0f};
    distance = 3.0f;
    return hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, pos.x, pos.y, pos.z, normal.x,
                          normal.y, normal.z) == NULL
               ? 2
               : 0;
}
static f32 mriRandf(f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;
    f32 d = max - min;

    return r * d + min;
}
void puniPositionRecover(PuniData* puni) {
    puni->field_0x12C = *(Vec*)&Punigp->field_0x344;

    puni->field_0x12C.x += mriRandf(0.0f, 2.0f) - 1.0f;
    puni->field_0x12C.z += mriRandf(0.0f, 2.0f) - 1.0f;

    *(Vec*)&puni->field_0x138 = puni->field_0x12C;
}
s32 puniMoveAtari(PuniData* punidat, Vec pos, Vec vel, s32 arg3, f32 mag) {
    Vec hit;
    Vec normal;
    Vec dir;
    Vec start;
    Vec reflect;
    f32 hit2x, hit2y, hit2z;
    f32 normal2x, normal2y, normal2z;
    f32 dist;
    s32 result = 1;

    punidat->field_0x214 = NULL;
    punidat->field_0x210 = NULL;
    dist = mag;

    if (punidat->unk4 & 0x1000) {
        punidat->field_0x210 =
            hitCheckFilter(0, &hit.x, &hit.y, &hit.z, &dist, &normal.x, &normal.y, &normal.z, pos.x, pos.y,
                                pos.z, vel.x, vel.y, vel.z);
    }
    if (punidat->field_0x210 == NULL) {
        PSVECAdd(&punidat->field_0x12C, &punidat->field_0x15C, &punidat->field_0x12C);
        goto done;
    }
    start = punidat->field_0x12C;
    punidat->field_0x560 = normal;
    PSVECAdd(&punidat->field_0x12C, &punidat->field_0x15C, &punidat->field_0x12C);
    if (0.0f == punidat->field_0x15C.y) {
        normal.y = 0.0f;
    }
    PSVECNormalize(&normal, &normal);
    if (arg3 != 0) {
        C_VECReflect(&vel, &normal, &reflect);
        PSVECScale(&reflect, &punidat->field_0x15C, mag);
        result = 0;
    }
    {
        f32 dot = PSVECDotProduct(&vel, &normal);

        PSVECScale(&normal, &normal, dot * -(mag - dist) + 0.1f);
    }
    PSVECAdd(&punidat->field_0x12C, &normal, &punidat->field_0x12C);
    PSVECSubtract(&punidat->field_0x12C, &start, &dir);
    dist = PSVECMag(&dir);
    if (dist > 0.0f) {
        PSVECNormalize(&dir, &dir);
        pos = start;
        pos.y += 1.0f;
        punidat->field_0x214 =
            hitCheckFilter(0, &hit2x, &hit2y, &hit2z, &dist, &normal2x, &normal2y, &normal2z, start.x,
                                start.y + 1.0f, start.z, dir.x, dir.y, dir.z);
        if (punidat->field_0x214 != NULL) {
            punidat->field_0x12C = start;
        }
    }
done:
    return result;
}

// NONMATCHING (96.04%): only the prologue's register scheduling differs (the target loads pos.x/y/z
// directly from r4 while we copy the pointer to r3 before loading pos.z); the rest matches.
void unk_mri_000095f0(PuniData* puni, Vec pos, Vec vel, f32 mag) {
    Vec hit;
    Vec normal;
    Vec dir;
    Vec start;
    f32 hit2x, hit2y, hit2z;
    f32 normal2x, normal2y, normal2z;
    f32 dist;

    puni->field_0x214 = NULL;
    dist = mag;
    puni->field_0x210 = NULL;

    puni->field_0x210 =
        hitCheckFilter(0, &hit.x, &hit.y, &hit.z, &dist, &normal.x, &normal.y, &normal.z, pos.x, pos.y, pos.z,
                       vel.x, vel.y, vel.z);
    if (puni->field_0x210 == NULL) {
        PSVECAdd(&puni->field_0x12C, &puni->field_0x15C, &puni->field_0x12C);
        return;
    }
    if (Punigp->unk328 == 0x12) {
        return;
    }
    start = puni->field_0x12C;
    puni->field_0x560 = normal;
    PSVECAdd(&puni->field_0x12C, &puni->field_0x15C, &puni->field_0x12C);
    if (0.0f == puni->field_0x15C.y) {
        normal.y = 0.0f;
    }
    PSVECNormalize(&normal, &normal);
    {
        f32 dot = PSVECDotProduct(&vel, &normal);

        PSVECScale(&normal, &normal, dot * -(mag - dist) + 0.1f);
    }
    PSVECAdd(&puni->field_0x12C, &normal, &puni->field_0x12C);
    PSVECSubtract(&puni->field_0x12C, &start, &dir);
    dist = PSVECMag(&dir);
    if (dist > 0.0f) {
        PSVECNormalize(&dir, &dir);
        puni->field_0x214 =
            hitCheckFilter(0, &hit2x, &hit2y, &hit2z, &dist, &normal2x, &normal2y, &normal2z, start.x,
                           start.y + 1.0f, start.z, dir.x, dir.y, dir.z);
        if (puni->field_0x214 != NULL) {
            puni->field_0x12C = start;
        }
    }
}

// NONMATCHING (85.82%): the `*pos = saved; pos->y += 1` copy and the second hitCheckFilter argument
// setup schedule differently (the target forwards saved.x/y/z, we reload through pos); the rest matches.
void unk_mri_0000980c(PuniData* puni, Vec* pos, Vec* dir, f32 mag) {
    Vec hitP;
    Vec hitN;
    Vec diff;
    Vec refl;
    Vec saved;
    f32 hitP2x, hitP2y, hitP2z;
    f32 hitN2x, hitN2y, hitN2z;
    f32 dist;
    f32 scale;

    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dist = mag;
    if (puni->unk4 & 0x1000) {
        puni->field_0x210 = hitCheckFilter(0, &hitP.x, &hitP.y, &hitP.z, &dist, &hitN.x,
                                                         &hitN.y, &hitN.z, pos->x, pos->y, pos->z, dir->x,
                                                         dir->y, dir->z);
    }
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->field_0x12C, (Vec*)&puni->field_0x15C, &puni->field_0x12C);
    } else {
        saved = puni->field_0x12C;
        puni->field_0x560 = hitN;
        PSVECAdd(&puni->field_0x12C, (Vec*)&puni->field_0x15C, &puni->field_0x12C);
        if (puni->field_0x15C.y == 0.0f) {
            hitN.y = 0.0f;
        }
        PSVECNormalize(&hitN, &hitN);
        C_VECReflect(dir, &hitN, &refl);
        PSVECScale(&refl, (Vec*)&puni->field_0x15C, mag);
        scale = PSVECDotProduct(dir, &hitN);
        scale = scale * -(mag - dist) + 0.1f;
        PSVECScale(&hitN, &hitN, scale);
        PSVECAdd(&puni->field_0x12C, &hitN, &puni->field_0x12C);
        PSVECSubtract(&puni->field_0x12C, &saved, &diff);
        dist = PSVECMag(&diff);
        if (dist > 0.0f) {
            PSVECNormalize(&diff, &diff);
            *pos = saved;
            pos->y += 1.0f;
            puni->field_0x214 =
                hitCheckFilter(0, &hitP2x, &hitP2y, &hitP2z, &dist, &hitN2x, &hitN2y, &hitN2z,
                                    saved.x, saved.y + 1.0f, saved.z, diff.x, diff.y, diff.z);
            if (puni->field_0x214 != 0) {
                puni->field_0x12C = saved;
            }
        }
    }

}

// NONMATCHING (95.85%): the stack frame is 0x10 smaller (the target keeps an extra Vec temp for
// `up = puni->field_0x12C; up.y += 1.0f;`); every instruction otherwise lines up after the shift.
// Declaring `tmp` before the other Vecs put it at the target's 0x78 slot (+0.07%).
void puniMoveDecel(PuniData* puni) {
    f32 mag;
    Vec tmp;
    Vec up;
    Vec dir;
    Vec hitN;
    Vec hitP;
    Vec saved;
    Vec refl;
    Vec diff;
    Vec hitN2;
    Vec hitP2;
    f32 dist;
    f32 scale;

    mag = PSVECMag(&puni->field_0x15C);

    if (mag == 0.0f) {
        return;
    }
    if (mag > 1.0f) {
        if (puni->field_0x15C.x < 0.0f) {
            puni->field_0x180 = 270.0f;
        } else {
            puni->field_0x180 = 90.0f;
        }
    }
    if (mag > puni->field_0x174) {
        PSVECScale(&puni->field_0x15C, &puni->field_0x15C, puni->field_0x174 / mag);
        mag = puni->field_0x174;
    }

    up = puni->field_0x12C;
    up.y += 1.0f;
    PSVECNormalize(&puni->field_0x15C, &tmp);
    dir = tmp;

    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dist = mag;
    if (puni->unk4 & 0x1000) {
        puni->field_0x210 =
            hitCheckFilter(0, &hitP.x, &hitP.y, &hitP.z, &dist, &hitN.x, &hitN.y, &hitN.z, up.x, up.y,
                                up.z, dir.x, dir.y, dir.z);
    }
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->field_0x12C, &puni->field_0x15C, &puni->field_0x12C);
    } else {
        saved = puni->field_0x12C;
        puni->field_0x560 = hitN;
        PSVECAdd(&puni->field_0x12C, &puni->field_0x15C, &puni->field_0x12C);
        if (puni->field_0x15C.y == 0.0f) {
            hitN.y = 0.0f;
        }
        PSVECNormalize(&hitN, &hitN);
        C_VECReflect(&dir, &hitN, &refl);
        PSVECScale(&refl, &puni->field_0x15C, mag);
        scale = PSVECDotProduct(&dir, &hitN);
        scale = scale * -(mag - dist) + 0.1f;
        PSVECScale(&hitN, &hitN, scale);
        PSVECAdd(&puni->field_0x12C, &hitN, &puni->field_0x12C);
        PSVECSubtract(&puni->field_0x12C, &saved, &diff);
        dist = PSVECMag(&diff);
        if (dist > 0.0f) {
            PSVECNormalize(&diff, &diff);
            puni->field_0x214 =
                hitCheckFilter(0, &hitP2.x, &hitP2.y, &hitP2.z, &dist, &hitN2.x, &hitN2.y, &hitN2.z,
                                    saved.x, saved.y + 1.0f, saved.z, diff.x, diff.y, diff.z);
            if (puni->field_0x214 != 0) {
                puni->field_0x12C = saved;
            }
        }
    }
    if (puni->field_0x210 != 0 || puni->field_0x214 != 0) {
        puni->unk4 |= 0x00400000;
    } else {
        PSVECScale(&puni->field_0x15C, &puni->field_0x15C, 0.6f);
        puni->unk4 &= ~0x00400000;
    }
}

// NONMATCHING (86.92%): register allocation and scheduling differ.
void puniMoveLimitedNormal(PuniData* puni) {
    f32 mag = PSVECMag(&puni->field_0x15C);
    Vec up;
    Vec dir;
    Vec hitN;
    Vec hitP;
    Vec saved;
    Vec diff;
    Vec hitN2;
    Vec hitP2;
    f32 dist;
    f32 scale;

    if (mag == 0.0f) {
        return;
    }
    if (mag > puni->field_0x174) {
        PSVECScale(&puni->field_0x15C, &puni->field_0x15C, puni->field_0x174 / mag);
        mag = puni->field_0x174;
    }

    up = puni->field_0x12C;
    up.y += 1.0f;
    PSVECNormalize(&puni->field_0x15C, &dir);

    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dist = mag;
    puni->field_0x210 =
        hitCheckFilter(0, &hitP.x, &hitP.y, &hitP.z, &dist, &hitN.x, &hitN.y, &hitN.z, up.x, up.y,
                            up.z, dir.x, dir.y, dir.z);
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->field_0x12C, &puni->field_0x15C, &puni->field_0x12C);
    } else if (Punigp->unk328 != 0x12) {
        saved = puni->field_0x12C;
        puni->field_0x560 = hitN;
        PSVECAdd(&puni->field_0x12C, &puni->field_0x15C, &puni->field_0x12C);
        if (puni->field_0x15C.y == 0.0f) {
            hitN.y = 0.0f;
        }
        PSVECNormalize(&hitN, &hitN);
        scale = PSVECDotProduct(&dir, &hitN);
        scale = scale * -(mag - dist) + 0.1f;
        PSVECScale(&hitN, &hitN, scale);
        PSVECAdd(&puni->field_0x12C, &hitN, &puni->field_0x12C);
        PSVECSubtract(&puni->field_0x12C, &saved, &diff);
        dist = PSVECMag(&diff);
        if (dist > 0.0f) {
            PSVECNormalize(&diff, &diff);
            puni->field_0x214 =
                hitCheckFilter(0, &hitP2.x, &hitP2.y, &hitP2.z, &dist, &hitN2.x, &hitN2.y, &hitN2.z,
                                    saved.x, saved.y + 1.0f, saved.z, diff.x, diff.y, diff.z);
            if (puni->field_0x214 != 0) {
                puni->field_0x12C = saved;
            }
        }
    }
    if (puni->field_0x210 != 0 || puni->field_0x214 != 0) {
        puni->unk4 |= 0x00400000;
    } else {
        puni->unk4 &= ~0x00400000;
    }
}

// NONMATCHING (89.87%): control flow and calls match; the stack frame is 0x10 smaller
// (the original keeps an extra Vec copy of `up`/`dir`) and the stack slots are ordered
// differently. Declaring `tmp` first puts it at the target's 0x78 slot (+0.08%).
void puniMoveNormal(PuniData* puni) {
    f32 mag = PSVECMag((Vec*)&puni->field_0x15C);
    Vec tmp;
    Vec up;
    Vec dir;
    Vec hitN;
    Vec hitP;
    Vec saved;
    Vec refl;
    Vec diff;
    Vec hitN2;
    Vec hitP2;
    f32 dist;
    f32 scale;
    f32 moved;

    if (mag == 0.0f) {
        return;
    }

    up = puni->field_0x12C;
    up.y += 1.0f;
    tmp = *(Vec*)&puni->field_0x15C;
    PSVECNormalize(&tmp, &tmp);
    dir = tmp;

    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dist = mag;
    if (puni->unk4 & 0x1000) {
        puni->field_0x210 =
            hitCheckFilter(0, &hitP.x, &hitP.y, &hitP.z, &dist, &hitN.x, &hitN.y, &hitN.z, up.x, up.y,
                                up.z, dir.x, dir.y, dir.z);
    }
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->field_0x12C, (Vec*)&puni->field_0x15C, &puni->field_0x12C);
    } else {
        saved = puni->field_0x12C;
        puni->field_0x560 = hitN;
        PSVECAdd(&puni->field_0x12C, (Vec*)&puni->field_0x15C, &puni->field_0x12C);
        if (((Vec*)&puni->field_0x15C)->y == 0.0f) {
            hitN.y = 0.0f;
        }
        PSVECNormalize(&hitN, &hitN);
        C_VECReflect(&dir, &hitN, &refl);
        PSVECScale(&refl, (Vec*)&puni->field_0x15C, mag);
        scale = PSVECDotProduct(&dir, &hitN);
        scale = scale * -(mag - dist) + 0.1f;
        PSVECScale(&hitN, &hitN, scale);
        PSVECAdd(&puni->field_0x12C, &hitN, &puni->field_0x12C);
        PSVECSubtract(&puni->field_0x12C, &saved, &diff);
        moved = PSVECMag(&diff);
        dist = moved;
        if (moved > 0.0f) {
            PSVECNormalize(&diff, &diff);
            puni->field_0x214 =
                hitCheckFilter(0, &hitP2.x, &hitP2.y, &hitP2.z, &dist, &hitN2.x, &hitN2.y, &hitN2.z,
                                    saved.x, saved.y + 1.0f, saved.z, diff.x, diff.y, diff.z);
            if (puni->field_0x214 != 0) {
                puni->field_0x12C = saved;
            }
        }
    }
    if (puni->field_0x210 != 0 || puni->field_0x214 != 0) {
        puni->unk4 |= 0x00400000;
    } else {
        puni->unk4 &= ~0x00400000;
    }
}
void puniMoveSimple(PuniData* puni) {
    f32 mag = PSVECMag((Vec*)&puni->field_0x15C);
#ifdef MUST_MATCH
    // named zero forces the original's fcmpu operand order (f1=mag, f0=zero)
    f32 zero = 0.0f;

    if (mag != zero) {
#else
    if (mag != 0.0f) {
#endif
        PSVECAdd(&puni->field_0x12C, (Vec*)&puni->field_0x15C, &puni->field_0x12C);
    }
}
void puniMoveCustom(PuniData* puni) {
    void (*move)(PuniData*) = (void (*)(PuniData*))puni->field_0x570;

    if (move != NULL) {
        move(puni);
    }
}

// NONMATCHING (98.98%): the branchless qrnd compare, the `||` combine, both action branches
// and the chance/span `mr` match; only the inlined-qrnd temporaries pick r6/r7 instead of
// the target's r7/r8 (chance and span swap one register).
void puniParaAttack(PuniData* puni, PuniData* other) {
    s16 chance;
    s16 span;
    s16 base;
    s32 flag;

    if (other->field_0x586 == 0 && other->unkC == 9) {
        return;
    }

#ifdef MUST_MATCH
    // `base` makes MWCC keep the original's mr chance<-base before the add
    if (other->unkC != 8) {
        base = puni->field_0x588;
        chance = base;
        span = base + other->field_0x588;
    } else {
        base = puni->field_0x58A;
        chance = base;
        span = base + (100 - other->field_0x58A);
    }
#else
    if (other->unkC != 8) {
        chance = puni->field_0x588;
        span = chance + other->field_0x588;
    } else {
        chance = puni->field_0x58A;
        span = chance + (100 - other->field_0x58A);
    }
#endif

#ifdef MUST_MATCH
    // splitting the `||` makes MWCC keep the original's branchless compare + li form
    flag = chance > (s16)puni_rnd(span);
    if (other->unk4 & 0x08000000) {
        flag = 1;
    }
    if (flag) {
#else
    if (chance > (s16)puni_rnd(span) || (other->unk4 & 0x08000000)) {
#endif
        if (puni->field_0x586 == 0) {
            other->field_0x584 = puni->field_0x57E - other->field_0x580;
            other->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
            other->field_0x1DC = (s32)puni;
            other->unk4 |= 0x2000;
            other->unk4 &= ~0x08000000;
            other->field_0x8 &= ~0x2;
            other->unkC = 9;
            other->field_0x114 = 0;
            other->unk4 &= ~0x02000000;
        }
    } else if (other->field_0x586 == 0) {
        puni->field_0x584 = other->field_0x57E - puni->field_0x580;
        puni->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
        puni->field_0x1DC = (s32)other;
        puni->unk4 |= 0x2000;
        puni->unk4 &= ~0x08000000;
        puni->field_0x8 &= ~0x2;
        puni->unkC = 9;
        puni->field_0x114 = 0;
        puni->unk4 &= ~0x02000000;
    }
    puni->field_0x586 = 1;
    other->field_0x586 = 1;
}
void puniParaDamage(PuniData* puni) {
    s16* hp = (s16*)((u8*)puni + 0x574);
    s16 damage = puni->field_0x584;

    *hp -= damage;
    if (*hp <= 0) {
        *hp = 0;
        puni->unk4 |= 0x4000;
        puni->field_0x8 |= 0x10;
    }
}

// NONMATCHING (99.2%): control flow matches; only `before` lands in r5 (target r4), and the
// function-local `.bss` static `debug` / 0.001f literal get a different symbol offset until
// the sibling compiler-tagged statics (`h0$2954`..`takasa$2947`) and constants are emitted.
void puniParaRecover(PuniData* puni) {
    s16 before;
    static u32 debug;

    debug = (u32)puni;
    if (puni->field_0x1AC > 0.001f || puni->unkC == 0xD || puni->unkC == 0xA) {
        puni->field_0x57A = puni->field_0x57C;
        return;
    }

    puni->field_0x57A -= 1;
    if (puni->field_0x57A != 0) {
        return;
    }

    before = puni->field_0x574;
    if (before < puni->field_0x576) {
        puni->field_0x574 = before + puni->field_0x578;
        if (puni->field_0x574 > puni->field_0x576) {
            puni->field_0x574 = puni->field_0x576;
        }
        if (puni->field_0x574 > before) {
            puni->field_0x8 |= 1;
            puni->field_0x121 = 0;
        }
    }
    puni->field_0x57A = puni->field_0x57C;
}
s32 puniParaDangerCheck(PuniData* puni) {
    return 30 >= puni->field_0x574 * 100 / puni->field_0x576;
}
s32 puniParaSelfCheck(PuniData* puni) {
    return puni->field_0x574 * 100 / puni->field_0x576 >= 80;
}
s32 puniParaRecoverCheck(PuniData* puni) {
    return puni->field_0x574 >= puni->field_0x576;
}
static inline s32 puniQrnd2(s32 range) {
    s32 v;

    Punigp->field_0x450 += Punigp->field_0x454 + 0x17340000 - 0x506F;
    Punigp->field_0x454 += Punigp->field_0x450 + 0x21270000 + 0x3F9C;
    v = Punigp->field_0x454;
    if ((s32)(v ^ 0) < 0) {
        v = -v;
    }
    return v % range;
}


// NONMATCHING (98.9%): the LCG/abs now match and `idx`/`i` land in the target's
// registers; only `end` (target r7, ours r10) and the short-lived `puni` cursor (target
// r10, ours r7) are swapped.
PuniData* puniSearchRandom(s32 start, s32 count) {
    PuniData* puni;
    s32 idx;
    s32 i;
    s32 end;

    idx = start + puniQrnd2(count - 1);
    end = start + count;
    for (i = 0;; i++) {
        if (i == count) {
            return NULL;
        }
        if (idx == end) {
            idx = start;
        }
        puni = &Punidat[idx];
        if ((puni->unk4 & 0x400) != 0 && (puni->unk4 & 0x4000) == 0) {
            break;
        }
        idx++;
    }
    return puni;
}

// NONMATCHING (95.76%): the LCG/abs now match; the loop counter/index registers are one
// lower than the target (target i=r5/idx=r4, ours i=r7/idx=r5).
PuniData* puniSearchRandom_Mikata(void) {
    s32 i;
    s32 idx;
    s32 r;
    PuniData* puni;

    r = puniQrnd2(101);
    idx = r + 1;
    for (i = 0;; i++) {
        if (i == 102) {
            return NULL;
        }
        if (idx == 103) {
            idx = 1;
        }
        puni = &Punidat[idx];
        if ((puni->unk4 & 0x400) != 0 && (puni->unk4 & 0x4000) == 0) {
            return puni;
        }
        idx++;
    }
}

// The original body is a single `blr`; the function is genuinely empty.
void fn_19_A820(void) {
    (void)0;
}

// NONMATCHING (95.76%): the LCG/abs now match; the loop counter/index registers are one
// lower than the target and mwcc emits an extra trailing blr from the infinite loop.
PuniData* puniSearchRandom_Teki(void) {
    PuniData* puni;
    PuniData* base;
    s32 i;
    s32 idx;

    idx = puniQrnd2(99) + 110;
    base = Punidat;
    for (i = 0;; i++) {
        if (i == 100) {
            return NULL;
        }
        if (idx == 210) {
            idx = 110;
        }
        puni = &base[idx];
        if ((puni->unk4 & 0x400) && !(puni->unk4 & 0x4000)) {
            return puni;
        }
        idx++;
    }
}

// The original body is a single `blr`; the function is genuinely empty.
void fn_19_A8DC(void) {
    (void)0;
}
PuniData* searchBattle(PuniData* puni, s32 start, s32 count, f32 maxDist) {
    s32 i;
    PuniData* cur;
    PuniData* best;
    f32 bestDist;
    f32 d;

    best = NULL;
    bestDist = 1e+06f;
    cur = &Punidat[start];
    for (i = start; i < start + count; i++, cur++) {
        if ((cur->unk4 & 0x400) && !(cur->unk4 & 0x4000) && cur->unkC != 9 && cur->unkC != 0xD) {
            d = PSVECSquareDistance(&puni->field_0x12C, &cur->field_0x12C);
            if (d < bestDist) {
                bestDist = d;
                best = cur;
            }
        }
    }
    if (bestDist < maxDist) {
        return best;
    }
    return NULL;
}
PuniData* searchMikata(PuniData* puni) {
    PuniData* best;
    PuniData* cur;
    s32 i;
    f32 bestDist;
    f32 d;

    best = NULL;
    bestDist = 1e+06f;
    cur = &Punidat[1];
    for (i = 1; i < 103; i++, cur++) {
        if ((cur->unk4 & 0x400) && !(cur->unk4 & 0x4000) && cur->unkC != 9 && cur->unkC != 0xD) {
            d = PSVECSquareDistance(&puni->field_0x12C, &cur->field_0x12C);
            if (d < bestDist) {
                bestDist = d;
                best = cur;
            }
        }
    }
    if (bestDist < 2500.0f) {
        return best;
    }
    return NULL;
}
PuniData* searchTeki(PuniData* puni) {
    PuniData* best;
    PuniData* cur;
    s32 i;
    f32 bestDist;
    f32 d;

    best = NULL;
    bestDist = 1e+06f;
    cur = &Punidat[110];
    for (i = 110; i < 210; i++, cur++) {
        if ((cur->unk4 & 0x400) && !(cur->unk4 & 0x4000) && cur->unkC != 9 && cur->unkC != 0xD) {
            d = PSVECSquareDistance(&puni->field_0x12C, &cur->field_0x12C);
            if (d < bestDist) {
                bestDist = d;
                best = cur;
            }
        }
    }
    if (bestDist < 2500.0f) {
        return best;
    }
    return NULL;
}
PuniData* searchBattleMikata(PuniData* puni) {
    PuniData* best;
    PuniData* cur;
    s32 i;
    f32 bestDist;
    f32 d;

    best = NULL;
    bestDist = 1e+06f;
    cur = &Punidat[1];
    for (i = 1; i < 103; i++, cur++) {
        if ((cur->unk4 & 0x400) && !(cur->unk4 & 0x4000) && cur->unkC != 9 && cur->unkC != 0xD) {
            d = PSVECSquareDistance(&puni->field_0x12C, &cur->field_0x12C);
            if (d < bestDist) {
                bestDist = d;
                best = cur;
            }
        }
    }
    if (bestDist < 1e+06f) {
        return best;
    }
    return NULL;
}
PuniData* searchBattleTeki(PuniData* puni) {
    PuniData* best;
    PuniData* cur;
    s32 i;
    f32 bestDist;
    f32 d;

    best = NULL;
    bestDist = 1e+06f;
    cur = &Punidat[110];
    for (i = 110; i < 210; i++, cur++) {
        if ((cur->unk4 & 0x400) && !(cur->unk4 & 0x4000) && cur->unkC != 9 && cur->unkC != 0xD) {
            d = PSVECSquareDistance(&puni->field_0x12C, &cur->field_0x12C);
            if (d < bestDist) {
                bestDist = d;
                best = cur;
            }
        }
    }
    if (bestDist < 1e+06f) {
        return best;
    }
    return NULL;
}

// NONMATCHING (99.2%): register allocation in the inlined battle searches differs.
void puniSearch(PuniData* puni) {
    if ((s32)(gp->retraceCount & 3) != (puni->field_0x116 & 3)) {
        return;
    }
    if (puni->unk4 & 1) {
        *(PuniData**)((u8*)puni + 0x1DC) = searchBattleTeki(puni);
    } else {
        *(PuniData**)((u8*)puni + 0x1DC) = searchBattleMikata(puni);
    }
}

// NONMATCHING (96.31%): inlined battle-search loop counters land in swapped registers.
void puniModeChangeAttack(void) {
    s32 i;
    PuniData* puni;
    PuniData* best;

    for (puni = &Punidat[1], i = 1; i < 211; i++, puni++) {
        if (!(puni->unk4 & 0x400)) {
            continue;
        }
        switch (puni->unkC) {
        case 0:
        case 1:
        case 2:
        case 0x1D:
            if (puni->unk4 & 1) {
                best = searchBattleTeki(puni);
            } else {
                best = searchBattleMikata(puni);
            }
            if (best != NULL) {
                *(PuniData**)((u8*)puni + 0x1DC) = best;
                puni->unkC = 7;
                puni->field_0x114 = 0;
                puni->field_0x11A = 0;
                puni->unk4 |= 0x100000;
                PuniDokuritsu(puni);
                if ((void*)puni->field_0x218 != NULL) {
                    effDelete((EffectEntry*)puni->field_0x218);
                    puni->field_0x218 = 0;
                }
            }
            break;
        }
    }
}
void puniModeChangeAway(void) {
    s32 i;
    PuniData* puni;

    for (puni = &Punidat[1], i = 1; i <= 102; i++, puni++) {
        if (!(puni->unk4 & 0x400)) {
            continue;
        }
        if (puni->field_0x1C0 == 1e+06f) {
            continue;
        }
        switch (puni->unkC) {
        case 0:
        case 1:
        case 2:
        case 4:
        case 0x10:
            PuniDokuritsu(puni);
            puni->field_0xE = puni->unkC;
            puni->unkC = 5;
            puni->field_0x114 = 0;
            if ((void*)puni->field_0x218 != NULL) {
                effDelete((EffectEntry*)puni->field_0x218);
                puni->field_0x218 = 0;
            }
            break;
        }
    }
}

// hitCheckFilter's three out-vectors and the distance live in contiguous stack slots; keeping
// them as Vecs (rather than separate floats) reproduces the original's hoisted pointer registers.
void puniModeChangeFallCheck(void) {
    s32 i;
    PuniData* puni;
    Vec normal;
    Vec pos;
    Vec hitN;
    Vec hit;
    f32 distance;

    for (puni = &Punidat[1], i = 1; i <= 102; i++, puni++) {
        if (!(puni->unk4 & 0x400)) {
            continue;
        }
        pos = puni->field_0x12C;
        pos.y = 1.0f + pos.y;
        normal = (Vec){0.0f, -1.0f, 0.0f};
        distance = 2.0f;
        if (hitCheckFilter(NULL, &hit.x, &hit.y, &hit.z, &distance, &hitN.x, &hitN.y, &hitN.z, pos.x, pos.y, pos.z,
                           normal.x, normal.y, normal.z) != NULL) {
            continue;
        }
        PuniDokuritsu(puni);
        puni->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
        puni->unkC = 0x11;
        puni->field_0x114 = 0;
        if ((void*)puni->field_0x218 != NULL) {
            effDelete((EffectEntry*)puni->field_0x218);
            puni->field_0x218 = 0;
        }
    }
}

// NONMATCHING (97.34%): register allocation and scheduling differ.
#pragma push
#pragma dont_inline on
void puniFukki(PuniData* puni) {
    s32 i1;
    PuniData* p1;
    s32 i2;
    PuniData* p2;
    s32 i3;
    PuniData* p3;
    s32 i4;
    PuniData* p4;
    s32 i5;
    PuniData* p5;

    for (p1 = &Punidat[1], i1 = 1; i1 <= 102; i1++, p1++) {
        if ((p1->unk4 & 0x400) && p1->unkC == 0x10 &&
            !(32.0 < __fabsf(puni->field_0x12C.y - p1->field_0x12C.y)) &&
            PSVECSquareDistance(&puni->field_0x12C, &p1->field_0x12C) < 900.0f) {
            p1->unkC = 1;
            for (p2 = &Punidat[1], i2 = 1; i2 <= 102; i2++, p2++) {
                if ((p2->unk4 & 0x400) && p2->unkC == 0x10 &&
                    !(32.0 < __fabsf(p1->field_0x12C.y - p2->field_0x12C.y)) &&
                    PSVECSquareDistance(&p1->field_0x12C, &p2->field_0x12C) < 900.0f) {
                    p2->unkC = 1;
                    for (p3 = &Punidat[1], i3 = 1; i3 <= 102; i3++, p3++) {
                        if ((p3->unk4 & 0x400) && p3->unkC == 0x10 &&
                            !(32.0 < __fabsf(p2->field_0x12C.y - p3->field_0x12C.y)) &&
                            PSVECSquareDistance(&p2->field_0x12C, &p3->field_0x12C) < 900.0f) {
                            p3->unkC = 1;
                            for (p4 = &Punidat[1], i4 = 1; i4 <= 102; i4++, p4++) {
                                if ((p4->unk4 & 0x400) && p4->unkC == 0x10 &&
                                    !(32.0 < __fabsf(p3->field_0x12C.y - p4->field_0x12C.y)) &&
                                    PSVECSquareDistance(&p3->field_0x12C, &p4->field_0x12C) < 900.0f) {
                                    p4->unkC = 1;
                                    for (p5 = &Punidat[1], i5 = 1; i5 <= 102; i5++, p5++) {
                                        if ((p5->unk4 & 0x400) && p5->unkC == 0x10 &&
                                            !(32.0 < __fabsf(p4->field_0x12C.y - p5->field_0x12C.y)) &&
                                            PSVECSquareDistance(&p4->field_0x12C, &p5->field_0x12C) < 900.0f) {
                                            p5->unkC = 1;
                                            puniFukki(p5);
                                            puniDeleteEffect(p5);
                                        }
                                    }
                                    if ((void*)p4->field_0x218 != NULL) {
                                        effDelete((EffectEntry*)p4->field_0x218);
                                        p4->field_0x218 = 0;
                                    }
                                }
                            }
                            if ((void*)p3->field_0x218 != NULL) {
                                effDelete((EffectEntry*)p3->field_0x218);
                                p3->field_0x218 = 0;
                            }
                        }
                    }
                    if ((void*)p2->field_0x218 != NULL) {
                        effDelete((EffectEntry*)p2->field_0x218);
                        p2->field_0x218 = 0;
                    }
                }
            }
            if ((void*)p1->field_0x218 != NULL) {
                effDelete((EffectEntry*)p1->field_0x218);
                p1->field_0x218 = 0;
            }
        }
    }
}
#pragma pop

// NONMATCHING (97.3%): the original emits an extra unreachable `b` before the case 2 body
// (as if case 1 existed) and allocates the loop i/pointer in r30/r29-r31 instead of r27/r28.
void puniModeALLCheck(void) {
    s32 i;
    PuniData* puni;
    PuniData* best;

    switch (Punigp->field_0x43C) {
    case 2:
        for (puni = &Punidat[1], i = 1; i <= 102; i++, puni++) {
            if (!(puni->unk4 & 0x400)) {
                continue;
            }
            if (puni->field_0x1C0 == 1e+06f) {
                continue;
            }
            switch (puni->unkC) {
            case 0:
            case 1:
            case 2:
            case 4:
            case 0x10:
                PuniDokuritsu(puni);
                puni->field_0xE = puni->unkC;
                puni->unkC = 5;
                puni->field_0x114 = 0;
                if ((void*)puni->field_0x218 != NULL) {
                    effDelete((EffectEntry*)puni->field_0x218);
                    puni->field_0x218 = 0;
                }
                break;
            }
        }
        break;
    case 3:
        for (puni = &Punidat[1], i = 1; i < 211; i++, puni++) {
            if (!(puni->unk4 & 0x400)) {
                continue;
            }
            switch (puni->unkC) {
            case 0:
            case 1:
            case 2:
            case 0x1D:
                if (puni->unk4 & 1) {
                    best = searchBattleTeki(puni);
                } else {
                    best = searchBattleMikata(puni);
                }
                if (best != NULL) {
                    *(PuniData**)((u8*)puni + 0x1DC) = best;
                    puni->unkC = 7;
                    puni->field_0x114 = 0;
                    puni->field_0x11A = 0;
                    puni->unk4 |= 0x100000;
                    PuniDokuritsu(puni);
                    if ((void*)puni->field_0x218 != NULL) {
                        effDelete((EffectEntry*)puni->field_0x218);
                        puni->field_0x218 = 0;
                    }
                }
                break;
            }
        }
        break;
    }
}
void puniModeALLEndCheck(PuniData* punidat) {
    switch (Punigp->field_0x43C) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        if (Punigp->field_0x440 == 2) {
            Punigp->field_0x43C = 0;
        }
        break;
    case 3:
        break;
    }
    Punigp->field_0x440 = Punigp->field_0x43C;
}
void puniModeNormal(PuniData* punidat) {
    if (punidat->field_0x1A0 > 10.0f) {
        punidat->field_0xE = punidat->unkC;
        PuniDokuritsu(punidat);
        punidat->field_0x1DC = Punigp->field_0x360;
        punidat->unkC = 13;
        punidat->field_0x114 = 0;
    } else if (punidat->field_0x1A4 < 150.0f) {
        punidat->unkC = 1;
    }
}

// NONMATCHING (99.05%): the instruction sequence is identical; only the loop
// pointer is allocated r6 instead of r3 (and the field temporary takes r3).
void puniModeParentpre(PuniData* punidat) {
    s32 i;
    s16 min;
    PuniData* best;
    PuniData* p;
    f32 mag;
    Vec vec;

    min = 10000;
    p = Punidat;
    for (i = 0; i < PUNI_DATA_MAX; i++, p++) {
        if (!(p->unk4 & 0x80000)) {
            continue;
        }
        if ((PuniData*)p->field_0x1D8 != NULL) {
            continue;
        }
        if ((PuniData*)p->field_0x1D4 != NULL && p->field_0x11A >= 2) {
            continue;
        }
        if (p == punidat) {
            continue;
        }
        if (min > p->field_0x11A) {
            best = p;
            min = p->field_0x11A;
            min += 1;
        }
    }
    if (min != 10000) {
        punidat->field_0x1D0 = (s32)best;
        punidat->field_0x11A = min;
        punidat->field_0x118 = best->field_0x118;
        if ((PuniData*)best->field_0x1D4 == NULL) {
            best->field_0x1D4 = (s32)punidat;
        } else {
            best->field_0x1D8 = (s32)punidat;
        }
    }
    punidat->unkC = 2;
    punidat->unk4 |= 0x180000;
    PSVECSubtract(&((PuniData*)punidat->field_0x1D0)->field_0x12C, &punidat->field_0x12C, &vec);
    mag = PSVECMag(&vec);
    punidat->field_0x1A8 = mag;
    punidat->field_0x1B0 = mag;
    punidat->field_0x128 = 0;
    punidat->field_0x114 = 0;
    punidat->field_0x11C = 1;
}
void puniModeParent(PuniData* punidat) {
    PuniData* parent;
    Vec vec;
    s32 flag;
    s32 a;
    s32 b;
    u16 v;
    u8 c;

    switch (*(u16*)((u8*)punidat + 0x11E)) {
    case 0:
    case 1:
        if (punidat->field_0x1AC < 1.5f) {
            punidat->field_0x11C = 0;
        } else {
            punidat->field_0x11C = 1;
        }
        *(u8*)((u8*)punidat + 0x120) = 0;
        break;
    default:
        if (punidat->field_0x1AC < 1.5f) {
            punidat->field_0x11C = 0;
        } else {
            punidat->field_0x11C = 1;
        }
        if (*(u16*)((u8*)punidat + 0x11E) == punidat->field_0x11C) {
            *(u8*)((u8*)punidat + 0x120) = 0;
        } else {
            c = *(u8*)((u8*)punidat + 0x120);
            c++;
            *(u8*)((u8*)punidat + 0x120) = c;
            if (c <= 8) {
                punidat->field_0x11C = *(u16*)((u8*)punidat + 0x11E);
            } else {
                *(u8*)((u8*)punidat + 0x120) = 0;
            }
        }
        break;
    }
    if (!(punidat->unk4 & 0x200)) {
        parent = (PuniData*)punidat->field_0x1D0;
        flag = 0;
        PSVECSubtract(&parent->field_0x12C, &punidat->field_0x12C, &vec);
        punidat->field_0x1A8 = PSVECMag(&vec);
        if (!(bero_get_BeroEXEC() & 1)) {
            parent = (PuniData*)punidat->field_0x1D0;
            if (32.0 < __fabs(punidat->field_0x12C.y - parent->field_0x12C.y)) {
                flag = 1;
            } else {
                if (punidat->field_0x1A8 > 80.0f) {
                    flag = 1;
                } else {
                    punidat->field_0x128 = 0;
                }
            }
        }
        if (0.1f + punidat->field_0x1A8 <= punidat->field_0x1B0) {
            punidat->field_0x128 = 0;
            flag = 0;
        }
        if (*(s32*)((u8*)Punimap + 0x1C) != 0) {
            flag = 0;
        }
        if (flag) {
            if (++punidat->field_0x128 > 30) {
                punidat->unkC = 0x10;
                punidat->field_0x114 = 0;
                PuniChildHagure(punidat);
                PuniDokuritsu(punidat);
                return;
            }
        }
        punidat->field_0x1B0 = punidat->field_0x1A8;
        if (punidat->field_0x1A0 > 10.0f) {
            punidat->field_0xE = punidat->unkC;
            PuniDokuritsu(punidat);
            punidat->field_0x1DC = Punigp->field_0x360;
            punidat->unkC = 13;
            punidat->field_0x114 = 0;
            return;
        }
        if (punidat->field_0x1BC != -1 && punidat->field_0x1C0 < 10000.0f) {
            Punigp->field_0x43C = 2;
            return;
        }
        if (punidat->field_0x1B4 != -1) {
            PuniDokuritsu(punidat);
            punidat->unkC = 4;
            punidat->field_0x114 = 0;
            return;
        }
        if (*(s32*)((u8*)Punimap + 0x14) != 0 && *(s32*)((u8*)Punimap + 0x1C) != 0) {
            f32 z = punidat->field_0x12C.z;
            f32 x = punidat->field_0x12C.x;

            switch (Punigp->field_0x340) {
            case 1:
                puniMapdata_MRI_06_getidx(&a, &b, x, z);
                break;
            case 2:
                puniMapdata_MRI_09_getidx(&a, &b, x, z);
                break;
            case 3:
                puniMapdata_MRI_16_getidx(&a, &b, x, z);
                break;
            case 0:
                break;
            default:
                break;
            }
            v = *(u16*)((u8*)(*(u32*)((u8*)Punimap + 0x20)) + 2 * (a + b * *(s32*)((u8*)Punimap + 0x8)));
            if ((v & 0xff) == 0) {
                v |= 0xffff;
            }
            if (v != 0xffff && (v & 0xfff) > 2) {
                PuniDokuritsu(punidat);
                punidat->unkC = 3;
                punidat->field_0x114 = 0;
                return;
            }
        }
        parent = (PuniData*)punidat->field_0x1D0;
        if (punidat->field_0x1A8 > punidat->field_0x190 + parent->field_0x190 + punidat->field_0x194 + punidat->field_0x198) {
            PSVECNormalize(&vec, &vec);
            vec.y = 0.0f;
            PSVECScale(&vec, &vec, 1.0f);
            PSVECAdd(&punidat->field_0x15C, &vec, &punidat->field_0x15C);
            punidat->field_0x174 = 3.0f;
        }
    }
}
#define PUNI_MODE_SEARCH_GETPOS(xx, yy) puniModeSearch_sub((xx), (yy), &c, &d)
#define PUNI_MAP_AT(xx, zz) \
    (((u16*)*(void**)((u8*)Punimap + 0x20))[(xx) + (zz) * (*(s32*)((u8*)Punimap + 0x8))])
void puniModeSearch(PuniData* puni) {
    static u16 takasa;
    static u16 move;
    static u16 find;
    static u16 m0;
    static u16 m1;
    static u16 m2;
    static u16 m3;
    static u16 h0;
    static u16 h1;
    static u16 h2;
    static u16 h3;
    s32 x;
    s32 z;
    f32 c;
    f32 d;
    u16 cell;
    s32 low;
    Vec v;

    if (puni->field_0x1A0 > 10.0f) {
        puni->field_0xE = puni->unkC;
        PuniDokuritsu(puni);
        puni->field_0x1DC = Punigp->field_0x360;
        puni->unkC = 13;
        puni->field_0x114 = 0;
        return;
    }
    if (*(s32*)((u8*)Punimap + 0x1C) == 0 && puni->field_0x1A4 == 1000.0f) { // PuniData 0x1A4
        puni->unkC = 16;
        puni->field_0x11C = 0;
        puni->field_0x114 = 0;
    }
    if (*(s32*)((u8*)Punimap + 0x14) == 0) {
        return;
    }
    if (*(s32*)((u8*)Punimap + 0x1C) == 0) {
        return;
    }

    {
        f32 pz = puni->field_0x12C.z;
        f32 px = puni->field_0x12C.x;

        switch (Punigp->field_0x340) {
        case 0:
            break;
        case 1:
            puniMapdata_MRI_06_getidx(&x, &z, px, pz);
            break;
        case 2:
            puniMapdata_MRI_09_getidx(&x, &z, px, pz);
            break;
        case 3:
            puniMapdata_MRI_16_getidx(&x, &z, px, pz);
            break;
        }
    }

    cell = PUNI_MAP_AT(x, z);
    if ((cell & 0xFF) == 0) {
        cell |= 0xFFFF;
    }
    low = cell & 0xFF;
    if (low <= 2) {
        puni->unkC = 1;
        puni->unk4 |= 0x100000;
        return;
    }
    takasa = cell & 0xF000;
    find = cell & 0x0F00;
    move = low - 1;

    {
        u16 t0, t1, t2, t3;

        t0 = PUNI_MAP_AT(x - 1, z);
        if ((t0 & 0xFF) == 0) {
            t0 |= 0xFFFF;
        }
        m0 = t0;
        t1 = PUNI_MAP_AT(x, z - 1);
        if ((t1 & 0xFF) == 0) {
            t1 |= 0xFFFF;
        }
        m1 = t1;
        t2 = PUNI_MAP_AT(x + 1, z);
        if ((t2 & 0xFF) == 0) {
            t2 |= 0xFFFF;
        }
        m2 = t2;
        t3 = PUNI_MAP_AT(x, z + 1);
        if ((t3 & 0xFF) == 0) {
            t3 |= 0xFFFF;
        }
        m3 = t3;

        h0 = t0 & 0xF000;
        h1 = t1 & 0xF000;
        h2 = t2 & 0xF000;
        h3 = t3 & 0xF000;
        m0 = t0 & 0xFF;
        m1 = t1 & 0xFF;
        m2 = t2 & 0xFF;
        m3 = t3 & 0xFF;
    }

    switch (find) {
    case 0x000:
        if (m0 == move && abs((s32)h0 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x - 1, z);
            break;
        }
        if (m1 == move && abs((s32)h1 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x, z - 1);
            break;
        }
        if (m2 == move && abs((s32)h2 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x + 1, z);
            break;
        }
        if (m3 == move && abs((s32)h3 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x, z + 1);
            break;
        }
        c = 0.0f;
        d = 0.0f;
        break;
    case 0x100:
        if (m1 == move && abs((s32)h1 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x, z - 1);
            break;
        }
        if (m2 == move && abs((s32)h2 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x + 1, z);
            break;
        }
        if (m3 == move && abs((s32)h3 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x, z + 1);
            break;
        }
        if (m0 == move && abs((s32)h0 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x - 1, z);
            break;
        }
        c = 0.0f;
        d = 0.0f;
        break;
    case 0x200:
        if (m2 == move && abs((s32)h2 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x + 1, z);
            break;
        }
        if (m3 == move && abs((s32)h3 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x, z + 1);
            break;
        }
        if (m0 == move && abs((s32)h0 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x - 1, z);
            break;
        }
        if (m1 == move && abs((s32)h1 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x, z - 1);
            break;
        }
        c = 0.0f;
        d = 0.0f;
        break;
    case 0x300:
        if (m3 == move && abs((s32)h3 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x, z + 1);
            break;
        }
        if (m0 == move && abs((s32)h0 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x - 1, z);
            break;
        }
        if (m1 == move && abs((s32)h1 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x, z - 1);
            break;
        }
        if (m2 == move && abs((s32)h2 - (s32)takasa) <= 0x1000) {
            PUNI_MODE_SEARCH_GETPOS(x + 1, z);
            break;
        }
        c = 0.0f;
        d = 0.0f;
        break;
    default:
        c = 0.0f;
        d = 0.0f;
        break;
    }

    v.x = c;
    v.y = puni->field_0x12C.y;
    v.z = d;
    PSVECSubtract(&v, &puni->field_0x12C, &v);
    if (v.x != 0.0f || v.y != 0.0f || v.z != 0.0f) {
        PSVECNormalize(&v, &v);
    }
    v.y = 0.0f;
    PSVECScale(&v, &v, 1.0f);
    PSVECAdd((Vec*)&puni->field_0x15C, &v, (Vec*)&puni->field_0x15C);
    puni->field_0x174 = 3.0f;
}

// NONMATCHING (98.91%): puniQrnd2(3) restores the original's `cmpwi; bge; neg` abs
// branch; the remaining diffs are pooled-rodata symbol names and the entry-loaded
// Punigp taking a different register (so the LCG temporaries shift).
void puniModeMass(PuniData* punidat) {
    u8* p = (u8*)punidat;
    u8* target;
    s32 idx = *(s32*)(p + 0x1B4);

    target = (u8*)Punigp + 0x374 + idx * 0x14;

    if (idx == -1) {
        if (*(u16*)(p + 0xE) == 0x18)
            punidat->unkC = 0x18;
        else
            punidat->unkC = 1;
        punidat->field_0x11C = 0;
        punidat->field_0x114 = 0;
        return;
    }

    if (*(s32*)(p + 0x1BC) != -1 && *(f32*)(p + 0x1C0) < 10000.0f) {
        Punigp->field_0x43C = 2;
        return;
    }

    {
        s32 a = gp->retraceCount & 0xF;
        s32 b = punidat->field_0x116 & 0xF;

        if (Punigp->unk328 == 0x12)
            a = b;
        if (a == b && *(f32*)(p + 0x1A0) > 10.0f) {
            PuniDokuritsu(punidat);
            *(s32*)(p + 0x1DC) = Punigp->field_0x360;
            punidat->unkC = 0xD;
            punidat->field_0x114 = 0;
            return;
        }
    }

    if (*(f32*)(p + 0x1B8) > 100.0f) {
        Vec temp;

        if (*(f32*)(p + 0x1B8) < 3600.0f) {
            s32 rnd = puniQrnd2(3);

            if (rnd == 0) {
                PSVECSubtract(&punidat->field_0x12C, (Vec*)(target + 4), &temp);
                PSVECNormalize(&temp, &temp);
                temp.y = 0.0f;
                PSVECScale(&temp, &temp, 1.5f);
                PSVECAdd((Vec*)(p + 0x15C), &temp, (Vec*)(p + 0x15C));
                goto mass_after_move;
            }
        }
        PSVECSubtract((Vec*)(target + 4), &punidat->field_0x12C, &temp);
        PSVECNormalize(&temp, &temp);
        temp.y = 0.0f;
        PSVECScale(&temp, &temp, 5.0f);
        PSVECAdd((Vec*)(p + 0x15C), &temp, (Vec*)(p + 0x15C));
    mass_after_move:;
    } else {
        *(Vec*)(p + 0x15C) = (Vec){0.0f, 0.0f, 0.0f};
    }

    switch (*(u16*)(p + 0x114)) {
        case 0:
            if (*(f32*)(p + 0x15C) < 0.0f)
                punidat->field_0x180 = 270.0f;
            else
                punidat->field_0x180 = 90.0f;
            punidat->field_0x114 = 1;
            if (Punigp->unk328 == 0xD)
                punidat->field_0x114 = 0xA;
            if (Punigp->unk328 == 0x12)
                punidat->field_0x114 = 0xA;
            *(s32*)(p + 0x1E0) = 0;
            punidat->field_0x1E4 = 0;
            break;
        case 1:
            break;
        case 10:
            if (*(s32*)(p + 0x1E0) == 0x78) {
                *(s32*)(p + 0x1E0) = 0;
                punidat->field_0x1E4 = 0;
                if (rand() % 2 != 0)
                    punidat->field_0x1F8 = (Vec){0.0f, 0.0f, 2.0f};
                else
                    punidat->field_0x1F8 = (Vec){0.0f, 0.0f, -2.0f};
                punidat->field_0x114 = 0xB;
            } else if (punidat->field_0x12C.x == *(f32*)(p + 0x138) && punidat->field_0x12C.y == *(f32*)(p + 0x13C)) {
                (*(s32*)(p + 0x1E0))++;
            }
            break;
        case 11:
            if (punidat->field_0x1E4 == 0x1E) {
                punidat->field_0x114 = 0xA;
            } else {
                PSVECAdd((Vec*)(p + 0x15C), &punidat->field_0x1F8, (Vec*)(p + 0x15C));
                punidat->field_0x1E4++;
            }
            break;
        default:
            break;
    }
}
void puniModeAwayjump(PuniData* punidat) {
    s32 elapsed;

    switch (punidat->field_0x114) {
    case 0:
        punidat->modeStartTime = gp->renderTime;
        punidat->field_0x1E4 = 0;
        punidat->field_0x1EC = punidat->field_0x12C.y;
        punidat->field_0x1F0 = -72.0f;
        *(f32*)((u8*)punidat + 0x1F4) = -punidat->field_0x1F0;  // PuniData 0x1F4
        punidat->field_0x114 = 1;
        punidat->field_0x11C = 7;
        punidat->field_0x124 = 0;
        punidat->unk4 |= 0x1000000;
        if (punidat->unk4 & 0x80) {
            psndSFXOn_3D("SFX_STG2_TOGETTI_JUMP1", &punidat->field_0x12C);
        } else {
            psndSFXOn_3D("SFX_STG2_PUNI_JUMP1", &punidat->field_0x12C);
        }
        // fallthrough
    case 1:
        elapsed = ((u32)gp->renderTime - (u32)punidat->modeStartTime) / ((__OSBusClock / 4) / 1000);
        if (elapsed > 100 && punidat->field_0x1E4 == 0) {
            punidat->field_0x1E4 = 1;
            punidat->field_0x11C = 8;
            punidat->field_0x124 = 0;
        }
        if (elapsed < 200) {
            f32 t = (f32)elapsed / 200.0f;

            punidat->field_0x12C.y =
                t * (punidat->field_0x1F0 * t + *(f32*)((u8*)punidat + 0x1F4)) + punidat->field_0x1EC;
        } else {
            punidat->field_0x12C.y = punidat->field_0x1EC;
            punidat->unkC = 6;
            punidat->field_0x11C = 2;
            punidat->unk4 &= ~0x1000000;
            punidat->field_0x114 = 0;
        }
        break;
    }
}
s32 puniModeAway_sub(PuniData* punidat) {
    Vec vel;
    Vec pos;
    f32 mag;
    HitEntry* p;

    mag = PSVECMag(&punidat->field_0x15C);
    pos = punidat->field_0x12C;
    pos.y += 1.0f;
    if (0.0f != mag) {
        PSVECNormalize(&punidat->field_0x15C, &vel);
    } else {
        vel = (Vec){0.0f, 0.0f, 0.0f};
    }
    puniMoveAtari(punidat, pos, vel, 0, mag);
    p = punidat->field_0x210;
    if (p != NULL || punidat->field_0x214 != NULL) {
        if (p != NULL && (p->attributes & 4)) {
            *(f32*)&punidat->unk168[0] = -punidat->field_0x560.x;  // PuniData 0x168, 0x560
            *(f32*)&punidat->unk168[4] = -punidat->field_0x560.y;  // PuniData 0x16C
            *(f32*)&punidat->unk168[8] = -punidat->field_0x560.z;  // PuniData 0x170, 0x568
            punidat->field_0x114 = 1;
            return 1;
        }
        p = punidat->field_0x214;
        if (p != NULL && (p->attributes & 4)) {
            *(f32*)&punidat->unk168[0] = -punidat->field_0x560.x;  // PuniData 0x168, 0x560
            *(f32*)&punidat->unk168[4] = -punidat->field_0x560.y;  // PuniData 0x16C
            *(f32*)&punidat->unk168[8] = -punidat->field_0x560.z;  // PuniData 0x170, 0x568
            punidat->field_0x114 = 1;
            return 1;
        }
        punidat->field_0x114 = 10;
        return 2;
    }
    return 0;
}

// NONMATCHING (99.93%): register allocation and scheduling differ.
#pragma push
#pragma dont_inline on
void puniModeAway(PuniData* punidat) {
    f32 hitNZ;
    f32 hitNY;
    f32 hitNX;
    f32 hitZ;
    f32 hitY;
    f32 hitX;
    f32 distance;
    Vec vec;
    Vec pos;
    s32 idx;
    PuniTarget* target;

    idx = punidat->field_0x1BC;
    target = (PuniTarget*)((u8*)Punigp + 0x374 + idx * 0x14);
    if (idx == -1) {
        punidat->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
        punidat->unkC = 1;
        punidat->field_0x114 = 0;
        punidat->field_0x11C = 0;
        return;
    }
    if (punidat->field_0x1A0 > 10.0f) {
        punidat->field_0xE = punidat->unkC;
        punidat->field_0x1DC = Punigp->field_0x360;
        punidat->unkC = 13;
        punidat->field_0x114 = 0;
        return;
    }
    switch (punidat->field_0x114) {
    case 0:
        if (punidat->field_0x1C0 < 90000.0f) {
            PSVECSubtract(&punidat->field_0x12C, &target->position, &vec);
            PSVECNormalize(&vec, &vec);
            vec.y = 0.0f;
            PSVECScale(&vec, &vec, 1.4f);
            PSVECAdd(&punidat->field_0x15C, &vec, &punidat->field_0x15C);
            punidat->field_0x174 = 4.2f;
            puniModeAway_sub(punidat);
        }
        break;
    case 1:
        punidat->field_0x8 |= 4;
        PSVECNormalize((Vec*)((u8*)punidat + 0x168), &vec);  // PuniData 0x168
        PSVECScale(&vec, &punidat->field_0x15C, punidat->field_0x190);
        PSVECAdd(&punidat->field_0x12C, &punidat->field_0x15C, &punidat->field_0x12C);
        pos = punidat->field_0x12C;
        pos.y += 1.0f;
        distance = 2.0f;
        vec = (Vec){0.0f, -1.0f, 0.0f};
        pos.y += 0.2f;
        if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, pos.x, pos.y, pos.z,
                           vec.x, vec.y, vec.z) == NULL) {
            punidat->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
            punidat->unkC = 0x11;
            punidat->field_0x11C = 0;
            punidat->field_0x114 = 0;
        } else {
            punidat->field_0x114 = 0;
        }
        break;
    case 0xA:
        break;
    }
}
static inline PuniData* puniSearchTekiInline(PuniData* puni) {
    s32 i;
    PuniData* cur;
    PuniData* best;
    f32 bestDist;
    f32 d;

    best = NULL;
    bestDist = 1e+06f;
    cur = &Punidat[0x6E];
    for (i = 0x6E; i < 0xD2; i++, cur++) {
        if ((cur->unk4 & 0x400) && !(cur->unk4 & 0x4000) && cur->unkC != 9 && cur->unkC != 0xD) {
            d = PSVECSquareDistance(&puni->field_0x12C, &cur->field_0x12C);
            if (d < bestDist) {
                bestDist = d;
                best = cur;
            }
        }
    }
    if (bestDist < 1e+06f) {
        return best;
    }
    return NULL;
}
static inline PuniData* puniSearchMikataInline(PuniData* puni) {
    s32 i;
    PuniData* cur;
    PuniData* best;
    f32 bestDist;
    f32 d;

    best = NULL;
    bestDist = 1e+06f;
    cur = &Punidat[1];
    for (i = 1; i < 103; i++, cur++) {
        if ((cur->unk4 & 0x400) && !(cur->unk4 & 0x4000) && cur->unkC != 9 && cur->unkC != 0xD) {
            d = PSVECSquareDistance(&puni->field_0x12C, &cur->field_0x12C);
            if (d < bestDist) {
                bestDist = d;
                best = cur;
            }
        }
    }
    if (bestDist < 1e+06f) {
        return best;
    }
    return NULL;
}


#pragma pop
void puniModeAttack(PuniData* punidat) {
    s32 i;
    PuniData* p;
    PuniData* found;
    f32 best;
    f32 dist;
    Vec vec;

    switch (punidat->field_0x114) {
    case 0:
        punidat->field_0x11C = 1;
        punidat->field_0x114 = 1;
        // fallthrough
    case 1:
        if (punidat->field_0x1A0 > 10.0f) {
            punidat->field_0xE = punidat->unkC;
            PuniDokuritsu(punidat);
            punidat->field_0x1DC = Punigp->field_0x360;
            punidat->unkC = 13;
            punidat->field_0x114 = 0;
            return;
        }
        if ((s32)(gp->retraceCount & 3) == (punidat->field_0x116 & 3)) {
            if (punidat->unk4 & 1) {
#ifdef MUST_MATCH
                *(PuniData**)&punidat->field_0x1DC = puniSearchTekiInline(punidat);  // PuniData 0x1DC
#else
                *(PuniData**)&punidat->field_0x1DC = searchBattleTeki(punidat);  // PuniData 0x1DC
#endif
            } else {
#ifdef MUST_MATCH
                *(PuniData**)&punidat->field_0x1DC = puniSearchMikataInline(punidat);  // PuniData 0x1DC
#else
                *(PuniData**)&punidat->field_0x1DC = searchBattleMikata(punidat);  // PuniData 0x1DC
#endif
            }
        }
        if (*(PuniData**)&punidat->field_0x1DC != NULL) {  // PuniData 0x1DC
            PSVECSubtract(&(*(PuniData**)&punidat->field_0x1DC)->field_0x12C, &punidat->field_0x12C, &vec);
            dist = PSVECMag(&vec);
            if (dist > *(f32*)&(*(PuniData**)&punidat->field_0x1DC)->unk19C[0]) {  // PuniData 0x19C
                PSVECNormalize(&vec, &vec);
                vec.y = 0.0f;
                PSVECScale(&vec, &vec, 1.0f);
                PSVECAdd(&punidat->field_0x15C, &vec, &punidat->field_0x15C);
                punidat->field_0x174 = 3.0f;
            } else {
                puniParaAttack(punidat, *(PuniData**)&punidat->field_0x1DC);
            }
        }
        break;
    }
}
void puniModeDamage(PuniData* punidat) {
    Vec vec;
    PuniData* target;
    f32 mag;

    switch (punidat->field_0x114) {
    case 0:
        if (Punigp->field_0x4C0++ < 6) {
            psndSFXOn_3D("SFX_STG2_FIGHT_TOGE1\000\000\000", &punidat->field_0x12C);
        }
        punidat->field_0x114 = 1;
        break;
    case 1:
        if (Punigp->field_0x4C4++ < 6) {
            psndSFXOn_3D("SFX_STG2_FIGHT_TOGE2\000\000\000", &punidat->field_0x12C);
        }
        punidat->field_0x114 = 2;
        // fallthrough
    case 2:
        target = *(PuniData**)((u8*)punidat + 0x1DC);  // PuniData 0x1DC
        PSVECSubtract(&punidat->field_0x12C, &target->field_0x12C, &vec);
        mag = PSVECMag(&vec);
        if (mag == 0.0f) {
            vec = (Vec){0.0f, 0.0f, 0.0f};
        } else {
            PSVECNormalize(&vec, &vec);
        }
        vec.y = 0.0f;
        PSVECScale(&vec, &vec, 3.0f);
        punidat->field_0x15C = vec;
        *(f32*)&punidat->unk178[0] = 5.0f;
        punidat->field_0x11C = 3;
        punidat->field_0x114 = 3;
        break;
    case 3:
        punidat->field_0x12C.y += *(f32*)&punidat->unk178[0];
        *(f32*)&punidat->unk178[0] -= 0.5f;
        if (punidat->field_0x12C.y <= 0.001f) {
            punidat->unk4 &= ~0x2000;
            punidat->field_0x12C.y = 0.0f;
            *(f32*)&punidat->unk178[0] = 0.0f;
            punidat->field_0x138 = punidat->field_0x12C;
            punidat->field_0x574 -= punidat->field_0x584;
            if (punidat->field_0x574 <= 0) {
                punidat->field_0x574 = 0;
                punidat->unk4 |= 0x4000;
                punidat->field_0x8 |= 0x10;
            }
            if (punidat->field_0x574 * 100 / punidat->field_0x576 <= 30) {
                punidat->unkC = 8;
                punidat->field_0x11C = 2;
                punidat->field_0x114 = 0;
            } else {
                punidat->unkC = 0;
                punidat->field_0x11C = 0;
                punidat->field_0x114 = 0;
            }
        }
        break;
    }
}
void puniModePiyori(PuniData* punidat) {
    if (punidat->field_0x1A0 > 10.0f) {
        punidat->field_0xE = punidat->unkC;
        PuniDokuritsu(punidat);
        punidat->field_0x1DC = Punigp->field_0x360;
        punidat->unk4 &= ~0x08000000;
        punidat->field_0x8 &= ~0x2;
        punidat->unkC = 13;
        punidat->field_0x114 = 0;
        return;
    }
    switch (punidat->field_0x114) {
    case 0:
        if (punidat->unk4 & 2) {
            punidat->field_0x128 = 600;
            punidat->field_0x11C = 4;
            punidat->unk4 |= 0x08100000;
            punidat->field_0x8 |= 0x2;
            punidat->field_0x114 = 2;
        } else {
            punidat->field_0x114 = 10;
        }
        break;
    case 2:
        if (punidat->field_0x128-- == 0) {
            punidat->unk4 &= ~0x08000000;
            punidat->field_0x8 &= ~0x2;
            punidat->field_0x114 = 10;
        }
        break;
    case 0xA:
        if (*(u16*)&punidat->field_0xE == 10) {  // PuniData 0xE (unsigned in the original)
            if (punidat->field_0x574 * 100 / punidat->field_0x576 <= 30) {
                punidat->unkC = 8;
                punidat->field_0x11C = 2;
                punidat->field_0x114 = 0;
            } else {
                punidat->unkC = 0;
                punidat->field_0x11C = 0;
                punidat->field_0x114 = 0;
            }
        } else {
            punidat->unkC = *(u16*)&punidat->field_0xE;  // PuniData 0xE (unsigned in the original)
            if (*(u16*)&punidat->field_0xE == 2) {       // PuniData 0xE (unsigned in the original)
                punidat->unkC = 1;
            }
            punidat->field_0x114 = 0;
        }
        break;
    }
}

// NONMATCHING (98.95%): only the `field_0xE = unkC` store in case 0 is scheduled one slot earlier
// than the target (ours before `li r3, 0xd`, the target after it); reordering the source statements
// or routing the old unkC through a temporary only makes it worse.
void puniModeEscape(PuniData* punidat) {
    s32 i;
    PuniData* p;
    PuniData* found;
    Vec vec;
    f32 best;

    switch (punidat->field_0x114) {
    case 0:
        if (punidat->field_0x1A0 > 10.0f) {
            punidat->field_0xE = punidat->unkC;
            punidat->field_0x1DC = Punigp->field_0x360;
            punidat->unkC = 13;
            punidat->field_0x114 = 0;
            return;
        }
        if (punidat->field_0x574 * 100 / punidat->field_0x576 >= 0x50) {
            punidat->unkC = 0;
            punidat->field_0x11C = 0;
            punidat->field_0x114 = 0;
            return;
        }
        if ((s32)(gp->retraceCount & 3) == (punidat->field_0x116 & 3)) {
            if (punidat->unk4 & 1) {
#ifdef MUST_MATCH
                *(PuniData**)&punidat->field_0x1DC = puniSearchTekiInline(punidat);  // PuniData 0x1DC
#else
                *(PuniData**)&punidat->field_0x1DC = searchBattleTeki(punidat);  // PuniData 0x1DC
#endif
            } else {
#ifdef MUST_MATCH
                *(PuniData**)&punidat->field_0x1DC = puniSearchMikataInline(punidat);  // PuniData 0x1DC
#else
                *(PuniData**)&punidat->field_0x1DC = searchBattleMikata(punidat);  // PuniData 0x1DC
#endif
            }
        }
        found = *(PuniData**)((u8*)punidat + 0x1DC);  // PuniData 0x1DC
        if (found == NULL) {
            return;
        }
        PSVECSubtract(&found->field_0x12C, &punidat->field_0x12C, &vec);
        best = PSVECMag(&vec);
        if (best >= 120.0f) {
            *(Vec*)&punidat->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
            punidat->unkC = 11;
            punidat->field_0x11C = 0;
            punidat->field_0x114 = 0;
            return;
        }
        if (0.0f == best) {
            vec = (Vec){0.0f, 0.0f, 0.0f};
        } else {
            PSVECNormalize(&vec, &vec);
        }
        vec.y = 0.0f;
        PSVECScale(&vec, &vec, 0.8f);
        PSVECSubtract((Vec*)((u8*)punidat + 0x15C), &vec, (Vec*)((u8*)punidat + 0x15C));  // PuniData 0x15C
        punidat->field_0x174 = 2.4f;
        break;
    }
}
static inline f32 puniRandfRecover(f32 a, f32 b) {
    f32 r = (f32)rand() / 32767.0f;
    return r * (b - a) + a;
}


// NONMATCHING (99.37%): only the `field_0xE = unkC` store in case 1 is scheduled one slot earlier
// than the target (ours before `li r3, 0xd`, the target after it); reordering the source statements
// or routing the old unkC through a temporary only makes it worse.
void puniModeRecover(PuniData* punidat) {
    s32 i;
    PuniData* p;
    PuniData* found;
    Vec vec;
    f32 best;
    s32 region;
    s32 type;

    switch (punidat->field_0x114) {
    case 0:
        region = 0;
        {
            f32 lo = Punigp->field_0x354;
            f32 hi = Punigp->field_0x350;

            if (hi != -1000.0f && punidat->field_0x12C.y < hi && punidat->field_0x12C.y > lo) {
                region = 1;
            }
        }
        {
            f32 hi = Punigp->field_0x358;
            f32 lo = Punigp->field_0x35C;

            if (hi != -1000.0f && punidat->field_0x12C.y < hi && punidat->field_0x12C.y > lo) {
                region = 1;
            }
        }
        if (region == 0) {
            type = 1;
        } else {
            Vec hitn;
            Vec hit;
            Vec normal;
            Vec pos;
            f32 dist;

            pos = punidat->field_0x12C;
            normal = (Vec){0.0f, -1.0f, 0.0f};
            pos.y += 1.0f;
            dist = 3.0f;
            if (!hitCheckFilter(NULL, &hit.x, &hit.y, &hit.z, &dist, &hitn.x, &hitn.y, &hitn.z, pos.x, pos.y,
                                pos.z, normal.x, normal.y, normal.z)) {
                type = 2;
            } else {
                type = 0;
            }
        }
        if (type != 0) {
            punidat->field_0x58C = type;
            *(Vec*)&punidat->field_0x590 = punidat->field_0x12C;
            *(Vec*)&punidat->field_0x12C = Punigp->field_0x344;
            punidat->field_0x12C.x +=
#ifdef MUST_MATCH
                puniRandfRecover(0.0f, 2.0f) - 1.0f;
#else
                puniRandf(0.0f, 2.0f) - 1.0f;
#endif
            punidat->field_0x12C.z +=
#ifdef MUST_MATCH
                puniRandfRecover(0.0f, 2.0f) - 1.0f;
#else
                puniRandf(0.0f, 2.0f) - 1.0f;
#endif
            *(Vec*)&punidat->field_0x138 = punidat->field_0x12C;
            *(Vec*)((u8*)punidat + 0x59C) = punidat->field_0x12C;  // PuniData 0x59C
        }
        *(Vec*)&punidat->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
        punidat->field_0x114 = 1;
        break;
    case 1:
        if (punidat->field_0x574 * 100 / punidat->field_0x576 >= 0x50) {
            punidat->unkC = 0;
            punidat->field_0x11C = 0;
            punidat->field_0x114 = 0;
            return;
        }
        if (punidat->field_0x1A0 > 10.0f) {
            punidat->field_0xE = punidat->unkC;
            punidat->field_0x1DC = Punigp->field_0x360;
            punidat->unkC = 13;
            punidat->field_0x114 = 0;
            return;
        }
        if ((s32)(gp->retraceCount & 3) == (punidat->field_0x116 & 3)) {
            if (punidat->unk4 & 1) {
#ifdef MUST_MATCH
                *(PuniData**)&punidat->field_0x1DC = puniSearchTekiInline(punidat);  // PuniData 0x1DC
#else
                *(PuniData**)&punidat->field_0x1DC = searchBattleTeki(punidat);  // PuniData 0x1DC
#endif
            } else {
#ifdef MUST_MATCH
                *(PuniData**)&punidat->field_0x1DC = puniSearchMikataInline(punidat);  // PuniData 0x1DC
#else
                *(PuniData**)&punidat->field_0x1DC = searchBattleMikata(punidat);  // PuniData 0x1DC
#endif
            }
        }
        found = *(PuniData**)((u8*)punidat + 0x1DC);  // PuniData 0x1DC
        if (found == NULL) {
            return;
        }
        PSVECSubtract(&found->field_0x12C, &punidat->field_0x12C, &vec);
        if (PSVECMag(&vec) < 80.0f) {
            punidat->unkC = 8;
            punidat->field_0x11C = 2;
            punidat->field_0x114 = 0;
        }
        break;
    }
}
void puniModeDead(PuniData* punidat) {
    switch (punidat->field_0x114) {
    case 0:
        psndSFXOn_3D("SFX_STG2_FIGHT_TOGE3\000\000\000", &punidat->field_0x12C);
        *(Vec*)&punidat->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
        punidat->field_0x11C = 4;
        punidat->field_0x114 = 1;
        break;
    case 1:
        break;
    }
}
s32 puniModeWind_sub(PuniData* punidat) {
    Vec vel;
    Vec pos;
    f32 mag;

    mag = PSVECMag(&punidat->field_0x15C);
    pos = punidat->field_0x12C;
    pos.y += 1.0f;
    if (0.0f != mag) {
        PSVECNormalize(&punidat->field_0x15C, &vel);
    } else {
        vel = (Vec){0.0f, 0.0f, 0.0f};
    }
    puniMoveAtari(punidat, pos, vel, 0, mag);
    if (Punigp->field_0x43C == 3) {
        if (punidat->field_0x210 != NULL || punidat->field_0x214 != NULL) {
            punidat->field_0x114 = 10;
            return 2;
        }
    }
    if (punidat->field_0x210 != NULL || punidat->field_0x214 != NULL) {
        if (punidat->field_0x210 != NULL && (punidat->field_0x210->attributes & 4)) {
            *(f32*)((u8*)punidat + 0x168) = -punidat->field_0x560.x;  // PuniData 0x168, 0x560
            *(f32*)((u8*)punidat + 0x16C) = 0.0f;  // PuniData 0x16C
            *(f32*)((u8*)punidat + 0x170) = -punidat->field_0x560.z;  // PuniData 0x170, 0x568
            *(Vec*)((u8*)punidat + 0x150) = punidat->field_0x12C;  // PuniData 0x150
            punidat->field_0x114 = 3;
            return 1;
        }
        if (punidat->field_0x214 != NULL && (punidat->field_0x214->attributes & 4)) {
            *(f32*)((u8*)punidat + 0x168) = -punidat->field_0x560.x;  // PuniData 0x168, 0x560
            *(f32*)((u8*)punidat + 0x16C) = 0.0f;  // PuniData 0x16C
            *(f32*)((u8*)punidat + 0x170) = -punidat->field_0x560.z;  // PuniData 0x170, 0x568
            *(Vec*)((u8*)punidat + 0x150) = punidat->field_0x12C;  // PuniData 0x150
            punidat->field_0x114 = 3;
            return 1;
        }
        punidat->field_0x114 = 10;
        return 2;
    }
    return 0;
}
void puniModeWind_kaiten(PuniData* punidat) {
    s32 dir;

    if (punidat->field_0x180 < 180.0f) {
        dir = *(s32*)((u8*)punidat + 0x1E0);  // PuniData 0x1E0
    } else {
        dir = *(s32*)((u8*)punidat + 0x1E0) ^ 1;  // PuniData 0x1E0
    }
    if (dir != 0) {
        punidat->field_0x184 += 10.0f;
        if (punidat->field_0x184 >= 360.0f) {
            punidat->field_0x184 -= 360.0f;
        }
    } else {
        punidat->field_0x184 -= 10.0f;
        if (punidat->field_0x184 <= 0.0f) {
            punidat->field_0x184 += 360.0f;
        }
    }
}

// NONMATCHING (98.91%): the f64 temporary fixes the `frsp`, but the target also spills the unrounded
// PSVECMag result (`stfs f1,0x8(r1)`) and case 4's hitCheckFilter argument setup keeps two constants
// in different registers (r8/r10 vs r10/r3). Opcodes, constants and control flow all match.
void puniModeWind(PuniData* punidat) {
    MarioWork* mario;
    Vec vec;
    Vec pos;
    f32 hz, hy, hx, hnz, hny, hnx, dist;
    f32 mag;

    mario = marioGetPtr();
    switch (punidat->field_0x114) {
    case 0:
        if (punidat->unk4 & 0x18) {
            punidat->field_0x188 = 8.0f;
        } else {
            punidat->field_0x148 = 8.0f;
        }
        punidat->unk4 &= ~0x100000;
        PSVECSubtract(&punidat->field_0x12C, &mario->position, &vec);
        if (vec.x < 0.0f) {
            *(s32*)((u8*)punidat + 0x1E0) = 0;  // PuniData 0x1E0
        } else {
            *(s32*)((u8*)punidat + 0x1E0) = 1;  // PuniData 0x1E0
        }
        vec.x *= 1.5f;
        vec.z *= 0.5f;
#ifdef MUST_MATCH
        {
            f64 m = PSVECMag(&vec);
            f32 m2 = (f32)m;

            mag = m2;
        }
#else
        mag = PSVECMag(&vec);
#endif
        if (mag != 0.0f) {
            PSVECNormalize(&vec, &vec);
        } else {
            vec = (Vec){0.0f, 0.0f, 0.0f};
        }
        vec.y = 0.0f;
        *(f32*)((u8*)punidat + 0x1F4) =  // PuniData 0x1F4
            6.0f * (punidat->field_0x1A0 / 100.0f) + 2.0f;
        punidat->field_0x1F8 = vec;
        punidat->field_0x1EC = 0.0f;
        punidat->field_0x1F0 = 0.0f;
        if (punidat->unk4 & 1) {
            *(f32*)((u8*)punidat + 0x178) = 0.06f;  // PuniData 0x178
        } else {
            *(f32*)((u8*)punidat + 0x178) = 0.6f;  // PuniData 0x178
        }
        punidat->field_0x11C = 5;
        punidat->field_0x114 = 1;
        // fallthrough
    case 1:
        PSVECScale(&punidat->field_0x1F8, &vec, punidat->field_0x1EC);
        vec.z = 0.0f;
        *(Vec*)&punidat->field_0x15C = vec;
        puniModeWind_kaiten(punidat);
        punidat->field_0x1EC += punidat->field_0x1F0;
        punidat->field_0x1F0 += *(f32*)((u8*)punidat + 0x178);  // PuniData 0x1F0, 0x178
        if (puniModeWind_sub(punidat) != 0) {
            break;
        }
        if (!(punidat->field_0x1EC >= *(f32*)((u8*)punidat + 0x1F4))) {  // PuniData 0x1F4
            break;
        }
        if (punidat->unk4 & 1) {
            *(f32*)((u8*)punidat + 0x178) = 0.03f;  // PuniData 0x178
        } else {
            *(f32*)((u8*)punidat + 0x178) = 0.003f;  // PuniData 0x178
        }
        punidat->field_0x1F0 = 0.0f;
        punidat->field_0x114 = 2;
        break;
    case 2:
        PSVECScale(&punidat->field_0x1F8, &vec, punidat->field_0x1EC);
        vec.z = 0.0f;
        *(Vec*)&punidat->field_0x15C = vec;
        puniModeWind_kaiten(punidat);
        punidat->field_0x1EC -= punidat->field_0x1F0;
        punidat->field_0x1F0 += *(f32*)((u8*)punidat + 0x178);  // PuniData 0x1F0, 0x178
        if (puniModeWind_sub(punidat) != 0) {
            break;
        }
        if (punidat->field_0x1EC < 0.0f) {
            punidat->field_0x114 = 10;
        }
        break;
    case 3:
        punidat->field_0x8 |= 4;
        PSVECNormalize((Vec*)((u8*)punidat + 0x168), &vec);  // PuniData 0x168
        PSVECScale(&vec, (Vec*)((u8*)punidat + 0x15C), punidat->field_0x190);  // PuniData 0x15C, 0x190
        punidat->field_0x114 = 4;
        // fallthrough
    case 4:
        PSVECAdd(&punidat->field_0x12C, (Vec*)((u8*)punidat + 0x15C), &punidat->field_0x12C);  // PuniData 0x15C
        pos = punidat->field_0x12C;
        pos.y += 0.2f;
        vec = (Vec){0.0f, -1.0f, 0.0f};
        dist = 2.0f;
        if (hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &hnx, &hny, &hnz, pos.x, pos.y, pos.z, vec.x, vec.y,
                           vec.z) == NULL) {
            *(Vec*)&punidat->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
            punidat->unkC = 0x11;
            punidat->field_0x11C = 0;
            punidat->field_0x188 = 0.0f;
            punidat->field_0x184 = 0.0f;
            punidat->field_0x148 = 0.0f;
            punidat->field_0x114 = 0;
        } else {
            punidat->field_0x114 = 10;
        }
        break;
    case 10:
        puniModeWind_kaiten(punidat);
        if (!(punidat->field_0x1A0 > 0.0f)) {
            if (Punigp->field_0x43C == 3) {
                punidat->unkC = 0xa;
                punidat->field_0x188 = 0.0f;
                punidat->field_0x184 = 0.0f;
                punidat->field_0x148 = 0.0f;
                *(Vec*)&punidat->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
                punidat->field_0x114 = 0;
            } else {
                punidat->field_0x188 = 0.0f;
                punidat->field_0x184 = 0.0f;
                punidat->field_0x148 = 0.0f;
                *(Vec*)&punidat->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
                punidat->unkC = 1;
                punidat->field_0x114 = 0;
            }
        }
        break;
    }
}
static inline f32 puniRandf2(f32 a, f32 b) {
    return puniRandf(a, b) - 1.0f;
}


// NONMATCHING (98.04%): one operand-order diff in the swByteGet/swByteSet mask
// (`and r0,r3,r0` vs `and r0,r0,r3`); everything else is symbolic/register naming.
void puniModeFall(PuniData* punidat) {
    PuniFallTarget* targets = (PuniFallTarget*)((u8*)Punigp + 0x374);
    PuniFallActor* actor210;
    PuniFallActor* actor214;

    switch (punidat->field_0x114) {
        case 0:
            punidat->unk4 &= ~0x100000;
            *(Vec*)&punidat->field_0x15C = (Vec){0.0f, -0.8f, 0.0f};
            punidat->field_0x114 = 1;
            break;
        case 1: {
            Vec pos;
            Vec vel;
            s32 found;
            s32 i;
            s32 base;
            s16 kind;

            punidat->field_0x15C.x = 0.0f;
            punidat->field_0x15C.y = punidat->field_0x15C.y - 0.45f;
            vel = (Vec){0.0f, -1.0f, 0.0f};
            {
                f32 mag = PSVECMag((Vec*)&punidat->field_0x15C);

                pos = punidat->field_0x12C;
                puniMoveAtari(punidat, pos, vel, 0, mag);
            }

            actor210 = *(PuniFallActor**)((u8*)punidat + 0x210);
            if (actor210 != NULL || *(PuniFallActor**)((u8*)punidat + 0x214) != NULL) {
                if (actor210 != NULL && (actor210->flags & 0x4)) {
                    break;
                }
                actor214 = *(PuniFallActor**)((u8*)punidat + 0x214);
                if (actor214 != NULL && (actor214->flags & 0x4)) {
                    break;
                }
                if (actor210 != NULL && (actor210->flags & 0x100)) {
                    *(s32*)((u8*)punidat + 0x1E0) = 0x12;
                    punidat->field_0x11C = 0;
                    punidat->field_0x114 = 2;
                    break;
                }
                if (actor214 != NULL && (actor214->flags & 0x100)) {
                    *(s32*)((u8*)punidat + 0x1E0) = 0x12;
                    punidat->field_0x11C = 0;
                    punidat->field_0x114 = 2;
                    break;
                }
                if (actor210 != NULL && (actor210->flags & 0x200)) {
                    *(s32*)((u8*)punidat + 0x1E0) = 0x16;
                    punidat->field_0x11C = 3;
                    punidat->field_0x114 = 2;
                    break;
                }
                if (actor214 != NULL && (actor214->flags & 0x200)) {
                    *(s32*)((u8*)punidat + 0x1E0) = 0x16;
                    punidat->field_0x11C = 3;
                    punidat->field_0x114 = 2;
                    break;
                }

                found = 0;
                for (i = 0; i < 2; i++, targets++) {
                    if (targets->unk10 != NULL) {
#ifdef MUST_MATCH
                        if (actor210 == targets->unk10) {
#else
                        if (targets->unk10 == actor210) {
#endif
                            punidat->field_0x1F8 = targets->vec;
                            found = 1;
                            break;
                        }
#ifdef MUST_MATCH
                        if (actor214 == targets->unk10) {
#else
                        if (targets->unk10 == actor214) {
#endif
                            punidat->field_0x1F8 = targets->vec;
                            found = 1;
                            break;
                        }
                    }
                }
                if (found) {
                    *(s32*)((u8*)punidat + 0x1E0) = 0x17;
                } else {
                    *(s32*)((u8*)punidat + 0x1E0) = 0xF;
                }

                if (Punigp->unk328 == 0x12 && punidat->field_0x12C.y < -300.0f) {
                    kind = punidat->field_0x116;
                    if (kind < 0x67) {
                        base = kind / 8;
                        swByteSet(base + 0x28C, (u8)((0xFF ^ (1 << (kind % 8))) & swByteGet(base + 0x28C)));
                    }
                    punidat->unk4 &= ~0x800000;
                }
                *(Vec*)&punidat->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
                punidat->field_0x114 = 2;
                break;
            }

            if (punidat->field_0x12C.y <= -1000.0f) {
                *(Vec*)((u8*)punidat + 0x590) = punidat->field_0x12C;
                punidat->field_0x12C = *(Vec*)&Punigp->field_0x344;
                punidat->field_0x12C.x += puniRandf2(0.0f, 2.0f);
                punidat->field_0x12C.z += puniRandf2(0.0f, 2.0f);
                *(Vec*)&punidat->field_0x138 = punidat->field_0x12C;
                *(Vec*)((u8*)punidat + 0x59C) = punidat->field_0x12C;
                *(s32*)((u8*)punidat + 0x1E0) = 0x10;
                *(Vec*)&punidat->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
                punidat->field_0x114 = 2;
            }
            break;
        }
        case 2:
            punidat->unkC = *(s32*)((u8*)punidat + 0x1E0);
            *(Vec*)&punidat->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
            punidat->field_0x114 = 0;
            break;
    }
}
void puniModePreupdraft(PuniData* punidat) {
    switch (punidat->field_0x114) {
        case 0: {
            Vec pos = punidat->field_0x12C;
            pos.y += 15.0f;
            pos.z += 3.0f;
            *(EffectEntry**)((u8*)punidat + 0x218) =
                effPuniBalloonEntry(0, pos.x, pos.y, pos.z, 1.5f);
            punidat->field_0x128 = rand() % 201 + 0x1E;
            punidat->field_0x114 = 1;
        }
        case 1:
            if ((punidat->field_0x128)-- == 0) {
                u32* p = *(u32**)((u8*)punidat + 0x0);
                *p |= 4;
                punidat->unkC = 0x13;
                punidat->field_0x114 = 0;
            }
            break;
    }
}
// NONMATCHING (99.74%): the effect userdata write block swaps the load of `v.y`
// with the load of the 3.0f constant; everything else is register naming.
void puniModeUpdraft(PuniData* punidat) {
    MarioWork* mario = marioGetPtr();
    Vec dir;
    Vec pos;
    Vec vel;

    if (punidat->field_0x114 == 0) {
        *(Vec*)&punidat->field_0x15C = (Vec){0.0f, 0.94f, 0.0f};
    } else {
        *(Vec*)&punidat->field_0x15C = (Vec){0.0f, 0.66f, 0.0f};
    }

    if ((gp->retraceCount & 0x1F) == 0) {
        psndSFXOn_3D("SFX_STG2_SOAPBALL_MOVE1", (Vec*)((u8*)punidat + 0x12C));
    }

    {
        s32 s = (gp->retraceCount + punidat->field_0x116) & 0xF;
        s32 x = s - 3;
        if (s > 7) {
            x = 4 - (s & 7);
        }
        punidat->field_0x144 = 0.5f * (f32)x;
    }

    switch (punidat->field_0x114) {
        case 0:
            if (punidat->field_0x1A0 > 10.0f) {
                punidat->field_0x1DC = Punigp->field_0x360;
                PSVECSubtract((Vec*)((u8*)punidat + 0x12C), &mario->position, &dir);
                if (PSVECMag(&dir) != 0.0f) {
                    PSVECNormalize(&dir, &dir);
                }
                dir.y = 0.0f;
                dir.z = 0.0f;
                *(f32*)((u8*)punidat + 0x1F4) =
                    1.2f + punidat->field_0x1A0 / 100.0f;
                punidat->field_0x1F8 = dir;
                punidat->field_0x1EC = 0.0f;
                punidat->field_0x1F0 = 0.0f;
                punidat->field_0x114 = 1;
            }
            break;
        case 1:
            PSVECScale(&punidat->field_0x1F8, &dir, punidat->field_0x1EC);
            PSVECAdd((Vec*)&punidat->field_0x15C, &dir, (Vec*)&punidat->field_0x15C);
#ifdef MUST_MATCH
            {
                f32 inc = 0.06f;
                punidat->field_0x1EC = punidat->field_0x1EC + punidat->field_0x1F0;
                punidat->field_0x1F0 = punidat->field_0x1F0 + inc;
            }
#else
            punidat->field_0x1EC = punidat->field_0x1EC + punidat->field_0x1F0;
            punidat->field_0x1F0 = punidat->field_0x1F0 + 0.06f;
#endif
            if (punidat->field_0x1EC >= *(f32*)((u8*)punidat + 0x1F4)) {
                punidat->field_0x1EC = *(f32*)((u8*)punidat + 0x1F4);
            }
            break;
        case 2:
            PSVECScale(&punidat->field_0x1F8, &dir, punidat->field_0x1EC);
            PSVECAdd((Vec*)&punidat->field_0x15C, &dir, (Vec*)&punidat->field_0x15C);
#ifdef MUST_MATCH
            {
                f32 inc = 0.03f;
                punidat->field_0x1EC = punidat->field_0x1EC - punidat->field_0x1F0;
                punidat->field_0x1F0 = punidat->field_0x1F0 + inc;
            }
#else
            punidat->field_0x1EC = punidat->field_0x1EC - punidat->field_0x1F0;
            punidat->field_0x1F0 = punidat->field_0x1F0 + 0.03f;
#endif
            if (punidat->field_0x1EC < 0.0f) {
                punidat->field_0x114 = 0;
            }
            break;
    }

    {
        f32 mag = PSVECMag((Vec*)&punidat->field_0x15C);
        if (0.0f != mag) {
            PSVECNormalize((Vec*)&punidat->field_0x15C, &dir);
        } else {
            dir = (Vec){0.0f, 0.0f, 0.0f};
        }
        pos = punidat->field_0x12C;
        vel = *(Vec*)&punidat->field_0x15C;
        puniMoveAtari(punidat, pos, dir, 1, mag);
    }

    {
        Vec v = punidat->field_0x12C;
        v.x += punidat->field_0x144;
        ((f32*)((EffectEntry*)punidat->field_0x218)->userdata)[1] = v.x;
        v.y += 15.0f;
        ((f32*)((EffectEntry*)punidat->field_0x218)->userdata)[2] = v.y;
        v.z = v.z + 3.0f;
        ((f32*)((EffectEntry*)punidat->field_0x218)->userdata)[3] = v.z;
    }

    {
        PuniFallActor* actor = *(PuniFallActor**)((u8*)punidat + 0x210);
        if (actor != NULL || *(PuniFallActor**)((u8*)punidat + 0x214) != NULL) {
            s32 hit = 0;
            if (actor != NULL && (actor->flags & 4)) {
                hit = 1;
            }
            actor = *(PuniFallActor**)((u8*)punidat + 0x214);
            if (actor != NULL && (actor->flags & 4)) {
                hit = 1;
            }
            if (hit == 0) {
                punidat->unkC = 0x14;
                punidat->field_0x114 = 0;
                punidat->field_0x144 = 0.0f;
            } else {
                punidat->field_0x12C = pos;
                *(Vec*)&punidat->field_0x15C = vel;
                PSVECAdd((Vec*)((u8*)punidat + 0x12C), (Vec*)&punidat->field_0x15C,
                         (Vec*)((u8*)punidat + 0x12C));
            }
        }
        if (punidat->field_0x12C.y >= 0.0f) {
            punidat->unkC = 0x14;
            punidat->field_0x114 = 0;
            punidat->field_0x144 = 0.0f;
        }
    }
}
void puniModeWonder(PuniData* punidat) {
    switch (punidat->field_0x114) {
        case 0: {
            Vec pos;
            psndSFXOn_3D("SFX_STG2_SOAPBALL_BREAK1\000\000\000", (Vec*)((u8*)punidat + 0x12C));
            if (*(u32*)((u8*)punidat + 0x218) != 0) {
                effDelete((EffectEntry*)*(u32*)((u8*)punidat + 0x218));
                punidat->field_0x218 = 0;
            }
            pos = punidat->field_0x12C;
            pos.y += 15.0f;
            pos.z += 3.0f;
            *(EffectEntry**)((u8*)punidat + 0x218) =
                effPuniBalloonEntry(1, pos.x, pos.y, pos.z, 1.5f);
            punidat->field_0x128 = 0x14;
            punidat->field_0x11C = 9;
            punidat->field_0x124 = 0;
            punidat->unk4 |= 0x01000000;
            punidat->field_0x114 = 1;
        }
        case 1:
            if ((punidat->field_0x128)-- == 0) {
                u32* p = *(u32**)((u8*)punidat + 0x0);
                *p &= ~0x4;
                punidat->unkC = 0x15;
                punidat->field_0x11C = 1;
                punidat->unk4 &= ~0x01000000;
                punidat->field_0x114 = 0;
            }
            break;
    }
}
void puniModeFunbari(PuniData* punidat) {
    switch (punidat->field_0x114) {
        case 0: {
            PuniFallTarget* entry = (PuniFallTarget*)((u8*)Punigp + 0x374);
            s32 i;
            for (i = 0; i < 10; i++, entry++) {
                if ((entry->unk0 & 1) && (entry->unk0 & 2)) {
                    break;
                }
            }
            if (i >= 10) {
                punidat->field_0x114 = 10;
                break;
            }
            if (punidat->field_0x12C.x > entry->vec.x) {
                *(f32*)((u8*)punidat + 0x1F4) = -1.0f;
                punidat->field_0x180 = 270.0f;
            } else {
                *(f32*)((u8*)punidat + 0x1F4) = 1.0f;
                punidat->field_0x180 = 90.0f;
            }
            punidat->field_0x15C.x = 0.5f;
            punidat->field_0x128 = 0x44;
            punidat->field_0x1EC = 1.0f;
            {
                u32* p = *(u32**)((u8*)punidat + 0x0);
                *p |= 4;
            }
            punidat->field_0x114 = 1;
            break;
        }
        case 1:
        case 2: {
            f32 s = (f32)sin(3.1415927f * punidat->field_0x1EC / 180.0f);
            if (punidat->field_0x1EC <= 180.0f) {
                punidat->field_0x1F0 = 0.5f * s;
            } else {
                punidat->field_0x1F0 = 0.45f * s;
            }
            punidat->field_0x1EC += 7.0f;
            if (punidat->field_0x1EC >= 360.0f) {
                punidat->field_0x1EC = 360.0f;
            }
            punidat->field_0x12C.y = punidat->field_0x12C.y + punidat->field_0x1F0;
            punidat->field_0x12C.x = punidat->field_0x15C.x * *(f32*)((u8*)punidat + 0x1F4) +
                                     punidat->field_0x12C.x;
            if ((punidat->field_0x128)-- == 0) {
                punidat->field_0x114 = 10;
            }
            break;
        }
        case 10: {
            u32* p;
            *(Vec*)&punidat->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
            punidat->unkC = 0x11;
            punidat->field_0x11C = 0;
            p = *(u32**)((u8*)punidat + 0x0);
            *p &= ~0x4;
            punidat->field_0x114 = 0;
            break;
        }
    }
}
void puniModeWater(PuniData* punidat) {
    switch (punidat->field_0x114) {
        case 0:
            punidat->unk4 |= 0x800;
            punidat->field_0x128 = 0x3C;
            punidat->field_0x114 = 1;
        case 1:
            if ((punidat->field_0x128)-- == 0) {
                punidat->unk4 &= ~0x800;
                punidat->field_0x114 = 2;
            }
            break;
        case 2: {
            Vec dir;
            f32 t;
            punidat->modeStartTime = gp->renderTime;
            punidat->field_0x1F8 = punidat->field_0x12C;
            punidat->field_0x1EC = -80.0f;
            t = *(f32*)((u8*)punidat + 0x154) - punidat->field_0x1F8.y;  // PuniData 0x154
            punidat->field_0x1F0 = t - punidat->field_0x1EC;
            PSVECNormalize((Vec*)((u8*)punidat + 0x168), &dir);
            PSVECScale(&dir, &dir, -20.0f);
            {
                f32 tx = dir.x + punidat->field_0x12C.x;
                f32 tz = dir.z + punidat->field_0x12C.z;
                *(f32*)((u8*)punidat + 0x150) = tx;
                *(f32*)((u8*)punidat + 0x158) = tz;
            }
            punidat->field_0x114 = 3;
        }
        case 3: {
            u32 divisor = (__OSBusClock >> 2) / 1000;
            u32 step = (u32)((u32)gp->renderTime - (u32)punidat->modeStartTime) / divisor;
            if ((s32)step < 0x12C) {
                f32 t = (f32)(s32)step / 300.0f;
                f32 y;
                punidat->field_0x12C.x =
                    intplGetValue(0, step, 0x12C, punidat->field_0x1F8.x, *(f32*)((u8*)punidat + 0x150));
                punidat->field_0x12C.z =
                    intplGetValue(0, step, 0x12C, punidat->field_0x1F8.z, *(f32*)((u8*)punidat + 0x158));
                y = punidat->field_0x1EC * t + punidat->field_0x1F0;
                punidat->field_0x12C.y = t * y + punidat->field_0x1F8.y;
                *(Vec*)&punidat->field_0x138 = punidat->field_0x12C;
            } else {
                punidat->field_0x12C = *(Vec*)((u8*)punidat + 0x150);
                *(Vec*)&punidat->field_0x138 = punidat->field_0x12C;
                punidat->field_0x114 = 4;
            }
            break;
        }
        case 4:
            punidat->unkC = 0x10;
            punidat->field_0x11C = 0;
            punidat->field_0x114 = 0;
            break;
    }
}

// NONMATCHING (82.98%): the frame is 0x10 smaller and the target keeps `pos` on
// the stack (0xc..0x14) before the hitCheckFilter call where we keep it in registers.
void puniModePrehagure(PuniData* puni) {
    u8* p = (u8*)puni;
    f32 dist;
    Vec pos;
#ifdef MUST_MATCH
    volatile Vec normal;
#else
    Vec normal;
#endif
    Vec hitPos;
    Vec hitNor;
    s32 flag;
    HitEntry* hit;

    switch (*(u16*)(p + 0x114)) {
        case 0:
            flag = 0;
            if (Punigp->field_0x350 != -1000.0f && *(f32*)(p + 0x130) < Punigp->field_0x350 &&
                *(f32*)(p + 0x130) > Punigp->field_0x354) {
                flag = 1;
            }
            if (Punigp->field_0x358 != -1000.0f && *(f32*)(p + 0x130) < Punigp->field_0x358 &&
                *(f32*)(p + 0x130) > Punigp->field_0x35C) {
                flag = 1;
            }
            if (flag == 0) {
                flag = 1;
            } else {
                dist = 3.0f;
                pos.x = *(f32*)(p + 0x12C);
                pos.y = *(f32*)(p + 0x130);
                pos.z = *(f32*)(p + 0x134);
                normal = (Vec){ 0.0f, -1.0f, 0.0f };
                pos.y += 1.0f;
                hit = hitCheckFilter(NULL, &hitPos.x, &hitPos.y, &hitPos.z, &dist, &hitNor.x, &hitNor.y, &hitNor.z,
                                     pos.x, pos.y, pos.z, normal.x, normal.y, normal.z);
                if (hit == NULL) {
                    flag = 2;
                } else {
                    flag = 0;
                }
            }
            if (flag != 0) {
                *(s32*)(p + 0x58C) = flag;
                *(Vec*)(p + 0x590) = *(Vec*)(p + 0x12C);
                *(Vec*)(p + 0x12C) = *(Vec*)&Punigp->field_0x344;
                *(f32*)(p + 0x12C) += puniRandf2(0.0f, 2.0f);
                *(f32*)(p + 0x134) += puniRandf2(0.0f, 2.0f);
                *(Vec*)(p + 0x138) = *(Vec*)(p + 0x12C);
                *(Vec*)(p + 0x59C) = *(Vec*)(p + 0x12C);
            }
            *(u16*)(p + 0x114) = 1;
            break;
        case 1:
            *(u16*)(p + 0xC) = 0x10;
            *(u16*)(p + 0x114) = 0;
            break;
    }
}
void puniModeHagure(PuniData* puni) {
    u8* p = (u8*)puni;
    u8* e;

    switch (*(u16*)(p + 0x114)) {
        case 0:
            *(s32*)(p + 0x218) = (s32)effFukidashiEntry(4, 0, (s32)(p + 0x220), -1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            *(u16*)(p + 0x11C) = 6;
            *(u32*)(p + 0x4) |= 0x100000;
            *(u16*)(p + 0x114) = 1;
            /* fallthrough */
        case 1:
            *(s32*)(p + 0x1E0) = rand() % 21 + 30;
            *(s32*)(p + 0x1E4) = rand() % 6 + 5;
            *(s32*)(p + 0x128) = *(s32*)(p + 0x1E0);
            *(s32*)(p + 0x1E8) = *(s32*)(p + 0x1E4);
            *(u16*)(p + 0x114) = 10;
            *(f32*)(p + 0x2AC) = 0.0f;
            *(f32*)(p + 0x2B0) = -1000.0f;
            *(f32*)(p + 0x2B4) = 0.0f;
            /* fallthrough */
        case 10:
            if ((*(s32*)(p + 0x128))-- != 0) {
                break;
            }
            if (*(s32*)(p + 0x1E8) & 1) {
                *(f32*)(p + 0x180) = 90.0f;
            } else {
                *(f32*)(p + 0x180) = 270.0f;
            }
            if ((*(s32*)(p + 0x1E8))-- == 0) {
                *(u16*)(p + 0x114) = 20;
                break;
            }
            *(s32*)(p + 0x128) = *(s32*)(p + 0x1E0);
            break;
        case 20:
            *(s32*)(p + 0x128) = 30;
            *(u16*)(p + 0x114) = 21;
            /* fallthrough */
        case 21:
            if ((*(s32*)(p + 0x128))-- == 0) {
                *(u16*)(p + 0x114) = 30;
            }
            break;
        case 30:
            if (rand() % 2) {
                *(f32*)(p + 0x2AC) = *(f32*)(p + 0x12C);
                *(f32*)(p + 0x2B0) = 10.0f + *(f32*)(p + 0x130);
                *(f32*)(p + 0x2B4) = *(f32*)(p + 0x134);
                e = *(u8**)(p + 0x218);
                e = *(u8**)(e + 0xC);
                *(s32*)(e + 0x34) = 0;
            }
            *(s32*)(p + 0x128) = rand() % 41 + 100;
            *(u16*)(p + 0x114) = 31;
            /* fallthrough */
        case 31:
            if ((*(s32*)(p + 0x128))-- == 0) {
                *(u16*)(p + 0x114) = 1;
            }
            break;
    }

    if (*(s32*)(p + 0x1BC) != -1 && *(f32*)(p + 0x1C0) < 10000.0f) {
        Punigp->field_0x43C = 2;
        *(u16*)(p + 0x114) = 0;
        if (*(u32*)(p + 0x218) != 0) {
            effDelete((EffectEntry*)*(s32*)(p + 0x218));
            *(s32*)(p + 0x218) = 0;
        }
    } else if (*(s32*)(p + 0x1B4) != -1) {
        PuniDokuritsu(puni);
        *(u16*)(p + 0xC) = 4;
        *(u16*)(p + 0x114) = 0;
        if (*(u32*)(p + 0x218) != 0) {
            effDelete((EffectEntry*)*(s32*)(p + 0x218));
            *(s32*)(p + 0x218) = 0;
        }
    } else if (*(f32*)(p + 0x1A0) > 10.0f) {
        PuniDokuritsu(puni);
        *(s32*)(p + 0x1DC) = Punigp->field_0x360;
        *(u16*)(p + 0xC) = 0xD;
        *(u16*)(p + 0x114) = 0;
        if (*(u32*)(p + 0x218) != 0) {
            effDelete((EffectEntry*)*(s32*)(p + 0x218));
            *(s32*)(p + 0x218) = 0;
        }
    } else if (*(f32*)(p + 0x1A4) < 150.0f) {
        *(u16*)(p + 0xC) = 1;
        puniFukki(puni);
        if (*(u32*)(p + 0x218) != 0) {
            effDelete((EffectEntry*)*(s32*)(p + 0x218));
            *(s32*)(p + 0x218) = 0;
        }
    }
}
void puniModeDodai(PuniData* puni) {
    u8* p = (u8*)puni;
    Vec diff;

    switch (*(u16*)(p + 0x114)) {
        case 0:
            PSVECSubtract((Vec*)(p + 0x12C), (Vec*)(p + 0x1F8), &diff);
            if (PSVECMag(&diff) < 10.0f) {
                *(Vec*)(p + 0x15C) = (Vec){ 0.0f, 0.0f, 0.0f };
                *(u16*)(p + 0xC) = 0x10;
                *(u16*)(p + 0x114) = 0;
                break;
            }
            PSVECNormalize(&diff, &diff);
            diff.y = 0.0f;
            PSVECScale(&diff, &diff, 1.0f);
            PSVECSubtract((Vec*)(p + 0x15C), &diff, (Vec*)(p + 0x15C));
            *(f32*)(p + 0x174) = 3.0f;
            break;
    }
}
void puniModeTrap_sub(PuniData* puni) {
    u8* p = (u8*)puni;
    NpcEntry* npc;

    if (*(u32*)(p + 0x4) & 0x20) {
        npc = npcNameToPtr("ガイド");
        *(u32*)(p + 0x4) &= ~0x800;
        *(u32*)(p + 0x4) |= 0x100000;
        npc->flags &= ~0x20;
        npc->position.x = 0.0f;
        npc->position.y = -1000.0f;
        npc->position.z = 0.0f;
    }
    if (*(u32*)(p + 0x4) & 0x40) {
        npc = npcNameToPtr("ガイド妹");
        *(u32*)(p + 0x4) &= ~0x800;
        *(u32*)(p + 0x4) |= 0x100000;
        npc->flags &= ~0x20;
        npc->position.x = 0.0f;
        npc->position.y = -1000.0f;
        npc->position.z = 0.0f;
    }
    if (*(u32*)(p + 0x4) & 0x100) {
        npc = npcNameToPtr("ライバル");
        *(u32*)(p + 0x4) &= ~0x800;
        *(u32*)(p + 0x4) |= 0x100000;
        npc->flags &= ~0x20;
        npc->position.x = 0.0f;
        npc->position.y = -1000.0f;
        npc->position.z = 0.0f;
    }
}
void puniModeTrap(PuniData* puni) {
    u8* p = (u8*)puni;
    NpcEntry* npc;

    if (*(u16*)(p + 0x114) == 0) {
        *(u32*)(p + 0x4) |= 0x100000;
        if (*(u32*)(p + 0x4) & 0x20) {
            npc = npcNameToPtr("ガイド");
            *(u32*)(p + 0x4) |= 0x800;
            npc->position.x = *(f32*)(p + 0x12C);
            npc->position.y = *(f32*)(p + 0x130);
            npc->position.z = *(f32*)(p + 0x134);
            npc->direction = *(f32*)(p + 0x180);
            npc->flags |= 0x20;
        }
        if (*(u32*)(p + 0x4) & 0x40) {
            npc = npcNameToPtr("ガイド妹");
            *(u32*)(p + 0x4) |= 0x800;
            npc->position.x = *(f32*)(p + 0x12C);
            npc->position.y = *(f32*)(p + 0x130);
            npc->position.z = *(f32*)(p + 0x134);
            npc->direction = *(f32*)(p + 0x180);
            npc->flags |= 0x20;
        }
        if (*(u32*)(p + 0x4) & 0x100) {
            npc = npcNameToPtr("ライバル");
            *(u32*)(p + 0x4) |= 0x800;
            npc->position.x = *(f32*)(p + 0x12C);
            npc->position.y = *(f32*)(p + 0x130);
            npc->position.z = *(f32*)(p + 0x134);
            npc->direction = *(f32*)(p + 0x180);
            npc->flags |= 0x20;
        }
        *(Vec*)(p + 0x15C) = (Vec){ 0.0f, 0.0f, 0.0f };
        *(u16*)(p + 0xE) = *(u16*)(p + 0xC);
    }
    if (*(f32*)(p + 0x1A0) > 10.0f) {
        if (*(u32*)(p + 0x4) & 0x20) {
            npc = npcNameToPtr("ガイド");
            *(u32*)(p + 0x4) &= ~0x800;
            *(u32*)(p + 0x4) |= 0x100000;
            npc->flags &= ~0x20;
            npc->position.x = 0.0f;
            npc->position.y = -1000.0f;
            npc->position.z = 0.0f;
        }
        if (*(u32*)(p + 0x4) & 0x40) {
            npc = npcNameToPtr("ガイド妹");
            *(u32*)(p + 0x4) &= ~0x800;
            *(u32*)(p + 0x4) |= 0x100000;
            npc->flags &= ~0x20;
            npc->position.x = 0.0f;
            npc->position.y = -1000.0f;
            npc->position.z = 0.0f;
        }
        if (*(u32*)(p + 0x4) & 0x100) {
            npc = npcNameToPtr("ライバル");
            *(u32*)(p + 0x4) &= ~0x800;
            *(u32*)(p + 0x4) |= 0x100000;
            npc->flags &= ~0x20;
            npc->position.x = 0.0f;
            npc->position.y = -1000.0f;
            npc->position.z = 0.0f;
        }
        PuniDokuritsu(puni);
        *(s32*)(p + 0x1DC) = Punigp->field_0x360;
        *(u16*)(p + 0xE) = *(u16*)(p + 0xC);
        *(u16*)(p + 0xC) = 13;
        *(u16*)(p + 0x114) = 0;
        return;
    }
    if (*(s32*)(p + 0x1B4) != -1) {
        if (*(u32*)(p + 0x4) & 0x20) {
            npc = npcNameToPtr("ガイド");
            *(u32*)(p + 0x4) &= ~0x800;
            *(u32*)(p + 0x4) |= 0x100000;
            npc->flags &= ~0x20;
            npc->position.x = 0.0f;
            npc->position.y = -1000.0f;
            npc->position.z = 0.0f;
        }
        if (*(u32*)(p + 0x4) & 0x40) {
            npc = npcNameToPtr("ガイド妹");
            *(u32*)(p + 0x4) &= ~0x800;
            *(u32*)(p + 0x4) |= 0x100000;
            npc->flags &= ~0x20;
            npc->position.x = 0.0f;
            npc->position.y = -1000.0f;
            npc->position.z = 0.0f;
        }
        if (*(u32*)(p + 0x4) & 0x100) {
            npc = npcNameToPtr("ライバル");
            *(u32*)(p + 0x4) &= ~0x800;
            *(u32*)(p + 0x4) |= 0x100000;
            npc->flags &= ~0x20;
            npc->position.x = 0.0f;
            npc->position.y = -1000.0f;
            npc->position.z = 0.0f;
        }
        PuniDokuritsu(puni);
        *(u16*)(p + 0xE) = *(u16*)(p + 0xC);
        *(u16*)(p + 0xC) = 4;
        *(u16*)(p + 0x114) = 0;
    }
}
void puniModeEnterSubInit(PuniData* puni) {
    u8* p = (u8*)puni;

    if ((*(u32*)((u8*)Punigp + 0x324) & 3) == 0) {
        return;
    }
    *(s32*)(p + 0x1E0) = 0;
    *(f32*)(p + 0x18C) = 0.0f;
    *(u32*)(*(u8**)(p + 0x0)) |= 4;
}
void puniModeEnterSub(PuniData* puni) {
    u8* p = (u8*)puni;

    if ((*(u32*)((u8*)Punigp + 0x324) & 3) == 0) {
        return;
    }
    *(s32*)(p + 0x1E0) += 1;
    if (*(s32*)(p + 0x1E0) < 10) {
        return;
    }
    *(f32*)(p + 0x18C) += 0.1f;
    if (*(f32*)(p + 0x18C) >= 1.0f) {
        *(f32*)(p + 0x18C) = 1.0f;
    }
}

// NONMATCHING (98.61%): the case-3 searchParent loop cursor lands in r6 where the original keeps it in r3
// (mine folds the global load to `lwz r6, Punidat@l(r3)`; the original emits `addi r3,r3,Punidat@l; lwz r3,0(r3)`),
// and the inlined puniQrnd(500) uses r4/r5 for the Punigp base/value where the original uses r5/r6.
void puniModeEnter(PuniData* puni) {
    u8* p = (u8*)puni;
    u8* parent;
    s32 i;
    s32 best;
    Vec diff;
    u32 bus;
    u32 ticksPerMs;
    s32 elapsed;
    u32 seed;

    switch (*(u16*)(p + 0x114)) {
        case 0:
            puni->modeStartTime = gp->renderTime;
            *(s32*)(p + 0x128) = 200;
            if ((*(u32*)((u8*)Punigp + 0x324) & 8) && !(*(u32*)((u8*)Punigp + 0x324) & 0x10)) {
                *(s32*)(p + 0x128) = 700;
            }
            // qrnd(500), inlined
            *(s32*)(p + 0x128) += puniQrnd(500);
            if (*(u32*)((u8*)Punigp + 0x324) & 3) {
                *(s32*)(p + 0x1E0) = 0;
                *(f32*)(p + 0x18C) = 0.0f;
                *(u32*)(*(u8**)(p + 0x0)) |= 4;
            }
            *(u16*)(p + 0x114) = 1;
            // fallthrough
        case 1:
            bus = __OSBusClock;
            ticksPerMs = (bus >> 2) / 1000;
            elapsed = (s32)(((u32)gp->renderTime - (u32)puni->modeStartTime) / ticksPerMs);
            if (elapsed > *(s32*)(p + 0x128)) {
                *(u16*)(p + 0x114) = 2;
            }
            break;
        case 2:
            if (*(u32*)((u8*)Punigp + 0x324) & 3) {
                *(s32*)(p + 0x1E0) += 1;
                if (*(s32*)(p + 0x1E0) >= 10) {
                    *(f32*)(p + 0x18C) += 0.1f;
                    if (*(f32*)(p + 0x18C) >= 1.0f) {
                        *(f32*)(p + 0x18C) = 1.0f;
                    }
                }
            }
            if (PSVECSquareDistance((Vec*)(p + 0x1F8), (Vec*)(p + 0x12C)) > 100.0f) {
                PSVECSubtract((Vec*)(p + 0x1F8), (Vec*)(p + 0x12C), &diff);
                PSVECNormalize(&diff, &diff);
                diff.y = 0.0f;
                PSVECScale(&diff, &diff, 1.0f);
                PSVECAdd((Vec*)(p + 0x15C), &diff, (Vec*)(p + 0x15C));
            } else {
                *(u32*)(p + 0x4) |= 0x1000;
                *(Vec*)(p + 0x15C) = (Vec){ 0.0f, 0.0f, 0.0f };
                *(f32*)(p + 0x18C) = 1.0f;
                *(u32*)(*(u8**)(p + 0x0)) &= ~4;
                *(u16*)(p + 0x114) = 3;
            }
            break;
        case 3: {
            PuniData* data;
            data = Punidat;
            best = 10000;
            for (i = 0; i < 211; i++) {
                if ((data->unk4 & 0x80000) && (u32)data->field_0x1D8 == 0 &&
                    ((u32)data->field_0x1D4 == 0 || data->field_0x11A < 2) && (u8*)data != p &&
                    (s16)best > data->field_0x11A) {
                    parent = (u8*)data;
                    best = data->field_0x11A + 1;
                }
                data++;
            }
            if ((s16)best != 10000) {
                *(u8**)(p + 0x1D0) = parent;
                *(s16*)(p + 0x11A) = best;
                *(s16*)(p + 0x118) = *(s16*)(parent + 0x118);
                if (*(u32*)(parent + 0x1D4) == 0) {
                    *(u8**)(parent + 0x1D4) = p;
                } else {
                    *(u8**)(parent + 0x1D8) = p;
                }
            }
            *(u16*)(p + 0xC) = 2;
            *(u32*)(p + 0x4) |= 0x180000;
            PSVECSubtract((Vec*)(*(u8**)(p + 0x1D0) + 0x12C), (Vec*)(p + 0x12C), &diff);
            *(f32*)(p + 0x1B0) = *(f32*)(p + 0x1A8) = PSVECMag(&diff);
            *(s32*)(p + 0x128) = 0;
            *(u16*)(p + 0x114) = 0;
            *(u16*)(p + 0x11C) = 1;
            break;
        }
    }
}
void puniModeExitSubInit(PuniData* puni) {
    u8* p = (u8*)puni;

    if ((*(u32*)((u8*)Punigp + 0x324) & 3) == 0) {
        return;
    }
    *(s32*)(p + 0x1E0) = 0;
    *(u32*)(*(u8**)(p + 0x0)) |= 4;
}
void puniModeExitSub(PuniData* puni) {
    u8* p = (u8*)puni;

    if ((*(u32*)((u8*)Punigp + 0x324) & 3) == 0) {
        return;
    }
    *(s32*)(p + 0x1E0) += 1;
    if (*(s32*)(p + 0x1E0) < 10) {
        return;
    }
    *(f32*)(p + 0x18C) = *(f32*)(p + 0x18C) - 0.3f;
    if (*(f32*)(p + 0x18C) <= 0.0f) {
        *(f32*)(p + 0x18C) = 0.0f;
    }
}
void puniModeExit(PuniData* punidat) {
    u8* p = (u8*)punidat;
    Vec v;

    switch (*(u16*)(p + 0x114)) {
        case 0:
            if (PSVECSquareDistance((Vec*)((u8*)Punigp + 0x480), (Vec*)(p + 0x12C)) >
                Punigp->field_0x4B0) {
                PSVECSubtract((Vec*)((u8*)Punigp + 0x480), (Vec*)(p + 0x12C), &v);
                PSVECNormalize(&v, &v);
                v.y = 0.0f;
                PSVECScale(&v, &v, 1.5f);
                PSVECAdd((Vec*)(p + 0x15C), &v, (Vec*)(p + 0x15C));
            } else {
                *(Vec*)(p + 0x15C) = (Vec){0.0f, 0.0f, 0.0f};
                *(u32*)(p + 0x4) &= 0xFFEFEFFF;
                if (*(u32*)((u8*)Punigp + 0x324) & 3) {
                    *(s32*)(p + 0x1E0) = 0;
                    *(u32*)(*(u32**)(p + 0x0)) |= 4;
                }
                *(u16*)(p + 0x114) = 1;
            }
            break;
        case 1:
            if (*(u32*)((u8*)Punigp + 0x324) & 3) {
                (*(s32*)(p + 0x1E0))++;
                if (*(s32*)(p + 0x1E0) >= 10) {
                    *(f32*)(p + 0x18C) -= 0.3f;
                    if (*(f32*)(p + 0x18C) <= 0.0f) {
                        *(f32*)(p + 0x18C) = 0.0f;
                    }
                }
            }
            if (PSVECSquareDistance((Vec*)((u8*)Punigp + 0x48C), (Vec*)(p + 0x12C)) >
                Punigp->field_0x4B4) {
                PSVECSubtract((Vec*)((u8*)Punigp + 0x48C), (Vec*)(p + 0x12C), &v);
                PSVECNormalize(&v, &v);
                v.y = 0.0f;
                PSVECScale(&v, &v, 2.0f);
                PSVECAdd((Vec*)(p + 0x15C), &v, (Vec*)(p + 0x15C));
            } else {
                *(u16*)(p + 0xC) = 0x1E;
                *(u32*)(p + 0x4) &= ~0x100000;
                *(Vec*)(p + 0x15C) = (Vec){0.0f, 0.0f, 0.0f};
                *(Vec*)(p + 0x12C) = (Vec){0.0f, -1000.0f, 0.0f};
                *(Vec*)(p + 0x138) = *(Vec*)(p + 0x12C);
                *(u16*)(p + 0x114) = 0;
            }
            break;
        case 2:
            break;
    }
}
void puniModePoint(PuniData* punidat) {
    u8* p = (u8*)punidat;
    Vec v;

    switch (*(u16*)(p + 0x114)) {
        case 0:
            if (PSVECSquareDistance((Vec*)((u8*)Punigp + 0x480), (Vec*)(p + 0x12C)) >
                Punigp->field_0x4B0) {
                PSVECSubtract((Vec*)((u8*)Punigp + 0x480), (Vec*)(p + 0x12C), &v);
                PSVECNormalize(&v, &v);
                v.y = 0.0f;
                PSVECScale(&v, &v, 1.5f);
                PSVECAdd((Vec*)(p + 0x15C), &v, (Vec*)(p + 0x15C));
            } else {
                *(Vec*)(p + 0x15C) = (Vec){0.0f, 0.0f, 0.0f};
                *(u32*)(p + 0x4) &= 0xFFEFEFFF;
                *(u16*)(p + 0x114) = 1;
            }
            break;
        case 1:
            if (PSVECSquareDistance((Vec*)((u8*)Punigp + 0x48C), (Vec*)(p + 0x12C)) >
                Punigp->field_0x4B4) {
                PSVECSubtract((Vec*)((u8*)Punigp + 0x48C), (Vec*)(p + 0x12C), &v);
                PSVECNormalize(&v, &v);
                v.y = 0.0f;
                PSVECScale(&v, &v, 2.0f);
                PSVECAdd((Vec*)(p + 0x15C), &v, (Vec*)(p + 0x15C));
            } else {
                *(u16*)(p + 0xC) = 0x1E;
                *(u32*)(p + 0x4) &= ~0x100000;
                *(Vec*)(p + 0x15C) = (Vec){0.0f, 0.0f, 0.0f};
                *(Vec*)(p + 0x12C) = (Vec){0.0f, -1000.0f, 0.0f};
                *(Vec*)(p + 0x138) = *(Vec*)(p + 0x12C);
                *(u16*)(p + 0x114) = 0;
            }
            break;
        case 2:
            break;
    }
}
void puniModeDokan(PuniData* punidat) {
    u8* p = (u8*)punidat;

    *(u16*)(p + 0x114) = 0;
    *(u32*)(p + 0x4) |= 0x800;
    *(u32*)(*(u32**)(p + 0x0)) |= 4;
    *(Vec*)(p + 0x15C) = (Vec){0.0f, 0.0f, 0.0f};
}
void puniModeCustom(PuniData* punidat) {
    void (*func)(PuniData*) = *(void (**)(PuniData*))((u8*)punidat + 0x56C);

    if (func != NULL) {
        func(punidat);
    }
}
void puni_puni_disp(CameraId cameraId, void* param) {
    EnvironmentYami yami;
    EnvironmentYami* pyami;
    s32 i;

    puniLoadRenderMode();
    puniLoadVertex();
    puniLoadTexture();
    puniLoadTev();
    if (_swGet(3) == 0) {
        puniDraw();
    } else {
        pyami = &yami;
        for (i = 0; i < puniGetPoseNum(); i++) {
            PuniPose* pose = &puniGetPosePtr()[i];

            pyami->tevStage = 1;
            pyami->texMap = 1;
            pyami->texCoord = 1;
            pyami->unk18 = 0;
            pyami->unk4 = (MtxPtr)pose->matrix;
            envSetYami(pyami);
            GXSetNumTexGens(pyami->texCoord);
            GXSetNumTevStages(pyami->tevStage);
            GXSetNumIndStages(pyami->unk18);
            puniPoseDraw(i);
        }
    }
    puniLoadTextureExit();
}
void puni_puni_shadow_disp(CameraId camId, void* param) {
    static Mtx shadow_mt = {
        { 10.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 10.0f, 0.0f, 0.1f },
        { 0.0f, 0.0f, 10.0f, 0.0f },
    };
    CameraEntry* cam = camGetPtr(camId);
    PuniPose* pose = (PuniPose*)param;
    Mtx mtx;
    GXTexObj texObj;
    GXColor color;

    if (!(pose->flags & 4)) {
        GXSetNumChans(1);
        GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
        color = (GXColor){0x00, 0x00, 0x00, 0xC0};
        GXSetChanMatColor(4, color);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
        effGetTexObjN64(0x2C, &texObj);
        GXLoadTexObj(&texObj, 0);
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0, 0, 4);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
        GXSetTevAlphaIn(0, 7, 4, 5, 7);
        GXClearVtxDesc();
        GXSetVtxDesc(9, 2);
        GXSetVtxDesc(0xD, 2);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
        GXSetArray(9, shadow_vtx, 0xC);
        GXSetArray(0xD, shadow_coord, 8);
        GXSetCullMode(0);
        PSMTXConcat(cam->view, (MtxPtr)pose->matrix, mtx);
        PSMTXConcat(mtx, shadow_mt, mtx);
        if (pose->flags & 0x10) {
            PSMTXTransApply(mtx, mtx, 0.0f,
                             1.0f + (-(*pose->matrix)[1][3] + pose->field_0x10), 0.0f);
        }
        GXLoadPosMtxImm(mtx, 0);
        GXSetCurrentMtx(0);
        GXCallDisplayList(shadow_dl, 0x20);
    }
}
void puni_puni_init(void) {
    s32 i;
    PuniPose* pose = puniGetPosePtr();
    MarioWork* mario = marioGetPtr();
    s32 rnd = (s32)randf(0.0f, 1e+08f);
    PuniData* pd;
    u8* puni;

    Punigp->field_0x450 = (s32)gp->renderTime + rnd + 0x3A3B01F9;
    Punigp->field_0x454 = 0x21708331;
    pd = Punidat;
    *(PuniData**)((u8*)Punigp + 0x360) = pd;
    pd->unk4 |= 0x180404;
    pd->field_0x12C = mario->position;
    *(Vec*)&pd->field_0x138 = pd->field_0x12C;
    pd->field_0x190 = 10.0f;
    pd->unkC = 30;
    pd->field_0x116 = 0;
    pd->field_0x118 = 0;
    puni = (u8*)&Punidat[1];
    for (i = 1; i < 211; i++, puni += 0x5A8) {
        pose->flags = 1;
        pose->flags |= 2;
        pose->field_0x4 = 0.0f;
        PSMTXIdentity((MtxPtr)pose->matrix);
        (*pose->matrix)[0][3] = 0.0f;
        (*pose->matrix)[1][3] = -1000.0f;
        (*pose->matrix)[2][3] = 0.0f;
        pose->field_0xC = 0.0f;
        *(PuniPose**)(puni + 0x0) = pose;
        pose++;
        *(Vec*)(puni + 0x144) = (Vec){0.0f, 0.0f, 0.0f};
        *(f32*)(puni + 0x180) = 0.0f;
        *(f32*)(puni + 0x188) = 0.0f;
        *(f32*)(puni + 0x18C) = 1.0f;
        *(f32*)(puni + 0x19C) = 10.0f;
        *(s32*)(puni + 0x4) = 0;
        *(s16*)(puni + 0x116) = i;
        *(s16*)(puni + 0x118) = -1;
        *(f32*)(puni + 0x174) = 3.0f;
    }
    Punigp->showPuniCounter = 1;
    Punigp->puniCounterAlpha = 0xFF;
    Punigp->unk448 = 0;
    Punimap = _mapAlloc(0x24);
    memset(Punimap, 0, 0x24);
}

// The original unit's rodata is not yet moved into this source; the symbols are declared above.
// NONMATCHING (99.81%): every instruction's opcode/control flow matches; only stack-temp slots differ
// (case 15's hitCheckFilter outputs and pos/normal, and case 27's diff Vec, land at different offsets).
void unk_mri_00012d58(PuniData* self) {
    switch (self->unkC) {
    case 0:
        self->unk4 &= ~0x8000;
        if (self->field_0x1A0 > 10.0f) {
            self->field_0xE = self->unkC;
            PuniDokuritsu(self);
            *(PuniData**)((u8*)self + 0x1DC) = (PuniData*)(s32)Punigp->field_0x360;  // PuniData 0x1DC
            self->unkC = 0xD;
            self->field_0x114 = 0;
        } else if (self->field_0x1A4 < 150.0f) {
            self->unkC = 1;
        }
        break;
    case 1: {
        self->unk4 &= ~0x8000;
        searchParent(self);
        self->unkC = 2;
        self->unk4 |= 0x180000;
        {
            Vec diff;
            f32 mag;
            PSVECSubtract(&((PuniData*)self->field_0x1D0)->field_0x12C, &self->field_0x12C, &diff);
            mag = PSVECMag(&diff);
            self->field_0x1A8 = mag;
            self->field_0x1B0 = mag;
        }
        self->field_0x128 = 0;
        self->field_0x114 = 0;
        self->field_0x11C = 1;
        break;
    }
    case 2:
        self->unk4 &= ~0x8000;
        puniModeParent(self);
        break;
    case 3:
        self->unk4 &= ~0x8000;
        puniModeSearch(self);
        break;
    case 4:
        self->unk4 |= 0x8000;
        puniModeMass(self);
        break;
    case 5:
        self->unk4 |= 0x8000;
        puniModeAwayjump(self);
        break;
    case 6:
        self->unk4 |= 0x8000;
        puniModeAway(self);
        break;
    case 7:
        self->unk4 &= ~0x8000;
        switch (self->field_0x114) {
        case 0:
            self->field_0x11C = 1;
            self->field_0x114 = 1;
            /* fallthrough */
        case 1:
            if (self->field_0x1A0 > 10.0f) {
                self->field_0xE = self->unkC;
                PuniDokuritsu(self);
                *(PuniData**)((u8*)self + 0x1DC) = (PuniData*)(s32)Punigp->field_0x360;  // PuniData 0x1DC
                self->unkC = 0xD;
                self->field_0x114 = 0;
            } else if (((s32)gp->retraceCount & 3) == (self->field_0x116 & 3)) {
                if (self->unk4 & 1) {
                    *(PuniData**)((u8*)self + 0x1DC) = searchBattleTeki(self);  // PuniData 0x1DC
                } else {
                    *(PuniData**)((u8*)self + 0x1DC) = searchBattleMikata(self);  // PuniData 0x1DC
                }
            }
            if (*(PuniData**)((u8*)self + 0x1DC) != NULL) {  // PuniData 0x1DC
                Vec diff;
                PSVECSubtract(&(*(PuniData**)((u8*)self + 0x1DC))->field_0x12C, &self->field_0x12C, &diff);  // PuniData 0x1DC
                if (PSVECMag(&diff) > *(f32*)((u8*)(*(PuniData**)((u8*)self + 0x1DC)) + 0x19C)) {  // PuniData 0x1DC
                    PSVECNormalize(&diff, &diff);
                    diff.y = 0.0f;
                    PSVECScale(&diff, &diff, 1.0f);
                    PSVECAdd((Vec*)&self->field_0x15C, &diff, (Vec*)&self->field_0x15C);
                    self->field_0x174 = 3.0f;
                } else {
                    puniParaAttack(self, *(PuniData**)((u8*)self + 0x1DC));  // PuniData 0x1DC
                }
            }
            break;
        default:
            break;
        }
        break;
    case 9:
        self->unk4 &= ~0x8000;
        puniModeDamage(self);
        break;
    case 10:
        self->unk4 &= ~0x8000;
        puniModePiyori(self);
        break;
    case 8:
        self->unk4 &= ~0x8000;
        puniModeEscape(self);
        break;
    case 11:
        self->unk4 &= ~0x8000;
        puniModeRecover(self);
        break;
    case 12:
        self->unk4 &= ~0x8000;
        switch (self->field_0x114) {
        case 0:
            psndSFXOn_3D("SFX_STG2_FIGHT_TOGE3\000\000\000", &self->field_0x12C);
            self->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
            self->field_0x11C = 4;
            self->field_0x114 = 1;
            break;
        case 1:
            break;
        }
        break;
    case 13:
        self->unk4 &= ~0x8000;
        puniModeWind(self);
        break;
    case 15:
        self->unk4 |= 0x8000;
        switch (self->field_0x114) {
        case 0: {
            BOOL range = FALSE;
            s32 state;
            f32 top;
            f32 bottom;
            f32 hitZ;
            f32 hitY;
            f32 hitX;
            f32 hitNZ;
            f32 hitNY;
            f32 hitNX;
            f32 distance;
            Vec normal;
            Vec pos;
            PuniWork* gp = Punigp;
            top = gp->field_0x350;
            bottom = gp->field_0x354;
            if (-1000.0f != top && self->field_0x12C.y < top && self->field_0x12C.y > bottom) {
                range = TRUE;
            }
            top = gp->field_0x358;
            bottom = gp->field_0x35C;
            if (-1000.0f != top && self->field_0x12C.y < top && self->field_0x12C.y > bottom) {
                range = TRUE;
            }
            if (!range) {
                state = 1;
            } else {
                pos = self->field_0x12C;
                pos.y += 1.0f;
                normal = (Vec){0.0f, -1.0f, 0.0f};
                distance = 3.0f;
                if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                                   pos.x, pos.y, pos.z, normal.x, normal.y, normal.z) == NULL) {
                    state = 2;
                } else {
                    state = 0;
                }
            }
            if (state != 0) {
                self->field_0x58C = state;
                *(Vec*)&self->field_0x590 = self->field_0x12C;
                self->field_0x12C = Punigp->field_0x344;
                self->field_0x12C.x += randf(0.0f, 2.0f) - 1.0f;
                self->field_0x12C.z += randf(0.0f, 2.0f) - 1.0f;
                self->field_0x138 = self->field_0x12C;
                *(Vec*)&self->field_0x59C = self->field_0x12C;
            }
            self->field_0x114 = 1;
            break;
        }
        case 1:
            self->unkC = 0x10;
            self->field_0x114 = 0;
            break;
        default:
            break;
        }
        break;
    case 16:
        self->unk4 |= 0x8000;
        puniModeHagure(self);
        break;
    case 18:
        self->unk4 |= 0x8000;
        puniModePreupdraft(self);
        break;
    case 19:
        self->unk4 |= 0x8000;
        puniModeUpdraft(self);
        break;
    case 20:
        self->unk4 |= 0x8000;
        puniModeWonder(self);
        break;
    case 21:
        self->unk4 |= 0x8000;
        puniModeFunbari(self);
        break;
    case 22:
        self->unk4 |= 0x8000;
        puniModeWater(self);
        break;
    case 17:
        self->unk4 |= 0x8000;
        puniModeFall(self);
        break;
    case 23:
        self->unk4 |= 0x8000;
        switch (self->field_0x114) {
        case 0: {
            Vec diff;
            PSVECSubtract(&self->field_0x12C, &self->field_0x1F8, &diff);
            if (PSVECMag(&diff) < 10.0f) {
                *(Vec*)&self->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
                self->unkC = 0x10;
                self->field_0x114 = 0;
            } else {
                PSVECNormalize(&diff, &diff);
                diff.y = 0.0f;
                PSVECScale(&diff, &diff, 1.0f);
                PSVECSubtract((Vec*)&self->field_0x15C, &diff, (Vec*)&self->field_0x15C);
                self->field_0x174 = 3.0f;
            }
            break;
        }
        }
        break;
    case 24:
        self->unk4 &= ~0x8000;
        puniModeTrap(self);
        break;
    case 25:
        self->unk4 &= ~0x8000;
        puniModeEnter(self);
        break;
    case 26:
        self->unk4 &= ~0x8000;
        puniModeExit(self);
        break;
    case 28:
        self->unk4 &= ~0x8000;
        self->field_0x114 = 0;
        self->unk4 |= 0x800;
        *(u32*)(*(void**)self) |= 4;
        *(Vec*)&self->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
        break;
    case 27:
        self->unk4 &= ~0x8000;
        switch (self->field_0x114) {
        case 0:
            if (PSVECSquareDistance((Vec*)&Punigp->field_0x480.x, &self->field_0x12C) > Punigp->field_0x4B0) {
                Vec diff;
                PSVECSubtract((Vec*)&Punigp->field_0x480.x, &self->field_0x12C, &diff);
                PSVECNormalize(&diff, &diff);
                diff.y = 0.0f;
                PSVECScale(&diff, &diff, 1.5f);
                PSVECAdd((Vec*)&self->field_0x15C, &diff, (Vec*)&self->field_0x15C);
            } else {
                *(Vec*)&self->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
                self->unk4 &= ~0x101000;
                self->field_0x114 = 1;
            }
            break;
        case 1:
            if (PSVECSquareDistance((Vec*)&Punigp->field_0x48C.x, &self->field_0x12C) > Punigp->field_0x4B4) {
                Vec diff;
                PSVECSubtract((Vec*)&Punigp->field_0x48C.x, &self->field_0x12C, &diff);
                PSVECNormalize(&diff, &diff);
                diff.y = 0.0f;
                PSVECScale(&diff, &diff, 2.0f);
                PSVECAdd((Vec*)&self->field_0x15C, &diff, (Vec*)&self->field_0x15C);
            } else {
                self->unkC = 0x1E;
                self->unk4 &= ~0x100000;
                *(Vec*)&self->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
                self->field_0x12C = (Vec){0.0f, -1000.0f, 0.0f};
                self->field_0x138 = self->field_0x12C;
                self->field_0x114 = 0;
            }
            break;
#ifdef MUST_MATCH
        // Empty case 2 before default reproduces the original's separate default block (an
        // extra `b` to the epilogue); without it MWCC merges default into the dispatch.
        case 2:
#endif
        default:
            break;
        }
        break;
    case 29:
        self->unk4 &= ~0x8000;
        if ((void*)self->field_0x56C != NULL) {
            ((void (*)(void))self->field_0x56C)();
        }
        break;
    default:
        break;
    }
}

// Matched. The second hitCheckFilter's point/normal outputs are written as separate f32s (declared z,y,x so
// the 4-byte-scalar allocator lands x at the lowest slot) because the original packs them with the distance
// temp after the Vecs; body B (case 4) has no `unk4 & 0x1000` guard and assigns rB before qB.
void fn_19_13A90(PuniData* puni) {
    Vec rA;
    Vec qA;
    Vec nA;
    Vec vA;
    Vec rB;
    Vec qB;
    Vec nB;
    Vec vB;
    Vec rD;
    Vec qD;
    Vec nD;
    Vec vD;
    Vec rE;
    Vec qE;
    Vec nE;
    Vec vE;
    Vec svA;
    Vec reflA;
    Vec sepA;
    Vec hitNA;
    Vec hitPA;
    Vec svB;
    Vec sepB;
    Vec hitNB;
    Vec hitPB;
    Vec svD;
    Vec reflD;
    Vec sepD;
    Vec hitND;
    Vec hitPD;
    Vec svE;
    Vec reflE;
    Vec sepE;
    Vec hitNE;
    Vec hitPE;
    f32 dA;
    f32 hitN2Az;
    f32 hitN2Ay;
    f32 hitN2Ax;
    f32 hitP2Az;
    f32 hitP2Ay;
    f32 hitP2Ax;
    f32 dB;
    f32 hitN2Bz;
    f32 hitN2By;
    f32 hitN2Bx;
    f32 hitP2Bz;
    f32 hitP2By;
    f32 hitP2Bx;
    f32 dD;
    f32 hitN2Dz;
    f32 hitN2Dy;
    f32 hitN2Dx;
    f32 hitP2Dz;
    f32 hitP2Dy;
    f32 hitP2Dx;
    f32 dE;
    f32 hitN2Ez;
    f32 hitN2Ey;
    f32 hitN2Ex;
    f32 hitP2Ez;
    f32 hitP2Ey;
    f32 hitP2Ex;

    switch (puni->unkC) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 7:
    case 8:
    case 10:
    case 14:
    case 16:
    case 23:
    case 25:
    case 26:
    case 27:
        {
    f32 mag;
    f32 t;

    mag = PSVECMag((Vec*)&puni->field_0x15C);
    if (!mag) goto endA;
    if (mag > 1.0f) {
        if (((Vec*)&puni->field_0x15C)->x < 0.0f) {
            puni->field_0x180 = 270.0f;
        } else {
            puni->field_0x180 = 90.0f;
        }
    }
    if (mag > *(f32*)(puni->unk168 + 0xC)) {
        PSVECScale((Vec*)&puni->field_0x15C, (Vec*)&puni->field_0x15C,
                   *(f32*)(puni->unk168 + 0xC) / mag);
        mag = *(f32*)(puni->unk168 + 0xC);
    }
    vA = puni->field_0x12C;
    vA.y += 1.0f;
    PSVECNormalize((Vec*)&puni->field_0x15C, &nA);
    qA = vA;
    rA = nA;
    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dA = mag;
    if (puni->unk4 & 0x1000) {
        puni->field_0x210 =
            hitCheckFilter(NULL, &hitPA.x, &hitPA.y, &hitPA.z, &dA, &hitNA.x, &hitNA.y, &hitNA.z, qA.x, qA.y, qA.z,
                                rA.x, rA.y, rA.z);
    }
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->field_0x12C, (Vec*)&puni->field_0x15C, &puni->field_0x12C);
    } else {
        svA = puni->field_0x12C;
        *(Vec*)(puni->unk21C + 0x344) = hitNA;
        PSVECAdd(&puni->field_0x12C, (Vec*)&puni->field_0x15C, &puni->field_0x12C);
        if (puni->field_0x15C.y == 0.0f) {
            hitNA.y = 0.0f;
        }
        PSVECNormalize(&hitNA, &hitNA);
        C_VECReflect(&rA, &hitNA, &reflA);
        PSVECScale(&reflA, (Vec*)&puni->field_0x15C, mag);
        t = PSVECDotProduct(&rA, &hitNA);
        PSVECScale(&hitNA, &hitNA, t * (-(mag - dA)) + 0.1f);
        PSVECAdd(&puni->field_0x12C, &hitNA, &puni->field_0x12C);
        PSVECSubtract(&puni->field_0x12C, &svA, &sepA);
        dA = PSVECMag(&sepA);
        if (dA > 0.0f) {
            PSVECNormalize(&sepA, &sepA);
            puni->field_0x214 = hitCheckFilter(NULL, &hitP2Ax, &hitP2Ay, &hitP2Az, &dA, &hitN2Ax,
                                                             &hitN2Ay, &hitN2Az, svA.x, svA.y + 1.0f, svA.z, sepA.x,
                                                             sepA.y, sepA.z);
            if (puni->field_0x214 != 0) {
                puni->field_0x12C = svA;
            }
        }
    }
    if (puni->field_0x210 != 0 || puni->field_0x214 != 0) {
        puni->unk4 |= 0x400000;
    } else {
        PSVECScale((Vec*)&puni->field_0x15C, (Vec*)&puni->field_0x15C, 0.6f);
        puni->unk4 &= ~0x400000;
    }
    endA:
    puniSetYPosition(puni);
        }
        break;
    case 4:
        {
    f32 mag;
    f32 t;

    mag = PSVECMag((Vec*)&puni->field_0x15C);
    if (!mag) goto endB;
    if (mag > *(f32*)(puni->unk168 + 0xC)) {
        PSVECScale((Vec*)&puni->field_0x15C, (Vec*)&puni->field_0x15C,
                   *(f32*)(puni->unk168 + 0xC) / mag);
        mag = *(f32*)(puni->unk168 + 0xC);
    }
    vB = puni->field_0x12C;
    vB.y += 1.0f;
    PSVECNormalize((Vec*)&puni->field_0x15C, &nB);
    rB = nB;
    qB = vB;
    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dB = mag;
    puni->field_0x210 =
        hitCheckFilter(NULL, &hitPB.x, &hitPB.y, &hitPB.z, &dB, &hitNB.x, &hitNB.y, &hitNB.z, qB.x, qB.y, qB.z,
                            rB.x, rB.y, rB.z);
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->field_0x12C, (Vec*)&puni->field_0x15C, &puni->field_0x12C);
    } else if (Punigp->unk328 != 0x12) {
        svB = puni->field_0x12C;
        *(Vec*)(puni->unk21C + 0x344) = hitNB;
        PSVECAdd(&puni->field_0x12C, (Vec*)&puni->field_0x15C, &puni->field_0x12C);
        if (puni->field_0x15C.y == 0.0f) {
            hitNB.y = 0.0f;
        }
        PSVECNormalize(&hitNB, &hitNB);
        t = PSVECDotProduct(&rB, &hitNB);
        PSVECScale(&hitNB, &hitNB, t * (-(mag - dB)) + 0.1f);
        PSVECAdd(&puni->field_0x12C, &hitNB, &puni->field_0x12C);
        PSVECSubtract(&puni->field_0x12C, &svB, &sepB);
        dB = PSVECMag(&sepB);
        if (dB > 0.0f) {
            PSVECNormalize(&sepB, &sepB);
            puni->field_0x214 = hitCheckFilter(NULL, &hitP2Bx, &hitP2By, &hitP2Bz, &dB, &hitN2Bx,
                                                             &hitN2By, &hitN2Bz, svB.x, svB.y + 1.0f, svB.z, sepB.x,
                                                             sepB.y, sepB.z);
            if (puni->field_0x214 != 0) {
                puni->field_0x12C = svB;
            }
        }
    }
    if (puni->field_0x210 != 0 || puni->field_0x214 != 0) {
        puni->unk4 |= 0x400000;
    } else {
        puni->unk4 &= ~0x400000;
    }
    endB:
    puniSetYPosition(puni);
        }
        break;
    case 9:
        {
    f32 mag;
    f32 t;

    mag = PSVECMag((Vec*)&puni->field_0x15C);
    if (!mag) goto endD;
    vD = puni->field_0x12C;
    vD.y += 1.0f;
    PSVECNormalize((Vec*)&puni->field_0x15C, &nD);
    qD = vD;
    rD = nD;
    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dD = mag;
    if (puni->unk4 & 0x1000) {
        puni->field_0x210 =
            hitCheckFilter(NULL, &hitPD.x, &hitPD.y, &hitPD.z, &dD, &hitND.x, &hitND.y, &hitND.z, qD.x, qD.y, qD.z,
                                rD.x, rD.y, rD.z);
    }
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->field_0x12C, (Vec*)&puni->field_0x15C, &puni->field_0x12C);
    } else {
        svD = puni->field_0x12C;
        *(Vec*)(puni->unk21C + 0x344) = hitND;
        PSVECAdd(&puni->field_0x12C, (Vec*)&puni->field_0x15C, &puni->field_0x12C);
        if (puni->field_0x15C.y == 0.0f) {
            hitND.y = 0.0f;
        }
        PSVECNormalize(&hitND, &hitND);
        C_VECReflect(&rD, &hitND, &reflD);
        PSVECScale(&reflD, (Vec*)&puni->field_0x15C, mag);
        t = PSVECDotProduct(&rD, &hitND);
        PSVECScale(&hitND, &hitND, t * (-(mag - dD)) + 0.1f);
        PSVECAdd(&puni->field_0x12C, &hitND, &puni->field_0x12C);
        PSVECSubtract(&puni->field_0x12C, &svD, &sepD);
        dD = PSVECMag(&sepD);
        if (dD > 0.0f) {
            PSVECNormalize(&sepD, &sepD);
            puni->field_0x214 = hitCheckFilter(NULL, &hitP2Dx, &hitP2Dy, &hitP2Dz, &dD, &hitN2Dx,
                                                             &hitN2Dy, &hitN2Dz, svD.x, svD.y + 1.0f, svD.z, sepD.x,
                                                             sepD.y, sepD.z);
            if (puni->field_0x214 != 0) {
                puni->field_0x12C = svD;
            }
        }
    }
    if (puni->field_0x210 != 0 || puni->field_0x214 != 0) {
        puni->unk4 |= 0x400000;
    } else {
        puni->unk4 &= ~0x400000;
    }
    endD:
    puniSetYPosition(puni);
        }
        break;
    case 29:
        if ((u32)puni->field_0x570 != 0) {
            ((void (*)(PuniData*))puni->field_0x570)(puni);
        }
        puniSetYPosition(puni);
        break;
    case 5:
    case 6:
    case 13:
        puniSetYPosition(puni);
        break;
    case 24:
        {
    f32 mag;
    f32 t;

    mag = PSVECMag((Vec*)&puni->field_0x15C);
    if (!mag) goto endE;
    if (mag > 1.0f) {
        if (((Vec*)&puni->field_0x15C)->x < 0.0f) {
            puni->field_0x180 = 270.0f;
        } else {
            puni->field_0x180 = 90.0f;
        }
    }
    if (mag > *(f32*)(puni->unk168 + 0xC)) {
        PSVECScale((Vec*)&puni->field_0x15C, (Vec*)&puni->field_0x15C,
                   *(f32*)(puni->unk168 + 0xC) / mag);
        mag = *(f32*)(puni->unk168 + 0xC);
    }
    vE = puni->field_0x12C;
    vE.y += 1.0f;
    PSVECNormalize((Vec*)&puni->field_0x15C, &nE);
    qE = vE;
    rE = nE;
    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dE = mag;
    if (puni->unk4 & 0x1000) {
        puni->field_0x210 =
            hitCheckFilter(NULL, &hitPE.x, &hitPE.y, &hitPE.z, &dE, &hitNE.x, &hitNE.y, &hitNE.z, qE.x, qE.y, qE.z,
                                rE.x, rE.y, rE.z);
    }
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->field_0x12C, (Vec*)&puni->field_0x15C, &puni->field_0x12C);
    } else {
        svE = puni->field_0x12C;
        *(Vec*)(puni->unk21C + 0x344) = hitNE;
        PSVECAdd(&puni->field_0x12C, (Vec*)&puni->field_0x15C, &puni->field_0x12C);
        if (puni->field_0x15C.y == 0.0f) {
            hitNE.y = 0.0f;
        }
        PSVECNormalize(&hitNE, &hitNE);
        C_VECReflect(&rE, &hitNE, &reflE);
        PSVECScale(&reflE, (Vec*)&puni->field_0x15C, mag);
        t = PSVECDotProduct(&rE, &hitNE);
        PSVECScale(&hitNE, &hitNE, t * (-(mag - dE)) + 0.1f);
        PSVECAdd(&puni->field_0x12C, &hitNE, &puni->field_0x12C);
        PSVECSubtract(&puni->field_0x12C, &svE, &sepE);
        dE = PSVECMag(&sepE);
        if (dE > 0.0f) {
            PSVECNormalize(&sepE, &sepE);
            puni->field_0x214 = hitCheckFilter(NULL, &hitP2Ex, &hitP2Ey, &hitP2Ez, &dE, &hitN2Ex,
                                                             &hitN2Ey, &hitN2Ez, svE.x, svE.y + 1.0f, svE.z, sepE.x,
                                                             sepE.y, sepE.z);
            if (puni->field_0x214 != 0) {
                puni->field_0x12C = svE;
            }
        }
    }
    if (puni->field_0x210 != 0 || puni->field_0x214 != 0) {
        puni->unk4 |= 0x400000;
    } else {
        PSVECScale((Vec*)&puni->field_0x15C, (Vec*)&puni->field_0x15C, 0.6f);
        puni->unk4 &= ~0x400000;
    }
    endE:
    puniSetYPosition(puni);
        }
        break;
    }
}
void puniOption(void) {
    PuniData* puni;
    PuniData* puni2;
    PuniData* begin;
    PuniData* end;
    Vec tmp;
    f32 dist;
    f32 range;
    f32 radius;
    f32 f0;
    f32 f1;

    for (puni = Punidat; puni < &Punidat[PUNI_DATA_MAX]; puni++) {
        puni->unk4 &= ~0x20000000;
        if (!(puni->unk4 & 0x400)) {
            continue;
        }
        if (!(puni->unk4 & 0x100000)) {
            continue;
        }
        if (puni < &Punidat[110]) {
            begin = Punidat;
            end = &Punidat[109];
        } else {
            begin = &Punidat[110];
            end = &Punidat[209];
        }
        for (puni2 = begin; puni2 <= end; puni2++) {
            if (!(puni2->unk4 & 0x400)) {
                continue;
            }
            if (!(puni2->unk4 & 0x100000)) {
                continue;
            }
            if (puni == puni2) {
                continue;
            }
            dist = PSVECDistance(&puni->field_0x12C, &puni2->field_0x12C);
            radius = puni->field_0x190 + puni2->field_0x190;
            range = puni->field_0x198 + (radius + puni->field_0x194);
            if (puni2->field_0x8 & 0x20) {
                if (dist < radius) {
                    PSVECSubtract(&puni->field_0x12C, &puni2->field_0x12C, &tmp);
                    if (PSVECMag(&tmp) == 0.0f) {
                        tmp.x = 1.0f;
                        dist = 1.0f;
                    }
                    PSVECNormalize(&tmp, &tmp);
                    PSVECScale(&tmp, &tmp, radius - dist);
                    PSVECAdd((Vec*)&puni->field_0x15C, &tmp, (Vec*)&puni->field_0x15C);
                }
            } else if (dist <= range) {
                if (dist < range - puni->field_0x194) {
                    puni->unk4 |= 0x20000000;
                }
                if (!(puni->unk4 & 0x10000000) &&
                    dist < puni->field_0x190 + puni2->field_0x190) {
                    puni->unk4 |= 0x10000000;
                }
                if (puni->unk4 & 0x10000000) {
                    PSVECSubtract(&puni->field_0x12C, &puni2->field_0x12C, &tmp);
                    if (PSVECMag(&tmp) == 0.0f) {
                        tmp.x = 1.0f;
                        dist = 1.0f;
                    }
                    PSVECNormalize(&tmp, &tmp);
                    f0 = (range - dist) / range;
                    f1 = 0.85f * (0.3f * f0) + 0.044999994f;
                    tmp.y = 0.0f;
                    PSVECScale(&tmp, &tmp, f1);
                    PSVECAdd((Vec*)&puni->field_0x15C, &tmp, (Vec*)&puni->field_0x15C);
                    PSVECSubtract((Vec*)&puni2->field_0x15C, &tmp, (Vec*)&puni2->field_0x15C);
                }
            }
        }
    }
    for (puni = Punidat; puni < &Punidat[PUNI_DATA_MAX]; puni++) {
        if (!(puni->unk4 & 0x400)) {
            continue;
        }
        if (!(puni->unk4 & 0x100000)) {
            continue;
        }
        if (!(puni->unk4 & 0x20000000)) {
            puni->unk4 &= ~0x10000000;
        }
        if (PSVECMag((Vec*)&puni->field_0x15C) >= 8.0f) {
            PSVECNormalize((Vec*)&puni->field_0x15C, (Vec*)&puni->field_0x15C);
            PSVECScale((Vec*)&puni->field_0x15C, (Vec*)&puni->field_0x15C, 8.0f);
        }
    }
}
void unk_mri_00014ad4(void) {
    PuniData* puni;
    PuniData* puni2;
    Vec tmp;
    f32 dist;
    f32 range;
    f32 radius;
    f32 f0;
    f32 f1;

    for (puni = Punidat; puni <= &Punidat[109]; puni++) {
        puni->unk4 &= ~0x20000000;
        if (!(puni->unk4 & 0x400)) {
            continue;
        }
        if (!(puni->unk4 & 0x100000)) {
            continue;
        }
        for (puni2 = Punidat; puni2 <= &Punidat[109]; puni2++) {
            if (!(puni2->unk4 & 0x400)) {
                continue;
            }
            if (!(puni2->unk4 & 0x100000)) {
                continue;
            }
            if (puni == puni2) {
                continue;
            }
            dist = PSVECDistance(&puni->field_0x12C, &puni2->field_0x12C);
            radius = puni->field_0x190 + puni2->field_0x190;
            range = puni->field_0x198 + (radius + puni->field_0x194);
            if (puni2->field_0x8 & 0x20) {
                if (dist < radius) {
                    PSVECSubtract(&puni->field_0x12C, &puni2->field_0x12C, &tmp);
                    if (PSVECMag(&tmp) == 0.0f) {
                        tmp.x = 1.0f;
                        dist = 1.0f;
                    }
                    PSVECNormalize(&tmp, &tmp);
                    PSVECScale(&tmp, &tmp, radius - dist);
                    PSVECAdd((Vec*)&puni->field_0x15C, &tmp, (Vec*)&puni->field_0x15C);
                }
            } else if (dist <= range) {
                if (dist < range - puni->field_0x194) {
                    puni->unk4 |= 0x20000000;
                }
                if (!(puni->unk4 & 0x10000000) &&
                    dist < puni->field_0x190 + puni2->field_0x190) {
                    puni->unk4 |= 0x10000000;
                }
                if (puni->unk4 & 0x10000000) {
                    PSVECSubtract(&puni->field_0x12C, &puni2->field_0x12C, &tmp);
                    if (PSVECMag(&tmp) == 0.0f) {
                        tmp.x = 1.0f;
                        dist = 1.0f;
                    }
                    PSVECNormalize(&tmp, &tmp);
                    f0 = (range - dist) / range;
                    f1 = 0.85f * (0.3f * f0) + 0.044999994f;
                    tmp.y = 0.0f;
                    PSVECScale(&tmp, &tmp, f1);
                    PSVECAdd((Vec*)&puni->field_0x15C, &tmp, (Vec*)&puni->field_0x15C);
                    PSVECSubtract((Vec*)&puni2->field_0x15C, &tmp, (Vec*)&puni2->field_0x15C);
                }
            }
        }
    }
    for (puni = Punidat; puni <= &Punidat[109]; puni++) {
        if (!(puni->unk4 & 0x400)) {
            continue;
        }
        if (!(puni->unk4 & 0x100000)) {
            continue;
        }
        if (!(puni->unk4 & 0x20000000)) {
            puni->unk4 &= ~0x10000000;
        }
        if (PSVECMag((Vec*)&puni->field_0x15C) >= 8.0f) {
            PSVECNormalize((Vec*)&puni->field_0x15C, (Vec*)&puni->field_0x15C);
            PSVECScale((Vec*)&puni->field_0x15C, (Vec*)&puni->field_0x15C, 8.0f);
        }
    }
}
void puniSound(void) {
    s32 i;
    s32 num = 0;
    s32 special = 0;
    s32 divisor;
    s32 target;
    s32 k;
    s32 count;
    PuniData* pd;
    PuniData* p;
    PuniData* q;
    s32 num2;
    Vec pos;

    for (i = 1; i <= 0x66; i++) {
        p = &Punidat[i];
        if ((p->unk4 & 0x400) != 0 && (p->field_0x11C == 1 || p->field_0x11C == 2)) {
            if (i == 3) {
                special++;
            } else {
                num++;
            }
        }
    }

    Punigp->field_0x45C = num;
    if (num == 0) {
        Punigp->field_0x460 = 0;
        divisor = 5;
    } else if (num <= 2) {
        Punigp->field_0x460 = 1;
        divisor = 5;
    } else if (num <= 0xB) {
        Punigp->field_0x460 = 2;
        divisor = 4;
    } else if (num <= 0x1E) {
        Punigp->field_0x460 = 3;
        divisor = 3;
    } else if (num <= 0x32) {
        Punigp->field_0x460 = 4;
        divisor = 2;
    } else if (num <= 0x46) {
        Punigp->field_0x460 = 5;
        divisor = 2;
    } else {
        Punigp->field_0x460 = 6;
        divisor = 2;
    }

    if (Punigp->field_0x460 != 0 && (gp->retraceCount % divisor) == 0) {
        count = Punigp->field_0x45C;
        target = rand() % count + 1;
        k = 0;
        for (i = 1; i <= 0x66; i++) {
            p = &Punidat[i];
            if ((p->unk4 & 0x400) != 0 && (p->field_0x11C == 1 || p->field_0x11C == 2)) {
                k++;
                if (k == target) {
                    break;
                }
            }
        }
        pos = p->field_0x12C;

        count = Punigp->field_0x460;
        switch (rand() % count + 1) {
        case 1:
            psndSFXOn_3D("SFX_STG2_PUNI_MOVE2_1\000\000", &pos);
            break;
        case 2:
            psndSFXOn_3D("SFX_STG2_PUNI_MOVE2_2\000\000", &pos);
            break;
        case 3:
            psndSFXOn_3D("SFX_STG2_PUNI_MOVE2_3\000\000", &pos);
            break;
        case 4:
            psndSFXOn_3D("SFX_STG2_PUNI_MOVE2_4\000\000", &pos);
            break;
        case 5:
            psndSFXOn_3D("SFX_STG2_PUNI_MOVE2_5\000\000", &pos);
            break;
        case 6:
            psndSFXOn_3D("SFX_STG2_PUNI_MOVE2_6\000\000", &pos);
            break;
        }
    }

    pd = Punidat;
    if (special != 0) {
        if (*(u32*)((u8*)Punigp + 0x478) == 0) {
            *(u32*)((u8*)Punigp + 0x478) = psndSFXOn_3D("SFX_STG2_TOGETTI_MOVE1\000", &pd[3].field_0x12C);
        }
    } else if (*(u32*)((u8*)Punigp + 0x478) != 0) {
        psndSFXOff(*(u32*)((u8*)Punigp + 0x478));
        *(u32*)((u8*)Punigp + 0x478) = 0;
    }
    if (*(u32*)((u8*)Punigp + 0x478) != 0) {
        psndSFX_pos(*(u32*)((u8*)Punigp + 0x478), &pd[3].field_0x12C);
    }

    num2 = 0;
    for (i = 0x6E; i <= 0xD1; i++) {
        q = &Punidat[i];
        if ((q->unk4 & 0x400) != 0 && (q->field_0x11C == 1 || q->field_0x11C == 2)) {
            num2++;
        }
    }
    Punigp->field_0x46C = num2;
    if (num2 == 0) {
        Punigp->field_0x470 = 0;
    } else if (num2 <= 2) {
        Punigp->field_0x470 = 1;
    } else if (num2 <= 0xB) {
        Punigp->field_0x470 = 2;
    } else {
        Punigp->field_0x470 = 3;
    }

    if (Punigp->field_0x470 != Punigp->field_0x474) {
        if ((u32)Punigp->field_0x468 != 0) {
            psndSFXOff(Punigp->field_0x468);
        }
        switch (Punigp->field_0x470) {
        case 1:
            Punigp->field_0x468 = psndSFXOn("SFX_STG2_TOGE_MOVE1");
            break;
        case 2:
            Punigp->field_0x468 = psndSFXOn("SFX_STG2_TOGE_MOVE2");
            break;
        case 3:
            Punigp->field_0x468 = psndSFXOn("SFX_STG2_TOGE_MOVE3");
            break;
        }
    }
    Punigp->field_0x474 = Punigp->field_0x470;
    Punigp->field_0x4C0 = 0;
    Punigp->field_0x4C4 = 0;
}

// No structural diff; only register colouring differs (first loop target pd/i = r27/r28, second j/i =
// r27/r29, else i/pd = r31/r29) and the `debug$` static gets a different generated suffix.
// NONMATCHING (99.03%): block-scoping the loop vars lifted 94.28% here.
void puni_puni_main(void) {
    static PuniData* debug;
    MarioWork* mario;
    PuniData* p0;
    Vec v;
    Vec tmp;
    Vec tmp2;
    Vec tmp3;

    mario = marioGetPtr();
    p0 = *(PuniData**)((u8*)Punigp + 0x360);
    p0->field_0x12C = mario->position;
    tmp = p0->field_0x12C;
    puniMapFindbuff(&tmp);
    puniModeALLCheck();

    if ((Punigp->field_0x324 & 0x20) != 0) {
        PuniData* pd = &Punidat[1];
        s32 i;
        s16 old;
        for (i = 1; i < 0xD3; i++, pd++) {
            PSVECSubtract(&pd->field_0x12C, (Vec*)&pd->field_0x138, &v);
            pd->field_0x1AC = PSVECMag(&v);
            if ((pd->unk4 & 0x400) != 0) {
                pd->unk4 &= ~0x06000000;
                pd->field_0x8 &= ~0x4;
                *(u16*)((u8*)pd + 0x586) = 0;
                debug = pd;
                if (pd->field_0x1AC > 0.001f || pd->unkC == 0xD || pd->unkC == 0xA) {
                    pd->field_0x57A = pd->field_0x57C;
                } else {
                    (pd->field_0x57A)--;
                    if (pd->field_0x57A == 0) {
                        old = pd->field_0x574;
                        if (old < pd->field_0x576) {
                            pd->field_0x574 = old + pd->field_0x578;
                            if (pd->field_0x574 > pd->field_0x576) {
                                pd->field_0x574 = pd->field_0x576;
                            }
                            if (pd->field_0x574 > old) {
                                pd->field_0x8 |= 1;
                                pd->field_0x121 = 0;
                            }
                        }
                        pd->field_0x57A = pd->field_0x57C;
                    }
                }
            }
        }
    }

    if ((Punigp->field_0x324 & 0x20) != 0) {
        s32 j;
        for (j = 0; j < 2; j++) {
            PuniData* pd = &Punidat[1];
            s32 i;
            PartyEntry* party;
            PuniSearchEntry* sei;
            s32 k;
            for (i = 1; i < 0xD3; i++, pd++) {
                if ((pd->unk4 & 0x400) != 0) {
                    if (j == 0) {
                        if (pd->unkC != 7) {
                            continue;
                        }
                        pd->unk4 |= 0x02000000;
                    } else {
                        if ((pd->unk4 & 0x02000000) != 0) {
                            continue;
                        }
                        pd->unk4 |= 0x02000000;
                    }
                    if ((pd->unk4 & 0x04000000) == 0) {
                    pd->unk4 |= 0x04000000;
                    if ((pd->field_0x8 & 0x10) != 0) {
                        pd->field_0x8 &= ~0x10;
                        pd->unkC = 0xC;
                        pd->field_0x114 = 0;
                    }
                    party = partyGetPtr(marioGetPartyId());
                    pd->field_0x1A0 = 0.0f;
                    if (Punigp->field_0x4CC == 0 && party != NULL) {
                        tmp2 = pd->field_0x12C;
                        pd->field_0x1A0 = cloudGetBreathPower(10.0f, &tmp2);
                    }
                    if (32.0 < __fabsf(pd->field_0x12C.y - mario->position.y)) {
                        pd->field_0x1A4 = 1000.0f;
                    } else {
                        pd->field_0x1A4 = PSVECDistance(&pd->field_0x12C, &mario->position);
                    }
                    sei = (PuniSearchEntry*)((u8*)Punigp + 0x374);
                    pd->field_0x1B4 = -1;
                    pd->field_0x1B8 = 1e+06f;
                    pd->field_0x1BC = -1;
                    pd->field_0x1C0 = 1e+06f;
                    for (k = 0; k < 10; k++, sei++) {
                        if ((sei->flags & 1) != 0 && !(32.0 < __fabsf(pd->field_0x12C.y - sei->pos.y))) {
                            if ((sei->flags & 4) != 0) {
                                f32 d = PSVECSquareDistance(&pd->field_0x12C, &sei->pos);
                                if (pd->field_0x1C0 > d) {
                                    pd->field_0x1BC = k;
                                    pd->field_0x1C0 = d;
                                }
                            }
                            if ((sei->flags & 2) != 0) {
                                f32 d = PSVECSquareDistance(&pd->field_0x12C, &sei->pos);
                                if (pd->field_0x1B8 > d) {
                                    pd->field_0x1B4 = k;
                                    pd->field_0x1B8 = d;
                                }
                            }
                        }
                    }
                    }
                    unk_mri_00012d58(pd);
                }
            }
        }
        puniOption();
    } else {
        PuniData* pd = &Punidat[1];
        s32 i;
        PartyEntry* party;
        PuniSearchEntry* sei;
        s32 k;
        for (i = 1; i <= 0x6D; i++, pd++) {
            if ((pd->unk4 & 0x400) != 0) {
                party = partyGetPtr(marioGetPartyId());
                pd->field_0x1A0 = 0.0f;
                if (Punigp->field_0x4CC == 0 && party != NULL) {
                    tmp3 = pd->field_0x12C;
                    pd->field_0x1A0 = cloudGetBreathPower(10.0f, &tmp3);
                }
                if (32.0 < __fabsf(pd->field_0x12C.y - mario->position.y)) {
                    pd->field_0x1A4 = 1000.0f;
                } else {
                    pd->field_0x1A4 = PSVECDistance(&pd->field_0x12C, &mario->position);
                }
                sei = (PuniSearchEntry*)((u8*)Punigp + 0x374);
                pd->field_0x1B4 = -1;
                pd->field_0x1B8 = 1e+06f;
                pd->field_0x1BC = -1;
                pd->field_0x1C0 = 1e+06f;
                for (k = 0; k < 10; k++, sei++) {
                    if ((sei->flags & 1) != 0 && !(32.0 < __fabsf(pd->field_0x12C.y - sei->pos.y))) {
                        if ((sei->flags & 4) != 0) {
                            f32 d = PSVECSquareDistance(&pd->field_0x12C, &sei->pos);
                            if (pd->field_0x1C0 > d) {
                                pd->field_0x1BC = k;
                                pd->field_0x1C0 = d;
                            }
                        }
                        if ((sei->flags & 2) != 0) {
                            f32 d = PSVECSquareDistance(&pd->field_0x12C, &sei->pos);
                            if (pd->field_0x1B8 > d) {
                                pd->field_0x1B4 = k;
                                pd->field_0x1B8 = d;
                            }
                        }
                    }
                }
                unk_mri_00012d58(pd);
            }
        }
        unk_mri_00014ad4();
    }

    if ((Punigp->field_0x324 & 0x20) != 0) {
        s32 i;
        PuniData* pd;
        PuniPose* pose;
        pose = puniGetPosePtr();
        pd = &Punidat[1];
        for (i = 1; i < 0xD3; i++, pd++) {
            if ((pd->unk4 & 0x400) != 0) {
                if ((pd->unk4 & 0x200) == 0) {
                    *(Vec*)&pd->field_0x138 = pd->field_0x12C;
                    fn_19_13A90(pd);
                }
                puniAnimation(pd, pose);
            }
            pose++;
        }
    } else {
        s32 i;
        PuniData* pd;
        PuniPose* pose;
        pose = puniGetPosePtr();
        pd = &Punidat[1];
        for (i = 1; i <= 0x6D; i++, pd++) {
            if ((pd->unk4 & 0x400) != 0) {
                if ((pd->unk4 & 0x200) == 0) {
                    *(Vec*)&pd->field_0x138 = pd->field_0x12C;
                    fn_19_13A90(pd);
                }
                puniAnimation(pd, pose);
            }
            pose++;
        }
    }

    puniSound();
    switch (Punigp->field_0x43C) {
    case 2:
        if (Punigp->field_0x440 == 2) {
            Punigp->field_0x43C = 0;
        }
        break;
    case 0:
        break;
    case 1:
        break;
    case 3:
        break;
    }
    Punigp->field_0x440 = Punigp->field_0x43C;
}
static inline Vec puniMakeVecZero(f32 x, f32 y, f32 z) {
    Vec v = { 0 };
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}
void puni_puni_dispent(void) {
    PuniPose* pose;
    s32 i;
    Vec pos;

    dispEntry(4, 0.0f, 1, (DispCallback)puni_puni_disp, NULL);
    pose = puniGetPosePtr();
    for (i = 0; i < 0xD2; i++) {
        pos = puniMakeVecZero(pose->matrix[0][0][3], pose->matrix[0][1][3], pose->matrix[0][2][3]);
        dispEntry(4, dispCalcZ(&pos), 2, (DispCallback)puni_puni_shadow_disp, pose);
        pose++;
    }
    dispEntry(4, 0.0f, 1, evt_icon_disp_func, NULL);
}
USER_FUNC(evt_puni_mem_init) {
    Punigp = _mapAlloc(sizeof(PuniWork));
    memset(Punigp, 0, sizeof(PuniWork));
    Punidat = _mapAlloc(sizeof(PuniData) * PUNI_DATA_MAX);
    memset(Punidat, 0, sizeof(PuniData) * PUNI_DATA_MAX);
    Punitex = _mapAlloc(sizeof(PuniTexInfo) * PUNI_TEX_INFO_MAX);
    memset(Punitex, 0, sizeof(PuniTexInfo) * PUNI_TEX_INFO_MAX);
    return EVT_RETURN_DONE;
}
static inline void puniPaternInitAll(PuniPaternData* pp, PuniPaternData* tp) {
    s32 i;
    s16* ep;
    s32 j;
    s16 v;

    for (i = 0; i < 10; i++) {
        pp->pat[i] = puni_patern_table[i];
        ep = &pp->end[i];
        j = 0;
        for (;;) {
            v = ((s16*)puni_patern_table[i])[j];
            if (v == -1) {
                *ep = v;
                break;
            }
            if (v == -2) {
                *ep = v;
                break;
            }
            j++;
        }
        pp->num[i] = j;

        tp->pat[i] = toge_patern_table[i];
        ep = &tp->end[i];
        j = 0;
        for (;;) {
            v = ((s16*)toge_patern_table[i])[j];
            if (v == -1) {
                *ep = v;
                break;
            }
            if (v == -2) {
                *ep = v;
                break;
            }
            j++;
        }
        tp->num[i] = j;
    }
}


// NONMATCHING (98.81%): the inlined puniPaternInitAll helper reproduces the scan exactly;
// the only remaining diff is register colouring of the pattern pointers and loop temps
// (target pp/tp/ep/v/j = r9/r8/r12/r11/r10, ours r11/r12/r10/r9/r8).
USER_FUNC(evt_puni_init) {
    s32* args = event->args;
    s32 count;
    PuniTexInfo* tex;
    void (*cb0)(void);
    void (*cb1)(void);
    void (*cb2)(void);
    PuniPaternData* pp;
    PuniPaternData* tp;

    count = evtGetValue(event, args[0]);
    tex = Punitex;
    cb0 = (void (*)(void))evtGetValue(event, args[1]);
    cb1 = (void (*)(void))evtGetValue(event, args[2]);
    cb2 = (void (*)(void))evtGetValue(event, args[3]);

    if (isFirstCall) {
        puniInit();
        return EVT_RETURN_BLOCK;
    }

    pp = (PuniPaternData*)((u8*)Punigp + 0x4);
    tp = (PuniPaternData*)((u8*)Punigp + 0x54);
    puniPaternInitAll(pp, tp);

    puniAnimationInit2();
    puniEntry(count, tex, cb0, cb1, cb2);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_set_yuka) {
    s32* args = event->args;

    Punigp->unk328 = evtGetValue(event, args[0]) + 1;
    Punigp->field_0x32C = (f32)evtGetValue(event, args[1]);
    Punigp->field_0x330 = (f32)evtGetValue(event, args[2]);
    Punigp->field_0x334 = (f32)evtGetValue(event, args[3]);
    Punigp->field_0x338 = (f32)evtGetValue(event, args[4]);
    Punigp->field_0x33C = evtGetValue(event, args[5]);
    Punigp->field_0x340 = evtGetValue(event, args[6]);
    swByteSet(0x299, Punigp->unk328);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_set_hamari) {
    s32* args = event->args;

    Punigp->field_0x344.x = (f32)evtGetValue(event, args[0]);
    Punigp->field_0x344.y = (f32)evtGetValue(event, args[1]);
    Punigp->field_0x344.z = (f32)evtGetValue(event, args[2]);
    Punigp->field_0x350 = (f32)evtGetValue(event, args[3]);
    Punigp->field_0x354 = (f32)evtGetValue(event, args[4]);
    Punigp->field_0x358 = (f32)evtGetValue(event, args[5]);
    Punigp->field_0x35C = (f32)evtGetValue(event, args[6]);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_set_maze) {
    switch (Punigp->field_0x340) {
    case 0:
        break;
    case 1:
        puniMapSetData(50.0f, -1.0f, 13, 5, puniMapdata_MRI_06);
        break;
    case 2:
        if (swGet(0xB22) != 0) {
            puniMapSetData(150.0f, -1.0f, 7, 6, puniMapdata_MRI_09_01);
        } else {
            puniMapSetData(150.0f, -1.0f, 7, 6, puniMapdata_MRI_09);
        }
        break;
    case 3:
        if (swGet(0xB33) != 0) {
            puniMapSetData(-269.0f, -320.0f, 11, 9, puniMapdata_MRI_16);
        }
        break;
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_main) {
    puniMain();
    return 0;
}
f32 puni_init_posy(f32 y) {
    f32 d = __fabs(y - Punigp->field_0x32C);
    f32 r = Punigp->field_0x32C;
    f32 t;

    t = __fabs(y - Punigp->field_0x330);
    if (d > t) {
        d = t;
        r = Punigp->field_0x330;
    }
    t = __fabs(y - Punigp->field_0x334);
    if (d > t) {
        d = t;
        r = Punigp->field_0x334;
    }
    t = __fabs(y - Punigp->field_0x338);
    if (d > t) {
        r = Punigp->field_0x338;
    }
    return r;
}
void puni_init_para(PuniData* punidat, f32 x, f32 y, f32 z) {
    PuniData* pd = punidat;

    pd->field_0x114 = 0;
    pd->field_0x11C = 0;
    pd->field_0x11E = 1000;
    pd->field_0x12C.x = x;
    pd->field_0x12C.y = y;
    pd->field_0x12C.z = z;
    pd->field_0x138 = pd->field_0x12C;
    pd->field_0x190 = 6.0f;
    pd->field_0x194 = 2.0f;
    pd->field_0x198 = 3.0f;
    pd->field_0x574 = 0x3C;
    pd->field_0x576 = 0x3C;
    pd->field_0x578 = 10;
    pd->field_0x57A = 0x3C;
    pd->field_0x57C = 0x3C;
    pd->field_0x57E = 20;
    pd->field_0x580 = 10;
    pd->field_0x582 = 10;
    pd->field_0x584 = 0;
    pd->field_0x586 = 0;
    pd->field_0x588 = 50;
    pd->field_0x58A = 70;
    pd->field_0x1D0 = 0;
    pd->field_0x1D4 = 0;
    pd->field_0x1D8 = 0;
    PuniDokuritsu(pd);
    pd->unk4 |= 0x1401;
}
void toge_init_para(PuniData* punidat, f32 x, f32 y, f32 z) {
    PuniData* pd = punidat;

    pd->field_0x11C = 0;
    pd->field_0x114 = 0;
    pd->field_0x11E = 1000;
    pd->field_0x12C.x = x;
    pd->field_0x12C.y = y;
    pd->field_0x12C.z = z;
    pd->field_0x138 = pd->field_0x12C;
    pd->field_0x190 = 6.0f;
    pd->field_0x194 = 2.0f;
    pd->field_0x198 = 3.0f;
    pd->field_0x574 = 0x3C;
    pd->field_0x576 = 0x3C;
    pd->field_0x578 = 10;
    pd->field_0x57A = 0x3C;
    pd->field_0x57C = 0x3C;
    pd->field_0x57E = 20;
    pd->field_0x580 = 10;
    pd->field_0x582 = 10;
    pd->field_0x584 = 0;
    pd->field_0x586 = 0;
    pd->field_0x588 = 50;
    pd->field_0x58A = 70;
    pd->unk4 |= 0x00101402;
}

// NONMATCHING (96.4%): the inlined puni_init_posy and both randf calls match; the target
// materialises posy in f27 (`fmr f27, f29` per case) and keeps rz in f26, while the compiler
// coalesces posy into the inline's f29 and uses f27 for rz. Pure coalescing.
USER_FUNC(evt_puni_rnd_pos) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 arg1 = args[1];
    s32 arg2 = args[2];
    s32 arg3 = args[3];
    f32 x = evtGetFloat(event, arg1);
    f32 posy = puni_init_posy(evtGetFloat(event, arg2));
    f32 z = evtGetFloat(event, arg3);
    f32 rx;
    f32 rz;

    switch (type) {
    case 0:
        rx = randf(x - 10.0f, x + 10.0f);
        rz = randf(z - 10.0f, z + 10.0f);
        break;
    case 1:
        rx = randf(x - 50.0f, x + 50.0f);
        rz = randf(z - 50.0f, z + 50.0f);
        break;
    }
    evtSetFloat(event, arg1, rx);
    evtSetFloat(event, arg2, posy);
    evtSetFloat(event, arg3, rz);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_get_systemmode) {
    evtSetValue(event, event->args[0], Punigp->field_0x0);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_set_systemmode) {
    Punigp->field_0x0 = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_wait_systemmode) {
    s32 mode = evtGetValue(event, event->args[0]);

    if (Punigp->field_0x0 != mode) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (99.3%): logic, constants, offsets and control flow all match. Diffs are pooled rodata
// symbols (double_to_int_mri_000253a0 and 5 sprintf strings are anonymous @N@ until the unit is done),
// int->float conversion temps (target r4/r3/r5, ours r3/r5/r6), flag/cond reusing b/c not f/base.
USER_FUNC(evt_puni_debug_info) {
    char buf[256];
    s32 i;

    for (i = 1; i <= 0x66; i++) {
        s32 xv;
        s32 yv;
        s32 zv;
        f32 x;
        f32 y;
        f32 z;
        s32 flag;
        s32 cond;
        s32 a;
        u8 c;
        u8 b;
        u8 d;
        u8 e;
        u8 f;
        u8 g;
        s32 base;

        base = (i - 1) * 7;
        a = (u8)_swByteGet(base);
        b = _swByteGet(base + 1);
        c = _swByteGet(base + 2);
        d = _swByteGet(base + 3);
        e = _swByteGet(base + 4);
        f = _swByteGet(base + 5);
        g = _swByteGet(base + 6);

        xv = puni_sext16(b + (c << 8));
        yv = puni_sext16(d + (e << 8));
        zv = puni_sext16(f + (g << 8));
        x = (f32)xv;
        y = (f32)yv;
        z = (f32)zv;
        if (i >= 0x67) {
            flag = 0;
        } else {
            flag = (swByteGet(0x28C + i / 8) >> (i % 8)) & 1;
        }
        cond = 0;
        if (i == 1) {
            if (swGet(0xB3C) == 0) {
                cond = 1;
            }
        } else if (i == 2) {
            if (swGet(0xB3D) == 0) {
                cond = 1;
            }
        } else if (i == 4) {
            if (swGet(0xB3F) == 0) {
                cond = 1;
            }
        } else if (i == 3) {
            if (swGet(0xB3E) == 0) {
                cond = 1;
            }
        } else if (swByteGet(0x28B) + 5 <= i) {
            cond = 1;
        }
        if (!flag) {
            if (!cond) {
                if (!a) {
                    sprintf(buf, "No%03d,ついて来ています\n", i);
                } else {
                    sprintf(buf, "No%03d,はぐれています area = mri_%02d, x = %04d, y = %04d, z = %04d,\n", i, a - 1,
                            (s32)x, (s32)y, (s32)z);
                }
            } else {
                sprintf(buf, "No%03d,＜まだ仲間になっていません＞\n", i);
            }
        } else {
            sprintf(buf, "No%03d,＜トラップにかかっています＞\n", i);
        }
        if (i == 1 && swGet(0xB14) != 0) {
            sprintf(buf, "No%03d,＜mri_03の牢屋に滞在しています＞\n", i);
        }
    }
    return EVT_RETURN_DONE;
}
static inline f32 puniRandfSetup(f32 a, f32 b) {
    f32 f = (f32)rand() / 32767.0f;
    f32 d = b - a;
    return f * d + a;
}


// NONMATCHING (99.46%): logic, constants and offsets match; declaring i before p matches the
// original's p=r28/i=r29. The remaining diffs are register allocation (event r22 vs r23, args r27
// vs r22, mario r31 vs r30, ...) and the anonymous float-pool symbols.
USER_FUNC(evt_puni_setup) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 a = evtGetValue(event, args[0]);
    s32 b;
    s32 i;
    PuniData* p;
    s32 j;
    s32 num;
    BeroEntry** beros;
    f32 sx, sy, sz;
    f32 ex, ey, ez;

    evtGetValue(event, args[1]);
    b = evtGetValue(event, args[2]);

    Punigp->field_0x43C = 0;
    if (a == 2) {
        Punigp->field_0x324 &= ~0xf;
        switch (b) {
        case 6:
            Punigp->field_0x324 |= 1;
            break;
        case 2:
            Punigp->field_0x324 |= 2;
            break;
        case 8:
            Punigp->field_0x324 |= 4;
            break;
        case 9:
            Punigp->field_0x324 |= 8;
            break;
        }
    }
    if (Punigp->unk328 == 9 || Punigp->unk328 == 0xc)
        Punigp->field_0x324 |= 0x20;
    if (swByteGet(0x299) == 1)
        return 2;

    p = &Punidat[1];
    for (i = 1; i <= 0x6d; i++, p++) {
        s32 c0;
        u8 b1, b2, b3, b4, b5, b6;
        f32 v1, v2, v3;

        if (p->unk4 & 0x40000000) {
            p->unk4 &= ~0x40000000;
            continue;
        }
        if (i >= 0x67) {
            p->field_0x8 |= 0x20;
            continue;
        }
        {
            s32 off = (i - 1) * 7;
            s32 iv1, iv2, iv3;

            c0 = (u8)_swByteGet(off + 0);
            b1 = _swByteGet(off + 1);
            b2 = _swByteGet(off + 2);
            b3 = _swByteGet(off + 3);
            b4 = _swByteGet(off + 4);
            b5 = _swByteGet(off + 5);
            b6 = _swByteGet(off + 6);
            iv1 = b1 + (b2 << 8);
            if (iv1 & 0x8000)
                iv1 |= 0xffff0000;
            iv2 = b3 + (b4 << 8);
            if (iv2 & 0x8000)
                iv2 |= 0xffff0000;
            iv3 = b5 + (b6 << 8);
            if (iv3 & 0x8000)
                iv3 |= 0xffff0000;
            v1 = (f32)iv1;
            v2 = (f32)iv2;
            v3 = (f32)iv3;
        }
        if (c0 != 0 && c0 != Punigp->unk328)
            continue;
        {
            s32 bit;
            if (i >= 0x67)
                bit = 0;
            else
                bit = (swByteGet(i / 8 + 0x28c) >> (i % 8)) & 1;
            if (bit != 0)
                continue;
        }
        if (i == 1) {
            if (swGet(0xb3c) == 0)
                continue;
            if (swGet(0xb14) != 0)
                continue;
            Punigp->field_0x364 = p;
            p->unk4 |= 0x20;
        } else if (i == 2) {
            if (swGet(0xb3d) == 0)
                continue;
            Punigp->field_0x368 = p;
            p->unk4 |= 0x40;
        } else if (i == 4) {
            if (swGet(0xb3f) == 0)
                continue;
            Punigp->field_0x370 = p;
            p->unk4 |= 0x100;
        } else if (i == 3) {
            if (swGet(0xb3e) == 0)
                continue;
            Punigp->field_0x36C = p;
            p->unk4 |= 0x80;
        } else {
            if (swByteGet(0x28b) + 5 <= i)
                continue;
            p->unk4 |= 0x8;
        }

        if (c0 == 0) {
            if (gp->flags & 1) {
                sx = puniRandfSetup(mario->position.x - 10.0f, mario->position.x + 10.0f);
                sy = mario->position.y;
                sz = puniRandfSetup(mario->position.z - 10.0f, mario->position.z + 10.0f);
                p->unkC = 1;
            } else {
                sx = bero_get_BeroSX();
                sy = puniNearest(bero_get_BeroSY());
                sz = bero_get_BeroSZ();
                ex = bero_get_BeroEX();
                ey = puniNearest(bero_get_BeroEY());
                ez = bero_get_BeroEZ();
                if ((Punigp->field_0x324 & 8) == 0) {
                    sx = puniRandfSetup(sx - 10.0f, sx + 10.0f);
                    sz = puniRandfSetup(sz - 10.0f, sz + 10.0f);
                } else {
                    ez += 60.0f;
                }
                p->field_0x1F8.x = puniRandfSetup(ex - 10.0f, ex + 10.0f);
                p->field_0x1F8.y = ey;
                p->field_0x1F8.z = puniRandfSetup(ez - 10.0f, ez + 10.0f);
                p->unkC = 0x19;
                p->unk4 &= ~0x1000;
            }
        } else {
            sx = v1;
            sy = v2;
            sz = v3;
            p->unkC = 0xf;
            p->unk4 |= 0x8000;
        }
        p->field_0x114 = 0;
        p->field_0x11C = 0;
        p->field_0x11E = 1000;
        p->field_0x12C.x = sx;
        p->field_0x12C.y = sy;
        p->field_0x12C.z = sz;
        *(Vec*)&p->field_0x138 = p->field_0x12C;
        p->field_0x190 = 6.0f;
        p->field_0x194 = 2.0f;
        p->field_0x198 = 3.0f;
        p->field_0x574 = 60;
        p->field_0x576 = 60;
        p->field_0x578 = 10;
        p->field_0x57A = 60;
        p->field_0x57C = 60;
        p->field_0x57E = 20;
        p->field_0x580 = 10;
        p->field_0x582 = 10;
        p->field_0x584 = 0;
        p->field_0x586 = 0;
        p->field_0x588 = 50;
        p->field_0x58A = 70;
        p->field_0x1D0 = 0;
        p->field_0x1D4 = 0;
        p->field_0x1D8 = 0;
        PuniDokuritsu(p);
        p->unk4 |= 0x1401;
    }

    beros = bero_get_ptr();
    num = bero_get_BeroNUM();
    for (j = 0; j < num; j++) {
        if (beros[j]->kinddirArg3 != 9)
            continue;
        p = &Punidat[103] + j;
        p->unk4 |= 0x00100400;
        p->field_0xE = 0x1c;
        p->unkC = 0x1c;
        p->field_0x190 = 25.0f;
        hitObjGetPos(beros[j]->hitName, &p->field_0x12C);
        p->field_0x12C.y = puniNearest(p->field_0x12C.y);
    }
    return 2;
}

// NONMATCHING (98.65%): computing the z bounds before the x bounds fixes most of the schedule;
// what remains is the x/z register colouring (target x=f21, z=f3; ours x=f22, z=f2) and the
// add-before-sub order of the two bound pairs.
USER_FUNC(evt_toge_setup) {
    s32 i;
    s32 count = 1;
    s32* args = event->args;
    s32 type;
    f32 x, y, z;
    f32 xlo, xhi, zlo, zhi;

    type = evtGetValue(event, args[0]);
    x = (f32)evtGetValue(event, args[1]);
    y = (f32)evtGetValue(event, args[2]);
    z = (f32)evtGetValue(event, args[3]);

    switch (type) {
    case 0:
        return 2;
    case 1:
        count = 210;
        break;
    case 2:
        count = 120;
        break;
    }
    zlo = z - 120.0f;
    zhi = z + 120.0f;
    xlo = x - 50.0f;
    xhi = x + 50.0f;
    for (i = 110; i < count; i++) {
        PuniData* p = &Punidat[i];

        f32 rx = (f32)rand() / 32767.0f;
        f32 rz = (f32)rand() / 32767.0f;

        p->unk4 |= 0x10;
        p->unkC = 0;
        p->field_0x116 = i;
        p->field_0x11C = 0;
        p->field_0x114 = 0;
        p->field_0x11E = 1000;
        p->field_0x12C.x = xlo + rx * (xhi - xlo);
        p->field_0x12C.y = y;
        p->field_0x12C.z = zlo + rz * (zhi - zlo);
        *(Vec*)&p->field_0x138 = p->field_0x12C;
        p->field_0x190 = 6.0f;
        p->field_0x194 = 2.0f;
        p->field_0x198 = 3.0f;
        p->field_0x574 = 60;
        p->field_0x576 = 60;
        p->field_0x578 = 10;
        p->field_0x57A = 60;
        p->field_0x57C = 60;
        p->field_0x57E = 20;
        p->field_0x580 = 10;
        p->field_0x582 = 10;
        p->field_0x584 = 0;
        p->field_0x586 = 0;
        p->field_0x588 = 50;
        p->field_0x58A = 70;
        p->unk4 |= 0x00101402;
    }
    return 2;
}
s32 _puniVAtari(PuniData* punidat, Vec* src, Vec* dest) {
    // declaration order reproduces the original's stack slots (MW allocates top-down: first declared = highest)
    Vec pos;
    Vec dir;
    f32 hitZ, hitY, hitX;
    f32 hitNZ, hitNY, hitNX;
    f32 distance;

    PSVECSubtract(dest, src, &dir);
    if (dir.x == 0.0f && dir.z == 0.0f) {
        punidat->field_0x12C = *src;
        return 1;
    }
    pos = *src;
    PSVECNormalize(&dir, &dir);
    distance = PSVECMag(&dir);
    if (hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, pos.x, pos.y, pos.z, dir.x,
                       dir.y, dir.z) == NULL) {
        punidat->field_0x12C = *dest;
        return 1;
    }
    return 0;
}
static inline BOOL puniHagureCheck2(PuniData* punidat, Vec* pos, Vec* target) {
    Vec p;
    Vec delta;
    Vec out;
    Vec n;
    f32 dist;

    PSVECSubtract(target, pos, &delta);
    if (delta.x == 0.0f && delta.z == 0.0f) {
        punidat->field_0x12C = *pos;
        return TRUE;
    }
    p = *pos;
    PSVECNormalize(&delta, &delta);
    dist = PSVECMag(&delta);
    if (hitCheckFilter(NULL, &out.x, &out.y, &out.z, &dist, &n.x, &n.y, &n.z, p.x, p.y, p.z, delta.x, delta.y,
                       delta.z) == NULL) {
        punidat->field_0x12C = *target;
        return TRUE;
    }
    return FALSE;
}
void puniHagurePos(PuniData* punidat) {
    Vec pos;
    Vec c3;
    Vec c2;
    Vec c1;
    Vec c0;
    s32 fx;
    s32 fz;
    s32 fx1;
    s32 fz1;
    s32 kind = punidat->unkC;

    switch (kind) {
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x15:
    case 0x16: {
        Vec v = { 0.0f, -1000.0f, 0.0f };

        punidat->field_0x12C = v;
        return;
    }
    }
    {
        pos = punidat->field_0x12C;
        fx = (s32)pos.x;
        if (pos.x < 0.0f) {
            fx--;
        }
        fx1 = fx + 1;
        fz = (s32)pos.z;
        if (pos.z < 0.0f) {
            fz--;
        }
        fz1 = fz + 1;
        c3.x = (f32)fx;
        c3.y = pos.y;
        c3.z = (f32)fz;
        c1.x = (f32)fx1;
        c1.y = pos.y;
        c1.z = (f32)fz1;
        c2.x = (f32)fx1;
        c2.y = pos.y;
        c2.z = (f32)fz;
        c0.x = (f32)fx;
        c0.y = pos.y;
        c0.z = (f32)fz1;
        if (puniHagureCheck2(punidat, &pos, &c3)) {
            return;
        }
        if (puniHagureCheck2(punidat, &pos, &c2)) {
            return;
        }
        if (puniHagureCheck2(punidat, &pos, &c1)) {
            return;
        }
        if (puniHagureCheck2(punidat, &pos, &c0)) {
            return;
        }
        return;
    }
}

// The inlined nearest-of-four helper (f28/f29 swapped) and the two scan loops
// (the original copies the Vec words to a stack temp and rounds in a different
// order) still differ; the case bodies are emitted in the original's order.
// NONMATCHING (84.28%): control flow, all call sites and all field writes match.
USER_FUNC(evt_puni_exit) {
    s32* args = event->args;
    s32 mode = evtGetValue(event, args[0]);
    s32 arg2;
    const char* name;
    f32 f31;
    f32 f30;
    f32 f29;
    f32 f28;
    f32 f27;
    f32 f1;
    s32 cmp;
    s32 i;

    evtGetValue(event, args[1]);
    arg2 = evtGetValue(event, args[2]);
    name = (const char*)evtGetValue(event, args[3]);

    Punigp->field_0x43C = 5;
    if (mode == 2) {
        Punigp->field_0x324 &= ~0xF;
        switch (arg2) {
            case 6:
                Punigp->field_0x324 |= 0x1;
                break;
            case 2:
                Punigp->field_0x324 |= 0x2;
                break;
            case 8:
                Punigp->field_0x324 |= 0x4;
                break;
            case 9:
                Punigp->field_0x324 |= 0x8;
                break;
        }
    }
    if (name == NULL) {
        Punigp->field_0x324 |= 0x10;
    } else {
        Punigp->field_0x324 &= ~0x10;
    }
    if (!(Punigp->field_0x324 & 0x8)) {
        f31 = bero_get_BeroSX();
        f30 = puniExitNearest(bero_get_BeroSY());
        f29 = bero_get_BeroSZ();
        f28 = bero_get_BeroEX();
        f27 = puniExitNearest(bero_get_BeroEY());
        f1 = bero_get_BeroEZ();
    } else {
        f31 = bero_get_BeroSX();
        f30 = puniExitNearest(bero_get_BeroSY());
        f29 = bero_get_BeroSZ();
        f28 = f31;
        f27 = f30;
        f1 = f29;
        f29 = f29 + 60.0f;
    }
    Punigp->field_0x47C = 2;
    Punigp->field_0x480.x = f31;
    Punigp->field_0x480.y = f30;
    Punigp->field_0x480.z = f29;
    Punigp->field_0x48C.x = f28;
    Punigp->field_0x48C.y = f27;
    Punigp->field_0x48C.z = f1;
    Punigp->field_0x4B0 = 100.0f;
    Punigp->field_0x4B4 = 100.0f;

    if (name != NULL) {
        cmp = strcmp(name, "mri_00");
    } else {
        cmp = 1;
    }

    for (i = 1; i <= 0x66; i++) {
        PuniData* punidat = &Punidat[i];

        if (!(punidat->unk4 & 0x400)) {
            continue;
        }
        if (punidat->unk4 & 0x800000) {
            continue;
        }
        if (punidat->unk4 & 0x8000) {
            if (name == NULL) {
                punidat->unk4 |= 0x40000000;
                continue;
            }
            puniHagurePos(punidat);
            {
                s32 base = (punidat->field_0x116 - 1) * 7;
                s32 x = (s32)punidat->field_0x12C.x;
                s32 y = (s32)punidat->field_0x12C.y;
                s32 z = (s32)punidat->field_0x12C.z;

                _swByteSet(base + 0, (u8)Punigp->unk328);
                _swByteSet(base + 1, (u8)x);
                _swByteSet(base + 2, (u8)((x >> 8) & 0xFF));
                _swByteSet(base + 3, (u8)y);
                _swByteSet(base + 4, (u8)((y >> 8) & 0xFF));
                _swByteSet(base + 5, (u8)z);
                _swByteSet(base + 6, (u8)((z >> 8) & 0xFF));
            }
        } else {
            Vec v = { 0.0f, 0.0f, 0.0f };
            s32 base = (punidat->field_0x116 - 1) * 7;
            s32 x = (s32)v.x;
            s32 y = (s32)v.y;
            s32 z = (s32)v.z;

            _swByteSet(base + 0, 0);
            _swByteSet(base + 1, (u8)x);
            _swByteSet(base + 2, (u8)((x >> 8) & 0xFF));
            _swByteSet(base + 3, (u8)y);
            _swByteSet(base + 4, (u8)((y >> 8) & 0xFF));
            _swByteSet(base + 5, (u8)z);
            _swByteSet(base + 6, (u8)((z >> 8) & 0xFF));
            PuniDokuritsu(punidat);
            if (cmp == 0) {
                punidat->unkC = 0xE;
                punidat->field_0x114 = 0;
                punidat->field_0x11C = 0;
            } else {
                punidat->unkC = 0x1A;
                punidat->field_0x114 = 0;
                punidat->field_0x11C = 2;
            }
        }
    }

    if (name != NULL && strcmp(name, "mri_00") == 0) {
        for (i = 1; i <= 0x66; i++) {
            Vec v = { 0.0f, 0.0f, 0.0f };
            s32 base = (i - 1) * 7;
            s32 x = (s32)v.x;
            s32 y = (s32)v.y;
            s32 z = (s32)v.z;

            _swByteSet(base + 0, 0);
            _swByteSet(base + 1, (u8)x);
            _swByteSet(base + 2, (u8)((x >> 8) & 0xFF));
            _swByteSet(base + 3, (u8)y);
            _swByteSet(base + 4, (u8)((y >> 8) & 0xFF));
            _swByteSet(base + 5, (u8)z);
            _swByteSet(base + 6, (u8)((z >> 8) & 0xFF));
        }
    }
    return 2;
}
USER_FUNC(evt_puni_init_puni) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 unkC = evtGetValue(event, args[1]);
    PuniData* punidat = &Punidat[type];
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    s32 flag = evtGetValue(event, args[5]);

    if (type == 1) {
        if (flag != 0 && swGet(0xB3C) == 0) {
            return 2;
        }
        if (flag != 0 && swGet(0xB14) != 0) {
            return 2;
        }
        *(PuniData**)((u8*)Punigp + 0x364) = punidat;   // PuniWork 0x364
        punidat->unk4 |= 0x20;
    } else if (type == 2) {
        if (flag != 0 && swGet(0xB3D) == 0) {
            return 2;
        }
        *(PuniData**)((u8*)Punigp + 0x368) = punidat;   // PuniWork 0x368
        punidat->unk4 |= 0x40;
    } else if (type == 4) {
        if (flag != 0 && swGet(0xB3F) == 0) {
            return 2;
        }
        *(PuniData**)((u8*)Punigp + 0x370) = punidat;   // PuniWork 0x370
        punidat->unk4 |= 0x100;
    } else if (type == 3) {
        if (flag != 0 && swGet(0xB3E) == 0) {
            return 2;
        }
        *(PuniData**)((u8*)Punigp + 0x36C) = punidat;   // PuniWork 0x36C
        punidat->unk4 |= 0x80;
    } else {
        if (flag != 0 && swByteGet(0x28B) + 5 <= type) {
            return 2;
        }
        punidat->unk4 |= 0x8;
    }

    punidat->unkC = unkC;
    punidat->field_0x114 = 0;
    punidat->field_0x11C = 0;
    punidat->field_0x11E = 1000;
    punidat->field_0x12C.x = x;
    punidat->field_0x12C.y = y;
    punidat->field_0x12C.z = z;
    punidat->field_0x138 = punidat->field_0x12C;
    punidat->field_0x190 = 6.0f;
    punidat->field_0x194 = 2.0f;
    punidat->field_0x198 = 3.0f;
    punidat->field_0x574 = 0x3C;
    punidat->field_0x576 = 0x3C;
    punidat->field_0x578 = 0x0A;
    punidat->field_0x57A = 0x3C;
    punidat->field_0x57C = 0x3C;
    punidat->field_0x57E = 0x14;
    punidat->field_0x580 = 0x0A;
    punidat->field_0x582 = 0x0A;
    punidat->field_0x584 = 0;
    punidat->field_0x586 = 0;
    punidat->field_0x588 = 0x32;
    punidat->field_0x58A = 0x46;
    punidat->field_0x1D0 = 0;
    punidat->field_0x1D4 = 0;
    punidat->field_0x1D8 = 0;
    PuniDokuritsu(punidat);
    punidat->unk4 |= 0x1401;
    return 2;
}
USER_FUNC(evt_puni_init_teki) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 unkC = evtGetValue(event, args[1]);
    PuniData* punidat = &Punidat[index];
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);

    punidat->unk4 |= 0x10;
    punidat->unkC = unkC;
    punidat->field_0x11C = 0;
    punidat->field_0x114 = 0;
    punidat->field_0x11E = 1000;
    punidat->field_0x12C.x = x;
    punidat->field_0x12C.y = y;
    punidat->field_0x12C.z = z;
    punidat->field_0x138 = punidat->field_0x12C;
    punidat->field_0x190 = 6.0f;
    punidat->field_0x194 = 2.0f;
    punidat->field_0x198 = 3.0f;
    punidat->field_0x574 = 0x3C;
    punidat->field_0x576 = 0x3C;
    punidat->field_0x578 = 0x0A;
    punidat->field_0x57A = 0x3C;
    punidat->field_0x57C = 0x3C;
    punidat->field_0x57E = 0x14;
    punidat->field_0x580 = 0x0A;
    punidat->field_0x582 = 0x0A;
    punidat->field_0x584 = 0;
    punidat->field_0x586 = 0;
    punidat->field_0x588 = 0x32;
    punidat->field_0x58A = 0x46;
    punidat->unk4 |= 0x101402;
    return 2;
}
USER_FUNC(evt_puni_get_position) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    evtSetValue(event, args[1], (s32)punidat->field_0x12C.x);
    evtSetValue(event, args[2], (s32)punidat->field_0x12C.y);
    evtSetValue(event, args[3], (s32)punidat->field_0x12C.z);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_set_position) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    punidat->field_0x12C.x = evtGetValue(event, args[1]);
    punidat->field_0x12C.y = evtGetValue(event, args[2]);
    punidat->field_0x12C.z = evtGetValue(event, args[3]);

    // copies the Vec at 0x12C word-for-word (not an int conversion).
    *(Vec*)&punidat->field_0x138 = punidat->field_0x12C;
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_get_dir) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    evtSetValue(event, args[1], (s32)punidat->field_0x180);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_set_dir) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    punidat->field_0x180 = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_get_anim) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    evtSetValue(event, args[1], punidat->field_0x11C);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_set_anim) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    punidat->field_0x11C = evtGetValue(event, args[1]);
    punidat->field_0x11E = 1000;
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_korokoro) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    switch ((s32)punidat->field_0x11C) {
    case 5:
        if (isFirstCall) {
            if (punidat->unk4 & 8) {
                punidat->field_0x188 = 8.0f;
            } else {
                punidat->field_0x148 = 8.0f;
            }
        }
        punidat->field_0x184 += evtGetValue(event, args[1]);
        return EVT_RETURN_BLOCK;
    default:
        if (punidat->unk4 & 8) {
            punidat->field_0x188 = 0.0f;
        } else {
            punidat->field_0x148 = 0.0f;
        }
        punidat->field_0x184 = evtGetValue(event, args[2]);
        return EVT_RETURN_DONE;
    }
}

// NONMATCHING (99.83%): `elapsed` is kept in r29; the original computes the division in r4
// and copies it to r29 across the intplGetValue calls (all other instructions identical).
USER_FUNC(evt_puni_move_position) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];
    f32 x = evtGetValue(event, args[1]);
    f32 z = evtGetValue(event, args[2]);
    s32 duration = evtGetValue(event, args[3]);

    if (isFirstCall) {
        event->savedTime = gp->renderTime;
        punidat->field_0x1F8 = punidat->field_0x12C;
        punidat->field_0x204.x = x;
        punidat->field_0x204.y = punidat->field_0x12C.y;
        punidat->field_0x204.z = z;
    }
    {
        s32 elapsed = ((u32)gp->renderTime - (u32)event->savedTime) / ((__OSBusClock / 4) / 1000);

        if (elapsed < duration) {
            punidat->field_0x12C.x =
                intplGetValue(0, elapsed, duration, punidat->field_0x1F8.x, punidat->field_0x204.x);
            punidat->field_0x12C.z =
                intplGetValue(0, elapsed, duration, punidat->field_0x1F8.z, punidat->field_0x204.z);
            *(Vec*)&punidat->field_0x138 = punidat->field_0x12C;
            return EVT_RETURN_BLOCK;
        }
    }
    punidat->field_0x12C = punidat->field_0x204;
    *(Vec*)&punidat->field_0x138 = punidat->field_0x12C;
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_move_position2) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];
    f32 x = evtGetValue(event, args[1]);
    f32 z = evtGetValue(event, args[2]);
    f32 speed = evtGetFloat(event, args[3]);

    if (isFirstCall) {
        event->savedTime = gp->renderTime;
        punidat->field_0x1F8 = punidat->field_0x12C;
        punidat->field_0x204.x = x;
        punidat->field_0x204.y = punidat->field_0x12C.y;
        punidat->field_0x204.z = z;
    }
    {
        f32 sec = sysFrame2SecFloat(speed);
        f32 dist = PSVECDistance(&punidat->field_0x1F8, &punidat->field_0x204);
        s32 msec = ((u32)gp->renderTime - (u32)event->savedTime) / ((__OSBusClock / 4) / 1000);
        s32 frame = (s32)(0.5f + sysMsec2FrameFloat((f32)msec));
        Vec dir;

        PSVECSubtract(&punidat->field_0x204, &punidat->field_0x1F8, &dir);
        PSVECNormalize(&dir, &dir);
        if (frame < (s32)(dist / sec)) {
            PSVECScale(&dir, &dir, sec * (f32)frame);
            PSVECAdd(&punidat->field_0x1F8, &dir, &punidat->field_0x12C);
            return EVT_RETURN_BLOCK;
        }
        punidat->field_0x12C = punidat->field_0x204;
        *(Vec*)&punidat->field_0x138 = punidat->field_0x12C;
        return EVT_RETURN_DONE;
    }
}

// NONMATCHING (99.72%): same elapsed-division register difference as evt_puni_move_position
// (`divwu` targets r28 instead of r4, which also shifts the double_to_int base register).
USER_FUNC(evt_puni_jump_position) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];
    f32 tx = evtGetValue(event, args[1]);
    f32 ty = evtGetValue(event, args[2]);
    f32 tz = evtGetValue(event, args[3]);
    s32 duration = evtGetValue(event, args[4]);
    f32 g = evtGetValue(event, args[5]);
    s32 elapsed;

    if (isFirstCall) {
        f32 a = -4.0f * g;

        punidat->modeStartTime = gp->renderTime;
        punidat->field_0x1F8 = punidat->field_0x12C;
        punidat->field_0x1EC = a;
        punidat->field_0x1F0 = (ty - punidat->field_0x1F8.y) - punidat->field_0x1EC;
        psndSFXOn_3D("SFX_STG2_PUNI_JUMP1", &punidat->field_0x12C);
    }
    elapsed = ((u32)gp->renderTime - (u32)punidat->modeStartTime) / ((__OSBusClock / 4) / 1000);
    if (elapsed < duration) {
        f32 ratio = (f32)elapsed / (f32)duration;

        punidat->field_0x12C.x =
            intplGetValue(0, elapsed, duration, punidat->field_0x1F8.x, tx);
        punidat->field_0x12C.z =
            intplGetValue(0, elapsed, duration, punidat->field_0x1F8.z, tz);
        punidat->field_0x12C.y =
            ratio * (punidat->field_0x1EC * ratio + punidat->field_0x1F0) + punidat->field_0x1F8.y;
        return EVT_RETURN_BLOCK;
    }
    punidat->field_0x12C.x = tx;
    punidat->field_0x12C.y = ty;
    punidat->field_0x12C.z = tz;
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_get_mode) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    evtSetValue(event, args[1], punidat->unkC);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_set_mode) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];
    s32 mode = evtGetValue(event, args[1]);

    punidat->unkC = mode;
    punidat->field_0xE = mode;
    punidat->field_0x114 = 0;
    punidat->field_0x11E = 1000;
    PuniDokuritsu(punidat);
    if ((EffectEntry*)punidat->field_0x218 != NULL) {
        effDelete((EffectEntry*)punidat->field_0x218);
        punidat->field_0x218 = 0;
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_setsudan) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    if (punidat->unk4 & 0x400) {
        PuniDokuritsu(punidat);
        if ((EffectEntry*)punidat->field_0x218 != NULL) {
            effDelete((EffectEntry*)punidat->field_0x218);
            punidat->field_0x218 = 0;
        }
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_get_flag) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    evtSetValue(event, args[1], punidat->unk4);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_flag_onoff) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];
    s32 on = evtGetValue(event, args[1]);
    s32 flag = evtGetValue(event, args[2]);

    if (on != 0) {
        punidat->unk4 &= ~flag;
    } else {
        punidat->unk4 |= flag;
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_dokan_onoff) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 on = evtGetValue(event, args[1]);
    s32 filtered = bero_id_filter(id);
    PuniData* puni = (Punidat + 103) + filtered;

    if (on == 0) {
        puni->unk4 |= 0x100000;
    } else {
        puni->unk4 &= ~0x100000;
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (96.28%): original keeps b0 in r27 and hoists `mr r5,r27` before t1, reusing r27 for
// t1; ours keeps b0 in r29 and computes t1 in r23.
USER_FUNC(evt_puni_get_lsw_index) {
    s32* args = event->args;
    u8 b0;
    u8 b1;
    u8 b2;
    u8 b3;
    u8 b4;
    u8 b5;
    u8 b6;
    s32 index = args[0];
    s32 t1;
    s32 t2;
    s32 t3;
    f32 v1;
    f32 v2;
    f32 v3;

    index += evtGetValue(event, args[1]) * 7;
    b0 = _swByteGet(index + EVTDAT_LSW_BASE + 0);
    b1 = _swByteGet(index + EVTDAT_LSW_BASE + 1);
    b2 = _swByteGet(index + EVTDAT_LSW_BASE + 2);
    b3 = _swByteGet(index + EVTDAT_LSW_BASE + 3);
    b4 = _swByteGet(index + EVTDAT_LSW_BASE + 4);
    b5 = _swByteGet(index + EVTDAT_LSW_BASE + 5);
    b6 = _swByteGet(index + EVTDAT_LSW_BASE + 6);
    t1 = b1 + (b2 << 8);
    if (t1 & 0x8000) {
        t1 |= 0xFFFF0000;
    }
    t2 = b3 + (b4 << 8);
    if (t2 & 0x8000) {
        t2 |= 0xFFFF0000;
    }
    t3 = b5 + (b6 << 8);
    if (t3 & 0x8000) {
        t3 |= 0xFFFF0000;
    }
    evtSetValue(event, args[2], b0);
    v1 = (f32)t1;
    v2 = (f32)t2;
    v3 = (f32)t3;
    evtSetValue(event, args[3], (s32)v1);
    evtSetValue(event, args[4], (s32)v2);
    evtSetValue(event, args[5], (s32)v3);
    return EVT_RETURN_DONE;
}

// NONMATCHING (95.36%): original saves 6 callee-saved (r26-r31): event=r28, args=r26, index=r27;
// ours saves 5 (event=r27, args=r28, index=r31).
USER_FUNC(evt_puni_set_lsw_index) {
    s32* args = event->args;
    s32 index = args[0];
    s32 i = evtGetValue(event, args[1]);
    s32 b0 = evtGetValue(event, args[2]);
    f32 v1 = (f32)evtGetValue(event, args[3]);
    f32 v2 = (f32)evtGetValue(event, args[4]);
    f32 v3 = (f32)evtGetValue(event, args[5]);
    s32 s1 = (s32)v1;
    s32 s2 = (s32)v2;
    s32 s3 = (s32)v3;

    index += i * 7;
    _swByteSet(index + EVTDAT_LSW_BASE + 0, b0);
    _swByteSet(index + EVTDAT_LSW_BASE + 1, s1 & 0xFF);
    _swByteSet(index + EVTDAT_LSW_BASE + 2, (s1 >> 8) & 0xFF);
    _swByteSet(index + EVTDAT_LSW_BASE + 3, s2 & 0xFF);
    _swByteSet(index + EVTDAT_LSW_BASE + 4, (s2 >> 8) & 0xFF);
    _swByteSet(index + EVTDAT_LSW_BASE + 5, s3 & 0xFF);
    _swByteSet(index + EVTDAT_LSW_BASE + 6, (s3 >> 8) & 0xFF);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_get_trap_index) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 value;

    if (index >= 0x67) {
        value = 0;
    } else {
        value = (swByteGet(0x28C + index / 8) >> (index % 8)) & 1;
    }
    evtSetValue(event, args[1], value);
    return EVT_RETURN_DONE;
}

// NONMATCHING (98.58%): event/args land in r29/r30 instead of r31/r29, which flips the `and`/`or`
// operand order (`and r4,r4,r3` vs `and r4,r3,r4`). Declaring `bit` before `div` fixed div/bit (r29/r30).
USER_FUNC(evt_puni_set_trap_index) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    if (index < 0x67) {
        s32 bit;
        s32 div = index / 8;
        s32 mask;

        bit = index % 8;
        mask = 1 << bit;
        swByteSet(0x28C + div, ((swByteGet(0x28C + div) & (mask ^ 0xFF)) | (value << bit)) & 0xFF);
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_set_ymode) {
    Punigp->field_0x33C = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

// NONMATCHING (98.29%): callee-saved register numbering for the hoisted hit/hitN addresses; logic and stack layout match.
USER_FUNC(evt_puni_all_fall_check) {
    Vec hit;
    Vec hitN;
    Vec pos;
    Vec normal;
    f32 distance;
    PuniData* puni = &Punidat[1];
    s32 i = 1;

    for (; i <= 0x66; i++, puni++) {
        if (!(puni->unk4 & 0x400)) {
            continue;
        }
        pos = puni->field_0x12C;
        pos.y += 1.0f;
        normal = (Vec){0.0f, -1.0f, 0.0f};
        distance = 2.0f;
        if (hitCheckFilter(NULL, &hit.x, &hit.y, &hit.z, &distance, &hitN.x, &hitN.y, &hitN.z, pos.x, pos.y, pos.z,
                           normal.x, normal.y, normal.z) == NULL) {
            PuniDokuritsu(puni);
            *(Vec*)&puni->field_0x15C = (Vec){0.0f, 0.0f, 0.0f};
            puni->unkC = 0x11;
            puni->field_0x114 = 0;
            if ((EffectEntry*)puni->field_0x218 != NULL) {
                effDelete((EffectEntry*)puni->field_0x218);
                puni->field_0x218 = 0;
            }
        }
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_all_movestop) {
    s32* args = event->args;
    s32 on = evtGetValue(event, args[0]);
    s32 i;

    for (i = 1; i <= 0x66; i++) {
        PuniData* puni = &Punidat[i];

        if (!(puni->unk4 & 0x400)) {
            continue;
        }
        if (on == 0) {
            puni->unk4 &= ~0x200;
        } else {
            puni->unk4 |= 0x200;
        }
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_count) {
    s32* args = event->args;
    s32 flag = evtGetValue(event, args[0]);
    s32 start;
    s32 end;
    s32 count;
    s32 i;

    if (flag == 0) {
        start = 1;
        end = 0x67;
    } else {
        start = 0x6E;
        end = 0xD2;
    }
    count = 0;
    for (i = start; i < end; i++) {
        PuniData* puni = &Punidat[i];

        if ((puni->unk4 & 0x400) && !(puni->unk4 & 0x8000)) {
            count++;
        }
    }
    evtSetValue(event, args[1], count);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_count_area) {
    s32* args = event->args;
    f32 x = (f32)evtGetValue(event, args[0]);
    f32 y = (f32)evtGetValue(event, args[1]);
    f32 z = (f32)evtGetValue(event, args[2]);
    f32 range = (f32)evtGetValue(event, args[3]);
    s32 i = 1;
    s32 count = 0;

    for (; i <= 0x66; i++) {
        PuniData* puni = &Punidat[i];

        if (!(puni->unk4 & 0x400)) {
            continue;
        }
        if (puni->unkC == 0x10) {
            continue;
        }
        if (10.0 < __fabs(y - puni->field_0x12C.y)) {
            continue;
        }
        if (range < __fabs(x - puni->field_0x12C.x)) {
            continue;
        }
        if (range < __fabs(z - puni->field_0x12C.z)) {
            continue;
        }
        count++;
    }
    evtSetValue(event, args[4], count);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_zenmetsu_check) {
    s32* args = event->args;
    s32 flag = evtGetValue(event, args[0]);
    s32 start;
    s32 end;
    s32 count;
    s32 i;

    if (flag == 0) {
        start = 1;
        end = 0x67;
    } else {
        start = 0x6E;
        end = 0xD2;
    }
    count = 0;
    for (i = start; i < end; i++) {
        PuniData* puni = &Punidat[i];

        if ((puni->unk4 & 0x400) && !(puni->unk4 & 0x4000)) {
            count++;
        }
    }
    if (count == 0) {
        evtSetValue(event, args[1], 1);
    } else {
        evtSetValue(event, args[1], 0);
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_landon_check) {
    s32* args = event->args;
    s32 flag = evtGetValue(event, args[0]);
    f32 threshold = evtGetFloat(event, args[1]);
    s32 start;
    s32 end;
    s32 count;
    s32 i;

    if (flag == 0) {
        start = 1;
        end = 0x67;
    } else {
        start = 0x6E;
        end = 0xD2;
    }
    count = 0;
    for (i = start; i < end; i++) {
        PuniData* puni = &Punidat[i];

        if (!(puni->unk4 & 0x400)) {
            continue;
        }
        if (puni->field_0x12C.y > threshold) {
            count++;
        }
    }
    if (count == 0) {
        evtSetValue(event, args[2], 1);
    } else {
        evtSetValue(event, args[2], 0);
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_pera_onoff) {
    s32* args = event->args;
    PuniData* puni = &Punidat[evtGetValue(event, args[0])];
    s32 on = evtGetValue(event, args[1]);

    if (on != 0) {
        *(u32*)(*(u32**)((u8*)puni + 0x0)) &= ~0x2; // PuniData 0x0 holds a u32* flags
    } else {
        *(u32*)(*(u32**)((u8*)puni + 0x0)) |= 0x2;
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_fade_onoff) {
    puniFadeOff(evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_attack_on) {
    Punigp->field_0x43C = 3;
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_puninum_onoff) {
    Punigp->showPuniCounter = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_puninum_alpha) {
    Punigp->puniCounterAlpha = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_winddisable_onoff) {
    Punigp->field_0x4CC = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_puni_force_se_off) {
    u32 se = *(u32*)((u8*)Punigp + 0x458); // PuniWork 0x458

    if (se != 0) {
        psndSFXOff(se);
    }
    se = Punigp->field_0x468;
    if (se != 0) {
        psndSFXOff(se);
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (62.61%): MWCC colours puni r31 vs the original r29 (and z r29 vs r31), and schedules the
// pos float->int conversions/base computation in a different order. `(Vec){0,0,0}` must be written as
// the named `(Vec){0.0f, 0.0f, 0.0f}`.
USER_FUNC(evt_puni_all_save) {
    PuniData* puni = &Punidat[1];
    s32 i;
    s32 base;
    Vec pos;
    s32 x;
    s32 xh;
    s32 y;
    s32 yh;
    s32 z;

    for (i = 1; i <= 0x66; i++, puni++) {
        if ((puni->unk4 & 0x400) == 0) {
            continue;
        }
        if ((puni->unk4 & 0x800000) != 0) {
            continue;
        }
        if ((puni->unk4 & 0x8000) != 0) {
            puniHagurePos(puni);
            pos = puni->field_0x12C;
            x = (s32)pos.x;
            y = (s32)pos.y;
            z = (s32)pos.z;
            base = (puni->field_0x116 - 1) * 7;
            xh = (x >> 8) & 0xFF;
            yh = (y >> 8) & 0xFF;
            _swByteSet(base + 0, (u8)Punigp->unk328);
            _swByteSet(base + 1, x);
            _swByteSet(base + 2, xh);
            _swByteSet(base + 3, y);
            _swByteSet(base + 4, yh);
            _swByteSet(base + 5, z);
            _swByteSet(base + 6, (z >> 8) & 0xFF);
        } else {
            pos = (Vec){0.0f, 0.0f, 0.0f};
            x = (s32)pos.x;
            y = (s32)pos.y;
            z = (s32)pos.z;
            base = (puni->field_0x116 - 1) * 7;
            xh = (x >> 8) & 0xFF;
            yh = (y >> 8) & 0xFF;
            _swByteSet(base + 0, 0);
            _swByteSet(base + 1, x);
            _swByteSet(base + 2, xh);
            _swByteSet(base + 3, y);
            _swByteSet(base + 4, yh);
            _swByteSet(base + 5, z);
            _swByteSet(base + 6, (z >> 8) & 0xFF);
        }
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_target_flag_onoff) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 on = evtGetValue(event, args[1]);
    s32 mask = evtGetValue(event, args[2]);
    s32* flag = (s32*)((u8*)Punigp + 0x374 + index * 0x14); // PuniWork target table

    if (on != 0) {
        *flag &= ~mask;
    } else {
        *flag |= mask;
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_target_set_position) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    PuniTarget* target = (PuniTarget*)((u8*)Punigp + 0x374 + index * 0x14); // PuniWork target table

    target->position.x = (f32)evtGetValue(event, args[1]);
    target->position.y = (f32)evtGetValue(event, args[2]);
    target->position.z = (f32)evtGetValue(event, args[3]);
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_target_set_soko) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    PuniTarget* target = (PuniTarget*)((u8*)Punigp + 0x374 + index * 0x14); // PuniWork target table

    target->hit = hitNameToPtr((const char*)evtGetValue(event, args[1]));
    return EVT_RETURN_DONE;
}
const char* npcGetTribeName(NpcTribeInfo* tribe) {
    return tribe->nameJp;
}

// Clears target-table entries 2..9. The original seeds the strength-reduced offset at 0x28
// (`li r3,0x28; addi r5,r3,0x374`), so index it as `i*0x14` with i starting at 2.
USER_FUNC(puni_piders_check_func) {
    s32 i;
    NpcWork* work = npcGetWorkPtr();
    s32 total = work->total;
    NpcEntry* npc = work->entries;
    s32 offset;

    for (i = 2; i < 10; i++) {
        *(s32*)((u8*)Punigp + 0x374 + i * 0x14) &= ~5; // PuniWork 0x374 target table
    }

    offset = 0x28;
    for (i = 0; i < total; i++, npc++) {
        s32 hit;
        Vec pos;

        if ((npc->flags & 1) == 0) {
            continue;
        }
        if ((npc->flags & 2) == 0) {
            continue;
        }
        if (npc->tribe == NULL) {
            continue;
        }
        if (strcmp(npc->tribe->nameJp, "\203p\203C\203_\201[\203X\000") != 0) {
            continue;
        }
        pos = npc->position;
        hit = 0;
        if (50.0 >= __fabs(pos.y - Punigp->field_0x32C)) {
            hit = 1;
            pos.y = Punigp->field_0x32C;
        }
        if (50.0 >= __fabs(pos.y - Punigp->field_0x330)) {
            hit = 1;
            pos.y = Punigp->field_0x330;
        }
        if (50.0 >= __fabs(pos.y - Punigp->field_0x334)) {
            hit = 1;
            pos.y = Punigp->field_0x334;
        }
        if (50.0 >= __fabs(pos.y - Punigp->field_0x338)) {
            hit = 1;
            pos.y = Punigp->field_0x338;
        }
        if (hit != 0) {
            PuniTarget* target = (PuniTarget*)((u8*)Punigp + 0x374 + offset); // PuniWork target table

            target->position = pos;
            target->flags |= 5;
        }
        offset += 0x14;
    }
    return EVT_RETURN_DONE;
}
USER_FUNC(puni_disp_info_kassencheck) {
    s32* args = event->args;
    s32 result = 0;

    if (Punigp->unk328 == 9 || Punigp->unk328 == 12) {
        result = 1;
    }
    evtSetValue(event, args[0], result);
    return EVT_RETURN_DONE;
}
void puniCountDisp(CameraId cameraId, void* param) {
    s32 count = (s32)param >> 16;
    s32 total = (s32)param & 0xFFFF;
    GXColor color0 = { 0xFF, 0xFF, 0xFF, 0x99 };
    GXColor color1 = { 0x00, 0x00, 0x00, 0xFF };
    GXColor color2 = { 0xFF, 0xFF, 0xFF, 0xFF };
    Mtx mtx;
    s32 alpha;

    if (evtShopIsActive()) {
        return;
    }

    alpha = Punigp->puniCounterAlpha;
    color0.a = color0.a * alpha / 255;
    color1.a = color1.a * alpha / 255;
    color2.a = color2.a * alpha / 255;

    PSMTXTrans(mtx, -160.0f, -180.0f, 0.0f);
    iconDispGxCol(mtx, 0x10, 0x203, color0);
    PSMTXTrans(mtx, -232.0f, -178.0f, 0.0f);
    iconDispGxCol(mtx, 0x10, 0x204, color1);
    PSMTXTrans(mtx, -160.0f, -178.0f, 0.0f);
    if (Punigp->unk448 != 0) {
        if ((gp->retraceCount & 0x10) != 0) {
            iconNumberDispGx(mtx, count, 0, color2);
        }
    } else {
        iconNumberDispGx(mtx, count, 0, color2);
    }
    PSMTXTrans(mtx, -140.0f, -178.0f, 0.0f);
    iconDispGxCol(mtx, 0x10, 0x1d8, color2);
    PSMTXTrans(mtx, -90.0f, -178.0f, 0.0f);
    iconNumberDispGx(mtx, total, 1, color2);
}
USER_FUNC(puni_disp_info_sub) {
    s32* args = event->args;
    PuniData* puni;
    s32 count = 0;
    s32 total;
    s32 isKassen;
    s32 i;
    void* param;

    for (i = 1; i <= 0x66; i++) {
        puni = &Punidat[i];
        if ((puni->unk4 & 0x400) == 0) {
            continue;
        }
        if ((puni->unk4 & 0x804000) != 0) {
            continue;
        }
        if ((puni->unk4 & 0x80) != 0) {
            continue;
        }
        if (puni->unkC == 0x10 || puni->unkC == 0xf) {
            continue;
        }
        count++;
    }

    total = swGet(0xB3C);
    total += swGet(0xB3D);
    total += swGet(0xB3F);
    total += swByteGet(0x28B);

    isKassen = 0;
    if (Punigp->unk328 == 9) {
        if (swGet(0xB21) == 0) {
            isKassen = 1;
        }
    }
    if (Punigp->unk328 == 12) {
        if (swGet(0xB29) == 0) {
            isKassen = 1;
        }
    }

    if (isKassen != 0) {
        evtSetValue(event, args[0], count);
        evtSetValue(event, args[1], 0);
        Punigp->unk448 = 0;
    } else {
        evtSetValue(event, args[0], count);
        evtSetValue(event, args[1], total - count);
        if (count == total) {
            Punigp->unk448 = 0;
        } else {
            Punigp->unk448 = 1;
        }
    }

    if ((Punigp->field_0x0 & 2) == 0) {
        count = swByteGet(0x29B);
        total = swByteGet(0x29C);
        Punigp->unk448 = 0;
    } else {
        swByteSet(0x29B, (u8)count);
        swByteSet(0x29C, (u8)total);
    }

    param = (void*)((count << 16) | total);
    if (Punigp->showPuniCounter != 0 && total >= 10) {
        dispEntry(CAMERA_2D, 1000.0f, 0, puniCountDisp, param);
    }
    return EVT_RETURN_DONE;
}
