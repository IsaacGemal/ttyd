#include "window/win_mario.h"
#include "window/win_root.h"
#include "driver/animdrv.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "pmario_sound.h"
#include <string.h>
#include <stdio.h>

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
    u8 field_0[0xC];      // 0x0
    const char* field_C;  // 0xC
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

// NONMATCHING (99.57%): tail `d`/linkDt base in r31 (target r30), tail `anim` in r30 (target r31);
// MWCC coalesces the early/tail `anim`; the int->float magic is a local @442@ not double_to_int_802f5e78
// (the latter resolves with the unit's full .rodata pool).
s32 winMarioMain(WindowWork* work) {
    LinkDt* d;
    const char* anim;
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
        if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            if (work->flags & 0x2000) {
                if (strcmp(str_8041D580, str_8041D580) == 0) {
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
                anim = str_8041D578;
            }
            animPoseSetAnim(WW_S32(work, 0x188), anim, 0);
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
        d = &linkDt[WW_S32(work, 0x160)];
        WW_F32(work, 0x158) = (f32)d->field_0;
        WW_F32(work, 0x15C) = (f32)d->field_2;

        switch (WW_S32(work, 0x160)) {
        case 2:
            if (work->flags & 0x2000) {
                if (strcmp(d->field_C, str_8041D580) == 0) {
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
                if (strcmp(d->field_C, str_8041D580) == 0) {
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
                anim = d->field_4;
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
            winMsgEntry(work, 0, d->field_14, 0);
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

// NONMATCHING (96.58%): the body is correct; only the pooled literals
// (str_802F5650 base + offset) and callee-saved register rotation differ, both
// of which resolve once the rest of the unit's literals exist.
void winMarioInit(WindowWork* work) {
    PouchData* pouch = pouchGetPtr();
    const char* anim;
    s32 i;

    if (work->flags & 0x2000) {
        anim = "S_1";
        switch (marioGetColor()) {
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
        WW_S32(work, 0x188) = animPoseEntry("d_mario", 0);
        animPoseSetAnim(WW_S32(work, 0x188), anim, 1);
    } else {
        anim = "a_mario";
        switch (marioGetColor()) {
        case 1:
            anim = "a_mario_l";
            break;
        case 2:
            anim = "a_mario_w";
            break;
        case 3:
            anim = "a_mario_wl";
            break;
        }
        WW_S32(work, 0x188) = animPoseEntry(anim, 0);
        animPoseSetAnim(WW_S32(work, 0x188), str_8041D578, 1);
    }

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
