#pragma once

#include "evt/evt_cmd.h"
#include <dolphin/types.h>

USER_FUNC(snd_door_in);
USER_FUNC(snd_door_out);
USER_FUNC(door_entry);
void door_dark_flag_sub(s32 flag, char** npcs, char** mobjs, char** items);
void door_position_sub(s32 mode, char* mapName, char* hitIn, char* hitOut, Vec* in, Vec* out, s32 flag);

// A map's door, read by evt_door_entry and evt_door_param
typedef struct DoorEntry {
    s32 flags;                    // 0x00 bit 0: hide npcNames while inside
    s32 type;                     // 0x04 door_position_sub's mode
    s32 field_0x8;                // 0x08
    const char* doorMapObj;       // 0x0C
    const char* closeHit;         // 0x10
    const char* openHit;          // 0x14
    const char* outsideMapObj;    // 0x18
    const char* outsideHit;       // 0x1C
    const char* insideMapObj;     // 0x20
    const char* insideHit;        // 0x24
    s32* doorEvt;                 // 0x28
    s32 field_0x2C;               // 0x2C
    s32* wallEvt;                 // 0x30
    const char** npcNames;        // 0x34
    const char** mapObjNames;     // 0x38
    const char** itemNames;       // 0x3C
    s32* openEvt;                 // 0x40
    s32* closeEvt;                // 0x44
} DoorEntry;

EVT_DEF(door_open_se);
EVT_DEF(evt_door_entry);
EVT_DEF(evt_door_open_quick);
