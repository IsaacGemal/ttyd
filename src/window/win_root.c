#include "pmario_sound.h"
#include "win/win_main.h"
#include "data/item_data.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/msgdrv.h"
#include "driver/swdrv.h"
#include "driver/windowdrv.h"
#include "manager/fontmgr.h"
#include "mario/mario_pouch.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include <dolphin/types.h>
#include "math_ext.h"
#include <string.h>
#include "driver/mapdrv.h"
#include "system.h"
#include "window/win_root.h"
#include "data/mail_data.h"
#include "window/win_item.h"
#include "window/win_badge.h"
#include "window/win_mario.h"
#include "window/win_party.h"
#include "window/win_log.h"
#include "battle/battle_monosiri.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_lottery.h"
#include <stdio.h>

#ifdef MUST_MATCH
// Retail shares one base register for this unit's literal and template pool.
#pragma pool_data on
#endif
extern const f32 float_2_8041d090;
extern const f32 float_neg1p3963_8041d0dc;
extern const f32 float_neg0p5_8041d0e0;
extern const f32 float_18_8041d1b8;
extern const f32 float_320_8041d074;
extern const f32 float_570_8041d1bc;
extern const f32 float_288_8041d16c;
extern const f32 float_neg285_8041d1c0;
extern const f32 float_160_8041d1c4;
extern const f32 float_neg160_8041d1c8;
extern const f32 float_285_8041d1cc;
extern const f32 float_144_8041d1d0;
extern const f32 float_neg144_8041d1d4;
extern const f32 float_1000_8041d1e8;
extern const f32 float_185_8041d1e4;
extern const f32 float_180_8041d18c;
extern const f32 float_0p25_8041d1ec;

// Extended fields of WindowWork used by this unit. win_main.h only declares the
// first 0x40 bytes, so the rest are reached through raw offsets below.
#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_U32(w, off) (*(u32*)((u8*)(w) + (off)))
#define WW_PTR(w, off) (*(void**)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))
#define WW_U16(w, off) (*(u16*)((u8*)(w) + (off)))

typedef struct SortTable {
    const char* name;                 // 0x0
    void (*func)(WindowWork* work);    // 0x4
} SortTable;

// Menu tab and record blocks embedded in WindowWork at 0x48/0xC0.
typedef struct WinRootTab {
    s32 id;          // 0x0
    f32 x;           // 0x4
    f32 y;           // 0x8
    s32 state;       // 0xC
    s32 counter;     // 0x10
    const char* str; // 0x14
} WinRootTab;

typedef struct WinRootRec {
    u32 a; // 0x0
    u32 b; // 0x4
    u32 c; // 0x8
    s32 d; // 0xC
    u32 e; // 0x10
} WinRootRec;

typedef struct WinRoot {
    u8 pad0[0x48];
    WinRootTab tabs[5]; // 0x48
    WinRootRec recs[5]; // 0xC0
} WinRoot;

#define WROOT(w) ((WinRoot*)(w))

extern const f32 float_neg1000_8041d0cc;
extern const f32 float_0_8041d068;
extern const f32 float_neg500_8041d0d0;
extern const f32 float_300_8041d0d4;
extern const f32 float_neg100_8041d0d8;
extern const f32 float_neg600_8041d0fc;
extern const f32 float_30_8041d100;
extern const f32 float_40_8041d0e8;
extern const f32 float_0p125_8041d104;
extern const f32 float_0p5_8041d06c;
extern const f32 float_1_8041d070;
extern ItemData itemDataTable[];

extern SortTable* sort_data[8];

static const GXColor dat_8041ced8 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cedc = {0x00, 0x00, 0x00, 0x80};
static const GXColor dat_8041cee0 = {0xFF, 0xFF, 0xFF, 0xFE};
static const GXColor dat_8041cee4 = {0x80, 0x80, 0x80, 0xFE};
static const GXColor dat_8041cee8 = {0xC8, 0x66, 0x60, 0xFF};
static const GXColor dat_8041ceec = {0xE1, 0xA5, 0xA1, 0xFF};
static const GXColor dat_8041cef0 = {0xF0, 0xCD, 0xCB, 0xFF};
static const GXColor dat_8041cef4 = {0xC8, 0x6B, 0x96, 0xFF};
static const GXColor dat_8041cef8 = {0xDB, 0xAB, 0xC6, 0xFF};
static const GXColor dat_8041cefc = {0xEC, 0xD1, 0xE1, 0xFF};
static const GXColor float_neg0p0047648_8041cf00 = {0xBB, 0x9C, 0x21, 0xFF};
static const GXColor dat_8041cf04 = {0xD7, 0xCD, 0x76, 0xFF};
static const GXColor dat_8041cf08 = {0xEA, 0xE5, 0xAD, 0xFF};
static const GXColor dat_8041cf0c = {0x2F, 0xA5, 0x30, 0xFF};
static const GXColor dat_8041cf10 = {0xA2, 0xD2, 0x6E, 0xFF};
static const GXColor dat_8041cf14 = {0xCB, 0xE7, 0xA7, 0xFF};
static const GXColor dat_8041cf18 = {0x28, 0x7E, 0xC5, 0xFF};
static const GXColor dat_8041cf1c = {0x78, 0xB3, 0xD6, 0xFF};
static const GXColor dat_8041cf20 = {0xAF, 0xD6, 0xEA, 0xFF};
static const GXColor dat_8041cf24 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf28 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf2c = {0x00, 0x00, 0x00, 0xFF};
static const GXColor dat_8041cf30 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf34 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf38 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf3c = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf40 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf44 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf48 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf4c = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf50 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf54 = {0x00, 0x00, 0x00, 0xFF};
static const GXColor dat_8041cf58 = {0x00, 0x00, 0x00, 0xFF};
static const GXColor dat_8041cf5c = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf60 = {0x00, 0x00, 0x00, 0x80};
static const GXColor dat_8041cf64 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf68 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf6c = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf70 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf74 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf78 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf7c = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf80 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf84 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf88 = {0x00, 0x00, 0x00, 0xFF};
static const GXColor dat_8041cf8c = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041cf90 = {0xFF, 0xFF, 0xFF, 0xFF};

static const char* tbl[] = {
    "nara", "gor",  "tik", "shiga", "hei", "nok", "stn", "gon", "win",
    "hou",  "mri",  "tou", "usu",   "gra", "jin", "muj", "dou", "hom",
    "eki",  "rsh",  "pik", "sin",   "bom", "moo", "aji", "las",
};

static const GXColor dat_8041d004 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041d008 = {0x00, 0x00, 0x00, 0x80};
static const GXColor dat_8041d00c = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor dat_8041d010 = {0x80, 0x80, 0x80, 0xFF};
static const GXColor dat_8041d014 = {0xFF, 0xFF, 0xFF, 0xFF};

static char msgbuf[0x3C];

extern const f32 float_90_8041d194;
extern const f32 float_74_8041d198;
extern const f32 float_66_8041d19c;
extern const f32 float_16_8041d0e4;
extern const f32 float_8_8041d0a0;

// The per-quad texture setup/scale of the kirinuki window: same shape as
// nameent.c's NAME_KIRINUKI_SCALE. `yscale` is a local of the caller, shared
// across the quads like nameKirinukiGX's. The LOD's zeroes are the `0.0f`
// literals of nameent.c, not reads of float_0_8041d068: the literals make MWCC
// create the frontend temporaries in the original's order (the named reads
// shift the whole FPR colouring and cost ~0.2%).
#define WIN_KIRINUKI_SCALE(id_, W, H) \
    { \
        GXTexObj texObj; \
        Mtx mtx; \
        f32 xs; \
        f64 hh; \
        f64 ww; \
        TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, (id_)); \
        GXInitTexObjLOD(&texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0); \
        GXLoadTexObj(&texObj, 0); \
        GXSetNumTexGens(1); \
        GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D); \
        hh = __fabs(H); \
        ww = __fabs(W); \
        yscale = (f32)hh / (f32)GXGetTexObjHeight(&texObj); \
        xs = (f32)ww / (f32)GXGetTexObjWidth(&texObj); \
        PSMTXScale(mtx, xs, yscale, float_1_8041d070); \
        GXLoadTexMtxImm(mtx, 0x1E, 1); \
    }

