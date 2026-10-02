/* "evt_badgeshop" - Badge Shop Script Functions
 * Status: Complete, not 1:1 (see comments throughout files, mostly
 * regalloc, along with bargainGeneration cleanup
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
	memset(bdsw->bargain_table, 0, sizeof(bdsw->bargain_table));
	memset(bdsw->starmaniac_table, 0, sizeof(bdsw->starmaniac_table));
	memset(bdsw->bottakuru_table, 0, sizeof(bdsw->bottakuru_table));
	memset(bdsw->bteresa_table, 0, sizeof(bdsw->bteresa_table));
	bdsw->field_0x114 = 1;

	for (ptr = badge_special_table; *ptr; ++ptr) {
		badgeShop_add(bdsw->special_table, (s16)*ptr, 1);
	}
	for (ptr = badge_starmaniac_table; *ptr; ++ptr) {
		badgeShop_add(bdsw->starmaniac_table, (s16)*ptr, 1);
	}
	for (ptr = badge_bottakuru_table; *ptr; ++ptr) {
		badgeShop_add(bdsw->bottakuru_table, (s16)*ptr, 1);
	}

	badgeShop_bottakuruGeneration();

	for (ptr = badge_bteresa_table; *ptr; ++ptr) {
		badgeShop_add(bdsw->bteresa_table, (s16)*ptr, 1);
	}
}

s32 bottakuruComp(const void** sortA, const void** sortB) {
	s32 a, b;

	a = (s32)sortA[1];
	b = (s32)sortB[1];
	if (a < b) {
		return -1;
	}
	return a > b;
}

// `s16 count`, `s32 j`, `u32 k`, `u32 dobreak`, `s32 i`, a `s32* p` row pointer for the
// `[1]` store and a separate `u32 m` counter for the last loop get most compares and the
// inline `badgeShop_get` (r30/r31) right. What is left: the target stores `[1]` through a
// pointer and `[0]` through base+scaled-index (`stwx r24, r28, r25`), so it keeps a row
// pointer AND an offset; ours strength-reduces `table[dobreak][0]` to a second pointer. The
// remaining diffs are register allocation (target r21-r31, ours r22-r31).
// NONMATCHING (96.89%): target keeps base+scaled-index for the `[0]` store; regalloc.
void badgeShop_bottakuruGeneration(void) {
	s32 table[0x11][2];
	s16 count;
	s32 j;
	u32 k;
	s32* bp;
	s32* p;
	u32 dobreak;
	s32 i;
	u32 m;
	for (k = 0; k < 0x11; k++) {
		table[k][1] = 200;
		table[k][0] = 0;
	}

	dobreak = 0;
	p = (s32*)table;
	for (i = 0; i < BADGE_MAX - KEY_ITEM_MIN; i++) {
		count = badgeShop_get(bdsw->bottakuru_table, (s16)(i + KEY_ITEM_MIN)); //inline
		for (j = 0; j < count; j++) {
			p[1] = irand(100);
			table[dobreak][0] = i + KEY_ITEM_MIN;
			dobreak += 1;
			p += 2;
			if (dobreak >= 0x10) break;
		}
		if (dobreak >= 0x10) break;
	}

	qqsort(table, 0x11, 8, (int (*)(void*, void*))bottakuruComp);

	for (i = 0; i < 4; i++) { //only 4 slots
		bp = badge_bottakuru_table;
		for (m = 0; m < 0x11; m++) {
			if (table[i][0] == *bp) {
				evtSetValue(NULL, i + GSW(118), m);
			}
			bp++;
		}
	}
}

void badgeShop_bteresaGeneration(void) {
	s32* ptr;

	if ((s32)pouchCheckItem(ITEM_SPECIAL_CARD) != 0) {
		if (!evtGetValue(NULL, GSWF(246))) {
			evtSetValue(NULL, GSWF(246), 1);
			for (ptr = badge_bteresa_table_card_special; *ptr; ptr++) {
				badgeShop_add(bdsw->bteresa_table, (s16)*ptr, 1);
			}
		}
	}

	if ((s32)pouchCheckItem(ITEM_SILVER_CARD) != 0) {
		if (!evtGetValue(NULL, GSWF(247))) {
			evtSetValue(NULL, GSWF(247), 1);
			for (ptr = badge_bteresa_table_card_silver; *ptr; ptr++) {
				badgeShop_add(bdsw->bteresa_table, (s16)*ptr, 1);
			}
		}
	}

	if ((s32)pouchCheckItem(ITEM_GOLD_CARD) != 0) {
		if (!evtGetValue(NULL, GSWF(248))) {
			evtSetValue(NULL, GSWF(248), 1);
			for (ptr = badge_bteresa_table_card_gold; *ptr; ptr++) {
				badgeShop_add(bdsw->bteresa_table, (s16)*ptr, 1);
			}
		}
	}

	if ((s32)pouchCheckItem(ITEM_PLATINUM_CARD) != 0) {
		if (!evtGetValue(NULL, GSWF(249))) {
			evtSetValue(NULL, GSWF(249), 1);
			for (ptr = badge_bteresa_table_card_platinum; *ptr; ptr++) {
				badgeShop_add(bdsw->bteresa_table, (s16)*ptr, 1);
			}
		}
	}
}

// `badge`/`value` are `s16`, `count4`/`stopcount` s16, the first loop's `value |= 1` (the
// target's `ori r0,r5,1`, not `li`) and dropping the `v28` local (re-reading
// `bdsw->field_0x10C`) each recovered a little; writing the bargain loop as a real
// `while (bdsw->field_0x10C + bdsw->field_0x110 < count3)` (not `while(1){if(...)break;}`)
// reproduces the target's rotated `b <cond>` loop layout and was the big win. What remains is
// structural: we keep one extra callee-saved register (stmw r23 vs the target's r24), the
// inlined `badgeShop_get(bdsw->special_table, ...)` keeps the bottakuru compare the target
// folds, and the compiler hoists the second loop's `count4 = 0` / `mtctr` setup above the first
// loop; the rest is register allocation.
// NONMATCHING (99.28%): the field-init stores share the loop counter's zero register in target
// (`li r25,0`; `stw r25,0xf8..`), ours materialises a fresh `r0` for the two `field_0x110`
// stores; one `extsh r4,r0` is one slot later than target.
void badgeShop_bargainGeneration(void) { //finished, needs cleanup and probably fixes
	s16 value;
	s32 count3;
	int i;
	u8 table[0x19];
	s16 stopcount;
	s32 count;
	s32 v7;
	s32* ptr;
	s32 count2;
	s16 count4;
	s32 badge;

	if (evtGetValue(NULL, GSW(0)) >= 55 && !evtGetValue(0, GSWF(238))) {
		evtSetValue(NULL, GSWF(238), 1);
		for (ptr = badge_special_table_stage_1_clear; *ptr; ptr++) {
			badgeShop_add(bdsw->special_table, (s16)*ptr, 1);
		}
	}

	if (evtGetValue(NULL, GSW(0)) >= 112 && !evtGetValue(0, GSWF(239))) {
		evtSetValue(NULL, GSWF(239), 1);
		for (ptr = badge_special_table_stage_2_clear; *ptr; ptr++) {
			badgeShop_add(bdsw->special_table, (s16)*ptr, 1);
		}
	}

	if (evtGetValue(NULL, GSW(0)) >= 164 && !evtGetValue(0, GSWF(240))) {
		evtSetValue(NULL, GSWF(240), 1);
		for (ptr = badge_special_table_stage_3_clear; *ptr; ptr++) {
			badgeShop_add(bdsw->special_table, (s16)*ptr, 1);
		}
	}

	if (evtGetValue(NULL, GSW(0)) >= 212 && !evtGetValue(0, GSWF(241))) {
		evtSetValue(NULL, GSWF(241), 1);
		for (ptr = badge_special_table_stage_4_clear; *ptr; ptr++) {
			badgeShop_add(bdsw->special_table, (s16)*ptr, 1);
		}
	}

	if (evtGetValue(NULL, GSW(0)) >= 253 && !evtGetValue(0, GSWF(242))) {
		evtSetValue(NULL, GSWF(242), 1);
		for (ptr = badge_special_table_stage_5_clear; *ptr; ptr++) {
			badgeShop_add(bdsw->special_table, (s16)*ptr, 1);
		}
	}

	if (evtGetValue(NULL, GSW(0)) >= 338 && !evtGetValue(0, GSWF(243))) {
		evtSetValue(NULL, GSWF(243), 1);
		for (ptr = badge_special_table_stage_6_clear; *ptr; ptr++) {
			badgeShop_add(bdsw->special_table, (s16)*ptr, 1);
		}
	}

	if (evtGetValue(NULL, GSW(0)) >= 373 && !evtGetValue(0, GSWF(244))) {
		evtSetValue(NULL, GSWF(244), 1);
	}

	if (evtGetValue(NULL, GSW(0)) >= 401 && !evtGetValue(0, GSWF(245))) {
		evtSetValue(NULL, GSWF(245), 1);
	}

	memset(table, 0, sizeof(table));
#ifdef MUST_MATCH
	/* The target's field-init stores reuse the loop counter's zero register
	   (r25) rather than a fresh constant; MWCC only does that when the store
	   value is `i` while `i` is not yet a known constant (its `for` init is
	   hoisted to just before the stores). Natural form under #else. */
	bdsw->field_0xF8[0] = i;
	bdsw->field_0xF8[1] = i;
	bdsw->field_0xF8[2] = i;
	bdsw->field_0xF8[3] = i;
	bdsw->field_0xF8[4] = i;
	bdsw->field_0x10C = i;
	bdsw->field_0x110 = 0;
