#pragma once

#include "evt/evt_cmd.h"
#include <dolphin/types.h>

USER_FUNC(snd_door_in);
USER_FUNC(snd_door_out);
USER_FUNC(door_entry);
void door_dark_flag_sub(s32 flag, const char** npcs, const char** mobjs, const char** items);
void door_position_sub(s32 mode, const char* mapName, const char* hitIn, const char* hitOut, Vec* in, Vec* out, s32 flag);

// A shop's or house's door, handed to the evt_door.c door scripts as their DoorWork's param
typedef struct DoorParam {
    s32 type;                    // 0x00, door_entry: which side of the outside door's normal the out point sits (0/1, 2, 3)
    s32 outsideConditionId;      // 0x04, inout_hit's LW(0): evt_door_setup's evt_run_case_evt condition for house_in
    const char* outsideHit;      // 0x08, the outside door's hit object: house_in's case target; door_entry puts the out
                                 //       point 40 in front of it
    s32 insideConditionId;       // 0x0C, inout_hit's LW(2): the evt_run_case_evt condition for house_out
    const char* insideHit;       // 0x10, the inside door's hit object: house_out's case target; door_entry puts the in point
                                 //       in front of it
    const char* openAnim;        // 0x14, animation's LW(0): the map animation in_houseanim plays
    const char* field_0x18;      // 0x18, animation's LW(1), unused by the scripts
    const char* closeAnim;       // 0x1C, animation's LW(2): the map animation out_houseanim plays
    const char* field_0x20;      // 0x20, animation's LW(3), unused by the scripts
    const char* outsideGroup;    // 0x24, outside_group: house_in sets MAPOBJ_FLAG_NO_BLEND on this map group, house_out clears it
    const char* doorGroup;       // 0x28, door_group: the door's map object (in_dooranim/out_dooranim rotate it; house_in sets
                                 //       NO_BLEND and NO_BLEND1 on it, house_out clears them)
    const char* insideGroup;     // 0x2C, inside_group's LW(0): house_in clears MAPOBJ_FLAG_HIDDEN on this map group,
                                 //       house_out sets it
    const char* insideHitGroup;  // 0x30, inside_group's LW(1): evt_hitobj_onoff turns it on in house_in, off in house_out
    const char* initInsideGroup; // 0x34, init_inside_group's LW(0): evt_door_setup sets MAPOBJ_FLAG_HIDDEN on it
    const char* initInsideHitGroup; // 0x38, init_inside_group's LW(1): evt_door_setup turns it off (evt_hitobj_onoff)
    const char** npcNames;       // 0x3C, npc_dark/npc_light darken and restore them
    const char** noclipMaps;     // 0x40, get_noclip_map's list: house_in clears MAPOBJ_FLAG_CLIP on each, house_out sets it
    const char** mobjNames;      // 0x44, npc_dark/npc_light
    const char** itemNames;      // 0x48, npc_dark/npc_light
} DoorParam;

// A map's door, read by evt_door_entry and evt_door_param
typedef struct DoorEntry {
    s32 flags;                    // 0x00 bit 0: npcNames hidden until the door is entered
    s32 type;                     // 0x04 door_position_sub's mode
    s32 sfxType;                  // 0x08 door_open_se/door_close_se pick SFX_DOOR_OPEN_*/SHUT_* by it; door_down_se plays nothing for 0
    const char* doorMapObj;       // 0x0C
    const char* closeHit;         // 0x10
    const char* openHit;          // 0x14
    const char* outsideMapObj;    // 0x18
    const char* outsideHit;       // 0x1C
    const char* insideMapObj;     // 0x20
    const char* insideHit;        // 0x24
    s32* doorEvt;                 // 0x28
    s32* downEvt;                 // 0x2C when set, spawned each frame of a 0-180 intpl with door_down_se 500 ms in (the door falls)
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