static inline void winKirinukiQuad(f32 x, f32 y, f32 w, f32 h) {
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + w, y - h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

// One name-plate half: a textured quad from (x,y) to (x+w,y-h), with the texture
// stretched across it. The scales are f64 because the original uses the double
// __fabs() (not the fabsf() of windowdrv.c); the (f32) conversion happens at the
// division, matching the target's frsp placement.
static inline void winNameQuad(WindowWork* work, s32 texId, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sy;
    f64 sx;
    f32 syf;

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = __fabs(h);
    sx = __fabs(w);
    syf = (f32)sy / (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx, (f32)sx / (f32)GXGetTexObjWidth(&texObj), syf, float_1_8041d070);
    GXLoadTexMtxImm(mtx, 0x1E, 1);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + w, y - h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

// The name plate used by win_mario's partner menu: two inlined winNameQuad halves.
// type 0/1 are the 32px-tall name bar (16px cap on the right), type 2 the 24px
// tall bar used by the partner panels (8px cap).
static inline void nameQuadTw(WindowWork* work, s32 texId, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj texObj;
    Mtx mtx;
    f32 syf;
    f64 sy;
    f64 sx;

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = __fabs(h);
    sx = __fabs(w);
    syf = (f32)sy / (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx, (f32)sx / (f32)GXGetTexObjWidth(&texObj), syf, float_1_8041d070);
    GXLoadTexMtxImm(mtx, 0x1E, 1);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + w, y - h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

extern const f32 float_neg240_8041d078;
extern const f32 float_4_8041d07c;
extern const f32 float_208_8041d080;
extern const f32 float_56_8041d084;
extern const f32 float_72_8041d088;
extern const f32 float_3_8041d08c;
extern const f32 float_88_8041d094;
extern const f32 float_104_8041d098;
extern const f32 float_120_8041d09c;
extern const f32 float_8_8041d0a0;
extern const f32 float_32_8041d0a4;
extern const f32 float_304_8041d0a8;
extern const f32 float_240_8041d0ac;
extern const f32 float_80_8041d0b0;
extern const f32 float_210_8041d0b4;
extern const f32 float_7_8041d0b8;
extern const f32 float_360_8041d0bc;
extern const f32 float_20_8041d0c0;
extern const f32 float_230_8041d0c4;
extern const f32 float_24_8041d0c8;

static inline Vec mail_vec(const Vec* base, f32 x, f32 y) {
    Vec v = *base;
    v.x = x;
    v.y = y;
    return v;
}

// A mail entry as winMailDisp reads it (mail_data.h returns it untyped).
typedef struct WinMailEntry {
    u16 flags;           // 0x0: 1/2/4 pick the envelope colour, 0x1000 has a picture
    u8 field_0x2[0x6];   // 0x2
    const char* name;    // 0x8
    const char* body;    // 0xC
    s32 picture;         // 0x10
} WinMailEntry;

// winFontSetLabel and winIconSet take their Vec and GXColor arguments by value (each call copies them);
// win_main.h declares pointers, which compiles to the same callee.
// winFontSetLabel takes its Vecs and colour by pointer; the original passes them by value here (caller copies plus a
// pointer, ABI-identical), and only the cast reproduces its argument scheduling.
#ifdef MUST_MATCH
#define winMailLabelV ((void (*)(Vec, Vec, GXColor, const char*))winFontSetLabel)
#else
static inline void winMailLabelV(Vec pos, Vec scale, GXColor color, const char* msg) {
    winFontSetLabel(&pos, &scale, &color, msg);
}
#endif
static inline void winMailIconV(s32 id, Vec pos, Vec scale, GXColor color) {
    winIconSet(id, &pos, &scale, &color);
}
#define WIN_MAIL_TEX(w) (**(TEXPalette***)(*(u8**)((u8*)(w) + 0x28) + 0xA0))

// The loop both comparators run: find `tag` in the 26-entry table, formatting each
// candidate into `buf`. Written as an inlined helper because inlining it twice is
// what reproduces the target's callee-saved register allocation (table base/pointer
// in r26/r27, tag/counter in r28/r29).
static inline s32 winRootFindTag(const char* const* table, const char* tag, u8* buf) {
    s32 i;

    for (i = 0; (u32)i < 0x1A; i++) {
        sprintf((char*)buf, "menu_monosiri_%s", table[i]);
        if (strcmp((char*)buf, tag) == 0) {
            break;
        }
    }
    if ((u32)i >= 0x1A) {
        i = 0;
    }
    return i;
}

// winMailGX's copy of winNameQuad. The body is the same, but w and h arrive as
// f64 so MWCC emits the target's `lfd double_<w>/<h>; fabs; frsp` scale idiom.
// The drawn rectangles are the f32 sum chains of the caller and the f64 constants
// fold back to the target's lfs words.
static inline void winMailQuad(WindowWork* work, s32 texId, f32 x, f32 y, f64 w, f64 h) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sx;
    f64 sy;

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = __fabs(h);
    sx = __fabs(w);
    PSMTXScale(mtx, (f32)sx / (f32)GXGetTexObjWidth(&texObj), (f32)sy / (f32)GXGetTexObjHeight(&texObj), float_1_8041d070);
    GXLoadTexMtxImm(mtx, 0x1E, 1);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - (f32)h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + (f32)w, y - (f32)h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + (f32)w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

// ---- winHakoGX ------------------------------------------------------------
// The item/badge container window ("hako" = box). It draws a scrolling grid of
// item tiles, an overlay of item icons, and the bottom status bar. Every tile is
// an inlined textured quad, so it uses its own hakoQuad copy below (the same body
// as wazaQuad, matching retail's height-then-width scale computation).
extern const f32 float_224_8041d10c;
extern const f32 float_392_8041d110;
extern const f32 float_400_8041d114;
extern const f32 float_neg400_8041d118;
extern const f32 float_48_8041d11c;
extern const f32 float_28_8041d120;
extern const f32 float_395_8041d124;
extern const f32 float_15_8041d128;
extern const f32 float_235_8041d12c;
extern const f32 float_220_8041d130;
extern const f32 float_0p7_8041d134;
extern const f32 float_380_8041d138;
extern const f32 float_228_8041d13c;
extern const f32 float_237_8041d140;

static inline void hakoQuad(WindowWork* work, s32 texId, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sy;
    f64 sx;
    f32 syf;

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = __fabs(h);
    sx = __fabs(w);
    syf = (f32)sy / (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx, (f32)sx / (f32)GXGetTexObjWidth(&texObj), syf, float_1_8041d070);
    GXLoadTexMtxImm(mtx, 0x1E, 1);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + w, y - h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

// winWazaGX's copy of winNameQuad. The only difference from winNameQuad is the
// nested `tw` local around the width division: with the division inlined MWCC gives
// winWazaGX's callee-saved FPRs a different colouring (w/w75/h in f26/f27/f17
// instead of retail's f22/f23/f31) and the whole function cascades. Naming the
// texture width first makes MWCC colour it exactly as retail. No other function uses
// this copy, so winNameQuad stays untouched for the rest of the unit.
static inline void wazaQuad(WindowWork* work, s32 texId, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sy;
    f64 sx;
    f32 syf;

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = __fabs(h);
    sx = __fabs(w);
    syf = (f32)sy / (f32)GXGetTexObjHeight(&texObj);
#ifdef MUST_MATCH
    // The named width temporary is what reproduces retail's FPR colouring; the
    // natural inlined form is under #else.
    { f32 tw = (f32)GXGetTexObjWidth(&texObj);
    PSMTXScale(mtx, (f32)sx / tw, syf, float_1_8041d070); }
#else
    PSMTXScale(mtx, (f32)sx / (f32)GXGetTexObjWidth(&texObj), syf, float_1_8041d070);
#endif
    GXLoadTexMtxImm(mtx, 0x1E, 1);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + w, y - h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

// ---- winSortGX ------------------------------------------------------------
extern const f32 float_240_8041d0ac;
extern const f32 float_304_8041d0a8;
extern const f32 float_16_8041d0e4;
extern const f32 float_20_8041d0c0;
extern const f32 float_24_8041d0c8;
extern const f32 float_128_8041d0ec;
extern const f32 float_3_8041d08c;
extern const f32 float_6_8041d0f8;
extern const f32 float_8_8041d0a0;

static inline Vec winSortMkVec(Vec base, f32 x, f32 y) {
    Vec v = base;
    v.x = x;
    v.y = y;
    return v;
}

// Draws one textured 1x1-uv quad. The original inlines this at every call site,
// which is why each panel gets its own GXTexObj/Mtx stack slots.
static inline void winSortPanel(WindowWork* work, s32 texId, f32 x, f32 y, f32 w, f64 h) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sx;
    f64 sy;

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = __fabs(h);
    sx = __fabs(w);
    PSMTXScale(mtx, (f32)sx / (f32)GXGetTexObjWidth(&texObj),
               (f32)sy / (f32)GXGetTexObjHeight(&texObj), float_1_8041d070);
    GXLoadTexMtxImm(mtx, 0x1E, 1);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - (f32)h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + w, y - (f32)h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

extern const f32 float_136_8041d0f0;

extern const f32 float_neg800_8041d1b0;
extern const f32 float_neg170_8041d1b4;

extern const f32 float_800_8041d1d8;
extern const f32 float_640_8041d1dc;
extern const f32 float_neg640_8041d1e0;

extern const f32 float_250_8041d1a4;
extern const f32 float_10_8041d1a0;
extern const f32 float_500_8041d1a8;
extern const f32 float_neg130_8041d1ac;
extern const f32 float_120_8041d09c;
extern const f32 float_8_8041d0a0;
extern const f32 float_16_8041d0e4;
extern const f32 float_32_8041d0a4;
extern const f32 float_240_8041d0ac;
extern const f32 float_24_8041d0c8;
extern const f32 float_14_8041d180;
extern const f32 float_80_8041d0b0;
extern const f32 float_20_8041d0c0;
extern const f32 float_4_8041d07c;

static inline void winMsgEdgeV(Vec pos, Vec scale, GXColor color, const char* msg) {
    winFontSetEdge(&pos, &scale, &color, msg);
}
static inline void winMsgFontV(Vec pos, Vec scale, GXColor color, const char* msg) {
    winFontSet(&pos, &scale, &color, msg);
}

// Picks the row colour. Keeping this as an inline helper (rather than the same
// if/else written at the call site) is what makes MWCC home the loop colour in the
// original's 0x8 slot.
static inline GXColor root_color(WindowWork* work, s32 j) {
    if (WW_S32(work, 0x40) == j) {
        return dat_8041cee0;
    }
    return dat_8041cee4;
}

// Builds winTexSet's by-value Vec argument at the call site. Taking the base Vec
// by pointer and doing `v = *base; v.x = x; v.y = y` reproduces the original's
// `translate = vp->vN; translate.x = ..; translate.y = ..` cadence (it loads
// base.x/y/z), while returning it inline keeps the argument evaluation order.
// Without this the loop/tail temporaries and by-value copies land in different
// stack slots. A named local in the caller scores 63%.
#ifdef MUST_MATCH
// MWCC evaluates the last argument first; retail loads x before y.
#define ROOT_COORDS f32 y, f32 x
#define ROOT_VEC(base, x, y) root_vec(base, y, x)
#else
#define ROOT_COORDS f32 x, f32 y
#define ROOT_VEC(base, x, y) root_vec(base, x, y)
#endif
static inline Vec root_vec(const Vec* base, ROOT_COORDS) {
    Vec v = *base;
    v.x = x;
    v.y = y;
    return v;
}

#ifdef MUST_MATCH
// MUST_MATCH: the original reloads each float constant immediately before its volatile
// FIFO store; MWCC otherwise CSEs the constants into f1-f4 and reuses them. Loading
// through a volatile pointer forces the per-store reload (same idiom as party.c).
#define BG_F32(v) (GXWGFifo.f32 = *(volatile f32*)&(v))
#define BG_POS(x, y, z) do { BG_F32(x); BG_F32(y); BG_F32(z); } while (0)
#define BG_TEX(u, v) do { BG_F32(u); BG_F32(v); } while (0)
#else
#define BG_POS(x, y, z) GXPosition3f32(x, y, z)
#define BG_TEX(u, v) GXTexCoord2f32(u, v)
#endif
extern const f32 float_neg8_8041d158;
#ifdef MUST_MATCH
// Existing HalfBook views retained where literal loads still change register allocation.
extern WinRootVecs str_802F3FE8;
#define HB_DBL(off, sym, val) (*(const f64*)(rodata + (off)))
#else
#define HB_DBL(off, sym, val) (val)
#endif
static inline void winHalfBookPage(void* pal, s32 texId, f64 numY, f64 numX,
                                   f32 x0, f32 y0, f32 x1, f32 y1) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sy;
    f64 sx;

    TEXGetGXTexObjFromPalette((TEXPalette*)pal, &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = (f32)numY / (f32)GXGetTexObjHeight(&texObj);
    sx = (f32)numX / (f32)GXGetTexObjWidth(&texObj);
    PSMTXScale(mtx, (f32)sx, (f32)sy, float_1_8041d070);
    GXLoadTexMtxImm(mtx, 0x1E, 1);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x0, y0, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x0, y1, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x1, y1, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x1, y0, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}
#define WIN_BOOK_TEX(w) (**(TEXPalette***)(*(u8**)((u8*)(w) + 0x28) + 0xA0))
extern const char** luigi_book_list[5];
// winFontSetLabel takes its Vec and GXColor arguments by value (each call copies them); win_main.h declares
// pointers, which compiles to the same callee.
// winFontSetLabel takes its Vecs and colour by pointer; the original passes them by value here (caller copies plus a
// pointer, ABI-identical), and only the cast reproduces its argument scheduling.
#ifdef MUST_MATCH
#define winFontSetLabelV ((void (*)(Vec, Vec, GXColor, const char*))winFontSetLabel)
#else
static inline void winFontSetLabelV(Vec pos, Vec scale, GXColor color, const char* msg) {
    winFontSetLabel(&pos, &scale, &color, msg);
}
#endif

// The book frame and pages (type 0: Luigi's book, with the open page's two labels and Luigi reading; type 1:
// a 2x4 grid of frames, 32 lower).
// One textured quad of the book. A macro, not an inline function: each argument is evaluated where it is used,
// after the texture setup, as in the original.
#define WIN_BOOK_PAGE(pal, texId, numY, numX, x0, y0, x1, y1)                       \
    {                                                                              \
        GXTexObj texObj;                                                           \
        Mtx mtx;                                                                   \
        f64 sy;                                                                    \
        f64 sx;                                                                    \
                                                                                   \
        TEXGetGXTexObjFromPalette((TEXPalette*)(pal), &texObj, (texId));           \
        GXInitTexObjLOD(&texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);                \
        GXLoadTexObj(&texObj, 0);                                                  \
        GXSetNumTexGens(1);                                                        \
        GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);                                 \
        sy = __fabs(numY);                                                         \
        sx = __fabs(numX);                                                         \
        sy = (f32)sy / (f32)GXGetTexObjHeight(&texObj);                            \
        sx = (f32)sx / (f32)GXGetTexObjWidth(&texObj);                             \
        PSMTXScale(mtx, (f32)sx, (f32)sy, float_1_8041d070);                       \
        GXLoadTexMtxImm(mtx, 0x1E, 1);                                             \
        GXBegin(0x80, 0, 4);                                                       \
        GXPosition3f32((x0), (y0), 0.0f);                                          \
        GXTexCoord2f32(0.0f, 0.0f);                                                \
        GXPosition3f32((x0), (y1), 0.0f);                                          \
        GXTexCoord2f32(0.0f, 1.0f);                                                \
        GXPosition3f32((x1), (y1), 0.0f);                                          \
        GXTexCoord2f32(1.0f, 1.0f);                                                \
        GXPosition3f32((x1), (y0), 0.0f);                                          \
        GXTexCoord2f32(1.0f, 0.0f);                                                \
    }

// Main menu. Builds the visible tab set into the work block, then runs the
// per-state logic and the common per-tab animation. The string array is the
// unit's static msg_help object; the byte-pointer casts reproduce the original
// `add base,base,idx; lwz disp(base)` reads of tabs[]/recs[].
// The tab-state loop initialization is the only remaining difference: retail copies the counter's initial
// value into cnt (`mr r27, r30`), while MWCC folds that copy to `li r27, 0`.
#pragma opt_common_subs off // keep each field load's own `add base,base,idx` instead of hoisting &work->tabs
// NONMATCHING (99.61%): see the loop-init note above.
s32 winRootMain(WindowWork* work) {
    static const char* msg_help[5] = {
        "msg_menu_mario", "msg_menu_nakama", "msg_menu_mochimono", "msg_menu_badge", "msg_menu_kiroku",
    };
    s32 result;
    WinRootTab* tab;
    s32 i;
    s32 j;
    const char* str;

    switch (WW_S32(work, 0x24)) {
    case 0:
        psndSFXOn((const char*)0x2002A);
        WW_S32(work, 0x44) = 0;
        tab = &WROOT(work)->tabs[WW_S32(work, 0x44)];
        tab->id = 0;
        tab->state = 0;
        tab->y = float_1000_8041d1e8;
        tab->x = float_1000_8041d1e8;
        tab->str = msg_help[0];
        WW_S32(work, 0x44)++;
        if (swByteGet(0) >= 7 || partyChkJoin(1) != 0) {
            tab = &WROOT(work)->tabs[WW_S32(work, 0x44)];
            tab->id = 1;
            tab->state = 0;
            tab->y = float_1000_8041d1e8;
            tab->x = float_1000_8041d1e8;
            tab->str = msg_help[1];
            WW_S32(work, 0x44)++;
        }
        tab = &WROOT(work)->tabs[WW_S32(work, 0x44)];
        tab->id = 2;
        tab->state = 0;
        tab->y = float_1000_8041d1e8;
        tab->x = float_1000_8041d1e8;
        tab->str = msg_help[2];
        WW_S32(work, 0x44)++;
        if (swByteGet(0) >= 0x14 || pouchGetHaveBadgeCnt() != 0) {
            tab = &WROOT(work)->tabs[WW_S32(work, 0x44)];
            tab->id = 3;
            tab->state = 0;
            tab->y = float_1000_8041d1e8;
            tab->x = float_1000_8041d1e8;
            tab->str = msg_help[3];
            WW_S32(work, 0x44)++;
        }
        tab = &WROOT(work)->tabs[WW_S32(work, 0x44)];
        tab->id = 4;
        tab->state = 0;
        tab->y = float_1000_8041d1e8;
        tab->x = float_1000_8041d1e8;
        tab->str = msg_help[4];
        WW_S32(work, 0x44)++;
        for (j = 0; j < WW_S32(work, 0x44); j++) {
            if (WROOT(work)->tabs[j].id == WW_S32(work, 0x1C)) {
                WW_S32(work, 0x40) = j;
            }
        }
        WW_S32(work, 0xCC) = -1;
        WW_F32(work, 0xC8) = float_1000_8041d1e8;
        WW_F32(work, 0xC4) = float_1000_8041d1e8;
        WW_S32(work, 0xC0) = -1;
        WW_S32(work, 0xE0) = -1;
        WW_F32(work, 0xDC) = float_1000_8041d1e8;
        WW_F32(work, 0xD8) = float_1000_8041d1e8;
        WW_S32(work, 0xD4) = -1;
        WW_S32(work, 0xF4) = -1;
        WW_F32(work, 0xF0) = float_1000_8041d1e8;
        WW_F32(work, 0xEC) = float_1000_8041d1e8;
        WW_S32(work, 0xE8) = -1;
        WW_S32(work, 0x108) = -1;
        WW_F32(work, 0x104) = float_1000_8041d1e8;
        WW_F32(work, 0x100) = float_1000_8041d1e8;
        WW_S32(work, 0xFC) = -1;
        WW_S32(work, 0x11C) = -1;
        WW_F32(work, 0x118) = float_1000_8041d1e8;
        WW_F32(work, 0x114) = float_1000_8041d1e8;
        WW_S32(work, 0x110) = -1;
        WW_F32(work, 0xC8) = float_1000_8041d1e8;
        WW_F32(work, 0xC4) = float_1000_8041d1e8;
        WW_S32(work, 0xCC) = 0;
        WW_S32(work, 0xC0) = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->id;
        WW_S32(work, 0x130) = 0;
        WW_F32(work, 0x12C) = float_1000_8041d1e8;
        WW_F32(work, 0x128) = float_1000_8041d1e8;
        WW_S32(work, 0x13C) = 0;
        WW_S32(work, 0x140) = 0;
        WW_S32(work, 0x144) = 0;
        winMarioInit(work);
        winPartyInit(work);
        winItemInit(work);
        winBadgeInit(work);
        winLogInit(work);
        {
            f32 pos = (f32)(WW_S32(work, 0x40) * 0x69 - 0x10E);

            WW_F32(work, 0x158) = pos;
            WW_F32(work, 0x150) = pos;
        }
        WW_F32(work, 0x154) = float_300_8041d0d4;
        WW_F32(work, 0x15C) = float_185_8041d1e4;
        WW_F32(work, 0x16C) = float_neg1000_8041d0cc;
        WW_F32(work, 0x164) = float_neg1000_8041d0cc;
        WW_F32(work, 0x170) = float_neg1000_8041d0cc;
        WW_F32(work, 0x168) = float_neg1000_8041d0cc;
        str = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->str;
        winMsgEntry(work, 0, str, 0);
        WW_S32(work, 0x24) = 0xA;
        switch (WW_S32(work, 0x1C)) {
        case 1:
            winPartyInit2(work);
            WW_S32(work, 0x24) = 0xC;
            work->flags |= 0x4000;
            break;
        case 2:
            winItemInit2(work);
            WW_S32(work, 0x24) = 0xD;
            work->flags |= 0x4000;
            break;
        case 3:
            winBadgeInit2(work);
            WW_S32(work, 0x24) = 0xE;
            work->flags |= 0x4000;
            break;
        case 4:
            winLogInit2(work);
            WW_S32(work, 0x24) = 0xF;
            work->flags |= 0x4000;
            break;
        }
        if (gp->flags & 0x1000) {
            WW_S32(work, 0x44) = 0;
            tab = &WROOT(work)->tabs[WW_S32(work, 0x44)];
            tab->id = 1;
            tab->state = 0;
            tab->y = float_1000_8041d1e8;
            tab->x = float_1000_8041d1e8;
            tab->str = msg_help[1];
            WW_S32(work, 0x44)++;
            winPartyInit2(work);
            WW_S32(work, 0x24) = 0xC;
            WW_S32(work, 0x40) = 0;
        }
        WW_PTR(work, 0x34) = smartAlloc(GXGetTexBufferSize(gp->fbWidth, gp->efbHeight, 4, 0, 0), 1);
        WW_S32(work, 0x38) = 0;
        dispEntry(CAMERA_2D, float_1000_8041d1e8, 0, (DispCallback)unk_8014d5dc, work);
        break;
    case 10:
        if (work->buttonTrg & 0x1200) {
            if (WW_S32(work, 0xCC) == 0x64) {
                WW_S32(work, 0x24) = 0x14;
            }
            break;
        }
        if (work->buttonTrg & 0x100) {
            if (((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->id != 1 || WW_S32(work, 0x1E0) != 0) {
                psndSFXOn((const char*)0x20012);
                WW_S32(work, 0x24) = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->id + 0xB;
                switch (((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->id) {
                case 0:
                    winMarioInit2(work);
                    break;
                case 1:
                    winPartyInit2(work);
                    break;
                case 2:
                    winItemInit2(work);
                    break;
                case 3:
                    winBadgeInit2(work);
                    break;
                case 4:
                    winLogInit2(work);
                    break;
                }
            }
            break;
        }
        if (work->dirRep & 0x4000) {
            psndSFXOn((const char*)0x20032);
            psndSFXOn((const char*)0x20031);
            WW_S32(work, 0x40)--;
            if (WW_S32(work, 0x40) < 0) {
                WW_S32(work, 0x40) = WW_S32(work, 0x44) - 1;
            }
            for (j = 4; j > 0; j--) {
                WROOT(work)->recs[j] = WROOT(work)->recs[j - 1];
            }
            WW_F32(work, 0xC8) = float_1000_8041d1e8;
            WW_F32(work, 0xC4) = float_1000_8041d1e8;
            WW_S32(work, 0xCC) = 0x1E;
            WW_S32(work, 0xC0) = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->id;
            WW_F32(work, 0x158) = (f32)(WW_S32(work, 0x40) * 0x69 - 0x10E);
            str = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->str;
            winMsgEntry(work, 0, str, 0);
        } else if (work->dirRep & 0x8000) {
            psndSFXOn((const char*)0x20032);
            psndSFXOn((const char*)0x20031);
            WW_S32(work, 0x40)++;
            if (WW_S32(work, 0x40) >= WW_S32(work, 0x44)) {
                WW_S32(work, 0x40) = 0;
            }
            for (j = 4; j > 0; j--) {
                WROOT(work)->recs[j] = WROOT(work)->recs[j - 1];
            }
            WW_F32(work, 0xC8) = float_1000_8041d1e8;
            WW_F32(work, 0xC4) = float_1000_8041d1e8;
            WW_S32(work, 0xCC) = 0x14;
            WW_S32(work, 0xC0) = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->id;
            WW_F32(work, 0x158) = (f32)(WW_S32(work, 0x40) * 0x69 - 0x10E);
            str = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->str;
            winMsgEntry(work, 0, str, 0);
        }
        break;
    case 100:
        if (((WinRootRec*)((u8*)WROOT(work)->recs + WW_S32(work, 0x40) * 0x14))->d == 0x64) {
            WW_S32(work, 0x24) = 0xA;
        }
        break;
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        switch (WW_S32(work, 0x24)) {
        case 11:
            result = winMarioMain(work);
            break;
        case 12:
            result = winPartyMain(work);
            break;
        case 13:
            result = winItemMain(work);
            break;
        case 14:
            result = winBadgeMain(work);
            break;
        case 15:
            result = winLogMain(work);
            break;
        }
        if ((gp->flags & 0x1000) && result == -1) {
            result = -2;
        }
        if (result == -1) {
            WW_F32(work, 0x158) = (f32)(WW_S32(work, 0x40) * 0x69 - 0x10E);
            WW_F32(work, 0x15C) = float_185_8041d1e4;
            WW_S32(work, 0x24) = 0xA;
            str = ((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->str;
            winMsgEntry(work, 0, str, 0);
            work->flags &= ~0x4000;
        } else if (result == -2) {
            WW_S32(work, 0x24) = 0x14;
            work->flags &= ~0x4000;
        }
        break;
    case 20:
        psndSFXOn((const char*)0x2002B);
        for (j = 0; j < WW_S32(work, 0x44); j++) {
            WROOT(work)->tabs[j].state = 0xA;
        }
        WW_S32(work, 0xCC) = 0xA;
        WW_S32(work, 0xE0) = 0xA;
        WW_S32(work, 0xF4) = 0xA;
        WW_S32(work, 0x108) = 0xA;
        WW_S32(work, 0x11C) = 0xA;
        WW_S32(work, 0x130) = 0xA;
        N_mapDispOn();
        camDispOn(CAMERA_3D);
        WW_S32(work, 0x24)++;
        break;
    default:
        if (WW_PTR(work, 0x34) != NULL) {
            smartFree((smartEntry*)WW_PTR(work, 0x34));
            WW_PTR(work, 0x34) = NULL;
            WW_S32(work, 0x38) = 0;
        }
        WW_S32(work, 0x24)++;
        if (WW_S32(work, 0x24) > 0x32) {
            winMarioExit(work);
            winPartyExit(work);
            winItemExit(work);
#ifdef MUST_MATCH
            // winBadgeExit's header says void(void), but the original passes work in r3.
            ((void (*)(WindowWork*))winBadgeExit)(work);
#else
            winBadgeExit();
#endif
            winLogExit(work);
            if (WW_S32(work, 0x19C) != 0 && WW_S32(work, 0x1DC) != WW_S32(work, 0x1FC)) {
                return -2;
            }
            if (WW_S32(work, 0x2D4) != 0) {
                return -3;
            }
            return -1;
        }
        break;
    }

    {
    u8* base;
    s32 xoff;
    s32 cnt;

    base = (u8*)work;
    cnt = 0;
    i = cnt;
    xoff = -0xCD;

    for (; i < 5; i++) {
        switch (*(s32*)(base + 0x54)) {
        case 0:
            *(f32*)(base + 0x4C) = (f32)xoff;
            *(f32*)(base + 0x50) = float_300_8041d0d4;
            *(s32*)(base + 0x58) = cnt;
            (*(s32*)(base + 0x54))++;
            break;
        case 1:
        case 11: {
            s32 c = *(s32*)(base + 0x58) - 1;

            *(s32*)(base + 0x58) = c;
            if (c < 0) {
                *(s32*)(base + 0x58) = 0;
                (*(s32*)(base + 0x54))++;
            }
            break;
        }
        case 2: {
            s32 c = *(s32*)(base + 0x58);

            *(s32*)(base + 0x58) = c + 1;
            *(f32*)(base + 0x50) = intplGetValue(0xB, c, 0xA, float_300_8041d0d4, float_185_8041d1e4);
            if (*(s32*)(base + 0x58) > 0xA) {
                (*(s32*)(base + 0x54))++;
            }
            break;
        }
        case 10:
            *(s32*)(base + 0x58) = (4 - i) * 4;
            (*(s32*)(base + 0x54))++;
            break;
        case 12: {
            s32 c = *(s32*)(base + 0x58);

            *(s32*)(base + 0x58) = c + 1;
            *(f32*)(base + 0x50) = intplGetValue(0xB, c, 0xA, float_180_8041d18c, float_300_8041d0d4);
            if (*(s32*)(base + 0x58) > 0xA) {
                (*(s32*)(base + 0x54))++;
            }
            break;
        }
        case 13:
            break;
        }
        base += 0x18;
        xoff += 0x69;
        cnt += 4;
    }
    }
    winBgMain(work);
    winMsgMain(work);
    winMarioMain2(work);
    winPartyMain2(work);
    winItemMain2(work);
    winBadgeMain2(work);
    winLogMain2(work);
    winSortMain(work);
    if (WW_S32(work, 0x24) < 0x14) {
        if (work->flags & 0x4000) {
            { f32 d = WW_F32(work, 0x158) - WW_F32(work, 0x150); WW_F32(work, 0x150) += d * float_0p25_8041d1ec; }
            { f32 d = WW_F32(work, 0x15C) - WW_F32(work, 0x154); WW_F32(work, 0x154) += d * float_0p25_8041d1ec; }
        } else {
            { f32 d = WW_F32(work, 0x158) - WW_F32(work, 0x150); WW_F32(work, 0x150) += d * float_0p25_8041d1ec; }
            { f32 d = WW_F32(work, 0x15C) + (((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->y - float_185_8041d1e4) - WW_F32(work, 0x154); WW_F32(work, 0x154) += d * float_0p25_8041d1ec; }
        }
    } else if (WW_F32(work, 0x154) > float_180_8041d18c) {
            { f32 d = WW_F32(work, 0x158) - WW_F32(work, 0x150); WW_F32(work, 0x150) += d * float_0p25_8041d1ec; }
            { f32 d = WW_F32(work, 0x15C) + (((WinRootTab*)((u8*)WROOT(work)->tabs + WW_S32(work, 0x40) * 0x18))->y - float_185_8041d1e4) - WW_F32(work, 0x154); WW_F32(work, 0x154) += d * float_0p25_8041d1ec; }
    } else {
            { f32 d = WW_F32(work, 0x158) + WW_F32(work, 0xC4) - WW_F32(work, 0x150); WW_F32(work, 0x150) += d * float_0p25_8041d1ec; }
            { f32 d = WW_F32(work, 0x15C) - WW_F32(work, 0x154); WW_F32(work, 0x154) += d * float_0p25_8041d1ec; }
    }
    WW_F32(work, 0x150) = (f32)(WW_F32(work, 0x150) +
        __fabs(float_2_8041d090 * (f32)sin((f32)gp->retraceCount * float_0p125_8041d104)));
    { f32 d = WW_F32(work, 0x16C) - WW_F32(work, 0x164); WW_F32(work, 0x164) += d * float_0p25_8041d1ec; }
    { f32 d = WW_F32(work, 0x170) - WW_F32(work, 0x168); WW_F32(work, 0x168) += d * float_0p25_8041d1ec; }
    dispEntry(CAMERA_2D, float_1000_8041d1e8, 0, (DispCallback)unk_8014d338, work);
    return 0;
}

#pragma opt_common_subs reset

// Named tail templates precede the tab loop's native compound-literal templates.
static const Vec vec3_802f403c = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802f4048 = { 1.0f, 1.0f, 1.0f };
static const Vec vec3_802f4054 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802f4060 = { 1.0f, 1.0f, 1.0f };

// NONMATCHING (97.74%): bgcolor load scheduling and by-value argument-copy stack slots.
void winRootDisp(CameraId camId, WindowWork* work) {
    GXColor bgcolor = dat_8041ced8;
    s32 i;
    s32 j;
    u8* p;

    if (WW_S32(work, 0x24) == 0) {
        return;
    }

    for (i = 4; i >= 0; i--) {
        u8* e = (u8*)work + i * 0x14;
        s32 state = WW_S32(e, 0xCC);

        if (state == -1) {
            continue;
        }
        if (i > 0 && WW_S32(e, 0xB8) == 0x64) {
            continue;
        }
        if (i > 0 && (u32)(state - 0xA) <= 1) {
            continue;
        }

        switch (WW_S32(e, 0xC0)) {
            case 0:
                winMarioDisp(camId, work, i);
                break;
            case 1:
                winPartyDisp(camId, work, i);
                break;
            case 2:
                winItemDisp(camId, work, i);
                break;
            case 3:
                winBadgeDisp(camId, work, i);
                break;
            case 4:
                winLogDisp(camId, work, i);
                break;
        }
    }

    p = (u8*)work;
    for (j = 0; j < WW_S32(work, 0x44); j++) {
        GXColor color;

        color = root_color(work, j);

        winTexInit_x2(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        winTexSet_x2(WW_S32(p, 0x48) + 1, 0xB2,
                     ROOT_VEC(&((Vec){0.0f, 0.0f, 0.0f}), WW_F32(p, 0x4C), WW_F32(p, 0x50)), (Vec){1.0f, 1.0f, 1.0f}, color);
        p += 0x18;
    }

    winMailDisp(work);
    winMsgDisp(work, WW_F32(work, 0x128), WW_F32(work, 0x12C));
    winSortGX(work);

    if (!(work->flags & 0x8000)) {
        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));

        {
            f32 two = float_2_8041d090;
            winTexSet(0, ROOT_VEC(&vec3_802f403c, two + WW_F32(work, 0x150), WW_F32(work, 0x154) - two), vec3_802f4048,
                      dat_8041cedc);
        }

        winTexSet(0, ROOT_VEC(&vec3_802f4054, WW_F32(work, 0x150), WW_F32(work, 0x154)), vec3_802f4060, bgcolor);
    }
}

// 5-page switch over work->0xcc. The explicit empty case -1 below (a redirect
// target of cases 11 and 41) is what makes MWCC pick 0 as the [0,10) search-tree
// pivot; without it the tree pivots on 1.
void winBgMain(WindowWork* work) {
    WindowWork* p = work;
    s32 i;

    for (i = 0; i < 5; i++) {
        switch (WW_S32(p, 0xCC)) {
            case 0:
                WW_F32(p, 0xC4) = float_800_8041d1d8;
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 1: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x10, float_800_8041d1d8, float_0_8041d068);
                if (WW_S32(p, 0xD0) > 0x10) {
                    WW_S32(p, 0xCC) = 0x64;
                }
                break;
            }
            case 10:
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 11: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x10, float_0_8041d068, float_800_8041d1d8);
                if (WW_S32(p, 0xD0) > 0x10) {
                    WW_S32(p, 0xCC) = -1;
                }
                break;
            }
            case 20:
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 21: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x18, float_640_8041d1dc, float_0_8041d068);
                if (WW_S32(p, 0xD0) > 0x18) {
                    WW_S32(p, 0xCC) = 0x64;
                }
                break;
            }
            case 30:
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 31: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x18, float_neg640_8041d1e0, float_0_8041d068);
                if (WW_S32(p, 0xD0) > 0x18) {
                    WW_S32(p, 0xCC) = 0x64;
                }
                break;
            }
            case 40:
                WW_F32(p, 0xC8) = float_0_8041d068;
                WW_S32(p, 0xD0) = 0;
                WW_S32(p, 0xCC) = WW_S32(p, 0xCC) + 1;
                break;
            case 41: {
                s32 step = WW_S32(p, 0xD0);
                WW_S32(p, 0xD0) = step + 1;
                WW_F32(p, 0xC4) = intplGetValue(0xB, step, 0x18, float_0_8041d068, float_neg640_8041d1e0);
                if (WW_S32(p, 0xD0) > 0x18) {
                    WW_S32(p, 0xCC) = -1;
                }
                break;
            }
#ifdef MUST_MATCH
            /* MUST_MATCH: -1 is a redirect target of cases 11/41; the empty case makes
               MWCC emit the retail search-tree pivot (0 then 2) instead of 1 then 0. */
            case -1:
#endif
            case 100:
                break;
            default:
                break;
        }
        p = (WindowWork*)((u8*)p + 0x14);
    }
}

void winBgGX(WindowWork* work, s32 type, f32 x, f32 y) {
    GXTexObj texObj;
    Mtx mtx;
    f32 scaleY;

    switch (type) {
        case 0:
            GXSetTevColor(1, dat_8041cee8);
            GXSetTevColor(2, dat_8041ceec);
            GXSetTevColor(3, dat_8041cef0);
            break;
        case 1:
            GXSetTevColor(1, dat_8041cef4);
            GXSetTevColor(2, dat_8041cef8);
            GXSetTevColor(3, dat_8041cefc);
            break;
        case 2:
            GXSetTevColor(1, float_neg0p0047648_8041cf00);
            GXSetTevColor(2, dat_8041cf04);
            GXSetTevColor(3, dat_8041cf08);
            break;
        case 3:
            GXSetTevColor(1, dat_8041cf0c);
            GXSetTevColor(2, dat_8041cf10);
            GXSetTevColor(3, dat_8041cf14);
            break;
        case 4:
            GXSetTevColor(1, dat_8041cf18);
            GXSetTevColor(2, dat_8041cf1c);
            GXSetTevColor(3, dat_8041cf20);
            break;
    }

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, float_0_8041d068, float_0_8041d068, float_0_8041d068, float_0_8041d068, dat_8041cf24);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
    GXSetTevAlphaIn(0, 7, 7, 7, 4);
    GXSetTevSwapMode(0, 0, 0);

    PSMTXTrans(mtx, x, float_18_8041d1b8 + y, float_0_8041d068);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx, mtx);
    GXLoadPosMtxImm(mtx, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, 7);
    GXInitTexObjLOD(&texObj, 0, 0, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    scaleY = float_320_8041d074 / (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx, float_570_8041d1bc / (f32)GXGetTexObjWidth(&texObj), scaleY, float_1_8041d070);
    GXLoadTexMtxImm(mtx, 0x1E, 1);

    GXBegin(0x80, 0, 4);
    BG_POS(float_neg285_8041d1c0, float_160_8041d1c4, float_0_8041d068);
    BG_TEX(float_0_8041d068, float_0_8041d068);
    BG_POS(float_neg285_8041d1c0, float_neg160_8041d1c8, float_0_8041d068);
    BG_TEX(float_0_8041d068, float_1_8041d070);
    BG_POS(float_0_8041d068, float_neg160_8041d1c8, float_0_8041d068);
    BG_TEX(float_1_8041d070, float_1_8041d070);
    BG_POS(float_0_8041d068, float_160_8041d1c4, float_0_8041d068);
    BG_TEX(float_1_8041d070, float_0_8041d068);

    GXBegin(0x80, 0, 4);
    BG_POS(float_0_8041d068, float_160_8041d1c4, float_0_8041d068);
    BG_TEX(float_1_8041d070, float_0_8041d068);
    BG_POS(float_0_8041d068, float_neg160_8041d1c8, float_0_8041d068);
    BG_TEX(float_1_8041d070, float_1_8041d070);
    BG_POS(float_285_8041d1cc, float_neg160_8041d1c8, float_0_8041d068);
    BG_TEX(float_0_8041d068, float_1_8041d070);
    BG_POS(float_285_8041d1cc, float_160_8041d1c4, float_0_8041d068);
    BG_TEX(float_0_8041d068, float_0_8041d068);

    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, 6);
    GXInitTexObjLOD(&texObj, 0, 0, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    scaleY = float_288_8041d16c / (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx, float_570_8041d1bc / (f32)GXGetTexObjWidth(&texObj), scaleY, float_1_8041d070);
    GXLoadTexMtxImm(mtx, 0x1E, 1);

    GXSetTevColorIn(0, 4, 6, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 7, 7, 3);

    GXBegin(0x80, 0, 4);
    BG_POS(float_neg285_8041d1c0, float_144_8041d1d0, float_0_8041d068);
    BG_TEX(float_0_8041d068, float_0_8041d068);
    BG_POS(float_neg285_8041d1c0, float_neg144_8041d1d4, float_0_8041d068);
    BG_TEX(float_0_8041d068, float_1_8041d070);
    BG_POS(float_285_8041d1cc, float_neg144_8041d1d4, float_0_8041d068);
    BG_TEX(float_1_8041d070, float_1_8041d070);
    BG_POS(float_285_8041d1cc, float_144_8041d1d0, float_0_8041d068);
    BG_TEX(float_1_8041d070, float_0_8041d068);
}

void winMsgMain(WindowWork* work) {
    switch (WW_S32(work, 0x130)) {
        case 0:
            WW_F32(work, 0x128) = float_neg800_8041d1b0;
            WW_F32(work, 0x12C) = float_neg170_8041d1b4;
            WW_S32(work, 0x134) = 0;
            WW_S32(work, 0x130) = WW_S32(work, 0x130) + 1;
            break;
        case 1: {
            s32 step = WW_S32(work, 0x134);
            WW_S32(work, 0x134) = step + 1;
            WW_F32(work, 0x128) = intplGetValue(0xB, step, 0x10, float_neg800_8041d1b0, float_0_8041d068);
            if (WW_S32(work, 0x134) > 0x10) {
                WW_S32(work, 0x130) = WW_S32(work, 0x130) + 1;
            }
            break;
        }
        case 2:
            if (keyGetDirTrg(0) & 0x100) {
                WW_S32(work, 0x148) = WW_S32(work, 0x148) - 2;
                if (WW_S32(work, 0x148) < 0) {
                    WW_S32(work, 0x148) = 0;
                } else {
                    psndSFXOn((const char*)0x20006);
                }
            } else if (keyGetDirTrg(0) & 0x200) {
                if (WW_S32(work, 0x148) / 2 < (WW_S32(work, 0x14C) + 1) / 2 - 1) {
                    WW_S32(work, 0x148) = WW_S32(work, 0x148) + 2;
                    psndSFXOn((const char*)0x20006);
                }
            }
            break;
        case 10:
            WW_S32(work, 0x134) = 0;
            WW_S32(work, 0x130) = WW_S32(work, 0x130) + 1;
            break;
        case 11: {
            s32 step = WW_S32(work, 0x134);
            WW_S32(work, 0x134) = step + 1;
            WW_F32(work, 0x128) = intplGetValue(0xB, step, 0x10, float_0_8041d068, float_neg800_8041d1b0);
            if (WW_S32(work, 0x134) > 0x10) {
                WW_S32(work, 0x130) = WW_S32(work, 0x130) + 1;
            }
            break;
        }
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 12:
        default:
            break;
    }
}

// Most of the 10/40/120/240/250 position sums stay literals: MWCC pools each literal at the named
// object's slot, so the code is the same either way (objdiff reports only a symbol-only diff); a couple
// of sites read the named objects directly.
// The 0x140 pointer is loaded once and shared by the NULL test and msgSearch, and ty is declared before width so
// the width/ty pair lands in f26/f27 as in the original. The rest are scratch-register assignments of compiler
// temporaries: the width/ty int-to-float sequence, the `fy -= 14` / `240 - fy` pair, and the icon block's
// `40 + (y - 40)` / `(250 + x) - 10`. The `msg_lottery_number` offset (0x5c0 vs 0x5d0) follows winHalfBookGX's
// still-missing .rodata conversion doubles.
// NONMATCHING (99.81%): scratch-register allocation of compiler temporaries in three float blocks, plus the
// winHalfBookGX-dependent string offset.
void winMsgDisp(WindowWork* work, f32 x, f32 y) {
    char buf[0x400];
    GXColor iconColor = dat_8041cf28;
    GXColor fontColor = dat_8041cf2c;
#ifdef MUST_MATCH
    // Semantically x - 250.0f; the original computes it as +10 then -10 (MUST_MATCH codegen shaping).
    f32 fx = (10.0f + (x - 250.0f)) - 10.0f;
    f32 fy = 40.0f + y;
#else
    f32 fx = x - 250.0f;
    f32 fy = 40.0f + y;
#endif

    windowDispGX_Waku_col(0, dat_8041cf30, fx, fy, float_500_8041d1a8, float_80_8041d0b0, float_20_8041d0c0);

    if (WW_S32(work, 0x138) != 0 && WW_S32(work, 0x138) > 0) {
        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        winTexSet(0x25, (Vec){ fx, fy, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, iconColor);
        winIconInit();
        winMailIconV(((0, (ItemDataView*)itemDataTable)[WW_S32(work, 0x138)]).icon_id, (Vec){ fx, fy, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, iconColor);
    }

    if (WW_PTR(work, 0x140) != NULL) {
        const char* msg;
        f32 ty;
        f32 width;
        f32 edge;

        edge = (WW_S32(work, 0x138) == -2) ? float_neg130_8041d1ac : float_0_8041d068;
        msg = msgSearch((const char*)WW_PTR(work, 0x140));
        width = FontGetMessageWidth(msg);
        ty = float_8_8041d0a0 + (float_16_8041d0e4 + fy);
        winNameGX(work, 1, (x - 120.0f) + edge, ty, 240.0f, float_32_8041d0a4);
        winFontInit();
        winMsgEdgeV((Vec){ edge + ((240.0f - width) * float_0p5_8041d06c + (x - 120.0f)), ty - float_4_8041d07c, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, iconColor, msg);
    }

    if (WW_U32(work, 0x13C) != 0) {
        fy -= float_14_8041d180;
        GXSetScissor(0, (u32)(240.0f - fy), 0x260, 0x38);
        if (strcmp((const char*)WW_PTR(work, 0x13C), "msg_lottery_number") == 0) {
            // lottery_info has no declared field at 0x20; the original loads an s16 there.
            sprintf(buf, msgSearch((const char*)WW_PTR(work, 0x13C)), *(s16*)((u8*)lotteryGetPtr() + 0x20));
            winFontInit();
            winMsgFontV((Vec){ (40.0f + (x - 250.0f)) - float_10_8041d1a0, fy + (f32)(WW_S32(work, 0x148) * 0x1C), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, fontColor, buf);
        } else {
            winFontInit();
            winMailLabelV((Vec){ (40.0f + (x - 250.0f)) - 10.0f, fy + (f32)(WW_S32(work, 0x148) * 0x1C), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, fontColor, (const char*)WW_PTR(work, 0x13C));
        }
        GXSetScissor(0, 0, 0x260, 0x1E0);
    }

    if (WW_S32(work, 0x14C) > 2) {
        s32 i;

        winIconInit();
        i = WW_S32(work, 0x148);
        if (i == 0) {
            if ((gp->retraceCount & 0x1F) < 0x14) winMailIconV(0x1B6, (Vec){ (float_250_8041d1a4 + x) - 10.0f, (40.0f + (y - 40.0f)) - float_24_8041d0c8, 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, dat_8041cf34);
        } else if (i / 2 == (WW_S32(work, 0x14C) + 1) / 2 - 1) {
            if ((gp->retraceCount & 0x1F) < 0x14) winMailIconV(0x1B5, (Vec){ (250.0f + x) - 10.0f, float_24_8041d0c8 + (40.0f + (y - 40.0f)), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, dat_8041cf38);
        } else if ((gp->retraceCount & 0x1F) < 0x14) {
            winMailIconV(0x1B5, (Vec){ (250.0f + x) - 10.0f, float_24_8041d0c8 + (40.0f + (y - 40.0f)), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, dat_8041cf3c);
            winMailIconV(0x1B6, (Vec){ (250.0f + x) - 10.0f, (40.0f + (y - 40.0f)) - float_24_8041d0c8, 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, dat_8041cf40);
        }
        winMailIconV(0x70, (Vec){ (250.0f + x) - 10.0f, 40.0f + (y - 40.0f), 0.0f }, (Vec){ 0.6f, 0.6f, 0.6f }, dat_8041cf44);
    }
}

void winMsgEntry(WindowWork* work, s32 a1, const char* str, s32 a3) {
    u16 width;
    BOOL changed = FALSE;

    if (str != NULL) {
        if (WW_PTR(work, 0x144) != (void*)str) {
            changed = TRUE;
        } else if (strcmp(msgbuf, str) != 0) {
            changed = TRUE;
        }
        if (changed) {
            WW_S32(work, 0x138) = a1;
            WW_PTR(work, 0x13C) = (void*)str;
            WW_S32(work, 0x140) = a3;
            FontGetMessageWidthLine((const char*)msgSearch((const char*)WW_PTR(work, 0x13C)), &width);
            WW_S32(work, 0x148) = 0;
            WW_S32(work, 0x14C) = width + 1;
            WW_PTR(work, 0x144) = (void*)str;
            strncpy(msgbuf, str, 0x3C);
        }
    }
}

#pragma inline_max_total_size(60000) // keep the 20 winKirinukiQuad instances inlined
#pragma inline_max_size(1000)
// The 9920-byte body reconstructed from the asm; the frame (0x8b0, saving f19..f31) matches.
//  * The 8.0 and 16.0 are literals: MWCC CSEs `w - 8.0f - 8.0f` and the
//    like across the GX calls (the original computes each once and reuses it for the scale and the quad),
//    which it does not do for reads of the named float_8/float_16 objects. 90.0 and 74.0 stay named reads
//    (as literals both score lower). The quads use the `w - 8`/`h - 8 - 8` expressions directly. The
//    else-branch 7th quad and the two x offsets after it group `x + (w - 90)` so MWCC computes that sum
//    once (a plain `x + w - 90` reassociates and adds an instruction). The second x offset (`x + (w - 90)
//    + 8`) is held in the same local as `yscale`, which is dead at that point; reusing one local is what
//    keeps the instruction multiset equal to the original's (a second local makes MWCC recompute a sum).
//  * The else-branch 7th quad's width is w-8 (single fsubs), not w-16, and the else-branch texture ids at
//    quads 11/12 are C then 8. Every texture id (if0 B,9,D,A,8,C,8,E; else B,9,D,A,8,C,8,F,8,A,8,C,8,E),
//    every scale W/H and every quad rectangle was checked against the asm.
//  * `winKirinukiQuad` must be inlined at all 20 call sites (the inline_max pragmas below); without them
//    MWCC outlines it and the function drops to 61.68%. Turning the kxw74 assignment into an assignment
//    expression has made mwcceppc loop forever.
// NONMATCHING (99.11%): the instruction multiset is exact and the four parameter saves now match the
// target (f28-f31). Ten frontend CSE temporaries still colour into f24-f27 differently (they overlap
// their neighbours here where the target's do not); that is the permuter's remaining 10-register case.
void winKirinukiGX(WindowWork* work, s32 type, f32 x, f32 y, f32 w, f32 h) {
    f32 yscale;
    f32 kxw74;

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f,
             dat_8041cf48);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
    GXSetCurrentMtx(0);

    if (type == 0) {
        WIN_KIRINUKI_SCALE(0xB, 8.0, 8.0);
        winKirinukiQuad(x, y, 8.0f, 8.0f);
        WIN_KIRINUKI_SCALE(0x9, w - 8.0f - 8.0f, 8.0);
        winKirinukiQuad(x + 8.0f, y, w - 8.0f - 8.0f, 8.0f);
        WIN_KIRINUKI_SCALE(0xD, 8.0, 8.0);
        winKirinukiQuad(x + w - 8.0f, y, 8.0f, 8.0f);
        WIN_KIRINUKI_SCALE(0xA, 8.0, h - 8.0f - 8.0f);
        winKirinukiQuad(x, y - 8.0f, 8.0f, (h - 8.0f - 8.0f));
        WIN_KIRINUKI_SCALE(0x8, w - 8.0f, (h - 8.0f - 8.0f));
        winKirinukiQuad(x + 8.0f, y - 8.0f, (w - 8.0f),
                        (h - 8.0f - 8.0f));
        WIN_KIRINUKI_SCALE(0xC, 8.0, 8.0);
        winKirinukiQuad(x, y - (h - 8.0f), 8.0f, 8.0f);
        WIN_KIRINUKI_SCALE(0x8, w - 8.0f - 8.0f, 8.0);
        winKirinukiQuad(x + 8.0f, y - (h - 8.0f),
                        w - 8.0f - 8.0f, 8.0f);
        WIN_KIRINUKI_SCALE(0xE, 8.0, 8.0);
        winKirinukiQuad(x + w - 8.0f, y - (h - 8.0f), 8.0f,
                        8.0f);
    } else {
        WIN_KIRINUKI_SCALE(0xB, 8.0, 8.0);
        winKirinukiQuad(x, y, 8.0f, 8.0f);
        WIN_KIRINUKI_SCALE(0x9, w - 8.0f - 8.0f, 8.0);
        winKirinukiQuad(x + 8.0f, y, w - 8.0f - 8.0f, 8.0f);
        WIN_KIRINUKI_SCALE(0xD, 8.0, 8.0);
        winKirinukiQuad(x + w - 8.0f, y, 8.0f, 8.0f);
        WIN_KIRINUKI_SCALE(0xA, 8.0, h - 8.0f - 8.0f);
        winKirinukiQuad(x, y - 8.0f, 8.0f, (h - 8.0f - 8.0f));
        WIN_KIRINUKI_SCALE(0x8, w - 8.0f, (h - 8.0f - 8.0f));
        winKirinukiQuad(x + 8.0f, y - 8.0f, (w - 8.0f),
                        (h - 8.0f - 8.0f));
        WIN_KIRINUKI_SCALE(0xC, 8.0, 8.0);
        winKirinukiQuad(x, y - (h - 8.0f), 8.0f, 8.0f);
        WIN_KIRINUKI_SCALE(0x8, w - 8.0f, 8.0);
        winKirinukiQuad(x + 8.0f, y - (h - 8.0f),
                        w - 8.0f, 8.0f);
        WIN_KIRINUKI_SCALE(0xF, 16.0, 8.0);
        winKirinukiQuad(x + (w - float_90_8041d194), y - h, 16.0f, 8.0f);
        kxw74 = x + (w - float_90_8041d194) + 16.0f;
        yscale = x + (w - float_90_8041d194) + 8.0f;
        WIN_KIRINUKI_SCALE(0x8, float_74_8041d198, 8.0);
        winKirinukiQuad(kxw74, y - h, float_74_8041d198,
                        8.0f);
        WIN_KIRINUKI_SCALE(0xA, 8.0, 8.0);
        winKirinukiQuad(yscale, y - h - 8.0f,
                        8.0f, 8.0f);
        WIN_KIRINUKI_SCALE(0x8, float_74_8041d198, 8.0);
        winKirinukiQuad(kxw74, y - h - 8.0f,
                        float_74_8041d198, 8.0f);
        WIN_KIRINUKI_SCALE(0xC, 8.0, 8.0);
        winKirinukiQuad(yscale, y - h - 16.0f,
                        8.0f, 8.0f);
        WIN_KIRINUKI_SCALE(0x8, float_66_8041d19c, 8.0);
        winKirinukiQuad(kxw74, y - h - 16.0f,
                        float_66_8041d19c, 8.0f);
        WIN_KIRINUKI_SCALE(0xE, 8.0, 8.0);
        winKirinukiQuad(x + w - 8.0f, y - h - 16.0f, 8.0f,
                        8.0f);
    }
}

#pragma inline_max_total_size reset
#pragma inline_max_size reset

// NONMATCHING (99.31%): all instructions match; retail colours the inlined
// quad temps f26/f27/f28 and saves one more FPR (frame 0x280 vs our 0x270).
// The winNameQuad/nameQuadTw mix is the minimum-diff colouring found.
void winNameGX(WindowWork* work, s32 type, f32 x, f32 y, f32 w, f32 h) {
    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041cf4c);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
    GXSetCurrentMtx(0);

    if (type == 0) {
        winNameQuad(work, 0x13, x, y, w - 16.0f, 32.0f);
        nameQuadTw(work, 0x14, x + w - 16.0f, y, 16.0f, 32.0f);
    } else if (type == 1) {
        nameQuadTw(work, 0x15, x, y, w - 16.0f, 32.0f);
        nameQuadTw(work, 0x16, x + w - 16.0f, y, 16.0f, 32.0f);
    } else if (type == 2) {
        nameQuadTw(work, 0x78, x, y, w - 8.0f, 24.0f);
        nameQuadTw(work, 0x79, x + w - 8.0f, y, 8.0f, 24.0f);
    }
}

// NONMATCHING (91.60%): page-span FPR allocation and by-value label copies differ.
void winBookGX(WindowWork* work, s32 type, f32 x, f32 y) {
    f32 yofs;
    s32 i;

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041cf50);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
    GXSetCurrentMtx(0);

    yofs = type == 0 ? 0.0f : 32.0f;

    // the page area and the top edge
    WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x33, 256.0f - yofs, 432.0,
                  8.0f + x, y - 8.0f, (8.0f + x) + 432.0f, ((y - 8.0f) - (256.0f - yofs)))
    WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x28, 8.0, 16.0,
                  x, y, x + float_16_8041d0e4, y - float_8_8041d0a0)
    WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x2C, 8.0, 184.0,
                  (16.0f + x), y, (float_16_8041d0e4 + x) + 184.0f, y - 8.0f)
    WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x26, 16.0, 48.0,
                  (184.0f + (16.0f + x)), y, (184.0f + (float_16_8041d0e4 + x)) + 48.0f, y - 16.0f)
    WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x2D, 8.0, 184.0,
                  (48.0f + (184.0f + (16.0f + x))), y, (float_48_8041d11c + (184.0f + (16.0f + x))) + 184.0f, y - 8.0f)
    WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x2A, 8.0, 16.0,
                  (184.0f + (48.0f + (184.0f + (16.0f + x)))), y, (184.0f + (48.0f + (184.0f + (16.0f + x)))) + 16.0f, y - 8.0f)

    // the sides and the spine
    WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x30, 256.0f - yofs, 8.0,
                  x, y - 8.0f, x + 8.0f, ((y - 8.0f) - (256.0f - yofs)))
    WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x32, 248.0f - yofs, 16.0,
                  8.0f + (184.0f + (16.0f + x)), y - 16.0f, (8.0f + (184.0f + (16.0f + x))) + 16.0f, (y - 16.0f) - (248.0f - yofs))
    WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x31, 256.0f - yofs, 8.0,
                  8.0f + (184.0f + (48.0f + (184.0f + (16.0f + x)))), y - 8.0f, (8.0f + (184.0f + (48.0f + (184.0f + (16.0f + x))))) + 8.0f, ((y - 8.0f) - (256.0f - yofs)))

    // the bottom edge
    WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x29, 16.0, 16.0,
                  x, (((y - 8.0f) - 256.0f) + yofs), x + float_16_8041d0e4, (((y - 8.0f) - 256.0f) + yofs) - 16.0f)
    WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x2E, 16.0, 184.0,
                  (16.0f + x), (((y - 8.0f) - 256.0f) + yofs), (16.0f + x) + 184.0f, (((y - 8.0f) - 256.0f) + yofs) - 16.0f)
    WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x27, 24.0, 48.0,
                  (184.0f + (16.0f + x)), (((y - 8.0f) - 256.0f) + yofs), (184.0f + (16.0f + x)) + 48.0f, (((y - 8.0f) - 256.0f) + yofs) - 24.0f)
    WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x2F, 16.0, 184.0,
                  (48.0f + (184.0f + (16.0f + x))), (((y - 8.0f) - 256.0f) + yofs), (48.0f + (184.0f + (16.0f + x))) + 184.0f, (((y - 8.0f) - 256.0f) + yofs) - 16.0f)
    WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x2B, 16.0, 16.0,
                  (184.0f + (48.0f + (184.0f + (16.0f + x)))), (((y - 8.0f) - 256.0f) + yofs), (184.0f + (48.0f + (184.0f + (16.0f + x)))) + 16.0f, (((y - 8.0f) - 256.0f) + yofs) - 16.0f)

    if (type == 1) {
        for (i = 0; i < 8; i++) {
            f32 cx = (float_16_8041d0e4 + x) + (f32)((i % 2) * 216);
            f32 cy = (y - 32.0f) - (f32)((i / 2) * 48);

            WIN_BOOK_PAGE(WIN_BOOK_TEX(work), 0x34, 32.0,
                            192.0, cx, cy, cx + 192.0f, cy - 32.0f)
        }
    }

    if (type == 0) {
        s32 pose;
        AnimData* anim;
        f32 labelX;

        winFontInit();
        labelX = 20.0f + x;
        winFontSetLabelV((Vec){labelX, y - 14.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f}, dat_8041cf54,
                         luigi_book_list[WW_S32(work, 0x2C8)][WW_S32(work, 0x2D0) * 2]);
        winFontSetLabelV((Vec){220.0f + labelX, y - 14.0f, 0.0f}, (Vec){0.8f, 0.8f, 0.8f},
                         dat_8041cf54,
                         luigi_book_list[WW_S32(work, 0x2C8)][WW_S32(work, 0x2D0) * 2 + 1]);

        // Luigi, reading: the closing pose on the last page
        pose = L_msgGetWork()->poseId;
        anim = animPoseGetAnimDataPtr(pose);
        if (WW_S32(work, 0x2D0) < WW_S32(work, 0x2CC) - 1) {
            animPoseSetAnim(pose, "HOS_C_1", 0);
        } else {
            animPoseSetAnim(pose, "HOS_E_1", 0);
        }
        animPoseSetLocalTime(pose, gp->retraceCount % (s32)*(f32*)(*(u8**)((u8*)anim + 0x24) + 0x8));
        animPoseMain(pose);
        animPoseDraw(pose, 425.0f + x, y - 275.0f, 0.0f, 180.0f, 5.0f, 2);
    }
}

