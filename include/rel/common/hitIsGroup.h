// _hitIsGroupSub and hitIsGroup, shared by aji_13, aji_19 and dou_11. Included where they sit in the unit's function
// order (it sets the .text layout).
#include "driver/hitdrv.h"

#include <string.h>

static BOOL _hitIsGroupSub(HitEntry* group, HitEntry* hit, BOOL recurse) {
    if (strcmp(group->joint->name, hit->joint->name) == 0) {
        return TRUE;
    }
    if (hit->child != NULL) {
        return _hitIsGroupSub(group, hit->child, TRUE);
    }
    if (recurse && hit->sibling != NULL) {
        return _hitIsGroupSub(group, hit->sibling, TRUE);
    }
    return FALSE;
}

static BOOL hitIsGroup(HitEntry* group, HitEntry* hit) {
    return _hitIsGroupSub(group, hit, FALSE);
}
