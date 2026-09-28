#include "battle/sac/sac_muki.h"

#include "battle/battle.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_audience.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include <dolphin/gx/GXTransform.h>
#include "driver/effdrv.h"
#include "eff/eff_starstone.h"
#include "eff/eff_hit.h"
#include "eff/n64/eff_stamp_n64.h"
#include "pmario_sound.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"
#include <string.h>
#include <math.h>
#include <dolphin/mtx.h>

// The file's .rodata, in the original order. The strings and vectors occupy 0xB4 bytes; the
// local tables in disp_3D follow them there, which is why its copies are at str_803006D8+0xB0.
const char str_803006D8[] = "btl_wn_sac_mukimuki_body";
const char str_803006F4[] = "msg_mukimuki_body";
const char str_80300708[] = "msg_sac_4";
const Vec vec3_80300714 = {0.5f, 0.5f, 1.0f};
const Vec vec3_80300720 = {0.0f, 0.0f, 20.0f};
const Vec vec3_8030072c = {0.0f, 0.0f, 1.0f};
const Vec vec3_80300738 = {0.5f, 0.5f, 1.0f};
const Vec vec3_80300744 = {0.0f, -1000.0f, 0.0f};
const Vec vec3_80300750 = {0.3f, 0.3f, 0.3f};
const Vec vec3_8030075c = {0.0f, 0.0f, 0.0f};
const Vec vec3_80300768 = {0.0f, 0.0f, 0.0f};
const Vec vec3_80300774 = {0.0f, 0.0f, 0.0f};
const Vec vec3_80300780 = {0.0f, 0.0f, 0.0f};
// disp_3D's local tables (dat_8030078c, dat_803007ac, dat_803007cc) sit here in the original.
const char str_803007EC[] = "SFX_BTL_SAC_START1";
const char str_80300800[] = "SFX_BTL_SAC_START2";
const char str_80300814[] = "SFX_BTL_SAC_START3";
const char str_80300830[] = "SFX_BTL_SAC_BODY6";
const char str_80300844[] = "SFX_BTL_SAC_BODY4";
const char str_80300858[] = "SFX_BTL_SAC_BODY5";
const char str_8030086C[] = "SFX_BTL_SAC_BODY3";

// The Muki action-command work area, at _battleWorkPointer + 0x1F4C. No header declares it.
typedef struct MukiEntry {
    s32 type;             // 0x00
    s32 field_04;         // 0x04
    s32 mode;             // 0x08
    Vec pos;              // 0x0C
    Vec scale;            // 0x18
    f32 field_24;         // 0x24
    f32 rot;              // 0x28
    f32 field_2C;         // 0x2C
    EffectEntry* effect;  // 0x30
    u8 alpha;             // 0x34
    u8 field_35[0x38 - 0x35];
} MukiEntry;

typedef struct MukiPoint {
    s32 type;          // 0x00
    s32 field_04;      // 0x04
    Vec pos;           // 0x08
    f32 field_14;      // 0x14
    f32 field_18;      // 0x18
    f32 field_1C;      // 0x1C
    MukiEntry* field_20; // 0x20, but MukiEntry is declared above? no
} MukiPoint;

typedef struct MukiWork {
    s32 field_00;                    // 0x000
    s32 state;                       // 0x004
    s32 field_08;                    // 0x008
    s32 field_0C;                    // 0x00C
    s32 field_10;                    // 0x010
    f32 field_14;                    // 0x014
    f32 field_18;                    // 0x018
    f32 field_1C;                    // 0x01C
    f32 field_20;                    // 0x020
    s32 field_24;                    // 0x024
    s32 field_28;                    // 0x028
    f32 field_2C;                    // 0x02C
    f32 field_30;                    // 0x030
    s32 field_34;                    // 0x034
    s32 field_38;                    // 0x038
    s32 field_3C;                    // 0x03C
    s32 field_40;                    // 0x040
    Vec field_44;                    // 0x044
    Vec field_50;                    // 0x050
    Vec field_5C;                    // 0x05C
    Vec field_68;                    // 0x068
    u32 field_74;                    // 0x074
    MukiPoint points[9];             // 0x078
    MukiEntry entries[9];            // 0x1BC
    s32 field_3B4;                   // 0x3B4
    s32 field_3B8;                   // 0x3B8
    f32 posX;                        // 0x3BC
    f32 posY;                        // 0x3C0
    f32 posZ;                        // 0x3C4
    f32 startX;                      // 0x3C8
    f32 startY;                      // 0x3CC
    f32 startZ;                      // 0x3D0
    f32 endX;                        // 0x3D4
    f32 endY;                        // 0x3D8
    f32 endZ;                        // 0x3DC
    u8 field_3E0[0x3EC - 0x3E0];     // 0x3E0
    Vec scale;                       // 0x3EC
    Vec scale2;                      // 0x3F8
    EffectEntry* starStone;          // 0x404
    u8 weapon[0xC0];                 // 0x408
} MukiWork;                          // size 0x4C8

static BattleWeapon weapon;

const f64 double_to_int_80300828 = 4503601774854144.0;

