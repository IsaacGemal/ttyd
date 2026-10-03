#include "window/win_mario.h"
#include "window/win_root.h"
#include "driver/animdrv.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "driver/msgdrv.h"
#include "manager/fontmgr.h"
#include "pmario_sound.h"
#include "battle/battle_seq_end.h"
#include "data/item_icon_view.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <string.h>
#include <stdio.h>

// .sdata2/.sdata objects this unit reads but does not define
#define EXT_F32(name) extern const f32 name
EXT_F32(float_265_8041d60c);
EXT_F32(float_154_8041d610);
EXT_F32(float_190_8041d614);
EXT_F32(float_24_8041d618);
EXT_F32(float_152_8041d620);
EXT_F32(float_0p8_8041d624);
EXT_F32(float_20_8041d628);
EXT_F32(float_142_8041d62c);
EXT_F32(float_0_8041d634);
EXT_F32(float_255_8041d640);
EXT_F32(float_112_8041d644);
EXT_F32(float_240_8041d648);
EXT_F32(float_124_8041d64c);
EXT_F32(float_270_8041d650);
EXT_F32(float_264_8041d654);
EXT_F32(float_110_8041d658);
EXT_F32(float_50_8041d65c);
EXT_F32(float_2_8041d660);
EXT_F32(float_0p5_8041d664);
EXT_F32(float_38_8041d668);
EXT_F32(float_4_8041d66c);
EXT_F32(float_21_8041d674);
EXT_F32(float_34_8041d678);
EXT_F32(float_68_8041d67c);
EXT_F32(float_130_8041d680);
EXT_F32(float_200_8041d684);
EXT_F32(float_260_8041d688);
EXT_F32(float_80_8041d68c);
EXT_F32(float_40_8041d690);
EXT_F32(float_106_8041d694);
EXT_F32(float_70_8041d698);
EXT_F32(float_150_8041d69c);
EXT_F32(float_28_8041d6a0);
EXT_F32(float_56_8041d6a4);
EXT_F32(float_84_8041d6a8);
EXT_F32(float_35_8041d6ac);
EXT_F32(float_8_8041d6b0);
EXT_F32(float_205_8041d6b4);
EXT_F32(float_172_8041d6b8);
EXT_F32(float_16_8041d6bc);
EXT_F32(float_220_8041d6c0);
EXT_F32(float_0_8041d6c4);
EXT_F32(float_1_8041d6c8);
EXT_F32(float_5_8041d6cc);
EXT_F32(float_160_8041d6d0);
EXT_F32(float_254_8041d6d4);
EXT_F32(float_90_8041d6d8);
EXT_F32(float_228_8041d6e4);
EXT_F32(float_17_8041d6e8);
EXT_F32(float_230_8041d6ec);
EXT_F32(float_72_8041d6f0);
EXT_F32(float_6_8041d6f4);
EXT_F32(float_210_8041d6f8);
EXT_F32(float_30_8041d6fc);
EXT_F32(float_60_8041d700);
#undef EXT_F32

// WindowWork only declares its first 0x40 bytes in the header; the fields this
// unit uses past that point are reached through raw offsets, like win_root.c.
#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))
#define WW_PTR(w, off) (*(void**)((u8*)(w) + (off)))

// .bss. The original's symbols are compiler-tagged (`tmp2$708`); the `$` cannot
// be spelled in C, so the name here is cosmetic and the address is what matters.
char unk_803dfbf8[0x100];

// .rodata / .sdata2 objects this unit reads but does not define yet
extern const char str_8041D578[];
const f64 double_to_int_802f5e78 = 4503601774854144.0;

// .data table: one entry per window "link" (see linkDt in win_mario.s)
typedef struct LinkDt {
    s16 field_0;          // 0x0
    s16 field_2;          // 0x2
    const char* field_4;  // 0x4
    const char* field_8;  // 0x8
    const char* field_C;  // 0xC
    u16 field_10;         // 0x10
    u16 field_12;         // 0x12
    const char* field_14; // 0x14
    u8 field_18[4];       // 0x18, four 1-byte "next" indices
} LinkDt;

typedef struct SuperActionEntry {
    const char* field_0;   // 0x0, label tag
    u8 field_4[0xC - 4];   // 0x4
    const char* field_C;   // 0xC
    u8 field_10[2];        // 0x10
    u8 field_12;           // 0x12, star power level
} SuperActionEntry;

extern LinkDt linkDt[];
extern SuperActionEntry* superActionTable[];
extern const f32 float_neg280_8041d72c;
extern const f32 float_120_8041d730;
extern const f32 float_26_8041d734;
extern const f32 float_0p89_8041d728;
// .sdata2 anim names shared with winMarioInit
const char str_8041D578[6] = "M_S_1";
const char str_8041D580[4] = "S_1";
const char str_8041D58C[4] = "R_1";
const char str_8041D704[4] = "S_2";
const char str_8041D708[4] = "S_3";
const char str_8041D70C[4] = "S_4";
const char str_8041D710[5] = "R_15";
const char str_8041D718[5] = "R_17";
const char str_8041D720[5] = "R_18";

// .sdata lookup tables (window "help" message ids per upgrade level)
const char* hammer_help[4] = { NULL, "msg_hammer_1", "msg_hammer_2", "msg_hammer_3" };
const char* boots_help[4] = { NULL, "msg_boots_1", "msg_boots_2", "msg_boots_3" };

char* unk_8016af5c(s32 num, s32 width) {
    signed char buf[0x100];
    const char* tbl[10] = {
        "\x82\x4F", "\x82\x50", "\x82\x51", "\x82\x52", "\x82\x53",
        "\x82\x54", "\x82\x55", "\x82\x56", "\x82\x57", "\x82\x58",
    };
    s32 i;

    if (num < 0) {
        strcpy(unk_803dfbf8, "-");
        num = -num;
    } else {
        strcpy(unk_803dfbf8, "");
    }
    sprintf((char*)buf, "%d", num);
    for (i = 0; i < width; i++) {
        if (i < width - strlen((char*)buf)) {
            strcat(unk_803dfbf8, "\x82\x4F");
        } else {
            strcat(unk_803dfbf8, tbl[buf[i - (width - (s32)strlen((char*)buf))] - 0x30]);
        }
    }
    return unk_803dfbf8;
}

char* winZenkakuStr(s32 num) {
    signed char buf[0x100];
    const char* tbl[10] = {
        "\x82\x4F", "\x82\x50", "\x82\x51", "\x82\x52", "\x82\x53",
        "\x82\x54", "\x82\x55", "\x82\x56", "\x82\x57", "\x82\x58",
    };
    static char tmp2[0x100];
    u32 i;

    if (num < 0) {
        strcpy(tmp2, "-");
        num = -num;
    } else {
        strcpy(tmp2, "");
    }
    sprintf((char*)buf, "%d", num);
    for (i = 0; i < strlen((char*)buf); i++) {
        strcat(tmp2, tbl[buf[i] - 0x30]);
    }
    return tmp2;
}

void winMarioMain2(WindowWork* work) {
    (void)work;
}

