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
#include "driver/icondrv_gx.h"
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

    Punigp->randSeedA += Punigp->randSeedB + 0x17340000 - 0x506F;
    Punigp->randSeedB += Punigp->randSeedA + 0x21270000 + 0x3F9C;
    s = Punigp->randSeedB;
    v = s;
    if (v < 0) {
        v = -v;
    }
    return v % range;
}

static inline s32 puniQrnd(s32 range) {
    s32 s;
    s32 v;


    Punigp->randSeedA =
        Punigp->randSeedB + 0x17340000 + Punigp->randSeedA - 0x506f;
    Punigp->randSeedB =
        Punigp->randSeedA + 0x21270000 + Punigp->randSeedB + 0x3f9c;
    s = Punigp->randSeedB;
    v = s;
    if (v < 0) {
        v = -v;
    }
    return v % range;
}

static inline void puniModeSearch_sub(s32 xx, s32 yy, f32* pc, f32* pd) {
    switch (Punigp->mapKind) {
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

static inline void puniMapSetData(f32 unk0, f32 unk4, s32 unk8, s32 unkC, u32* data) {
    Punimap->yMax = unk0;
    Punimap->yMin = unk4;
    if (data == NULL) {
        Punimap->hasData = 0;
        return;
    }
    Punimap->width = unk8;
    Punimap->height = unkC;
    Punimap->hasData = 1;
    Punimap->gridSrc = data;
    Punimap->valid = 0;
    Punimap->dataSize = unk8 * unkC * 4;
    Punimap->grid = _mapAlloc(Punimap->dataSize);
    memcpy(Punimap->gridSrc, data, Punimap->dataSize);
}

static inline f32 puni_rndf(f32 min, f32 max) {
    return (f32)rand() / 32767.0f * (max - min) + min;
}

static inline BOOL puniHagureCheck(PuniData* punidat, Vec* pos, Vec* target) {
    Vec delta;
    Vec out;
    f32 dist;
    f32 nx, ny, nz;

    PSVECSubtract(target, pos, &delta);
    if (delta.x == 0.0f && delta.z == 0.0f) {
        punidat->pos = *pos;
        return TRUE;
    }
    PSVECNormalize(&delta, &delta);
    dist = PSVECMag(&delta);
    if (hitCheckFilter(NULL, pos->x, pos->y, pos->z, delta.x, delta.y, delta.z, &out.x, &out.y, &out.z, &dist,
                       &nx, &ny, &nz) == NULL) {
        punidat->pos = *target;
        return TRUE;
    }
    return FALSE;
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
    43, 44, 45, 46, 47, 48, 49, 51, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 65535, 0,
};

u16 puni_wlk_patern_data[] = {
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 65535, 0,
};

u16 puni_run_patern_data[] = { 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 65535, 0 };

u16 puni_dmg_patern_data[] = { 72, 65535 };

u16 puni_dwn_patern_data[] = { 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 65535, 0 };

u16 puni_rol_patern_data[] = { 83, 65535 };

u16 puni_hag_patern_data[] = { 0, 65535 };

u16 puni_jpu_patern_data[] = { 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 65534, 0 };

u16 puni_jpd_patern_data[] = { 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 65534, 0 };

u16 puni_wan_patern_data[] = { 104, 105, 106, 107, 108, 109, 110, 111, 65534, 0 };

u16* puni_patern_table[] = { puni_nom_patern_data, puni_wlk_patern_data, puni_run_patern_data, puni_dmg_patern_data,
                             puni_dwn_patern_data, puni_rol_patern_data, puni_hag_patern_data, puni_jpu_patern_data,
                             puni_jpd_patern_data, puni_wan_patern_data };

u16 toge_nom_patern_data[26] = {
    84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 65535, 0,
};

u16 toge_wlk_patern_data[] = { 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 65535, 0 };

u16 toge_run_patern_data[] = { 108, 110, 112, 114, 116, 118, 65535, 0 };

u16 toge_dmg_patern_data[] = { 120, 65535 };

u16 toge_dwn_patern_data[] = { 121, 122, 123, 124, 125, 126, 127, 128, 65535, 0 };

u16 toge_rol_patern_data[] = { 129, 65535 };

u16 toge_hag_patern_data[] = { 0, 65535 };

u16* toge_patern_table[] = { toge_nom_patern_data, toge_wlk_patern_data, toge_run_patern_data, toge_dmg_patern_data,
                             toge_dwn_patern_data, toge_rol_patern_data, toge_hag_patern_data, toge_hag_patern_data,
                             toge_hag_patern_data, toge_hag_patern_data };



















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

static u32 gap_05_0000006C_bss;

PuniData* Punidat;

PuniWork* Punigp;

PuniTexInfo* Punitex;

PuniMap* Punimap;
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

    if (work->area != 9 && work->area != 12) {
        return;
    }

    for (i = 1; i < PUNI_DATA_MAX; i++) {
        dat = &Punidat[i];
        if ((dat->flags & 0x400) && !(dat->flags & 0x804000)) {
            Vec pos;
            Vec pos2;

            pos = dat->pos;
            if (dat->field_0x8 & 1) {
                (dat->field_0x121)++;
                if (*(s8*)((u8*)dat + 0x121) > 30) {
                    dat->field_0x121 = 0;
                    dat->field_0x8 &= ~1;
                }
            }
            if (dat->field_0x8 & 2) {
                pos2 = pos;
                puniSinCosRet(dat->angle + gp->retraceCount * 10, &pos.z, &pos.x, 9.0f, 9.0f);
                pos.y += 13.0f;
                iconDispGx(0.6f, &pos, 0, 0x14D);

                puniSinCosRet(dat->angle + gp->retraceCount * 10 + 0xB4, &pos2.z, &pos2.x, 9.0f, 9.0f);
                pos2.y += 15.0f;
                iconDispGx(0.5f, &pos2, 0, 0x14D);
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

    if (Punigp->area == 9 || Punigp->area == 12) {
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

static inline s32 puniAnimCountInline(s16** arr, s32 idx, s16* term, s32 count) {
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
    MriPuniAnimSet* set0 = &work->animSet[0];
    MriPuniAnimSet* set1 = &work->animSet[1];
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

    p = (char*)Punigp->puniNames[0];
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
    p = (char*)Punigp->puniNames[1];
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
    p = (char*)Punigp->puniNames[2];
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
    p = (char*)Punigp->puniNames[3];
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

    if (dat->flags & 0x40000) {
        return;
    }

    if (dat->flags & 0x1E0) {
        s32 frame;

        if (dat->flags & 0x20) {
            npc = npcNameToPtr("ガイドsp");
            names = Punigp->puniNames[0];
        } else if (dat->flags & 0x40) {
            npc = npcNameToPtr("ガイド妹sp");
            names = Punigp->puniNames[1];
        } else if (dat->flags & 0x80) {
            npc = npcNameToPtr("ちびトゲsp");
            names = Punigp->puniNames[2];
        } else if (dat->flags & 0x100) {
            npc = npcNameToPtr("ライバルsp");
            names = Punigp->puniNames[3];
        } else {
            npc = npcNameToPtr("ライバルsp");
            names = Punigp->puniNames[3];
        }

        frame = (s32)dat->field_0x1A4;
        if (dat->flags & 0x01000000) {
            (dat->animTime)++;
        } else {
            if (dat->mode != 2) {
                frame = dat->angle;
            } else {
                frame = frame / 8;
                if (dat->animIdx == 0) {
                    frame += dat->angle * 40;
                }
            }
            dat->animTime = gp->retraceCount + frame;
        }

        if (!(dat->flags & 0x20000)) {
            animPoseSetAnim(npc->poseId, names[dat->animIdx], 1);
            animPoseSetLocalTime(npc->poseId, (f32) * (s32*)((u8*)dat + 0x124));
        }

        visible = 0;
        if (dat->flags & 0x400) {
            visible = 1;
        }
        if (dat->flags & 0x800) {
            visible = 0;
        }
        if (visible) {
            npc->position.x = dat->pos.x + dat->field_0x144;
            npc->position.y = dat->pos.y + dat->field_0x148;
            npc->position.z = dat->pos.z + dat->field_0x14C;
        } else {
            npc->position.x = 0.0f;
            npc->position.y = -1000.0f;
            npc->position.z = 0.0f;
        }
        pose->matrix[0][0][3] = 0.0f;
        pose->matrix[0][1][3] = -1000.0f;
        pose->matrix[0][2][3] = 0.0f;
        npc->scale.x = dat->scale;
        npc->scale.y = dat->scale;
        npc->direction = dat->direction;
        npc->rotation.z = dat->field_0x184;
        npc->rotationOffset.y = dat->field_0x188;
    } else {
        MriPuniAnimSet* set;
        s32 count;
        s32 frame;
        s16* anim;
        s16 term;
        s32 idx;

        set = &Punigp->animSet[0];
        if (dat->flags & 0x8) {
            set = &Punigp->animSet[0];
        }
        if (dat->flags & 0x10) {
            set = &Punigp->animSet[1];
        }
        anim = set->unk0[dat->animIdx];
        count = set->unk28[dat->animIdx];
        term = set->unk3C[dat->animIdx];
        frame = (s32)dat->field_0x1A4;

        if (dat->flags & 0x01000000) {
            idx = dat->animTime % count;
            if (term == -2 && count <= dat->animTime) {
                idx = count - 1;
            }
            pose->field_0x1C = anim[idx];
            (dat->animTime)++;
        } else {
            if (dat->mode != 2) {
                frame = dat->angle;
            } else {
                frame = frame / 8;
                if (dat->animIdx == 0) {
                    frame += dat->angle * (count / 3);
                }
            }
            idx = (u32)(gp->retraceCount + frame) % (u32)count;
            pose->field_0x1C = anim[idx];
        }

        visible = 0;
        if (dat->flags & 0x400) {
            visible = 1;
        }
        if (dat->flags & 0x800) {
            visible = 0;
        }
        if (visible) {
            pose->matrix[0][0][3] = dat->pos.x + dat->field_0x144;
            pose->matrix[0][1][3] = dat->pos.y + dat->field_0x148;
            pose->matrix[0][2][3] = dat->pos.z + dat->field_0x14C;
        } else {
            pose->matrix[0][0][3] = 0.0f;
            pose->matrix[0][1][3] = -1000.0f;
            pose->matrix[0][2][3] = 0.0f;
        }
        pose->field_0x14 = dat->scale;
        pose->field_0x4 = -dat->direction;
        pose->field_0x8 = dat->field_0x184;
        pose->field_0xC = dat->field_0x188;
    }

    dat->prevAnimIdx = dat->animIdx;
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

    if (dat->pos.x <= 400.0f && dat->pos.x > 200.0f && dat->pos.y <= 100.0f &&
        dat->pos.y >= 0.0f) {
        z = dat->pos.z;
        if (z <= -55.0f) {
            dat->pos.y = 100.0f;
        } else if (z <= -35.0f) {
            dat->pos.y = ymodeRamp(-35.0f, z, 90.0f);
        } else if (z <= -15.0f) {
            dat->pos.y = ymodeRamp(-15.0f, z, 80.0f);
        } else if (z <= 5.0f) {
            dat->pos.y = ymodeRamp(5.0f, z, 70.0f);
        } else if (z <= 25.0f) {
            dat->pos.y = ymodeRamp(25.0f, z, 60.0f);
        } else if (z <= 45.0f) {
            dat->pos.y = ymodeRamp(45.0f, z, 50.0f);
        } else if (z <= 65.0f) {
            dat->pos.y = ymodeRamp(65.0f, z, 40.0f);
        } else if (z <= 85.0f) {
            dat->pos.y = ymodeRamp(85.0f, z, 30.0f);
        } else if (z <= 105.0f) {
            dat->pos.y = ymodeRamp(105.0f, z, 20.0f);
        } else if (z <= 125.0f) {
            dat->pos.y = ymodeRamp(125.0f, z, 10.0f);
        } else if (z <= 145.0f) {
            dat->pos.y = ymodeRamp(145.0f, z, 0.0f);
        } else {
            dat->pos.y = 0.0f;
        }
    }

    if (dat->pos.x >= -170.0f && dat->pos.x <= 170.0f && dat->pos.z >= -80.0f &&
        dat->pos.z <= 140.0f) {
        dat->pos.y = 0.0f;
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
        puni->pos.y = 10.0f * t + (base);                                                                      \
    } while (0)
void _ymode_mri_16_00(PuniData* puni) {
    u32 entry = *(u32*)((u8*)puni + 0x8); // PuniData 0x8
    u32 flags;
    f32 x, z;

    *(u32*)((u8*)puni + 0x8) = entry & ~0x8; // PuniData 0x8

    if (puni->pos.y <= -200.0f && puni->pos.y >= -400.0f) {
        x = puni->pos.x;
        if (x <= 40.0f && x >= -210.0f && (z = puni->pos.z) <= -76.46314f && z >= -200.0f) {
            if (x <= -190.0f) {
                puni->pos.y = -320.0f;
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
                puni->pos.y = -400.0f;
            }
            if (puni->pos.y < -320.0f && puni->pos.y > -400.0f) {
                *(u32*)((u8*)puni + 0x8) |= 0x8; // PuniData 0x8
            }
        }
        if (puni->pos.x <= 40.0f && puni->pos.x >= -200.0f && puni->pos.z > -76.46314f &&
            puni->pos.z < 200.0f) {
            puni->pos.y = -400.0f;
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
    if (!(puni->pos.z > -76.46314f)) {
        return;
    }
    if (!(puni->field_0x138.z <= -76.46314f)) {
        return;
    }

    *(Vec*)&puni->pos = puni->field_0x138;
    puni->pos.z -= 0.0002f;
}

void _ymode_mri_16_01(PuniData* puni) {
    u32 entry = *(u32*)((u8*)puni + 0x8); // PuniData 0x8
    u32 flags;
    f32 x, z;

    *(u32*)((u8*)puni + 0x8) = entry & ~0x8; // PuniData 0x8

    if (puni->pos.y <= 100.0f && puni->pos.y >= 0.0f) {
        if (puni->pos.x <= 40.0f && puni->pos.x >= -200.0f && puni->pos.z > -200.0f &&
            puni->pos.z < 200.0f) {
            puni->pos.y = 0.0f;
        }
        x = puni->pos.x;
        if (x <= 40.0f && x >= -210.0f && (z = puni->pos.z) <= 60.00006f && z >= -10.00012f) {
            if (x <= -190.0f) {
                puni->pos.y = 80.0f;
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
                puni->pos.y = 0.0f;
            }
            if (puni->pos.y < 80.0f && puni->pos.y > 0.0f) {
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

    if (puni->pos.z > 60.00006f && puni->field_0x138.z <= 60.00006f) {
        *(Vec*)&puni->pos = puni->field_0x138;
        puni->pos.z -= 0.0002f;
    }
    if (puni->pos.z < -10.00012f && puni->field_0x138.z >= -10.00012f) {
        *(Vec*)&puni->pos = puni->field_0x138;
        puni->pos.z += 0.0002f;
    }
}

void puniSetYPosition(PuniData* puni) {
    if ((puni->flags & 1) == 0) {
        return;
    }

    switch (Punigp->yMode) {
    case 0:
        break;
    case 1: {
        f32 x, z;
        s32 ok = 1;

        puni->pos.y = 0.0f;
        x = puni->pos.x;
        z = puni->pos.z;
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
            puni->pos.y = getMri02Yuka();
        }
        break;
    }
    case 2:
        _ymode_mri_09(puni);
        if (swGet(0xB23) == 0) {
            f32 x, z;
            s32 ok = 1;

            x = puni->pos.x;
            z = puni->pos.z;
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
                puni->pos.y = 6.0f;
            }
        }
        break;
    case 3:
        puni->pos.y = 0.0f;
        if (swGet(0xB2A) == 0) {
            f32 x, z;
            s32 ok = 1;

            x = puni->pos.x;
            z = puni->pos.z;
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
                puni->pos.y = 6.0f;
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
        puni->pos.y = 0.0f;
        if (swGet(0xB37) == 0) {
            f32 x, z;
            s32 ok = 1;

            x = puni->pos.x;
            z = puni->pos.z;
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
                puni->pos.y = 6.0f;
            }
        }
        break;
    }

    switch (Punigp->yMode) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        if (11.0 < __fabsf(puni->field_0x138.y - puni->pos.y)) {
            puni->pos.y = puni->field_0x138.y;
        }
        break;
    }
}

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
    switch (Punigp->mapKind) {
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
    switch (Punigp->mapKind) {
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
    u32 info = map->grid[y * map->width + x];

    if ((info & 0xFF) == 0)
        return info | 0xFFFF;
    return info;
}

void puniMapInit(void) {
    Punimap = _mapAlloc(0x24);
    memset(Punimap, 0, 0x24);
}

void puniMapDataSet(s32 w, s32 h, f32 a, f32 b, void* data) {
    Punimap->yMax = a;
    Punimap->yMin = b;
    if (data == NULL) {
        Punimap->hasData = 0;
    } else {
        Punimap->width = w;
        Punimap->height = h;
        Punimap->hasData = 1;
        Punimap->gridSrc = data;
        Punimap->valid = 0;
        Punimap->dataSize = w * 4 * h;
        Punimap->grid = _mapAlloc(Punimap->dataSize);
        memcpy(Punimap->gridSrc, data, Punimap->dataSize);
    }
}

// The caller passes a local position copy, independent of the map and Puni work buffers.
void puniMapFindbuff(Vec* __restrict pos) {
    s32 index;
    u16 hi;
    u16* buf;
    u16* above;
    u16* left;
    u16 id;
    s32 i;
    u16* below;
    s32 total;
    u16 cur;
    u16* right;
    s32 changed;
    u16 nid;
    s32 out1;
    s32 out2;

    if (Punimap->hasData == 0)
        return;
    if (Punimap->yMax < pos->y || Punimap->yMin > pos->y) {
        Punimap->valid = 0;
        return;
    }
    memcpy(Punimap->grid, Punimap->gridSrc, Punimap->dataSize);
    Punimap->valid = 1;
    {
        f32 z = pos->z;
        f32 x = pos->x;

        switch (Punigp->mapKind) {
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
    index = Punimap->width * out2 + out1;
    if (index < 0)
        Punimap->valid = 0;
    if (Punimap->valid == 0)
        return;
    Punimap->grid[index] |= 1;
    id = 1;
    while (1) {
        changed = 0;
        total = Punimap->width * Punimap->height;
        for (i = 0; i < total; i++) {
            buf = Punimap->grid;
            cur = buf[i];

            if (cur == 0xFFFF)
                continue;
            if (id != (cur & 0xFFF))
                continue;
            {
                hi = cur & 0xF000;
                above = &buf[i - Punimap->width];
                below = &buf[i + Punimap->width];
                left = &buf[i - 1];
                right = &buf[i + 1];
                {
                    u16 nb = *above;
                    if ((nb & 0xFF) == 0 && (nb & 0xFF) < id && abs((nb & 0xF000) - hi) <= 0x1000) {
                        nid = id + 1;
                        *above = nb | nid;
                        changed = 1;
                    }
                }
                {
                    u16 nb = *below;
                    if ((nb & 0xFF) == 0 && (nb & 0xFF) < id && abs((nb & 0xF000) - hi) <= 0x1000) {
                        nid = id + 1;
                        *below = nb | nid;
                        changed = 1;
                    }
                }
                {
                    u16 nb = *left;
                    if ((nb & 0xFF) == 0 && (nb & 0xFF) < id && abs((nb & 0xF000) - hi) <= 0x1000) {
                        nid = id + 1;
                        *left = nb | nid;
                        changed = 1;
                    }
                }
                {
                    u16 nb = *right;
                    if ((nb & 0xFF) == 0 && (nb & 0xFF) < id && abs((nb & 0xF000) - hi) <= 0x1000) {
                        nid = id + 1;
                        *right = nb | nid;
                        changed = 1;
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
    PuniTarget* target = Punigp->targets; // PuniWork 0x374, 10 entries of 0x14


    punidat->field_0x1B4 = -1;
    punidat->field_0x1B8 = 1e+06f;
    punidat->field_0x1BC = -1;
    punidat->field_0x1C0 = 1e+06f;
    for (i = 0; i < 10; i++, target++) {
        if (target->flags & 1) {
            f64 limit = 32.0;

            if (limit < __fabs(punidat->pos.y - target->position.y))
                continue;
            if (target->flags & 4) {
                f32 dist = PSVECSquareDistance(&punidat->pos, &target->position);
                if (punidat->field_0x1C0 > dist) {
                    punidat->field_0x1BC = i;
                    punidat->field_0x1C0 = dist;
                }
            }
            if (target->flags & 2) {
                f32 dist = PSVECSquareDistance(&punidat->pos, &target->position);
                if (punidat->field_0x1B8 > dist) {
                    punidat->field_0x1B4 = i;
                    punidat->field_0x1B8 = dist;
                }
            }
        }
    }
}

void puniswGetindex(u32 base, s32 index, s32* out, Vec* pos) {
    u8 b[7];

    b[0] = _swByteGet(base + index * 7 + 0x8F0D180);
    b[1] = _swByteGet(base + index * 7 + 0x8F0D181);
    b[2] = _swByteGet(base + index * 7 + 0x8F0D182);
    b[3] = _swByteGet(base + index * 7 + 0x8F0D183);
    b[4] = _swByteGet(base + index * 7 + 0x8F0D184);
    b[5] = _swByteGet(base + index * 7 + 0x8F0D185);
    b[6] = _swByteGet(base + index * 7 + 0x8F0D186);

    *out = b[0];
    {
        s32 x = b[1] + (b[2] << 8);

        if (x & 0x8000)
            x |= 0xFFFF0000;
        {
            s32 y = b[3] + (b[4] << 8);

            if (y & 0x8000)
                y |= 0xFFFF0000;
            {
                s32 z = b[5] + (b[6] << 8);

                if (z & 0x8000)
                    z |= 0xFFFF0000;
                pos->x = x;
                pos->y = y;
                pos->z = z;
            }
        }
    }
}

// Writes the record puniswGetindex reads, through the same u8 b[7]. Lifetime splitting is off here and in the three
// callers that inline it (evt_puni_exit, evt_puni_all_save, evt_puni_set_lsw_index): with it on, the second stores to
// b[2]/b[4] become renames numbered after the offset's common subexpression, and the original colours the high bytes ahead of it.
#pragma opt_lifetimes off
void puniswSetindex(u32 base, s32 index, s32* out, Vec* pos) {
    u8 b[7];
    s32 x = pos->x;
    s32 y = pos->y;
    s32 z = pos->z;

    b[0] = *out;
    b[1] = x & 0xFF;
    b[2] = (x >> 8) & 0xFF;
#ifdef MUST_MATCH
    // Redundant re-masks: the second definitions of b[2]/b[4] make MWCC compute the x and y high bytes up front in
    // callee-saved registers (z's stays at its call), as the original does. Without them every byte is computed at
    // its _swByteSet (88.5%; s16/u16 locals, implicit u8 truncation and `& 0xFF00` forms score lower).
    b[2] = b[2] & 0xFF;
#endif
    b[3] = y & 0xFF;
    b[4] = (y >> 8) & 0xFF;
#ifdef MUST_MATCH
    b[4] = b[4] & 0xFF;
#endif
    b[5] = z & 0xFF;
    b[6] = (z >> 8) & 0xFF;
    _swByteSet(base + index * 7 + 0x8F0D180, b[0]);
    _swByteSet(base + index * 7 + 0x8F0D181, b[1]);
    _swByteSet(base + index * 7 + 0x8F0D182, b[2]);
    _swByteSet(base + index * 7 + 0x8F0D183, b[3]);
    _swByteSet(base + index * 7 + 0x8F0D184, b[4]);
    _swByteSet(base + index * 7 + 0x8F0D185, b[5]);
    _swByteSet(base + index * 7 + 0x8F0D186, b[6]);
}
#pragma opt_lifetimes reset

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
    Punigp->randSeedA = seed + 0x3A3B01F9;
    Punigp->randSeedB = 0x21708331;
}

s32 qrnd(s32 max) {
    s32 v;

    Punigp->randSeedA += Punigp->randSeedB + 0x17340000 - 0x506F;
    Punigp->randSeedB += Punigp->randSeedA + 0x21270000 + 0x3F9C;
    v = Punigp->randSeedB;
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
    EffectEntry* effect = (EffectEntry*)puni->effect;

    if (effect != NULL) {
        effDelete(effect);
        puni->effect = 0;
    }
}

void _daime_change(PuniData* puni) {
    if ((PuniData*)puni->leftPuni != NULL) {
        ((PuniData*)puni->leftPuni)->field_0x11A--;
        _daime_change((PuniData*)puni->leftPuni);
    }
    if ((PuniData*)puni->rightPuni != NULL) {
        ((PuniData*)puni->rightPuni)->field_0x11A--;
        _daime_change((PuniData*)puni->rightPuni);
    }
}

void PuniDokuritsu(PuniData* puni) {
    PuniData* left;
    PuniData* right;
    PuniData* parent;

    puni->field_0x118 = -1;
    puni->flags &= ~0x00080000;
    puni->field_0x11A = 0;
    left = (PuniData*)puni->leftPuni;
    parent = (PuniData*)puni->parentPuni;
    right = (PuniData*)puni->rightPuni;
    puni->parentPuni = 0;
    puni->leftPuni = 0;
    puni->rightPuni = 0;

    if (left != NULL) {
        left->field_0x11A--;
        _daime_change(left);
        left->parentPuni = (s32)parent;
        if ((PuniData*)left->parentPuni != NULL) {
            if ((PuniData*)parent->leftPuni == puni) {
                parent->leftPuni = (s32)left;
            } else if ((PuniData*)parent->rightPuni == puni) {
                parent->rightPuni = (s32)left;
            }
        }
        left->rightPuni = (s32)right;
        if ((PuniData*)left->rightPuni != NULL) {
            ((PuniData*)left->rightPuni)->parentPuni = (s32)left;
        }
    } else if (right != NULL) {
        right->field_0x11A--;
        _daime_change(right);
        right->parentPuni = (s32)parent;
        if ((PuniData*)right->parentPuni != NULL) {
            if ((PuniData*)parent->leftPuni == puni) {
                parent->leftPuni = (s32)right;
            } else if ((PuniData*)parent->rightPuni == puni) {
                parent->rightPuni = (s32)right;
            }
        }
        right->leftPuni = (s32)left;
        if ((PuniData*)right->leftPuni != NULL) {
            ((PuniData*)right->leftPuni)->parentPuni = (s32)right;
        }
    } else if (parent != NULL) {
        if ((PuniData*)parent->leftPuni == puni) {
            parent->leftPuni = 0;
        } else if ((PuniData*)parent->rightPuni == puni) {
            parent->rightPuni = 0;
        }
    }
}

PuniData* PuniSakanobori(PuniData* puni) {
    PuniData* next;

    while ((next = (PuniData*)puni->parentPuni) != NULL) {
        puni = next;
    }
    return puni;
}

static inline void puniHagureChild(PuniData* parent, PuniData* child) {
    child->mode = 0x10;
    parent->state = 0;
    PuniChildHagure(child);
    PuniDokuritsu(child);
}

// Each level of the recursion costs two inline levels (helper + PuniChildHagure); depth 5 inlines it as far as retail.
#pragma push
#pragma inline_depth(5)
void PuniChildHagure(PuniData* puni) {
    if ((PuniData*)puni->leftPuni != NULL) {
        puniHagureChild(puni, (PuniData*)puni->leftPuni);
    }
    if ((PuniData*)puni->rightPuni != NULL) {
        puniHagureChild(puni, (PuniData*)puni->rightPuni);
    }
}
#pragma pop

s32 searchParent(PuniData* puni) {
    s32 best = 10000;
    PuniData* parent;
    s32 i;
    PuniData* p;

    p = Punidat;
    for (i = 0; i < PUNI_DATA_MAX; i++, p++) {
        if (!(p->flags & 0x00080000) || (PuniData*)p->rightPuni != NULL) {
            continue;
        }
        if ((PuniData*)p->leftPuni != NULL && p->field_0x11A >= 2) {
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
    puni->parentPuni = (s32)parent;
    puni->field_0x11A = best;
    puni->field_0x118 = parent->field_0x118;
    if ((PuniData*)parent->leftPuni == NULL) {
        parent->leftPuni = (s32)puni;
    } else {
        parent->rightPuni = (s32)puni;
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
    Vec hit;
    Vec hitN;
    f32 distance;
    s32 inBand = 0;
    PuniWork* gp = Punigp;

    top = gp->bandTop0;
    bottom = gp->bandBottom0;
    if (-1000.0f != top && puni->pos.y < top && puni->pos.y > bottom) {
        inBand = 1;
    }
    top = gp->bandTop1;
    bottom = gp->bandBottom1;
    if (-1000.0f != top && puni->pos.y < top && puni->pos.y > bottom) {
        inBand = 1;
    }
    if (inBand == 0) {
        return 1;
    }
    pos = puni->pos;
    pos.y += 1.0f;
    normal = (Vec){0.0f, -1.0f, 0.0f};
    distance = 3.0f;
    if (hitCheckFilter(0, pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, &hit.x, &hit.y, &hit.z, &distance, &hitN.x,
                       &hitN.y, &hitN.z) == NULL) {
        return 2;
    }
    return 0;
}

void puniPositionRecover(PuniData* puni) {
    puni->pos = Punigp->respawnPos;

    puni->pos.x += randf(0.0f, 2.0f) - 1.0f;
    puni->pos.z += randf(0.0f, 2.0f) - 1.0f;

    *(Vec*)&puni->field_0x138 = puni->pos;
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

    if (punidat->flags & 0x1000) {
        punidat->field_0x210 =
            hitCheckFilter(0, pos.x, pos.y, pos.z, vel.x, vel.y, vel.z, &hit.x, &hit.y, &hit.z,
                                &dist, &normal.x, &normal.y, &normal.z);
    }
    if (punidat->field_0x210 == NULL) {
        PSVECAdd(&punidat->pos, &punidat->vel, &punidat->pos);
        goto done;
    }
    start = punidat->pos;
    punidat->field_0x560 = normal;
    PSVECAdd(&punidat->pos, &punidat->vel, &punidat->pos);
    if (0.0f == punidat->vel.y) {
        normal.y = 0.0f;
    }
    PSVECNormalize(&normal, &normal);
    if (arg3 != 0) {
        C_VECReflect(&vel, &normal, &reflect);
        PSVECScale(&reflect, &punidat->vel, mag);
        result = 0;
    }
    {
        f32 dot = PSVECDotProduct(&vel, &normal);

        PSVECScale(&normal, &normal, dot * -(mag - dist) + 0.1f);
    }
    PSVECAdd(&punidat->pos, &normal, &punidat->pos);
    PSVECSubtract(&punidat->pos, &start, &dir);
    dist = PSVECMag(&dir);
    if (dist > 0.0f) {
        PSVECNormalize(&dir, &dir);
        pos = start;
        pos.y += 1.0f;
        punidat->field_0x214 =
            hitCheckFilter(0, start.x, start.y + 1.0f, start.z, dir.x, dir.y, dir.z, &hit2x, &hit2y,
                                &hit2z, &dist, &normal2x, &normal2y, &normal2z);
        if (punidat->field_0x214 != NULL) {
            punidat->pos = start;
        }
    }
done:
    return result;
}

void unk_mri_000095f0(PuniData* puni, Vec pos, Vec vel, f32 mag) {
    Vec hit;
    Vec normal;
    Vec dir;
    Vec start;
    f32 hit2x, hit2y, hit2z;
    f32 normal2x, normal2y, normal2z;
    f32 dist;

    puni->field_0x214 = NULL;
    puni->field_0x210 = NULL;
    dist = mag;

    puni->field_0x210 =
        hitCheckFilter(0, pos.x, pos.y, pos.z, vel.x, vel.y, vel.z, &hit.x, &hit.y, &hit.z, &dist,
                       &normal.x, &normal.y, &normal.z);
    if (puni->field_0x210 == NULL) {
        PSVECAdd(&puni->pos, &puni->vel, &puni->pos);
        return;
    }
    if (Punigp->area == 0x12) {
        return;
    }
    start = puni->pos;
    puni->field_0x560 = normal;
    PSVECAdd(&puni->pos, &puni->vel, &puni->pos);
    if (0.0f == puni->vel.y) {
        normal.y = 0.0f;
    }
    PSVECNormalize(&normal, &normal);
    {
        f32 dot = PSVECDotProduct(&vel, &normal);

        PSVECScale(&normal, &normal, dot * -(mag - dist) + 0.1f);
    }
    PSVECAdd(&puni->pos, &normal, &puni->pos);
    PSVECSubtract(&puni->pos, &start, &dir);
    dist = PSVECMag(&dir);
    if (dist > 0.0f) {
        PSVECNormalize(&dir, &dir);
        puni->field_0x214 =
            hitCheckFilter(0, start.x, start.y + 1.0f, start.z, dir.x, dir.y, dir.z, &hit2x, &hit2y,
                           &hit2z, &dist, &normal2x, &normal2y, &normal2z);
        if (puni->field_0x214 != NULL) {
            puni->pos = start;
        }
    }
}

void unk_mri_0000980c(PuniData* punidat, Vec pos, Vec vel, f32 mag) {
    Vec hit;
    Vec normal;
    Vec dir;
    Vec reflect;
    Vec start;
    f32 hit2x, hit2y, hit2z;
    f32 normal2x, normal2y, normal2z;
    f32 dist;

    punidat->field_0x214 = NULL;
    punidat->field_0x210 = NULL;
    dist = mag;

    if (punidat->flags & 0x1000) {
        punidat->field_0x210 =
            hitCheckFilter(0, pos.x, pos.y, pos.z, vel.x, vel.y, vel.z, &hit.x, &hit.y, &hit.z,
                                &dist, &normal.x, &normal.y, &normal.z);
    }
    if (punidat->field_0x210 == NULL) {
        PSVECAdd(&punidat->pos, &punidat->vel, &punidat->pos);
        return;
    }
    start = punidat->pos;
    punidat->field_0x560 = normal;
    PSVECAdd(&punidat->pos, &punidat->vel, &punidat->pos);
    if (0.0f == punidat->vel.y) {
        normal.y = 0.0f;
    }
    PSVECNormalize(&normal, &normal);
    C_VECReflect(&vel, &normal, &reflect);
    PSVECScale(&reflect, &punidat->vel, mag);
    {
        f32 dot = PSVECDotProduct(&vel, &normal);

        PSVECScale(&normal, &normal, dot * -(mag - dist) + 0.1f);
    }
    PSVECAdd(&punidat->pos, &normal, &punidat->pos);
    PSVECSubtract(&punidat->pos, &start, &dir);
    dist = PSVECMag(&dir);
    if (dist > 0.0f) {
        PSVECNormalize(&dir, &dir);
        pos = start;
        pos.y += 1.0f;
        punidat->field_0x214 =
            hitCheckFilter(0, start.x, start.y + 1.0f, start.z, dir.x, dir.y, dir.z, &hit2x, &hit2y,
                                &hit2z, &dist, &normal2x, &normal2y, &normal2z);
        if (punidat->field_0x214 != NULL) {
            punidat->pos = start;
        }
    }
}

void puniMoveDecel(PuniData* puni) {
    f32 mag;
    Vec up;
    Vec tmp;
    Vec up2;
    Vec dir;
    Vec saved;
    Vec refl;
    Vec diff;
    Vec hitN;
    Vec hitP;
    f32 dist;
    f32 hitN2z;
    f32 hitN2y;
    f32 hitN2x;
    f32 hitP2z;
    f32 hitP2y;
    f32 hitP2x;
    f32 scale;
    f32 k;

    mag = PSVECMag(&puni->vel);

#ifdef MUST_MATCH
    // named zero forces the original's fcmpu operand order (f1=mag, f0=zero)
    {
        f32 zero = 0.0f;

        if (mag == zero) {
            return;
        }
    }
#else
    if (mag == 0.0f) {
        return;
    }
#endif
    if (mag > 1.0f) {
        if (puni->vel.x < 0.0f) {
            puni->direction = 270.0f;
        } else {
            puni->direction = 90.0f;
        }
    }
    if (mag > puni->field_0x174) {
        PSVECScale(&puni->vel, &puni->vel, puni->field_0x174 / mag);
        mag = puni->field_0x174;
    }

    up = puni->pos;
    up.y += 1.0f;
    PSVECNormalize(&puni->vel, &tmp);
    dir = tmp;
    up2 = up;

    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dist = mag;
    if (puni->flags & 0x1000) {
        puni->field_0x210 =
            hitCheckFilter(0, up2.x, up2.y, up2.z, dir.x, dir.y, dir.z, &hitP.x, &hitP.y, &hitP.z,
                                &dist, &hitN.x, &hitN.y, &hitN.z);
    }
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->pos, &puni->vel, &puni->pos);
    } else {
        saved = puni->pos;
        puni->field_0x560 = hitN;
        PSVECAdd(&puni->pos, &puni->vel, &puni->pos);
        if (puni->vel.y == 0.0f) {
            hitN.y = 0.0f;
        }
        PSVECNormalize(&hitN, &hitN);
        C_VECReflect(&dir, &hitN, &refl);
        PSVECScale(&refl, &puni->vel, mag);
        scale = PSVECDotProduct(&dir, &hitN);
        k = scale * -(mag - dist) + 0.1f;
        PSVECScale(&hitN, &hitN, k);
        PSVECAdd(&puni->pos, &hitN, &puni->pos);
        PSVECSubtract(&puni->pos, &saved, &diff);
        dist = PSVECMag(&diff);
        if (dist > 0.0f) {
            PSVECNormalize(&diff, &diff);
            puni->field_0x214 =
                hitCheckFilter(0, saved.x, saved.y + 1.0f, saved.z, diff.x, diff.y, diff.z, &hitP2x,
                                    &hitP2y, &hitP2z, &dist, &hitN2x, &hitN2y, &hitN2z);
            if (puni->field_0x214 != 0) {
                puni->pos = saved;
            }
        }
    }
    if (puni->field_0x210 != 0 || puni->field_0x214 != 0) {
        puni->flags |= 0x00400000;
    } else {
        PSVECScale(&puni->vel, &puni->vel, 0.6f);
        puni->flags &= ~0x00400000;
    }
}

void puniMoveLimitedNormal(PuniData* puni) {
    f32 mag = PSVECMag(&puni->vel);
    Vec up;
    Vec tmp;
    Vec up2;
    Vec dir;
    Vec saved;
    Vec diff;
    Vec hitN;
    Vec hitP;
    f32 dist;
    f32 hitN2z;
    f32 hitN2y;
    f32 hitN2x;
    f32 hitP2z;
    f32 hitP2y;
    f32 hitP2x;
    f32 scale;
    f32 zero = 0.0f;

    if (mag == zero) {
        return;
    }
    if (mag > puni->field_0x174) {
        PSVECScale(&puni->vel, &puni->vel, puni->field_0x174 / mag);
        mag = puni->field_0x174;
    }

    up = puni->pos;
    up.y += 1.0f;
    PSVECNormalize(&puni->vel, &tmp);
    dir = tmp;
    up2 = up;

    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dist = mag;
    puni->field_0x210 =
        hitCheckFilter(0, up2.x, up2.y, up2.z, dir.x, dir.y, dir.z, &hitP.x, &hitP.y, &hitP.z,
                            &dist, &hitN.x, &hitN.y, &hitN.z);
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->pos, &puni->vel, &puni->pos);
    } else if (Punigp->area != 0x12) {
        saved = puni->pos;
        puni->field_0x560 = hitN;
        PSVECAdd(&puni->pos, &puni->vel, &puni->pos);
        if (0.0f == puni->vel.y) {
            hitN.y = 0.0f;
        }
        PSVECNormalize(&hitN, &hitN);
        scale = PSVECDotProduct(&dir, &hitN);
        PSVECScale(&hitN, &hitN, scale * -(mag - dist) + 0.1f);
        PSVECAdd(&puni->pos, &hitN, &puni->pos);
        PSVECSubtract(&puni->pos, &saved, &diff);
        dist = PSVECMag(&diff);
        if (dist > 0.0f) {
            PSVECNormalize(&diff, &diff);
            puni->field_0x214 =
                hitCheckFilter(0, saved.x, 1.0f + saved.y, saved.z, diff.x, diff.y, diff.z, &hitP2x,
                                    &hitP2y, &hitP2z, &dist, &hitN2x, &hitN2y, &hitN2z);
            if (puni->field_0x214 != 0) {
                puni->pos = saved;
            }
        }
    }
    if (puni->field_0x210 != 0 || puni->field_0x214 != 0) {
        puni->flags |= 0x00400000;
    } else {
        puni->flags &= ~0x00400000;
    }
}

void puniMoveNormal(PuniData* puni) {
    f32 mag = PSVECMag((Vec*)&puni->vel);
    Vec up;
    Vec tmp;
    Vec up2;
    Vec dir;
    Vec saved;
    Vec refl;
    Vec diff;
    Vec hitN;
    Vec hitP;
    f32 dist;
    f32 hitN2z;
    f32 hitN2y;
    f32 hitN2x;
    f32 hitP2z;
    f32 hitP2y;
    f32 hitP2x;
    f32 scale;
    f32 k;

#ifdef MUST_MATCH
    // named zero forces the original's fcmpu operand order (f1=mag, f0=zero)
    {
        f32 zero = 0.0f;

        if (mag == zero) {
            return;
        }
    }
#else
    if (mag == 0.0f) {
        return;
    }
#endif

    up = puni->pos;
    up.y += 1.0f;
    PSVECNormalize((Vec*)&puni->vel, &tmp);
    dir = tmp;
    up2 = up;

    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dist = mag;
    if (puni->flags & 0x1000) {
        puni->field_0x210 =
            hitCheckFilter(0, up2.x, up2.y, up2.z, dir.x, dir.y, dir.z, &hitP.x, &hitP.y, &hitP.z,
                                &dist, &hitN.x, &hitN.y, &hitN.z);
    }
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->pos, (Vec*)&puni->vel, &puni->pos);
    } else {
        saved = puni->pos;
        puni->field_0x560 = hitN;
        PSVECAdd(&puni->pos, (Vec*)&puni->vel, &puni->pos);
        if (((Vec*)&puni->vel)->y == 0.0f) {
            hitN.y = 0.0f;
        }
        PSVECNormalize(&hitN, &hitN);
        C_VECReflect(&dir, &hitN, &refl);
        PSVECScale(&refl, (Vec*)&puni->vel, mag);
        scale = PSVECDotProduct(&dir, &hitN);
        k = scale * -(mag - dist) + 0.1f;
        PSVECScale(&hitN, &hitN, k);
        PSVECAdd(&puni->pos, &hitN, &puni->pos);
        PSVECSubtract(&puni->pos, &saved, &diff);
        dist = PSVECMag(&diff);
        if (dist > 0.0f) {
            PSVECNormalize(&diff, &diff);
            puni->field_0x214 =
                hitCheckFilter(0, saved.x, saved.y + 1.0f, saved.z, diff.x, diff.y, diff.z, &hitP2x,
                                    &hitP2y, &hitP2z, &dist, &hitN2x, &hitN2y, &hitN2z);
            if (puni->field_0x214 != 0) {
                puni->pos = saved;
            }
        }
    }
    if (puni->field_0x210 != 0 || puni->field_0x214 != 0) {
        puni->flags |= 0x00400000;
    } else {
        puni->flags &= ~0x00400000;
    }
}

void puniMoveSimple(PuniData* puni) {
    f32 mag = PSVECMag((Vec*)&puni->vel);
#ifdef MUST_MATCH
    // named zero forces the original's fcmpu operand order (f1=mag, f0=zero)
    f32 zero = 0.0f;

    if (mag != zero) {
#else
    if (mag != 0.0f) {
#endif
        PSVECAdd(&puni->pos, (Vec*)&puni->vel, &puni->pos);
    }
}

void puniMoveCustom(PuniData* puni) {
    void (*move)(PuniData*) = (void (*)(PuniData*))puni->field_0x570;

    if (move != NULL) {
        move(puni);
    }
}

void puniParaAttack(PuniData* puni, PuniData* other) {
    s16 chance;
    s16 span;
    s16 base;
    s32 flag;

    if (other->engaged == 0 && other->mode == 9) {
        return;
    }

#ifdef MUST_MATCH
    // `base` makes MWCC keep the original's mr chance<-base before the add
    if (other->mode != 8) {
        base = puni->hitChance;
        chance = base;
        span = base + other->hitChance;
    } else {
        base = puni->hitChanceAlt;
        chance = base;
        span = base + (100 - other->hitChanceAlt);
    }
#else
    if (other->mode != 8) {
        chance = puni->hitChance;
        span = chance + other->hitChance;
    } else {
        chance = puni->hitChanceAlt;
        span = chance + (100 - other->hitChanceAlt);
    }
#endif

#ifdef MUST_MATCH
    // splitting the `||` makes MWCC keep the original's branchless compare + li form;
    // writing it as `roll < chance` (not `chance > roll`) is what keeps chance in r6
    flag = (s16)puni_rnd(span) < chance;
    if (other->flags & 0x08000000) {
        flag = 1;
    }
    if (flag) {
#else
    if (chance > (s16)puni_rnd(span) || (other->flags & 0x08000000)) {
#endif
        if (puni->engaged == 0) {
            other->damage = puni->attack - other->defense;
            other->vel = (Vec){0.0f, 0.0f, 0.0f};
            other->target = (s32)puni;
            other->flags |= 0x2000;
            other->flags &= ~0x08000000;
            other->field_0x8 &= ~0x2;
            other->mode = 9;
            other->state = 0;
            other->flags &= ~0x02000000;
        }
    } else if (other->engaged == 0) {
        puni->damage = other->attack - puni->defense;
        puni->vel = (Vec){0.0f, 0.0f, 0.0f};
        puni->target = (s32)other;
        puni->flags |= 0x2000;
        puni->flags &= ~0x08000000;
        puni->field_0x8 &= ~0x2;
        puni->mode = 9;
        puni->state = 0;
        puni->flags &= ~0x02000000;
    }
    puni->engaged = 1;
    other->engaged = 1;
}

void puniParaDamage(PuniData* puni) {
    s16* hp = &puni->hp;
    s16 damage = puni->damage;

    *hp -= damage;
    if (*hp <= 0) {
        *hp = 0;
        puni->flags |= 0x4000;
        puni->field_0x8 |= 0x10;
    }
}

// The compare and the add read field_0x574 again rather than through `before` (found by the permuter): with
// `before` in both, `before` lands in r5 where the target has r4.
void puniParaRecover(PuniData* puni) {
    s16 before;
    static u32 debug;

    debug = (u32)puni;
    if (puni->field_0x1AC > 0.001f || puni->mode == 0xD || puni->mode == 0xA) {
        puni->regenTimer = puni->regenInterval;
        return;
    }

    puni->regenTimer -= 1;
    if (puni->regenTimer != 0) {
        return;
    }

    before = puni->hp;
    if (puni->hp < puni->maxHp) {
        puni->hp = puni->hp + puni->hpRegen;
        if (puni->hp > puni->maxHp) {
            puni->hp = puni->maxHp;
        }
        if (puni->hp > before) {
            puni->field_0x8 |= 1;
            puni->field_0x121 = 0;
        }
    }
    puni->regenTimer = puni->regenInterval;
}

s32 puniParaDangerCheck(PuniData* puni) {
    return 30 >= puni->hp * 100 / puni->maxHp;
}

s32 puniParaSelfCheck(PuniData* puni) {
    return puni->hp * 100 / puni->maxHp >= 80;
}

s32 puniParaRecoverCheck(PuniData* puni) {
    return puni->hp >= puni->maxHp;
}

static inline s32 puniQrnd2(s32 range) {
    s32 v;

    Punigp->randSeedA += Punigp->randSeedB + 0x17340000 - 0x506F;
    Punigp->randSeedB += Punigp->randSeedA + 0x21270000 + 0x3F9C;
    v = Punigp->randSeedB;
    if ((s32)(v ^ 0) < 0) {
        v = -v;
    }
    return v % range;
}


PuniData* puniSearchRandom(s32 start, s32 count) {
    s32 end;
    s32 idx;
    s32 i;
    PuniData* puni;
    s32 v;

    Punigp->randSeedA += Punigp->randSeedB + 0x17340000 - 0x506F;
    Punigp->randSeedB += Punigp->randSeedA + 0x21270000 + 0x3F9C;
    v = Punigp->randSeedB;
    if ((s32)(v ^ 0) < 0) {
        v = -v;
    }
    idx = start + v % (count - 1);
    end = start + count;
    for (i = 0;; i++) {
        if (i == count) {
            return NULL;
        }
        if (idx == end) {
            idx = start;
        }
        puni = &Punidat[idx];
        if ((puni->flags & 0x400) != 0 && (puni->flags & 0x4000) == 0) {
            break;
        }
        idx++;
    }
    return puni;
}

PuniData* puniSearchRandom_Mikata(void) {
    s32 i;
    s32 idx;
    s32 range;
    PuniData* base;
    s32 r;
    PuniData* puni;

    range = 101;
    Punigp->randSeedA += Punigp->randSeedB + 0x17340000 - 0x506F;
    Punigp->randSeedB += Punigp->randSeedA + 0x21270000 + 0x3F9C;
    r = Punigp->randSeedB;
    if ((s32)(r ^ 0) < 0) {
        r = -r;
    }
    idx = r % range + 1;
    base = Punidat;
    i = 0;
    while (1) {
        if (i == 102) {
            return NULL;
        }
        if (idx == 103) {
            idx = 1;
        }
        puni = &base[idx];
        {
            s32 flag = puni->flags;
            if ((flag & 0x400) != 0 && (flag & 0x4000) == 0) {
                break;
            }
        }
        idx++;
        i++;
    }
    return puni;
}

PuniData* puniSearchRandom_Teki(void) {
    s32 i;
    s32 idx;
    s32 range;
    PuniData* base;
    s32 r;
    PuniData* puni;

    range = 99;
    Punigp->randSeedA += Punigp->randSeedB + 0x17340000 - 0x506F;
    Punigp->randSeedB += Punigp->randSeedA + 0x21270000 + 0x3F9C;
    r = Punigp->randSeedB;
    if ((s32)(r ^ 0) < 0) {
        r = -r;
    }
    idx = r % range + 110;
    base = Punidat;
    i = 0;
    while (1) {
        if (i == 100) {
            return NULL;
        }
        if (idx == 210) {
            idx = 110;
        }
        puni = &base[idx];
        {
            s32 flag = puni->flags;
            if ((flag & 0x400) != 0 && (flag & 0x4000) == 0) {
                break;
            }
        }
        idx++;
        i++;
    }
    return puni;
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
        if ((cur->flags & 0x400) && !(cur->flags & 0x4000) && cur->mode != 9 && cur->mode != 0xD) {
            d = PSVECSquareDistance(&puni->pos, &cur->pos);
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
        if ((cur->flags & 0x400) && !(cur->flags & 0x4000) && cur->mode != 9 && cur->mode != 0xD) {
            d = PSVECSquareDistance(&puni->pos, &cur->pos);
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
        if ((cur->flags & 0x400) && !(cur->flags & 0x4000) && cur->mode != 9 && cur->mode != 0xD) {
            d = PSVECSquareDistance(&puni->pos, &cur->pos);
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
        if ((cur->flags & 0x400) && !(cur->flags & 0x4000) && cur->mode != 9 && cur->mode != 0xD) {
            d = PSVECSquareDistance(&puni->pos, &cur->pos);
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
        if ((cur->flags & 0x400) && !(cur->flags & 0x4000) && cur->mode != 9 && cur->mode != 0xD) {
            d = PSVECSquareDistance(&puni->pos, &cur->pos);
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
static inline PuniData* puniSearchTekiNamed(PuniData* puni) {
    s32 i;
    PuniData* cur;
    PuniData* best;
    f32 bestDist;
    f32 d;

    best = NULL;
    bestDist = 1e+06f;
    cur = &Punidat[0x6E];
    for (i = 0x6E; i < 0xD2; i++, cur++) {
        if ((cur->flags & 0x400) && !(cur->flags & 0x4000) && cur->mode != 9 && cur->mode != 0xD) {
            d = PSVECSquareDistance(&puni->pos, &cur->pos);
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
static inline PuniData* puniSearchMikataNamed(PuniData* puni) {
    s32 i;
    PuniData* cur;
    PuniData* best;
    f32 bestDist;
    f32 d;

    best = NULL;
    bestDist = 1e+06f;
    cur = &Punidat[1];
    for (i = 1; i < 103; i++, cur++) {
        if ((cur->flags & 0x400) && !(cur->flags & 0x4000) && cur->mode != 9 && cur->mode != 0xD) {
            d = PSVECSquareDistance(&puni->pos, &cur->pos);
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



void puniSearch(PuniData* puni) {
    if ((s32)(gp->retraceCount & 3) != (puni->angle & 3)) {
        return;
    }
    if (puni->flags & 1) {
        *(PuniData**)((u8*)puni + 0x1DC) = puniSearchTekiNamed(puni);
    } else {
        *(PuniData**)((u8*)puni + 0x1DC) = puniSearchMikataNamed(puni);
    }
}
static inline PuniData* puniSearchTekiInlineATK(PuniData* puni) {
    s32 i;
    PuniData* cur;
    PuniData* best;
    f32 bestDist;
    f32 d;

    best = NULL;
    bestDist = 1e+06f;
    cur = &Punidat[0x6E];
    for (i = 0x6E; i < 0xD2; i++, cur++) {
        if ((cur->flags & 0x400) && !(cur->flags & 0x4000) && cur->mode != 9 && cur->mode != 0xD) {
            d = PSVECSquareDistance(&puni->pos, &cur->pos);
            if (d < bestDist) {
                bestDist = d;
                best = cur;
            }
        }
    }
    {
        PuniData* result;

        if (bestDist < 1e+06f) {
            result = best;
        } else {
            result = NULL;
        }
        return result;
    }
}
static inline PuniData* puniSearchMikataInlineATK(PuniData* puni) {
    s32 i;
    PuniData* cur;
    PuniData* best;
    f32 bestDist;
    f32 d;

    best = NULL;
    bestDist = 1e+06f;
    cur = &Punidat[1];
    for (i = 1; i < 103; i++, cur++) {
        if ((cur->flags & 0x400) && !(cur->flags & 0x4000) && cur->mode != 9 && cur->mode != 0xD) {
            d = PSVECSquareDistance(&puni->pos, &cur->pos);
            if (d < bestDist) {
                bestDist = d;
                best = cur;
            }
        }
    }
    {
        PuniData* result;

        if (bestDist < 1e+06f) {
            result = best;
        } else {
            result = NULL;
        }
        return result;
    }
}



void puniModeChangeAttack(void) {
    s32 i;
    PuniData* puni;
    PuniData* best;

    for (puni = &Punidat[1], i = 1; i < PUNI_DATA_MAX; i++, puni++) {
        if (!(puni->flags & 0x400)) {
            continue;
        }
        switch (puni->mode) {
        case 0:
        case 1:
        case 2:
        case 0x1D:
            if (puni->flags & 1) {
                best = puniSearchTekiInlineATK(puni);
            } else {
                best = puniSearchMikataInlineATK(puni);
            }
            if (best != NULL) {
                *(PuniData**)((u8*)puni + 0x1DC) = best;
                puni->mode = 7;
                puni->state = 0;
                puni->field_0x11A = 0;
                puni->flags |= 0x100000;
                PuniDokuritsu(puni);
                if ((void*)puni->effect != NULL) {
                    effDelete((EffectEntry*)puni->effect);
                    puni->effect = 0;
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
        if (!(puni->flags & 0x400)) {
            continue;
        }
        if (puni->field_0x1C0 == 1e+06f) {
            continue;
        }
        switch (puni->mode) {
        case 0:
        case 1:
        case 2:
        case 4:
        case 0x10:
            PuniDokuritsu(puni);
            puni->field_0xE = puni->mode;
            puni->mode = 5;
            puni->state = 0;
            if ((void*)puni->effect != NULL) {
                effDelete((EffectEntry*)puni->effect);
                puni->effect = 0;
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
        if (!(puni->flags & 0x400)) {
            continue;
        }
        pos = puni->pos;
        pos.y = 1.0f + pos.y;
        normal = (Vec){0.0f, -1.0f, 0.0f};
        distance = 2.0f;
        if (hitCheckFilter(NULL, pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, &hit.x, &hit.y, &hit.z, &distance,
                           &hitN.x, &hitN.y, &hitN.z) != NULL) {
            continue;
        }
        PuniDokuritsu(puni);
        puni->vel = (Vec){0.0f, 0.0f, 0.0f};
        puni->mode = 0x11;
        puni->state = 0;
        if ((void*)puni->effect != NULL) {
            effDelete((EffectEntry*)puni->effect);
            puni->effect = 0;
        }
    }
}

void puniFukki(PuniData* puni) {
    s32 i;
    PuniData* p;

    for (p = &Punidat[1], i = 1; i <= 102; i++, p++) {
        if ((p->flags & 0x400) && p->mode == 0x10 &&
            !(32.0 < __fabsf(puni->pos.y - p->pos.y)) &&
            PSVECSquareDistance(&puni->pos, &p->pos) < 900.0f) {
            p->mode = 1;
            puniFukki(p);
            puniDeleteEffect(p);
        }
    }
}

void puniModeALLCheck(void) {
    switch (Punigp->phase) {
    case 4:
        break;
    case 2:
        puniModeChangeAway();
        break;
    case 3:
        puniModeChangeAttack();
        break;
    }
}

void puniModeALLEndCheck(PuniData* punidat) {
    switch (Punigp->phase) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        if (Punigp->prevPhase == 2) {
            Punigp->phase = 0;
        }
        break;
    case 3:
        break;
    }
    Punigp->prevPhase = Punigp->phase;
}

void puniModeNormal(PuniData* punidat) {
    if (punidat->field_0x1A0 > 10.0f) {
        punidat->field_0xE = punidat->mode;
        PuniDokuritsu(punidat);
        punidat->target = Punigp->marioPuni;
        punidat->mode = 13;
        punidat->state = 0;
    } else if (punidat->field_0x1A4 < 150.0f) {
        punidat->mode = 1;
    }
}

// Picks the parent puni with the lowest field_0x11A. Kept as an inline helper so
// the loop pointer colours ahead of the field-load CSE temp (r3 in the original).
static inline PuniData* puniParentpreFind(PuniData* punidat, s16* outMin) {
    s32 i;
    s16 min;
    PuniData* best;
    PuniData* p;

    min = 10000;
    p = Punidat;
    for (i = 0; i < PUNI_DATA_MAX; i++, p++) {
        if (!(p->flags & 0x80000)) {
            continue;
        }
        if ((PuniData*)p->rightPuni != NULL) {
            continue;
        }
        if ((PuniData*)p->leftPuni != NULL && p->field_0x11A >= 2) {
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
    *outMin = min;
    return best;
}

void puniModeParentpre(PuniData* punidat) {
    s16 min;
    PuniData* best;
    f32 mag;
    Vec vec;

    best = puniParentpreFind(punidat, &min);
    if (min != 10000) {
        punidat->parentPuni = (s32)best;
        punidat->field_0x11A = min;
        punidat->field_0x118 = best->field_0x118;
        if ((PuniData*)best->leftPuni == NULL) {
            best->leftPuni = (s32)punidat;
        } else {
            best->rightPuni = (s32)punidat;
        }
    }
    punidat->mode = 2;
    punidat->flags |= 0x180000;
    PSVECSubtract(&((PuniData*)punidat->parentPuni)->pos, &punidat->pos, &vec);
    mag = PSVECMag(&vec);
    punidat->field_0x1A8 = mag;
    punidat->field_0x1B0 = mag;
    punidat->timer = 0;
    punidat->state = 0;
    punidat->animIdx = 1;
}

void puniModeParent(PuniData* punidat) {
    PuniData* parent;
    Vec vec;
    s32 flag;
    s32 a;
    s32 b;
    u16 v;
    u8 c;

    switch (punidat->prevAnimIdx) {
    case 0:
    case 1:
        if (punidat->field_0x1AC < 1.5f) {
            punidat->animIdx = 0;
        } else {
            punidat->animIdx = 1;
        }
        *(u8*)((u8*)punidat + 0x120) = 0;
        break;
    default:
        if (punidat->field_0x1AC < 1.5f) {
            punidat->animIdx = 0;
        } else {
            punidat->animIdx = 1;
        }
        if (punidat->prevAnimIdx == punidat->animIdx) {
            *(u8*)((u8*)punidat + 0x120) = 0;
        } else {
            c = *(u8*)((u8*)punidat + 0x120);
            c++;
            *(u8*)((u8*)punidat + 0x120) = c;
            if (c <= 8) {
                punidat->animIdx = punidat->prevAnimIdx;
            } else {
                *(u8*)((u8*)punidat + 0x120) = 0;
            }
        }
        break;
    }
    if (!(punidat->flags & 0x200)) {
        parent = (PuniData*)punidat->parentPuni;
        flag = 0;
        PSVECSubtract(&parent->pos, &punidat->pos, &vec);
        punidat->field_0x1A8 = PSVECMag(&vec);
        if (!(bero_get_BeroEXEC() & 1)) {
            parent = (PuniData*)punidat->parentPuni;
            if (32.0 < __fabs(punidat->pos.y - parent->pos.y)) {
                flag = 1;
            } else {
                if (punidat->field_0x1A8 > 80.0f) {
                    flag = 1;
                } else {
                    punidat->timer = 0;
                }
            }
        }
        if (0.1f + punidat->field_0x1A8 <= punidat->field_0x1B0) {
            punidat->timer = 0;
            flag = 0;
        }
        if (Punimap->valid != 0) {
            flag = 0;
        }
        if (flag) {
            if (++punidat->timer > 30) {
                punidat->mode = 0x10;
                punidat->state = 0;
                PuniChildHagure(punidat);
                PuniDokuritsu(punidat);
                return;
            }
        }
        punidat->field_0x1B0 = punidat->field_0x1A8;
        if (punidat->field_0x1A0 > 10.0f) {
            punidat->field_0xE = punidat->mode;
            PuniDokuritsu(punidat);
            punidat->target = Punigp->marioPuni;
            punidat->mode = 13;
            punidat->state = 0;
            return;
        }
        if (punidat->field_0x1BC != -1 && punidat->field_0x1C0 < 10000.0f) {
            Punigp->phase = 2;
            return;
        }
        if (punidat->field_0x1B4 != -1) {
            PuniDokuritsu(punidat);
            punidat->mode = 4;
            punidat->state = 0;
            return;
        }
        if (Punimap->hasData != 0 && Punimap->valid != 0) {
            f32 z = punidat->pos.z;
            f32 x = punidat->pos.x;

            switch (Punigp->mapKind) {
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
            v = Punimap->grid[a + b * Punimap->width];
            if ((v & 0xff) == 0) {
                v |= 0xffff;
            }
            if (v != 0xffff && (v & 0xfff) > 2) {
                PuniDokuritsu(punidat);
                punidat->mode = 3;
                punidat->state = 0;
                return;
            }
        }
        parent = (PuniData*)punidat->parentPuni;
        if (punidat->field_0x1A8 > punidat->field_0x190 + parent->field_0x190 + punidat->field_0x194 + punidat->field_0x198) {
            PSVECNormalize(&vec, &vec);
            vec.y = 0.0f;
            PSVECScale(&vec, &vec, 1.0f);
            PSVECAdd(&punidat->vel, &vec, &punidat->vel);
            punidat->field_0x174 = 3.0f;
        }
    }
}
#define PUNI_MODE_SEARCH_GETPOS(xx, yy) puniModeSearch_sub((xx), (yy), &c, &d)
#define PUNI_MAP_AT(xx, zz) \
    (Punimap->grid[(xx) + (zz) * Punimap->width])
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
        puni->field_0xE = puni->mode;
        PuniDokuritsu(puni);
        puni->target = Punigp->marioPuni;
        puni->mode = 13;
        puni->state = 0;
        return;
    }
    if (Punimap->valid == 0 && puni->field_0x1A4 == 1000.0f) { // PuniData 0x1A4
        puni->mode = 16;
        puni->animIdx = 0;
        puni->state = 0;
    }
    if (Punimap->hasData == 0) {
        return;
    }
    if (Punimap->valid == 0) {
        return;
    }

    {
        f32 pz = puni->pos.z;
        f32 px = puni->pos.x;

        switch (Punigp->mapKind) {
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
        puni->mode = 1;
        puni->flags |= 0x100000;
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
    v.y = puni->pos.y;
    v.z = d;
    PSVECSubtract(&v, &puni->pos, &v);
    if (v.x != 0.0f || v.y != 0.0f || v.z != 0.0f) {
        PSVECNormalize(&v, &v);
    }
    v.y = 0.0f;
    PSVECScale(&v, &v, 1.0f);
    PSVECAdd((Vec*)&puni->vel, &v, (Vec*)&puni->vel);
    puni->field_0x174 = 3.0f;
}

void puniModeMass(PuniData* punidat) {
    u8* p = (u8*)punidat;
    u8* target;
    s32 idx = *(s32*)(p + 0x1B4);

#ifdef MUST_MATCH
    // Reloading 0x1B4 here (instead of using `idx`) makes MWCC materialise `*Punigp` with
    // `lis; addi; lwz` into r3 and put the `mulli idx*0x14` in r4, as the target does; with the
    // `idx` variable it folds the load to `lwz r8, Punigp@l(r3)`. The function already reads
    // this field as `*(s32*)(p + 0x1B4)`, hence the raw form. // PuniData 0x1B4
    target = (u8*)Punigp + 0x374 + *(s32*)(p + 0x1B4) * 0x14;
#else
    target = (u8*)Punigp + 0x374 + idx * 0x14;
#endif

    if (idx == -1) {
        if (*(u16*)(p + 0xE) == 0x18)
            punidat->mode = 0x18;
        else
            punidat->mode = 1;
        punidat->animIdx = 0;
        punidat->state = 0;
        return;
    }

    if (*(s32*)(p + 0x1BC) != -1 && *(f32*)(p + 0x1C0) < 10000.0f) {
        Punigp->phase = 2;
        return;
    }

    {
        s32 a = gp->retraceCount & 0xF;
        s32 b = punidat->angle & 0xF;

        if (Punigp->area == 0x12)
            a = b;
        if (a == b && *(f32*)(p + 0x1A0) > 10.0f) {
            PuniDokuritsu(punidat);
            *(s32*)(p + 0x1DC) = Punigp->marioPuni;
            punidat->mode = 0xD;
            punidat->state = 0;
            return;
        }
    }

    if (*(f32*)(p + 0x1B8) > 100.0f) {
        Vec temp;

        if (*(f32*)(p + 0x1B8) < 3600.0f) {
            s32 rnd = puniQrnd2(3);

            if (rnd == 0) {
                PSVECSubtract(&punidat->pos, (Vec*)(target + 4), &temp);
                PSVECNormalize(&temp, &temp);
                temp.y = 0.0f;
                PSVECScale(&temp, &temp, 1.5f);
                PSVECAdd((Vec*)(p + 0x15C), &temp, (Vec*)(p + 0x15C));
                goto mass_after_move;
            }
        }
        PSVECSubtract((Vec*)(target + 4), &punidat->pos, &temp);
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
                punidat->direction = 270.0f;
            else
                punidat->direction = 90.0f;
            punidat->state = 1;
            if (Punigp->area == 0xD)
                punidat->state = 0xA;
            if (Punigp->area == 0x12)
                punidat->state = 0xA;
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
                    punidat->accel = (Vec){0.0f, 0.0f, 2.0f};
                else
                    punidat->accel = (Vec){0.0f, 0.0f, -2.0f};
                punidat->state = 0xB;
            } else if (punidat->pos.x == *(f32*)(p + 0x138) && punidat->pos.y == *(f32*)(p + 0x13C)) {
                (*(s32*)(p + 0x1E0))++;
            }
            break;
        case 11:
            if (punidat->field_0x1E4 == 0x1E) {
                punidat->state = 0xA;
            } else {
                PSVECAdd((Vec*)(p + 0x15C), &punidat->accel, (Vec*)(p + 0x15C));
                punidat->field_0x1E4++;
            }
            break;
        default:
            break;
    }
}

void puniModeAwayjump(PuniData* punidat) {
    s32 elapsed;

    switch (punidat->state) {
    case 0:
        punidat->modeStartTime = gp->renderTime;
        punidat->field_0x1E4 = 0;
        punidat->height = punidat->pos.y;
        punidat->vertVel = -72.0f;
        punidat->jumpVelStart = -punidat->vertVel;  // PuniData 0x1F4
        punidat->state = 1;
        punidat->animIdx = 7;
        punidat->animTime = 0;
        punidat->flags |= 0x1000000;
        if (punidat->flags & 0x80) {
            psndSFXOn_3D("SFX_STG2_TOGETTI_JUMP1", &punidat->pos);
        } else {
            psndSFXOn_3D("SFX_STG2_PUNI_JUMP1", &punidat->pos);
        }
        // fallthrough
    case 1:
        elapsed = ((u32)gp->renderTime - (u32)punidat->modeStartTime) / ((__OSBusClock / 4) / 1000);
        if (elapsed > 100 && punidat->field_0x1E4 == 0) {
            punidat->field_0x1E4 = 1;
            punidat->animIdx = 8;
            punidat->animTime = 0;
        }
        if (elapsed < 200) {
            f32 t = (f32)elapsed / 200.0f;

            punidat->pos.y =
                t * (punidat->vertVel * t + punidat->jumpVelStart) + punidat->height;
        } else {
            punidat->pos.y = punidat->height;
            punidat->mode = 6;
            punidat->animIdx = 2;
            punidat->flags &= ~0x1000000;
            punidat->state = 0;
        }
        break;
    }
}

s32 puniModeAway_sub(PuniData* punidat) {
    Vec vel;
    Vec pos;
    f32 mag;
    HitEntry* p;

    mag = PSVECMag(&punidat->vel);
    pos = punidat->pos;
    pos.y += 1.0f;
    if (0.0f != mag) {
        PSVECNormalize(&punidat->vel, &vel);
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
            punidat->state = 1;
            return 1;
        }
        p = punidat->field_0x214;
        if (p != NULL && (p->attributes & 4)) {
            *(f32*)&punidat->unk168[0] = -punidat->field_0x560.x;  // PuniData 0x168, 0x560
            *(f32*)&punidat->unk168[4] = -punidat->field_0x560.y;  // PuniData 0x16C
            *(f32*)&punidat->unk168[8] = -punidat->field_0x560.z;  // PuniData 0x170, 0x568
            punidat->state = 1;
            return 1;
        }
        punidat->state = 10;
        return 2;
    }
    return 0;
}

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
    target = &Punigp->targets[idx];
    if (idx == -1) {
        punidat->vel = (Vec){0.0f, 0.0f, 0.0f};
        punidat->mode = 1;
        punidat->state = 0;
        punidat->animIdx = 0;
        return;
    }
    if (punidat->field_0x1A0 > 10.0f) {
        punidat->field_0xE = punidat->mode;
        punidat->target = Punigp->marioPuni;
        punidat->mode = 13;
        punidat->state = 0;
        return;
    }
    switch (punidat->state) {
    case 0:
        if (punidat->field_0x1C0 < 90000.0f) {
            PSVECSubtract(&punidat->pos, &target->position, &vec);
            PSVECNormalize(&vec, &vec);
            vec.y = 0.0f;
            PSVECScale(&vec, &vec, 1.4f);
            PSVECAdd(&punidat->vel, &vec, &punidat->vel);
            punidat->field_0x174 = 4.2f;
            puniModeAway_sub(punidat);
        }
        break;
    case 1:
        punidat->field_0x8 |= 4;
        PSVECNormalize((Vec*)((u8*)punidat + 0x168), &vec);  // PuniData 0x168
        PSVECScale(&vec, &punidat->vel, punidat->field_0x190);
        PSVECAdd(&punidat->pos, &punidat->vel, &punidat->pos);
        pos = punidat->pos;
        pos.y = 1.0f + pos.y;
        distance = 2.0f;
        vec = (Vec){0.0f, -1.0f, 0.0f};
        pos.y += 0.2f;
        if (hitCheckFilter(0, pos.x, pos.y, pos.z, vec.x, vec.y, vec.z, &hitX, &hitY, &hitZ, &distance,
                           &hitNX, &hitNY, &hitNZ) == NULL) {
            punidat->vel = (Vec){0.0f, 0.0f, 0.0f};
            punidat->mode = 0x11;
            punidat->animIdx = 0;
            punidat->state = 0;
        } else {
            punidat->state = 0;
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
        if ((cur->flags & 0x400) && !(cur->flags & 0x4000) && cur->mode != 9 && cur->mode != 0xD) {
            d = PSVECSquareDistance(&puni->pos, &cur->pos);
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
        if ((cur->flags & 0x400) && !(cur->flags & 0x4000) && cur->mode != 9 && cur->mode != 0xD) {
            d = PSVECSquareDistance(&puni->pos, &cur->pos);
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

    switch (punidat->state) {
    case 0:
        punidat->animIdx = 1;
        punidat->state = 1;
        // fallthrough
    case 1:
        if (punidat->field_0x1A0 > 10.0f) {
            punidat->field_0xE = punidat->mode;
            PuniDokuritsu(punidat);
            punidat->target = Punigp->marioPuni;
            punidat->mode = 13;
            punidat->state = 0;
            return;
        }
        if ((s32)(gp->retraceCount & 3) == (punidat->angle & 3)) {
            if (punidat->flags & 1) {
#ifdef MUST_MATCH
                *(PuniData**)&punidat->target = puniSearchTekiInline(punidat);  // PuniData 0x1DC
#else
                *(PuniData**)&punidat->target = searchBattleTeki(punidat);  // PuniData 0x1DC
#endif
            } else {
#ifdef MUST_MATCH
                *(PuniData**)&punidat->target = puniSearchMikataInline(punidat);  // PuniData 0x1DC
#else
                *(PuniData**)&punidat->target = searchBattleMikata(punidat);  // PuniData 0x1DC
#endif
            }
        }
        if (*(PuniData**)&punidat->target != NULL) {  // PuniData 0x1DC
            PSVECSubtract(&(*(PuniData**)&punidat->target)->pos, &punidat->pos, &vec);
            dist = PSVECMag(&vec);
            if (dist > *(f32*)&(*(PuniData**)&punidat->target)->unk19C[0]) {  // PuniData 0x19C
                PSVECNormalize(&vec, &vec);
                vec.y = 0.0f;
                PSVECScale(&vec, &vec, 1.0f);
                PSVECAdd(&punidat->vel, &vec, &punidat->vel);
                punidat->field_0x174 = 3.0f;
            } else {
                puniParaAttack(punidat, *(PuniData**)&punidat->target);
            }
        }
        break;
    }
}

void puniModeDamage(PuniData* punidat) {
    Vec vec;
    PuniData* target;
    f32 mag;

    switch (punidat->state) {
    case 0:
        if (Punigp->fightSfxCount0++ < 6) {
            psndSFXOn_3D("SFX_STG2_FIGHT_TOGE1\000\000\000", &punidat->pos);
        }
        punidat->state = 1;
        break;
    case 1:
        if (Punigp->fightSfxCount1++ < 6) {
            psndSFXOn_3D("SFX_STG2_FIGHT_TOGE2\000\000\000", &punidat->pos);
        }
        punidat->state = 2;
        // fallthrough
    case 2:
        target = *(PuniData**)((u8*)punidat + 0x1DC);  // PuniData 0x1DC
        PSVECSubtract(&punidat->pos, &target->pos, &vec);
        mag = PSVECMag(&vec);
        if (mag == 0.0f) {
            vec = (Vec){0.0f, 0.0f, 0.0f};
        } else {
            PSVECNormalize(&vec, &vec);
        }
        vec.y = 0.0f;
        PSVECScale(&vec, &vec, 3.0f);
        punidat->vel = vec;
        punidat->gravity = 5.0f;
        punidat->animIdx = 3;
        punidat->state = 3;
        break;
    case 3:
        punidat->pos.y += punidat->gravity;
        punidat->gravity -= 0.5f;
        if (punidat->pos.y <= 0.001f) {
            punidat->flags &= ~0x2000;
            punidat->pos.y = 0.0f;
            punidat->gravity = 0.0f;
            punidat->field_0x138 = punidat->pos;
            punidat->hp -= punidat->damage;
            if (punidat->hp <= 0) {
                punidat->hp = 0;
                punidat->flags |= 0x4000;
                punidat->field_0x8 |= 0x10;
            }
            if (punidat->hp * 100 / punidat->maxHp <= 30) {
                punidat->mode = 8;
                punidat->animIdx = 2;
                punidat->state = 0;
            } else {
                punidat->mode = 0;
                punidat->animIdx = 0;
                punidat->state = 0;
            }
        }
        break;
    }
}

void puniModePiyori(PuniData* punidat) {
    if (punidat->field_0x1A0 > 10.0f) {
        punidat->field_0xE = punidat->mode;
        PuniDokuritsu(punidat);
        punidat->target = Punigp->marioPuni;
        punidat->flags &= ~0x08000000;
        punidat->field_0x8 &= ~0x2;
        punidat->mode = 13;
        punidat->state = 0;
        return;
    }
    switch (punidat->state) {
    case 0:
        if (punidat->flags & 2) {
            punidat->timer = 600;
            punidat->animIdx = 4;
            punidat->flags |= 0x08100000;
            punidat->field_0x8 |= 0x2;
            punidat->state = 2;
        } else {
            punidat->state = 10;
        }
        break;
    case 2:
        if (punidat->timer-- == 0) {
            punidat->flags &= ~0x08000000;
            punidat->field_0x8 &= ~0x2;
            punidat->state = 10;
        }
        break;
    case 0xA:
        if (*(u16*)&punidat->field_0xE == 10) {  // PuniData 0xE (unsigned in the original)
            if (punidat->hp * 100 / punidat->maxHp <= 30) {
                punidat->mode = 8;
                punidat->animIdx = 2;
                punidat->state = 0;
            } else {
                punidat->mode = 0;
                punidat->animIdx = 0;
                punidat->state = 0;
            }
        } else {
            punidat->mode = *(u16*)&punidat->field_0xE;  // PuniData 0xE (unsigned in the original)
            if (*(u16*)&punidat->field_0xE == 2) {       // PuniData 0xE (unsigned in the original)
                punidat->mode = 1;
            }
            punidat->state = 0;
        }
        break;
    }
}

// The `field_0xE` store must be an unsigned 16-bit access so MWCC schedules `li r3, 0xd`
// before it (the header declares `field_0xE` as s16); taking the field's address as u16* is the
// only field-based way to get the target's order.
void puniModeEscape(PuniData* punidat) {
    s32 i;
    PuniData* p;
    PuniData* found;
    Vec vec;
    f32 best;

    switch (punidat->state) {
    case 0:
        if (punidat->field_0x1A0 > 10.0f) {
#ifdef MUST_MATCH
            *(u16*)&punidat->field_0xE = punidat->mode;
#else
            punidat->field_0xE = punidat->mode;
#endif
            punidat->target = Punigp->marioPuni;
            punidat->mode = 13;
            punidat->state = 0;
            return;
        }
        if (punidat->hp * 100 / punidat->maxHp >= 0x50) {
            punidat->mode = 0;
            punidat->animIdx = 0;
            punidat->state = 0;
            return;
        }
        if ((s32)(gp->retraceCount & 3) == (punidat->angle & 3)) {
            if (punidat->flags & 1) {
#ifdef MUST_MATCH
                *(PuniData**)&punidat->target = puniSearchTekiInline(punidat);  // PuniData 0x1DC
#else
                *(PuniData**)&punidat->target = searchBattleTeki(punidat);  // PuniData 0x1DC
#endif
            } else {
#ifdef MUST_MATCH
                *(PuniData**)&punidat->target = puniSearchMikataInline(punidat);  // PuniData 0x1DC
#else
                *(PuniData**)&punidat->target = searchBattleMikata(punidat);  // PuniData 0x1DC
#endif
            }
        }
        found = *(PuniData**)((u8*)punidat + 0x1DC);  // PuniData 0x1DC
        if (found == NULL) {
            return;
        }
        PSVECSubtract(&found->pos, &punidat->pos, &vec);
        best = PSVECMag(&vec);
        if (best >= 120.0f) {
            *(Vec*)&punidat->vel = (Vec){0.0f, 0.0f, 0.0f};
            punidat->mode = 11;
            punidat->animIdx = 0;
            punidat->state = 0;
            return;
        }
        if (0.0f == best) {
            vec = (Vec){0.0f, 0.0f, 0.0f};
        } else {
            PSVECNormalize(&vec, &vec);
        }
        vec.y = 0.0f;
        PSVECScale(&vec, &vec, 0.8f);
        PSVECSubtract(&punidat->vel, &vec, &punidat->vel);  // PuniData 0x15C
        punidat->field_0x174 = 2.4f;
        break;
    }
}

void puniModeRecover(PuniData* punidat) {
    s32 i;
    PuniData* p;
    PuniData* found;
    Vec vec;
    f32 best;
    s32 region;
    s32 type;

    switch (punidat->state) {
    case 0:
        type = puniPositionCheck(punidat);
        if (type != 0) {
            punidat->field_0x58C = type;
            *(Vec*)&punidat->field_0x590 = punidat->pos;
            puniPositionRecover(punidat);
            *(Vec*)((u8*)punidat + 0x59C) = punidat->pos;  // PuniData 0x59C
        }
        *(Vec*)&punidat->vel = (Vec){0.0f, 0.0f, 0.0f};
        punidat->state = 1;
        break;
    case 1:
        if (punidat->hp * 100 / punidat->maxHp >= 0x50) {
            punidat->mode = 0;
            punidat->animIdx = 0;
            punidat->state = 0;
            return;
        }
        if (punidat->field_0x1A0 > 10.0f) {
#ifdef MUST_MATCH
            // An unsigned 16-bit access makes MWCC schedule `li r3, 0xd` before the store, as in
            // the target (the header declares `field_0xE` as s16).
            *(u16*)&punidat->field_0xE = punidat->mode;
#else
            punidat->field_0xE = punidat->mode;
#endif
            punidat->target = Punigp->marioPuni;
            punidat->mode = 13;
            punidat->state = 0;
            return;
        }
        if ((s32)(gp->retraceCount & 3) == (punidat->angle & 3)) {
            if (punidat->flags & 1) {
#ifdef MUST_MATCH
                *(PuniData**)&punidat->target = puniSearchTekiInline(punidat);  // PuniData 0x1DC
#else
                *(PuniData**)&punidat->target = searchBattleTeki(punidat);  // PuniData 0x1DC
#endif
            } else {
#ifdef MUST_MATCH
                *(PuniData**)&punidat->target = puniSearchMikataInline(punidat);  // PuniData 0x1DC
#else
                *(PuniData**)&punidat->target = searchBattleMikata(punidat);  // PuniData 0x1DC
#endif
            }
        }
        found = *(PuniData**)((u8*)punidat + 0x1DC);  // PuniData 0x1DC
        if (found == NULL) {
            return;
        }
        PSVECSubtract(&found->pos, &punidat->pos, &vec);
        if (PSVECMag(&vec) < 80.0f) {
            punidat->mode = 8;
            punidat->animIdx = 2;
            punidat->state = 0;
        }
        break;
    }
}

void puniModeDead(PuniData* punidat) {
    switch (punidat->state) {
    case 0:
        psndSFXOn_3D("SFX_STG2_FIGHT_TOGE3\000\000\000", &punidat->pos);
        *(Vec*)&punidat->vel = (Vec){0.0f, 0.0f, 0.0f};
        punidat->animIdx = 4;
        punidat->state = 1;
        break;
    case 1:
        break;
    }
}

s32 puniModeWind_sub(PuniData* punidat) {
    Vec vel;
    Vec pos;
    f32 mag;

    mag = PSVECMag(&punidat->vel);
    pos = punidat->pos;
    pos.y += 1.0f;
    if (0.0f != mag) {
        PSVECNormalize(&punidat->vel, &vel);
    } else {
        vel = (Vec){0.0f, 0.0f, 0.0f};
    }
    puniMoveAtari(punidat, pos, vel, 0, mag);
    if (Punigp->phase == 3) {
        if (punidat->field_0x210 != NULL || punidat->field_0x214 != NULL) {
            punidat->state = 10;
            return 2;
        }
    }
    if (punidat->field_0x210 != NULL || punidat->field_0x214 != NULL) {
        if (punidat->field_0x210 != NULL && (punidat->field_0x210->attributes & 4)) {
            *(f32*)((u8*)punidat + 0x168) = -punidat->field_0x560.x;  // PuniData 0x168, 0x560
            *(f32*)((u8*)punidat + 0x16C) = 0.0f;  // PuniData 0x16C
            *(f32*)((u8*)punidat + 0x170) = -punidat->field_0x560.z;  // PuniData 0x170, 0x568
            *(Vec*)((u8*)punidat + 0x150) = punidat->pos;  // PuniData 0x150
            punidat->state = 3;
            return 1;
        }
        if (punidat->field_0x214 != NULL && (punidat->field_0x214->attributes & 4)) {
            *(f32*)((u8*)punidat + 0x168) = -punidat->field_0x560.x;  // PuniData 0x168, 0x560
            *(f32*)((u8*)punidat + 0x16C) = 0.0f;  // PuniData 0x16C
            *(f32*)((u8*)punidat + 0x170) = -punidat->field_0x560.z;  // PuniData 0x170, 0x568
            *(Vec*)((u8*)punidat + 0x150) = punidat->pos;  // PuniData 0x150
            punidat->state = 3;
            return 1;
        }
        punidat->state = 10;
        return 2;
    }
    return 0;
}

void puniModeWind_kaiten(PuniData* punidat) {
    s32 dir;

    if (punidat->direction < 180.0f) {
        dir = punidat->subState;  // PuniData 0x1E0
    } else {
        dir = punidat->subState ^ 1;  // PuniData 0x1E0
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

void puniModeWind(PuniData* punidat) {
    MarioWork* mario;
    Vec vec;
    Vec pos;
    f32 hz, hy, hx, hnz, hny, hnx, dist;
    f32 mag;

    mario = marioGetPtr();
    switch (punidat->state) {
    case 0:
        if (punidat->flags & 0x18) {
            punidat->field_0x188 = 8.0f;
        } else {
            punidat->field_0x148 = 8.0f;
        }
        punidat->flags &= ~0x100000;
        PSVECSubtract(&punidat->pos, &mario->position, &vec);
        if (vec.x < 0.0f) {
            punidat->subState = 0;  // PuniData 0x1E0
        } else {
            punidat->subState = 1;  // PuniData 0x1E0
        }
        vec.x *= 1.5f;
        vec.z *= 0.5f;
        dist = PSVECMag(&vec);
        mag = dist;
        if (mag != 0.0f) {
            PSVECNormalize(&vec, &vec);
        } else {
            vec = (Vec){0.0f, 0.0f, 0.0f};
        }
        vec.y = 0.0f;
        punidat->jumpVelStart =  // PuniData 0x1F4
            6.0f * (punidat->field_0x1A0 / 100.0f) + 2.0f;
        punidat->accel = vec;
        punidat->height = 0.0f;
        punidat->vertVel = 0.0f;
        if (punidat->flags & 1) {
            punidat->gravity = 0.06f;  // PuniData 0x178
        } else {
            punidat->gravity = 0.6f;  // PuniData 0x178
        }
        punidat->animIdx = 5;
        punidat->state = 1;
        // fallthrough
    case 1:
        PSVECScale(&punidat->accel, &vec, punidat->height);
        vec.z = 0.0f;
        *(Vec*)&punidat->vel = vec;
        puniModeWind_kaiten(punidat);
        punidat->height += punidat->vertVel;
        punidat->vertVel += punidat->gravity;  // PuniData 0x1F0, 0x178
        if (puniModeWind_sub(punidat) != 0) {
            break;
        }
        if (!(punidat->height >= punidat->jumpVelStart)) {  // PuniData 0x1F4
            break;
        }
        if (punidat->flags & 1) {
            punidat->gravity = 0.03f;  // PuniData 0x178
        } else {
            punidat->gravity = 0.003f;  // PuniData 0x178
        }
        punidat->vertVel = 0.0f;
        punidat->state = 2;
        break;
    case 2:
        PSVECScale(&punidat->accel, &vec, punidat->height);
        vec.z = 0.0f;
        *(Vec*)&punidat->vel = vec;
        puniModeWind_kaiten(punidat);
        punidat->height -= punidat->vertVel;
        punidat->vertVel += punidat->gravity;  // PuniData 0x1F0, 0x178
        if (puniModeWind_sub(punidat) != 0) {
            break;
        }
        if (punidat->height < 0.0f) {
            punidat->state = 10;
        }
        break;
    case 3:
        punidat->field_0x8 |= 4;
        PSVECNormalize((Vec*)((u8*)punidat + 0x168), &vec);  // PuniData 0x168
        PSVECScale(&vec, &punidat->vel, punidat->field_0x190);  // PuniData 0x15C, 0x190
        punidat->state = 4;
        // fallthrough
    case 4:
        PSVECAdd(&punidat->pos, &punidat->vel, &punidat->pos);  // PuniData 0x15C
        pos = punidat->pos;
        vec = (Vec){0.0f, -1.0f, 0.0f};
        dist = 2.0f;
        pos.y += 0.2f;
        if (hitCheckFilter(NULL, pos.x, pos.y, pos.z, vec.x, vec.y, vec.z, &hx, &hy, &hz, &dist, &hnx, &hny,
                           &hnz) == NULL) {
            *(Vec*)&punidat->vel = (Vec){0.0f, 0.0f, 0.0f};
            punidat->mode = 0x11;
            punidat->animIdx = 0;
            punidat->field_0x188 = 0.0f;
            punidat->field_0x184 = 0.0f;
            punidat->field_0x148 = 0.0f;
            punidat->state = 0;
        } else {
            punidat->state = 10;
        }
        break;
    case 10:
        puniModeWind_kaiten(punidat);
        if (!(punidat->field_0x1A0 > 0.0f)) {
            if (Punigp->phase == 3) {
                punidat->mode = 0xa;
                punidat->field_0x188 = 0.0f;
                punidat->field_0x184 = 0.0f;
                punidat->field_0x148 = 0.0f;
                *(Vec*)&punidat->vel = (Vec){0.0f, 0.0f, 0.0f};
                punidat->state = 0;
            } else {
                punidat->field_0x188 = 0.0f;
                punidat->field_0x184 = 0.0f;
                punidat->field_0x148 = 0.0f;
                *(Vec*)&punidat->vel = (Vec){0.0f, 0.0f, 0.0f};
                punidat->mode = 1;
                punidat->state = 0;
            }
        }
        break;
    }
}

static inline f32 puniRandf2(f32 a, f32 b) {
    return puniRandf(a, b) - 1.0f;
}


void puniModeFall(PuniData* punidat) {
    PuniFallTarget* targets = (PuniFallTarget*)Punigp->targets;
    PuniFallActor* actor210;
    PuniFallActor* actor214;

    switch (punidat->state) {
        case 0:
            punidat->flags &= ~0x100000;
            *(Vec*)&punidat->vel = (Vec){0.0f, -0.8f, 0.0f};
            punidat->state = 1;
            break;
        case 1: {
            Vec pos;
            Vec vel;
            s32 found;
            s32 i;
            s32 base;
            s16 kind;

            punidat->vel.x = 0.0f;
            punidat->vel.y = punidat->vel.y - 0.45f;
            vel = (Vec){0.0f, -1.0f, 0.0f};
            {
                f32 mag = PSVECMag((Vec*)&punidat->vel);

                pos = punidat->pos;
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
                    punidat->subState = 0x12;
                    punidat->animIdx = 0;
                    punidat->state = 2;
                    break;
                }
                if (actor214 != NULL && (actor214->flags & 0x100)) {
                    punidat->subState = 0x12;
                    punidat->animIdx = 0;
                    punidat->state = 2;
                    break;
                }
                if (actor210 != NULL && (actor210->flags & 0x200)) {
                    punidat->subState = 0x16;
                    punidat->animIdx = 3;
                    punidat->state = 2;
                    break;
                }
                if (actor214 != NULL && (actor214->flags & 0x200)) {
                    punidat->subState = 0x16;
                    punidat->animIdx = 3;
                    punidat->state = 2;
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
                            punidat->accel = targets->vec;
                            found = 1;
                            break;
                        }
#ifdef MUST_MATCH
                        if (actor214 == targets->unk10) {
#else
                        if (targets->unk10 == actor214) {
#endif
                            punidat->accel = targets->vec;
                            found = 1;
                            break;
                        }
                    }
                }
                if (found) {
                    punidat->subState = 0x17;
                } else {
                    punidat->subState = 0xF;
                }

                if (Punigp->area == 0x12 && punidat->pos.y < -300.0f) {
                    kind = punidat->angle;
                    if (kind < 0x67) {
                        u32 v;

                        base = kind / 8;
                        v = swByteGet(base + 0x28C);
                        v = (u8)(v & (0xFF ^ (1 << (kind % 8))));
                        swByteSet(base + 0x28C, v);
                    }
                    punidat->flags &= ~0x800000;
                }
                *(Vec*)&punidat->vel = (Vec){0.0f, 0.0f, 0.0f};
                punidat->state = 2;
                break;
            }

            if (punidat->pos.y <= -1000.0f) {
                *(Vec*)((u8*)punidat + 0x590) = punidat->pos;
                puniPositionRecover(punidat);
                *(Vec*)((u8*)punidat + 0x59C) = punidat->pos;
                punidat->subState = 0x10;
                *(Vec*)&punidat->vel = (Vec){0.0f, 0.0f, 0.0f};
                punidat->state = 2;
            }
            break;
        }
        case 2:
            punidat->mode = punidat->subState;
            *(Vec*)&punidat->vel = (Vec){0.0f, 0.0f, 0.0f};
            punidat->state = 0;
            break;
    }
}

void puniModePreupdraft(PuniData* punidat) {
    switch (punidat->state) {
        case 0: {
            Vec pos = punidat->pos;
            pos.y += 15.0f;
            pos.z += 3.0f;
            *(EffectEntry**)((u8*)punidat + 0x218) =
                effPuniBalloonEntry(0, pos.x, pos.y, pos.z, 1.5f);
            punidat->timer = rand() % 201 + 0x1E;
            punidat->state = 1;
        }
        case 1:
            if ((punidat->timer)-- == 0) {
                u32* p = *(u32**)((u8*)punidat + 0x0);
                *p |= 4;
                punidat->mode = 0x13;
                punidat->state = 0;
            }
            break;
    }
}

void puniModeUpdraft(PuniData* punidat) {
    MarioWork* mario = marioGetPtr();
    Vec dir;
    Vec pos;
    Vec vel;

    if (punidat->state == 0) {
        *(Vec*)&punidat->vel = (Vec){0.0f, 0.94f, 0.0f};
    } else {
        *(Vec*)&punidat->vel = (Vec){0.0f, 0.66f, 0.0f};
    }

    if ((gp->retraceCount & 0x1F) == 0) {
        psndSFXOn_3D("SFX_STG2_SOAPBALL_MOVE1", &punidat->pos);
    }

    {
        s32 s = (gp->retraceCount + punidat->angle) & 0xF;
        s32 x = s - 3;
        if (s > 7) {
            x = 4 - (s & 7);
        }
        punidat->field_0x144 = 0.5f * (f32)x;
    }

    switch (punidat->state) {
        case 0:
            if (punidat->field_0x1A0 > 10.0f) {
                punidat->target = Punigp->marioPuni;
                PSVECSubtract(&punidat->pos, &mario->position, &dir);
                if (PSVECMag(&dir) != 0.0f) {
                    PSVECNormalize(&dir, &dir);
                }
                dir.y = 0.0f;
                dir.z = 0.0f;
                punidat->jumpVelStart =
                    1.2f + punidat->field_0x1A0 / 100.0f;
                punidat->accel = dir;
                punidat->height = 0.0f;
                punidat->vertVel = 0.0f;
                punidat->state = 1;
            }
            break;
        case 1:
            PSVECScale(&punidat->accel, &dir, punidat->height);
            PSVECAdd((Vec*)&punidat->vel, &dir, (Vec*)&punidat->vel);
#ifdef MUST_MATCH
            {
                f32 inc = 0.06f;
                punidat->height = punidat->height + punidat->vertVel;
                punidat->vertVel = punidat->vertVel + inc;
            }
#else
            punidat->height = punidat->height + punidat->vertVel;
            punidat->vertVel = punidat->vertVel + 0.06f;
#endif
            if (punidat->height >= punidat->jumpVelStart) {
                punidat->height = punidat->jumpVelStart;
            }
            break;
        case 2:
            PSVECScale(&punidat->accel, &dir, punidat->height);
            PSVECAdd((Vec*)&punidat->vel, &dir, (Vec*)&punidat->vel);
#ifdef MUST_MATCH
            {
                f32 inc = 0.03f;
                punidat->height = punidat->height - punidat->vertVel;
                punidat->vertVel = punidat->vertVel + inc;
            }
#else
            punidat->height = punidat->height - punidat->vertVel;
            punidat->vertVel = punidat->vertVel + 0.03f;
#endif
            if (punidat->height < 0.0f) {
                punidat->state = 0;
            }
            break;
    }

    {
        f32 mag = PSVECMag((Vec*)&punidat->vel);
        if (0.0f != mag) {
            PSVECNormalize((Vec*)&punidat->vel, &dir);
        } else {
            dir = (Vec){0.0f, 0.0f, 0.0f};
        }
        pos = punidat->pos;
        vel = *(Vec*)&punidat->vel;
        puniMoveAtari(punidat, pos, dir, 1, mag);
    }

    {
        Vec v = punidat->pos;
        v.x += punidat->field_0x144;
        v.y += 15.0f;
        v.z += 3.0f;
        ((f32*)((EffectEntry*)punidat->effect)->userdata)[1] = v.x;
        ((f32*)((EffectEntry*)punidat->effect)->userdata)[2] = v.y;
        ((f32*)((EffectEntry*)punidat->effect)->userdata)[3] = v.z;
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
                punidat->mode = 0x14;
                punidat->state = 0;
                punidat->field_0x144 = 0.0f;
            } else {
                punidat->pos = pos;
                *(Vec*)&punidat->vel = vel;
                PSVECAdd(&punidat->pos, (Vec*)&punidat->vel,
                         &punidat->pos);
            }
        }
        if (punidat->pos.y >= 0.0f) {
            punidat->mode = 0x14;
            punidat->state = 0;
            punidat->field_0x144 = 0.0f;
        }
    }
}

void puniModeWonder(PuniData* punidat) {
    switch (punidat->state) {
        case 0: {
            Vec pos;
            psndSFXOn_3D("SFX_STG2_SOAPBALL_BREAK1\000\000\000", &punidat->pos);
            if (*(u32*)((u8*)punidat + 0x218) != 0) {
                effDelete((EffectEntry*)*(u32*)((u8*)punidat + 0x218));
                punidat->effect = 0;
            }
            pos = punidat->pos;
            pos.y += 15.0f;
            pos.z += 3.0f;
            *(EffectEntry**)((u8*)punidat + 0x218) =
                effPuniBalloonEntry(1, pos.x, pos.y, pos.z, 1.5f);
            punidat->timer = 0x14;
            punidat->animIdx = 9;
            punidat->animTime = 0;
            punidat->flags |= 0x01000000;
            punidat->state = 1;
        }
        case 1:
            if ((punidat->timer)-- == 0) {
                u32* p = *(u32**)((u8*)punidat + 0x0);
                *p &= ~0x4;
                punidat->mode = 0x15;
                punidat->animIdx = 1;
                punidat->flags &= ~0x01000000;
                punidat->state = 0;
            }
            break;
    }
}

void puniModeFunbari(PuniData* punidat) {
    switch (punidat->state) {
        case 0: {
            PuniFallTarget* entry = (PuniFallTarget*)Punigp->targets;
            s32 i;
            for (i = 0; i < 10; i++, entry++) {
                if ((entry->unk0 & 1) && (entry->unk0 & 2)) {
                    break;
                }
            }
            if (i >= 10) {
                punidat->state = 10;
                break;
            }
            if (punidat->pos.x > entry->vec.x) {
                punidat->jumpVelStart = -1.0f;
                punidat->direction = 270.0f;
            } else {
                punidat->jumpVelStart = 1.0f;
                punidat->direction = 90.0f;
            }
            punidat->vel.x = 0.5f;
            punidat->timer = 0x44;
            punidat->height = 1.0f;
            {
                u32* p = *(u32**)((u8*)punidat + 0x0);
                *p |= 4;
            }
            punidat->state = 1;
            break;
        }
        case 1:
        case 2: {
            f32 s = (f32)sin(3.1415927f * punidat->height / 180.0f);
            if (punidat->height <= 180.0f) {
                punidat->vertVel = 0.5f * s;
            } else {
                punidat->vertVel = 0.45f * s;
            }
            punidat->height += 7.0f;
            if (punidat->height >= 360.0f) {
                punidat->height = 360.0f;
            }
            punidat->pos.y = punidat->pos.y + punidat->vertVel;
            punidat->pos.x = punidat->vel.x * punidat->jumpVelStart +
                                     punidat->pos.x;
            if ((punidat->timer)-- == 0) {
                punidat->state = 10;
            }
            break;
        }
        case 10: {
            u32* p;
            *(Vec*)&punidat->vel = (Vec){0.0f, 0.0f, 0.0f};
            punidat->mode = 0x11;
            punidat->animIdx = 0;
            p = *(u32**)((u8*)punidat + 0x0);
            *p &= ~0x4;
            punidat->state = 0;
            break;
        }
    }
}

void puniModeWater(PuniData* punidat) {
    switch (punidat->state) {
        case 0:
            punidat->flags |= 0x800;
            punidat->timer = 0x3C;
            punidat->state = 1;
        case 1:
            if ((punidat->timer)-- == 0) {
                punidat->flags &= ~0x800;
                punidat->state = 2;
            }
            break;
        case 2: {
            Vec dir;
            f32 t;
            punidat->modeStartTime = gp->renderTime;
            punidat->accel = punidat->pos;
            punidat->height = -80.0f;
            t = *(f32*)((u8*)punidat + 0x154) - punidat->accel.y;  // PuniData 0x154
            punidat->vertVel = t - punidat->height;
            PSVECNormalize((Vec*)((u8*)punidat + 0x168), &dir);
            PSVECScale(&dir, &dir, -20.0f);
            {
                f32 tx = dir.x + punidat->pos.x;
                f32 tz = dir.z + punidat->pos.z;
                *(f32*)((u8*)punidat + 0x150) = tx;
                *(f32*)((u8*)punidat + 0x158) = tz;
            }
            punidat->state = 3;
        }
        case 3: {
            u32 divisor = (__OSBusClock >> 2) / 1000;
            u32 step = (u32)((u32)gp->renderTime - (u32)punidat->modeStartTime) / divisor;
            if ((s32)step < 0x12C) {
                f32 t = (f32)(s32)step / 300.0f;
                f32 y;
                punidat->pos.x =
                    intplGetValue(0, step, 0x12C, punidat->accel.x, *(f32*)((u8*)punidat + 0x150));
                punidat->pos.z =
                    intplGetValue(0, step, 0x12C, punidat->accel.z, *(f32*)((u8*)punidat + 0x158));
                y = punidat->height * t + punidat->vertVel;
                punidat->pos.y = t * y + punidat->accel.y;
                *(Vec*)&punidat->field_0x138 = punidat->pos;
            } else {
                punidat->pos = *(Vec*)((u8*)punidat + 0x150);
                *(Vec*)&punidat->field_0x138 = punidat->pos;
                punidat->state = 4;
            }
            break;
        }
        case 4:
            punidat->mode = 0x10;
            punidat->animIdx = 0;
            punidat->state = 0;
            break;
    }
}

// The flag-test bounds are hoisted into locals so the compiler loads both Punigp fields before
// the first compare, as the target does; the case-0 setup statement order is the target's.
void puniModePrehagure(PuniData* puni) {
    u8* p = (u8*)puni;
    s32 flag;

    switch (((PuniData*)p)->state) {
        case 0:
            flag = puniPositionCheck(puni);
            if (flag != 0) {
                ((PuniData*)p)->field_0x58C = flag;
                *(Vec*)(p + 0x590) = ((PuniData*)p)->pos;
                puniPositionRecover(puni);
                *(Vec*)(p + 0x59C) = ((PuniData*)p)->pos;
            }
            ((PuniData*)p)->state = 1;
            break;
        case 1:
            ((PuniData*)p)->mode = 0x10;
            ((PuniData*)p)->state = 0;
            break;
    }
}

void puniModeHagure(PuniData* puni) {
    u8* p = (u8*)puni;
    u8* e;

    switch (((PuniData*)p)->state) {
        case 0:
            ((PuniData*)p)->effect = (s32)effFukidashiEntry(4, 0, (s32)(p + 0x220), -1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            ((PuniData*)p)->animIdx = 6;
            *(u32*)(p + 0x4) |= 0x100000;
            ((PuniData*)p)->state = 1;
            /* fallthrough */
        case 1:
            *(s32*)(p + 0x1E0) = rand() % 21 + 30;
            ((PuniData*)p)->field_0x1E4 = rand() % 6 + 5;
            ((PuniData*)p)->timer = *(s32*)(p + 0x1E0);
            *(s32*)(p + 0x1E8) = ((PuniData*)p)->field_0x1E4;
            ((PuniData*)p)->state = 10;
            *(f32*)(p + 0x2AC) = 0.0f;
            *(f32*)(p + 0x2B0) = -1000.0f;
            *(f32*)(p + 0x2B4) = 0.0f;
            /* fallthrough */
        case 10:
            if ((((PuniData*)p)->timer)-- != 0) {
                break;
            }
            if (*(s32*)(p + 0x1E8) & 1) {
                ((PuniData*)p)->direction = 90.0f;
            } else {
                ((PuniData*)p)->direction = 270.0f;
            }
            if ((*(s32*)(p + 0x1E8))-- == 0) {
                ((PuniData*)p)->state = 20;
                break;
            }
            ((PuniData*)p)->timer = *(s32*)(p + 0x1E0);
            break;
        case 20:
            ((PuniData*)p)->timer = 30;
            ((PuniData*)p)->state = 21;
            /* fallthrough */
        case 21:
            if ((((PuniData*)p)->timer)-- == 0) {
                ((PuniData*)p)->state = 30;
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
            ((PuniData*)p)->timer = rand() % 41 + 100;
            ((PuniData*)p)->state = 31;
            /* fallthrough */
        case 31:
            if ((((PuniData*)p)->timer)-- == 0) {
                ((PuniData*)p)->state = 1;
            }
            break;
    }

    if (((PuniData*)p)->field_0x1BC != -1 && ((PuniData*)p)->field_0x1C0 < 10000.0f) {
        Punigp->phase = 2;
        ((PuniData*)p)->state = 0;
        if (*(u32*)(p + 0x218) != 0) {
            effDelete((EffectEntry*)((PuniData*)p)->effect);
            ((PuniData*)p)->effect = 0;
        }
    } else if (((PuniData*)p)->field_0x1B4 != -1) {
        PuniDokuritsu(puni);
        ((PuniData*)p)->mode = 4;
        ((PuniData*)p)->state = 0;
        if (*(u32*)(p + 0x218) != 0) {
            effDelete((EffectEntry*)((PuniData*)p)->effect);
            ((PuniData*)p)->effect = 0;
        }
    } else if (((PuniData*)p)->field_0x1A0 > 10.0f) {
        PuniDokuritsu(puni);
        ((PuniData*)p)->target = Punigp->marioPuni;
        ((PuniData*)p)->mode = 0xD;
        ((PuniData*)p)->state = 0;
        if (*(u32*)(p + 0x218) != 0) {
            effDelete((EffectEntry*)((PuniData*)p)->effect);
            ((PuniData*)p)->effect = 0;
        }
    } else if (((PuniData*)p)->field_0x1A4 < 150.0f) {
        ((PuniData*)p)->mode = 1;
        puniFukki(puni);
        if (*(u32*)(p + 0x218) != 0) {
            effDelete((EffectEntry*)((PuniData*)p)->effect);
            ((PuniData*)p)->effect = 0;
        }
    }
}

void puniModeDodai(PuniData* puni) {
    u8* p = (u8*)puni;
    Vec diff;

    switch (((PuniData*)p)->state) {
        case 0:
            PSVECSubtract(&((PuniData*)p)->pos, &((PuniData*)p)->accel, &diff);
            if (PSVECMag(&diff) < 10.0f) {
                ((PuniData*)p)->vel = (Vec){ 0.0f, 0.0f, 0.0f };
                ((PuniData*)p)->mode = 0x10;
                ((PuniData*)p)->state = 0;
                break;
            }
            PSVECNormalize(&diff, &diff);
            diff.y = 0.0f;
            PSVECScale(&diff, &diff, 1.0f);
            PSVECSubtract(&((PuniData*)p)->vel, &diff, &((PuniData*)p)->vel);
            ((PuniData*)p)->field_0x174 = 3.0f;
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

    if (((PuniData*)p)->state == 0) {
        *(u32*)(p + 0x4) |= 0x100000;
        if (*(u32*)(p + 0x4) & 0x20) {
            npc = npcNameToPtr("ガイド");
            *(u32*)(p + 0x4) |= 0x800;
            npc->position.x = *(f32*)(p + 0x12C);
            npc->position.y = *(f32*)(p + 0x130);
            npc->position.z = *(f32*)(p + 0x134);
            npc->direction = ((PuniData*)p)->direction;
            npc->flags |= 0x20;
        }
        if (*(u32*)(p + 0x4) & 0x40) {
            npc = npcNameToPtr("ガイド妹");
            *(u32*)(p + 0x4) |= 0x800;
            npc->position.x = *(f32*)(p + 0x12C);
            npc->position.y = *(f32*)(p + 0x130);
            npc->position.z = *(f32*)(p + 0x134);
            npc->direction = ((PuniData*)p)->direction;
            npc->flags |= 0x20;
        }
        if (*(u32*)(p + 0x4) & 0x100) {
            npc = npcNameToPtr("ライバル");
            *(u32*)(p + 0x4) |= 0x800;
            npc->position.x = *(f32*)(p + 0x12C);
            npc->position.y = *(f32*)(p + 0x130);
            npc->position.z = *(f32*)(p + 0x134);
            npc->direction = ((PuniData*)p)->direction;
            npc->flags |= 0x20;
        }
        ((PuniData*)p)->vel = (Vec){ 0.0f, 0.0f, 0.0f };
        *(u16*)(p + 0xE) = ((PuniData*)p)->mode;
    }
    if (((PuniData*)p)->field_0x1A0 > 10.0f) {
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
        ((PuniData*)p)->target = Punigp->marioPuni;
        *(u16*)(p + 0xE) = ((PuniData*)p)->mode;
        ((PuniData*)p)->mode = 13;
        ((PuniData*)p)->state = 0;
        return;
    }
    if (((PuniData*)p)->field_0x1B4 != -1) {
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
        *(u16*)(p + 0xE) = ((PuniData*)p)->mode;
        ((PuniData*)p)->mode = 4;
        ((PuniData*)p)->state = 0;
    }
}

void puniModeEnterSubInit(PuniData* puni) {
    u8* p = (u8*)puni;

    if ((Punigp->flags & 3) == 0) {
        return;
    }
    *(s32*)(p + 0x1E0) = 0;
    ((PuniData*)p)->scale = 0.0f;
    *(u32*)(*(u8**)(p + 0x0)) |= 4;
}

void puniModeEnterSub(PuniData* puni) {
    u8* p = (u8*)puni;

    if ((Punigp->flags & 3) == 0) {
        return;
    }
    *(s32*)(p + 0x1E0) += 1;
    if (*(s32*)(p + 0x1E0) < 10) {
        return;
    }
    ((PuniData*)p)->scale += 0.1f;
    if (((PuniData*)p)->scale >= 1.0f) {
        ((PuniData*)p)->scale = 1.0f;
    }
}

// The original calls the real `qrnd`, not the identical-looking `puniQrnd` inline: the inlined body
// is the same, but the callee name changes MWCC's virtual-register numbering and with it the qrnd
// expansion's register colours (Punigp base/value r5/r6 instead of r4/r5).
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
            if ((Punigp->flags & 8) && !(Punigp->flags & 0x10)) {
                *(s32*)(p + 0x128) = 700;
            }
            // qrnd(500), inlined
            *(s32*)(p + 0x128) += qrnd(500);
            if (Punigp->flags & 3) {
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
            if (Punigp->flags & 3) {
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
            for (i = 0; i < PUNI_DATA_MAX; i++) {
                if ((data->flags & 0x80000) && (u32)data->rightPuni == 0 &&
                    ((u32)data->leftPuni == 0 || data->field_0x11A < 2) && (u8*)data != p) {
#ifdef MUST_MATCH
                    s16 val = data->field_0x11A;
                    if ((s16)best > val) {
                        parent = (u8*)data;
                        best = val + 1;
                    }
#else
                    if ((s16)best > data->field_0x11A) {
                        parent = (u8*)data;
                        best = data->field_0x11A + 1;
                    }
#endif
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

    if ((Punigp->flags & 3) == 0) {
        return;
    }
    *(s32*)(p + 0x1E0) = 0;
    *(u32*)(*(u8**)(p + 0x0)) |= 4;
}

void puniModeExitSub(PuniData* puni) {
    u8* p = (u8*)puni;

    if ((Punigp->flags & 3) == 0) {
        return;
    }
    *(s32*)(p + 0x1E0) += 1;
    if (*(s32*)(p + 0x1E0) < 10) {
        return;
    }
    ((PuniData*)p)->scale = ((PuniData*)p)->scale - 0.3f;
    if (((PuniData*)p)->scale <= 0.0f) {
        ((PuniData*)p)->scale = 0.0f;
    }
}

void puniModeExit(PuniData* punidat) {
    u8* p = (u8*)punidat;
    Vec v;

    switch (*(u16*)(p + 0x114)) {
        case 0:
            if (PSVECSquareDistance(&Punigp->arrivePos, (Vec*)(p + 0x12C)) >
                Punigp->arriveDistSq) {
                PSVECSubtract(&Punigp->arrivePos, (Vec*)(p + 0x12C), &v);
                PSVECNormalize(&v, &v);
                v.y = 0.0f;
                PSVECScale(&v, &v, 1.5f);
                PSVECAdd((Vec*)(p + 0x15C), &v, (Vec*)(p + 0x15C));
            } else {
                *(Vec*)(p + 0x15C) = (Vec){0.0f, 0.0f, 0.0f};
                *(u32*)(p + 0x4) &= 0xFFEFEFFF;
                if (Punigp->flags & 3) {
                    *(s32*)(p + 0x1E0) = 0;
                    *(u32*)(*(u32**)(p + 0x0)) |= 4;
                }
                *(u16*)(p + 0x114) = 1;
            }
            break;
        case 1:
            if (Punigp->flags & 3) {
                (*(s32*)(p + 0x1E0))++;
                if (*(s32*)(p + 0x1E0) >= 10) {
                    *(f32*)(p + 0x18C) -= 0.3f;
                    if (*(f32*)(p + 0x18C) <= 0.0f) {
                        *(f32*)(p + 0x18C) = 0.0f;
                    }
                }
            }
            if (PSVECSquareDistance(&Punigp->arrivePos2, (Vec*)(p + 0x12C)) >
                Punigp->arriveDistSq2) {
                PSVECSubtract(&Punigp->arrivePos2, (Vec*)(p + 0x12C), &v);
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
            if (PSVECSquareDistance(&Punigp->arrivePos, (Vec*)(p + 0x12C)) >
                Punigp->arriveDistSq) {
                PSVECSubtract(&Punigp->arrivePos, (Vec*)(p + 0x12C), &v);
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
            if (PSVECSquareDistance(&Punigp->arrivePos2, (Vec*)(p + 0x12C)) >
                Punigp->arriveDistSq2) {
                PSVECSubtract(&Punigp->arrivePos2, (Vec*)(p + 0x12C), &v);
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

    ((PuniData*)p)->state = 0;
    *(u32*)(p + 0x4) |= 0x800;
    *(u32*)(*(u32**)(p + 0x0)) |= 4;
    ((PuniData*)p)->vel = (Vec){0.0f, 0.0f, 0.0f};
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

// TODO: type
static u32 shadow_dl[8] ATTRIBUTE_ALIGN(32) = {
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

    Punigp->randSeedA = (s32)gp->renderTime + rnd + 0x3A3B01F9;
    Punigp->randSeedB = 0x21708331;
    pd = Punidat;
    *(PuniData**)&Punigp->marioPuni = pd;
    pd->flags |= 0x180404;
    pd->pos = mario->position;
    *(Vec*)&pd->field_0x138 = pd->pos;
    pd->field_0x190 = 10.0f;
    pd->mode = 30;
    pd->angle = 0;
    pd->field_0x118 = 0;
    puni = (u8*)&Punidat[1];
    for (i = 1; i < PUNI_DATA_MAX; i++, puni += sizeof(PuniData)) {
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
        ((PuniData*)puni)->direction = 0.0f;
        ((PuniData*)puni)->field_0x188 = 0.0f;
        ((PuniData*)puni)->scale = 1.0f;
        *(f32*)(puni + 0x19C) = 10.0f;
        ((PuniData*)puni)->flags = 0;
        ((PuniData*)puni)->angle = i;
        ((PuniData*)puni)->field_0x118 = -1;
        ((PuniData*)puni)->field_0x174 = 3.0f;
    }
    Punigp->showPuniCounter = 1;
    Punigp->puniCounterAlpha = 0xFF;
    Punigp->unk448 = 0;
    Punimap = _mapAlloc(0x24);
    memset(Punimap, 0, 0x24);
}

// case 7 keeps the `if (field_0x1DC != NULL)` block inside the `else`, so the field_0x1A0 branch skips both
// the search and the hit block on its way to the epilogue.
void unk_mri_00012d58(PuniData* self) {
    switch (self->mode) {
    case 0:
        self->flags &= ~0x8000;
        puniModeNormal(self);
        break;
    case 1: {
        self->flags &= ~0x8000;
        searchParent(self);
        self->mode = 2;
        self->flags |= 0x180000;
        {
            Vec diff;
            f32 mag;
            PSVECSubtract(&((PuniData*)self->parentPuni)->pos, &self->pos, &diff);
            mag = PSVECMag(&diff);
            self->field_0x1A8 = mag;
            self->field_0x1B0 = mag;
        }
        self->timer = 0;
        self->state = 0;
        self->animIdx = 1;
        break;
    }
    case 2:
        self->flags &= ~0x8000;
        puniModeParent(self);
        break;
    case 3:
        self->flags &= ~0x8000;
        puniModeSearch(self);
        break;
    case 4:
        self->flags |= 0x8000;
        puniModeMass(self);
        break;
    case 5:
        self->flags |= 0x8000;
        puniModeAwayjump(self);
        break;
    case 6:
        self->flags |= 0x8000;
        puniModeAway(self);
        break;
    case 7:
        self->flags &= ~0x8000;
        switch (self->state) {
        case 0:
            self->animIdx = 1;
            self->state = 1;
            /* fallthrough */
        case 1:
            if (self->field_0x1A0 > 10.0f) {
                self->field_0xE = self->mode;
                PuniDokuritsu(self);
                *(PuniData**)((u8*)self + 0x1DC) = (PuniData*)(s32)Punigp->marioPuni;  // PuniData 0x1DC
                self->mode = 0xD;
                self->state = 0;
            } else {
                if (((s32)gp->retraceCount & 3) == (self->angle & 3)) {
                    if (self->flags & 1) {
                        *(PuniData**)((u8*)self + 0x1DC) = puniSearchTekiNamed(self);  // PuniData 0x1DC
                    } else {
                        *(PuniData**)((u8*)self + 0x1DC) = puniSearchMikataNamed(self);  // PuniData 0x1DC
                    }
                }
                if (*(PuniData**)((u8*)self + 0x1DC) != NULL) {  // PuniData 0x1DC
                    Vec diff;
                    PSVECSubtract(&(*(PuniData**)((u8*)self + 0x1DC))->pos, &self->pos, &diff);  // PuniData 0x1DC
                    if (PSVECMag(&diff) > *(f32*)((u8*)(*(PuniData**)((u8*)self + 0x1DC)) + 0x19C)) {  // PuniData 0x1DC
                        PSVECNormalize(&diff, &diff);
                        diff.y = 0.0f;
                        PSVECScale(&diff, &diff, 1.0f);
                        PSVECAdd((Vec*)&self->vel, &diff, (Vec*)&self->vel);
                        self->field_0x174 = 3.0f;
                    } else {
                        puniParaAttack(self, *(PuniData**)((u8*)self + 0x1DC));  // PuniData 0x1DC
                    }
                }
            }
            break;
        default:
            break;
        }
        break;
    case 9:
        self->flags &= ~0x8000;
        puniModeDamage(self);
        break;
    case 10:
        self->flags &= ~0x8000;
        puniModePiyori(self);
        break;
    case 8:
        self->flags &= ~0x8000;
        puniModeEscape(self);
        break;
    case 11:
        self->flags &= ~0x8000;
        puniModeRecover(self);
        break;
    case 12:
        self->flags &= ~0x8000;
        puniModeDead(self);
        break;
    case 13:
        self->flags &= ~0x8000;
        puniModeWind(self);
        break;
    case 15:
        self->flags |= 0x8000;
        puniModePrehagure(self);
        break;
    case 16:
        self->flags |= 0x8000;
        puniModeHagure(self);
        break;
    case 18:
        self->flags |= 0x8000;
        puniModePreupdraft(self);
        break;
    case 19:
        self->flags |= 0x8000;
        puniModeUpdraft(self);
        break;
    case 20:
        self->flags |= 0x8000;
        puniModeWonder(self);
        break;
    case 21:
        self->flags |= 0x8000;
        puniModeFunbari(self);
        break;
    case 22:
        self->flags |= 0x8000;
        puniModeWater(self);
        break;
    case 17:
        self->flags |= 0x8000;
        puniModeFall(self);
        break;
    case 23:
        self->flags |= 0x8000;
        puniModeDodai(self);
        break;
    case 24:
        self->flags &= ~0x8000;
        puniModeTrap(self);
        break;
    case 25:
        self->flags &= ~0x8000;
        puniModeEnter(self);
        break;
    case 26:
        self->flags &= ~0x8000;
        puniModeExit(self);
        break;
    case 28:
        self->flags &= ~0x8000;
        puniModeDokan(self);
        break;
    case 27:
        self->flags &= ~0x8000;
        puniModePoint(self);
        break;
    case 29:
        self->flags &= ~0x8000;
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

    switch (puni->mode) {
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

    mag = PSVECMag((Vec*)&puni->vel);
    if (!mag) goto endA;
    if (mag > 1.0f) {
        if (((Vec*)&puni->vel)->x < 0.0f) {
            puni->direction = 270.0f;
        } else {
            puni->direction = 90.0f;
        }
    }
    if (mag > *(f32*)(puni->unk168 + 0xC)) {
        PSVECScale((Vec*)&puni->vel, (Vec*)&puni->vel,
                   *(f32*)(puni->unk168 + 0xC) / mag);
        mag = *(f32*)(puni->unk168 + 0xC);
    }
    vA = puni->pos;
    vA.y += 1.0f;
    PSVECNormalize((Vec*)&puni->vel, &nA);
    qA = vA;
    rA = nA;
    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dA = mag;
    if (puni->flags & 0x1000) {
        puni->field_0x210 =
            hitCheckFilter(NULL, qA.x, qA.y, qA.z, rA.x, rA.y, rA.z, &hitPA.x, &hitPA.y, &hitPA.z, &dA,
                                &hitNA.x, &hitNA.y, &hitNA.z);
    }
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->pos, (Vec*)&puni->vel, &puni->pos);
    } else {
        svA = puni->pos;
        *(Vec*)(puni->unk21C + 0x344) = hitNA;
        PSVECAdd(&puni->pos, (Vec*)&puni->vel, &puni->pos);
        if (puni->vel.y == 0.0f) {
            hitNA.y = 0.0f;
        }
        PSVECNormalize(&hitNA, &hitNA);
        C_VECReflect(&rA, &hitNA, &reflA);
        PSVECScale(&reflA, (Vec*)&puni->vel, mag);
        t = PSVECDotProduct(&rA, &hitNA);
        PSVECScale(&hitNA, &hitNA, t * (-(mag - dA)) + 0.1f);
        PSVECAdd(&puni->pos, &hitNA, &puni->pos);
        PSVECSubtract(&puni->pos, &svA, &sepA);
        dA = PSVECMag(&sepA);
        if (dA > 0.0f) {
            PSVECNormalize(&sepA, &sepA);
            puni->field_0x214 = hitCheckFilter(NULL, svA.x, svA.y + 1.0f, svA.z, sepA.x, sepA.y,
                                                             sepA.z, &hitP2Ax, &hitP2Ay, &hitP2Az, &dA, &hitN2Ax,
                                                             &hitN2Ay, &hitN2Az);
            if (puni->field_0x214 != 0) {
                puni->pos = svA;
            }
        }
    }
    if (puni->field_0x210 != 0 || puni->field_0x214 != 0) {
        puni->flags |= 0x400000;
    } else {
        PSVECScale((Vec*)&puni->vel, (Vec*)&puni->vel, 0.6f);
        puni->flags &= ~0x400000;
    }
    endA:
    puniSetYPosition(puni);
        }
        break;
    case 4:
        {
    f32 mag;
    f32 t;

    mag = PSVECMag((Vec*)&puni->vel);
    if (!mag) goto endB;
    if (mag > *(f32*)(puni->unk168 + 0xC)) {
        PSVECScale((Vec*)&puni->vel, (Vec*)&puni->vel,
                   *(f32*)(puni->unk168 + 0xC) / mag);
        mag = *(f32*)(puni->unk168 + 0xC);
    }
    vB = puni->pos;
    vB.y += 1.0f;
    PSVECNormalize((Vec*)&puni->vel, &nB);
    rB = nB;
    qB = vB;
    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dB = mag;
    puni->field_0x210 =
        hitCheckFilter(NULL, qB.x, qB.y, qB.z, rB.x, rB.y, rB.z, &hitPB.x, &hitPB.y, &hitPB.z, &dB,
                            &hitNB.x, &hitNB.y, &hitNB.z);
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->pos, (Vec*)&puni->vel, &puni->pos);
    } else if (Punigp->area != 0x12) {
        svB = puni->pos;
        *(Vec*)(puni->unk21C + 0x344) = hitNB;
        PSVECAdd(&puni->pos, (Vec*)&puni->vel, &puni->pos);
        if (puni->vel.y == 0.0f) {
            hitNB.y = 0.0f;
        }
        PSVECNormalize(&hitNB, &hitNB);
        t = PSVECDotProduct(&rB, &hitNB);
        PSVECScale(&hitNB, &hitNB, t * (-(mag - dB)) + 0.1f);
        PSVECAdd(&puni->pos, &hitNB, &puni->pos);
        PSVECSubtract(&puni->pos, &svB, &sepB);
        dB = PSVECMag(&sepB);
        if (dB > 0.0f) {
            PSVECNormalize(&sepB, &sepB);
            puni->field_0x214 = hitCheckFilter(NULL, svB.x, svB.y + 1.0f, svB.z, sepB.x, sepB.y,
                                                             sepB.z, &hitP2Bx, &hitP2By, &hitP2Bz, &dB, &hitN2Bx,
                                                             &hitN2By, &hitN2Bz);
            if (puni->field_0x214 != 0) {
                puni->pos = svB;
            }
        }
    }
    if (puni->field_0x210 != 0 || puni->field_0x214 != 0) {
        puni->flags |= 0x400000;
    } else {
        puni->flags &= ~0x400000;
    }
    endB:
    puniSetYPosition(puni);
        }
        break;
    case 9:
        {
    f32 mag;
    f32 t;

    mag = PSVECMag((Vec*)&puni->vel);
    if (!mag) goto endD;
    vD = puni->pos;
    vD.y += 1.0f;
    PSVECNormalize((Vec*)&puni->vel, &nD);
    qD = vD;
    rD = nD;
    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dD = mag;
    if (puni->flags & 0x1000) {
        puni->field_0x210 =
            hitCheckFilter(NULL, qD.x, qD.y, qD.z, rD.x, rD.y, rD.z, &hitPD.x, &hitPD.y, &hitPD.z, &dD,
                                &hitND.x, &hitND.y, &hitND.z);
    }
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->pos, (Vec*)&puni->vel, &puni->pos);
    } else {
        svD = puni->pos;
        *(Vec*)(puni->unk21C + 0x344) = hitND;
        PSVECAdd(&puni->pos, (Vec*)&puni->vel, &puni->pos);
        if (puni->vel.y == 0.0f) {
            hitND.y = 0.0f;
        }
        PSVECNormalize(&hitND, &hitND);
        C_VECReflect(&rD, &hitND, &reflD);
        PSVECScale(&reflD, (Vec*)&puni->vel, mag);
        t = PSVECDotProduct(&rD, &hitND);
        PSVECScale(&hitND, &hitND, t * (-(mag - dD)) + 0.1f);
        PSVECAdd(&puni->pos, &hitND, &puni->pos);
        PSVECSubtract(&puni->pos, &svD, &sepD);
        dD = PSVECMag(&sepD);
        if (dD > 0.0f) {
            PSVECNormalize(&sepD, &sepD);
            puni->field_0x214 = hitCheckFilter(NULL, svD.x, svD.y + 1.0f, svD.z, sepD.x, sepD.y,
                                                             sepD.z, &hitP2Dx, &hitP2Dy, &hitP2Dz, &dD, &hitN2Dx,
                                                             &hitN2Dy, &hitN2Dz);
            if (puni->field_0x214 != 0) {
                puni->pos = svD;
            }
        }
    }
    if (puni->field_0x210 != 0 || puni->field_0x214 != 0) {
        puni->flags |= 0x400000;
    } else {
        puni->flags &= ~0x400000;
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

    mag = PSVECMag((Vec*)&puni->vel);
    if (!mag) goto endE;
    if (mag > 1.0f) {
        if (((Vec*)&puni->vel)->x < 0.0f) {
            puni->direction = 270.0f;
        } else {
            puni->direction = 90.0f;
        }
    }
    if (mag > *(f32*)(puni->unk168 + 0xC)) {
        PSVECScale((Vec*)&puni->vel, (Vec*)&puni->vel,
                   *(f32*)(puni->unk168 + 0xC) / mag);
        mag = *(f32*)(puni->unk168 + 0xC);
    }
    vE = puni->pos;
    vE.y += 1.0f;
    PSVECNormalize((Vec*)&puni->vel, &nE);
    qE = vE;
    rE = nE;
    puni->field_0x214 = 0;
    puni->field_0x210 = 0;
    dE = mag;
    if (puni->flags & 0x1000) {
        puni->field_0x210 =
            hitCheckFilter(NULL, qE.x, qE.y, qE.z, rE.x, rE.y, rE.z, &hitPE.x, &hitPE.y, &hitPE.z, &dE,
                                &hitNE.x, &hitNE.y, &hitNE.z);
    }
    if (puni->field_0x210 == 0) {
        PSVECAdd(&puni->pos, (Vec*)&puni->vel, &puni->pos);
    } else {
        svE = puni->pos;
        *(Vec*)(puni->unk21C + 0x344) = hitNE;
        PSVECAdd(&puni->pos, (Vec*)&puni->vel, &puni->pos);
        if (puni->vel.y == 0.0f) {
            hitNE.y = 0.0f;
        }
        PSVECNormalize(&hitNE, &hitNE);
        C_VECReflect(&rE, &hitNE, &reflE);
        PSVECScale(&reflE, (Vec*)&puni->vel, mag);
        t = PSVECDotProduct(&rE, &hitNE);
        PSVECScale(&hitNE, &hitNE, t * (-(mag - dE)) + 0.1f);
        PSVECAdd(&puni->pos, &hitNE, &puni->pos);
        PSVECSubtract(&puni->pos, &svE, &sepE);
        dE = PSVECMag(&sepE);
        if (dE > 0.0f) {
            PSVECNormalize(&sepE, &sepE);
            puni->field_0x214 = hitCheckFilter(NULL, svE.x, svE.y + 1.0f, svE.z, sepE.x, sepE.y,
                                                             sepE.z, &hitP2Ex, &hitP2Ey, &hitP2Ez, &dE, &hitN2Ex,
                                                             &hitN2Ey, &hitN2Ez);
            if (puni->field_0x214 != 0) {
                puni->pos = svE;
            }
        }
    }
    if (puni->field_0x210 != 0 || puni->field_0x214 != 0) {
        puni->flags |= 0x400000;
    } else {
        PSVECScale((Vec*)&puni->vel, (Vec*)&puni->vel, 0.6f);
        puni->flags &= ~0x400000;
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
        puni->flags &= ~0x20000000;
        if (!(puni->flags & 0x400)) {
            continue;
        }
        if (!(puni->flags & 0x100000)) {
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
            if (!(puni2->flags & 0x400)) {
                continue;
            }
            if (!(puni2->flags & 0x100000)) {
                continue;
            }
            if (puni == puni2) {
                continue;
            }
            dist = PSVECDistance(&puni->pos, &puni2->pos);
            radius = puni->field_0x190 + puni2->field_0x190;
            range = puni->field_0x198 + (radius + puni->field_0x194);
            if (puni2->field_0x8 & 0x20) {
                if (dist < radius) {
                    PSVECSubtract(&puni->pos, &puni2->pos, &tmp);
                    if (PSVECMag(&tmp) == 0.0f) {
                        tmp.x = 1.0f;
                        dist = 1.0f;
                    }
                    PSVECNormalize(&tmp, &tmp);
                    PSVECScale(&tmp, &tmp, radius - dist);
                    PSVECAdd((Vec*)&puni->vel, &tmp, (Vec*)&puni->vel);
                }
            } else if (dist <= range) {
                if (dist < range - puni->field_0x194) {
                    puni->flags |= 0x20000000;
                }
                if (!(puni->flags & 0x10000000) &&
                    dist < puni->field_0x190 + puni2->field_0x190) {
                    puni->flags |= 0x10000000;
                }
                if (puni->flags & 0x10000000) {
                    PSVECSubtract(&puni->pos, &puni2->pos, &tmp);
                    if (PSVECMag(&tmp) == 0.0f) {
                        tmp.x = 1.0f;
                        dist = 1.0f;
                    }
                    PSVECNormalize(&tmp, &tmp);
                    f0 = (range - dist) / range;
                    f1 = 0.85f * (0.3f * f0) + 0.044999994f;
                    tmp.y = 0.0f;
                    PSVECScale(&tmp, &tmp, f1);
                    PSVECAdd((Vec*)&puni->vel, &tmp, (Vec*)&puni->vel);
                    PSVECSubtract((Vec*)&puni2->vel, &tmp, (Vec*)&puni2->vel);
                }
            }
        }
    }
    for (puni = Punidat; puni < &Punidat[PUNI_DATA_MAX]; puni++) {
        if (!(puni->flags & 0x400)) {
            continue;
        }
        if (!(puni->flags & 0x100000)) {
            continue;
        }
        if (!(puni->flags & 0x20000000)) {
            puni->flags &= ~0x10000000;
        }
        if (PSVECMag((Vec*)&puni->vel) >= 8.0f) {
            PSVECNormalize((Vec*)&puni->vel, (Vec*)&puni->vel);
            PSVECScale((Vec*)&puni->vel, (Vec*)&puni->vel, 8.0f);
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
        puni->flags &= ~0x20000000;
        if (!(puni->flags & 0x400)) {
            continue;
        }
        if (!(puni->flags & 0x100000)) {
            continue;
        }
        for (puni2 = Punidat; puni2 <= &Punidat[109]; puni2++) {
            if (!(puni2->flags & 0x400)) {
                continue;
            }
            if (!(puni2->flags & 0x100000)) {
                continue;
            }
            if (puni == puni2) {
                continue;
            }
            dist = PSVECDistance(&puni->pos, &puni2->pos);
            radius = puni->field_0x190 + puni2->field_0x190;
            range = puni->field_0x198 + (radius + puni->field_0x194);
            if (puni2->field_0x8 & 0x20) {
                if (dist < radius) {
                    PSVECSubtract(&puni->pos, &puni2->pos, &tmp);
                    if (PSVECMag(&tmp) == 0.0f) {
                        tmp.x = 1.0f;
                        dist = 1.0f;
                    }
                    PSVECNormalize(&tmp, &tmp);
                    PSVECScale(&tmp, &tmp, radius - dist);
                    PSVECAdd((Vec*)&puni->vel, &tmp, (Vec*)&puni->vel);
                }
            } else if (dist <= range) {
                if (dist < range - puni->field_0x194) {
                    puni->flags |= 0x20000000;
                }
                if (!(puni->flags & 0x10000000) &&
                    dist < puni->field_0x190 + puni2->field_0x190) {
                    puni->flags |= 0x10000000;
                }
                if (puni->flags & 0x10000000) {
                    PSVECSubtract(&puni->pos, &puni2->pos, &tmp);
                    if (PSVECMag(&tmp) == 0.0f) {
                        tmp.x = 1.0f;
                        dist = 1.0f;
                    }
                    PSVECNormalize(&tmp, &tmp);
                    f0 = (range - dist) / range;
                    f1 = 0.85f * (0.3f * f0) + 0.044999994f;
                    tmp.y = 0.0f;
                    PSVECScale(&tmp, &tmp, f1);
                    PSVECAdd((Vec*)&puni->vel, &tmp, (Vec*)&puni->vel);
                    PSVECSubtract((Vec*)&puni2->vel, &tmp, (Vec*)&puni2->vel);
                }
            }
        }
    }
    for (puni = Punidat; puni <= &Punidat[109]; puni++) {
        if (!(puni->flags & 0x400)) {
            continue;
        }
        if (!(puni->flags & 0x100000)) {
            continue;
        }
        if (!(puni->flags & 0x20000000)) {
            puni->flags &= ~0x10000000;
        }
        if (PSVECMag((Vec*)&puni->vel) >= 8.0f) {
            PSVECNormalize((Vec*)&puni->vel, (Vec*)&puni->vel);
            PSVECScale((Vec*)&puni->vel, (Vec*)&puni->vel, 8.0f);
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
        if ((p->flags & 0x400) != 0 && (p->animIdx == 1 || p->animIdx == 2)) {
            if (i == 3) {
                special++;
            } else {
                num++;
            }
        }
    }

    Punigp->soundNum = num;
    if (num == 0) {
        Punigp->soundLevel = 0;
        divisor = 5;
    } else if (num <= 2) {
        Punigp->soundLevel = 1;
        divisor = 5;
    } else if (num <= 0xB) {
        Punigp->soundLevel = 2;
        divisor = 4;
    } else if (num <= 0x1E) {
        Punigp->soundLevel = 3;
        divisor = 3;
    } else if (num <= 0x32) {
        Punigp->soundLevel = 4;
        divisor = 2;
    } else if (num <= 0x46) {
        Punigp->soundLevel = 5;
        divisor = 2;
    } else {
        Punigp->soundLevel = 6;
        divisor = 2;
    }

    if (Punigp->soundLevel != 0 && (gp->retraceCount % divisor) == 0) {
        count = Punigp->soundNum;
        target = rand() % count + 1;
        k = 0;
        for (i = 1; i <= 0x66; i++) {
            p = &Punidat[i];
            if ((p->flags & 0x400) != 0 && (p->animIdx == 1 || p->animIdx == 2)) {
                k++;
                if (k == target) {
                    break;
                }
            }
        }
        pos = p->pos;

        count = Punigp->soundLevel;
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
        if (Punigp->togettiSfx == 0) {
            Punigp->togettiSfx = psndSFXOn_3D("SFX_STG2_TOGETTI_MOVE1\000", &pd[3].pos);
        }
    } else if (Punigp->togettiSfx != 0) {
        psndSFXOff(Punigp->togettiSfx);
        Punigp->togettiSfx = 0;
    }
    if (Punigp->togettiSfx != 0) {
        psndSFX_pos(Punigp->togettiSfx, &pd[3].pos);
    }

    num2 = 0;
    for (i = 0x6E; i <= 0xD1; i++) {
        q = &Punidat[i];
        if ((q->flags & 0x400) != 0 && (q->animIdx == 1 || q->animIdx == 2)) {
            num2++;
        }
    }
    Punigp->moveCount = num2;
    if (num2 == 0) {
        Punigp->moveLevel = 0;
    } else if (num2 <= 2) {
        Punigp->moveLevel = 1;
    } else if (num2 <= 0xB) {
        Punigp->moveLevel = 2;
    } else {
        Punigp->moveLevel = 3;
    }

    if (Punigp->moveLevel != Punigp->prevMoveLevel) {
        if ((u32)Punigp->moveSfx != 0) {
            psndSFXOff(Punigp->moveSfx);
        }
        switch (Punigp->moveLevel) {
        case 1:
            Punigp->moveSfx = psndSFXOn("SFX_STG2_TOGE_MOVE1");
            break;
        case 2:
            Punigp->moveSfx = psndSFXOn("SFX_STG2_TOGE_MOVE2");
            break;
        case 3:
            Punigp->moveSfx = psndSFXOn("SFX_STG2_TOGE_MOVE3");
            break;
        }
    }
    Punigp->prevMoveLevel = Punigp->moveLevel;
    Punigp->fightSfxCount0 = 0;
    Punigp->fightSfxCount1 = 0;
}

void puni_puni_main(void) {
    MarioWork* mario;
    PuniData* p0;
    s32 i;
    PuniData* pd;
    s32 i2;
    PuniData* pd2;
    s32 j;
    Vec v;
    Vec tmp;
    Vec tmp2;
    Vec tmp3;

    mario = marioGetPtr();
    p0 = *(PuniData**)&Punigp->marioPuni;
    p0->pos = mario->position;
    tmp = p0->pos;
    puniMapFindbuff(&tmp);
    puniModeALLCheck();

    if ((Punigp->flags & 0x20) != 0) {
        PuniData* pd = &Punidat[1];
        s32 i;
        for (i = 1; i < PUNI_DATA_MAX; i++, pd++) {
            PSVECSubtract(&pd->pos, (Vec*)&pd->field_0x138, &v);
            pd->field_0x1AC = PSVECMag(&v);
            if ((pd->flags & 0x400) != 0) {
                pd->flags &= ~0x06000000;
                pd->field_0x8 &= ~0x4;
                *(u16*)((u8*)pd + 0x586) = 0;
                puniParaRecover(pd);
            }
        }
    }

    if ((Punigp->flags & 0x20) != 0) {
        PartyEntry* party;
        PuniSearchEntry* sei;
        s32 k;
        for (j = 0; j < 2; j++) {
            pd2 = &Punidat[1];
            for (i2 = 1; i2 < 0xD3; i2++, pd2++) {
                if ((pd2->flags & 0x400) != 0) {
                    if (j == 0) {
                        if (pd2->mode != 7) {
                            continue;
                        }
                        pd2->flags |= 0x02000000;
                    } else {
                        if ((pd2->flags & 0x02000000) != 0) {
                            continue;
                        }
                        pd2->flags |= 0x02000000;
                    }
                    if ((pd2->flags & 0x04000000) == 0) {
                    pd2->flags |= 0x04000000;
                    if ((pd2->field_0x8 & 0x10) != 0) {
                        pd2->field_0x8 &= ~0x10;
                        pd2->mode = 0xC;
                        pd2->state = 0;
                    }
                    party = partyGetPtr(marioGetPartyId());
                    pd2->field_0x1A0 = 0.0f;
                    if (Punigp->windDisabled == 0 && party != NULL) {
                        tmp2 = pd2->pos;
                        pd2->field_0x1A0 = cloudGetBreathPower(10.0f, &tmp2);
                    }
                    if (32.0 < __fabsf(pd2->pos.y - mario->position.y)) {
                        pd2->field_0x1A4 = 1000.0f;
                    } else {
                        pd2->field_0x1A4 = PSVECDistance(&pd2->pos, &mario->position);
                    }
                    sei = (PuniSearchEntry*)Punigp->targets;
                    pd2->field_0x1B4 = -1;
                    pd2->field_0x1B8 = 1e+06f;
                    pd2->field_0x1BC = -1;
                    pd2->field_0x1C0 = 1e+06f;
                    for (k = 0; k < 10; k++, sei++) {
                        if ((sei->flags & 1) != 0 && !(32.0 < __fabsf(pd2->pos.y - sei->pos.y))) {
                            if ((sei->flags & 4) != 0) {
                                f32 d = PSVECSquareDistance(&pd2->pos, &sei->pos);
                                if (pd2->field_0x1C0 > d) {
                                    pd2->field_0x1BC = k;
                                    pd2->field_0x1C0 = d;
                                }
                            }
                            if ((sei->flags & 2) != 0) {
                                f32 d = PSVECSquareDistance(&pd2->pos, &sei->pos);
                                if (pd2->field_0x1B8 > d) {
                                    pd2->field_0x1B4 = k;
                                    pd2->field_0x1B8 = d;
                                }
                            }
                        }
                    }
                    }
                    unk_mri_00012d58(pd2);
                }
            }
        }
        puniOption();
    } else {
        PartyEntry* party;
        PuniSearchEntry* sei;
        s32 k;
        pd = &Punidat[1];
        for (i = 1; i <= 0x6D; i++, pd++) {
            if ((pd->flags & 0x400) != 0) {
                party = partyGetPtr(marioGetPartyId());
                pd->field_0x1A0 = 0.0f;
                if (Punigp->windDisabled == 0 && party != NULL) {
                    tmp3 = pd->pos;
                    pd->field_0x1A0 = cloudGetBreathPower(10.0f, &tmp3);
                }
                if (32.0 < __fabsf(pd->pos.y - mario->position.y)) {
                    pd->field_0x1A4 = 1000.0f;
                } else {
                    pd->field_0x1A4 = PSVECDistance(&pd->pos, &mario->position);
                }
                sei = (PuniSearchEntry*)Punigp->targets;
                pd->field_0x1B4 = -1;
                pd->field_0x1B8 = 1e+06f;
                pd->field_0x1BC = -1;
                pd->field_0x1C0 = 1e+06f;
                for (k = 0; k < 10; k++, sei++) {
                    if ((sei->flags & 1) != 0 && !(32.0 < __fabsf(pd->pos.y - sei->pos.y))) {
                        if ((sei->flags & 4) != 0) {
                            f32 d = PSVECSquareDistance(&pd->pos, &sei->pos);
                            if (pd->field_0x1C0 > d) {
                                pd->field_0x1BC = k;
                                pd->field_0x1C0 = d;
                            }
                        }
                        if ((sei->flags & 2) != 0) {
                            f32 d = PSVECSquareDistance(&pd->pos, &sei->pos);
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

    if ((Punigp->flags & 0x20) != 0) {
        s32 i;
        PuniData* pd;
        PuniPose* pose;
        pose = puniGetPosePtr();
        pd = &Punidat[1];
        for (i = 1; i < PUNI_DATA_MAX; i++, pd++) {
            if ((pd->flags & 0x400) != 0) {
                if ((pd->flags & 0x200) == 0) {
                    *(Vec*)&pd->field_0x138 = pd->pos;
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
            if ((pd->flags & 0x400) != 0) {
                if ((pd->flags & 0x200) == 0) {
                    *(Vec*)&pd->field_0x138 = pd->pos;
                    fn_19_13A90(pd);
                }
                puniAnimation(pd, pose);
            }
            pose++;
        }
    }

    puniSound();
    switch (Punigp->phase) {
    case 2:
        if (Punigp->prevPhase == 2) {
            Punigp->phase = 0;
        }
        break;
    case 0:
        break;
    case 1:
        break;
    case 3:
        break;
    }
    Punigp->prevPhase = Punigp->phase;
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

static inline void puniPaternInitAll(void) {
    s32 i;
    s16* ep;
    s32 j;
    s16 v;
    s16* ep2;
    s32 j2;
    s16 v2;
    PuniPaternData* pp = (PuniPaternData*)&Punigp->animSet[0];
    PuniPaternData* tp = (PuniPaternData*)&Punigp->animSet[1];

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
        ep2 = &tp->end[i];
        j2 = 0;
        for (;;) {
            v2 = ((s16*)toge_patern_table[i])[j2];
            if (v2 == -1) {
                *ep2 = v2;
                break;
            }
            if (v2 == -2) {
                *ep2 = v2;
                break;
            }
            j2++;
        }
        tp->num[i] = j2;
    }
}


USER_FUNC(evt_puni_init) {
    s32* args = event->args;
    s32 count;
    PuniTexInfo* tex;
    void (*cb0)(void);
    void (*cb1)(void);
    void (*cb2)(void);
    count = evtGetValue(event, args[0]);
    tex = Punitex;
    cb0 = (void (*)(void))evtGetValue(event, args[1]);
    cb1 = (void (*)(void))evtGetValue(event, args[2]);
    cb2 = (void (*)(void))evtGetValue(event, args[3]);

    if (isFirstCall) {
        puniInit();
        return EVT_RETURN_BLOCK;
    }

    puniPaternInitAll();

    puniAnimationInit2();
    puniEntry(count, tex, cb0, cb1, cb2);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_yuka) {
    s32* args = event->args;

    Punigp->area = evtGetValue(event, args[0]) + 1;
    Punigp->levelY[0] = (f32)evtGetValue(event, args[1]);
    Punigp->levelY[1] = (f32)evtGetValue(event, args[2]);
    Punigp->levelY[2] = (f32)evtGetValue(event, args[3]);
    Punigp->levelY[3] = (f32)evtGetValue(event, args[4]);
    Punigp->yMode = evtGetValue(event, args[5]);
    Punigp->mapKind = evtGetValue(event, args[6]);
    swByteSet(0x299, Punigp->area);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_hamari) {
    s32* args = event->args;

    Punigp->respawnPos.x = (f32)evtGetValue(event, args[0]);
    Punigp->respawnPos.y = (f32)evtGetValue(event, args[1]);
    Punigp->respawnPos.z = (f32)evtGetValue(event, args[2]);
    Punigp->bandTop0 = (f32)evtGetValue(event, args[3]);
    Punigp->bandBottom0 = (f32)evtGetValue(event, args[4]);
    Punigp->bandTop1 = (f32)evtGetValue(event, args[5]);
    Punigp->bandBottom1 = (f32)evtGetValue(event, args[6]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_maze) {
    switch (Punigp->mapKind) {
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
    f32 d = __fabs(y - Punigp->levelY[0]);
    f32 r = Punigp->levelY[0];
    f32 t;

    t = __fabs(y - Punigp->levelY[1]);
    if (d > t) {
        d = t;
        r = Punigp->levelY[1];
    }
    t = __fabs(y - Punigp->levelY[2]);
    if (d > t) {
        d = t;
        r = Punigp->levelY[2];
    }
    t = __fabs(y - Punigp->levelY[3]);
    if (d > t) {
        r = Punigp->levelY[3];
    }
    return r;
}

void puni_init_para(PuniData* punidat, f32 x, f32 y, f32 z) {
    PuniData* pd = punidat;

    pd->state = 0;
    pd->animIdx = 0;
    pd->prevAnimIdx = 1000;
    pd->pos.x = x;
    pd->pos.y = y;
    pd->pos.z = z;
    pd->field_0x138 = pd->pos;
    pd->field_0x190 = 6.0f;
    pd->field_0x194 = 2.0f;
    pd->field_0x198 = 3.0f;
    pd->hp = 0x3C;
    pd->maxHp = 0x3C;
    pd->hpRegen = 10;
    pd->regenTimer = 0x3C;
    pd->regenInterval = 0x3C;
    pd->attack = 20;
    pd->defense = 10;
    pd->field_0x582 = 10;
    pd->damage = 0;
    pd->engaged = 0;
    pd->hitChance = 50;
    pd->hitChanceAlt = 70;
    pd->parentPuni = 0;
    pd->leftPuni = 0;
    pd->rightPuni = 0;
    PuniDokuritsu(pd);
    pd->flags |= 0x1401;
}

void toge_init_para(PuniData* punidat, f32 x, f32 y, f32 z) {
    PuniData* pd = punidat;

    pd->animIdx = 0;
    pd->state = 0;
    pd->prevAnimIdx = 1000;
    pd->pos.x = x;
    pd->pos.y = y;
    pd->pos.z = z;
    pd->field_0x138 = pd->pos;
    pd->field_0x190 = 6.0f;
    pd->field_0x194 = 2.0f;
    pd->field_0x198 = 3.0f;
    pd->hp = 0x3C;
    pd->maxHp = 0x3C;
    pd->hpRegen = 10;
    pd->regenTimer = 0x3C;
    pd->regenInterval = 0x3C;
    pd->attack = 20;
    pd->defense = 10;
    pd->field_0x582 = 10;
    pd->damage = 0;
    pd->engaged = 0;
    pd->hitChance = 50;
    pd->hitChanceAlt = 70;
    pd->flags |= 0x00101402;
}

// target reproduces the inline result f29 and the per-case copy; matched 100% by putting the
// `posy = t` copy between the two randf calls and declaring rx before posy.
USER_FUNC(evt_puni_rnd_pos) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 arg1 = args[1];
    s32 arg2 = args[2];
    s32 arg3 = args[3];
    f32 x = evtGetFloat(event, arg1);
    f32 t = puni_init_posy(evtGetFloat(event, arg2));
    f32 z = evtGetFloat(event, arg3);
    f32 rx;
    f32 posy;
    f32 rz;

    switch (type) {
    case 0:
        rx = randf(x - 10.0f, x + 10.0f);
        posy = t;
        rz = randf(z - 10.0f, z + 10.0f);
        break;
    case 1:
        rx = randf(x - 50.0f, x + 50.0f);
        posy = t;
        rz = randf(z - 50.0f, z + 50.0f);
        break;
    }
    evtSetFloat(event, arg1, rx);
    evtSetFloat(event, arg2, posy);
    evtSetFloat(event, arg3, rz);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_get_systemmode) {
    evtSetValue(event, event->args[0], Punigp->systemMode);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_systemmode) {
    Punigp->systemMode = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_wait_systemmode) {
    s32 mode = evtGetValue(event, event->args[0]);

    if (Punigp->systemMode != mode) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_debug_info) {
    char buf[256];
    s32 i;
    s32 index;
    Vec pos;
    s32 cond;
    s32 trap;

    for (i = 1; i <= 0x66; i++) {
        puniswGetindex(0xF70F2E80, i - 1, &index, &pos);
        puniswTrapGetindex(i, &trap);
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
        if (!trap) {
            if (!cond) {
                if (!index) {
                    sprintf(buf, "No%03d,ついて来ています\n", i);
                } else {
                    sprintf(buf, "No%03d,はぐれています area = mri_%02d, x = %04d, y = %04d, z = %04d,\n", i, index - 1,
                            (s32)pos.x, (s32)pos.y, (s32)pos.z);
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

    Punigp->phase = 0;
    if (a == 2) {
        Punigp->flags &= ~0xf;
        switch (b) {
        case 6:
            Punigp->flags |= 1;
            break;
        case 2:
            Punigp->flags |= 2;
            break;
        case 8:
            Punigp->flags |= 4;
            break;
        case 9:
            Punigp->flags |= 8;
            break;
        }
    }
    if (Punigp->area == 9 || Punigp->area == 0xc)
        Punigp->flags |= 0x20;
    if (swByteGet(0x299) == 1)
        return 2;

    p = &Punidat[1];
    for (i = 1; i <= 0x6d; i++, p++) {
        s32 c0;
        Vec pos;

        if (p->flags & 0x40000000) {
            p->flags &= ~0x40000000;
            continue;
        }
        if (i >= 0x67) {
            p->field_0x8 |= 0x20;
            continue;
        }
        puniswGetindex(0xF70F2E80, i - 1, &c0, &pos);
        if (c0 != 0 && c0 != Punigp->area)
            continue;
        {
            s32 bit;
            puniswTrapGetindex(i, &bit);
            if (bit != 0)
                continue;
        }
        if (i == 1) {
            if (swGet(0xb3c) == 0)
                continue;
            if (swGet(0xb14) != 0)
                continue;
            Punigp->switchPuni[0] = p;
            p->flags |= 0x20;
        } else if (i == 2) {
            if (swGet(0xb3d) == 0)
                continue;
            Punigp->switchPuni[1] = p;
            p->flags |= 0x40;
        } else if (i == 4) {
            if (swGet(0xb3f) == 0)
                continue;
            Punigp->switchPuni[3] = p;
            p->flags |= 0x100;
        } else if (i == 3) {
            if (swGet(0xb3e) == 0)
                continue;
            Punigp->switchPuni[2] = p;
            p->flags |= 0x80;
        } else {
            if (swByteGet(0x28b) + 5 <= i)
                continue;
            p->flags |= 0x8;
        }

        if (c0 == 0) {
            if (gp->flags & 1) {
                sx = puniRandfSetup(mario->position.x - 10.0f, mario->position.x + 10.0f);
                sy = mario->position.y;
                sz = puniRandfSetup(mario->position.z - 10.0f, mario->position.z + 10.0f);
                p->mode = 1;
            } else {
                sx = bero_get_BeroSX();
                sy = puni_init_posy(bero_get_BeroSY());
                sz = bero_get_BeroSZ();
                ex = bero_get_BeroEX();
                ey = puni_init_posy(bero_get_BeroEY());
                ez = bero_get_BeroEZ();
                if ((Punigp->flags & 8) == 0) {
                    sx = puniRandfSetup(sx - 10.0f, sx + 10.0f);
                    sz = puniRandfSetup(sz - 10.0f, sz + 10.0f);
                } else {
                    ez += 60.0f;
                }
                p->accel.x = puniRandfSetup(ex - 10.0f, ex + 10.0f);
                p->accel.y = ey;
                p->accel.z = puniRandfSetup(ez - 10.0f, ez + 10.0f);
                p->mode = 0x19;
                p->flags &= ~0x1000;
            }
        } else {
            sx = pos.x;
            sy = pos.y;
            sz = pos.z;
            p->mode = 0xf;
            p->flags |= 0x8000;
        }
        p->state = 0;
        p->animIdx = 0;
        p->prevAnimIdx = 1000;
        p->pos.x = sx;
        p->pos.y = sy;
        p->pos.z = sz;
        *(Vec*)&p->field_0x138 = p->pos;
        p->field_0x190 = 6.0f;
        p->field_0x194 = 2.0f;
        p->field_0x198 = 3.0f;
        p->hp = 60;
        p->maxHp = 60;
        p->hpRegen = 10;
        p->regenTimer = 60;
        p->regenInterval = 60;
        p->attack = 20;
        p->defense = 10;
        p->field_0x582 = 10;
        p->damage = 0;
        p->engaged = 0;
        p->hitChance = 50;
        p->hitChanceAlt = 70;
        p->parentPuni = 0;
        p->leftPuni = 0;
        p->rightPuni = 0;
        PuniDokuritsu(p);
        p->flags |= 0x1401;
    }

    beros = bero_get_ptr();
    num = bero_get_BeroNUM();
    for (j = 0; j < num; j++) {
        if (beros[j]->kinddirArg3 != 9)
            continue;
        p = &Punidat[103] + j;
        p->flags |= 0x00100400;
        p->field_0xE = 0x1c;
        p->mode = 0x1c;
        p->field_0x190 = 25.0f;
        hitObjGetPos(beros[j]->hitName, &p->pos);
        p->pos.y = puni_init_posy(p->pos.y);
    }
    return 2;
}

static inline f32 puniRandUnit(void) {
    return (f32)rand() / 32767.0f;
}

// 100%: the bounds written inline in the loop fix the constant pool; the two random draws through an inline helper
// and `zhi` as a named temp give x/xrange/zrange/rx the original's f21-f23 (permuter, 2026-10-05).
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
    for (i = 110; i < count; i++) {
        PuniData* p = &Punidat[i];

        f32 rx = puniRandUnit();
        f32 rz = puniRandUnit();

        p->flags |= 0x10;
        p->mode = 0;
        p->angle = i;
        p->animIdx = 0;
        p->state = 0;
        p->prevAnimIdx = 1000;
        p->pos.x = (x - 50.0f) + rx * ((x + 50.0f) - (x - 50.0f));
        p->pos.y = y;
        zhi = z + 120.0f;
        p->pos.z = (z - 120.0f) + rz * (zhi - (z - 120.0f));
        *(Vec*)&p->field_0x138 = p->pos;
        p->field_0x190 = 6.0f;
        p->field_0x194 = 2.0f;
        p->field_0x198 = 3.0f;
        p->hp = 60;
        p->maxHp = 60;
        p->hpRegen = 10;
        p->regenTimer = 60;
        p->regenInterval = 60;
        p->attack = 20;
        p->defense = 10;
        p->field_0x582 = 10;
        p->damage = 0;
        p->engaged = 0;
        p->hitChance = 50;
        p->hitChanceAlt = 70;
        p->flags |= 0x00101402;
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
        punidat->pos = *src;
        return 1;
    }
    pos = *src;
    PSVECNormalize(&dir, &dir);
    distance = PSVECMag(&dir);
    if (hitCheckFilter(NULL, pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, &hitX, &hitY, &hitZ, &distance, &hitNX,
                       &hitNY, &hitNZ) == NULL) {
        punidat->pos = *dest;
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
        punidat->pos = *pos;
        return TRUE;
    }
    p = *pos;
    PSVECNormalize(&delta, &delta);
    dist = PSVECMag(&delta);
    if (hitCheckFilter(NULL, p.x, p.y, p.z, delta.x, delta.y, delta.z, &out.x, &out.y, &out.z, &dist, &n.x, &n.y,
                       &n.z) == NULL) {
        punidat->pos = *target;
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
    s32 kind = punidat->mode;

    switch (kind) {
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x15:
    case 0x16: {
        Vec v = { 0.0f, -1000.0f, 0.0f };

        punidat->pos = v;
        return;
    }
    }
    {
        pos = punidat->pos;
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

// Lifetime splitting stays off, as in puniswSetindex and evt_puni_all_save: with it on, the inlined copies'
// high-byte renames get their own register block ahead of the offset, where the original colours them behind it.
#pragma opt_lifetimes off
USER_FUNC(evt_puni_exit) {
    s32 arg2;
    s32 k;
    PuniData* punidat;
    const char* name;
    s32* args;
    s32 i;
    s32 mode;
    s32 cmp;
    f32 f31;
    f32 f30;
    f32 f29;
    f32 f28;
    f32 f27;
    f32 f1;
    Vec pos;
    s32 area;

    args = event->args;
    mode = evtGetValue(event, args[0]);
    evtGetValue(event, args[1]);
    arg2 = evtGetValue(event, args[2]);
    name = (const char*)evtGetValue(event, args[3]);

    Punigp->phase = 5;
    if (mode == 2) {
        Punigp->flags &= ~0xF;
        switch (arg2) {
            case 6:
                Punigp->flags |= 0x1;
                break;
            case 2:
                Punigp->flags |= 0x2;
                break;
            case 8:
                Punigp->flags |= 0x4;
                break;
            case 9:
                Punigp->flags |= 0x8;
                break;
        }
    }
    if (name == NULL) {
        Punigp->flags |= 0x10;
    } else {
        Punigp->flags &= ~0x10;
    }
    if (!(Punigp->flags & 0x8)) {
        f31 = bero_get_BeroSX();
        f30 = puni_init_posy(bero_get_BeroSY());
        f29 = bero_get_BeroSZ();
        f28 = bero_get_BeroEX();
        f27 = puni_init_posy(bero_get_BeroEY());
        f1 = bero_get_BeroEZ();
    } else {
        f31 = bero_get_BeroSX();
        f30 = puni_init_posy(bero_get_BeroSY());
        f29 = bero_get_BeroSZ();
        f28 = f31;
        f27 = f30;
        f1 = f29;
        f29 += 60.0f;
    }
    Punigp->field_0x47C = 2;
    Punigp->arrivePos.x = f31;
    Punigp->arrivePos.y = f30;
    Punigp->arrivePos.z = f29;
    Punigp->arriveDistSq = 100.0f;
    Punigp->arrivePos2.x = f28;
    Punigp->arrivePos2.y = f27;
    Punigp->arrivePos2.z = f1;
    Punigp->arriveDistSq2 = 100.0f;

    if (name != NULL) {
        cmp = strcmp(name, "mri_00");
    } else {
        cmp = 1;
    }

    punidat = &Punidat[1];
    for (k = 1; k <= 0x66; k++, punidat++) {

        if (!(punidat->flags & 0x400)) {
            continue;
        }
        if (punidat->flags & 0x800000) {
            continue;
        }
        if (punidat->flags & 0x8000) {
            if (name == NULL) {
                punidat->flags |= 0x40000000;
                continue;
            }
            puniHagurePos(punidat);
            area = Punigp->area;
            pos = punidat->pos;
            puniswSetindex(0xF70F2E80, punidat->angle - 1, &area, &pos);
        } else {
            pos = (Vec){ 0.0f, 0.0f, 0.0f };
            area = 0;
            puniswSetindex(0xF70F2E80, punidat->angle - 1, &area, &pos);
            PuniDokuritsu(punidat);
            if (cmp == 0) {
                punidat->mode = 0xE;
                punidat->state = 0;
                punidat->animIdx = 0;
            } else {
                punidat->mode = 0x1A;
                punidat->state = 0;
                punidat->animIdx = 2;
            }
        }
    }

    if (name != NULL && strcmp(name, "mri_00") == 0) {
        for (i = 1; i <= 0x66; i++) {
            Vec v = { 0.0f, 0.0f, 0.0f };

            area = 0;
            puniswSetindex(0xF70F2E80, i - 1, &area, &v);
        }
    }
    return 2;
}
#pragma opt_lifetimes reset

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
        Punigp->switchPuni[0] = punidat;   // PuniWork 0x364
        punidat->flags |= 0x20;
    } else if (type == 2) {
        if (flag != 0 && swGet(0xB3D) == 0) {
            return 2;
        }
        Punigp->switchPuni[1] = punidat;   // PuniWork 0x368
        punidat->flags |= 0x40;
    } else if (type == 4) {
        if (flag != 0 && swGet(0xB3F) == 0) {
            return 2;
        }
        Punigp->switchPuni[3] = punidat;   // PuniWork 0x370
        punidat->flags |= 0x100;
    } else if (type == 3) {
        if (flag != 0 && swGet(0xB3E) == 0) {
            return 2;
        }
        Punigp->switchPuni[2] = punidat;   // PuniWork 0x36C
        punidat->flags |= 0x80;
    } else {
        if (flag != 0 && swByteGet(0x28B) + 5 <= type) {
            return 2;
        }
        punidat->flags |= 0x8;
    }

    punidat->mode = unkC;
    punidat->state = 0;
    punidat->animIdx = 0;
    punidat->prevAnimIdx = 1000;
    punidat->pos.x = x;
    punidat->pos.y = y;
    punidat->pos.z = z;
    punidat->field_0x138 = punidat->pos;
    punidat->field_0x190 = 6.0f;
    punidat->field_0x194 = 2.0f;
    punidat->field_0x198 = 3.0f;
    punidat->hp = 0x3C;
    punidat->maxHp = 0x3C;
    punidat->hpRegen = 0x0A;
    punidat->regenTimer = 0x3C;
    punidat->regenInterval = 0x3C;
    punidat->attack = 0x14;
    punidat->defense = 0x0A;
    punidat->field_0x582 = 0x0A;
    punidat->damage = 0;
    punidat->engaged = 0;
    punidat->hitChance = 0x32;
    punidat->hitChanceAlt = 0x46;
    punidat->parentPuni = 0;
    punidat->leftPuni = 0;
    punidat->rightPuni = 0;
    PuniDokuritsu(punidat);
    punidat->flags |= 0x1401;
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

    punidat->flags |= 0x10;
    punidat->mode = unkC;
    punidat->animIdx = 0;
    punidat->state = 0;
    punidat->prevAnimIdx = 1000;
    punidat->pos.x = x;
    punidat->pos.y = y;
    punidat->pos.z = z;
    punidat->field_0x138 = punidat->pos;
    punidat->field_0x190 = 6.0f;
    punidat->field_0x194 = 2.0f;
    punidat->field_0x198 = 3.0f;
    punidat->hp = 0x3C;
    punidat->maxHp = 0x3C;
    punidat->hpRegen = 0x0A;
    punidat->regenTimer = 0x3C;
    punidat->regenInterval = 0x3C;
    punidat->attack = 0x14;
    punidat->defense = 0x0A;
    punidat->field_0x582 = 0x0A;
    punidat->damage = 0;
    punidat->engaged = 0;
    punidat->hitChance = 0x32;
    punidat->hitChanceAlt = 0x46;
    punidat->flags |= 0x101402;
    return 2;
}

USER_FUNC(evt_puni_get_position) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    evtSetValue(event, args[1], (s32)punidat->pos.x);
    evtSetValue(event, args[2], (s32)punidat->pos.y);
    evtSetValue(event, args[3], (s32)punidat->pos.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_position) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    punidat->pos.x = evtGetValue(event, args[1]);
    punidat->pos.y = evtGetValue(event, args[2]);
    punidat->pos.z = evtGetValue(event, args[3]);

    // copies the Vec at 0x12C word-for-word (not an int conversion).
    *(Vec*)&punidat->field_0x138 = punidat->pos;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_get_dir) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    evtSetValue(event, args[1], (s32)punidat->direction);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_dir) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    punidat->direction = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_get_anim) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    evtSetValue(event, args[1], punidat->animIdx);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_anim) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    punidat->animIdx = evtGetValue(event, args[1]);
    punidat->prevAnimIdx = 1000;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_korokoro) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    switch ((s32)punidat->animIdx) {
    case 5:
        if (isFirstCall) {
            if (punidat->flags & 8) {
                punidat->field_0x188 = 8.0f;
            } else {
                punidat->field_0x148 = 8.0f;
            }
        }
        punidat->field_0x184 += evtGetValue(event, args[1]);
        return EVT_RETURN_BLOCK;
    default:
        if (punidat->flags & 8) {
            punidat->field_0x188 = 0.0f;
        } else {
            punidat->field_0x148 = 0.0f;
        }
        punidat->field_0x184 = evtGetValue(event, args[2]);
        return EVT_RETURN_DONE;
    }
}

// Matching: assigning the unsigned division to `cond` inside the condition (then casting to s32)
// makes MWCC CSE it into a temp that feeds a signed `cmpw`, then copy it into the callee-saved
// `elapsed` across the intplGetValue calls -- exactly the original's r4 -> r29 sequence.
USER_FUNC(evt_puni_move_position) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];
    f32 x = evtGetValue(event, args[1]);
    f32 z = evtGetValue(event, args[2]);
    s32 duration = evtGetValue(event, args[3]);

    if (isFirstCall) {
        event->savedTime = gp->renderTime;
        punidat->accel = punidat->pos;
        punidat->field_0x204.x = x;
        punidat->field_0x204.y = punidat->pos.y;
        punidat->field_0x204.z = z;
    }
    {
        u32 cond;
        s32 elapsed;

        if ((s32)(cond = ((u32)gp->renderTime - (u32)event->savedTime) / ((__OSBusClock / 4) / 1000)) < duration) {
            elapsed = cond;
            punidat->pos.x =
                intplGetValue(0, elapsed, duration, punidat->accel.x, punidat->field_0x204.x);
            punidat->pos.z =
                intplGetValue(0, elapsed, duration, punidat->accel.z, punidat->field_0x204.z);
            *(Vec*)&punidat->field_0x138 = punidat->pos;
            return EVT_RETURN_BLOCK;
        }
    }
    punidat->pos = punidat->field_0x204;
    *(Vec*)&punidat->field_0x138 = punidat->pos;
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
        punidat->accel = punidat->pos;
        punidat->field_0x204.x = x;
        punidat->field_0x204.y = punidat->pos.y;
        punidat->field_0x204.z = z;
    }
    {
        f32 sec = sysFrame2SecFloat(speed);
        f32 dist = PSVECDistance(&punidat->accel, &punidat->field_0x204);
        s32 msec = ((u32)gp->renderTime - (u32)event->savedTime) / ((__OSBusClock / 4) / 1000);
        s32 frame = (s32)(0.5f + sysMsec2FrameFloat((f32)msec));
        Vec dir;

        PSVECSubtract(&punidat->field_0x204, &punidat->accel, &dir);
        PSVECNormalize(&dir, &dir);
        if (frame < (s32)(dist / sec)) {
            PSVECScale(&dir, &dir, sec * (f32)frame);
            PSVECAdd(&punidat->accel, &dir, &punidat->pos);
            return EVT_RETURN_BLOCK;
        }
        punidat->pos = punidat->field_0x204;
        *(Vec*)&punidat->field_0x138 = punidat->pos;
        return EVT_RETURN_DONE;
    }
}

// Match-only shaping: retail computes the unsigned divide into r4, compares it signed and copies it to r28
// before the intplGetValue calls. With the natural `s32 elapsed = ...; if (elapsed < duration)` MWCC folds
// the comparison into the same callee-saved register (r28), so the `mr r28, r4` never appears. Writing the
// divide in the condition and again in the branch, with `opt_common_subs off`, reproduces retail's r4 + copy.
// The duplicate divide is value-numbered away, so the object still emits exactly one elapsed `divwu`.
#ifdef MUST_MATCH
#pragma opt_common_subs off // keep the elapsed divide out of the common-subexpression pass (see above)
#endif
USER_FUNC(evt_puni_jump_position) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];
    f32 tx = evtGetValue(event, args[1]);
    f32 ty = evtGetValue(event, args[2]);
    f32 tz = evtGetValue(event, args[3]);
    s32 duration = evtGetValue(event, args[4]);
    f32 g = evtGetValue(event, args[5]);
#ifndef MUST_MATCH
    s32 elapsed;
#endif

    if (isFirstCall) {
        f32 a = -4.0f * g;

        punidat->modeStartTime = gp->renderTime;
        punidat->accel = punidat->pos;
        punidat->height = a;
        punidat->vertVel = (ty - punidat->accel.y) - punidat->height;
        psndSFXOn_3D("SFX_STG2_PUNI_JUMP1", &punidat->pos);
    }
#ifdef MUST_MATCH
    if ((s32)(((u32)gp->renderTime - (u32)punidat->modeStartTime) / ((__OSBusClock / 4) / 1000)) < duration) {
        s32 elapsed = (s32)(((u32)gp->renderTime - (u32)punidat->modeStartTime) / ((__OSBusClock / 4) / 1000));
        f32 ratio = (f32)elapsed / (f32)duration;

        punidat->pos.x =
            intplGetValue(0, elapsed, duration, punidat->accel.x, tx);
        punidat->pos.z =
            intplGetValue(0, elapsed, duration, punidat->accel.z, tz);
        punidat->pos.y =
            ratio * (punidat->height * ratio + punidat->vertVel) + punidat->accel.y;
        return EVT_RETURN_BLOCK;
    }
#else
    elapsed = ((u32)gp->renderTime - (u32)punidat->modeStartTime) / ((__OSBusClock / 4) / 1000);
    if (elapsed < duration) {
        f32 ratio = (f32)elapsed / (f32)duration;

        punidat->pos.x =
            intplGetValue(0, elapsed, duration, punidat->accel.x, tx);
        punidat->pos.z =
            intplGetValue(0, elapsed, duration, punidat->accel.z, tz);
        punidat->pos.y =
            ratio * (punidat->height * ratio + punidat->vertVel) + punidat->accel.y;
        return EVT_RETURN_BLOCK;
    }
#endif
    punidat->pos.x = tx;
    punidat->pos.y = ty;
    punidat->pos.z = tz;
    return EVT_RETURN_DONE;
}

#ifdef MUST_MATCH
#pragma opt_common_subs reset
#endif

USER_FUNC(evt_puni_get_mode) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    evtSetValue(event, args[1], punidat->mode);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_mode) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];
    s32 mode = evtGetValue(event, args[1]);

    punidat->mode = mode;
    punidat->field_0xE = mode;
    punidat->state = 0;
    punidat->prevAnimIdx = 1000;
    PuniDokuritsu(punidat);
    if ((EffectEntry*)punidat->effect != NULL) {
        effDelete((EffectEntry*)punidat->effect);
        punidat->effect = 0;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_setsudan) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    if (punidat->flags & 0x400) {
        PuniDokuritsu(punidat);
        if ((EffectEntry*)punidat->effect != NULL) {
            effDelete((EffectEntry*)punidat->effect);
            punidat->effect = 0;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_get_flag) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];

    evtSetValue(event, args[1], punidat->flags);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_flag_onoff) {
    s32* args = event->args;
    PuniData* punidat = &Punidat[evtGetValue(event, args[0])];
    s32 on = evtGetValue(event, args[1]);
    s32 flag = evtGetValue(event, args[2]);

    if (on != 0) {
        punidat->flags &= ~flag;
    } else {
        punidat->flags |= flag;
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
        puni->flags |= 0x100000;
    } else {
        puni->flags &= ~0x100000;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_get_lsw_index) {
    s32* args = event->args;
    s32 out;
    Vec pos;
    s32 index = args[0];

    puniswGetindex(index, evtGetValue(event, args[1]), &out, &pos);
    evtSetValue(event, args[2], out);
    evtSetValue(event, args[3], (s32)pos.x);
    evtSetValue(event, args[4], (s32)pos.y);
    evtSetValue(event, args[5], (s32)pos.z);
    return EVT_RETURN_DONE;
}

#pragma opt_lifetimes off // as in puniswSetindex, which inlines here: with it on, the high bytes colour behind the offset
USER_FUNC(evt_puni_set_lsw_index) {
    s32* args = event->args;
    s32 out;
    Vec pos;
    s32 index = args[0];
    s32 i = evtGetValue(event, args[1]);

    out = evtGetValue(event, args[2]);
    pos.x = (f32)evtGetValue(event, args[3]);
    pos.y = (f32)evtGetValue(event, args[4]);
    pos.z = (f32)evtGetValue(event, args[5]);
    puniswSetindex(index, i, &out, &pos);
    return EVT_RETURN_DONE;
}
#pragma opt_lifetimes reset

USER_FUNC(evt_puni_get_trap_index) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 value;

    puniswTrapGetindex(index, &value);
    evtSetValue(event, args[1], value);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_trap_index) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    puniswTrapSetindex(index, &value);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_set_ymode) {
    Punigp->yMode = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_all_fall_check) {
    puniModeChangeFallCheck();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_all_movestop) {
    s32* args = event->args;
    s32 on = evtGetValue(event, args[0]);
    s32 i;

    for (i = 1; i <= 0x66; i++) {
        PuniData* puni = &Punidat[i];

        if (!(puni->flags & 0x400)) {
            continue;
        }
        if (on == 0) {
            puni->flags &= ~0x200;
        } else {
            puni->flags |= 0x200;
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

        if ((puni->flags & 0x400) && !(puni->flags & 0x8000)) {
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

        if (!(puni->flags & 0x400)) {
            continue;
        }
        if (puni->mode == 0x10) {
            continue;
        }
        if (10.0 < __fabs(y - puni->pos.y)) {
            continue;
        }
        if (range < __fabs(x - puni->pos.x)) {
            continue;
        }
        if (range < __fabs(z - puni->pos.z)) {
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

        if ((puni->flags & 0x400) && !(puni->flags & 0x4000)) {
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

        if (!(puni->flags & 0x400)) {
            continue;
        }
        if (puni->pos.y > threshold) {
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
    Punigp->phase = 3;
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
    Punigp->windDisabled = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_puni_force_se_off) {
    u32 se = Punigp->field_0x458; // PuniWork 0x458

    if (se != 0) {
        psndSFXOff(se);
    }
    se = Punigp->moveSfx;
    if (se != 0) {
        psndSFXOff(se);
    }
    return EVT_RETURN_DONE;
}

// Lifetime splitting has to stay off, as in puniswSetindex: without it the inlined copies' high-byte renames
// move into their own register block ahead of the offset, and the original colours them behind it.
#pragma opt_lifetimes off
USER_FUNC(evt_puni_all_save) {
    s32 i;
    PuniData* puni = &Punidat[1];
    s32 area;
    Vec pos;

    for (i = 1; i <= 0x66; i++, puni++) {
        if ((puni->flags & 0x400) == 0) {
            continue;
        }
        if ((puni->flags & 0x800000) != 0) {
            continue;
        }
        if ((puni->flags & 0x8000) != 0) {
            puniHagurePos(puni);
            area = Punigp->area;
            pos = puni->pos;
            puniswSetindex(0xF70F2E80, puni->angle - 1, &area, &pos);
        } else {
            pos = (Vec){ 0.0f, 0.0f, 0.0f };
            area = 0;
            puniswSetindex(0xF70F2E80, puni->angle - 1, &area, &pos);
        }
    }
    return EVT_RETURN_DONE;
}
#pragma opt_lifetimes reset

USER_FUNC(evt_target_flag_onoff) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    s32 on = evtGetValue(event, args[1]);
    s32 mask = evtGetValue(event, args[2]);
    s32* flag = &Punigp->targets[index].flags; // PuniWork target table

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
    PuniTarget* target = &Punigp->targets[index]; // PuniWork target table

    target->position.x = (f32)evtGetValue(event, args[1]);
    target->position.y = (f32)evtGetValue(event, args[2]);
    target->position.z = (f32)evtGetValue(event, args[3]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_target_set_soko) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    PuniTarget* target = &Punigp->targets[index]; // PuniWork target table

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
        Punigp->targets[i].flags &= ~5; // PuniWork 0x374 target table
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
        if (50.0 >= __fabs(pos.y - Punigp->levelY[0])) {
            hit = 1;
            pos.y = Punigp->levelY[0];
        }
        if (50.0 >= __fabs(pos.y - Punigp->levelY[1])) {
            hit = 1;
            pos.y = Punigp->levelY[1];
        }
        if (50.0 >= __fabs(pos.y - Punigp->levelY[2])) {
            hit = 1;
            pos.y = Punigp->levelY[2];
        }
        if (50.0 >= __fabs(pos.y - Punigp->levelY[3])) {
            hit = 1;
            pos.y = Punigp->levelY[3];
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

    if (Punigp->area == 9 || Punigp->area == 12) {
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
        if ((puni->flags & 0x400) == 0) {
            continue;
        }
        if ((puni->flags & 0x804000) != 0) {
            continue;
        }
        if ((puni->flags & 0x80) != 0) {
            continue;
        }
        if (puni->mode == 0x10 || puni->mode == 0xf) {
            continue;
        }
        count++;
    }

    total = swGet(0xB3C);
    total += swGet(0xB3D);
    total += swGet(0xB3F);
    total += swByteGet(0x28B);

    isKassen = 0;
    if (Punigp->area == 9) {
        if (swGet(0xB21) == 0) {
            isKassen = 1;
        }
    }
    if (Punigp->area == 12) {
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

    if ((Punigp->systemMode & 2) == 0) {
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

