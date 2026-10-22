#include "mario/mario_pouch.h"

#include "driver/itemdrv.h"
#include "driver/icondrv.h"
#include "driver/swdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "manager/winmgr.h"
#include "manager/fontmgr.h"
void FontDrawColor(GXColor color);
#include "driver/msgdrv.h"
#include "driver/seqdrv.h"
#include "party/party_vivian.h"
#include "data/item_data.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "motion/mot_jabara.h"
#include "driver/hitdrv.h"
#include "driver/camdrv.h"
#include "driver/windowdrv.h"
#include "eff/eff_stardust.h"
#include "eff/eff_starstone.h"
#include "eff/eff_splash.h"
#include "eff/eff_itemget.h"
#include "eff/n64/eff_hagetaka_flush_n64.h"
#include "mario/mario_motion.h"
#include "mario/mario_cam.h"
#include "pmario_sound.h"
#include "math_ext.h"
#include "system.h"
#include "memory.h"
#include <string.h>
#include "driver/npcdrv.h"
#include "party/party_nokonoko.h"
#include <stdio.h>
#include "party.h"
#include "mario/mario_party.h"

extern GlobalWork* gp;

extern int __float_nan[];
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

static inline s32 __fpclassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
        case 0x7F800000:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_NAN;
            } else {
                return FP_INFINITE;
            }
            break;
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return FP_SUBNORMAL;
            } else {
                return FP_ZERO;
            }
            break;
        default:
            return FP_NORMAL;
    }
}

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

static inline f32 sqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}


extern ItemData itemDataTable[];
const Vec vec3_802cabe0 = {0.0f, 0.0f, 0.0f};

//.bss
static ItemWork work[2];

//TODO: inline function? I see nothing in the symbol map
#define itemGetWork() (gp->inBattle ? &work[1] : &work[0])

inline void itemCheckBadge(ItemEntry* entry) {
    if (!gp->inBattle) {
        if (BADGE_MIN <= entry->itemId && entry->itemId < BADGE_MAX) {
            if (entry->flags & 0x100) {
                pouchArriveBadge(entry->itemId);
            }
        }
    }
}

void itemInit(void) { // 1:1
    ItemWork* wp;

    wp = &work[0];
    wp->count = 32;
    wp->entries = __memAlloc(HEAP_DEFAULT, sizeof(ItemEntry) * wp->count);
    memset(wp->entries, 0, sizeof(ItemEntry) * wp->count);
    wp->currId = 0;

    wp = &work[1];
    wp->count = 32;
    wp->entries = __memAlloc(HEAP_DEFAULT, sizeof(ItemEntry) * wp->count);
    memset(wp->entries, 0, sizeof(ItemEntry) * wp->count);
    wp->currId = 0;
}

extern WinMgrDesc itemdrv_window_desc[];
extern s32 all_juyoitem_lecture[];

void itemReInit(void) {
    ItemWork* wp = itemGetWork();
    ItemEntry* entry;
    int i;

    if (!gp->inBattle) {
        for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
            if (entry->flags & 1) { //flush all current badges
                itemCheckBadge(entry);
            }
        }
    }

    //clear all existing entries
    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        iconDelete(entry->name);
        entry->flags &= ~1;
        memset(entry->name, 0, sizeof(entry->name));
    }
}

void itemCoinDrop(Vec* pos) {
    ItemEntry* item;

    if (pouchGetCoin() > 0) {
        pouchAddCoin(-1);
        item = itemEntry(NULL, ITEM_COIN, 8, pos->x, pos->y, pos->z, -1, 0);
        if (item != NULL) {
            item->status |= 0x40;
        }
    }
}

s32 itemGetCheck(ItemEntry* entry) {
    MarioWork* mario;
    ItemWork* wp;
    ItemEntry* e;
    int i;

    mario = marioGetPtr();
    if (!entry) {
        return 0;
    }
    if (itemStatus(entry, 0x100000)) {
        return 0;
    }

    if (mario->characterId == 2) {
        if (!(mario->field_0x14 & 1)) {
            if (marioStGetSystemLevel() != 0) {
                return 0;
            }
            if (mario->flags & 0xa) {
                return 0;
            }
        }
    }
    else {
        if (marioStGetSystemLevel() != 0) {
            return 0;
        }
        if (mario->flags & 0xa) {
            return 0;
        }
    }

    if ((s32)gp->mapInitEvtId != 0) {
        return 0;
    }
    if (seqGetSeq() != 2) {
        return 0;
    }
    if (mario->currMotionId == MARIO_MOTION_SHIP) {
        return 0;
    }
    if (mario->currMotionId == MARIO_MOTION_PLANE) {
        return 0;
    }
    if (vivianGetStatus() != 0) {
        return 0;
    }
    if (mario->currMotionId == MARIO_MOTION_DAMAGE || mario->currMotionId == MARIO_MOTION_DAMAGE_TOGE) {
        return 0;
    }
    if (nokonoko_holdItem() != 0) {
        return 0;
    }

    wp = itemGetWork();
    for (e = wp->entries, i = 0; i < wp->count; i++, e++) {
        if ((e->flags & 1) && e->mode == 2) {
            return 0;
        }
    }
    if (N_fbatPreventMarioEventChk() != 0) {
        return 0;
    }

    switch (entry->mode) {
    case 5:
    case 6:
        if (entry->subMode != 0 && !itemStatus(entry, 0x4) && !itemStatus(entry, 0x8)) {
            return 1;
        }
        break;
    case 3:
    case 4:
        if (itemStatus(entry, 0x8)) {
            break;
        }
        return 1;
    case 1:
    case 7:
        return 1;
    default:
        break;
    }
    return 0;
}

s32 itemGetNokoCheck(ItemEntry* entry) {
    MarioWork* mario;
    u32 motion;
    ItemWork* wp;
    ItemEntry* e;
    int i;

    mario = marioGetPtr();
    if (!entry) {
        return 0;
    }
    if (itemStatus(entry, 0x100000)) {
        return 0;
    }
    if (marioStGetSystemLevel() != 0) {
        return 0;
    }
    if (mario->flags & 0xa) {
        return 0;
    }
    if (itemStatus(entry, 0x2000000)) {
        return 0;
    }
    if ((s32)gp->mapInitEvtId != 0) {
        return 0;
    }
    if (seqGetSeq() != 2) {
        return 0;
    }
    if (mario->currMotionId == MARIO_MOTION_SHIP) {
        return 0;
    }
    if (vivianGetStatus() != 0) {
        return 0;
    }
    motion = mario->currMotionId;
    if (motion == MARIO_MOTION_DAMAGE || motion == MARIO_MOTION_DAMAGE_TOGE) {
        return 0;
    }
    if ((u16)(motion - MARIO_MOTION_JUMP_SW) <= 3 || motion == MARIO_MOTION_GET_ITEM || motion == MARIO_MOTION_TALK) {
        return 0;
    }

    wp = itemGetWork();
    for (e = wp->entries, i = 0; i < wp->count; i++, e++) {
        if ((e->flags & 1) && e->mode == 2) {
            return 0;
        }
    }

    return N_fbatPreventMarioEventChk() == 0;
}

s32 N_itemPickUpFromFieldCheck(void) {
    ItemWork* wp = itemGetWork();
    ItemEntry* entry;
    int i;

    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        if (entry->flags & 1 && entry->mode == 2) {
            return 1;
        }
    }
    return 0;
}