// The last differences are the preheader's spill widths and the int->float mask's
// addressing. Retail keeps the nine __fabs doubles alive as f64 and spills several raw
// doubles (stfd/lfd) before the hoisted (f32) conversions, and reaches its 2^52 mask
// through the str_802F3FE8 base (0x4e0(rBase)); MWCC here emits an extra `lis` for the
// mask and spills some conversions as f32 (stfs/lfs), which is one extra instruction
// overall (+6 lfs, -3 lfd, +2 stfs, +1 fadds, +1 lis). Calls, immediates and the
// inlined winHalfBookPage bodies all match.
// NONMATCHING (95.92%): preheader spill widths and int->float mask addressing.
void winHalfBookGX(void* pal, f32 x, f32 y, s32 a4) {
#ifdef MUST_MATCH
    const u8* rodata = (const u8*)&str_802F3FE8;
#endif
    f32 y16;
    f32 y8;
    f32 x264;
    f32 x312;
    f32 y264;
    f32 x8;
    f32 x16;
    f32 x32;
    f32 x320;
    f32 y200;
    f32 y256;
    f64 sn16;
    f64 s24;
    f64 s40;
    f64 s320;
    f64 s16;
    f64 s48;
    f64 s248;
    f32 s8;
    f32 s240;
    Mtx mtx;
    s32 i;

    (void)a4;

    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041cf5c);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOp(0, 0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

    y8 = y - 8.0f;
    y16 = y - 16.0f;
    y200 = y - 200.0f;
    y256 = y - 256.0f;
    y264 = y256 - 8.0f;
    x8 = x + 8.0f;
    x16 = x + float_16_8041d0e4;
    x32 = x + 32.0f;
    x264 = x16 + 248.0f;
    x312 = x264 + 48.0f;
    x320 = x + 320.0f;

    s8 = __fabs(HB_DBL(0x548, double_8_802f4530, 8.0));
    sn16 = __fabs(HB_DBL(0x5A0, double_neg16_802f4588, -16.0));
    s320 = __fabs(HB_DBL(0x598, double_320_802f4580, 320.0));
    s248 = __fabs(HB_DBL(0x580, double_248_802f4568, 248.0));
    s40 = __fabs(HB_DBL(0x4F8, double_40_802f44e0, 40.0));
    s48 = __fabs(HB_DBL(0x558, double_48_802f4540, 48.0));
    s16 = __fabs(HB_DBL(0x500, double_16_802f44e8, 16.0));
    s240 = __fabs(HB_DBL(0x588, double_240_802f4570, 240.0));
    s24 = __fabs(HB_DBL(0x4F0, double_24_802f44d8, 24.0));

    for (i = 0; i < 2; i++) {
        if (i == 0) {
            GXSetChanMatColor(4, dat_8041cf60);
            GXSetBlendMode(1, 4, 5, 0);
            GXSetZCompLoc(1);
            GXSetAlphaCompare(7, 0, 0, 7, 0);
            GXSetZMode(0, 7, 0);
            PSMTXTrans(mtx, float_8_8041d0a0, float_neg8_8041d158, 0.0f);
            PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx, mtx);
            GXLoadPosMtxImm(mtx, 0);
            GXSetCurrentMtx(0);
        } else {
            GXSetChanMatColor(4, dat_8041cf64);
            GXSetBlendMode(0, 1, 0, 0);
            GXSetZCompLoc(0);
            GXSetAlphaCompare(6, 0x80, 1, 0, 0);
            GXSetZMode(0, 7, 0);
            GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
            GXSetCurrentMtx(0);
        }

        winHalfBookPage(pal, 0x33, s248, s320, x8,           y8,   x8 + 320.0f,              y8 - 248.0f);
        winHalfBookPage(pal, 0x28, s8,   s16,  x,            y,    x16,                      y8);
        winHalfBookPage(pal, 0x35, s8,   s248, x16,          y,    x264,                     y8);
        winHalfBookPage(pal, 0x26, s16,  s48,  x264,         y,    x312,                     y16);
        winHalfBookPage(pal, 0x3B, s16,  s24,  x312,         y,    x312 + 24.0f,             y16);
        winHalfBookPage(pal, 0x30, s248, s8,   x,            y8,   x8,                       y8 - 248.0f);
        winHalfBookPage(pal, 0x32, s240, s16,  x264 + 8.0f,  y16,  (x264 + 8.0f) + 16.0f,    y16 - float_240_8041d0ac);
        winHalfBookPage(pal, 0x36, s40,  s248, x16,          y200, x264,                     y200 - 40.0f);
        winHalfBookPage(pal, 0x3A, s8,   s16,  x,            y256, x16,                      y264);
        winHalfBookPage(pal, 0x38, s8,   sn16, x32,          y256, x32 + -16.0f,              y264);
        winHalfBookPage(pal, 0x38, s8,   s240, x32,          y256, x32 + float_240_8041d0ac,             y264);
        winHalfBookPage(pal, 0x39, s8,   s16,  x + 272.0f,   y256, (x + 272.0f) + 16.0f,     y264);
        winHalfBookPage(pal, 0x38, s8,   s16,  x + float_288_8041d16c,   y256, (x + 288.0f) + 16.0f,     y264);
        winHalfBookPage(pal, 0x38, s8,   sn16, x320,         y256, x320 + -16.0f,             y264);
        winHalfBookPage(pal, 0x38, s8,   s16,  x320,         y256, x320 + 16.0f,             y264);
        winHalfBookPage(pal, 0x3C, s240, s8,   x + 328.0f,   y16,  (x + 328.0f) + 8.0f,      y16 - float_240_8041d0ac);

    }
}

