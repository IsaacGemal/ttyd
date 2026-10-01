#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "driver/extdrv.h"
#include "driver/dispdrv.h"
#include "driver/hitdrv.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <string.h>
#include "evt/evt_nannpc.h"

// Local declarations for this translation unit. Nothing below is in a header yet:
//   NanNpcWork / NanNpcEntry  - the nameless-NPC work and per-entry state
//   makestring                - scratch buffer used to format numeric names
//   ext* / qqsort             - driver/extdrv.c internals used by the ext_ callbacks
//   nannpc_ext_*              - this unit's draw callbacks
typedef struct NanNpcEntry NanNpcEntry;

struct NanNpcWork {
    NanNpcEntry* entries;   // 0x0
    ExtSortEntry* sortList; // 0x4
    s16* field_0x8;         // 0x8
    s32 count;              // 0xC
    u32 flags;              // 0x10
    void (*subfunc0)(void); // 0x14
    void (*subfunc1)(void); // 0x18
    void (*subfunc2)(void); // 0x1C
};

struct NanNpcEntry {
    char name[16];                  // 0x0
    ExtPose* pose;                  // 0x10
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

NanNpcWork* nanNPCWork;
static char makestring[0x10];
static Mtx mt_544 = {
    {10.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 10.0f, 0.0f, 0.1f},
    {0.0f, 0.0f, 10.0f, 0.0f},
};
// Non-const (so the compiler does not hoist the loop loads out of evt_nannpc_init) but forced into
// .rodata to keep the original's section. Declared global to match the original's symbol linkage.
__declspec(section ".rodata") Vec vec3_802f2ff8[6] = {
    {0.0f, -1000.0f, 0.0f},
    {0.0f, -1000.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, -1000.0f, 0.0f},
    {0.0f, -1000.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
};
// The original's `dat_8041c5xx` are global, read-only `.sdata2` constants; section is forced so
// they get the exact symbol names and placement.
__declspec(section ".sdata2") GXColor dat_8041c570 = {0xFF, 0xFF, 0xFF, 0xFF};
__declspec(section ".sdata2") GXColor dat_8041c56c = {0xFF, 0xFF, 0xFF, 0xFF};

// Look up an entry by numeric id (formatted as "%06x") or by the string the id
// points at. Small enough that the original compiler inlined it everywhere.
static inline NanNpcEntry* nannpcFind(const char* name) {
    s32 i;
    NanNpcEntry* entry;
    NanNpcWork* work;

    work = nanNPCWork;

    for (i = 0; i < work->count; i++) {
        entry = &work->entries[i];
        if (!((s32)name & 0x80000000)) {
            sprintf(makestring, "%06x", (s32)name);
            name = makestring;
        }
        if (strcmp(entry->name, name) == 0) {
            return entry;
        }
    }
    return NULL;
}

USER_FUNC(evt_nannpcwork_flag_onoff) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    NanNpcWork* work = nanNPCWork;
    u16 value = evtGetValue(event, args[1]);

    if (type != 0) {
        work->flags &= ~value;
    } else {
        work->flags |= value;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_nannpc_set_color) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    NanNpcEntry* entry = nannpcFind((const char*)id);

    entry->color.r = evtGetValue(event, args[1]);
    entry->color.g = evtGetValue(event, args[2]);
    entry->color.b = evtGetValue(event, args[3]);
    entry->color.a = evtGetValue(event, args[4]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_nannpc_set_shadow_position) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    NanNpcEntry* entry = nannpcFind((const char*)id);

    entry->pose->flags |= 0x10;
    entry->pose->field_0x8 = evtGetFloat(event, args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_nannpc_set_func) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    NanNpcEntry* entry = nannpcFind((const char*)id);

    entry->callback = (void (*)(NanNpcEntry*))evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_nannpc_ppflag_onoff) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 type = evtGetValue(event, args[1]);
    NanNpcEntry* entry = nannpcFind((const char*)id);
    u16 mask = evtGetValue(event, args[2]);

    if (type != 0) {
        entry->pose->flags &= ~mask;
    } else {
        entry->pose->flags |= mask;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_nannpc_flag_onoff) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 type = evtGetValue(event, args[1]);
    NanNpcEntry* entry = nannpcFind((const char*)id);
    u16 mask = evtGetValue(event, args[2]);

    if (type != 0) {
        entry->flags &= ~mask;
    } else {
        entry->flags |= mask;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_nannpc_set_size) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    NanNpcEntry* entry = nannpcFind((const char*)id);

