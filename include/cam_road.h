#pragma once

#include <dolphin/types.h>

BOOL camRoadSetup(const char* filename);
void camRoadMain(f32* dst, f32 x, f32 y, f32 z, f32 ax, f32 ay, f32 az, f32 bank);
void camRoadReset(void);
