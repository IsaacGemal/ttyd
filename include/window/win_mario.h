#pragma once

#include "win/win_main.h"

// .rodata block that starts with the window's msg_menu_mario_* strings.  The
// Vec/GXColor constants winMarioDisp reads all live past the string tables, so
// they are reached through this one base (kept in a register as r31).
typedef struct WinMarioVecs { u8 pad[0x1000]; } WinMarioVecs;
extern WinMarioVecs str_802F5650;

// .sdata2/.sdata data this unit reads from elsewhere
extern const GXColor dat_8041d5e8;
extern const GXColor dat_8041d5ec;
extern const GXColor dat_8041d608;
extern const char str_8041D630[4];
extern const char str_8041D638[4];
extern const char str_8041D63C[4];
extern const char str_8041D6DC[5];
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
