#define pouchGetCoin pouchGetCoin_decl
#include "mario/mario_pouch.h"
#undef pouchGetCoin
extern int pouchGetCoin(void);

#include "driver/itemdrv.h"
#include "driver/icondrv.h"
#include "driver/swdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "manager/winmgr.h"
#include "manager/fontmgr.h"
#include "driver/msgdrv.h"
#include "driver/seqdrv.h"
#include "party/party_vivian.h"
#include "data/item_data.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "memory.h"
#include <string.h>

extern GlobalWork* gp;
extern int sprintf(char* str, const char* format, ...);

s32 itemGetNokoCheck(ItemEntry* item);
s32 N_fbatPreventMarioEventChk(void);
s32 nokonoko_holdItem(void);

extern ItemData itemDataTable[];
extern const char str_802CADE8[];
extern const char str_802CAE00[];
extern const char str_802CAE18[];
extern const char str_802CAE30[];
extern const char str_802CAE48[];
extern u32 dat_8041af90;
extern u32 dat_8041af94;
extern u32 dat_8041af98;
extern const Vec vec3_802cabec;
extern const Vec vec3_802cabf8;
extern void FontDrawEdge(void);
extern void FontDrawEdgeOff(void);
extern void FontDrawColor(GXColor* color);
extern u16 FontGetMessageWidth(const char* msg);
extern void FontDrawMessage(s32 x, s32 y, const char* msg);

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






