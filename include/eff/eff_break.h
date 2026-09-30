#pragma once
#include "driver/camdrv.h"
#include "driver/effdrv.h"
EffectEntry* effBreakEntry(s32 type, s32 a2, f32 x, f32 y, f32 z);
void effBreakMain(EffectEntry* effect);
void effBreakDisp(CameraId cameraId, void* param);
extern const f32 float_deg2rad_804210a8;
extern const f32 float_48_804210ac;
extern const f32 float_0p5_804210b4;
extern const f32 float_24_804210b8;
// The original rematerialises the 1.0 texcoord from its .sdata2 pool on every vertex. A plain
// `extern const f32` makes MWCC hoist it into a callee-saved FPR; volatile keeps the reload.
#ifdef MUST_MATCH
extern const volatile f32 float_1_804210bc;
#else
extern const f32 float_1_804210bc;
#endif
