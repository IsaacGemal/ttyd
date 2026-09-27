#pragma once

typedef struct EventEntry EventEntry;

typedef struct MapObjectEntry MapObjectEntry;

#include "driver/camdrv.h"

#include <dolphin/types.h>
void kpa_disp_pole_score(CameraId camera, void* param);
s32 mobj_signboard(MapObjectEntry* obj);
s32 mobj_lock(MapObjectEntry* obj);
s32 mobj_switch_red(MapObjectEntry* obj);
s32 mobj_switch_blue(MapObjectEntry* obj);
s32 mobj_jumpstand_blue(MapObjectEntry* obj);
s32 mobj_jumpstand_red(MapObjectEntry* obj);
s32 mobj_rideswitch_lightblue(MapObjectEntry* obj);
s32 mobj_koopa_blk(MapObjectEntry* obj);
s32 mobj_float_blk(MapObjectEntry* obj);
s32 fire_func(EventEntry* event, BOOL isFirstCall);
