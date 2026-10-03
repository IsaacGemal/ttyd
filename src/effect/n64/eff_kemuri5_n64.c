#include "eff/n64/eff_kemuri5_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>
#include <stdlib.h>

// .rodata constants defined by this unit (order matches the original).
const Vec vec3_802fa918 = { 0.0f, 0.0f, 0.0f };
const char str_802FA924[] = "Kemuri5N64";
const f64 double_to_int_802fa930 = 4503601774854144.0;

// .sdata2 float constants (named after their original addresses).
extern const f32 float_12_8041f368;
extern const f32 float_6p2832_8041f36c;
extern const f32 float_360_8041f370;
extern const f32 float_0p1_8041f374;
extern const f32 float_0p83_8041f378;
extern const f32 float_0p006_8041f37c;
extern const f32 float_7p5_8041f380;
extern const f32 float_0p98_8041f384;
extern const f32 float_0p9_8041f388;
extern const f32 float_0p0625_8041f35c;
extern const f32 float_0_8041f360;
extern const f32 float_deg2rad_8041f364;
extern const f32 float_1_8041f38c;
extern const f32 float_0p03_8041f390;
extern const f32 float_1p7_8041f394;
extern const f32 float_0p5_8041f398;
extern const f32 float_neg0p02_8041f39c;
extern const f32 float_5Eneg05_8041f3a0;
extern const f32 float_neg3p9_8041f3a4;

// .sdata2 GXColor: r=0x70, g=0x60, b=0x18, a=0x00.
extern const volatile u32 dat_8041f358;

// .data quad vertex array for effSetVtxDescN64 (kmr_a_1_v).
static u32 kmr_a_1_v[14] = {
    0xFFF80007, 0x00000000, 0x0000FFFF, 0xFFFF0007,
    0x00070000, 0x02000000, 0xFFFFFFFF, 0xFFF8FFF8,
    0x00000000, 0x0200FFFF, 0xFFFF0007, 0xFFF80000,
    0x02000200, 0xFFFFFFFF,
};

typedef struct effKemuri5Data {
    s32 field_0x0;   // 0x0
    s16 field_0x4;   // 0x4
    s16 field_0x6;   // 0x6
    u8 field_0x8;    // 0x8
    u8 pad_9[3];     // 0x9
    Vec position;    // 0xC
    f32 field_0x18;  // 0x18
    f32 field_0x1C;  // 0x1C
    f32 field_0x20;  // 0x20
    f32 field_0x24;  // 0x24
    f32 field_0x28;  // 0x28
    f32 field_0x2C;  // 0x2C
    f32 field_0x30;  // 0x30
    f32 field_0x34;  // 0x34
    f32 field_0x38;  // 0x38
    f32 field_0x3C;  // 0x3C
    f32 field_0x40;  // 0x40
    f32 field_0x44;  // 0x44
    f32 field_0x48;  // 0x48
} effKemuri5Data;

// Local copy of system.c's inline helper so it inlines here (as in eff_water_damage_n64.c).
// The compound literal resolves to the original's zero Vec (eff_water_damage_n64.c does the same).
static inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

