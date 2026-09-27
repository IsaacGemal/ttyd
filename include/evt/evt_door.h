#pragma once

#include "evt/evt_cmd.h"
#include <dolphin/types.h>

USER_FUNC(snd_door_in);
USER_FUNC(snd_door_out);
USER_FUNC(door_entry);
void door_dark_flag_sub(s32 flag, char** npcs, char** mobjs, char** items);
void door_position_sub(s32 mode, char* mapName, char* hitIn, char* hitOut, Vec* in, Vec* out, s32 flag);
