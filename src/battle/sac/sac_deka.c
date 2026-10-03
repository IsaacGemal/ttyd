#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "battle/battle_disp.h"
#include "battle/battle_audience.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/mapdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include <string.h>
#include "math_ext.h"
#include "pmario_sound.h"
#include "eff/eff_starstone.h"
#include "eff/eff_small_star.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/sac/sac_deka.h"
#include "battle/sac/sac_genki.h"
#include <stdlib.h>
static struct DekaWork* get_ptr(void);
static void disp_2D(CameraId cameraId, void* param);  // local to this file in the original
static void main_star(void);  // local to this file in the original
static void main_base(void);  // local to this file in the original
static void disp_3D_alpha(CameraId cameraId, void* param);  // local to this file in the original
static void disp_3D(CameraId cameraId, void* param);  // local to this file in the original
static void yuka_main(void);  // local to this file in the original
static void yuka_init(void);  // local to this file in the original

// LOCAL_PROTO(psndSFX_pit): retail callers disagree on pitch width and signedness; the shared u16 signature regresses matched sound commands.
// main_base is this file's only caller and passes a full int (no extsh in its asm), so it uses the s32 width.
void psndSFX_pit(s32 id, s32 pitch);


// The original sac_deka unit was compiled with -O4,s (optimize for size): the
// prologues/epilogues use stmw/lmw, which -O4,p never emits. There is no
// per-Object flag for this unit, so set it here.
#pragma optimize_for_size on

// ---- local declarations (no header yet) ----

// ---- local work struct at _battleWorkPointer + 0x1F4C ----
typedef struct DekaTiming {
    /*0x00*/ s32 state;
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ f32 time;
    /*0x10*/ Vec unk10;
    /*0x1C*/ u8 pad1C[0x34 - 0x1C];
    /*0x34*/ u8 alpha;
    /*0x35*/ u8 flag;
    /*0x36*/ u8 pad36[0x38 - 0x36];
} DekaTiming;

typedef struct DekaStarEffectData {
    /*0x00*/ u8 pad00[0x8];
    /*0x08*/ Vec position;
    /*0x14*/ f32 unk14;
    /*0x18*/ Vec unk18;
    /*0x24*/ u8 pad24[0x34 - 0x24];
    /*0x34*/ void* unk34;
    /*0x38*/ u8 pad38[0x5F - 0x38];
    /*0x5F*/ u8 unk5F;
} DekaStarEffectData;

typedef struct DekaWork {
    /*0x000*/ smartEntry* texEntry;
    /*0x004*/ Vec plane0;
    /*0x010*/ Vec plane1;
    /*0x01C*/ s32 width;
    /*0x020*/ s32 height;
    /*0x024*/ s32 texWidth;
    /*0x028*/ s32 texHeight;
    /*0x02C*/ smartEntry* matEntry;
    /*0x030*/ s32 baseState;
    /*0x034*/ s32 baseTimer;
    /*0x038*/ f32 beatPeriod;
    /*0x03C*/ f32 beatPos;
    /*0x040*/ s32 beatCount;
    /*0x044*/ s32 combo;
    /*0x048*/ s32 unk48;
    /*0x04C*/ f32 unk4C;
    /*0x050*/ s32 gaugeState;
    /*0x054*/ s32 gaugeTimer;
    /*0x058*/ f32 gaugeWidth;
    /*0x05C*/ Vec gaugePos;
    /*0x068*/ u8 gaugeAlpha;
    /*0x069*/ u8 pad069[0x6C - 0x69];
    /*0x06C*/ s32 sndHandle;
    /*0x070*/ DekaTiming timing[8];
    /*0x230*/ DekaTiming timingCopy[8];
    /*0x3F0*/ s32 starState;
    /*0x3F4*/ s32 starTimer;
    /*0x3F8*/ s32 starCounter;
    /*0x3FC*/ Vec starPos;
    /*0x408*/ Vec starPrev;
    /*0x414*/ Vec starTarget;
    /*0x420*/ u8 pad420[0x424 - 0x420];
    /*0x424*/ f32 starVel;
    /*0x428*/ u8 pad428[0x42C - 0x428];
    /*0x42C*/ Vec starScale;
    /*0x438*/ Vec starRot;
    /*0x444*/ f32 starUnk44;
    /*0x448*/ f32 starUnk48;
    /*0x44C*/ EffectEntry* starEffect;
    /*0x450*/ s32 unitIds[10];
    /*0x478*/ struct { f32 x, y, z; } enemyVel[10];
    /*0x4F0*/ f32 enemyUnk[10];
    /*0x518*/ u8 yukaActive[6];
    /*0x51E*/ u8 pad51E[0x520 - 0x51E];
    /*0x520*/ f32 yukaPos[6];
    /*0x538*/ s32 yukaIndex[6];
    /*0x550*/ f32 yukaScale[6];
    /*0x568*/ f32 yukaUnk[6];
} DekaWork;

static DekaWork* get_ptr(void);

