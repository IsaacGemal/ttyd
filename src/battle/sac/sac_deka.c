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
extern s32 dmg_evt[];
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
    /*0x04*/ s32 field_0x4;
    /*0x08*/ s32 kind;  // 1: a press in the window scores; other kinds boo
    /*0x0C*/ f32 time;
    /*0x10*/ Vec offset;  // added to the icon position; zeroed in state 2
    /*0x1C*/ u8 pad_0x1C[0x34 - 0x1C];
    /*0x34*/ u8 alpha;
    /*0x35*/ u8 flag;
    /*0x36*/ u8 pad_0x36[0x38 - 0x36];
} DekaTiming;

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
    /*0x048*/ s32 field_0x48;
    /*0x04C*/ f32 timingSlideY;  // -40 when timing icons appear, eased to 0 over 30 frames
    /*0x050*/ s32 gaugeState;
    /*0x054*/ s32 gaugeTimer;
    /*0x058*/ f32 gaugeWidth;
    /*0x05C*/ Vec gaugePos;
    /*0x068*/ u8 gaugeAlpha;
    /*0x069*/ u8 pad_0x69[0x6C - 0x69];
    /*0x06C*/ s32 sndHandle;
    /*0x070*/ DekaTiming timing[8];
    /*0x230*/ DekaTiming timingCopy[8];
    /*0x3F0*/ s32 starState;
    /*0x3F4*/ s32 starTimer;
    /*0x3F8*/ s32 starCounter;
    /*0x3FC*/ Vec starPos;
    /*0x408*/ Vec starPrev;
    /*0x414*/ Vec starTarget;
    /*0x420*/ u8 pad_0x420[0x424 - 0x420];
    /*0x424*/ f32 starVel;
    /*0x428*/ u8 pad_0x428[0x42C - 0x428];
    /*0x42C*/ Vec starScale;
    /*0x438*/ Vec starRot;
    /*0x444*/ f32 starScaleFrom;
    /*0x448*/ f32 starScaleTo;
    /*0x44C*/ EffectEntry* starEffect;
    /*0x450*/ s32 unitIds[10];
    /*0x478*/ Vec enemyVel[10];  // main_enemy_sub adds it to the enemy's position; gravity -1.25, the floor's vel.y pushes it up
    /*0x4F0*/ f32 enemyRotZVel[10];  // main_enemy_sub adds it to mRotation.z; +-20 when the rising floor hits the enemy
    /*0x518*/ u8 yukaActive[6];
    /*0x51E*/ u8 pad_0x51E[0x520 - 0x51E];
    /*0x520*/ f32 yukaWaveSpeed[6];  // columns per frame (+-2) a floor wave moves; its sign steps yukaIndex
    /*0x538*/ s32 yukaIndex[6];      // the next mesh column the wave kicks (stops at 0 and 0x80)
    /*0x550*/ f32 yukaWavePos[6];    // the column the wave has reached; columns are kicked until yukaIndex passes it
    /*0x568*/ f32 yukaWaveVelY[6];   // added to the vel.y of each column the wave passes
} DekaWork;

static DekaWork* get_ptr(void);

// Vertex data allocated by yuka_init into matEntry->address (0x3870 bytes).
typedef struct DekaYukaMesh {
    /*0x0000*/ Vec pos[2][0x81];
    /*0x0C18*/ Vec disp[2][0x81];
    /*0x1830*/ Vec vel[2][0x81];     // per-vertex velocity: yuka_main adds vel.y to pos.y, then gravity (-2)
    /*0x2448*/ Vec prev[2][0x81];
    /*0x3060*/ f32 texU[2][0x81];
    /*0x3468*/ f32 texV[2][0x81];
} DekaYukaMesh;

// EVT script, local to this file in the original (.sdata).
static EVT_BEGIN(nice_event)
    CALL(btlevtcmd_ACSuccessEffect, LW(0), 0, 100, 0)
    RETURN
EVT_END()

