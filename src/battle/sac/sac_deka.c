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
#include <math.h>
#include "pmario_sound.h"
#include "eff/eff_starstone.h"

void psndSFX_pit(s32 id, s16 pitch);

// The original sac_deka unit was compiled with -O4,s (optimize for size): the
// prologues/epilogues use stmw/lmw, which -O4,p never emits. There is no
// per-Object flag for this unit, so set it here.
#pragma optimize_for_size on

// ---- local declarations (no header yet) ----
void btl_camera_shake_h(s32 id, s32 a, s32 b, f32 x, f32 y);
void BtlUnit_SetAnimType(BattleUnitPart* part, s32 type);
void mapGrpFlagOn(const char* name, u32 flag);
void mapGrpFlagOff(const char* name, u32 flag);
void BattleAudienceSoundHandBeat(void);
void BattleAudienceSoundCallKind(s32 kind);
void BattleAudienceSoundWhistleKind(s32 kind);

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

void main_enemy_sub(BattleUnit* unit, s32* count);
void yuka_init(void);
void yuka_main(void);
void yuka_disp(CameraId cameraId, void* param);
void main_base(void);
void main_star(void);
void disp_2D(CameraId cameraId, void* param);
void disp_3D(CameraId cameraId, void* param);
void disp_3D_alpha(CameraId cameraId, void* param);

const Vec vec3_80300528 = { -1000.0f, 100.0f, 0.0f };
const Vec vec3_80300534 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_803005e0 = { 0.0f, 0.0f, -1.0f };

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
extern const f64 double_15p75_80300628;
extern const f64 double_to_int_80300630;
extern const f64 double_0p21817_80300638;
extern const f64 double_to_int_mask_80300640;

int rand(void);

