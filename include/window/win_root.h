#pragma once

#include "driver/camdrv.h"
#include "sdk/texPalette.h"
typedef struct WindowWork WindowWork;

#include <dolphin/types.h>
void winRootDisp(CameraId camId, WindowWork* work);
s32 winRootMain(WindowWork* work);
void winBgMain(WindowWork* work);
void winMsgMain(WindowWork* work);
void winSortMain(WindowWork* work);
void winRootFbDraw(s32 unused, WindowWork* work);
void winRootFbCopy(s32 unused, WindowWork* work);
int compare_func1(const void* a, const void* b);
int compare_func1_r(const void* a, const void* b);
int compare_func2(const void* a, const void* b);
int compare_func2_r(const void* a, const void* b);
int compare_func3(const void* a, const void* b);
int compare_func3_r(const void* a, const void* b);
int compare_func4(const void* a, const void* b);
int compare_func4_r(const void* a, const void* b);
int compare_func5(const void* a, const void* b);
int compare_func5_r(const void* a, const void* b);
int compare_func6_1(const void* a, const void* b);
int compare_func6_1_r(const void* a, const void* b);
int compare_func6_2(const void* a, const void* b);
int compare_func6_2_r(const void* a, const void* b);
int compare_func5_1(const void* a, const void* b);
int compare_func5_1_r(const void* a, const void* b);
int compare_func5_2(const void* a, const void* b);
int compare_func5_2_r(const void* a, const void* b);
int N_compare_func4_1(const void* a, const void* b);
int N_compare_func4_1_r(const void* a, const void* b);
int N_compare_func4_2(const void* a, const void* b);
int N_compare_func4_2_r(const void* a, const void* b);
int compare_func5_2(const void* a, const void* b);
int compare_func6_2(const void* a, const void* b);
void winMsgEntry(WindowWork* work, s32 a1, const char* str, const char* a3);
void winHalfBookGX(TEXPalette* pal, f32 x, f32 y, s32 a4);
void winZClear(void);
void winBgGX(WindowWork* work, s32 type, f32 x, f32 y);
void winNameGX(WindowWork* work, s32 type, f32 x, f32 y, f32 w, f32 h);
void winKirinukiGX(WindowWork* work, s32 type, f32 x, f32 y, f32 w, f32 h);
void winKageGX(WindowWork* work, f32 x, f32 y, f32 w, f32 h, GXColor* color);
void winWazaGX(WindowWork* work, s32 type, f32 x, f32 y, f32 w, f32 h);
void winSortEntry(WindowWork* work, s32 idx, f32 a, f32 b);
s32 winSortWait(WindowWork* work);
void winMailGX(WindowWork* work, f32 x, f32 y);
void winMailDisp(WindowWork* work);
void winHakoGX(WindowWork* work, s32 type, f32 x, f32 y);
void winMsgDisp(WindowWork* work, f32 x, f32 y);
void winSortGX(WindowWork* work);
void winBookGX(WindowWork* work, s32 type, f32 x, f32 y);
// Menu-display rodata block that starts at the msg_menu_* strings.  winRootDisp
// reads the Vec constants at the listed offsets through this one base so MWCC
// emits the same single lis/addi + offset loads as the original.
typedef struct WinRootVecs {
    u8 pad[0x54]; // 0x00: msg_menu_* strings
    Vec v0;       // 0x54
    Vec v1;       // 0x60
    Vec v2;       // 0x6C
    Vec v3;       // 0x78
    Vec v4;       // 0x84
    Vec v5;       // 0x90
} WinRootVecs;
