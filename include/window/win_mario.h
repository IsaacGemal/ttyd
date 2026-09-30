#pragma once

#include "win/win_main.h"

// .sdata2/.sdata data this unit reads from elsewhere
extern const char* hammer_pose[4];
extern const f32 float_10_8041d61c;
extern const f32 float_12_8041d670;

void winMarioDisp(CameraId camId, WindowWork* work, s32 idx);
s32 winMarioMain(WindowWork* work);
void winMarioMain2(WindowWork* work);
void winMarioInit(WindowWork* work);
void winMarioInit2(WindowWork* work);
void winMarioExit(WindowWork* work);
void fukidashi(WindowWork* work, u32 type, f32 x, f32 y);
char* winZenkakuStr(s32 num);
char* unk_8016af5c(s32 num, s32 width);
