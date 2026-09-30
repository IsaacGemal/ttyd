#pragma once
#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"
EffectEntry* effStardust2N64Entry(s32 a, s32 b, s32 c, f32 x, f32 y, f32 z, f32 d, f32 e);
void effStardust2N64SetDrawCam(EffectEntry* entry, s32 cam);
void effStardust2Main(EffectEntry* effect);
void effStardust2Disp(CameraId cameraId, void* param);
// .sdata2 constants used by effStardust2Main (named after their original addresses).
extern const f32 float_neg0p02_8041feb4;
extern const f32 float_0p01_8041feb8;
extern const f32 float_60_8041febc;
extern const f32 float_360_8041fec0;
extern const f32 float_120_8041fec4;
extern const f32 float_6p2832_8041fec8;
extern const f32 float_neg0p05_8041fecc;
extern const f32 float_0p96_8041fed0;
// constants used by effStardust2N64Entry (data externs; the definitions are at the bottom of the unit)
extern const f32 float_0p4_8041fed4;
extern const f32 float_10_8041fed8;
extern const f32 float_90_8041fedc;
extern const f32 float_0p03_8041fee0;
extern const f32 float_0p04_8041fee4;
extern const f32 float_0p005_8041fee8;
extern const Vec vec3_802fb740;
extern const f64 double_to_int_mask_802fb750;
extern const f64 double_to_int_802fb758;
