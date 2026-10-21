#include "pmario_sound.h"
#include "statuswindow.h"
#include "battle/battle.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "driver/seqdrv.h"
#include "driver/swdrv.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "memory.h"
#include "math_ext.h"
#include <string.h>
#include "mario/mario_party.h"

extern GlobalWork* gp;

// Named .sdata2 constants the original keeps as objects (they land before the unit's literal pool).
extern const f32 float_0_8041c900;
extern const f32 float_1_8041c904;
extern const f32 float_3p1416_8041c908;
extern const f32 float_0p5_8041c90c;

// .sdata2 colour constants of this unit (white with alpha), passed to the icon draw functions.
__declspec(section ".sdata2") u32 dat_8041c8e0 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041c8e4 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041c8e8 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041c8ec = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041c8f0 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041c8f4 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041c8f8 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041c8fc = 0xFFFFFFFF;

#define STATUS_WIN_OPEN_FRAMES 16.0f // open and close ramp lengths at animSpeed 1
#define STATUS_WIN_CLOSE_FRAMES 28.0f
#define ICON_SP_BANNER 466 // alwaysDt's blue star power banner; the draw loop stops here while no star power is owned
#define STATUS_WIN_IDLE_SECONDS 10 // Mario standing still before the window opens; cooldown after an automatic toggle
#define STATUS_WIN_CHANGE_SECONDS 2 // the window stays open this long after a value changes
#define STATUS_WIN_BLINK_SECONDS 2 // blink length once a changed value settles
#define STATUS_WIN_BLINK_PERIOD 16 // blink cycle in frames; the icon is hidden while blinkTimer % PERIOD < HIDE_FRAMES
#define STATUS_WIN_BLINK_HIDE_FRAMES 6
#define STATUS_WIN_MAX_STAR_POINT 99
#define STATUS_WIN_MAX_COIN 999

// Indices into alwaysDt (and partysDt for the party icon) that the blink masks hide.
enum {
    STATUS_DT_MARIO_HP = 1,
    STATUS_DT_FP = 4,
    STATUS_DT_COIN = 8,
    STATUS_DT_AP = 12,
    STATUS_DT_PARTY_ICON = 1,
};

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
static inline s32 valueMoveAmount(s16 cur, s16 target);
static inline void valueMoveUpdate(s16* cur, s16 target);
static inline void valueMoveParty(s16* cur, s16* target);

// Matching notes (all codegen-only; each call is ABI-identical to the header's by-value form):
// * Every GXColor argument is staged in a function-scope local declared colours-first
//   (colAlways..colB216 then numHp..numPartyMaxHp) and passed through a GXColor*-taking
//   function-pointer cast (SW_ICON_COL/SW_NUM_GX, MUST_MATCH builds only). MWCC passes a by-value GXColor as a pointer in r6 either way, so this
//   is safe, but it makes the compiler pass each local's home directly instead of building the
//   fresh outgoing-copy pool that GC/2.6 allocates in program order. The local homes reproduce
//   the original's slot grouping (iconDispGxCol at 0x28..0x40 above iconNumberDispGx at 0x08..0x24).
// * `ty2` gives the badge block its own FPR (the original reuses f28 there); folding it into `ty`
//   makes MWCC split `ty` and use f29 instead.
// * The star `ty` is assigned inside the PSMTXTrans argument so its constants colour f0/f1 like
//   the original; the plain `ty = ...;` form swaps them.
#ifdef MUST_MATCH
#define SW_ICON_COL(m, s, id, var, col) var = (col); ((void(*)(Mtx,s16,s32,GXColor*))iconDispGxCol)(m, s, id, &var)
#define SW_NUM_GX(m, n, f, var, col) var = (col); ((s32(*)(Mtx,s32,BOOL,GXColor*))iconNumberDispGx)(m, n, f, &var)
#else
#define SW_ICON_COL(m, s, id, var, col) iconDispGxCol(m, s, id, col)
#define SW_NUM_GX(m, n, f, var, col) iconNumberDispGx(m, n, f, col)
#endif

