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
s32 mobj_powerupblk(MapObjectEntry* obj);
s32 mobj_save_blk(MapObjectEntry* obj);
s32 mobj_koopa_ojama_blk_evt(MapObjectEntry* obj);
s32 mobj_koopa_badgeblk(MapObjectEntry* obj);
s32 mobj_badgeblk(MapObjectEntry* obj);
s32 mobj_lv_blk_evt(MapObjectEntry* obj);
s32 mobj_recovery_blk(MapObjectEntry* obj);
s32 mobj_koopa_pole_evt(MapObjectEntry* obj);
s32 mobj_koopa_fireber_dodai_evt(MapObjectEntry* obj);
s32 mobj_koopa_sango_evt(MapObjectEntry* obj);
s32 mobj_koopa_dokan_evt(MapObjectEntry* obj);
s32 mobj_koopa_brickblk(MapObjectEntry* obj);
s32 mobj_kururing_floor(MapObjectEntry* obj);
s32 mobj_breaking_floor(MapObjectEntry* obj);
s32 mobj_10countblk(MapObjectEntry* obj);
s32 mobj_brickblk(MapObjectEntry* obj);
s32 mobj_itembox(MapObjectEntry* obj);
s32 mobj_switch_float_blk(MapObjectEntry* obj);
s32 mobj_timerswitch(MapObjectEntry* obj);
void kururing_capture(CameraId cameraId, void* param);
