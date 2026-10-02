#include <dolphin/types.h>
#include <string.h>
#include <stddef.h>
#include "window/win_log.h"
#include "window/win_root.h"
#include "win/win_main.h"
#include "battle/battle_monosiri.h"
#include "driver/animdrv.h"
#include "driver/effdrv.h"
#include "driver/msgdrv.h"
#include "manager/fontmgr.h"
#include "dolphin/gx.h"
#include "dolphin/mtx.h"
#include "math_ext.h"
#include "eff/eff_starstone.h"
#include "mario/mario_pouch.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "driver/swdrv.h"
#include "data/item_data.h"
#include "pmario_sound.h"
#include "mario/mario.h"
#include <stdio.h>
#include "manager/filemgr.h"
#include "mario/mariost.h"
#include "sdk/texPalette.h"
#include "system.h"
#include "memory.h"

// The monosiri (tattle) viewer work struct. Not declared in any header yet;
// fields recovered from monosiriInit / monosiriMain / monosiriExit.
typedef struct MonosiriWork {
    s32 state;         // 0x00
    s32 unitKind;      // 0x04
    FileEntry* file1;  // 0x08
    FileEntry* file2;  // 0x0C
    f32 x;             // 0x10
    f32 y;             // 0x14
    f32 targetX;       // 0x18
    f32 targetY;       // 0x1C
    s32 poseId;        // 0x20
    s32 poseId2;       // 0x24
    void* alloc;       // 0x28
    s32 alpha;         // 0x2C
} MonosiriWork;

extern const f32 float_500_8041d808;
extern const f32 float_neg300_8041d80c;
extern const f32 float_neg128_8041d8dc;
extern const f32 float_320_8041d8b0;
extern const f32 float_neg240_8041d894;
extern const f32 float_neg200_8041d8d4;
extern const f32 float_8_8041d7d4;
extern const f32 float_45_8041d880;
extern const f32 float_0p5_8041d7c8;
extern const f32 float_0_8041d7bc;
extern const f32 float_600_8041d814;
extern const f32 float_neg400_8041d818;
extern const f32 float_6_8041d81c;
extern const f32 float_0p125_8041d890;
extern const f32 float_140_8041d8c0;
extern const f32 float_neg160_8041d8c4;
extern const f32 float_0p25_8041d8c8;
extern const f64 double_0p125_802f63c0;
extern const char str_802F5EB8[];
extern const char str_8041D7AC[];
extern const char str_8041D7B0[];
extern const char str_8041D810[];

// Compiler-generated int->float conversion constant, a global data symbol in
// the original object; defining it here makes the cast below reference the
// original name instead of an anonymous @NNN local.
const f64 double_to_int_802f62d8 = 4503601774854144.0;

#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))
#define WW_PTR(w, off) (*(void**)((u8*)(w) + (off)))

void monosiriExit(void* work) {
    MonosiriWork* w = work;

    if (w != NULL) {
        if (w->file1 != NULL) {
            fileFree(w->file1);
            w->file1 = NULL;
        }
        if (w->file2 != NULL) {
            fileFree(w->file2);
            w->file2 = NULL;
        }
        if (w->poseId != -1) {
            animPoseRelease(w->poseId);
            w->poseId = -1;
        }
        if (w->poseId2 != -1) {
            animPoseRelease(w->poseId2);
            w->poseId2 = -1;
        }
        if (w->alloc != NULL) {
            smartFree(w->alloc);
            w->alloc = NULL;
        }
        _mapFree(w);
    }
}

BOOL monosiriCloseWait(void* work) {
    MonosiriWork* w = work;

    if (w == NULL) {
        return TRUE;
    }
    if (w->x > float_500_8041d808 && w->y < float_neg300_8041d80c) {
        return TRUE;
    }
    return FALSE;
}

void monosiriClose(void* work) {
    MonosiriWork* w = work;

    if (w != NULL) {
        w->state = 2;
    }
}

// The original addresses these .rodata objects as offsets from one base symbol.
#ifdef MUST_MATCH
#define RO(base, off, sym) ((const char*)(base) + (off))
#else
extern const char str_802F62F0[];
extern const char str_802F6300[];
extern const char str_802F6314[];
extern const char str_802F6360[];
extern const char str_802F636C[];
extern const char str_802F6378[];
extern const char str_802F6384[];
extern const char str_802F6390[];
extern const char str_802F639C[];
extern const char str_802F63A8[];
extern const char str_802F63B4[];
#define RO(base, off, sym) ((const char*)&(sym))
#endif


// NONMATCHING (91.96%): only the language-table base differs. `jp`/`us` live in
// a local table the original pools at str_802F5EB8+0x350 and copies through the
// section base in r31; our object pools the same table as an anonymous @185
// local at .rodata+0x8 and copies it through its own base register, so the two
// prologue `lis`/`addi` and the two `lwz` bases differ (the surrounding schedule
// is identical). Caching gp in `g` keeps it in r29 as the original does, and the
// first path lookup uses `g->language` while the second reloads gp, matching the
// original's `lwz r5, gp@sda21`. GXGetTexObjHeight's u16 result is kept in a u16
// local and fileAsyncf's result is compared unsigned, matching the original.
void monosiriMain(void* work) {
    MonosiriWork* w = work;
    TattleEntry* tattle;
    const char* base = str_802F5EB8;
    GlobalWork* g = gp;
    const char* lang[2] = {str_8041D7AC, str_8041D7B0};
    s32 inBattle = (g->inBattle != 0);
    GXTexObj texObj;
    u16 height;
    s32 size;

    if (w == NULL) {
        return;
    }

    switch (w->state) {
    case 0:
        if ((u32)fileAsyncf(4, NULL, RO(base, 0x438, str_802F62F0), getMarioStDvdRoot(), lang[g->language]) != 0) {
            if ((u32)fileAsyncf(0, NULL, RO(base, 0x448, str_802F6300), getMarioStDvdRoot()) != 0) {
                w->file1 = fileAllocf(4, RO(base, 0x438, str_802F62F0), getMarioStDvdRoot(), lang[gp->language]);
                w->file2 = fileAllocf(0, RO(base, 0x448, str_802F6300), getMarioStDvdRoot());
                w->state++;
                TEXGetGXTexObjFromPalette((TEXPalette*)w->file1->data[0], &texObj, 0x37);
                height = GXGetTexObjHeight(&texObj);
                size = GXGetTexBufferSize(GXGetTexObjWidth(&texObj), height, 5, 0, 0);
                w->alloc = smartAlloc(size, 0);
            }
        }
        break;
    case 1:
        tattle = battleGetUnitMonosiriPtr(w->unitKind);
        if (animGroupBaseAsync(tattle->modelName, inBattle, NULL)) {
            if (w->unitKind != 0x6B || animGroupBaseAsync(RO(base, 0x45C, str_802F6314), inBattle, NULL)) {
                w->alpha += 0x14;
                if (w->alpha > 0xFF) {
                    w->alpha = 0xFF;
                }
                if (w->poseId == -1) {
                    if (w->unitKind == 0x6B) {
                        w->poseId = animPoseEntry(tattle->modelName, inBattle);
                        animPoseSetAnim(w->poseId, tattle->poseName, TRUE);
                        animPoseWorldPositionEvalOn(w->poseId);
                        animPoseMain(w->poseId);
                        w->poseId2 = animPoseEntry(RO(base, 0x45C, str_802F6314), inBattle);
                        animPoseSetAnim(w->poseId2, str_8041D810, TRUE);
                        animPoseMain(w->poseId2);
                    } else {
                        w->poseId = animPoseEntry(tattle->modelName, inBattle);
                        animPoseSetAnim(w->poseId, tattle->poseName, TRUE);
                        animPoseMain(w->poseId);
                    }
                    dispEntry(2, float_0_8041d7bc, 6, capture, w);
                }
            }
        }
        break;
    case 2:
        w->targetX = float_600_8041d814;
        w->targetY = float_neg400_8041d818;
        break;
    }

    w->x += (w->targetX - w->x) / float_6_8041d81c;
    w->y += (w->targetY - w->y) / float_6_8041d81c;
    if (w->state > 0) {
        dispEntry(8, float_0_8041d7bc, 0, monosiri_disp, w);
    }
}


void* monosiriInit(BattleUnitType kind, f32 x, f32 y) {
    MonosiriWork* w = _mapAlloc(sizeof(MonosiriWork));

    memset(w, 0, sizeof(MonosiriWork));
    w->state = 0;
    w->unitKind = kind;
    w->targetX = x;
    w->targetY = y;
    w->x = float_500_8041d808;
    w->y = float_neg300_8041d80c;
    w->alpha = 0;
    w->poseId = -1;
    w->poseId2 = -1;
    return w;
}

// The last arm's original codegen keeps swByteGet's result in r3 and computes
// the `>= 0x15C` value before the branch, so the C default-result/override
// ordering below is needed to reproduce it (see rule 7).
const char* winGetMapTplName(void) {
    const char* base = str_802F5EB8;

    if (swByteGet(0) < 0x13) {
        return RO(base, 0x4A8, str_802F6360);
    }
    if (swByteGet(0) < 0x41) {
        return RO(base, 0x4B4, str_802F636C);
    }
    if (swByteGet(0) < 0x79) {
        return RO(base, 0x4C0, str_802F6378);
    }
    if (swByteGet(0) < 0xAF) {
        return RO(base, 0x4CC, str_802F6384);
    }
    if (swByteGet(0) < 0xDE) {
        return RO(base, 0x4D8, str_802F6390);
    }
    if (swByteGet(0) < 0x110) {
        return RO(base, 0x4E4, str_802F639C);
    }
    {
#ifdef MUST_MATCH
        // Default to the `>= 0x15C` template, then override it on the `<` path;
        // the original compares r3 before overwriting it and branches on that
        // earlier compare (rule 7).
        const char* result;
        s32 v = swByteGet(0);

        result = RO(base, 0x4FC, str_802F63B4);
        if (v < 0x15C) {
            result = RO(base, 0x4F0, str_802F63A8);
        }
        return result;
#else
        if (swByteGet(0) >= 0x15C) {
            return RO(base, 0x4FC, str_802F63B4);
        }
        return RO(base, 0x4F0, str_802F63A8);
#endif
    }
}

// A single tattle-log row inside WindowWork, at 0xD78 with stride 0x20.
typedef struct WinLogEntry {
    f32 curX;             // 0xD78
    f32 curY;             // 0xD7C
    f32 targetX;          // 0xD80
    f32 targetY;          // 0xD84
    s32 state;            // 0xD88
    s32 timer;            // 0xD8C
    const char* name;     // 0xD90
    u32 unk_0x1C;         // 0xD94
} WinLogEntry;

typedef struct WinLogWork {
    u8 pad_0[0xD6C];
    s32 sel;              // 0xD6C
    s32 count;            // 0xD70
    u8 pad_74[4];         // 0xD74
    WinLogEntry entries[1]; // 0xD78
} WinLogWork;