    entry->width = evtGetFloat(event, args[1]);
    entry->field_0x4C = evtGetFloat(event, args[2]);
    return EVT_RETURN_DONE;
}

#pragma opt_common_subs off // needed: with CSE on the elapsed division result is coalesced into r24; off keeps it in r4
USER_FUNC(evt_nannpc_jump_position) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    NanNpcEntry* entry = nannpcFind((const char*)id);
    f32 targetX = (f32)evtGetValue(event, args[1]);
    f32 targetY = (f32)evtGetValue(event, args[2]);
    f32 targetZ = (f32)evtGetValue(event, args[3]);
    s32 duration = evtGetValue(event, args[4]);
    f32 arcHeight = (f32)evtGetValue(event, args[5]);
#ifdef MUST_MATCH
    // MUST_MATCH: the u32 temporary and assigning elapsed inside the first call keep the division
    // result in r4 and its callee-saved copy in r24; the natural s32 form homes it in r24 throughout.
    u32 rawElapsed;
#endif
    s32 elapsed;
    f32 t;

    if (isFirstCall) {
        entry->field_0x50 = gp->renderTime;
        entry->field_0x68 = (s32)entry->position.x;
        entry->field_0x6C = (s32)entry->position.y;
        entry->field_0x70 = (s32)entry->position.z;
        event->userdataf[1] = -4.0f * arcHeight;
        event->userdataf[2] = (targetY - (f32)entry->field_0x6C) - event->userdataf[1];
    }

#ifdef MUST_MATCH
    rawElapsed = OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)entry->field_0x50);
    if ((s32)rawElapsed < duration) {
        t = (f32)(s32)rawElapsed / (f32)duration;
        entry->position.x = intplGetValue(0, elapsed = rawElapsed, duration, (f32)entry->field_0x68, targetX);
#else
    elapsed = (s32)OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)entry->field_0x50);
    if (elapsed < duration) {
        t = (f32)elapsed / (f32)duration;
        entry->position.x = intplGetValue(0, elapsed, duration, (f32)entry->field_0x68, targetX);
#endif
        entry->position.z = intplGetValue(0, elapsed, duration, (f32)entry->field_0x70, targetZ);
        entry->position.y = (f32)entry->field_0x6C + t * ((event->userdataf[1] * t) + event->userdataf[2]);
        return EVT_RETURN_BLOCK;
    }
    entry->position.x = targetX;
    entry->position.y = targetY;
    entry->position.z = targetZ;
    return EVT_RETURN_DONE;
}
#pragma opt_common_subs reset

USER_FUNC(evt_nannpc_move_position2) {
    OSTime time;
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    NanNpcEntry* entry = nannpcFind((const char*)id);
    Vec target;
    Vec delta;
    Vec move;
    f32 speed;
    s32 duration;
    s32 elapsed;

    if (gp->inBattle != 0) {
        time = gp->renderTime;
    } else {
        time = gp->renderFieldTime;
    }
    target.x = (f32)evtGetValue(event, args[1]);
    target.y = (f32)evtGetValue(event, args[2]);
    target.z = (f32)evtGetValue(event, args[3]);
    speed = evtGetFloat(event, args[4]);
    if (isFirstCall) {
        event->savedTime = time;
        entry->field_0x88 = entry->position;
    }
    PSVECSubtract(&target, &entry->field_0x88, &delta);
    duration = (s32)((1000.0f * PSVECMag(&delta)) / speed);
    elapsed = (s32)OSTicksToMilliseconds((u64)(s32)((s32)time - (s32)event->savedTime));
    PSVECScale(&delta, &move, (f32)elapsed / (f32)duration);
    if (elapsed < duration) {
        PSVECAdd(&entry->field_0x88, &move, &entry->position);
        return EVT_RETURN_BLOCK;
    }
    entry->position = target;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_nannpc_get_work) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    NanNpcEntry* entry = nannpcFind((const char*)id);

    evtSetValue(event, args[2], entry->work[evtGetValue(event, args[1])]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_nannpc_set_work) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    NanNpcEntry* entry = nannpcFind((const char*)id);
    s32 index = evtGetValue(event, args[1]);

    entry->work[index] = evtGetValue(event, args[2]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_nannpc_set_dir) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    NanNpcEntry* entry = nannpcFind((const char*)id);

    entry->field_0x60 = (f32)evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_nannpc_set_animtbl) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    NanNpcEntry* entry = nannpcFind((const char*)id);
    s16* table = (s16*)evtGetValue(event, args[1]);
    s16 count = 0;

    entry->animTable = table;
    for (;;) {
        if (*table == -1) {
            break;
        }
        table++;
        count++;
    }
    entry->animCount = count;
    if (entry->animCount == 0) {
        entry->animTable = NULL;
    }
    return EVT_RETURN_DONE;
}