// The mail window frame: nineteen textured quads (outer border and inner panels),
// all drawn with the same inline quad helper. The native texture sizes are the f64
// literals MWCC pools with its own i2f magic; the drawn rectangles are f32 sums of
// the caller's x/y, reproducing retail's left-to-right summation chains. The helper
// is byte-perfect with its `f64 sx;`/`f64 sy;` declaration order and the h-then-w fabs.
// NONMATCHING (99.83%): only the global FPR of `x` (retail f30, ours f29) and
// its deep-sum temporaries differ; all 212 calls and immediates match.
void winMailGX(WindowWork* work, f32 x, f32 y) {
    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041cf68);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
    GXSetCurrentMtx(0);

    winMailQuad(work, 0x4D, x, y, 32.0f, 24.0);
    winMailQuad(work, 0x63, x + 32.0f, y, 344.0f, 24.0);
    winMailQuad(work, 0x64, x + 200.0f, y, 16.0f, 24.0);
    winMailQuad(work, 0x55, x + 32.0f, y - 24.0f, 344.0f, 8.0);
    winMailQuad(work, 0x4E, 16.0f + (40.0f + (112.0f + (16.0f + (48.0f + (40.0f + (72.0f + (x + 32.0f))))))), y, 40.0f, 24.0);
    winMailQuad(work, 0x51, x, y - 24.0f, 16.0f, 248.0);
    winMailQuad(work, 0x55, x + 16.0f, y - 24.0f, 16.0f, 248.0);
    winMailQuad(work, 0x55, 16.0f + (248.0f + (8.0f + (72.0f + (16.0f + (16.0f + x))))), y - 24.0f, 16.0f, 248.0);
    winMailQuad(work, 0x51, 16.0f + (16.0f + (248.0f + (8.0f + (72.0f + (16.0f + (16.0f + x)))))), y - 24.0f, 16.0f, 248.0);
    winMailQuad(work, 0x52, 16.0f + (16.0f + (16.0f + (248.0f + (8.0f + (72.0f + (16.0f + (16.0f + x))))))), y - 24.0f, 8.0f, 248.0);
    winMailQuad(work, 0x5C, x + 32.0f, y - 32.0f, 344.0f, 240.0);
    winMailQuad(work, 0x55, x + 32.0f, y - 248.0f, 344.0f, 24.0);
    winMailQuad(work, 0x4F, x, y - 272.0f, 32.0f, 32.0);
    winMailQuad(work, 0x53, x + 32.0f, y - 272.0f, 344.0f, 32.0);
    winMailQuad(work, 0x50, 344.0f + (x + 32.0f), y - 272.0f, 40.0f, 32.0);
    winMailQuad(work, 0x54, x + 16.0f, (y - 24.0f) - 8.0f, 8.0f, -8.0);
    winMailQuad(work, 0x54, 8.0f + (384.0f + x), (y - 24.0f) - 8.0f, -8.0f, -8.0);
    winMailQuad(work, 0x54, x + 16.0f, y - 264.0f, 8.0f, 8.0);
    winMailQuad(work, 0x54, 8.0f + (384.0f + x), y - 264.0f, -8.0f, 8.0);

    GXSetScissor((u32)(32.0f + (304.0f + x)), (u32)(1.0f + (32.0f + (240.0f - y))), 0x158, 0xC8);
}