// Every instruction and every GPR matches; only the callee-saved FPR colouring differs. The target puts
// the four arguments in f23-f26, the four reset constants in f17-f20, 6p2832/360 in f21/f22 and the
// early loop-invariant constants in f27-f31 (rad in f16). MWCC treats the c34..c30 locals as user locals,
// so it colours them above the parameters (ours: args f17-f20, 6p2832/360 f21/f22, reset f23-f26, early
// f27-f31); the target instead needs the reset values coloured *below* the arguments, as compiler
// temporaries. The declaration order of the MUST_MATCH locals sets their colour order (the last declared
// gets the lowest register): declaring c360 then ct last makes 6p2832/360 land in f21/f22 as in the
// target, but the args/reset blocks stay swapped. The int->float magic also emits an anonymous @N pool
// entry instead of the named double_to_int_802fa930; objdiff does not score anonymous pool entries (see
// win_log.c), so that line is cosmetic.
// NONMATCHING (99.59%): only the callee-saved FPR colouring differs (the four reset constants are f23-f26
// instead of f17-f20, and the arguments f17-f20 instead of f23-f26; the rest match). Tried and rejected
// (all <= 99.59%): the direct-store form (99.04, the reset temps land in f28-f31), an inline 4-argument
// setter / pointer / array / struct reset forms (99.09, one FPR reused or memory), reversing and
// interleaving the reset/ct declarations and stores, moving the reset assignments to the top of the loop
// or before it (98.83), param copies (MWCC always coalesces them with the incoming args), local
// `register ... asm("fNN")` pins (ignored by mwcceppc 2.6), and the opt_* pragmas (no change or worse).
// Also tried (DeepSeek V4.1 Flash): all 64 explicit-local/direct subsets of the six reset+ct locals and
// every literal/named mix (the args never go above f19-f22; the reset never reaches f17-f20), the
// optimization_level/optimize_for_size pragmas, statement reordering, and loop-body declarations; the
// 16 mismatched instructions are always exactly the args (f23-f26) and reset (f17-f20) block swap.
// A 15 minute decomp-permuter run on this function did not beat its base score.
// Deeper notes: the shipped reference object was built with
// GC/1.3.2, not the project's GC/2.6 (its .comment is CodeWarrior 02040201, ours is 02040701), so
// this is a genuine 1.3.2-vs-2.6 allocator difference. Recompiling our source under GC/1.3, 1.3.2,
// 2.0, 2.5 and 2.6 all give the identical wrong colouring, so the version alone does not fix it.
// A K&R-style definition (params declared in the body) reproduces the *exact* target colouring
// (args f23-f26, reset f17-f20, 6p2832/360 f21/f22) but MWCC then emits four `frsp` for the old-style
// default argument promotion instead of `fmr`, and rejects the definition outright when the header
// prototype is in scope; `#pragma ignore_oldstyle`/`require_prototypes` do not remove the promotion,
// and `short double`/`double` params avoid `frsp` but promote the arithmetic to double. The target's
// preheader also loads its hoisted constants in first-use order (early temps f27-f31, then reset
// f17-f20, then 6p2832/360 f21/f22), whereas we hoist the explicit reset assignments first; that
// reordering does not move the arg/reset colour blocks either. Third pass: `tools/diff_shape.py`
// confirms calls/immediates match (pure register-allocation residue), K&R emits `frsp` on every
// installed compiler (1.0-3.0a5), file-scope `register f32 x asm("fNN")` pins emit `fmr` but are not
// saved in the prologue, and no flag (`-O4,s`, nodefer, `-inline off`, `-fp_contract off`), opt_* pragma,
// local/param qualifier, dummy extra parameter, declaration/statement position or parameter rename
// changes the swap. Queue for the permuter (same class as eff_magic3_n64 / eff_kumokumo_n64 / eff_bomb).
// Fourth pass: inlining the rad tail moves the 6p2832/360 temps before the params (f19-f22) but the
// reset still colours after them; siblings effSweat/effKemuri9/effConfetti all keep params at the
// bottom of their callee-saved range, so the target's params-above-locals order has no reusable idiom.
// Fifth pass (DeepSeek V4.1 Flash): the local form is the best (16 instructions). The direct-literal
// form is 99.04% (params f19-f22, 6p2832/360 f17/f18, early f23-f27, reset f28-f31); a
// local/literal x opt_lifetimes/opt_propagation/opt_common_subs matrix, declaration/assignment-order
// and -position sweeps, parameter qualifiers (const/register/volatile) and inline getter/setter
// forms are all equal or worse. An anonymous or named struct of the reset values, a MATCH_F32
// volatile read of them, and `#pragma ignore_oldstyle` + a K&R definition each send mwcceppc 2.6
// into an endless loop (do not retry). A K&R definition with the header prototype hidden gives
// params f23-f26 but leaves reset f19-f22 and 6p2832/360 f17/f18 plus the four old-style-promotion
// `frsp`, and the header must keep its prototype for evt_eff.c, so it is not usable.
// Sixth pass (DeepSeek V4.1 Flash): the retail object's .comment says compiler core 2.4.2.1, not
// 2.6 (our GC/2.6 is 2.4.7.1); the exact core is available as GC/1.3.2. Recompiling this source
// under GC/1.3.2 gives the same 16-instruction colouring, so the difference is the source, not the
// compiler. A fast byte-diff harness against the retail object was used to search, under GC/1.3.2:
// all 64 local/literal subsets of the six reset+ct values, ~600 random declaration/assignment
// orders and positions, all 24 store orders, `auto`/`register`/`volatile` locals, file-scope vs
// function-scope vs loop-scope declarations, and a pragma matrix (opt_lifetimes, opt_propagation,
// opt_common_subs, opt_loop_invariants, opt_deadcode, opt_dead_assignments, opt_strength_reduction,
// peephole, scheduling, optimize_for_size, pool_data, explicit_zero_data). None beats 16 diffs, so
// this remains permuter-only register colouring.
// Seventh pass (DeepSeek V4.1 Flash): every compiler that compiles the source to the right
// instruction shape (GC 1.3, 1.3.2, 1.3.2r = core 2.4.2.1, 2.0, 2.5, 2.6, 2.7) yields the same 36
// word-diffs, so no available compiler reproduces the retail allocation. All hooks in the loop are
// fixed by the target asm (the target evaluates `reviseAngle(a4 + (f32)(i*45))` into f1 directly,
// no shared angle local). The effKemuri3 idiom (one local reused for the int->float angle and the
// rad) was transplanted (`rad = a4 + (f32)(i*45); rad = ct*reviseAngle(rad)/c360;`) and does not
// change the colouring here (43 diffs). Also tried: declaration position (no effect), init at
// function top / after the callback, inline setter function (49), all 720 reset/ct assignment
// orders (canonical wins), and all 64 reset/ct local-vs-literal subsets (best 36). A normalised text
// diff (register names -> f#/r#) shows the emitted Entry stream is instruction-for-instruction
// identical to the target apart from the register numbers, so the seam is a pure allocator tie-break
// on an identical IR and has resisted every source-level lever.
EffectEntry* effKemuri5N64Entry(f32 x, f32 y, f32 z, f32 a4) {
    EffectEntry* effect;
    effKemuri5Data* data;
    f32 rad;
    s32 i;
#ifdef MUST_MATCH
    f32 c34;
    f32 c38;
    f32 c3C;
    f32 c30;
    f32 c360;
    f32 ct;
#endif

    effect = effEntry();
    effect->type = str_802FA924;
    effect->count = 8;
    data = __memAlloc(HEAP_EFFECT, sizeof(effKemuri5Data) * 8);
    effect->userdata = data;
    effect->callback = effKemuri5Main;

    for (i = 0; i < 8; i++, data++) {
        data->field_0x0 = 1;
        data->field_0x40 = float_0_8041f360;
        data->position.x = x;
        data->position.y = y;
        data->position.z = z;
        data->field_0x18 = float_1_8041f38c;
        data->field_0x1C = float_1_8041f38c;
        data->field_0x20 = float_1_8041f38c;
        data->field_0x8 = 0xFF;
        data->field_0x24 = float_0p03_8041f390 * (f32)(rand() % 11) + float_1_8041f38c;
        data->field_0x28 = float_0p03_8041f390 * (f32)(rand() % 11) + float_1p7_8041f394;
#ifdef MUST_MATCH
        c34 = float_0p5_8041f398;
        c38 = float_neg0p02_8041f39c;
        c3C = float_5Eneg05_8041f3a0;
        c30 = float_neg3p9_8041f3a4;
        ct = float_6p2832_8041f36c;
        c360 = float_360_8041f370;
#endif
        data->field_0x2C = (f32)(rand() % 61);
        data->field_0x6 = 0x1E;
#ifdef MUST_MATCH
        data->field_0x34 = c34;
        data->field_0x38 = c38;
        data->field_0x3C = c3C;
        data->field_0x30 = c30;
#else
        data->field_0x34 = float_0p5_8041f398;
        data->field_0x38 = float_neg0p02_8041f39c;
        data->field_0x3C = float_5Eneg05_8041f3a0;
        data->field_0x30 = float_neg3p9_8041f3a4;
#endif
#ifdef MUST_MATCH
        rad = ct * reviseAngle(a4 + (f32)(i * 45)) / c360;
#else
        rad = float_6p2832_8041f36c * reviseAngle(a4 + (f32)(i * 45)) / float_360_8041f370;
#endif
        data->field_0x44 = sin(rad);
        data->field_0x48 = cos(rad);
    }

    return effect;
}

