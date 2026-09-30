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
#include "eff/eff_starstone.h"
#include "battle/battle_stage.h"
#include "battle/sac/sac_suki.h"
#include "eff/n64/eff_stardust_n64.h"
#include "eff/n64/eff_crystal_n64.h"
#include "battle/battle_disp.h"
#include "evt/evt_paper.h"

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
    s32 starState;           // 0x22C
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

typedef struct SukiBytes6 {
    u8 x[6];
} SukiBytes6;

typedef struct SukiTevParam {
    u8 pad[0xC];
    s32 field_C;   // 0xC
    s32 field_10;  // 0x10
    s32 field_14;  // 0x14
} SukiTevParam;

// File .rodata, in the original object's address order (main_suki reaches dat_8030096c at +0x3C,
// vec3_803009b4 at +0x84 and the img strings at +0x260 through the str_80300930 base).
const char str_80300930[0x1C] = "btl_wn_sac_sukkari_sukkiri";
const char str_8030094C[0x14] = "msg_sukkari_sukkiri";
const char str_80300960[0xC] = "msg_sac_7";
__declspec(section ".rodata") Vec dat_8030096c[6] = {
    {95.0f, 35.0f, -150.0f},
    {180.0f, 80.0f, -150.0f},
    {95.0f, 110.0f, -150.0f},
    {10.0f, 80.0f, -150.0f},
    {30.0f, -10.0f, -150.0f},
    {160.0f, -10.0f, -150.0f},
};
__declspec(section ".rodata") Vec vec3_803009b4 = {0.0f, -1000.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_803009c0 = {0.0f, -45.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_803009cc = {0.0f, -45.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_803009d8 = {0.0f, -45.0f, 0.0f};
const u32 dat_803009e4[4] = {0x100, 0x200, 0x400, 0x800};
__declspec(section ".rodata") Vec vec3_803009f4 = {0.0f, -40.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_80300a00 = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_80300a0c = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_80300a18 = {0.0f, 300.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_80300a24 = {2.0f, 2.0f, 2.0f};
__declspec(section ".rodata") Vec vec3_80300a30 = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_80300a3c = {1.5f, 1.5f, 1.5f};
__declspec(section ".rodata") Vec vec3_80300a48 = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_80300a54 = {0.0f, 300.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_80300a60 = {2.0f, 2.0f, 2.0f};
__declspec(section ".rodata") Vec vec3_80300a6c = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_80300a78 = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_80300a84 = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") Vec vec3_80300a90 = {0.0f, 0.0f, 0.0f};
const char str_80300A9C[0x14] = "SFX_BTL_SAC_START1";
const char str_80300AB0[0x14] = "SFX_BTL_SAC_START2";
const char str_80300AC4[0x14] = "SFX_BTL_SAC_START3";
const char str_80300AD8[0x14] = "OFF_d_yabu_small";
const char str_80300AEC[0xC] = "indirect";
const char str_80300AF8[0x18] = "SFX_BTL_SAC_SUKKIRI4";
const char str_80300B10[0x18] = "SFX_BTL_SAC_SUKKIRI4_2";
const char str_80300B28[0x18] = "SFX_BTL_SAC_SUKKIRI5";
const char str_80300B40[0x18] = "SFX_BTL_SAC_SUKKIRI3";
const double double_to_int_80300b58 = 4503601774854144.0;
const char str_80300B60[0x18] = "SFX_BTL_SAC_SUKKIRI2";
const char str_80300B78[0x18] = "SFX_BTL_SAC_SUKKIRI1";
const char str_80300B90[0x10] = "sac_suki_img0";
const char str_80300BA0[0x10] = "sac_suki_img1";
const char str_80300BB0[0x10] = "sac_suki_img2";
const char str_80300BC0[0x10] = "sac_suki_img3";
const char str_80300BD0[0x10] = "sac_suki_img4";
const char str_80300BE0[0x10] = "sac_suki_img5";
extern u8 N_ohko_rate_tbl[6];
extern const f32 float_1_804220d8;
extern const f32 float_0_804220e8;
extern const f32 float_60_804220ec;
extern const f32 float_80_804220e4;
extern const f32 float_neg1000_804220f8;
extern const f32 float_neg80_8042212c;
extern const f32 float_neg60_80422130;
extern u8 weapon[];
static u32 nice_event[8] = {
    0x0005005B, (u32)btlevtcmd_ACSuccessEffect, 0xFE363C80, 0xFFFFFF88,
    0x00000078, 0,                     2,          1,
};
extern const GXColor dat_804220a0;

static SukiWork* get_ptr(void);
static void main_base(void);
static void main_star(void);
static void disp_2D(CameraId camera, void* param);
static s32 set_weapon(EventEntry* event, BOOL isFirstCall);
static s32 star_stone_attack(EventEntry* event, BOOL isFirstCall);
static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall);
static s32 wait_game_end(EventEntry* event, BOOL isFirstCall);
static s32 start_game(EventEntry* event, BOOL isFirstCall);
static void disp_3D_alpha(CameraId camera, void* param);
static void disp_3D(CameraId camera, void* param);

#ifdef MUST_MATCH
// MUST_MATCH: a 6x Vec struct assignment makes MWCC emit the original's inline 9-iteration copy
// loop; a plain memcpy over a Vec[6] emits a real `bl memcpy`.
typedef struct SukiTable6 {
    Vec v[6];
} SukiTable6;
#endif

// .text:0x5BC | 0x8024AE84 | size: 0x174
s32 main_suki(EventEntry* event, BOOL isFirstCall) {
    SukiWork* work = get_ptr();
    s32* args = event->args;
#ifdef MUST_MATCH
    SukiTable6 table = *(SukiTable6*)dat_8030096c;
#else
    Vec table[6];
#endif
    s32 i;

#ifndef MUST_MATCH
    memcpy(table, dat_8030096c, 0x48);
#endif

    if (isFirstCall) {
        memset(work, 0, sizeof(SukiWork));
        work->field_8 = evtGetValue(event, args[0]);
        memcpy(work->table, &table, 0x48);
        strcpy(work->img[0], str_80300B90);
        strcpy(work->img[1], str_80300BA0);
        strcpy(work->img[2], str_80300BB0);
        strcpy(work->img[3], str_80300BC0);
        strcpy(work->img[4], str_80300BD0);
        strcpy(work->img[5], str_80300BE0);
        work->pos = vec3_803009b4;
    }
    main_base();
    for (i = 0; i < 7; i++) {
        main_button(i);
    }
    main_star();
    dispEntry(8, 901.0f, 1, disp_2D, NULL);
    dispEntry(4, 0.0f, 1, disp_3D, NULL);
    dispEntry(4, 0.0f, 2, disp_3D_alpha, NULL);
    return 0;
}

// .text:0x1230 | 0x8024AA14 | size: 0x470
// The three intplGetValue argument setups differ in instruction scheduling: the original keeps the
// `lwz r4, work->counter` load as the last instruction before the call (after the two lfs of the float
// bounds), and the case-2 Vec copy uses r6/r4/r0 instead of r4/r6/r0. Everything else is verified.
// NONMATCHING (97.62%): intplGetValue arg scheduling (counter load position) and case-2 Vec registers.
static void main_base(void) {
    SukiWork* work = get_ptr();
    s32 audienceNum = BattleAudience_GetAudienceNum();
    s32 i;

    switch (work->state) {
    case 1:
        work->state = 2;
        work->counter = 0;
        // fall through
    case 2:
        if (work->count == 0) {
            Vec pos = vec3_803009c0;

            pos.x = intplGetValue(4, work->counter, 15, float_neg1000_804220f8, float_0_804220e8);
            work->pos = pos;
        }
        if (work->counter == 0) {
            if (work->starState != 4) {
                return;
            }
            memcpy(work->buttons2, work->buttons, 0xA8);
            work->prevSelected = work->selected;
            if (work->count < 5) {
                work->selected = 0;
                work->max = work->count + 3;
                work->timer = work->count * 10 + 0xB4;
                for (i = 0; i < 7; i++) {
                    SukiButton* button = &work->buttons[i];

                    if (i < work->max) {
                        button->state = 1;
                    }
                    else {
                        button->state = 0;
                    }
                }
            }
            else {
                for (i = 0; i < 7; i++) {
                    work->buttons[i].state = 0;
                }
            }
        }
        work->alpha = intplGetValue(0, work->counter, 15, float_neg80_8042212c, float_0_804220e8);
        work->counter++;
        if (work->counter >= 15) {
            if (work->count >= 5) {
                work->state = 5;
                work->counter = 0;
                work->starState = 8;
            }
            else {
                work->state = 4;
            }
            work->pos = vec3_803009cc;
        }
        break;
    case 3:
        if (work->counter == 0) {
            if (work->starState != 4) {
                return;
            }
            psndSFXOn(str_80300B60);
            memcpy(work->buttons2, work->buttons, 0xA8);
            work->prevSelected = work->selected;
            for (i = 0; i < 7; i++) {
                work->buttons[i].state = 0;
            }
        }
        work->alpha = intplGetValue(0, work->counter, 15, float_neg60_80422130, float_0_804220e8);
        work->counter++;
        if (work->counter >= 15) {
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
            break;
        }
        if (keyGetButtonTrg(0) & 0xF00) {
            if (keyGetButtonTrg(0) & work->buttons[work->selected].button) {
                EventEntry* entry;

                work->selected++;
                psndSFXOn(str_80300B78);
                if (work->selected >= work->max) {
                    work->state = 2;
                    work->counter = 0;
                    work->count++;
                    work->starState = 5;
                    entry = evtEntry(nice_event, 0, 0x20);
                    {
                        s32 n = work->count + 1;

                        entry->lwData[0] = (n < 6) ? n : 6;
                    }
                    if (audienceNum >= 1 && audienceNum <= 0x31) {
                        BattleAudienceSoundCallKind(1);
                    }
                    if (audienceNum >= 0x32 && audienceNum <= 0x63) {
                        BattleAudienceSoundCallKind(1);
                        BattleAudienceSoundWhistleKind(1);
                    }
                    if (audienceNum >= 0x64 && audienceNum <= 0x95) {
                        BattleAudienceSoundCallKind(2);
                        BattleAudienceSoundWhistleKind(2);
                    }
                    if (audienceNum >= 0x96) {
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
        if (work->counter <= 30) {
            Vec pos = vec3_803009d8;

            pos.x = intplGetValue(4, work->counter, 30, float_0_804220e8, float_neg1000_804220f8);
            work->pos = pos;
        }
        break;
    default:
        break;
    }
}

// .text:0x0 | 0x80249920 | size: 0xE8
// NONMATCHING (99.88%): same instructions; only register numbers or operands differ.
void set_tev(SukiTevParam* param) {
    SukiWork* work = get_ptr();
    u8 flash = work->flash;

    GXSetTevOrder(0, 0xFF, 0xFF, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
    GXSetTevAlphaIn(0, 7, 7, 7, 1);
    {
        GXColor color;
        color = dat_804220a0;
        color.r = flash;
        color.g = flash;
        color.b = flash;
        GXSetTevColor(1, color);
    }
    param->field_C = 1;
    param->field_14 = 0;
    param->field_10 = 0;
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
// The original loads event->args before the BattleAudienceBaseGetPtr call.
s32 aud_set_draw(EventEntry* event) {
    s32* args = event->args;
    BattleAudience* aud = BattleAudienceBaseGetPtr();

    if (evtGetValue(event, args[0]) == 0) {
        aud->flags |= 0x20000;
    }
    else {
        aud->flags &= ~0x20000;
    }
    return EVT_RETURN_DONE;
}

// .text:0x360 | 0x80249C80 | size: 0x84
static s32 set_weapon(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    SukiWork* work = get_ptr();
#ifdef MUST_MATCH
    // MUST_MATCH: a 6-byte struct assignment makes MWCC inline the copy as the retail `lwz`/`sth`
    // direct SDA loads; a plain `u8[6]` + memcpy emits a real `bl memcpy`.
    SukiBytes6 tbl;

    tbl = *(SukiBytes6*)N_ohko_rate_tbl;
    memcpy(work->weapon, weapon, 0xC0);
    work->weapon[0x9E] = tbl.x[index - 1];
#else
    u8 tbl[6];

    memcpy(tbl, N_ohko_rate_tbl, 6);
    memcpy(work->weapon, weapon, 0xC0);
    work->weapon[0x9E] = tbl[index - 1];
#endif
    evtSetValue(event, args[1], (s32)work->weapon);
    return EVT_RETURN_DONE;
}

// .text:0x3E4 | 0x80249D04 | size: 0x38
s32 wait_star_stone_attack_end(EventEntry* event, BOOL isFirstCall) {
    SukiWork* work = get_ptr();

    return (work->starState == 8) ? 2 : 0;
}

// .text:0x41C | 0x80249D3C | size: 0xCC
static s32 star_stone_attack(EventEntry* event, BOOL isFirstCall) {
    SukiWork* work = get_ptr();
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);
    CameraEntry* cam;

    work->starState = 9;
    work->starCounter = 0;
    work->starStart = work->table[index];
    work->starCur = work->starStart;
    cam = camGetPtr(CAMERA_3D);
    work->starEnd = cam->cameraPos;
    work->starEnd.y += 20.0f;
    work->starEnd.z += 10.0f;
    return 2;
}

// .text:0x4E8 | 0x80249E08 | size: 0x84
// The original loads event->args only after the get_ptr call.
s32 star_stone_get_attack_point(EventEntry* event) {
    SukiWork* work = get_ptr();
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    evtSetFloat(event, args[1], work->table[index].x);
    evtSetFloat(event, args[2], work->table[index].y);
    evtSetFloat(event, args[3], work->table[index].z);
    return EVT_RETURN_DONE;
}

// .text:0x56C | 0x80249E8C | size: 0x38
s32 wait_star_stone_up_end(EventEntry* event, BOOL isFirstCall) {
    SukiWork* work = get_ptr();

    return (work->starState == 4) ? 2 : 0;
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
// NONMATCHING (99.52%): the only difference is `extsh r5, r31` where the original has `mr r5, r31`
// at the iconDispGx call. The original's prototype for iconDispGx/iconDispGxAlpha takes the icon id
// as s32 (callers pass s32 without sign-extending, and the wrapper does not convert), but the header
// declares it s16, so this build must sign-extend. With iconDispGx and iconDispGxAlpha changed to s32
// in include/driver/icondrv.h and src/driver/icondrv.c this function is 100% (verified) with no
// regressions; that edit is outside the one allowed source file, so it is reported instead.
void N__disp_2D(s32 type, s32 index) {
    SukiWork* work = get_ptr();
    SukiButton* button;
    s32 selected;
    s32 iconId;
    f32 yoff;
    Vec pos;

    if (type == 0) {
        button = &work->buttons2[index];
        selected = work->prevSelected;
        yoff = float_80_804220e4;
    }
    else {
        button = &work->buttons[index];
        selected = work->selected;
        yoff = float_0_804220e8;
    }

    if (type == 0 && button->state == 0) {
        return;
    }
    if (type == 1 && button->state != 2) {
        return;
    }

    switch (button->button) {
    case 0x100: {
        u16 t = 0x64;
        if (index < selected) t = 0x65;
        iconId = t;
        break;
    }
    case 0x200: {
        u16 t = 0x66;
        if (index < selected) t = 0x67;
        iconId = t;
        break;
    }
    case 0x400: {
        u16 t = 0x68;
        if (index < selected) t = 0x69;
        iconId = t;
        break;
    }
    case 0x800: {
        u16 t = 0x6A;
        if (index < selected) t = 0x6B;
        iconId = t;
        break;
    }
    case 0x40: {
        u16 t = 0x7E;
        if (index < selected) t = 0x7F;
        iconId = t;
        break;
    }
    case 0x20: {
        u16 t = 0x80;
        if (index < selected) t = 0x81;
        iconId = t;
        break;
    }
    case 0x10: {
        u16 t = 0x82;
        if (index < selected) t = 0x83;
        iconId = t;
        break;
    }
    default:
        break;
    }
    if ((u16)iconId == 0) {
        return;
    }

    pos = vec3_80300a90;
    pos.x = button->pos.x;
    pos.y = button->pos.y + yoff + work->alpha + float_60_804220ec;
    pos.z = button->pos.z;
    iconDispGx(&pos, 0x10, iconId, float_1_804220d8);
}
// .text:0x85C | 0x8024A17C | size: 0x180
// NONMATCHING (82.40%): 8 instructions differ, 8 missing, 4 extra, plus register differences.
static void disp_2D(CameraId camera, void* param) {
    SukiWork* work = get_ptr();
    Mtx mtx;
    s32 i;
    Vec pos1;
    Vec pos2;

    PSMTXScale(mtx, 26.0f, 1.0f, 1.0f);
    PSMTXTransApply(mtx, mtx, work->pos.x, work->pos.y + 60.0f, work->pos.z);
    iconDispGx2(mtx, 0x10, 0x8E);

    pos1 = vec3_80300a78;
    pos1.x = work->pos.x + 215.0f;
    pos1.y = work->pos.y + 60.0f;
    pos1.z = work->pos.z;
    iconDispGx(&pos1, 0x10, 0x90, 1.0f);

    pos2 = vec3_80300a84;
    pos2.x = work->pos.x - 215.0f;
    pos2.y = work->pos.y + 60.0f;
    pos2.z = work->pos.z;
    iconDispGx(&pos2, 0x10, 0x8F, 1.0f);

    GXSetScissor(0, 0x91, gp->fbWidth, 0x50);
    for (i = 0; i < 7; i++) {
        N__disp_2D(0, i);
        N__disp_2D(1, i);
    }
    GXSetScissor(0, 0, gp->fbWidth, gp->efbHeight);
}

// dont_inline: MWCC would otherwise inline main_button's switch into main_suki's loop
// (the original keeps the bl).
#pragma dont_inline on
// .text:0xFE0 | 0x8024A900 | size: 0x114
void main_button(s32 index) {
    SukiWork* work = get_ptr();
    SukiButton* button = &work->buttons[index];
    s32 tbl[4] = {0x100, 0x200, 0x400, 0x800};
    Vec pos;

    switch (button->state) {
    case 0:
        break;
    case 1:
        button->state = 2;
        pos = vec3_803009f4;
        pos.x = 60.0f * (f32)index - 30.0f * (f32)(work->max - 1);
        button->pos = pos;
        button->button = tbl[irand(4)];
        break;
    case 2:
        break;
    }
}
#pragma dont_inline reset

// .text:0x9DC | 0x8024A2FC | size: 0x48
s32 end_suki(EventEntry* event, BOOL isFirstCall) {
    SukiWork* work = get_ptr();

    effDelete(*(EffectEntry**)((u8*)work->effect->userdata + 0x34));
    effDelete(work->effect);
    return 2;
}

static void main_star(void);
static void main_base(void);
