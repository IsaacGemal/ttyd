// lect_map_cam_chg_sub and lect_map_cam_chg. Lecture helpers: see rel/common/evt_lect.h.
#include "driver/mapdrv.h"
#include "evt/evt_cmd.h"

static void lect_map_cam_chg_sub(MapObject* obj, s32 camId, BOOL recurse) {
    obj->cameraId = camId;
    if (obj->child != NULL) {
        lect_map_cam_chg_sub(obj->child, camId, TRUE);
    }
    if (recurse && obj->next != NULL) {
        lect_map_cam_chg_sub(obj->next, camId, TRUE);
    }
}

static USER_FUNC(lect_map_cam_chg) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 camId = evtGetValue(event, args[1]);
    MapObject* obj = mapGetMapObj((const char*)name);

    if (obj != NULL) {
        lect_map_cam_chg_sub(obj, camId, FALSE);
    }
    return EVT_RETURN_DONE;
}
