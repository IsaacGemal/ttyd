#include <dolphin/types.h>
#include <string.h>
#include "window/win_log.h"
#include "window/win_root.h"
#include "win/win_main.h"
#include "battle/battle_monosiri.h"
#include "driver/animdrv.h"
#include "driver/effdrv.h"
#include "driver/swdrv.h"
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


// NONMATCHING (87.44%): the language table indexed by gp->language is not part of
// the str_802F5EB8 base in our object (the original addresses it as
// str_802F5EB8+0x350), and register allocation of work/inBattle/gp differs.
void monosiriMain(void* work) {
    MonosiriWork* w = work;
    TattleEntry* tattle;
    const char* lang[2] = {str_8041D7AC, str_8041D7B0};
    s32 inBattle = (gp->inBattle != 0);
    GXTexObj texObj;
    s32 height;
    s32 size;

    if (w == NULL) {
        return;
    }

    switch (w->state) {
    case 0:
        if (fileAsyncf(4, NULL, RO(str_802F5EB8, 0x438, str_802F62F0), getMarioStDvdRoot(), lang[gp->language]) != 0) {
            if (fileAsyncf(0, NULL, RO(str_802F5EB8, 0x448, str_802F6300), getMarioStDvdRoot()) != 0) {
                w->file1 = fileAllocf(4, RO(str_802F5EB8, 0x438, str_802F62F0), getMarioStDvdRoot(), lang[gp->language]);
                w->file2 = fileAllocf(0, RO(str_802F5EB8, 0x448, str_802F6300), getMarioStDvdRoot());
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
            if (w->unitKind != 0x6B || animGroupBaseAsync(RO(str_802F5EB8, 0x45C, str_802F6314), inBattle, NULL)) {
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
                        w->poseId2 = animPoseEntry(RO(str_802F5EB8, 0x45C, str_802F6314), inBattle);
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

// NONMATCHING (85.77%): the int->float conversion below references an anonymous
// compiler literal (@222) where the original uses the named global
// double_to_int_802f62d8; the trailing `add r5, work, idx` also emits its
// operands in the other order. Everything else (frame, stores, call) matches.
void winLogInit2(WindowWork* work) {
    f32 v;
    s32 a = WW_S32(work, 0xD70);
    s32 b = WW_S32(work, 0xD6C);
    const char* name;

    WW_F32(work, 0x158) = float_neg128_8041d8dc;
    WW_F32(work, 0x15C) = float_8_8041d7d4 + (float_45_8041d880 * (f32)a * float_0p5_8041d7c8 - float_45_8041d880 * (f32)b);
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
    name = WW_PTR(work, 0xD90 + WW_S32(work, 0xD6C) * 0x20);
    winMsgEntry(work, 0, name, 0);
}