#else
	bdsw->field_0xF8[0] = ITEM_NULL;
	bdsw->field_0xF8[1] = ITEM_NULL;
	bdsw->field_0xF8[2] = ITEM_NULL;
	bdsw->field_0xF8[3] = ITEM_NULL;
	bdsw->field_0xF8[4] = ITEM_NULL;
	bdsw->field_0x10C = 0;
	bdsw->field_0x110 = 0;
#endif

	v7 = 0;
	count = 0;
	count2 = 0;
	for (i = 0; i < 99; i++) {
		value = 0;
		badge = (s16)badgeShop_get(bdsw->special_table, (s16)(i + BADGE_MIN)); //inline
		if ((s16)badge) {
			count++;
			value |= 1;
		}
		if (value) {
			badgeShop_set(table, (s16)(i + BADGE_MIN), value);
			count2++;
		}
	}
	count3 = 5;
	if (count2 <= 5) {
		count3 = count2; //less than 5 needed
	}
	irand(0x100); //unused
	while ((s32)(bdsw->field_0x10C + bdsw->field_0x110) < count3) {
		if ((s32)bdsw->field_0x10C < 3 || !v7) {
			stopcount = irand(count);
			count4 = 0;
			for (i = 0; i < 99; i++) {
				badge = badgeShop_get(table, (s16)(i + BADGE_MIN)); //inline
				if (badge & 1) {
					if (count4 == stopcount) {
						break;
					}
					count4++;
				}
			}
			if (i < 99) {
				bdsw->field_0xF8[bdsw->field_0x10C] = i + BADGE_MIN;
				bdsw->field_0x10C++;
				badgeShop_set(table, (s16)(i + BADGE_MIN), 0);
				if ((s16)badge & 1) {
					count--;
				}
				if ((s16)badge & 2) {
					v7--;
				}
			}
		}
	}
	for (i = 0; i < sizeof(bdsw->special_table); i++) {
		bdsw->bargain_table[i] = bdsw->special_table[i];
	}
	for (i = 0; i < 5; i++) {
		badge = bdsw->field_0xF8[i];
		if (badge) {
			badgeShop_add(bdsw->bargain_table, (s16)badge, -1);
		}
	}
}