// The tail walks its own link pointer (`d2`) so MWCC colours it in r30 and the tail
// `anim` in r31, as the target does; the early name is held in `d2` as well (the
// match-only cast is guarded by MUST_MATCH below). The int->float magic still
// pools as a local @NNN@ rather than double_to_int_802f5e78; that symbol name does
// not affect the match score.
s32 winMarioMain(WindowWork* work) {
    LinkDt* d;
    const char* anim;
    LinkDt* d2;
    f32 f2;

    switch (WW_S32(work, 0x190)) {
    case 0:
        if (work->buttonTrg & 0x100) {
            if (WW_S32(work, 0x160) == 0xc) {
                WW_S32(work, 0x190) = 1;
                WW_S32(work, 0x194) = 0;
                psndSFXOn((const char*)0x20012);
            }
            goto tail;
        }
#ifdef MUST_MATCH
        // Match shape: the early name must land in the tail pointer's register
        // (r30, `d2`); the plain `anim` form under #else scores 99.90%.
#define SET_ANIM(s) (d2 = (LinkDt*)(s))
#else
#define SET_ANIM(s) (anim = (s))
#endif
        if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            if (work->flags & 0x2000) {
                if (strcmp(str_8041D580, str_8041D580) == 0) {
                    switch (marioGetColor()) {
                    case 0:
                        SET_ANIM(str_8041D580);
                        break;
                    case 1:
                        SET_ANIM(str_8041D704);
                        break;
                    case 2:
                        SET_ANIM(str_8041D708);
                        break;
                    case 3:
                        SET_ANIM(str_8041D70C);
                        break;
                    }
                } else {
                    switch (marioGetColor()) {
                    case 0:
                        SET_ANIM(str_8041D58C);
                        break;
                    case 1:
                        SET_ANIM(str_8041D710);
                        break;
                    case 2:
                        SET_ANIM(str_8041D718);
                        break;
                    case 3:
                        SET_ANIM(str_8041D720);
                        break;
                    }
                }
            } else {
                SET_ANIM(str_8041D578);
            }
#ifdef MUST_MATCH
            animPoseSetAnim(WW_S32(work, 0x188), (const char*)d2, 0);
#else
            animPoseSetAnim(WW_S32(work, 0x188), anim, 0);
#endif
#undef SET_ANIM
            return -1;
        }
        if (work->buttonTrg & 0x1000) {
            return -2;
        }

#define MARIO_TRAVERSE(off)                                                                          \
    do {                                                                                             \
        d = &linkDt[WW_S32(work, 0x160)];                                                            \
        WW_S32(work, 0x160) = d->field_18[off];                                                      \
        d = &linkDt[WW_S32(work, 0x160)];                                                            \
        while (1) {                                                                                  \
            if (WW_S32(work, 0x160) == 0xb || WW_S32(work, 0x160) == 0xc) {                           \
                if (WW_S32(work, 0x198) != 0) {                                                      \
                    break;                                                                           \
                }                                                                                    \
            } else {                                                                                 \
                if (d->field_10 == 0) {                                                              \
                    break;                                                                           \
                }                                                                                    \
                if ((s32)pouchCheckItem(d->field_10) != 0) {                                         \
                    break;                                                                           \
                }                                                                                    \
            }                                                                                        \
            WW_S32(work, 0x160) = d->field_18[off];                                                  \
            d = &linkDt[WW_S32(work, 0x160)];                                                        \
        }                                                                                            \
    } while (0)

        if (work->dirRep & 0x1000) {
            MARIO_TRAVERSE(0);
            psndSFXOn((const char*)0x20005);
            goto tail;
        }
        if (work->dirRep & 0x2000) {
            MARIO_TRAVERSE(1);
            psndSFXOn((const char*)0x20005);
            goto tail;
        }
        if (work->dirRep & 0x4000) {
            MARIO_TRAVERSE(2);
            psndSFXOn((const char*)0x20005);
            goto tail;
        }
        if (work->dirRep & 0x8000) {
            MARIO_TRAVERSE(3);
            psndSFXOn((const char*)0x20005);
        }

#undef MARIO_TRAVERSE

    tail:
        d2 = &linkDt[WW_S32(work, 0x160)];
        WW_F32(work, 0x158) = (f32)d2->field_0;
        WW_F32(work, 0x15C) = (f32)d2->field_2;

        switch (WW_S32(work, 0x160)) {
        case 2:
            if (work->flags & 0x2000) {
                if (strcmp(d2->field_C, str_8041D580) == 0) {
                    switch (marioGetColor()) {
                    case 0:
                        anim = str_8041D580;
                        break;
                    case 1:
                        anim = str_8041D704;
                        break;
                    case 2:
                        anim = str_8041D708;
                        break;
                    case 3:
                        anim = str_8041D70C;
                        break;
                    }
                } else {
                    switch (marioGetColor()) {
                    case 0:
                        anim = str_8041D58C;
                        break;
                    case 1:
                        anim = str_8041D710;
                        break;
                    case 2:
                        anim = str_8041D718;
                        break;
                    case 3:
                        anim = str_8041D720;
                        break;
                    }
                }
            } else {
                anim = hammer_pose[pouchGetHammerLv()];
            }

            animPoseSetAnim(WW_S32(work, 0x188), anim, 0);
            if (WW_S32(work, 0x18C) == 0) {
                if (animPoseGetLoopTimes(WW_S32(work, 0x188)) >= float_0p89_8041d728) {
                    WW_S32(work, 0x18C) = 30;
                }
            } else {
                WW_S32(work, 0x18C) -= 1;
            }
            if (work->flags & 0x2000) {
                WW_S32(work, 0x18C) = 0;
            }
                    break;
        default:
            WW_S32(work, 0x18C) = 0;
            if (work->flags & 0x2000) {
                if (strcmp(d2->field_C, str_8041D580) == 0) {
                    switch (marioGetColor()) {
                    case 0:
                        anim = str_8041D580;
                        break;
                    case 1:
                        anim = str_8041D704;
                        break;
                    case 2:
                        anim = str_8041D708;
                        break;
                    case 3:
                        anim = str_8041D70C;
                        break;
                    }
                } else {
                    switch (marioGetColor()) {
                    case 0:
                        anim = str_8041D58C;
                        break;
                    case 1:
                        anim = str_8041D710;
                        break;
                    case 2:
                        anim = str_8041D718;
                        break;
                    case 3:
                        anim = str_8041D720;
                        break;
                    }
                }
            } else {
                anim = d2->field_4;
            }
            animPoseSetAnim(WW_S32(work, 0x188), anim, 0);
                    break;
        }

        switch (WW_S32(work, 0x160)) {
        case 2:
            winMsgEntry(work, 0, hammer_help[pouchGetHammerLv()], 0);
            break;
        case 3:
            winMsgEntry(work, 0, boots_help[pouchGetJumpLv()], 0);
            break;
        default:
            winMsgEntry(work, 0, d2->field_14, 0);
            break;
        }
        break;
    case 1:
        if (work->dirRep & 0x1000) {
            WW_S32(work, 0x194) -= 1;
            if (WW_S32(work, 0x194) < 0) {
                WW_S32(work, 0x194) = WW_S32(work, 0x198) - 1;
            }
            psndSFXOn((const char*)0x20005);
        } else if (work->dirRep & 0x2000) {
            WW_S32(work, 0x194) += 1;
            if (WW_S32(work, 0x194) >= WW_S32(work, 0x198)) {
                WW_S32(work, 0x194) = 0;
            }
            psndSFXOn((const char*)0x20005);
        } else if (work->buttonTrg & 0x200) {
            WW_S32(work, 0x190) = 0;
            psndSFXOn((const char*)0x20013);
        } else if (work->buttonTrg & 0x1000) {
            return -2;
        }
        WW_F32(work, 0x158) = float_neg280_8041d72c;
        f2 = (f32)WW_S32(work, 0x194);
        WW_F32(work, 0x15C) = float_10_8041d61c + ((float_120_8041d730 - float_26_8041d734 * f2) - float_12_8041d670);
        winMsgEntry(work, 0, superActionTable[WW_S32(work, 0x194)]->field_C, 0);
        break;
    }
    return 0;
}

void winMarioExit(WindowWork* work) {
    animPoseRelease(WW_S32(work, 0x188));
}

void winMarioInit2(WindowWork* work) {
    f32 x;
    f32 y;

    WW_S32(work, 0x160) = 0;
    x = (f32)linkDt[WW_S32(work, 0x160)].field_0;
    WW_F32(work, 0x158) = x;
    WW_F32(work, 0x158) = x;
    y = (f32)linkDt[WW_S32(work, 0x160)].field_2;
    WW_F32(work, 0x15C) = y;
    WW_F32(work, 0x15C) = y;

    switch (WW_S32(work, 0x160)) {
    case 2:
        winMsgEntry(work, 0, hammer_help[pouchGetHammerLv()], 0);
        break;
    case 3:
        winMsgEntry(work, 0, boots_help[pouchGetJumpLv()], 0);
        break;
    default:
        winMsgEntry(work, 0, linkDt[WW_S32(work, 0x160)].field_14, 0);
        break;
    }

    WW_S32(work, 0x18C) = 0;
    WW_S32(work, 0x190) = 0;
    WW_S32(work, 0x194) = 0;
}


