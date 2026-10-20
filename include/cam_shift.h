#pragma once

#include <dolphin/types.h>

struct CameraEntry;
struct MarioWork;
void camShiftMain(struct CameraEntry* camera, struct MarioWork* mario, f32* out);
void camShiftPostMain(void);
void camShiftReset(void);
void camShiftInit(void);
// cam_shift.c work struct (.bss "work", pointed to by the file-local "wp").
typedef struct CamShiftWork {
	/* 0x00 */ f32 bank;           // current bank, written to the caller
	/* 0x04 */ f32 targetBank;     // road shift X rate the bank is easing towards
	/* 0x08 */ f32 stepScale;      // 0..1 scale on the remaining bank delta
	/* 0x0C */ f32 stickAccum;     // accumulated sideways stick input
	/* 0x10 */ f32 scaleActive;    // stepScale is ramping up
	/* 0x14 */ u8 pad_14[0x4];
	/* 0x18 */ s64 prevTime;       // renderTime of the previous frame
	/* 0x20 */ f32 stickAccumMax;  // clamp for stickAccum
	/* 0x24 */ f32 maxStep;        // clamp for one frame's bank change
	/* 0x28 */ f32 scaleRate;      // stepScale increment per frame
	/* 0x2C */ f32 lastMarioX;
	/* 0x30 */ f32 lastMarioZ;
} CamShiftWork;
