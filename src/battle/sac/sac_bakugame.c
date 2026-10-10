#include "battle/sac/sac_bakugame.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_camera.h"
#include "party/party_motion.h"
#include "evt/evt_audience.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "battle/battle_audience.h"
#include "battle/battle_disp.h"
#include "battle/battle_database_common.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/mapdrv.h"
#include "eff/eff_starstone.h"
#include "eff/eff_sandars.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <string.h>

// Data objects defined later in this file, declared before their first use.
extern s32 hitCheck_MarioTeam[];

extern s32 hitCheck_EnemyTeam[];
extern s32 hitCheck_AudienceTeam[];
extern s32 sac_start_camera_evt[];
extern s32 sac_mario_item_on_evt[];
extern s32 sac_mario_item_on_end_evt[];
extern s32 sac_mario_back_home[];
extern s32 sac_start_stardust[];
extern s32 sac_ac_help_on[];
extern s32 sac_ac_help_off[];

extern BattleWork* _battleWorkPointer;

typedef struct BakuGameWorkEntry {
    s32 field_0x0;   // 0x0
    s32 field_0x4;   // 0x4
    s32 type;        // 0x8, 0x100/0x200/0x400
    s32 index;       // 0xC
    u8 field_0x10[0x14 - 0x10];
    Vec pos;         // 0x14
    f32 field_0x20;  // 0x20
} BakuGameWorkEntry;

// Bounce-arrow counters living inside BattleWork at 0x1F2C (bytes 0..4 unsigned, 5..6 signed).
typedef struct BakuGameBouncePos {
    u8 field_0x0;
    u8 field_0x1;
    u8 field_0x2;
    u8 field_0x3;
    u8 field_0x4;
    s8 field_0x5;
    s8 field_0x6;
} BakuGameBouncePos;

struct BakuGameWork {
    s32 flags;                        // 0x0
    u8 field_0x4[0xC - 0x4];          // 0x4
    BakuGameWorkEntry entries[3];     // 0xC
    u8 field_0x78[0x88 - 0x78];       // 0x78
    Vec position;                     // 0x88
    u8 field_0x94[0xC4 - 0x94];       // 0x94
    f32 field_0xC4;                   // 0xC4
    u8 field_0xC8[0xD0 - 0xC8];       // 0xC8
    f32 field_0xD0;                   // 0xD0
    u8 field_0xD4[0xDC - 0xD4];       // 0xD4
    f32 field_0xDC;                   // 0xDC
    f32 field_0xE0;                   // 0xE0
    u8 field_0xE4[0xE8 - 0xE4];       // 0xE4
    f32 field_0xE8;                   // 0xE8
    s32 field_0xEC;                   // 0xEC
    u8 field_0xF0[0xF4 - 0xF0];       // 0xF0
    s32 field_0xF4;                   // 0xF4
    BattleWeapon weapon;              // 0xF8
    u8 field_0x1B8[0x1C0 - 0x1B8];    // 0x1B8
    s32 field_0x1C0;                  // 0x1C0
    s32 field_0x1C4;                  // 0x1C4
    s32 field_0x1C8;                  // 0x1C8
    s8 field_0x1CC;                   // 0x1CC
    u8 field_0x1CD[0x1D0 - 0x1CD];    // 0x1CD
    Vec field_0x1D0;                  // 0x1D0
    Vec field_0x1DC;                  // 0x1DC
    Vec field_0x1E8;                  // 0x1E8
    f32 field_0x1F4;                  // 0x1F4
    f32 field_0x1F8;                  // 0x1F8
    u8 field_0x1FC[0x200 - 0x1FC];    // 0x1FC
    s32 field_0x200;                  // 0x200
    s32 state;                        // 0x204
    s32 frame;                        // 0x208
    Vec field_0x20C;                  // 0x20C
    Vec field_0x218;                  // 0x218
    Vec field_0x224;                  // 0x224
    u8 field_0x230[0xC];              // 0x230
    Vec field_0x23C;                  // 0x23C
    Vec field_0x248;                  // 0x248
    EffectEntry* starStone;           // 0x254
};

// The weapon strings open .rodata. Every later entry is a local initializer's template (pooled where the
// function is parsed), a named string defined after the code, or a literal pooled in .text order.
const char str_802FEA98[20] = "btl_wn_sac_bakugame";
const char str_802FEAAC[16] = "msg_bakugame\000\000\000";
const char str_802FEABC[12] = "msg_sac_3\000\000";
// bakuGameDisp3D's material colour, the first .sdata2 word; the floats and short strings are literals.
const u32 dat_80421600 = 0x000000FF;

// The weapon bakuGameDecideWeapon copies into the work (0xC0 bytes, a BattleWeapon): name, message, stats and
// the attack event script. Written as words because BattleWeapon's header leaves most of its fields unnamed.
extern s32 marioAttackEvent_BakuGame[];
static void* weapon[0xC0 / 4] = {
    (void*)str_802FEA98, (void*)0x01980000, NULL, (void*)str_802FEAAC,
    (void*)0x64000200, (void*)0x3F800000, (void*)0x01056432, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, (void*)0x02100060, (void*)0x20000000, (void*)0x00000302,
    (void*)str_802FEABC, (void*)0x00020088, (void*)0x000007FF, NULL,
    (void*)0x00006401, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    (void*)marioAttackEvent_BakuGame, (void*)0x00000064, NULL, NULL,
};
static s32 main_star(EventEntry* event, BOOL isFirstCall);
static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall);
extern s32 psndSFX_pit(s32 id, s32 pit);

// Keep GetBakuGamePtr out of line: every caller emits bl GetBakuGamePtr as in the original.
#pragma dont_inline on
// Prototypes for the forward references the original function order needs.
static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall);
static USER_FUNC(main_star);

BakuGameWork* GetBakuGamePtr(void) {
    return (BakuGameWork*)((u8*)_battleWorkPointer + 0x1F4C);
}
#pragma dont_inline reset