// Small rodata tables main_star copies to the stack (the original inlines the copy).
typedef struct { s32 v[11]; } DekaTableInt11;
typedef struct { f32 v[6]; } DekaTableF32x6;
// .rodata in retail order.
const Vec vec3_803004f8[3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
__declspec(section ".rodata") Vec vec3_8030051c[1] = { { 0, 0, 0 } };
__declspec(section ".rodata") Vec vec3_80300528 = { -1000.0f, 100.0f, 0.0f };
const Vec vec3_80300534 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_80300540 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_8030054c = { 0.0f, 0.0f, 0.0f };
const DekaTableInt11 dat_80300558 = { { 1, 2, 0, 2, 3, 1, 3, 4, 2, 4, 5 } };
const DekaTableInt11 dat_80300584 = { { 1, 2, 0, 2, 3, 1, 3, 4, 2, 4, 5 } };
const DekaTableF32x6 dat_803005b0 = { { 1.0f, 1.3f, 2.0f, 2.6f, 3.3f, 3.8f } };
const DekaTableF32x6 dat_803005c8 = { { 1.0f, 1.3f, 2.0f, 2.6f, 3.3f, 3.8f } };
const Vec vec3_803005e0 = { 0.0f, 0.0f, -1.0f };
const char str_803005EC[0x14] = "SFX_BTL_SAC_START1";
const char str_80300600[0x14] = "SFX_BTL_SAC_START2";
const char str_80300614[0x14] = "SFX_BTL_SAC_START3";

// Vertex data allocated by yuka_init into matEntry->address (0x3870 bytes).
typedef struct DekaYukaMesh {
    /*0x0000*/ Vec pos[2][0x81];
    /*0x0C18*/ Vec disp[2][0x81];
    /*0x1830*/ Vec normal[2][0x81];
    /*0x2448*/ Vec prev[2][0x81];
    /*0x3060*/ f32 texU[2][0x81];
    /*0x3468*/ f32 texV[2][0x81];
} DekaYukaMesh;

// EVT script, local to this file in the original (.sdata).
static s32 nice_event[8] = { 0x0005005B, (s32)btlevtcmd_ACSuccessEffect, 0xFE363C80, 0, 0x64, 0, 2, 1 };

// main_star's .rodata objects (not in a header yet).

GXColor dat_80421e40 = { 0xFF, 0xFF, 0xFF, 0x00 };
GXColor dat_80421e44 = { 0xFF, 0xFF, 0xFF, 0x00 };
GXColor dat_80421e48 = { 0xFF, 0xFF, 0xFF, 0x00 };

// .sdata2 constants (names from the original object; used by name so the relocations match)
f32 float_0p5_80421e54 = 0.5f;
f32 float_0p25_80421e58 = 0.25f;
f32 float_64_80421e5c = 64.0f;
f32 float_5_80421e60 = 5.0f;
f32 float_4_80421e64 = 4.0f;
extern const f32 float_40_80421e68;
extern const f32 float_32767_80421e6c;
extern const f32 float_30_80421e70;
extern const f32 float_20_80421e74;
extern const f32 float_7_80421e78;
extern const f32 float_2_80421e7c;
extern const f32 float_70_80421e80;
extern const f32 float_25_80421e84;
extern const f32 float_1_80421e88;
extern const f32 float_0_80421e8c;
extern const f32 float_8_80421e90;
extern const f32 float_neg1_80421e94;
extern const f32 float_255_80421e98;
extern const f32 float_0p75_80421e9c;
extern const f32 float_18_80421ea0;
extern const f32 float_1p25_80421ea4;
extern const f32 float_250_80421ea8;
extern const f32 float_0p0078125_80421eac;
extern const f32 float_120_80421eb0;
extern const f32 float_neg1000_80421eb4;
extern const f32 float_37_80421eb8;
extern const f32 float_50_80421ebc;
extern const f32 float_2160_80421ec0;
extern const f32 float_neg100_80421ec4;
extern const f32 float_12p5_80421ec8;
extern const f32 float_neg0p5_80421ecc;
extern const f32 float_43_80421ed0;
extern const f32 float_1p5_80421ed4;
extern const f32 float_500_80421ed8;
extern const f32 float_180_80421edc;
extern const f32 float_0p8_80421ee0;
extern const f32 float_0p1_80421ee4;
extern const f32 float_683_80421ee8;
extern const f32 float_4096_80421eec;
extern const f32 float_neg40_80421ef0;
extern const f32 float_0p01_80421ef4;
extern const f32 float_901_80421ef8;


#ifdef MUST_MATCH
// The original builds each GXColor in a helper that returns it by value: the
// helper's result lands in one stack slot and the assignment copies it to the
// variable the call then passes. The plain form under #else is equivalent.
static inline GXColor disp2D_color(GXColor base, u8 alpha) {
    base.a = alpha;
    return base;
}
#endif




// .text:0x2CD0 | 0x80245CC8 | size: 0x130
s32 main_deka(EventEntry* event, s32 arg) {
    DekaWork* p = get_ptr();
    s32* args = event->args;

    if (arg != 0) {
        memset(p, 0, 0x580);
        p->unk48 = evtGetValue(event, args[0]);
        yuka_init();
    }
    if (p->texEntry->address == NULL) {
        return 2;
    }
    if (p->matEntry->address == NULL) {
        return 2;
    }
    main_base();
    main_gauge();
    for (arg = 0; arg < 8; arg++) {
        main_timing(arg);
    }
    main_star();
    main_enemy();
    yuka_main();
    dispEntry(CAMERA_2D, float_901_80421ef8, 1, disp_2D, NULL);
    dispEntry(CAMERA_3D, float_0_80421e8c, 1, disp_3D, NULL);
    dispEntry(CAMERA_3D, float_0_80421e8c, 2, disp_3D_alpha, NULL);
    dispEntry(CAMERA_3D, float_0_80421e8c, 0, yuka_disp, NULL);
    return 0;
}

// .text:0x29F8 | 0x802459F0 | size: 0x2D8
// Every instruction matches except the compiler's int->float magic constant: the original
// pools `double_to_int_80300630` off `r30=&vec3_803004f8` (`lfd f6, 0x138(r30)`), ours emits
// an anonymous `@735` with its own `lis` because `vec3_803004f8` is only an extern. Using two
// separate Vec locals (`v`, `v2`) instead of reusing one was the key to the frame/register
// layout (86.59 -> 98.18).
// NONMATCHING (98.18%): only that unpolled magic constant differs.
static void yuka_init(void) {
#ifdef MUST_MATCH
    const u8* base = (const u8*)&vec3_803004f8;
#endif
    DekaWork* p = get_ptr();
#ifdef MUST_MATCH
    MapObject* obj = mapGetMapObj((const char*)(base + 0x1B8));
#else
    MapObject* obj = mapGetMapObj("stg_b_yuka");
#endif
    DekaYukaMesh* mesh;
    s32 i, j;

#ifdef MUST_MATCH
    mapGrpFlagOn((const char*)(base + 0x1B8), 0x400001);
#else
    mapGrpFlagOn("stg_b_yuka", 0x400001);
#endif

    PSMTXMultVec(obj->modelWorldMtx, &obj->joints->bboxMin, &p->plane0);
    PSMTXMultVec(obj->modelWorldMtx, &obj->joints->bboxMax, &p->plane1);

    p->width = p->texWidth = (s32)(p->plane1.x - p->plane0.x);
    p->height = p->texHeight = (s32)(p->plane1.z - p->plane0.z);

    p->texWidth += p->texWidth & 1;
    p->texHeight += p->texHeight & 1;

    if (p->texWidth > 0x260) {
        p->texWidth = 0x260;
    }
    if (p->texHeight > 0x1E0) {
        p->texHeight = 0x1E0;
    }

    p->texEntry = smartAlloc(GXGetTexBufferSize(p->texWidth, p->texHeight, 4, 0, 0), 2);
    p->matEntry = smartAlloc(0x3870, 2);

    mesh = p->matEntry->address;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 0x81; j++) {
            Vec v, v2;
            f32 x = p->plane0.x + (f32)p->width * float_0p0078125_80421eac * (f32)j;
            f32 z = p->plane0.z + (f32)p->height * (f32)i;

#ifdef MUST_MATCH
            v = *(Vec*)(base + 0x00);
            v.x = x;
            v.z = z;
            mesh->disp[i][j] = v;
            v2 = *(Vec*)(base + 0x0C);
            v2.x = x;
            v2.z = z;
            mesh->pos[i][j] = v2;
            mesh->normal[i][j] = *(Vec*)(base + 0x18);
#else
            v = vec3_803004f8[0];
            v.x = x;
            v.z = z;
            mesh->disp[i][j] = v;
            v2 = vec3_803004f8[1];
            v2.x = x;
            v2.z = z;
            mesh->pos[i][j] = v2;
            mesh->normal[i][j] = vec3_803004f8[2];
#endif
            mesh->texU[i][j] = float_0p0078125_80421eac * (f32)j;
            mesh->texV[i][j] = (f32)i;
        }
    }

    dispEntry(CAMERA_SHADOW, float_0_80421e8c, 0, (DispCallback)yuka_capture, NULL);
}

