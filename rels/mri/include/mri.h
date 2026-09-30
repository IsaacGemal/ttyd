#pragma once

#include "evt/evt_cmd.h"
#include "driver/npcdrv.h"
#include "mri_puni.h"

typedef void (*voidfunctionptr)(void);
extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

void _prolog(void);
void _epilog(void);
void _unresolved(void);
USER_FUNC(evt_mri_door_naname_func);
USER_FUNC(hakari_color);
USER_FUNC(evt_mri_get16bitdata);
USER_FUNC(mri_npc_buruburu);
s32 toge_searchParent(PuniData* puni, s16 id);
void togeModeParent(PuniData* puni);
void togeMoveParent(PuniData* puni);
void puniModeWinjump(PuniData* puni);
USER_FUNC(evt_mri_toge_make_retsu);
USER_FUNC(evt_mri_toge_retsu_stop);
USER_FUNC(evt_mri_toge_enter_init);
USER_FUNC(evt_mri_puni_win_init);
USER_FUNC(evt_mri_puni_win_attack);
USER_FUNC(evt_mri_puni_win_end);
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

// Cross-unit declarations for this REL, provided by mri_puni.c.
PuniWork* puniGetPunigp(void);
void puniMoveDecel(PuniData* puni);
void PuniDokuritsu(PuniData* puni);

// Battle setup data defined by the original data unit.
extern BattleSetupData btlsetup_mri_tbl[22];
extern s32 btl_mri_setup_no_tbl[];

// Per-map init scripts registered by _prolog (defined by the mri_NN units).
EVT_DEF(mri_00_init_evt);
EVT_DEF(mri_01_init_evt);
EVT_DEF(mri_02_init_evt);
EVT_DEF(mri_03_init_evt);
EVT_DEF(mri_04_init_evt);
EVT_DEF(mri_05_init_evt);
EVT_DEF(mri_06_init_evt);
EVT_DEF(mri_07_init_evt);
EVT_DEF(mri_08_init_evt);
EVT_DEF(mri_09_init_evt);
EVT_DEF(mri_10_init_evt);
EVT_DEF(mri_11_init_evt);
EVT_DEF(mri_12_init_evt);
EVT_DEF(mri_13_init_evt);
EVT_DEF(mri_14_init_evt);
EVT_DEF(mri_15_init_evt);
EVT_DEF(mri_16_init_evt);
EVT_DEF(mri_17_init_evt);
EVT_DEF(mri_18_init_evt);
EVT_DEF(mri_19_init_evt);
EVT_DEF(mri_20_init_evt);