// 100%: advances the tattle-log rows (state machine) and eases the panel
// springs. The original's int->float conversions reference
// double_to_int_802f62d8; our object emits an anonymous @271 literal instead,
// which objdiff does not score (same as winBadgeInit2). The per-block `dN`
// temps give the diff-first fmadd operand order; `ep` steps the entry base so
// the prologue copy lands first. The MUST_MATCH `new_var` alias is scheduling
// shaping only (see its comment).
void winLogMain2(WindowWork* work) {
    WinLogWork* w = (WinLogWork*)work;
    u8* ep = (u8*)work;
#ifdef MUST_MATCH
    // Match-only second base alias: with one `work` base MWCC hoists the
    // second int->float fudge store (`stw r0, 0x8(r1)`) one /6 block early;
    // the natural single-base form is under #else (99.22%).
    u8* new_var;
#endif
    s32 i;
#ifdef MUST_MATCH
    new_var = (u8*)work;
#endif

    for (i = 0; i < w->count; i++, ep += 0x20) {
        WinLogEntry* e = (WinLogEntry*)(ep + 0xD78);

        switch (e->state) {
#ifdef MUST_MATCH
        // Empty low/high cases force the jump table to span 0..0x29 (indexed
        // from 0), as in the original; both fall through to the loop tail.
        case 0:
            break;
#endif
        case 10:
            e->targetX = float_0_8041d7bc;
            e->targetY = float_140_8041d8c0;
            e->timer = 0;
            e->state++;
            break;
        case 11:
            if (++e->timer > 8) {
                e->targetX = float_neg160_8041d8c4;
                e->targetY = float_140_8041d8c0;
                e->timer = 0;
                e->state++;
            }
            break;
        case 20:
            e->targetX = float_0_8041d7bc;
            e->targetY = float_140_8041d8c0;
            e->timer = 0;
            e->state++;
            break;
        case 21:
            if (++e->timer > 8) {
                e->targetX = float_0_8041d7bc;
                {
                    f32 prod = float_45_8041d880 * (f32)w->count;
                    e->targetY = prod * float_0p5_8041d7c8 - float_45_8041d880 * (f32)i;
                }
                e->timer = 0;
                e->state++;
            }
            break;
        case 30:
            e->targetX = float_0_8041d7bc;
            e->targetY = float_neg160_8041d8c4;
            e->timer = 0;
            e->state++;
            break;
        case 40:
            e->targetX = float_0_8041d7bc;
            {
                f32 prod = float_45_8041d880 * (f32)w->count;
                e->targetY = prod * float_0p5_8041d7c8 - float_45_8041d880 * (f32)i;
            }
            e->timer = 0;
            e->state++;
            break;
#ifdef MUST_MATCH
        case 41:
            break;
#endif
        }
        {
            f32 dx, dy;
            dx = e->targetX - e->curX;
            e->curX += dx * float_0p125_8041d890;
            dy = e->targetY - e->curY;
            e->curY += dy * float_0p125_8041d890;
        }
    }

#ifdef MUST_MATCH
    {
        f32 d1 = WW_F32(new_var, 0xE28) - WW_F32(new_var, 0xE2C);
        WW_F32(work, 0xE2C) += d1 * double_0p125_802f63c0;
    }
    {
        f32 d2 = WW_F32(new_var, 0xE30) - WW_F32(work, 0xE34);
        WW_F32(new_var, 0xE34) += d2 * double_0p125_802f63c0;
    }
    {
        f32 d3 = WW_F32(work, 0xE64) - WW_F32(work, 0xE68);
        WW_F32(new_var, 0xE68) += d3 * double_0p125_802f63c0;
    }
    {
        f32 d4 = WW_F32(work, 0xE6C) - WW_F32(work, 0xE70);
        WW_F32(new_var, 0xE70) += d4 * double_0p125_802f63c0;
    }
    {
        f32 d5 = WW_F32(new_var, 0xE74) - WW_F32(work, 0xE78);
        WW_F32(work, 0xE78) += d5 * double_0p125_802f63c0;
    }
    {
        f32 d6 = WW_F32(work, 0xE7C) - WW_F32(new_var, 0xE80);
        WW_F32(new_var, 0xE80) += d6 * float_0p25_8041d8c8;
    }
    WW_F32(work, 0xEA4) = (f32)(WW_S32(work, 0xE9C) * 0xE8);
    WW_F32(work, 0xEA0) += (WW_F32(new_var, 0xEA4) - WW_F32(new_var, 0xEA0)) / float_6_8041d81c;
    WW_F32(work, 0xE84) += (WW_F32(work, 0xE88) - WW_F32(new_var, 0xE84)) / float_6_8041d81c;
    WW_F32(new_var, 0xE8C) += (WW_F32(work, 0xE90) - WW_F32(new_var, 0xE8C)) / float_6_8041d81c;
    WW_F32(work, 0xEA8) += (WW_F32(work, 0xEAC) - WW_F32(new_var, 0xEA8)) / float_6_8041d81c;
    WW_F32(new_var, 0xEB0) += (WW_F32(work, 0xEB4) - WW_F32(work, 0xEB0)) / float_6_8041d81c;
    WW_F32(work, 0xFA0) = (f32)(WW_S32(new_var, 0xF98) * 0xE8);
    WW_F32(work, 0xF9C) += (WW_F32(work, 0xFA0) - WW_F32(new_var, 0xF9C)) / float_6_8041d81c;
    WW_F32(new_var, 0xF80) += (WW_F32(new_var, 0xF84) - WW_F32(new_var, 0xF80)) / float_6_8041d81c;
    WW_F32(work, 0xF88) += (WW_F32(work, 0xF8C) - WW_F32(new_var, 0xF88)) / float_6_8041d81c;
    WW_F32(new_var, 0xFA4) += (WW_F32(new_var, 0xFA8) - WW_F32(new_var, 0xFA4)) / float_6_8041d81c;
    WW_F32(work, 0xFAC) += (WW_F32(new_var, 0xFB0) - WW_F32(work, 0xFAC)) / float_6_8041d81c;
#else
    {
        f32 d1 = WW_F32(work, 0xE28) - WW_F32(work, 0xE2C);
        WW_F32(work, 0xE2C) += d1 * double_0p125_802f63c0;
    }
    {
        f32 d2 = WW_F32(work, 0xE30) - WW_F32(work, 0xE34);
        WW_F32(work, 0xE34) += d2 * double_0p125_802f63c0;
    }
    {
        f32 d3 = WW_F32(work, 0xE64) - WW_F32(work, 0xE68);
        WW_F32(work, 0xE68) += d3 * double_0p125_802f63c0;
    }
    {
        f32 d4 = WW_F32(work, 0xE6C) - WW_F32(work, 0xE70);
        WW_F32(work, 0xE70) += d4 * double_0p125_802f63c0;
    }
    {
        f32 d5 = WW_F32(work, 0xE74) - WW_F32(work, 0xE78);
        WW_F32(work, 0xE78) += d5 * double_0p125_802f63c0;
    }
    {
        f32 d6 = WW_F32(work, 0xE7C) - WW_F32(work, 0xE80);
        WW_F32(work, 0xE80) += d6 * float_0p25_8041d8c8;
    }
    WW_F32(work, 0xEA4) = (f32)(WW_S32(work, 0xE9C) * 0xE8);
    WW_F32(work, 0xEA0) += (WW_F32(work, 0xEA4) - WW_F32(work, 0xEA0)) / float_6_8041d81c;
    WW_F32(work, 0xE84) += (WW_F32(work, 0xE88) - WW_F32(work, 0xE84)) / float_6_8041d81c;
    WW_F32(work, 0xE8C) += (WW_F32(work, 0xE90) - WW_F32(work, 0xE8C)) / float_6_8041d81c;
    WW_F32(work, 0xEA8) += (WW_F32(work, 0xEAC) - WW_F32(work, 0xEA8)) / float_6_8041d81c;
    WW_F32(work, 0xEB0) += (WW_F32(work, 0xEB4) - WW_F32(work, 0xEB0)) / float_6_8041d81c;
    WW_F32(work, 0xFA0) = (f32)(WW_S32(work, 0xF98) * 0xE8);
    WW_F32(work, 0xF9C) += (WW_F32(work, 0xFA0) - WW_F32(work, 0xF9C)) / float_6_8041d81c;
    WW_F32(work, 0xF80) += (WW_F32(work, 0xF84) - WW_F32(work, 0xF80)) / float_6_8041d81c;
    WW_F32(work, 0xF88) += (WW_F32(work, 0xF8C) - WW_F32(work, 0xF88)) / float_6_8041d81c;
    WW_F32(work, 0xFA4) += (WW_F32(work, 0xFA8) - WW_F32(work, 0xFA4)) / float_6_8041d81c;
    WW_F32(work, 0xFAC) += (WW_F32(work, 0xFB0) - WW_F32(work, 0xFAC)) / float_6_8041d81c;
#endif
}

#define SFX(id) psndSFXOn((const char*)(id))

typedef struct TubuDt {
    s16 x;           // 0x0
    s16 y;           // 0x2
    u8 visible;      // 0x4
    u8 flagIdx;      // 0x5
    u16 minProgress; // 0x6
    const char* id;  // 0x8
} TubuDt;


// winGetMapTplName as the compiler inlined it into winLogMain, sharing the caller's string base register.
#ifdef MUST_MATCH
#define MAP_TPL_NAME(dst, base)                                       \
    if (swByteGet(0) < 0x13) {                                        \
        dst = RO(base, 0x4A8, str_802F6360);                          \
    } else if (swByteGet(0) < 0x41) {                                 \
        dst = RO(base, 0x4B4, str_802F636C);                          \
    } else if (swByteGet(0) < 0x79) {                                 \
        dst = RO(base, 0x4C0, str_802F6378);                          \
    } else if (swByteGet(0) < 0xAF) {                                 \
        dst = RO(base, 0x4CC, str_802F6384);                          \
    } else if (swByteGet(0) < 0xDE) {                                 \
        dst = RO(base, 0x4D8, str_802F6390);                          \
    } else if (swByteGet(0) < 0x110) {                                \
        dst = RO(base, 0x4E4, str_802F639C);                          \
    } else if (swByteGet(0) < 0x15C) {                                \
        dst = RO(base, 0x4F0, str_802F63A8);                          \
    } else {                                                          \
        dst = RO(base, 0x4FC, str_802F63B4);                          \
    }
#else
#define MAP_TPL_NAME(dst, base) dst = winGetMapTplName()
#endif

typedef struct LogRow {
    s32 type;         // 0x00
    f32 curX;         // 0x04
    f32 curY;         // 0x08
    f32 targetX;      // 0x0C
    f32 targetY;      // 0x10
    s32 phase;        // 0x14
    s32 timer;        // 0x18
    const char* name; // 0x1C
} LogRow;

typedef struct LogChar {
    u8 pad;
    u8 id;
} LogChar;

// The fields of WindowWork that winLogMain touches (win_main.h only declares the first 0x1210 bytes).
typedef struct LogWin {
    u16 flags; // 0x0
    u8 pad_2[0x2];
    u32 buttonTrg; // 0x4
    u32 buttonRep; // 0x8
    u32 dirTrg; // 0xC
    u32 dirRep; // 0x10
    u8 pad_14[0x1C];
    FileEntry* file; // 0x30
    u8 pad_34[0xF0];
    s32 retState; // 0x124
    u8 pad_128[0x30];
    f32 curX; // 0x158
    f32 curY; // 0x15C
    u8 pad_160[0xC08];
    s32 state; // 0xD68
    s32 sel; // 0xD6C
    s32 count; // 0xD70
    LogRow rows[5]; // 0xD74
    s32 mapSel; // 0xE14
    s32 mapPick; // 0xE18
    s32 pose; // 0xE1C
    f32 mapCx; // 0xE20
    f32 mapCy; // 0xE24
    f32 e28; // 0xE28
    f32 e2c; // 0xE2C
    f32 e30; // 0xE30
    f32 e34; // 0xE34
    f32 mapScrollX; // 0xE38
    f32 mapScrollY; // 0xE3C
    s32 stoneNum; // 0xE40
    EffectEntry* stones[7]; // 0xE44
    s32 stoneSel; // 0xE60
    f32 e64; // 0xE64
    f32 e68; // 0xE68
    f32 e6c; // 0xE6C
    f32 e70; // 0xE70
    f32 e74; // 0xE74
    f32 e78; // 0xE78
    f32 e7c; // 0xE7C
    f32 e80; // 0xE80
    f32 e84; // 0xE84
    f32 e88; // 0xE88
    f32 e8c; // 0xE8C
    f32 e90; // 0xE90
    s32 cardCnt; // 0xE94
    s32 cardSel; // 0xE98
    s32 cardPage; // 0xE9C
    f32 ea0; // 0xEA0
    f32 ea4; // 0xEA4
    f32 ea8; // 0xEA8
    f32 eac; // 0xEAC
    f32 eb0; // 0xEB0
    f32 eb4; // 0xEB4
    s16 cardIcon[100]; // 0xEB8
    f32 f80; // 0xF80
    f32 f84; // 0xF84
    f32 f88; // 0xF88
    f32 f8c; // 0xF8C
    s32 badgeCnt; // 0xF90
    s32 badgeSel; // 0xF94
    s32 badgePage; // 0xF98
    f32 f9c; // 0xF9C
    f32 fa0; // 0xFA0
    f32 fa4; // 0xFA4
    f32 fa8; // 0xFA8
    f32 fac; // 0xFAC
    f32 fb0; // 0xFB0
    s16 badgeIcon[58]; // 0xFB4
    f32 x1028; // 0x1028
    f32 x102c; // 0x102C
    f32 x1030; // 0x1030
    f32 x1034; // 0x1034
    s32 charCnt; // 0x1038
    s32 seenCnt; // 0x103C
    s32 charSel; // 0x1040
    s32 charPage; // 0x1044
    f32 x1048; // 0x1048
    f32 x104c; // 0x104C
    LogChar chars[216]; // 0x1050
    void* mono; // 0x1200
    u8 pad_1204[0xC];
} LogWin;

#define LOGWIN_CHECK(field, off) typedef char logwin_##field##_check[(offsetof(LogWin, field) == (off)) ? 1 : -1]
LOGWIN_CHECK(rows, 0xD74);
LOGWIN_CHECK(mapSel, 0xE14);
LOGWIN_CHECK(f84, 0xF84);
LOGWIN_CHECK(badgeIcon, 0xFB4);
LOGWIN_CHECK(charCnt, 0x1038);
LOGWIN_CHECK(chars, 0x1050);
LOGWIN_CHECK(mono, 0x1200);