static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall) {
    BakuGameWork* baku = GetBakuGamePtr();

    baku->state = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(main_star) {
    BakuGameWork* baku = GetBakuGamePtr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 scale;

    switch (baku->state) {
        case 0:
        case 4:
            break;
        case 1: {
            baku->state = 2;
            baku->frame = 0;
            {
                f32 zero = 0.0f;

                baku->starStone = effStarStoneEntry(1, zero, -1000.0f, zero, 1.0f);
            }
            BtlUnit_GetPos(mario, &baku->field_0x20C.x, &baku->field_0x20C.y, &baku->field_0x20C.z);
            baku->field_0x20C.y += mario->mSizeMultiplier * (f32)mario->mHeight + 37.0f;
            baku->field_0x218 = baku->field_0x20C;
            baku->field_0x224 = baku->field_0x20C;
            baku->field_0x224.y += 50.0f;
            baku->field_0x224.z += -1.0f;
            baku->field_0x23C = (Vec){ 0.0f, 0.0f, 0.0f };
            baku->field_0x248 = (Vec){ 0.0f, 0.0f, 0.0f };
        }
            // fall through
        case 2:
            baku->frame++;
            if (baku->frame <= 100) {
                baku->field_0x20C.x = intplGetValue(0, baku->frame, 100, baku->field_0x218.x, baku->field_0x224.x);
                baku->field_0x20C.y = intplGetValue(0, baku->frame, 100, baku->field_0x218.y, baku->field_0x224.y);
                baku->field_0x20C.z = intplGetValue(0, baku->frame, 100, baku->field_0x218.z, baku->field_0x224.z);
                scale = intplGetValue(0, baku->frame, 100, 0.0f, 2.0f);
                baku->field_0x23C.z = scale;
                baku->field_0x23C.y = scale;
                baku->field_0x23C.x = scale;
            } else {
                baku->field_0x20C.x = baku->field_0x224.x;
                baku->field_0x20C.y = baku->field_0x224.y;
                baku->field_0x20C.z = baku->field_0x224.z;
                baku->field_0x23C.z = 2.0f;
                baku->field_0x23C.y = 2.0f;
                baku->field_0x23C.x = 2.0f;
            }
            baku->field_0x248.y = intplGetValue(4, baku->frame, 0x78, 0.0f, 2160.0f);
            if (baku->frame >= 0x78) {
                baku->state = 3;
                baku->frame = 0;
                baku->field_0x218 = baku->field_0x20C;
                baku->field_0x224.y = 300.0f;
            }
            break;
        case 3:
            baku->frame++;
            baku->field_0x20C.y = intplGetValue(1, baku->frame, 0x3C, baku->field_0x218.y, baku->field_0x224.y);
            if (baku->frame >= 0x3C) {
                baku->state = 4;
                baku->frame = 0;
            }
            break;
    }
    if (baku->starStone != NULL) {
        f32* e = baku->starStone->userdata;

        e[2] = baku->field_0x20C.x;
        e[3] = baku->field_0x20C.y;
        e[4] = baku->field_0x20C.z;
        e[6] = baku->field_0x248.x;
        e[7] = baku->field_0x248.y;
        e[8] = baku->field_0x248.z;
        e[5] = 1.5f * baku->field_0x23C.x;
    }
    dispEntry(4, 0.0f, 2, bakuGameDispStar, NULL);
    return EVT_RETURN_BLOCK;
}

s32 init_bakugame(EventEntry* event, BOOL isFirstCall) {
    memset(GetBakuGamePtr(), 0, 0x2B8);
    return EVT_RETURN_DONE;
}

s32 end_bakugame(EventEntry* event, BOOL isFirstCall) {
    BakuGameWork* baku = GetBakuGamePtr();
    void* q = baku->starStone->userdata;
    effDelete(*(EffectEntry**)((u8*)q + 0x34));
    effDelete(baku->starStone);
    return EVT_RETURN_DONE;
}

s32 bakuGameDecideWeapon(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 v;
    BakuGameWork* baku;
    s8* p;
#ifdef MUST_MATCH
    s32 weaponArg;
#endif

    evtGetFloat(event, event->args[0]);
    v = evtGetValue(event, args[1]);
    baku = GetBakuGamePtr();
#ifdef MUST_MATCH
    // The +0x80/-0x80 round trip forces MWCC to keep the interior pointer in a register across the memcpy
    // instead of folding it into each store offset; net value is unchanged.
    p = (s8*)&baku->weapon.sleepChance;
    p = (s8*)((u32)p + 0x80);
    p = (s8*)((u32)p - 0x80);
#else
    p = (s8*)&baku->weapon.sleepChance;
#endif
    memcpy(&baku->weapon, &weapon, sizeof(BattleWeapon));
    p[2] = 100;
    if (v != -1) {
        if (baku->field_0xD0 >= 100.0f) {
            p[2] = 0x7F;
        } else {
            p[2] = 100;
        }
        p[3] = v - 1;
    } else {
        p[2] = 0;
        p[3] = 0;
    }
    evtSetValue(event, args[2], (s32)&baku->weapon);
    return EVT_RETURN_DONE;
}

s32 bakuGameBombEntry(EventEntry* event, BOOL isFirstCall) {
    BakuGameWork* baku = GetBakuGamePtr();

    if (animGroupBaseAsync("c_star_bom", 2, 0) == 0) {
        return 0;
    }
    baku->field_0xEC = animPoseEntry("c_star_bom", 2);
    animPoseSetAnim(baku->field_0xEC, "Z_1", 1);
    return 2;
}

// psndSFX_pit has no shared prototype; this file passes a full-width pitch (fctiwz result).

// `e` and one loop's `unit` are declared at function scope and `args` is assigned after the declarations:
// those ranks give the original's callee-saved registers. `order` and the case-0 `home` are never read; their
// initializer templates are the original's unreferenced .rodata entries between main_star's and this
// function's vectors. The 40-frame count is a local so `(s32)t` stays a run-time conversion as in the original.
s32 bakuGameMain(EventEntry* event, BOOL isFirstCall) {
    BakuGameWork* baku = GetBakuGamePtr();
    BakuGameWorkEntry* e;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    BattleUnit* mario = BattleGetUnitPtr(wp, BattleTransID(event, -3));
#ifdef MUST_MATCH
    volatile s32* args;
#else
    s32* args;
#endif
    BattleUnit* party = BattleGetUnitPtr(wp, BattleTransID(event, -4));
    f32 px = 0.0f;
    f32 py = 0.0f;
    f32 pz = 0.0f;
#ifdef MUST_MATCH
    // Retail loads -2.0f into f31 only when the counter is exactly 0x1A and reads f31 for every count >= 0x1A, so
    // on counts 0x1B-0x1E it adds whatever f31 still holds; the natural build uses -2.0f throughout.
    f32 neg2;
#else
    f32 neg2 = -2.0f;
#endif
    f32 hx = 0.0f;
    f32 hy = 0.0f;
    f32 hz = 0.0f;
    f32 c80v, cnv, c2v;
    f32 f30v;
    s32 order[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    s32 tbl[3] = { 0x100, 0x200, 0x400 };
    s32 i;
    s32 flag;
    s32 d;

    args = event->args;

    if (isFirstCall) {
        *(s32*)((u8*)baku + 0x4) = 0;
        bakuGameDecideButton(1);
        for (i = 0; i < 3; i++) {
            BakuGameWorkEntry* entry = &baku->entries[i];
            entry->type = tbl[i];
            *(f32*)&entry->field_0x10[0] = 100.0f;
            entry->field_0x20 = 0.0f;
        }
        memcpy(&baku->weapon, &weapon, 0xC0);

        flag = 1;
        for (i = 0; i < 64; i++) {
            BattleUnit* unit = BattleGetUnitPtr(wp, i);
            if (unit != NULL && unit->mAlliance == 1) {
                if ((f32)unit->mWidth >= 100.0f) {
                    flag = 1;
                    break;
                }
                if (!(unit->mAttributeFlags & 2)) {
                    flag = 0;
                }
            }
        }
        if (flag == 0) {
            baku->flags &= ~1;
        } else {
            baku->flags |= 1;
        }

        flag = 0;
        for (i = 0; i < 0xC8; i++) {
            if (bakuGameAudienceCanThrowPos(i) == 1) {
                if ((u8)BattleAudience_GetWaiting(i) == 1) {
                    flag = 1;
                }
            }
        }
        if (flag == 0) {
            baku->flags |= 2;
        } else {
            baku->flags &= ~2;
        }
    }

    switch (*(s32*)((u8*)baku + 0x4)) {
        case 0:
            for (i = 0; i < 3; i++) {
                BakuGameWorkEntry* entry = &baku->entries[i];
                if (entry->field_0x0 != 5) goto first_done;
            }
            if (*(s32*)((u8*)baku + 0x7C) != 8) break;
            if ((baku->flags & 1) && baku->field_0x1C0 != 5) break;
            *(s32*)((u8*)baku + 0x4) = 5;
            break;
        case 5:
            *(s32*)((u8*)baku + 0x4) = 10;
            *(s32*)((u8*)baku + 0x8) = 0;
            for (i = 0; i < 3; i++) {
                BakuGameWorkEntry* entry = &baku->entries[i];
                entry->field_0x0 = 10;
                entry->field_0x4 = 0;
            }
            *(s32*)((u8*)baku + 0x7C) = 10;
            *(s32*)((u8*)baku + 0xF0) = irand(0x78) + 0x21C;
            /* fall through */
        case 10:
            (*(s32*)((u8*)baku + 0x8))++;
            if (*(s32*)((u8*)baku + 0x8) > 0x3C && irand(0x96) == 0) {
                for (i = 0; i < 3; i++) {
                    BakuGameWorkEntry* entry = &baku->entries[i];
                    entry->field_0x0 = 10;
                }
                bakuGameDecideButton(0);
                *(s32*)((u8*)baku + 0x8) = 0;
            }
            if (*(s32*)((u8*)baku + 0x8) == 0xB4) {
                *(s32*)((u8*)baku + 0x8) = 0xB4;
                for (i = 0; i < 3; i++) {
                    BakuGameWorkEntry* entry = &baku->entries[i];
                    entry->field_0x0 = 10;
                }
                bakuGameDecideButton(0);
                *(s32*)((u8*)baku + 0x8) = 0;
            }
            {
                BakuGameWorkEntry* entry;
                s32 sel = -1;
                if (*(u8*)((u8*)baku + 0xCC) == 1) {
                    if (irand(2) == 0) {
                        sel = 0;
                    } else {
                        sel = 2;
                    }
                }
                if (*(u8*)((u8*)baku + 0xCC) == 2) {
                    if (irand(2) == 0) {
                        sel = 0;
                    } else {
                        sel = 1;
                    }
                }
                if (sel != -1) {
                    for (i = 0; i < 3; i++) {
                        entry = &baku->entries[i];
                        if (entry->index == sel) {
                            *(f32*)&entry->field_0x10[0] -= 5.0f;
                        }
                    }
                }
            }
            break;
        case 20:
            for (i = 0; i < 3; i++) {
                BakuGameWorkEntry* entry = &baku->entries[i];
                entry->field_0x0 = 0x14;
            }
            *(s32*)((u8*)baku + 0x7C) = 0x28;
            *(s32*)((u8*)baku + 0x80) = 0x3C;
            *(s32*)((u8*)baku + 0x4) = 0x19;
            break;
        case 25:
            if (*(s32*)((u8*)baku + 0x7C) == 0x32) {
                *(s32*)((u8*)baku + 0x4) = 0x1E;
            }
            break;
        case 30:
            animPoseRelease(baku->field_0xEC);
            baku->field_0xEC = -1;
            evtSetValue(event, args[0], *(u8*)((u8*)baku + 0xCC));
            evtSetFloat(event, args[1], baku->field_0xD0);
            {
                s32 r = -1;
                if (baku->field_0xD0 > 0.0f && baku->field_0xD0 < 33.0f) {
                    r = 2;
                }
                if (baku->field_0xD0 >= 33.0f && baku->field_0xD0 < 66.0f) {
                    r = 3;
                }
                if (baku->field_0xD0 >= 66.0f) {
                    r = 4;
                }
                evtSetValue(event, args[2], (r < 4) ? r : 4);
            }
            return 2;
        default:
            break;
    }
first_done:

    for (i = 0; i < 3; i++) {
        e = &baku->entries[i];
        switch (e->field_0x0) {
            case 0:
                e->field_0x0 = 5;
                break;
            case 10:
                e->field_0x0 = 15;
                e->field_0x20 = 1.0f;
                break;
            case 12:
                e->field_0x20 = intplGetValue(4, e->field_0x4, 0x1E, 0.0f, 1.0f);
                e->field_0x4++;
                if (e->field_0x4 > 0x1E) {
                    e->field_0x0 = 15;
                }
                break;
            case 15:
                if ((keyGetButtonTrg(0) & e->type) != 0 && baku->field_0xF4 == 0) {
                    *(f32*)&e->field_0x10[0] -= 100.0f;
                }
                break;
            case 20:
                e->pos = (Vec){ 0.0f, -1000.0f, 0.0f };
                break;
            default:
                break;
        }
    }

    {
        Vec home = { 125.0f, 0.0f, -30.0f };
    }
    switch (baku->field_0x1C0) {
        case 0:
            if (!(baku->flags & 1)) break;
            baku->field_0x1C0 = 2;
            baku->field_0x1CC = -1;
            baku->field_0x200 = 0x5A;
            baku->field_0x1D0 = (Vec){ 125.0f, 0.0f, 100.0f };
            baku->field_0x1DC = baku->field_0x1D0;
            baku->field_0x1D0.x = 300.0f;
            baku->field_0x1DC.x = 300.0f;
            baku->field_0x1E8 = (Vec){ 125.0f, 0.0f, 100.0f };
            baku->field_0x1C4 = 0;
            baku->field_0x1C8 = 0x1E;
            /* fall through */
        case 2:
            baku->field_0x1D0.x = intplGetValue(0, baku->field_0x1C4, baku->field_0x1C8, baku->field_0x1DC.x,
                                                baku->field_0x1E8.x);
            {
                s32 m = baku->field_0x1C4 % 12;
                if (m >= 0 && m <= 3) baku->field_0x200 = 0x5A;
                if (m >= 4 && m <= 7) baku->field_0x200 = 0x5B;
                if (m >= 8 && m <= 0xB) baku->field_0x200 = 0x5C;
            }
            baku->field_0x1C4++;
            if (baku->field_0x1C4 > baku->field_0x1C8) {
                baku->field_0x1C0 = 5;
                baku->field_0x1D0 = baku->field_0x1E8;
            }
            break;
        case 5:
            baku->field_0x200 = 0x52;
            break;
        case 10:
            baku->field_0x200 = 0x54;
            break;
        case 15:
            break;
        default:
            break;
    }

    if (*(s32*)((u8*)baku + 0x7C) >= 0xA && *(s32*)((u8*)baku + 0x7C) <= 0x14 &&
        *(s32*)((u8*)baku + 0xF0) <= 0) {
        *(s32*)((u8*)baku + 0x4) = 0x14;
        switch (*(u8*)((u8*)baku + 0xCC)) {
            case 0:
                bakuGameMarioSurpriseReset(mario, party);
                break;
            case 1:
                bakuGameEnemySurpriseReset();
                break;
            case 2:
                bakuGameAudienceSurpriseReset();
                break;
            default:
                break;
        }
    }

    if (*(s32*)((u8*)baku + 0x7C) >= 0xA && *(s32*)((u8*)baku + 0x7C) <= 0x1E) {
        if ((keyGetButtonTrg(0) & 0x700) != 0) {
            *(f32*)((u8*)baku + 0xD4) = 1.5f;
        }
        *(f32*)((u8*)baku + 0xD8) = -0.3f;
        baku->field_0xD0 += *(f32*)((u8*)baku + 0xD4);
        baku->field_0xD0 += *(f32*)((u8*)baku + 0xD8);
        if (baku->field_0xD0 > 110.0f) {
            baku->field_0xD0 = 110.0f;
        }
        if (baku->field_0xD0 < 0.0f) {
            baku->field_0xD0 = 0.0f;
            if (psndSFXChk(*(s32*)((u8*)baku + 0x1B8)) == 0) {
                psndSFXOff(*(s32*)((u8*)baku + 0x1B8));
            }
        }
        if (baku->field_0xD0 > 0.0f && psndSFXChk(*(s32*)((u8*)baku + 0x1B8)) == -1) {
            *(s32*)((u8*)baku + 0x1B8) = psndSFXOn("SFX_BTL_SAC_TIME1");
        }
        if (psndSFXChk(*(s32*)((u8*)baku + 0x1B8)) == 0) {
            psndSFX_pit(*(s32*)((u8*)baku + 0x1B8),
                        (s32)(4096.0f * baku->field_0xD0 / 100.0f));
        }
        *(f32*)((u8*)baku + 0xD4) -= 0.25f;
        if (*(f32*)((u8*)baku + 0xD4) < 0.0f) {
            *(f32*)((u8*)baku + 0xD4) = 0.0f;
        }
        {
            f32 v;
            if (baku->field_0xD0 >= 100.0f) {
                v = 1.0f;
            } else {
                v = baku->field_0xD0 / 100.0f;
            }
            baku->field_0xE8 = v;
        }
        (*(s32*)((u8*)baku + 0xF0))--;
        (baku->field_0xF4)--;
        if (baku->field_0xF4 < 0) {
            baku->field_0xF4 = 0;
        }
        if (*(s32*)((u8*)baku + 0xF0) == 0x1E0) {
            psndSFXOn("SFX_BTL_SAC_TIME4");
        }
        if (*(s32*)((u8*)baku + 0xF0) >= 0x1E0 && *(s32*)((u8*)baku + 0xF0) < 0x1EA) {
            f32 v = intplGetValue(0, 0x1E9 - *(s32*)((u8*)baku + 0xF0), 0xA, 2.0f, 3.0f);
            *(f32*)((u8*)baku + 0xC8) = v;
            baku->field_0xC4 = v;
        }
        if (*(s32*)((u8*)baku + 0xF0) == 0x168) {
            psndSFXOn("SFX_BTL_SAC_TIME4");
        }
        if (*(s32*)((u8*)baku + 0xF0) >= 0x168 && *(s32*)((u8*)baku + 0xF0) < 0x172) {
            f32 v = intplGetValue(0, 0x171 - *(s32*)((u8*)baku + 0xF0), 0xA, 3.0f, 4.0f);
            *(f32*)((u8*)baku + 0xC8) = v;
            baku->field_0xC4 = v;
        }
        if (*(s32*)((u8*)baku + 0xF0) == 0xF0) {
            psndSFXOn("SFX_BTL_SAC_TIME4");
        }
        if (*(s32*)((u8*)baku + 0xF0) >= 0xF0 && *(s32*)((u8*)baku + 0xF0) < 0xFA) {
            f32 v = intplGetValue(0, 0xF9 - *(s32*)((u8*)baku + 0xF0), 0xA, 4.0f, 5.0f);
            *(f32*)((u8*)baku + 0xC8) = v;
            baku->field_0xC4 = v;
        }
        if (*(s32*)((u8*)baku + 0xF0) == 0x78) {
            psndSFXOn("SFX_BTL_SAC_TIME4");
        }
        if (*(s32*)((u8*)baku + 0xF0) >= 0x78 && *(s32*)((u8*)baku + 0xF0) < 0x82) {
            f32 v = intplGetValue(0, 0x81 - *(s32*)((u8*)baku + 0xF0), 0xA, 5.0f, 6.0f);
            *(f32*)((u8*)baku + 0xC8) = v;
            baku->field_0xC4 = v;
        }
        if (*(s32*)((u8*)baku + 0xF0) == 0x257) {
            animPoseSetAnim(baku->field_0xEC, "S_1", 0);
        }
        if (*(s32*)((u8*)baku + 0xF0) == 0x190) {
            animPoseSetAnim(baku->field_0xEC, "S_2", 0);
        }
        if (*(s32*)((u8*)baku + 0xF0) == 0xC8) {
            animPoseSetAnim(baku->field_0xEC, "S_3", 0);
        }
    }

    switch (*(s32*)((u8*)baku + 0x7C)) {
        case 0:
            *(s32*)((u8*)baku + 0x7C) = 2;
            baku->field_0xE0 = 50.0f;
            *(s32*)((u8*)baku + 0x80) = 0;
            baku->position = (Vec){ 0.0f, -1000.0f, 0.0f };
            {
                s32* pF0 = (s32*)((u8*)baku + 0xF0);
                *pF0 = -1;
            }
            *(f32*)((u8*)baku + 0xC8) = 2.0f;
            baku->field_0xC4 = 2.0f;
            *(f32*)((u8*)baku + 0xC0) = 90.0f;
            /* fall through */
        case 2:
            baku->field_0xDC =
                intplGetValue(4, *(s32*)((u8*)baku + 0x80), 0x1E, -1000.0f, 50.0f);
            (*(s32*)((u8*)baku + 0x80))++;
            if (*(s32*)((u8*)baku + 0x80) > 0x1E) {
                *(s32*)((u8*)baku + 0x7C) = 4;
            }
            break;
        case 4:
            c2v = 2.0f;
            cnv = -800.0f;
            c80v = 80.0f;
            *(s32*)((u8*)baku + 0x7C) = 5;
            baku->position = (Vec){ -125.0f, 500.0f, -30.0f };
            {
                Vec* p94 = (Vec*)((u8*)baku + 0x94);
                *p94 = baku->position;
            }
            *(Vec*)((u8*)baku + 0xA0) = (Vec){ -125.0f, 0.0f, -30.0f };
            {
                f32* pA4 = (f32*)((u8*)baku + 0xA4);
                f32* p98 = (f32*)((u8*)baku + 0x98);
                f32* pB0 = (f32*)((u8*)baku + 0xB0);
                *pB0 = (c2v * (*pA4 - *p98) - cnv) / c80v;
            }
            *(s32*)((u8*)baku + 0x80) = 0x28;
            *(s32*)((u8*)baku + 0x84) = 0x28;
            /* fall through */
        case 5:
            baku->position.y += *(f32*)((u8*)baku + 0xB0);
            *(f32*)((u8*)baku + 0xB0) += -0.5f;
            (*(s32*)((u8*)baku + 0x80))--;
            if (*(s32*)((u8*)baku + 0x80) < 0) {
                *(s32*)((u8*)baku + 0x7C) = 8;
                baku->position.y = *(f32*)((u8*)baku + 0xA4);
                *(u8*)((u8*)baku + 0xCC) = 0;
                bakuGameMarioSurprise(mario, party);
                btl_camera_shake_h(0, 0xA, 4, 4.0f, 0.0f);
                psndSFXOn("SFX_BTL_SAC_TIME2");
                BtlUnit_snd_se_pos(mario, "SFX_VOICE_MARIO_SURPRISED1", 0x7F, 0, &mario->mPosition);
            }
            break;
        case 10:
            *(s32*)((u8*)baku + 0x7C) = 0xB;
            *(s32*)((u8*)baku + 0x80) = 0;
            *(f32*)((u8*)baku + 0xC0) = 70.0f;
            /* fall through */
        case 11:
            {
            f32 zero = 0.0f;
            for (i = 0; i < 3; i++) {
                BakuGameWorkEntry* entry = &baku->entries[i];
                if (*(f32*)&entry->field_0x10[0] <= zero) {
                    *(s32*)((u8*)baku + 0x7C) = 0x14;
                    *(s32*)((u8*)baku + 0x80) = 0;
                    *(u8*)((u8*)baku + 0xCD) = (u8)entry->index;
                    break;
                }
            }
            }
            if (*(u8*)((u8*)baku + 0xCC) != 0) break;
            BtlUnit_GetPos(mario, &px, &py, &pz);
            BtlUnit_GetHomePos(mario, &hx, &hy, &hz);
            mario->faceDirection = -1;
            if (py == hy) {
                if (irand(0x50) == 0) {
                    {
                        f32 h = -200.0f;
                        *(f32*)((u8*)baku + 0x274) = -h / 20.0f;
                    }
                    BtlUnit_SetPos(mario, px, py + 0.1f, pz);
                }
            } else {
                py += *(f32*)((u8*)baku + 0x274);
                *(f32*)((u8*)baku + 0x274) += -2.0f;
                if (py < hy) py = hy;
                BtlUnit_SetPos(mario, px, py, pz);
            }
            if (bakuGamePartyExist(party) == 1) {
                BtlUnit_GetPos(party, &px, &py, &pz);
                BtlUnit_GetHomePos(party, &hx, &hy, &hz);
                if (py == hy) {
                    if (irand(0x50) == 0) {
                        {
                            f32 h = -200.0f;
                            *(f32*)((u8*)baku + 0x298) = -h / 20.0f;
                        }
                        BtlUnit_SetPos(party, px, py + 0.1f, pz);
                    }
                } else {
                    py += *(f32*)((u8*)baku + 0x298);
                    *(f32*)((u8*)baku + 0x298) += -2.0f;
                    if (py < hy) py = hy;
                    BtlUnit_SetPos(party, px, py, pz);
                }
            }
            break;
        case 20:
            if (*(u8*)((u8*)baku + 0xCC) == *(u8*)((u8*)baku + 0xCD)) {
                *(s32*)((u8*)baku + 0x7C) = 0x1E;
                break;
            }
            switch (*(u8*)((u8*)baku + 0xCC)) {
                case 0:
                    if (*(s32*)((u8*)baku + 0x80) == 0) {
                        BtlUnit_GetPos(mario, &px, &py, &pz);
                        BtlUnit_GetHomePos(mario, &hx, &hy, &hz);
                        BtlUnit_SetPos(mario, hx, hy, hz);
                        {
                            Vec v = { 0.0f, 0.0f, 0.0f };
                            v.x = px;
                            v.y = hy;
                            v.z = pz;
                            *(Vec*)((u8*)baku + 0x258) = v;
                        }
                        {
                            Vec v;
                            v = (Vec){ 0.0f, 0.0f, 0.0f };
                            v.x = baku->position.x;
                            v.y = hy;
                            v.z = 7.0f + baku->position.z;
                            *(Vec*)((u8*)baku + 0x264) = v;
                        }
                        mario->faceDirection = -1;
                        if (bakuGamePartyExist(party) == 1) {
                            BtlUnit_GetPos(party, &px, &py, &pz);
                            BtlUnit_GetHomePos(party, &hx, &hy, &hz);
                            BtlUnit_SetPos(party, hx, hy, hz);
                        }
                        BtlUnit_SetBodyAnim(mario, "M_W_1");
                    }
                    {
                        s32 d = *(s32*)((u8*)baku + 0x80);
                        if (d >= 0 && d < 8) {
                            px = intplGetValue(0, d, 7, *(f32*)((u8*)baku + 0x258),
                                               *(f32*)((u8*)baku + 0x264));
                            py = *(f32*)((u8*)baku + 0x25C);
                            pz = intplGetValue(0, *(s32*)((u8*)baku + 0x80), 7, *(f32*)((u8*)baku + 0x260),
                                               *(f32*)((u8*)baku + 0x26C));
                            BtlUnit_SetPos(mario, px, py, pz);
                        }
                    }
                    if (*(s32*)((u8*)baku + 0x80) == 8) {
                        BtlUnit_SetBodyAnim(mario, "M_U_2");
                        mario->faceDirection = 1;
                        *(Vec*)((u8*)baku + 0xA0) = baku->position;
                        *(Vec*)((u8*)baku + 0x94) = *(Vec*)((u8*)baku + 0xA0);
                        *(f32*)((u8*)baku + 0xA4) += 30.0f * mario->mSizeMultiplier;
                    }
                    {
                        s32 d = *(s32*)((u8*)baku + 0x80);
                        if (d >= 8 && d < 0xD) {
                            baku->position.y = intplGetValue(4, d - 8, 4,
                                                                       *(f32*)((u8*)baku + 0x98),
                                                                       *(f32*)((u8*)baku + 0xA4));
                        }
                    }
                    if (*(s32*)((u8*)baku + 0x80) > 0xF) {
                        *(s32*)((u8*)baku + 0x7C) = 0x16;
                    }
                    break;
                case 1:
                    if (!(baku->flags & 1)) {
                        if (*(s32*)((u8*)baku + 0x80) == 0x1A) {
                            {
                                f32 h = -100.0f;
                                baku->field_0x1F8 = -h / 20.0f;
                            }
                            baku->field_0x1D0.y = 0.0f;
                        }
                        if (*(s32*)((u8*)baku + 0x80) >= 0x1A) {
                            baku->field_0x1D0.y += baku->field_0x1F8;
                            baku->field_0x1F8 += -1.0f;
                        }
                        if (*(s32*)((u8*)baku + 0x80) > 0x1E) {
                            *(s32*)((u8*)baku + 0x7C) = 0x16;
                        }
                        for (i = 0; i < 64; i++) {
                            unit = BattleGetUnitPtr(wp, i);
                            if (unit != NULL && !(unit->mAttributeFlags & 0x20000) &&
                                !BtlUnit_CheckStatus(unit, 0x1B) && unit->mAlliance == 1 &&
                                BtlUnit_CanActStatus(unit)) {
                                if (*(s32*)((u8*)baku + 0x80) == 0) {
                                    BtlUnit_SetBodyAnimType(unit, 0x41);
                                }
                                if (*(s32*)((u8*)baku + 0x80) == 0x1A) {
                                    btlDispPoseAnime(BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit)));
                                    if (!(unit->mAttributeFlags & 4) && !(unit->mAttributeFlags & 2)) {
                                        BtlUnit_GetHomePos(unit, &hx, &hy, &hz);
                                        BtlUnit_SetPos(unit, hx, hy, hz);
                                    }
                                }
                                if (*(s32*)((u8*)baku + 0x80) >= 0x1A &&
                                    !(unit->mAttributeFlags & 4) && !(unit->mAttributeFlags & 2)) {
                                    BtlUnit_GetHomePos(unit, &hx, &hy, &hz);
                                    BtlUnit_SetPos(unit, hx, hy + baku->field_0x1D0.y, hz);
                                }
                            }
                        }
                    } else {
                        baku->field_0x1C0 = 0xF;
                        if (*(s32*)((u8*)baku + 0x80) == 0x1A) {
                            {
                                f32 h = -200.0f;
                                baku->field_0x1F8 = -h / 20.0f;
                            }
                            neg2 = -2.0f;
                        }
                        if (*(s32*)((u8*)baku + 0x80) >= 0x1A) {
                            baku->field_0x1D0.y += baku->field_0x1F8;
                            baku->field_0x1F8 += neg2;
                            baku->position.y = 30.0f + baku->field_0x1D0.y;
                        }
                        if (*(s32*)((u8*)baku + 0x80) > 0x1E) {
                            *(s32*)((u8*)baku + 0x7C) = 0x16;
                        }
                    }
                    break;
                case 2: {
                    f32 lim;
                    f32 scale;

                    if (*(u8*)((u8*)baku + 0xCD) == 1) {
                        scale = 2.0f;
                    } else {
                        scale = 1.0f;
                    }
                    if (baku->flags & 2) {
                        *(s32*)((u8*)baku + 0x7C) = 0x1E;
                        goto state_done;
                    }
                    lim = 26.0f / scale;
                    if ((f32)*(s32*)((u8*)baku + 0x80) == lim) {
                        *(f32*)((u8*)baku + 0x2A4) = 0.0f;
                        {
                            f32 h = -200.0f;
                            *(f32*)((u8*)baku + 0x2B0) = -h / 20.0f;
                        }
                    }
                    if ((f32)*(s32*)((u8*)baku + 0x80) >= lim) {
                        *(f32*)((u8*)baku + 0x2A4) += *(f32*)((u8*)baku + 0x2B0);
                        *(f32*)((u8*)baku + 0x2B0) += -2.0f;
                    }
                    if ((f32)*(s32*)((u8*)baku + 0x80) > 30.0f / scale) {
                        *(s32*)((u8*)baku + 0x7C) = 0x16;
                    }
                    for (i = 0; i < 0xC8; i++) {
                        BattleAudienceMember* m = BattleAudienceGetPtr(i);
                        if (bakuGameAudienceCanThrowPos(i) == 1 && (u8)BattleAudience_GetExist(i) &&
                            (m->flags & 0x10)) {
                            BattleAudience_GetHomePosition(i, &px, &py, &pz);
                            BattleAudience_SetPosition(i, px, py + *(f32*)((u8*)baku + 0x2A4), pz);
                            baku->position.y =
                                py + *(f32*)((u8*)baku + 0x2A4) + 20.0f;
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            (*(s32*)((u8*)baku + 0x80))++;
            if ((keyGetButtonTrg(0) & 0x400) != 0) {
                *(u8*)((u8*)baku + 0xCD) = (u8)baku->entries[2].index;
            }
            if ((keyGetButtonTrg(0) & 0x200) != 0) {
                *(u8*)((u8*)baku + 0xCD) = (u8)baku->entries[1].index;
            }
            if ((keyGetButtonTrg(0) & 0x100) != 0) {
                *(u8*)((u8*)baku + 0xCD) = (u8)baku->entries[0].index;
            }
            break;
        case 22:
            switch (*(u8*)((u8*)baku + 0xCC)) {
                case 0:
                    f30v = 30.0f;
                    baku->position = (Vec){ -125.0f, 0.0f, -30.0f };
                    *(Vec*)((u8*)baku + 0x94) = baku->position;
                    {
                        f32* p98 = (f32*)((u8*)baku + 0x98);
                        *p98 += f30v;
                    }
                    baku->position.y += f30v;
                    bakuGameMarioSurpriseReset(mario, party);
                    BtlUnit_SetBodyAnim(mario, "M_C_1");
                    break;
                case 1:
                    baku->position = (Vec){ 125.0f, 0.0f, -30.0f };
                    *(Vec*)((u8*)baku + 0x94) = baku->position;
                    if (baku->flags & 1) {
                        baku->position = baku->field_0x1D0;
                        *(Vec*)((u8*)baku + 0x94) = baku->position;
                        {
                            f32 t = 30.0f + baku->field_0x1D0.y;
                            baku->position.y = t;
                            *(f32*)((u8*)baku + 0x98) = t;
                        }
                    }
                    bakuGameEnemySurpriseReset();
                    break;
                case 2:
                    baku->position = (Vec){ 0.0f, -50.0f, 150.0f };
                    *(Vec*)((u8*)baku + 0x94) = baku->position;
                    *(f32*)((u8*)baku + 0x98) += 30.0f;
                    baku->position.y += 20.0f;
                    bakuGameAudienceSurpriseReset();
                    break;
                default:
                    break;
            }
            switch (*(u8*)((u8*)baku + 0xCD)) {
                case 0:
                    *(Vec*)((u8*)baku + 0xA0) = (Vec){ -125.0f, 0.0f, -30.0f };
                    bakuGameMarioSurprise(mario, party);
                    break;
                case 1:
                    *(Vec*)((u8*)baku + 0xA0) = (Vec){ 125.0f, 0.0f, -30.0f };
                    if (baku->flags & 1) {
                        *(Vec*)((u8*)baku + 0xA0) = (Vec){ 125.0f, 0.0f, 100.0f };
                        *(f32*)((u8*)baku + 0xA4) += 30.0f;
                    }
                    bakuGameEnemySurprise();
                    break;
                case 2:
                    if (baku->flags & 2) {
                        *(Vec*)((u8*)baku + 0xA0) = (Vec){ 0.0f, -50.0f, 150.0f };
                    } else {
                        *(Vec*)((u8*)baku + 0xA0) = (Vec){ 0.0f, -50.0f, 150.0f };
                        *(f32*)((u8*)baku + 0xA4) += 20.0f;
                    }
                    bakuGameAudienceSurprise();
                    break;
                default:
                    break;
            }
            *(s32*)((u8*)baku + 0x7C) = 0x19;
            {
                f32 t = 40.0f;

                *(f32*)((u8*)baku + 0xB0) =
                    (2.0f * (*(f32*)((u8*)baku + 0xA4) - *(f32*)((u8*)baku + 0x98)) - -800.0f) / 80.0f;
                *(s32*)((u8*)baku + 0x80) = *(s32*)((u8*)baku + 0x84) = (s32)t;
            }
            if (*(u8*)((u8*)baku + 0xCC) == 0) {
                BtlUnit_GetPos(mario, &px, &py, &pz);
                {
                    Vec v = { 0.0f, 0.0f, 0.0f };
                    v.x = px;
                    v.y = py;
                    v.z = pz;
                    *(Vec*)((u8*)baku + 0x258) = v;
                }
                BtlUnit_GetHomePos(mario, &px, &py, &pz);
                {
                    Vec v = { 0.0f, 0.0f, 0.0f };
                    v.x = px;
                    v.y = py;
                    v.z = pz;
                    *(Vec*)((u8*)baku + 0x264) = v;
                }
            }
            *(s32*)((u8*)baku + 0x1BC) = psndSFXOn("SFX_BTL_SAC_TIME3");
            {
                s32 num = BattleAudience_GetAudienceNum();
                switch (*(u8*)((u8*)baku + 0xCD)) {
                    case 0:
                        BattleAudienceSoundBooing();
                        break;
                    case 1:
                        if (num >= 1 && num <= 0x31) BattleAudienceSoundCallKind(1);
                        if (num >= 0x32 && num <= 0x63) {
                            BattleAudienceSoundCallKind(1);
                            BattleAudienceSoundWhistleKind(1);
                        }
                        if (num >= 0x64 && num <= 0x95) {
                            BattleAudienceSoundCallKind(2);
                            BattleAudienceSoundWhistleKind(2);
                        }
                        if (num >= 0x96) {
                            BattleAudienceSoundCallKind(2);
                            BattleAudienceSoundWhistleKind(3);
                        }
                        break;
                    case 2:
                        if (num >= 1 && num <= 0x31) BattleAudienceSoundBooingKind(5);
                        if (num >= 0x32 && num <= 0x63) BattleAudienceSoundBooingKind(6);
                        if (num >= 0x64 && num <= 0x95) BattleAudienceSoundBooingKind(7);
                        if (num >= 0x96) BattleAudienceSoundBooingKind(8);
                        break;
                    default:
                        break;
                }
            }
            /* fall through */
        case 25:
            baku->position.x =
                intplGetValue(0, *(s32*)((u8*)baku + 0x84) - *(s32*)((u8*)baku + 0x80),
                              *(s32*)((u8*)baku + 0x84), *(f32*)((u8*)baku + 0x94),
                              *(f32*)((u8*)baku + 0xA0));
            baku->position.z =
                intplGetValue(0, *(s32*)((u8*)baku + 0x84) - *(s32*)((u8*)baku + 0x80),
                              *(s32*)((u8*)baku + 0x84), *(f32*)((u8*)baku + 0x9C),
                              *(f32*)((u8*)baku + 0xA8));
            baku->position.y += *(f32*)((u8*)baku + 0xB0);
            *(f32*)((u8*)baku + 0xB0) += -0.5f;
            switch (*(u8*)((u8*)baku + 0xCC)) {
                case 0:
                    if (*(s32*)((u8*)baku + 0x84) - *(s32*)((u8*)baku + 0x80) == 0x14) {
                        BtlUnit_SetBodyAnim(mario, "M_W_1");
                    }
                    if ((d = *(s32*)((u8*)baku + 0x84) - *(s32*)((u8*)baku + 0x80)) >= 0x14 && d <= 0x22) {
                        px = intplGetValue(0, d - 0x14, 0xE,
                                           *(f32*)((u8*)baku + 0x258), *(f32*)((u8*)baku + 0x264));
                        py = *(f32*)((u8*)baku + 0x25C);
                        pz = intplGetValue(0, *(s32*)((u8*)baku + 0x84) - *(s32*)((u8*)baku + 0x80) - 0x14, 0xE,
                                           *(f32*)((u8*)baku + 0x260), *(f32*)((u8*)baku + 0x26C));
                        BtlUnit_SetPos(mario, px, py, pz);
                    }
                    if (*(s32*)((u8*)baku + 0x84) - *(s32*)((u8*)baku + 0x80) == 0x22) {
                        btlDispPoseAnime(BtlUnit_GetPartsPtr(mario, BtlUnit_GetBodyPartsId(mario)));
                    }
                    break;
                case 1:
                    if (baku->field_0x1D0.y != 0.0) {
                        baku->field_0x1D0.y += baku->field_0x1F8;
                        baku->field_0x1F8 += -1.0f;
                    }
                    if (baku->field_0x1D0.y < 0.0f) {
                        baku->field_0x1D0.y = 0.0f;
                    }
                    if (!(baku->flags & 1)) {
                        for (i = 0; i < 64; i++) {
                            BattleUnit* unit = BattleGetUnitPtr(wp, i);
                            if (unit != NULL && unit->mAlliance == 1 && BtlUnit_CanActStatus(unit) &&
                                !(unit->mAttributeFlags & 0x20000) && !BtlUnit_CheckStatus(unit, 0x1B) &&
                                !(unit->mAttributeFlags & 4) && !(unit->mAttributeFlags & 2)) {
                                BtlUnit_GetHomePos(unit, &hx, &hy, &hz);
                                BtlUnit_SetPos(unit, hx, hy + baku->field_0x1D0.y, hz);
                            }
                        }
                    } else {

                        baku->field_0x1C0 = 0xF;
                        if (baku->field_0x1D0.y != 0.0f) {
                            baku->field_0x1D0.y += baku->field_0x1F8;
                            baku->field_0x1F8 += -2.0f;
                        }
                        if (baku->field_0x1D0.y <= 0.0f) {
                            baku->field_0x1D0.y = 0.0f;
                            baku->field_0x1C0 = 5;
                        }
                    }
                    break;
                case 2:
                    if (*(f32*)((u8*)baku + 0x2A4) != 0.0f) {
                        *(f32*)((u8*)baku + 0x2A4) += *(f32*)((u8*)baku + 0x2B0);
                        *(f32*)((u8*)baku + 0x2B0) += -2.0f;
                    }
                    if (*(f32*)((u8*)baku + 0x2A4) <= 0.0f) {
                        *(f32*)((u8*)baku + 0x2A4) = 0.0f;
                    }
                    for (i = 0; i < 0xC8; i++) {
                        BattleAudienceMember* m = BattleAudienceGetPtr(i);
                        if (bakuGameAudienceCanThrowPos(i) == 1 && (u8)BattleAudience_GetExist(i) &&
                            (m->flags & 0x10)) {
                            BattleAudience_GetHomePosition(i, &px, &py, &pz);
                            BattleAudience_SetPosition(i, px, py + *(f32*)((u8*)baku + 0x2A4), pz);
                        }
                    }
                    break;
                default:
                    break;
            }
            (*(s32*)((u8*)baku + 0x80))--;
            if (*(s32*)((u8*)baku + 0x80) < 0) {
                *(s32*)((u8*)baku + 0x7C) = 0x1E;
                baku->position = *(Vec*)((u8*)baku + 0xA0);
                *(u8*)((u8*)baku + 0xCC) = *(u8*)((u8*)baku + 0xCD);
                btl_camera_shake_h(0, 0xA, 4, baku->field_0xC4, 0.0f);
                psndSFXOn("SFX_BTL_SAC_TIME2");
                if (psndSFXChk(*(s32*)((u8*)baku + 0x1BC)) == 0) {
                    psndSFXOff(*(s32*)((u8*)baku + 0x1BC));
                }
            }
            break;
        case 30:
            *(s32*)((u8*)baku + 0x7C) = 0xA;
            for (i = 0; i < 3; i++) {
                BakuGameWorkEntry* entry = &baku->entries[i];
                *(f32*)&entry->field_0x10[0] = 100.0f;
            }
            if (*(u8*)((u8*)baku + 0xCC) == 1 && (baku->flags & 1)) {
                baku->field_0x1C0 = 0xA;
            }
            if (*(u8*)((u8*)baku + 0xCD) == 0) {
                BtlUnit_snd_se_pos(mario, "SFX_VOICE_MARIO_SURPRISED1", 0x7F, 0, &mario->mPosition);
            }
            break;
        case 40:
            effSandarsEntry(3, baku->position.x, baku->position.y,
                            baku->position.z, 0.0f);
            btl_camera_shake_h(0, 0x1E, 4, baku->field_0xC4, 0.0f);
            psndSFXOn("SFX_BTL_SAC_TIME5");
            if (psndSFXChk(*(s32*)((u8*)baku + 0x1B8)) == 0) {
                psndSFXOff(*(s32*)((u8*)baku + 0x1B8));
            }
            if (psndSFXChk(*(s32*)((u8*)baku + 0x1BC)) == 0) {
                psndSFXOff(*(s32*)((u8*)baku + 0x1BC));
            }
            *(s32*)((u8*)baku + 0x7C) = 0x32;
            for (i = 0; i < 0xC8; i++) {
                BattleAudienceMember* m = BattleAudienceGetPtr(i);
                m->flags &= ~0x10;
                if (bakuGameAudienceCanThrowPos(i) == 1) {
                    BattleAudience_GetHomePosition(i, &px, &py, &pz);
                    BattleAudience_SetPosition(i, px, py, pz);
                }
            }
            break;
        case 50:
            break;
        default:
            break;
    }
state_done:

    if (baku->field_0xEC != -1) {
        animPoseMain(baku->field_0xEC);
    }
    dispEntry(8, 900.0f, 1, (DispCallback)bakuGameDisp2D, NULL);
    dispEntry(4, 0.0f, 1, bakuGameDisp3D, NULL);
    return 0;
}

s32 bakuGameHeihoReturn(EventEntry* event, BOOL isFirstCall) {
    BakuGameWork* baku = GetBakuGamePtr();
    f32 zero;

    if (!(baku->flags & 1)) {
        return 2;
    }
    if (isFirstCall) {
        baku->field_0x1C0 = 0;
    }
    switch (baku->field_0x1C0) {
        case 0:
            if (0.0f != baku->field_0x1D0.y) {
                baku->field_0x1D0.y += baku->field_0x1F8;
                baku->field_0x1F8 += -2.0f;
            }
            if (baku->field_0x1D0.y <= (zero = 0.0f)) {
                baku->field_0x1D0.y = zero;
                baku->field_0x1C0 = 5;
            }
            break;
        case 5: {
            Vec v;

            baku->field_0x1C0 = 10;
            baku->field_0x1CC = 1;
            baku->field_0x1DC = baku->field_0x1D0;
            v = (Vec){ 300.0f, 0.0f, 0.0f };
            v.z = baku->field_0x1D0.z;
            baku->field_0x1E8 = v;
            baku->field_0x1C4 = 0;
            baku->field_0x1C8 = 0x1E;
            // fall through
        }
        case 10: {
            s32 m;

            baku->field_0x1D0.x = intplGetValue(0, baku->field_0x1C4, baku->field_0x1C8, baku->field_0x1DC.x,
                                                baku->field_0x1E8.x);
            m = baku->field_0x1C4 % 12;
            if (m >= 0 && m <= 3) {
                baku->field_0x200 = 0x5A;
            }
            if (m >= 4 && m <= 7) {
                baku->field_0x200 = 0x5B;
            }
            if (m >= 8 && m <= 0xB) {
                baku->field_0x200 = 0x5C;
            }
            baku->field_0x1C4++;
            if (baku->field_0x1C4 > baku->field_0x1C8) {
                return 2;
            }
            break;
        }
    }
    dispEntry(4, 0.0f, 1, bakuGameDisp3D, NULL);
    return 0;
}

// Keep bakuGameDecideButton out of line: bakuGameMain calls it three times and the original issues bl each time.
#pragma dont_inline on
// The position and permutation tables are local initializers (their templates are the original's .rodata), and
// entry = &baku->entries[i] gives the loop index/entry registers of the original.
void bakuGameDecideButton(s32 arg) {
    BakuGameWork* baku = GetBakuGamePtr();
    Vec positions[3] = {
        { -125.0f, 50.0f, 0.0f },
        { 125.0f, 50.0f, 0.0f },
        { 0.0f, 10.0f, 150.0f },
    };
    s32 perms[6][3] = {
        { 0, 1, 2 },
        { 0, 2, 1 },
        { 1, 0, 2 },
        { 1, 2, 0 },
        { 2, 0, 1 },
        { 2, 1, 0 },
    };
    s32 rnd;
    s32 i;

    rnd = irand(6);

    for (i = 0; i < 3; i++) {
        BakuGameWorkEntry* entry = &baku->entries[i];
        entry->index = perms[rnd][i];
        entry->pos = positions[perms[rnd][i]];
    }
    if (arg == 0) {
        baku->field_0xF4 = 0x3C;
    }
}
#pragma dont_inline reset

// Keep bakuGamePartyExist out of line: callers emit bl instead of inlining the status checks.
#pragma dont_inline on
BOOL bakuGamePartyExist(BattleUnit* unit) {
    if (unit == NULL) {
        return 0;
    }
    if (BtlUnit_CheckStatus(unit, 0x1B)) {
        return 0;
    }
    return BtlUnit_CanActStatus(unit) != 0;
}
#pragma dont_inline reset

// Keep bakuGameAudienceCanThrowPos out of line: callers emit bl instead of inlining the loop.
#pragma dont_inline on
s32 bakuGameAudienceCanThrowPos(s32 id) {
    s32 i;
    s32 ofs = 0;

    for (i = 0; i < 3; i++) {
        if (id >= ofs + 8 && id <= ofs + 0xB) {
            return 1;
        }
        ofs += 0x14;
    }
    return 0;
}
#pragma dont_inline reset

// Keep bakuGameMarioSurpriseReset out of line: the original issues a call (bl) to it.
#pragma dont_inline on
void bakuGameMarioSurpriseReset(BattleUnit* unit, BattleUnit* party) {
    btlDispPoseAnime(BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit)));
    if (bakuGamePartyExist(party)) {
        btlDispPoseAnime(BtlUnit_GetPartsPtr(party, BtlUnit_GetBodyPartsId(party)));
    }
}
#pragma dont_inline reset

// Keep bakuGameEnemySurpriseReset out of line: the original issues a call (bl) to it.
#pragma dont_inline on
void bakuGameEnemySurpriseReset(void) {
    s32 i;
    BattleWork* wp = _battleWorkPointer;

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);

        if (unit != NULL && unit->mAlliance == 1 && !(unit->mAttributeFlags & 0x20000) &&
            !BtlUnit_CheckStatus(unit, 0x1B)) {
            btlDispPoseAnime(BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit)));
        }
    }
}
#pragma dont_inline reset