void evt_nannpc_set_animtbl_sub(NanNpcEntry* entry, s16* table) {
    s16 count = 0;

    entry->animTable = table;
    for (;;) {
        if (*table == -1) {
            break;
        }
        table++;
        count++;
    }
    entry->animCount = count;
    if (entry->animCount == 0) {
        entry->animTable = NULL;
    }
}

USER_FUNC(evt_nannpc_set_hosei_position) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    NanNpcEntry* entry = nannpcFind((const char*)id);

    entry->hoseiPosition.x = evtGetFloat(event, args[1]);
    entry->hoseiPosition.y = evtGetFloat(event, args[2]);
    entry->hoseiPosition.z = evtGetFloat(event, args[3]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_nannpc_get_position) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    NanNpcEntry* entry = nannpcFind((const char*)id);

    evtSetFloat(event, args[1], entry->position.x);
    evtSetFloat(event, args[2], entry->position.y);
    evtSetFloat(event, args[3], entry->position.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_nannpc_set_position) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    NanNpcEntry* entry = nannpcFind((const char*)id);
    f32 x;
    f32 y;
    f32 z;

    x = evtGetFloat(event, args[1]);
    entry->position.x = x;
    entry->homePosition.x = x;
    y = evtGetFloat(event, args[2]);
    entry->position.y = y;
    entry->homePosition.y = y;
    z = evtGetFloat(event, args[3]);
    entry->position.z = z;
    entry->homePosition.z = z;
    return EVT_RETURN_DONE;
}

