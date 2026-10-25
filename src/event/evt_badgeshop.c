/* "evt_badgeshop" - Badge Shop Script Functions
 * Status: 1:1
 *
 * Function: part of the backend for various shops
 */
#include "evt/evt_badgeshop.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "system.h"
#include <string.h>

//.data
s32 badge_starmaniac_table[] = { //Dazzle
	ITEM_ATTACK_FX_Y, ITEM_CHILL_OUT, ITEM_PRETTY_LUCKY, ITEM_HAPPY_HEART,
	ITEM_HAPPY_FLOWER, ITEM_HAPPY_HEART_P, ITEM_ITEM_HOG, ITEM_HEART_FINDER,
	ITEM_FLOWER_FINDER, ITEM_PEEKABOO, ITEM_QUICK_CHANGE, ITEM_FLOWER_SAVER,
	ITEM_FLOWER_SAVER_P, ITEM_POWER_PLUS, ITEM_POWER_PLUS_P, ITEM_NULL
};

s32 badge_bottakuru_table[] = { //Charlieton Rogueport
	ITEM_SHOOTING_STAR, ITEM_EARTH_QUAKE, ITEM_BOOS_SHEET, ITEM_REPEL_CAPE,
	ITEM_STOPWATCH, ITEM_ULTRA_SHROOM, ITEM_JAMMIN_JELLY, ITEM_DRIED_SHROOM,
	ITEM_DOUBLE_PAIN, ITEM_JUMPMAN, ITEM_HAMMERMAN, ITEM_ATTACK_FX_G,
	ITEM_SLOW_GO, ITEM_MEGA_RUSH, ITEM_TIMING_TUTOR, ITEM_W_EMBLEM, ITEM_NULL
};

//.sdata
s32 badge_special_table[] = { //Lovely Howz of Badges
	ITEM_PIERCING_BLOW, ITEM_POWER_JUMP, ITEM_SIMPLIFIER, ITEM_UNSIMPLIFIER,
	ITEM_FP_DRAIN, ITEM_LAST_STAND, ITEM_LAST_STAND_P, ITEM_HAPPY_FLOWER,
	ITEM_SUPER_APPEAL, ITEM_NULL
};

s32 badge_special_table_stage_1_clear[] = { //Hooktail Castle
	ITEM_CLOSE_CALL, ITEM_CLOSE_CALL_P, ITEM_SLEEPY_STOMP, ITEM_NULL
};

s32 badge_special_table_stage_2_clear[] = { //Boggly Tree
	ITEM_FIRST_ATTACK, ITEM_PRETTY_LUCKY_P, ITEM_POWER_RUSH, ITEM_NULL
};

s32 badge_special_table_stage_3_clear[] = { //Glitz Pit
	ITEM_SHRINK_STOMP, ITEM_ICE_SMASH, ITEM_POWER_RUSH_P, ITEM_NULL
};

s32 badge_special_table_stage_4_clear[] = { //Creepy Steeple
	ITEM_SOFT_STOMP, ITEM_HEAD_RATTLE, ITEM_DAMAGE_DODGE, ITEM_NULL
};

s32 badge_special_table_stage_5_clear[] = { //Keelhaul Key
	ITEM_SUPER_APPEAL_P, ITEM_DAMAGE_DODGE_P, ITEM_FIRE_DRIVE, ITEM_NULL
};

s32 badge_special_table_stage_6_clear[] = { //Poshley Heights
	ITEM_SIMPLIFIER, ITEM_UNSIMPLIFIER, ITEM_ATTACK_FX_P, ITEM_NULL
};

s32 badge_bottakuru100_table[] = { //Charlieton Pit of 100 Trials
	ITEM_MUSHROOM, ITEM_SUPER_SHROOM, ITEM_HONEY_SYRUP, ITEM_MAPLE_SYRUP,
	ITEM_FIRE_FLOWER, ITEM_THUNDER_RAGE, ITEM_NULL
};

