#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_audience.h"
#include "battle/battle_event_cmd.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/mapdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "mario/mario_pouch.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <string.h>

extern BattleWork* _battleWorkPointer;

typedef struct SukiButton {
    s32 state;   // 0x0
    s32 field_4; // 0x4
    s32 button;  // 0x8
    Vec pos;     // 0xC
} SukiButton;

typedef struct SukiWork {
    s32 state;               // 0x0
    s32 counter;             // 0x4
    s32 field_8;             // 0x8
    s32 count;               // 0xC
    s32 selected;            // 0x10
    s32 prevSelected;        // 0x14
    s32 max;                 // 0x18
    s32 timer;               // 0x1C
    u8 flash;                // 0x20
    u8 pad_21[3];            // 0x21
    f32 alpha;               // 0x24
    Vec table[6];            // 0x28
    char img[6][0x10];       // 0x70
    Vec pos;                 // 0xD0
    SukiButton buttons[7];   // 0xDC
    SukiButton buttons2[7];  // 0x184
    u32 starState;           // 0x22C
    s32 starCounter;         // 0x230
    Vec starCur;             // 0x234
    Vec starStart;           // 0x240
    Vec starEnd;             // 0x24C
    u8 field_258[0xC];       // 0x258
    Vec scale1;              // 0x264
    Vec scale2;              // 0x270
    EffectEntry* effect;     // 0x27C
    s32 flag;                // 0x280
    u8 weapon[0xC0];         // 0x284
    u8 field_344[0x4C8 - 0x344]; // 0x344
} SukiWork;

typedef struct SukiTevParam {
    u8 pad[0xC];
    s32 field_C;   // 0xC
    s32 field_10;  // 0x10
    s32 field_14;  // 0x14
} SukiTevParam;

extern Vec dat_8030096c[6];
extern Vec vec3_803009f4;
extern Vec vec3_803009b4;
extern Vec vec3_80300a00;
extern Vec vec3_80300a0c;
extern Vec vec3_80300a78;
extern Vec vec3_80300a84;
extern Vec vec3_80300a90;
extern u8 N_ohko_rate_tbl[6];
extern u8 weapon[];
extern s32 nice_event[];

static SukiWork* get_ptr(void);
static void main_base(void);
static void main_star(void);
static void disp_2D(CameraId camera, void* param);
static void set_weapon(EventEntry* event);
static void star_stone_attack(EventEntry* event);
static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall);
static s32 wait_game_end(EventEntry* event, BOOL isFirstCall);
static s32 start_game(EventEntry* event, BOOL isFirstCall);
static void disp_3D_alpha(CameraId camera, void* param);
static void disp_3D(CameraId camera, void* param);

EffectEntry* effStarStoneEntry(s32 a, f32 b, f32 c, f32 d, f32 e);
EffectEntry* effStardustN64Entry(s32 a, f32 b, f32 c, f32 d, f32 e);
EffectEntry* effCrystalN64Entry(s32 a, s32 b, f32 c, f32 d, f32 e, f32 f);
void btlGetScreenPoint(Vec* pos, Vec* out);
void BattleAudienceSoundCallKind(s32 kind);
void BattleAudienceSoundWhistleKind(s32 kind);
void evt_btl_camera_set_mode(void);
void evt_btl_camera_set_moveSpeedLv(void);
void evt_batstage_set_stg_dark(void);
void evt_batstage_set_aud_dark(void);
void evt_batstage_return_stg_dark_base(void);
void evt_batstage_return_aud_dark_base(void);
void tou_gamen_screen_tev(void);
void evt_paper_delete(void);
void mapSetTevCallback(s32 idx, void* cb);

// .text:0x0 | 0x80249920 | size: 0xE8
void set_tev(SukiTevParam* param) {
    SukiWork* work = get_ptr();
    u8 flash = work->flash;
    GXColor color = { 0xFF, 0, 0, 0 };

    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);

    color.r = flash;
    color.g = flash;
    color.b = flash;
    GXSetTevColor(GX_TEVREG1, color);

    param->field_C = 1;
    param->field_10 = 0;
    param->field_14 = 0;
}

