#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/mtx.h>
#include <dolphin/types.h>

void togeMakeRetsu(Vec* pos, s16 a, s16 b, s16 c);
USER_FUNC(evt_puni_flag_onoff);
USER_FUNC(evt_puni_get_flag);
USER_FUNC(evt_puni_get_position);
USER_FUNC(evt_puni_mem_init);
USER_FUNC(evt_puni_set_anim);
USER_FUNC(evt_puni_set_dir);
USER_FUNC(evt_puni_set_hamari);
USER_FUNC(evt_puni_set_mode);
USER_FUNC(evt_puni_set_yuka);
USER_FUNC(evt_puni_wait_systemmode);
USER_FUNC(evt_target_flag_onoff);
USER_FUNC(evt_target_set_position);
extern s32 mri_countdown[];
extern s32 mri_hikari_color[];
extern s32 mri_puni_init[];
extern s32 mri_puni_init_none[];
extern s32 mri_punipuni_itemremove[];
extern s32 mri_punipuni_itemselect[];
extern s32 mri_punipuni_itemset[];
extern s32 mri_punipuni_setup[];
extern s32 puni_disp_info[];
extern s32 evt_puni_dokan_onoff[];