// .text:0x2CD0 | 0x80245CC8 | size: 0x130
s32 main_deka(EventEntry* event, s32 arg) {
    DekaWork* p = get_ptr();
    s32* args = event->args;

    if (arg != 0) {
        memset(p, 0, 0x580);
        p->field_0x48 = evtGetValue(event, args[0]);
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
            mesh->vel[i][j] = (Vec){ 0.0f, 0.0f, 0.0f };
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
            p->yukaWavePos[i] += p->yukaWaveSpeed[i];
            for (;;) {
                s32 idx = p->yukaIndex[i];
                s32 sc = (s32)p->yukaWavePos[i];

                if ((sc > idx && p->yukaWaveSpeed[i] > 0.0f) ||
                    (sc < idx && p->yukaWaveSpeed[i] < 0.0f)) {
                    for (j = 0; j < 2; j++) {
                        mesh->vel[j][p->yukaIndex[i]].y += p->yukaWaveVelY[i];
                    }
                    if (p->yukaWaveSpeed[i] > 0.0f) {
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

            PSVECAdd(&mesh->vel[i][j], &v2, &mesh->vel[i][j]);
            PSVECScale(&mesh->vel[i][j], &v8, 0.1f);
            PSVECAdd(&mesh->pos[i][j], &v8, &mesh->pos[i][j]);

            mesh->pos[i][j].y += mesh->vel[i][j].y;
            mesh->vel[i][j].y -= two;
            if (mesh->pos[i][j].y <= zero) {
                mesh->pos[i][j].y = zero;
                mesh->vel[i][j].y = zero;
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
            t->field_0x4 = 0;
            break;
        case 1:
        case 4:
        case 5:
            break;
    }
}

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
                        p->yukaWaveSpeed[i] = 2.0f;
                        if (j == 1) {
                            p->yukaWaveSpeed[i] *= -1.0f;
                        }
                        if (j == 0) {
                            p->yukaIndex[i] = 0x2A;
                        } else {
                            p->yukaIndex[i] = 0x29;
                        }
                        p->yukaWavePos[i] = 43.0f;
                        p->yukaWaveVelY[i] = (f32)p->starCounter * tblB[p->combo] + 4.0f;
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
        effStarStoneData* d = p->starEffect->userdata;

        d->x = p->starPos.x;
        d->y = p->starPos.y;
        d->z = p->starPos.z;
        d->tipRotY = p->starRot.x;
        d->rotY = p->starRot.y;
        d->tipRotZ = p->starRot.z;
        d->scale = 1.5f * p->starScale.x;
        d->tevReg0A = (u8)alpha;
    }
}

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
                    if (mesh->vel[0][j].y > 0.0f) {
                        BattleUnitPart* part;
                        s32 partId;

                        if (p->enemyVel[*count].y < 0.0f) {
                            p->enemyVel[*count].y = 0.0f;
                        }
                        unit->mPosition.y = groundY;
                        unit->mRotation.z = 0.0f;
                        p->enemyRotZVel[*count] = 0.0f;
                        p->enemyVel[*count].y += 0.5f * mesh->vel[0][j].y;

                        partId = BtlUnit_GetBodyPartsId(unit);
                        part = BtlUnit_GetPartsPtr(unit, partId);
                        if (!(part->attributes & kUnk_ImmuneToDamageOrStatus) && !(part->attributes & kUnknown_0x4000)) {
                            if (!(unit->mAttributeFlags & kInactive) && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
                                BtlUnit_SetAnimType(part, 0x27);
                            }
                            effSmallStarEntry(0, 2, cx, unit->mPosition.y, unit->mPosition.z, 0.0f, -1.0f, 0.0f);
                            BtlUnit_snd_se(unit, unit->mKindParams->damageSfxName, 0xF1194D80, 0);
                            if (!(unit->mAttributeFlags & 0x200000) && !(unit->mAttributeFlags & kInactive) && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
                                p->enemyRotZVel[*count] = 20.f;
                                if (irand(2) == 0) {
                                    p->enemyRotZVel[*count] *= -1.0f;
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
    unit->mRotation.z += p->enemyRotZVel[*count];

    if (unit->mPosition.y < unit->mHomePosition.y) {
        unit->mPosition.y = unit->mHomePosition.y;
        p->enemyVel[*count].y = 0.0f;
        if (!(unit->mAttributeFlags & kInactive) && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
            unit->mRotation.z = 0.0f;
            p->enemyRotZVel[*count] = 0.0f;
        }
    }

    p->unitIds[*count] = BtlUnit_GetUnitId(unit);
    (*count)++;
}

// .text:0x1158 | 0x80244150 | size: 0x48
s32 end_deka(EventEntry* event) {
    DekaWork* p = get_ptr();
    effStarStoneData* data = p->starEffect->userdata;
    effDelete(data->dustEffect);
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

    C_MTXLookAt(cam->view, &cam->cameraPos, &cam->cameraUp, &cam->cameraAt);
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

// .text:0x5D8 | 0x802435D0 | size: 0xC
static DekaWork* get_ptr(void) {
    return (DekaWork*)&_battleWorkPointer->actionCommands.acWork[0];
}

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
EVT_BEGIN(marioAttackEvent_Deka)
    CALL(btlevtcmd_GetUnitId, -4, LW(14))
    CALL(btlevtcmd_GetBodyId, LW(14), LW(15))
    CALL(btlevtcmd_CheckActStatus, LW(14), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        SET_VALUE(LW(14), -1)
    END_IF
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(0))
    CALL(btlevtcmd_reset_move_color_lv_all)
    BEGIN_SIBLING_EVENT
        CALL(main_deka, LW(14))
    END_SIBLING_EVENT
    CALL_CHILD(sac_start_camera_evt)
    WAIT_MS(500)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_START1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL_CHILD(sac_mario_item_on_evt)
    WAIT_MS(500)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_START2"), EVTDAT_POINTER_MAX, 0, LW(11))
    CALL(star_stone_appear)
    BEGIN_SIBLING_EVENT
        CALL_CHILD(sac_start_stardust)
    END_SIBLING_EVENT
    WAIT_FRAMES(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_START3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_snd_sfxoff, LW(11))
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(40)
    CALL_CHILD(sac_mario_item_on_end_evt)
    CALL(btlevtcmd_SacBgsetMove, 0)
    CALL_CHILD(sac_mario_back_home)
    CALL(wait_star_stone_take_on)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 44)
        CALL(get_ride_pos, 0, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpSetting, -2, 30, 0, FLOAT(0.4f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_I_Y"))
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        IF_INT_NOT_EQUAL(LW(14), -1)
            CALL(btlevtcmd_GetUnitKind, -4, LW(0))
            IF_INT_EQUAL(LW(0), 228)
                LOOP(0)
                    CALL(btlevtcmd_SetUnitWork, LW(14), 3, 1)
                    CALL(btlevtcmd_GetPos, LW(14), LW(0), LW(1), LW(2))
                    CALL(btlevtcmd_SetUnitWork, LW(14), 0, LW(0))
                    CALL(btlevtcmd_SetUnitWork, LW(14), 1, LW(1))
                    CALL(btlevtcmd_SetUnitWork, LW(14), 2, LW(2))
                    WAIT_FRAMES(1)
                END_LOOP
            END_IF
        END_IF
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        IF_INT_NOT_EQUAL(LW(14), -1)
            CALL(btlevtcmd_AnimeChangePoseType, LW(14), LW(15), 44)
            CALL(get_ride_pos, 1, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_JumpSetting, LW(14), 30, 0, FLOAT(0.4f))
            CALL(btlevtcmd_JumpPosition, LW(14), LW(0), LW(1), LW(2), 30, -1)
            CALL(btlevtcmd_AnimeChangePoseType, LW(14), LW(15), 43)
        END_IF
    END_SIBLING_EVENT
    WAIT_FRAMES(30)
    BEGIN_SIBLING_EVENT_GET_ID(LW(13))
        LOOP(0)
            WAIT_FRAMES(1)
            CALL(get_ride_pos, 0, LW(0), LW(1), LW(2))
            ADD(LW(2), FLOAT(5.0f))
            CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
            IF_INT_NOT_EQUAL(LW(14), -1)
                CALL(get_ride_pos, 1, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetPos, LW(14), LW(0), LW(1), LW(2))
            END_IF
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(sac_cheer, 0)
    END_SIBLING_EVENT
    CALL(sac_ac_help_on)
    CALL(start_game)
    CALL(wait_game_end)
    CALL(sac_ac_help_off)
    CALL(sac_cheer_end)
    WAIT_MS(1000)
    CALL(star_stone_attack)
    LABEL(90)
    DELETE_EVENT(LW(13))
    BEGIN_INLINE_EVENT
        WAIT_MS(1000)
        CALL_CHILD(dmg_evt)
    END_INLINE_EVENT
    WAIT_MS(500)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 44)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpSetting, -2, 30, 0, FLOAT(0.4f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        IF_INT_NOT_EQUAL(LW(14), -1)
            CALL(btlevtcmd_AnimeChangePoseType, LW(14), LW(15), 44)
            CALL(btlevtcmd_GetHomePos, LW(14), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_JumpSetting, LW(14), 30, 0, FLOAT(0.4f))
            CALL(btlevtcmd_JumpPosition, LW(14), LW(0), LW(1), LW(2), 30, -1)
            CALL(btlevtcmd_StartWaitEvent, LW(14))
        END_IF
    END_SIBLING_EVENT
    WAIT_FRAMES(30)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_SacBgsetMove, 1)
    IF_INT_NOT_EQUAL(LW(14), -1)
        CALL(btlevtcmd_GetUnitKind, -4, LW(0))
        IF_INT_EQUAL(LW(0), 228)
            CALL(btlevtcmd_SetUnitWork, LW(14), 3, 0)
        END_IF
    END_IF
    CALL(yuka_end)
    CALL(end_deka)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// .data | .obj dmg_evt
EVT_BEGIN(dmg_evt)
    CALL(get_result, LW(10))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    LABEL(10)
    IF_INT_EQUAL(LW(3), -1)
        RETURN
    END_IF
    IF_INT_LESS_OR_EQUAL(LW(10), 1)
        CALL(btlevtcmd_CheckAttribute, LW(3), 2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            GOTO(30)
        END_IF
        CALL(btlevtcmd_CheckAttribute, LW(3), 4, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_CheckPartsAttribute, LW(3), LW(4), 2097152, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                GOTO(30)
            END_IF
        END_IF
        GOTO(20)
    END_IF
    IF_INT_LESS_OR_EQUAL(LW(10), 3)
        CALL(btlevtcmd_CheckAttribute, LW(3), 2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            GOTO(30)
        END_IF
        GOTO(20)
    END_IF
    GOTO(20)
    LABEL(20)
    CALL(btlevtcmd_CheckPartsAttribute, LW(3), LW(4), 16384, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CommandCheckDamage, -3, LW(3), LW(4), 256, LW(5))
    END_IF
    LABEL(30)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    GOTO(10)
    RETURN
EVT_END()



// ---- constant definitions (at end so uses emit symbol relocs) ----