extern f32 float_0_80421fb4;
extern f32 float_neg1000_80421fe8;
extern f32 float_1_80421fc4;
extern f32 float_901_80422050;
extern f32 float_2_80421fa4;
extern f32 float_37_80421fec;
extern f32 float_50_80421ff0;
extern f32 float_neg1_80421fc8;
extern f32 float_2160_80421ff4;
extern f32 float_300_80421ff8;
extern f32 float_1p5_80421ffc;
extern f32 float_180_80422000;
extern f32 float_1080_80422004;
extern f32 float_255_80422008;
extern f32 float_900_8042200c;
extern f32 float_9_80422010;
extern f32 float_0p5_80421fd0;
extern u32 dat_80421f80;
extern u32 str_80421F84;
extern u32 dat_80421f88;
extern u32 float_0p74965_80421f8c;
extern u32 dat_80421f90;
extern u32 dat_80421f94;
extern f32 float_deg2rad_80421fa0;
extern f32 float_3p1416_80421fa8;
extern f32 float_360_80421fac;
extern f32 float_15_80421fb0;
extern f32 float_neg400_8042202c;
extern f32 float_90_80422030;
extern f32 float_neg120_80422034;
extern f32 float_0p1_80422048;
extern f32 float_neg0p1_8042204c;
extern char str_80422038[];
extern char str_80422040[];
extern char str_80421F98[];
extern char str_8042201C[];
extern f32 float_5_80422014;
extern f32 float_1_80422018;
extern f32 float_20_80422024;
extern f32 float_10_80422028;
extern u32 dat_80421f50;
extern u32 dat_80421f54;
extern u32 dat_80421f58;
extern u32 dat_80421f5c;
extern f32 float_32_80421fb8;
extern f32 float_8_80421fbc;
extern f32 float_16_80421fc0;
extern f32 float_80_80421fcc;
extern f32 float_30_80421fd4;
extern f32 float_0p7_80421fd8;
extern f32 float_35_80421fdc;
extern f32 float_14_80421fe0;
extern f32 float_25_80421fe4;
extern u32 dat_80421f60;
extern u32 dat_80421f64;
extern u32 dat_80421f68;
extern u32 dat_80421f6c;
extern u32 dat_80421f70;
extern u32 float_0p74966_80421f74;
extern u32 dat_80421f78;
extern u32 dat_80421f7c;

u32 nice_event_pow[8] = {0x0005005B, 0, 0xFE363C80, 0xFFFFFF9C, 0x000000AF, 0, 2, 1};

static MukiWork* get_ptr(void);
static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall);
static s32 start_game(EventEntry* event, BOOL isFirstCall);
static s32 set_weapon(EventEntry* event, BOOL isFirstCall);
static s32 wait_game_end(EventEntry* event, BOOL isFirstCall);
static void disp_3D_alpha(CameraId cameraId, void* param);
static void disp_3D(CameraId cameraId, void* param);
static void disp_2D(CameraId cameraId, void* param);
static void main_star(void);
static void main_base(void);

// The original never inlines its own helpers: every call to one is a real `bl`.
#pragma dont_inline on

// .text:0x0 | 0x80246DCC | size: 0x2C
static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall) {
    MukiWork* work = get_ptr();
    work->field_3B4 = 1;
    return 2;
}

// .text:0x2C | 0x80246DF8 | size: 0x84
static s32 wait_game_end(EventEntry* event, BOOL isFirstCall) {
    MukiWork* work = get_ptr();
    s32* args = event->args;

    if (work->state == 7) {
        evtSetValue(event, args[0], (s32)work->field_14);
        evtSetValue(event, args[1], (s32)work->field_1C);
        return 2;
    }
    return 0;
}

// .text:0xB0 | 0x80246E7C | size: 0x2C
static s32 start_game(EventEntry* event, BOOL isFirstCall) {
    MukiWork* work = get_ptr();
    work->state = 1;
    return 2;
}

// .text:0xDC | 0x80246EA8 | size: 0xBC
static s32 set_weapon(EventEntry* event, BOOL isFirstCall) {
    MukiWork* work;
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 value = evtGetValue(event, args[1]);

    work = get_ptr();
    memcpy(work->weapon, &weapon, 0xC0);
    switch (type) {
    case 0:
        work->weapon[0x96] = 0x64;
        work->weapon[0x97] = 3;
        work->weapon[0x98] = value;
        break;
    case 1:
        work->weapon[0x99] = 0x64;
        work->weapon[0x9A] = 3;
        work->weapon[0x9B] = value;
        break;
    }
    evtSetValue(event, args[2], (s32)work->weapon);
    return 2;
}

// .text:0x198 | 0x80246F64 | size: 0xC
static MukiWork* get_ptr(void) {
    return (MukiWork*)((u8*)_battleWorkPointer + 0x1F4C);
}