// Section 1 draws the scrolling page of the box. Retail reads the row counter
// before it is ever written (its `mulli r4, r25, 0x38` is at the top of the
// outer loop, and r25 is only assigned inside the inner loops), i.e. the
// original had an uninitialised `row`. That is undefined behaviour, so `row` is
// initialised to 0 here; the only observable difference is page 0's visibility
// test on the very first call, and the instruction count is unchanged (MWCC
// still keeps it in a register and emits the `mulli`).
// The four winIconSet calls take Vec/Vec/GXColor by value (winMailIconV), which gives the original 0x720 frame.
// Hako's sixteen native Vec templates, in retail parse order.
static const Vec vec3_802f41a4 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802f41b0 = { 1.0f, 1.0f, 1.0f };
static const Vec vec3_802f41bc = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802f41c8 = { 0.8f, 0.8f, 0.8f };
static const Vec vec3_802f41d4 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802f41e0 = { -1.0f, -1.0f, -1.0f };
static const Vec vec3_802f41ec = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802f41f8 = { 0.6f, 0.6f, 0.6f };
static const Vec vec3_802f4204 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802f4210 = { 1.0f, 1.0f, 1.0f };
static const Vec vec3_802f421c = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802f4228 = { 0.6f, 0.6f, 0.6f };
static const Vec vec3_802f4234 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802f4240 = { 0.7f, 0.7f, 0.7f };
static const Vec vec3_802f424c = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802f4258 = { 0.6f, 0.6f, 0.6f };

// NONMATCHING (91.30%): register allocation in the inlined quads and outer-loop lowering.
void winHakoGX(WindowWork* work, s32 type, f32 x, f32 y) {
    f32 scroll;
    s32 count;
    s32 cursor;
    s32 pageCount;
    s32 texBase;
    s32 page;
    s32 row = 0; // retail reads r25 (row) without setting it first at 0x80152E00, not only on page 0; 0 keeps it defined
    s32 col;
    s32 i;
    s32 ofs;
    s32 t7e, t7f, t84, t80, t87, t82, t81, t85, t86, t7d, t7b, t83;
    f32 f24;
    f32 f26;

    if (type == 0) {
        scroll = WW_F32(work, 0xEA0);
        count = WW_S32(work, 0xE94);
        cursor = WW_S32(work, 0xE9C);
    } else {
        scroll = WW_F32(work, 0xF9C);
        count = WW_S32(work, 0xF90);
        cursor = WW_S32(work, 0xF98);
    }

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, float_0_8041d068, float_0_8041d068, float_0_8041d068, float_0_8041d068, dat_8041cf6c);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
    GXSetCurrentMtx(0);
    GXSetScissor((u32)(float_304_8041d0a8 + x), (u32)(float_240_8041d0ac - y), 0x190, 0xE4);

    texBase = type * 0xD;
    pageCount = (count + 0x1B) / 28;
    t7d = texBase + 0x7D;
    t7e = texBase + 0x7E;
    t85 = texBase + 0x85;
    t84 = texBase + 0x84;
    t7b = texBase + 0x7B;
    t81 = texBase + 0x81;
    t86 = texBase + 0x86;
    t80 = texBase + 0x80;
    t7f = texBase + 0x7F;
    t83 = texBase + 0x83;
    t87 = texBase + 0x87;
    t82 = texBase + 0x82;
    f24 = float_392_8041d110 + x;
    f26 = (y - float_224_8041d10c) + scroll;
    ofs = 0;
    for (page = 0; page < pageCount; ofs += 0xE8, page++) {
        f32 vis = (scroll + (y - (f32)(row * 0x38))) - (f32)ofs;

        if (vis > float_400_8041d114 || vis < float_neg400_8041d118) {
            continue;
        }
        if ((texBase != 0 && page == 2) || (texBase != 1 && page == 3)) {
                hakoQuad(work, t7b, x, (y + scroll) - (f32)(page * 0xE8), 56.0f, 56.0f);
                hakoQuad(work, t7d, x, ((y - 56.0f) + scroll) - (f32)(page * 0xE8), 8.0f, 8.0f);
                hakoQuad(work, t7e, x + 8.0f, ((y - 56.0f) + scroll) - (f32)(page * 0xE8), float_48_8041d11c, 8.0f);
                hakoQuad(work, t85, x + 56.0f, (y + scroll) - (f32)(page * 0xE8), 8.0f, 8.0f);
                hakoQuad(work, t84, x + 56.0f, ((y - 8.0f) + scroll) - (f32)(page * 0xE8), 8.0f, 48.0f);
                hakoQuad(work, t81, x + 56.0f, ((y - 56.0f) + scroll) - (f32)(page * 0xE8), 8.0f, 8.0f);
                break; // retail leaves the page loop once the special page is drawn (b .L_80154348 at 0x8015386C)
            } else {
                f32 f23;
                f32 f22;

                for (col = 0; col < 7; col++) {
                    for (row = 0; row < 4; row++) {
                        hakoQuad(work, t7b + ((col + 7 * row) & 1),
                                 x + (f32)(col * 0x38),
                                 (scroll + (y - (f32)(row * 0x38))) - (f32)ofs,
                                 56.0f, 56.0f);
                    }
                }
                f23 = f26 - (f32)ofs;
                f22 = f23 - 8.0f;
                for (i = 0; i < 7; i++) {
                    s32 t1 = i == 0 ? t7d : ((i & 1) ? t7f : t86);
                    s32 t2 = (i & 1) ? t80 : t7e;
                    hakoQuad(work, t1, x + (f32)(i * 0x38), f23, 8.0f, 8.0f);
                    hakoQuad(work, t2, x + (f32)(i * 0x38) + 8.0f, f23, 48.0f, 8.0f);
                }
                for (i = 0; i < 4; i++) {
                    s32 t1 = i == 0 ? t85 : ((i & 1) ? t83 : t87);
                    f32 qy = (scroll + (y - (f32)(i * 0x38))) - (f32)ofs;
                    hakoQuad(work, t1, f24, qy, 8.0f, 8.0f);
                    {
                        s32 t2 = (i & 1) ? t82 : t84;
                        hakoQuad(work, t2, f24, (scroll + ((y - (f32)(i * 0x38)) - 8.0f)) - (f32)ofs, 8.0f, 48.0f);
                    }
                }
                hakoQuad(work, t81, f24, f23, 8.0f, 8.0f);
            }
    }

    // Section 2: the empty-slot tiles. Drawn when the item is not yet obtained.
    winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
    for (page = 0; page < pageCount; page++) {
        f32 vis2 = (scroll + (y - (f32)(row * 0x38))) - (f32)(page * 0xE8);
        if (vis2 > float_400_8041d114 || vis2 < float_neg400_8041d118) {
            continue;
        }
        for (row = 0; row < 4; row++) {
            f32 ty = ((scroll + (y - (f32)(row * 0x38))) - (f32)(page * 0xE8)) - float_28_8041d120;
            for (col = 0; col < 7; col++) {
                s32 idx = page * 0x1C + row * 7 + col;
                if (idx < count) {
                    Vec translate;

                    if (type == 0) {
                        s16 itemId = *(s16*)((u8*)work + 0xEB8 + idx * 2);
                        if (swGet(itemId + 0x80) != 0) {
                            continue;
                        }
                    } else {
                        s16 itemId = *(s16*)((u8*)work + 0xFB4 + idx * 2);
                        if (swGet(itemId + 0x41) != 0) {
                            continue;
                        }
                    }
                    translate = vec3_802f41a4;
                    translate.x = ((x + (f32)(col * 0x38)) + float_24_8041d0c8) + float_4_8041d07c;
                    translate.y = ty;
                    winTexSet(0x97, translate, vec3_802f41b0, dat_8041cf70);
                }
            }
        }
    }

    // Section 3: the item icons, drawn when the item is obtained.
    winIconInit();
    for (page = 0; page < pageCount; page++) {
        for (row = 0; row < 4; row++) {
            f32 ty = ((scroll + (y - (f32)(row * 0x38))) - (f32)(page * 0xE8)) - float_28_8041d120;
            for (col = 0; col < 7; col++) {
                s32 idx = page * 0x1C + row * 7 + col;
                if (idx < count) {
                    s32 iconId;
                    Vec translate;
                    Vec scale;
                    GXColor color;

                    if (type == 0) {
                        if (swGet(*(s16*)((u8*)work + 0xEB8 + idx * 2) + 0x80) == 0) {
                            continue;
                        }
                        { ItemData* it = &itemDataTable[*(s16*)((u8*)work + 0xEB8 + idx * 2)];
                        iconId = *(u16*)&it[0xF0].icon_id; }
                    } else {
                        if (swGet(*(s16*)((u8*)work + 0xFB4 + idx * 2) + 0x41) == 0) {
                            continue;
                        }
                        { ItemData* it = &itemDataTable[*(s16*)((u8*)work + 0xFB4 + idx * 2)];
                        iconId = *(u16*)&it[0xB3].icon_id; }
                    }
                    translate = vec3_802f41bc;
                    scale = vec3_802f41c8;
                    color = dat_8041cf74;
                    translate.x = ((x + (f32)(col * 0x38)) + float_24_8041d0c8) + float_4_8041d07c;
                    translate.y = ty;
                    winMailIconV(iconId, translate, scale, color);
                }
            }
        }
    }

    // Bottom status bar.
    GXSetScissor(0, 0, 0x260, 0x1E0);
    if (cursor > 0) {
        Vec translate;
        Vec scale;
        GXColor color;
        Vec itranslate;
        Vec iscale;
        GXColor icolor;
        f32 bx;

        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        bx = float_395_8041d124 + x;
        translate = vec3_802f41d4;
        scale = vec3_802f41e0;
        color = dat_8041cf78;
        translate.x = bx;
        translate.y = float_15_8041d128 + y;
        winTexSet(0x17, translate, scale, color);
        winIconInit();
        itranslate = vec3_802f41ec;
        iscale = vec3_802f41f8;
        icolor = dat_8041cf7c;
        itranslate.x = bx;
        itranslate.y = y;
        winMailIconV(0x7E, itranslate, iscale, icolor);
    }
    if (count > cursor * 0x1C + 0x1C) {
        Vec translate;
        Vec scale;
        GXColor color;
        Vec itranslate;
        Vec iscale;
        GXColor icolor;
        f32 bx;

        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        bx = float_395_8041d124 + x;
        translate = vec3_802f4204;
        scale = vec3_802f4210;
        color = dat_8041cf80;
        translate.x = bx;
        translate.y = y - float_235_8041d12c;
        winTexSet(0x17, translate, scale, color);
        winIconInit();
        itranslate = vec3_802f421c;
        iscale = vec3_802f4228;
        icolor = dat_8041cf84;
        itranslate.x = bx;
        itranslate.y = y - float_220_8041d130;
        winMailIconV(0x80, itranslate, iscale, icolor);
    }
    {
        const char* msg;
        f32 w0;
        f32 fx;
        Vec pos;
        Vec scale;
        GXColor color;
        Vec ipos;
        Vec iscale;
        GXColor icolor;

        msg = msgSearch("msg_menu_sort_narabikae");
        w0 = float_0p7_8041d134 * (f32)FontGetMessageWidth(msg);
        winFontInit();
        fx = float_380_8041d138 - w0;
        pos = vec3_802f4234;
        scale = vec3_802f4240;
        color = dat_8041cf88;
        pos.x = x + fx;
        pos.y = y - float_228_8041d13c;
        winFontSet(&pos, &scale, &color, msg);
        winIconInit();
        ipos = vec3_802f424c;
        iscale = vec3_802f4258;
        icolor = dat_8041cf8c;
        ipos.x = (x + fx) - float_30_8041d100;
        ipos.y = y - float_237_8041d140;
        winMailIconV(0x217, ipos, iscale, icolor);
    }
}

