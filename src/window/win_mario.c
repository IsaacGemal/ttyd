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
// .sdata2 anim names shared with winMarioInit
const char str_8041D578[6] = "M_S_1";
const char str_8041D580[4] = "S_1";
const char str_8041D584[6] = "M_H_1";
const char str_8041D58C[4] = "R_1";
const char str_8041D590[6] = "M_R_1";
const char str_8041D598[6] = "M_H_4";
const char str_8041D5A0[6] = "M_H_7";

// WindowWork only declares its first 0x40 bytes in the header; the fields this
// unit uses past that point are reached through raw offsets, like win_root.c.
#define WW_S32(w, off) (*(s32*)((u8*)(w) + (off)))
#define WW_F32(w, off) (*(f32*)((u8*)(w) + (off)))
#define WW_PTR(w, off) (*(void**)((u8*)(w) + (off)))

// .bss. The original's symbols are compiler-tagged (`tmp2$708`); the `$` cannot
// be spelled in C, so the name here is cosmetic and the address is what matters.
char unk_803dfbf8[0x100];

// .rodata / .sdata2 objects this unit reads but does not define yet

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

// .rodata strings, before the code: their jumptable pools land after them
__declspec(section ".rodata") const char str_802F5650_base[20] = "msg_menu_mario_lv\x00\x00\x00";
#ifdef MUST_MATCH
#define WM_MSG_BASE ((const char*)&str_802F5650)
#else
#define WM_MSG_BASE str_802F5650_base
#endif
const char str_802F5664[24] = "msg_menu_mario_star_r\x00\x00\x00";
const char str_802F567C[20] = "msg_menu_mario_hp\x00\x00\x00";
const char str_802F5690[20] = "msg_menu_mario_fp\x00\x00\x00";
const char str_802F56A4[20] = "msg_menu_mario_bp\x00\x00\x00";
const char str_802F56B8[12] = "msg_slit\x00\x00\x00\x00";
const char str_802F56C4[12] = "msg_plane\x00\x00\x00";
const char str_802F56D0[12] = "msg_ship\x00\x00\x00\x00";
const char str_802F56DC[12] = "msg_roll\x00\x00\x00\x00";
const char str_802F56E8[20] = "msg_menu_star_power\x00";
const char str_802F56FC[20] = "msg_menu_mario_waza\x00";
const char str_802F5710[24] = "msg_menu_mario_star_p\x00\x00\x00";
const char str_802F5728[20] = "msg_menu_mario_coin\x00";
const char str_802F573C[24] = "msg_menu_mario_kakera\x00\x00\x00";
const char str_802F5754[24] = "msg_menu_mario_scoin\x00\x00\x00\x00";
const char str_802F576C[20] = "msg_menu_mario_time\x00";
const char str_802F5780[16] = "msg_hammer_1\x00\x00\x00\x00";
const char str_802F5790[16] = "msg_hammer_2\x00\x00\x00\x00";
const char str_802F57A0[16] = "msg_hammer_3\x00\x00\x00\x00";
const char str_802F57B0[12] = "msg_boots_1\x00";
const char str_802F57BC[12] = "msg_boots_2\x00";
const char str_802F57C8[12] = "msg_boots_3\x00";

// .sdata lookup tables (window "help" message ids per upgrade level)
const char* hammer_help[4] = { NULL, str_802F5780, str_802F5790, str_802F57A0 };
const char* boots_help[4] = { NULL, str_802F57B0, str_802F57BC, str_802F57C8 };

#ifdef MUST_MATCH
#define FUKI_VEC(off, sym) (*(Vec*)(vp + (off)))
#else
#define FUKI_VEC(off, sym) sym
#endif
#ifdef MUST_MATCH
#define WM_BASE_PARAMS const u8* base, s32 off,
#else
#define WM_BASE_PARAMS
#endif

// The five anim-name strings live in .rodata past the unit's msg tables; the
// target reaches them through the r30 rodata base (`addi rN, r30, 0x830..0x858`).
// Named literals pool at the section start instead, so the matching build reads
// them through `vp` (same base winMarioDisp uses) and the natural form under #else.
// Prototypes for the forward references the original function order needs.
static inline char* wmDispZenkaku(WM_BASE_PARAMS s32 num);
static inline char* wmDispNum(WM_BASE_PARAMS s32 width, s32 num);

void winMarioInit(WindowWork* work) {
#ifdef MUST_MATCH
    const u8* vp = (const u8*)&str_802F5650;
#endif
    PouchData* pouch = pouchGetPtr();
    const char* anim;
    s32 i;
#ifdef MUST_MATCH
#define D_MARIO "d_mario"
#define A_MARIO "a_mario"
#define A_MARIO_L "a_mario_l"
#define A_MARIO_W "a_mario_w"
#define A_MARIO_WL "a_mario_wl"
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
            anim = "S_2";
            break;
        case 2:
            anim = "S_3";
            break;
        case 3:
            anim = "S_4";
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

void winMarioExit(WindowWork* work) {
    animPoseRelease(WW_S32(work, 0x188));
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
                        SET_ANIM("S_2");
                        break;
                    case 2:
                        SET_ANIM("S_3");
                        break;
                    case 3:
                        SET_ANIM("S_4");
                        break;
                    }
                } else {
                    switch (marioGetColor()) {
                    case 0:
                        SET_ANIM(str_8041D58C);
                        break;
                    case 1:
                        SET_ANIM("R_15");
                        break;
                    case 2:
                        SET_ANIM("R_17");
                        break;
                    case 3:
                        SET_ANIM("R_18");
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
                        anim = "S_2";
                        break;
                    case 2:
                        anim = "S_3";
                        break;
                    case 3:
                        anim = "S_4";
                        break;
                    }
                } else {
                    switch (marioGetColor()) {
                    case 0:
                        anim = str_8041D58C;
                        break;
                    case 1:
                        anim = "R_15";
                        break;
                    case 2:
                        anim = "R_17";
                        break;
                    case 3:
                        anim = "R_18";
                        break;
                    }
                }
            } else {
                anim = hammer_pose[pouchGetHammerLv()];
            }

            animPoseSetAnim(WW_S32(work, 0x188), anim, 0);
            if (WW_S32(work, 0x18C) == 0) {
                if (animPoseGetLoopTimes(WW_S32(work, 0x188)) >= 0.89f) {
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
                        anim = "S_2";
                        break;
                    case 2:
                        anim = "S_3";
                        break;
                    case 3:
                        anim = "S_4";
                        break;
                    }
                } else {
                    switch (marioGetColor()) {
                    case 0:
                        anim = str_8041D58C;
                        break;
                    case 1:
                        anim = "R_15";
                        break;
                    case 2:
                        anim = "R_17";
                        break;
                    case 3:
                        anim = "R_18";
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
        WW_F32(work, 0x158) = -280.0f;
        f2 = (f32)WW_S32(work, 0x194);
        WW_F32(work, 0x15C) = 10.0f + ((120.0f - 26.0f * f2) - 12.0f);
        winMsgEntry(work, 0, superActionTable[WW_S32(work, 0x194)]->field_C, 0);
        break;
    }
    return 0;
}