void statusWinInit(void) {
    wp = __memAlloc(HEAP_DEFAULT, sizeof(StatusWindowWork));
    memset(wp, 0, sizeof(StatusWindowWork));
    wp->animSpeed = float_1_8041c904;
    wp->openX = -304.0f;
    wp->openY = 208.0f;
    wp->closedX = -304.0f;
    wp->closedY = 334.0f;
    wp->idleFrames = 0;
    wp->toggleCooldown = 0;
    wp->flags &= ~(STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
    wp->flags |= (STATUS_WIN_OPEN | STATUS_WIN_OPENING);
}

void statusWinReInit(void) {
    wp->idleFrames = 0;
    wp->toggleCooldown = 0;
    if (wp->flags & STATUS_WIN_FORCE_OPEN) {
        wp->flags &= ~STATUS_WIN_FORCE_OPEN;
        wp->flags &= ~(STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
        wp->flags |= (STATUS_WIN_OPEN | STATUS_WIN_OPENING);
    } else if (wp->flags & STATUS_WIN_OPEN) {
        wp->flags &= ~(STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
        wp->flags |= (STATUS_WIN_OPEN | STATUS_WIN_OPENING);
    }
    if (wp->flags & STATUS_WIN_FORCE_CLOSE) {
        wp->flags &= ~STATUS_WIN_FORCE_CLOSE;
        wp->flags |= (STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
        wp->flags &= ~(STATUS_WIN_OPEN | STATUS_WIN_OPENING);
    } else if (wp->flags & STATUS_WIN_CLOSED) {
        wp->flags |= (STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
        wp->flags &= ~(STATUS_WIN_OPEN | STATUS_WIN_OPENING);
    }
}

// The cos()/lerp tail keeps the original's temporaries for its FPR colouring.
void statusWinMain(void) {
    MarioWork* mario;

    mario = marioGetPtr();
    if (mario->characterId != 0) {
        wp->flags = STATUS_WIN_CLOSED;
        wp->animDuration = float_1_8041c904;
        wp->animTime = float_1_8041c904;
        wp->openTarget = float_0_8041c900;
        wp->openAmount = float_0_8041c900;
    } else if (strcmp(gp->currentAreaName, "yuu") != 0) {
        switch (seqGetSeq()) {
            case SEQ_GAME:
            case SEQ_MAP_CHANGE:
                {
                    BOOL notCtrl;
                    BOOL notKey;

                    notCtrl = !marioChkCtrl();
#ifdef MUST_MATCH
                    // The comma stores notKey before testing it, as retail does.
                    if (notCtrl || (notKey = !marioChkKey(), notKey) || !(mario->flags & 1)) {
#else
                    if (notCtrl || !marioChkKey() || !(mario->flags & 1)) {
#endif
                        if (statusWinCheck()) {
                            wp->flags |= (STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
                            wp->flags &= ~(STATUS_WIN_OPEN | STATUS_WIN_OPENING);
                        }
                        valueCheck();
                        wp->idleFrames = 0;
                        wp->toggleCooldown = 0;
                    } else {
                        if (PSVECDistance(&wp->marioPos, &mario->position) < float_1_8041c904) {
                            wp->idleFrames++;
                            if (wp->idleFrames > gp->framerate * STATUS_WIN_IDLE_SECONDS) {
                                wp->idleFrames = gp->framerate * STATUS_WIN_IDLE_SECONDS;
                                if (wp->manualToggle == 0 && wp->toggleCooldown == 0 && !statusWinCheck()) {
                                    wp->flags &= ~(STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
                                    wp->flags |= (STATUS_WIN_OPEN | STATUS_WIN_OPENING);
                                    wp->flags |= STATUS_WIN_BADGES;
                                }
                            }
                        } else {
                            wp->idleFrames = 0;
                            wp->manualToggle = 0;
                            if (wp->toggleCooldown == 0) {
                                if (statusWinCheck()) {
                                    wp->flags |= (STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
                                    wp->flags &= ~(STATUS_WIN_OPEN | STATUS_WIN_OPENING);
                                }
                            }
                        }
                        if (mario->buttonNew & 0x10) {
                            if (!statusWinCheck()) {
                                wp->flags &= ~(STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
                                wp->flags |= (STATUS_WIN_OPEN | STATUS_WIN_OPENING);
                                wp->toggleCooldown = gp->framerate * STATUS_WIN_IDLE_SECONDS;
                                psndSFXOn((const char*)0x2A);
                                wp->flags |= STATUS_WIN_BADGES;
                            } else {
                                wp->flags |= (STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
                                wp->flags &= ~(STATUS_WIN_OPEN | STATUS_WIN_OPENING);
                                wp->toggleCooldown = gp->framerate * STATUS_WIN_IDLE_SECONDS;
                                wp->manualToggle = 1;
                                psndSFXOn((const char*)0x2B);
                            }
                        }
                        if (valueCheck() != 0) {
                            wp->toggleCooldown = gp->framerate * STATUS_WIN_CHANGE_SECONDS;
                            if (!statusWinCheck()) {
                                wp->flags &= ~(STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
                                wp->flags |= (STATUS_WIN_OPEN | STATUS_WIN_OPENING);
                            }
                        }
                    }
                }
                if (wp->toggleCooldown != 0) {
                    wp->toggleCooldown--;
                }
                wp->marioPos = mario->position;
                if (wp->flags & STATUS_WIN_FORCE_OPEN) {
                    wp->flags &= ~(STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
                }
                if (wp->flags & STATUS_WIN_FORCE_CLOSE) {
                    wp->flags &= ~(STATUS_WIN_OPEN | STATUS_WIN_OPENING);
                }
                break;
            case SEQ_BATTLE:
                break;
            default:
                if (statusWinCheck()) {
                    wp->flags |= (STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
                    wp->flags &= ~(STATUS_WIN_OPEN | STATUS_WIN_OPENING);
                }
                break;
        }

        if ((wp->flags & STATUS_WIN_OPENING) || (wp->flags & STATUS_WIN_CLOSING) || (wp->flags & STATUS_WIN_FORCE_OPENING) || (wp->flags & STATUS_WIN_FORCE_CLOSING)) {
            wp->openFrom = wp->openAmount;
            wp->openTarget = (wp->flags & (STATUS_WIN_OPENING | STATUS_WIN_FORCE_OPENING)) ? float_1_8041c904 : float_0_8041c900;
            wp->animTime = float_0_8041c900;
            wp->animDuration = (wp->flags & (STATUS_WIN_OPENING | STATUS_WIN_FORCE_OPENING)) ? STATUS_WIN_OPEN_FRAMES / wp->animSpeed : STATUS_WIN_CLOSE_FRAMES / wp->animSpeed;
            wp->flags &= ~STATUS_WIN_OPENING;
            wp->flags &= ~STATUS_WIN_CLOSING;
            wp->flags &= ~STATUS_WIN_FORCE_OPENING;
            wp->flags &= ~STATUS_WIN_FORCE_CLOSING;
        }

        wp->animTime += float_1_8041c904;
        if (wp->animTime >= wp->animDuration) {
            wp->animTime = wp->animDuration;
            wp->openAmount = wp->openTarget;
            // Codegen-only: naming the zero object makes MWCC evaluate the field first and load
            // the constant into f0; writing the constant on the left restores the original's
            // 0.0-first fcmpu.
            if (float_0_8041c900 == wp->openAmount) {
                wp->flags &= ~STATUS_WIN_BADGES;
            }
        } else {
#ifdef MUST_MATCH
            // Codegen-only: the f64 temp gives the in-place `frsp f1,f1`, `x = x;` and the
            // `a = q*cc; q = a;` copy colour the diff/product into f2/f1, and `half` keeps the
            // 0.5 load in f0 so the fmadds reads product*0.5 like the original.
            f64 c;
            f32 x;
            f32 cc;
            f32 q;
            f32 a;
            f32 half;

            c = cos(float_3p1416_8041c908 * wp->animTime / wp->animDuration);
            x = (f32)c;
            x = x;
            cc = float_1_8041c904 - x;
            a = wp->openFrom;
            q = wp->openTarget - a;
            a = q * cc;
            q = a;
            half = float_0p5_8041c90c;
            wp->openAmount = q * half + wp->openFrom;
#else
            f32 x = cos(float_3p1416_8041c908 * wp->animTime / wp->animDuration);

            wp->openAmount = (wp->openTarget - wp->openFrom) * (float_1_8041c904 - x) * float_0p5_8041c90c +
                             wp->openFrom;
#endif
        }

        {
            f32 t;
            f32 x = wp->openAmount;
            t = float_1_8041c904 - x;
            wp->originX = wp->openX * x + wp->closedX * t;
            wp->originY = wp->openY * x + wp->closedY * t;
        }
        valueUpdate();
        if (!(wp->flags & STATUS_WIN_DISP_OFF)) {
            dispEntry(8, 500.0f, 0, statusWinDisp, NULL);
        }
    }
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
        pos.z = float_0_8041c900;
        iconDispGx(float_1_8041c904, &pos, 0x10, gauge_wakka[fifteenths]);
    }
    y = 12.0f + y;
    for (i = 0; i < 8u; i++) {
        Vec pos;
        u16 id;

        if (!(pouchGetPtr()->mStarPowersObtained & (1 << i))) {
            break;
        }
        pos.x = x + i * 32;
        pos.y = y;
        pos.z = float_0_8041c900;
        id = (i < hundreds) ? gauge_back[i] : 0x1BF;
        iconDispGx(float_1_8041c904, &pos, 0x10, id);
    }
}

void statusWinDisp(CameraId camId, void* param) {
    alwaysDtEntry* dtEntry;
    s32 i;
    s32 v6;
    s32 number;
    f32 tx, ty;
    f32 baseX, baseY;
    f32 ty2;
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
#ifdef MUST_MATCH
    GXColor colAlways, colParty, colB212, colB213, colB214, colB215, colB216;
    GXColor numHp, numMaxHp, numFp, numMaxFp, numStar, numCoin, numPartyHp, numPartyMaxHp;
#endif

    baseX = wp->originX;
    baseY = wp->originY;
    v6 = 1;
    if (wp->blinkTimer > 0) {
        wp->blinkTimer--;
        v6 = (wp->blinkTimer % STATUS_WIN_BLINK_PERIOD) >= STATUS_WIN_BLINK_HIDE_FRAMES;
    }

    dtEntry = alwaysDt;
    for (i = 0; i < 13u; i++, dtEntry++) {
        if (dtEntry->iconId == ICON_SP_BANNER && !pouchGetPtr()->mStarPowersObtained) {
            break; // don't draw star power bar
        }
        if (!v6) {
            if ((wp->blinkMask & STATUS_BLINK_MARIO_HP) && i == STATUS_DT_MARIO_HP) {
                continue;
            }
            if (((wp->blinkMask & STATUS_BLINK_FP) && i == STATUS_DT_FP) || ((wp->blinkMask & STATUS_BLINK_COIN) && i == STATUS_DT_COIN) ||
            ((wp->blinkMask & STATUS_BLINK_AP) && i == STATUS_DT_AP)) {
                continue;
            }
        }
        PSMTXTrans(mtx, baseX + (f32)dtEntry->xpos, baseY - (f32)dtEntry->ypos, float_0_8041c900);
        PSMTXScale(scale, dtEntry->scale, dtEntry->scale, dtEntry->scale);
        PSMTXConcat(mtx, scale, mtx);
        SW_ICON_COL(mtx, 0x10, dtEntry->iconId, colAlways, *(GXColor*)&dat_8041c8e4);
    }

    if (!(wp->blinkMask & STATUS_BLINK_MARIO_HP) || v6 != 0) {
        number = wp->hp;
        PSMTXTrans(hpMtx, baseX + 120.0f - 6.0f - 10.0f, baseY - 38.0f + 2.0f, float_0_8041c900);
        SW_NUM_GX(hpMtx, number, 0, numHp, *(GXColor*)&dat_8041c8e0);
    }
    number = wp->maxHp;
    tx = baseX + 186.0f - 6.0f - 10.0f;
    ty = baseY - 38.0f + 3.0f;
    PSMTXTrans(maxHpMtx, tx, ty, float_0_8041c900);
    SW_NUM_GX(maxHpMtx, number, 1, numMaxHp, *(GXColor*)&dat_8041c8e0);

    if (!(wp->blinkMask & STATUS_BLINK_FP) || v6 != 0) {
        number = wp->fp;
        PSMTXTrans(fpMtx, baseX + 292.0f - 6.0f, baseY - 38.0f + 2.0f, float_0_8041c900);
        SW_NUM_GX(fpMtx, number, 0, numFp, *(GXColor*)&dat_8041c8e0);
    }
    number = wp->maxFp;
    PSMTXTrans(maxFpMtx, baseX + 358.0f - 6.0f, ty, float_0_8041c900);
    SW_NUM_GX(maxFpMtx, number, 1, numMaxFp, *(GXColor*)&dat_8041c8e0);

    number = wp->starPoint;
    PSMTXTrans(starMtx, baseX + 462.0f, ty = baseY - 38.0f + 2.0f, float_0_8041c900);
    SW_NUM_GX(starMtx, number, 0, numStar, *(GXColor*)&dat_8041c8e0);

    if (!(wp->blinkMask & STATUS_BLINK_COIN) || v6 != 0) {
        number = wp->coin;
        PSMTXTrans(coinMtx, baseX + 572.0f, ty, float_0_8041c900);
        SW_NUM_GX(coinMtx, number, 0, numCoin, *(GXColor*)&dat_8041c8e0);
    }

    if (!(wp->blinkMask & STATUS_BLINK_AP) || v6 != 0) {
        gaugeDisp(wp->ap, baseX + 260.0f, baseY - 87.0f + 4.0f);
    }

    if (wp->partyId != 0) {
        s32 partyId = wp->partyId;

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
            if ((wp->blinkMask & STATUS_BLINK_PARTY_HP) && v6 == 0 && i == STATUS_DT_PARTY_ICON) {
                continue;
            }
            PSMTXTrans(mtx, baseX + (f32)dtEntry->xpos, baseY - (f32)dtEntry->ypos, float_0_8041c900);
            PSMTXScale(scale, dtEntry->scale, dtEntry->scale, dtEntry->scale);
            PSMTXConcat(mtx, scale, mtx);
            SW_ICON_COL(mtx, 0x10, dtEntry->iconId, colParty, *(GXColor*)&dat_8041c8e8);
        }
        if (!(wp->blinkMask & STATUS_BLINK_PARTY_HP) || v6 != 0) {
            number = wp->partyHp;
            PSMTXTrans(partyHpMtx, baseX + 120.0f - 6.0f - 10.0f, baseY - 78.0f + 2.0f + 4.0f, float_0_8041c900);
            SW_NUM_GX(partyHpMtx, number, 0, numPartyHp, *(GXColor*)&dat_8041c8e0);
        }
        number = wp->partyMaxHp;
        PSMTXTrans(partyMaxHpMtx, tx, baseY - 78.0f + 2.0f + 4.0f, float_0_8041c900);
        SW_NUM_GX(partyMaxHpMtx, number, 1, numPartyMaxHp, *(GXColor*)&dat_8041c8e0);
    }

    if ((wp->flags & STATUS_WIN_BADGES) && gp->inBattle == 0) {
        ty2 = -180.0f + (wp->openY - wp->originY);
        PSMTXTrans(badgeMtx, -190.0f, ty2, float_0_8041c900);
        PSMTXScale(badgeScale, 0.75f, 0.75f, 0.75f);
        PSMTXConcat(badgeMtx, badgeScale, badgeMtx);
        SW_ICON_COL(badgeMtx, 0x10, 0x212, colB212, *(GXColor*)&dat_8041c8ec);
        if (swByteGet(0) >= 7 || partyChkJoin(1) != 0) {
            PSMTXTrans(badgeMtx, -250.0f, 16.0f + ty2, float_0_8041c900);
            PSMTXScale(badgeScale, 0.75f, 0.75f, 0.75f);
            PSMTXConcat(badgeMtx, badgeScale, badgeMtx);
            SW_ICON_COL(badgeMtx, 0x10, 0x213, colB213, *(GXColor*)&dat_8041c8f0);
        }
        PSMTXTrans(badgeMtx, -190.0f, 50.0f + ty2, float_0_8041c900);
        PSMTXScale(badgeScale, 0.75f, 0.75f, 0.75f);
        PSMTXConcat(badgeMtx, badgeScale, badgeMtx);
        SW_ICON_COL(badgeMtx, 0x10, 0x214, colB214, *(GXColor*)&dat_8041c8f4);
        if (swByteGet(0) >= 0x14 || pouchGetHaveBadgeCnt() != 0) {
            PSMTXTrans(badgeMtx, -130.0f, 16.0f + ty2, float_0_8041c900);
            PSMTXScale(badgeScale, 0.75f, 0.75f, 0.75f);
            PSMTXConcat(badgeMtx, badgeScale, badgeMtx);
            SW_ICON_COL(badgeMtx, 0x10, 0x215, colB215, *(GXColor*)&dat_8041c8f8);
        }
        PSMTXTrans(badgeMtx, -190.0f, ty2 - 25.0f, float_0_8041c900);
        PSMTXScale(badgeScale, 0.75f, 0.75f, 0.75f);
        PSMTXConcat(badgeMtx, badgeScale, badgeMtx);
        SW_ICON_COL(badgeMtx, 0x10, 0x216, colB216, *(GXColor*)&dat_8041c8fc);
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

s32 valueCheck(void) {
    StatusWindowValue values;
    s32 changed;

    changed = 0;
    statusGetValue(&values);

    if (wp->hp != values.hp) {
        changed = 1;
        wp->changedMask |= STATUS_BLINK_MARIO_HP;
    }
    if (wp->maxHp != values.maxHp) {
        changed = 1;
    }
    if (wp->fp != values.fp) {
        changed = 1;
        wp->changedMask |= STATUS_BLINK_FP;
    }
    if (wp->maxFp != values.maxFp) {
        changed = 1;
    }
    if (wp->ap != values.ap) {
        changed = 1;
    }
    if (wp->maxAp != values.maxAp) {
        changed = 1;
    }
    if (wp->partyId != values.partyId) {
        wp->partyId = values.partyId;
    } else if (wp->partyId != 0) {
        if (wp->partyHp != values.partyHp) {
            changed = 1;
            wp->changedMask |= STATUS_BLINK_PARTY_HP;
        }
        if (wp->partyMaxHp != values.partyMaxHp) {
            changed = 1;
        }
    }
    if (wp->starPoint != values.starPoint) {
        changed = 1;
    }
    if (wp->coin != values.coin) {
        changed = 1;
        wp->changedMask |= STATUS_BLINK_COIN;
    }

    if (changed == 0) {
        if (wp->changedMask != 0) {
            wp->blinkMask = wp->changedMask;
            wp->changedMask = 0;
            wp->blinkTimer = gp->framerate * STATUS_WIN_BLINK_SECONDS;
        }
    } else {
        wp->blinkMask = 0;
    }
    return changed;
}

void valueUpdate(void) {
    static s32 count;
    s16 curCoin;
    StatusWindowValue values;

    statusGetValue(&values);
    count++;
    count %= 1200;
    if (count & 1) {
        return;
    }
    if (!statusWinCheck()) {
        return;
    }
    curCoin = wp->coin;
    if (valueMoveAmount(curCoin, values.coin) != 0) {
        if (count % 8 == 0) {
            psndSFXOn("SFX_COUNT_COIN1");
        }
    }

    valueMoveUpdate(&wp->hp, values.hp);
    valueMoveUpdate(&wp->maxHp, values.maxHp);
    valueMoveUpdate(&wp->fp, values.fp);
    valueMoveUpdate(&wp->maxFp, values.maxFp);
    valueMoveUpdate(&wp->ap, values.ap);
    valueMoveUpdate(&wp->maxAp, values.maxAp);
    valueMoveUpdate(&wp->starPoint, values.starPoint);
    valueMoveUpdate(&wp->coin, values.coin);

    wp->partyId = values.partyId;
    if (wp->partyId != 0) {
        valueMoveParty(&wp->partyHp, &values.partyHp);
        valueMoveUpdate(&wp->partyMaxHp, values.partyMaxHp);
    }
    if (wp->starPoint > STATUS_WIN_MAX_STAR_POINT) {
        wp->starPoint = STATUS_WIN_MAX_STAR_POINT;
    }
    if (wp->coin > STATUS_WIN_MAX_COIN) {
        wp->coin = STATUS_WIN_MAX_COIN;
    }
}

void statusWinOpen(void) {
    wp->flags &= ~(STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
    wp->flags |= (STATUS_WIN_OPEN | STATUS_WIN_OPENING);
}

void statusWinClose(void) {
    wp->flags |= (STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
    wp->flags &= ~(STATUS_WIN_OPEN | STATUS_WIN_OPENING);
}

void statusWinForceOpen(void) {
    wp->flags &= ~(STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
    wp->flags &= ~(STATUS_WIN_FORCE_CLOSE | STATUS_WIN_FORCE_CLOSING);
    wp->flags |= (STATUS_WIN_FORCE_OPEN | STATUS_WIN_FORCE_OPENING);
}

void statusWinForceClose(void) {
    wp->flags &= ~(STATUS_WIN_OPEN | STATUS_WIN_OPENING);
    wp->flags &= ~(STATUS_WIN_FORCE_OPEN | STATUS_WIN_FORCE_OPENING);
    wp->flags |= (STATUS_WIN_FORCE_CLOSE | STATUS_WIN_FORCE_CLOSING);
}

void statusWinForceCloseClear(void) {
    wp->flags &= ~(STATUS_WIN_FORCE_CLOSE | STATUS_WIN_FORCE_CLOSING);
}

void statusWinForceOff(void) {
    if (wp->flags & STATUS_WIN_FORCE_OPEN) {
        wp->flags &= ~STATUS_WIN_FORCE_OPEN;
        wp->flags &= ~(STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
        wp->flags |= (STATUS_WIN_OPEN | STATUS_WIN_OPENING);
    } else if (wp->flags & STATUS_WIN_OPEN) {
        wp->flags &= ~(STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
        wp->flags |= (STATUS_WIN_OPEN | STATUS_WIN_OPENING);
    }
    if (wp->flags & STATUS_WIN_FORCE_CLOSE) {
        wp->flags &= ~STATUS_WIN_FORCE_CLOSE;
        wp->flags |= (STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
        wp->flags &= ~(STATUS_WIN_OPEN | STATUS_WIN_OPENING);
    } else if (wp->flags & STATUS_WIN_CLOSED) {
        wp->flags |= (STATUS_WIN_CLOSED | STATUS_WIN_CLOSING);
        wp->flags &= ~(STATUS_WIN_OPEN | STATUS_WIN_OPENING);
    }
}

void statusWinDispOn(void) {
    wp->flags &= ~STATUS_WIN_DISP_OFF;
}

void statusWinDispOff(void) {
    wp->flags |= STATUS_WIN_DISP_OFF;
}

BOOL statusWinCheck(void) {
    if (wp->flags & (STATUS_WIN_OPEN | STATUS_WIN_FORCE_OPEN)) {
        return TRUE;
    }
    return FALSE;
}

BOOL statusWinCheckUpdate(void) {
    return valueCheck() != 0;
}

void statusWinForceUpdate(void) {
    StatusWindowValue values;

    statusGetValue(&values);
    wp->hp = values.hp;
    wp->maxHp = values.maxHp;
    wp->fp = values.fp;
    wp->maxFp = values.maxFp;
    wp->ap = values.ap;
    wp->maxAp = values.maxAp;
    wp->starPoint = values.starPoint;
    wp->coin = values.coin;
    wp->partyId = values.partyId;
    if (wp->partyId != 0) {
        wp->partyHp = values.partyHp;
        wp->partyMaxHp = values.partyMaxHp;
    }
}

void statusWinForceUpdateCoin(void) {
    StatusWindowValue values;

    statusGetValue(&values);
    wp->coin = values.coin;
}

Vec statusGetApPos(void) {
    Vec pos;

    pos.x = wp->originX + alwaysDt[STATUS_DT_AP].xpos;
    pos.y = wp->originY - alwaysDt[STATUS_DT_AP].ypos;
    pos.z = float_0_8041c900;
    return pos;
}

void statusAPBlink(s32 value) {
    wp->blinkMask = 0;
    wp->blinkMask |= STATUS_BLINK_AP;
    wp->blinkTimer = value;
}

void N_statusClearBlink(void) {
    wp->blinkMask = 0;
}

static inline s32 valueMoveAmount(s16 cur, s16 target) {
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

void statusFPBlink(s32 value) {
    wp->blinkMask |= STATUS_BLINK_FP;
    wp->blinkTimer = value;
}

void statusMarioHPBlink(s32 value) {
    wp->blinkMask |= STATUS_BLINK_MARIO_HP;
    wp->blinkTimer = value;
}

void statusPartyHPBlink(s32 value) {
    wp->blinkMask |= STATUS_BLINK_PARTY_HP;
    wp->blinkTimer = value;
}

const f32 float_0_8041c900 = 0.0f;
const f32 float_1_8041c904 = 1.0f;
const f32 float_3p1416_8041c908 = 3.1415927f;
const f32 float_0p5_8041c90c = 0.5f;

// 4-byte zero word that ends the unit's .sdata: an all-zero object needs explicit_zero_data to stay in .sdata and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_804118FC_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