// Same body as itemseq_deleteMatching; a separate helper so itemMain inlines the loop with the
// register allocation the target has (wp2/e/j in r25/r26/r27) without moving the shared helper.
static inline void itemseq_delMatchingMain(ItemEntry* item) {
    ItemWork* wp2 = itemGetWork();
    ItemEntry* e = wp2->entries;
    s32 j;

    for (j = 0; j < wp2->count; j++, e++) {
        if (!strcmp(e->name, item->name)) {
            itemCheckBadge(e);
            switch (e->itemId) {
            case ITEM_DIAMOND_STAR:
            case ITEM_EMERALD_STAR:
            case ITEM_GOLD_STAR:
            case ITEM_RUBY_STAR:
            case ITEM_SAPPHIRE_STAR:
            case ITEM_GARNET_STAR:
            case ITEM_CRYSTAL_STAR:
                effSoftDelete(e->effect);
                break;
            default:
                iconDelete(e->name);
                break;
            }
            e->flags &= ~1;
        }
    }
}
// The mode-9 loop is inlined via itemseq_delMatchingMain, giving the target's r25/r26/r27 allocation.
void itemMain(void) {
    ItemWork* wp;
    ItemEntry* entry;
    ItemEntry* itemHit;
    HitEntry* hit;
    MarioWork* mario;
    int i;

    wp = itemGetWork();
    mario = marioGetPtr();

    itemHit = itemHitCheck(mario->position.x, mario->position.y, mario->position.z,
                           0.5f * mario->wPlayerCollisionBox.x + 12.0f);
    if (itemGetCheck(itemHit)) {
        itemHit->mode = 2;
        itemHit->subMode = 0;
    }

    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        int active;
        u16 mode;

        if (!(entry->flags & 1)) {
            continue;
        }

        mode = entry->mode;
        active = 1;
        switch (mode) {
        case 2:
        case 7:
        case 8:
        case 10:
            break;
        default:
            if (gp->systemLevelFlags & 0xF) {
                active = 0;
            }
            break;
        }

        if (!active) {
            entry->lastTime = gp->renderTime;
            continue;
        }

        if (mode != 2) {
            if (entry->lifeTime == 0) {
                entry->status &= ~0x100;
            } else if (entry->status & 0x4000) {
                entry->status &= ~0x100;
            } else {
                f32 elapsed = (f32)OSTicksToMicroseconds(gp->renderTime - entry->lastTime) / 1000.0f;

                entry->lifeTime -= elapsed;
                if (entry->lifeTime <= 0) {
                    entry->mode = 9;
                    entry->subMode = 0;
                } else if (entry->lifeTime <= 3000
                           && ((entry->lifeTime >> 4) & 1)) {
                    entry->status ^= 0x100;
                }
            }
        } else {
            entry->status &= ~0x100;
        }

        switch (entry->mode) {
        case 5:
        case 6:
        case 10:
            itemseq_Bound(entry);
            break;
        case 3:
        case 4:
            itemseq_Bound(entry);
            break;
        case 1:
        {
            f32 vy = -1.0f;
            f32 y = entry->position.y;
            f32 hitNZ;
            f32 hitNY;
            f32 hitNX;
            f32 distance;
            f32 hitZ;
            f32 hitY;
            f32 hitX;
            f32 newY;

            if (entry->status & 0x100000) {
                hit = NULL;
                newY = y + vy;
            } else {
                f32 xx;
                f32 yy;
                f32 zz;
                f64 d;

                xx = entry->position.x;
                zz = entry->position.z;
                d = __fabs(-1.0);
                distance = (f32)(20.0 + d);
                yy = 20.0f + y;
                hit = hitCheckFilter(NULL, xx, yy, zz, 0.0f, -1.0f, 0.0f, &hitX,
                                     &hitY, &hitZ, &distance,
                                     &hitNX, &hitNY, &hitNZ);
                if (hit != NULL) {
                    newY = hitY;
                } else {
                    hit = NULL;
                    newY = y + vy;
                }
            }
            if (hit == NULL) {
                entry->mode = 3;
                entry->subMode = 0;
            } else {
                entry->position.y = newY;
            }
        }
        break;
        case 2:
            itemseq_GetItem(entry);
            break;
        case 8:
        {
            PartyEntry* party = partyGetPtr(marioGetPartyId());
            if (party != NULL) {
                f32 x = party->position.x;
                f32 y = 15.0f + party->position.y;
                f32 z = party->position.z;

                entry->position.x = x;
                entry->position.y = y;
                entry->position.z = z;
            } else {
                entry->mode = 3;
                entry->subMode = 0;
                entry->status &= ~0x1000;
            }
        }
        break;
        case 9:
            itemseq_delMatchingMain(entry);
            break;
        default:
            break;
        }

        if (!(entry->flags & 1)) {
            continue;
        }

        switch (entry->itemId) {
        case ITEM_DIAMOND_STAR:
        case ITEM_EMERALD_STAR:
        case ITEM_GOLD_STAR:
        case ITEM_RUBY_STAR:
        case ITEM_SAPPHIRE_STAR:
        case ITEM_GARNET_STAR:
        case ITEM_CRYSTAL_STAR:
            if (!(entry->status & 0x100)) {
                ((effStarStoneData*)entry->effect->userdata)->x = entry->position.x;
                ((effStarStoneData*)entry->effect->userdata)->y =
                    16.0f + entry->position.y;
                ((effStarStoneData*)entry->effect->userdata)->z = entry->position.z;
            } else {
                ((effStarStoneData*)entry->effect->userdata)->x = entry->position.x;
                ((effStarStoneData*)entry->effect->userdata)->y = -1000.0f;
                ((effStarStoneData*)entry->effect->userdata)->z = entry->position.z;
            }
            break;
        default:
            if (!(entry->status & 0x100)) {
                iconSetPos(entry->name, entry->position.x, entry->position.y, entry->position.z);
                iconSetScale(entry->name, entry->scale);
                iconSetAlpha(entry->name, entry->alpha);
            } else {
                iconSetPos(entry->name, entry->position.x, -1000.0f, entry->position.z);
            }
            break;
        }

        entry->lastTime = gp->renderTime;

        if (entry->itemId == ITEM_COIN || entry->itemId == ITEM_PIANTA_TOKEN) {
            if (irand(1000) < 0x1e) {
                s32 a = 10 - irand(20);
                s32 b = irand(20) + 10;

                effHagetakaFlushN64Entry(0, 0xa, entry->position.x + (f32)a, entry->position.y + (f32)b,
                                         entry->position.z, 1.0f);
            }
        }
    }

    {
        ItemWork* wp2 = gp->inBattle ? &work[0] : &work[1];
        ItemEntry* e;
        int j;

        for (e = wp2->entries, j = 0; j < wp2->count; j++, e++) {
            if (e->flags & 1) {
                e->lastTime = gp->renderTime;
            }
        }
    }
}

void itemModeChange(ItemEntry* item, u16 mode) {
    item->mode = mode;
    item->subMode = 0;
}