// .sdata2 colour constants (referenced by fukidashi/winMarioDisp)

void winMarioMain2(WindowWork* work) {
    (void)work;
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
        strcpy(tmp2, "\x81\x7C");
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

char* unk_8016af5c(s32 num, s32 width) {
    signed char buf[0x100];
    const char* tbl[10] = {
        "\x82\x4F", "\x82\x50", "\x82\x51", "\x82\x52", "\x82\x53",
        "\x82\x54", "\x82\x55", "\x82\x56", "\x82\x57", "\x82\x58",
    };
    s32 i;

    if (num < 0) {
        strcpy(unk_803dfbf8, "\x81\x7C");
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

const GXColor dat_8041d5d0 = { 0x00, 0x00, 0x00, 0x80 };
const GXColor dat_8041d5d4 = { 0xE1, 0xE1, 0xE1, 0xFE };
const GXColor dat_8041d5d8 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_8041d5dc = { 0x00, 0x00, 0x00, 0x80 };
const GXColor dat_8041d5e0 = { 0xE1, 0xE1, 0xE1, 0xFE };
const GXColor dat_8041d5e4 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_8041d5e8 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_8041d5ec = { 0x00, 0x00, 0x00, 0xFF };

// The two bubble shapes fukidashi draws: the three texture/icon positions are read from
// the unit's shared rodata vectors through one base (vp), as winMarioDisp does.
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
        tmp1.x = (x - 230.0f) + 5.0f;
        y = 72.0f + y;
        tmp1.y = y - 5.0f;
        tr1 = tmp1;
        WINTEXSET(0xB1, tr1, sc1, c1);
        c2 = dat_8041d5e0;
        sc2 = FUKI_VEC(0x240, vec3_802f5890);
        tmp2 = FUKI_VEC(0x234, vec3_802f5884);
        tmp2.x = x - 230.0f;
        tmp2.y = y;
        tr2 = tmp2;
        WINTEXSET(0xB1, tr2, sc2, c2);
        winIconInit();
        c3 = dat_8041d5e4;
        sc3 = FUKI_VEC(0x258, vec3_802f58a8);
        tmp3 = FUKI_VEC(0x24C, vec3_802f589c);
        tmp3.x = (x - 230.0f) - 6.0f;
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
        tmp1.x = (x - 210.0f) + 5.0f;
        y = 20.0f + y;
        tmp1.y = y - 5.0f;
        tr1 = tmp1;
        WINTEXSET(0xB1, tr1, sc1, c1);
        c2 = dat_8041d5e0;
        sc2 = FUKI_VEC(0x240, vec3_802f5890);
        tmp2 = FUKI_VEC(0x234, vec3_802f5884);
        tmp2.x = x - 210.0f;
        tmp2.y = y;
        tr2 = tmp2;
        WINTEXSET(0xB1, tr2, sc2, c2);
        winIconInit();
        c3 = dat_8041d5e4;
        sc3 = FUKI_VEC(0x258, vec3_802f58a8);
        tmp3 = FUKI_VEC(0x24C, vec3_802f589c);
        tmp3.x = (x - 210.0f) - 6.0f;
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
        tmp1.x = (x - 80.0f) + 5.0f;
        y = 90.0f + y;
        tmp1.y = y - 5.0f;
        tr1 = tmp1;
        WINTEXSET(0xB1, tr1, sc1, c1);
        c2 = dat_8041d5d4;
        sc2 = FUKI_VEC(0x1F8, vec3_802f5848);
        tmp2 = FUKI_VEC(0x1EC, vec3_802f583c);
        tmp2.x = x - 80.0f;
        tmp2.y = y;
        tr2 = tmp2;
        WINTEXSET(0xB1, tr2, sc2, c2);
        winIconInit();
        c3 = dat_8041d5d8;
        sc3 = FUKI_VEC(0x210, vec3_802f5860);
        tmp3 = FUKI_VEC(0x204, vec3_802f5854);
        tmp3.x = (x - 80.0f) + 6.0f;
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
        tmp1.x = (x - 80.0f) + 5.0f;
        y = 20.0f + y;
        tmp1.y = y - 5.0f;
        tr1 = tmp1;
        WINTEXSET(0xB1, tr1, sc1, c1);
        c2 = dat_8041d5d4;
        sc2 = FUKI_VEC(0x1F8, vec3_802f5848);
        tmp2 = FUKI_VEC(0x1EC, vec3_802f583c);
        tmp2.x = x - 80.0f;
        tmp2.y = y;
        tr2 = tmp2;
        WINTEXSET(0xB1, tr2, sc2, c2);
        winIconInit();
        c3 = dat_8041d5d8;
        sc3 = FUKI_VEC(0x210, vec3_802f5860);
        tmp3 = FUKI_VEC(0x204, vec3_802f5854);
        tmp3.x = (x - 80.0f) + 6.0f;
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
        tmp1.x = (x - 30.0f) + 5.0f;
        y = 130.0f + y;
        tmp1.y = y - 5.0f;
        tr1 = tmp1;
        WINTEXSET(0xB1, tr1, sc1, c1);
        c2 = dat_8041d5d4;
        sc2 = FUKI_VEC(0x1F8, vec3_802f5848);
        tmp2 = FUKI_VEC(0x1EC, vec3_802f583c);
        tmp2.x = x - 30.0f;
        tmp2.y = y;
        tr2 = tmp2;
        WINTEXSET(0xB1, tr2, sc2, c2);
        winIconInit();
        c3 = dat_8041d5d8;
        sc3 = FUKI_VEC(0x210, vec3_802f5860);
        tmp3 = FUKI_VEC(0x204, vec3_802f5854);
        tmp3.x = (x - 30.0f) + 6.0f;
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
        tmp1.x = (x - 30.0f) + 5.0f;
        y = 60.0f + y;
        tmp1.y = y - 5.0f;
        tr1 = tmp1;
        WINTEXSET(0xB1, tr1, sc1, c1);
        c2 = dat_8041d5d4;
        sc2 = FUKI_VEC(0x1F8, vec3_802f5848);
        tmp2 = FUKI_VEC(0x1EC, vec3_802f583c);
        tmp2.x = x - 30.0f;
        tmp2.y = y;
        tr2 = tmp2;
        WINTEXSET(0xB1, tr2, sc2, c2);
        winIconInit();
        c3 = dat_8041d5d8;
        sc3 = FUKI_VEC(0x210, vec3_802f5860);
        tmp3 = FUKI_VEC(0x204, vec3_802f5854);
        tmp3.x = (x - 30.0f) + 6.0f;
        tmp3.y = y;
        tr3 = tmp3;
        winIconSet(iconId, &tr3, &sc3, &c3);
        }
        break;
    default:
        break;
    }
}

