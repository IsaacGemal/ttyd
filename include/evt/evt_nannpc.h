#pragma once

#include "evt/evt_cmd.h"

typedef struct ExtSortEntry ExtSortEntry;

#include <dolphin/types.h>
void nannpc_ext_dispent(void);
void nannpc_ext_main(void);
void nannpc_ext_main_sub(void);
void nannpc_ext_init(void);
void nannpc_ext_shadow_disp(void);
void nannpc_ext_disp(void);
s32 nannpc_zcompare(ExtSortEntry* a, ExtSortEntry* b);

USER_FUNC(evt_nannpc_entry);
USER_FUNC(evt_nannpc_flag_onoff);
USER_FUNC(evt_nannpc_get_position);
USER_FUNC(evt_nannpc_get_work);
USER_FUNC(evt_nannpc_init);
USER_FUNC(evt_nannpc_jump_position);
USER_FUNC(evt_nannpc_move_position2);
USER_FUNC(evt_nannpc_ppflag_onoff);
USER_FUNC(evt_nannpc_set_animtbl);
USER_FUNC(evt_nannpc_set_color);
USER_FUNC(evt_nannpc_set_dir);
USER_FUNC(evt_nannpc_set_func);
USER_FUNC(evt_nannpc_set_hosei_position);
USER_FUNC(evt_nannpc_set_position);
USER_FUNC(evt_nannpc_set_shadow_position);
USER_FUNC(evt_nannpc_set_size);
USER_FUNC(evt_nannpc_set_subfunc);
USER_FUNC(evt_nannpc_set_work);
USER_FUNC(evt_nannpcwork_flag_onoff);