// NONMATCHING (87.63%): the original keeps the angle in f30 across cos/sin; otherwise the
// switch and transforms match.
// .text:0x1A4 | 0x80246F70 | size: 0x38C
static void disp_3D_alpha(CameraId cameraId, void* param) {
    MukiWork* work = get_ptr();
    MukiEntry* entry;
    Mtx mtx;
    GXColor cA;
    GXColor cB;
    f32 f;
    s32 i;

    for (i = 0; i < 9; i++) {
        entry = &work->entries[i];
        if (entry->type >= 1 && entry->type <= 9) {
            switch (entry->mode) {
            case 0:
                PSMTXRotRad(mtx, 'y', float_deg2rad_80421fa0 * entry->rot);
                PSMTXScaleApply(mtx, mtx, float_2_80421fa4 * entry->scale.x,
                                float_2_80421fa4 * entry->scale.y, entry->scale.z);
                PSMTXTransApply(mtx, mtx, entry->pos.x, entry->pos.y, entry->pos.z);
                f = float_2_80421fa4 * entry->rot * float_3p1416_80421fa8 / float_360_80421fac;
                PSMTXTransApply(mtx, mtx, float_0_80421fb4, float_15_80421fb0 * (f32)cos(f),
                                float_15_80421fb0 * (f32)sin(f));
                cA = *(GXColor*)&str_80421F84;
                cA.a = entry->alpha;
                cB = *(GXColor*)&dat_80421f88;
                cB.a = entry->alpha;
                btlDispTexPlane3(mtx, 0x45, cA, cA, cB, cB);
                break;
            case 1:
                PSMTXRotRad(mtx, 'y', float_deg2rad_80421fa0 * entry->rot);
                PSMTXScaleApply(mtx, mtx, float_2_80421fa4 * entry->scale.x,
                                float_2_80421fa4 * entry->scale.y, entry->scale.z);
                PSMTXTransApply(mtx, mtx, entry->pos.x, entry->pos.y, entry->pos.z);
                f = float_2_80421fa4 * entry->rot * float_3p1416_80421fa8 / float_360_80421fac;
                PSMTXTransApply(mtx, mtx, float_0_80421fb4, float_15_80421fb0 * (f32)cos(f),
                                float_15_80421fb0 * (f32)sin(f));
                cA = *(GXColor*)&float_0p74965_80421f8c;
                cA.a = entry->alpha;
                cB = *(GXColor*)&dat_80421f90;
                cB.a = entry->alpha;
                btlDispTexPlane3(mtx, 0x45, cA, cA, cB, cB);
                break;
            case 2:
                PSMTXRotRad(mtx, 'y', float_deg2rad_80421fa0 * entry->rot);
                PSMTXScaleApply(mtx, mtx, entry->scale.x, entry->scale.y, entry->scale.z);
                PSMTXTransApply(mtx, mtx, entry->pos.x, entry->pos.y, entry->pos.z);
                f = float_2_80421fa4 * entry->rot * float_3p1416_80421fa8 / float_360_80421fac;
                PSMTXTransApply(mtx, mtx, float_0_80421fb4, float_15_80421fb0 * (f32)cos(f),
                                float_15_80421fb0 * (f32)sin(f));
                cA = *(GXColor*)&dat_80421f94;
                cA.a = entry->alpha;
                btlDispTexPlane2(mtx, 0x31, cA, 0);
                break;
            }
        }
    }
}

// NONMATCHING (87.35%): the table initializers land 0xC later in .rodata, so the base-relative
// copies use 0xBC instead of 0xB0; the rest matches.
// .text:0x530 | 0x802472FC | size: 0x2F8
static void disp_3D(CameraId cameraId, void* param) {
    MukiWork* work = get_ptr();
    MukiPoint* point;
    Mtx m;
    s32 arr0[8] = {-8, 8, 8, 8, 8, -8, -8, -8};
    s32 arr1[8] = {-24, 24, 24, 24, 24, -24, -24, -24};
    s32 arr2[8] = {1, 1, -1, 1, -1, -1, 1, -1};
    GXColor color;
    s32 i;
    s32 t;

    for (i = 0; i < 9; i++) {
        point = (MukiPoint*)((u8*)work + i * 0x24 + 0x78);
        if (point->type >= 2 && point->type < 8) {
            for (t = 0; t < 4; t++) {
                PSMTXScale(m, (f32)arr2[t * 2] * point->field_14,
                           (f32)arr2[t * 2 + 1] * point->field_18, point->field_1C);
                PSMTXTransApply(m, m, point->pos.x, point->pos.y, point->pos.z);
                PSMTXTransApply(m, m, (f32)arr0[t * 2] * point->field_14,
                                (f32)arr0[t * 2 + 1] * point->field_18, float_0_80421fb4);
                color = *(GXColor*)&dat_80421f80;
                btlDispTexPlane2(m, 0x42, color, 0);
            }
        }
    }
    if (work->field_38 >= 2 && work->field_38 < 7) {
        for (t = 0; t < 4; t++) {
            PSMTXScale(m, (f32)arr1[t * 2] * work->field_68.x,
                       (f32)arr1[t * 2 + 1] * work->field_68.y, work->field_68.z);
            PSMTXTransApply(m, m, work->field_44.x, work->field_44.y, work->field_44.z);
            PSMTXTransApply(m, m, (f32)arr1[t * 2] * work->field_68.x,
                            (f32)arr1[t * 2 + 1] * work->field_68.y, float_0_80421fb4);
            color = *(GXColor*)&work->field_74;
            btlDispTexPlane2(m, 0x41, color, 0);
        }
    }
}