s32 badge_bteresa_table[] = { //Pianta Parlor
	ITEM_SUPER_SHROOM, ITEM_CAKE_MIX, ITEM_SUPER_APPEAL, ITEM_REFUND, ITEM_NULL
};

s32 badge_bteresa_table_card_special[] = { //Pianta Parlor Special Card
	ITEM_MAPLE_SYRUP, ITEM_POWER_JUMP, ITEM_HP_PLUS, ITEM_FP_PLUS, ITEM_NULL
};

s32 badge_bteresa_table_card_silver[] = { //Pianta Parlor Silver Card
	ITEM_GOLD_BAR_X3, ITEM_POWER_SMASH, ITEM_HP_PLUS_P, ITEM_NULL
};

s32 badge_bteresa_table_card_gold[] = { //Pianta Parlor Gold Card
	ITEM_HAMMER_THROW, ITEM_POWER_RUSH, ITEM_TORNADO_JUMP, ITEM_MULTIBOUNCE,
	ITEM_POWER_RUSH_P, ITEM_NULL
};

s32 badge_bteresa_table_card_platinum[] = { //Pianta Parlor Platinum Card
	ITEM_QUAKE_HAMMER, ITEM_MONEY_MONEY, ITEM_ULTRA_SHROOM, ITEM_JAMMIN_JELLY, ITEM_NULL
};

//.bss
BadgeShopWork badgeshopwork;

//.sbss
BadgeShopWork* bdsw;

//local prototypes

s32 getBadgeBottakuru100TableMaxCount(void) {
	return (sizeof(badge_bottakuru100_table) / sizeof(s32)) - 1;
}

s32 getBadgeStarmaniacTableMaxCount(void) {
	return (sizeof(badge_starmaniac_table) / sizeof(s32)) - 1;
}

s32 getBadgeBteresaTableMaxCount(void) {
	return ((sizeof(badge_bteresa_table) / sizeof(s32)) - 1) +
		((sizeof(badge_bteresa_table_card_special) / sizeof(s32)) - 1) +
		((sizeof(badge_bteresa_table_card_silver) / sizeof(s32)) - 1) +
		((sizeof(badge_bteresa_table_card_gold) / sizeof(s32)) - 1) +
		((sizeof(badge_bteresa_table_card_platinum) / sizeof(s32)) - 1);
}

void badgeShop_init(void) {
	s32* ptr;

	bdsw = &badgeshopwork;
	memset(bdsw->special_table, 0, sizeof(bdsw->special_table));
	memset(bdsw->throw_table, 0, sizeof(bdsw->throw_table));
	memset(bdsw->starmaniac_table, 0, sizeof(bdsw->starmaniac_table));
	memset(bdsw->bottakuru_table, 0, sizeof(bdsw->bottakuru_table));
	memset(bdsw->bteresa_table, 0, sizeof(bdsw->bteresa_table));
	bdsw->field_0x114 = 1;

	for (ptr = badge_special_table; *ptr; ++ptr) {
		badgeShop_add(bdsw->special_table, *ptr, 1);
	}
	for (ptr = badge_starmaniac_table; *ptr; ++ptr) {
		badgeShop_add(bdsw->starmaniac_table, *ptr, 1);
	}
	for (ptr = badge_bottakuru_table; *ptr; ++ptr) {
		badgeShop_add(bdsw->bottakuru_table, *ptr, 1);
	}

	badgeShop_bottakuruGeneration();

	for (ptr = badge_bteresa_table; *ptr; ++ptr) {
		badgeShop_add(bdsw->bteresa_table, *ptr, 1);
	}
}

int bottakuruComp(const void* sortA, const void* sortB) {
	s32 a, b;

	a = ((const s32*)sortA)[1];
	b = ((const s32*)sortB)[1];
	if (a < b) {
		return -1;
	}
	return a > b;
}

