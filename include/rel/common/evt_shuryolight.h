#pragma once

#include "driver/npcdrv.h"

// Shared by the evt_shuryolight units of aji and las, with their first helpers in rel/common/shuryolight.h.

// Per-light state owned by evt_shuryolight_init and handed to shuryolight_callback as its display user data.
typedef struct ShuryolightEntry {
    NpcEntry* npc;    // 0x0
    char** animTable; // 0x4
    s32 animIdx;      // 0x8
    s32 groupIdx;     // 0xC
    s32 shapeIdx;     // 0x10
} ShuryolightEntry;

// The 0x70-byte block allocated by evt_shuryolight_init; slhp points at it.
typedef struct ShuryolightWork {
    s32 powerEvtId;               // 0x0
    s32 headEvtId;                // 0x4
    s32 tueEvtId;                 // 0x8
    ShuryolightEntry entries[5];  // 0xC
} ShuryolightWork;