ItemEntry* itemEntry(const char* name, ItemType type, s32 a3, s32 evtId, s32 a5, f32 a6, f32 a7, f32 a8) {
	ItemWork* wp = itemGetWork();
	ItemEntry* entry;
	s32 evtVar;
	int i;
	char itemName[64];


	evtVar = evtId;
	if (evtId != -1) {
		if (evtId >= GSWF(0) && evtId <= EVTDAT_GSWF_MAX) {
			evtVar = evtId + EVTDAT_GSWF_BASE;
			if (swGet(evtId + EVTDAT_GSWF_BASE)) {
				return NULL;
			}
		}
		else {
			if (evtId >= LSWF(0) && evtId <= EVTDAT_LSWF_MAX) {
				evtVar = evtId + EVTDAT_LSWF_BASE;
				if (_swGet(evtId + EVTDAT_LSWF_BASE)) {
					return NULL;
				}
			}
		}
	}

	if (name == NULL) {
		if (++wp->currId >= 0xFFFFF) { //fix overflow
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
	if (wp->count > 0) { //TODO: redundant check?
		for (i = 0; i < wp->count; i++, entry++) {
			if (!(entry->flags & 1)) {
				break;
			}
		}
	}
}


void itemMain(void) {

}

// item status lives at 0x38 (u32, not yet declared in the header)
#define ITEM_STATUS(item) (*(u32*)((u8*)(item) + 0x38))

void itemPickUp(ItemEntry* item) {
    ITEM_STATUS(item) |= 0x1000;
    item->mode = 8;
    item->field_0x26 = 0;
    *(s32*)((u8*)item + 0x7c) = 0;
    *(s32*)((u8*)item + 0x78) = 0;
    ITEM_STATUS(item) &= ~0x100;
}

s32 itemStatus(ItemEntry* item, u32 mask) {
    return ITEM_STATUS(item) & mask;
}

void itemStatusOn(ItemEntry* item, u32 mask) {
    ITEM_STATUS(item) |= mask;
}

u16 itemFlag(ItemEntry* item, u16 flag) {
    return item->flags & flag;
}

void itemFlagOff(ItemEntry* item, u16 flag) {
    item->flags &= ~flag;
}

void itemFlagOn(ItemEntry* item, u16 flag) {
    item->flags |= flag;
}

void itemModeChange(ItemEntry* item, u16 mode) {
    item->mode = mode;
    item->field_0x26 = 0;
}

void itemSetPosition(ItemEntry* item, f32 x, f32 y, f32 z) {
    item->position.x = x;
    item->position.y = y;
    item->position.z = z;
}

void itemForceGet(ItemEntry* item) {
    *(s32*)((u8*)item + 0x7c) = 0;
    *(s32*)((u8*)item + 0x78) = 0;
    item->mode = 2;
    item->field_0x26 = 0;
    item->flags |= 0x1000;
    ITEM_STATUS(item) &= ~0x10000;
}

void itemNokoForceGet(ItemEntry* item) {
    if (itemGetNokoCheck(item)) {
        *(s32*)((u8*)item + 0x7c) = 0;
        *(s32*)((u8*)item + 0x78) = 0;
        item->mode = 2;
        item->field_0x26 = 0;
        item->flags |= 0x5000;
        ITEM_STATUS(item) &= ~0x10000;
    }
    else {
        *(s32*)((u8*)item + 0x7c) = 0;
        *(s32*)((u8*)item + 0x78) = 0;
        item->mode = 1;
        item->field_0x26 = 0;
        ITEM_STATUS(item) &= ~0x4;
        ITEM_STATUS(item) &= ~0x8;
        ITEM_STATUS(item) &= ~0x4000;
        ITEM_STATUS(item) &= ~0x1000;
    }
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

ItemEntry* itemNameToPtr(char* name) {
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

// NONMATCHING (98.25%): the itemId range check emits `blt` to the else block where the
// original emits `bge` to the then block plus an extra `b`; logic is identical, 2 instr differ.
void itemDelete(char* name) {
    ItemWork* wp = itemGetWork();
    ItemEntry* entry = wp->entries;
    int i;

    for (i = 0; i < wp->count; i++, entry++) {
        if (!strcmp(entry->name, name)) {
            itemCheckBadge(entry);
            if (entry->itemId < 0x79 && entry->itemId >= 0x72) {
                effSoftDelete(entry->effect);
            }
            else {
                iconDelete(entry->name);
            }
            entry->flags &= ~1;
        }
    }
}

void winHelpMain(WinMgrEntry* entry) {
    winMgrHelpMain(entry);
}

void winHelpDisp(WinMgrEntry* entry) {
    ItemEntry* item = entry->param;

    if (!winMgrAction(*(s32*)((u8*)item + 0x2c))) {
        entry->field_0x34 = (s32)msgSearch(itemDataTable[item->itemId].description);
        winMgrHelpDraw(entry);
    }
}

void winFullDisp(WinMgrEntry* entry) {
    ItemEntry* item = entry->param;
    const char* msg;
    u16 width;

    msg = (item->itemId >= 0xf0 && item->itemId < 0x153) ? msgSearch(str_802CADE8) : msgSearch(str_802CAE00);

    width = FontGetMessageWidth(msg);
    if (width + 0x14 > entry->field_0x20) {
        entry->field_0x20 = width + 0x14;
        entry->field_0x18 = -entry->field_0x20 / 2;
    }

    if (!winMgrAction(*(s32*)((u8*)item + 0x30))) {
        GXColor color;

        FontDrawStart();
        FontDrawEdge();
        *(u32*)&color = dat_8041af98;
        FontDrawColor(&color);
        FontDrawMessage(-width / 2, entry->field_0x1C - 0xa, msg);
        FontDrawEdgeOff();
    }
}

void winNameDisp2(WinMgrEntry* entry) {
    ItemEntry* item = entry->param;
    char buf[0x400];
    const char* msg;
    const char* name;
    u16 width;

    name = msgSearch(itemDataTable[item->itemId].name);
    msg = msgSearch(str_802CAE18);
    sprintf(buf, msg, name);

    width = FontGetMessageWidth(buf);
    if (width + 0x14 > entry->field_0x20) {
        entry->field_0x20 = width + 0x14;
        entry->field_0x18 = -entry->field_0x20 / 2;
    }

    if (!winMgrAction(*(s32*)((u8*)item + 0x28))) {
        GXColor color;

        FontDrawStart();
        *(u32*)&color = dat_8041af94;
        FontDrawColor(&color);
        FontDrawMessage(-width / 2, entry->field_0x1C - 8, buf);
    }
}

void winNameDisp(WinMgrEntry* entry) {
    ItemEntry* item = entry->param;
    char buf[0x400];
    const char* msg;
    u16 width;

    msg = msgSearch(itemDataTable[item->itemId].name);
    if (item->itemId >= 0xf0 && item->itemId < 0x153) {
        sprintf(buf, msgSearch(str_802CAE30), msg);
    }
    else {
        sprintf(buf, msgSearch(str_802CAE48), msg);
    }

    width = FontGetMessageWidth(buf);
    if (width + 0x14 > entry->field_0x20) {
        entry->field_0x20 = width + 0x14;
        entry->field_0x18 = -entry->field_0x20 / 2;
    }

    if (!winMgrAction(*(s32*)((u8*)item + 0x28))) {
        GXColor color;

        FontDrawStart();
        *(u32*)&color = dat_8041af90;
        FontDrawColor(&color);
        FontDrawMessage(-width / 2, entry->field_0x1C - 8, buf);
    }
}

void itemCoinDrop(Vec* pos) {
    ItemEntry* item;

    if (pouchGetCoin() > 0) {
        pouchAddCoin(-1);
        item = itemEntry(NULL, 0x79, 8, -1, 0, pos->x, pos->y, pos->z);
        if (item != NULL) {
            ITEM_STATUS(item) |= 0x40;
        }
    }
}

// NONMATCHING (80.38%): logic is complete but the struct-init temporaries / register
// allocation differ from the original (frame 0x80 vs 0xa0); stack-offset diffs cascade.
ItemEntry* itemNearDistCheck(f32 x, f32 y, f32 z, f32 maxDist) {
    ItemEntry* entry;
    ItemEntry* result = NULL;
    ItemWork* wp;
    Vec pos;
    Vec itemPos;
    Vec delta;
    f32 best = maxDist;
    f32 mag;
    int i;

    marioGetPtr();
    wp = itemGetWork();

    pos = vec3_802cabec;
    pos.x = x;
    pos.y = y;
    pos.z = z;

    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        if ((entry->flags & 1) && !itemStatus(entry, 0x1000) && !itemStatus(entry, 0x100000)
            && entry->mode != 2 && !itemStatus(entry, 0x2000000)) {
            itemPos = vec3_802cabf8;
            itemPos.x = entry->position.x;
            itemPos.y = entry->position.y;
            itemPos.z = entry->position.z;

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
    if ((u32)marioStGetSystemLevel() != 0) {
        return 0;
    }
    if (mario->flags & 0xa) {
        return 0;
    }
    if (itemStatus(entry, 0x2000000)) {
        return 0;
    }
    if ((s32)gp->field_0x164 != 0) {
        return 0;
    }
    if (seqGetSeq() != 2) {
        return 0;
    }
    if (mario->currMotionId == 0x19) {
        return 0;
    }
    if (vivianGetStatus() != 0) {
        return 0;
    }
    motion = mario->currMotionId;
    if (motion == 0x1f || motion == 0x20) {
        return 0;
    }
    if ((u16)(motion - 5) <= 3 || motion == 0xf || motion == 0xe) {
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

// NONMATCHING (93.34%): switch lowering block layout differs; the mode 1 / mode 3-4 return
// blocks are not tail-merged the way the original has them. Tree split was moved to 5 by
// reordering the cases.
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
        if (!(*(u32*)((u8*)mario + 0x14) & 1)) {
            if ((u32)marioStGetSystemLevel() != 0) {
                return 0;
            }
            if (mario->flags & 0xa) {
                return 0;
            }
        }
    }
    else {
        if ((u32)marioStGetSystemLevel() != 0) {
            return 0;
        }
        if (mario->flags & 0xa) {
            return 0;
        }
    }

    if ((s32)gp->field_0x164 != 0) {
        return 0;
    }
    if (seqGetSeq() != 2) {
        return 0;
    }
    if (mario->currMotionId == 0x19) {
        return 0;
    }
    if (mario->currMotionId == 0x18) {
        return 0;
    }
    if (vivianGetStatus() != 0) {
        return 0;
    }
    if (mario->currMotionId == 0x1f || mario->currMotionId == 0x20) {
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
    case 2:
        return 0;
    case 5:
    case 6:
        if (entry->field_0x26 != 0 && !itemStatus(entry, 0x4) && !itemStatus(entry, 0x8)) {
            return 1;
        }
        return 0;
    case 3:
    case 4:
        if (itemStatus(entry, 0x8)) {
            return 0;
        }
        return 1;
    case 1:
        return 1;
    case 7:
        return 1;
    default:
        return 0;
    }
}