// NONMATCHING (81.26%): the stack frame is 0x10 smaller (one fewer saved temp) and the
// Vec copy from .rodata is materialised differently; bodies otherwise match.
// .text:0x908 | 0x802475F4 | size: 0x6EC
static void disp_2D(CameraId cameraId, void* param) {
    MukiWork* work = get_ptr();
    Mtx mtx;
    Vec sp;
    Vec out;
    GXColor cA;
    GXColor cB;
    GXColor c;
    f32 sx;
    f32 sy;
    f32 left;
    f32 frac;
    f32 d;
    s32 n;

    if (work->state >= 2 && work->state <= 7) {
        sp = vec3_80300774;
        sp.x = work->field_2C;
        sp.y = work->field_30;
        sp.z = float_0_80421fb4;
        btlGetScreenPoint(&sp, &out);
        sx = out.x;
        sy = out.y;
        PSMTXTrans(mtx, sx - float_32_80421fb8 - float_8_80421fbc, sy, float_0_80421fb4);
        c = *(GXColor*)&dat_80421f60;
        btlDispTexPlane2(mtx, 0x43, c, 0);
        PSMTXScale(mtx, float_16_80421fc0, float_1_80421fc4, float_1_80421fc4);
        PSMTXTransApply(mtx, mtx, sx, sy, float_0_80421fb4);
        btlDispTexPlane2(mtx, 0x44, c, 0);
        PSMTXScale(mtx, float_neg1_80421fc8, float_1_80421fc4, float_1_80421fc4);
        PSMTXTransApply(mtx, mtx, sx + float_32_80421fb8 + float_8_80421fbc, sy, float_0_80421fb4);
        btlDispTexPlane2(mtx, 0x43, c, 0);

        d = work->field_14 - work->field_18;
        frac = work->field_18 - (f32)(s32)work->field_18;
        left = sx - float_32_80421fb8 - float_8_80421fbc;
        btlDispGXInit2DRasta();
        GXLoadPosMtxImm(camGetPtr(8)->view, 0);
        if (d != float_0_80421fb4) {
            btlDispGXQuads2DRasta(left, float_8_80421fbc + sy, frac * float_80_80421fcc + left,
                                  sy - float_8_80421fbc, 0xFF, 0xB4, 0xB4, 0xFF);
        }
        else {
            btlDispGXQuads2DRasta(left, float_8_80421fbc + sy, frac * float_80_80421fcc + left,
                                  sy - float_8_80421fbc, 0xF0, 0xA0, 0xA0, 0xFF);
        }
        n = work->field_24 % 12;
        if (n >= 0 && n <= 5) {
            PSMTXScale(mtx, float_0p5_80421fd0, float_0p5_80421fd0, float_0p5_80421fd0);
            PSMTXTransApply(mtx, mtx, left - float_30_80421fd4, sy, float_0_80421fb4);
            cA = *(GXColor*)&dat_80421f64;
            cB = *(GXColor*)&dat_80421f68;
            btlDispTexPlane3(mtx, 0x45, cA, cA, cB, cB);
            PSMTXScale(mtx, float_0p7_80421fd8, float_0p7_80421fd8, float_0p7_80421fd8);
            PSMTXTransApply(mtx, mtx, left - float_35_80421fdc, sy - float_14_80421fe0, float_0_80421fb4);
            iconDispGx2(mtx, 0x10, 0x1ED);
            PSMTXScale(mtx, float_0p7_80421fd8, float_0p7_80421fd8, float_0p7_80421fd8);
            PSMTXTransApply(mtx, mtx, left - float_25_80421fe4, sy - float_14_80421fe0, float_0_80421fb4);
            c = *(GXColor*)&dat_80421f6c;
            iconNumberDispGx(mtx, (s32)work->field_18, 0, c);
        }
    }

    if (work->state >= 2 && work->state <= 7) {
        sp = vec3_80300780;
        sp.x = work->field_2C;
        sp.y = work->field_30 - float_30_80421fd4;
        sp.z = float_0_80421fb4;
        btlGetScreenPoint(&sp, &out);
        sx = out.x;
        sy = out.y;
        PSMTXTrans(mtx, sx - float_32_80421fb8 - float_8_80421fbc, sy, float_0_80421fb4);
        c = *(GXColor*)&dat_80421f70;
        btlDispTexPlane2(mtx, 0x43, c, 0);
        PSMTXScale(mtx, float_16_80421fc0, float_1_80421fc4, float_1_80421fc4);
        PSMTXTransApply(mtx, mtx, sx, sy, float_0_80421fb4);
        btlDispTexPlane2(mtx, 0x44, c, 0);
        PSMTXScale(mtx, float_neg1_80421fc8, float_1_80421fc4, float_1_80421fc4);
        PSMTXTransApply(mtx, mtx, sx + float_32_80421fb8 + float_8_80421fbc, sy, float_0_80421fb4);
        btlDispTexPlane2(mtx, 0x43, c, 0);

        frac = work->field_20 - (f32)(s32)work->field_20;
        left = sx - float_32_80421fb8 - float_8_80421fbc;
        btlDispGXInit2DRasta();
        GXLoadPosMtxImm(camGetPtr(8)->view, 0);
        if (d != float_0_80421fb4) {
            btlDispGXQuads2DRasta(left, float_8_80421fbc + sy, frac * float_80_80421fcc + left,
                                  sy - float_8_80421fbc, 0xB4, 0xB4, 0xFF, 0xFF);
        }
        else {
            btlDispGXQuads2DRasta(left, float_8_80421fbc + sy, frac * float_80_80421fcc + left,
                                  sy - float_8_80421fbc, 0xA0, 0xA0, 0xF0, 0xFF);
        }
        n = work->field_28 % 12;
        if (n >= 0 && n <= 5) {
            PSMTXScale(mtx, float_0p5_80421fd0, float_0p5_80421fd0, float_0p5_80421fd0);
            PSMTXTransApply(mtx, mtx, left - float_30_80421fd4, sy, float_0_80421fb4);
            cA = *(GXColor*)&float_0p74966_80421f74;
            cB = *(GXColor*)&dat_80421f78;
            btlDispTexPlane3(mtx, 0x45, cA, cA, cB, cB);
            PSMTXScale(mtx, float_0p7_80421fd8, float_0p7_80421fd8, float_0p7_80421fd8);
            PSMTXTransApply(mtx, mtx, left - float_35_80421fdc, sy - float_14_80421fe0, float_0_80421fb4);
            iconDispGx2(mtx, 0x10, 0x1ED);
            PSMTXScale(mtx, float_0p7_80421fd8, float_0p7_80421fd8, float_0p7_80421fd8);
            PSMTXTransApply(mtx, mtx, left - float_25_80421fe4, sy - float_14_80421fe0, float_0_80421fb4);
            c = *(GXColor*)&dat_80421f7c;
            iconNumberDispGx(mtx, (s32)work->field_20, 0, c);
        }
    }
}