// The waza window frame: eleven textured quads (a 3x3 border plus inner strips),
// each an inlined wazaQuad. They come in three rows: the top bar at y (height
// 32), the body at y-32 (height h-48), and the bottom bar at y-h+16 (height 8/16).
// Pool-base offsets now reproduce retail; this function matches at 100%.
void winWazaGX(WindowWork* work, s32 type, f32 x, f32 y, f32 w, f32 h) {
    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041cf90);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
    GXSetCurrentMtx(0);

    {
    f32 w75 = 0.75f * w;
    wazaQuad(work, 0x98, x, y, w75, 32.0f);
    wazaQuad(work, 0x9A, x + w75, y, (w - w75) - 16.0f, 32.0f);
    wazaQuad(work, 0x9B, x + (w - 16.0f), y, 16.0f, 32.0f);
    wazaQuad(work, 0xA2, x, y - 32.0f, w75, (h - 32.0f) - 16.0f);
    wazaQuad(work, 0x9E, x + w75, y - 32.0f, (w - w75) - 8.0f, (h - 32.0f) - 16.0f);
    wazaQuad(work, 0xA1, x + (w - 8.0f), y - 32.0f, 8.0f, (h - 32.0f) - 16.0f);
    wazaQuad(work, 0x9E, x + w75, y - (h - 16.0f), (w - w75) - 8.0f, 8.0f);
    wazaQuad(work, 0xA1, x + (w - 8.0f), y - (h - 16.0f), 8.0f, 8.0f);
    wazaQuad(work, 0x9C, x, y - (h - 16.0f), w75, 16.0f);
    wazaQuad(work, 0x9F, x + w75, y - (h - 8.0f), (w - w75) - 16.0f, 8.0f);
    wazaQuad(work, 0x9D, x + (w - 16.0f), y - (h - 16.0f), 16.0f, 16.0f);
    }
}

void sort_1_1_func(WindowWork* work) {
    s32 item = WW_S32(work, 0x214);
    pouchGetHaveItemCnt();
    pouchHaveItem(item);
    {
        s32 type = 7;
        if (WW_S32(work, 0x180) == 0) {
            type = 0;
        }
        pouchSortItem(type);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeSkipList(work);
    WW_S32(work, 0x214) = 0;
    WW_S32(work, 0x21C) = WW_S32(work, 0x214) / 10;
}

void sort_1_2_func(WindowWork* work) {
    s32 item = WW_S32(work, 0x214);
    pouchGetHaveItemCnt();
    pouchHaveItem(item);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 1 : 8);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeSkipList(work);
    WW_S32(work, 0x214) = 0;
    WW_S32(work, 0x21C) = WW_S32(work, 0x214) / 10;
}

void sort_2_1_func(WindowWork* work) {
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 2 : 9);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeSkipList(work);
    WW_S32(work, 0x218) = 0;
    WW_S32(work, 0x220) = WW_S32(work, 0x218) / 10;
}

void sort_2_2_func(WindowWork* work) {
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 3 : 10);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeSkipList(work);
    WW_S32(work, 0x218) = 0;
    WW_S32(work, 0x220) = WW_S32(work, 0x218) / 10;
}

void sort_3_1_func(WindowWork* work) {
    s32 badge = WW_S32(work, 0x3E4);
    pouchGetHaveBadgeCnt();
    pouchHaveBadge(badge);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 4 : 11);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeEquipList(work);
    WW_S32(work, 0x3E4) = 0;
    WW_S32(work, 0x3EC) = WW_S32(work, 0x3E4) / 8;
}

void sort_3_2_func(WindowWork* work) {
    s32 badge = WW_S32(work, 0x3E4);
    pouchGetHaveBadgeCnt();
    pouchHaveBadge(badge);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 5 : 12);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeEquipList(work);
    WW_S32(work, 0x3E4) = 0;
    WW_S32(work, 0x3EC) = WW_S32(work, 0x3E4) / 8;
}

void sort_3_3_func(WindowWork* work) {
    s32 badge = WW_S32(work, 0x3E4);
    pouchGetHaveBadgeCnt();
    pouchHaveBadge(badge);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 6 : 13);
    pouchSortItem(WW_S32(work, 0x180) == 0 ? 6 : 13);
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    winMakeEquipList(work);
    WW_S32(work, 0x3E4) = 0;
    WW_S32(work, 0x3EC) = WW_S32(work, 0x3E4) / 8;
}

// 1:1 (the `const` parameters reproduce the original's hoisted `lwz r4,0x8(r4)` prologue load).
int N_compare_func4_1(const void* a, const void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_b = (s16)*(s32*)((u8*)b + 8);
    s16 id_a = (s16)*(s32*)((u8*)a + 8);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_a, (char*)buf_b);
}

// 1:1 (same `const`-parameter prologue hoist as N_compare_func4_1).
int N_compare_func4_1_r(const void* a, const void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_b = (s16)*(s32*)((u8*)b + 8);
    s16 id_a = (s16)*(s32*)((u8*)a + 8);

    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_b, (char*)buf_a);
}

int N_compare_func4_2(void* a, void* b) {
    s32 id_a = (s16)*(s32*)((u8*)a + 8);
    s32 id_b = (s16)*(s32*)((u8*)b + 8);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va > vb) {
        return 1;
    } else if (va < vb) {
        return -1;
    } else {
        return 0;
    }
}

int N_compare_func4_2_r(void* a, void* b) {
    s32 id_a = (s16)*(s32*)((u8*)a + 8);
    s32 id_b = (s16)*(s32*)((u8*)b + 8);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va < vb) {
        return 1;
    } else if (va > vb) {
        return -1;
    } else {
        return 0;
    }
}

void sort_4_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, (int (*)(void*, void*))N_compare_func4_1);
    } else {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, (int (*)(void*, void*))N_compare_func4_1_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x3E8) = 0;
    WW_S32(work, 0x3F0) = WW_S32(work, 0x3E8) / 8;
}

void sort_4_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, N_compare_func4_2);
    } else {
        qqsort((u8*)work + 0x404, WW_S32(work, 0xD64), 0xC, N_compare_func4_2_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x3E8) = 0;
    WW_S32(work, 0x3F0) = WW_S32(work, 0x3E8) / 8;
}

// Bubble sort of the 0xC-byte records at work+0x404 (the item id is the s16 at
// +0x40C). Writing the two record addresses as indices (`work + i*0xC`) rather than
// raw `pa`/`pb` pointers is what makes MWCC strength-reduce them to the target's
// pa=r4/pb=r12 induction variables and colour the item-table base r11; the pointer
// form leaves those in r9/r10/r11. The `SortRec` (3-word) copy `t = *pa; *pa = *pb;
// *pb = t;` reproduces the original's interleaved load/store sequence and its stack
// spills at 0x14/0x18/0x1c exactly.
void sort_4_3_func(WindowWork* work) {
    u8* base = (u8*)work;
    if (WW_S32(work, 0x180) == 0) {
        s32 ida;
        s32 idb;
        s32 i;
        s32 j;
        s32 swap;

        for (i = 0; i < WW_S32(work, 0xD64) - 1; i++) {
            j = i + 1;
            while (j < WW_S32(work, 0xD64)) {
                ida = (s16)*(u32*)(base + i * 0xC + 0x40C);
                idb = (s16)*(u32*)(base + j * 0xC + 0x40C);
                swap = 0;

                if (itemDataTable[ida].bp_cost > itemDataTable[idb].bp_cost) {
                    swap = 1;
                } else if (itemDataTable[ida].bp_cost == itemDataTable[idb].bp_cost && itemDataTable[ida].type_sort_order > itemDataTable[idb].type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    SortRec t = *(SortRec*)(base + i * 0xC + 0x404);
                    *(SortRec*)(base + i * 0xC + 0x404) = *(SortRec*)(base + j * 0xC + 0x404);
                    *(SortRec*)(base + j * 0xC + 0x404) = t;
                }
                j++;
            }
        }
    } else {
        s32 ida;
        s32 idb;
        s32 i;
        s32 j;
        s32 swap;

        for (i = 0; i < WW_S32(work, 0xD64) - 1; i++) {
            j = i + 1;
            while (j < WW_S32(work, 0xD64)) {
                ida = (s16)*(u32*)(base + i * 0xC + 0x40C);
                idb = (s16)*(u32*)(base + j * 0xC + 0x40C);
                swap = 0;

                if (itemDataTable[ida].bp_cost < itemDataTable[idb].bp_cost) {
                    swap = 1;
                } else if (itemDataTable[ida].bp_cost == itemDataTable[idb].bp_cost && itemDataTable[ida].type_sort_order < itemDataTable[idb].type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    SortRec t = *(SortRec*)(base + i * 0xC + 0x404);
                    *(SortRec*)(base + i * 0xC + 0x404) = *(SortRec*)(base + j * 0xC + 0x404);
                    *(SortRec*)(base + j * 0xC + 0x404) = t;
                }
                j++;
            }
        }
    }

    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x3E8) = 0;
    WW_S32(work, 0x3F0) = WW_S32(work, 0x3E8) / 8;
}

// 1:1 (the `const` parameters reproduce the target's hoisted `lha` prologue load).
int compare_func5_1(const void* a, const void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xF0);
    s16 id_b = (s16)(*(s16*)b + 0xF0);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_a, (char*)buf_b);
}

// 1:1 (same `const`-parameter prologue hoist as compare_func5_1).
int compare_func5_1_r(const void* a, const void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xF0);
    s16 id_b = (s16)(*(s16*)b + 0xF0);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_b, (char*)buf_a);
}

int compare_func5_2(void* a, void* b) {
    s16 id_a = (s16)(*(s16*)a + 0xF0);
    s16 id_b = (s16)(*(s16*)b + 0xF0);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va > vb) {
        return 1;
    } else if (va < vb) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func5_2_r(void* a, void* b) {
    s16 id_a = (s16)(*(s16*)a + 0xF0);
    s16 id_b = (s16)(*(s16*)b + 0xF0);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va < vb) {
        return 1;
    } else if (va > vb) {
        return -1;
    } else {
        return 0;
    }
}

void sort_5_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, (int (*)(void*, void*))compare_func5_1);
    } else {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, (int (*)(void*, void*))compare_func5_1_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xE98) = 0;
    WW_S32(work, 0xE9C) = WW_S32(work, 0xE98) / 28;
}

void sort_5_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, compare_func5_2);
    } else {
        qqsort((u8*)work + 0xEB8, WW_S32(work, 0xE94), 2, compare_func5_2_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xE98) = 0;
    WW_S32(work, 0xE9C) = WW_S32(work, 0xE98) / 28;
}

// Same bubble sort as sort_4_3 but over the s16 array at work+0xEB8 (index +0xF0).
// Writing the two record addresses as indices (`work + i*2`) rather than the raw
// `pa`/`pb` pointers lets MWCC strength-reduce them to the same pa=r7/pb=r6
// induction variables the target uses. The per-branch declaration order of the
// locals reproduces the target's register colours (j=r31/swap=r30 in the ascending
// branch, swap=r30/j=r31 in the descending one); the orders differ between the two
// branches only for that reason.
void sort_5_3_func(WindowWork* work) {
    u8* base = (u8*)work;
    if (WW_S32(work, 0x180) == 0) {
        s16 la;
        s16 lb;
        s32 i;
        s32 j;
        s16 ia;
        s16 ib;
        s32 swap;

        for (i = 0; i < WW_S32(work, 0xE94) - 1; i++) {
            j = i + 1;
            while (j < WW_S32(work, 0xE94)) {
                la = *(s16*)(base + i * 2 + 0xEB8);
                lb = *(s16*)(base + j * 2 + 0xEB8);
                ia = (s16)((s32)la + 0xF0);
                ib = (s16)((s32)lb + 0xF0);
                swap = 0;

                if (itemDataTable[ia].bp_cost > itemDataTable[ib].bp_cost) {
                    swap = 1;
                } else if (itemDataTable[ia].bp_cost == itemDataTable[ib].bp_cost && itemDataTable[ia].type_sort_order > itemDataTable[ib].type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    *(s16*)(base + i * 2 + 0xEB8) = lb;
                    *(s16*)(base + j * 2 + 0xEB8) = la;
                }
                j++;
            }
        }
    } else {
        s16 la;
        s16 lb;
        s32 i;
        s32 swap;
        s32 j;
        s16 ia;
        s16 ib;

        for (i = 0; i < WW_S32(work, 0xE94) - 1; i++) {
            j = i + 1;
            while (j < WW_S32(work, 0xE94)) {
                la = *(s16*)(base + i * 2 + 0xEB8);
                lb = *(s16*)(base + j * 2 + 0xEB8);
                ia = (s16)((s32)la + 0xF0);
                ib = (s16)((s32)lb + 0xF0);
                swap = 0;

                if (itemDataTable[ia].bp_cost < itemDataTable[ib].bp_cost) {
                    swap = 1;
                } else if (itemDataTable[ia].bp_cost == itemDataTable[ib].bp_cost && itemDataTable[ia].type_sort_order < itemDataTable[ib].type_sort_order) {
                    swap = 1;
                }
                if (swap) {
                    *(s16*)(base + i * 2 + 0xEB8) = lb;
                    *(s16*)(base + j * 2 + 0xEB8) = la;
                }
                j++;
            }
        }
    }

    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xE98) = 0;
    WW_S32(work, 0xE9C) = WW_S32(work, 0xE98) / 28;
}

// 1:1 (the `const` parameters reproduce the target's hoisted `lha` prologue load).
int compare_func6_1(const void* a, const void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xB3);
    s16 id_b = (s16)(*(s16*)b + 0xB3);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_a, (char*)buf_b);
}

// 1:1 (same `const`-parameter prologue hoist as compare_func6_1).
int compare_func6_1_r(const void* a, const void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    s16 id_a = (s16)(*(s16*)a + 0xB3);
    s16 id_b = (s16)(*(s16*)b + 0xB3);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_a].name), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(itemDataTable[id_b].name), (char*)buf_b);
    return strcmp((char*)buf_b, (char*)buf_a);
}

int compare_func6_2(void* a, void* b) {
    s16 id_a = (s16)(*(s16*)a + 0xB3);
    s16 id_b = (s16)(*(s16*)b + 0xB3);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va > vb) {
        return 1;
    } else if (va < vb) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func6_2_r(void* a, void* b) {
    s16 id_a = (s16)(*(s16*)a + 0xB3);
    s16 id_b = (s16)(*(s16*)b + 0xB3);
    s16 va = itemDataTable[id_a].type_sort_order;
    s16 vb = itemDataTable[id_b].type_sort_order;

    if (va < vb) {
        return 1;
    } else if (va > vb) {
        return -1;
    } else {
        return 0;
    }
}

void sort_6_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0xFB4, WW_S32(work, 0xF90), 2, (int (*)(void*, void*))compare_func6_1);
    } else {
        qqsort((u8*)work + 0xFB4, WW_S32(work, 0xF90), 2, (int (*)(void*, void*))compare_func6_1_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xF94) = 0;
    WW_S32(work, 0xF98) = WW_S32(work, 0xF94) / 28;
}

void sort_6_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0xFB4, WW_S32(work, 0xF90), 2, compare_func6_2);
    } else {
        qqsort((u8*)work + 0xFB4, WW_S32(work, 0xF90), 2, compare_func6_2_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0xF94) = 0;
    WW_S32(work, 0xF98) = WW_S32(work, 0xF94) / 28;
}

int compare_func1(void* a, void* b) {
    if (*(u8*)a > *(u8*)b) {
        return 1;
    } else if (*(u8*)a < *(u8*)b) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func1_r(void* a, void* b) {
    if (*(u8*)a < *(u8*)b) {
        return 1;
    } else if (*(u8*)a > *(u8*)b) {
        return -1;
    } else {
        return 0;
    }
}

void sort_7_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x1050, WW_S32(work, 0x1038), 2, compare_func1);
    } else {
        qqsort((u8*)work + 0x1050, WW_S32(work, 0x1038), 2, compare_func1_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x1040) = 0;
    WW_S32(work, 0x1044) = WW_S32(work, 0x1040) / 16;
}

