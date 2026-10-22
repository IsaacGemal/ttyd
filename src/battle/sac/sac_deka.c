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
#include "party/party_motion.h"
#include "eff/eff_starstone.h"
#include "eff/eff_small_star.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/sac/sac_deka.h"
#include "battle/sac/sac_genki.h"
#include <stdlib.h>

// Data objects defined later in this file or in other units, declared before their first use.
extern s32 sac_start_camera_evt[];
extern s32 sac_mario_item_on_evt[];
extern s32 sac_mario_item_on_end_evt[];
extern s32 sac_mario_back_home[];
extern s32 sac_start_stardust[];
extern s32 sac_cheer[];
extern s32 sac_cheer_end[];
extern s32 sac_ac_help_on[];
extern s32 sac_ac_help_off[];
extern u32 dmg_evt[];
#pragma pool_data on
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



// ---- local declarations (no header yet) ----

// ---- local work struct at _battleWorkPointer + 0x1F4C ----
typedef struct DekaTiming {
    /*0x00*/ s32 state;
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 kind;  // 1: a press in the window scores; other kinds boo
    /*0x0C*/ f32 time;
    /*0x10*/ Vec offset;  // added to the icon position; zeroed in state 2
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
    /*0x04C*/ f32 timingSlideY;  // -40 when timing icons appear, eased to 0 over 30 frames
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
    /*0x444*/ f32 starScaleFrom;
    /*0x448*/ f32 starScaleTo;
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
    dispEntry(CAMERA_2D, 901.0f, 1, disp_2D, NULL);
    dispEntry(CAMERA_3D, 0.0f, 1, disp_3D, NULL);
    dispEntry(CAMERA_3D, 0.0f, 2, disp_3D_alpha, NULL);
    dispEntry(CAMERA_3D, 0.0f, 0, yuka_disp, NULL);
    return 0;
}