ItemEntry* itemEntry(const char* name, ItemType type, u16 a3, f32 a6, f32 a7, f32 a8, s32 evtId, s32 a5) {
	ItemWork* wp = itemGetWork();
	int i;
	ItemEntry* entry;
	int j;
	int isGswf = 0;
	int isLswf = 0;
	char itemName[8];

	if (evtId != -1) {
		if (evtId >= GSWF(0) && evtId <= EVTDAT_GSWF_MAX) {
			isGswf = 1;
			evtId += EVTDAT_GSWF_BASE;
			if (swGet(evtId)) {
				return NULL;
			}
		}
		else if (evtId >= LSWF(0) && evtId <= EVTDAT_LSWF_MAX) {
			isLswf = 1;
			evtId += EVTDAT_LSWF_BASE;
			if (_swGet(evtId)) {
				return NULL;
			}
		}
	}

	if (name == NULL) {
		wp->currId = wp->currId + 1;
		if (wp->currId >= 0xFFFFF) {
			wp->currId = 1;
		}
		sprintf(itemName, "_i%05x", wp->currId);
		name = itemName;
	}

	entry = wp->entries;
	for (i = 0; i < wp->count; i++, entry++) {
		if (entry->flags & 1 && !strcmp(entry->name, name)) {
			break;
		}
	}

	entry = wp->entries;
	for (j = 0; j < wp->count; j++, entry++) {
		if (!(entry->flags & 1)) {
			break;
		}
	}

	if (j >= wp->count) {
		entry = wp->entries;
		if ((entry->flags & 1) && entry->itemId == ITEM_COIN && (entry->flags & 0x400)) {
			itemDelete(entry->name);
		}
		else {
			entry = wp->entries;
			if ((entry->flags & 1) && entry->itemId == ITEM_FLOWER && (entry->flags & 0x400)) {
				itemDelete(entry->name);
			}
			else {
				entry = wp->entries;
				if ((entry->flags & 1) && entry->itemId == ITEM_HEART && (entry->flags & 0x400)) {
					itemDelete(entry->name);
				}
				else {
					entry = NULL;
				}
			}
		}
		if (entry == NULL) {
			return NULL;
		}
	}

	memset(entry, 0, sizeof(ItemEntry));
	strcpy(entry->name, name);
	entry->position.x = a6;
	entry->position.y = a7;
	entry->position.z = a8;
	entry->itemId = type;
	entry->spawnType = a3;
	entry->getScript = (void*)a5;
	entry->flags |= 1;
	entry->scale = 1.0f;
	entry->alpha = 0xFF;

	if (evtId != -1) {
		if (isGswf) {
			entry->evtId = evtId;
			entry->flags |= 0x20;
		}
		else if (isLswf) {
			entry->evtId = evtId;
			entry->flags |= 0x40;
		}
	}
	else {
		entry->evtId = -1;
	}

	switch (entry->itemId) {
	case ITEM_DIAMOND_STAR:
	case ITEM_EMERALD_STAR:
	case ITEM_GOLD_STAR:
	case ITEM_RUBY_STAR:
	case ITEM_SAPPHIRE_STAR:
	case ITEM_GARNET_STAR:
	case ITEM_CRYSTAL_STAR:
		entry->effect = effStarStoneEntry(entry->itemId - ITEM_DIAMOND_STAR, a6, 16.0f + a7, a8, 1.0f);
		break;
	default:
		entry->effect = NULL;
		iconEntry(entry->name, itemDataTable[type].icon_id);
		iconSetPos(entry->name, a6, a7, a8);
		iconSetScale(entry->name, entry->scale);
		iconSetAlpha(entry->name, entry->alpha);
		break;
	}

	entry->gravity = 1.0f;
	entry->boundRate = 1.0f;
	entry->lastTime = gp->renderTime;

	switch (entry->itemId) {
	default:
		break;
	case ITEM_COIN:
	case ITEM_PIANTA_TOKEN:
	case ITEM_HEART:
	case ITEM_FLOWER:
		entry->flags |= 4;
		break;
	}

	switch (a3) {
	case 17:
		entry->flags |= 2;
	case 16:
		entry->mode = 7;
		entry->subMode = 0;
		break;
	case 0:
		entry->mode = 1;
		entry->subMode = 0;
		break;
	case 1:
		entry->mode = 1;
		entry->subMode = 0;
		entry->flags |= 2;
		break;
	case 4:
		entry->mode = 6;
		entry->subMode = 0;
		break;
	case 3:
		entry->lifeTime = 5000;
		entry->mode = 5;
		entry->subMode = 0;
		break;
	case 11:
		entry->status |= 0x4000;
		entry->lifeTime = 20000;
		entry->mode = 6;
		entry->subMode = 0;
		break;
	case 12:
		entry->status |= 0x4000;
		entry->lifeTime = 20000;
		entry->mode = 6;
		entry->subMode = 0;
		entry->flags |= 0x400;
		break;
	case 9:
		entry->flags |= 0x100;
	case 8:
		entry->status |= 0x4000;
		entry->lifeTime = 8000;
		entry->mode = 6;
		entry->subMode = 0;
		break;
	case 10:
		entry->status |= 0x4000;
		entry->lifeTime = 8000;
		entry->mode = 6;
		entry->subMode = 0;
		entry->flags |= 0x400;
		break;
	case 7:
		entry->status |= 0x4000;
	case 5:
		entry->lifeTime = 5000;
		entry->mode = 6;
		entry->subMode = 0;
		break;
	case 6:
		entry->lifeTime = 5000;
		entry->status |= 0x40;
		entry->mode = 6;
		entry->subMode = 0;
		break;
	case 2:
		entry->mode = 5;
		entry->subMode = 0;
		break;
	case 13:
		entry->mode = 3;
		entry->subMode = 0;
		break;
	case 14:
		entry->mode = 4;
		entry->subMode = 0;
		break;
	case 15:
		entry->mode = 10;
		entry->subMode = 0;
		break;
	}
	return entry;
}

void itemDelete(char* name) {
    ItemWork* wp = itemGetWork();
    ItemEntry* entry = wp->entries;
    int i;

    for (i = 0; i < wp->count; i++, entry++) {
        if (!strcmp(entry->name, name)) {
            itemCheckBadge(entry);
            switch (entry->itemId) {
                case ITEM_DIAMOND_STAR:
                case ITEM_EMERALD_STAR:
                case ITEM_GOLD_STAR:
                case ITEM_RUBY_STAR:
                case ITEM_SAPPHIRE_STAR:
                case ITEM_GARNET_STAR:
                case ITEM_CRYSTAL_STAR:
                    effSoftDelete(entry->effect);
                    break;
                default:
                    iconDelete(entry->name);
                    break;
            }
            entry->flags &= ~1;
        }
    }
}

ItemEntry* itemNameToPtr(const char* name) {
    ItemWork* wp = itemGetWork();
    ItemEntry* entry = wp->entries;
    int i;

    for (i = 0; i < wp->count; i++, entry++) {
        if (!strcmp(entry->name, name)) {
            return entry;
        }
    }
    return NULL;
}

ItemEntry* itemHitCheck(f32 x, f32 y, f32 z, f32 maxDist) {
    ItemWork* wp = itemGetWork();
    MarioWork* mario;
    ItemEntry* entry;
    int i;
    ItemEntry* result;
    HitEntry* hit;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 horizDist;
    f32 totalDist;
    f32 hitX;
    f32 hitY;
    f32 hitZ;
    f32 distance;
    f32 hitNX;
    f32 hitNY;
    f32 hitNZ;

    mario = marioGetPtr();
    entry = wp->entries;
    result = NULL;
    for (i = 0; i < wp->count; i++, entry++) {
        if (!(entry->flags & 1)) {
            continue;
        }
        if (entry->flags & 2) {
            continue;
        }
        if (itemStatus(entry, 0x1000)) {
            continue;
        }
        if (entry->mode == 2) {
            continue;
        }

        dx = x - entry->position.x;
        dy = y - entry->position.y;
        dz = z - entry->position.z;
        horizDist = sqrtf(dx * dx + dz * dz);
        totalDist = sqrtf(dx * dx + dy * dy + dz * dz);
        if (horizDist < maxDist
            && marioItemGetOk()
            && marioGetJabaraState() != 1
            && marioGetJabaraState() != 4
            && marioGetJabaraState() != 5
            && __fabsf((0.5f * mario->wPlayerCollisionBox.y + y) - entry->position.y)
                < (0.5f * mario->wPlayerCollisionBox.y + 10.0f)) {
            hit = NULL;
            if (y < entry->position.y) {
                distance = 10.0f + (entry->position.y - y);
                if (distance > 0.0f) {
                    hit = hitCheckAttr(0x80000000, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                                       x, y, z, 0.0f, 1.0f, 0.0f);
                }
            }
            if (hit == NULL) {
                result = entry;
                maxDist = totalDist;
            }
        }
    }
    return result;
}

ItemEntry* itemNearDistCheck(f32 x, f32 y, f32 z, f32 maxDist) {
    ItemEntry* entry;
    ItemEntry* result;
    ItemWork* wp;
    Vec itemPos;
    Vec delta;
    Vec pos;
    f32 best = maxDist;
    f32 mag;
    int i;

    marioGetPtr();
    wp = itemGetWork();

    pos = (Vec){x, y, z};
    result = NULL;

    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        if ((entry->flags & 1) && !itemStatus(entry, 0x1000) && !itemStatus(entry, 0x100000)
            && entry->mode != 2 && !itemStatus(entry, 0x2000000)) {
            itemPos = (Vec){entry->position.x, entry->position.y, entry->position.z};

            PSVECSubtract(&itemPos, &pos, &delta);
            mag = PSVECMag(&delta);
            if (mag < best) {
                result = entry;
                best = mag;
            }
        }
    }
    return result;
}

