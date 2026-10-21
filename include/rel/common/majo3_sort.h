// majo3_sort, shared by the two Beldam-sisters units (unit_boss_majolyne in win, unit_boss_majolyne_las). Included where
// it sits in the unit's function order (it sets the .text layout).
#include "evt/evt_cmd.h"

typedef struct Majo3SortEntry {
    s32 id;
    s32 hp;
} Majo3SortEntry;

static USER_FUNC(majo3_sort) {
    Majo3SortEntry entry[3];
    Majo3SortEntry tmp;
    s32 i;

    for (i = 0; i < 3; i++) {
        entry[i].id = event->lwData[i + 3];
        entry[i].hp = event->lwData[i];
    }
    if (entry[0].hp < entry[1].hp) {
        tmp = entry[0];
        entry[0] = entry[1];
        entry[1] = tmp;
    }
    if (entry[1].hp < entry[2].hp) {
        tmp = entry[1];
        entry[1] = entry[2];
        entry[2] = tmp;
    }
    if (entry[0].hp < entry[1].hp) {
        tmp = entry[0];
        entry[0] = entry[1];
        entry[1] = tmp;
    }
    for (i = 0; i < 3; i++) {
        event->lwData[i] = entry[i].id;
    }
    return EVT_RETURN_DONE;
}