const char* hammer_pose[4] = { NULL, str_8041D584, str_8041D598, str_8041D5A0 };
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
        strcpy(tmp2, "\x81\x7C");
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
        strcpy(unk_803dfbf8, "\x81\x7C");
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
// 0x1C0 matches the 0x1940 frame.
#pragma inline_max_total_size(60000) // keep wmDispZenkaku/wmDispNum inlined with the by-value color copies
#pragma inline_max_size(1000)
// NONMATCHING (93.90%): same 179 calls and no differing immediates; FP/GPR colouring, the i2f-magic
// addressing and the stack layout differ. The independent local/Vec load and store order below
// (colours, f20/f21/f19, tmp/sc/scale/pos/c/tr, hours/mins) reproduces the original codegen schedule.
extern const f32 float_10_8041d61c;
void winMarioDisp(CameraId camId, WindowWork* work, s32 idx) {
    // Draws the Mario status panel: name/level/rank, HP/FP/BP, star pieces and
    // shine sprites, the play-time clock and the collected star powers.
#ifdef MUST_MATCH
    const u8* vp;
#endif
        GXColor colTbl[3];
        GXColor hiliteTbl[3];
        const char* rawTbl[3];
        const char* tagTbl[3];
    PouchData* pouch;
    const u8* e;
#ifdef MUST_MATCH
    // Match-only local order: `c` before the other colour locals colours the
    // argument copies as the original does.
    GXColor white3, black3, white2, black2;
    GXColor c1, c2, c3, c4, c40, c5, c41, c6, c7, c8, c9, c10, c11, c12, c13, c42, c43, c14, c15, c16, c17, c18, c19, c20, c21, c22, c23, c24, c25, c26, c27, c28, c29, c30, c31, c32, c33, c34, c35, c44, c45, c36, c37, c38, c39;

    GXColor white, black;
#else
    GXColor white, black;
    GXColor white2, white3;
    GXColor c;
// The match-only colour copies below all name the one natural colour local.
#define c1 c
#define c2 c
#define c3 c
#define c4 c
#define c40 c
#define c5 c
#define c41 c
#define c6 c
#define c7 c
#define c8 c
#define c9 c
#define c10 c
#define c11 c
#define c12 c
#define c13 c
#define c42 c
#define c43 c
#define c14 c
#define c15 c
#define c16 c
#define c17 c
#define c18 c
#define c19 c
#define c20 c
#define c21 c
#define c22 c
#define c23 c
#define c24 c
#define c25 c
#define c26 c
#define c27 c
#define c28 c
#define c29 c
#define c30 c
#define c31 c
#define c32 c
#define c33 c
#define c34 c
#define c35 c
#define c44 c
#define c45 c
#define c36 c
#define c37 c
#define c38 c
#define c39 c
#define black2 black
#define black3 black
#endif
    f32 f29, f28, f27, f26, f25, f24, f23, f22, f21, f20, f19, f18;
    f32 x, y;
    s32 i, j;
    (void)camId;

#ifdef MUST_MATCH
    vp = (const u8*)&str_802F5650;
#endif
    pouch = pouchGetPtr();
    white = *(GXColor*)&dat_8041d5e8;
    black = *(GXColor*)&dat_8041d5ec;
    white2 = *(GXColor*)&dat_8041d5e8;
    black2 = *(GXColor*)&dat_8041d5ec;
    white3 = *(GXColor*)&dat_8041d5e8;
    black3 = *(GXColor*)&dat_8041d5ec;
    e = (const u8*)work + idx * 0x14;
    x = *(f32*)(e + 0xC4);
    y = *(f32*)(e + 0xC8);

    winBgGX(work, 0, x, y);
    {
        Vec pos, tmp, scale;
        winNameGX(work, 2, x - 265.0f, 154.0f + y, 190.0f, 24.0f);
        f18 = 10.0f + (x - 265.0f);
        winFontInit();
        f25 = 152.0f + y;
        tmp = VEC_AT(0x264);
        tmp.x = f18;
        tmp.y = f25;
        c1 = white2;
        scale = VEC_AT(0x270);
        pos = tmp;
        winFontSetEdge(&pos, &scale, &c1, msgSearch(STR_AT(0x744)));
    }
    f18 = f18 + 0.8f * (f32)(u16)FontGetMessageWidth(msgSearch(STR_AT(0x744)));
    f18 = f18 + 20.0f;
    winIconInit();
    {
        Vec tr, tmp, sc;
        c2 = white2;
        sc = VEC_AT(0x288);
        tmp = VEC_AT(0x27C);
        tmp.x = f18;
        tmp.y = 142.0f + y;
        tr = tmp;
        winIconSet(0x19E, &tr, &sc, &c2);
    }
    f18 = f18 + 20.0f;
    winFontInit();
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x294);
        tmp.x = f18;
        tmp.y = f25;
        c3 = white2;
        scale = VEC_AT(0x2A0);
        pos = tmp;
        winFontSetEdge(&pos, &scale, &c3, msgSearch(STR_AT(0x750)));
    }
    {
        char buf[0x100];
        Vec pos, tmp, scale;
        sprintf(buf, "%s", WM_ZENKAKU(pouch->level));
        winFontInit();
        tmp = VEC_AT(0x2AC);
        scale = VEC_AT(0x2B8);
        tmp.x = (x - 265.0f + 190.0f)
                - 0.8f * (f32)(u16)FontGetMessageWidth(buf)
                - 10.0f;
        tmp.y = f25;
        pos = tmp;
        c4 = white2;
        winFontSetEdge(&pos, &scale, &c4, buf);
    }
    winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
    {
        Vec tr, tmp, sc;
        tmp = VEC_AT(0x2C4);
        tmp.x = x - 255.0f;
        tmp.y = 112.0f + y;
        tr = tmp;
        sc = VEC_AT(0x2D0);
        c40 = white;
        WINTEXSET(0xB0, tr, sc, c40);
    }
    winFontInit();
    {
        Vec pos, tmp, scale;
        c5 = black2;
        tmp = VEC_AT(0x2DC);
        tmp.x = x - 240.0f;
        tmp.y = 124.0f + y;
        scale = VEC_AT(0x2E8);
        pos = tmp;
        winFontSet(&pos, &scale, &c5, msgSearch(BattleGetRankNameLabel(pouch->level)));
    }

    for (i = 0; i < 6; i++) {
        if (WW_S32(work, 0x160) != pri[i]) {
            fukidashi(work, pri[i], x, y);
        }
    }
    fukidashi(work, WW_S32(work, 0x160), x, y);

    winKirinukiGX(work, 0, x - 270.0f, y - 10.0f,
                  264.0f, 110.0f);

    f27 = 0.5f;
    f24 = 50.0f + (x - 270.0f);
    f25 = f24 - 2.0f;
    f28 = y - 38.0f;
