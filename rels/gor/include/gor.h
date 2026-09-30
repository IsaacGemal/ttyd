#pragma once

#include "evt/evt_cmd.h"
#include "driver/npcdrv.h"

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

extern void* unk_gor_bss00000000[];
extern s32 unk_gor_bss00000004[];
extern BattleSetupData btlsetup_gor_tbl[];
extern u32 btl_gor_setup_no_tbl[];

USER_FUNC(unk_gor_000000cc);
USER_FUNC(unk_gor_00000104);
USER_FUNC(evt_goki_sub_intpl_get_value);
USER_FUNC(evt_goki_sub_intpl_init);
USER_FUNC(evt_gor_villager_urouro_check_func);
USER_FUNC(unk_gor_0000041c);
USER_FUNC(unk_gor_0000050c);
void _unresolved(void);
void _epilog(void);
void _prolog(void);
EVT_DEF(gor_villager_urouro);
EVT_DEF(inemuri_snd);
EVT_DEF(mario_and_party_sleep);
EVT_DEF(luigi_gor_first_talk);
EVT_DEF(gor_11_init_evt);
EVT_DEF(gor_12_init_evt);
