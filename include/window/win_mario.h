#pragma once

#include "win/win_main.h"

void winMarioDisp(CameraId camId, WindowWork* work, s32 idx);
s32 winMarioMain(WindowWork* work);
void winMarioMain2(WindowWork* work);
void winMarioInit(WindowWork* work);
void winMarioInit2(WindowWork* work);
void winMarioExit(WindowWork* work);
void fukidashi(WindowWork* work, u32 type, f32 x, f32 y);
char* winZenkakuStr(s32 num);
char* unk_8016af5c(s32 num, s32 width);
