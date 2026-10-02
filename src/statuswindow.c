#include "pmario_sound.h"
#include "statuswindow.h"
#include "battle/battle.h"
#include "driver/icondrv.h"
#include "driver/seqdrv.h"
#include "driver/swdrv.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "memory.h"
#include <math.h>
#include <string.h>
#include "mario/mario_party.h"

extern GlobalWork* gp;

// .sdata2 colour constants of this unit (white with alpha), passed to the icon draw functions.
__declspec(section ".sdata2") u32 dat_8041c8e0 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041c8e4 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041c8e8 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041c8ec = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041c8f0 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041c8f4 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041c8f8 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041c8fc = 0xFFFFFFFF;

//.data
typedef struct alwaysDtEntry {
    s16 xpos;   // 0x0
    s16 ypos;   // 0x2
    u16 iconId; // 0x4
    f32 scale;  // 0x8
} alwaysDtEntry;

alwaysDtEntry alwaysDt[] = {
    { 108, 40, 464, 1.0f }, // red hp banner
    { 38, 40, 419, 1.0f },  // mario hp icon
    { 124, 34, 472, 1.0f }, // division sign
    { 288, 40, 465, 1.0f }, // orange fp banner
    { 220, 40, 433, 1.0f }, // fp icon
    { 305, 34, 472, 1.0f }, // division sign
    { 492, 40, 467, 1.0f }, // green item banner
    { 400, 40, 435, 1.0f }, // star points
    { 490, 40, 436, 1.0f }, // coin icon
    { 422, 34, 470, 1.0f }, // "x"
    { 512, 34, 470, 1.0f }, // "x"
    { 360, 76, 466, 1.0f }, // blue sp banner
    { 220, 80, 434, 0.9f }  // star
};

alwaysDtEntry partysDt[3] = {
    { 108, 76, 464, 1.0f },
    { 38, 80, 420, 1.0f },
    { 124, 70, 472, 1.0f },
};

u16 party_icon[8] = {
    0x01A4, 0x01A4, 0x01A5, 0x01A6, 0x01A7, 0x01AE, 0x01AF, 0x01B0,
};

u16 gauge_back[8] = {
    0x01B7, 0x01B8, 0x01B9, 0x01BA, 0x01BB, 0x01BC, 0x01BD, 0x01BE,
};

u16 gauge_wakka[16] = {
    0x01C0, 0x01C1, 0x01C2, 0x01C3, 0x01C4, 0x01C5, 0x01C6, 0x01C7,
    0x01C8, 0x01C9, 0x01CA, 0x01CB, 0x01CC, 0x01CD, 0x01CE, 0x01CF,
};

//.sbss
static StatusWindowWork* wp;

// Local mirror of the opaque StatusValue from the header (fields not declared
// there). 0x16 bytes, padded to 0x18 on the stack.
typedef struct StatusWindowValue {
    s16 hp;         // 0x0
    s16 maxHp;      // 0x2
    s16 fp;         // 0x4
    s16 maxFp;      // 0x6
    s16 partyId;    // 0x8
    s16 partyHp;    // 0xA
    s16 partyMaxHp; // 0xC
    s16 starPoint;  // 0xE
    s16 coin;       // 0x10
    s16 ap;         // 0x12
    s16 maxAp;      // 0x14
} StatusWindowValue;

// local prototypes


void statusWinInit(void) {
    wp = __memAlloc(HEAP_DEFAULT, sizeof(StatusWindowWork));
    memset(wp, 0, sizeof(StatusWindowWork));
    wp->field_0x1C = 1.0f;
    wp->field_0x30 = -304.0f;
    wp->field_0x34 = 208.0f;
    wp->field_0x40 = -304.0f;
    wp->field_0x44 = 334.0f;
    wp->field_0x74 = 0;
    wp->field_0x78 = 0;
    wp->field_0x4 &= ~0xA;
    wp->field_0x4 |= 5;
}

// TODO: #defines?
void statusWinReInit(void) {
    wp->field_0x74 = 0;
    wp->field_0x78 = 0;
    if (wp->field_0x4 & 0x10) {
        wp->field_0x4 &= ~0x10;
        wp->field_0x4 &= ~0xA;
        wp->field_0x4 |= 5;
    } else if (wp->field_0x4 & 1) {
        wp->field_0x4 &= ~0xA;
        wp->field_0x4 |= 5;
    }
    if (wp->field_0x4 & 0x20) {
        wp->field_0x4 &= ~0x20;
        wp->field_0x4 |= 0xA;
        wp->field_0x4 &= ~5;
    } else if (wp->field_0x4 & 2) {
        wp->field_0x4 |= 0xA;
        wp->field_0x4 &= ~5;
    }
}

