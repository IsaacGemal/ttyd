#include "mario/mario_pouch.h"
#include "mario/mario_party.h"
#include "driver/swdrv.h"
#include "evt/evt_badgeshop.h"
#include "evt/evt_yuugijou.h"
#include "data/item_data.h"
#include "driver/msgdrv.h"
#include "memory.h"
#include "system.h"
#include <string.h>

extern PiantaParlorWork* yuwp;
extern BadgeShopWork* bdsw;
extern ItemData itemDataTable[];

static PouchData* mpp;


//.data
s16 _party_max_hp_table[8][4] = {
	{0, 0, 0, 0},
	{10, 20, 30, 200},
	{10, 15, 25, 200},
	{20, 30, 40, 200},
	{10, 20, 30, 200},
	{15, 25, 35, 200},
	{15, 20, 30, 200},
	{15, 20, 25, 200}
};

PouchData* pouchGetPtr(void) {
	return mpp;
}

void pouchInit(void) {
	PouchPartyData* party;
	int i;

	mpp = __memAlloc(HEAP_DEFAULT, sizeof(PouchData));
	memset(mpp, 0, sizeof(PouchData));
	for (i = 0; i < MAX_KEY_ITEMS; i++) {
		mpp->keyItems[i] = ITEM_NULL;
	}
	for (i = 0; i < MAX_HELD_ITEMS; i++) {
		mpp->heldItems[i] = ITEM_NULL;
	}
	for (i = 0; i < MAX_STORED_ITEMS; i++) {
		mpp->storedItems[i] = ITEM_NULL;
	}
	for (i = 0; i < MAX_BADGES; i++) {
		mpp->badges[i] = ITEM_NULL;
	}
	for (i = 0; i < MAX_BADGES; i++) {
		mpp->equippedBadges[i] = ITEM_NULL;
	}
	mpp->maximumHP = 10;
	mpp->baseMaxHP = 10;
	mpp->maximumFP = 5;
	mpp->baseMaxFP = 5;
	mpp->availableBP = 3;
	mpp->maximumBP = 3;
	mpp->currentHP = mpp->maximumHP;
	mpp->currentFP = mpp->maximumFP;
	mpp->currentSP = 0;
	mpp->field_0x7E = 0;
	mpp->maximumSP = 0;
	mpp->field_0x80 = 0;
	mpp->audienceCount = 0.0f;
	mpp->rank = 0;
	mpp->level = 1;
	mpp->jumpLevel = 0;
	mpp->hammerLevel = 0;
	for (i = 0; i < 8; i++) {
		party = &mpp->partyData[i];
		if (i != PARTNER_NONE) {
			party->flags = 0;
			party->baseMaxHP = 10;
			party->maximumHP = party->baseMaxHP;
			party->currentHP = party->baseMaxHP;
			party->attackLevel = 0;
			party->techLevel = 0;
		}
	}
	strcpy(mpp->yoshiName, "チビヨッシー"); //TODO: US
}

ItemType pouchKeyItem(s32 id) {
	return mpp->keyItems[id];
}

ItemType pouchHaveItem(s32 id) {
	return mpp->heldItems[id];
}

ItemType pouchKeepItem(s32 id) {
	return mpp->storedItems[id];
}

ItemType pouchHaveBadge(s32 id) {
	return mpp->badges[id];
}

s32 pouchGetHaveItemCnt(void) {
	s32 i, count;

	for (count = 0, i = 0; i < MAX_HELD_ITEMS; i++) {
		if (mpp->heldItems[i] != ITEM_NULL) {
			count++;
		}
	}
	return count;
}

s32 pouchGetKeepItemCnt(void) {
	s32 i, count;

	for (count = 0, i = 0; i < MAX_STORED_ITEMS; i++) {
		if (mpp->storedItems[i] != ITEM_NULL) {
			count++;
		}
	}
	return count;
}

s32 pouchGetHaveBadgeCnt(void) {
	s32 i, count;

	for (count = 0, i = 0; i < MAX_BADGES; i++) {
		if (mpp->badges[i] != ITEM_NULL) {
			count++;
		}
	}
	return count;
}

s32 pouchGetEquipBadgeCnt(void) {
	s32 i, count;

	for (count = 0, i = 0; i < MAX_BADGES; i++) {
		if (mpp->equippedBadges[i] != ITEM_NULL) {
			count++;
		}
	}
	return count;
}

s32 pouchGetEmptyHaveItemCnt(void) {
	s32 i, count, max;
	s32 have;

	have = (s32)pouchCheckItem(ITEM_STRANGE_SACK);
	max = have ? MAX_HELD_ITEMS : HELD_ITEM_COUNT_BASE;
	for (i = 0, count = 0; i < max; i++) {
		if (mpp->heldItems[i] == ITEM_NULL) {
			count++;
		}
	}
	return count;
}

