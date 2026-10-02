#pragma once

typedef struct EventEntry EventEntry;

#include <dolphin/types.h>

extern const f32 float_2_80421fa4;
s32 main_muki(EventEntry* event, BOOL isFirstCall);
s32 end_muki(EventEntry* event, BOOL isFirstCall);
void main_icon(s32 index);
void main_point(s32 index);
void main_cursor(void);