// NONMATCHING (97.96%): the entry-loop induction variables land in different registers
// (r30/r31 swapped); every instruction otherwise matches.
// .text:0xF80 | 0x80247CE0 | size: 0x6C
s32 end_muki(EventEntry* event, BOOL isFirstCall) {
    MukiWork* work = get_ptr();
    s32 i;
    s32 off;
    MukiEntry* entry;

    effDelete(*(EffectEntry**)((u8*)work->starStone->userdata + 0x34));
    effDelete(work->starStone);
    for (i = 0, off = 0; i < 9; i++, off += 0x38) {
        entry = (MukiEntry*)((u8*)work + off + 0x1BC);
        effDelete(entry->effect);
    }
    return 2;
}

// NONMATCHING (55.91%): the init case's float loads and the vector copies are scheduled
// differently, and the int->float magic constant uses an anonymous pool symbol.
// .text:0xF80 | 0x80247D4C | size: 0x320
static void main_star(void) {
    MukiWork* work = get_ptr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32* data;
    f32 f;

    switch (work->field_3B4) {
    case 0:
        break;
    case 1:
        work->field_3B4 = 2;
        work->field_3B8 = 0;
        work->starStone = effStarStoneEntry(2, float_0_80421fb4, float_neg1000_80421fe8,
                                             float_0_80421fb4, float_1_80421fc4);
        BtlUnit_GetPos(mario, &work->posX, &work->posY, &work->posZ);
        work->posY += mario->mSizeMultiplier * (f32)mario->mHeight + float_37_80421fec;
        work->startX = work->posX;
        work->startY = work->posY;
        work->startZ = work->posZ;
        work->endX = work->posX;
        work->endY = work->posY;
        work->endZ = work->posZ;
        work->endY += float_50_80421ff0;
        work->endZ += float_neg1_80421fc8;
        memcpy(&work->scale, &vec3_8030075c, sizeof(Vec));
        memcpy(&work->scale2, &vec3_80300768, sizeof(Vec));
        /* fallthrough */
    case 2:
        work->field_3B8++;
        if (work->field_3B8 <= 0x64) {
            work->posX = intplGetValue(0, work->field_3B8, 0x64, work->startX, work->endX);
            work->posY = intplGetValue(0, work->field_3B8, 0x64, work->startY, work->endY);
            work->posZ = intplGetValue(0, work->field_3B8, 0x64, work->startZ, work->endZ);
            f = intplGetValue(0, work->field_3B8, 0x64, float_0_80421fb4, float_2_80421fa4);
            work->scale.z = f;
            work->scale.y = f;
            work->scale.x = f;
        }
        else {
            work->posX = work->endX;
            work->posY = work->endY;
            work->posZ = work->endZ;
            work->scale.z = float_2_80421fa4;
            work->scale.y = float_2_80421fa4;
            work->scale.x = float_2_80421fa4;
        }
        work->scale2.y = intplGetValue(4, work->field_3B8, 0x78, float_0_80421fb4, float_2160_80421ff4);
        if (work->field_3B8 >= 0x78) {
            work->field_3B4 = 3;
            work->field_3B8 = 0;
            work->startX = work->posX;
            work->startY = work->posY;
            work->startZ = work->posZ;
            work->endY = float_300_80421ff8;
        }
        break;
    case 3:
        work->field_3B8++;
        work->posY = intplGetValue(1, work->field_3B8, 0x3C, work->startY, work->endY);
        if (work->field_3B8 >= 0x3C) {
            work->field_3B4 = 5;
            work->field_3B8 = 0;
        }
        break;
    case 4:
    case 5:
        break;
    }

    if (work->starStone != NULL) {
        data = work->starStone->userdata;
        data[2] = work->posX;
        data[3] = work->posY;
        data[4] = work->posZ;
        data[6] = work->scale2.x;
        data[7] = work->scale2.y;
        data[8] = work->scale2.z;
        data[5] = float_1p5_80421ffc * work->scale.x;
    }
}

// NONMATCHING (81.46%): float argument loads are hoisted before the field_04 stores in the
// original; only scheduling differs.
// .text:0x12A0 | 0x8024806C | size: 0x288
void main_icon(s32 index) {
    MukiWork* work = get_ptr();
    MukiEntry* entry = &work->entries[index];
    f32* data;

    switch (entry->type) {
    case 0:
        entry->pos = vec3_80300744;
        entry->alpha = 0xFF;
        break;
    case 1:
        entry->type = 2;
        entry->field_04 = 0;
        entry->mode = irand(3);
        entry->rot = float_0_80421fb4;
        entry->scale = vec3_80300750;
        /* fallthrough */
    case 2:
        entry->field_04++;
        entry->rot = intplGetValue(4, entry->field_04, 0x1E, float_180_80422000, float_1080_80422004);
        entry->alpha = (u8)intplGetValue(0, entry->field_04, 0x1E, float_0_80421fb4, float_255_80422008);
        if (entry->field_04 >= 0x1E) {
            entry->type = 7;
        }
        break;
    case 3:
        entry->type = 4;
        entry->field_04 = 0;
        entry->rot = float_0_80421fb4;
        /* fallthrough */
    case 4:
        entry->field_04++;
        entry->rot = intplGetValue(1, entry->field_04, 0x1E, float_0_80421fb4, float_900_8042200c);
        entry->alpha = (u8)intplGetValue(0, entry->field_04, 0x1E, float_255_80422008, float_0_80421fb4);
        if (entry->field_04 >= 0x1E) {
            entry->type = 0;
        }
        break;
    case 5:
        entry->type = 6;
        /* fallthrough */
    case 6:
        entry->type = 0;
        break;
    case 7:
        entry->type = 8;
        entry->field_04 = 0;
        entry->rot = float_0_80421fb4;
        break;
    case 8:
        entry->field_04++;
        entry->alpha = 0xFF;
        if (entry->field_04 >= 0x5A) {
            entry->type = 3;
        }
        break;
    case 9:
        if (entry->alpha >= 8) {
            entry->alpha -= 8;
        }
        else {
            entry->alpha = 0;
        }
        break;
    }

    if (entry->effect != NULL) {
        data = entry->effect->userdata;
        data[2] = entry->pos.x;
        data[3] = entry->pos.y;
        data[4] = entry->pos.z;
        data[6] = entry->field_24;
        data[7] = entry->rot;
        data[8] = entry->field_2C;
        data[5] = float_9_80422010 * entry->scale.x;
        ((u8*)data)[0x5F] = entry->alpha;
    }
}