s32 pouchGetEmptyKeepItemCnt(void) {
	s32 i, count;

	for (count = 0, i = 0; i < MAX_STORED_ITEMS; i++) {
		if (mpp->storedItems[i] == ITEM_NULL) {
			count++;
		}
	}
	return count;
}

static inline BOOL pouchInsertItem(ItemType type) {
	s16* p1;
	s16* p2;
	s32 maximum;
	BOOL valid;
	s16* table1;
	s16* table2;
	int i;

	maximum = 0;
	table1 = NULL;
	table2 = NULL;
	if (type >= ITEM_MIN && type < ITEM_MAX) {
		table1 = mpp->heldItems;
		maximum = (s32)pouchCheckItem(ITEM_STRANGE_SACK) ? MAX_HELD_ITEMS : HELD_ITEM_COUNT_BASE;
		valid = TRUE;
	}
	else if (type >= KEY_ITEM_MIN && type < KEY_ITEM_MAX) {
		table1 = mpp->keyItems;
		table2 = NULL;
		maximum = MAX_KEY_ITEMS;
		valid = TRUE;
	}
	else if (type >= BADGE_MIN && type < BADGE_MAX) {
		table1 = mpp->badges;
		table2 = mpp->equippedBadges;
		maximum = MAX_BADGES;
		valid = TRUE;
	}
	else {
		valid = FALSE;
	}
	if (!valid) {
		return FALSE;
	}
	for (i = maximum - 1; i >= 0; i--) {
		if (table1[i] == ITEM_NULL) {
			p1 = &table1[i];
			p2 = table2 + i;
			if (table2 != NULL) {
				while (i-- > 0) {
					*p1 = *(p1 - 1);
					*p2 = *(p2 - 1);
					p1--;
					p2--;
				}
			}
			else {
				while (i-- > 0) {
					*p1 = *(p1 - 1);
					p1--;
				}
			}
			table1[0] = type;
			if (table2 != NULL) {
				table2[0] = ITEM_NULL;
			}
			return TRUE;
		}
	}
	return FALSE;
}

//we have obtained an item, update the appropriate data
BOOL pouchGetItem(ItemType type) {
	s32 partyId;

	if (type == ITEM_COIN) {
		if (mpp->coins < 999) {
			mpp->coins++;
		}
		return TRUE;
	}
	if (type == ITEM_PIANTA_TOKEN) {
		if (yuwp->tokens < 99999) {
			yuwp->tokens++;
		}
		return TRUE;
	}
	if (type == ITEM_HEART) {
		partyId = marioGetParty();
		if (mpp->currentHP < mpp->maximumHP) {
			mpp->currentHP++;
		}
		if (partyId != 0) {
			if (mpp->partyData[partyId].currentHP < mpp->partyData[partyId].maximumHP) {
				mpp->partyData[partyId].currentHP++;
			}
		}
		return TRUE;
	}
	if (type == ITEM_FLOWER) {
		if (mpp->currentFP < mpp->maximumFP) {
			mpp->currentFP++;
		}
		return TRUE;
	}
	if (type == ITEM_STAR_PIECE) {
		mpp->mStarPieceCount++;
		return TRUE;
	}
	if (type == ITEM_SHINE_SPRITE) {
		mpp->mShineSpriteCount++;
		return TRUE;
	}
	if (type == ITEM_BOOTS || type == ITEM_SUPER_BOOTS || type == ITEM_ULTRA_BOOTS) {
		switch (type) {
		case ITEM_BOOTS:
			mpp->jumpLevel = 1;
			break;
		case ITEM_SUPER_BOOTS:
			mpp->jumpLevel = 2;
			break;
		case ITEM_ULTRA_BOOTS:
			mpp->jumpLevel = 3;
			break;
		}
	}
	else if (type == ITEM_HAMMER || type == ITEM_SUPER_HAMMER || type == ITEM_ULTRA_HAMMER) {
		switch (type) {
		case ITEM_HAMMER:
			mpp->hammerLevel = 1;
			break;
		case ITEM_SUPER_HAMMER:
			mpp->hammerLevel = 2;
			break;
		case ITEM_ULTRA_HAMMER:
			mpp->hammerLevel = 3;
			break;
		}
	}
	else {
		if (type == ITEM_BOWSER_BROS_MEAT) {
			return TRUE;
		}
		if (type >= BADGE_MIN && type < BADGE_MAX) {
			swSet(type - 0x70);
		}
	}
	return pouchInsertItem(type);
}