// The five anim-name strings live in .rodata past the unit's msg tables; the
// target reaches them through the r30 rodata base (`addi rN, r30, 0x830..0x858`).
// Named literals pool at the section start instead, so the matching build reads
// them through `vp` (same base winMarioDisp uses) and the natural form under #else.
void winMarioInit(WindowWork* work) {
#ifdef MUST_MATCH
    const u8* vp = (const u8*)&str_802F5650;
#endif
    PouchData* pouch = pouchGetPtr();
    const char* anim;
    s32 i;
#ifdef MUST_MATCH
#define D_MARIO ((const char*)(vp + 0x830))
#define A_MARIO ((const char*)(vp + 0x838))
#define A_MARIO_L ((const char*)(vp + 0x840))
#define A_MARIO_W ((const char*)(vp + 0x84c))
#define A_MARIO_WL ((const char*)(vp + 0x858))
#else
#define D_MARIO "d_mario"
#define A_MARIO "a_mario"
#define A_MARIO_L "a_mario_l"
#define A_MARIO_W "a_mario_w"
#define A_MARIO_WL "a_mario_wl"
#endif

    if (work->flags & 0x2000) {
        anim = str_8041D580;
        switch (marioGetColor()) {
        case 0:
            anim = str_8041D580;
            break;
        case 1:
            anim = str_8041D704;
            break;
        case 2:
            anim = str_8041D708;
            break;
        case 3:
            anim = str_8041D70C;
            break;
        }
        WW_S32(work, 0x188) = animPoseEntry(D_MARIO, 0);
        animPoseSetAnim(WW_S32(work, 0x188), anim, 1);
    } else {
        anim = A_MARIO;
        switch (marioGetColor()) {
        case 0:
            anim = A_MARIO;
            break;
        case 1:
            anim = A_MARIO_L;
            break;
        case 2:
            anim = A_MARIO_W;
            break;
        case 3:
            anim = A_MARIO_WL;
            break;
        }
        WW_S32(work, 0x188) = animPoseEntry(anim, 0);
        animPoseSetAnim(WW_S32(work, 0x188), str_8041D578, 1);
    }
#undef D_MARIO
#undef A_MARIO
#undef A_MARIO_L
#undef A_MARIO_W
#undef A_MARIO_WL

    animPoseSetMaterialFlagOn(WW_S32(work, 0x188), 0x1800);
    if (marioGetPtr()->flags & 0x40000000) {
        animPoseSetMaterialFlagOn(WW_S32(work, 0x188), 0xA000000);
    }

    WW_S32(work, 0x18C) = 0;
    WW_S32(work, 0x190) = 0;
    WW_S32(work, 0x194) = 0;
    WW_S32(work, 0x198) = 0;

    for (i = 0; i < 8; i++) {
        if (pouch->mStarPowersObtained & (1 << i)) {
            WW_S32(work, 0x198) += 1;
        }
    }
}

// The two bubble shapes fukidashi draws: the three texture/icon positions are read from
// the unit's shared rodata vectors through one base (vp), as winMarioDisp does.
#ifdef MUST_MATCH
#define FUKI_VEC(off, sym) (*(Vec*)(vp + (off)))
#else
#define FUKI_VEC(off, sym) sym
#endif
// Match shape: the original calls winTexSet (declared by value) in a way that gives one
// contiguous 0x6c Vec block per switch case. MWCC passes a by-value Vec/GXColor by
// invisible reference (a pointer), so calling the real function through a pointer-parameter
// signature is ABI-identical but lets the caller pass its own locals instead of hidden
// argument copies. That is what produces the original's stack layout. The explicit per-part
// `sc = vec; tmp = vec; tr = tmp;` order plus this cast is what matches.
#ifdef MUST_MATCH
typedef void (*WinTexSetPtr)(u32, Vec*, Vec*, GXColor*);
#define WINTEXSET(id, tr, sc, c) ((WinTexSetPtr)winTexSet)((id), &(tr), &(sc), &(c))
#else
#define WINTEXSET(id, tr, sc, c) winTexSet((id), (tr), (sc), (c))
#endif

void fukidashi(WindowWork* work, u32 type, f32 x, f32 y) {
#ifdef MUST_MATCH
    const u8* vp;
#endif
    u16 iconId;

#ifdef MUST_MATCH
    vp = (const u8*)&str_802F5650;
#endif
    switch (type) {
    case 2:
        if (pouchGetHammerLv() > 0) {
            GXColor c3,c2,c1;
            Vec sc3,tmp3,tr3,sc2,tmp2,tr2,sc1,tmp1,tr1;
            iconId = itemDataTable[pouchGetHammerLv() + 8].icon_id;
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        c1 = dat_8041d5dc;
        sc1 = FUKI_VEC(0x228, vec3_802f5878);
        tmp1 = FUKI_VEC(0x21C, vec3_802f586c);
        tmp1.x = (x - float_230_8041d6ec) + float_5_8041d6cc;
        y = float_72_8041d6f0 + y;
        tmp1.y = y - float_5_8041d6cc;
        tr1 = tmp1;
        WINTEXSET(0xB1, tr1, sc1, c1);
        c2 = dat_8041d5e0;
        sc2 = FUKI_VEC(0x240, vec3_802f5890);
        tmp2 = FUKI_VEC(0x234, vec3_802f5884);
        tmp2.x = x - float_230_8041d6ec;
        tmp2.y = y;
        tr2 = tmp2;
        WINTEXSET(0xB1, tr2, sc2, c2);
        winIconInit();
        c3 = dat_8041d5e4;
        sc3 = FUKI_VEC(0x258, vec3_802f58a8);
        tmp3 = FUKI_VEC(0x24C, vec3_802f589c);
        tmp3.x = (x - float_230_8041d6ec) - float_6_8041d6f4;
        tmp3.y = y;
        tr3 = tmp3;
        winIconSet(iconId, &tr3, &sc3, &c3);
        }
        break;
    case 3:
        if (pouchGetJumpLv() > 0) {
            GXColor c3,c2,c1;
            Vec sc3,tmp3,tr3,sc2,tmp2,tr2,sc1,tmp1,tr1;
            iconId = itemDataTable[pouchGetJumpLv() + 5].icon_id;
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        c1 = dat_8041d5dc;
        sc1 = FUKI_VEC(0x228, vec3_802f5878);
        tmp1 = FUKI_VEC(0x21C, vec3_802f586c);
        tmp1.x = (x - float_210_8041d6f8) + float_5_8041d6cc;
        y = float_20_8041d628 + y;
        tmp1.y = y - float_5_8041d6cc;
        tr1 = tmp1;
        WINTEXSET(0xB1, tr1, sc1, c1);
        c2 = dat_8041d5e0;
        sc2 = FUKI_VEC(0x240, vec3_802f5890);
        tmp2 = FUKI_VEC(0x234, vec3_802f5884);
        tmp2.x = x - float_210_8041d6f8;
        tmp2.y = y;
        tr2 = tmp2;
        WINTEXSET(0xB1, tr2, sc2, c2);
        winIconInit();
        c3 = dat_8041d5e4;
        sc3 = FUKI_VEC(0x258, vec3_802f58a8);
        tmp3 = FUKI_VEC(0x24C, vec3_802f589c);
        tmp3.x = (x - float_210_8041d6f8) - float_6_8041d6f4;
        tmp3.y = y;
        tr3 = tmp3;
        winIconSet(iconId, &tr3, &sc3, &c3);
        }
        break;
    case 7:
        if ((s32)pouchCheckItem(2) != 0) {
            GXColor c3,c2,c1;
            Vec sc3,tmp3,tr3,sc2,tmp2,tr2,sc1,tmp1,tr1;
            iconId = itemDataTable[2].icon_id;
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        c1 = dat_8041d5d0;
        sc1 = FUKI_VEC(0x1E0, vec3_802f5830);
        tmp1 = FUKI_VEC(0x1D4, vec3_802f5824);
        tmp1.x = (x - float_80_8041d68c) + float_5_8041d6cc;
        y = float_90_8041d6d8 + y;
        tmp1.y = y - float_5_8041d6cc;
        tr1 = tmp1;
        WINTEXSET(0xB1, tr1, sc1, c1);
        c2 = dat_8041d5d4;
        sc2 = FUKI_VEC(0x1F8, vec3_802f5848);
        tmp2 = FUKI_VEC(0x1EC, vec3_802f583c);
        tmp2.x = x - float_80_8041d68c;
        tmp2.y = y;
        tr2 = tmp2;
        WINTEXSET(0xB1, tr2, sc2, c2);
        winIconInit();
        c3 = dat_8041d5d8;
        sc3 = FUKI_VEC(0x210, vec3_802f5860);
        tmp3 = FUKI_VEC(0x204, vec3_802f5854);
        tmp3.x = (x - float_80_8041d68c) + float_6_8041d6f4;
        tmp3.y = y;
        tr3 = tmp3;
        winIconSet(iconId, &tr3, &sc3, &c3);
        }
        break;
    case 8:
        if ((s32)pouchCheckItem(4) != 0) {
            GXColor c3,c2,c1;
            Vec sc3,tmp3,tr3,sc2,tmp2,tr2,sc1,tmp1,tr1;
            iconId = itemDataTable[4].icon_id;
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        c1 = dat_8041d5d0;
        sc1 = FUKI_VEC(0x1E0, vec3_802f5830);
        tmp1 = FUKI_VEC(0x1D4, vec3_802f5824);
        tmp1.x = (x - float_80_8041d68c) + float_5_8041d6cc;
        y = float_20_8041d628 + y;
        tmp1.y = y - float_5_8041d6cc;
        tr1 = tmp1;
        WINTEXSET(0xB1, tr1, sc1, c1);
        c2 = dat_8041d5d4;
        sc2 = FUKI_VEC(0x1F8, vec3_802f5848);
        tmp2 = FUKI_VEC(0x1EC, vec3_802f583c);
        tmp2.x = x - float_80_8041d68c;
        tmp2.y = y;
        tr2 = tmp2;
        WINTEXSET(0xB1, tr2, sc2, c2);
        winIconInit();
        c3 = dat_8041d5d8;
        sc3 = FUKI_VEC(0x210, vec3_802f5860);
        tmp3 = FUKI_VEC(0x204, vec3_802f5854);
        tmp3.x = (x - float_80_8041d68c) + float_6_8041d6f4;
        tmp3.y = y;
        tr3 = tmp3;
        winIconSet(iconId, &tr3, &sc3, &c3);
        }
        break;
    case 9:
        if ((s32)pouchCheckItem(5) != 0) {
            GXColor c3,c2,c1;
            Vec sc3,tmp3,tr3,sc2,tmp2,tr2,sc1,tmp1,tr1;
            iconId = itemDataTable[5].icon_id;
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        c1 = dat_8041d5d0;
        sc1 = FUKI_VEC(0x1E0, vec3_802f5830);
        tmp1 = FUKI_VEC(0x1D4, vec3_802f5824);
        tmp1.x = (x - float_30_8041d6fc) + float_5_8041d6cc;
        y = float_130_8041d680 + y;
        tmp1.y = y - float_5_8041d6cc;
        tr1 = tmp1;
        WINTEXSET(0xB1, tr1, sc1, c1);
        c2 = dat_8041d5d4;
        sc2 = FUKI_VEC(0x1F8, vec3_802f5848);
        tmp2 = FUKI_VEC(0x1EC, vec3_802f583c);
        tmp2.x = x - float_30_8041d6fc;
        tmp2.y = y;
        tr2 = tmp2;
        WINTEXSET(0xB1, tr2, sc2, c2);
        winIconInit();
        c3 = dat_8041d5d8;
        sc3 = FUKI_VEC(0x210, vec3_802f5860);
        tmp3 = FUKI_VEC(0x204, vec3_802f5854);
        tmp3.x = (x - float_30_8041d6fc) + float_6_8041d6f4;
        tmp3.y = y;
        tr3 = tmp3;
        winIconSet(iconId, &tr3, &sc3, &c3);
        }
        break;
    case 10:
        if ((s32)pouchCheckItem(3) != 0) {
            GXColor c3,c2,c1;
            Vec sc3,tmp3,tr3,sc2,tmp2,tr2,sc1,tmp1,tr1;
            iconId = itemDataTable[3].icon_id;
            winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        c1 = dat_8041d5d0;
        sc1 = FUKI_VEC(0x1E0, vec3_802f5830);
        tmp1 = FUKI_VEC(0x1D4, vec3_802f5824);
        tmp1.x = (x - float_30_8041d6fc) + float_5_8041d6cc;
        y = float_60_8041d700 + y;
        tmp1.y = y - float_5_8041d6cc;
        tr1 = tmp1;
        WINTEXSET(0xB1, tr1, sc1, c1);
        c2 = dat_8041d5d4;
        sc2 = FUKI_VEC(0x1F8, vec3_802f5848);
        tmp2 = FUKI_VEC(0x1EC, vec3_802f583c);
        tmp2.x = x - float_30_8041d6fc;
        tmp2.y = y;
        tr2 = tmp2;
        WINTEXSET(0xB1, tr2, sc2, c2);
        winIconInit();
        c3 = dat_8041d5d8;
        sc3 = FUKI_VEC(0x210, vec3_802f5860);
        tmp3 = FUKI_VEC(0x204, vec3_802f5854);
        tmp3.x = (x - float_30_8041d6fc) + float_6_8041d6f4;
        tmp3.y = y;
        tr3 = tmp3;
        winIconSet(iconId, &tr3, &sc3, &c3);
        }
        break;
    default:
        break;
    }
}