// .text:0x2664 | 0x80245664 | size: 0x38C
static void yuka_main(void) {
    DekaWork* p = get_ptr();
    DekaYukaMesh* mesh = p->matEntry->address;
    s32 i, j;
    Vec v2;
    Vec vA;
    Vec vN;
    Vec v8;
    f32 f;

    for (i = 0; i < 6; i++) {
        if (p->yukaActive[i] == 1) {
            p->yukaScale[i] += p->yukaPos[i];
            for (;;) {
                s32 idx = p->yukaIndex[i];
                s32 sc = (s32)p->yukaScale[i];

                if ((sc > idx && p->yukaPos[i] > float_0_80421e8c) ||
                    (sc < idx && p->yukaPos[i] < float_0_80421e8c)) {
                    for (j = 0; j < 2; j++) {
                        mesh->normal[j][p->yukaIndex[i]].y += p->yukaUnk[i];
                    }
                    if (p->yukaPos[i] > float_0_80421e8c) {
                        p->yukaIndex[i]++;
                    }
                    else {
                        p->yukaIndex[i]--;
                    }
                    if (p->yukaIndex[i] <= 0 || p->yukaIndex[i] >= 0x80) {
                        p->yukaActive[i] = 0;
                        break;
                    }
                }
                else {
                    break;
                }
            }
        }
    }

    memcpy_as4(mesh->prev, mesh->pos, 0xC18);

    {
#ifdef MUST_MATCH
        // Register allocation: the original assigns the callee-saved FP registers
        // in reverse declaration order, so declare two/zero/one and initialise
        // one/zero/two to get f29=one, f30=zero, f31=two.
        f32 two, zero, one;
        one = float_1_80421e88;
        zero = float_0_80421e8c;
        two = float_2_80421e7c;
#else
        const f32 one = float_1_80421e88;
        const f32 zero = float_0_80421e8c;
        const f32 two = float_2_80421e7c;
#endif

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 0x81; j++) {
            v2 = vec3_8030051c[0];

            PSVECSubtract(&mesh->prev[i][j], &mesh->prev[i][j + 1], &vA);
            PSVECNormalize(&vA, &vN);
            f = one - PSVECMag(&vA);
            PSVECScale(&vN, &vN, f);
            PSVECAdd(&v2, &vN, &v2);

            PSVECSubtract(&mesh->prev[i][j], &mesh->prev[i][j - 1], &vA);
            PSVECNormalize(&vA, &vN);
            f = one - PSVECMag(&vA);
            PSVECScale(&vN, &vN, f);
            PSVECAdd(&v2, &vN, &v2);

            v2.x = zero;
            v2.z = zero;
            PSVECScale(&v2, &v2, float_0p01_80421ef4);

            PSVECAdd(&mesh->normal[i][j], &v2, &mesh->normal[i][j]);
            PSVECScale(&mesh->normal[i][j], &v8, float_0p1_80421ee4);
            PSVECAdd(&mesh->pos[i][j], &v8, &mesh->pos[i][j]);

            mesh->pos[i][j].y += mesh->normal[i][j].y;
            mesh->normal[i][j].y -= two;
            if (mesh->pos[i][j].y <= zero) {
                mesh->pos[i][j].y = zero;
                mesh->normal[i][j].y = zero;
            }
        }
    }
    }

#ifdef MUST_MATCH
    {
        // float_0p5_80421e54 is not const in this file, so copy it to a local to
        // keep its load out of the inner loop, as the original does.
        f32 half = float_0p5_80421e54;

        for (i = 0; i < 2; i++) {
            for (j = 1; j < 0x80; j++) {
                mesh->pos[i][j].y = half * (mesh->pos[i][j - 1].y + mesh->pos[i][j + 1].y);
            }
        }
    }
#else
    for (i = 0; i < 2; i++) {
        for (j = 1; j < 0x80; j++) {
            mesh->pos[i][j].y = float_0p5_80421e54 * (mesh->pos[i][j - 1].y + mesh->pos[i][j + 1].y);
        }
    }
#endif
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 0x81; j++) {
            mesh->disp[i][j].x = mesh->pos[i][j].x;
            mesh->disp[i][j].y = mesh->pos[i][j].y;
            mesh->disp[i][j].z = mesh->pos[i][j].z;
        }
    }
}

// .text:0x2600 | 0x802455F8 | size: 0x6C
s32 yuka_end(EventEntry* event) {
    DekaWork* p = get_ptr();

    smartFree(p->texEntry);
    p->texEntry->address = NULL;
    smartFree(p->matEntry);
    p->matEntry->address = NULL;
    mapGrpFlagOff("stg_b_yuka", 0x400001);
    return 2;
}

// .text:0x8C8 | 0x80245120 | size: 0x4D8
// The original addresses main_base's .rodata objects (the DOKKAN strings and the
// int->double constant) as offsets from vec3_803004f8, held in r30. Under MUST_MATCH
// reuse party_hit.c's RO trick to force that base; the natural form is under #else.
#ifdef MUST_MATCH
#define RO(base, off, sym) ((const char*)(base) + (off))
#else
#define RO(base, off, sym) ((const char*)&(sym))
#endif
// The int->double magic constant is a local @660 instead of the pooled
// double_to_int_80300630 at 0x138(r30) (one extra lis; the file's .rodata pool is
// incomplete), and the third loop holds 7 in f2 and beatPos in f1 where the original
// holds 7 in f1 and beatPos in f2 (same instructions, swapped f-registers).
// NONMATCHING (99.60%): only those two register-allocation differences remain.
static void main_base(void) {
    const u8* base = (const u8*)&vec3_803004f8;
    DekaWork* p = get_ptr();
    s32 idx[8];
    f32 times[8];
    s32 i, j;
    s32 found;
    f32 tf;
    s32 ti;

    switch (p->baseState) {
    case 1:
        p->baseState = 2;
        p->beatCount = 1;
        p->combo = 0;
        p->beatPeriod = float_180_80421edc;
        p->beatPos = float_0_80421e8c;
        create_timing();
        p->gaugeState = 1;
        /* fallthrough */
    case 2:
        if (p->gaugeState != 3) {
            break;
        }
        p->baseState = 3;
        p->sndHandle = psndSFXOn("SFX_BTL_SAC_DOKKAN1_1");
        /* fallthrough */
    case 3:
        if (p->gaugeState == 3) {
            p->beatPos += float_0p8_80421ee0 + float_0p1_80421ee4 * (f32)p->combo;
            psndSFX_pit(p->sndHandle,
                (s32)(float_683_80421ee8 * (f32)p->combo + float_4096_80421eec * p->beatPos / p->beatPeriod));
            if (p->beatPos >= p->beatPeriod) {
                p->beatPos = float_0_80421e8c;
                p->beatCount++;
                p->combo++;
                p->starState = 6;
                psndSFXOff(p->sndHandle);
                {
                    EventEntry* e = evtEntry(nice_event, 0, 0x20);
                    s32 n = p->combo + 1;
                    e->lwData[0] = n < 6 ? n : 6;
                }
                if (p->beatCount > 5) {
                    p->baseState = 5;
                    p->gaugeState = 5;
                }
                else {
                    p->baseState = 4;
                    p->baseTimer = 30;
                    p->unk4C = float_neg40_80421ef0;
                    create_timing();
                }
            }
        }
        for (i = 0; i < 8; i++) {
            idx[i] = i;
            times[i] = p->timing[i].time;
        }
        for (i = 0; i < 7; i++) {
            for (j = i + 1; j < 8; j++) {
                tf = times[i];
                if (tf > times[j]) {
                    ti = idx[i];
                    times[i] = times[j];
                    idx[i] = idx[j];
                    idx[j] = ti;
                    times[j] = tf;
                }
            }
        }

        if (keyGetButtonTrg(0) & 0x100) {
            for (i = 0; i < 8; i++) {
                DekaTiming* t = &p->timing[idx[i]];
                if (t->state == 2 && p->beatPos >= t->time - float_7_80421e78 &&
                    p->beatPos <= float_7_80421e78 + t->time) {
                    if (t->unk08 == 1) {
                        t->state = 3;
                        t->flag = 1;
                        psndSFXOn("SFX_BTL_SAC_DOKKAN2");
                        BattleAudienceSoundHandBeat();
                    }
                    else {
                        p->combo = 0;
                        p->baseState = 5;
                        p->gaugeState = 4;
                        p->starState = 8;
                        t->flag = 2;
                        psndSFXOn("SFX_BTL_SAC_DOKKAN3");
                        BattleAudienceSoundBooing();
                        psndSFXOff(p->sndHandle);
                    }
                    break;
                }
            }
            if (i == 8) {
                found = 0;
                for (i = 0; i < 8; i++) {
                    DekaTiming* t = &p->timing[idx[i]];
                    if (t->state == 2 && t->unk08 == 1) {
                        p->baseState = 5;
                        p->gaugeState = 4;
                        t->flag = 2;
                        psndSFXOn("SFX_BTL_SAC_DOKKAN3");
                        BattleAudienceSoundBooing();
                        psndSFXOff(p->sndHandle);
                        found = 1;
                        break;
                    }
                }
                if (found == 1) {
                    return;
                }
            }
        }

        for (i = 0; i < 8; i++) {
            DekaTiming* t = &p->timing[idx[i]];
            if (t->state == 2 && t->unk08 != 2 && p->beatPos > float_7_80421e78 + t->time) {
                p->baseState = 5;
                p->gaugeState = 4;
                t->flag = 2;
                psndSFXOn("SFX_BTL_SAC_DOKKAN3");
                BattleAudienceSoundBooing();
                psndSFXOff(p->sndHandle);
                break;
            }
        }
        break;
    case 4:
        p->baseTimer--;
        p->unk4C = intplGetValue(0, 30 - p->baseTimer, 30, float_neg40_80421ef0, float_0_80421e8c);
        if (p->baseTimer <= 0) {
            p->baseState = 3;
            p->sndHandle = psndSFXOn("SFX_BTL_SAC_DOKKAN1_1");
        }
        break;
    case 5:
    default:
        break;
    }
}