// keep pouchCheckItem out of line: its callers must emit `bl pouchCheckItem`, not an inlined loop
#pragma dont_inline on
u32 pouchCheckItem(u32 itemId) {
	int i;
	u32 itemCnt;
	s32 id = (s32)itemId;

	if (ITEM_MIN <= id && id < ITEM_MAX) {
		for (itemCnt = 0, i = 0; i < MAX_HELD_ITEMS; i++) {
			if (id == mpp->heldItems[i]) {
				itemCnt++;
			}
		}
		return itemCnt;
	}
	else if (KEY_ITEM_MIN <= id && id < KEY_ITEM_MAX) {
		for (itemCnt = 0, i = 0; i < MAX_KEY_ITEMS; i++) {
			if (id == mpp->keyItems[i]) {
				itemCnt++;
			}
		}
		return itemCnt;
	}
	else if (BADGE_MIN <= id && id < BADGE_MAX) {
		for (itemCnt = 0, i = 0; i < MAX_BADGES; i++) {
			if (id == mpp->badges[i]) {
				itemCnt++;
			}
		}
		return itemCnt;
	}
	else {
		return 0;
	}
}
#pragma dont_inline reset


static inline void pouchShiftItems(s16* table1, s16* table2, s32 index, s32 maximum) {
	table1[index] = ITEM_NULL;
	if (table2 != NULL) {
		table2[index] = ITEM_NULL;
	}
	for (; index < maximum - 1; index++) {
		table1[index] = table1[index + 1];
		table1[index + 1] = ITEM_NULL;
		if (table2 != NULL) {
			table2[index] = table2[index + 1];
			table2[index + 1] = ITEM_NULL;
		}
	}
}

BOOL pouchRemoveItem(ItemType type) {
	s16* table1, * table2;
	BOOL valid;
	s32 maximum;
	int i;

	maximum = 0;
	table1 = NULL;
	table2 = NULL;
	if (type >= ITEM_MIN && type < ITEM_MAX) {
		table1 = mpp->heldItems;
		table2 = NULL;
		maximum = (s32)pouchCheckItem(ITEM_STRANGE_SACK) ? MAX_HELD_ITEMS : HELD_ITEM_COUNT_BASE;
		valid = TRUE;
	}
	else if (type >= KEY_ITEM_MIN && type < KEY_ITEM_MAX) {
		table1 = mpp->keyItems;
		table2 = NULL;
		maximum = 121;
		valid = TRUE;
	}
	else if (type >= BADGE_MIN && type < BADGE_MAX) {
		table1 = mpp->badges;
		table2 = mpp->equippedBadges;
		maximum = 200;
		valid = TRUE;
	}
	else {
		valid = FALSE;
	}
	if (!valid) {
		return FALSE;
	}
	for (i = 0; i < maximum; i++) {
		if (type == table1[i]) {
			pouchShiftItems(table1, table2, i, maximum);
			return TRUE;
		}
	}
	return FALSE;
}

s32 pouchRemoveItemIndex(s32 id, s32 index) {
	s16* table1, * table2;
	s16* p;
	BOOL valid;
	s32 maximum;
	int i;

	maximum = 0;
	table1 = NULL;
	table2 = NULL;
	if (id >= ITEM_MIN && id < ITEM_MAX) {
		table1 = mpp->heldItems;
		table2 = NULL;
		maximum = (s32)pouchCheckItem(ITEM_STRANGE_SACK) ? MAX_HELD_ITEMS : HELD_ITEM_COUNT_BASE;
		valid = TRUE;
	}
	else if (id >= KEY_ITEM_MIN && id < KEY_ITEM_MAX) {
		table1 = mpp->keyItems;
		table2 = NULL;
		maximum = 121;
		valid = TRUE;
	}
	else if (id >= BADGE_MIN && id < BADGE_MAX) {
		table1 = mpp->badges;
		table2 = mpp->equippedBadges;
		maximum = 200;
		valid = TRUE;
	}
	else {
		valid = FALSE;
	}
	if (!valid) {
		return FALSE;
	}
	p = table1 + index;
	if (id != *p) {
		return FALSE;
	}
	for (i = index; i < maximum; p++, i++) {
		if (id == *p) {
			pouchShiftItems(table1, table2, i, maximum);
			return TRUE;
		}
	}
	return FALSE;
}

void pouchHiraganaToKatakana(char* src, char* dst) {
	u16 c;
	int len = strlen(src) / 2;
	char* s = src;
	char* d = dst;
	int i;

	for (i = 0; i < len; i++) {
		c = *(u16*)s;
		if (c >= 0x829F && c < 0x82DE) {
			c += 0xA1;
		}
		else if (c >= 0x82DE && c <= 0x82F1) {
			c += 0xA2;
		}
		*(u16*)d = c;
		s += 2;
		d += 2;
	}
	dst[i * 2] = 0;
}