#ifdef MUST_MATCH
    // Match-only evaluation order (f20/f21 before f19) for the row-block constants.
    f20 = 12.0f;
    f21 = 21.0f;
    f19 = 4.0f;
#else
    f19 = 4.0f;
    f20 = 12.0f;
    f21 = 21.0f;
#endif
    {
    for (i = 0, j = 0; i < 3; i++, j += 0x22) {
        const char* mRaw;
        const char* mTag;
        Vec tr, tmp, sc, pos, tmp1, scale, pos2, tmp2, scale2;
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
        mTag = tagTbl[i];
        mRaw = rawTbl[i];
        f23 = (f32)(u16)FontGetMessageWidth(mRaw);
        f22 = 0.5f * (f32)(u16)FontGetMessageWidth(msgSearch(mTag));
        winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        f18 = f28 - (f32)j;
        tmp = VEC_AT(0x36C);
        tmp.x = f25;
        tmp.y = f18 - 4.0f;
        tr = tmp;
        sc = VEC_AT(0x378);
        c41 = colTbl[i];
        WINTEXSET(0xAF, tr, sc, c41);
        winFontInit();
        tmp1 = VEC_AT(0x384);
        tmp1.x = f24 - f27 * f23;
        tmp1.y = f20 + f18;
        scale = VEC_AT(0x390);
        c6 = black2;
        pos = tmp1;
        winFontSet(&pos, &scale, &c6, mRaw);
        tmp2 = VEC_AT(0x39C);
        tmp2.x = f24 - f27 * f22;
        tmp2.y = f21 + f18;
        c7 = hiliteTbl[i];
        scale2 = VEC_AT(0x3A8);
        pos2 = tmp2;
        winFontSet(&pos2, &scale2, &c7, msgSearch(mTag));
    }
    }
    winIconInit();
    f18 = 110.0f + (x - 270.0f);
    f26 = 4.0f + (y - 38.0f);
    {
        Vec tr, tmp, sc;
        c8 = white2;
        sc = VEC_AT(0x3C0);
        tmp = VEC_AT(0x3B4);
        tmp.x = f18;
        tmp.y = f26;
        tr = tmp;
        winIconSet(0x19F, &tr, &sc, &c8);
    }
    f24 = (y - 38.0f) - 34.0f;
    f27 = 4.0f + f24;
    {
        Vec tr, tmp, sc;
        c9 = white2;
        sc = VEC_AT(0x3D8);
        tmp = VEC_AT(0x3CC);
        tmp.x = f18;
        tmp.y = f27;
        tr = tmp;
        winIconSet(0x1A0, &tr, &sc, &c9);
    }
    f25 = (y - 38.0f) - 68.0f;
    {
        Vec tr, tmp, sc;
        c10 = white2;
        sc = VEC_AT(0x3F0);
        tmp = VEC_AT(0x3E4);
        tmp.x = f18;
        tmp.y = 4.0f + f25;
        tr = tmp;
        winIconSet(0x1A1, &tr, &sc, &c10);
    }
    winFontInit();
    f18 = 130.0f + (x - 270.0f);
    f28 = 4.0f + (12.0f + (y - 38.0f));
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x3FC);
        tmp.x = f18;
        tmp.y = f28;
        scale = VEC_AT(0x408);
        pos = tmp;
        c11 = black2;
        winFontSetR(&pos, &scale, &c11, "%s", WM_ZENKAKU(pouch->currentHP));
    }
    f24 = 4.0f + (12.0f + f24);
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x414);
        tmp.x = f18;
        tmp.y = f24;
        scale = VEC_AT(0x420);
        pos = tmp;
        c12 = black2;
        winFontSetR(&pos, &scale, &c12, "%s", WM_ZENKAKU(pouch->currentFP));
    }
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x42C);
        tmp.x = f18;
        tmp.y = 4.0f + (12.0f + f25);
        scale = VEC_AT(0x438);
        pos = tmp;
        c13 = black2;
        winFontSetR(&pos, &scale, &c13, "%s", WM_ZENKAKU(pouch->maximumBP));
    }
    winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
    f18 = 190.0f + (x - 270.0f);
    {
        Vec tr, tmp, sc;
        tmp = VEC_AT(0x444);
        tmp.x = f18;
        tmp.y = f26;
        tr = tmp;
        sc = VEC_AT(0x450);
        c42 = white;
        WINTEXSET(0x10, tr, sc, c42);
    }
    {
        Vec tr, tmp, sc;
        tmp = VEC_AT(0x45C);
        tmp.x = f18;
        tmp.y = f27;
        tr = tmp;
        sc = VEC_AT(0x468);
        c43 = white;
        WINTEXSET(0x10, tr, sc, c43);
    }
    winFontInit();
    f18 = 200.0f + (x - 270.0f);
    f28 = 4.0f + (12.0f + (y - 38.0f));
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x474);
        tmp.x = f18;
        tmp.y = f28;
        scale = VEC_AT(0x480);
        pos = tmp;
        c14 = black2;
        winFontSet(&pos, &scale, &c14, "%s", WM_ZENKAKU(pouch->maximumHP));
    }
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x48C);
        tmp.x = f18;
        tmp.y = f24;
        pos = tmp;
        scale = VEC_AT(0x498);
        c15 = black2;
        winFontSet(&pos, &scale, &c15, "%s", WM_ZENKAKU(pouch->maximumFP));
    }

    if (WW_S32(work, 0x198) > 0) {
        winKirinukiGX(work, 0, 10.0f + x,
                      (20.0f + (130.0f + y)) - 20.0f,
                      260.0f, 80.0f);
        winIconInit();
        f18 = 10.0f + (40.0f + x);
        f24 = 106.0f + y;
        {
            Vec tr, tmp, sc;
            c16 = white2;
            sc = VEC_AT(0x4B0);
            tmp = VEC_AT(0x4A4);
            tmp.x = f18;
            tmp.y = f24;
            tr = tmp;
            winIconSet(0x20F, &tr, &sc, &c16);
        }
        f19 = 70.0f + y;
        {
            Vec tr, tmp, sc;
            c17 = white2;
            sc = VEC_AT(0x4C8);
            tmp = VEC_AT(0x4BC);
#ifdef MUST_MATCH
            tmp.y = f19;
            tmp.x = f18;
#else
            tmp.x = f18;
            tmp.y = f19;
#endif
            tr = tmp;
            winIconSet(0x197, &tr, &sc, &c17);
        }
        winFontInit();
        {
            Vec pos, tmp, scale;
            tmp = VEC_AT(0x4D4);
            tmp.x = 10.0f + (70.0f + x);
            tmp.y = 10.0f + f19;
            c18 = white2;
            scale = VEC_AT(0x4E0);
            pos = tmp;
            winFontSet(&pos, &scale, &c18, msgSearch(STR_AT(0x768)));
        }
        winIconInit();
        f19 = 10.0f + (80.0f + x);
        for (i = 0, j = 0; i < WW_S32(work, 0x198); i++, j += 0x16) {
            c19 = white3;
            if (i < pouch->currentSP / 100) {
                Vec tr, tmp, sc;
                sc = VEC_AT(0x4F8);
                tmp = VEC_AT(0x4EC);
                tmp.x = f19 + (f32)j;
                tmp.y = f24;
                tr = tmp;
                winIconSet(0x1B7 + i, &tr, &sc, &c19);
            } else {
                Vec tr, tmp, sc;
                sc = VEC_AT(0x510);
                tmp = VEC_AT(0x504);
                tmp.x = f19 + (f32)j;
                tmp.y = f24;
                tr = tmp;
                winIconSet(0x1BF, &tr, &sc, &c19);
            }
        }
    }

    f23 = 10.0f + x;
    winKirinukiGX(work, 0, f23,
                  20.0f + (float_10_8041d61c + y),
                  260.0f, 150.0f);
    winIconInit();
    f24 = 10.0f + (20.0f + x);
    {
        Vec tr, tmp, sc;
        c20 = white2;
        sc = VEC_AT(0x528);
        tmp = VEC_AT(0x51C);
        tmp.x = f24;
        tmp.y = 10.0f + y;
        tr = tmp;
        winIconSet(0x18C, &tr, &sc, &c20);
    }
    {
        Vec tr, tmp, sc;
        c21 = white2;
        sc = VEC_AT(0x540);
        tmp = VEC_AT(0x534);
        tmp.x = f24;
        tmp.y = 10.0f + (y - 28.0f);
        tr = tmp;
        winIconSet(0x18B, &tr, &sc, &c21);
    }
    {
        Vec tr, tmp, sc;
        c22 = white2;
        sc = VEC_AT(0x558);
        tmp = VEC_AT(0x54C);
        tmp.x = f24;
        tmp.y = 10.0f + (y - 56.0f);
        tr = tmp;
        winIconSet(0x18D, &tr, &sc, &c22);
    }
    {
        Vec tr, tmp, sc;
        c23 = white2;
        sc = VEC_AT(0x570);
        tmp = VEC_AT(0x564);
        tmp.x = f24;
        tmp.y = 10.0f + (y - 84.0f);
        tr = tmp;
        winIconSet(0x102, &tr, &sc, &c23);
    }
    {
        Vec tr, tmp, sc;
        c24 = white2;
        sc = VEC_AT(0x588);
        tmp = VEC_AT(0x57C);
        tmp.x = f24;
        tmp.y = 10.0f + (y - 112.0f);
        tr = tmp;
        winIconSet(0x1A2, &tr, &sc, &c24);
    }

    winFontInit();
    f24 = 10.0f + (35.0f + x);
    f25 = 12.0f + y;
    f26 = 10.0f + f25;
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x594);
        tmp.x = f24;
        tmp.y = f26;
        c25 = white2;
        scale = VEC_AT(0x5A0);
        pos = tmp;
        winFontSet(&pos, &scale, &c25, msgSearch(STR_AT(0x780)));
    }
    f27 = 10.0f + (f25 - 28.0f);
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x5AC);
        tmp.x = f24;
        tmp.y = f27;
        c26 = white2;
        scale = VEC_AT(0x5B8);
        pos = tmp;
        winFontSet(&pos, &scale, &c26, msgSearch(STR_AT(0x798)));
    }
    f28 = 10.0f + (f25 - 56.0f);
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x5C4);
        tmp.x = f24;
        tmp.y = f28;
        c27 = white2;
        scale = VEC_AT(0x5D0);
        pos = tmp;
        winFontSet(&pos, &scale, &c27, msgSearch(STR_AT(0x7B0)));
    }
    f29 = 10.0f + (f25 - 84.0f);
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x5DC);
#ifdef MUST_MATCH
        tmp.y = f29;
        tmp.x = f24;
