#pragma once
#include <dolphin/types.h>
typedef struct EventEntry EventEntry;
s32 sac_wao(EventEntry* event, BOOL isFirstCall);
s32 sac_handbeat(EventEntry* event, BOOL isFirstCall);
s32 sac_handbeat_end(EventEntry* event, BOOL isFirstCall);
s32 sac_enemy_slide_go(EventEntry* event, BOOL isFirstCall);
s32 sac_enemy_slide_return(EventEntry* event, BOOL isFirstCall);
s32 sac_ac_help_on(EventEntry* event, BOOL isFirstCall);
s32 sac_ac_help_off(EventEntry* event, BOOL isFirstCall);
