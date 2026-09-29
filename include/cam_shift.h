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
	/* 0x00 */ f32 unk_00;
	/* 0x04 */ f32 unk_04;
	/* 0x08 */ f32 unk_08;
	/* 0x0C */ f32 unk_0C;
	/* 0x10 */ f32 unk_10;
	/* 0x14 */ u8 pad_14[0x4];
	/* 0x18 */ s64 unk_18;
	/* 0x20 */ f32 unk_20;
	/* 0x24 */ f32 unk_24;
	/* 0x28 */ f32 unk_28;
	/* 0x2C */ f32 unk_2C;
	/* 0x30 */ f32 unk_30;
} CamShiftWork;
