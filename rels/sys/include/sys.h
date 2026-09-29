#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(init);
USER_FUNC(bg);
USER_FUNC(start);
USER_FUNC(display);
void display_func(void);  // TODO: real signature
void getLightColor(void);  // TODO: real signature
void sys_01_release(void);  // TODO: real signature
void _prolog(void);  // TODO: real signature
EVT_DEF(dummy_evt);
EVT_DEF(sys_01_init_evt);
EVT_DEF(evt_start);