static s32 pri[6] = { 3, 2, 8, 10, 7, 9 };

#ifdef MUST_MATCH
// The original reaches every rodata constant of winMarioDisp through one base register (vp).
#define VEC_AT(o) (*(Vec*)(vp + (o)))
#define GXC_AT(o) (*(GXColor*)(vp + (o)))
#define STR_AT(o) ((char*)(vp + (o)))
#define WM_RAW_TBL(i) (*(const char**)((u8*)vp + 0x30C + (i) * 4))
#define WM_TAG_TBL(i) (*(const char**)((u8*)vp + 0x360 + (i) * 4))
#define WM_ZENKAKU(num) wmDispZenkaku(vp, 0x184, num)
#define WM_NUM(width, num) wmDispNum(vp, 0x1AC, width, num)
#else
extern const Vec vec3_802f58b4;
extern const Vec vec3_802f58c0;
extern const Vec vec3_802f58cc;
extern const Vec vec3_802f58d8;
extern const Vec vec3_802f58e4;
extern const Vec vec3_802f58f0;
extern const Vec vec3_802f58fc;
extern const Vec vec3_802f5908;
extern const Vec vec3_802f5914;
extern const Vec vec3_802f5920;
extern const Vec vec3_802f592c;
extern const Vec vec3_802f5938;
extern const GXColor dat_802f5944[3];
extern const GXColor dat_802f5950[3];
extern const Vec vec3_802f59bc;
extern const Vec vec3_802f59c8;
extern const Vec vec3_802f59d4;
extern const Vec vec3_802f59e0;
extern const Vec vec3_802f59ec;
extern const Vec vec3_802f59f8;
extern const Vec vec3_802f5a04;
extern const Vec vec3_802f5a10;
extern const Vec vec3_802f5a1c;
extern const Vec vec3_802f5a28;
extern const Vec vec3_802f5a34;
extern const Vec vec3_802f5a40;
extern const Vec vec3_802f5a4c;
extern const Vec vec3_802f5a58;
extern const Vec vec3_802f5a64;
extern const Vec vec3_802f5a70;
extern const Vec vec3_802f5a7c;
extern const Vec vec3_802f5a88;
extern const Vec vec3_802f5a94;
extern const Vec vec3_802f5aa0;
extern const Vec vec3_802f5aac;
extern const Vec vec3_802f5ab8;
extern const Vec vec3_802f5ac4;
extern const Vec vec3_802f5ad0;
extern const Vec vec3_802f5adc;
extern const Vec vec3_802f5ae8;
extern const Vec vec3_802f5af4;
extern const Vec vec3_802f5b00;
extern const Vec vec3_802f5b0c;
extern const Vec vec3_802f5b18;
extern const Vec vec3_802f5b24;
extern const Vec vec3_802f5b30;
extern const Vec vec3_802f5b3c;
extern const Vec vec3_802f5b48;
extern const Vec vec3_802f5b54;
extern const Vec vec3_802f5b60;
extern const Vec vec3_802f5b6c;
extern const Vec vec3_802f5b78;
extern const Vec vec3_802f5b84;
extern const Vec vec3_802f5b90;
extern const Vec vec3_802f5b9c;
extern const Vec vec3_802f5ba8;
extern const Vec vec3_802f5bb4;
extern const Vec vec3_802f5bc0;
extern const Vec vec3_802f5bcc;
extern const Vec vec3_802f5bd8;
extern const Vec vec3_802f5be4;
extern const Vec vec3_802f5bf0;
extern const Vec vec3_802f5bfc;
extern const Vec vec3_802f5c08;
extern const Vec vec3_802f5c14;
extern const Vec vec3_802f5c20;
extern const Vec vec3_802f5c2c;
extern const Vec vec3_802f5c38;
extern const Vec vec3_802f5c44;
extern const Vec vec3_802f5c50;
extern const Vec vec3_802f5c5c;
extern const Vec vec3_802f5c68;
extern const Vec vec3_802f5c74;
extern const Vec vec3_802f5c80;
extern const Vec vec3_802f5c8c;
extern const Vec vec3_802f5c98;
extern const Vec vec3_802f5ca4;
extern const Vec vec3_802f5cb0;
extern const Vec vec3_802f5cbc;
extern const Vec vec3_802f5cc8;
extern const Vec vec3_802f5cd4;
extern const Vec vec3_802f5ce0;
extern const Vec vec3_802f5cec;
extern const Vec vec3_802f5cf8;
extern const Vec vec3_802f5d04;
extern const Vec vec3_802f5d10;
extern const Vec vec3_802f5d1c;
extern const Vec vec3_802f5d28;
extern const Vec vec3_802f5d34;
extern const Vec vec3_802f5d40;
extern const Vec vec3_802f5d4c;
extern const Vec vec3_802f5d58;
extern const Vec vec3_802f5d64;
extern const Vec vec3_802f5d70;
extern const Vec vec3_802f5d7c;
extern const Vec vec3_802f5d88;
extern const char str_802F5D94[];
extern const char str_802F5DA0[];
extern const char str_802F5DB8[];
extern const char str_802F5DD0[];
extern const char str_802F5DE8[];
extern const char str_802F5E00[];
extern const char str_802F5E1C[];
extern const char str_802F5E3C[];
extern const char str_802F5E58[];

