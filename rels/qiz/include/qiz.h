#pragma once

#include "evt/evt_cmd.h"

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

EVT_DEF(qiz_00_init_evt);

void _unresolved(void);
void _epilog(void);
void _prolog(void);
