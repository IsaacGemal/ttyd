#pragma once

#include <dolphin/mtx.h>

// Not in icondrv.h: the original's callers disagree on these signatures (see the iconDispGx and iconDispGxAlpha
// entries in tools/local_prototype_exceptions.json), so a unit that saw another one declares it locally instead.
void iconDispGx(f32 scale, Vec* position, s16 flags, s16 iconId);
void iconDispGxAlpha(Vec* position, s16 flags, s16 iconId, u8 alpha, f32 scale);