#define WMV_0x264 vec3_802f58b4
#define WMV_0x270 vec3_802f58c0
#define WMV_0x27C vec3_802f58cc
#define WMV_0x288 vec3_802f58d8
#define WMV_0x294 vec3_802f58e4
#define WMV_0x2A0 vec3_802f58f0
#define WMV_0x2AC vec3_802f58fc
#define WMV_0x2B8 vec3_802f5908
#define WMV_0x2C4 vec3_802f5914
#define WMV_0x2D0 vec3_802f5920
#define WMV_0x2DC vec3_802f592c
#define WMV_0x2E8 vec3_802f5938
#define WMG_0x2F4 dat_802f5944[0]
#define WMG_0x2F8 dat_802f5944[1]
#define WMG_0x2FC dat_802f5944[2]
#define WMG_0x300 dat_802f5950[0]
#define WMG_0x304 dat_802f5950[1]
#define WMG_0x308 dat_802f5950[2]
#define WMV_0x36C vec3_802f59bc
#define WMV_0x378 vec3_802f59c8
#define WMV_0x384 vec3_802f59d4
#define WMV_0x390 vec3_802f59e0
#define WMV_0x39C vec3_802f59ec
#define WMV_0x3A8 vec3_802f59f8
#define WMV_0x3B4 vec3_802f5a04
#define WMV_0x3C0 vec3_802f5a10
#define WMV_0x3CC vec3_802f5a1c
#define WMV_0x3D8 vec3_802f5a28
#define WMV_0x3E4 vec3_802f5a34
#define WMV_0x3F0 vec3_802f5a40
#define WMV_0x3FC vec3_802f5a4c
#define WMV_0x408 vec3_802f5a58
#define WMV_0x414 vec3_802f5a64
#define WMV_0x420 vec3_802f5a70
#define WMV_0x42C vec3_802f5a7c
#define WMV_0x438 vec3_802f5a88
#define WMV_0x444 vec3_802f5a94
#define WMV_0x450 vec3_802f5aa0
#define WMV_0x45C vec3_802f5aac
#define WMV_0x468 vec3_802f5ab8
#define WMV_0x474 vec3_802f5ac4
#define WMV_0x480 vec3_802f5ad0
#define WMV_0x48C vec3_802f5adc
#define WMV_0x498 vec3_802f5ae8
#define WMV_0x4A4 vec3_802f5af4
#define WMV_0x4B0 vec3_802f5b00
#define WMV_0x4BC vec3_802f5b0c
#define WMV_0x4C8 vec3_802f5b18
#define WMV_0x4D4 vec3_802f5b24
#define WMV_0x4E0 vec3_802f5b30
#define WMV_0x4EC vec3_802f5b3c
#define WMV_0x4F8 vec3_802f5b48
#define WMV_0x504 vec3_802f5b54
#define WMV_0x510 vec3_802f5b60
#define WMV_0x51C vec3_802f5b6c
#define WMV_0x528 vec3_802f5b78
#define WMV_0x534 vec3_802f5b84
#define WMV_0x540 vec3_802f5b90
#define WMV_0x54C vec3_802f5b9c
#define WMV_0x558 vec3_802f5ba8
#define WMV_0x564 vec3_802f5bb4
#define WMV_0x570 vec3_802f5bc0
#define WMV_0x57C vec3_802f5bcc
#define WMV_0x588 vec3_802f5bd8
#define WMV_0x594 vec3_802f5be4
#define WMV_0x5A0 vec3_802f5bf0
#define WMV_0x5AC vec3_802f5bfc
#define WMV_0x5B8 vec3_802f5c08
#define WMV_0x5C4 vec3_802f5c14
#define WMV_0x5D0 vec3_802f5c20
#define WMV_0x5DC vec3_802f5c2c
#define WMV_0x5E8 vec3_802f5c38
#define WMV_0x5F4 vec3_802f5c44
#define WMV_0x600 vec3_802f5c50
#define WMV_0x60C vec3_802f5c5c
#define WMV_0x618 vec3_802f5c68
#define WMV_0x624 vec3_802f5c74
#define WMV_0x630 vec3_802f5c80
#define WMV_0x63C vec3_802f5c8c
#define WMV_0x648 vec3_802f5c98
#define WMV_0x654 vec3_802f5ca4
#define WMV_0x660 vec3_802f5cb0
#define WMV_0x66C vec3_802f5cbc
#define WMV_0x678 vec3_802f5cc8
#define WMV_0x684 vec3_802f5cd4
#define WMV_0x690 vec3_802f5ce0
#define WMV_0x69C vec3_802f5cec
#define WMV_0x6A8 vec3_802f5cf8
#define WMV_0x6B4 vec3_802f5d04
#define WMV_0x6C0 vec3_802f5d10
#define WMV_0x6CC vec3_802f5d1c
#define WMV_0x6D8 vec3_802f5d28
#define WMV_0x6E4 vec3_802f5d34
#define WMV_0x6F0 vec3_802f5d40
#define WMV_0x6FC vec3_802f5d4c
#define WMV_0x708 vec3_802f5d58
#define WMV_0x714 vec3_802f5d64
#define WMV_0x720 vec3_802f5d70
#define WMV_0x72C vec3_802f5d7c
#define WMV_0x738 vec3_802f5d88
#define WMS_0x744 str_802F5D94
#define WMS_0x750 str_802F5DA0
#define WMS_0x768 str_802F5DB8
#define WMS_0x780 str_802F5DD0
#define WMS_0x798 str_802F5DE8
#define WMS_0x7B0 str_802F5E00
#define WMS_0x7CC str_802F5E1C
#define WMS_0x7EC str_802F5E3C
#define WMS_0x808 str_802F5E58
extern const char* const N_stat_name_tbl[3];
extern const char str_802F5968[];
extern const char str_802F5980[];
extern const char str_802F5998[];
#define VEC_AT(o) WMV_##o
#define GXC_AT(o) WMG_##o
#define STR_AT(o) ((char*)WMS_##o)
#define WM_RAW_TBL(i) N_stat_name_tbl[i]
#define WM_TAG_TBL(i) ((i) == 0 ? str_802F5968 : (i) == 1 ? str_802F5980 : str_802F5998)
#define WM_ZENKAKU(num) wmDispZenkaku(num)
#define WM_NUM(width, num) wmDispNum(width, num)
#endif
#define POSE (WW_S32(work, 0x188))

// Same body as winZenkakuStr. Under MUST_MATCH the digit table is read through the panel's
// rodata base (`base + off`) so MWCC folds it into the existing `r31` displacement
// instead of reloading a separate symbol for the local table template.
#ifdef MUST_MATCH
#define WM_BASE_PARAMS const u8* base, s32 off,
#else
#define WM_BASE_PARAMS
#endif
static inline char* wmDispZenkaku(WM_BASE_PARAMS s32 num) {
    signed char buf[0x100];
    const char* tbl[10];
    static char tmp2[0x100];
    u32 i;

#ifdef MUST_MATCH
    tbl[0] = *(const char**)(base + off + 0x00);
    tbl[1] = *(const char**)(base + off + 0x04);
    tbl[2] = *(const char**)(base + off + 0x08);
    tbl[3] = *(const char**)(base + off + 0x0C);
    tbl[4] = *(const char**)(base + off + 0x10);
    tbl[5] = *(const char**)(base + off + 0x14);
    tbl[6] = *(const char**)(base + off + 0x18);
    tbl[7] = *(const char**)(base + off + 0x1C);
    tbl[8] = *(const char**)(base + off + 0x20);
    tbl[9] = *(const char**)(base + off + 0x24);
#else
    tbl[0] = "\x82\x4F";
    tbl[1] = "\x82\x50";
    tbl[2] = "\x82\x51";
    tbl[3] = "\x82\x52";
    tbl[4] = "\x82\x53";
    tbl[5] = "\x82\x54";
    tbl[6] = "\x82\x55";
    tbl[7] = "\x82\x56";
    tbl[8] = "\x82\x57";
    tbl[9] = "\x82\x58";
#endif
    if (num < 0) {
        strcpy(tmp2, "-");
        num = -num;
    } else {
        strcpy(tmp2, "");
    }
    sprintf((char*)buf, "%d", num);
    for (i = 0; i < strlen((char*)buf); i++) {
        strcat(tmp2, tbl[buf[i] - 0x30]);
    }
    return tmp2;
}

