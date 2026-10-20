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

typedef struct MapObjectEntry {
    s32 flags;                   // 0x0
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
    u8 unk1A0[0x1A8 - 0x1A0];    // 0x1A0, per-type data; 0x1A4 coin count (read as unk1A0[4])
    s32 msgWindowId;             // 0x1A8
    f32 tiltAngle;               // 0x1AC, degrees; rotates the object about Z and is passed to marioChgJumpStandMotion
    s32 timer;                   // 0x1B0, countdown decremented each frame by the timed object types
    u8 unk1B4[0x1B8 - 0x1B4];    // 0x1B4, sound handle or reload value, depending on the type
    char childName[0x10];        // 0x1B8, name of the child object this one spawns (strcpy'd in evt_mobj)
    EffectEntry* effect;         // 0x1C8
    s32 eventId;                 // 0x1CC
    s32 (*update)(struct MapObjectEntry*); // 0x1D0, per-type update callback
    s32* eventCode;              // 0x1D4, event script run by mobjRunEvent
    s32* unk1D8;                 // 0x1D8, event script
    s32 state;                   // 0x1DC, per-type step: 0 idle, 0x63 done (mobjCheckItemboxOpen tests 99)
    s32 triggerFlag;             // 0x1E0, the entry->flags bit that last hit Mario (0x200000, 0x1000000, ...)
    s32 flagVar;                 // 0x1E4, script variable/flag: set when activated, read by evtGetValue
    GXTexObj paperTexObj;        // 0x1E8
    smartEntry* paperTex;        // 0x208, smartAlloc buffer that GXCopyTex fills for the paper texture
    Mtx paperTexMtx;             // 0x20C
} MapObjectEntry;

typedef struct MapObjectWork {
    s32 count;                // 0x0
    MapObjectEntry* entries;  // 0x4
    u8 field_0x8[0x1C - 0x8]; // 0x8
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
