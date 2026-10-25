#pragma once

#include "driver/camdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <dolphin/os.h>

typedef struct ImageShadow {
	s32 flags; //0x0
	u16 left; //0x4
	u16 top; //0x6
	u16 bboxX; //0x8
	u16 bboxY; //0xA
	u16 bboxW; //0xC
	u16 bboxH; //0xE
	s32 offscreenId; //0x10
	s32 dstFormat; //0x14
	s32 texFormat; //0x18
	GXTexObj texObjData; //0x1C
	smartEntry* imageData; //0x3C
	s32 captureMode; //0x40
} ImageShadow;

typedef struct ImageEntry {
	ImageShadow shadows[3]; //0x0
	u32 flags; //0xCC
	s32 type; //0xD0
	char name[16]; //0xD4
	u16 captureWidth; //0xE4
	u16 captureHeight; //0xE6
	f32 posX; //0xE8
	f32 posY; //0xEC
	Vec point; //0xF0, virtual point
	f32 order; //0xFC
	s32 shadowId; //0x100
	s32 poseId; //0x104
	const char* animName; //0x108
	u8 pad_0x10C[0x110 - 0x10C]; //0x10C
	OSTime startTime; //0x110
	f32 loopTimes; //0x118
	f32 depthPlaneOffset; //0x11C
	Mtx matrix; //0x120
	CameraId camId; //0x150
	s32 captureCamId; //0x154
} ImageEntry;

typedef struct ImageWork {
	s32 count; //0x0
	ImageEntry* entries; //0x4
} ImageWork;

void imgInit(void);
void imgAutoRelease(s32 type);
s32 imgEntry(const char* name, s32 type);
void imgMain(void);
ImageEntry* imgNameToPtr(const char* name, s32 type);
void imgFreeCapture(ImageEntry* entry, s32 shadowId);
void imgSetVirtualPoint(ImageEntry* entry, const Vec* point);
void imgClearVirtualPoint(ImageEntry* entry);
void imgRelease(ImageEntry* entry, s32 type);
void imgSetShadow(ImageEntry* entry, s32 shadowId);
void imgDisp(CameraId camId, void* param);
void imgDisp_ProjPlane(CameraId camId, void* param);
void imgCapture_Prim(ImageEntry* entry, s32 id);
void imgCapture(CameraId camId, void* param);
void imgShadowDisp(CameraId camId, void* param);
