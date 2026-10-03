#pragma once
#include <dolphin/types.h>
// The original calcCamRoad reads a seventh float argument in f7; camRoadMain leaves its own
// seventh float argument (bank) live there across the call. The old prototype in cam_road.h only
// declares six floats, so cam_road.c renames that declaration away and uses this exact one.
struct CamRoadState;
void calcCamRoad(struct CamRoadState* a, struct CamRoadState* b, f32 x, f32 y, f32 z, f32 ax, f32 ay, f32 az, f32 bank);
