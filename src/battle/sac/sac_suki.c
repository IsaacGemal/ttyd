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
extern u32 dat_804220a0;

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







// .text:0x0 | 0x80249920 | size: 0xE8
// The original's prologue is stmw/lmw, which only -O4,s (optimize_for_size) emits.
#pragma optimize_for_size on
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
        color = *(GXColor*)&dat_804220a0;
        color.r = flash;
        color.g = flash;
        color.b = flash;
        GXSetTevColor(1, color);
    }
    param->field_C = 1;
    param->field_14 = 0;
    param->field_10 = 0;
}
#pragma optimize_for_size reset

// .text:0xE8 | 0x80249A08 | size: 0x5C
// The original's prologue is stmw/lmw, which only -O4,s (optimize_for_size) emits; the body itself is identical.
#pragma optimize_for_size on
s32 get_img_name(EventEntry* event) {
    s32* args = event->args;
    s8 index = (s8)evtGetValue(event, args[0]);
    SukiWork* work = get_ptr();

    evtSetValue(event, args[1], (s32)(work->img[index]));
    return EVT_RETURN_DONE;
}
#pragma optimize_for_size reset

// .text:0x144 | 0x80249A64 | size: 0x84
// The original's prologue is stmw/lmw, which only -O4,s (optimize_for_size) emits.
#pragma optimize_for_size on
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
#pragma optimize_for_size reset

// .text:0x1C8 | 0x80249AE8 | size: 0x88
// The original's prologue is stmw/lmw, which only -O4,s (optimize_for_size) emits.
#pragma optimize_for_size on
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
#pragma optimize_for_size reset

// .text:0x250 | 0x80249B70 | size: 0x68
// The original's prologue is stmw/lmw, which only -O4,s (optimize_for_size) emits.
#pragma optimize_for_size on
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
#pragma optimize_for_size reset

// .text:0x2B8 | 0x80249BD8 | size: 0x40
s32 flash_update(EventEntry* event) {
    s32* args = event->args;
    u8 value = evtGetValue(event, args[0]);
    SukiWork* work = get_ptr();

    work->flash = value;
    return EVT_RETURN_DONE;
}

// .text:0x2F8 | 0x80249C18 | size: 0x68
// The original's prologue is stmw/lmw (-O4,s), and it loads event->args before the BattleAudienceBaseGetPtr call.
#pragma optimize_for_size on
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
#pragma optimize_for_size reset

// .text:0x360 | 0x80249C80 | size: 0x84
// The original's prologue is stmw/lmw, which only -O4,s (optimize_for_size) emits.
#pragma optimize_for_size on
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
#pragma optimize_for_size reset

// .text:0x3E4 | 0x80249D04 | size: 0x38
// The original was built with -O4,s: this comparison only compiles to subic/subfe under size optimization.
#pragma optimize_for_size on
s32 wait_star_stone_attack_end(EventEntry* event, BOOL isFirstCall) {
    SukiWork* work = get_ptr();

    return (work->starState == 8) ? 2 : 0;
}
#pragma optimize_for_size reset

// .text:0x41C | 0x80249D3C | size: 0xCC
// The original's prologue is stmw/lmw, which only -O4,s (optimize_for_size) emits.
#pragma optimize_for_size on
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
#pragma optimize_for_size reset

// .text:0x4E8 | 0x80249E08 | size: 0x84
// The original's prologue is stmw/lmw (-O4,s) and it loads event->args only after the get_ptr call.
#pragma optimize_for_size on
s32 star_stone_get_attack_point(EventEntry* event) {
    SukiWork* work = get_ptr();
    s32* args = event->args;
    s32 index = evtGetValue(event, args[0]);

    evtSetFloat(event, args[1], work->table[index].x);
    evtSetFloat(event, args[2], work->table[index].y);
    evtSetFloat(event, args[3], work->table[index].z);
    return EVT_RETURN_DONE;
}
#pragma optimize_for_size reset

// .text:0x56C | 0x80249E8C | size: 0x38
// The original was built with -O4,s: this comparison only compiles to subic/subfe under size optimization.
#pragma optimize_for_size on
s32 wait_star_stone_up_end(EventEntry* event, BOOL isFirstCall) {
    SukiWork* work = get_ptr();

    return (work->starState == 4) ? 2 : 0;
}
#pragma optimize_for_size reset

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

// .text:0x85C | 0x8024A17C | size: 0x180
// NONMATCHING (51.31%): the retail iconDispGx takes `Vec*` (include/driver/icondrv.h wrongly declares it
// by value), so our by-value calls copy each Vec into an argument temp and add 0x10 to the frame; that
// pushes Mtx to 0x38 instead of 0x20 and the two Vec slots to 0x2c/0x20 instead of 0x14/0x08. Body logic
// and the scissor/N__disp_2D loop match.
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
    iconDispGx(pos1, 0x10, 0x90, 1.0f);

    pos2 = vec3_80300a84;
    pos2.x = work->pos.x - 215.0f;
    pos2.y = work->pos.y + 60.0f;
    pos2.z = work->pos.z;
    iconDispGx(pos2, 0x10, 0x8F, 1.0f);

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

static void main_star(void);
static void main_base(void);
