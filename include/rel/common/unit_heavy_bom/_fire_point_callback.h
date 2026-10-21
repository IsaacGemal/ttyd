// _fire_point_callback, shared by unit_heavy_bom and unit_giant_bomb: the display callback _set_draw_callback installs
// (animdrv calls it with the user data, the group being drawn and its matrix). Included where it sits in the unit's
// function order (it sets the .text layout).
#include "battle/battle_unit.h"
#include "driver/animdrv.h"
#include <string.h>

static void _fire_point_callback(void* userData, s32 group, Mtx mtx) {
    BattleUnitPart* part = userData;
    BattleUnit* unit;
    const char* name;
    s32 pose;
    s32 poseId;
    s32 shape;

    unit = part->mOwner;
    if (unit->work[0] != 0) {
        pose = part->poseId;
        switch (unit->work[1]) {
            case 0:
                name = "locator11";
                break;
            case 1:
                name = "locator10";
                break;
            case 2:
                name = "locator9";
                break;
            case 3:
            default:
                name = "locator8";
                break;
        }
        if (strcmp(name, animPoseGetGroupName(pose, group)) == 0) {
            poseId = BtlUnit_GetPartsPtr(unit, 2)->poseId;
            shape = animPoseGetShapeIdx(poseId, "pPlaneShape3");
            animPoseDrawShape(poseId, shape);
        }
    }
}
