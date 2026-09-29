#pragma once

#include <dolphin/types.h>

// The steal-item functions of the borodo units read itemDataTable[i].icon_id with `lhz` (unsigned), while ItemData
// declares it as s16 (`lha`), and the DOL functions that use the field are scored with the signed type. This view of
// the same table has the field unsigned; include it instead of data/item_data.h in those units. Fix ItemData.icon_id
// (and btlUnitItemDisp, whose dead `(f32)BtlUnit_GetHeight` statement stops compiling the way the target does with
// an unsigned field) and this header can go.
typedef struct ItemIconView {
    u8 pad_0x0[0x20];
    u16 icon_id; // 0x20
    u8 pad_0x22[0x28 - 0x22];
} ItemIconView;

extern ItemIconView itemDataTable[];
