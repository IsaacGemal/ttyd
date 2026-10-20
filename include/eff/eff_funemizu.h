#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
// The unit's .sdata tables: the section attribute makes MWCC emit the original's r13 (@sda21) addressing.
extern __declspec(section ".sdata") u8 funemizu_normal_tbl[32];
extern __declspec(section ".sdata") u8 funemizu_color0_tbl[32];
extern __declspec(section ".sdata") u8* funemizu_dl_0_tbl[3];
extern __declspec(section ".sdata") u8 funemizu_dl_0_size_tbl[4];
extern __declspec(section ".sdata") u8* funemizu_dl_1_tbl[4];
extern __declspec(section ".sdata") u8 funemizu_dl_1_size_tbl[4];
extern __declspec(section ".sdata") u8 gor_fune_normal_tbl[32];
extern __declspec(section ".sdata") u8 gor_fune_color0_tbl[8];
extern __declspec(section ".sdata") u8* gor_fune_dl_0_tbl[2];
extern __declspec(section ".sdata") u8 gor_fune_dl_0_size_tbl[4];
extern __declspec(section ".sdata") u8* gor_fune_dl_1_tbl[2];
extern __declspec(section ".sdata") u8 gor_fune_dl_1_size_tbl[4];
EffectEntry* effFunemizuEntry(s32 type, f32 a, f32 b, f32 c, f32 d, f32 e);
u32 unk_80224298(void);
#include "driver/camdrv.h"
void effFunemizuMain(EffectEntry* effect);
void effFunemizuShibukiDisp(CameraId camId, void* param);
void effFunemizuDisp(CameraId camId, void* param);
typedef struct EffFunemizuData {
	s32 type;       // 0x0
	f32 x;          // 0x4
	f32 y;          // 0x8
	f32 z;          // 0xC
	f32 speed;      // 0x10: droplet horizontal speed (Entry: d * 0.5); emitter strength, clamped to 0..1 for the layers
	f32 yaw;        // 0x14: heading in degrees; minus the camera's viewYaw for the draw and the droplet direction
	f32 scaleA;     // 0x18: layer A X/Z scale, eased toward clamp01(speed)
	f32 scaleB;     // 0x1C: layer B X/Z scale, eased toward clamp01(speed - 1)
	f32 scaleAY;    // 0x20: layer A Y scale (same ease as scaleA, half rate when speed <= 1)
	f32 scaleBY;    // 0x24: layer B Y scale
	f32 texScrollA; // 0x28: layer A texture scroll, += 0.05 * scaleA per frame
	f32 texScrollB; // 0x2C: layer B texture scroll, += 0.1 * scaleB per frame
	s32 timer;      // 0x30: frame counter, wraps at 1000; droplets spawn when it is a multiple of 10
	f32 velY;       // 0x34: droplet vertical velocity
	f32 gravity;    // 0x38: subtracted from velY each frame
	s32 delay;      // 0x3C: frames before a droplet starts moving
	s32 alpha;      // 0x40: droplet alpha (fades by 8 per frame, floor 0)
	f32 scale;      // 0x44: overall scale (1.0)
	s32 camId;      // 0x48: camera the effect draws into (4)
} EffFunemizuData;
#include <dolphin/mtx.h>