void badgeShop_bottakuruGeneration(void) {
	// {item id, random rank} pairs; qqsort orders them by rank (second field). `n` is the fill count,
	// then the index into badge_bottakuru_table.
	s32 entries[0x11][2];
	s32 (*base)[2] = entries;
	s32 j;
	s32 i;
	u32 n;
	s32 count;
	for (i = 0; i < sizeof(entries) / sizeof(entries[0]); i++) {
		base = &entries[i];
		(*base)[1] = 200;
		(*base)[0] = 0;
	}

	base = entries;
	n = 0;
	for (i = 0; i < BADGE_MAX - KEY_ITEM_MIN; i++) {
		count = badgeShop_get(bdsw->bottakuru_table, (s16)(i + KEY_ITEM_MIN)); //inline
		for (j = 0; j < count; j++) {
			entries[n][1] = irand(100);
			entries[n++][0] = i + KEY_ITEM_MIN;
			if (n >= 0x10) break;
		}
		if (n >= 0x10) break;
	}

	qqsort(entries, 0x11, 8, bottakuruComp);

	for (i = 0; i < 4; i++, base++) { //only 4 slots
		for (n = 0; n < 0x11; n++) {
			if ((*base)[0] == badge_bottakuru_table[n]) {
				evtSetValue(NULL, i + GSW(118), n);
			}
		}
	}
}

void badgeShop_bteresaGeneration(void) {
	s32* ptr;

	if (pouchCheckItem(ITEM_SPECIAL_CARD) != 0) {
		if (!evtGetValue(NULL, GSWF(246))) {
			evtSetValue(NULL, GSWF(246), 1);
			for (ptr = badge_bteresa_table_card_special; *ptr; ptr++) {
				badgeShop_add(bdsw->bteresa_table, *ptr, 1);
			}
		}
	}

	if (pouchCheckItem(ITEM_SILVER_CARD) != 0) {
		if (!evtGetValue(NULL, GSWF(247))) {
			evtSetValue(NULL, GSWF(247), 1);
			for (ptr = badge_bteresa_table_card_silver; *ptr; ptr++) {
				badgeShop_add(bdsw->bteresa_table, *ptr, 1);
			}
		}
	}

	if (pouchCheckItem(ITEM_GOLD_CARD) != 0) {
		if (!evtGetValue(NULL, GSWF(248))) {
			evtSetValue(NULL, GSWF(248), 1);
			for (ptr = badge_bteresa_table_card_gold; *ptr; ptr++) {
				badgeShop_add(bdsw->bteresa_table, *ptr, 1);
			}
		}
	}

	if (pouchCheckItem(ITEM_PLATINUM_CARD) != 0) {
		if (!evtGetValue(NULL, GSWF(249))) {
			evtSetValue(NULL, GSWF(249), 1);
			for (ptr = badge_bteresa_table_card_platinum; *ptr; ptr++) {
				badgeShop_add(bdsw->bteresa_table, *ptr, 1);
			}
		}
	}
}