// The original addresses these .rodata objects as offsets from one base symbol.
#ifdef MUST_MATCH
#define RO(base, off, sym) ((const char*)(base) + (off))
#else
#define RO(base, off, sym) ((const char*)&(sym))
#endif


// NONMATCHING (81.87%): the stack frame is 0x10 smaller (the int->float temporaries are laid
// out differently); instructions otherwise match.
// .text:0x1528 | 0x802482F4 | size: 0x2AC
void main_point(s32 index) {
    const char* base = str_803006D8;
    MukiWork* work = get_ptr();
    MukiPoint* point = (MukiPoint*)((u8*)work + index * 0x24 + 0x78);
    MukiEntry* entry;
    s32 i;
    f32 f;

    switch (point->type) {
    case 1:
        point->type = 2;
        point->field_04 = irand(0x1E) + 0x1E;
        point->pos.x = (f32)((index % 3) * 0x30 - 0x30);
        point->pos.y = (f32)(0x7D - (index / 3) * 0x30);
        point->pos.z = ((const Vec*)(RO(base, 0x48, vec3_80300720)))->z;
        point->field_14 = ((const Vec*)(RO(base, 0x54, vec3_8030072c)))->x;
        point->field_18 = ((const Vec*)(RO(base, 0x54, vec3_8030072c)))->y;
        point->field_1C = ((const Vec*)(RO(base, 0x54, vec3_8030072c)))->z;
        point->field_20 = NULL;
        /* fallthrough */
    case 2:
        point->field_04--;
        if (point->field_04 <= 0x1E) {
            f = intplGetValue(0, 0x1E - point->field_04, 0x1E, float_0_80421fb4, float_0p5_80421fd0);
            point->field_18 = f;
            point->field_14 = f;
        }
        if (point->field_04 <= 0) {
            point->type = 3;
            point->field_04 = 0;
        }
        break;
    case 3:
        point->type = 4;
        point->field_04 = 0;
        point->field_14 = vec3_80300738.x;
        point->field_18 = vec3_80300738.y;
        point->field_1C = vec3_80300738.z;
        break;
    case 0:
    case 4:
    case 8:
        break;
    case 5:
        entry = point->field_20;
        if (entry != NULL) {
            if (entry->type == 0) {
                point->field_20 = NULL;
            }
        }
        else if (irand(0x41) == 0) {
            for (i = 0; i < 9; i++) {
                entry = &work->entries[i];
                if (entry->type == 0) {
                    entry->type = 1;
                    entry->pos = point->pos;
                    entry->pos.z = float_0_80421fb4;
                    point->field_20 = entry;
                    break;
                }
            }
        }
        break;
    case 6:
        point->type = 7;
        point->field_04 = irand(0x1E) + 0x1E;
        point->field_20 = NULL;
        /* fallthrough */
    case 7:
        point->field_04--;
        if (point->field_04 <= 0x1E) {
            f = intplGetValue(0, 0x1E - point->field_04, 0x1E, float_0p5_80421fd0, float_0_80421fb4);
            point->field_18 = f;
            point->field_14 = f;
        }
        if (point->field_04 <= 0) {
            point->type = 8;
            point->field_04 = 0;
        }
        break;
    }
}

