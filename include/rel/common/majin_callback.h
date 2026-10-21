// majin_callback, shared by gon_06 and tik_19 (dou_07's and usu_01's copies animate other map objects). Included where
// it sits in the unit's function order (it sets the .text layout).
#include "driver/animdrv.h"
#include "driver/mobjdrv.h"

static void majin_callback(s32 state) {
    MapObjectEntry* entry = mobjNameToPtr("box");

    if (state == 0) {
        animPoseSetAnim(entry->poseId, "A_2", 1);
    }
    else if (state == 5) {
        if (animPoseGetLoopTimes(entry->poseId) >= 1.0f) {
            animPoseSetAnim(entry->poseId, "A_2", 1);
        }
    }
    else if (state == 2) {
        animPoseSetAnim(entry->poseId, "S_1", 1);
    }
}