extern TubuDt tubuDt[0x5D];
extern s32 stone_tbl[7];
extern const char str_802F63C8[];
extern const char str_802F63D4[];
extern const char str_802F63DC[];
extern const char str_802F63E4[];
extern const char str_802F63F0[];
extern const char str_802F63FC[];
extern const char str_802F6408[];
extern const char str_802F641C[];
extern const char str_802F6428[];
extern const char str_802F6444[];
extern const char str_802F6460[];
extern const f32 float_20_8041d8ac;
extern const f32 float_255_8041d8cc;
extern const f32 float_neg130_8041d8d0;
extern const f32 float_120_8041d870;
extern const f32 float_60_8041d8d8;
extern const f32 float_neg480_8041d89c;
extern const f32 float_neg230_8041d908;
extern const f32 float_74_8041d86c;
extern const f32 float_neg120_8041d90c;
extern const f32 float_10000_8041d904;
extern const f32 float_50_8041d828;
extern const f32 float_0p2_8041d914;
extern const f32 float_304_8041d874;
extern const f32 float_240_8041d858;
extern const f32 float_16_8041d7e4;
extern const f32 float_604_8041d918;
extern const f32 float_468_8041d91c;
extern const f32 float_230_8041d878;
extern const f32 float_166_8041d830;
extern const f32 float_neg150_8041d920;
extern const f32 float_150_8041d924;
extern const f32 float_80_8041d928;
extern const f32 float_neg80_8041d92c;
extern const f32 float_neg148_8041d898;
extern const f32 float_24_8041d838;
extern const f32 float_51p429_8041d8a8;
extern const f32 float_360_8041d930;
extern const f32 float_neg50_8041d934;
extern const f32 float_330_8041d938;
extern const f32 float_neg250_8041d93c;
extern const f32 float_neg310_8041d940;
extern const f32 float_neg60_8041d944;
extern const f32 float_neg8_8041d910;

