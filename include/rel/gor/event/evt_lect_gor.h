#pragma once

#include <dolphin/mtx.h>
#include <dolphin/types.h>

// The original gor call site zero-extends iconId (`clrlwi r5, r5, 16`) before the call, so
// this unit sees iconDispGxAlpha with a u16 iconId (the global icondrv.h declares s16).
void iconDispGxAlpha(Vec* position, s16 flags, u16 iconId, u8 alpha, f32 scale);
void iconFlagOn(const char* name, s16 flags);
void iconFlagOff(const char* name, s16 mask);