// NONMATCHING (94.72%): the loop callee-saved registers are still permuted (orig
// vec=r31, args=r24, poses=r28, id=r25, work=r23, mask=r29, name=r27, entry=r30, offset=r26;
// ours vec=r29, args=r27, poses=r30, id=r26, work=r25, mask=r27, name=r23, entry=r31, offset=r28).
USER_FUNC(evt_nannpc_entry) {
    Vec* vec = (Vec*)vec3_802f2ff8;
    s32* args = event->args;
    ExtPose* poses = extGetPosePtr();
    s32 id = evtGetValue(event, args[0]);
    NanNpcWork* work = nanNPCWork;
    s32 i;

    for (i = 0; i < work->count; i++) {
        NanNpcEntry* entry = &work->entries[i];
        const char* name = (const char*)id;

        if (!(id & 0x80000000)) {
            sprintf(makestring, "%06x", id);
            name = makestring;
        }
        (void)strcmp(entry->name, name);
        if (entry->pose != NULL) {
            continue;
        }
            strcpy(entry->name, name);
            entry->pose = &poses[i];
            entry->flags = 0;
            entry->field_0x16 = 0;
            entry->position = vec[3];
            entry->homePosition = vec[4];
            entry->hoseiPosition = vec[5];
            entry->width = 32.0f;
            entry->field_0x4C = 32.0f;
            entry->field_0x50 = 0;
            entry->animCount = 0;
            entry->field_0x60 = 0.0f;
            entry->pose->field_0x4 = -entry->field_0x60;
            entry->color = dat_8041c570;
            break;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_nannpc_init) {
    NanNpcWork* work;
    Vec* vec = (Vec*)vec3_802f2ff8;
    s32* args = event->args;
    s32 a0 = evtGetValue(event, args[0]);
    s16* src = (s16*)evtGetValue(event, args[1]);
    void (*a2)(void) = (void (*)(void))evtGetValue(event, args[2]);
    void (*a3)(void) = (void (*)(void))evtGetValue(event, args[3]);
    void (*cb)(void) = nannpc_ext_dispent;
    s32 i;

    if (isFirstCall) {
        return EVT_RETURN_BLOCK;
    }
    nanNPCWork = _mapAlloc(0x20);
    memset(nanNPCWork, 0, 0x20);
    work = nanNPCWork;
    work->count = evtGetValue(event, args[4]);
    work->entries = _mapAlloc(work->count * 0xC0);
    memset(work->entries, 0, work->count * 0xC0);
    work->sortList = _mapAlloc(work->count * 8);
    memset(work->sortList, 0, work->count * 8);
    work->field_0x8 = _mapAlloc(0x14);
    memset(work->field_0x8, 0, 0x14);
    work->subfunc0 = NULL;
    nanNPCWork->subfunc1 = nannpc_ext_main_sub;
    work->subfunc2 = NULL;
    if (a2 == NULL) {
        a2 = nannpc_ext_init;
    }
    if (a3 == NULL) {
        a3 = nannpc_ext_main;
    }
    extEntry(work->count, (void*)a0, a2, a3, cb);
    for (i = 0; i < work->count; i++) {
        NanNpcEntry* entry = &work->entries[i];

        entry->name[0] = 0;
        entry->pose = NULL;
        entry->flags = 0;
        entry->field_0x16 = 0;
        entry->position = vec[0];
        entry->homePosition = vec[1];
        entry->hoseiPosition = vec[2];
        entry->width = 32.0f;
        entry->field_0x4C = 32.0f;
        entry->field_0x50 = 0;
        entry->animTable = NULL;
        entry->animCount = 0;
        entry->animCurrent = 0;
        entry->field_0x60 = 0.0f;
        entry->callback = NULL;
        entry->color = dat_8041c56c;
    }
    if (src == NULL) {
        return EVT_RETURN_DONE;
    }
    {
        if (src[0] != -1) {
            work->field_0x8[0] = src[0];
            work->field_0x8[1] = src[1];
            if (src[2] != -1) {
                work->field_0x8[2] = src[2];
                work->field_0x8[3] = src[3];
                if (src[4] != -1) {
                    work->field_0x8[4] = src[4];
                    work->field_0x8[5] = src[5];
                    if (src[6] != -1) {
                        work->field_0x8[6] = src[6];
                        work->field_0x8[7] = src[7];
                        if (src[8] != -1) {
                            work->field_0x8[8] = src[8];
                            work->field_0x8[9] = src[9];
                        }
                    }
                }
            }
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_nannpc_set_subfunc) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    NanNpcWork* work = nanNPCWork;

    switch (type) {
        case 0:
            work->subfunc0 = (void (*)(void))evtGetValue(event, args[1]);
            break;
        case 1:
            work->subfunc1 = (void (*)(void))evtGetValue(event, args[1]);
            break;
        case 2:
            work->subfunc2 = (void (*)(void))evtGetValue(event, args[1]);
            break;
    }
    return EVT_RETURN_DONE;
}

s32 nannpc_zcompare(ExtSortEntry* a, ExtSortEntry* b) {
    if (a->field_0x4 < b->field_0x4) {
        return 1;
    }
    if (a->field_0x4 > b->field_0x4) {
        return -1;
    }
    return 0;
}

void nannpc_ext_shadow_disp(void) {
    extLoadShadowMtx(mt_544);
    extLoadShadowRenderMode();
    extLoadShadowVertex();
    extLoadShadowTev();
    extDrawShadow();
}

void nannpc_ext_main(void) {
    NanNpcWork* work = nanNPCWork;

    if (work->subfunc0 != NULL) {
        work->subfunc0();
    }
    if (work->subfunc1 != NULL) {
        work->subfunc1();
    }
    if (work->subfunc2 != NULL) {
        work->subfunc2();
    }
}

void nannpc_ext_main_sub(void) {
    s32 i;
    NanNpcWork* work = nanNPCWork;

    for (i = extGetPoseNum() - 1; i >= 0; i--) {
        NanNpcEntry* entry = &work->entries[i];

        if ((s8)entry->name[0] != 0 && entry->callback != NULL) {
            entry->callback(entry);
        }
    }
}

void nannpc_ext_main_sub_fast(void) {
    s32 i;
    NanNpcEntry* entry;
    NanNpcWork* work;

    work = nanNPCWork;
    for (i = extGetPoseNum() - 1; i >= 0; i--) {
        entry = &work->entries[i];

        if ((s8)entry->name[0] != 0) {
            if (entry->callback != NULL) {
                entry->callback(entry);
            }
            if (entry->animTable != NULL) {
                entry->animCurrent = entry->animTable[(gp->retraceCount + i * i) % entry->animCount];
            }
            entry->pose->field_0x10 = entry->animCurrent;
            entry->pose->field_0x4 = -entry->field_0x60;
            if (entry->flags & 2) {
                (*entry->pose->matrix)[0][3] = 0.0f;
                (*entry->pose->matrix)[1][3] = -1000.0f;
                (*entry->pose->matrix)[2][3] = 0.0f;
            } else {
                (*entry->pose->matrix)[0][3] = entry->position.x;
                (*entry->pose->matrix)[1][3] = entry->position.y;
                (*entry->pose->matrix)[2][3] = entry->position.z;
            }
        }
    }
}

#ifdef MUST_MATCH
// The original compiler inlines this GX state setup (fog colour included) after the draw
// loops, which is what places the fog GXColor copy below the tev one in the frame.
static inline void nannpcExtSetup(void) {
    extLoadVertex();
    extLoadTexture();
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 8, 2, 0xF);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 4, 1, 7);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    {
        GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};
        GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, color);
    }
    GXSetNumTexGens(1);
    GXSetNumTevStages(1);
}
#endif