// .text:0xE8 | 0x80249A08 | size: 0x5C
s32 get_img_name(EventEntry* event) {
    s32* args = event->args;
    s8 index = (s8)evtGetValue(event, args[0]);
    SukiWork* work = get_ptr();

    evtSetValue(event, args[1], (s32)(work->img[index]));
    return EVT_RETURN_DONE;
}

// .text:0x144 | 0x80249A64 | size: 0x84
s32 eff_crystal(EventEntry* event) {
    s32* args = event->args;
    f32 f1;
    f32 f2;
    EffectEntry* effect;

    f1 = evtGetFloat(event, args[0]);
    f2 = evtGetFloat(event, args[1]);
    effect = effCrystalN64Entry(6, 0x3C, f1, f2, 30.0f, 1.0f);
    *(s32*)((u8*)effect->userdata + 0x38) = 7;
    return EVT_RETURN_DONE;
}

// .text:0x1C8 | 0x80249AE8 | size: 0x88
s32 get_screen_point(EventEntry* event) {
    s32* args = event->args;
    Vec pos;

    pos.x = evtGetFloat(event, args[0]);
    pos.y = evtGetFloat(event, args[1]);
    pos.z = evtGetFloat(event, args[2]);
    btlGetScreenPoint(&pos, &pos);
    evtSetFloat(event, args[3], pos.x);
    evtSetFloat(event, args[4], pos.y);
    return EVT_RETURN_DONE;
}

// .text:0x250 | 0x80249B70 | size: 0x68
s32 exist_map(EventEntry* event) {
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);

    if (mapGetMapObj((const char*)value) != NULL) {
        evtSetValue(event, args[1], 1);
    }
    else {
        evtSetValue(event, args[1], 0);
    }
    return EVT_RETURN_DONE;
}

// .text:0x2B8 | 0x80249BD8 | size: 0x40
s32 flash_update(EventEntry* event) {
    s32* args = event->args;
    u8 value = evtGetValue(event, args[0]);
    SukiWork* work = get_ptr();

    work->flash = value;
    return EVT_RETURN_DONE;
}

// .text:0x2F8 | 0x80249C18 | size: 0x68
s32 aud_set_draw(EventEntry* event) {
    BattleAudience* aud = BattleAudienceBaseGetPtr();
    s32* args = event->args;

    if (evtGetValue(event, args[0]) == 0) {
        aud->flags |= 0x20000;
    }
    else {
        aud->flags &= ~0x1C000;
    }
    return EVT_RETURN_DONE;
}

// .text:0x360 | 0x80249C80 | size: 0x84
static void set_weapon(EventEntry* event) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    SukiWork* work = get_ptr();
    u8 tbl[6];

    memcpy(tbl, N_ohko_rate_tbl, 6);
    memcpy(work->weapon, weapon, 0xC0);
    work->weapon[0x9E] = tbl[index - 1];
    evtSetValue(event, args[1], (s32)work->weapon);
}

// .text:0x3E4 | 0x80249D04 | size: 0x38
s32 wait_star_stone_attack_end(EventEntry* event, BOOL isFirstCall) {
    SukiWork* work = get_ptr();

    return (work->starState < 5) ? 2 : 0;
}

// .text:0x41C | 0x80249D3C | size: 0xCC
static void star_stone_attack(EventEntry* event) {
    SukiWork* work = get_ptr();
    s32 index = evtGetValue(event, event->args[0]);
    CameraEntry* cam;

    work->starState = 9;
    work->starCounter = 0;
    work->starStart = work->table[index];
    work->starCur = work->starStart;
    cam = camGetPtr(CAMERA_3D);
    work->starEnd = cam->cameraPos;
    work->starEnd.y += 20.0f;
    work->starEnd.z += 10.0f;
}

// .text:0x4E8 | 0x80249E08 | size: 0x84
s32 star_stone_get_attack_point(EventEntry* event) {
    s32* args = event->args;
    SukiWork* work;
    s32 index;

    work = get_ptr();
    index = evtGetValue(event, args[0]);
    evtSetFloat(event, args[1], work->table[index].x);
    evtSetFloat(event, args[2], work->table[index].y);
    evtSetFloat(event, args[3], work->table[index].z);
    return EVT_RETURN_DONE;
}

