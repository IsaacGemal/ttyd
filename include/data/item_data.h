#pragma once

#include <dolphin/types.h>
#include "battle/battle_database_common.h"

typedef struct ItemData {
	const char* id; //0x0, japanese name of the item
	const char* name; //0x4
	const char* description; //0x8
	const char* menu_description; //0xC
	u16 usable_locations; //0x10
	s16 type_sort_order; //0x12
	s16 buy_price; //0x14
	s16 discount_price; //0x16
	u16 star_piece_price; //0x18
	u16 sell_price; //0x1A
	s8 bp_cost; //0x1C
	u8 hp_restored; //0x1D
	u8 fp_restored; //0x1E
	u8 sp_restored; //0x1F
	s16 icon_id; //0x20
	s16 field_0x22; //0x22
	BattleWeapon* battle_params; //0x24
} ItemData;

extern ItemData itemDataTable[];
// winMgrSelectOther reads ItemData.buy_price with `lhz` (unsigned), while the field above is s16. This view has
// the field unsigned; use it for that one read and fix ItemData.buy_price (then delete this) when the header is
// corrected.
typedef struct ItemDataView {
	u8 pad_0x0[0x14];
	u16 buy_price; //0x14
	u8 pad_0x16[0x28 - 0x16];
} ItemDataView;
