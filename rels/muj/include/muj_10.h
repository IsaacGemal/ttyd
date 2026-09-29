#pragma once

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include "evt/evt_cmd.h"

typedef struct NanNpcEntry NanNpcEntry;

// Nameless-NPC work and per-entry state. src/event/evt_nannpc.c keeps these private, but the
// REL needs the same layout to drive suifu_option_* / elmosalpha_func, so it is mirrored here
// (names and offsets copied from that file).
struct NanNpcEntry {
    char name[16];                  // 0x0
    void* pose;                     // 0x10
    u16 flags;                      // 0x14
    s16 field_0x16;                 // 0x16
    Vec position;                   // 0x18
    Vec hoseiPosition;              // 0x24
    Vec homePosition;               // 0x30
    Vec field_0x3C;                 // 0x3C
    f32 width;                      // 0x48
    f32 field_0x4C;                 // 0x4C
    OSTime field_0x50;              // 0x50
    s16* animTable;                 // 0x58
    s16 animCount;                  // 0x5C
    s16 animCurrent;                // 0x5E
    f32 field_0x60;                 // 0x60
    void (*callback)(NanNpcEntry*); // 0x64
    s32 field_0x68;                 // 0x68
    s32 field_0x6C;                 // 0x6C
    s32 field_0x70;                 // 0x70
    u8 field_0x74[0x14];            // 0x74
    Vec field_0x88;                 // 0x88
    u8 field_0x94[0x10];            // 0x94
    s32 work[6];                    // 0xA4
    GXColor color;                  // 0xBC
};

typedef struct NanNpcWork {
    NanNpcEntry* entries;   // 0x0
    void* sortList;         // 0x4
    s16* field_0x8;         // 0x8
    s32 count;              // 0xC
    u32 flags;              // 0x10
    void (*subfunc0)(void); // 0x14
    void (*subfunc1)(void); // 0x18
    void (*subfunc2)(void); // 0x1C
} NanNpcWork;

extern NanNpcWork* nanNPCWork;

USER_FUNC(suifu_option_func);
USER_FUNC(suifu_option_init_func);
void elmosalpha_func(NanNpcEntry* entry);
EVT_DEF(nazo_event);
EVT_DEF(oidasare_event);
EVT_DEF(senin_event);
EVT_DEF(muj_10_init_evt);