void nannpc_ext_disp(void) {
    NanNpcWork* work = nanNPCWork;

    if (work->flags & 1) {
        s32 i;
        s32 count;

#ifdef MUST_MATCH
        nannpcExtSetup();
#else
        extLoadVertex();
        extLoadTexture();
        GXSetTevOrder(0, 0, 0, 4);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0, 0xF, 8, 2, 0xF);
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(0, 7, 4, 1, 7);
        GXSetNumChans(0);
        GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
        {
            GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};
            GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, color);
        }
        GXSetNumTexGens(1);
        GXSetNumTevStages(1);
#endif
        count = extGetPoseNum();
        for (i = count - 1; i >= 0; i--) {
            NanNpcEntry* entry = &work->entries[i];
            ExtSortEntry* sort = &work->sortList[i];

            sort->field_0x0 = i;
            sort->field_0x4 = entry->position.z;
        }
        qqsort(work->sortList, work->count, 8, (int (*)(void*, void*))nannpc_zcompare);
        for (i = extGetPoseNum() - 1; i >= 0; i--) {
            s32 id = work->sortList[i].field_0x0;

            GXSetTevColor(1, work->entries[id].color);
            extPoseDraw(id);
        }
        extLoadTextureExit();
    } else {
        extLoadRenderMode();
        extLoadVertex();
        extLoadTexture();
        extLoadTev();
        extDraw();
        extLoadTextureExit();
    }
}

void nannpc_ext_init(void) {
    s32 i;
    ExtPose* pose = extGetPosePtr();

    for (i = extGetPoseNum() - 1; i >= 0; i--) {
        pose->flags = 3;
        pose->field_0x4 = 0.0f;
        PSMTXIdentity(*pose->matrix);
        (*pose->matrix)[0][3] = 0.0f;
        (*pose->matrix)[1][3] = -1000.0f;
        (*pose->matrix)[2][3] = 0.0f;
        pose++;
    }
}

void nannpc_ext_dispent(void) {
    NanNpcWork* work = nanNPCWork;

    if (!(work->flags & 1)) {
        dispEntry(CAMERA_3D, 0.0f, 1, (DispCallback)nannpc_ext_disp, NULL);
        dispEntry(CAMERA_3D, 0.0f, 2, (DispCallback)nannpc_ext_shadow_disp, NULL);
    } else {
        dispEntry(CAMERA_3D, 0.0f, 2, (DispCallback)nannpc_ext_disp, NULL);
        dispEntry(CAMERA_3D, 0.0f, 2, (DispCallback)nannpc_ext_shadow_disp, NULL);
    }
}

// Passing the operands as by-value Vecs reproduces the original's struct-copy staging (frame now
// matches at 0xe0).
static inline HitEntry* hitCheckFilterVec(HitFilterCallback callback, f32* hitX, f32* hitY, f32* hitZ,
                                            f32* distance, f32* hitNX, f32* hitNY, f32* hitNZ, Vec pos, Vec dir) {
    return hitCheckFilter(callback, hitX, hitY, hitZ, distance, hitNX, hitNY, hitNZ, pos.x, pos.y, pos.z, dir.x,
                          dir.y, dir.z);
}