// Target reuses r6 for `index / 4` (`addze r6, r0`); our separate `word` temp gets r9
// because the allocator treats `bound` (also r6) as still live, so the load/store index and
// the val sign-extend register differ. Declaration order permutations, opt_lifetimes/
// opt_common_subs/opt_propagation and inline forms do not move it (inline `table[index/4]`
// drops the val clamp instead).
// The target never inlines `badgeShop_set` (both callers emit a real `bl`), but it always
// inlines the smaller `badgeShop_get`.
// NONMATCHING (99.84%): the only diff is the sign-extend of `val` for the shift
// (`extsh r0,r5` target vs `extsh r5,r5` ours); everything else, including `word` in r6
// (the dead `bound` register) and `val` in r5, now matches.
s32 badgeShop_set(u8* table, s16 id, s16 val) {
	s32 bound;
	s16 index;
	u8 mask;
	s32 shift;
	s32 word;

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
	word = index / 4;
	if (val < 0) {
		val = 0;
	}
	else if (val > 3) {
		val = 3;
	}
	table[word] = (u8)((u8)(mask & table[word]) + (u8)(val << shift));
#ifdef MUST_MATCH
	// `+ word - word` folds to zero but keeps `word` live until the return; without it the
	// allocator lets `word` die earlier and colours it r9 instead of the target's r6.
	return val + word - word;
#else
	return val;
#endif
}

s32 badgeShop_get(u8* table, s16 id) { //1:1
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
	return (s16)((mask & table[index / 4]) >> shift);
}