// Two `menu_monosiri_%s` lookups over the 26-entry enemy-code table.
int compare_func2(const void* a, const void* b) {
    u8 buf_a[64];
    u8 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(((u8*)a)[1]);
    TattleEntry* eb = battleGetUnitMonosiriPtr(((u8*)b)[1]);
    s32 i = winRootFindTag(tbl, ea->locationTag, buf_a);
    s32 j = winRootFindTag(tbl, eb->locationTag, buf_b);

    if (i > j) {
        return 1;
    } else if (i < j) {
        return -1;
    } else {
        return 0;
    }
}

// Two `menu_monosiri_%s` lookups over the 26-entry enemy-code table.
int compare_func2_r(const void* a, const void* b) {
    u8 buf_a[64];
    u8 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(((u8*)a)[1]);
    TattleEntry* eb = battleGetUnitMonosiriPtr(((u8*)b)[1]);
    s32 i = winRootFindTag(tbl, ea->locationTag, buf_a);
    s32 j = winRootFindTag(tbl, eb->locationTag, buf_b);

    if (i < j) {
        return 1;
    } else if (i > j) {
        return -1;
    } else {
        return 0;
    }
}

void sort_7_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, (int (*)(void*, void*))compare_func2);
    } else {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1_r);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, (int (*)(void*, void*))compare_func2_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x1040) = 0;
    WW_S32(work, 0x1044) = WW_S32(work, 0x1040) / 16;
}

int compare_func3(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(*(u8*)((u8*)a + 1));
    TattleEntry* eb = battleGetUnitMonosiriPtr(*(u8*)((u8*)b + 1));

    unk_800d06b4((char*)msgSearch(ea->enemyTag), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(eb->enemyTag), (char*)buf_b);

    if (*(u8*)((u8*)a + 1) == 0x4D || *(u8*)((u8*)a + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch("btl_un_hatena"), (char*)buf_a);
        } else {
            unk_800d06b4((char*)msgSearch("btl_un_rampell"), (char*)buf_a);
        }
    }
    if (*(u8*)((u8*)b + 1) == 0x4D || *(u8*)((u8*)b + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch("btl_un_hatena"), (char*)buf_b);
        } else {
            unk_800d06b4((char*)msgSearch("btl_un_rampell"), (char*)buf_b);
        }
    }

    return strcmp((char*)buf_a, (char*)buf_b);
}

int compare_func3_r(void* a, void* b) {
    u16 buf_a[64];
    u16 buf_b[64];
    TattleEntry* ea = battleGetUnitMonosiriPtr(*(u8*)((u8*)a + 1));
    TattleEntry* eb = battleGetUnitMonosiriPtr(*(u8*)((u8*)b + 1));

    unk_800d06b4((char*)msgSearch(ea->enemyTag), (char*)buf_a);
    unk_800d06b4((char*)msgSearch(eb->enemyTag), (char*)buf_b);

    if (*(u8*)((u8*)a + 1) == 0x4D || *(u8*)((u8*)a + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch("btl_un_hatena"), (char*)buf_a);
        } else {
            unk_800d06b4((char*)msgSearch("btl_un_rampell"), (char*)buf_a);
        }
    }
    if (*(u8*)((u8*)b + 1) == 0x4D || *(u8*)((u8*)b + 1) == 0x87) {
        if (evtGetValue(0, 0xF5DE0180) < 0xD2) {
            unk_800d06b4((char*)msgSearch("btl_un_hatena"), (char*)buf_b);
        } else {
            unk_800d06b4((char*)msgSearch("btl_un_rampell"), (char*)buf_b);
        }
    }

    return strcmp((char*)buf_b, (char*)buf_a);
}

void sort_7_3_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func3);
    } else {
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func1_r);
        qqsort(&work->field_40[0x1010], WW_S32(work, 0x1038), 2, compare_func3_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x1040) = 0;
    WW_S32(work, 0x1044) = WW_S32(work, 0x1040) / 16;
}

// 1:1 (the `const` parameters reproduce the target's hoisted `lbz r3,0x1(r3)` prologue load).
int compare_func4(const void* a, const void* b) {
    void* ma = mailGetPtr(*(u8*)((u8*)a + 1));
    void* mb = mailGetPtr(*(u8*)((u8*)b + 1));
    const char* sa = (const char*)msgSearch(*(const char**)((u8*)ma + 8));
    const char* sb = (const char*)msgSearch(*(const char**)((u8*)mb + 8));

    return strcmp(sa, sb);
}

// 1:1 (same `const`-parameter prologue hoist as compare_func4).
int compare_func4_r(const void* a, const void* b) {
    void* ma = mailGetPtr(*(u8*)((u8*)a + 1));
    void* mb = mailGetPtr(*(u8*)((u8*)b + 1));
    const char* sa = (const char*)msgSearch(*(const char**)((u8*)ma + 8));
    const char* sb = (const char*)msgSearch(*(const char**)((u8*)mb + 8));

    return strcmp(sb, sa);
}

void sort_8_1_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, (int (*)(void*, void*))compare_func4);
    } else {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, (int (*)(void*, void*))compare_func4_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x258) = 0;
    WW_S32(work, 0x25C) = WW_S32(work, 0x258) / 5;
}

int compare_func5(void* a, void* b) {
    if (*(u8*)a > *(u8*)b) {
        return 1;
    } else if (*(u8*)a < *(u8*)b) {
        return -1;
    } else {
        return 0;
    }
}

int compare_func5_r(void* a, void* b) {
    if (*(u8*)a < *(u8*)b) {
        return 1;
    } else if (*(u8*)a > *(u8*)b) {
        return -1;
    } else {
        return 0;
    }
}

void sort_8_2_func(WindowWork* work) {
    if (WW_S32(work, 0x180) == 0) {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, compare_func5);
    } else {
        qqsort((u8*)work + 0x26E, WW_S32(work, 0x254), 2, compare_func5_r);
    }
    WW_S32(work, 0x180) = 1 - WW_S32(work, 0x180);
    WW_S32(work, 0x258) = 0;
    WW_S32(work, 0x25C) = WW_S32(work, 0x258) / 5;
}

// The two trailing lerps use separate deltas so MWCC keeps the first fmadd's
// difference/accumulator in the original's FPRs.
void winSortMain(WindowWork* work) {
    u32 trg = work->buttonTrg;
    u32 rep = work->dirRep;

    if (WW_S32(work, 0x17C) == 0) {
        trg = 0;
        rep = 0;
    }

    if (trg & 0x200) {
        psndSFXOn((const char*)0x20013);
        WW_S32(work, 0x17C) = 0;
        WW_F32(work, 0x16C) = float_neg600_8041d0fc;
    } else if (rep & 0x1000) {
        psndSFXOn((const char*)0x20005);
        WW_S32(work, 0x174) = WW_S32(work, 0x174) - 1;
        if (WW_S32(work, 0x174) < 0) {
            WW_S32(work, 0x174) = WW_S32(work, 0x178) - 1;
        }
    } else if (rep & 0x2000) {
        psndSFXOn((const char*)0x20005);
        WW_S32(work, 0x174) = WW_S32(work, 0x174) + 1;
        if (WW_S32(work, 0x174) >= WW_S32(work, 0x178)) {
            WW_S32(work, 0x174) = 0;
        }
    } else if (trg & 0x100) {
        psndSFXOn((const char*)0x20012);
        sort_data[WW_S32(work, 0x184)][WW_S32(work, 0x174)].func(work);
    } else if (trg & 0x1000) {
        WW_S32(work, 0x17C) = 0;
        WW_F32(work, 0x16C) = float_neg600_8041d0fc;
    }

    if (WW_S32(work, 0x17C) != 0) {
        BOOL changed = FALSE;
        const char* msg;
        u16 width;

        WW_F32(work, 0x158) = float_30_8041d100 + WW_F32(work, 0x16C);
        WW_F32(work, 0x15C) = (WW_F32(work, 0x170) - float_40_8041d0e8) -
                              (f32)(WW_S32(work, 0x174) * 0x28);
        msg = sort_data[WW_S32(work, 0x184)][WW_S32(work, 0x174)].name;
        if (msg != NULL) {
            if (WW_PTR(work, 0x144) != (void*)msg) {
                changed = TRUE;
            } else if (strcmp(msgbuf, msg) != 0) {
                changed = TRUE;
            }
            if (changed) {
                WW_S32(work, 0x138) = 0;
                WW_PTR(work, 0x13C) = (void*)msg;
                WW_S32(work, 0x140) = 0;
                FontGetMessageWidthLine((const char*)msgSearch((const char*)WW_PTR(work, 0x13C)), &width);
                WW_S32(work, 0x148) = 0;
                WW_S32(work, 0x14C) = width + 1;
                WW_PTR(work, 0x144) = (void*)msg;
                strncpy(msgbuf, msg, 0x3C);
            }
        }
    }

    {
#ifdef MUST_MATCH
        // Two deltas (the natural form shares one): a single temp makes MWCC swap
        // the first fmadd's difference and accumulator registers.
        f32 d1;
        f32 d2;
        d1 = WW_F32(work, 0x16C) - WW_F32(work, 0x164);
        WW_F32(work, 0x164) += d1 * float_0p125_8041d104;
        d2 = WW_F32(work, 0x170) - WW_F32(work, 0x168);
        WW_F32(work, 0x168) += d2 * float_0p125_8041d104;
#else
        WW_F32(work, 0x164) += float_0p125_8041d104 * (WW_F32(work, 0x16C) - WW_F32(work, 0x164));
        WW_F32(work, 0x168) += float_0p125_8041d104 * (WW_F32(work, 0x170) - WW_F32(work, 0x168));
#endif
    }
}

// The panels, sort-name tex selection and winTexSet/winTexSet_x2 loop all reproduce the
// target: `h` is passed as f64 so MWCC emits the target's `lfd double_24/40/16; fabs; frsp`
// scale idiom, and `w` as f32 (lfs + fabs + frsp). The six vec3_802f42xx constants are
// defined as unit-local `const Vec` (all {0,0,0}/{1,1,1}, extracted from the target's
// .rodata), which lets MWCC emit the target's rodata-base-relative loads.
//
// The panel-8 (0xA8) `w` is the named float_136_8041d0f0 rather than the literal 136.0f:
// the target loads it with lfs+fabs and computes the int->float height with fsubs (asm
// 0x8014F600/0x8014F664), while the literal/f64 form folded to lfd.
//
// The winTex loop's three x offsets are hoisted before the loop as literals
// (`x + 64.0f + 3.0f + 16.0f`, `x + 64.0f + 16.0f + 4.0f`, and the latter `- 6.0f`): only
// when the whole computation is written out does MWCC share the target's `x+64`/`+16`
// partial sums (asm 0x8014F724-0x8014F768). The literals also keep the target's pooled
// .sdata2 words (64/3/16/4/6) instead of replacing them with a single folded 83/78/84
// constant, which is what makes the unit's .sdata2 score rise rather than fall.
// Descriptor labels pool before the sort draw templates, in retail order.
static SortTable sort_1[] = {
    {"msg_menu_sort_aiueo", sort_1_1_func},
    {"msg_menu_sort_kind", sort_1_2_func},
    {NULL, NULL},
};
static SortTable sort_2[] = {
    {"msg_menu_sort_aiueo", sort_2_1_func},
    {"msg_menu_sort_kind", sort_2_2_func},
    {NULL, NULL},
};
static SortTable sort_3[] = {
    {"msg_menu_sort_aiueo", sort_3_1_func},
    {"msg_menu_sort_kind", sort_3_2_func},
    {"msg_menu_sort_bp", sort_3_3_func},
    {NULL, NULL},
};
static SortTable sort_4[] = {
    {"msg_menu_sort_aiueo", sort_4_1_func},
    {"msg_menu_sort_kind", sort_4_2_func},
    {"msg_menu_sort_bp", sort_4_3_func},
    {NULL, NULL},
};
static SortTable sort_5[] = {
    {"msg_menu_sort_aiueo", sort_5_1_func},
    {"msg_menu_sort_kind", sort_5_2_func},
    {"msg_menu_sort_bp", sort_5_3_func},
    {NULL, NULL},
};
static SortTable sort_6[] = {
    {"msg_menu_sort_aiueo", sort_6_1_func},
    {"msg_menu_sort_kind", sort_6_2_func},
    {NULL, NULL},
};
static SortTable sort_7[] = {
    {"msg_menu_sort_number", sort_7_1_func},
    {"msg_menu_sort_place", sort_7_2_func},
    {"msg_menu_sort_aiueo", sort_7_3_func},
    {NULL, NULL},
};
static SortTable sort_8[] = {
    {"msg_menu_sort_aiueo", sort_8_1_func},
    {"msg_menu_sort_number", sort_8_2_func},
    {NULL, NULL},
};

SortTable* sort_data[8] = {sort_1, sort_2, sort_3, sort_4, sort_5, sort_6, sort_7, sort_8};

static const Vec vec3_802f42cc = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802f42d8 = { 1.0f, 1.0f, 1.0f };
static const Vec vec3_802f42e4 = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802f42f0 = { 1.0f, 1.0f, 1.0f };
static const Vec vec3_802f42fc = { 0.0f, 0.0f, 0.0f };
static const Vec vec3_802f4308 = { 1.0f, 1.0f, 1.0f };

