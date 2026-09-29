#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(evt_moamoa_set_seq_all);
USER_FUNC(evt_moamoa_set_one);
void moamoa_dispent(void);  // TODO: real signature
void moamoa_main(void);  // TODO: real signature
void moamoa_init(void);  // TODO: real signature
EVT_DEF(evt_rsh_moamoa_on_window);
EVT_DEF(evt_handle);
EVT_DEF(regu_unten);
EVT_DEF(rsh_00_init_evt);
EVT_DEF(rsh_00_a_init_evt);
EVT_DEF(rsh_00_b_init_evt);
EVT_DEF(rsh_00_c_init_evt);
EVT_DEF(rsh_00_init_evt_nidome);
EVT_DEF(rsh_00_a_init_evt_nidome);
EVT_DEF(rsh_00_b_init_evt_nidome);
EVT_DEF(rsh_00_c_init_evt_nidome);