void winNameDisp(WinMgrEntry* entry) {
    ItemEntry* item = entry->param;
    char buf[0x400];
    const char* msg;
    u16 width;

    msg = msgSearch(itemDataTable[item->itemId].name);
    if (item->itemId >= BADGE_MIN && item->itemId < BADGE_MAX) {
        sprintf(buf, msgSearch("msg_window_badge_get"), msg);
    }
    else {
        sprintf(buf, msgSearch("msg_window_item_get"), msg);
    }

    width = FontGetMessageWidth(buf);
    if (width + 0x14 > entry->width) {
        entry->width = width + 0x14;
        entry->x = -entry->width / 2;
    }

    if (!winMgrAction(item->winMainId)) {
        FontDrawStart();
        FontDrawColor((GXColor){0x00, 0x00, 0x00, 0xFF});
        FontDrawMessage(-width / 2, entry->y - 8, buf);
    }
}

void winNameDisp2(WinMgrEntry* entry) {
    ItemEntry* item = entry->param;
    char buf[0x400];
    const char* msg;
    const char* name;
    u16 width;

    name = msgSearch(itemDataTable[item->itemId].name);
    msg = msgSearch("msg_window_item_release");
    sprintf(buf, msg, name);

    width = FontGetMessageWidth(buf);
    if (width + 0x14 > entry->width) {
        entry->width = width + 0x14;
        entry->x = -entry->width / 2;
    }

    if (!winMgrAction(item->winMainId)) {
        FontDrawStart();
        FontDrawColor((GXColor){0x00, 0x00, 0x00, 0xFF});
        FontDrawMessage(-width / 2, entry->y - 8, buf);
    }
}

void winHelpMain(WinMgrEntry* entry) {
    winMgrHelpMain(entry);
}

void winHelpDisp(WinMgrEntry* entry) {
    ItemEntry* item = entry->param;

    if (!winMgrAction(item->winHelpId)) {
        entry->message = msgSearch(itemDataTable[item->itemId].description);
        winMgrHelpDraw(entry);
    }
}

void winFullDisp(WinMgrEntry* entry) {
    ItemEntry* item = entry->param;
    const char* msg;
    u16 width;

    msg = (item->itemId >= BADGE_MIN && item->itemId < BADGE_MAX) ? msgSearch("msg_window_badge_full") : msgSearch("msg_window_item_full");

    width = FontGetMessageWidth(msg);
    if (width + 0x14 > entry->width) {
        entry->width = width + 0x14;
        entry->x = -entry->width / 2;
    }

    if (!winMgrAction(item->winFullId)) {
        FontDrawStart();
        FontDrawEdge();
        FontDrawColor((GXColor){0xFF, 0xFF, 0xFF, 0xFF});
        FontDrawMessage(-width / 2, entry->y - 0xa, msg);
        FontDrawEdgeOff();
    }
}


// Removes every live entry in the current item work whose name matches `item`'s,
// reporting badges that arrive and clearing each entry's active flag.
static inline void itemseq_deleteMatching(ItemEntry* item) {
    ItemWork* wp = itemGetWork();
    ItemEntry* entry = wp->entries;
    s32 i;

    for (i = 0; i < wp->count; i++, entry++) {
        if (strcmp(entry->name, item->name) == 0) {
            if (!gp->inBattle) {
                if (entry->itemId >= BADGE_MIN && entry->itemId < BADGE_MAX && (entry->flags & 0x100)) {
                    pouchArriveBadge(entry->itemId);
                }
            }
            switch (entry->itemId) {
            case ITEM_DIAMOND_STAR: case ITEM_EMERALD_STAR: case ITEM_GOLD_STAR: case ITEM_RUBY_STAR: case ITEM_SAPPHIRE_STAR: case ITEM_GARNET_STAR: case ITEM_CRYSTAL_STAR:
                effSoftDelete(entry->effect);
                break;
            default:
                iconDelete(entry->name);
                break;
            }
            entry->flags &= ~1;
        }
    }
}


// Puts the item into the pouch (clearing its switch if it has one), spawning its
// first-hit event at the item position. Returns 0 when the pouch is already full.
static inline s32 itemseq_tryGet(ItemEntry* item) {
    if (!pouchGetItem(item->itemId)) {
        return 0;
    }
    if (item->flags & 0x20) {
        swSet(item->evtId);
    } else if (item->flags & 0x40) {
        _swSet(item->evtId);
    }
    if (item->getScript != NULL) {
        EventEntry* evt = evtEntry(item->getScript, 0, 0x20);

        evt->lwData[0] = (s32)(1024.0f * item->position.x) - 0x0DB58580;
        evt->lwData[1] = (s32)(1024.0f * item->position.y) - 0x0DB58580;
        evt->lwData[2] = (s32)(1024.0f * item->position.z) - 0x0DB58580;
    }
    item->status |= 1;
    return 1;
}

