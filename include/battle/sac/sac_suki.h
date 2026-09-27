#pragma once

typedef struct EventEntry EventEntry;

#include <dolphin/types.h>
void N__disp_2D(s32 type, s32 index);
void main_button(s32 index);
s32 main_suki(EventEntry* event, BOOL isFirstCall);

typedef struct SukiTevParam SukiTevParam;
void set_tev(SukiTevParam* param);