// NONMATCHING (84.28%): the original hoists `str_80300830` into r29 for the whole
// function; the rest differs only in register/scheduling choices.
// .text:0x14BC | 0x802485A0 | size: 0x638
void main_cursor(void) {
    MukiWork* work = get_ptr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    BattleUnitPart* part = BtlUnit_GetPartsPtr(mario, BtlUnit_GetBodyPartsId(mario));
    s32 audienceNum = BattleAudience_GetAudienceNum();
    MukiEntry* entry;
    f32 old;
    f32 x;
    f32 y;
    f32 z;
    u8* p;

    switch (work->field_38) {
    case 1:
        if (work->points[4].type < 5) {
            break;
        }
        work->field_38 = 2;
        work->field_44.x = work->points[4].pos.x;
        work->field_44.y = work->points[4].pos.y;
        work->field_44.z = work->points[4].pos.z + float_5_80422014;
        work->field_68 = *(Vec*)(RO(str_803006D8, 0x3C, vec3_80300714));
        work->field_40 = 4;
        work->field_74 = dat_80421f50;
        /* fallthrough */
    case 2:
        work->field_74 = dat_80421f54;
        if (work->field_34 >= 0x78) {
            BtlUnit_SetAnim(part, str_80421F98);
            work->field_00 &= ~1;
            work->field_34 = 0;
        }
        if (keyGetButtonTrg(0) & 0x100) {
            work->field_38 = 5;
            entry = work->points[work->field_40].field_20;
            if (entry != NULL) {
                entry->type = 5;
                effHitEntry(0, 5, entry->pos.x, entry->pos.y, entry->pos.z, float_1_80421fc4);
                switch (entry->mode) {
                case 0:
                    old = work->field_14;
                    work->field_14 = old + float_1_80422018;
                    if (work->field_14 > float_9_80422010) {
                        work->field_14 = float_9_80422010;
                    }
                    if ((s32)work->field_14 > (s32)old) {
                        work->field_24 = 0x3C;
                        psndSFXOn(str_80300830);
                        if (audienceNum >= 1 && audienceNum <= 0x31) {
                            BattleAudienceSoundCallKind(1);
                        }
                        if (audienceNum >= 0x32 && audienceNum <= 0x63) {
                            BattleAudienceSoundCallKind(1);
                            BattleAudienceSoundHandBeat();
                        }
                        if (audienceNum >= 0x64 && audienceNum <= 0x95) {
                            BattleAudienceSoundCallKind(2);
                        }
                        if (audienceNum >= 0x96) {
                            BattleAudienceSoundCallKind(2);
                            BattleAudienceSoundHandBeat();
                        }
                    }
                    psndSFXOn(str_80300844);
                    break;
                case 1:
                    old = work->field_1C;
                    work->field_1C = old + float_1_80422018;
                    if (work->field_1C > float_9_80422010) {
                        work->field_1C = float_9_80422010;
                    }
                    if ((s32)work->field_1C > (s32)old) {
                        work->field_28 = 0x3C;
                        psndSFXOn(str_80300830);
                        if (audienceNum >= 1 && audienceNum <= 0x31) {
                            BattleAudienceSoundCallKind(1);
                        }
                        if (audienceNum >= 0x32 && audienceNum <= 0x63) {
                            BattleAudienceSoundCallKind(1);
                            BattleAudienceSoundHandBeat();
                        }
                        if (audienceNum >= 0x64 && audienceNum <= 0x95) {
                            BattleAudienceSoundCallKind(2);
                        }
                        if (audienceNum >= 0x96) {
                            BattleAudienceSoundCallKind(2);
                            BattleAudienceSoundHandBeat();
                        }
                    }
                    psndSFXOn(str_80300844);
                    break;
                case 2:
                    BtlUnit_SetAnim(part, str_8042201C);
                    BtlUnit_GetPos(mario, &x, &y, &z);
                    p = *(u8**)((u8*)effStampN64Entry(2, x, y + float_20_80422024, z + float_10_80422028) + 0xC);
                    p[0x38] = 0;
                    p[0x39] = 0xC8;
                    p[0x3A] = 0;
                    p[0x3B] = 0;
                    p[0x3C] = 0x0A;
                    p[0x3D] = 0;
                    work->field_34 = 0;
                    work->field_00 |= 1;
                    psndSFXOn(str_80300858);
                    if (audienceNum >= 1 && audienceNum <= 0x31) {
                        BattleAudienceSoundBooingKind(1);
                    }
                    if (audienceNum >= 0x32 && audienceNum <= 0x63) {
                        BattleAudienceSoundBooingKind(1);
                    }
                    if (audienceNum >= 0x64 && audienceNum <= 0x95) {
                        BattleAudienceSoundBooingKind(2);
                    }
                    if (audienceNum >= 0x96) {
                        BattleAudienceSoundBooingKind(3);
                    }
                    break;
                }
            }
        }
        if (keyGetDirTrg(0) & 0x8000) {
            if (work->field_40 % 3 != 2) {
                work->field_38 = 3;
                work->field_40++;
            }
        }
        if (keyGetDirTrg(0) & 0x4000) {
            if (work->field_40 % 3 != 0) {
                work->field_38 = 3;
                work->field_40--;
            }
        }
        if (keyGetDirTrg(0) & 0x1000) {
            if (work->field_40 / 3 != 0) {
                work->field_38 = 3;
                work->field_40 -= 3;
            }
        }
        if (keyGetDirTrg(0) & 0x2000) {
            if (work->field_40 / 3 != 2) {
                work->field_38 = 3;
                work->field_40 += 3;
            }
        }
        break;
    case 3:
        work->field_38 = 4;
        work->field_3C = 0;
        work->field_50 = work->field_44;
        work->field_5C = work->points[work->field_40].pos;
        psndSFXOn(str_8030086C);
        /* fallthrough */
    case 4:
        work->field_3C++;
        work->field_74 = dat_80421f58;
        if (!(work->field_00 & 1)) {
            work->field_44.x = intplGetValue(4, work->field_3C, 5, work->field_50.x, work->field_5C.x);
            work->field_44.y = intplGetValue(4, work->field_3C, 5, work->field_50.y, work->field_5C.y);
            if (work->field_3C >= 5) {
                work->field_38 = 2;
            }
        }
        else {
            work->field_44.x = intplGetValue(4, work->field_3C, 0x19, work->field_50.x, work->field_5C.x);
            work->field_44.y = intplGetValue(4, work->field_3C, 0x19, work->field_50.y, work->field_5C.y);
            if (work->field_3C >= 0x19) {
                work->field_38 = 2;
            }
        }
        break;
    case 5:
        work->field_38 = 6;
        work->field_3C = 0;
        /* fallthrough */
    case 6:
        work->field_3C++;
        work->field_74 = dat_80421f5c;
        if (work->field_3C >= 1) {
            work->field_38 = 2;
            work->field_3C = 0;
        }
        break;
    }
}

