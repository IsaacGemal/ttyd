#pragma once

typedef struct EventEntry EventEntry;
typedef struct EffectEntry EffectEntry;
typedef struct smartEntry smartEntry;

#include "dolphin/gx/GXStruct.h"
#include "driver/camdrv.h"
#include "driver/hitdrv.h"
#include <dolphin/types.h>
#include <dolphin/mtx.h>

typedef struct MapObjectHitEntry {
    HitEntry* hitObject;    // 0x0
    char name[16];        // 0x4
    u8 unk4[0x88 - 0x14]; // 0x14
} MapObjectHitEntry;

// MapObjectEntry.flags bits whose effect is visible in mobjdrv.c; the rest stay hex at their uses.
// The MOBJ_FLAG_TRIGGER_* bits arm one mobjMain hit test each; the bit that fired is copied to triggerFlag.
// 0x10000, 0x40000 and 0x80000 are hit tests on MarioWork HitEntry pointers that have no names yet
// (0x1FC, 0x1E0); 0x2 and 0x800 both suppress the shadow.
enum MapObjectEntryFlags {
    MOBJ_FLAG_ACTIVE = 0x1,               // slot in use (mobjEntry sets it, mobjDelete clears it)
    MOBJ_FLAG_RECALC_MTX = 0x4,           // mobjMain rebuilds the matrix once, then clears it
    MOBJ_FLAG_TRIGGERED = 0x8,            // set when a hit test fires; no hit tests run while it is set
    MOBJ_FLAG_GRAVITY = 0x40,             // mobjMain applies velocity, gravity and bounce
    MOBJ_FLAG_NO_CULL = 0x80,             // skip the view-frustum test before dispEntry
    MOBJ_FLAG_RETRIGGER = 0x100,          // mobjMain clears MOBJ_FLAG_TRIGGERED every frame
    MOBJ_FLAG_PAPER_TEX = 0x400,          // draw with paperTexObj/paperTexMtx
    MOBJ_FLAG_BLINK = 0x2000,             // not drawn on alternate 8-frame spans
    MOBJ_FLAG_NO_DISP = 0x4000,           // animated but never passed to dispEntry
    MOBJ_FLAG_TRIGGER_HEAD = 0x400000,    // Mario's head hit (MarioWork 0x1F4, marioSearchHead); also a type-1 shadow
    MOBJ_FLAG_TRIGGER_STAND_ON = 0x200000, // Mario stands on it (hitobjStandOn), also checked as Bowser
    MOBJ_FLAG_TRIGGER_HIP_DROP = 0x1000000, // Mario stands on it and marioChkHipBump
    MOBJ_FLAG_TRIGGER_PARTY = 0x800000,   // partner's frontHit, or Bobbery within range
    MOBJ_FLAG_TRIGGER_BOBBERY = 0x2000000, // Bobbery's frontHit (in range reports MOBJ_FLAG_TRIGGER_PARTY)
    MOBJ_FLAG_TRIGGER_KPA_HEAD = 0x4000000, // Bowser: kpaGetHitobjHead
    MOBJ_FLAG_TRIGGER_KPA_LV3 = 0x8000000, // Bowser: kpaGetHitobj at kpaGetLevel 3
    MOBJ_FLAG_TRIGGER_KPA_HIP = 0x10000000, // Bowser: kpaGetHitobjRide and kpaHipAttackCheck
    MOBJ_FLAG_TRIGGER_KPA_FLAME = 0x20000000, // Bowser: kpaGetHitFlame
};

typedef struct MapObjectEntry {
    s32 flags;                   // 0x0, MapObjectEntryFlags
    u8 camId;                    // 0x4
    char name[16];               // 0x5
    char animName[16];           // 0x15
    u8 unk4[0x38 - 0x25];        // 0x25
    Vec position;                // 0x38
    Vec scale;                   // 0x44
    Vec rotation;                // 0x50
    Vec velocity;                // 0x5C, y is vertical speed: mobjMain adds it to position.y and subtracts gravity
    f32 gravity;                 // 0x68, per-frame decrement of velocity.y (evt_mobj_set_gravity_bound arg 1)
    f32 bounce;                  // 0x6C, landing scales velocity.y by -0.5 * bounce (evt_mobj_set_gravity_bound arg 2)
    s32 poseId;                  // 0x70
    MapObjectHitEntry hitObj[2]; // 0x74
    Vec scale2;                  // 0x184
    Vec translation;             // 0x190
    s32 offscreenId;             // 0x19C
    s32 unk1A0[2];               // 0x1A0, per-type: [0] item id (item-list pointer if bit 31 set); [1] coin count, part count, or switch anim index | state bits
    s32 msgWindowId;             // 0x1A8
    f32 unk1AC;                  // 0x1AC, jump stand: launch height for marioChgJumpStandMotion; fire bar: Z rotation in degrees
    s32 timer;                   // 0x1B0, countdown decremented each frame by the timed object types
    s32 unk1B4;                  // 0x1B4, sound handle or reload value, depending on the type
    char childName[0x10];        // 0x1B8, name of the child object this one spawns (strcpy'd in evt_mobj)
    EffectEntry* effect;         // 0x1C8
    s32 eventId;                 // 0x1CC
    s32 (*update)(struct MapObjectEntry*); // 0x1D0, per-type update callback
    s32* eventCode;              // 0x1D4, event script run by mobjRunEvent
    s32* unk1D8;                 // 0x1D8, event script
    s32 state;                   // 0x1DC, per-type step: 0 idle, 0x63 done (mobjCheckItemboxOpen tests 99)
    s32 triggerFlag;             // 0x1E0, the MOBJ_FLAG_TRIGGER_* (or other hit-test) bit that last fired
    s32 flagVar;                 // 0x1E4, script variable/flag: set when activated, read by evtGetValue
    GXTexObj paperTexObj;        // 0x1E8
    smartEntry* paperTex;        // 0x208, smartAlloc buffer that GXCopyTex fills for the paper texture
    Mtx paperTexMtx;             // 0x20C
} MapObjectEntry;

typedef struct MapObjectWork {
    s32 count;                // 0x0
    MapObjectEntry* entries;  // 0x4
    HitEntry* field_0x8;      // 0x8
    HitEntry* field_0xC;      // 0xC
    HitEntry* field_0x10;     // 0x10
    HitEntry* field_0x14;     // 0x14
    HitEntry* field_0x18;     // 0x18
} MapObjectWork;

void mobjInit(void);
void mobjMain(void);
BOOL mobjCheckExec(void);

void mobjHitEntry(MapObjectEntry* entry, s32 type);
BOOL mobjGetHint(MapObjectEntry* entry);
MapObjectEntry* mobjHitObjPtrToPtr(HitEntry* hit);
s32 mobjEntry(const char* name, const char* animPoseName);
void mobjDelete(const char* name);
void mobjSetPosition(const char* name, f32 x, f32 y, f32 z);
MapObjectEntry* mobjNameToPtr(const char* name);
MapObjectEntry* mobjNameToPtrNoAssert(const char* name);
EventEntry* mobjRunEvent(MapObjectEntry* entry, s32* eventCode);
void mobjCalcMtx(MapObjectEntry* entry);
void mobjCalcMtx2(MapObjectEntry* entry);
s32 mobjCheckItemboxOpen(MapObjectEntry* entry);
MapObjectEntry* mobjNearDistCheck2(f32 x, f32 y, f32 z, f32 minMagnitude, char** names);
void mobjReset(BOOL inBattle);
void mobjKoopaOn(void);