// Keep bakuGameAudienceSurpriseReset out of line: the original issues a call (bl) to it.
#pragma dont_inline on
void bakuGameAudienceSurpriseReset(void) {
    s32 i;

    for (i = 0; i < 200; i++) {
        if ((u8)BattleAudience_GetExist(i)) {
            BattleAudienceMember* member = BattleAudienceGetPtr(i);

            member->field_0xE8 = 90.0f;
            if (!bakuGameAudienceCanThrowPos(i)) {
                member->flags &= ~0x10;
            }
        }
    }
}
#pragma dont_inline reset

// Keep bakuGameMarioSurprise out of line: the original issues a call (bl) to it.
#pragma dont_inline on
void bakuGameMarioSurprise(BattleUnit* unit, BattleUnit* party) {
    BtlUnit_SetBodyAnim(unit, "M_N_7");
    if (bakuGamePartyExist(party)) {
        switch (party->initialKind) {
            case 0xE0:
                BtlUnit_SetBodyAnim(party, "PKR_D_1");
                break;
            case 0xE1:
                BtlUnit_SetBodyAnim(party, "PNK_T_2");
                break;
            case 0xE3:
                BtlUnit_SetBodyAnim(party, "PWD_D_1");
                break;
            case 0xE2:
                BtlUnit_SetBodyAnim(party, "PYS_D_1");
                break;
            case 0xE4:
                BtlUnit_SetBodyAnim(party, "PTR_H_1");
                break;
            case 0xE5:
                BtlUnit_SetBodyAnim(party, "PBM_D_1");
                break;
            case 0xE6:
                BtlUnit_SetBodyAnim(party, "PCH_A2_3");
                break;
        }
    }
}
#pragma dont_inline reset