// .text:0x1F98 | 0x80244F90 | size: 0x190
void main_gauge(void) {
    DekaWork* p = get_ptr();

    switch (p->gaugeState) {
        case 0:
            break;
        case 1:
            p->gaugeState = 2;
            p->gaugeTimer = 0;
            p->gaugePos = vec3_80300528;
            p->gaugeWidth = float_500_80421ed8;
            p->gaugeAlpha = 0xFF;
            // fallthrough
        case 2:
            p->gaugeTimer++;
            p->gaugePos.x = intplGetValue(4, p->gaugeTimer, 30, float_neg1000_80421eb4, float_0_80421e8c);
            if (p->gaugeTimer >= 30) {
                p->gaugeState = 3;
                p->gaugeTimer = 0;
            }
            break;
        case 3:
            p->gaugeTimer = 0;
            break;
        case 4: {
            s32 r;
            p->gaugeTimer++;
            r = p->gaugeTimer % 12;
            if (r >= 0 && r <= 5) {
                p->gaugeAlpha = 0;
            }
            else {
                p->gaugeAlpha = 0xFF;
            }
            if (p->gaugeTimer >= 0x50) {
                p->gaugeState = 5;
                p->gaugeTimer = 0;
                p->gaugeAlpha = 0xFF;
            }
            break;
        }
        case 5:
            p->gaugeTimer++;
            p->gaugePos.x = intplGetValue(4, p->gaugeTimer, 30, float_0_80421e8c, float_neg1000_80421eb4);
            if (p->gaugeTimer >= 30) {
                p->gaugeState = 3;
                p->gaugeTimer = 0;
            }
            break;
        case 6:
            break;
    }
}

#pragma dont_inline on
// .text:0x1EE0 | 0x80244ED8 | size: 0xB8
// NONMATCHING (85.22%): 1 instruction differs, 3 missing, 6 extra, plus register differences.
void main_timing(s32 idx) {
    DekaWork* p = get_ptr();
    DekaTiming* t = &p->timing[idx];

    switch (t->state) {
        case 0:
            t->unk08 = 0;
            break;
        case 2:
            t->alpha = 0xFF;
            t->unk10 = vec3_80300534;
            break;
        case 3:
            t->state = 4;
            t->alpha = 0xFF;
            t->unk04 = 0;
            break;
        case 1:
        case 4:
        case 5:
            break;
    }
}
#pragma dont_inline reset