// .text:0x29F8 | 0x802459F0 | size: 0x2D8
static void yuka_init(void) {
    f32 x;
    DekaWork* p = get_ptr();
    DekaYukaMesh* mesh;
    MapObject* obj = mapGetMapObj("stg_b_yuka");
    f32 z;
    s32 j, i;
    s32 texW;

    mapGrpFlagOn("stg_b_yuka", 0x400001);

    PSMTXMultVec(obj->modelWorldMtx, &obj->joints->bboxMin, &p->plane0);
    PSMTXMultVec(obj->modelWorldMtx, &obj->joints->bboxMax, &p->plane1);

    texW = (s32)(p->plane1.x - p->plane0.x);
    p->width = p->texWidth = texW;
    p->height = p->texHeight = (s32)(p->plane1.z - p->plane0.z);

    p->texWidth += p->texWidth & 1;
    p->texHeight += p->texHeight & 1;

    if (p->texWidth > 0x260) {
        p->texWidth = 0x260;
    }
    if (0x1E0 < p->texHeight) {
        p->texHeight = 0x1E0;
    }

    p->texEntry = smartAlloc(GXGetTexBufferSize(p->texWidth, p->texHeight, 4, 0, 0), 2);
    p->matEntry = smartAlloc(0x3870, 2);

    mesh = p->matEntry->address;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 0x81; j++) {
            Vec v, v2;
            f32 tmp;
            f32 fw = (f32)p->width;
            f32 wc = fw / 128.0f;
            x = p->plane0.x + wc * (f32)j;

            tmp = (f32)p->height;
            z = p->plane0.z + tmp * (f32)i;

            v = (Vec){ x, 0.0f, z };
            mesh->disp[i][j] = v;
            v2 = (Vec){ x, 0.0f, z };
            mesh->pos[i][j] = v2;
            mesh->normal[i][j] = (Vec){ 0.0f, 0.0f, 0.0f };
            tmp = (f32)j;
            mesh->texU[i][j] = 0.0078125f * tmp;
            mesh->texV[i][j] = (f32)i;
        }
    }

    dispEntry(CAMERA_SHADOW, 0.0f, 0, (DispCallback)yuka_capture, NULL);
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

                if ((sc > idx && p->yukaPos[i] > 0.0f) ||
                    (sc < idx && p->yukaPos[i] < 0.0f)) {
                    for (j = 0; j < 2; j++) {
                        mesh->normal[j][p->yukaIndex[i]].y += p->yukaUnk[i];
                    }
                    if (p->yukaPos[i] > 0.0f) {
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
        const f32 one = 1.0f;
        const f32 zero = 0.0f;
        const f32 two = 2.0f;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 0x81; j++) {
            v2 = (Vec){ 0.0f, 0.0f, 0.0f };

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
            PSVECScale(&v2, &v2, 0.01f);

            PSVECAdd(&mesh->normal[i][j], &v2, &mesh->normal[i][j]);
            PSVECScale(&mesh->normal[i][j], &v8, 0.1f);
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

    for (i = 0; i < 2; i++) {
        for (j = 1; j < 0x80; j++) {
            mesh->pos[i][j].y = 0.5f * (mesh->pos[i][j - 1].y + mesh->pos[i][j + 1].y);
        }
    }
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
static void main_base(void) {
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
        p->beatPeriod = 180.f;
        p->beatPos = 0.0f;
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
            p->beatPos += 0.8f + 0.1f * (f32)p->combo;
            psndSFX_pit(p->sndHandle,
                (s32)(683.0f * (f32)p->combo + 4096.0f * p->beatPos / p->beatPeriod));
            if (p->beatPos >= p->beatPeriod) {
                p->beatPos = 0.0f;
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
                    p->timingSlideY = -40.f;
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
                if (t->state == 2 && p->beatPos >= t->time - 7.0f &&
                    p->beatPos <= 7.0f + t->time) {
                    if (t->kind == 1) {
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
                    if (t->state == 2 && t->kind == 1) {
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
            if (t->state == 2 && t->kind != 2 && p->beatPos > 7.0f + t->time) {
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
        p->timingSlideY = intplGetValue(0, 30 - p->baseTimer, 30, -40.f, 0.0f);
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
            p->gaugePos = (Vec){ -1000.0f, 100.0f, 0.0f };
            p->gaugeWidth = 500.f;
            p->gaugeAlpha = 0xFF;
            // fallthrough
        case 2:
            p->gaugeTimer++;
            p->gaugePos.x = intplGetValue(4, p->gaugeTimer, 30, -1000.f, 0.0f);
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
            p->gaugePos.x = intplGetValue(4, p->gaugeTimer, 30, 0.0f, -1000.f);
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
void main_timing(s32 idx) {
    DekaWork* p = get_ptr();
    DekaTiming* t = &p->timing[idx];

    switch (t->state) {
        case 0:
            t->kind = 0;
            break;
        case 2:
            t->alpha = 0xFF;
            t->offset = (Vec){ 0.0f, 0.0f, 0.0f };
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

// .text:0x1640 | 0x80244638 | size: 0x89C
static void main_star(void) {
    DekaWork* p = get_ptr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    s32 audience = BattleAudience_GetAudienceNum();
    s32 alpha = 0xFF;

    switch (p->starState) {
    case 1:
        p->starState = 2;
        p->starTimer = 0;
        p->starEffect = effStarStoneEntry(0, 0.0f, -1000.f, 0.0f, 1.0f);
        BtlUnit_GetPos(mario, &p->starPos.x, &p->starPos.y, &p->starPos.z);
        p->starPos.y += mario->mSizeMultiplier * (f32)mario->mHeight + 37.0f;
        p->starPrev = p->starPos;
        p->starTarget = p->starPos;
        p->starTarget.y += 50.f;
        p->starTarget.z += -1.0f;
        p->starScale = (Vec){ 0.0f, 0.0f, 0.0f };
        p->starRot = (Vec){ 0.0f, 0.0f, 0.0f };
        /* fallthrough */
    case 2:
        p->starTimer++;
        if (p->starTimer <= 100) {
            p->starPos.x = intplGetValue(0, p->starTimer, 100, p->starPrev.x, p->starTarget.x);
            p->starPos.y = intplGetValue(0, p->starTimer, 100, p->starPrev.y, p->starTarget.y);
            p->starPos.z = intplGetValue(0, p->starTimer, 100, p->starPrev.z, p->starTarget.z);
            {
                f32 s = intplGetValue(0, p->starTimer, 100, 0.0f, 2.0f);
                p->starScale.z = s;
                p->starScale.y = s;
                p->starScale.x = s;
            }
        } else {
            p->starPos.x = p->starTarget.x;
            p->starPos.y = p->starTarget.y;
            p->starPos.z = p->starTarget.z;
            p->starScale.z = 2.0f;
            p->starScale.y = 2.0f;
            p->starScale.x = 2.0f;
        }
        p->starRot.y = intplGetValue(4, p->starTimer, 120, 0.0f, 2160.f);
        if (p->starTimer >= 120) {
            p->starState = 3;
            p->starTimer = 0;
            p->starPrev = p->starPos;
            p->starTarget.x = -100.f;
            p->starTarget.y = 30.f;
            p->starTarget.z = 50.f;
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
        p->starScaleFrom = p->starScaleTo = p->starScale.x;
        p->starScaleTo += 1.0f;
        psndSFXOn("SFX_BTL_SAC_DOKKAN4");
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
        s32 tbl[11] = { 1, 2, 0, 2, 3, 1, 3, 4, 2, 4, 5 };
        f32 s;
        p->starTimer++;
        s = intplGetValue(4, tbl[p->starTimer / 4], 5, p->starScaleFrom, p->starScaleTo);
        p->starScale.z = s;
        p->starScale.y = s;
        p->starScale.x = s;
        p->starPos.y = 12.5f * (p->starScale.y - 2.0f) + 30.f;
        if (p->starTimer >= 0x2B) {
            p->starState = 5;
            p->starTimer = 0;
            p->starScale.x = p->starScale.y = p->starScale.z = p->starScaleTo;
        }
        break;
    }
    case 8:
        p->starState = 9;
        p->starTimer = 0;
        p->starScaleFrom = p->starScaleTo = p->starScale.x;
        p->starScaleTo = 2.0f;
        /* fallthrough */
    case 9: {
        s32 tbl[11] = { 1, 2, 0, 2, 3, 1, 3, 4, 2, 4, 5 };
        f32 s;
        p->starTimer++;
        s = intplGetValue(4, tbl[p->starTimer / 4], 5, p->starScaleFrom, p->starScaleTo);
        p->starScale.z = s;
        p->starScale.y = s;
        p->starScale.x = s;
        p->starPos.y = 12.5f * (p->starScale.y - 2.0f) + 30.f;
        if (p->starTimer >= 0x2B) {
            p->starState = 5;
            p->starTimer = 0;
            p->starScale.x = p->starScale.y = p->starScale.z = p->starScaleTo;
        }
        break;
    }
    case 10:
        p->starState = 11;
        p->starTarget.y = p->starPos.y;
        p->starVel = 5.0f;
        p->starTimer = 0;
        p->starCounter = 0;
        /* fallthrough */
    case 11: {
        s32 j;

        p->starPos.y = p->starPos.y + p->starVel;
        p->starVel += -0.5f;
        if (p->starPos.y < p->starTarget.y) {
            f32 tblA[6] = { 1.0f, 1.3f, 2.0f, 2.6f, 3.3f, 3.8f };
            f32 tblB[6] = { 1.0f, 1.3f, 2.0f, 2.6f, 3.3f, 3.8f };

            p->starCounter++;
            btl_camera_shake_h(0, 20, 0, (f32)p->starCounter * tblA[p->combo], 0.0f);
            psndSFXOn("SFX_BTL_SAC_DOKKAN5");
            for (j = 0; j < 2; j++) {
                s32 i;
                for (i = 0; i < 6; i++) {
                    if (p->yukaActive[i] == 0) {
                        p->yukaActive[i] = 1;
                        p->yukaPos[i] = 2.0f;
                        if (j == 1) {
                            p->yukaPos[i] *= -1.0f;
                        }
                        if (j == 0) {
                            p->yukaIndex[i] = 0x2A;
                        } else {
                            p->yukaIndex[i] = 0x29;
                        }
                        p->yukaScale[i] = 43.0f;
                        p->yukaUnk[i] = (f32)p->starCounter * tblB[p->combo] + 4.0f;
                        break;
                    }
                }
            }
            if (p->starCounter >= 6) {
                p->starState = 12;
                p->starTimer = 30;
                p->starTarget = p->starPos;
                p->starPrev = p->starTarget;
                p->starTarget.y += 40.f;
            } else {
                f32 x = 4.0f * (f32)p->starCounter + 20.f;
                f32 half = 0.5f;
                // The half*x product is its own local so MWCC keeps the original's multiply order.
                f32 hx = half * x;

                p->starPos.y = p->starTarget.y;
                p->starVel = hx * x / (2.0f * x);
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
        alpha = (s32)intplGetValue(0, p->starTimer, 30, 255.0f, 0.0f);
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
        d->unk14 = 1.5f * p->starScale.x;
        d->unk5F = (u8)alpha;
    }
}

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

// .text:0x11A0 | 0x80244198 | size: 0x404
// The int->float magic this function loads is the compiler's own pool entry (an anonymous
// @NNN local), not double_to_int_80300630; objdiff ignores that relocation, so the only real
// differences were FPR allocation.
void main_enemy_sub(BattleUnit* unit, s32* count) {
    DekaWork* p = get_ptr();
    DekaYukaMesh* mesh = p->matEntry->address;
    f32 x;
    f32 cx;
    s32 j;

    unit->mRotationOffset = unit->offsetCenter;
    p->enemyVel[*count].y -= 1.25f;

    x = unit->mPosition.x;
    if (x >= 250.f) {
        cx = 250.f;
    }
    else if (x <= 0.0f) {
        cx = 0.0f;
    }
    else {
        cx = x;
    }

    for (j = 0x40; j < 0x81; j++) {
        f32 step = (f32)p->width / 128.0f;
        f32 xj = p->plane0.x + step * (f32)j;

        if (cx >= xj) {
            if (cx < xj + step) {
                f32 groundY = mesh->disp[0][j].y + (cx - xj) / step * (mesh->disp[0][j + 1].y - mesh->disp[0][j].y);

                if (unit->mPosition.y < groundY) {
                    if ((unit->mAttributeFlags & kCeiling) && groundY < 120.f) {
                        break;
                    }
                    if (mesh->normal[0][j].y > 0.0f) {
                        BattleUnitPart* part;
                        s32 partId;

                        if (p->enemyVel[*count].y < 0.0f) {
                            p->enemyVel[*count].y = 0.0f;
                        }
                        unit->mPosition.y = groundY;
                        unit->mRotation.z = 0.0f;
                        p->enemyUnk[*count] = 0.0f;
                        p->enemyVel[*count].y += 0.5f * mesh->normal[0][j].y;

                        partId = BtlUnit_GetBodyPartsId(unit);
                        part = BtlUnit_GetPartsPtr(unit, partId);
                        if (!(part->attributes & kUnk_ImmuneToDamageOrStatus) && !(part->attributes & kUnknown_0x4000)) {
                            if (!(unit->mAttributeFlags & kInactive) && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
                                BtlUnit_SetAnimType(part, 0x27);
                            }
                            effSmallStarEntry(0, 2, cx, unit->mPosition.y, unit->mPosition.z, 0.0f, -1.0f, 0.0f);
                            BtlUnit_snd_se(unit, unit->mKindParams->damageSfxName, 0xF1194D80, 0);
                            if (!(unit->mAttributeFlags & 0x200000) && !(unit->mAttributeFlags & kInactive) && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
                                p->enemyUnk[*count] = 20.f;
                                if (irand(2) == 0) {
                                    p->enemyUnk[*count] *= -1.0f;
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

    if (unit->mPosition.y < unit->mHomePosition.y) {
        unit->mPosition.y = unit->mHomePosition.y;
        p->enemyVel[*count].y = 0.0f;
        if (!(unit->mAttributeFlags & kInactive) && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
            unit->mRotation.z = 0.0f;
            p->enemyUnk[*count] = 0.0f;
        }
    }

    p->unitIds[*count] = BtlUnit_GetUnitId(unit);
    (*count)++;
}

// .text:0x1158 | 0x80244150 | size: 0x48
s32 end_deka(EventEntry* event) {
    DekaWork* p = get_ptr();
    effDelete(((DekaStarEffectData*)p->starEffect->userdata)->unk34);
    effDelete(p->starEffect);
    return 2;
}

// Draws the deka gauge: two planes with a scaling middle section, a beat bar, then the
// timing notes (timingCopy for the falling icons, timing for the resting ones). The `width; left;
// right;` declaration order, the (deliberately kept) `s32 id;` local, and declaring the loop
// cursor `DekaTiming* t;` *before* `s32 i;` (each loop then assigns it after a leading `f32 x;`
// declaration) all steer MWCC's GPR/FPR colouring and constant hoisting; dropping any of them
// regresses the score. The shapes that recover the original's FPR colouring are: the
// `ratio` temp in each loop and the recomputed `left`/`right` at the plane transforms.
// .text:0xAA8 | 0x80243AA0 | size: 0x6B0
static void disp_2D(CameraId cameraId, void* param) {
    DekaWork* p = get_ptr();
    GXColor color;
    Mtx mtx;
    Mtx scale;
    Mtx trans;
    f32 width;
    f32 left;
    f32 right;
    DekaTiming* t;
    s32 i;
    s32 id;

    f32 mid = p->gaugePos.x;
    f32 e8 = 8.0f;
    f32 hw = 0.5f * p->gaugeWidth;
    left = mid - hw - e8;
    right = mid + hw + e8;

    if (p->gaugeState >= 2 && p->gaugeState <= 5) {
        color = (GXColor){ 0xFF, 0xFF, 0xFF, p->gaugeAlpha };
        // `left` is recomputed at this call rather than copied out of its register, as in the original.
        PSMTXTrans(mtx, p->gaugePos.x - 0.5f * p->gaugeWidth - e8, p->gaugePos.y, p->gaugePos.z);
        btlDispTexPlane2(mtx, 0x43, color, 0);

        PSMTXScale(scale, p->gaugeWidth / 4.0f, 1.0f, 1.0f);
        PSMTXTrans(trans, p->gaugePos.x, p->gaugePos.y, p->gaugePos.z);
        PSMTXConcat(trans, scale, mtx);
        btlDispTexPlane2(mtx, 0x44, color, 0);

        PSMTXScale(scale, -1.0f, 1.0f, 1.0f);
        // Likewise `right` is recomputed here.
        PSMTXTrans(trans, 0.5f * p->gaugeWidth + p->gaugePos.x + 8.0f, p->gaugePos.y, p->gaugePos.z);
        PSMTXConcat(trans, scale, mtx);
        btlDispTexPlane2(mtx, 0x43, color, 0);
    }

    if (p->gaugeState >= 2 && p->gaugeState <= 5) {
        f32 ratio = p->beatPos / p->beatPeriod;

        if (ratio >= 0.0f && ratio <= 1.0f) {
            btlDispGXInit2DRasta();
            GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
            btlDispGXQuads2DRasta(left, 8.0f + p->gaugePos.y,
                ratio * (right - left) + left, p->gaugePos.y - 8.0f,
                0xFF, 0, 0, p->gaugeAlpha);
        }
    }

    GXSetScissor(0, 0x78, gp->fbWidth, gp->efbHeight);

    if (p->gaugeState >= 2 && p->gaugeState <= 5) {
        width = right - left;

        for (i = 0; i < 8; i++) {
            f32 x;
            t = &p->timingCopy[i];
            // The ratio is its own local so the multiply-add takes the original's operand order.
            {
                f32 ratio = t->time / p->beatPeriod;
                x = ratio * width + left + t->offset.x;
            }

            color = (GXColor){ 0xFF, 0xFF, 0xFF, p->gaugeAlpha };
            color.a = color.a * (t->alpha / 255.0f);

            if (t->kind == 1) {
                PSMTXScale(scale, 0.75f, 0.75f, 0.75f);
                PSMTXTrans(trans, x,
                    40.f + (p->timingSlideY + (p->gaugePos.y - 18.0f + t->offset.y)),
                    p->gaugePos.z + t->offset.z);
                PSMTXConcat(trans, scale, mtx);
                iconDispGxCol(mtx, 0x10, 0x95, color);
            }
            else if (t->kind == 2) {
                PSMTXScale(scale, 1.0f, 1.0f, 1.0f);
                PSMTXTrans(trans, x,
                    40.f + (p->timingSlideY + (p->gaugePos.y + t->offset.y)),
                    p->gaugePos.z + t->offset.z);
                PSMTXConcat(trans, scale, mtx);
                btlDispTexPlane2(mtx, 0x2C, color, 0);
            }
        }

        for (i = 0; i < 8; i++) {
            f32 x;
            t = &p->timing[i];
            // The ratio is its own local so the multiply-add takes the original's operand order.
            {
                f32 ratio = t->time / p->beatPeriod;
                x = ratio * width + left + t->offset.x;
            }

            if (t->state >= 2 && t->state <= 5) {
                color = (GXColor){ 0xFF, 0xFF, 0xFF, p->gaugeAlpha };
                color.a = color.a * (t->alpha / 255.0f);

                if (t->kind == 1) {
                    PSMTXScale(scale, 0.75f, 0.75f, 0.75f);
                    PSMTXTrans(trans, x,
                        p->timingSlideY + (p->gaugePos.y - 18.0f + t->offset.y),
                        p->gaugePos.z + t->offset.z);
                    PSMTXConcat(trans, scale, mtx);
                    id = p->beatPos >= t->time - 7.0f ? 0x95 : 0x91;
                    iconDispGxCol(mtx, 0x10, id, color);
                }
                else {
                    PSMTXScale(scale, 1.0f, 1.0f, 1.0f);
                    PSMTXTrans(trans, x,
                        p->timingSlideY + (p->gaugePos.y + t->offset.y),
                        p->gaugePos.z + t->offset.z);
                    PSMTXConcat(trans, scale, mtx);
                    btlDispTexPlane2(mtx, 0x2C, color, 0);
                }
            }

            PSMTXScale(scale, 1.0f, 1.0f, 1.0f);
            PSMTXTrans(trans, x,
                p->gaugePos.y + t->offset.y - 40.f,
                p->gaugePos.z + t->offset.z);
            PSMTXConcat(trans, scale, mtx);
            switch (t->flag) {
            case 0:
                break;
            case 1:
                btlDispTexPlane2(mtx, 0x3E, color, 0);
                break;
            case 2:
                btlDispTexPlane2(mtx, 0x3F, color, 0);
                break;
            }
        }
    }

    GXSetScissor(0, 0, gp->fbWidth, gp->efbHeight);
}

// .text:0xAA4 | 0x80243A9C | size: 0x4
static void disp_3D(CameraId cameraId, void* param) {}

// .text:0xAA0 | 0x80243A98 | size: 0x4
static void disp_3D_alpha(CameraId cameraId, void* param) {}

// .text:0x83C | 0x8024380C | size: 0x28C
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
    target.x = sx / 2.0f;
    target.y = sy / 2.0f;
    target.z = sz / 2.0f;

    cam->cameraUp = (Vec){ 0.0f, 0.0f, -1.0f };
    cam->cameraPos = target;
    cam->cameraAt = cam->cameraPos;

    t = (f32)tan(0.21816615760326385);
    cam->cameraPos.y += (f32)p->height / 2.0f / t;

    C_MTXLookAt(cam->view, (Point3d*)&cam->cameraPos, &cam->cameraUp, (Point3d*)&cam->cameraAt);
    C_MTXPerspective(cam->projection, 25.0f, (f32)p->width / (f32)p->height, 1.0f, 32767.0f);
    cam->projType = GX_PERSPECTIVE;
    GXSetProjection(cam->projection, cam->projType);
    GXSetViewport(0.0f, 0.0f, (f32)p->texWidth, (f32)p->texHeight, 0.0f, 1.0f);
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
    DekaYukaMesh* base;
    s32 i;
    s32 j;

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

    for (j = 0; j < 1; j++) {
        for (i = 0; i < 0x80; i++) {
            GXBegin(0x80, 0, 4);
            GXPosition3f32(base->disp[j][i].x, base->disp[j][i].y, base->disp[j][i].z);
            GXTexCoord2f32(base->texU[j][i], base->texV[j][i]);
            GXPosition3f32(base->disp[j + 1][i].x, base->disp[j + 1][i].y, base->disp[j + 1][i].z);
            GXTexCoord2f32(base->texU[j + 1][i], base->texV[j + 1][i]);
            GXPosition3f32(base->disp[j + 1][i + 1].x, base->disp[j + 1][i + 1].y, base->disp[j + 1][i + 1].z);
            GXTexCoord2f32(base->texU[j + 1][i + 1], base->texV[j + 1][i + 1]);
            GXPosition3f32(base->disp[j][i + 1].x, base->disp[j][i + 1].y, base->disp[j][i + 1].z);
            GXTexCoord2f32(base->texU[j][i + 1], base->texV[j][i + 1]);
            GXEnd();
        }
    }
}

// get_ptr is kept out of line: the original callers all emit `bl get_ptr`.
#pragma dont_inline on
// .text:0x5D8 | 0x802435D0 | size: 0xC
static DekaWork* get_ptr(void) {
    return (DekaWork*)&_battleWorkPointer->actionCommands.field_0x2BC[0];
}
#pragma dont_inline reset

// .text:0x4BC | 0x8024327C | size: 0x354
// tm/tj have function scope and are declared before ti, which reproduces the original's registers.
void create_timing(void) {
    DekaWork* p = get_ptr();
    s32 idx[8];
    // These take r29/r6 like the original; declared before ti so the allocator agrees.
    DekaTiming* tm;
    DekaTiming* tj;
    DekaTiming* ti;
    f32 times[8];
    f32 tf;
    s32 i, j, n;
    s32 tidx;
    s32 tries;
    f32 f;

    memcpy(p->timingCopy, p->timing, 0x1C0);

restart:
    n = p->beatCount + 3;
    for (i = 0; i < 8; i++) {
        ti = &p->timing[i];
        ti->flag = 0;
        if (i < n) {
            ti->kind = 1;
            f = (p->beatPeriod - 40.f) * (f32)rand() / 32767.0f;
            if (f <= 30.f) {
                f = 30.f;
            }
            if (f >= p->beatPeriod - 20.f) {
                f = p->beatPeriod - 20.f;
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
        tm = &p->timing[i];
        for (j = 0; j < n; j++) {
            if (i == j) {
                continue;
            }
            tj = &p->timing[j];
            if (tj->state != 2) {
                continue;
            }
            if (__fabs(tj->time - tm->time) <= 15.75) {
                f = p->beatPeriod * (f32)rand() / 32767.0f;
                if (f <= 30.f) {
                    f = 30.f;
                }
                if (f >= p->beatPeriod - 20.f) {
                    f = p->beatPeriod - 20.f;
                }
                tm->time = f;
                if (++tries == 100) {
                    tm->state = 0;
                    tm->flag = 0;
                    goto restart;
                }
                goto scan; // a clash rescans from the start (redraws all after 100 tries) out of both loops
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

    f = p->beatPeriod - 7.0f - 2.0f;
    p->timing[idx[n - 1]].time = f;
    if (p->timing[idx[0]].time > 70.f) {
        p->timing[idx[0]].time = 70.f;
    }
}

// .text:0x25C | 0x80243254 | size: 0x28
u32 weaponGetPower_Deka(BattleUnit* unit1, BattleWeapon* weapon, BattleUnit* unit2, BattleUnitPart* part) {
    DekaWork* p = get_ptr();
    return p->combo + 1;
}

// .text:0x224 | 0x8024321C | size: 0x38
s32 wait_star_stone_take_on(EventEntry* event) {
    DekaWork* p = get_ptr();
    return (p->starState - 4u) <= 0 ? 2 : 0;
}

// .text:0x1F8 | 0x802431F0 | size: 0x2C
static s32 start_game(EventEntry* event) {
    DekaWork* p = get_ptr();
    p->baseState = 1;
    return 2;
}

// .text:0x1C0 | 0x802431B8 | size: 0x38
static s32 wait_game_end(EventEntry* event) {
    DekaWork* p = get_ptr();
    return (p->baseState - 5u) <= 0 ? 2 : 0;
}

// .text:0x1C0 | 0x8024318C | size: 0x24
static s32 star_stone_appear(EventEntry* event) {
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
    f32 w = 64.0f;

    if (flag == 0) {
        s32 a = args[1];
        args += 2;
        f = p->starScale.x * w * 0.25f;
        evtSetFloat(event, a, 0.5f * f + p->starPos.x);
    }
    else {
        s32 a = args[1];
        args += 2;
        f = p->starScale.x * w * 0.25f;
        evtSetFloat(event, a, p->starPos.x - 0.5f * f);
    }

    f = 0.25f * (p->starScale.y * w);
    evtSetFloat(event, args[0], 0.25f * f + p->starPos.y);
    evtSetFloat(event, args[1], (5.0f + p->starPos.z) + 4.0f * p->starScale.z);
    return 2;
}

// .text:0x44 | 0x8024303C | size: 0x54
static s32 star_stone_attack(EventEntry* event, s32 arg) {
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


// ---- EVT scripts (.data, parsed before the code's jump tables) ----

// .data | .obj marioAttackEvent_Deka
u32 marioAttackEvent_Deka[396] = {
    0x0003005B, (u32)(void*)btlevtcmd_GetUnitId, 0xFFFFFFFC, 0xFE363C8E,
    0x0003005B, (u32)(void*)btlevtcmd_GetBodyId, 0xFE363C8E, 0xFE363C8F,
    0x0003005B, (u32)(void*)btlevtcmd_CheckActStatus, 0xFE363C8E, 0xFE363C80,
    0x00020018, 0xFE363C80, 0x00000000, 0x00020032,
    0xFE363C8E, 0xFFFFFFFF, 0x00000021, 0x0003005B,
    (u32)(void*)btlevtcmd_GetSelectEnemy, 0xFE363C83, 0xFE363C84, 0x00020018,
    0xFE363C83, 0xFFFFFFFF, 0x00010004, 0x00000063,
    0x00000021, 0x0004005B, (u32)(void*)btlevtcmd_AttackDeclare, 0xFFFFFFFE,
    0xFE363C83, 0xFE363C84, 0x0001005B, (u32)(void*)btlevtcmd_WaitGuardMove,
    0x0002005B, (u32)(void*)btlevtcmd_CommandPayWeaponCost, 0xFFFFFFFE, 0x0003005B,
    (u32)(void*)btlevtcmd_CommandGetWeaponAddress, 0xFFFFFFFE, 0xFE363C80, 0x0002005B,
    (u32)(void*)btlevtcmd_WeaponAftereffect, 0xFE363C80, 0x0001005B, (u32)(void*)btlevtcmd_reset_move_color_lv_all,
    0x0000006E, 0x0002005B, (u32)(void*)main_deka, 0xFE363C8E,
    0x00000070, 0x0001005E, (u32)(void*)sac_start_camera_evt, 0x0001000A,
    0x000001F4, 0x0006005B, (u32)(void*)btlevtcmd_snd_se, 0xFFFFFFFE,
    (u32)(void*)"SFX_BTL_SAC_START1", 0xF1194D80, 0x00000000, 0xF1194D80,
    0x0001005E, (u32)(void*)sac_mario_item_on_evt, 0x0001000A, 0x000001F4,
    0x0006005B, (u32)(void*)btlevtcmd_snd_se, 0xFFFFFFFE, (u32)(void*)"SFX_BTL_SAC_START2",
    0xF1194D80, 0x00000000, 0xFE363C8B, 0x0001005B,
    (u32)(void*)star_stone_appear, 0x0000006E, 0x0001005E, (u32)(void*)sac_start_stardust,
    0x00000070, 0x00010009, 0x00000064, 0x0006005B,
    (u32)(void*)btlevtcmd_snd_se, 0xFFFFFFFE, (u32)(void*)"SFX_BTL_SAC_START3", 0xF1194D80,
    0x00000000, 0xF1194D80, 0x0002005B, (u32)(void*)evt_snd_sfxoff,
    0xFE363C8B, 0x0003005B, (u32)(void*)evt_btl_camera_set_mode, 0x00000000,
    0x00000000, 0x00010009, 0x00000028, 0x0001005E,
    (u32)(void*)sac_mario_item_on_end_evt, 0x0002005B, (u32)(void*)btlevtcmd_SacBgsetMove, 0x00000000,
    0x0001005E, (u32)(void*)sac_mario_back_home, 0x0001005B, (u32)(void*)wait_star_stone_take_on,
    0x0000006E, 0x0004005B, (u32)(void*)btlevtcmd_AnimeChangePoseType, 0xFFFFFFFE,
    0x00000001, 0x0000002C, 0x0005005B, (u32)(void*)get_ride_pos,
    0x00000000, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (u32)(void*)btlevtcmd_JumpSetting, 0xFFFFFFFE, 0x0000001E,
    0x00000000, 0xF24A7C19, 0x0007005B, (u32)(void*)btlevtcmd_JumpPosition,
    0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0000001E, 0xFFFFFFFF, 0x0004005B, (u32)(void*)btlevtcmd_AnimeChangePose,
    0xFFFFFFFE, 0x00000001, (u32)(void*)"M_I_Y", 0x00000070,
    0x0000006E, 0x00020019, 0xFE363C8E, 0xFFFFFFFF,
    0x0003005B, (u32)(void*)btlevtcmd_GetUnitKind, 0xFFFFFFFC, 0xFE363C80,
    0x00020018, 0xFE363C80, 0x000000E4, 0x00010005,
    0x00000000, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFE363C8E,
    0x00000003, 0x00000001, 0x0005005B, (u32)(void*)btlevtcmd_GetPos,
    0xFE363C8E, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFE363C8E, 0x00000000,
    0xFE363C80, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFE363C8E,
    0x00000001, 0xFE363C81, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork,
    0xFE363C8E, 0x00000002, 0xFE363C82, 0x00010009,
    0x00000001, 0x00000006, 0x00000021, 0x00000021,
    0x00000070, 0x0000006E, 0x00020019, 0xFE363C8E,
    0xFFFFFFFF, 0x0004005B, (u32)(void*)btlevtcmd_AnimeChangePoseType, 0xFE363C8E,
    0xFE363C8F, 0x0000002C, 0x0005005B, (u32)(void*)get_ride_pos,
    0x00000001, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (u32)(void*)btlevtcmd_JumpSetting, 0xFE363C8E, 0x0000001E,
    0x00000000, 0xF24A7C19, 0x0007005B, (u32)(void*)btlevtcmd_JumpPosition,
    0xFE363C8E, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0000001E, 0xFFFFFFFF, 0x0004005B, (u32)(void*)btlevtcmd_AnimeChangePoseType,
    0xFE363C8E, 0xFE363C8F, 0x0000002B, 0x00000021,
    0x00000070, 0x00010009, 0x0000001E, 0x0001006F,
    0xFE363C8D, 0x00010005, 0x00000000, 0x00010009,
    0x00000001, 0x0005005B, (u32)(void*)get_ride_pos, 0x00000000,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00020035,
    0xFE363C82, 0xF24A8E80, 0x0005005B, (u32)(void*)btlevtcmd_SetPos,
    0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00020019, 0xFE363C8E, 0xFFFFFFFF, 0x0005005B,
    (u32)(void*)get_ride_pos, 0x00000001, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (u32)(void*)btlevtcmd_SetPos, 0xFE363C8E,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000021,
    0x00000006, 0x00000070, 0x0000006E, 0x0002005B,
    (u32)(void*)sac_cheer, 0x00000000, 0x00000070, 0x0001005B,
    (u32)(void*)sac_ac_help_on, 0x0001005B, (u32)(void*)start_game, 0x0001005B,
    (u32)(void*)wait_game_end, 0x0001005B, (u32)(void*)sac_ac_help_off, 0x0001005B,
    (u32)(void*)sac_cheer_end, 0x0001000A, 0x000003E8, 0x0001005B,
    (u32)(void*)star_stone_attack, 0x00010003, 0x0000005A, 0x0001005F,
    0xFE363C8D, 0x0000006B, 0x0001000A, 0x000003E8,
    0x0001005E, (u32)(void*)dmg_evt, 0x0000006D, 0x0001000A,
    0x000001F4, 0x0000006E, 0x0004005B, (u32)(void*)btlevtcmd_AnimeChangePoseType,
    0xFFFFFFFE, 0x00000001, 0x0000002C, 0x0005005B,
    (u32)(void*)btlevtcmd_GetHomePos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0005005B, (u32)(void*)btlevtcmd_JumpSetting, 0xFFFFFFFE,
    0x0000001E, 0x00000000, 0xF24A7C19, 0x0007005B,
    (u32)(void*)btlevtcmd_JumpPosition, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0000001E, 0xFFFFFFFF, 0x0004005B,
    (u32)(void*)btlevtcmd_AnimeChangePoseType, 0xFFFFFFFE, 0x00000001, 0x0000002B,
    0x00000070, 0x0000006E, 0x00020019, 0xFE363C8E,
    0xFFFFFFFF, 0x0004005B, (u32)(void*)btlevtcmd_AnimeChangePoseType, 0xFE363C8E,
    0xFE363C8F, 0x0000002C, 0x0005005B, (u32)(void*)btlevtcmd_GetHomePos,
    0xFE363C8E, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0005005B, (u32)(void*)btlevtcmd_JumpSetting, 0xFE363C8E, 0x0000001E,
    0x00000000, 0xF24A7C19, 0x0007005B, (u32)(void*)btlevtcmd_JumpPosition,
    0xFE363C8E, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0000001E, 0xFFFFFFFF, 0x0002005B, (u32)(void*)btlevtcmd_StartWaitEvent,
    0xFE363C8E, 0x00000021, 0x00000070, 0x00010009,
    0x0000001E, 0x0003005B, (u32)(void*)evt_btl_camera_set_mode, 0x00000000,
    0x00000000, 0x0002005B, (u32)(void*)btlevtcmd_SacBgsetMove, 0x00000001,
    0x00020019, 0xFE363C8E, 0xFFFFFFFF, 0x0003005B,
    (u32)(void*)btlevtcmd_GetUnitKind, 0xFFFFFFFC, 0xFE363C80, 0x00020018,
    0xFE363C80, 0x000000E4, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork,
    0xFE363C8E, 0x00000003, 0x00000000, 0x00000021,
    0x00000021, 0x0001005B, (u32)(void*)yuka_end, 0x0001005B,
    (u32)(void*)end_deka, 0x00010003, 0x00000063, 0x0002005B,
    (u32)(void*)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002, 0x00000001,
};

// .data | .obj dmg_evt
u32 dmg_evt[100] = {
    0x0002005B, (u32)(void*)get_result, 0xFE363C8A, 0x0003005B,
    (u32)(void*)btlevtcmd_GetSelectEnemy, 0xFE363C83, 0xFE363C84, 0x00010003,
    0x0000000A, 0x00020018, 0xFE363C83, 0xFFFFFFFF,
    0x00000002, 0x00000021, 0x0002001C, 0xFE363C8A,
    0x00000001, 0x0004005B, (u32)(void*)btlevtcmd_CheckAttribute, 0xFE363C83,
    0x00000002, 0xFE363C80, 0x00020019, 0xFE363C80,
    0x00000000, 0x00010004, 0x0000001E, 0x00000021,
    0x0004005B, (u32)(void*)btlevtcmd_CheckAttribute, 0xFE363C83, 0x00000004,
    0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0005005B, (u32)(void*)btlevtcmd_CheckPartsAttribute, 0xFE363C83, 0xFE363C84,
    0x00200000, 0xFE363C80, 0x00020019, 0xFE363C80,
    0x00000000, 0x00010004, 0x0000001E, 0x00000021,
    0x00000021, 0x00010004, 0x00000014, 0x00000021,
    0x0002001C, 0xFE363C8A, 0x00000003, 0x0004005B,
    (u32)(void*)btlevtcmd_CheckAttribute, 0xFE363C83, 0x00000002, 0xFE363C80,
    0x00020019, 0xFE363C80, 0x00000000, 0x00010004,
    0x0000001E, 0x00000021, 0x00010004, 0x00000014,
    0x00000021, 0x00010004, 0x00000014, 0x00010003,
    0x00000014, 0x0005005B, (u32)(void*)btlevtcmd_CheckPartsAttribute, 0xFE363C83,
    0xFE363C84, 0x00004000, 0xFE363C80, 0x00020018,
    0xFE363C80, 0x00000000, 0x0006005B, (u32)(void*)btlevtcmd_CommandCheckDamage,
    0xFFFFFFFD, 0xFE363C83, 0xFE363C84, 0x00000100,
    0xFE363C85, 0x00000021, 0x00010003, 0x0000001E,
    0x0003005B, (u32)(void*)btlevtcmd_GetSelectNextEnemy, 0xFE363C83, 0xFE363C84,
    0x00010004, 0x0000000A, 0x00000002, 0x00000001,
};



// ---- constant definitions (at end so uses emit symbol relocs) ----
