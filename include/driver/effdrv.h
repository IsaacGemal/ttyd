#pragma once

#include <dolphin/dvd.h>
#include "manager/filemgr.h"
#include "sdk/texPalette.h"

typedef void (*EffectCallback)(struct EffectEntry* effect);

typedef struct EffectEntry {
    u32 flags;               // 0x0
    u32 inBattle;            // 0x4
    s32 count;               // 0x8
    void* userdata;          // 0xC, for effects to store their own data
    EffectCallback callback; // 0x10
    const char* type;        // 0x14
    char name[16];           // 0x18
} EffectEntry;

typedef struct EffectSet {
    s16 id;           // 0x0
    const char* name; // 0x4
} EffectSet;

typedef struct EffectWork {
    s32 count;            // 0x0
    EffectEntry* entries; // 0x4
    TEXPalette* texture;  // 0x8
    BOOL texLoaded;       // 0xC
    FileEntry* handle;    // 0x10
    s32 language;         // 0x14
} EffectWork;

typedef struct EffectMayaAnimEntry {
    f32 mtx[3][4];  // 0x0
    f32 field_0x30; // 0x30
} EffectMayaAnimEntry;

typedef struct EffectMayaAnimData {
    u32 field_0x0; // 0x0
    f32 field_0x4; // 0x4
    s32* count;    // 0x8
} EffectMayaAnimData;

typedef struct EffectMayaAnim {
    u16 flags;                      // 0x0
    u8 field_0x2[0x2];              // 0x2
    f32 field_0x4;                  // 0x4
    OSTime startTime;               // 0x8
    EffectMayaAnimData* animData;   // 0x10
    EffectMayaAnimEntry entries[];  // 0x14
} EffectMayaAnim;

void effInit(void);
void effTexSetup(void);
void effGetTexObj(u32 id, GXTexObj* obj);
void effAutoRelease(BOOL inBattle);
EffectEntry* effEntry(void);
void effSetName(EffectEntry* effect, const char* name);
void effMain(void);
void effDelete(EffectEntry* effect);
void effSoftDelete(EffectEntry* effect);
EffectEntry* effNameToPtr(const char* name);
EffectSet* effGetSet(const char* name);
void* effMayaAnimAlloc(void* animData);
void effPlayMayaAnim(void* anim);
void effCalcMayaAnim(void* anim);
void effDeleteMayaAnim(void* anim);
void effDrawMayaPoly(void* data);
void effCalcMayaAnimMatrix(void* anim, void* data, void* matrix, f32* alpha);
// .sdata2 float words this unit reads by name
