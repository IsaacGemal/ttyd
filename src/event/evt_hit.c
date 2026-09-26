/* "evt_hit" - Hit-related Event Script Functions
 * Status: In progress.
 */
#include <string.h>

#include <dolphin/types.h>
#include <dolphin/mtx.h>

#include "driver/hitdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"

// Prototypes not yet in a shared header (all defined in src/driver/hitdrv.c).
void hitObjAttrOn(const char* name, s32 attr);
void hitObjAttrOff(const char* name, s32 attr);
void hitGrpAttrOn(const char* name, s32 attr);
void hitGrpAttrOff(const char* name, s32 attr);
void hitObjFlagOff(const char* name, s32 flag);
void hitGrpFlagOn(const char* name, s32 flag);
void hitGrpFlagOff(const char* name, s32 flag);
void hitBindUpdate(const char* name);

// Defined below (mutually recursive with evt_hitobj_name_callback).
void name_callback_sub(const char* name, void* script, s32 recurse);

// .sbss
HitDamageReturn* hitDmgretPtr;

USER_FUNC(evt_hitobj_onoff) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 group = evtGetValue(event, args[1]);
    s32 on = evtGetValue(event, args[2]);

    if (group == 0) {
        if (on != 0) {
            hitObjFlagOff(name, 1);
        } else {
            hitObjFlagOn(name, 1);
        }
    } else {
        if (on != 0) {
            hitGrpFlagOff(name, 1);
        } else {
            hitGrpFlagOn(name, 1);
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_hit_damage_return_set) {
    s32* args = event->args;
    HitDamageReturn* damage = (HitDamageReturn*)evtGetValue(event, args[0]);
    s32 count;
    HitDamageReturn* p;

    if (isFirstCall != 0) {
        for (p = damage, count = 1;; count++) {
            if (p->mapobjName == NULL) {
                break;
            }
            p++;
        }
        p = hitDmgretPtr = _mapAlloc(count << 4);
        memcpy(p, damage, count << 4);

        for (;; p++) {
            if (p->mapobjName == NULL) {
                break;
            }
            if (p->position.y == -1000.0f) {
                hitObjGetPos(p->mapobjName, &p->position);
            }
            hitGrpDamageReturnSet(p->mapobjName, p);
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_hit_bind_mapobj) {
    s32* args = event->args;
    s32 hitName = evtGetValue(event, args[0]);
    s32 mapObjName = evtGetValue(event, args[1]);

    hitBindMapObj((const char*)hitName, (const char*)mapObjName);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_hit_bind_update) {
    s32* args = event->args;

    hitBindUpdate((const char*)evtGetValue(event, args[0]));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_hit_get_position) {
    s32* args = event->args;
    Vec pos;
    const char* name = (const char*)evtGetValue(event, args[0]);

    hitObjGetPos(name, &pos);
    evtSetFloat(event, args[1], pos.x);
    evtSetFloat(event, args[2], pos.y);
    evtSetFloat(event, args[3], pos.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(L_evt_hitobj_flag_onoff) {
    s32* args = event->args;
    s32 group = evtGetValue(event, args[0]);
    s32 on = evtGetValue(event, args[1]);
    const char* name = (const char*)evtGetValue(event, args[2]);
    s32 flag = args[3];

    if (group == 0) {
        if (on == 0) {
            hitObjFlagOff(name, (u16)flag);
        } else {
            hitObjFlagOn(name, (u16)flag);
        }
    } else {
        if (on == 0) {
            hitGrpFlagOff(name, (u16)flag);
        } else {
            hitGrpFlagOn(name, (u16)flag);
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_hitobj_attr_onoff) {
    s32* args = event->args;
    s32 group = evtGetValue(event, args[0]);
    s32 on = evtGetValue(event, args[1]);
    const char* name = (const char*)evtGetValue(event, args[2]);
    s32 attr = args[3];

    if (group == 0) {
        if (on == 0) {
            hitObjAttrOff(name, attr);
        } else {
            hitObjAttrOn(name, attr);
        }
    } else {
        if (on == 0) {
            hitGrpAttrOff(name, attr);
        } else {
            hitGrpAttrOn(name, attr);
        }
    }
    return EVT_RETURN_DONE;
}

void name_callback_sub(const char* name, void* script, s32 recurse) {
    HitEntry* hit = hitNameToPtr(name);

    if (hit == NULL) {
        return;
    }
    evtEntry(script, 0, 0x20)->lwData[0] = (s32)name;
    if (hit->child != NULL) {
        name_callback_sub(hit->child->joint->name, script, 1);
    }
    if (recurse != 0 && hit->sibling != NULL) {
        name_callback_sub(hit->sibling->joint->name, script, 1);
    }
}

USER_FUNC(evt_hitobj_name_callback) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 script = evtGetValue(event, args[1]);

    name_callback_sub((const char*)name, (void*)script, 0);
    return EVT_RETURN_DONE;
}