// .text:0x56C | 0x80249E8C | size: 0x38
s32 wait_star_stone_up_end(EventEntry* event, BOOL isFirstCall) {
    SukiWork* work = get_ptr();

    return (work->starState != 5) ? 2 : 0;
}

// .text:0x5A4 | 0x80249EC4 | size: 0x2C
static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall) {
    SukiWork* work = get_ptr();

    work->starState = 1;
    return 2;
}

// .text:0x5D0 | 0x80249EF0 | size: 0x5C
static s32 wait_game_end(EventEntry* event, BOOL isFirstCall) {
    SukiWork* work = get_ptr();
    s32* args = event->args;

    if (work->state == 5) {
        evtSetValue(event, args[0], work->count + 1);
        return 2;
    }
    return 0;
}

// .text:0x62C | 0x80249F4C | size: 0x2C
static s32 start_game(EventEntry* event, BOOL isFirstCall) {
    SukiWork* work = get_ptr();

    work->state = 1;
    return 2;
}

// .text:0x658 | 0x80249F78 | size: 0xC
// Kept out of line: the original calls get_ptr rather than inlining it.
#pragma dont_inline on
static SukiWork* get_ptr(void) {
    return (SukiWork*)((u8*)_battleWorkPointer + 0x1F4C);
}
#pragma dont_inline reset

// .text:0x664 | 0x80249F84 | size: 0x4
static void disp_3D_alpha(CameraId camera, void* param) {
}

// .text:0x668 | 0x80249F88 | size: 0x4
static void disp_3D(CameraId camera, void* param) {
}

// .text:0x66C | 0x80249F8C | size: 0x1F0
void N__disp_2D(s32 type, s32 index) {
    SukiWork* work = get_ptr();
    SukiButton* btn;
    s32 sel;
    f32 yoff;
    u16 icon = 0;

    if (type == 0) {
        btn = &work->buttons2[index];
        sel = work->prevSelected;
        yoff = 80.0f;
    }
    else {
        btn = &work->buttons[index];
        sel = work->selected;
        yoff = 0.0f;
    }

    if (type == 0 && btn->state == 0) {
        return;
    }
    if (type == 1 && btn->state != 2) {
        return;
    }

    switch (btn->button) {
    case 0x10: icon = (index < sel) ? 0x82 : 0x83; break;
    case 0x20: icon = (index < sel) ? 0x80 : 0x81; break;
    case 0x40: icon = (index < sel) ? 0x7E : 0x7F; break;
    case 0x80: icon = (index < sel) ? 0x68 : 0x69; break;
    case 0x100: icon = (index < sel) ? 0x64 : 0x65; break;
    case 0x200: icon = (index < sel) ? 0x66 : 0x67; break;
    case 0x400: icon = (index < sel) ? 0x6A : 0x6B; break;
    case 0x800: icon = (index < sel) ? 0x7E : 0x7F; break;
    }
    if (icon == 0) {
        return;
    }
    {
        Vec pos = vec3_80300a90;
        pos.x = btn->pos.x;
        pos.y = work->alpha + btn->pos.y + yoff + 60.0f;
        pos.z = btn->pos.z;
        iconDispGx(pos, 0x10, icon, 1.0f);
    }
}

// .text:0x85C | 0x8024A17C | size: 0x180
static void disp_2D(CameraId camera, void* param) {
    SukiWork* work = get_ptr();
    Mtx mtx;
    s32 i;
    Vec pos;

    PSMTXScale(mtx, 26.0f, 1.0f, 1.0f);
    PSMTXTransApply(mtx, mtx, work->pos.x, work->pos.y + 60.0f, work->pos.z);
    iconDispGx2(mtx, 0x10, 0x8E);

    pos = vec3_80300a78;
    pos.x = work->pos.x + 215.0f;
    pos.y = work->pos.y + 60.0f;
    pos.z = work->pos.z;
    iconDispGx(pos, 0x10, 0x90, 1.0f);

    pos = vec3_80300a84;
    pos.x = work->pos.x - 215.0f;
    pos.y = work->pos.y + 60.0f;
    pos.z = work->pos.z;
    iconDispGx(pos, 0x10, 0x8F, 1.0f);

    GXSetScissor(0, 0x91, gp->fbWidth, 0x50);
    for (i = 0; i < 7; i++) {
        N__disp_2D(0, i);
        N__disp_2D(1, i);
    }
    GXSetScissor(0, 0, gp->fbWidth, gp->efbHeight);
}