// 100%: `const` on the first parameter is the whole difference. The compiler hoists the id_a load
// (`lha r0,0x0(r3)`) above the itemDataTable address computation only when `a` is `const`; with that
// read first MWCC's scheduler emits the target's prologue: each callee-save store sinks next to its
// register's first definition (`stw r31` after the address `lis`, `stw r30` before `addi r30`,
// `stw r29` before `lha r29`) and `id_b` loads after `addi r30`. With a plain `s16* a` the load is
// sunk to just before the address computation and all three stores cluster at the top (90.24%).
// The `base` local and the MUST_MATCH `base + id*sizeof(ItemData)` access are still needed: written
// as itemDataTable[id].name MWCC colours the base r31 and the msgSearch result r30, the reverse of
// the target. The two conversion loops already matched. The `b` parameter is deliberately not
// `const`: making both const (or only b) puts id_b, not str, in r31.
s32 comp_aiueo(const s16* a, s16* b) {
	u16 buf_a[64];
	u16 buf_b[64];
	const char* str;
	s16 id_a = *a;
#ifdef MUST_MATCH
	// Match-only: spelling the access as itemDataTable[id].name lets MWCC colour the base r31 and
	// the msgSearch result r30; the explicit base local gives the original's r30/r31. MUST_MATCH
	// only because the original compiler CSE'd the same base itself.
	const char* base = (const char*)&itemDataTable[0].name;
#endif
	s16 id_b = *b;
	{
		s32 i;
		u32 c;
		u16* buf;
		s32 len;

#ifdef MUST_MATCH
		str = msgSearch(*(const char**)(base + id_a * sizeof(ItemData)));
#else
		str = msgSearch(itemDataTable[id_a].name);
#endif
		len = strlen(str) / 2;
		buf = buf_a;
		for (i = 0; i < len; i++) {
			c = *(u16*)str;
			if (c >= 0x829F && c < 0x82DE) {
				c += 0xA1;
			}
			else if (c >= 0x82DE && c <= 0x82F1) {
				c += 0xA2;
			}
			*buf = c;
			str += 2;
			buf++;
		}
		((u8*)buf_a)[i * 2] = 0;
	}
	{
		s32 i;
		u32 c;
		u16* buf;
		s32 len;

#ifdef MUST_MATCH
		str = msgSearch(*(const char**)(base + id_b * sizeof(ItemData)));
#else
		str = msgSearch(itemDataTable[id_b].name);
#endif
		len = strlen(str) / 2;
		buf = buf_b;
		for (i = 0; i < len; i++) {
			c = *(u16*)str;
			if (c >= 0x829F && c < 0x82DE) {
				c += 0xA1;
			}
			else if (c >= 0x82DE && c <= 0x82F1) {
				c += 0xA2;
			}
			*buf = c;
			str += 2;
			buf++;
		}
		((u8*)buf_b)[i * 2] = 0;
	}

	return strcmp((const char*)buf_a, (const char*)buf_b);
}

s32 comp_kind(s16* a, s16* b) {
	s32 va = itemDataTable[*a].type_sort_order;
	s32 vb = itemDataTable[*b].type_sort_order;

	if (va > vb) {
		return 1;
	}
	if (va < vb) {
		return -1;
	}
	return 0;
}

// NONMATCHING (90.24%): same as comp_aiueo (its callee-saved registers now match too); the
// remaining diff is the same callee-save store placement and id_b load slot (8 instructions), and the
// same experiments above did not move it.
s32 comp_aiueo_r(const s16* a, s16* b) {
	u16 buf_a[64];
	u16 buf_b[64];
	const char* str;
	s16 id_a = *a;
#ifdef MUST_MATCH
	// Match-only: see comp_aiueo.
	const char* base = (const char*)&itemDataTable[0].name;
#endif
	s16 id_b = *b;
	{
		s32 i;
		u32 c;
		u16* buf;
		s32 len;

#ifdef MUST_MATCH
		str = msgSearch(*(const char**)(base + id_a * sizeof(ItemData)));
#else
		str = msgSearch(itemDataTable[id_a].name);
#endif
		len = strlen(str) / 2;
		buf = buf_a;
		for (i = 0; i < len; i++) {
			c = *(u16*)str;
			if (c >= 0x829F && c < 0x82DE) {
				c += 0xA1;
			}
			else if (c >= 0x82DE && c <= 0x82F1) {
				c += 0xA2;
			}
			*buf = c;
			str += 2;
			buf++;
		}
		((u8*)buf_a)[i * 2] = 0;
	}
	{
		s32 i;
		u32 c;
		u16* buf;
		s32 len;

#ifdef MUST_MATCH
		str = msgSearch(*(const char**)(base + id_b * sizeof(ItemData)));
#else
		str = msgSearch(itemDataTable[id_b].name);
#endif
		len = strlen(str) / 2;
		buf = buf_b;
		for (i = 0; i < len; i++) {
			c = *(u16*)str;
			if (c >= 0x829F && c < 0x82DE) {
				c += 0xA1;
			}
			else if (c >= 0x82DE && c <= 0x82F1) {
				c += 0xA2;
			}
			*buf = c;
			str += 2;
			buf++;
		}
		((u8*)buf_b)[i * 2] = 0;
	}

	return strcmp((const char*)buf_b, (const char*)buf_a);
}

