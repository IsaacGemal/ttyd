#pragma once

#include "win/win_main.h"

// .sdata2/.sdata data this unit reads from elsewhere
extern const GXColor winWhiteColor;
extern const GXColor winBlackColor;
extern const char* hammer_pose[4];
extern const GXColor fukiShadowColor;
extern const GXColor fukiBubbleColor;
extern const GXColor fukiIconColor;
extern const GXColor fukiShadowColorR;
extern const GXColor fukiBubbleColorR;
extern const GXColor fukiIconColorR;

void winMarioDisp(CameraId camId, void* param, s32 idx);
s32 winMarioMain(WindowWork* work);
void winMarioMain2(WindowWork* work);
void winMarioInit(WindowWork* work);
void winMarioInit2(WindowWork* work);
void winMarioExit(WindowWork* work);
void fukidashi(WindowWork* work, u32 type, f32 x, f32 y);
char* winZenkakuStr(s32 num);
char* winZenkakuPadStr(s32 num, s32 width);
