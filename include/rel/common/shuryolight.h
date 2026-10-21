// shuryolight_callback, shuryolightSetAnim and shuryolightRunEvt, shared by the evt_shuryolight units of aji and las
// (rel/common/evt_shuryolight.h). Included where they sit in the unit's function order (it sets the .text layout);
// the unit defines `ShuryolightWork* slhp` before it.
#include "driver/animdrv.h"
#include "manager/evtmgr.h"
#include "rel/common/evt_shuryolight.h"

// Display callback for the light pose: find the entry whose group is being drawn and draw its plane shape.
static void shuryolight_callback(void* userData, s32 groupId, Mtx mtx) {
    ShuryolightEntry* entries = userData;
    s32 i;

    for (i = 0; i < 5; i++) {
        if (groupId == entries[i].groupIdx) {
            break;
        }
    }
    if (i != 5) {
        animPoseMain(entries[i].npc->poseId);
        animPoseDrawShape(entries[i].npc->poseId, entries[i].shapeIdx);
    }
}

static void shuryolightSetAnim(s32 index, s32 anim) {
    ShuryolightEntry* entry = &slhp->entries[index];

    entry->animIdx = anim;
    animPoseSetAnim(entry->npc->poseId, entry->animTable[anim], TRUE);
}

static s32 shuryolightRunEvt(void* evtCode, s32 id) {
    if (evtCheckID(id)) {
        evtDeleteID(id);
    }
    return evtEntryType(evtCode, 0, 0, 0)->eventId;
}