s32 comp_kind_r(s16* a, s16* b) {
	s32 va = itemDataTable[*a].type_sort_order;
	s32 vb = itemDataTable[*b].type_sort_order;

	if (va < vb) {
		return 1;
	}
	if (va > vb) {
		return -1;
	}
	return 0;
}

void pouchSortItem(s32 type) {
	s16 badgeList[MAX_BADGES];
	s16 equippedList[MAX_BADGES];
	s16 keyList[MAX_KEY_ITEMS];
	s16 holdList[MAX_HELD_ITEMS];
	ItemData* itemA;
	ItemData* itemB;
	s32 count;
	// Calling the inlined pouchGetPtr() instead of reading mpp directly is what makes MWCC keep the
	// pouch pointer in r29 for the whole function (the original caches it there).
	PouchData* pouch = pouchGetPtr();
	s32 i, j, k, badgeCount;
	BOOL swap;
	s16 temp1, temp2;
	s32 bpA, bpB;
	const char *idA, *idB;

	if (type == 0 || type == 1 || type == 7 || type == 8) {
		count = 0;
		for (i = 0; i < MAX_HELD_ITEMS; i++) {
			if (pouch->heldItems[i] != ITEM_NULL) {
				holdList[count++] = pouch->heldItems[i];
			}
		}
		for (i = count; i < MAX_HELD_ITEMS; i++) {
			holdList[i] = 0;
		}
		if (type == 0) {
			qqsort(holdList, count, 2, (int (*)(void*, void*))comp_aiueo);
		}
		else if (type == 1) {
			qqsort(holdList, count, 2, (int (*)(void*, void*))comp_kind);
		}
		else if (type == 7) {
			qqsort(holdList, count, 2, (int (*)(void*, void*))comp_aiueo_r);
		}
		else if (type == 8) {
			qqsort(holdList, count, 2, (int (*)(void*, void*))comp_kind_r);
		}
		memcpy(pouch->heldItems, holdList, sizeof(holdList));
	}
	if (type == 2 || type == 3 || type == 9 || type == 10) {
		count = 0;
		for (i = 0; i < MAX_KEY_ITEMS; i++) {
			if (pouch->keyItems[i] != ITEM_NULL) {
				keyList[count++] = pouch->keyItems[i];
			}
		}
		for (i = count; i < MAX_KEY_ITEMS; i++) {
			keyList[i] = 0;
		}
		if (type == 2) {
			qqsort(keyList, count, 2, (int (*)(void*, void*))comp_aiueo);
		}
		else if (type == 3) {
			qqsort(keyList, count, 2, (int (*)(void*, void*))comp_kind);
		}
		else if (type == 9) {
			qqsort(keyList, count, 2, (int (*)(void*, void*))comp_aiueo_r);
		}
		else if (type == 10) {
			qqsort(keyList, count, 2, (int (*)(void*, void*))comp_kind_r);
		}
		memcpy(pouch->keyItems, keyList, sizeof(keyList));
	}
	if (type == 4 || type == 5 || type == 6 || type == 11 || type == 12 || type == 13) {
		badgeCount = 0;
		for (k = 0; k < MAX_BADGES; k++) {
			if (pouch->badges[k] != ITEM_NULL) {
				badgeList[badgeCount] = pouch->badges[k];
				equippedList[badgeCount] = pouch->equippedBadges[k];
				badgeCount++;
			}
		}
		for (k = badgeCount; k < MAX_BADGES; k++) {
			badgeList[k] = 0;
			equippedList[k] = 0;
		}
		for (i = 0; i < badgeCount - 1; i++) {
			for (j = i + 1; j < badgeCount; j++) {
				itemA = &itemDataTable[badgeList[i]];
				itemB = &itemDataTable[badgeList[j]];
				bpA = itemA->bp_cost;
				bpB = itemB->bp_cost;
				idA = itemA->id;
				idB = itemB->id;
				swap = FALSE;
				if (type == 4) {
					if (strcmp(idA, idB) > 0) {
						swap = TRUE;
					}
				}
				else if (type == 11) {
					if (strcmp(idA, idB) < 0) {
						swap = TRUE;
					}
				}
				else if (type == 5) {
					if (itemA->type_sort_order > itemB->type_sort_order) {
						swap = TRUE;
					}
				}
				else if (type == 12) {
					if (itemA->type_sort_order < itemB->type_sort_order) {
						swap = TRUE;
					}
				}
				else if (type == 6) {
					if (bpA > bpB) {
						swap = TRUE;
					}
					else if (bpA == bpB && itemA->type_sort_order > itemB->type_sort_order) {
						swap = TRUE;
					}
				}
				else if (type == 13) {
					if (bpA < bpB) {
						swap = TRUE;
					}
					else if (bpA == bpB && itemA->type_sort_order < itemB->type_sort_order) {
						swap = TRUE;
					}
				}
				if (swap) {
					temp1 = badgeList[i];
					temp2 = badgeList[j];
					badgeList[i] = temp2;
					badgeList[j] = temp1;
					temp1 = equippedList[i];
					temp2 = equippedList[j];
					equippedList[i] = temp2;
					equippedList[j] = temp1;
				}
			}
		}
		memcpy(pouch->badges, badgeList, sizeof(badgeList));
		memcpy(pouch->equippedBadges, equippedList, sizeof(equippedList));
	}
}

