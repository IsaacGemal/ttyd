#pragma once

#include "evt/evt_cmd.h"

void _prolog(void);  // TODO: real signature
void _epilog(void);  // TODO: real signature
void _unresolved(void);  // TODO: real signature
USER_FUNC(evt_mri_door_naname_func);
USER_FUNC(hakari_color);
void evt_mri_get16bitdata(void);  // TODO: real signature
void mri_npc_buruburu(void);  // TODO: real signature
void toge_searchParent(void);  // TODO: real signature
void togeModeParent(void);  // TODO: real signature
void togeMoveParent(void);  // TODO: real signature
void puniModeWinjump(void);  // TODO: real signature
void evt_mri_toge_make_retsu(void);  // TODO: real signature
void evt_mri_toge_retsu_stop(void);  // TODO: real signature
void evt_mri_toge_enter_init(void);  // TODO: real signature
void evt_mri_puni_win_init(void);  // TODO: real signature
void evt_mri_puni_win_attack(void);  // TODO: real signature
void evt_mri_puni_win_end(void);  // TODO: real signature
EVT_DEF(evt_mri_door_open_pure);
EVT_DEF(evt_mri_door_open);
EVT_DEF(evt_mri_door_close_pure);
EVT_DEF(evt_mri_door_close);
EVT_DEF(evt_mri_door_naname_open_pure);
EVT_DEF(evt_mri_door_naname_open);
EVT_DEF(evt_mri_door_naname_close_pure);
EVT_DEF(evt_mri_door_naname_close);
EVT_DEF(_ueki_anime);
EVT_DEF(mri_ueki_entry);
EVT_DEF(mri_puni_save);
EVT_DEF(mri_punipuni_setup);
EVT_DEF(mri_punipuni_itemselect);
EVT_DEF(mri_punipuni_itemset);
EVT_DEF(mri_punipuni_itemremove);
EVT_DEF(mri_hikari_color);
EVT_DEF(mri_countdown);
EVT_DEF(mri_waterfall_effect);
EVT_DEF(mri_guide_hagure_check);
EVT_DEF(mri_guide_goryu);
EVT_DEF(mri_puni_walk_se);
EVT_DEF(mri_puni_jump_se);