// Keep bakuGameEnemySurprise out of line: the original issues a call (bl) to it.
#pragma dont_inline on
void bakuGameEnemySurprise(void) {
    s32 i;
    BattleWork* wp = _battleWorkPointer;

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);

        if (unit != NULL && unit->mAlliance == 1 && !(unit->mAttributeFlags & 0x20000) &&
            !BtlUnit_CheckStatus(unit, 0x1B)) {
            BtlUnit_SetBodyAnimType(unit, 0x27);
        }
    }
}
#pragma dont_inline reset

// Keep bakuGameAudienceSurprise out of line: the original issues a call (bl) to it.
#pragma dont_inline on
void bakuGameAudienceSurprise(void) {
    s32 i;

    for (i = 0; i < 200; i++) {
        BattleAudienceMember* member = BattleAudienceGetPtr(i);

        if ((u8)BattleAudience_GetWaiting(i)) {
            member->flags |= 0x10;
            memset(&member->interpFramesLeft, 0, 0x34);
            if (!bakuGameAudienceCanThrowPos(i)) {
                BattleAudience_SetAnim(i, 7, 0);
                if (irand(2) == 0) {
                    member->field_0xE8 = 90.0f;
                } else {
                    member->field_0xE8 = 270.0f;
                }
            } else {
                BattleAudience_SetAnim(i, 3, 0);
            }
        }
    }
}
#pragma dont_inline reset