// .text:0x0 | 0x80242FF8 | size: 0x44
s32 get_result(EventEntry* event) {
    DekaWork* p = get_ptr();
    evtSetValue(event, event->args[0], p->combo);
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

// .text:0x98 | 0x80243090 | size: 0xFC
// NONMATCHING (84.10%): 6 missing, 8 extra, plus register differences.
s32 get_ride_pos(EventEntry* event) {
    DekaWork* p = get_ptr();
    s32* args = event->args;
    s32 flag = evtGetValue(event, args[0]);
    f32 f;

    if (flag == 0) {
        f = p->starScale.x * float_64_80421e5c * float_0p25_80421e58;
        evtSetFloat(event, args[1], float_0p5_80421e54 * f + p->starPos.x);
    }
    else {
        f = p->starScale.x * float_64_80421e5c * float_0p25_80421e58;
        evtSetFloat(event, args[1], p->starPos.x - float_0p5_80421e54 * f);
    }
    args += 2;

    f = float_0p25_80421e58 * (p->starScale.y * float_64_80421e5c);
    evtSetFloat(event, args[0], float_0p25_80421e58 * f + p->starPos.y);
    evtSetFloat(event, args[1], float_4_80421e64 * p->starScale.z + (float_5_80421e60 + p->starPos.z));
    return 2;
}

// .text:0x1C0 | 0x8024318C | size: 0x24
s32 star_stone_appear(EventEntry* event) {
    DekaWork* p = get_ptr();
    p->starState = 1;
    return 2;
}

// .text:0x1C0 | 0x802431B8 | size: 0x38
s32 wait_game_end(EventEntry* event) {
    DekaWork* p = get_ptr();
    return (u32)(p->baseState - 5u) <= 0 ? 2 : 0;
}

// .text:0x1F8 | 0x802431F0 | size: 0x2C
s32 start_game(EventEntry* event) {
    DekaWork* p = get_ptr();
    p->baseState = 1;
    return 2;
}

// .text:0x224 | 0x8024321C | size: 0x38
s32 wait_star_stone_take_on(EventEntry* event) {
    DekaWork* p = get_ptr();
    return (u32)(p->starState - 4u) <= 0 ? 2 : 0;
}

// .text:0x25C | 0x80243254 | size: 0x28
s32 weaponGetPower_Deka(void) {
    DekaWork* p = get_ptr();
    return p->combo + 1;
}

// .text:0x4BC | 0x8024327C | size: 0x354
// NONMATCHING (96.25%): 4 instructions differ, 1 missing, plus register differences. Known issue: tries is not reset after each outer pass (the original has li r26,0 at sac_deka.s:350).
void create_timing(void) {
    DekaWork* p = get_ptr();
    f32 times[8];
    s32 idx[12];
    s32 i, j, m, n;
    s32 tries;
    f32 f;
    f32 lim;

    memcpy(p->timingCopy, p->timing, 0x1C0);

restart:
    n = p->beatCount + 3;
    for (i = 0; i < 8; i++) {
        DekaTiming* ti = &p->timing[i];
        ti->flag = 0;
        if (i < n) {
            ti->unk08 = 1;
            f = (p->beatPeriod - float_40_80421e68) * (f32)rand() / float_32767_80421e6c;
            if (f <= float_30_80421e70) {
                f = float_30_80421e70;
            }
            lim = p->beatPeriod - float_20_80421e74;
            if (f >= lim) {
                f = lim;
            }
            ti->time = f;
            ti->state = 2;
        } else {
            ti->state = 0;
        }
    }

    tries = 0;
scan:
    for (m = 0; m < n; m++) {
        DekaTiming* tm = &p->timing[m];
        for (j = 0; j < n; j++) {
            DekaTiming* tj;
            if (m == j) {
                continue;
            }
            tj = &p->timing[j];
            if (tj->state != 2) {
                continue;
            }
            if (fabs(tj->time - tm->time) <= double_15p75_80300628) {
                f = p->beatPeriod * (f32)rand() / float_32767_80421e6c;
                if (f <= float_30_80421e70) {
                    f = float_30_80421e70;
                }
                lim = p->beatPeriod - float_20_80421e74;
                if (f >= lim) {
                    f = lim;
                }
                tm->time = f;
                if (++tries == 100) {
                    tm->state = 0;
                    tm->flag = 0;
                    goto restart;
                }
                goto scan;
            }
        }
    }

    for (i = 0; i < n; i++) {
        idx[i] = i;
        times[i] = p->timing[i].time;
    }
    for (i = 0; i < n - 1; i++) {
        for (j = i + 1; j < n; j++) {
            if (times[i] > times[j]) {
                f32 tf = times[i];
                s32 tidx = idx[i];
                times[i] = times[j];
                idx[i] = idx[j];
                idx[j] = tidx;
                times[j] = tf;
            }
        }
    }

    p->timing[idx[n - 1]].time = p->beatPeriod - (float_7_80421e78 + float_2_80421e7c);
    if (p->timing[idx[0]].time > float_70_80421e80) {
        p->timing[idx[0]].time = float_70_80421e80;
    }
}

// .text:0x5E4 | 0x802435DC | size: 0x230
// NONMATCHING (95.59%): 3 missing, 6 extra, plus register differences.
void yuka_disp(CameraId cameraId, void* param) {
    DekaWork* p = get_ptr();
    CameraEntry* cam = camGetPtr(cameraId);
    GXTexObj texObj;
    u8* base;
    s32 i;

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
}

// .text:0x83C | 0x8024380C | size: 0x28C
// NONMATCHING (94.56%): 4 instructions differ, 3 missing, 6 extra, plus register differences.
void yuka_capture(CameraId cameraId) {
    DekaWork* p = get_ptr();
    CameraEntry* cam = camGetPtr(cameraId);
    CameraEntry save;
    Vec target;
    f32 t;

    memcpy(&save, camGetPtr(cameraId), 0x260);

    target.x = (p->plane0.x + p->plane1.x) * float_0p5_80421e54;
    target.y = (p->plane0.y + p->plane1.y) * float_0p5_80421e54;
    target.z = (p->plane0.z + p->plane1.z) * float_0p5_80421e54;

    cam->cameraUp = vec3_803005e0;
    cam->cameraPos = target;
    cam->cameraAt = cam->cameraPos;

    t = (f32)tan(double_0p21817_80300638);
    cam->cameraPos.y += float_0p5_80421e54 * (f32)p->height / t;

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

// .text:0x1158 | 0x80244150 | size: 0x48
s32 end_deka(EventEntry* event) {
    DekaWork* p = get_ptr();
    effDelete(((DekaStarEffectData*)p->starEffect->userdata)->unk34);
    effDelete(p->starEffect);
    return 2;
}

// main_enemy is kept out of line: main_deka calls it rather than inlining it.
#pragma dont_inline on
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

// .text:0x1EE0 | 0x80244ED8 | size: 0xB8
#pragma dont_inline reset
#pragma dont_inline on
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

// .text:0x1F98 | 0x80244F90 | size: 0x190
#pragma dont_inline reset
// NONMATCHING (95.57%): 4 instructions differ, 1 missing, 2 extra, plus register differences.
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

// .text:0xAA0 | 0x80243A98 | size: 0x4
void disp_3D_alpha(CameraId cameraId, void* param) {}

// .text:0xAA4 | 0x80243A9C | size: 0x4
void disp_3D(CameraId cameraId, void* param) {}

// get_ptr is kept out of line: the original callers all emit `bl get_ptr`.
#pragma dont_inline on
// .text:0x5D8 | 0x802435D0 | size: 0xC
static DekaWork* get_ptr(void) {
    return (DekaWork*)((u8*)_battleWorkPointer + 0x1F4C);
}
#pragma dont_inline reset

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
const f64 double_15p75_80300628 = 15.75;
const f64 double_to_int_80300630 = 4503601774854144.0;
const f64 double_0p21817_80300638 = 0.21816615760326385;
const f64 double_to_int_mask_80300640 = 4503599627370496.0;