// Without this the cos() result colours to f2 instead of the original's f3, swapping f2/f3 through
// the rest of the loop; the pragma is scoped to this function only.
#pragma opt_common_subs off
void effKemuri5Main(EffectEntry* effect) {
    effKemuri5Data* data = effect->userdata;
    Vec position;
    s32 i;
    s32 any;
    f32 v;

    position = initializeVec(&data->position);
    any = 0;
    for (i = 0; i < effect->count; i++, data++) {
        if (data->field_0x0 == 0) {
            continue;
        }
        if (--data->field_0x6 <= 0) {
            data->field_0x0 = 0;
            continue;
        }
        any = 1;
        data->field_0x2C = reviseAngle(float_12_8041f368 + data->field_0x2C);
        v = (f32)sin(float_6p2832_8041f36c * data->field_0x2C / float_360_8041f370);
        data->field_0x18 = float_0p1_8041f374 * v + data->field_0x24;
        v = (f32)cos(float_6p2832_8041f36c * data->field_0x2C / float_360_8041f370);
        data->field_0x1C = float_0p1_8041f374 * v + data->field_0x28;
        data->field_0x30 *= float_0p83_8041f378;
        data->position.x += data->field_0x30 * data->field_0x44;
        data->position.z += data->field_0x30 * data->field_0x48;
        data->field_0x38 += data->field_0x3C;
        data->field_0x34 += data->field_0x38;
        data->position.y += data->field_0x34;
        data->field_0x24 = float_0p006_8041f37c * (float_7p5_8041f380 - data->field_0x24) + data->field_0x24;
        data->field_0x28 *= float_0p98_8041f384;
        if (data->field_0x6 < 10) {
            data->field_0x28 *= float_0p9_8041f388;
        }
        if (data->field_0x6 < 15) {
            data->field_0x8 -= 0x10;
        }
    }
    if (any == 0) {
        effDelete(effect);
        return;
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKemuri5Disp, effect);
}
#pragma opt_common_subs reset