// All logic, switch dispatch, constants, call order and the whole .rodata layout match.
void itemseq_GetItem(ItemEntry* item) {
    ItemWork* wp = itemGetWork();
    MarioWork* mario = marioGetPtr();
    s32 effType;
    s32 bgmType;
    s32 got;
    s32 id;
    s32 sel;
    u32 motion;

    switch (item->subMode) {
    case 0:
        item->status &= ~0x10000;
        if (item->flags & 0x100) {
            item->status |= 0x4000000;
        } else {
            item->status &= ~0x4000000;
        }
        item->flags &= ~0x100;
        if (item->flags & 4) {
            got = itemseq_tryGet(item);
            if (got != 0) {
                item->lifeTime = 0;
                itemseq_sfxGet(item);
                itemseq_deleteMatching(item);
            } else {
                item->mode = 4;
                item->subMode = 0;
            }
            break;
        }
        item->subMode = 1;
        item->lifeTime = 0;
        camGetPtr(CAMERA_2D)->flags |= 0x200;
        marioKeyOff();
        marioStSystemLevel(2);
        psndSetFlag(0x80);
        mario->riderDelta.x = 0.0f;
        mario->riderDelta.z = 0.0f;
        mario->baseSpeed = 0.0f;
        mario->field_0x18C = 0.0f;
        id = item->itemId;
        // goto: as one || condition (or a switch) MWCC merges these id tests into range checks
        if (id == ITEM_SUPER_BOOTS) {
            goto itemseq_setFlag;
        }
        if (id == ITEM_ULTRA_BOOTS) {
            goto itemseq_setFlag;
        }
        if (id == ITEM_SUPER_HAMMER) {
            goto itemseq_setFlag;
        }
        if (id != ITEM_ULTRA_HAMMER) {
            goto state1;
        }
    itemseq_setFlag:
        item->status |= 0x2000;
        // fallthrough
    case 1:
    state1:
        if (!(((item->flags & 0x1000) && !(item->flags & 0x4000)) || marioChkItemGetMotion())) {
            motion = mario->currMotionId;
            if (motion == MARIO_MOTION_JUMP_SW || (u16)(motion - MARIO_MOTION_JUMP_STAND) <= 2 || motion == MARIO_MOTION_GET_ITEM || motion == MARIO_MOTION_TALK) {
                item->subMode = 8;
            }
            break;
        }
        item->subMode = 2;
        marioChgGetItemMotion();
        effType = 7;
        bgmType = -1;
        {
            f32 px = mario->position.x;
            f32 py = 50.0f + mario->position.y;
            f32 pz = mario->position.z;

            item->position.x = px;
            item->position.y = py;
            item->position.z = pz;
        }
        id = item->itemId;
        if (id == ITEM_SHINE_SPRITE) {
            effType = 4;
            bgmType = -1;
        } else if (id == ITEM_STAR_PIECE) {
            effType = 8;
            bgmType = 0;
        } else if (id < ITEM_CASTLE_KEY_0x0C) {
            effType = 3;
            bgmType = 3;
        } else if (id < ITEM_DIAMOND_STAR) {
            effType = 6;
            bgmType = 1;
        } else if (id < ITEM_COIN) {
            effType = 0;
            bgmType = -1;
        } else if (id < ITEM_MAX) {
            effType = 7;
            bgmType = -1;
        } else if (id < BADGE_MAX) {
            effType = 5;
            bgmType = 2;
        }
        wp->getEffect = effItemGetEntry(effType, mario->position.x,
                                                          62.0f + mario->position.y, mario->position.z);
        if (item->status & 0x2000) {
            EffectEntry* eff = wp->getEffect;
            effItemGetData* data = eff->userdata;

            data->camId = 5;
            iconFlagOn(item->name, 0x200);
            marioSetCamId(CAMERA_3D_EFFECTB);
        } else {
            EffectEntry* eff = wp->getEffect;
            effItemGetData* data = eff->userdata;

            data->camId = 5;
            iconFlagOn(item->name, 0x100);
            if (effType == 0 || effType == 3) {
                marioSetCamId(CAMERA_3D_EFFECTA);
            }
        }
        if (!(item->flags & 0x8000)) {
            {
                u32 silent = psndBGMChkSilent(0);

                if (silent != 0) {
                    item->flags |= 8;
                }
            }
            switch (bgmType) {
            case 0:
                if (!(item->flags & 8)) {
                    psndBGMOff(0x3800);
                }
                psndBGMOn(0x211, "BGM_FF_GET_STARPIECE1");
                break;
            case 1:
                if (!(item->flags & 8)) {
                    psndBGMOff(0x3800);
                }
                psndBGMOn(0x211, "BGM_FF_GET_KEYITEM1");
                break;
            case 2:
                if (!(item->flags & 8)) {
                    psndBGMOff(0x3800);
                }
                psndBGMOn(0x211, "BGM_FF_GET_BADGE1");
                break;
            case 3:
                if (!(item->flags & 8)) {
                    psndBGMOff(0x3800);
                }
                psndBGMOn(0x211, "BGM_FF_GET_IMPORTANT_ITEM1");
                break;
            default:
                if (!(item->flags & 8)) {
                    psndBGMOff(0x3800);
                }
                psndBGMOn(0x211, "BGM_FF_GET_ITEM1");
                break;
            }
        }
        if (effType == 8) {
            evtSetValue(NULL, 0xF5DE01A2, evtGetValue(NULL, 0xF5DE01A2) + 1);
        }
        item->flags &= ~0x2000;
        itemseq_sfxGet(item);
        item->winMainId = winMgrEntry(&itemdrv_window_desc[0]);
        winMgrOpen(item->winMainId);
        winMgrSetParam(item->winMainId, item);
        if (!(item->status & 0x2000)) {
            item->winHelpId = winMgrEntry(&itemdrv_window_desc[1]);
            winMgrOpen(item->winHelpId);
            winMgrSetParam(item->winHelpId, item);
        }
        got = itemseq_tryGet(item);
        if (got == 0) {
            item->winFullId = winMgrEntry(&itemdrv_window_desc[2]);
            winMgrOpen(item->winFullId);
            winMgrSetParam(item->winFullId, item);
        }
        if (item->itemId == ITEM_WEDDING_RING) {
            iconChange(item->name, 0x114);
        }
        if (item->effect == NULL) {
            iconNameToPtr(item->name)->flags |= 0x80;
        }
        break;

    case 2:
        if (!(item->status & 0x2000)) {
            marioCtrlOff();
            item->status |= 0x1000000;
        }
        item->subMode = 3;
        /* fallthrough */
    case 3:
        if (!(item->flags & 0x8000) && !(item->flags & 0x2000)) {
            if (psndBGMChk(1)) {
                item->flags |= 0x2000;
                psndBGMOff(0x201);
                if (!(item->flags & 8)) {
                    psndBGMOn(0xA0, NULL);
                }
            }
        }
        if (winMgrAction(item->winMainId)) {
            break;
        }
        if (!(keyGetButtonTrg(0) & 0x300)) {
            break;
        }
        winMgrCloseAutoDelete(item->winMainId);
        if (!(item->status & 0x2000)) {
            winMgrCloseAutoDelete(item->winHelpId);
        }
        if (!(item->flags & 0x8000) && !(item->flags & 0x2000)) {
            psndBGMOff(0x201);
            if (!(item->flags & 8)) {
                psndBGMOn(0xA0, NULL);
            }
        }
        if (item->status & 0x2000) {
            item->subMode = 6;
            break;
        }
        if (item->itemId >= ITEM_MIN && item->itemId < ITEM_MAX && item->itemId != ITEM_STAR_PIECE && !swGet(0xEB) && !(gp->flags & 0x1000)) {
            swSet(0xEB);
            item->subMode = 4;
            wp->getMsgId = msgWindow_Entry(msgSearch("item_first_hit"), 0, 0);
            break;
        }
        effSoftDelete(wp->getEffect);
        marioSetCamId(CAMERA_3D);
        iconFlagOff(item->name, 0x100);
        if (item->itemId == ITEM_WEDDING_RING) {
            iconChange(item->name, 0x115);
        }
        if (item->effect == NULL) {
            iconNameToPtr(item->name)->flags &= ~0x80;
        }
        if (item->status & 1) {
            marioStSystemLevel(0);
            psndClearFlag(0x80);
            if (item->status & 0x1000000) {
                item->status &= ~0x1000000;
                marioCtrlOn();
            }
            marioKeyOn();
            marioChgStayMotion();
            camGetPtr(CAMERA_2D)->flags &= ~0x200;
            itemseq_deleteMatching(item);
            break;
        }
        item->subMode = 5;
        winMgrCloseAutoDelete(item->winFullId);
        marioChgStayMotion();
        iconFlagOn(item->name, 2);
        id = item->itemId;
        if (id >= BADGE_MIN && id < BADGE_MAX) {
            wp->getSelect = winMgrSelectEntry(2, id, 1);
        } else {
            wp->getSelect = winMgrSelectEntry(1, id, 1);
        }
        break;

    case 4:
        if (windowCheckID(wp->getMsgId)) {
            break;
        }
        effSoftDelete(wp->getEffect);
        marioSetCamId(CAMERA_3D);
        iconFlagOff(item->name, 0x100);
        marioStSystemLevel(0);
        psndClearFlag(0x80);
        if (item->status & 0x1000000) {
            item->status &= ~0x1000000;
            marioCtrlOn();
        }
        marioKeyOn();
        marioChgStayMotion();
        camGetPtr(CAMERA_2D)->flags &= ~0x200;
        itemseq_deleteMatching(item);
        break;

    case 5:
        sel = winMgrSelect(wp->getSelect);
        if (sel == 0) {
            break;
        }
        winMgrSelectDelete(wp->getSelect);
        if (sel < 0) {
            if (item->status & 0x4000000) {
                item->flags |= 0x100;
            }
        } else {
            pouchRemoveItem(sel);
            itemseq_tryGet(item);
            item->status &= ~1;
            item->itemId = sel;
            item->getScript = NULL;
            item->evtId = 0;
            item->flags &= ~0x60;
            iconChange(item->name, itemDataTable[item->itemId].icon_id);
            wp->currId++;
            {
                IconEntry* icon = iconNameToPtr(item->name);

                sprintf(icon->name, "_i%05x", wp->currId);
                sprintf(item->name, "_i%05x", wp->currId);
            }
            item->flags |= 0x100;
        }
        item->flags &= ~0x1000;
        item->moveSpeed = 85.0f;
        item->jumpPower = 120.0f;
        item->boundRate = 0.7f;
        item->gravity = 1.0f;
        item->winMainId = winMgrEntry(&itemdrv_window_desc[3]);
        winMgrOpen(item->winMainId);
        winMgrSetParam(item->winMainId, item);
        marioChgGetItemMotion();
        iconFlagOff(item->name, 2);
        if (mario->flags & 0x2000000) {
            item->status |= 0x8000;
        }
        item->subMode = 9;
        break;

    case 6:
        wp->getEvent = evtEntryType(all_juyoitem_lecture, 0, 0, 0);
        wp->getEvent->lwData[8] = (s32)item;
        wp->getEvent->lwData[9] = (s32)wp->getEffect;
        psndClearFlag(0x80);
        item->subMode = 7;
        break;

    case 7:
        {
            EventEntry* evt = wp->getEvent;

            if (evtCheckID(evt->eventId)) {
                break;
            }
        }
        marioStSystemLevel(0);
        psndClearFlag(0x80);
        if (item->status & 0x1000000) {
            item->status &= ~0x1000000;
            marioCtrlOn();
        }
        marioKeyOn();
        marioChgStayMotion();
        camGetPtr(CAMERA_2D)->flags &= ~0x200;
        itemseq_deleteMatching(item);
        break;

    case 9:
        if (!(keyGetButtonTrg(0) & 0x100)) {
            break;
        }
        winMgrCloseAutoDelete(item->winMainId);
        item->status |= 8;
        item->mode = 6;
        item->subMode = 0;
        item->flags &= ~0x10;
        id = item->itemId;
        if (id >= KEY_ITEM_MIN && id < KEY_ITEM_MAX) {
            item->lifeTime = 0;
        } else if (id >= BADGE_MIN && id < BADGE_MAX) {
            item->lifeTime = 0;
        } else {
            item->lifeTime = 0x4E20;
        }
        item->status |= 0x10000;
        item->status &= ~0x1000;
        item->flags &= ~0x2;
        marioStSystemLevel(0);
        psndClearFlag(0x80);
        if (item->status & 0x1000000) {
            item->status &= ~0x1000000;
            marioCtrlOn();
        }
        marioKeyOn();
        marioChgStayMotion();
        camGetPtr(CAMERA_2D)->flags &= ~0x200;
        break;

    case 8:
        if (mario->flags & 0x2000000) {
            item->status |= 0x8000;
        }
        item->lifeTime = 0;
        if (item->status & 0x1000) {
            item->mode = 1;
            item->subMode = 0;
            item->status &= ~0x4;
            item->status &= ~0x8;
            item->status &= ~0x4000;
            item->status &= ~0x1000;
            item->flags &= ~0x1000;
        } else {
            item->moveSpeed = 85.0f;
            item->jumpPower = 120.0f;
            item->boundRate = 0.7f;
            item->gravity = 1.0f;
            item->status |= 8;
            item->mode = 6;
            item->subMode = 0;
            item->flags &= ~0x10;
            item->status &= ~0x1000;
            item->flags &= ~0x2;
            item->flags &= ~0x1000;
        }
        marioStSystemLevel(0);
        psndClearFlag(0x80);
        if (item->status & 0x1000000) {
            item->status &= ~0x1000000;
            marioCtrlOn();
        }
        marioKeyOn();
        break;

    default:
        break;
    }
}

