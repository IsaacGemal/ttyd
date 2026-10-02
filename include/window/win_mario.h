#pragma once

#include "win/win_main.h"

#ifdef MUST_MATCH
// .rodata block that starts with the window's msg_menu_mario_* strings.  The
// Vec/GXColor constants winMarioDisp reads all live past the string tables, so
// the original reaches them through this one base (kept in a register as r31).
// Match-only: the named symbols are used otherwise (see win_mario.c).
typedef struct WinMarioVecs { u8 pad[0x1000]; } WinMarioVecs;
extern WinMarioVecs str_802F5650;
#endif

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
// rodata vectors fukidashi and winMarioDisp read (named form of the vp-relative loads)
extern const Vec vec3_802f5824;
extern const Vec vec3_802f5830;
extern const Vec vec3_802f583c;
extern const Vec vec3_802f5848;
extern const Vec vec3_802f5854;
extern const Vec vec3_802f5860;
extern const Vec vec3_802f586c;
extern const Vec vec3_802f5878;
extern const Vec vec3_802f5884;
extern const Vec vec3_802f5890;
extern const Vec vec3_802f589c;
extern const Vec vec3_802f58a8;
extern const GXColor dat_8041d5d0;
extern const GXColor dat_8041d5d4;
extern const GXColor dat_8041d5d8;
extern const GXColor dat_8041d5dc;
extern const GXColor dat_8041d5e0;
extern const GXColor dat_8041d5e4;

void winMarioDisp(CameraId camId, WindowWork* work, s32 idx);
s32 winMarioMain(WindowWork* work);
void winMarioMain2(WindowWork* work);
void winMarioInit(WindowWork* work);
void winMarioInit2(WindowWork* work);
void winMarioExit(WindowWork* work);
void fukidashi(WindowWork* work, u32 type, f32 x, f32 y);
char* winZenkakuStr(s32 num);
char* unk_8016af5c(s32 num, s32 width);