static inline BOOL tubuUnlocked(TubuDt* t) {
    switch (t->flagIdx) {
    case 0:
        return FALSE;
    case 1:
        if (swGet(0xFD) != 0) {
            return TRUE;
        }
        break;
    case 2:
        if (swGet(0xFE) != 0) {
            return TRUE;
        }
        break;
    case 3:
        if (swGet(0xFF) != 0) {
            return TRUE;
        }
        break;
    case 4:
        if (swGet(0x100) != 0) {
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// NONMATCHING (95.42%): behaviour complete, checked against the asm. What differs is register allocation (the
// original keeps the string base in r31 and `work` in r29; ours swaps them) and the int->float conversion double,
// which the original reaches through the string base register (base+0x420) while ours pools an anonymous literal.
// Tattle log / menu main: the tabbed "kiroku" window (map, crystal stars, cards, tattle log).
s32 winLogMain(WindowWork* work) {
    LogWin* w = (LogWin*)work;
    const char* base = str_802F5EB8;
    const char* tpl;
    s32 i;

    {
        switch (w->state) {
        case 0:
            if (w->buttonTrg & 0x200) {
                SFX(0x20013);
                return -1;
            }
            if (w->buttonTrg & 0x1000) {
                return -2;
            }
            if (w->dirRep & 0x2000) {
                s32 old = w->sel;
                w->sel = old + 1;
                if (w->sel >= w->count) {
                    w->sel = 0;
                }
                if (old != w->sel) {
                    SFX(0x20005);
                }
            } else if (w->dirRep & 0x1000) {
                s32 old = w->sel;
                w->sel = old - 1;
                if (w->sel < 0) {
                    w->sel = w->count - 1;
                }
                if (old != w->sel) {
                    SFX(0x20005);
                }
            } else if (w->buttonTrg & 0x100) {
                for (i = 0; i < w->count; i++) {
                    w->rows[i].phase = 0x1E;
                }
                w->rows[w->sel].phase = 0xA;
                w->state = w->rows[w->sel].type + 0xA;
                switch (w->rows[w->sel].type) {
                case 4:
                    break;
                case 0:
                    w->e30 = float_20_8041d8ac;
                    break;
                case 1:
                    w->e6c = float_20_8041d8ac;
                    w->e74 = float_255_8041d8cc;
                    break;
                case 2:
                    w->e88 = float_neg130_8041d8d0;
                    w->e90 = float_120_8041d870;
                    w->eac = float_neg200_8041d8d4;
                    w->eb4 = float_60_8041d8d8;
                    break;
                case 3:
                    w->f84 = float_neg130_8041d8d0;
                    w->f8c = float_120_8041d870;
                    w->fa8 = float_neg200_8041d8d4;
                    w->fb0 = float_60_8041d8d8;
                    break;
                }
                SFX(0x20012);
            }
            w->curX = float_neg128_8041d8dc;
            {
                f32 t = float_45_8041d880 * (f32)w->count;
                w->curY =
                    float_8_8041d7d4 + (t * float_0p5_8041d7c8 - float_45_8041d880 * (f32)w->sel);
            }
            winMsgEntry(work, 0, w->rows[w->sel].name, 0);
            break;
        case 10:
            MAP_TPL_NAME(tpl, base);
            if ((u32)fileAsyncf(4, NULL, RO(base, 0x510, str_802F63C8), getMarioStDvdRoot(), tpl) != 0) {
                if (w->file == 0) {
                    MAP_TPL_NAME(tpl, base);
                    w->file = (FileEntry*)fileAllocf(4, RO(base, 0x510, str_802F63C8), getMarioStDvdRoot(), tpl);
                }
                if (w->pose == -1) {
                    if (w->flags & 0x2000) {
                        const char* anim = "W_1";
                        w->pose = animPoseEntry(RO(base, 0x51C, str_802F63D4), 0);
                        switch (marioGetColor()) {
                        case 0:
                            anim = "W_1";
                            break;
                        case 1:
                            anim = "W_15";
                            break;
                        case 2:
                            anim = "W_17";
                            break;
                        case 3:
                            anim = "W_18";
                            break;
                        }
                        animPoseSetAnim(w->pose, anim, 1);
                    } else {
                        const char* model = RO(base, 0x524, str_802F63DC);
                        switch (marioGetColor()) {
                        case 0:
                            break;
                        case 1:
                            model = RO(base, 0x52C, str_802F63E4);
                            break;
                        case 2:
                            model = RO(base, 0x538, str_802F63F0);
                            break;
                        case 3:
                            model = RO(base, 0x544, str_802F63FC);
                            break;
                        }
                        w->pose = animPoseEntry(model, 0);
                        animPoseSetAnim(w->pose, "M_W_1", 1);
                    }
                    animPoseSetMaterialFlagOn(w->pose, 0x1800);
                }
                if (w->buttonTrg & 0x200) {
                    SFX(0x20013);
                    for (i = 0; i < w->count; i++) {
                        w->rows[i].phase = 0x28;
                    }
                    w->rows[w->sel].phase = 0x14;
                    w->state = 0;
                    w->e30 = float_neg480_8041d89c;
                    fileFree((FileEntry*)w->file);
                    w->file = 0;
                    if (w->pose != -1) {
                        animPoseRelease(w->pose);
                    }
                    w->pose = -1;
                } else if (w->buttonTrg & 0x1000) {
                    return -2;
                }
                {
                    f32 x0 = w->mapScrollX;
                    f32 y0 = w->mapScrollY;
                    f32 px = float_neg230_8041d908 + x0 + w->mapCx;
                    f32 qx = float_74_8041d86c + x0;
                    f32 py = float_neg120_8041d90c + y0 + w->mapCy;
                    f32 qy = float_120_8041d870 + y0;
                    f32 best = float_10000_8041d904;
                    s32 bestIdx;
                    u32 j;
                    TubuDt* t = tubuDt;
                    f32 sx, sy;
                    s32 ox, oy;

                    for (j = 0; j < 0x5D; j++, t++) {
                        if (swByteGet(0) < (s32)t->minProgress) {
                            if (!tubuUnlocked(t)) {
                                continue;
                            }
                        }
                        if (t->visible != 0) {
                            f32 d = distABf(qx + (f32)t->x, qy + (f32)t->y, px, py);
                            if (best > d) {
                                best = d;
                                bestIdx = j;
                            }
                        }
                    }
                    if (best < float_50_8041d828) {
                        static char name[0x40];
                        static char help[0x40];
                        sprintf(name, RO(base, 0x550, str_802F6408), tubuDt[bestIdx].id);
                        sprintf(help, RO(base, 0x564, str_802F641C), tubuDt[bestIdx].id);
                        if (w->mapSel != bestIdx) {
                            SFX(0x20035);
                        }
                        w->mapSel = bestIdx;
                        winMsgEntry(work, 0, help, (s32)name);
                    } else {
                        w->mapSel = -1;
                        winMsgEntry(work, 0, base, 0);
                    }
                    sx = (f32)keyGetStickX(0) * float_0p125_8041d890;
                    sy = (f32)keyGetStickY(0) * float_0p125_8041d890;
                    if (float_0_8041d7bc == sx && float_0_8041d7bc == sy) {
                        if (keyGetButton(0) & 1) {
                            sx = float_neg8_8041d910;
                        } else if (keyGetButton(0) & 2) {
                            sx = float_8_8041d7d4;
                        }
                        if (keyGetButton(0) & 8) {
                            sy = float_8_8041d7d4;
                        } else if (keyGetButton(0) & 4) {
                            sy = float_neg8_8041d910;
                        }
                    }
                    if (float_0_8041d7bc == sx && float_0_8041d7bc == sy) {
                        s32 sel = w->mapSel;
                        if (sel != -1) {
                            sx = ((float_304_8041d874 + (f32)tubuDt[sel].x) - w->mapCx) * float_0p2_8041d914;
                            sy = ((float_240_8041d858 + (f32)tubuDt[sel].y) - w->mapCy) * float_0p2_8041d914;
                        }
                    }
                    w->mapCx = w->mapCx + sx;
                    w->mapCy = w->mapCy + sy;
                    if (w->mapCx < float_16_8041d7e4) {
                        w->mapCx = float_16_8041d7e4;
                    }
                    if (w->mapCy < float_8_8041d7d4) {
                        w->mapCy = float_8_8041d7d4;
                    }
                    if (w->mapCx >= float_604_8041d918) {
                        w->mapCx = float_604_8041d918;
                    }
                    if (w->mapCy >= float_468_8041d91c) {
                        w->mapCy = float_468_8041d91c;
                    }
                    ox = (s32)((w->mapCx + w->mapScrollX) - float_230_8041d878);
                    oy = (s32)((w->mapCy + w->mapScrollY) - float_120_8041d870);
                    if ((f32)ox >= float_166_8041d830) {
                        w->mapScrollX = w->mapScrollX - ((f32)ox - float_166_8041d830);
                    }
                    if ((f32)ox <= float_neg150_8041d920) {
                        w->mapScrollX = w->mapScrollX - (float_150_8041d924 + (f32)ox);
                    }
                    if ((f32)oy >= float_80_8041d928) {
                        w->mapScrollY = w->mapScrollY - ((f32)oy - float_80_8041d928);
                    }
                    if ((f32)oy <= float_neg80_8041d92c) {
                        w->mapScrollY = w->mapScrollY - (float_80_8041d928 + (f32)oy);
                    }
                    if (w->mapScrollX > float_0_8041d7bc) {
                        w->mapScrollX = float_0_8041d7bc;
                    }
                    if (w->mapScrollY > float_0_8041d7bc) {
                        w->mapScrollY = float_0_8041d7bc;
                    }
                    if (w->mapScrollX < float_neg148_8041d898) {
                        w->mapScrollX = float_neg148_8041d898;
                    }
                    if (w->mapScrollY < float_neg240_8041d894) {
                        w->mapScrollY = float_neg240_8041d894;
                    }
                    w->curX = float_neg230_8041d908 + w->mapScrollX + w->mapCx;
                    w->curY = float_neg120_8041d90c + w->mapScrollY + w->mapCy;
                    w->curX = w->curX - float_24_8041d838;
                    w->curY = w->curY + float_20_8041d8ac;
                }
            }
            break;
        case 11:
            if (w->buttonTrg & 0x200) {
                SFX(0x20013);
                for (i = 0; i < w->count; i++) {
                    w->rows[i].phase = 0x28;
                }
                w->rows[w->sel].phase = 0x14;
                w->state = 0;
                w->e6c = float_neg480_8041d89c;
                w->e74 = float_0_8041d7bc;
            } else if (w->dirRep & 0x4000) {
                w->stoneSel = w->stoneSel - 1;
                while (w->stoneSel < 0) {
                    w->stoneSel = w->stoneSel + 7;
                }
                w->e7c = float_51p429_8041d8a8 * (f32)w->stoneSel;
                while (w->e80 < w->e7c) {
                    w->e80 += float_360_8041d930;
                }
                SFX(0x209D6);
                SFX(0x20037);
            } else if (w->dirRep & 0x8000) {
                w->stoneSel = w->stoneSel + 1;
                while (w->stoneSel > 6) {
                    w->stoneSel = w->stoneSel - 7;
                }
                w->e7c = float_51p429_8041d8a8 * (f32)w->stoneSel;
                while (w->e80 > w->e7c) {
                    w->e80 -= float_360_8041d930;
                }
                SFX(0x209D6);
                SFX(0x20037);
            } else if (w->buttonTrg & 0x1000) {
                return -2;
            }
            w->curX = float_neg50_8041d934;
            w->curY = float_neg50_8041d934;
            if (w->stoneSel < w->stoneNum) {
                ItemData* it = &itemDataTable[stone_tbl[w->stoneSel]];
                winMsgEntry(work, -1, it->description, (s32)it->name);
            } else {
                winMsgEntry(work, -1, RO(base, 0x570, str_802F6428), (s32)RO(base, 0x58C, str_802F6444));
            }
            break;
        case 12: {
            s32 cnt = w->cardCnt;
            u32 trg = w->buttonTrg;
            s32 pages = (cnt + 0x1B) / 28;
            if (trg & 0x200) {
                SFX(0x20013);
                for (i = 0; i < w->count; i++) {
                    w->rows[i].phase = 0x28;
                }
                w->rows[w->sel].phase = 0x14;
                w->state = 0;
                w->e88 = float_330_8041d938;
                w->e90 = float_neg250_8041d93c;
                w->eac = float_neg200_8041d8d4;
                w->eb4 = float_neg300_8041d80c;
            } else if (w->buttonRep & 0x40) {
                w->cardPage = w->cardPage - 1;
                if (w->cardPage < 0) {
                    w->cardPage = 0;
                }
                w->cardSel = w->cardPage * 0x1C;
                SFX(0x20035);
            } else if (w->buttonRep & 0x20) {
                w->cardPage = w->cardPage + 1;
                if (w->cardPage >= pages) {
                    w->cardPage = pages - 1;
                }
                w->cardSel = w->cardPage * 0x1C;
                SFX(0x20035);
            } else if (w->dirRep & 0x8000) {
                if (w->cardSel % 7 < 6) {
                    s32 v = w->cardSel + 1;
                    if (v < cnt) {
                        w->cardSel = v;
                    }
                }
                SFX(0x20035);
            } else if (w->dirRep & 0x4000) {
                if (w->cardSel % 7 > 0) {
                    w->cardSel = w->cardSel - 1;
                }
                SFX(0x20035);
            } else if (w->dirRep & 0x1000) {
                s32 cur = w->cardSel;
                s32 v = cur - 7;
                if (v >= 0 && v < cnt) {
                    w->cardSel = cur - 7;
                }
                w->cardPage = w->cardSel / 28;
                SFX(0x20035);
            } else if (w->dirRep & 0x2000) {
                s32 cur = w->cardSel;
                s32 v = cur + 7;
                if (v >= 0 && v < cnt) {
                    w->cardSel = cur + 7;
                } else if (cur + (7 - w->cardSel % 7) < cnt) {
                    w->cardSel = cnt - 1;
                }
                w->cardPage = w->cardSel / 28;
                SFX(0x20035);
            } else if (trg & 0x10) {
                winSortEntry(work, 4, float_neg310_8041d940, float_150_8041d924);
                w->retState = w->state;
                SFX(0x20012);
                w->state = 0x3E8;
            } else if (trg & 0x1000) {
                return -2;
            }
            w->curX = (f32)(((w->cardSel % 28) % 7) * 0x38 - 0x8C);
            w->curY = (f32)(0x5A - ((w->cardSel % 28) / 7) * 0x38);
            if (swGet(w->cardIcon[w->cardSel] + 0x80) != 0) {
                ItemData* it = &itemDataTable[w->cardIcon[w->cardSel]];
                winMsgEntry(work, -2, (it + 240)->menu_description, (s32)(it + 240)->name);
            } else {
                winMsgEntry(work, -2, RO(base, 0x570, str_802F6428), (s32)RO(base, 0x58C, str_802F6444));
            }
            break;
        }
        case 13: {
            s32 cnt = w->badgeCnt;
            u32 trg = w->buttonTrg;
            s32 pages = (cnt + 0x1B) / 28;
            if (trg & 0x200) {
                SFX(0x20013);
                for (i = 0; i < w->count; i++) {
                    w->rows[i].phase = 0x28;
                }
                w->rows[w->sel].phase = 0x14;
                w->state = 0;
                w->f84 = float_330_8041d938;
                w->f8c = float_neg250_8041d93c;
                w->fa8 = float_neg200_8041d8d4;
                w->fb0 = float_neg300_8041d80c;
            } else if (w->buttonRep & 0x40) {
                w->badgePage = w->badgePage - 1;
                if (w->badgePage < 0) {
                    w->badgePage = 0;
                }
                w->badgeSel = w->badgePage * 0x1C;
                SFX(0x20035);
            } else if (w->buttonRep & 0x20) {
                w->badgePage = w->badgePage + 1;
                if (w->badgePage >= pages) {
                    w->badgePage = pages - 1;
                }
                w->badgeSel = w->badgePage * 0x1C;
                SFX(0x20035);
            } else if (w->dirRep & 0x8000) {
                if (w->badgeSel % 7 < 6) {
                    s32 v = w->badgeSel + 1;
                    if (v < cnt) {
                        w->badgeSel = v;
                    }
                }
                SFX(0x20035);
            } else if (w->dirRep & 0x4000) {
                if (w->badgeSel % 7 > 0) {
                    w->badgeSel = w->badgeSel - 1;
                }
                SFX(0x20035);
            } else if (w->dirRep & 0x1000) {
                s32 cur = w->badgeSel;
                s32 v = cur - 7;
                if (v >= 0 && v < cnt) {
                    w->badgeSel = cur - 7;
                }
                w->badgePage = w->badgeSel / 28;
                SFX(0x20035);
            } else if (w->dirRep & 0x2000) {
                s32 cur = w->badgeSel;
                s32 v = cur + 7;
                if (v >= 0 && v < cnt) {
                    w->badgeSel = cur + 7;
                } else if (cur + (7 - w->badgeSel % 7) < cnt) {
                    w->badgeSel = cnt - 1;
                }
                w->badgePage = w->badgeSel / 28;
                SFX(0x20035);
            } else if (trg & 0x10) {
                winSortEntry(work, 5, float_neg310_8041d940, float_150_8041d924);
                w->retState = w->state;
                SFX(0x20012);
                w->state = 0x3E8;
            } else if (trg & 0x1000) {
                return -2;
            }
            w->curX = (f32)(((w->badgeSel % 28) % 7) * 0x38 - 0x8C);
            w->curY = (f32)(0x5A - ((w->badgeSel % 28) / 7) * 0x38);
            if (swGet(w->badgeIcon[w->badgeSel] + 0x41) != 0) {
                ItemData* it = &itemDataTable[w->badgeIcon[w->badgeSel]];
                winMsgEntry(work, -2, (it + 179)->menu_description, (s32)(it + 179)->name);
            } else {
                winMsgEntry(work, -2, RO(base, 0x570, str_802F6428), (s32)RO(base, 0x58C, str_802F6444));
            }
            break;
        }
        case 0xE: {
            u32 trg = w->buttonTrg;
            s32 pages = (w->charCnt + 0xF) / 16;
            if (trg & 0x200) {
                SFX(0x20013);
                for (i = 0; i < w->count; i++) {
                    w->rows[i].phase = 0x28;
                }
                w->rows[w->sel].phase = 0x14;
                w->state = 0;
                w->x102c = float_320_8041d8b0;
                w->x1034 = float_neg240_8041d894;
            } else if (w->buttonRep & 0x40) {
                w->charPage = w->charPage - 1;
                if (w->charPage < 0) {
                    w->charPage = 0;
                }
                w->charSel = w->charPage * 0x10;
                SFX(0x20035);
            } else if (w->buttonRep & 0x20) {
                w->charPage = w->charPage + 1;
                if (w->charPage >= pages) {
                    w->charPage = pages - 1;
                }
                w->charSel = w->charPage * 0x10;
                SFX(0x20035);
            } else if (w->dirRep & 0xC000) {
                s32 cur = w->charSel;
                if (cur % 16 < 8) {
                    w->charSel = cur + 8;
                } else {
                    w->charSel = cur - 8;
                }
                if (w->charSel >= w->charCnt) {
                    w->charSel = w->charCnt - 1;
                }
                if (w->charSel < 0) {
                    w->charSel = 0;
                }
                SFX(0x20035);
            } else if (w->dirRep & 0x1000) {
                s32 cur = w->charSel;
                if (cur % 8 == 0) {
                    w->charSel = cur - 9;
                } else {
                    w->charSel = cur - 1;
                }
                if (w->charSel < 0) {
                    w->charSel = 0;
                }
                w->charPage = w->charSel / 16;
                SFX(0x20035);
            } else if (w->dirRep & 0x2000) {
                s32 cur = w->charSel;
                if (cur % 8 == 7) {
                    w->charSel = cur + 9;
                } else {
                    w->charSel = cur + 1;
                }
                if (w->charSel >= w->charCnt) {
                    w->charSel = w->charCnt - 1;
                }
                w->charPage = w->charSel / 16;
                SFX(0x20035);
            } else if (trg & 0x100) {
                if (swGet(w->chars[w->charSel].id + 0x117A) != 0) {
                    u8 kind;
                    SFX(0x20012);
                    w->state = 0x15;
                    kind = w->chars[w->charSel].id;
                    w->mono = monosiriInit(kind, float_neg60_8041d944, float_150_8041d924);
                }
            } else if (trg & 0x10) {
                winSortEntry(work, 6, float_neg310_8041d940, float_150_8041d924);
                w->retState = w->state;
                SFX(0x20012);
                w->state = 0x3E9;
            } else if (trg & 0x1000) {
                return -2;
            }
            w->curX = (f32)((w->charSel % 16 / 8) * 0xD8 - 0xB4);
            w->curY = (f32)(0x6A - (w->charSel % 16 % 8) * 0x18);
            if (swGet(w->chars[w->charSel].id + 0x117A) != 0) {
                winMsgEntry(work, 0,
                            battleGetUnitMonosiriPtr(w->chars[w->charSel].id)->tattleLogTag, 0);
            } else {
                winMsgEntry(work, 0, RO(base, 0x5A8, str_802F6460), 0);
            }
    
            break;
        }
        case 21:
            if (w->buttonTrg & 0x200) {
                SFX(0x20013);
                w->state = 0x16;
                monosiriClose(w->mono);
            } else if (w->buttonTrg & 0x1000) {
                monosiriExit(w->mono);
                return -2;
            } else {
                monosiriMain(w->mono);
            }
            break;
        case 22:
            if (monosiriCloseWait(w->mono)) {
                w->state = 0xE;
                monosiriExit(w->mono);
            } else {
                monosiriMain(w->mono);
            }
            break;
        case 0x3E8:
        case 0x3E9:
            if (winSortWait(work) == 0) {
                w->state = w->retState;
            }
            if (w->buttonTrg & 0x1000) {
                return -2;
            }
            break;
        }
    }
    return 0;
}

void winLogExit(WindowWork* work) {
    s32 i;

    if (WW_PTR(work, 0x30) != NULL) {
        fileFree(WW_PTR(work, 0x30));
        WW_PTR(work, 0x30) = NULL;
    }
    if (WW_S32(work, 0xE1C) != -1) {
        animPoseRelease(WW_S32(work, 0xE1C));
        WW_S32(work, 0xE1C) = -1;
    }
    for (i = 0; i < 7; i++) {
        effSoftDelete(*(EffectEntry**)((u8*)work + 0xE44 + i * 4));
    }
}

// 100%: the 45.0f * (f32)a product is held in a named temp so MWCC colors the
// long-lived int->float conversion double f7 (as the original does) instead of
// f6; the entry address is formed with real WinLogEntry pointer indexing, which
// makes the compiler emit `add rWork, rIndex` in the original's operand order.
// The conversion double still pools as an anonymous @271 literal instead of the
// named double_to_int_802f62d8 (project-wide limitation, not scored).
void winLogInit2(WindowWork* work) {
    f32 v;
    const char* name;
    s32 a;
    s32 b;

    WW_F32(work, 0x158) = float_neg128_8041d8dc;
    a = WW_S32(work, 0xD70);
    b = WW_S32(work, 0xD6C);
    {
        f32 t = float_45_8041d880 * (f32)a;
        WW_F32(work, 0x15C) = float_8_8041d7d4 + (t * float_0p5_8041d7c8 - float_45_8041d880 * (f32)b);
    }
    WW_F32(work, 0xE88) = float_320_8041d8b0;
    WW_F32(work, 0xE84) = float_320_8041d8b0;
    WW_F32(work, 0xE90) = float_neg240_8041d894;
    WW_F32(work, 0xE8C) = float_neg240_8041d894;
    v = WW_S32(work, 0xE9C) * 0xE8;
    WW_F32(work, 0xEA4) = v;
    WW_F32(work, 0xEA0) = v;
    WW_F32(work, 0xEAC) = float_neg200_8041d8d4;
    WW_F32(work, 0xEA8) = float_neg200_8041d8d4;
    WW_F32(work, 0xEB4) = float_neg300_8041d80c;
    WW_F32(work, 0xEB0) = float_neg300_8041d80c;
    WW_F32(work, 0xF84) = float_320_8041d8b0;
    WW_F32(work, 0xF80) = float_320_8041d8b0;
    WW_F32(work, 0xF8C) = float_neg240_8041d894;
    WW_F32(work, 0xF88) = float_neg240_8041d894;
    v = WW_S32(work, 0xF98) * 0xE8;
    WW_F32(work, 0xFA0) = v;
    WW_F32(work, 0xF9C) = v;
    WW_F32(work, 0xFA8) = float_neg200_8041d8d4;
    WW_F32(work, 0xFA4) = float_neg200_8041d8d4;
    WW_F32(work, 0xFB0) = float_neg300_8041d80c;
    WW_F32(work, 0xFAC) = float_neg300_8041d80c;
    name = ((WinLogEntry*)((u8*)work + 0xD78))[WW_S32(work, 0xD6C)].name;
    winMsgEntry(work, 0, name, 0);
}

extern const u8 enemy_monoshiri_sort_table[];
extern const char str_802F6478[];
extern const char str_802F6480[];
extern const char str_802F6488[];
extern const char str_802F6490[];
extern const char str_802F6498[];
extern const char str_802F64A0[];
extern const char str_802F64A8[];
extern const char str_802F64B0[];
extern const f32 float_2_8041d87c;
extern const f32 float_neg1000_8041d948;

// Appends a tab to the menu and re-centres every tab row.
#define LOG_ADD_TAB(w, tabType, msg)                                                  \
    do {                                                                              \
        LogRow* r_ = &(w)->rows[(w)->count];                                          \
        s32 k_;                                                                       \
        r_->type = (tabType);                                                         \
        r_->timer = 0;                                                                \
        r_->phase = 0;                                                                \
        r_->name = (msg);                                                             \
        (w)->count++;                                                                 \
        for (k_ = 0; k_ < (w)->count; k_++) {                                         \
            (w)->rows[k_].curX = float_0_8041d7bc;                                    \
            (w)->rows[k_].targetX = float_0_8041d7bc;                                 \
            {                                                                         \
                f32 y_ = float_45_8041d880 * (f32)(w)->count * float_0p5_8041d7c8    \
                         - float_45_8041d880 * (f32)k_;                               \
                (w)->rows[k_].curY = y_;                                              \
                (w)->rows[k_].targetY = y_;                                           \
            }                                                                         \
        }                                                                             \
    } while (0)

// NONMATCHING (82.24%): behaviour complete, checked against the asm; the rest is register allocation (the original keeps
// the stone counter in r27) and the int->float conversion double reached through the string base register.
void winLogInit(WindowWork* work) {
    LogWin* w = (LogWin*)work;
    static const char* msg_help[5] = {str_802F5EB8, str_802F5EB8 + 0x14, str_802F5EB8 + 0x2C, str_802F5EB8 + 0x44, str_802F5EB8 + 0x5C};
    const char* base = str_802F5EB8;
    const char* area;
    s32 i;
    s32 n;
    const u8* sortp;

    w->state = 0;
    w->sel = 0;
    w->count = 0;
    LOG_ADD_TAB(w, 0, msg_help[0]);
    if (swByteGet(0) >= 0x13) {
        LOG_ADD_TAB(w, 1, msg_help[1]);
    }
    if (swByteGet(0) >= 7) {
        LOG_ADD_TAB(w, 4, msg_help[4]);
    }
    if (swByteGet(0) >= 0x14) {
        LOG_ADD_TAB(w, 2, msg_help[2]);
    }
    if (swGet(0x40) != 0) {
        LOG_ADD_TAB(w, 3, msg_help[3]);
    }
    w->stoneNum = 0;
    for (i = 0; i < 7; i++) {
        if (pouchCheckItem(stone_tbl[i]) != 0) {
            w->stoneNum++;
        }
    }
    for (i = 0; i < 7; i++) {
        EffectEntry* e;

        if (i < w->stoneNum) {
            e = w->stones[i] =
                effStarStoneEntry(i, float_0_8041d7bc, float_neg1000_8041d948, float_0_8041d7bc, float_2_8041d87c);
        } else {
            e = w->stones[i] = effStarStoneEntry(i + 7, float_0_8041d7bc, float_neg1000_8041d948, float_0_8041d7bc,
                                                 float_2_8041d87c);
            ((u8*)e->userdata)[0x5C] = 0x20;
            ((u8*)e->userdata)[0x5D] = 0x20;
            ((u8*)e->userdata)[0x5E] = 0x20;
        }
        *(s32*)((u8*)e->userdata + 0x38) = 8;
        *(u16*)((u8*)e->userdata + 4) &= ~4;
        *(u16*)((u8*)e->userdata + 4) |= 0x20;
    }
    w->file = NULL;
    w->pose = -1;
    w->mapSel = -1;
    w->mapPick = 0;
    w->mapCy = float_0_8041d7bc;
    w->mapCx = float_0_8041d7bc;
    w->e2c = float_0_8041d7bc;
    w->e28 = float_0_8041d7bc;
    w->e34 = float_neg480_8041d89c;
    w->e30 = float_neg480_8041d89c;
    w->mapScrollY = float_0_8041d7bc;
    w->mapScrollX = float_0_8041d7bc;
    area = gp->currentAreaName;
    if (strcmp(gp->currentMapName, RO(base, 0x5C0, str_802F6478)) == 0
        || strcmp(gp->currentMapName, RO(base, 0x5C8, str_802F6480)) == 0) {
        area = "sin";
    }
    if (strcmp(gp->currentMapName, RO(base, 0x5D0, str_802F6488)) == 0
        || strcmp(gp->currentMapName, RO(base, 0x5D8, str_802F6490)) == 0) {
        area = "hou";
    }
    if (strncmp(gp->currentMapName, "rsh", 3) == 0) {
        if (swByteGet(0) < 0x136) {
            area = "gor";
        } else if (swByteGet(0) < 0x14E || swGet(0xDFB) != 0) {
            area = "eki";
        } else if (swGet(0xDFC) != 0) {
            area = "pik";
        } else {
            switch (swByteGet(0x366)) {
            case 0:
                area = "gor";
                break;
            case 1:
                area = "eki";
                break;
            case 2:
                area = "pik";
                break;
            case 3:
                area = "gor";
                break;
            }
        }
    }
    if (strncmp(gp->currentMapName, "hom", 3) == 0) {
        area = "eki";
    }
    if (strncmp(gp->currentMapName, "jon", 3) == 0) {
        area = "tik";
    }
    if (strcmp(gp->currentMapName, RO(base, 0x5E0, str_802F6498)) == 0) {
        area = "jin";
    }
    if (strcmp(gp->currentMapName, RO(base, 0x5E8, str_802F64A0)) == 0
        || strcmp(gp->currentMapName, RO(base, 0x5F0, str_802F64A8)) == 0
        || strcmp(gp->currentMapName, RO(base, 0x5F8, str_802F64B0)) == 0) {
        area = "dou";
    }
    for (n = 0; n < 0x5D; n++) {
        if (area != NULL && strcmp(area, tubuDt[n].id) == 0) {
            break;
        }
    }
    if ((u32)n < 0x5D) {
        w->mapPick = n;
    }
    w->mapCx = float_304_8041d874 + (f32)tubuDt[w->mapPick].x;
    w->mapCy = float_240_8041d858 + (f32)tubuDt[w->mapPick].y;
    w->mapScrollX = w->mapScrollX - (f32)(s32)((w->mapCx + w->mapScrollX) - float_230_8041d878);
    w->mapScrollY = w->mapScrollY - (f32)(s32)((w->mapCy + w->mapScrollY) - float_120_8041d870);
    if (w->mapScrollX > float_0_8041d7bc) {
        w->mapScrollX = float_0_8041d7bc;
    }
    if (w->mapScrollY > float_0_8041d7bc) {
        w->mapScrollY = float_0_8041d7bc;
    }
    if (w->mapScrollX < float_neg148_8041d898) {
        w->mapScrollX = float_neg148_8041d898;
    }
    if (w->mapScrollY < float_neg240_8041d894) {
        w->mapScrollY = float_neg240_8041d894;
    }
    w->stoneSel = 0;
    w->e7c = float_51p429_8041d8a8 * (f32)w->stoneSel;
    w->e80 = w->e7c;
    w->e68 = float_0_8041d7bc;
    w->e64 = float_0_8041d7bc;
    w->e70 = float_neg480_8041d89c;
    w->e6c = float_neg480_8041d89c;
    w->e78 = float_0_8041d7bc;
    w->e74 = float_0_8041d7bc;
    w->cardCnt = 0;
    for (i = 0; i < 0x63; i++) {
        if (itemDataTable[0xF0 + i].type_sort_order != -1) {
            w->cardIcon[w->cardCnt++] = i;
        }
    }
    qqsort(w->cardIcon, w->cardCnt, 2, compare_func5_2);
    w->cardSel = 0;
    w->e88 = float_320_8041d8b0;
    w->e84 = float_320_8041d8b0;
    w->e90 = float_neg240_8041d894;
    w->e8c = float_neg240_8041d894;
    w->cardPage = 0;
    w->ea4 = float_0_8041d7bc;
    w->ea0 = float_0_8041d7bc;
    w->eac = float_neg200_8041d8d4;
    w->ea8 = float_neg200_8041d8d4;
    w->eb4 = float_neg300_8041d80c;
    w->eb0 = float_neg300_8041d80c;
    w->badgeCnt = 0;
    for (i = 0; i < 0x63; i++) {
        if (itemDataTable[0xB3 + i].type_sort_order != -1) {
            w->badgeIcon[w->badgeCnt++] = i;
        }
    }
    qqsort(w->badgeIcon, w->badgeCnt, 2, compare_func6_2);
    w->badgeSel = 0;
    w->f84 = float_320_8041d8b0;
    w->f80 = float_320_8041d8b0;
    w->f8c = float_neg240_8041d894;
    w->f88 = float_neg240_8041d894;
    w->badgePage = 0;
    w->fa0 = float_0_8041d7bc;
    w->f9c = float_0_8041d7bc;
    w->fa8 = float_neg200_8041d8d4;
    w->fa4 = float_neg200_8041d8d4;
    w->fb0 = float_neg300_8041d80c;
    w->fac = float_neg300_8041d80c;
    w->charCnt = 0;
    sortp = enemy_monoshiri_sort_table + 1;
    for (i = 1; i < 0xD8; i++, sortp++) {
        if (battleGetUnitMonosiriPtr(i)->tattleLogTag != NULL) {
            if (evtGetValue(NULL, GSW(0)) < 0x187) {
                if ((u32)(i - 0x85) <= 1 || i == 0x87) {
                    continue;
                }
            } else {
                if ((u32)(i - 0x1F) <= 1 || i == 0x4D) {
                    continue;
                }
            }
            if (evtGetValue(NULL, GSW(0)) < 0x190) {
                if (i == 0x90) {
                    continue;
                }
            } else {
                if (i == 0x3F) {
                    continue;
                }
            }
            w->chars[w->charCnt].pad = *sortp;
            w->chars[w->charCnt++].id = i;
        }
    }
    {
        u16* list = (u16*)w->chars;
        s32 a;
        s32 b;

        for (a = 0; a < w->charCnt - 1; a++) {
            for (b = a + 1; b < w->charCnt; b++) {
                if ((u8)(list[a] >> 8) > (u8)(list[b] >> 8)) {
                    u16 t = list[a];
                    list[a] = list[b];
                    list[b] = t;
                }
            }
        }
    }
    w->charSel = 0;
    w->x102c = float_0_8041d7bc;
    w->x1028 = float_0_8041d7bc;
    w->x1034 = float_0_8041d7bc;
    w->x1030 = float_0_8041d7bc;
    w->charPage = 0;
    w->x104c = float_0_8041d7bc;
    w->x1048 = float_0_8041d7bc;
    w->seenCnt = 0;
    for (i = 1; i < 0xD8; i++) {
        if (battleGetUnitMonosiriPtr(i)->tattleLogTag != NULL && (u32)(i - 0x1F) > 1 && i != 0x4D && i != 0x3F
            && swGet(i + 0x117A) != 0) {
            w->seenCnt++;
        }
    }
}

extern const Vec vec3_802f5f2c;
extern const Vec vec3_802f5f38;
extern const Vec vec3_802f5f44;
extern const Vec vec3_802f5f50;
extern const Vec vec3_802f5f5c;
extern const Vec vec3_802f5f68;
extern const Vec vec3_802f5f74;
extern const Vec vec3_802f5f80;
extern const Vec vec3_802f5f8c;
extern const Vec vec3_802f5f98;
extern const Vec vec3_802f5fa4;
extern const Vec vec3_802f5fb0;
extern const Vec vec3_802f5fbc;
extern const Vec vec3_802f5fc8;
extern const Vec vec3_802f5fd4;
extern const Vec vec3_802f5fe0;
extern const Vec vec3_802f5fec;
extern const GXColor dat_8041d78c;
extern const GXColor dat_8041d790;
extern const GXColor dat_8041d794;
extern const GXColor dat_8041d798;
extern const GXColor dat_8041d79c;
extern const f32 float_0p017453_8041d8a4;
extern const f32 float_0p43633_8041d8a0;
extern const f32 float_30_8041d864;
extern const f32 float_4_8041d88c;
extern const f32 float_10_8041d8b4;
extern const f32 float_170_8041d8b8;
extern const f32 float_130_8041d8bc;
extern const f32 float_1_8041d7c0;

#ifdef MUST_MATCH
#define LV(off, sym) (*(const Vec*)((const char*)base + (off)))
#else
#define LV(off, sym) (sym)
#endif
static inline Vec logMkVec(Vec base, f32 x, f32 y) {
    base.x = x;
    base.y = y;
    return base;
}

#define LOG_TEXPAL() (**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0))

// NONMATCHING (81.85%): behaviour complete; register colouring and the stack order of the Vec copies differ.
// Draws one page of the log window: map, crystal stars, cards, badges, the tattle viewer and the tab strip.
void winLogDisp(CameraId camId, WindowWork* work, s32 idx) {
    LogWin* w = (LogWin*)work;
    const char* base = str_802F5EB8;
    GXColor white = dat_8041d78c;
    GXColor white2 = dat_8041d78c;
    GXColor white3 = dat_8041d78c;
    f32 x = WW_F32(work, 0xC4 + idx * 0x14);
    f32 y = WW_F32(work, 0xC8 + idx * 0x14);
    s32 i;

    winBgGX(work, 4, x, y);
    if (y + w->e34 > float_neg480_8041d89c) {
        mapGX(work, x + w->e2c, y + w->e34);
    }
    if (y + w->e70 > float_neg480_8041d89c) {
        GXColor col = dat_8041d790;

        winTexInit(LOG_TEXPAL());
        col.a = (s8)w->e78;
        winTexSet(0x24, logMkVec(LV(0x74, vec3_802f5f2c), x + w->e68, y + w->e70), LV(0x80, vec3_802f5f38), col);
        for (i = 0; i < 7; i++) {
            Vec p = LV(0x8C, vec3_802f5f44);
            Mtx rx;
            Mtx ry;
            GXColor shadow = dat_8041d794;

            PSMTXRotRad(rx, 'x', float_0p43633_8041d8a0);
            PSMTXRotRad(ry, 'y', float_0p017453_8041d8a4 * (float_51p429_8041d8a8 * (f32)i - w->e80));
            PSMTXMultVec(ry, &p, &p);
            PSMTXMultVec(rx, &p, &p);
            winKageGX(work, &shadow, p.x + (x + w->e68), (p.y + (y + w->e70)) - float_30_8041d864, p.z,
                      float_1_8041d7c0);
            ((f32*)w->stones[i]->userdata)[2] = p.x + (x + w->e68);
            ((f32*)w->stones[i]->userdata)[3] = float_20_8041d8ac + (p.y + (y + w->e70));
            ((f32*)w->stones[i]->userdata)[4] = p.z;
        }
        for (i = 0; i < 7; i++) {
            effStarStoneDraw(8, w->stones[i]);
        }
    }
    if (w->e84 < float_320_8041d8b0 && w->e8c > float_neg240_8041d894) {
        winHakoGX(work, 0, x + w->e84, y + w->e8c);
        winTexInit(LOG_TEXPAL());
        winTexSet(0x95, logMkVec(LV(0x98, vec3_802f5f50), x + w->ea8, y + w->eb0), LV(0xA4, vec3_802f5f5c), white);
        if (swGet(w->cardIcon[w->cardSel] + 0x80) != 0) {
            Vec scale = LV(0xBC, vec3_802f5f74);
            Vec translate;

            winIconInit();
            translate = logMkVec(LV(0xB0, vec3_802f5f68), x + w->ea8, (y + w->eb0) - float_4_8041d88c);
            winIconSet(itemDataTable[w->cardIcon[w->cardSel] + 240].icon_id, &translate, &scale, &white2);
        }
    }
    if (w->f80 < float_320_8041d8b0 && w->f88 > float_neg240_8041d894) {
        winHakoGX(work, 1, x + w->f80, y + w->f88);
        winTexInit(LOG_TEXPAL());
        winTexSet(0x96, logMkVec(LV(0xC8, vec3_802f5f80), x + w->fa4, y + w->fac), LV(0xD4, vec3_802f5f8c), white);
        if (swGet(w->badgeIcon[w->badgeSel] + 0x41) != 0) {
            Vec scale = LV(0xEC, vec3_802f5fa4);
            Vec translate;

            winIconInit();
            translate = logMkVec(LV(0xE0, vec3_802f5f98), x + w->fa4, (y + w->fac) - float_10_8041d8b4);
            winIconSet(itemDataTable[w->badgeIcon[w->badgeSel] + 179].icon_id, &translate, &scale, &white2);
        }
    }
    if (w->state == 0xE || (u32)(w->state - 0x14) <= 2 || w->state == 0x3E9) {
        monoshiriGX(work, x - float_170_8041d8b8, float_130_8041d8bc + y);
    }
    for (i = 0; i < w->count; i++) {
        if (i == w->sel) {
            GXColor c3 = dat_8041d798;

            winTexInit(LOG_TEXPAL());
            winTexSet(0xB5, logMkVec(LV(0xF8, vec3_802f5fb0), x + w->rows[i].curX, y + w->rows[i].curY),
                      LV(0x104, vec3_802f5fbc), c3);
            winTexInit_x2(LOG_TEXPAL());
            winTexSet_x2(w->rows[i].type + 0x1F, 0xB5,
                         logMkVec(LV(0x110, vec3_802f5fc8), (x + w->rows[i].curX) - float_8_8041d7d4,
                                  float_8_8041d7d4 + (y + w->rows[i].curY)),
                         LV(0x11C, vec3_802f5fd4), white3);
        } else {
            GXColor c4 = dat_8041d79c;

            winTexInit_x2(LOG_TEXPAL());
            winTexSet_x2(w->rows[i].type + 0x1F, 0xB5,
                         logMkVec(LV(0x128, vec3_802f5fe0), x + w->rows[i].curX, y + w->rows[i].curY),
                         LV(0x134, vec3_802f5fec), c4);
        }
    }
}
extern const Vec vec3_802f5ff8;
extern const Vec vec3_802f6004;
extern const Vec vec3_802f6010;
extern const Vec vec3_802f601c;
extern const Vec vec3_802f6028;
extern const Vec vec3_802f6034;
extern const Vec vec3_802f6040;
extern const Vec vec3_802f604c;
extern const Vec vec3_802f6058;
extern const Vec vec3_802f6064;
extern const Vec vec3_802f6070;
extern const Vec vec3_802f607c;
extern const Vec vec3_802f6088;
extern const Vec vec3_802f6094;
extern const Vec vec3_802f60a0;
extern const Vec vec3_802f60ac;
extern const Vec vec3_802f60b8;
extern const Vec vec3_802f60c4;
extern const Vec vec3_802f60d0;
extern const Vec vec3_802f60dc;
extern const Vec vec3_802f60e8;
extern const Vec vec3_802f60f4;
extern const Vec vec3_802f6100;
extern const Vec vec3_802f610c;

extern const GXColor dat_8041d7a0;
extern const f32 float_74_8041d86c;
extern const f32 float_120_8041d870;
extern const f32 float_304_8041d874;
extern const f32 float_230_8041d878;
extern const f32 float_240_8041d858;
extern const f32 float_220_8041d868;
extern const f32 float_1p5_8041d884;
extern const f32 float_3_8041d888;
extern const f32 float_0p125_8041d890;
extern const f32 float_neg148_8041d898;

// NONMATCHING (75.01%): behaviour complete, checked against the asm; Vec copy order and register colouring differ.
void mapGX(WindowWork* work, f32 x, f32 y) {
    static f32 pos_tbl[7][2] = {
        {165.0f, 68.0f}, {0.0f, 75.0f}, {97.0f, 150.0f}, {-139.0f, -33.0f}, {41.0f, -163.0f}, {211.0f, -91.0f}, {-209.0f, 159.0f},
    };
    LogWin* w = (LogWin*)work;
    const char* base = str_802F5EB8;
    f32 tx = float_74_8041d86c + (x + w->mapScrollX);
    f32 ty = float_120_8041d870 + (y + w->mapScrollY);
    GXColor c0 = dat_8041d7a0;
    GXColor c1 = dat_8041d7a0;
    GXColor c2 = dat_8041d7a0;
    f32 top;
    f32 bounce;
    s32 i;
    TubuDt* t;

    if (w->file != NULL) {
        top = float_120_8041d870 + y;
        GXSetScissor((u32)(((float_304_8041d874 + x) - float_230_8041d878) - float_2_8041d87c),
                     (u32)((float_240_8041d858 - top) - float_2_8041d87c), 0x1D0, 0xF4);
        winTexInit((TEXPalette*)w->file->data[0]);
        winTexSet(0, logMkVec(LV(0x140, vec3_802f5ff8), tx, ty), LV(0x14C, vec3_802f6004), c1);
        winTexInit(LOG_TEXPAL());
        t = tubuDt;
        for (i = 0; (u32)i < 0x5D; i++, t++) {
            if (swByteGet(0) < (s32)t->minProgress) {
                if (!tubuUnlocked(t)) {
                    continue;
                }
            }
            if (w->mapSel == i) {
                s32 id = 0xBA;
                GXColor c = c2;

                if ((gp->retraceCount >> 4) & 1) {
                    id = 0xB9;
                }
                winTexSet(id, logMkVec(LV(0x158, vec3_802f6010), tx + (f32)t->x, ty + (f32)t->y), LV(0x164, vec3_802f601c), c);
            } else {
                s32 id = 0xB8;
                GXColor c = c2;

                if (t->visible != 0) {
                    id = 0xBA;
                }
                winTexSet(id, logMkVec(LV(0x170, vec3_802f6028), tx + (f32)t->x, ty + (f32)t->y), LV(0x17C, vec3_802f6034), c);
            }
        }
        {
            s32 sw = swByteGet(0);
            s32 icon = -1;
            s32 k;

            if (sw >= 0x13 && sw < 0x37) {
                icon = 0x197;
                k = 0;
            } else if (sw >= 0x41 && sw < 0x70) {
                icon = 0x198;
                k = 1;
            } else if (sw >= 0x79 && sw < 0xA4) {
                icon = 0x19A;
                k = 2;
            } else if (sw >= 0xAF && sw < 0xD5) {
                icon = 0x19C;
                k = 3;
            } else if (sw >= 0xDE && sw < 0x105) {
                icon = 0x19D;
                k = 4;
            } else if (sw >= 0x110 && sw < 0x152) {
                icon = 0x199;
                k = 5;
            } else if (sw >= 0x15C && sw < 0x175) {
                icon = 0x19B;
                k = 6;
            }
            if (icon != -1) {
                Vec scale = LV(0x194, vec3_802f604c);
                Vec translate;

                winIconInit();
                translate = logMkVec(LV(0x188, vec3_802f6040), tx + pos_tbl[k][0], float_45_8041d880 + (ty + pos_tbl[k][1]));
                winIconSet(icon, &translate, &scale, &c0);
            }
        }
        if (w->pose != -1) {
            AnimData* anim;
            f32 scale = float_1p5_8041d884;
            Mtx m;
            s32 n;

            animPoseGetAnimPosePtr(WW_S32(work, 0x188));
            anim = animPoseGetAnimDataPtr(WW_S32(work, 0x188));
            if (w->flags & 0x2000) {
                scale = float_3_8041d888;
            }
            PSMTXTrans(m, tx + (f32)tubuDt[w->mapPick].x, ty + (f32)tubuDt[w->mapPick].y, float_0_8041d7bc);
            n = (s32)*(f32*)(*(u8**)((u8*)anim + 0x24) + 8);
            animPoseSetLocalTime(w->pose, (f32)(gp->retraceCount % n));
            animPoseMain(w->pose);
            animPoseDrawMtx(w->pose, m, 1, float_0_8041d7bc, scale);
            animPoseDrawMtx(w->pose, m, 2, float_0_8041d7bc, scale);
            animPoseDrawMtx(w->pose, m, 3, float_0_8041d7bc, scale);
        }
        GXSetScissor(0, 0, 0x260, 0x1E0);
        winTexInit(LOG_TEXPAL());
        winTexSet(0xBF, logMkVec(LV(0x1A0, vec3_802f6058), (x - float_230_8041d878) - float_4_8041d88c, y), LV(0x1AC, vec3_802f6064), c1);
        winTexSet(0xBF, logMkVec(LV(0x1B8, vec3_802f6070), float_4_8041d88c + (float_230_8041d878 + x), y), LV(0x1C4, vec3_802f607c), c1);
        winTexSet(0xBF, logMkVec(LV(0x1D0, vec3_802f6088), x, float_4_8041d88c + top), LV(0x1DC, vec3_802f6094), c1);
        winTexSet(0xBF, logMkVec(LV(0x1E8, vec3_802f60a0), x, (y - float_120_8041d870) - float_4_8041d88c), LV(0x1F4, vec3_802f60ac), c1);
        bounce = (f32)__fabs(float_2_8041d87c * (f32)sin((f32)gp->retraceCount * float_0p125_8041d890));
        winTexInit(LOG_TEXPAL());
        winTexSet(0xBD, logMkVec(LV(0x200, vec3_802f60b8), x, top + (float_neg240_8041d894 == w->mapScrollY ? float_0_8041d7bc : bounce)), LV(0x20C, vec3_802f60c4), c1);
        winTexInit(LOG_TEXPAL());
        winTexSet(0xBC, logMkVec(LV(0x218, vec3_802f60d0), float_220_8041d868 + x + (float_neg148_8041d898 == w->mapScrollX ? float_0_8041d7bc : bounce), y), LV(0x224, vec3_802f60dc), c1);
        winTexInit(LOG_TEXPAL());
        winTexSet(0xBE, logMkVec(LV(0x230, vec3_802f60e8), x, (y - float_120_8041d870) - (float_0_8041d7bc == w->mapScrollY ? float_0_8041d7bc : bounce)), LV(0x23C, vec3_802f60f4), c1);
        winTexInit(LOG_TEXPAL());
        winTexSet(0xBB, logMkVec(LV(0x248, vec3_802f6100), (x - float_220_8041d868) - (float_0_8041d7bc == w->mapScrollX ? float_0_8041d7bc : bounce), y), LV(0x254, vec3_802f610c), c1);
    }
}
extern const Vec vec3_802f6118;
extern const Vec vec3_802f6124;
extern const Vec vec3_802f6130;
extern const Vec vec3_802f613c;
extern const Vec vec3_802f6148;
extern const Vec vec3_802f6154;
extern const Vec vec3_802f6160;
extern const Vec vec3_802f616c;
extern const Vec vec3_802f6178;
extern const Vec vec3_802f6184;
extern const Vec vec3_802f6190;
extern const Vec vec3_802f619c;
extern const Vec vec3_802f61a8;
extern const Vec vec3_802f61b4;
extern const Vec vec3_802f61c0;
extern const Vec vec3_802f61cc;
extern const Vec vec3_802f61d8;
extern const Vec vec3_802f61e4;
extern const Vec vec3_802f61f0;
extern const Vec vec3_802f61fc;
extern const f32 float_110_8041d820;
extern const f32 float_160_8041d7d8;
extern const f32 float_100_8041d824;
extern const f32 float_50_8041d828;
extern const f32 float_0p75_8041d7e0;
extern const f32 float_108_8041d82c;
extern const f32 float_166_8041d830;
extern const f32 float_184_8041d834;
extern const f32 float_56_8041d840;
extern const f32 float_24_8041d838;
extern const f32 float_5_8041d83c;
extern const f32 float_16_8041d7e4;
extern const f32 float_440_8041d84c;
extern const f32 float_18_8041d850;
extern const f32 float_15_8041d854;
extern const f32 float_240_8041d858;
extern const f32 float_0p7_8041d85c;
extern const f32 float_420_8041d860;
extern const f32 float_210_8041d7f0;
extern const f32 float_30_8041d864;
extern const f32 float_220_8041d868;

extern const GXColor dat_8041d7a4;
extern const GXColor dat_8041d7a8;
extern const char str_802F6320[];
extern const char str_802F6338[];
extern const char str_802F6340[];
extern const char str_802F6348[];
extern const char str_802F6290[];
extern const char str_802F62A0[];

// NONMATCHING (74.42%): behaviour complete, checked against the asm; Vec copy order and register colouring differ.
void monoshiriGX(WindowWork* work, f32 x, f32 y) {
    LogWin* w = (LogWin*)work;
    const char* base = str_802F5EB8;
    GXColor c8 = dat_8041d7a8;
    GXColor c4 = dat_8041d7a4;
    GXColor c8b = dat_8041d7a8;
    GXColor c8c = dat_8041d7a8;
    const char* msg;
    s32 wdt;
    s32 i;
    f32 fx;
    f32 fy;
    f32 xr;
    f32 yr;
    char buf[0x20];
    char buf2[0x20];

    winBookGX(work, 1);
    winKirinukiGX(work, 0, x - float_110_8041d820, y - float_160_8041d7d8, float_100_8041d824, float_50_8041d828);
    msg = msgSearch(RO(base, 0x468, str_802F6320));
    wdt = (s32)(float_0p75_8041d7e0 * (f32)FontGetMessageWidth(msg));
    winFontInit();
    {
        Vec scale = LV(0x26C, vec3_802f6124);
        Vec pos = logMkVec(LV(0x260, vec3_802f6118), (x - float_108_8041d82c) + (f32)((100 - (u16)wdt) / 2),
                           y - float_166_8041d830);
        GXColor c = c8b;

        winFontSet(&pos, &scale, &c, msg);
    }
    sprintf(buf, RO(base, 0x480, str_802F6338), w->seenCnt, w->charCnt);
    wdt = (s32)(float_0p75_8041d7e0 * (f32)FontGetMessageWidth(buf));
    {
        Vec scale = LV(0x284, vec3_802f613c);
        Vec pos = logMkVec(LV(0x278, vec3_802f6130), (x - float_108_8041d82c) + (f32)((100 - (u16)wdt) / 2),
                           y - float_184_8041d834);
        GXColor c = c8b;

        winFontSet(&pos, &scale, &c, buf);
    }
    winFontInit();
    xr = float_56_8041d840 + x;
    fx = (float_24_8041d838 + x) - float_5_8041d83c;
    fy = y - float_16_8041d7e4;
    for (i = 0; i < 16; i++) {
        s32 idx = i + w->charPage * 16;
        s32 id = w->chars[idx].id;
        TattleEntry* tattle = battleGetUnitMonosiriPtr(id);
        f32 xo;
        f32 yo;

        if (idx >= w->charCnt) {
            break;
        }
        if (swGet(id + 0x117A) != 0) {
            if (id == 0x4D || id == 0x87) {
                if (evtGetValue(NULL, GSW(0)) < 0xD2) {
                    msg = msgSearch(RO(base, 0x3D8, str_802F6290));
                } else {
                    msg = msgSearch(RO(base, 0x3E8, str_802F62A0));
                }
            } else {
                msg = msgSearch(tattle->enemyTag);
            }
        } else {
            msg = RO(base, 0x488, str_802F6340);
        }
        sprintf(buf2, "%d\x81\x44", (s32)enemy_monoshiri_sort_table[id]);
        xo = (f32)((i / 8) * 0xD8);
        yo = fy - (f32)((i % 8) * 0x18);
        {
            Vec scale = LV(0x29C, vec3_802f6154);
            Vec pos = logMkVec(LV(0x290, vec3_802f6148), fx + xo, yo);
            GXColor c = c8c;

            winFontSet(&pos, &scale, &c, buf2);
        }
        {
            Vec scale = LV(0x2B4, vec3_802f616c);
            Vec pos = logMkVec(LV(0x2A8, vec3_802f6160), xr + xo, yo);
            GXColor c = c8c;

            winFontSet(&pos, &scale, &c, msg);
        }
    }
    if (w->charPage > 0) {
        f32 ax = float_440_8041d84c + x;

        winTexInit(LOG_TEXPAL());
        winTexSet(0x17, logMkVec(LV(0x2C0, vec3_802f6178), ax, float_18_8041d850 + y), LV(0x2CC, vec3_802f6184), c4);
        winIconInit();
        {
            Vec scale = LV(0x2E4, vec3_802f619c);
            Vec pos = logMkVec(LV(0x2D8, vec3_802f6190), ax, y);

            winIconSet(0x7E, &pos, &scale, &c4);
        }
    }
    if (w->charPage < (w->charCnt + 0xF) / 16 - 1) {
        f32 ay = y - float_15_8041d854;
        f32 ax = float_440_8041d84c + x;

        winTexInit(LOG_TEXPAL());
        winTexSet(0x17, logMkVec(LV(0x2F0, vec3_802f61a8), ax, ay - float_240_8041d858), LV(0x2FC, vec3_802f61b4), c4);
        winIconInit();
        {
            Vec scale = LV(0x314, vec3_802f61cc);
            Vec pos = logMkVec(LV(0x308, vec3_802f61c0), ax, y - float_240_8041d858);

            winIconSet(0x80, &pos, &scale, &c4);
        }
    }
    msg = msgSearch(RO(base, 0x490, str_802F6348));
    {
        f32 fw = float_0p7_8041d85c * (f32)FontGetMessageWidth(msg);
        f32 ox;

        winFontInit();
        ox = x + (float_420_8041d860 - fw);
        {
            Vec scale = LV(0x32C, vec3_802f61e4);
            Vec pos = logMkVec(LV(0x320, vec3_802f61d8), ox, y - float_210_8041d7f0);
            GXColor c = c8;

            winFontSet(&pos, &scale, &c, msg);
        }
        winIconInit();
        {
            Vec scale = LV(0x344, vec3_802f61fc);
            Vec pos = logMkVec(LV(0x338, vec3_802f61f0), ox - float_30_8041d864, y - float_220_8041d868);

            winIconSet(0x217, &pos, &scale, &c4);
        }
    }
}
extern const Vec vec3_802f6258;
extern const Vec vec3_802f6264;
extern const Vec vec3_802f6270;
extern const Vec vec3_802f627c;

extern const GXColor dat_8041d7b4;
extern const GXColor dat_8041d7b8;
extern const GXColor unk_80423050;
extern const char str_802F6288[];
extern const char str_802F62B0[];
extern const char str_802F62B8[];
extern const f32 float_0p04_8041d7c4;
extern const f32 float_neg0p5_8041d7cc;
extern const f32 float_32_8041d7d0;
extern const f32 float_216_8041d7dc;
extern const f32 float_248_8041d7e8;
extern const f32 float_180_8041d7ec;
extern const f32 float_160_8041d7d8;
extern const f32 float_210_8041d7f0;
extern const f32 float_16_8041d7e4;

// NONMATCHING (81.85%): behaviour complete, checked against the asm; Vec copy order and register colouring differ.
void monosiri_disp(CameraId camId, void* param) {
    MonosiriWork* w = param;
    const char* base = str_802F5EB8;
    GXColor c1 = dat_8041d7b4;
    GXColor c2 = dat_8041d7b4;
    f32 x = w->x;
    f32 y = w->y;

    if (w->file1 != NULL) {
        GXTexObj obj;
        Mtx m1;
        Mtx m2;
        Mtx m3;
        u16 tw;
        u16 th;
        s32 i;
        GXColor reg0 = dat_8041d7b8;
        GXColor reg1 = unk_80423050;
        f32 x0;
        f32 y0;
        f32 x1;
        f32 y1;
        const TattleEntry* tattle;
        char buf[0x80];
        f32 wdt;
        f32 tx;
        const char* loc;

        winHalfBookGX((void*)w->file1->data[0], x, y, 0);
        TEXGetGXTexObjFromPalette((TEXPalette*)w->file1->data[0], &obj, 0x37);
        tw = GXGetTexObjWidth(&obj);
        th = GXGetTexObjHeight(&obj);
        TEXGetGXTexObjFromPalette((TEXPalette*)w->file1->data[0], &obj, 0x37);
        GXInitTexObjLOD(&obj, 1, 1, float_0_8041d7bc, float_0_8041d7bc, float_0_8041d7bc, 0, 0, 0);
        GXLoadTexObj(&obj, 0);
        GXInitTexObj(&obj, *(void**)w->alloc, tw, th, 5, 0, 0, 0);
        GXInitTexObjLOD(&obj, 1, 1, float_0_8041d7bc, float_0_8041d7bc, float_0_8041d7bc, 0, 0, 0);
        GXLoadTexObj(&obj, 1);
        GXSetNumTexGens(8);
        for (i = 0; i < 8; i++) {
            f32 s = -((float_0p04_8041d7c4 * (f32)(7 - i)) - float_1_8041d7c0);

            PSMTXTrans(m1, float_0p5_8041d7c8, float_0p5_8041d7c8, float_0_8041d7bc);
            PSMTXScale(m2, s, s, s);
            PSMTXTrans(m3, float_neg0p5_8041d7cc, float_neg0p5_8041d7cc, float_0_8041d7bc);
            PSMTXConcat(m1, m2, m1);
            PSMTXConcat(m1, m3, m1);
            GXLoadTexMtxImm(m1, 0x1E + i * 3, 1);
            GXSetTexCoordGen2(i, 1, 4, 0x1E + i * 3, 0, 0x7D);
        }
        GXSetTevColor(1, reg0);
        reg1.a = (s8)w->alpha;
        GXSetTevColor(2, reg1);
        GXSetNumTevStages(0xC);
        for (i = 0; i < 8; i++) {
            GXSetTevOrder(i, i, 1, 0xFF);
            GXSetTevColorOp(i, 0, 0, 0, 1, 0);
            GXSetTevAlphaOp(i, 0, 0, 0, 1, 0);
            GXSetTevColorIn(i, 0xF, 0xF, 0xF, 2);
            GXSetTevAlphaIn(i, 7, 4, 6, i == 0 ? 7 : 0);
            GXSetTevKAlphaSel(i, 7);
        }
        GXSetTevOrder(8, 7, 1, 0xFF);
        GXSetTevColorOp(8, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(8, 0, 0, 0, 1, 0);
        GXSetTevColorIn(8, 0, 8, 9, 0xF);
        GXSetTevAlphaIn(8, 0, 7, 4, 4);
        GXSetTevOrder(9, 0xFF, 0xFF, 0xFF);
        GXSetTevColorOp(9, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(9, 0, 0, 0, 1, 0);
        GXSetTevColorIn(9, 0xF, 0xF, 0xF, 0);
        GXSetTevAlphaIn(9, 7, 0, 2, 7);
        GXSetTevOrder(0xA, 7, 0, 0xFF);
        GXSetTevColorOp(0xA, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(0xA, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0xA, 0xF, 8, 0, 0xF);
        GXSetTevAlphaIn(0xA, 7, 7, 7, 0);
        GXSetTevOrder(0xB, 7, 0, 0xFF);
        GXSetTevColorOp(0xB, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(0xB, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0xB, 8, 0, 1, 0xF);
        GXSetTevAlphaIn(0xB, 7, 7, 7, 4);
        GXBegin(0x80, 0, 4);
        x0 = float_32_8041d7d0 + x;
        y0 = y - float_8_8041d7d4;
        x1 = x0 + float_216_8041d7dc;
        y1 = y0 - float_160_8041d7d8;
        GXPosition3f32(x0, y0, float_0_8041d7bc);
        GXTexCoord2f32(float_0_8041d7bc, float_0_8041d7bc);
        GXPosition3f32(x0, y1, float_0_8041d7bc);
        GXTexCoord2f32(float_0_8041d7bc, float_1_8041d7c0);
        GXPosition3f32(x1, y1, float_0_8041d7bc);
        GXTexCoord2f32(float_1_8041d7c0, float_1_8041d7c0);
        GXPosition3f32(x1, y0, float_0_8041d7bc);
        GXTexCoord2f32(float_1_8041d7c0, float_0_8041d7bc);
        tattle = battleGetUnitMonosiriPtr(w->unitKind);
        if (w->unitKind == 0x4D || w->unitKind == 0x87) {
            if (evtGetValue(NULL, GSW(0)) < 0xD2) {
                sprintf(buf, RO(base, 0x3D0, str_802F6288), enemy_monoshiri_sort_table[w->unitKind],
                        msgSearch(RO(base, 0x3D8, str_802F6290)));
            } else {
                sprintf(buf, RO(base, 0x3D0, str_802F6288), enemy_monoshiri_sort_table[w->unitKind],
                        msgSearch(RO(base, 0x3E8, str_802F62A0)));
            }
        } else {
            sprintf(buf, RO(base, 0x3D0, str_802F6288), enemy_monoshiri_sort_table[w->unitKind],
                    msgSearch(tattle->enemyTag));
        }
        wdt = float_0p75_8041d7e0 * (f32)FontGetMessageWidth(buf);
        winFontInit();
        tx = float_16_8041d7e4 + x;
        {
            Vec scale = LV(0x3AC, vec3_802f6264);
            Vec pos = logMkVec(LV(0x3A0, vec3_802f6258), (float_248_8041d7e8 - wdt) * float_0p5_8041d7c8 + tx,
                               y - float_180_8041d7ec);

            winFontSet(&pos, &scale, &c1, buf);
        }
        loc = msgSearch(tattle->locationTag);
        sprintf(buf, RO(base, 0x3F8, str_802F62B0), msgSearch(RO(base, 0x400, str_802F62B8)), loc);
        wdt = float_248_8041d7e8 - float_0p75_8041d7e0 * (f32)FontGetMessageWidth(buf);
        {
            Vec scale = LV(0x3C4, vec3_802f627c);
            Vec pos = logMkVec(LV(0x3B8, vec3_802f6270), float_1_8041d7c0 + (wdt * float_0p5_8041d7c8 + tx),
                               y - float_210_8041d7f0);

            winFontSet(&pos, &scale, &c2, buf);
        }
    }
}

extern const f32 float_25_8041d7f4;
extern const f32 float_1p2667_8041d7f8;
extern const f32 float_1000_8041d7fc;
extern const f32 float_608_8041d800;
extern const f32 float_480_8041d804;

extern const Vec vec3_802f6228;
extern const Vec vec3_802f6234;
extern const Vec vec3_802f6240;
extern const Vec vec3_802f624c;

#ifdef MUST_MATCH
#define HALF_FOV_TAN (*(const f64*)((const char*)base + 0x430))
#else
#define HALF_FOV_TAN 0.21816615760326385
#endif

typedef struct TattleCamEntry {
    s32 kind;
    f32 x;
    f32 y;
    f32 scale;
} TattleCamEntry;

// Renders a tattle model into the viewer's texture: frames the model's bounding box, draws the pose (and the swarm
// for the moamoa tattle) and copies the EFB into the viewer's image.
// NONMATCHING (85.95%): behaviour complete, checked against the asm; Vec copy order and register colouring differ.
void capture(CameraId camId, void* param) {
    MonosiriWork* w = param;
    const char* base = str_802F5EB8;
    AnimData* ad;
    Vec mn;
    Vec mx;
    f32 cx;
    f32 cy;
    f32 vp[6];
    f32 proj[7];
    u32 sl;
    u32 st;
    u32 sw;
    u32 sh;
    GXTexObj obj;
    s32 tw;
    s32 th;
    Mtx44 persp;
    Mtx view;
    Mtx tmp;
    Mtx scaleM;
    Mtx identM;
    f32 dist;
    Vec eye;
    Vec up;
    Vec at;
    s32 i;
    s32 n;
    TattleCamEntry* tbl;
    f32 ox;
    f32 oy;
    f32 os;
    CameraEntry saved;
    u16 bl;
    u16 bt;
    u16 br;
    u16 bb;

    ad = animPoseGetAnimDataPtr(w->poseId);
    mn = *(Vec*)((u8*)ad + 0x44);
    mx = *(Vec*)((u8*)ad + 0x50);
    cx = (mx.x + mn.x) * float_0p5_8041d7c8;
    cy = (mx.y + mn.y) * float_0p5_8041d7c8;
    GXGetViewportv(vp);
    GXGetProjectionv(proj);
    GXGetScissor(&sl, &st, &sw, &sh);
    TEXGetGXTexObjFromPalette((TEXPalette*)w->file1->data[0], &obj, 0x37);
    tw = GXGetTexObjWidth(&obj);
    th = GXGetTexObjHeight(&obj);
    GXSetViewport(float_0_8041d7bc, float_0_8041d7bc, (f32)tw, (f32)th, float_0_8041d7bc, float_1_8041d7c0);
    GXSetScissor(1, 1, tw - 2, th - 2);
    C_MTXPerspective(persp, float_25_8041d7f4, float_1p2667_8041d7f8, float_1_8041d7c0, float_1000_8041d7fc);
    GXSetProjection(persp, 0);
    if ((mx.x - mn.x) / (f32)tw > (mx.y - mn.y) / (f32)th) {
        dist = (mx.x - cx) / (f32)tan(HALF_FOV_TAN);
    } else {
        dist = ((float_608_8041d800 * (mx.y - cy)) / float_480_8041d804) / (f32)tan(HALF_FOV_TAN);
    }
    at = LV(0x388, vec3_802f6240);
    at.x = cx;
    at.y = cy;
    up = LV(0x37C, vec3_802f6234);
    eye = LV(0x370, vec3_802f6228);
    eye.x = cx;
    eye.y = cy;
    eye.z = dist;
    C_MTXLookAt(view, &eye, &up, &at);
    {
        tbl = (TattleCamEntry*)w->file2->data[0];
        n = (u32)w->file2->data[1] >> 4;
        for (i = 0; i < n; i++) {
            if (tbl[i].kind == w->unitKind) {
                break;
            }
        }
        if (i < n) {
            ox = tbl[i].x;
            oy = tbl[i].y;
            os = tbl[i].scale;
        } else {
            ox = float_0_8041d7bc;
            os = float_1_8041d7c0;
            oy = ox;
        }
    }
    PSMTXTrans(tmp, ox, oy, float_0_8041d7bc);
    PSMTXScale(scaleM, os, os, os);
    PSMTXConcat(tmp, scaleM, tmp);
    PSMTXConcat(view, tmp, view);
    sysWaitDrawSync();
    GXClearBoundingBox();
    saved = *camGetPtr(camId);
    PSMTXCopy(view, camGetCurPtr()->view);
    if (animPoseGetVivianType(w->poseId) != 0) {
        Vec v = LV(0x394, vec3_802f624c);

        animPoseVivianMain(w->poseId, &v);
    }
    animPoseSetMaterialFlagOn(w->poseId, 0x1800);
    animPosePeraOff(w->poseId);
    PSMTXIdentity(identM);
    animPoseDrawMtx(w->poseId, identM, 1, float_0_8041d7bc, float_1_8041d7c0);
    animPoseDrawMtx(w->poseId, identM, 2, float_0_8041d7bc, float_1_8041d7c0);
    animPoseDrawMtx(w->poseId, identM, 3, float_0_8041d7bc, float_1_8041d7c0);
    animPosePeraOn(w->poseId);
    if (w->unitKind == 0x6B) {
        Vec* verts = (Vec*)((smartEntry*)*(void**)((u8*)animPoseGetAnimPosePtr(w->poseId) + 0x15C))->address;
        s32* counts = (s32*)((smartEntry*)smartAlloc(dat_80419a28 * 4, 3))->address;
        s32* order = (s32*)((smartEntry*)smartAlloc(0x7D0, 3))->address;
        s32 j;

        memset(counts, 0, dat_80419a28 * 4);
        for (j = 0; j < 0x1F4; j++) {
            s32 k = j % dat_80419a28;

            order[j] = counts[k]++;
        }
        animPoseSetMaterialFlagOn(w->poseId2, 0x1800);
        animPosePeraOff(w->poseId2);
        for (j = 0; j < 0x1F4; j++) {
            s32 k = j % dat_80419a28;
            f32 t = (f32)order[j] / (f32)counts[k];
            Vec* a = &verts[animPoseGetGroupIdx(w->poseId, g_moamoa_chain_tbl[k].name)];
            Vec* b = &verts[animPoseGetGroupIdx(w->poseId, g_moamoa_chain_tbl[k].anim)];
            Vec p;
            Vec q;
            f32 jx;
            f32 jy;
            f32 jz;
            Mtx m;

            PSVECScale(a, &p, t);
            PSVECScale(b, &q, float_1_8041d7c0 - t);
            PSVECAdd(&p, &q, &p);
            jz = (f32)(5 - irand(10));
            jy = (f32)(5 - irand(10));
            jx = (f32)(5 - irand(10));
            PSMTXTrans(m, p.x + jx, p.y + jy, p.z + jz);
            animPoseDrawMtx(w->poseId2, m, 1, float_0_8041d7bc, float_1_8041d7c0);
            animPoseDrawMtx(w->poseId2, m, 2, float_0_8041d7bc, float_1_8041d7c0);
            animPoseDrawMtx(w->poseId2, m, 3, float_0_8041d7bc, float_1_8041d7c0);
        }
        animPosePeraOn(w->poseId2);
    }
    PSMTXCopy(saved.view, camGetCurPtr()->view);
    sysWaitDrawSync();
    GXReadBoundingBox(&bl, &bt, &br, &bb);
    GXSetTexCopySrc(0, 0, tw, th);
    GXSetTexCopyDst(tw, th, 5, 0);
    GXCopyTex(*(void**)w->alloc, 1);
    GXPixModeSync();
    GXSetViewport(vp[0], vp[1], vp[2], vp[3], vp[4], vp[5]);
    GXSetProjectionv(proj);
    GXSetScissor(sl, st, sw, sh);
}