static s32 _move_dir;

// Falling/bouncing motion of a dropped item: gravity from the elapsed time, the side/floor/ceiling hit checks, the
// bounce sounds and the water splash. The bounce float polynomial is split into locals so MWCC does not contract it
// into `fmsubs`/`fnmadds`; the itemId range test is written as explicit switch cases (0x72..0x78) so the branch
// layout matches; hardHit is zeroed after the switch; the two identical `item->bounceTime += d1` tails and the two
// `bounce = 0` tails are reached through a label/goto so MWCC keeps one shared out-of-line copy, as the target does.
// The ceiling/floor rays, the bounce SFX and the badge-delete loop (itemDelete) are out-of-line functions,
// auto-inlined here; the rays return from each branch, which keeps the hit pointer out of the floor ray's saved
// register until the hit is known. Their literals pool at their own .text slots, which is where retail's
// .rodata/.sdata2 has them.
void itemseq_Bound(ItemEntry* item) {
    MarioWork* mario = marioGetPtr();
    CameraEntry* camera = camGetPtr(CAMERA_3D);

    switch (item->subMode) {
    case 0:
        item->subMode = 1;
        item->bounceTime = 0;
        item->lastTime = gp->renderTime;
        item->curTime = gp->renderTime;
        item->boundCount = 0;
        if (!(item->status & 0x10)) {
            switch (item->mode) {
            case 6:
                item->status |= 4;
            case 5:
                if (!(item->flags & 0x10)) {
                    item->boundRate = 1.0f;
                    item->gravity = 1.0f;
                    item->boundLimit = 0;
                    item->jumpPower = 500.0f;
                }
                if (!(item->status & 0x40)) {
                    s32 id = item->itemId;

                    switch (id) {
                    case ITEM_COIN:
                    case ITEM_HEART:
                    case ITEM_FLOWER:
                        item->moveSpeed = (f32)(irand(0x1E) + 0x2D);
                        if (irand(0x64) < 0x32) {
                            item->moveDir = (s32)((90.0f + camera->viewYaw + (f32)irand(0x3C)) - 30.0f);
                        } else {
                            item->moveDir = (s32)(((camera->viewYaw - 90.0f) + (f32)irand(0x3C)) - 30.0f);
                        }
                        break;
                    default:
                        item->moveSpeed = 100.0f;
                        if (irand(0x64) < 0x32) {
                            item->moveDir = (s32)((90.0f + camera->viewYaw + (f32)irand(0x1E)) - 15.0f);
                        } else {
                            item->moveDir = (s32)(((camera->viewYaw - 90.0f) + (f32)irand(0x1E)) - 15.0f);
                        }
                        break;
                    }
                } else {
                    item->moveDir = _move_dir;
                    _move_dir += 0x84;
                    if (_move_dir >= 0x168) {
                        _move_dir -= 0x168;
                    }
                    item->moveSpeed = 0.67f * (f32)(irand(0x1E) + 0x41);
                }
                break;
            case 4:
                item->status |= 8;
            case 3:
                if (!(item->flags & 0x10)) {
                    item->boundRate = 1.0f;
                    item->gravity = 1.0f;
                    item->boundLimit = 0;
                }
                item->moveSpeed = 0.0f;
                item->moveDir = 0;
                item->jumpPower = 0.0f;
                break;
            case 10:
                item->boundRate = 1.0f;
                item->gravity = 1.0f;
                item->boundLimit = 0;
                item->jumpPower = 500.0f;
                item->moveSpeed = 0.0f;
                item->moveDir = 0;
                item->status |= 0x20;
                item->status |= 4;
                break;
            }
        }
        break;
    case 1: {
        s64 d1;
        f32 y2;
        f32 t1;
        f32 t2;
        f32 t3;
        f32 vx;
        f32 vz;
        f32 vy;
        f32 nx;
        f32 nz;
        s32 angle;
        HitEntry* hit;
        Vec p1;

        item->curTime = gp->renderTime;
        d1 = item->curTime - item->lastTime;
        t1 = (f32)OSTicksToMicroseconds(d1) / 1000000.0f;
        t2 = (f32)OSTicksToMicroseconds(item->bounceTime) / 1000000.0f;
        t3 = (f32)OSTicksToMicroseconds(item->bounceTime + d1) / 1000000.0f;
        vx = t1 * (item->moveSpeed * (f32)sin((6.2831855f * (f32)item->moveDir) / 360.0f));
        {
            f32 c = (f32)cos((6.2831855f * (f32)item->moveDir) / 360.0f);

            vz = t1 * (item->moveSpeed * -c);
        }
        if (item->status & 0x8000) {
            vz = 0.0f;
        }
        y2 = t3 * t3;
        vy = item->jumpPower * t1 + (-980.0f * item->gravity * -((t2 * t2) - y2));
        if ((item->status & 4) && (item->boundCount != 0 || vy < 0.0f)) {
            item->status &= ~4;
        }
        if ((item->status & 8) && item->boundCount != 0) {
            item->status &= ~8;
        }
        if ((item->status & 0x20) && vy < 0.0f) {
            effStardustEntry(1, 3, 0x1E, item->position.x, item->position.y, item->position.z, 16.0f, 16.0f);
            itemDelete(item->name);
            return;
        }
        angle = item->moveDir;
        hit = itemHitCheckSide(item, &nx, &nz, &angle, vx, vz);
        item->position.x = nx;
        item->position.z = nz;
        item->moveDir = angle;
        if (hit != NULL) {
            f32 k = 0.25f;
            item->moveSpeed = item->moveSpeed * k;
        }
        if (!(item->status & 0x20) && !(item->flags & 0x800)) {
            hit = itemBoundHitCeiling(item, vy);
            if (hit != NULL) {
                vy = 0.0f;
                {
                    f32 k = 0.25f;

                    item->jumpPower = -item->jumpPower * k;
                }
                item->bounceTime = 0;
                item->lastTime = item->curTime;
            }
        }
        if ((item->status & 0x100000) && item->position.y <= -1000.0f) {
            s32 id;

            item->mode = 7;
            item->subMode = 0;
            item->status &= ~4;
            item->status &= ~8;
            item->status &= ~0x4000;
            id = item->itemId;
            if (id >= KEY_ITEM_MIN && id < KEY_ITEM_MAX) {
                item->lifeTime = 0;
            } else if (id >= BADGE_MIN && id < BADGE_MAX) {
                item->lifeTime = 0;
            } else {
                item->lifeTime = 20000;
            }
            item->status |= 0x10000;
            item->status &= ~0x1000;
            item->flags |= 0x100;
        } else {
            hit = itemBoundHitFloor(item, vy);
            if (hit != NULL) {
                if (hitGetAttr(hit) & 0x600) {
                    Vec pos;

                    item->status |= 0x100000;
                    pos = (Vec){0.0f, 0.0f, 0.0f};
                    pos.x = item->position.x;
                    pos.y = item->position.y;
                    pos.z = item->position.z;
                    psndSFXOn_3D((char*)0x194, &pos);
                    effSplashEntry(0, item->position.x, item->position.y, item->position.z, 0.7f);
                    item->flags |= 2;
                    goto bound_add_time;
                } else {
                    s32 cnt = item->boundCount;

                    item->boundCount = cnt + 1;
                    if (!(hitGetAttr(hit) & 0x80000000) && item->boundLimit != 0 && item->boundCount >= item->boundLimit) {
                        item->mode = 1;
                        item->subMode = 0;
                        item->status &= ~4;
                        item->status &= ~8;
                        item->status &= ~0x4000;
                    } else {
                        int hardHit;
                        int bounce;
                        f32 tt, dd;

                        itemseq_sfxBound(item, &p1);
                        hardHit = 0;
                        if ((item->flags & 0x80) && item->jumpPower != 0.0f) {
                            bounce = 1;
                        } else if ((hitGetAttr(hit) & 0x80000000) && item->jumpPower != 0.0f) {
                            MapObjectEntry* obj = mobjHitObjPtrToPtr(hit);

                            if (mario->characterId == 2) {
                                bounce = 0;
                            } else if (obj != NULL && strcmp(obj->animName, "MOBJ_KururinFloor") != 0) {
                                bounce = 1;
                                hardHit = 1;
                            } else {
                                goto bounce_zero;
                            }
                        } else {
                        bounce_zero:
                            bounce = 0;
                        }
                        if (bounce == 0) {
                            f32 tt2;
                            f32 gg;
                            f32 ee;
                            f32 ff;

                            tt = t3 + t1;
                            tt2 = tt * tt;
                            gg = -980.0f * item->gravity;
                            ee = gg * (tt2 - y2);
                            ff = item->jumpPower * t1 + ee;
                            item->jumpPower = 0.7f * (item->boundRate * (-ff / t1));
                        }
                        {
                            f64 a = __fabs(0.5f * (item->jumpPower / (-980.0f * item->gravity)));

                            dd = (f32)a;
                        }
                        {
                            f32 hitval = item->jumpPower * dd + dd * ((-980.0f * item->gravity) * dd);

                            if (hardHit == 0) {
                                if (hitval <= 8.0f) {
                                    item->mode = 1;
                                    item->subMode = 0;
                                    item->status &= ~4;
                                    item->status &= ~8;
                                    item->status &= ~0x4000;
                                }
                            }
                        }
                        if ((item->flags & 0x200) && item->boundCount <= 1) {
                            item->moveDir = item->boundNextDir;
                            item->moveSpeed = item->boundNextSpeed;
                            item->flags &= ~0x200;
                        }
                        item->bounceTime = 0;
                        item->lastTime = gp->renderTime;
                    }
                }
            } else {
            bound_add_time:
                item->bounceTime += d1;
                item->lastTime = item->curTime;
            }
        }
        break;
    }
    }
    item->status &= ~0x10;
}