// The first hitCheckFilter's staged pos/velNorm copies land in local spill slots (0x24/0x30) instead
// of the original's outgoing-arg area (0x78/0x84); the second ray's scalar hit buffers are declared
// in reverse (fnz2..fpx2) so their stack slots ascend like the original's.
// NONMATCHING (91.99%): argument staging location and a few callee-saved registers differ.
void nanNPCOption(void) {
    Vec delta;
    Vec velNorm;
    Vec hitNormal;
    Vec reflect;
    Vec pos;
    f32 hitDist;
    f32 fnz2, fny2, fnx2, fpz2, fpy2, fpx2;
    Vec hitPos;
    f32 vlen;
    Vec savedPos;
    NanNpcEntry* other;
    BOOL scale;
    NanNpcEntry* entry;
    Vec move;
    NanNpcWork* work = nanNPCWork;

    for (entry = work->entries; entry < &work->entries[work->count]; entry++) {
        if (!(entry->flags & 4)) {
            continue;
        }
        for (other = work->entries; other < &work->entries[work->count]; other++) {
            if (entry == other) {
                continue;
            }
            if (!(other->flags & 4)) {
                continue;
            }
            vlen = PSVECDistance(&entry->position, &other->position);
            PSVECSubtract(&entry->position, &other->position, &delta);
            if (vlen <= entry->field_0x4C) {
                if (PSVECMag(&delta) == 0.0f) {
                    delta.x = 1.0f;
                }
                PSVECNormalize(&delta, &delta);
                delta.y = 0.0f;
                PSVECScale(&delta, &delta, 0.5f);
                PSVECAdd(&entry->field_0x3C, &delta, &entry->field_0x3C);
                PSVECSubtract(&other->field_0x3C, &delta, &other->field_0x3C);
            }
        }
    }

    for (entry = work->entries; entry < &work->entries[work->count]; entry++) {
        if (!(entry->flags & 4)) {
            continue;
        }
        PSVECSubtract(&entry->homePosition, &entry->position, &delta);
        if (PSVECMag(&delta) > 0.8f * entry->field_0x4C) {
            PSVECNormalize(&delta, &delta);
            delta.y = 0.0f;
            PSVECScale(&delta, &delta, 0.5f);
            PSVECAdd(&entry->field_0x3C, &delta, &entry->field_0x3C);
        }
    }

    for (entry = work->entries; entry < &work->entries[work->count]; entry++) {
        if (!(entry->flags & 4)) {
            continue;
        }
        vlen = PSVECMag(&entry->field_0x3C);
        if (vlen == 0.0f) {
            continue;
        }
        if (vlen > 10.0f) {
            PSVECScale(&entry->field_0x3C, &entry->field_0x3C, 5.0f / vlen);
            vlen = 5.0f;
        }
        pos = entry->position;
        pos.y += 1.0f;
        PSVECNormalize(&entry->field_0x3C, &velNorm);
        if (entry->flags & 8) {
            PSVECAdd(&entry->position, &entry->field_0x3C, &entry->position);
            PSVECScale(&entry->field_0x3C, &entry->field_0x3C, 0.6f);
            continue;
        }
        entry->work[5] = 0;
        entry->work[4] = 0;
        scale = TRUE;
        hitDist = vlen;
        entry->work[4] = (s32)hitCheckFilterVec((HitFilterCallback)0, &hitPos.x, &hitPos.y, &hitPos.z, &hitDist,
                                                &hitNormal.x, &hitNormal.y, &hitNormal.z, pos, velNorm);
        if ((u32)entry->work[4] == 0) {
            PSVECAdd(&entry->position, &entry->field_0x3C, &entry->position);
        } else {
            savedPos = entry->position;
            PSVECAdd(&entry->position, &entry->field_0x3C, &entry->position);
            if (entry->field_0x3C.y == 0.0f) {
                hitNormal.y = 0.0f;
            }
            PSVECNormalize(&hitNormal, &hitNormal);
            C_VECReflect(&velNorm, &hitNormal, &reflect);
            PSVECScale(&reflect, &entry->field_0x3C, vlen);
            scale = FALSE;
            PSVECScale(&hitNormal, &hitNormal, PSVECDotProduct(&velNorm, &hitNormal) * -(vlen - hitDist) + 0.1f);
            PSVECAdd(&entry->position, &hitNormal, &entry->position);
            PSVECSubtract(&entry->position, &savedPos, &move);
            hitDist = PSVECMag(&move);
            if (hitDist > 0.0f) {
                PSVECNormalize(&move, &move);
                entry->work[5] = (s32)hitCheckFilter((HitFilterCallback)0, &fpx2, &fpy2, &fpz2,
                                                     &hitDist, &fnx2, &fny2, &fnz2,
                                                     savedPos.x, savedPos.y + 1.0f, savedPos.z, move.x, move.y,
                                                     move.z);
                if ((u32)entry->work[5] != 0) {
                    entry->position = savedPos;
                }
            }
        }
        if (scale) {
            PSVECScale(&entry->field_0x3C, &entry->field_0x3C, 0.6f);
        }
    }
}
