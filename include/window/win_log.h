#pragma once

#include <dolphin/types.h>
#include "battle/battle_unit.h"

// Tattle ("monosiri") viewer window (win_log in the original; no source file yet).
void* monosiriInit(BattleUnitType kind, f32 x, f32 y);
void monosiriMain(void* work);
void monosiriClose(void* work);
BOOL monosiriCloseWait(void* work);
void monosiriExit(void* work);
#include "driver/dispdrv.h"
void monosiri_disp(CameraId camId, void* param);
void capture(CameraId camId, void* param);
typedef struct WindowWork WindowWork;

void monoshiriGX(WindowWork* work, f32 x, f32 y);
void winLogDisp(CameraId camId, void* param, s32 idx);
void mapGX(WindowWork* work, f32 x, f32 y);
const char* winGetMapTplName(void);

void winLogMain2(WindowWork* work);
s32 winLogMain(WindowWork* work);
void winLogExit(WindowWork* work);
void winLogInit2(WindowWork* work);
void winLogInit(WindowWork* work);

// The win_log unit's read-only rodata base. Match-only non-const declaration: it stops MWCC hoisting the
// vec3 loads out of monoshiriGX's and winLogDisp's loops (the data is in fact read-only).
#ifdef MUST_MATCH
extern char str_msg_menu_kiroku_map[];
#else
extern const char str_msg_menu_kiroku_map[];
#endif

// The 4-byte "S_1" anim name in .sdata2, addressed with @sda21 by the original. Declared with its real size
// so MWCC treats it as small data; an unsized array is loaded with lis/addi instead.
extern const char str_8041D810[4];