// Plays the bounce SFX for the item id at the item's position, which it also leaves in *p.
void itemseq_sfxBound(ItemEntry* item, Vec* p) {
    Vec p0;
    s32 id;

    p0 = (Vec){item->position.x, item->position.y, item->position.z};
    *p = p0;
    id = item->itemId;
    switch (id) {
    case ITEM_COIN:
        psndSFXOn_3D("SFX_COIN_LEAPED1", p);
        break;
    case ITEM_PIANTA_TOKEN:
        psndSFXOn_3D("SFX_EVT_GAME_MONTE_LEAPED1", p);
        break;
    case ITEM_HEART:
        psndSFXOn_3D("SFX_HEART_LEAPED1", p);
        break;
    case ITEM_FLOWER:
        psndSFXOn_3D("SFX_FLOWER_LEAPED1", p);
        break;
    case ITEM_STAR_PIECE:
        psndSFXOn_3D("SFX_STAR_PIECE_LEAPED1", p);
        break;
    case ITEM_DIAMOND_STAR:
    case ITEM_EMERALD_STAR:
    case ITEM_GOLD_STAR:
    case ITEM_RUBY_STAR:
    case ITEM_SAPPHIRE_STAR:
    case ITEM_GARNET_STAR:
    case ITEM_CRYSTAL_STAR:
        psndSFXOn_3D("SFX_EVT_STARSTONE_JUMP1", p);
        break;
    default:
        if (id >= BADGE_MIN && id < BADGE_MAX) {
            psndSFXOn_3D("SFX_BADGE_LEAPED1", p);
        } else {
            psndSFXOn_3D("SFX_ITEM_LEAPED1", p);
        }
        break;
    }
}

// Plays the "item obtained" stardust/SFX for the item id, at the item's position.
void itemseq_sfxGet(ItemEntry* item) {
    Vec pos2;
    Vec pos = {0.0f, 0.0f, 0.0f};

    pos.x = item->position.x;
    pos.y = item->position.y;
    pos.z = item->position.z;

    pos2 = pos;
    switch (item->itemId) {
    case ITEM_COIN:
        effStardustEntry(1, 3, 0x1E, pos.x, 35.0f + pos.y, pos.z, 16.0f, 16.0f);
        psndSFXOn_3D("SFX_COIN_GET1", &pos2);
        break;
    case ITEM_PIANTA_TOKEN:
        effStardustEntry(1, 3, 0x1E, pos.x, 35.0f + pos.y, pos.z, 16.0f, 16.0f);
        psndSFXOn_3D("SFX_EVT_GAME_MONTE_COUNT1", &pos2);
        break;
    case ITEM_HEART:
        psndSFXOn_3D("SFX_HEART_GET1", &pos2);
        break;
    case ITEM_FLOWER:
        psndSFXOn_3D("SFX_FLOWER_GET1", &pos2);
        break;
    case ITEM_STAR_PIECE:
        break;
    }
}

void itemSetPosition(ItemEntry* item, f32 x, f32 y, f32 z) {
    item->position.x = x;
    item->position.y = y;
    item->position.z = z;
}