void badgeShop_bargainGeneration(void) {
	s16 slotValue;
	s16 pickIndex;
	s32 pickTarget;
	int i;
	u8 remaining[0x19];
	s32 count2;
	s16 presentIndex;
	s32 totalBadges;
	s32 bit2Count;
	s16 badge;
	s32* ptr;

	if (evtGetValue(NULL, GSW(0)) >= 55 && !evtGetValue(0, GSWF(238))) {
		evtSetValue(NULL, GSWF(238), 1);
		for (ptr = badge_special_table_stage_1_clear; *ptr; ptr++) {
			badgeShop_add(bdsw->special_table, *ptr, 1);
		}
	}

	if (evtGetValue(NULL, GSW(0)) >= 112 && !evtGetValue(0, GSWF(239))) {
		evtSetValue(NULL, GSWF(239), 1);
		for (ptr = badge_special_table_stage_2_clear; *ptr; ptr++) {
			badgeShop_add(bdsw->special_table, *ptr, 1);
		}
	}

	if (evtGetValue(NULL, GSW(0)) >= 164 && !evtGetValue(0, GSWF(240))) {
		evtSetValue(NULL, GSWF(240), 1);
		for (ptr = badge_special_table_stage_3_clear; *ptr; ptr++) {
			badgeShop_add(bdsw->special_table, *ptr, 1);
		}
	}

	if (evtGetValue(NULL, GSW(0)) >= 212 && !evtGetValue(0, GSWF(241))) {
		evtSetValue(NULL, GSWF(241), 1);
		for (ptr = badge_special_table_stage_4_clear; *ptr; ptr++) {
			badgeShop_add(bdsw->special_table, *ptr, 1);
		}
	}

	if (evtGetValue(NULL, GSW(0)) >= 253 && !evtGetValue(0, GSWF(242))) {
		evtSetValue(NULL, GSWF(242), 1);
		for (ptr = badge_special_table_stage_5_clear; *ptr; ptr++) {
			badgeShop_add(bdsw->special_table, *ptr, 1);
		}
	}

	if (evtGetValue(NULL, GSW(0)) >= 338 && !evtGetValue(0, GSWF(243))) {
		evtSetValue(NULL, GSWF(243), 1);
		for (ptr = badge_special_table_stage_6_clear; *ptr; ptr++) {
			badgeShop_add(bdsw->special_table, *ptr, 1);
		}
	}

	if (evtGetValue(NULL, GSW(0)) >= 373 && !evtGetValue(0, GSWF(244))) {
		evtSetValue(NULL, GSWF(244), 1);
	}

	if (evtGetValue(NULL, GSW(0)) >= 401 && !evtGetValue(0, GSWF(245))) {
		evtSetValue(NULL, GSWF(245), 1);
	}

	// Copy the special table into a scratch bitmap, then pull up to five distinct
	// badges out of it into bargainTable (the bargain slots).
	memset(remaining, 0, sizeof(remaining));
	bdsw->field_0x110 = bdsw->field_0x10C = bdsw->bargainTable[4] = bdsw->bargainTable[3] =
		bdsw->bargainTable[2] = bdsw->bargainTable[1] = bdsw->bargainTable[0] = i = 0;

	bit2Count = 0;
	totalBadges = 0;
	count2 = 0;
	for (; i < 99; i++) {
		slotValue = 0;
		badge = badgeShop_get(bdsw->special_table, (s16)(i + BADGE_MIN)); //inline
		if (badge) {
			totalBadges++;
			slotValue |= 1;
		}
		if (slotValue) {
			badgeShop_set(remaining, (s16)(i + BADGE_MIN), slotValue);
			count2++;
		}
	}
	pickTarget = 5;
	if (count2 <= 5) {
		pickTarget = count2; //less than 5 needed
	}
	irand(0x100); //unused
	while ((s32)(bdsw->field_0x10C + bdsw->field_0x110) < pickTarget) {
		if (bdsw->field_0x10C < 3 || !bit2Count) {
			pickIndex = irand(totalBadges);
			presentIndex = 0;
			for (i = 0; i < 99; i++) {
				badge = badgeShop_get(remaining, (s16)(i + BADGE_MIN)); //inline
				if (badge & 1) {
					if (presentIndex == pickIndex) {
						break;
					}
					presentIndex++;
				}
			}
			if (i < 99) {
				bdsw->bargainTable[bdsw->field_0x10C] = i + BADGE_MIN;
				bdsw->field_0x10C++;
				badgeShop_set(remaining, (s16)(i + BADGE_MIN), 0);
				if (badge & 1) {
					totalBadges--;
				}
				if (badge & 2) {
					bit2Count--;
				}
			}
		}
	}
	for (i = 0; i < sizeof(bdsw->special_table); i++) {
		bdsw->throw_table[i] = bdsw->special_table[i];
	}
	for (i = 0; i < 5; i++) {
		if (bdsw->bargainTable[i]) {
			badgeShop_add(bdsw->throw_table, bdsw->bargainTable[i], -1);
		}
	}
}

