#pragma once

#include "driver/imgdrv.h"
#include "sdk/texPalette.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

typedef void (*OffscreenCallback)(CameraId cameraId, void* param);

typedef enum FadeType {
	FADE_IN_BLACK = 9,
	FADE_OUT_BLACK = 10,






	FADE_OUT_WHITE = 17,
	FADE_IN_WHITE2 = 18,





	TITLE_SCREEN_CURTAIN_LEAVE_GRADUAL2 = 39,

	MARIO_HEAD_FADE_IN = 52,
	MARIO_HEAD_FADE_OUT = 53,
} FadeType;

typedef struct FadeEntry {
	u16 flags; //0x0, 4 = use virtual pos
	u8 pad_2[2]; //0x2
	FadeType type; //0x4
	s32 step; //0x8, index into the per-type state machine in fadeMain
	u8 pad_C[0x10 - 0xC]; //0xC
	s64 duration; //0x10
	GXColor color; //0x18
	f32 fadeSpotX; //0x1C
	f32 fadeSpotY; //0x20
	f32 alpha; //0x24, 0..1 fade amount
	f32 angle; //0x28, degrees (rotation of the fade texture)
	Mtx texMtx; //0x2C, texture matrix written by disp_texture
	f32 elapsed; //0x5C, accumulated by 16.67 per frame
	const char* groupName; //0x60
	const char* animName; //0x64
	s32 imageType; //0x68
	s32 pose; //0x6C, animPoseEntry handle (-1 = none)
	s32 paperPose; //0x70, animPaperPoseEntry handle (-1 = none)
	GXColor poseColor; //0x74, colour fed to animPoseSetMaterialEvtColor
	f32 animOffsetX; //0x78
	f32 animOffsetY; //0x7C
	Vec VirtualPos; //0x80
	ImageEntry* image; //0x8C
	f32 scale; //0x90
	s32 frameCount; //0x94
	s32 sfxHandle; //0x98
	OffscreenCallback callback; //0x9C
	void* offparam; //0xA0
	s32 delayFrames; //0xA4, frames to hold before the fade starts
} FadeEntry;

typedef struct FadeWork {
	u16 flags; //0x0
	u8 pad_2[2]; //0x2
	u32 pad_4; //0x4
	FadeEntry entries[5]; //0x8
	s32 activeEntry; //0x350, index of the entry the fade is running (-1 = none)
	f32 softAmount; //0x354, ramp 0..1 for fadeTecSoftOn/Off
	TEXPalette* texture; //0x358
	BOOL loaded; //0x35C
} FadeWork;

void fadeInit(void);
void fadeTexSetup(void);
void fadeEntry(s32 type, s32 duration, GXColor color);
void fadeTecOn(void);
void fadeTecOff(void);
void fadeTecSoftOn(void);
void fadeTecSoftOff(void);
void fadeSoftFocusOn(void);
void fadeSoftFocusOff(void);
void fadeWhiteOutOn(void);
void fadeWhiteOutOff(void);
void fadeSetSpotPos(f32 x, f32 y);
void fadeSetAnimOfsPos(f32 x, f32 y);
void fadeSetOffscreenCallback(OffscreenCallback callback, void* param);
void fadeMain(void);
BOOL fadeIsFinish(void);
void fadeReset(s32 id);
TEXPalette* fadeGetTpl(void);
void fadeSetNarrowFast(void);
void disp_texture(CameraId camId, void* param);
void disp_maku(CameraId camId, void* param);
void disp_tec(CameraId camId, void* param);
void disp_dummy_black(CameraId camId, void* param);
void fadeSetAnimVirtualPos(f32 x, f32 y, f32 z);