// 100% per objdiff. The zero tests and the reflection's 0.5 are written as literals (0.0f/0.5f)
// rather than the extern float_0/float_0p5 names: MWCC then pools them into local @ symbols,
// but only a known-constant value makes it colour the scratch FPRs the way the original does
// (the externs give pos.x/zero/pos.z f0/f1/f0 and rotate the reflect block f5/f6/f4). objdiff
// scores the relocation-to-@-symbol difference as a match; semdiff treats the two as equal.
HitEntry* itemHitCheckSide(ItemEntry* item, f32* outX, f32* outZ, s32* angle, f32 dirX, f32 dirZ) {
    f32 hitX;
    f32 hitY;
    f32 hitZ;
    f32 distance;
    f32 hitNX;
    f32 hitNY;
    f32 hitNZ;
    f32 sin;
    f32 cos;
    HitEntry* hit;
    f32 dot;
    f32 nx;
    f32 nz;
    f32 height = 15.0f;

    *outX = item->position.x;
    *outZ = item->position.z;

    if (dirX == 0.0f && dirZ == 0.0f) {
        return NULL;
    }

    sincosf(&sin, &cos, angleABf(0.0f, 0.0f, dirX, dirZ));

    distance = 10.0f + sqrtf(dirX * dirX + dirZ * dirZ);
    hit = hitCheckFilter(NULL, item->position.x, item->position.y + height, item->position.z, sin, 0.0f, cos, &hitX,
                         &hitY, &hitZ, &distance,
                         &hitNX, &hitNY, &hitNZ);
    if (hit != NULL) {
        f32 half = 0.5f;
        dot = dirX * hitNX + dirZ * hitNZ;
        nx = (dirX - dot * hitNX) * half;
        nz = (dirZ - dot * hitNZ) * half;
        *outX = *outX + nx;
        *outZ = *outZ + nz;
        dirZ = reviseAngle(180.0f + (f32)*angle);
        dirX = reviseAngle(angleABf(0.0f, 0.0f, 100.0f * hitNX,
                                    100.0f * hitNZ));
        *angle = (s32)reviseAngle(dirX + reviseAngle(dirX - dirZ));
        return hit;
    }
    else {
        distance = 10.0f + sqrtf(dirX * dirX + dirZ * dirZ);
        hit = hitCheckFilter(NULL, item->position.x, item->position.y + height, item->position.z, sin, 0.0f, cos, &hitX,
                             &hitY, &hitZ, &distance,
                             &hitNX, &hitNY, &hitNZ);
        if (hit != NULL) {
            f32 half = 0.5f;
            dot = dirX * hitNX + dirZ * hitNZ;
            nx = (dirX - dot * hitNX) * half;
            nz = (dirZ - dot * hitNZ) * half;
            *outX = *outX + nx;
            *outZ = *outZ + nz;
            dirZ = reviseAngle(180.0f + (f32)*angle);
            dirX = reviseAngle(angleABf(0.0f, 0.0f, 100.0f * hitNX,
                                        100.0f * hitNZ));
            *angle = (s32)reviseAngle(dirX + reviseAngle(dirX - dirZ));
            return hit;
        }
        else {
            *outX = *outX + dirX;
            *outZ = *outZ + dirZ;
            return NULL;
        }
    }
}

// Floor ray of a falling item: lands it on the hit's y, or moves it down by vy when nothing is hit.
HitEntry* itemBoundHitFloor(ItemEntry* item, f32 vy) {
    f32 hx, hy, hz, dist, hnx, hny, hnz;
    f64 d;
    HitEntry* hit;

    if (vy >= 0.0f) {
        item->position.y += vy;
        return NULL;
    }
#ifdef MUST_MATCH
    if (*(volatile u32*)&item->status & 0x100000) {
#else
    if (item->status & 0x100000) {
#endif
        item->position.y += vy;
        return NULL;
    } else {
        f32 px = item->position.x;
        f32 py = 20.0f + item->position.y;
        f32 pz = item->position.z;

        d = __fabs(vy);
        dist = (f32)(20.0 + d);
        hit = hitCheckFilter(NULL, px, py, pz, 0.0f, -1.0f, 0.0f, &hx, &hy, &hz, &dist, &hnx,
                             &hny, &hnz);
        if (hit != NULL) {
            item->position.y = hy;
            return hit;
        }
        item->position.y += vy;
        return NULL;
    }
}

// Ceiling ray of a rising item: puts it under the hit, 4 units below the ceiling when the raised ray hits.
HitEntry* itemBoundHitCeiling(ItemEntry* item, f32 vy) {
    f32 hx, hy, hz, dist, hnx, hny, hnz;
    f64 d;
    HitEntry* hit;

    if (vy <= 0.0f) {
        return NULL;
    } else {
        f32 px = item->position.x;
        f32 py = 20.0f + item->position.y;
        f32 pz = item->position.z;

        d = __fabs(vy);
        dist = (f32)(20.0 + d);
        hit = hitCheckFilter(NULL, px, py, pz, 0.0f, 1.0f, 0.0f, &hx, &hy, &hz, &dist, &hnx,
                             &hny, &hnz);
        if (hit != NULL) {
            item->position.y = (hy - 20.0f) - 4.0f;
        } else {
            f32 px = item->position.x;
            f32 py = item->position.y;
            f32 pz = item->position.z;

            d = __fabs(vy);
            dist = (f32)(20.0 + d);
            hit = hitCheckFilter(NULL, px, py, pz, 0.0f, 1.0f, 0.0f, &hx, &hy, &hz, &dist, &hnx, &hny,
                                 &hnz);
            if (hit != NULL) {
                item->position.y = hy - 20.0f;
                return hit;
            }
            return NULL;
        }
    }
    return hit;
}

void itemForceGet(ItemEntry* item) {
    item->lifeTime = 0;
    item->mode = 2;
    item->subMode = 0;
    item->flags |= 0x1000;
    item->status &= ~0x10000;
}

void itemNokoForceGet(ItemEntry* item) {
    if (itemGetNokoCheck(item)) {
        item->lifeTime = 0;
        item->mode = 2;
        item->subMode = 0;
        item->flags |= 0x5000;
        item->status &= ~0x10000;
    }
    else {
        item->lifeTime = 0;
        item->mode = 1;
        item->subMode = 0;
        item->status &= ~0x4;
        item->status &= ~0x8;
        item->status &= ~0x4000;
        item->status &= ~0x1000;
    }
}

void itemFlagOn(ItemEntry* item, u16 flag) {
    item->flags |= flag;
}

void itemFlagOff(ItemEntry* item, u16 flag) {
    item->flags &= ~flag;
}

u16 itemFlag(ItemEntry* item, u16 flag) {
    return item->flags & flag;
}

void itemStatusOn(ItemEntry* item, u32 mask) {
    item->status |= mask;
}

u32 itemStatus(ItemEntry* item, u32 mask) {
    return item->status & mask;
}

void itemPickUp(ItemEntry* item) {
    item->status |= 0x1000;
    item->mode = 8;
    item->subMode = 0;
    item->lifeTime = 0;
    item->status &= ~0x100;
}

// Window descriptions for the item windows. Referenced by name in itemseq_GetItem.
WinMgrDesc itemdrv_window_desc[] = {
    { 0, 0, 8, 0xFFFFFF51, 0x78, 0x15E, 0x2A, {0xE1, 0xD7, 0x9B, 0xFF}, NULL, winNameDisp },
    { 0, 0, 8, 0xFFFFFF10, 0xFFFFFF7E, 0x1F4, 0x50, {0xF0, 0xF0, 0xF0, 0xFF}, winHelpMain, winHelpDisp },
    { 0, 0, 8, 0xFFFFFF51, 0xC8, 0x15E, 0x4B, {0x2B, 0xA4, 0xFF, 0xFF}, NULL, winFullDisp },
    { 0, 0, 8, 0xFFFFFF51, 0x78, 0x15E, 0x2A, {0xE1, 0xD7, 0x9B, 0xFF}, NULL, winNameDisp2 },
};