s32 badgeShop_add(u8* table, s32 id, s32 value) { // 1:1, can blow up in your face
    s32 temp;
	s32 mask;
	s32 shift;
    s32 bound;
    int curr;
    int data;

    if (table == bdsw->bottakuru_table || table == bdsw->bteresa_table) {
        bound = KEY_ITEM_MIN;
    } else {
        bound = BADGE_MIN;
    }
    temp = (s16)id;
    if (temp < bound || temp >= BADGE_MAX) return 0;

    id = (s16)(temp - bound);
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
    data = value;
    mask &= 0xFF;
    curr = (mask & table[id / 4]) >> shift;
    data = (s16)((s16)curr + data);
    if (data < 0) {
        data = 0;
    }
    else if (data > 3) {
        data = 3;
    }
    table[id / 4] = (u8)(table[id / 4] & ~mask) + (u8)((s16)data << shift);
    return data;
}

USER_FUNC(badgeShop_getBargainTable) {
	evtSetValue(event, *event->args, (s32)&bdsw->field_0xF8);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_badgeShop_throw_inc) {
	s32 value;

	value = evtGetValue(event, *event->args);
#ifdef MUST_MATCH
	// Original badgeShop_add takes its id as s16, not the header's s32: the cast
	// restores the per-call `extsh r4,r31` the target has (report notes the header).
	((s32 (*)(u8*, s16, s32))badgeShop_add)(bdsw->bargain_table, value, 1);
	((s32 (*)(u8*, s16, s32))badgeShop_add)(bdsw->special_table, value, 1);
#else
	badgeShop_add(bdsw->bargain_table, (s16)value, 1);
	badgeShop_add(bdsw->special_table, (s16)value, 1);
#endif
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_badgeShop_throw_dec) {
	s32 value;

	value = evtGetValue(event, *event->args);
#ifdef MUST_MATCH
	// See evt_badgeShop_throw_inc: the original badgeShop_add id is s16.
	((s32 (*)(u8*, s16, s32))badgeShop_add)(bdsw->bargain_table, value, -1);
	((s32 (*)(u8*, s16, s32))badgeShop_add)(bdsw->special_table, value, -1);
#else
	badgeShop_add(bdsw->bargain_table, (s16)value, -1);
	badgeShop_add(bdsw->special_table, (s16)value, -1);
#endif
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_badgeShop_special_dec) {
	s32 value;

	value = evtGetValue(event, *event->args);
	badgeShop_add(bdsw->special_table, (s16)value, -1);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_badgeShop_starmaniac_dec) {
	s32 value;

	value = (s16)evtGetValue(event, *event->args);
	badgeShop_add(bdsw->starmaniac_table, (s16)value, -1);
	return EVT_RETURN_DONE;
}

#pragma opt_common_subs off // without it the GSW(118+i) index is CSE'd and the target's fresh evtSetValue index is lost
USER_FUNC(evt_badgeShop_bottakuru_dec) {
	s32 value;
	int i;

	value = evtGetValue(event, *event->args);
#ifdef MUST_MATCH
	// Original badgeShop_add takes its id as s16 (see report); the cast restores the
	// target's `extsh r4,r30` with the raw result kept in r30.
	((s32 (*)(u8*, s16, s32))badgeShop_add)(bdsw->bottakuru_table, value, -1);
#else
	badgeShop_add(bdsw->bottakuru_table, (s16)value, -1);
#endif
	for (i = 0; i < 4; i++) {
		if (value == badge_bottakuru_table[evtGetValue(event, GSW(118 + i))]) {
			evtSetValue(event, GSW(118 + i), (sizeof(badge_bottakuru_table) / sizeof(s32)) - 1); //last entry aka kNullItem
		}
	}
	return EVT_RETURN_DONE;
}
#pragma opt_common_subs on

USER_FUNC(evt_badgeShop_throw_get_kind_cnt) {
	s16 badge;
	s32 count;
	s32* args = event->args;
	int i;

	for (count = 0, i = 0; i < 99; i++) {
		badge = badgeShop_get(bdsw->bargain_table, (s16)(i + BADGE_MIN)); //inline
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
#ifdef MUST_MATCH
		// `!(badge <= 0)` is `badge > 0`, but the target keeps the compiler's
		// cntlzw/rlwnm boolean form (see btlevtcmd_AnnounceMessage) rather than extsh./ble.
		if (!(badge <= 0)) {
#else
		if (badge > 0) {
#endif
			count++;
		}
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
		if (itemId == bdsw->field_0xF8[i + bdsw->field_0x10C]) {
			count++;
		}
	}
	return count;
}

s32 U_badgeShop_SpecialCheck(void) {
	s32 count;
	int i;

	for (count = 0, i = 0; i < 5; i++) {
		if (bdsw->field_0xF8[i]) {
			count++;
		}
	}
	return count;
}
