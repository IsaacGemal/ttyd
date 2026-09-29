#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(dou_mario_pos_osreport_func);
USER_FUNC(evt_dou_normal_puka);
void _unresolved(void);  // TODO: real signature
void _epilog(void);  // TODO: real signature
void _prolog(void);  // TODO: real signature
EVT_DEF(dou_normal_puka);
EVT_DEF(dou_puka_event_sub);
EVT_DEF(dou_puka_event);
EVT_DEF(dou_mario_pos_osreport);