// The target never inlines `badgeShop_set` (both callers emit a real `bl`), but it always
// inlines the smaller `badgeShop_get`.
s16 badgeShop_set(u8* table, s16 id, s16 val) {
	s32 bound;
	s16 index;
	u8 mask;
	s32 shift;
	s32 byteIndex;

	if (table == bdsw->bottakuru_table || table == bdsw->bteresa_table) {
		bound = KEY_ITEM_MIN;
	}
	else {
		bound = BADGE_MIN;
	}
	if (id < bound || id >= BADGE_MAX) return 0;
	index = (s16)(id - bound);
	switch (index & 3) {
		case 0:
			mask = (u8)~(3 << 0);
			shift = 0;
			break;
		case 1:
			mask = (u8)~(3 << 2);
			shift = 2;
			break;
		case 2:
			mask = (u8)~(3 << 4);
			shift = 4;
			break;
		case 3:
			mask = (u8)~(3 << 6);
			shift = 6;
			break;
	}
	byteIndex = index / 4;
	if (val < 0) {
		val = 0;
	}
	else if (val > 3) {
		val = 3;
	}
	table[byteIndex] = (u8)((u8)(mask & table[byteIndex]) + (u8)(val << shift));
	return val;
}

s16 badgeShop_get(u8* table, s16 id) { //1:1
	s32 bound;
	s16 index;
	u8 mask;
	s32 shift;

	if (table == bdsw->bottakuru_table || table == bdsw->bteresa_table) {
		bound = KEY_ITEM_MIN;
	}
	else {
		bound = BADGE_MIN;
	}
	if (id < bound || id >= BADGE_MAX) return 0;
	index = (s16)(id - bound);
	switch (index & 3) {
		case 0:
			mask = 3 << 0;
			shift = 0;
			break;
		case 1:
			mask = 3 << 2;
			shift = 2;
			break;
		case 2:
			mask = 3 << 4;
			shift = 4;
			break;
		case 3:
			mask = 3 << 6;
			shift = 6;
			break;
	}
	return (mask & table[index / 4]) >> shift;
}

s32 badgeShop_add(u8* table, s16 id, s32 value) { // 1:1, can blow up in your face
    s32 itemId;
	s32 mask;
	s32 shift;
    s32 bound;
    int currentValue;
    int newValue;

    if (table == bdsw->bottakuru_table || table == bdsw->bteresa_table) {
        bound = KEY_ITEM_MIN;
    } else {
        bound = BADGE_MIN;
    }
    itemId = id;
    if (itemId < bound || itemId >= BADGE_MAX) return 0;

    id = itemId - bound;
    switch (id & 3) {
        case 0:
            mask = 3 << 0;
            shift = 0;
            break;
        case 1:
            mask = 3 << 2;
            shift = 2;
            break;
        case 2:
            mask = 3 << 4;
            shift = 4;
            break;
        case 3:
            mask = 3 << 6;
            shift = 6;
            break;
    }
    newValue = value;
    mask &= 0xFF;
    currentValue = (mask & table[id / 4]) >> shift;
    newValue = (s16)((s16)currentValue + newValue);
    if (newValue < 0) {
        newValue = 0;
    }
    else if (newValue > 3) {
        newValue = 3;
    }
    table[id / 4] = (u8)(table[id / 4] & ~mask) + (u8)((s16)newValue << shift);
    return newValue;
}