// Taking the entry through this accessor is what gets bakuGameDisp2D's loads in the original's order (permuter).
static inline BakuGameWorkEntry* bakuGameEntry(BakuGameWork* work, s32 i) {
    return &work->entries[i];
}

void bakuGameDisp2D(void) {
    BattleWork* wp;
    BakuGameWork* baku;
    s32 i;
    s32 flag;

    baku = GetBakuGamePtr();
    wp = _battleWorkPointer;
#ifdef MUST_MATCH
    // Match-only round trip (net value unchanged): forces MWCC to keep wp in r31 so baku lands in r30.
    wp = (BattleWork*)((u32)wp + 0x1F2C);
    wp = (BattleWork*)((u32)wp - 0x1F2C);
#endif
    for (i = 0; i < 3; i++) {
        BakuGameWorkEntry* entry = bakuGameEntry(baku, i);
        s16 icon;

        switch (entry->type) {
            case 0x100:
                icon = 0x64;
                break;
            case 0x200:
                icon = 0x66;
                break;
            case 0x400:
                icon = 0x68;
                break;
            default:
                icon = 0;
                break;
        }
        if ((u16)icon != 0) {
            Vec screen;

            btlGetScreenPoint(&entry->pos, &screen);
            iconDispGx(entry->field_0x20, &screen, 0x10, icon);
        }
    }
    flag = 0;
    if (baku->field_0xD0 >= 100.0f) {
        BakuGameBouncePos* p = (BakuGameBouncePos*)((u8*)wp + 0x1F2C);

        flag = 1;
        p->field_0x0 = 0;
        p->field_0x3 = 0xFF;
        p->field_0x1 += p->field_0x5;
        p->field_0x2 += p->field_0x6;
        if (p->field_0x1 <= 0x80 && p->field_0x5 <= 0) {
            p->field_0x6 = 0x10;
            p->field_0x5 = 0x10;
        }
        if (p->field_0x1 >= 0xF5 && p->field_0x5 >= 0) {
            p->field_0x6 = -0x10;
            p->field_0x5 = -0x10;
        }
    }
    BattleAcDrawGauge((s32)baku->field_0xDC, (s32)baku->field_0xE0, 0xB1, 3, 0x21, 0x42, 0x64, flag, baku->field_0xE8);
}