// .text:0x9DC | 0x8024A2FC | size: 0x48
s32 end_suki(EventEntry* event, BOOL isFirstCall) {
    SukiWork* work = get_ptr();

    effDelete(*(EffectEntry**)((u8*)work->effect->userdata + 0x34));
    effDelete(work->effect);
    return 2;
}

// .text:0xA24 | 0x8024A344 | size: 0x5BC
static void main_star(void) {
    SukiWork* work = get_ptr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 f;

    switch (work->starState) {
    case 0:
        break;
    case 1:
        work->starState = 2;
        work->starCounter = 0;
        work->effect = effStarStoneEntry(5, 0.0f, -1000.0f, 0.0f, 1.0f);
        BtlUnit_GetPos(mario, &work->starStart.x, &work->starStart.y, &work->starStart.z);
        f = (f32)mario->mHeight;
        work->starStart.y += f * mario->mSizeMultiplier + 37.0f;
        work->starCur = work->starStart;
        work->starEnd = work->starStart;
        work->scale1 = vec3_80300a00;
        work->scale2 = vec3_80300a0c;
        work->flag = 4;
        /* fallthrough */
    case 2:
        work->starCounter++;
        if (work->starCounter <= 0x64) {
            work->starCur.x = intplGetValue(0, work->starCounter, 0x64, work->starStart.x, work->starEnd.x);
            work->starCur.y = intplGetValue(0, work->starCounter, 0x64, work->starStart.y, work->starEnd.y);
            work->starCur.z = intplGetValue(0, work->starCounter, 0x64, work->starStart.z, work->starEnd.z);
            f = intplGetValue(0, work->starCounter, 0x64, 0.0f, 2.0f);
            work->scale2 = *(Vec*)&f;
            work->scale1 = *(Vec*)&f;
        }
        else {
            work->starCur = work->starEnd;
            f = 2.0f;
            work->scale2 = *(Vec*)&f;
            work->scale1 = *(Vec*)&f;
        }
        work->scale2.x = work->scale2.y = work->scale2.z = f;
        work->scale1.x = work->scale1.y = work->scale1.z = f;
        break;
    default:
        break;
    }
    (void)mario;
}

// .text:0xFE0 | 0x8024A900 | size: 0x114
void main_button(s32 index) {
    SukiWork* work = get_ptr();
    SukiButton* btn = &work->buttons[index];
    static const s32 maskTbl[4] = { 0x100, 0x200, 0x400, 0x800 };

    if (btn->state == 1) {
        btn->state = 2;
        btn->pos = vec3_803009f4;
        btn->pos.x = 30.0f * (work->max - 1) - 60.0f * (f32)index;
        btn->button = maskTbl[irand(4)];
    }
}