// The three negated `wp->field_0x4 & 0x11` tests materialise the boolean and branch `bne skip` in the
// original; a plain if folds the materialisation and a switch lowers `case 0` out of line (`beq` +
// extra b), so they use the `(x ? 1 : 0)` shape under MUST_MATCH.
// The cos()/lerp tail keeps the wrong FPRs: a plain `f32 c` rounds the cos() result into f2
// (target f1) and the compiler then swaps the lerp's 1.0 and field_0x18 between f0/f2, leaving 6
// FP instructions with the wrong register. Everything else is byte-identical.
// NONMATCHING (99.88%): only FPR allocation in the cos()/lerp tail differs.
void statusWinMain(void) {
    MarioWork* mario;

    mario = marioGetPtr();
    if (mario->characterId != 0) {
        wp->field_0x4 = 2;
        wp->field_0xC = 1.0f;
        wp->field_0x8 = 1.0f;
        wp->field_0x18 = 0.0f;
        wp->field_0x10 = 0.0f;
    } else if (strcmp(gp->currentAreaName, "yuu") != 0) {
        switch (seqGetSeq()) {
            case SEQ_GAME:
            case SEQ_MAP_CHANGE:
                {
                    BOOL notCtrl;
                    BOOL notKey;

                    notCtrl = !marioChkCtrl();
                    if (notCtrl || (notKey = !marioChkKey(), notKey) || !(mario->flags & 1)) {
#ifdef MUST_MATCH
                        switch ((wp->field_0x4 & 0x11) != 0) {
                        default:
                            wp->field_0x4 |= 0xA;
                            wp->field_0x4 &= ~5;
                            break;
                        case 0:
                            break;
                        }
#else
                        if (wp->field_0x4 & 0x11) {
                            wp->field_0x4 |= 0xA;
                            wp->field_0x4 &= ~5;
                        }
#endif
                        valueCheck();
                        wp->field_0x74 = 0;
                        wp->field_0x78 = 0;
                    } else {
                        if (PSVECDistance(&wp->field_0x68, &mario->position) < 1.0f) {
                            wp->field_0x74++;
                            if (wp->field_0x74 > gp->framerate * 10) {
                                wp->field_0x74 = gp->framerate * 10;
#ifdef MUST_MATCH
                                if (wp->field_0x7C == 0 && wp->field_0x78 == 0 && !(wp->field_0x4 & 0x11 ? 1 : 0)) {
#else
                                if (wp->field_0x7C == 0 && wp->field_0x78 == 0 && !(wp->field_0x4 & 0x11)) {
#endif
                                    wp->field_0x4 &= ~0xA;
                                    wp->field_0x4 |= 5;
                                    wp->field_0x4 |= 0x100;
                                }
                            }
                        } else {
                            wp->field_0x74 = 0;
                            wp->field_0x7C = 0;
#ifdef MUST_MATCH
                            if (wp->field_0x78 == 0) {
                                switch ((wp->field_0x4 & 0x11) != 0) {
                                default:
                                    wp->field_0x4 |= 0xA;
                                    wp->field_0x4 &= ~5;
                                    break;
                                case 0:
                                    break;
                                }
                            }
#else
                            if (wp->field_0x78 == 0 && (wp->field_0x4 & 0x11)) {
                                wp->field_0x4 |= 0xA;
                                wp->field_0x4 &= ~5;
                            }
#endif
                        }
                        if (*(u16*)(mario->field_0x245 + 3) & 0x10) {
#ifdef MUST_MATCH
                            if (!(wp->field_0x4 & 0x11 ? 1 : 0)) {
#else
                            if (!(wp->field_0x4 & 0x11)) {
#endif
                                wp->field_0x4 &= ~0xA;
                                wp->field_0x4 |= 5;
                                wp->field_0x78 = gp->framerate * 10;
                                psndSFXOn((const char*)0x2A);
                                wp->field_0x4 |= 0x100;
                            } else {
                                wp->field_0x4 |= 0xA;
                                wp->field_0x4 &= ~5;
                                wp->field_0x78 = gp->framerate * 10;
                                wp->field_0x7C = 1;
                                psndSFXOn((const char*)0x2B);
                            }
                        }
                        if (valueCheck() != 0) {
                            wp->field_0x78 = gp->framerate * 2;
#ifdef MUST_MATCH
                            if (!(wp->field_0x4 & 0x11 ? 1 : 0)) {
#else
                            if (!(wp->field_0x4 & 0x11)) {
#endif
                                wp->field_0x4 &= ~0xA;
                                wp->field_0x4 |= 5;
                            }
                        }
                    }
                }
                if (wp->field_0x78 != 0) {
                    wp->field_0x78--;
                }
                wp->field_0x68 = mario->position;
                if (wp->field_0x4 & 0x10) {
                    wp->field_0x4 &= ~0xA;
                }
                if (wp->field_0x4 & 0x20) {
                    wp->field_0x4 &= ~5;
                }
                break;
            case SEQ_BATTLE:
                break;
            default:
#ifdef MUST_MATCH
                switch ((wp->field_0x4 & 0x11) != 0) {
                default:
                    wp->field_0x4 |= 0xA;
                    wp->field_0x4 &= ~5;
                    break;
                case 0:
                    break;
                }
#else
                if (wp->field_0x4 & 0x11) {
                    wp->field_0x4 |= 0xA;
                    wp->field_0x4 &= ~5;
                }
#endif
                break;
        }

        if ((wp->field_0x4 & 0x4) || (wp->field_0x4 & 0x8) || (wp->field_0x4 & 0x40) || (wp->field_0x4 & 0x80)) {
            *(f32*)((u8*)wp + 0x14) = wp->field_0x10;
            wp->field_0x18 = (wp->field_0x4 & 0x44) ? 1.0f : 0.0f;
            wp->field_0x8 = 0.0f;
            wp->field_0xC = (wp->field_0x4 & 0x44) ? 16.0f / wp->field_0x1C : 28.0f / wp->field_0x1C;
            wp->field_0x4 &= ~4;
            wp->field_0x4 &= ~8;
            wp->field_0x4 &= ~0x40;
            wp->field_0x4 &= ~0x80;
        }

        wp->field_0x8 += 1.0f;
        if (wp->field_0x8 >= wp->field_0xC) {
            wp->field_0x8 = wp->field_0xC;
            wp->field_0x10 = wp->field_0x18;
            if (wp->field_0x10 == 0.0f) {
                wp->field_0x4 &= ~0x100;
            }
        } else {
#ifdef MUST_MATCH
            // The original's cos() result is rounded in place in f1 (frsp f1,f1), which a plain
            // `f32 c` does not reproduce; keeping it in an f64 temp does. The `cc = cc;` SSA copy is
            // what makes MWCC put the lerp's 1.0/0.5 in f0 like the original (without it they land
            // in f2). Both are codegen-only; the #else is the plain form with identical semantics.
            f64 c;
            f32 cc;

            c = cos(3.1415927f * wp->field_0x8 / wp->field_0xC);
            cc = 1.0f - (f32)c;
            cc = cc;
            wp->field_0x10 = (wp->field_0x18 - *(f32*)wp->field_0x14) * cc * 0.5f + *(f32*)wp->field_0x14;
#else
            f32 c;

            c = (f32)cos(3.1415927f * wp->field_0x8 / wp->field_0xC);
            wp->field_0x10 = (wp->field_0x18 - *(f32*)wp->field_0x14) * (1.0f - c) * 0.5f + *(f32*)wp->field_0x14;
#endif
        }

        {
#ifdef MUST_MATCH
            // The original materialises `t = 1.0f` (f1) and then `t = 1.0f - x` (result f2); with a
            // plain `t -= x` MWCC reuses one register and the 1.0 load lands in f2. Codegen-only.
            f32 t = 1.0f;
            f32 x = wp->field_0x10;
            t = 1.0f - x;
#else
            f32 x = wp->field_0x10;
            f32 t = 1.0f - x;
#endif
            wp->field_0x20 = wp->field_0x30 * x + wp->field_0x40 * t;
            wp->field_0x24 = wp->field_0x34 * x + wp->field_0x44 * t;
        }
        valueUpdate();
        if (!(wp->field_0x4 & 0x1000)) {
            dispEntry(8, 500.0f, 0, statusWinDisp, NULL);
        }
    }
}

void statusPartyHPBlink(s32 value) {
    wp->field_0x80 |= 8;
    wp->field_0x88 = value;
}

void statusMarioHPBlink(s32 value) {
    wp->field_0x80 |= 1;
    wp->field_0x88 = value;
}

void statusFPBlink(s32 value) {
    wp->field_0x80 |= 2;
    wp->field_0x88 = value;
}

void N_statusClearBlink(void) {
    wp->field_0x80 = 0;
}

void statusAPBlink(s32 value) {
    wp->field_0x80 = 0;
    wp->field_0x80 |= 4;
    wp->field_0x88 = value;
}

Vec statusGetApPos(void) {
    Vec pos;

    pos.x = wp->field_0x20 + alwaysDt[12].xpos;
    pos.y = wp->field_0x24 - alwaysDt[12].ypos;
    pos.z = 0.0f;
    return pos;
}

void statusWinForceUpdateCoin(void) {
    StatusWindowValue values;

    statusGetValue(&values);
    *(s16*)((u8*)wp + 0x60) = values.coin;
}

void statusWinForceUpdate(void) {
    StatusWindowValue values;

    statusGetValue(&values);
    *(s16*)((u8*)wp + 0x50) = values.hp;
    *(s16*)((u8*)wp + 0x52) = values.maxHp;
    *(s16*)((u8*)wp + 0x54) = values.fp;
    *(s16*)((u8*)wp + 0x56) = values.maxFp;
    wp->field_0x62 = values.ap;
    *(s16*)((u8*)wp + 0x64) = values.maxAp;
    *(s16*)((u8*)wp + 0x5E) = values.starPoint;
    *(s16*)((u8*)wp + 0x60) = values.coin;
    *(s16*)((u8*)wp + 0x58) = values.partyId;
    if (*(s16*)((u8*)wp + 0x58) != 0) {
        *(s16*)((u8*)wp + 0x5A) = values.partyHp;
        *(s16*)((u8*)wp + 0x5C) = values.partyMaxHp;
    }
}

BOOL statusWinCheckUpdate(void) {
    return valueCheck() != 0;
}

BOOL statusWinCheck(void) {
    return (wp->field_0x4 & 0x11) != 0;
}

void statusWinDispOff(void) {
    wp->field_0x4 |= 0x1000;
}

void statusWinDispOn(void) {
    wp->field_0x4 &= ~0x1000;
}

void statusWinForceOff(void) {
    if (wp->field_0x4 & 0x10) {
        wp->field_0x4 &= ~0x10;
        wp->field_0x4 &= ~0xA;
        wp->field_0x4 |= 5;
    } else if (wp->field_0x4 & 1) {
        wp->field_0x4 &= ~0xA;
        wp->field_0x4 |= 5;
    }
    if (wp->field_0x4 & 0x20) {
        wp->field_0x4 &= ~0x20;
        wp->field_0x4 |= 0xA;
        wp->field_0x4 &= ~5;
    } else if (wp->field_0x4 & 2) {
        wp->field_0x4 |= 0xA;
        wp->field_0x4 &= ~5;
    }
}

void statusWinForceCloseClear(void) {
    wp->field_0x4 &= ~0xA0;
}

void statusWinForceClose(void) {
    wp->field_0x4 &= ~5;
    wp->field_0x4 &= ~0x50;
    wp->field_0x4 |= 0xA0;
}

void statusWinForceOpen(void) {
    wp->field_0x4 &= ~0xA;
    wp->field_0x4 &= ~0xA0;
    wp->field_0x4 |= 0x50;
}

void statusWinClose(void) {
    wp->field_0x4 |= 0xA;
    wp->field_0x4 &= ~5;
}

void statusWinOpen(void) {
    wp->field_0x4 &= ~0xA;
    wp->field_0x4 |= 5;
}

// Draws Mario's star-power gauge: a wakka (fifteenths) icon for the partial hundreds, then the
// gauge_back ticks for each obtained Star Power. The second loop stops at the first unobtained
// power (the obtained powers are a contiguous set of low bits), which is the original's shape.
void gaugeDisp(s32 a1, f32 x, f32 y) {
    s32 i;
    s32 count;
    s32 hundreds;
    s32 remainder;
    s32 fifteenths;

    count = 0;
    for (i = 0; i < 8u; i++) {
        if (pouchGetPtr()->mStarPowersObtained & (1 << i)) {
            count += 100;
        }
    }
    if (a1 > count) {
        a1 = count;
    }
    if (a1 < 0) {
        a1 = 0;
    }
    hundreds = a1 / 100;
    remainder = a1 % 100;
    fifteenths = remainder * 15 / 99;
    if (remainder != 0 && a1 != 0 && fifteenths == 0) {
        fifteenths = 1;
    }
    if (fifteenths != 0) {
        Vec pos;

        pos.x = x + hundreds * 32;
        pos.y = y;
        pos.z = 0.0f;
#ifdef MUST_MATCH
        // Match-only: original passes a u16 id (no extsh) and evaluates the scale before the id.
        ((void (*)(Vec*, s16, f32, u16))iconDispGx)(&pos, 0x10, 1.0f, gauge_wakka[fifteenths]);
#else
        iconDispGx(&pos, 0x10, gauge_wakka[fifteenths], 1.0f);
#endif
    }
    y = 12.0f + y;
    for (i = 0; i < 8u; i++) {
        Vec pos;

        if (!(pouchGetPtr()->mStarPowersObtained & (1 << i))) {
            break;
        }
        pos.x = x + i * 32;
        pos.y = y;
        pos.z = 0.0f;
#ifdef MUST_MATCH
        // Match-only: original passes a u16 id (no extsh) and evaluates the scale before the id.
        ((void (*)(Vec*, s16, f32, u16))iconDispGx)(&pos, 0x10, 1.0f, (i < hundreds) ? gauge_back[i] : 0x1BF);
#else
        iconDispGx(&pos, 0x10, (i < hundreds) ? gauge_back[i] : 0x1BF, 1.0f);
#endif
    }
}

s32 valueCheck(void) {
    StatusWindowValue values;
    s32 changed;

    changed = 0;
    statusGetValue(&values);

    if (*(s16*)((u8*)wp + 0x50) != values.hp) {
        changed = 1;
        *(u32*)((u8*)wp + 0x84) |= 1;
    }
    if (*(s16*)((u8*)wp + 0x52) != values.maxHp) {
        changed = 1;
    }
    if (*(s16*)((u8*)wp + 0x54) != values.fp) {
        changed = 1;
        *(u32*)((u8*)wp + 0x84) |= 2;
    }
    if (*(s16*)((u8*)wp + 0x56) != values.maxFp) {
        changed = 1;
    }
    if (wp->field_0x62 != values.ap) {
        changed = 1;
    }
    if (*(s16*)((u8*)wp + 0x64) != values.maxAp) {
        changed = 1;
    }
    if (*(s16*)((u8*)wp + 0x58) != values.partyId) {
        *(s16*)((u8*)wp + 0x58) = values.partyId;
    } else if (*(s16*)((u8*)wp + 0x58) != 0) {
        if (*(s16*)((u8*)wp + 0x5A) != values.partyHp) {
            changed = 1;
            *(u32*)((u8*)wp + 0x84) |= 8;
        }
        if (*(s16*)((u8*)wp + 0x5C) != values.partyMaxHp) {
            changed = 1;
        }
    }
    if (*(s16*)((u8*)wp + 0x5E) != values.starPoint) {
        changed = 1;
    }
    if (*(s16*)((u8*)wp + 0x60) != values.coin) {
        changed = 1;
        *(u32*)((u8*)wp + 0x84) |= 0x10;
    }

    if (changed == 0) {
        if (*(u32*)((u8*)wp + 0x84) != 0) {
            wp->field_0x80 = *(u32*)((u8*)wp + 0x84);
            *(u32*)((u8*)wp + 0x84) = 0;
            wp->field_0x88 = gp->framerate * 2;
        }
    } else {
        wp->field_0x80 = 0;
    }
    return changed;
}

static s32 valueMoveAmount(s16 cur, s16 target) {
    s32 diff = target - cur;
    s32 step = diff;

    if (step != 0) {
        step = diff / 10;
        if (step == 0) {
            step = (target > cur) ? 1 : -1;
        }
    }
    return step;
}

// Single-variable form: keeps diff and step in the same register, as the
// original's inlined valueMove does.
static inline void valueMoveUpdate(s16* cur, s16 target) {
    s32 step = target - *cur;

    if (step != 0) {
        step /= 10;
        if (step == 0) {
            step = (target > *cur) ? 1 : -1;
        }
    }
    *cur += step;
}

// The original inlines valueMove for the first party bar with *cur in r6 and
// target in r5 (wp is still live in r7). Taking target by pointer makes both
// operands memory loads, so *cur is loaded first and lands in r6; taking it by
// value schedules the target load first and swaps the two registers.
#ifdef MUST_MATCH
static inline void valueMoveParty(s16* cur, s16* target) {
    s16 c = *cur;
    s16 t = *target;
    s32 step = t - c;

    if (step != 0) {
        step /= 10;
        if (step == 0) {
            step = (t > c) ? 1 : -1;
        }
    }
    *cur += step;
}
#endif

void valueUpdate(void) {
    static s32 count;
#ifdef MUST_MATCH
    s16 curCoin;
#endif
    StatusWindowValue values;

    statusGetValue(&values);
    count++;
    count %= 1200;
    if (count & 1) {
        return;
    }
    // The original materialises the statusWinCheck() result; a plain
    // if (!statusWinCheck()) folds the boolean away, so dispatch on it.
#ifdef MUST_MATCH
    switch ((wp->field_0x4 & 0x11) != 0) {
    default:
#else
    if (!statusWinCheck()) {
        return;
    }
#endif
#ifdef MUST_MATCH
    curCoin = ((s16*)wp)[0x30];
    if (valueMoveAmount(curCoin, values.coin) != 0) {
#else
    if (valueMoveAmount(*(s16*)((u8*)wp + 0x60), values.coin) != 0) {
#endif
        if (count % 8 == 0) {
            psndSFXOn("SFX_COUNT_COIN1");
        }
    }

    valueMoveUpdate((s16*)((u8*)wp + 0x50), values.hp);
    valueMoveUpdate((s16*)((u8*)wp + 0x52), values.maxHp);
    valueMoveUpdate((s16*)((u8*)wp + 0x54), values.fp);
    valueMoveUpdate((s16*)((u8*)wp + 0x56), values.maxFp);
    valueMoveUpdate(&wp->field_0x62, values.ap);
    valueMoveUpdate((s16*)((u8*)wp + 0x64), values.maxAp);
    valueMoveUpdate((s16*)((u8*)wp + 0x5E), values.starPoint);
    valueMoveUpdate((s16*)((u8*)wp + 0x60), values.coin);

    *(s16*)((u8*)wp + 0x58) = values.partyId;
    if (*(s16*)((u8*)wp + 0x58) != 0) {
#ifdef MUST_MATCH
        valueMoveParty((s16*)((u8*)wp + 0x5A), &values.partyHp);
#else
        valueMoveUpdate((s16*)((u8*)wp + 0x5A), values.partyHp);
#endif
        valueMoveUpdate((s16*)((u8*)wp + 0x5C), values.partyMaxHp);
    }
    if (*(s16*)((u8*)wp + 0x5E) > 0x63) {
        *(s16*)((u8*)wp + 0x5E) = 0x63;
    }
    if (*(s16*)((u8*)wp + 0x60) > 0x3E7) {
        *(s16*)((u8*)wp + 0x60) = 0x3E7;
    }
#ifdef MUST_MATCH
        break;
    case 0:
        return;
    }
#endif
}

// NONMATCHING (99.82%): body complete; the original groups the by-value GXColor outgoing temps by
// callee (iconDispGxCol slots, then iconNumberDispGx slots), GC/2.6 allocates them in program order.
// All remaining diffs are that stack-slot order.
void statusWinDisp(CameraId camId, void* param) {
    alwaysDtEntry* dtEntry;
    s32 i;
    s32 v6;
    s32 number;
    f32 tx, ty;
    f32 baseX, baseY;
    Mtx mtx;
    Mtx scale;
    Mtx badgeMtx;
    Mtx badgeScale;
    Mtx hpMtx;
    Mtx maxHpMtx;
    Mtx fpMtx;
    Mtx maxFpMtx;
    Mtx starMtx;
    Mtx coinMtx;
    Mtx partyHpMtx;
    Mtx partyMaxHpMtx;

    baseX = wp->field_0x20;
    baseY = wp->field_0x24;
    v6 = 1;
    if (wp->field_0x88 > 0) {
        wp->field_0x88--;
        v6 = (wp->field_0x88 % 16) >= 6;
    }

    dtEntry = alwaysDt;
    for (i = 0; i < 13u; i++, dtEntry++) {
        if (dtEntry->iconId == 466 && !pouchGetPtr()->mStarPowersObtained) {
            break; // don't draw star power bar
        }
        if (!v6) {
            if ((wp->field_0x80 & 1) && i == 1) {
                continue;
            }
            if (((wp->field_0x80 & 2) && i == 4) || ((wp->field_0x80 & 0x10) && i == 8) || ((wp->field_0x80 & 4) && i == 12)) {
                continue;
            }
        }
        PSMTXTrans(mtx, baseX + (f32)dtEntry->xpos, baseY - (f32)dtEntry->ypos, 0.0f);
        PSMTXScale(scale, dtEntry->scale, dtEntry->scale, dtEntry->scale);
        PSMTXConcat(mtx, scale, mtx);
        iconDispGxCol(mtx, 0x10, dtEntry->iconId, *(GXColor*)&dat_8041c8e4);
    }

    if (!(wp->field_0x80 & 1) || v6 != 0) {
        number = *(s16*)((u8*)wp + 0x50);
        PSMTXTrans(hpMtx, baseX + 120.0f - 6.0f - 10.0f, baseY - 38.0f + 2.0f, 0.0f);
        iconNumberDispGx(hpMtx, number, 0, *(GXColor*)&dat_8041c8e0);
    }
    number = *(s16*)((u8*)wp + 0x52);
    tx = baseX + 186.0f - 6.0f - 10.0f;
    ty = baseY - 38.0f + 3.0f;
    PSMTXTrans(maxHpMtx, tx, ty, 0.0f);
    iconNumberDispGx(maxHpMtx, number, 1, *(GXColor*)&dat_8041c8e0);

    if (!(wp->field_0x80 & 2) || v6 != 0) {
        number = *(s16*)((u8*)wp + 0x54);
        PSMTXTrans(fpMtx, baseX + 292.0f - 6.0f, baseY - 38.0f + 2.0f, 0.0f);
        iconNumberDispGx(fpMtx, number, 0, *(GXColor*)&dat_8041c8e0);
    }
    number = *(s16*)((u8*)wp + 0x56);
    PSMTXTrans(maxFpMtx, baseX + 358.0f - 6.0f, ty, 0.0f);
    iconNumberDispGx(maxFpMtx, number, 1, *(GXColor*)&dat_8041c8e0);

    number = *(s16*)((u8*)wp + 0x5E);
    ty = baseY - 38.0f + 2.0f;
    PSMTXTrans(starMtx, baseX + 462.0f, ty, 0.0f);
    iconNumberDispGx(starMtx, number, 0, *(GXColor*)&dat_8041c8e0);

    if (!(wp->field_0x80 & 0x10) || v6 != 0) {
        number = *(s16*)((u8*)wp + 0x60);
        PSMTXTrans(coinMtx, baseX + 572.0f, ty, 0.0f);
        iconNumberDispGx(coinMtx, number, 0, *(GXColor*)&dat_8041c8e0);
    }

    if (!(wp->field_0x80 & 4) || v6 != 0) {
        gaugeDisp(*(s16*)((u8*)wp + 0x62), baseX + 260.0f, baseY - 87.0f + 4.0f);
    }

    if (*(s16*)((u8*)wp + 0x58) != 0) {
        s32 partyId = *(s16*)((u8*)wp + 0x58);

        partysDt[1].iconId = party_icon[partyId];
        if (partyId == 4) {
            switch (pouchGetPartyColor(4)) {
            case 0:
                partysDt[1].iconId = 0x1A7;
                break;
            case 1:
                partysDt[1].iconId = 0x1A8;
                break;
            case 2:
                partysDt[1].iconId = 0x1A9;
                break;
            case 3:
                partysDt[1].iconId = 0x1AA;
                break;
            case 4:
                partysDt[1].iconId = 0x1AB;
                break;
            case 5:
                partysDt[1].iconId = 0x1AC;
                break;
            case 6:
            default:
                partysDt[1].iconId = 0x1AD;
                break;
            }
        }
        dtEntry = partysDt;
        for (i = 0; i < 3u; i++, dtEntry++) {
            if ((wp->field_0x80 & 8) && v6 == 0 && i == 1) {
                continue;
            }
            PSMTXTrans(mtx, baseX + (f32)dtEntry->xpos, baseY - (f32)dtEntry->ypos, 0.0f);
            PSMTXScale(scale, dtEntry->scale, dtEntry->scale, dtEntry->scale);
            PSMTXConcat(mtx, scale, mtx);
            iconDispGxCol(mtx, 0x10, dtEntry->iconId, *(GXColor*)&dat_8041c8e8);
        }
        if (!(wp->field_0x80 & 8) || v6 != 0) {
            number = *(s16*)((u8*)wp + 0x5A);
            PSMTXTrans(partyHpMtx, baseX + 120.0f - 6.0f - 10.0f, baseY - 78.0f + 2.0f + 4.0f, 0.0f);
            iconNumberDispGx(partyHpMtx, number, 0, *(GXColor*)&dat_8041c8e0);
        }
        number = *(s16*)((u8*)wp + 0x5C);
        PSMTXTrans(partyMaxHpMtx, tx, baseY - 78.0f + 2.0f + 4.0f, 0.0f);
        iconNumberDispGx(partyMaxHpMtx, number, 1, *(GXColor*)&dat_8041c8e0);
    }

    if ((wp->field_0x4 & 0x100) && gp->inBattle == 0) {
        ty = -180.0f + (wp->field_0x34 - wp->field_0x24);
        PSMTXTrans(badgeMtx, -190.0f, ty, 0.0f);
        PSMTXScale(badgeScale, 0.75f, 0.75f, 0.75f);
        PSMTXConcat(badgeMtx, badgeScale, badgeMtx);
        iconDispGxCol(badgeMtx, 0x10, 0x212, *(GXColor*)&dat_8041c8ec);
        if (swByteGet(0) >= 7 || partyChkJoin(1) != 0) {
            PSMTXTrans(badgeMtx, -250.0f, 16.0f + ty, 0.0f);
            PSMTXScale(badgeScale, 0.75f, 0.75f, 0.75f);
            PSMTXConcat(badgeMtx, badgeScale, badgeMtx);
            iconDispGxCol(badgeMtx, 0x10, 0x213, *(GXColor*)&dat_8041c8f0);
        }
        PSMTXTrans(badgeMtx, -190.0f, 50.0f + ty, 0.0f);
        PSMTXScale(badgeScale, 0.75f, 0.75f, 0.75f);
        PSMTXConcat(badgeMtx, badgeScale, badgeMtx);
        iconDispGxCol(badgeMtx, 0x10, 0x214, *(GXColor*)&dat_8041c8f4);
        if (swByteGet(0) >= 0x14 || pouchGetHaveBadgeCnt() != 0) {
            PSMTXTrans(badgeMtx, -130.0f, 16.0f + ty, 0.0f);
            PSMTXScale(badgeScale, 0.75f, 0.75f, 0.75f);
            PSMTXConcat(badgeMtx, badgeScale, badgeMtx);
            iconDispGxCol(badgeMtx, 0x10, 0x215, *(GXColor*)&dat_8041c8f8);
        }
        PSMTXTrans(badgeMtx, -190.0f, ty - 25.0f, 0.0f);
        PSMTXScale(badgeScale, 0.75f, 0.75f, 0.75f);
        PSMTXConcat(badgeMtx, badgeScale, badgeMtx);
        iconDispGxCol(badgeMtx, 0x10, 0x216, *(GXColor*)&dat_8041c8fc);
    }
}

void statusGetValue(StatusWindowValue* values) {
    BattleWork* bw;
    BattleUnit* mario;
    BattleUnit* party;
    s16 partyId;

    if (!gp->inBattle) {
        values->hp = pouchGetHP();
        values->maxHp = pouchGetMaxHP();
        values->fp = pouchGetFP();
        values->maxFp = pouchGetMaxFP();
        values->ap = pouchGetAP();
        values->maxAp = pouchGetMaxAP();
        values->starPoint = pouchGetStarPoint();
        values->coin = pouchGetCoin();
        if (marioBgmodeChk() == 0) {
            partyId = marioGetParty();
        } else {
            partyId = marioGetPtr()->prevPartyMemberId[0];
        }
        values->partyId = partyId;
        values->partyHp = pouchGetPtr()->partyData[values->partyId].currentHP;
        values->partyMaxHp = pouchGetPtr()->partyData[values->partyId].maximumHP;
    } else {
        bw = _battleWorkPointer;
        mario = BattleGetMarioPtr(bw);
        party = BattleGetPartyPtr(bw);
        values->hp = mario->currentHp;
        values->maxHp = mario->maxHp;
        values->fp = mario->currentFp;
        values->maxFp = mario->maxFp;
        values->ap = pouchGetAP();
        values->maxAp = pouchGetMaxAP();
        values->starPoint = pouchGetStarPoint();
        values->coin = pouchGetCoin();
        if (party != NULL) {
            values->partyId = BattleTransPartyId(party->currentKind);
            values->partyHp = party->currentHp;
            values->partyMaxHp = party->maxHp;
        } else {
            values->partyId = 0;
            values->partyHp = 0;
            values->partyMaxHp = 0;
        }
    }
}