#else
        tmp.x = f24;
        tmp.y = f29;
#endif
        c28 = white2;
        scale = VEC_AT(0x5E8);
        pos = tmp;
        winFontSet(&pos, &scale, &c28, msgSearch(STR_AT(0x7CC)));
    }
    f25 = 10.0f + (f25 - 112.0f);
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x5F4);
        tmp.x = f24;
        tmp.y = f25;
        c29 = white2;
        scale = VEC_AT(0x600);
        pos = tmp;
        winFontSet(&pos, &scale, &c29, msgSearch(STR_AT(0x7EC)));
    }

    f18 = 2.0f + (8.0f + (205.0f + x));
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x60C);
        tmp.x = f18;
        tmp.y = f26;
        pos = tmp;
        scale = VEC_AT(0x618);
        c30 = white2;
        winFontSetR(&pos, &scale, &c30, "%s", WM_ZENKAKU(pouch->mStarPoints));
    }
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x624);
        tmp.x = f18;
        tmp.y = f27;
        pos = tmp;
        scale = VEC_AT(0x630);
        c31 = white2;
        winFontSetR(&pos, &scale, &c31, "%s", WM_ZENKAKU(pouch->coins));
    }
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x63C);
        tmp.x = f18;
        tmp.y = f28;
        pos = tmp;
        scale = VEC_AT(0x648);
        c32 = white2;
        winFontSetR(&pos, &scale, &c32, "%s", WM_ZENKAKU(pouch->mStarPieceCount));
    }
    {
        Vec pos, tmp, scale;
        tmp = VEC_AT(0x654);
#ifdef MUST_MATCH
        tmp.y = f29;
        tmp.x = f18;
#else
        tmp.x = f18;
        tmp.y = f29;
#endif
        pos = tmp;
        scale = VEC_AT(0x660);
        c33 = white2;
        winFontSetR(&pos, &scale, &c33, "%s", WM_ZENKAKU(pouch->mShineSpriteCount));
    }

    {
        Vec pos1, tmp, scale1, pos2, tmp2, scale2;
        s64 secs;
        u32 hours;
        u32 mins;
        secs = gp->retraceLocalTime / (s64)(*(volatile u32*)0x800000F8 >> 2);
        if ((u64)secs > 359999) {
            mins = 59;
            hours = 99;
        } else {
            hours = (u32)((u64)secs / 3600);
            mins = (u32)(((u64)secs % 3600) / 60);
        }
        tmp = VEC_AT(0x66C);
        tmp.x = 2.0f + (2.0f + ((8.0f + (172.0f + x)) - 16.0f));
        tmp.y = f25;
        pos1 = tmp;
        scale1 = VEC_AT(0x678);
        c34 = white2;
        winFontSetR(&pos1, &scale1, &c34, "%s", WM_NUM(2, hours));
        tmp2 = VEC_AT(0x684);
        tmp2.x = f18;
        tmp2.y = f25;
        pos2 = tmp2;
        scale2 = VEC_AT(0x690);
        c35 = white2;
        winFontSetR(&pos2, &scale2, &c35, "%s", WM_NUM(2, mins));
    }

    winTexInit(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0));
    f19 = 10.0f + (190.0f + x);
    for (i = 0, j = 0; i < 4; i++, j += 0x1C) {
        Vec tr, tmp, sc;
        tmp = VEC_AT(0x69C);
        tmp.x = f19;
        tmp.y = 10.0f + (y - (f32)j);
        tr = tmp;
        sc = VEC_AT(0x6A8);
        c44 = white;
        WINTEXSET(0x11, tr, sc, c44);
    }
    {
        Vec tr, tmp, sc;
        tmp = VEC_AT(0x6B4);
        f24 = 220.0f + x;
        tmp.x = f24;
        tmp.y = (10.0f + (2.0f + (y - 112.0f))) - 2.0f;
        tr = tmp;
        sc = VEC_AT(0x6C0);
        c45 = white;
        WINTEXSET(0x12, tr, sc, c45);
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
        winKageGX(work, &kc, x - 150.0f, y, 0.0f, 1.0f);
        {
            Mtx mtx;
            PSMTXTrans(mtx, x - 150.0f, y, 0.0f);
            winZClear();
            animPoseDrawMtx(POSE, mtx, 2, 0.0f, 5.0f);
        }
    }

    if (WW_S32(work, 0x190) != 0) {
        winWazaGX(work, 0, x - 270.0f, 160.0f,
                  270.0f, 254.0f);
        winFontInit();
        f26 = 90.0f + x - 270.0f;
        {
            Vec pos, tmp, scale;
            c36 = white2;
            scale = VEC_AT(0x6D8);
            tmp = VEC_AT(0x6CC);
            tmp.x = f26;
            pos = tmp;
            winFontSetEdge(&pos, &scale, &c36, msgSearch(STR_AT(0x808)));
        }
        {
            Vec pos, tmp, scale;
            c37 = white2;
            scale = VEC_AT(0x6F0);
            tmp = VEC_AT(0x6E4);
            tmp.x = f24 - 270.0f;
            pos = tmp;
            winFontSetEdge(&pos, &scale, &c37, "\x82r\x82o");
        }
        f20 = 228.0f + x - 270.0f;
        f21 = 4.0f + (17.0f + f24 - 270.0f);
        f19 = 10.0f + (10.0f + x - 270.0f);
        for (i = 0, j = 0; i < WW_S32(work, 0x198); j += 0x1A, i++) {
            SuperActionEntry* sa;
            char buf[0x40];
            f32 drop;
            Vec pos1, tmp, scale1, pos2, tmp2, scale2, pos3, tmp3, scale3;
            scale1 = VEC_AT(0x708);
            tmp = VEC_AT(0x6FC);
            c38 = black3;
            sa = superActionTable[i];
            tmp.x = f19;
            tmp.y = (f32)(0x82 - j);
            pos1 = tmp;
            winFontSetLabel(&pos1, &scale1, &c38, sa->field_0);
            sprintf(buf, "%s", WM_ZENKAKU(sa->field_12));
            drop = (f32)(0x82 - j);
            scale2 = VEC_AT(0x720);
            tmp2 = VEC_AT(0x714);
            c39 = black3;
            tmp2.x = f20 - (f32)((u16)FontGetMessageWidth(buf) >> 1);
            tmp2.y = drop;
            pos2 = tmp2;
            winFontSet(&pos2, &scale2, &c39, buf);
            scale3 = VEC_AT(0x738);
            tmp3 = VEC_AT(0x72C);
            tmp3.x = f21;
            tmp3.y = (f32)(0x78 - j);
            pos3 = tmp3;
            winFontSet(&pos3, &scale3, &black, "\x82r\x82o");
        }
    }

