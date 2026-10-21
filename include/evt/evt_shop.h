#pragma once

#include <dolphin/types.h>
#include "evt/evt_cmd.h"

typedef struct ShopObj {
	const char* name;    //0x0
	const char* hitName; //0x4
} ShopObj;

extern const char shop_map_rsh_03[];

USER_FUNC(evt_shop_setup);
s32 evtShopIsActive(void);
// Data objects defined in src/event/evt_shop.c (the scripts other scripts reference).
