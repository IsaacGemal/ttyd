#pragma once

#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"

typedef enum ExtPoseFlags {
	EXT_POSE_VISIBLE = (1 << 0),
	EXT_POSE_BILLBOARD = (1 << 1),
	EXT_POSE_NO_SHADOW = (1 << 2),
	EXT_POSE_SHADOW_AT_GROUND = (1 << 4),
} ExtPoseFlags;

typedef struct ExtPose {
	u32 flags;       //0x0, ExtPoseFlags
	f32 heading;     //0x4, facing angle in degrees
	f32 groundY;     //0x8, shadow height with EXT_POSE_SHADOW_AT_GROUND
	Mtx* matrix;     //0xC
	s32 texId;       //0x10, index into ExtWork.textures
	f32 yaw;         //0x14, current yaw in degrees, eased toward targetYaw
	f32 field_0x18;  //0x18
	f32 targetYaw;   //0x1C
} ExtPose;

typedef struct ExtTexture {
	void* image;        //0x0
	GXTexObj texObj;    //0x4
	GXTexRegion region; //0x24
} ExtTexture;

// 0xC-byte entry `param` points at in extMakeTexture/extEntry (name, animName, frame).
typedef struct ExtMakeTextureEntry {
	const char* name; //0x0
	void* animName;   //0x4
	f32 frame;        //0x8, animPoseDisp_MakeExtTexture draws the pose at this frame
} ExtMakeTextureEntry;

typedef struct ExtSortEntry {
	s32 index; //0x0
	f32 key;   //0x4
} ExtSortEntry;

typedef struct ExtWork {
	s32 poseNum;           //0x0
	s32 texCount;          //0x4
	ExtPose* poses;        //0x8
	ExtTexture* textures;  //0xC
	Mtx* mtx;              //0x10
	f32* vertices;         //0x14
	void* normals;         //0x18
	void* colors;          //0x1C
	void* uvs;             //0x20
	ExtSortEntry* sortList;//0x24
	u8 (*dlBuffer)[0x80];  //0x28, texCount display lists of 0x80 bytes
	u32* dlSizes;          //0x2C
	void (*cb0)(void);     //0x30
	void (*cb1)(void);     //0x34
	s32 lastTex;           //0x38
	Mtx shadowMtx;         //0x3C
	u32 texSize;           //0x6C
} ExtWork;

void extInit(void);
void extMain(void);
void extLoadShadowRenderMode(void);
void extLoadShadowVertex(void);
void extLoadShadowTev(void);
void extDrawShadow(void);
void extLoadVertex(void);
void extLoadTexture(void);
void extLoadTextureExit(void);
void extLoadRenderMode(void);
void extLoadTev(void);
void extDraw(void);
s32 extGetPoseNum(void);
void extPoseDraw(s32 poseId);
ExtPose* extGetPosePtr(void);
void extLoadShadowMtx(Mtx mtx);
void extMakeTexture(CameraId camId, void* param);
void extEntry(s32 count, void* param, void (*callback)(void), void (*cb0)(void), void (*cb1)(void));
void extReset(void);