USER_FUNC(badgeShop_getBargainTable) {
	evtSetValue(event, *event->args, (s32)&bdsw->bargainTable);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_badgeShop_throw_inc) {
	s32 value;

	value = evtGetValue(event, *event->args);
	badgeShop_add(bdsw->throw_table, value, 1);
	badgeShop_add(bdsw->special_table, value, 1);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_badgeShop_throw_dec) {
	s32 value;

	value = evtGetValue(event, *event->args);
	badgeShop_add(bdsw->throw_table, value, -1);
	badgeShop_add(bdsw->special_table, value, -1);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_badgeShop_special_dec) {
	s32 value;

	value = evtGetValue(event, *event->args);
	badgeShop_add(bdsw->special_table, value, -1);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_badgeShop_starmaniac_dec) {
	s32 value;

	value = (s16)evtGetValue(event, *event->args);
	badgeShop_add(bdsw->starmaniac_table, value, -1);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_badgeShop_bottakuru_dec) {
	s32 value;
	s32 i;

	value = evtGetValue(event, *event->args);
	badgeShop_add(bdsw->bottakuru_table, value, -1);
	for (i = 0; i < 4; i++) {
		if (value == badge_bottakuru_table[evtGetValue(event, GSW(118 + i))]) {
			evtSetValue(event, GSW(118 + i), (sizeof(badge_bottakuru_table) / sizeof(s32)) - 1); //last entry aka kNullItem
		}
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_badgeShop_throw_get_kind_cnt) {
	s16 badge;
	s32 count;
	s32* args = event->args;
	int i;

	for (count = 0, i = 0; i < 99; i++) {
		badge = badgeShop_get(bdsw->throw_table, (s16)(i + BADGE_MIN)); //inline
		if (badge > 0) {
			count++;
		}
	}
	evtSetValue(event, *args, count);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_badgeShop_starmaniac_get_kind_cnt) {
	s16 badge;
	s32 count;
	s32* args = event->args;
	int i;

	for (count = 0, i = 0; i < 99; i++) {
		badge = badgeShop_get(bdsw->starmaniac_table, (s16)(i + BADGE_MIN)); //inline
		if (badge > 0) {
			count++;
		}
	}
	evtSetValue(event, *args, count);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_badgeShop_bottakuru_get_kind_cnt) {
	s32* args = event->args;
	s32 count;
	int i;

	for (count = 0, i = 0; i < 4; i++) {
		if (badge_bottakuru_table[evtGetValue(event, i + GSW(118))]) {
			count++;
		}
	}
	evtSetValue(event, *args, count);
	return EVT_RETURN_DONE;
}

USER_FUNC(U_evt_badgeShop_get_special_zaiko) {
	s16 badge;
	s32 count;
	s32* args = event->args;
	int i;

	for (count = 0, i = 0; i < BADGE_MAX - KEY_ITEM_MIN; i++) {
		badge = badgeShop_get(bdsw->bottakuru_table, (s16)(i + KEY_ITEM_MIN));
		if (badge <= 0) {
			continue;
		}
		count++;
	}
	evtSetValue(event, *args, count);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_badgeShop_bteresa_get_kind_cnt) {
	s16 badge;
	s32 count;
	s32* args = event->args;
	int i;

	for (count = 0, i = 0; i < BADGE_MAX - KEY_ITEM_MIN; i++) {
		badge = badgeShop_get(bdsw->bteresa_table, (s16)(i + KEY_ITEM_MIN));
		if (badge > 0) {
			count++;
		}
	}
	evtSetValue(event, *args, count);
	return EVT_RETURN_DONE;
}

s32 badgeShop_ThrowCheck(s32 itemId) {
	int i;
	s32 count;

	for (count = 0, i = 0; i < bdsw->field_0x10C; i++) {
		if (itemId == bdsw->bargainTable[i + bdsw->field_0x10C]) {
			count++;
		}
	}
	return count;
}

s32 U_badgeShop_SpecialCheck(void) {
	s32 count;
	int i;

	for (count = 0, i = 0; i < 5; i++) {
		if (bdsw->bargainTable[i]) {
			count++;
		}
	}
	return count;
}