// Same body as unk_8016af5c (zero-padded), reading the digit table as wmDispZenkaku does.
static inline char* wmDispNum(WM_BASE_PARAMS s32 width, s32 num) {
    signed char buf[0x100];
    const char* tbl[10];
    s32 i;

#ifdef MUST_MATCH
    tbl[0] = *(const char**)(base + off + 0x00);
    tbl[1] = *(const char**)(base + off + 0x04);
    tbl[2] = *(const char**)(base + off + 0x08);
    tbl[3] = *(const char**)(base + off + 0x0C);
    tbl[4] = *(const char**)(base + off + 0x10);
    tbl[5] = *(const char**)(base + off + 0x14);
    tbl[6] = *(const char**)(base + off + 0x18);
    tbl[7] = *(const char**)(base + off + 0x1C);
    tbl[8] = *(const char**)(base + off + 0x20);
    tbl[9] = *(const char**)(base + off + 0x24);
#else
    tbl[0] = "\x82\x4F";
    tbl[1] = "\x82\x50";
    tbl[2] = "\x82\x51";
    tbl[3] = "\x82\x52";
    tbl[4] = "\x82\x53";
    tbl[5] = "\x82\x54";
    tbl[6] = "\x82\x55";
    tbl[7] = "\x82\x56";
    tbl[8] = "\x82\x57";
    tbl[9] = "\x82\x58";
#endif
    if (num < 0) {
        strcpy(unk_803dfbf8, "-");
        num = -num;
    } else {
        strcpy(unk_803dfbf8, "");
    }
    sprintf((char*)buf, "%d", num);
    for (i = 0; i < width; i++) {
        if (i < width - strlen((char*)buf)) {
            strcat(unk_803dfbf8, "\x82\x4F");
        } else {
            strcat(unk_803dfbf8, tbl[buf[i - (width - (s32)strlen((char*)buf))] - 0x30]);
        }
    }
    return unk_803dfbf8;
}

