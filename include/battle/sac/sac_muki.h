#pragma once

typedef struct EventEntry EventEntry;

#include <dolphin/types.h>
s32 main_muki(EventEntry* event, BOOL isFirstCall);
s32 end_muki(EventEntry* event, BOOL isFirstCall);
void main_icon(s32 index);
void main_point(s32 index);
void main_cursor(void);