#pragma dont_inline on
// .text:0x1640 | 0x80244638 | size: 0x89C
// The algorithm and nearly all instructions match. The rest is the unit's incomplete
// .rodata pool: the original resolves its int->float magic constant
// (double_to_int_80300630) as vec3_803004f8+0x138, while ours must materialise an
// anonymous @NNN constant with its own lis; that extra base register also shifts a few
// GPRs around it (state 1, and the state-11 camera-shake / yukaUnk / velocity block).
// Same root cause as main_base and yuka_init. A few register/scheduling diffs remain in
// the state-10 setup.
// NONMATCHING (96.59%): the anonymous int->float magic and its register fallout.
static void main_star(void) {
#ifdef MUST_MATCH
    const u8* base = (const u8*)&vec3_803004f8;
#endif
    DekaWork* p = get_ptr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    s32 audience = BattleAudience_GetAudienceNum();
    s32 alpha = 0xFF;

    switch (p->starState) {
    case 1:
        p->starState = 2;
        p->starTimer = 0;
        p->starEffect = effStarStoneEntry(0, float_0_80421e8c, float_neg1000_80421eb4, float_0_80421e8c, float_1_80421e88);
        BtlUnit_GetPos(mario, &p->starPos.x, &p->starPos.y, &p->starPos.z);
        p->starPos.y += mario->mSizeMultiplier * (f32)mario->mHeight + float_37_80421eb8;
        p->starPrev = p->starPos;
        p->starTarget = p->starPos;
        p->starTarget.y += float_50_80421ebc;
        p->starTarget.z += float_neg1_80421e94;
#ifdef MUST_MATCH
        p->starScale = *(Vec*)(base + 0x48);
        p->starRot = *(Vec*)(base + 0x54);
#else
        p->starScale = vec3_80300540;
        p->starRot = vec3_8030054c;
#endif
        /* fallthrough */
    case 2:
        p->starTimer++;
        if (p->starTimer <= 100) {
            p->starPos.x = intplGetValue(0, p->starTimer, 100, p->starPrev.x, p->starTarget.x);
            p->starPos.y = intplGetValue(0, p->starTimer, 100, p->starPrev.y, p->starTarget.y);
            p->starPos.z = intplGetValue(0, p->starTimer, 100, p->starPrev.z, p->starTarget.z);
            {
                f32 s = intplGetValue(0, p->starTimer, 100, float_0_80421e8c, float_2_80421e7c);
                p->starScale.z = s;
                p->starScale.y = s;
                p->starScale.x = s;
            }
        } else {
            p->starPos.x = p->starTarget.x;
            p->starPos.y = p->starTarget.y;
            p->starPos.z = p->starTarget.z;
            p->starScale.z = float_2_80421e7c;
            p->starScale.y = float_2_80421e7c;
            p->starScale.x = float_2_80421e7c;
        }
        p->starRot.y = intplGetValue(4, p->starTimer, 120, float_0_80421e8c, float_2160_80421ec0);
        if (p->starTimer >= 120) {
            p->starState = 3;
            p->starTimer = 0;
            p->starPrev = p->starPos;
            p->starTarget.x = float_neg100_80421ec4;
            p->starTarget.y = float_30_80421e70;
            p->starTarget.z = float_50_80421ebc;
        }
        break;
    case 3:
        p->starTimer++;
        p->starPos.x = intplGetValue(4, p->starTimer, 60, p->starPrev.x, p->starTarget.x);
        p->starPos.y = intplGetValue(4, p->starTimer, 60, p->starPrev.y, p->starTarget.y);
        p->starPos.z = intplGetValue(4, p->starTimer, 60, p->starPrev.z, p->starTarget.z);
        if (p->starTimer >= 60) {
            p->starState = 4;
            p->starTimer = 0;
        }
        break;
    case 6:
        p->starState = 7;
        p->starTimer = 0;
        p->starUnk44 = p->starUnk48 = p->starScale.x;
        p->starUnk48 += float_1_80421e88;
#ifdef MUST_MATCH
        psndSFXOn((const char*)(base + 0x150));
#else
        psndSFXOn("SFX_BTL_SAC_DOKKAN4");
#endif
        if (audience >= 1 && audience <= 0x31) {
            BattleAudienceSoundCallKind(1);
        }
        if (audience >= 0x32 && audience <= 0x63) {
            BattleAudienceSoundCallKind(1);
            BattleAudienceSoundWhistleKind(1);
        }
        if (audience >= 0x64 && audience <= 0x95) {
            BattleAudienceSoundCallKind(2);
            BattleAudienceSoundWhistleKind(2);
        }
        if (audience >= 0x96) {
            BattleAudienceSoundCallKind(2);
            BattleAudienceSoundWhistleKind(3);
        }
        /* fallthrough */
    case 7: {
        DekaTableInt11 tbl;
        f32 s;
#ifdef MUST_MATCH
        tbl = *(const DekaTableInt11*)(base + 0x60);
#else
        tbl = dat_80300558;
#endif
        p->starTimer++;
        s = intplGetValue(4, tbl.v[p->starTimer / 4], 5, p->starUnk44, p->starUnk48);
        p->starScale.z = s;
        p->starScale.y = s;
        p->starScale.x = s;
        p->starPos.y = float_12p5_80421ec8 * (p->starScale.y - float_2_80421e7c) + float_30_80421e70;
        if (p->starTimer >= 0x2B) {
            p->starState = 5;
            p->starTimer = 0;
            p->starScale.x = p->starScale.y = p->starScale.z = p->starUnk48;
        }
        break;
    }
    case 8:
        p->starState = 9;
        p->starTimer = 0;
        p->starUnk44 = p->starUnk48 = p->starScale.x;
        p->starUnk48 = float_2_80421e7c;
        /* fallthrough */
    case 9: {
        DekaTableInt11 tbl;
        f32 s;
#ifdef MUST_MATCH
        tbl = *(const DekaTableInt11*)(base + 0x8c);
#else
        tbl = dat_80300584;
#endif
        p->starTimer++;
        s = intplGetValue(4, tbl.v[p->starTimer / 4], 5, p->starUnk44, p->starUnk48);
        p->starScale.z = s;
        p->starScale.y = s;
        p->starScale.x = s;
        p->starPos.y = float_12p5_80421ec8 * (p->starScale.y - float_2_80421e7c) + float_30_80421e70;
        if (p->starTimer >= 0x2B) {
            p->starState = 5;
            p->starTimer = 0;
            p->starScale.x = p->starScale.y = p->starScale.z = p->starUnk48;
        }
        break;
    }
    case 10:
        p->starState = 11;
        p->starTarget.y = p->starPos.y;
        p->starVel = float_5_80421e60;
        p->starTimer = 0;
        p->starCounter = 0;
        /* fallthrough */
    case 11: {
        DekaTableF32x6 tblA;
        DekaTableF32x6 tblB;
        s32 j;

        p->starPos.y = p->starPos.y + p->starVel;
        p->starVel = p->starVel + float_neg0p5_80421ecc;
        if (p->starPos.y < p->starTarget.y) {
#ifdef MUST_MATCH
            tblA = *(const DekaTableF32x6*)(base + 0xb8);
            tblB = *(const DekaTableF32x6*)(base + 0xd0);
#else
            tblA = dat_803005b0;
            tblB = dat_803005c8;
#endif
            p->starCounter++;
#ifdef MUST_MATCH
            btl_camera_shake_h(0, 20, 0, (f32)p->starCounter * tblA.v[p->combo], float_0_80421e8c);
            psndSFXOn((const char*)(base + 0x164));
#else
            btl_camera_shake_h(0, 20, 0, (f32)p->starCounter * tblA.v[p->combo], float_0_80421e8c);
            psndSFXOn("SFX_BTL_SAC_DOKKAN5");
#endif
            for (j = 0; j < 2; j++) {
                s32 i;
                for (i = 0; i < 6; i++) {
                    if (p->yukaActive[i] == 0) {
                        p->yukaActive[i] = 1;
                        p->yukaPos[i] = float_2_80421e7c;
                        if (j == 1) {
                            p->yukaPos[i] *= float_neg1_80421e94;
                        }
                        if (j == 0) {
                            p->yukaIndex[i] = 0x2A;
                        } else {
                            p->yukaIndex[i] = 0x29;
                        }
                        p->yukaScale[i] = float_43_80421ed0;
                        p->yukaUnk[i] = (f32)p->starCounter * tblB.v[p->combo] + float_4_80421e64;
                        break;
                    }
                }
            }
            if (p->starCounter >= 6) {
                p->starState = 12;
                p->starTimer = 30;
                p->starTarget = p->starPos;
                p->starPrev = p->starTarget;
                p->starTarget.y += float_40_80421e68;
            } else {
                f32 x = float_4_80421e64 * (f32)p->starCounter + float_20_80421e74;
                f32 half = float_0p5_80421e54;

                p->starPos.y = p->starTarget.y;
                p->starVel = half * x * x / (float_2_80421e7c * x);
            }
        }
        break;
    }
    case 12:
        p->starTimer--;
        p->starPos.y = intplGetValue(4, 30 - p->starTimer, 30, p->starPrev.y, p->starTarget.y);
        if (p->starTimer <= 0) {
            p->starState = 13;
            p->starTimer = 0;
        }
        break;
    case 13:
        p->starTimer++;
        if (p->starTimer > 30) {
            p->starTimer = 30;
        }
        alpha = (s32)intplGetValue(0, p->starTimer, 30, float_255_80421e98, float_0_80421e8c);
        break;
    default:
        break;
    }

    if (p->starEffect != NULL) {
        DekaStarEffectData* d = p->starEffect->userdata;

        d->position.x = p->starPos.x;
        d->position.y = p->starPos.y;
        d->position.z = p->starPos.z;
        d->unk18.x = p->starRot.x;
        d->unk18.y = p->starRot.y;
        d->unk18.z = p->starRot.z;
        d->unk14 = float_1p5_80421ed4 * p->starScale.x;
        d->unk5F = (u8)alpha;
    }
}
#pragma dont_inline reset

