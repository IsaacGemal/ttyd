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
char tmp2_708[0x100];

// .rodata / .sdata2 objects this unit reads but does not define yet
extern const char str_8041D630[];
extern const char str_8041D638[];
extern const char str_8041D578[];
extern const char str_8041D5A8[];
extern const char str_8041D5AC[];
extern const char str_8041D5B0[];
extern const char str_8041D5B4[];
extern const char str_8041D5B8[];
extern const char str_8041D5BC[];
extern const char str_8041D5C0[];
extern const char str_8041D5C4[];
extern const char str_8041D5C8[];
extern const char str_8041D5CC[];
extern const f32 float_0_8041d634;
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
    u32 field_18;         // 0x18, four 1-byte "next" indices
} LinkDt;

typedef struct SuperActionEntry {
    u8 field_0[0xC];      // 0x0
    const char* field_C;  // 0xC
} SuperActionEntry;

extern LinkDt linkDt[];
extern SuperActionEntry* superActionTable[];
extern const char* hammer_pose[];
extern const f32 float_neg280_8041d72c;
extern const f32 float_120_8041d730;
extern const f32 float_26_8041d734;
extern const f32 float_0p89_8041d728;
// .sdata lookup tables (window "help" message ids per upgrade level)
const char* hammer_help[4] = { NULL, "msg_hammer_1", "msg_hammer_2", "msg_hammer_3" };
const char* boots_help[4] = { NULL, "msg_boots_1", "msg_boots_2", "msg_boots_3" };

// NONMATCHING (69.65%): the original copies its digit table from
// jumptable_802F57D4 and this file's .rodata pool is still incomplete, so the
// pool base/offsets differ; `out` also stays in a register instead of reloading.
char* unk_8016af5c(s32 num, s32 width) {
    char buf[0x100];
    const char* tbl[10] = {
        str_8041D5A8, str_8041D5AC, str_8041D5B0, str_8041D5B4, str_8041D5B8,
        str_8041D5BC, str_8041D5C0, str_8041D5C4, str_8041D5C8, str_8041D5CC,
    };
    char* out = unk_803dfbf8;
    s32 i;

    if (num < 0) {
        strcpy(out, str_8041D630);
        num = -num;
    } else {
        strcpy(out, (const char*)&float_0_8041d634);
    }
    sprintf(buf, str_8041D638, num);
    for (i = 0; i < width; i++) {
        if (i < width - (s32)strlen(buf)) {
            strcat(out, str_8041D5A8);
        } else {
            strcat(out, tbl[buf[i - (width - (s32)strlen(buf))] - 0x30]);
        }
    }
    return out;
}

// NONMATCHING (76.40%): same digit-table/pool issue as unk_8016af5c; no `width`
// padding here.
char* winZenkakuStr(s32 num) {
    char buf[0x100];
    const char* tbl[10] = {
        str_8041D5A8, str_8041D5AC, str_8041D5B0, str_8041D5B4, str_8041D5B8,
        str_8041D5BC, str_8041D5C0, str_8041D5C4, str_8041D5C8, str_8041D5CC,
    };
    char* out = tmp2_708;
    s32 i;

    if (num < 0) {
        strcpy(out, str_8041D630);
        num = -num;
    } else {
        strcpy(out, (const char*)&float_0_8041d634);
    }
    sprintf(buf, str_8041D638, num);
    for (i = 0; i < (s32)strlen(buf); i++) {
        strcat(out, tbl[buf[i] - 0x30]);
    }
    return out;
}

void winMarioMain2(WindowWork* work) {
    (void)work;
}

// NONMATCHING (0%): the compile has the same 901 instructions as the original
// but they align differently; the original's literals pool into str_802F5650
// (only complete once the whole unit exists) and its sda2 names are objects.
s32 winMarioMain(WindowWork* work) {
    const char* anim;
    LinkDt* d;
    f32 f2;

    if (WW_S32(work, 0x190) == 1) {
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
        WW_F32(work, 0x15C) = 10.0f + ((float_120_8041d730 - float_26_8041d734 * f2) - 12.0f);
        winMsgEntry(work, 0, superActionTable[WW_S32(work, 0x194)]->field_C, 0);
        return 0;
    }
    if (WW_S32(work, 0x190) != 0) {
        return 0;
    }

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
            if (strcmp("S_1", "S_1") == 0) {
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
            } else {
                anim = "R_1";
                switch (marioGetColor()) {
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
            anim = "M_S_1";
        }
        animPoseSetAnim(WW_S32(work, 0x188), anim, 0);
        return -1;
    }
    if (work->buttonTrg & 0x1000) {
        return -2;
    }

#define MARIO_TRAVERSE(off)                                                        \
    do {                                                                           \
        WW_S32(work, 0x160) = *(u8*)((u8*)&linkDt[WW_S32(work, 0x160)] + (0x18 + (off))); \
        while (1) {                                                                \
            d = &linkDt[WW_S32(work, 0x160)];                                      \
            if (WW_S32(work, 0x160) == 0xb || WW_S32(work, 0x160) == 0xc) {        \
                if (WW_S32(work, 0x198) != 0) {                                    \
                    break;                                                         \
                }                                                                  \
            } else {                                                               \
                if (d->field_10 == 0) {                                            \
                    break;                                                         \
                }                                                                  \
                if (pouchCheckItem(d->field_10) != 0) {                            \
                    break;                                                         \
                }                                                                  \
            }                                                                      \
            WW_S32(work, 0x160) = *(u8*)((u8*)&linkDt[WW_S32(work, 0x160)] + (0x18 + (off))); \
        }                                                                          \
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

    if (WW_S32(work, 0x160) == 2) {
        if (work->flags & 0x2000) {
            if (strcmp(d->field_C, "S_1") == 0) {
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
            } else {
                anim = "R_1";
                switch (marioGetColor()) {
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
            if (animPoseGetLoopTimes(WW_S32(work, 0x188)) >= float_0p89_8041d728) {
                WW_S32(work, 0x18C) = 30;
            }
        } else {
            WW_S32(work, 0x18C) -= 1;
        }
        if (work->flags & 0x2000) {
            WW_S32(work, 0x18C) = 0;
        }
    } else {
        WW_S32(work, 0x18C) = 0;
        if (work->flags & 0x2000) {
            if (strcmp(d->field_C, "S_1") == 0) {
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
            } else {
                anim = "R_1";
                switch (marioGetColor()) {
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
            anim = d->field_4;
        }
        animPoseSetAnim(WW_S32(work, 0x188), anim, 0);
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

// NONMATCHING (91.48%): the body is correct; only the pooled literals
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