// NONMATCHING (87.05%): float-constant loads are scheduled differently throughout;
// the switch/jump table and calls match.
// .text:0x1D0C | 0x80248BD8 | size: 0x484
static void main_base(void) {
    MukiWork* work = get_ptr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    BattleUnitPart* part = BtlUnit_GetPartsPtr(mario, BtlUnit_GetBodyPartsId(mario));
    s32 i;
    s32 count;
    f32 f;
    f32 diff;

    if (work->field_24 > 0) {
        work->field_24--;
    }
    if (work->field_28 > 0) {
        work->field_28--;
    }

    switch (work->state) {
    case 0:
        break;
    case 1:
        work->state = 2;
        work->field_18 = float_0_80421fb4;
        work->field_14 = float_0_80421fb4;
        work->field_20 = float_0_80421fb4;
        work->field_1C = float_0_80421fb4;
        work->field_2C = float_neg400_8042202c;
        work->field_30 = float_90_80422030;
        work->field_0C = 0x383;
        for (i = 0; i < 9; i++) {
            work->points[i].type = 1;
        }
        /* fallthrough */
    case 2:
        for (i = 0, count = 0; i < 9; i++) {
            if (work->points[i].type == 4) {
                count++;
            }
            else {
                break;
            }
        }
        if (count == 9) {
            work->state = 3;
            work->field_08 = 0;
        }
        break;
    case 3:
        work->field_08++;
        work->field_2C = intplGetValue(4, work->field_08, 0x1E, float_neg400_8042202c, float_neg120_80422034);
        if (work->field_08 >= 0x1E) {
            work->state = 4;
            work->field_08 = 0;
            work->field_38 = 1;
            for (i = 0; i < 9; i++) {
                work->points[i].type = 5;
            }
        }
        break;
    case 4:
        if (work->field_00 & 1) {
            work->field_34++;
            if (work->field_34 == 0x1E) {
                BtlUnit_SetAnim(part, str_80422038);
            }
        }
        work->field_0C--;
        if (work->field_0C <= 0) {
            work->state = 5;
            work->field_08 = 0;
            work->field_38 = 7;
            BtlUnit_SetAnim(part, str_80422040);
            for (i = 0; i < 9; i++) {
                work->points[i].type = 6;
                work->entries[i].type = 9;
            }
            count = (s32)(work->field_14 + work->field_1C);
            if (count >= 1) {
                if (count == 1) count = 2;
                else if (count == 2) count = 3;
                else if (count == 3) count = 4;
                else if (count == 4) count = 5;
                else if (count >= 5) count = 6;
            }
            evtEntry(nice_event_pow, 0, 0x20)->lwData[0] = count;
        }
        break;
    case 5:
        for (i = 0, count = 0; i < 9; i++) {
            if (work->points[i].type == 8) {
                count++;
            }
            else {
                break;
            }
        }
        if (count == 9) {
            work->state = 6;
            work->field_08 = 0;
        }
        break;
    case 6:
        work->field_08++;
        if (work->field_08 >= 0x1E) {
            work->state = 7;
            work->field_08 = 0;
        }
        break;
    case 7:
        break;
    }

    diff = work->field_14 - work->field_18;
    if (diff != float_0_80421fb4) {
        if (diff > float_0_80421fb4) {
            f = float_0p1_80422048 * diff;
            if (f < float_0p1_80422048) {
                f = float_0p1_80422048;
            }
            work->field_18 += f;
            if (work->field_14 - work->field_18 < float_0_80421fb4) {
                work->field_18 = work->field_14;
            }
        }
        else {
            f = float_neg0p1_8042204c * diff;
            if (f < float_0p1_80422048) {
                f = float_0p1_80422048;
            }
            work->field_18 -= f;
            if (work->field_14 - work->field_18 > float_0_80421fb4) {
                work->field_18 = work->field_14;
            }
        }
    }

    diff = work->field_1C - work->field_20;
    if (diff != float_0_80421fb4) {
        if (diff > float_0_80421fb4) {
            f = float_0p1_80422048 * diff;
            if (f < float_0p1_80422048) {
                f = float_0p1_80422048;
            }
            work->field_20 += f;
            if (work->field_1C - work->field_20 < float_0_80421fb4) {
                work->field_20 = work->field_1C;
            }
        }
        else {
            f = float_neg0p1_8042204c * diff;
            if (f < float_0p1_80422048) {
                f = float_0p1_80422048;
            }
            work->field_20 -= f;
            if (work->field_1C - work->field_20 > float_0_80421fb4) {
                work->field_20 = work->field_1C;
            }
        }
    }
}

// .text:0x908 | 0x8024905C | size: 0x134
s32 main_muki(EventEntry* event, BOOL isFirstCall) {
    MukiWork* work = get_ptr();
    s32* args = event->args;
    s32 i;

    if (isFirstCall) {
        memset(work, 0, 0x4C8);
        work->field_10 = evtGetValue(event, args[0]);
        for (i = 0; i < 9; i++) {
            f32 zero = float_0_80421fb4;
            work->entries[i].effect = effStarStoneEntry(9, zero, float_neg1000_80421fe8, zero,
                                                        float_1_80421fc4);
        }
    }
    main_base();
    main_cursor();
    for (i = 0; i < 9; i++) {
        main_point(i);
    }
    for (i = 0; i < 9; i++) {
        main_icon(i);
    }
    main_star();
    dispEntry(CAMERA_2D, float_901_80422050, 1, disp_2D, NULL);
    dispEntry(CAMERA_3D, float_0_80421fb4, 1, disp_3D, NULL);
    dispEntry(CAMERA_3D, float_0_80421fb4, 2, disp_3D_alpha, NULL);
    return 0;
}

#pragma dont_inline reset