void bakuGameDisp3D(CameraId camId, void* param) {
    BakuGameWork* baku = GetBakuGamePtr();
    CameraEntry* cam = camGetPtr(4);
    Mtx scale;
    Mtx trans;
    Mtx mtx;
    GXTexObj texObj;
    s32 idx;

    PSMTXScale(scale, -1.0f, 1.0f, 1.0f);
    PSMTXTrans(trans, baku->position.x, baku->position.y, baku->position.z);
    PSMTXConcat(trans, scale, mtx);
    if (baku->field_0xEC != -1) {
        animPoseDrawMtx(baku->field_0xEC, mtx, 2, 180.0f, baku->field_0xC4);
    }
    if (baku->flags & 1) {
        void* file;
        TEXPalette* pal;

        GXSetCullMode(0);
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xD, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
        GXSetNumChans(1);
        GXSetChanCtrl(4, 0, 1, 0, 0, 2, 2);
        GXSetChanMatColor(4, *(GXColor*)&dat_80421600);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
        GXSetNumTevStages(1);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0, 0xF, 0xC, 8, 0xA);
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(0, 7, 5, 4, 7);
        GXSetTevOrder(0, 0, 0, 4);
        file = BattleAudienceBaseGetPtr()->normalTex;
        pal = *(TEXPalette**)(*(u8**)((u8*)file + 0xA0));
        TEXGetGXTexObjFromPalette(pal, &texObj, baku->field_0x200);
        GXLoadTexObj(&texObj, 0);
        PSMTXTrans(trans, baku->field_0x1D0.x, baku->field_0x1D0.y, baku->field_0x1D0.z);
        PSMTXConcat(cam->view, trans, mtx);
        GXLoadPosMtxImm(mtx, 0);
        {
            f32 texCoords[16] = {
                0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            };

            idx = (baku->field_0x1CC == 1) ? 4 : 0;
            GXBegin(0x80, 0, 4);
            GXPosition3f32(-20.0f, 40.0f, 0.0f);
            GXTexCoord2f32(texCoords[(idx + 0) * 2], texCoords[(idx + 0) * 2 + 1]);
            GXPosition3f32(20.0f, 40.0f, 0.0f);
            GXTexCoord2f32(texCoords[(idx + 1) * 2], texCoords[(idx + 1) * 2 + 1]);
            GXPosition3f32(20.0f, 0.0f, 0.0f);
            GXTexCoord2f32(texCoords[(idx + 2) * 2], texCoords[(idx + 2) * 2 + 1]);
            GXPosition3f32(-20.0f, 0.0f, 0.0f);
            GXTexCoord2f32(texCoords[(idx + 3) * 2], texCoords[(idx + 3) * 2 + 1]);
            GXEnd();
        }
    }
}

