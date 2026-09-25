#include "statuswindow.h"
#include "battle/battle.h"
#include "driver/icondrv.h"
#include "driver/seqdrv.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "memory.h"
#include <math.h>
#include <string.h>

extern GlobalWork* gp;

//.data
typedef struct alwaysDtEntry {
    s16 xpos;   // 0x0
    s16 ypos;   // 0x2
    s16 iconId; // 0x4
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

u16 gauge_back[8] = {
    0x01B7, 0x01B8, 0x01B9, 0x01BA, 0x01BB, 0x01BC, 0x01BD, 0x01BE,
};

u16 gauge_wakka[16] = {
    0x01C1, 0x01C0, 0x01C3, 0x01C2, 0x01C5, 0x01C4, 0x01C7, 0x01C6,
    0x01C9, 0x01C8, 0x01CB, 0x01CA, 0x01CD, 0x01CC, 0x01CF, 0x01CE,
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
void gaugeDisp(s32 a1, f32 x, f32 y);
void statusGetValue(StatusWindowValue* values);
s32 marioGetParty(void);
s32 valueCheck(void);
void valueUpdate(void);
void statusWinDisp(CameraId camId, void* param);
void psndSFXOn();

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
                if (!(marioChkCtrl() && marioChkKey() && (mario->flags & 1))) {
                    if (wp->field_0x4 & 0x11) {
                        wp->field_0x4 |= 0xA;
                        wp->field_0x4 &= ~5;
                    }
                    valueCheck();
                    wp->field_0x74 = 0;
                    wp->field_0x78 = 0;
                } else {
                    if (PSVECDistance(&wp->field_0x68, &mario->position) < 1.0f) {
                        wp->field_0x74++;
                        if (wp->field_0x74 > gp->framerate * 10) {
                            wp->field_0x74 = gp->framerate * 10;
                            if (wp->field_0x7C == 0 && wp->field_0x78 == 0 && !(wp->field_0x4 & 0x11)) {
                                wp->field_0x4 &= ~0xA;
                                wp->field_0x4 |= 5;
                                wp->field_0x4 |= 0x100;
                            }
                        }
                    } else {
                        wp->field_0x74 = 0;
                        wp->field_0x7C = 0;
                        if (wp->field_0x78 == 0 && (wp->field_0x4 & 0x11)) {
                            wp->field_0x4 |= 0xA;
                            wp->field_0x4 &= ~5;
                        }
                    }
                    if (*(u16*)((u8*)mario + 0x248) & 0x10) {
                        if (!(wp->field_0x4 & 0x11)) {
                            wp->field_0x4 &= ~0xA;
                            wp->field_0x4 |= 5;
                            wp->field_0x78 = gp->framerate * 10;
                            psndSFXOn(0x2A);
                            wp->field_0x4 |= 0x100;
                        } else {
                            wp->field_0x4 |= 0xA;
                            wp->field_0x4 &= ~5;
                            wp->field_0x78 = gp->framerate * 10;
                            wp->field_0x7C = 1;
                            psndSFXOn(0x2B);
                        }
                    }
                    if (valueCheck() != 0) {
                        wp->field_0x78 = gp->framerate * 2;
                        if (!(wp->field_0x4 & 0x11)) {
                            wp->field_0x4 &= ~0xA;
                            wp->field_0x4 |= 5;
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
                if (wp->field_0x4 & 0x11) {
                    wp->field_0x4 |= 0xA;
                    wp->field_0x4 &= ~5;
                }
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
            f32 c;

            c = (f32)cos(3.1416f * wp->field_0x8 / wp->field_0xC);
            wp->field_0x10 = (wp->field_0x18 - *(f32*)((u8*)wp + 0x14)) * (1.0f - c) * 0.5f + *(f32*)((u8*)wp + 0x14);
        }

        wp->field_0x20 = wp->field_0x30 * wp->field_0x10 + wp->field_0x40 * (1.0f - wp->field_0x10);
        wp->field_0x24 = wp->field_0x34 * wp->field_0x10 + wp->field_0x44 * (1.0f - wp->field_0x10);
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

void gaugeDisp(s32 a1, f32 x, f32 y) {
    s32 count;
    s32 i;
    s32 hundreds;
    s32 remainder;
    s32 fifteenths;

    count = 0;
    for (i = 0; i < 8; i++) {
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
    remainder = a1 - hundreds * 100;
    fifteenths = remainder * 15 / 100;
    if (remainder != 0 && a1 != 0 && fifteenths == 0) {
        fifteenths = 1;
    }
    if (fifteenths != 0) {
        Vec pos;

        pos.x = x + hundreds * 32;
        pos.y = y;
        pos.z = 0.0f;
        iconDispGx(pos, 0x10, gauge_wakka[fifteenths], 1.0f);
    }
    y += 12.0f;
    for (i = 0; i < 8; i++) {
        if (pouchGetPtr()->mStarPowersObtained & (1 << i)) {
            Vec pos;

            pos.x = x + i * 32;
            pos.y = y;
            pos.z = 0.0f;
            iconDispGx(pos, 0x10, (i < hundreds) ? gauge_back[i] : 0x1BF, 1.0f);
        }
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

static void valueMove(s16* cur, s16 target) {
    s32 diff = target - *cur;
    s32 step;

    if (diff != 0) {
        step = diff / 10;
        if (step == 0) {
            step = (target > *cur) ? 1 : -1;
        }
        *cur += step;
    }
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

void valueUpdate(void) {
    static s32 count;
    StatusWindowValue values;

    statusGetValue(&values);
    count++;
    count %= 1200;
    if (count & 1) {
        return;
    }
    if (!(wp->field_0x4 & 0x11)) {
        return;
    }

    if (valueMoveAmount(*(s16*)((u8*)wp + 0x60), values.coin) != 0) {
        if (count % 8 == 0) {
            psndSFXOn("SFX_COUNT_COIN1");
        }
    }

    valueMove((s16*)((u8*)wp + 0x50), values.hp);
    valueMove((s16*)((u8*)wp + 0x52), values.maxHp);
    valueMove((s16*)((u8*)wp + 0x54), values.fp);
    valueMove((s16*)((u8*)wp + 0x56), values.maxFp);
    valueMove(&wp->field_0x62, values.ap);
    valueMove((s16*)((u8*)wp + 0x64), values.maxAp);
    valueMove((s16*)((u8*)wp + 0x5E), values.starPoint);
    valueMove((s16*)((u8*)wp + 0x60), values.coin);

    *(s16*)((u8*)wp + 0x58) = values.partyId;
    if (*(s16*)((u8*)wp + 0x58) != 0) {
        valueMove((s16*)((u8*)wp + 0x5A), values.partyHp);
        valueMove((s16*)((u8*)wp + 0x5C), values.partyMaxHp);
    }
    if (*(s16*)((u8*)wp + 0x5E) > 0x63) {
        *(s16*)((u8*)wp + 0x5E) = 0x63;
    }
    if (*(s16*)((u8*)wp + 0x60) > 0x3E7) {
        *(s16*)((u8*)wp + 0x60) = 0x3E7;
    }
}

void statusWinDisp(CameraId camId, void* param) {
    alwaysDtEntry* dtEntry;
    s32 v6;
    int i;
    Mtx iconPos, iconScale;

    v6 = 1;
    if (wp->field_0x88 > 0) {
        wp->field_0x88--;
        v6 = (wp->field_0x88 % 16) > 0;
    }

    dtEntry = alwaysDt;
    for (i = 0; i < 13; i++, dtEntry++) {
        if (dtEntry->iconId == 466 && !pouchGetPtr()->mStarPowersObtained) {
            break; // don't draw star power bar
        }
        if (!v6) {
            if (wp->field_0x80 & 1 && i == 1) {
                continue;
            }
            if (wp->field_0x80 & 2 && i == 4 || wp->field_0x80 & 0x10 && i == 8 || wp->field_0x80 & 4 && i == 12) {
                continue;
            }
        }
        MTXTrans(iconPos, wp->field_0x20 + (f32)dtEntry->xpos, wp->field_0x24 - (f32)dtEntry->ypos, 0.0f);
        MTXScale(iconScale, dtEntry->scale, dtEntry->scale, dtEntry->scale);
        MTXConcat(iconPos, iconScale, iconPos);
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
