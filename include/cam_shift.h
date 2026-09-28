#pragma once

#include <dolphin/types.h>

struct CameraEntry;
struct MarioWork;
void camShiftMain(struct CameraEntry* camera, struct MarioWork* mario, f32* out);
void camShiftPostMain(void);
void camShiftReset(void);
void camShiftInit(void);
