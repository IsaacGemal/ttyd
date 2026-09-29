#pragma once

#include <dolphin/types.h>

BOOL camRoadSetup(const char* filename);
void camRoadMain(f32* dst, f32 x, f32 y, f32 z, f32 ax, f32 ay, f32 az, f32 bank);
void camRoadReset(void);
struct CamRoadState;
u32 compareCamRoad(struct CamRoadState* cur, struct CamRoadState* prev, f32* out);
// The original reads a seventh float in f7 on entry, which its only caller (camRoadMain) never sets.
void calcCamRoad(struct CamRoadState* a, struct CamRoadState* b, f32 x, f32 y, f32 z, f32 ax, f32 ay, f32 az);
void calcHokanCamRoad(s32 flag, struct CamRoadState* a, struct CamRoadState* b, f32 t, struct CamRoadState* c, struct CamRoadState* out);
void calcCurrentCamState(f32* dst);
void camRoadCurveOn(const char* name);
void camRoadCurveOff(const char* name);