#pragma dont_inline on
// main_enemy is kept out of line: main_deka calls it rather than inlining it.
// .text:0x15A4 | 0x8024459C | size: 0x9C
void main_enemy(void) {
    BattleWork* work = _battleWorkPointer;
    DekaWork* p = get_ptr();
    s32 count = 0;
    s32 i;
    s32 j;

    for (i = 0; i < 0x40; i++) {
        BattleUnit* unit = BattleGetUnitPtr(work, i);
        if (unit != NULL && unit->mAlliance == 1) {
            main_enemy_sub(unit, &count);
        }
    }
    for (j = count; j < 10; j++) {
        p->unitIds[j] = -1;
    }
}
#pragma dont_inline reset

#pragma dont_inline on
// .text:0x11A0 | 0x80244198 | size: 0x404
// The int->float magic this function loads is the compiler's own pool entry (an anonymous
// @NNN local), not double_to_int_80300630; objdiff ignores that relocation, so the only real
// differences were FPR allocation. Two pragmas fix them: with ordinary CSE the opening
// velocity/clamp constants land swapped in f0/f1/f2, and with propagation on the final
// mPosition.y/mHomePosition.y loads are scheduled in the other order.
#pragma opt_common_subs off
#pragma opt_propagation off
void main_enemy_sub(BattleUnit* unit, s32* count) {
    DekaWork* p = get_ptr();
    DekaYukaMesh* mesh = p->matEntry->address;
    f32 x;
    f32 cx;
    s32 j;

    unit->mRotationOffset = unit->offsetCenter;
    p->enemyVel[*count].y -= float_1p25_80421ea4;

    x = unit->mPosition.x;
    if (x >= float_250_80421ea8) {
        cx = float_250_80421ea8;
    }
    else if (x <= float_0_80421e8c) {
        cx = float_0_80421e8c;
    }
    else {
        cx = x;
    }

    for (j = 0x40; j < 0x81; j++) {
        f32 step = (f32)p->width * float_0p0078125_80421eac;
        f32 xj = p->plane0.x + step * (f32)j;

        if (cx >= xj) {
            if (cx < xj + step) {
#ifdef MUST_MATCH
                // Match trick: the original factors the row base mesh + j*sizeof(Vec) once and
                // reads disp/normal through fixed offsets. Under opt_common_subs off the named
                // field form rematerialises both addresses into separate registers, so spell
                // the base out. The #else form below is the natural one.
                u8* row = (u8*)mesh + j * 12;
                f32 d0y = *(f32*)(row + 0xC1C);
                f32 d1y = *(f32*)(row + 0xC28);
                f32 groundY = d0y + (cx - xj) / step * (d1y - d0y);
#else
                f32 groundY = mesh->disp[0][j].y + (cx - xj) / step * (mesh->disp[0][j + 1].y - mesh->disp[0][j].y);
#endif

                if (unit->mPosition.y < groundY) {
                    if ((unit->mAttributeFlags & kCeiling) && groundY < float_120_80421eb0) {
                        break;
                    }
#ifdef MUST_MATCH
                    if (*(f32*)(row + 0x1834) > float_0_80421e8c) {
#else
                    if (mesh->normal[0][j].y > float_0_80421e8c) {
#endif
                        BattleUnitPart* part;
                        s32 partId;

                        if (p->enemyVel[*count].y < float_0_80421e8c) {
                            p->enemyVel[*count].y = float_0_80421e8c;
                        }
                        unit->mPosition.y = groundY;
#ifdef MUST_MATCH
                        // Loading 0.5f into a local here makes MWCC issue the lfs before
                        // the two stores, as the original does; the natural form below
                        // schedules it after them.
                        {
                            f32 half = float_0p5_80421e54;
                            unit->mRotation.z = float_0_80421e8c;
                            p->enemyUnk[*count] = float_0_80421e8c;
                            p->enemyVel[*count].y += half * *(f32*)(row + 0x1834);
                        }
#else
                        unit->mRotation.z = float_0_80421e8c;
                        p->enemyUnk[*count] = float_0_80421e8c;
                        p->enemyVel[*count].y += float_0p5_80421e54 * mesh->normal[0][j].y;
#endif

                        partId = BtlUnit_GetBodyPartsId(unit);
                        part = BtlUnit_GetPartsPtr(unit, partId);
                        if (!(part->attributes & kUnk_ImmuneToDamageOrStatus) && !(part->attributes & kUnknown_0x4000)) {
                            if (!(unit->mAttributeFlags & kInactive) && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
                                BtlUnit_SetAnimType(part, 0x27);
                            }
                            effSmallStarEntry(0, 2, cx, unit->mPosition.y, unit->mPosition.z, float_0_80421e8c, float_neg1_80421e94, float_0_80421e8c);
                            BtlUnit_snd_se(unit, unit->mKindParams->damageSfxName, 0xF1194D80, 0);
                            if (!(unit->mAttributeFlags & 0x200000) && !(unit->mAttributeFlags & kInactive) && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
                                p->enemyUnk[*count] = float_20_80421e74;
                                if (irand(2) == 0) {
                                    p->enemyUnk[*count] *= float_neg1_80421e94;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    unit->mPosition.x = x;
    unit->mPosition.x += p->enemyVel[*count].x;
    unit->mPosition.y += p->enemyVel[*count].y;
    unit->mPosition.z += p->enemyVel[*count].z;
    unit->mRotation.z += p->enemyUnk[*count];

#ifdef MUST_MATCH
    // Match trick: separate locals make MWCC load mPosition.y before mHomePosition.y; the
    // natural form below loads them in the other order. The body is otherwise identical.
    {
        f32 py = unit->mPosition.y;
        f32 hy = unit->mHomePosition.y;

        if (py < hy) {
            unit->mPosition.y = hy;
            p->enemyVel[*count].y = float_0_80421e8c;
            if (!(unit->mAttributeFlags & kInactive) && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
                unit->mRotation.z = float_0_80421e8c;
                p->enemyUnk[*count] = float_0_80421e8c;
            }
        }
    }
#else
    if (unit->mPosition.y < unit->mHomePosition.y) {
        unit->mPosition.y = unit->mHomePosition.y;
        p->enemyVel[*count].y = float_0_80421e8c;
        if (!(unit->mAttributeFlags & kInactive) && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
            unit->mRotation.z = float_0_80421e8c;
            p->enemyUnk[*count] = float_0_80421e8c;
        }
    }
#endif

    p->unitIds[*count] = BtlUnit_GetUnitId(unit);
    (*count)++;
}
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma dont_inline reset

// .text:0x1158 | 0x80244150 | size: 0x48
s32 end_deka(EventEntry* event) {
    DekaWork* p = get_ptr();
    effDelete(((DekaStarEffectData*)p->starEffect->userdata)->unk34);
    effDelete(p->starEffect);
    return 2;
}

// .text:0xAA4 | 0x80243A9C | size: 0x4
static void disp_3D(CameraId cameraId, void* param) {}

// .text:0xAA0 | 0x80243A98 | size: 0x4
static void disp_3D_alpha(CameraId cameraId, void* param) {}

// .text:0x83C | 0x8024380C | size: 0x28C
// The residual is FPR allocation/scheduling: target keeps the y midpoint in f2 (so it can
// hoist the `tan` argument load into the freed f1 before the camera stores) and emits
// `fmuls f2,f2,f4`; ours materialises the y product in f1 and cannot hoist. Declaring the
// three sums as named locals (`sx/sy/sz`) lifted it 94.78 -> 94.87.
// NONMATCHING (94.87%): algorithm and every call/constant match; only that FPR/schedule
// colouring differs.
void yuka_capture(CameraId cameraId) {
    DekaWork* p = get_ptr();
    CameraEntry* cam = camGetPtr(cameraId);
    CameraEntry save;
    Vec target;
    f32 t;
    f32 sx, sy, sz;

    memcpy(&save, camGetPtr(cameraId), 0x260);

    sx = p->plane0.x + p->plane1.x;
    sy = p->plane0.y + p->plane1.y;
    sz = p->plane0.z + p->plane1.z;
    target.x = sx * float_0p5_80421e54;
    target.y = sy * float_0p5_80421e54;
    target.z = sz * float_0p5_80421e54;

    cam->cameraUp = vec3_803005e0;
    cam->cameraPos = target;
    cam->cameraAt = cam->cameraPos;

    t = (f32)tan(0.21816615760326385);
    cam->cameraPos.y += (f32)p->height * float_0p5_80421e54 / t;

    C_MTXLookAt(cam->view, (Point3d*)&cam->cameraPos, &cam->cameraUp, (Point3d*)&cam->cameraAt);
    C_MTXPerspective(cam->projection, float_25_80421e84, (f32)p->width / (f32)p->height, float_1_80421e88, float_32767_80421e6c);
    cam->projType = GX_PERSPECTIVE;
    GXSetProjection(cam->projection, cam->projType);
    GXSetViewport(float_0_80421e8c, float_0_80421e8c, (f32)p->texWidth, (f32)p->texHeight, float_0_80421e8c, float_1_80421e88);
    GXSetScissor(0, 0, p->texWidth, p->texHeight);
    test_kururing_mapdisp(cameraId);
    sysWaitDrawSync();
    GXSetTexCopySrc(0, 0, p->texWidth, p->texHeight);
    GXSetTexCopyDst(p->texWidth, p->texHeight, 4, 0);
    GXSetZMode(1, 3, 1);
    GXCopyTex(p->texEntry->address, 1);
    GXPixModeSync();
    memcpy(camGetPtr(cameraId), &save, 0x260);
}

// .text:0x5E4 | 0x802435DC | size: 0x230
void yuka_disp(CameraId cameraId, void* param) {
    DekaWork* p = get_ptr();
    CameraEntry* cam = camGetPtr(cameraId);
    GXTexObj texObj;
    u8* base;
    s32 i;
#ifdef MUST_MATCH
    s32 j;
#endif

    if (p->texEntry->address == NULL) {
        return;
    }
    base = p->matEntry->address;
    if (base == NULL) {
        return;
    }

    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXInitTexObj(&texObj, p->texEntry->address, p->texWidth, p->texHeight, 4, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXLoadPosMtxImm(cam->view, 0);
    GXSetCurrentMtx(0);

#ifdef MUST_MATCH
    // Match trick: standing alone, MWCC emits the GX FIFO base (lis r29,0xcc01) after the
    // loop's induction-variable inits. Wrapping the loop in a one-iteration outer loop makes
    // the hoisted address come first, as the original does; the outer loop folds away. The
    // #else form is the natural single loop.
    for (j = 0; j < 1; j++) {
#endif
    for (i = 0; i < 0x80; i++) {
        GXBegin(0x80, 0, 4);
        GXPosition3f32(*(f32*)(base + i * 0xC + 0xC18), *(f32*)(base + i * 0xC + 0xC1C), *(f32*)(base + i * 0xC + 0xC20));
        GXTexCoord2f32(*(f32*)(base + i * 4 + 0x3060), *(f32*)(base + i * 4 + 0x3468));
        GXPosition3f32(*(f32*)(base + i * 0xC + 0x1224), *(f32*)(base + i * 0xC + 0x1228), *(f32*)(base + i * 0xC + 0x122C));
        GXTexCoord2f32(*(f32*)(base + i * 4 + 0x3264), *(f32*)(base + i * 4 + 0x366C));
        GXPosition3f32(*(f32*)(base + i * 0xC + 0x1230), *(f32*)(base + i * 0xC + 0x1234), *(f32*)(base + i * 0xC + 0x1238));
        GXTexCoord2f32(*(f32*)(base + i * 4 + 0x3268), *(f32*)(base + i * 4 + 0x3670));
        GXPosition3f32(*(f32*)(base + i * 0xC + 0xC24), *(f32*)(base + i * 0xC + 0xC28), *(f32*)(base + i * 0xC + 0xC2C));
        GXTexCoord2f32(*(f32*)(base + i * 4 + 0x3064), *(f32*)(base + i * 4 + 0x346C));
        GXEnd();
    }
#ifdef MUST_MATCH
    }
#endif
}

// get_ptr is kept out of line: the original callers all emit `bl get_ptr`.
#pragma dont_inline on
// .text:0x5D8 | 0x802435D0 | size: 0xC
static DekaWork* get_ptr(void) {
    return (DekaWork*)((u8*)_battleWorkPointer + 0x1F4C);
}
#pragma dont_inline reset

// .text:0x4BC | 0x8024327C | size: 0x354
// 100%: the loop-1 and final scratch FPRs are pinned by writing the literal
// 30.0f / 7.0f / 2.0f / 70.0f where the named .sdata2 constants would be used. A
// literal is a constant node and the named constant is an extern load, so MWCC colours
// the surrounding float temporaries one register lower for the named form; the literals
// hold the same values and pool to the same slots (objdiff normalises those symbols).
// The MUST_MATCH block also hoists the scan loop's two clamp bounds as callee-saved
// locals (30 in f30, 20 in f31) and gives tm/tj function scope, which reproduces the
// original's registers. The int->float magic stays the compiler's local @668 instead of
// double_to_int_80300630 (objdiff ignores that reloc).
void create_timing(void) {
    DekaWork* p = get_ptr();
    s32 idx[8];
#ifdef MUST_MATCH
    // These take r29/r6 like the original; declared before ti so the allocator agrees.
    DekaTiming* tm;
    DekaTiming* tj;
#endif
    DekaTiming* ti;
    f32 times[8];
    f32 tf;
    s32 i, j, n;
    s32 tidx;
    s32 tries;
    f32 f;
#ifdef MUST_MATCH
    // Declared in this order and initialised 30-then-20 they land in f30/f31.
    f32 c20;
    f32 c30;
#endif

    memcpy(p->timingCopy, p->timing, 0x1C0);

restart:
    n = p->beatCount + 3;
#ifdef MUST_MATCH
    c30 = float_30_80421e70;
    c20 = float_20_80421e74;
#endif
    for (i = 0; i < 8; i++) {
        ti = &p->timing[i];
        ti->flag = 0;
        if (i < n) {
#ifdef MUST_MATCH
            f32 fr;
            f32 d;
#endif
            ti->unk08 = 1;
#ifdef MUST_MATCH
            fr = (f32)rand();
            d = (p->beatPeriod - float_40_80421e68);
            f = d * fr / float_32767_80421e6c;
#else
            f = (p->beatPeriod - float_40_80421e68) * (f32)rand() / float_32767_80421e6c;
#endif
#ifdef MUST_MATCH
            if (f <= 30.0f) {
                f = 30.0f;
            }
#else
            if (f <= float_30_80421e70) {
                f = float_30_80421e70;
            }
#endif
            if (f >= p->beatPeriod - float_20_80421e74) {
                f = p->beatPeriod - float_20_80421e74;
            }
            ti->time = f;
            ti->state = 2;
        } else {
            ti->state = 0;
        }
    }

    tries = 0;
scan:
    for (i = 0; i < n; i++) {
#ifdef MUST_MATCH
        tm = &p->timing[i];
#else
        DekaTiming* tm = &p->timing[i];
#endif
        for (j = 0; j < n; j++) {
#ifndef MUST_MATCH
            DekaTiming* tj;
#endif
            if (i == j) {
                continue;
            }
            tj = &p->timing[j];
            if (tj->state != 2) {
                continue;
            }
            if (__fabs(tj->time - tm->time) <= 15.75) {
                f = p->beatPeriod * (f32)rand() / float_32767_80421e6c;
#ifdef MUST_MATCH
                if (f <= c30) {
                    f = c30;
                }
                if (f >= p->beatPeriod - c20) {
                    f = p->beatPeriod - c20;
                }
#else
                if (f <= float_30_80421e70) {
                    f = float_30_80421e70;
                }
                if (f >= p->beatPeriod - float_20_80421e74) {
                    f = p->beatPeriod - float_20_80421e74;
                }
#endif
                tm->time = f;
                if (++tries == 100) {
                    tm->state = 0;
                    tm->flag = 0;
                    goto restart;
                }
                goto scan;
            }
        }
        tries = 0;
    }

    for (i = 0; i < n; i++) {
        idx[i] = i;
        times[i] = p->timing[i].time;
    }
    for (i = 0; i < n - 1; i++) {
        for (j = i + 1; j < n; j++) {
            tf = times[i];
            if (tf > times[j]) {
                tidx = idx[i];
                times[i] = times[j];
                idx[i] = idx[j];
                idx[j] = tidx;
                times[j] = tf;
            }
        }
    }

#ifdef MUST_MATCH
    f = p->beatPeriod - 7.0f - 2.0f;
#else
    f = p->beatPeriod - float_7_80421e78;
    f = f - float_2_80421e7c;
#endif
    p->timing[idx[n - 1]].time = f;
#ifdef MUST_MATCH
    if (p->timing[idx[0]].time > 70.0f) {
        p->timing[idx[0]].time = 70.0f;
    }
#else
    if (p->timing[idx[0]].time > float_70_80421e80) {
        p->timing[idx[0]].time = float_70_80421e80;
    }
#endif
}

// .text:0x25C | 0x80243254 | size: 0x28
s32 weaponGetPower_Deka(void) {
    DekaWork* p = get_ptr();
    return p->combo + 1;
}

// .text:0x224 | 0x8024321C | size: 0x38
s32 wait_star_stone_take_on(EventEntry* event) {
    DekaWork* p = get_ptr();
    return (u32)(p->starState - 4u) <= 0 ? 2 : 0;
}

// .text:0x1F8 | 0x802431F0 | size: 0x2C
s32 start_game(EventEntry* event) {
    DekaWork* p = get_ptr();
    p->baseState = 1;
    return 2;
}

// .text:0x1C0 | 0x802431B8 | size: 0x38
s32 wait_game_end(EventEntry* event) {
    DekaWork* p = get_ptr();
    return (u32)(p->baseState - 5u) <= 0 ? 2 : 0;
}

// .text:0x1C0 | 0x8024318C | size: 0x24
s32 star_stone_appear(EventEntry* event) {
    DekaWork* p = get_ptr();
    p->starState = 1;
    return 2;
}

// .text:0x98 | 0x80243090 | size: 0xFC
s32 get_ride_pos(EventEntry* event) {
    DekaWork* p = get_ptr();
    s32* args = event->args;
    s32 flag = evtGetValue(event, args[0]);
    f32 f;

    if (flag == 0) {
        s32 a = args[1];
        args += 2;
        f = p->starScale.x * float_64_80421e5c * float_0p25_80421e58;
        evtSetFloat(event, a, float_0p5_80421e54 * f + p->starPos.x);
    }
    else {
        s32 a = args[1];
        args += 2;
        f = p->starScale.x * float_64_80421e5c * float_0p25_80421e58;
        evtSetFloat(event, a, p->starPos.x - float_0p5_80421e54 * f);
    }

    f = float_0p25_80421e58 * (p->starScale.y * float_64_80421e5c);
    evtSetFloat(event, args[0], float_0p25_80421e58 * f + p->starPos.y);
    evtSetFloat(event, args[1], float_4_80421e64 * p->starScale.z + (float_5_80421e60 + p->starPos.z));
    return 2;
}

// .text:0x44 | 0x8024303C | size: 0x54
s32 star_stone_attack(EventEntry* event, s32 arg) {
    DekaWork* p = get_ptr();
    if (arg != 0) {
        p->starState = 10;
    }
    if (p->starState == 13) {
        return 2;
    }
    return 0;
}

// .text:0x0 | 0x80242FF8 | size: 0x44
s32 get_result(EventEntry* event) {
    DekaWork* p = get_ptr();
    evtSetValue(event, event->args[0], p->combo);
    return 2;
}


// ---- constant definitions (at end so uses emit symbol relocs) ----
const f32 float_40_80421e68 = 40.0f;
const f32 float_32767_80421e6c = 32767.0f;
const f32 float_30_80421e70 = 30.0f;
const f32 float_20_80421e74 = 20.0f;
const f32 float_7_80421e78 = 7.0f;
const f32 float_2_80421e7c = 2.0f;
const f32 float_70_80421e80 = 70.0f;
const f32 float_25_80421e84 = 25.0f;
const f32 float_1_80421e88 = 1.0f;
const f32 float_0_80421e8c = 0.0f;
const f32 float_8_80421e90 = 8.0f;
const f32 float_neg1_80421e94 = -1.0f;
const f32 float_255_80421e98 = 255.0f;
const f32 float_0p75_80421e9c = 0.75f;
const f32 float_18_80421ea0 = 18.0f;
const f32 float_1p25_80421ea4 = 1.25f;
const f32 float_250_80421ea8 = 250.0f;
const f32 float_0p0078125_80421eac = 0.0078125f;
const f32 float_120_80421eb0 = 120.0f;
const f32 float_neg1000_80421eb4 = -1000.0f;
const f32 float_37_80421eb8 = 37.0f;
const f32 float_50_80421ebc = 50.0f;
const f32 float_2160_80421ec0 = 2160.0f;
const f32 float_neg100_80421ec4 = -100.0f;
const f32 float_12p5_80421ec8 = 12.5f;
const f32 float_neg0p5_80421ecc = -0.5f;
const f32 float_43_80421ed0 = 43.0f;
const f32 float_1p5_80421ed4 = 1.5f;
const f32 float_500_80421ed8 = 500.0f;
const f32 float_180_80421edc = 180.0f;
const f32 float_0p8_80421ee0 = 0.8f;
const f32 float_0p1_80421ee4 = 0.1f;
const f32 float_683_80421ee8 = 683.0f;
const f32 float_4096_80421eec = 4096.0f;
const f32 float_neg40_80421ef0 = -40.0f;
const f32 float_0p01_80421ef4 = 0.01f;
const f32 float_901_80421ef8 = 901.0f;