static inline void kemuri5SetColor(u8 a) {
    GXColor color = *(GXColor*)&dat_8041f358;
    color.a = a;
    GXSetTevColor(1, color);
}

void effKemuri5Disp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    CameraEntry* cam = camGetPtr(camId);
    effKemuri5Data* data = effect->userdata;
    GXTexObj texObj;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    s32 i;

    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    PSMTXScale(scale, float_0p0625_8041f35c, float_0p0625_8041f35c, float_0_8041f360);
    GXLoadTexMtxImm(scale, 0x1E, 1);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 8, 15, 15, 2);
    GXSetTevAlphaIn(0, 7, 4, 1, 7);
    effGetTexObjN64(0x4E, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);
    effSetVtxDescN64((effVtxArray*)kmr_a_1_v);

    for (i = 0; i < effect->count; i++, data++) {
        if (data->field_0x0 == 0) {
            continue;
        }
        kemuri5SetColor(data->field_0x8);
        PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
        PSMTXRotRad(rot, 'y', float_deg2rad_8041f364 * -camGetPtr(CAMERA_3D)->viewYaw);
        PSMTXConcat(trans, rot, trans);
        PSMTXScale(scale, data->field_0x18, data->field_0x1C, data->field_0x20);
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(cam->view, trans, trans);
        GXLoadPosMtxImm(trans, 0);
        GXBegin(0x90, 0, 6);
        tri2(0, 3, 1, 0, 0, 2, 3, 0);
    }
}
