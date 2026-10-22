#pragma once

#include "evt/evt_cmd.h"
#include <dolphin/types.h>

USER_FUNC(snd_door_in);
USER_FUNC(snd_door_out);
USER_FUNC(door_entry);
void door_dark_flag_sub(s32 flag, const char** npcs, const char** mobjs, const char** items);
void door_position_sub(s32 mode, const char* mapName, const char* hitIn, const char* hitOut, Vec* in, Vec* out, s32 flag);

// A map's door, read by evt_door_entry and evt_door_param
typedef struct DoorEntry {
    s32 flags;                    // 0x00 bit 0: npcNames hidden until the door is entered
    s32 type;                     // 0x04 door_position_sub's mode
    s32 field_0x8;                // 0x08 door_open_se picks the sound by it
    const char* doorMapObj;       // 0x0C
    const char* closeHit;         // 0x10
    const char* openHit;          // 0x14
    const char* outsideMapObj;    // 0x18
    const char* outsideHit;       // 0x1C
    const char* insideMapObj;     // 0x20
    const char* insideHit;        // 0x24
    s32* doorEvt;                 // 0x28
    s32* field_0x2C;              // 0x2C run with door_down_se
    s32* houseEvt;                // 0x30 run with house_open_se/house_close_se
    const char** npcNames;        // 0x34
    const char** mobjNames;       // 0x38
    const char** itemNames;       // 0x3C
    s32* openEvt;                 // 0x40
    s32* closeEvt;                // 0x44
} DoorEntry;

EVT_DEF(door_open_se);
EVT_DEF(evt_door_entry);
EVT_DEF(evt_door_open_quick);

EVT_DEF(door_close_se);
EVT_DEF(door_dark);
USER_FUNC(door_dark_flag);
EVT_DEF(door_light);
EVT_DEF(door_light2);
EVT_DEF(door_up_se);
EVT_DEF(evt_door_close);
EVT_DEF(evt_door_close_roll);
USER_FUNC(evt_door_data_copy);
USER_FUNC(evt_door_end_wait);
USER_FUNC(evt_door_load_mapflag);
EVT_DEF(evt_door_open);
EVT_DEF(evt_door_open_bom_01);
EVT_DEF(evt_door_open_roll);
USER_FUNC(evt_door_param);
USER_FUNC(evt_door_save_mapflag);
USER_FUNC(evt_door_set_param);
EVT_DEF(evt_door_setup);
EVT_DEF(house_close_se);
