#include <dolphin/types.h>

#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "evt/evt_urouro.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "system.h"

// Work structure allocated by urouro_init_func, held in event->lwData[10].
// Not declared in a header yet.
typedef struct UrouroWork {
    /* 0x00 */ s32 flags;
    /* 0x04 */ s32 mode;
    /* 0x08 */ s32 timer;
    /* 0x0C */ s32 rotateCount;
    /* 0x10 */ s32 field_0x10;
    /* 0x14 */ s32 field_0x14;
    /* 0x18 */ s32 field_0x18;
    /* 0x1C */ f32 field_0x1C;
    /* 0x20 */ f32 field_0x20;
    /* 0x24 */ f32 field_0x24;
    /* 0x28 */ f32 field_0x28;
} UrouroWork; // 0x2C

USER_FUNC(urouro_init_func) {
    s32* args = event->args;
    NpcEntry* npc;
    UrouroWork* work;
    s32 name;
    f32 a;
    f32 b;
    f32 c;
    f32 d;
    s32 flags;

    name = evtGetValue(event, args[0]);
    npc = evtNpcNameToPtr(event, (const char*)name);
    a = evtGetFloat(event, args[1]);
    b = evtGetFloat(event, args[2]);
    c = evtGetFloat(event, args[3]);
    d = evtGetFloat(event, args[4]);
    flags = evtGetValue(event, args[5]);

    event->lwData[10] = (s32)_mapAlloc(sizeof(UrouroWork));
    work = (UrouroWork*)event->lwData[10];
    work->flags = flags;
    work->mode = 0;
    work->timer = 0;
    work->rotateCount = 0;
    work->field_0x10 = sysMsec2Frame(6000);
    work->field_0x14 = sysMsec2Frame(9000);
    work->field_0x18 = sysMsec2Frame(1500);
    work->field_0x1C = a;
    work->field_0x20 = npc->position.y;
    work->field_0x24 = b;
    work->field_0x28 = c;
    npc->moveSpeed = d;
    npc->flags |= 0x400000;
    return EVT_RETURN_DONE;
}
