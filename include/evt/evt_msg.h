#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/types.h>

void evt_msg_init(void);
USER_FUNC(evt_msg_print);
USER_FUNC(evt_msg_print_add);
USER_FUNC(evt_msg_print_insert);
USER_FUNC(evt_msg_select);
s32 evt_msg_print_add_insert(EventEntry* event, BOOL isFirstCall);
s32 evt_msg_continue(EventEntry* event, BOOL isFirstCall);
s32 _evt_msg_print(EventEntry* event, BOOL isFirstCall, s32 flags, const char* msg, s32 toge, const char* npcName);

USER_FUNC(evt_msg_close);
USER_FUNC(evt_msg_npc);
USER_FUNC(evt_msg_pri);
USER_FUNC(evt_msg_print_battle_party);
USER_FUNC(evt_msg_print_party);
USER_FUNC(evt_msg_print_party_add);
USER_FUNC(evt_msg_repeat);
USER_FUNC(evt_msg_toge);
USER_FUNC(evt_msg_togelr);