s32 pouchGetCoin(void) {
	return mpp->coins;
}

s32 pouchAddCoin(s32 coins) {
	mpp->coins += coins;
	if (mpp->coins < 0) {
		mpp->coins = 0;
	}
	if (mpp->coins > 999) {
		mpp->coins = 999;
	}
	return mpp->coins;
}

s32 pouchSetCoin(s32 coins) {
	mpp->coins = coins;
	if (mpp->coins < 0) {
		mpp->coins = 0;
	}
	if (mpp->coins > 999) {
		mpp->coins = 999;
	}
	return mpp->coins;
}

s32 pouchGetSuperCoin(void) {
	return mpp->mShineSpriteCount;
}

s32 pouchSetSuperCoin(s32 count) {
	mpp->mShineSpriteCount = count;
	return mpp->mShineSpriteCount;
}

s32 pouchGetStarPiece(void) {
	return mpp->mStarPieceCount;
}

BOOL pouchUnEquipBadgeID(s32 badgeId) {
	int i;

	for (i = 0; i < 200; i++) {
		if (mpp->equippedBadges[i] == badgeId) {
			if (mpp->equippedBadges[i]) {
				mpp->equippedBadges[i] = 0;
			}
			return TRUE;
		}
	}
	return FALSE;
}

s32 pouchAddStarPiece(s32 count) {
	mpp->mStarPieceCount += count;
	return mpp->mStarPieceCount;
}

s16 pouchAddHP(s16 points) {
	s32 hp;

	hp = mpp->currentHP + points;
	mpp->currentHP = hp;
	if (mpp->currentHP < 0) {
		mpp->currentHP = 0;
	}
	if (mpp->currentHP > mpp->maximumHP) {
		mpp->currentHP = mpp->maximumHP;
	}
	return mpp->currentHP;
}

s32 pouchGetHP(void) {
	return mpp->currentHP;
}

s32 pouchGetMaxHP(void) {
	return mpp->maximumHP;
}

void pouchSetHP(s32 points) {
	mpp->currentHP = points;
	if (mpp->currentHP > mpp->maximumHP) {
		mpp->currentHP = mpp->maximumHP;
	}
}

void pouchSetMaxHP(s16 points) {
	mpp->maximumHP = points;
	mpp->baseMaxHP = mpp->maximumHP;
}

s16 pouchGetPartyHP(PartyId partnerId) {
	return mpp->partyData[partnerId].currentHP;
}

void pouchSetPartyHP(PartyId partnerId, s16 points) {
	PouchPartyData* party;

	mpp->partyData[partnerId].currentHP = points;
	party = &mpp->partyData[partnerId];
	if (party->currentHP > party->maximumHP) {
		party->currentHP = party->maximumHP;
	}
}

s16 pouchGetFP(void) {
	return mpp->currentFP;
}

s32 pouchGetMaxFP(void) {
	return mpp->maximumFP;
}

void pouchSetFP(s32 points) {
	mpp->currentFP = points;
	if (mpp->currentFP > mpp->maximumFP) {
		mpp->currentFP = mpp->maximumFP;
	}
}

void pouchSetMaxFP(s16 points) {
	mpp->maximumFP = points;
	mpp->baseMaxFP = mpp->maximumFP;
}

s16 pouchGetAP(void) {
	return mpp->currentSP;
}

void pouchAddAP(s16 points) {
	mpp->currentSP += points;
	if (mpp->currentSP < 0) {
		mpp->currentSP = 0;
	}
	if (mpp->currentSP > mpp->maximumSP) {
		mpp->currentSP = mpp->maximumSP;
	}
}

void pouchSetAP(s32 points) {
	mpp->currentSP = points;
	if (mpp->currentSP < 0) {
		mpp->currentSP = 0;
	}
	if (mpp->currentSP > mpp->maximumSP) {
		mpp->currentSP = mpp->maximumSP;
	}
}

s32 pouchGetMaxAP(void) {
	return mpp->maximumSP;
}

f32 pouchGetAudienceNum(void) {
	return mpp->audienceCount;
}

void pouchSetAudienceNum(f32 num) {
	mpp->audienceCount = num;
}

