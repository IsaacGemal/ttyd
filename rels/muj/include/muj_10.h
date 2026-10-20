#pragma once

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include "evt/evt_cmd.h"

typedef struct NanNpcEntry NanNpcEntry;

// Nameless-NPC work and per-entry state. src/event/evt_nannpc.c keeps these private, but the
// REL needs the same layout to drive suifu_option_* / elmosalpha_func, so it is mirrored here
// (offsets copied from that file).
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
    OSTime stateStart;              // 0x50, renderTime when the current state began
    s16* animTable;                 // 0x58
    s16 animCount;                  // 0x5C
    s16 animCurrent;                // 0x5E
    f32 direction;                  // 0x60, facing in degrees (90 or 270)
    void (*callback)(NanNpcEntry*); // 0x64
    s32 state;                      // 0x68
    s32 stateDuration;              // 0x6C, ms until the current state ends
    s32 field_0x70;                 // 0x70
    s32 radius;                     // 0x74, orbit radius (elmos1000tai) and the base of its first wait
    f32 jumpCurveA;                 // 0x78, y = hoseiPosition.y + t * (jumpCurveA * t + jumpCurveB)
    f32 jumpCurveB;                 // 0x7C
    u8 pad_0x80[0x8];               // 0x80
    Vec field_0x88;                 // 0x88
    u8 pad_0x94[0xC];               // 0x94
    void* data;                     // 0xA0, per-entry record (tobikakari: home Vec, kind at +0xC)
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
