#pragma once

#include "evt/evt_cmd.h"

typedef void (*voidfunctionptr)(void);
extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

USER_FUNC(bress);
EVT_DEF(evt_bress);
EVT_DEF(bom_00_init_evt);
EVT_DEF(bom_03_init_evt);
EVT_DEF(bom_04_init_evt);
