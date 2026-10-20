#pragma once

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include "evt/evt_cmd.h"

typedef struct NanNpcEntry NanNpcEntry;

// Nameless-NPC work and per-entry state. src/event/evt_nannpc.c keeps these private, so the REL mirrors the
// layout. Names follow evt_nannpc.c and rels/gor/include/gor_00.h; the 0x70-0x84 and 0xA0 slots are free for
// the map's seq callbacks, and the names there are what muj uses them for.
typedef struct TobikakariSpot TobikakariSpot;

struct NanNpcEntry {
    char name[16];                  // 0x0
    void* pose;                     // 0x10
    u16 flags;                      // 0x14
    s16 field_0x16;                 // 0x16
    Vec position;                   // 0x18
    Vec hoseiPosition;              // 0x24
    Vec homePosition;               // 0x30, tobikakari: landing spot of the current jump
    Vec velocity;                   // 0x3C
    f32 width;                      // 0x48
    f32 range;                      // 0x4C
    OSTime startTime;               // 0x50, renderTime when the current state began
    s16* animTable;                 // 0x58
    s16 animCount;                  // 0x5C
    s16 animCurrent;                // 0x5E
    f32 facingAngle;                // 0x60, y rotation in degrees; 90 or 270 here
    void (*callback)(NanNpcEntry*); // 0x64
    s32 state;                      // 0x68, switch selector of the seq functions
    s32 timer;                      // 0x6C, ms the current state lasts
    s32 field_0x70;                 // 0x70, state entered after a tobikakari jump, or elmos1000tai's entry number
    s32 radius;                     // 0x74, elmos1000tai: distance from the formation centre; also delays its fade-in
    f32 jumpQuad;                   // 0x78, tobikakari jump arc: y = hoseiPosition.y + t * (jumpQuad * t + jumpLinear)
    f32 jumpLinear;                 // 0x7C
    u8 field_0x80[0x8];             // 0x80
    Vec moveStart;                  // 0x88, start position of an evt_nannpc_move_position2 move
    u8 field_0x94[0xC];             // 0x94
    TobikakariSpot* spot;           // 0xA0, tobikakari: this entry's home record in Tobidat
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
