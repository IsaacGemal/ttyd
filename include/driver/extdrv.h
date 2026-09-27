#pragma once

#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

typedef struct ExtPose {
	u32 flags;      //0x0
	f32 field_0x4;  //0x4
	f32 field_0x8;  //0x8
	Mtx* matrix;    //0xC
	s32 field_0x10; //0x10
	f32 field_0x14; //0x14
	f32 field_0x18; //0x18
	f32 field_0x1C; //0x1C
} ExtPose;

typedef struct ExtTexture {
	void* image;        //0x0
	GXTexObj texObj;    //0x4
	GXTexRegion region; //0x24
} ExtTexture;

typedef struct ExtSortEntry {
	s32 field_0x0; //0x0
	f32 field_0x4; //0x4
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
	void* dlBuffer;        //0x28
	u32* dlSizes;          //0x2C
	void (*cb0)(void);     //0x30
	void (*cb1)(void);     //0x34
	s32 lastTex;           //0x38
	Mtx field_0x3C;        //0x3C
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
void extEntry(s32 count, void* param, void (*callback)(void), void (*cb0)(void), void (*cb1)(void));
void extReset(void);