void bakuGameDispStar(CameraId camId, void* param) {
}

// The attack event's sound names come after every function's templates in .rodata.
const char str_802FECA8[20] = "SFX_BTL_SAC_START1\000";
const char str_802FECBC[20] = "SFX_BTL_SAC_START2\000";
const char str_802FECD0[20] = "SFX_BTL_SAC_START3\000";

s32 marioAttackEvent_BakuGame[119] = {
    0x0001005B, (s32)(void*)btlevtcmd_WaitGuardMove, 0x0002005B, (s32)(void*)btlevtcmd_CommandPayWeaponCost,
    0xFFFFFFFE, 0x0003005B, (s32)(void*)btlevtcmd_CommandGetWeaponAddress, 0xFFFFFFFE,
    0xFE363C80, 0x0002005B, (s32)(void*)btlevtcmd_WeaponAftereffect, 0xFE363C80,
    0x0001005B, (s32)(void*)btlevtcmd_reset_move_color_lv_all, 0x0001005B, (s32)(void*)init_bakugame,
    0x0000006E, 0x0001005B, (s32)(void*)main_star, 0x00000070,
    0x0001005E, (s32)(void*)sac_start_camera_evt, 0x0001000A, 0x000001F4,
    0x0006005B, (s32)(void*)btlevtcmd_snd_se, 0xFFFFFFFE, (s32)(void*)str_802FECA8,
    0xF1194D80, 0x00000000, 0xF1194D80, 0x0001005E,
    (s32)(void*)sac_mario_item_on_evt, 0x0001000A, 0x000001F4, 0x0006005B,
    (s32)(void*)btlevtcmd_snd_se, 0xFFFFFFFE, (s32)(void*)str_802FECBC, 0xF1194D80,
    0x00000000, 0xFE363C8B, 0x0001005B, (s32)(void*)star_stone_appear,
    0x0000006E, 0x0001005E, (s32)(void*)sac_start_stardust, 0x00000070,
    0x00010009, 0x00000064, 0x0006005B, (s32)(void*)btlevtcmd_snd_se,
    0xFFFFFFFE, (s32)(void*)str_802FECD0, 0xF1194D80, 0x00000000,
    0xF1194D80, 0x0002005B, (s32)(void*)evt_snd_sfxoff, 0xFE363C8B,
    0x0003005B, (s32)(void*)evt_btl_camera_set_mode, 0x00000000, 0x00000000,
    0x00010009, 0x00000028, 0x0001005E, (s32)(void*)sac_mario_item_on_end_evt,
    0x0001005E, (s32)(void*)sac_mario_back_home, 0x0001005B, (s32)(void*)bakuGameBombEntry,
    0x0001005B, (s32)(void*)sac_ac_help_on, 0x0004005B, (s32)(void*)bakuGameMain,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0001005B,
    (s32)(void*)sac_ac_help_off, 0x0001005B, (s32)(void*)end_bakugame, 0x0000006B,
    0x0001005B, (s32)(void*)bakuGameHeihoReturn, 0x0000006D, 0x0004005B,
    (s32)(void*)bakuGameDecideWeapon, 0xFE363C81, 0xFE363C82, 0xFE363C89,
    0x00010009, 0x00000005, 0x00010022, 0xFE363C80,
    0x00010024, 0x00000000, 0x0001005E, (s32)(void*)hitCheck_MarioTeam,
    0x0000002E, 0x00010024, 0x00000001, 0x0001005E,
    (s32)(void*)hitCheck_EnemyTeam, 0x0000002E, 0x00010024, 0x00000002,
    0x0001005E, (s32)(void*)hitCheck_AudienceTeam, 0x0000002E, 0x00000031,
    0x00010003, 0x00000063, 0x0002005B, (s32)(void*)btlevtcmd_StartWaitEvent,
    0xFFFFFFFE, 0x00000002, 0x00000001,
};
s32 hitCheck_MarioTeam[146] = {
    0x0003005B, (s32)(void*)btlevtcmd_GetUnitId, 0xFFFFFFFC, 0xFE363C83,
    0x00020018, 0xFE363C83, 0xFFFFFFFF, 0x00010004,
    0x00000032, 0x00000021, 0x0004005B, (s32)(void*)btlevtcmd_CheckStatus,
    0xFE363C83, 0x0000001B, 0xFE363C80, 0x00020019,
    0xFE363C80, 0x00000000, 0x00010004, 0x00000032,
    0x00000021, 0x0003005B, (s32)(void*)btlevtcmd_GetBodyId, 0xFE363C83,
    0xFE363C84, 0x0006005B, (s32)(void*)btlevtcmd_GetHitPos, 0xFE363C83,
    0xFE363C84, 0xFE363C8A, 0xFE363C8B, 0xFE363C8C,
    0x0005005B, (s32)(void*)btlevtcmd_ACSuccessEffect, 0xFE363C82, 0xFE363C8A,
    0xFE363C8B, 0xFE363C8C, 0x0007005B, (s32)(void*)btlevtcmd_CheckDamage,
    0xFFFFFFFE, 0xFE363C83, 0xFE363C84, 0xFE363C89,
    0x00000100, 0xFE363C85, 0x00010003, 0x00000032,
    0x0003005B, (s32)(void*)btlevtcmd_GetUnitId, 0xFFFFFFFD, 0xFE363C83,
    0x00020018, 0xFE363C83, 0xFFFFFFFF, 0x00010004,
    0x00000063, 0x00000021, 0x0004005B, (s32)(void*)btlevtcmd_CheckStatus,
    0xFE363C83, 0x0000001B, 0xFE363C80, 0x00020019,
    0xFE363C80, 0x00000000, 0x00010004, 0x00000063,
    0x00000021, 0x0003005B, (s32)(void*)btlevtcmd_GetBodyId, 0xFE363C83,
    0xFE363C84, 0x0003005B, (s32)(void*)btlevtcmd_GetFaceDirection, 0xFE363C83,
    0xFE363C80, 0x00020018, 0xFE363C80, 0xFFFFFFFF,
    0x00010005, 0x00000005, 0x0005005B, (s32)(void*)btlevtcmd_AddRotate,
    0xFE363C83, 0x00000000, 0x00000012, 0x00000000,
    0x00010009, 0x00000001, 0x00000006, 0x0004005B,
    (s32)(void*)btlevtcmd_AnimeChangePoseType, 0xFE363C83, 0xFE363C84, 0x00000027,
    0x00010005, 0x00000005, 0x0005005B, (s32)(void*)btlevtcmd_AddRotate,
    0xFE363C83, 0x00000000, 0x00000012, 0x00000000,
    0x00010009, 0x00000001, 0x00000006, 0x0002005B,
    (s32)(void*)btlevtcmd_ResetFaceDirection, 0xFE363C83, 0x0005005B, (s32)(void*)btlevtcmd_SetRotate,
    0xFE363C83, 0x00000000, 0x00000000, 0x00000000,
    0x00000021, 0x0006005B, (s32)(void*)btlevtcmd_GetHitPos, 0xFE363C83,
    0xFE363C84, 0xFE363C8A, 0xFE363C8B, 0xFE363C8C,
    0x00020019, 0xFE363C82, 0xFFFFFFFF, 0x0005005B,
    (s32)(void*)btlevtcmd_ACSuccessEffect, 0xFE363C82, 0xFE363C8A, 0xFE363C8B,
    0xFE363C8C, 0x00000021, 0x0007005B, (s32)(void*)btlevtcmd_CheckDamage,
    0xFFFFFFFE, 0xFE363C83, 0xFE363C84, 0xFE363C89,
    0x00000100, 0xFE363C85, 0x00010003, 0x00000063,
    0x00000002, 0x00000001,
};
s32 hitCheck_EnemyTeam[53] = {
    0x0003005B, (s32)(void*)btlevtcmd_GetSelectEnemy, 0xFE363C83, 0xFE363C84,
    0x00010003, 0x0000000A, 0x0006005B, (s32)(void*)btlevtcmd_GetHitPos,
    0xFE363C83, 0xFE363C84, 0xFE363C8A, 0xFE363C8B,
    0xFE363C8C, 0x0005005B, (s32)(void*)btlevtcmd_CheckPartsAttribute, 0xFE363C83,
    0xFE363C84, 0x00004000, 0xFE363C80, 0x00020018,
    0xFE363C80, 0x00000000, 0x00020019, 0xFE363C82,
    0xFFFFFFFF, 0x0005005B, (s32)(void*)btlevtcmd_ACSuccessEffect, 0xFE363C82,
    0xFE363C8A, 0xFE363C8B, 0xFE363C8C, 0x00000021,
    0x0007005B, (s32)(void*)btlevtcmd_CheckDamage, 0xFFFFFFFE, 0xFE363C83,
    0xFE363C84, 0xFE363C89, 0x00000100, 0xFE363C85,
    0x00000021, 0x0003005B, (s32)(void*)btlevtcmd_GetSelectNextEnemy, 0xFE363C83,
    0xFE363C84, 0x00020019, 0xFE363C83, 0xFFFFFFFF,
    0x00010004, 0x0000000A, 0x00000021, 0x00000002,
    0x00000001,
};
s32 hitCheck_AudienceTeam[15] = {
    0x0002005B, (s32)(void*)evt_audience_abnormal_natural_all, 0x00000064, 0x00020019,
    0xFE363C82, 0xFFFFFFFF, 0x0005005B, (s32)(void*)btlevtcmd_ACSuccessEffect,
    0xFE363C82, 0x00000000, 0x00000064, 0x00000000,
    0x00000021, 0x00000002, 0x00000001,
};
