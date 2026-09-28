#pragma once

#include "driver/camdrv.h"
typedef struct WindowWork WindowWork;

#include <dolphin/types.h>
void winRootDisp(CameraId camId, WindowWork* work);
int compare_func1(void* a, void* b);
int compare_func1_r(void* a, void* b);
int compare_func2(void* a, void* b);
int compare_func2_r(void* a, void* b);
int compare_func3(void* a, void* b);
int compare_func3_r(void* a, void* b);
int compare_func4(void* a, void* b);
int compare_func4_r(void* a, void* b);
int compare_func5(void* a, void* b);
int compare_func5_r(void* a, void* b);
int compare_func6_1(void* a, void* b);
int compare_func6_1_r(void* a, void* b);
int compare_func6_2(void* a, void* b);
int compare_func6_2_r(void* a, void* b);
int compare_func5_1(void* a, void* b);
int compare_func5_1_r(void* a, void* b);
int compare_func5_2(void* a, void* b);
int compare_func5_2_r(void* a, void* b);
int N_compare_func4_1(void* a, void* b);
int N_compare_func4_1_r(void* a, void* b);
int N_compare_func4_2(void* a, void* b);
int N_compare_func4_2_r(void* a, void* b);
void winMsgEntry(WindowWork* work, s32 a1, const char* str, s32 a3);
void winHalfBookGX(void* pal, f32 x, f32 y, s32 a4);
void winZClear(void);
void winBgGX(WindowWork* work, s32 type, f32 x, f32 y);
void winNameGX(WindowWork* work, s32 type, f32 x, f32 y, f32 w, f32 h);
void winKirinukiGX(WindowWork* work, s32 type, f32 x, f32 y, f32 w, f32 h);
void winKageGX(WindowWork* work, void* color, f32 x, f32 y, f32 w, f32 h);
void winWazaGX(WindowWork* work, s32 type, f32 x, f32 y, f32 w, f32 h);