// The target's i2f magic doubles are read through the rodata base (0x820/0x828);
// MWCC pools anonymous copies (project-wide limitation, see win_item/win_log).
// Declaring f29..f18 with x,y last colours x=f31/y=f30, and the level buf at
// 0x1C0 matches the 0x1940 frame; both are the wins over the 92.05% version.
#pragma inline_max_total_size(60000) // keep wmDispZenkaku/wmDispNum inlined with the by-value color copies
#pragma inline_max_size(1000)
// NONMATCHING (93.80%): same 179 calls and no differing immediates; FP/GPR colouring and scheduling differ.
void winMarioDisp(CameraId camId, WindowWork* work, s32 idx) {
    // Draws the Mario status panel: name/level/rank, HP/FP/BP, star pieces and
    // shine sprites, the play-time clock and the collected star powers.
#ifdef MUST_MATCH
    const u8* vp;
#endif
    PouchData* pouch;
    const u8* e;
    GXColor white, black;
    GXColor white2, white3;
    GXColor c;
    f32 f29, f28, f27, f26, f25, f24, f23, f22, f21, f20, f19, f18;
    f32 x, y;
    s32 i, j;
    (void)camId;

#ifdef MUST_MATCH
    vp = (const u8*)&str_802F5650;
#endif
    pouch = pouchGetPtr();
    black = *(GXColor*)&dat_8041d5ec;
    e = (const u8*)work + idx * 0x14;
    white2 = *(GXColor*)&dat_8041d5e8;
    white3 = *(GXColor*)&dat_8041d5e8;
    white = *(GXColor*)&dat_8041d5e8;
    x = *(f32*)(e + 0xC4);
    y = *(f32*)(e + 0xC8);

    winBgGX(work, 0, x, y);
    {
        Vec pos, scale, tmp;
        winNameGX(work, 2, x - float_265_8041d60c, float_154_8041d610 + y, float_190_8041d614, float_24_8041d618);
        f18 = float_10_8041d61c + (x - float_265_8041d60c);
        winFontInit();
        f25 = float_152_8041d620 + y;
        tmp = VEC_AT(0x264);
        tmp.x = f18;
        tmp.y = f25;
        c = white2;
        scale = VEC_AT(0x270);
        pos = tmp;
        winFontSetEdge(&pos, &scale, &c, msgSearch(STR_AT(0x744)));
    }
    f18 = f18 + float_0p8_8041d624 * (f32)(u16)FontGetMessageWidth(msgSearch(STR_AT(0x744))) + float_20_8041d628;
    winIconInit();
    {
        Vec tr, sc, tmp;
        c = white2;
        sc = VEC_AT(0x288);
        tmp = VEC_AT(0x27C);
        tmp.x = f18;
        tmp.y = float_142_8041d62c + y;
        tr = tmp;
        winIconSet(0x19E, &tr, &sc, &c);
    }
    f18 = f18 + float_20_8041d628;
    winFontInit();
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x294);
        tmp.x = f18;
        tmp.y = f25;
        c = white2;
        scale = VEC_AT(0x2A0);
        pos = tmp;
        winFontSetEdge(&pos, &scale, &c, msgSearch(STR_AT(0x750)));
    }
    {
        char buf[0x1C0];
        Vec pos, scale, tmp;
        sprintf(buf, str_8041D63C, WM_ZENKAKU(pouch->level));
        winFontInit();
        tmp = VEC_AT(0x2AC);
        scale = VEC_AT(0x2B8);
        tmp.x = (x - float_265_8041d60c + float_190_8041d614)
                - float_0p8_8041d624 * (f32)(u16)FontGetMessageWidth(buf)
                - float_10_8041d61c;
        pos = tmp;
        c = white2;
        tmp.y = f25;
        winFontSetEdge(&pos, &scale, &c, buf);
    }
    winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
    {
        Vec tr, sc, tmp;
        tmp = VEC_AT(0x2C4);
        tmp.x = x - float_255_8041d640;
        tmp.y = float_112_8041d644 + y;
        tr = tmp;
        sc = VEC_AT(0x2D0);
        winTexSet(0xB0, tr, sc, white);
    }
    winFontInit();
    {
        Vec pos, scale, tmp;
        c = black;
        tmp = VEC_AT(0x2DC);
        tmp.x = x - float_240_8041d648;
        tmp.y = float_124_8041d64c + y;
        scale = VEC_AT(0x2E8);
        pos = tmp;
        winFontSet(&pos, &scale, &c, msgSearch(BattleGetRankNameLabel(pouch->level)));
    }

    for (i = 0; i < 6; i++) {
        if (WW_S32(work, 0x160) != pri[i]) {
            fukidashi(work, pri[i], x, y);
        }
    }
    fukidashi(work, WW_S32(work, 0x160), x, y);

    winKirinukiGX(work, 0, x - float_270_8041d650, y - float_10_8041d61c,
                  float_264_8041d654, float_110_8041d658);

    f27 = float_0p5_8041d664;
    f24 = float_50_8041d65c + (x - float_270_8041d650);
    f25 = f24 - float_2_8041d660;
    f28 = y - float_38_8041d668;
    f19 = float_4_8041d66c;
    f20 = float_12_8041d670;
    f21 = float_21_8041d674;
    {
        GXColor colTbl[3];
        GXColor hiliteTbl[3];
        const char* rawTbl[3];
        const char* tagTbl[3];
    for (i = 0, j = 0; i < 3; i++, j += 0x22) {
        const char* mRaw;
        const char* mTag;
        Vec tr, sc, tmp, pos, scale;
        tagTbl[0] = WM_TAG_TBL(0);
        tagTbl[1] = WM_TAG_TBL(1);
        tagTbl[2] = WM_TAG_TBL(2);
        colTbl[0] = GXC_AT(0x2F4);
        colTbl[1] = GXC_AT(0x2F8);
        colTbl[2] = GXC_AT(0x2FC);
        rawTbl[0] = WM_RAW_TBL(0);
        rawTbl[1] = WM_RAW_TBL(1);
        rawTbl[2] = WM_RAW_TBL(2);
        hiliteTbl[0] = GXC_AT(0x300);
        hiliteTbl[1] = GXC_AT(0x304);
        hiliteTbl[2] = GXC_AT(0x308);
        mRaw = rawTbl[i];
        mTag = tagTbl[i];
        f23 = (f32)(u16)FontGetMessageWidth(mRaw);
        f22 = float_0p5_8041d664 * (f32)(u16)FontGetMessageWidth(msgSearch(mTag));
        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        f18 = f28 - (f32)j;
        tmp = VEC_AT(0x36C);
        tmp.x = f25;
        tmp.y = f18 - float_4_8041d66c;
        tr = tmp;
        sc = VEC_AT(0x378);
        winTexSet(0xAF, tr, sc, colTbl[i]);
        winFontInit();
        tmp = VEC_AT(0x384);
        tmp.x = f24 - f27 * f23;
        tmp.y = f20 + f18;
        scale = VEC_AT(0x390);
        pos = tmp;
        c = black;
        winFontSet(&pos, &scale, &c, mRaw);
        tmp = VEC_AT(0x39C);
        tmp.x = f24 - f27 * f22;
        tmp.y = f21 + f18;
        c = hiliteTbl[i];
        scale = VEC_AT(0x3A8);
        pos = tmp;
        winFontSet(&pos, &scale, &c, msgSearch(mTag));
    }
    }
    winIconInit();
    f18 = float_110_8041d658 + (x - float_270_8041d650);
    f26 = float_4_8041d66c + (y - float_38_8041d668);
    {
        Vec tr, sc, tmp;
        c = white2;
        sc = VEC_AT(0x3C0);
        tmp = VEC_AT(0x3B4);
        tmp.x = f18;
        tmp.y = f26;
        tr = tmp;
        winIconSet(0x19F, &tr, &sc, &c);
    }
    f24 = (y - float_38_8041d668) - float_34_8041d678;
    f27 = float_4_8041d66c + f24;
    {
        Vec tr, sc, tmp;
        c = white2;
        sc = VEC_AT(0x3D8);
        tmp = VEC_AT(0x3CC);
        tmp.x = f18;
        tmp.y = f27;
        tr = tmp;
        winIconSet(0x1A0, &tr, &sc, &c);
    }
    f25 = (y - float_38_8041d668) - float_68_8041d67c;
    {
        Vec tr, sc, tmp;
        c = white2;
        sc = VEC_AT(0x3F0);
        tmp = VEC_AT(0x3E4);
        tmp.x = f18;
        tmp.y = float_4_8041d66c + f25;
        tr = tmp;
        winIconSet(0x1A1, &tr, &sc, &c);
    }
    winFontInit();
    f18 = float_130_8041d680 + (x - float_270_8041d650);
    f28 = float_4_8041d66c + (float_12_8041d670 + (y - float_38_8041d668));
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x3FC);
        tmp.x = f18;
        tmp.y = f28;
        scale = VEC_AT(0x408);
        pos = tmp;
        c = black;
        winFontSetR(&pos, &scale, &c, str_8041D63C, WM_ZENKAKU(pouch->currentHP));
    }
    f24 = float_4_8041d66c + (float_12_8041d670 + f24);
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x414);
        tmp.x = f18;
        tmp.y = f24;
        scale = VEC_AT(0x420);
        pos = tmp;
        c = black;
        winFontSetR(&pos, &scale, &c, str_8041D63C, WM_ZENKAKU(pouch->currentFP));
    }
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x42C);
        tmp.x = f18;
        tmp.y = float_4_8041d66c + (float_12_8041d670 + f25);
        scale = VEC_AT(0x438);
        pos = tmp;
        c = black;
        winFontSetR(&pos, &scale, &c, str_8041D63C, WM_ZENKAKU(pouch->maximumBP));
    }
    winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
    f18 = float_190_8041d614 + (x - float_270_8041d650);
    {
        Vec tr, sc, tmp;
        tmp = VEC_AT(0x444);
        tmp.x = f18;
        tmp.y = f26;
        tr = tmp;
        sc = VEC_AT(0x450);
        winTexSet(0x10, tr, sc, white);
    }
    {
        Vec tr, sc, tmp;
        tmp = VEC_AT(0x45C);
        tmp.x = f18;
        tmp.y = f27;
        tr = tmp;
        sc = VEC_AT(0x468);
        winTexSet(0x10, tr, sc, white);
    }
    winFontInit();
    f18 = float_200_8041d684 + (x - float_270_8041d650);
    f28 = float_4_8041d66c + (float_12_8041d670 + (y - float_38_8041d668));
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x474);
        tmp.x = f18;
        tmp.y = f28;
        scale = VEC_AT(0x480);
        pos = tmp;
        c = black;
        winFontSet(&pos, &scale, &c, str_8041D63C, WM_ZENKAKU(pouch->maximumHP));
    }
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x48C);
        tmp.x = f18;
        tmp.y = f24;
        scale = VEC_AT(0x498);
        pos = tmp;
        c = black;
        winFontSet(&pos, &scale, &c, str_8041D63C, WM_ZENKAKU(pouch->maximumFP));
    }

    if (WW_S32(work, 0x198) > 0) {
        winKirinukiGX(work, 0, float_10_8041d61c + x,
                      (float_20_8041d628 + (float_130_8041d680 + y)) - float_20_8041d628,
                      float_260_8041d688, float_80_8041d68c);
        winIconInit();
        f18 = float_10_8041d61c + (float_40_8041d690 + x);
        f24 = float_106_8041d694 + y;
        {
            Vec tr, sc, tmp;
            c = white2;
            sc = VEC_AT(0x4B0);
            tmp = VEC_AT(0x4A4);
            tmp.x = f18;
            tmp.y = f24;
            tr = tmp;
            winIconSet(0x20F, &tr, &sc, &c);
        }
        f19 = float_70_8041d698 + y;
        {
            Vec tr, sc, tmp;
            c = white2;
            sc = VEC_AT(0x4C8);
            tmp = VEC_AT(0x4BC);
            tmp.x = f18;
            tmp.y = f19;
            tr = tmp;
            winIconSet(0x197, &tr, &sc, &c);
        }
        winFontInit();
        {
            Vec pos, scale, tmp;
            tmp = VEC_AT(0x4D4);
            tmp.x = float_10_8041d61c + (float_70_8041d698 + x);
            tmp.y = float_10_8041d61c + f19;
            c = white2;
            scale = VEC_AT(0x4E0);
            pos = tmp;
            winFontSet(&pos, &scale, &c, msgSearch(STR_AT(0x768)));
        }
        winIconInit();
        f19 = float_10_8041d61c + (float_80_8041d68c + x);
        for (i = 0, j = 0; i < WW_S32(work, 0x198); i++, j += 0x16) {
            Vec tr, sc, tmp;
            c = white3;
            if (i < pouch->currentSP / 100) {
                tmp = VEC_AT(0x4EC);
                sc = VEC_AT(0x4F8);
                tmp.x = f19 + (f32)j;
                tmp.y = f24;
                tr = tmp;
                winIconSet(0x1B7 + i, &tr, &sc, &c);
            } else {
                tmp = VEC_AT(0x504);
                sc = VEC_AT(0x510);
                tmp.x = f19 + (f32)j;
                tmp.y = f24;
                tr = tmp;
                winIconSet(0x1BF, &tr, &sc, &c);
            }
        }
    }

    winKirinukiGX(work, 0, float_10_8041d61c + x,
                  float_20_8041d628 + (float_10_8041d61c + y),
                  float_260_8041d688, float_150_8041d69c);
    winIconInit();
    f24 = float_10_8041d61c + (float_20_8041d628 + x);
    {
        Vec tr, sc, tmp;
        c = white2;
        tmp = VEC_AT(0x51C);
        sc = VEC_AT(0x528);
        tmp.x = f24;
        tmp.y = float_10_8041d61c + y;
        tr = tmp;
        winIconSet(0x18C, &tr, &sc, &c);
    }
    {
        Vec tr, sc, tmp;
        c = white2;
        tmp = VEC_AT(0x534);
        sc = VEC_AT(0x540);
        tmp.x = f24;
        tmp.y = float_10_8041d61c + (y - float_28_8041d6a0);
        tr = tmp;
        winIconSet(0x18B, &tr, &sc, &c);
    }
    {
        Vec tr, sc, tmp;
        c = white2;
        tmp = VEC_AT(0x54C);
        sc = VEC_AT(0x558);
        tmp.x = f24;
        tmp.y = float_10_8041d61c + (y - float_56_8041d6a4);
        tr = tmp;
        winIconSet(0x18D, &tr, &sc, &c);
    }
    {
        Vec tr, sc, tmp;
        c = white2;
        tmp = VEC_AT(0x564);
        sc = VEC_AT(0x570);
        tmp.x = f24;
        tmp.y = float_10_8041d61c + (y - float_84_8041d6a8);
        tr = tmp;
        winIconSet(0x102, &tr, &sc, &c);
    }
    {
        Vec tr, sc, tmp;
        c = white2;
        tmp = VEC_AT(0x57C);
        sc = VEC_AT(0x588);
        tmp.x = f24;
        tmp.y = float_10_8041d61c + (y - float_112_8041d644);
        tr = tmp;
        winIconSet(0x1A2, &tr, &sc, &c);
    }

    winFontInit();
    f24 = float_10_8041d61c + (float_35_8041d6ac + x);
    f25 = float_12_8041d670 + y;
    f26 = float_10_8041d61c + f25;
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x594);
        tmp.x = f24;
        tmp.y = f26;
        c = white2;
        scale = VEC_AT(0x5A0);
        pos = tmp;
        winFontSet(&pos, &scale, &c, msgSearch(STR_AT(0x780)));
    }
    f27 = float_10_8041d61c + (f25 - float_28_8041d6a0);
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x5AC);
        tmp.x = f24;
        tmp.y = f27;
        c = white2;
        scale = VEC_AT(0x5B8);
        pos = tmp;
        winFontSet(&pos, &scale, &c, msgSearch(STR_AT(0x798)));
    }
    f28 = float_10_8041d61c + (f25 - float_56_8041d6a4);
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x5C4);
        tmp.x = f24;
        tmp.y = f28;
        c = white2;
        scale = VEC_AT(0x5D0);
        pos = tmp;
        winFontSet(&pos, &scale, &c, msgSearch(STR_AT(0x7B0)));
    }
    f29 = float_10_8041d61c + (f25 - float_84_8041d6a8);
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x5DC);
        tmp.x = f24;
        tmp.y = f29;
        c = white2;
        scale = VEC_AT(0x5E8);
        pos = tmp;
        winFontSet(&pos, &scale, &c, msgSearch(STR_AT(0x7CC)));
    }
    f25 = float_10_8041d61c + (f25 - float_112_8041d644);
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x5F4);
        tmp.x = f24;
        tmp.y = f25;
        c = white2;
        scale = VEC_AT(0x600);
        pos = tmp;
        winFontSet(&pos, &scale, &c, msgSearch(STR_AT(0x7EC)));
    }

    f18 = float_2_8041d660 + (float_8_8041d6b0 + (float_205_8041d6b4 + x));
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x60C);
        tmp.x = f18;
        tmp.y = f26;
        pos = tmp;
        scale = VEC_AT(0x618);
        c = white2;
        winFontSetR(&pos, &scale, &c, str_8041D63C, WM_ZENKAKU(pouch->mStarPoints));
    }
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x624);
        tmp.x = f18;
        tmp.y = f27;
        pos = tmp;
        scale = VEC_AT(0x630);
        c = white2;
        winFontSetR(&pos, &scale, &c, str_8041D63C, WM_ZENKAKU(pouch->coins));
    }
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x63C);
        tmp.x = f18;
        tmp.y = f28;
        pos = tmp;
        scale = VEC_AT(0x648);
        c = white2;
        winFontSetR(&pos, &scale, &c, str_8041D63C, WM_ZENKAKU(pouch->mStarPieceCount));
    }
    {
        Vec pos, scale, tmp;
        tmp = VEC_AT(0x654);
        tmp.x = f18;
        tmp.y = f29;
        pos = tmp;
        scale = VEC_AT(0x660);
        c = white2;
        winFontSetR(&pos, &scale, &c, str_8041D63C, WM_ZENKAKU(pouch->mShineSpriteCount));
    }

    {
        Vec pos1, scale1, pos2, scale2, tmp;
        s64 secs;
        u32 hours;
        u32 mins;
        secs = gp->retraceLocalTime / (s64)(*(volatile u32*)0x800000F8 >> 2);
        if ((u64)secs > 359999) {
            hours = 99;
            mins = 59;
        } else {
            hours = (u32)((u64)secs / 3600);
            mins = (u32)(((u64)secs % 3600) / 60);
        }
        tmp = VEC_AT(0x66C);
        scale1 = VEC_AT(0x678);
        tmp.x = float_2_8041d660 + (float_2_8041d660 + ((float_8_8041d6b0 + (float_172_8041d6b8 + x)) - float_16_8041d6bc));
        tmp.y = f25;
        pos1 = tmp;
        c = white2;
        winFontSetR(&pos1, &scale1, &c, str_8041D63C, WM_NUM(2, hours));
        tmp = VEC_AT(0x684);
        scale2 = VEC_AT(0x690);
        tmp.x = f18;
        tmp.y = f25;
        pos2 = tmp;
        c = white2;
        winFontSetR(&pos2, &scale2, &c, str_8041D63C, WM_NUM(2, mins));
    }

    winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
    f19 = float_10_8041d61c + (float_190_8041d614 + x);
    for (i = 0, j = 0; i < 4; i++, j += 0x1C) {
        Vec tr, sc, tmp;
        tmp = VEC_AT(0x69C);
        tmp.x = f19;
        tmp.y = float_10_8041d61c + (y - (f32)j);
        tr = tmp;
        sc = VEC_AT(0x6A8);
        winTexSet(0x11, tr, sc, white);
    }
    {
        Vec tr, sc, tmp;
        tmp = VEC_AT(0x6B4);
        f24 = float_220_8041d6c0 + x;
        tmp.x = f24;
        tmp.y = (float_10_8041d61c + (float_2_8041d660 + (y - float_112_8041d644))) - float_2_8041d660;
        tr = tmp;
        sc = VEC_AT(0x6C0);
        winTexSet(0x12, tr, sc, white);
    }

    {
        AnimData* anim;
        void* p;
        f32 frac;
        GXColor kc;
        animPoseGetAnimPosePtr(POSE);
        anim = animPoseGetAnimDataPtr(POSE);
        if (WW_S32(work, 0x18C) == 0) {
            p = *(void**)((u8*)anim + 0x24);
            frac = (f32)(gp->retraceCount % (s32)*(f32*)((u8*)p + 8));
            animPoseSetLocalTime(POSE, frac);
        }
        animPoseMain(POSE);
        kc = *(GXColor*)&dat_8041d608;
        winKageGX(work, &kc, x - float_150_8041d69c, y, float_0_8041d6c4, float_1_8041d6c8);
        {
            Mtx mtx;
            PSMTXTrans(mtx, x - float_150_8041d69c, y, float_0_8041d6c4);
            winZClear();
            animPoseDrawMtx(POSE, mtx, 2, float_0_8041d6c4, float_5_8041d6cc);
        }
    }

    if (WW_S32(work, 0x190) != 0) {
        winWazaGX(work, 0, x - float_270_8041d650, float_160_8041d6d0,
                  float_270_8041d650, float_254_8041d6d4);
        winFontInit();
        f26 = float_90_8041d6d8 + x - float_270_8041d650;
        {
            Vec pos, scale, tmp;
            scale = VEC_AT(0x6D8);
            c = white2;
            tmp = VEC_AT(0x6CC);
            tmp.x = f26;
            pos = tmp;
            winFontSetEdge(&pos, &scale, &c, msgSearch(STR_AT(0x808)));
        }
        {
            Vec pos, scale, tmp;
            scale = VEC_AT(0x6F0);
            c = white2;
            tmp = VEC_AT(0x6E4);
            tmp.x = f24 - float_270_8041d650;
            pos = tmp;
            winFontSetEdge(&pos, &scale, &c, str_8041D6DC);
        }
        f20 = float_228_8041d6e4 + x - float_270_8041d650;
        f21 = float_4_8041d66c + (float_17_8041d6e8 + f24 - float_270_8041d650);
        f19 = float_10_8041d61c + (float_10_8041d61c + x - float_270_8041d650);
        for (i = 0, j = 0; i < WW_S32(work, 0x198); i++, j += 0x1A) {
            SuperActionEntry* sa;
            char buf[0x40];
            f32 drop;
            Vec pos1, scale1, pos2, scale2, pos3, scale3, tmp;
            scale1 = VEC_AT(0x708);
            tmp = VEC_AT(0x6FC);
            c = black;
            sa = superActionTable[i];
            tmp.x = f19;
            tmp.y = (f32)(0x82 - j);
            pos1 = tmp;
            winFontSetLabel(&pos1, &scale1, &c, sa->field_0);
            sprintf(buf, str_8041D63C, WM_ZENKAKU(sa->field_12));
            drop = (f32)(0x82 - j);
            tmp = VEC_AT(0x714);
            scale2 = VEC_AT(0x720);
            c = black;
            tmp.x = f20 - (f32)((u16)FontGetMessageWidth(buf) >> 1);
            tmp.y = drop;
            pos2 = tmp;
            winFontSet(&pos2, &scale2, &c, buf);
            tmp = VEC_AT(0x72C);
            scale3 = VEC_AT(0x738);
            tmp.x = f21;
            tmp.y = (f32)(0x78 - j);
            pos3 = tmp;
            winFontSet(&pos3, &scale3, &black, str_8041D6DC);
        }
    }

#pragma inline_max_total_size reset
#pragma inline_max_size reset
#undef POSE
#undef STR_AT
#undef GXC_AT
#undef VEC_AT
}