s32 pouchGetJumpLv(void) {
	int level = 0;
	s32 c;

	c = (s32)pouchCheckItem(ITEM_ULTRA_BOOTS);
	if (c) {
		level = 3;
	}
	else {
		c = (s32)pouchCheckItem(ITEM_SUPER_BOOTS);
		if (c) {
			level = 2;
		}
		else {
			c = (s32)pouchCheckItem(ITEM_BOOTS);
			if (c) {
				level = 1;
			}
		}
	}
	mpp->jumpLevel = level;
	return mpp->jumpLevel;
}

s32 pouchGetHammerLv(void) {
	int level = 0;
	s32 c;

	c = (s32)pouchCheckItem(ITEM_ULTRA_HAMMER);
	if (c) {
		level = 3;
	}
	else {
		c = (s32)pouchCheckItem(ITEM_SUPER_HAMMER);
		if (c) {
			level = 2;
		}
		else {
			c = (s32)pouchCheckItem(ITEM_HAMMER);
			if (c) {
				level = 1;
			}
		}
	}
	mpp->hammerLevel = level;
	return mpp->hammerLevel;
}

s32 pouchGetPartyAttackLv(PartyId partyId) {
	PouchPartyData* party = &mpp->partyData[partyId];

	return party->attackLevel;
}