// NONMATCHING (98.67%): FPR colouring and extra arithmetic in the panels.
void winSortGX(WindowWork* work) {
    f32 x = WW_F32(work, 0x164);
    f32 y = WW_F32(work, 0x168);
    s32 i;

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, float_0_8041d068, float_0_8041d068, float_0_8041d068, float_0_8041d068, dat_8041d004);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXLoadPosMtxImm((const f32(*)[4])((u8*)camGetPtr(8) + 0x11C), 0);
    GXSetCurrentMtx(0);
    if (WW_S32(work, 0x184) == 7) {
        GXSetScissor((u32)(16.0f +
                           (16.0f + (float_304_8041d0a8 + WW_F32(work, 0x244)))),
                     (u32)(float_40_8041d0e8 + (float_240_8041d0ac - WW_F32(work, 0x24C))), 0x200,
                     0xC0);
    }

    winSortPanel(work, 0xA5, x, y, float_128_8041d0ec, 24.0);
    winSortPanel(work, 0xA3, x + float_128_8041d0ec, y, 24.0f, 24.0);

    for (i = 0; i < WW_S32(work, 0x178); i++) {
        winSortPanel(work, 0xA6, x + 136.0f, y - 24.0f - (f32)(i * 0x28), 16.0f,
                     40.0);
    }

    winSortPanel(work, 0xA4, x + float_128_8041d0ec,
                 y - 24.0f - (f32)(WW_S32(work, 0x178) * 0x28), 24.0f,
                 24.0);
    winSortPanel(work, 0xA7, x, y - 24.0f - (f32)(WW_S32(work, 0x178) * 0x28),
                 float_128_8041d0ec, 24.0);
    winSortPanel(work, 0xA8, x, y - 24.0f, float_136_8041d0f0,
                 (f32)(WW_S32(work, 0x178) * 0x28));

    {
        f32 x83 = x + 64.0f + 3.0f + 16.0f;
        f32 x84 = x + 64.0f + 16.0f + 4.0f;
        f32 x78 = x + 64.0f + 16.0f + 4.0f - 6.0f;

    for (i = 0; i < WW_S32(work, 0x178); i++) {
        const SortTable* list = sort_data[WW_S32(work, 0x184)];
        s32 tex = 0xAA;

        if (strcmp(list[i].name, "msg_menu_sort_aiueo") == 0) {
            tex = 0xA9;
        } else if (strcmp(list[i].name, "msg_menu_sort_kind") == 0) {
            tex = 0xAC;
        } else if (strcmp(list[i].name, "msg_menu_sort_bp") == 0) {
            tex = 0xAA;
        } else if (strcmp(list[i].name, "msg_menu_sort_number") == 0) {
            tex = 0xAD;
        } else if (strcmp(list[i].name, "msg_menu_sort_place") == 0) {
            tex = 0xAE;
        } else {
            tex = 0xAA;
        }

        if (i == WW_S32(work, 0x174)) {
            f32 row;

            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            row = y - float_20_8041d0c0 - (f32)(i * 0x28) - 16.0f - float_8_8041d0a0;
            winTexSet(0xB6, winSortMkVec(vec3_802f42cc, x83, row), vec3_802f42d8, dat_8041d008);
            winTexInit_x2(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            winTexSet_x2(tex, 0xB6, winSortMkVec(vec3_802f42e4, x78, float_6_8041d0f8 + row),
                         vec3_802f42f0, dat_8041d00c);
        } else {
            f32 row;

            winTexInit_x2(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            row = y - float_20_8041d0c0 - (f32)(i * 0x28) - 16.0f - float_8_8041d0a0;
            winTexSet_x2(tex, 0xB6, winSortMkVec(vec3_802f42fc, x84, row), vec3_802f4308,
                         dat_8041d010);
        }
    }
    }

    if (WW_S32(work, 0x184) == 7) {
        GXSetScissor(0, 0, 0x260, 0x1E0);
    }
}

void winSortEntry(WindowWork* work, s32 idx, f32 a, f32 b) {
    SortTable* p = sort_data[idx];
    s32 count = 0;

    while (p->name != NULL) {
        p++;
        count++;
    }

    WW_S32(work, 0x184) = idx;
    WW_S32(work, 0x178) = count;
    WW_S32(work, 0x174) = 0;
    WW_S32(work, 0x180) = 0;
    WW_F32(work, 0x164) = float_neg1000_8041d0cc;
    WW_F32(work, 0x16C) = a;
    WW_F32(work, 0x168) = b;
    WW_F32(work, 0x170) = b;
    WW_S32(work, 0x17C) = 1;
}

s32 winSortWait(WindowWork* work) {
    return WW_S32(work, 0x17C) != 0;
}

// Kage (paper) window quad. The vertex constants are literals so MWCC reloads each
// through f0 into the FIFO as the original does; the same-literal GXSetFog zero lets
// MWCC land it in f1 after saving x. The int->f32 magic double still pools anonymously.
void winKageGX(WindowWork* work, void* color, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj texObj;
    Mtx mtx4;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    f32 scaleY;

    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041d014);
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZMode(0, 7, 0);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 2, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 1, 4, 7);
    GXSetTevSwapMode(0, 0, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    TEXGetGXTexObjFromPalette(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), &texObj, 0x7A);
    GXInitTexObjLOD(&texObj, 1, 1, float_0_8041d068, float_0_8041d068, float_0_8041d068, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetTevColor(1, *(GXColor*)color);
    PSMTXTrans(mtx1, x, y, w);
    scaleY = h * (float_2_8041d090 * (f32)GXGetTexObjHeight(&texObj));
    PSMTXScale(mtx2, h * (float_2_8041d090 * (f32)GXGetTexObjWidth(&texObj)), scaleY, float_1_8041d070);
    PSMTXRotRad(mtx3, 0x78, float_neg1p3963_8041d0dc);
    PSMTXConcat(mtx1, mtx3, mtx4);
    PSMTXConcat(mtx4, mtx2, mtx4);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx4, mtx4);
    GXLoadPosMtxImm(mtx4, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(-0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

void winZClear(void) {
    Mtx mtx;

    PSMTXTrans(mtx, float_0_8041d068, float_0_8041d068, float_neg1000_8041d0cc);
    PSMTXConcat(camGetCurPtr()->view, mtx, mtx);
    GXLoadPosMtxImm(mtx, 0);
    GXSetCurrentMtx(0);
    GXSetAlphaUpdate(0);
    GXSetColorUpdate(0);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0xFF, 0xFF, 4);
    GXSetTevOp(0, 4);
    GXSetNumTexGens(0);
    GXSetZMode(1, 7, 1);
    GXSetBlendMode(0, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxDesc(9, 1);
    GXBegin(0x80, 0, 4);
#ifdef MUST_MATCH
    // volatile reads stop MWCC from CSE-ing the position constants across the
    // four vertices; the original reloads each one into f0 before its store.
    GXWGFifo.f32 = *(volatile f32*)&float_neg500_8041d0d0;
    GXWGFifo.f32 = *(volatile f32*)&float_300_8041d0d4;
    GXWGFifo.f32 = *(volatile f32*)&float_0_8041d068;
    GXWGFifo.f32 = *(volatile f32*)&float_0_8041d068;
    GXWGFifo.f32 = *(volatile f32*)&float_300_8041d0d4;
    GXWGFifo.f32 = *(volatile f32*)&float_0_8041d068;
    GXWGFifo.f32 = *(volatile f32*)&float_0_8041d068;
    GXWGFifo.f32 = *(volatile f32*)&float_neg100_8041d0d8;
    GXWGFifo.f32 = *(volatile f32*)&float_0_8041d068;
    GXWGFifo.f32 = *(volatile f32*)&float_neg500_8041d0d0;
    GXWGFifo.f32 = *(volatile f32*)&float_neg100_8041d0d8;
    GXWGFifo.f32 = *(volatile f32*)&float_0_8041d068;
#else
    GXPosition3f32(float_neg500_8041d0d0, float_300_8041d0d4, float_0_8041d068);
    GXPosition3f32(float_0_8041d068, float_300_8041d0d4, float_0_8041d068);
    GXPosition3f32(float_0_8041d068, float_neg100_8041d0d8, float_0_8041d068);
    GXPosition3f32(float_neg500_8041d0d0, float_neg100_8041d0d8, float_0_8041d068);
#endif
    GXEnd();
    GXSetColorUpdate(1);
}

// NONMATCHING (99.25%): read/unread icon selection uses a branch-free select.
// The mail window: the list of letters (an envelope in the mail's colour, a read/unread mark, the two-digit
// number and the title per row), or with one open (state 0xCA) its header, the scrolling body with its
// picture, and the scroll arrows.
void winMailDisp(WindowWork* work) {
    s32 i;

    if (WW_F32(work, 0x244) < float_320_8041d074 && WW_F32(work, 0x24C) > float_neg240_8041d078) {
    winMailGX(work, WW_F32(work, 0x244), WW_F32(work, 0x24C));

    if (WW_S32(work, 0x20C) != 0xCA) {
        for (i = 0; i < WW_S32(work, 0x254); i++) {
            s32 id = *((u8*)work + 0x26F + i * 2);
            s32 num = *((u8*)work + 0x26E + i * 2);
            WinMailEntry* mail = mailGetPtr(id);
            GXColor color;
            GXColor black = {0x00, 0x00, 0x00, 0xFF};
            GXColor white;

            if (mail->flags & 1) {
                color = (GXColor){0xD7, 0xAC, 0xBC, 0xFF};
            } else if (mail->flags & 2) {
                color = (GXColor){0xD3, 0xD7, 0xAC, 0xFF};
            } else if (mail->flags & 4) {
                color = (GXColor){0xAC, 0xC2, 0xD7, 0xFF};
            } else {
                color = (GXColor){0xE1, 0xE1, 0xE1, 0xFF};
            }
            winTexInit(WIN_MAIL_TEX(work));
            winTexSet(0x77,
                      (Vec){float_4_8041d07c + (float_208_8041d080 + WW_F32(work, 0x244)), WW_F32(work, 0x260) + ((WW_F32(work, 0x24C) - float_56_8041d084) - (f32)(i * 40)), 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, color);
            white = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
            winTexSet((s32)pouchCheckMail(id) == 2 ? 0x5E : 0x5D,
                      (Vec){float_72_8041d088 + WW_F32(work, 0x244), WW_F32(work, 0x260) + (((WW_F32(work, 0x24C) - float_56_8041d084) - (f32)(i * 40)) - float_3_8041d08c), 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, white);
            winTexSet((num + 1) / 10 + 0x65,
                      (Vec){float_2_8041d090 + (float_88_8041d094 + WW_F32(work, 0x244)), WW_F32(work, 0x260) + (((WW_F32(work, 0x24C) - float_56_8041d084) - (f32)(i * 40)) - float_3_8041d08c), 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, white);
            winTexSet((num + 1) % 10 + 0x65,
                      (Vec){float_2_8041d090 + (float_104_8041d098 + WW_F32(work, 0x244)), WW_F32(work, 0x260) + (((WW_F32(work, 0x24C) - float_56_8041d084) - (f32)(i * 40)) - float_3_8041d08c), 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, white);
            winFontInit();
            winMailLabelV((Vec){float_120_8041d09c + WW_F32(work, 0x244), WW_F32(work, 0x260) + (float_8_8041d0a0 + ((WW_F32(work, 0x24C) - float_56_8041d084) - (f32)(i * 40))), 0.0f},
                          (Vec){0.75f, 0.75f, 0.75f}, black, mail->name);
        }
        GXSetScissor(0, 0, 0x260, 0x1E0);
    } else {
        s32 sel = WW_S32(work, 0x258);
        GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};
        GXColor black = {0x00, 0x00, 0x00, 0xFF};
        s32 num = *((u8*)work + 0x26E + sel * 2);
        WinMailEntry* mail = mailGetPtr(*((u8*)work + 0x26F + sel * 2));
        GXColor color;
        u32 top;

        if (mail->flags & 1) {
            color = (GXColor){0xD7, 0xAC, 0xBC, 0xFF};
        } else if (mail->flags & 2) {
            color = (GXColor){0xD3, 0xD7, 0xAC, 0xFF};
        } else if (mail->flags & 4) {
            color = (GXColor){0xAC, 0xC2, 0xD7, 0xFF};
        } else {
            color = (GXColor){0xE1, 0xE1, 0xE1, 0xFF};
        }

        // the header: envelope, opened mark, number and title
        winTexInit(WIN_MAIL_TEX(work));
        winTexSet(0x77,
                  (Vec){float_4_8041d07c + (float_208_8041d080 + WW_F32(work, 0x244)), WW_F32(work, 0x24C) - float_56_8041d084, 0.0f},
                  (Vec){1.0f, 1.0f, 1.0f}, color);
        winTexSet(0x5E,
                  (Vec){float_72_8041d088 + WW_F32(work, 0x244), WW_F32(work, 0x24C) - float_56_8041d084, 0.0f},
                  (Vec){1.0f, 1.0f, 1.0f}, white);
        winTexSet((num + 1) / 10 + 0x65,
                  (Vec){float_2_8041d090 + (float_88_8041d094 + WW_F32(work, 0x244)), (WW_F32(work, 0x24C) - float_56_8041d084) - float_3_8041d08c, 0.0f},
                  (Vec){1.0f, 1.0f, 1.0f}, white);
        winTexSet((num + 1) % 10 + 0x65,
                  (Vec){float_2_8041d090 + (float_104_8041d098 + WW_F32(work, 0x244)), (WW_F32(work, 0x24C) - float_56_8041d084) - float_3_8041d08c, 0.0f},
                  (Vec){1.0f, 1.0f, 1.0f}, white);
        winFontInit();
        winMailLabelV((Vec){float_120_8041d09c + WW_F32(work, 0x244), float_8_8041d0a0 + (WW_F32(work, 0x24C) - float_56_8041d084), 0.0f},
                      (Vec){0.75f, 0.75f, 0.75f}, black, mail->name);

        // the body, clipped to the page and scrolled by whole lines
        top = float_72_8041d088 + (float_240_8041d0ac - WW_F32(work, 0x24C));
        GXSetScissor(float_32_8041d0a4 + (float_304_8041d0a8 + WW_F32(work, 0x244)), top, 0x158, 0xB1);
        winTexInit(WIN_MAIL_TEX(work));
        winTexSet(0x77,
                  (Vec){float_4_8041d07c + (float_208_8041d080 + WW_F32(work, 0x244)), (WW_F32(work, 0x24C) - float_56_8041d084) - float_80_8041d0b0, 0.0f},
                  (Vec){1.0f, 7.5f, 1.0f}, color);
        winFontInit();
        winMailLabelV((Vec){float_80_8041d0b0 + WW_F32(work, 0x244), (WW_F32(work, 0x24C) - float_80_8041d0b0) + (f32)(WW_S32(work, 0x268) * 21), 0.0f},
                      (Vec){0.75f, 0.75f, 0.75f}, (GXColor){0x00, 0x00, 0x00, 0xFF}, mail->body);
        if (mail->flags & 0x1000) {
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x2C) + 0xA0));
            winTexSet(mail->picture,
                      (Vec){float_210_8041d0b4 + WW_F32(work, 0x244), float_7_8041d0b8 + ((WW_F32(work, 0x24C) - (f32)((*(u16*)((u8*)work + 0x26C) - 1) * 21))
                                                   + (f32)(WW_S32(work, 0x268) * 21)), 0.0f},
                      (Vec){1.0f, 1.0f, 1.0f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        }
        GXSetScissor(0, 0, 0x260, 0x1E0);

        // scroll arrows (blinking) and the scroll bar, when the body is longer than a page
        if (*(u16*)((u8*)work + 0x26C) > 8) {
            winIconInit();
            if (WW_S32(work, 0x268) == 0) {
                if ((gp->retraceCount & 0x1F) < 20) {
                    winMailIconV(0x1B6,
                                 (Vec){float_360_8041d0bc + WW_F32(work, 0x244), (float_20_8041d0c0 + (WW_F32(work, 0x24C) - float_230_8041d0c4)) - float_24_8041d0c8, 0.0f},
                                 (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
                }
            } else if (WW_S32(work, 0x268) + 6 == *(u16*)((u8*)work + 0x26C)) {
                if ((gp->retraceCount & 0x1F) < 20) {
                    winMailIconV(0x1B5,
                                 (Vec){float_360_8041d0bc + WW_F32(work, 0x244), float_24_8041d0c8 + (float_20_8041d0c0 + (WW_F32(work, 0x24C) - float_230_8041d0c4)), 0.0f},
                                 (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
                }
            } else if ((gp->retraceCount & 0x1F) < 20) {
                winMailIconV(0x1B5,
                             (Vec){float_360_8041d0bc + WW_F32(work, 0x244), float_24_8041d0c8 + (float_20_8041d0c0 + (WW_F32(work, 0x24C) - float_230_8041d0c4)), 0.0f},
                             (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
                winMailIconV(0x1B6,
                             (Vec){float_360_8041d0bc + WW_F32(work, 0x244), (float_20_8041d0c0 + (WW_F32(work, 0x24C) - float_230_8041d0c4)) - float_24_8041d0c8, 0.0f},
                             (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
            }
            winMailIconV(0x70,
                         (Vec){float_360_8041d0bc + WW_F32(work, 0x244), float_20_8041d0c0 + (WW_F32(work, 0x24C) - float_230_8041d0c4), 0.0f},
                         (Vec){0.6f, 0.6f, 0.6f}, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        }
    }    }
}

void unk_8014d5dc(s32 unused, WindowWork* work) {
    GXSetTexCopySrc(0, 0, gp->fbWidth, gp->efbHeight);
    GXSetTexCopyDst(gp->fbWidth, gp->efbHeight, GX_TF_RGB565, GX_FALSE);
    GXCopyTex(**(void***)((u8*)work + 0x34), GX_FALSE);
    GXTexModeSync();
    WW_S32(work, 0x38) = 1;
    N_mapDispOff();
    camDispOff(4);
}

// Full-screen textured quad. Writing the vertex constants as literals makes MWCC
// reload them per FIFO store (named externs let it common the load), which is what
// the original does; the local `half` gives the original's value*constant operand
// order. The int->f32 magic double still pools anonymously, which objdiff accepts.
static const GXColor dat_8041d064 = {0xFF, 0xFF, 0xFF, 0xFF};

void unk_8014d338(s32 unused, WindowWork* work) {
    GXTexObj texObj;
    f32 w;
    f32 h;

    if (WW_S32(work, 0x38) != 0) {
        GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041d064);
        GXSetNumChans(0);
        GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0, 0, 0xFF);
        GXSetTevOp(0, 3);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
        GXSetBlendMode(1, 4, 5, 0);
        GXSetZCompLoc(1);
        GXSetAlphaCompare(7, 0, 0, 7, 0);
        GXSetZMode(0, 7, 0);
        GXLoadPosMtxImm(camGetPtr(8)->view, 0);
        GXSetCurrentMtx(0);
        GXInitTexObj(&texObj, **(void***)((u8*)work + 0x34), gp->fbWidth, gp->efbHeight, 4, 0, 0, 0);
        GXLoadTexObj(&texObj, 0);
        GXSetCullMode(0);
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(13, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 13, 1, 4, 0);
        GXBegin(0x80, 0, 4);
        w = (f32)gp->fbWidth;
        h = (f32)gp->efbHeight;
        {
            f32 half = 0.5f;
            GXPosition3f32(-w * half, -h * half, 0.0f);
            GXTexCoord2f32(0.0f, 1.0f);
            GXPosition3f32(w * half, -h * half, 0.0f);
            GXTexCoord2f32(1.0f, 1.0f);
            GXPosition3f32(w * half, h * half, 0.0f);
            GXTexCoord2f32(1.0f, 0.0f);
            GXPosition3f32(-w * half, h * half, 0.0f);
            GXTexCoord2f32(0.0f, 0.0f);
        }
    }
}

// Scalar objects whose loads must retain the original named-constant behaviour.
const f32 float_neg240_8041d078 = -240.0f;
const f32 float_208_8041d080 = 208.0f;
const f32 float_2_8041d090 = 2.0f;
const f32 float_88_8041d094 = 88.0f;
const f32 float_104_8041d098 = 104.0f;
const f32 float_80_8041d0b0 = 80.0f;
const f32 float_210_8041d0b4 = 210.0f;
const f32 float_7_8041d0b8 = 7.0f;
const f32 float_360_8041d0bc = 360.0f;
const f32 float_230_8041d0c4 = 230.0f;
const f32 float_neg1000_8041d0cc = -1000.0f;
const f32 float_neg500_8041d0d0 = -500.0f;
const f32 float_300_8041d0d4 = 300.0f;
const f32 float_neg100_8041d0d8 = -100.0f;
const f32 float_neg1p3963_8041d0dc = -1.3962634f;
const f32 float_128_8041d0ec = 128.0f;
const f32 float_neg600_8041d0fc = -600.0f;
const f32 float_30_8041d100 = 30.0f;
const f32 float_0p125_8041d104 = 0.125f;
const f32 float_224_8041d10c = 224.0f;
const f32 float_392_8041d110 = 392.0f;
const f32 float_400_8041d114 = 400.0f;
const f32 float_neg400_8041d118 = -400.0f;
const f32 float_28_8041d120 = 28.0f;
const f32 float_395_8041d124 = 395.0f;
const f32 float_15_8041d128 = 15.0f;
const f32 float_235_8041d12c = 235.0f;
const f32 float_0p7_8041d134 = 0.7f;
const f32 float_380_8041d138 = 380.0f;
const f32 float_228_8041d13c = 228.0f;
const f32 float_237_8041d140 = 237.0f;
const f32 float_90_8041d194 = 90.0f;
const f32 float_74_8041d198 = 74.0f;
const f32 float_66_8041d19c = 66.0f;
const f32 float_500_8041d1a8 = 500.0f;
const f32 float_neg130_8041d1ac = -130.0f;
const f32 float_neg800_8041d1b0 = -800.0f;
const f32 float_neg170_8041d1b4 = -170.0f;
const f32 float_18_8041d1b8 = 18.0f;
const f32 float_570_8041d1bc = 570.0f;
const f32 float_neg285_8041d1c0 = -285.0f;
const f32 float_160_8041d1c4 = 160.0f;
const f32 float_neg160_8041d1c8 = -160.0f;
const f32 float_285_8041d1cc = 285.0f;
const f32 float_144_8041d1d0 = 144.0f;
const f32 float_neg144_8041d1d4 = -144.0f;
const f32 float_800_8041d1d8 = 800.0f;
const f32 float_640_8041d1dc = 640.0f;
const f32 float_neg640_8041d1e0 = -640.0f;
const f32 float_185_8041d1e4 = 185.0f;
const f32 float_1000_8041d1e8 = 1000.0f;
const f32 float_0p25_8041d1ec = 0.25f;