// .text:0x10F4 | 0x8024AA14 | size: 0x470
static void main_base(void) {
    SukiWork* work = get_ptr();
    s32 audNum = BattleAudience_GetAudienceNum();
    s32 i;

    switch (work->state) {
    case 1:
        work->state = 2;
        work->counter = 0;
        /* fallthrough */
    case 2:
        if (work->count == 0) {
            Vec v = *(Vec*)&dat_8030096c[0];
            v.y = intplGetValue(4, work->counter, 0xF, -1000.0f, 0.0f);
            work->pos = v;
        }
        if (work->counter == 0) {
            if (work->starState == 4) {
                memcpy(work->buttons2, work->buttons, 0xA8);
                work->prevSelected = work->selected;
                if (work->count < 5) {
                    work->selected = 0;
                    work->max = work->count + 3;
                    work->timer = work->count * 10 + 0xB4;
                    for (i = 0; i < 7; i++) {
                        work->buttons[i].state = (i < work->max) ? 1 : 0;
                    }
                }
                else {
                    for (i = 0; i < 7; i++) {
                        work->buttons[i].state = 0;
                    }
                }
            }
        }
        if (work->counter == 0) {
        }
        work->alpha = intplGetValue(0, work->counter, 0xF, -80.0f, 0.0f);
        work->counter++;
        if (work->counter >= 0xF) {
            if (work->count >= 5) {
                work->state = 5;
                work->counter = 0;
                work->starState = 8;
            }
            else {
                work->state = 4;
            }
            work->pos = vec3_803009b4;
        }
        break;
    case 3:
        if (work->counter == 0) {
            if (work->starState == 4) {
                psndSFXOn((const char*)0x80300A9C);
                memcpy(work->buttons2, work->buttons, 0xA8);
                work->prevSelected = work->selected;
                for (i = 0; i < 7; i++) {
                    work->buttons[i].state = 0;
                }
            }
        }
        work->alpha = intplGetValue(0, work->counter, 0xF, -60.0f, 0.0f);
        work->counter++;
        if (work->counter >= 0xF) {
            work->state = 5;
            work->counter = 0;
            work->starState = 8;
        }
        break;
    case 4:
        work->timer--;
        if (work->timer <= 0) {
            work->state = 3;
            work->counter = 0;
        }
        else if ((keyGetButtonTrg(0) & 0xF00000) != 0) {
            if ((work->buttons[work->selected].button & keyGetButtonTrg(0)) != 0) {
                EventEntry* evt;
                work->selected++;
                psndSFXOn((const char*)0x80300B78);
                if (work->selected >= work->max) {
                    work->state = 2;
                    work->counter = 0;
                    evt = evtEntry(nice_event, 0, 0x20);
                    work->count++;
                    work->starState = 5;
                    evt->lwData[0] = (work->count + 1 < 6) ? work->count + 1 : 6;
                    if (audNum >= 1 && audNum <= 0x31) {
                        BattleAudienceSoundCallKind(1);
                    }
                    if (audNum >= 0x32 && audNum <= 0x63) {
                        BattleAudienceSoundCallKind(1);
                        BattleAudienceSoundWhistleKind(1);
                    }
                    if (audNum >= 0x64 && audNum <= 0x95) {
                        BattleAudienceSoundCallKind(2);
                        BattleAudienceSoundWhistleKind(2);
                    }
                    if (audNum >= 0x96) {
                        BattleAudienceSoundCallKind(2);
                        BattleAudienceSoundWhistleKind(3);
                    }
                }
            }
            else {
                work->state = 3;
                work->counter = 0;
            }
        }
        break;
    case 5:
        work->counter++;
        if (work->counter <= 0x1E) {
            Vec v = *(Vec*)&dat_8030096c[0];
            v.y = intplGetValue(4, work->counter, 0x1E, 0.0f, -1000.0f);
            work->pos = v;
        }
        break;
    default:
        break;
    }
}

// .text:0x1564 | 0x8024AE84 | size: 0x174
s32 main_suki(EventEntry* event, BOOL isFirstCall) {
    SukiWork* work = get_ptr();
    s32* args = event->args;
    Vec table[6] = {
        { 95.0f, 35.0f, -150.0f },
        { 180.0f, 80.0f, -150.0f },
        { 95.0f, 110.0f, -150.0f },
        { 10.0f, 80.0f, -150.0f },
        { 30.0f, -10.0f, -150.0f },
        { 160.0f, -10.0f, -150.0f },
    };
    s32 i;

    if (isFirstCall) {
        memset(work, 0, 0x4C8);
        work->field_8 = evtGetValue(event, args[0]);
        memcpy(work->table, table, 0x48);
        strcpy(work->img[0], "sac_suki_img0");
        strcpy(work->img[1], "sac_suki_img1");
        strcpy(work->img[2], "sac_suki_img2");
        strcpy(work->img[3], "sac_suki_img3");
        strcpy(work->img[4], "sac_suki_img4");
        strcpy(work->img[5], "sac_suki_img5");
        work->scale1 = vec3_80300a00;
        work->scale2 = vec3_80300a0c;
        work->pos = vec3_803009b4;
    }

    main_base();
    for (i = 0; i < 7; i++) {
        main_button(i);
    }
    main_star();
    dispEntry(CAMERA_2D, 901.0f, 8, disp_2D, NULL);
    dispEntry(CAMERA_3D, 0.0f, 4, disp_3D, NULL);
    dispEntry(CAMERA_3D, 0.0f, 4, disp_3D_alpha, NULL);
    return 0;
}