//TODO: this function
BOOL pouchAddKeepItem(s32 id) {
	int i, j;

	for (i = MAX_STORED_ITEMS - 1; i >= 0; i--) {
		if (mpp->storedItems[i] == ITEM_NULL) {
			for (j = i; j > 0; j--) {
				mpp->storedItems[j] = mpp->storedItems[j - 1];
			}
			mpp->storedItems[0] = id;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL pouchRemoveKeepItem(s32 id, int index) {
	int i, j;

	if (mpp->storedItems[index] != id) {
		return FALSE;
	}
	for (i = index; i < MAX_STORED_ITEMS; i++) {
		if (mpp->storedItems[i] == id) {
			mpp->storedItems[i] = ITEM_NULL;
			for (j = i; j < MAX_STORED_ITEMS - 1; j++) {
				mpp->storedItems[j] = mpp->storedItems[j + 1];
				mpp->storedItems[j + 1] = ITEM_NULL;
			}
			return TRUE;
		}
	}
	return FALSE;
}

void pouchReviseMarioParam(void) {
	int i;

	mpp->maximumHP = mpp->baseMaxHP;
	mpp->maximumFP = mpp->baseMaxFP;

	for (i = 0; i < 200; i++) {
		s16 badge = mpp->equippedBadges[i];

		if (badge == ITEM_NULL) {
			continue;
		}
		switch (badge) {
			case ITEM_HP_PLUS:
				mpp->maximumHP += 5;
				break;
			case ITEM_FP_PLUS:
				mpp->maximumFP += 5;
				break;
		}
	}
	if (mpp->currentHP > mpp->maximumHP) {
		mpp->currentHP = mpp->maximumHP;
	}
	if (mpp->currentFP > mpp->maximumFP) {
		mpp->currentFP = mpp->maximumFP;
	}
	mpp->availableBP = mpp->maximumBP;

	for (i = 0; i < 200; i++) {
		mpp->availableBP -= itemDataTable[mpp->equippedBadges[i]].bp_cost;
	}
}

void pouchRevisePartyParam(void) {
	PouchPartyData* party;
	int i, count;

	count = pouchEquipCheckBadge(ITEM_HP_PLUS_P);
	for (i = 0; i < 8; i++) { //TODO: NUM_PARTNERS macro
		if (i != PARTNER_NONE) {
			party = &mpp->partyData[i];
			party->baseMaxHP = _party_max_hp_table[i][party->HPLevel];
			party->maximumHP = (s16)(party->baseMaxHP + (count * 5));
			if (party->currentHP > party->maximumHP) {
				party->currentHP = party->maximumHP;
			}
		}
	}
}

// Return type is s32 like the file's other s16-field accessors: the original's `lha r3, 0x96(r3)`
// sign-extends with no clrlwi, so the header's previous `u16` was wrong.
s32 pouchGetStarPoint(void) {
	return mpp->mStarPoints;
}

BOOL pouchEquipBadgeIndex(s32 index) {
	if (mpp->badges[index] == 0) {
		return FALSE;
	}
	mpp->equippedBadges[index] = mpp->badges[index];
	return TRUE;
}

BOOL pouchUnEquipBadgeIndex(s32 index) {
	if (mpp->equippedBadges[index] == 0) {
		return FALSE;
	}
	mpp->equippedBadges[index] = 0;
	return TRUE;
}

s32 pouchEquipCheckBadge(s32 badgeId) {
	int i, count;

	count = 0;
	for (i = 0; i < 200; i++) {
		if (badgeId == mpp->equippedBadges[i]) {
			count++;
		}
	}
	return count;
}

BOOL pouchEquipCheckBadgeIndex(s32 index) {
	return mpp->equippedBadges[index] != 0;
}

BOOL pouchEquipBadgeID(s32 badgeId) {
	int i;

	for (i = 0; i < 200; i++) {
		if (mpp->badges[i] == badgeId) {
			if (mpp->badges[i]) {
				mpp->equippedBadges[i] = mpp->badges[i];
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL N_pouchUnEquipBadgeID(s32 badgeId) {
	int i;

	for (i = 0; i < 200; i++) {
		if (mpp->equippedBadges[i] == badgeId) {
			if (mpp->equippedBadges[i]) {
				mpp->equippedBadges[i] = 0;
			}
			return TRUE;
		}
	}
	return FALSE;
}

s32 pouchArriveBadge(s32 id) {
	return badgeShop_add(bdsw->special_table, (s16)id, 1);
}

void pouchMajinaiInit(u32 curseCount) {
	if ((s32)mpp->mMerleeCurseUsesRemaining < (s32)curseCount) {
		mpp->mMerleeCurseUsesRemaining = (u8)curseCount;
		mpp->mTurnsUntilMerleeActivation = -1;
		mpp->mMerleeNextCurseType = 0;
	}
}

u8 pouchGetKpaCoin(void) {
	return mpp->mSuperBowserCoins;
}

BOOL pouchAddKpaCoin(s32 coins) {
	BOOL wrapped = FALSE;

	mpp->mSuperBowserCoins += coins;
	if (mpp->mSuperBowserCoins >= 100) {
		mpp->mSuperBowserCoins -= 100;
		wrapped = TRUE;
	}
	return wrapped;
}

u32 pouchGetKpaScore(void) {
	return mpp->mSuperBowserScore;
}

void pouchAddKpaScore(u32 score) {
	mpp->mSuperBowserScore += score;
	if (mpp->mSuperBowserScore > 999999)
	{
		mpp->mSuperBowserScore = 999999;
	}
}

void pouchGetStarStone(u32 id) {
	if (!mpp->mStarPowersObtained) {
		mpp->audienceCount = 2.0f;
	}
	mpp->mStarPowersObtained |= 1 << id;
	mpp->maximumSP = (s16)(100 * (id + 1));
	if (mpp->maximumSP < 0) {
		mpp->maximumSP = 0;
	}
	if (mpp->maximumSP > 800) {
		mpp->maximumSP = 800;
	}
}








s32 pouchReceiveMailCount(void) {
	s32 i, count;

	for (count = 0, i = 0; i < 99; i++) {
		if (mpp->mEmailReceivedFlags[i / 32] & (1 << (i % 32))) {
			count++;
		}
	}
	return count;
}
void pouchReceiveMail(s32 mailId) {
#ifdef MUST_MATCH
	// Found by the permuter: the loop counter and the running count share a two-element array
	// (they sit side by side on the stack), which fixes the original's register and slot order.
	int count0;
	int slot[2];
	s32 index;
	s32 shift;

	index = mailId / 32;
	shift = mailId % 32;

	count0 = 0;
	slot[0] = count0;
	for (slot[1] = 0; slot[1] < 99; slot[1]++) {
		if (mpp->mEmailReceivedFlags[slot[1] / 32] & (1 << (slot[1] % 32))) {
			slot[0]++;
		}
	}
	mpp->mEmailReceivedFlags[index] |= (1 << shift);
	mpp->mEmailIds[slot[0]] = (u8)mailId;
#else
	int i;
	s32 count;
	s32 index;
	s32 shift;

	index = mailId / 32;
	shift = mailId % 32;

	count = 0;
	for (i = 0; i < 99; i++) {
		if (mpp->mEmailReceivedFlags[i / 32] & (1 << (i % 32))) {
			count++;
		}
	}
	mpp->mEmailReceivedFlags[index] |= (1 << shift);
	mpp->mEmailIds[count] = (u8)mailId;
#endif
}

void pouchOpenMail(s32 mailId) {
	mpp->mEmailReadFlags[mailId / 32] |= (1 << (mailId % 32));
}

u32 pouchCheckMail(s32 mailId) {
	s32 mask = (1 << (mailId % 32));
	if (mpp->mEmailReadFlags[mailId / 32] & mask) {
		return 2;
	}
	else {
		return (mpp->mEmailReceivedFlags[mailId / 32] & mask) != 0;
	}
}

s32 pouchGetPartyColor(PartyId partnerId) {
	return mpp->partyData[partnerId].flags >> 13;
}

void pouchSetPartyColor(PartyId partnerId, u16 color) {
	mpp->partyData[partnerId].flags = (u16)((mpp->partyData[partnerId].flags & 0x1FFF) | ((color & 7) << 13));
}

void pouchSetYoshiName(char* name) {
	strcpy(mpp->yoshiName, name);
}

char* pouchGetYoshiName(void) {
	if (strlen(mpp->yoshiName) < 1) {
		return (char*)msgSearch("name_party3");
	}
	return mpp->yoshiName;
}
