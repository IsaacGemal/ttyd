#include <dolphin/types.h>
#include <string.h>
#include <stddef.h>
#include "window/win_log.h"
#include "window/win_root.h"
#include "win/win_main.h"
#include "battle/battle_monosiri.h"
#include "driver/animdrv.h"
#include "driver/effdrv.h"
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
    u16 flags;           // 0x0
    u8 pad_2[2];         // 0x2
    u32 buttonTrg;       // 0x4
    u32 buttonRep;       // 0x8
    u32 dirTrg;          // 0xC
    u32 dirRep;          // 0x10
    u8 pad_14[0x30 - 0x14];
    FileEntry* file;     // 0x30
    u8 pad_34[0x124 - 0x34];
    s32 retState;        // 0x124
    u8 pad_128[0x158 - 0x128];
    f32 curX;            // 0x158
    f32 curY;            // 0x15C
    u8 pad_160[0xD68 - 0x160];
    s32 state;           // 0xD68
    s32 sel;             // 0xD6C
    s32 count;           // 0xD70
    LogRow rows[5];      // 0xD74
    s32 mapSel;          // 0xE14
    u8 pad_E18[4];
    s32 pose;            // 0xE1C
    f32 mapCx;           // 0xE20
    f32 mapCy;           // 0xE24
    u8 pad_E28[0xE30 - 0xE28];
    f32 e30;             // 0xE30
    u8 pad_E34[4];
    f32 mapScrollX;      // 0xE38
    f32 mapScrollY;      // 0xE3C
    s32 stoneNum;        // 0xE40
    u8 pad_E44[0xE60 - 0xE44];
    s32 stoneSel;        // 0xE60
    u8 pad_E64[0xE6C - 0xE64];
    f32 e6c;             // 0xE6C
    u8 pad_E70[4];
    f32 e74;             // 0xE74
    u8 pad_E78[4];
    f32 e7c;             // 0xE7C
    f32 e80;             // 0xE80
    u8 pad_E84[4];
    f32 e88;             // 0xE88
    u8 pad_E8C[4];
    f32 e90;             // 0xE90
    s32 cardCnt;         // 0xE94
    s32 cardSel;         // 0xE98
    s32 cardPage;        // 0xE9C
    u8 pad_EA0[0xEAC - 0xEA0];
    f32 eac;             // 0xEAC
    u8 pad_EB0[4];
    f32 eb4;             // 0xEB4
    s16 cardIcon[100];   // 0xEB8
    u8 pad_F80[4];
    f32 f84;             // 0xF84
    u8 pad_F88[4];
    f32 f8c;             // 0xF8C
    s32 badgeCnt;        // 0xF90
    s32 badgeSel;        // 0xF94
    s32 badgePage;       // 0xF98
    u8 pad_F9C[0xFA8 - 0xF9C];
    f32 fa8;             // 0xFA8
    u8 pad_FAC[4];
    f32 fb0;             // 0xFB0
    s16 badgeIcon[60];   // 0xFB4
    f32 x102c;           // 0x102C
    u8 pad_1030[4];
    f32 x1034;           // 0x1034
    s32 charCnt;         // 0x1038
    u8 pad_103C[4];
    s32 charSel;         // 0x1040
    s32 charPage;        // 0x1044
    u8 pad_1048[8];
    LogChar chars[216];  // 0x1050
    void* mono;          // 0x1200
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

#ifdef MUST_MATCH
#define RO2(base, off, sym) ((const char*)(base) + (off))
#else
#define RO2(base, off, sym) ((const char*)&(sym))
#endif

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
            if ((u32)fileAsyncf(4, NULL, RO2(base, 0x510, str_802F63C8), getMarioStDvdRoot(), tpl) != 0) {
                if (w->file == 0) {
                    MAP_TPL_NAME(tpl, base);
                    w->file = (FileEntry*)fileAllocf(4, RO2(base, 0x510, str_802F63C8), getMarioStDvdRoot(), tpl);
                }
                if (w->pose == -1) {
                    if (w->flags & 0x2000) {
                        const char* anim = "W_1";
                        w->pose = animPoseEntry(RO2(base, 0x51C, str_802F63D4), 0);
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
                        const char* model = RO2(base, 0x524, str_802F63DC);
                        switch (marioGetColor()) {
                        case 0:
                            break;
                        case 1:
                            model = RO2(base, 0x52C, str_802F63E4);
                            break;
                        case 2:
                            model = RO2(base, 0x538, str_802F63F0);
                            break;
                        case 3:
                            model = RO2(base, 0x544, str_802F63FC);
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
                        sprintf(name, RO2(base, 0x550, str_802F6408), tubuDt[bestIdx].id);
                        sprintf(help, RO2(base, 0x564, str_802F641C), tubuDt[bestIdx].id);
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
                winMsgEntry(work, -1, RO2(base, 0x570, str_802F6428), (s32)RO2(base, 0x58C, str_802F6444));
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
                winMsgEntry(work, -2, RO2(base, 0x570, str_802F6428), (s32)RO2(base, 0x58C, str_802F6444));
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
                winMsgEntry(work, -2, RO2(base, 0x570, str_802F6428), (s32)RO2(base, 0x58C, str_802F6444));
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
                winMsgEntry(work, 0, RO2(base, 0x5A8, str_802F6460), 0);
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
