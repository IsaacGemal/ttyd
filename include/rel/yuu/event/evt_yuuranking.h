#pragma once

#include <dolphin/mtx.h>
#include <dolphin/gx/GXStruct.h>

// Match-only by-pointer colour parameters for evt_yuuranking_disp_func. The target coalesces each by-value
// GXColor into the callee's parameter slot; declaring the colour as a pointer makes MWCC pass the local's
// address instead of copying it, which reproduces the original frame. The callee ABI is identical (r4 is a
// pointer either way), so windowdrv.h/icondrv.h's by-value prototypes stay untouched for every other caller.
void windowDispGX_Waku_col(s32 texMap, GXColor* color, f32 x, f32 y, f32 w, f32 h, f32 curve);
void iconDispGxCol(Mtx pos, s16 flags, s32 iconId, GXColor* color);
s32 iconNumberDispGx(Mtx mtx, s32 number, BOOL small, GXColor* color);