#undef POSE
#undef STR_AT
#undef GXC_AT
#undef VEC_AT
}

#ifndef MUST_MATCH
#undef c1
#undef c2
#undef c3
#undef c4
#undef c40
#undef c5
#undef c41
#undef c6
#undef c7
#undef c8
#undef c9
#undef c10
#undef c11
#undef c12
#undef c13
#undef c42
#undef c43
#undef c14
#undef c15
#undef c16
#undef c17
#undef c18
#undef c19
#undef c20
#undef c21
#undef c22
#undef c23
#undef c24
#undef c25
#undef c26
#undef c27
#undef c28
#undef c29
#undef c30
#undef c31
#undef c32
#undef c33
#undef c34
#undef c35
#undef c44
#undef c45
#undef c36
#undef c37
#undef c38
#undef c39
#undef black2
#undef black3
#endif
#pragma inline_max_total_size reset
#pragma inline_max_size reset

// ---- named data, defined after the code so MWCC does not fold it ----
#pragma explicit_zero_data on
#pragma force_active on
// .rodata named objects in address order
const Vec vec3_802f5824 = { 0, 0, 0 };
const Vec vec3_802f5830 = { 0.8, 0.8, 0.8 };
const Vec vec3_802f583c = { 0, 0, 0 };
const Vec vec3_802f5848 = { 0.8, 0.8, 0.8 };
const Vec vec3_802f5854 = { 0, 0, 0 };
const Vec vec3_802f5860 = { 0.7, 0.7, 0.7 };
const Vec vec3_802f586c = { 0, 0, 0 };
const Vec vec3_802f5878 = { -0.8, 0.8, 0.8 };
const Vec vec3_802f5884 = { 0, 0, 0 };
const Vec vec3_802f5890 = { -0.8, 0.8, 0.8 };
const Vec vec3_802f589c = { 0, 0, 0 };
const Vec vec3_802f58a8 = { 0.7, 0.7, 0.7 };
const Vec vec3_802f58b4 = { 0, 0, 0 };
const Vec vec3_802f58c0 = { 0.8, 0.8, 0.8 };
const Vec vec3_802f58cc = { 0, 0, 0 };
const Vec vec3_802f58d8 = { 0.6, 0.6, 0.6 };
const Vec vec3_802f58e4 = { 0, 0, 0 };
const Vec vec3_802f58f0 = { 0.8, 0.8, 0.8 };
const Vec vec3_802f58fc = { 0, 0, 0 };
const Vec vec3_802f5908 = { 0.8, 0.8, 0.8 };
const Vec vec3_802f5914 = { 0, 0, 0 };
const Vec vec3_802f5920 = { 1, 1, 1 };
const Vec vec3_802f592c = { 0, 0, 0 };
const Vec vec3_802f5938 = { 0.7, 0.7, 0.7 };
const GXColor dat_802f5944[3] = { { 0xCB, 0x8C, 0xA2, 0xFE }, { 0xCB, 0xBE, 0x89, 0xFE }, { 0x94, 0xB9, 0xC7, 0xFE } };
const GXColor dat_802f5950[3] = { { 0xA7, 0x3C, 0x3C, 0xFF }, { 0x37, 0x69, 0x1E, 0xFF }, { 0x0F, 0x21, 0x7A, 0xFF } };
const char* const N_stat_name_tbl[3] = { "\x82g\x82o", "\x82""e\x82o", "\x82""a\x82o" };
const char str_802F5968[24] = "msg_menu_mario_ruby_hp\x00\x00";
const char str_802F5980[24] = "msg_menu_mario_ruby_fp\x00\x00";
const char str_802F5998[24] = "msg_menu_mario_ruby_bp\x00\x00";
const GXColor dat_8041d608 = { 0x00, 0x00, 0x00, 0x80 };
__declspec(section ".rodata") const u32 jumptable_802F59B0[3] = { 0, 0, 0 };
const Vec vec3_802f59bc = { 0, 0, 0 };
const Vec vec3_802f59c8 = { 1, 1, 1 };
const Vec vec3_802f59d4 = { 0, 0, 0 };
const Vec vec3_802f59e0 = { 1, 1, 1 };
const Vec vec3_802f59ec = { 0, 0, 0 };
const Vec vec3_802f59f8 = { 0.5, 0.5, 0.5 };
const Vec vec3_802f5a04 = { 0, 0, 0 };
const Vec vec3_802f5a10 = { 0.82, 0.82, 0.82 };
const Vec vec3_802f5a1c = { 0, 0, 0 };
const Vec vec3_802f5a28 = { 0.82, 0.82, 0.82 };
const Vec vec3_802f5a34 = { 0, 0, 0 };
const Vec vec3_802f5a40 = { 0.82, 0.82, 0.82 };
const Vec vec3_802f5a4c = { 0, 0, 0 };
const Vec vec3_802f5a58 = { 1, 1, 1 };
const Vec vec3_802f5a64 = { 0, 0, 0 };
const Vec vec3_802f5a70 = { 1, 1, 1 };
const Vec vec3_802f5a7c = { 0, 0, 0 };
const Vec vec3_802f5a88 = { 1, 1, 1 };
const Vec vec3_802f5a94 = { 0, 0, 0 };
const Vec vec3_802f5aa0 = { 1, 1, 1 };
const Vec vec3_802f5aac = { 0, 0, 0 };
const Vec vec3_802f5ab8 = { 1, 1, 1 };
const Vec vec3_802f5ac4 = { 0, 0, 0 };
const Vec vec3_802f5ad0 = { 1, 1, 1 };
const Vec vec3_802f5adc = { 0, 0, 0 };
const Vec vec3_802f5ae8 = { 1, 1, 1 };
const Vec vec3_802f5af4 = { 0, 0, 0 };
const Vec vec3_802f5b00 = { 0.8, 0.8, 0.8 };
const Vec vec3_802f5b0c = { 0, 0, 0 };
const Vec vec3_802f5b18 = { 0.8, 0.8, 0.8 };
const Vec vec3_802f5b24 = { 0, 0, 0 };
const Vec vec3_802f5b30 = { 1, 1, 1 };
const Vec vec3_802f5b3c = { 0, 0, 0 };
const Vec vec3_802f5b48 = { 0.8, 0.8, 0.8 };
const Vec vec3_802f5b54 = { 0, 0, 0 };
const Vec vec3_802f5b60 = { 0.8, 0.8, 0.8 };
const Vec vec3_802f5b6c = { 0, 0, 0 };
const Vec vec3_802f5b78 = { 0.62, 0.62, 0.62 };
const Vec vec3_802f5b84 = { 0, 0, 0 };
const Vec vec3_802f5b90 = { 0.62, 0.62, 0.62 };
const Vec vec3_802f5b9c = { 0, 0, 0 };
const Vec vec3_802f5ba8 = { 0.62, 0.62, 0.62 };
const Vec vec3_802f5bb4 = { 0, 0, 0 };
const Vec vec3_802f5bc0 = { 0.62, 0.62, 0.62 };
const Vec vec3_802f5bcc = { 0, 0, 0 };
const Vec vec3_802f5bd8 = { 0.62, 0.62, 0.62 };
const Vec vec3_802f5be4 = { 0, 0, 0 };
const Vec vec3_802f5bf0 = { 0.9, 0.9, 0.9 };
const Vec vec3_802f5bfc = { 0, 0, 0 };
const Vec vec3_802f5c08 = { 0.9, 0.9, 0.9 };
const Vec vec3_802f5c14 = { 0, 0, 0 };
const Vec vec3_802f5c20 = { 0.9, 0.9, 0.9 };
const Vec vec3_802f5c2c = { 0, 0, 0 };
const Vec vec3_802f5c38 = { 0.9, 0.9, 0.9 };
const Vec vec3_802f5c44 = { 0, 0, 0 };
const Vec vec3_802f5c50 = { 0.9, 0.9, 0.9 };
const Vec vec3_802f5c5c = { 0, 0, 0 };
const Vec vec3_802f5c68 = { 0.9, 0.9, 0.9 };
const Vec vec3_802f5c74 = { 0, 0, 0 };
const Vec vec3_802f5c80 = { 0.9, 0.9, 0.9 };
const Vec vec3_802f5c8c = { 0, 0, 0 };
const Vec vec3_802f5c98 = { 0.9, 0.9, 0.9 };
const Vec vec3_802f5ca4 = { 0, 0, 0 };
const Vec vec3_802f5cb0 = { 0.9, 0.9, 0.9 };
const Vec vec3_802f5cbc = { 0, 0, 0 };
const Vec vec3_802f5cc8 = { 0.9, 0.9, 0.9 };
const Vec vec3_802f5cd4 = { 0, 0, 0 };
const Vec vec3_802f5ce0 = { 0.9, 0.9, 0.9 };
const Vec vec3_802f5cec = { 0, 0, 0 };
const Vec vec3_802f5cf8 = { 1, 1, 1 };
const Vec vec3_802f5d04 = { 0, 0, 0 };
const Vec vec3_802f5d10 = { 1, 1, 1 };
const Vec vec3_802f5d1c = { 0, 155, 0 };
const Vec vec3_802f5d28 = { 0.8, 0.8, 0.8 };
const Vec vec3_802f5d34 = { 0, 155, 0 };
const Vec vec3_802f5d40 = { 0.8, 0.8, 0.8 };
const Vec vec3_802f5d4c = { 0, 0, 0 };
const Vec vec3_802f5d58 = { 1, 1, 1 };
const Vec vec3_802f5d64 = { 0, 0, 0 };
const Vec vec3_802f5d70 = { 1, 1, 1 };
const Vec vec3_802f5d7c = { 0, 0, 0 };
const Vec vec3_802f5d88 = { 0.5, 0.5, 0.5 };
const char str_802F5D94[12] = "name_mario\x00\x00";
const char str_802F5DA0[24] = "msg_menu_mario_level\x00\x00\x00\x00";
const char str_802F5DB8[24] = "msg_menu_mario_special\x00\x00";
const char str_802F5DD0[24] = "msg_menu_mario_num_sp\x00\x00\x00";
const char str_802F5DE8[24] = "msg_menu_mario_num_coin\x00";
const char str_802F5E00[28] = "msg_menu_mario_num_hoshi\x00\x00\x00\x00";
const char str_802F5E1C[32] = "msg_menu_mario_num_super_coin\x00\x00\x00";
const char str_802F5E3C[28] = "msg_menu_mario_num_playtime\x00";
const char str_802F5E58[24] = "msg_menu_mario_sac_waza\x00";


