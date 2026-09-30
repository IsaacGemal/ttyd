#pragma once

#include "evt/evt_cmd.h"

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

EVT_DEF(dig_00_init_evt);

void _unresolved(void);  // TODO: real signature
void _epilog(void);  // TODO: real signature
void _prolog(void);  // TODO: real signature
