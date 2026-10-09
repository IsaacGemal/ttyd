#pragma once

#include "win/win_main.h"

// .sdata2/.sdata data this unit reads from elsewhere
extern const GXColor dat_8041d5e8;
extern const GXColor dat_8041d5ec;
extern const char* hammer_pose[4];
extern const GXColor dat_8041d5d0;
extern const GXColor dat_8041d5d4;
extern const GXColor dat_8041d5d8;
extern const GXColor dat_8041d5dc;
extern const GXColor dat_8041d5e0;
extern const GXColor dat_8041d5e4;

void winMarioDisp(CameraId camId, void* param, s32 idx);
s32 winMarioMain(WindowWork* work);
void winMarioMain2(WindowWork* work);
void winMarioInit(WindowWork* work);
void winMarioInit2(WindowWork* work);
void winMarioExit(WindowWork* work);
void fukidashi(WindowWork* work, u32 type, f32 x, f32 y);
char* winZenkakuStr(s32 num);
char* unk_8016af5c(s32 num, s32 width);