// .data: link table
LinkDt linkDt[18] = {
    { (s16)0xFEF4, (s16)0x008E, str_8041D578, NULL, str_8041D580, 0x0000, 0x0000, WM_MSG_BASE, { 0x06, 0x01, 0x0B, 0x09 } },
    { (s16)0xFEF4, (s16)0x0076, str_8041D578, NULL, str_8041D580, 0x0000, 0x0000, str_802F5664, { 0x00, 0x02, 0x0B, 0x07 } },
    { (s16)0xFEF0, (s16)0x0044, str_8041D584, NULL, str_8041D58C, 0x0009, 0x0000, NULL, { 0x01, 0x03, 0x0B, 0x07 } },
    { (s16)0xFF0E, (s16)0x0018, str_8041D590, NULL, str_8041D58C, 0x0006, 0x0000, NULL, { 0x02, 0x04, 0x0B, 0x08 } },
    { (s16)0xFEF2, (s16)0xFFDE, str_8041D578, NULL, str_8041D580, 0x0000, 0x0000, str_802F567C, { 0x03, 0x05, 0x0D, 0x0D } },
    { (s16)0xFEF2, (s16)0xFFBC, str_8041D578, NULL, str_8041D580, 0x0000, 0x0000, str_802F5690, { 0x04, 0x06, 0x0D, 0x0D } },
    { (s16)0xFEF2, (s16)0xFF9A, str_8041D578, NULL, str_8041D580, 0x0000, 0x0000, str_802F56A4, { 0x05, 0x00, 0x0D, 0x0D } },
    { (s16)0xFF9C, (s16)0x005A, str_8041D578, str_8041D578, str_8041D580, 0x0002, 0x0000, str_802F56B8, { 0x09, 0x0A, 0x02, 0x09 } },
    { (s16)0xFF9C, (s16)0x0014, str_8041D578, str_8041D578, str_8041D580, 0x0004, 0x0000, str_802F56C4, { 0x0A, 0x04, 0x03, 0x0A } },
    { (s16)0xFFCE, (s16)0x0082, str_8041D578, str_8041D578, str_8041D580, 0x0005, 0x0000, str_802F56D0, { 0x00, 0x07, 0x00, 0x0B } },
    { (s16)0xFFCE, (s16)0x003C, str_8041D578, str_8041D578, str_8041D580, 0x0003, 0x0000, str_802F56DC, { 0x07, 0x08, 0x08, 0x0C } },
    { (s16)0x0014, (s16)0x006A, str_8041D578, NULL, str_8041D580, 0x0000, 0x0000, str_802F56E8, { 0x11, 0x0C, 0x09, 0x00 } },
    { (s16)0x0014, (s16)0x0046, str_8041D578, NULL, str_8041D580, 0x0000, 0x0000, str_802F56FC, { 0x0B, 0x0D, 0x0A, 0x00 } },
    { (s16)0x0003, (s16)0x000A, str_8041D578, NULL, str_8041D580, 0x0000, 0x0000, str_802F5710, { 0x0C, 0x0E, 0x04, 0x04 } },
    { (s16)0x0003, (s16)0xFFEE, str_8041D578, NULL, str_8041D580, 0x0000, 0x0000, str_802F5728, { 0x0D, 0x0F, 0x04, 0x04 } },
    { (s16)0x0003, (s16)0xFFD2, str_8041D578, NULL, str_8041D580, 0x0000, 0x0000, str_802F573C, { 0x0E, 0x10, 0x04, 0x04 } },
    { (s16)0x0003, (s16)0xFFB6, str_8041D578, NULL, str_8041D580, 0x0000, 0x0000, str_802F5754, { 0x0F, 0x11, 0x04, 0x04 } },
    { (s16)0x0003, (s16)0xFF9A, str_8041D578, NULL, str_8041D580, 0x0000, 0x0000, str_802F576C, { 0x10, 0x0B, 0x04, 0x04 } },
};

__declspec(section ".data") const u32 gap_05_80374F34_data = 0;

#pragma force_active off
#pragma explicit_zero_data reset
