#pragma once

#include <dolphin/mtx.h>

// Not in icondrv.h: the original's callers disagree on this signature (see the iconDispGxAlpha entry in
// tools/local_prototype_exceptions.json), so a unit that saw another one declares it locally instead.
void iconDispGxAlpha(Vec* position, s16 flags, s16 iconId, u8 alpha, f32 scale);
