#pragma once

#include "driver/camdrv.h"

#include <dolphin/mtx.h>
#include "battle/battle_unit.h"

typedef struct BattleDispPose { //for BattleWorkUnitPart
	u32 poseId; //0x0
	const char* poseName; //0x4
} BattleDispPose;

void btlDispGXInit2DRasta(void);
void btlDispGXInit2D(void);
void btlDispGXPoint2DRasta(f32 x, f32 y, u8 r, u8 g, u8 b, u8 a);
void btlDispGXQuads2DRasta(f32 x0, f32 y0, f32 x1, f32 y1, u8 r, u8 g, u8 b, u8 a);
void btlDispGXPoint2D(f32 x, f32 y, u8 r, u8 g, u8 b, u8 a, f32 s, f32 t);
void btlDispGXQuads2D(f32 x0, f32 y0, f32 x1, f32 y1, u8 r, u8 g, u8 b, u8 a);
void btlDispTexPlane(f32 tX, f32 tY, f32 tZ, f32 sX, f32 sY, u32 id, GXColor color, s32 unused);
void btlDispTexPlane3(Mtx mtx, u32 id, GXColor color0, GXColor color1, GXColor color2, GXColor color3, s32 unused);
void btlDispInit(void);
StatusEffectType _GetStatusPoseType(BattleUnit* unit);


void btlDispMain(void);




void btlDispEntAnime(BattleUnit* unit);
void btlDispChangePeraAnime(BattleUnitPart* part, const char* animName);
void btlDispTexPlane2(Mtx mtx, u32 id, GXColor color, s32 unused);
const char* searchPoseTbl(BattleDispPose* table, s32 poseId);
void btlUnitPartsDisp(CameraId camId, void* param);
void btlUnitPartsBlurDisp(CameraId camId, BattleUnitPart* parts);
void _btlStockExpDisp(CameraId cameraId, void* param);
void btlUnitItemDisp(CameraId cameraId, BattleUnit* unit);
void btlDispPoseAnime(BattleUnitPart* part);
void btlDispChangeAnime(BattleUnitPart* part, const char* animName, BOOL reset);
void btlGetScreenPoint(Vec* pos, Vec* out);
void btlDispGXInit2DSub(void);
void btlDispTexPlainGX(u32 id, GXColor color0, GXColor color1, GXColor color2, GXColor color3);
void btlDispGetTexSize(u32 id, u16* width, u16* height);
void _pose_def(BattleUnitPart* part);
void _pose_one_pattern(BattleUnitPart* part);
void _pose_two_pattern(BattleUnitPart* part);
void _partsBlurControl(BattleUnitPart* part, s32 flags, Mtx orientation, GXColor color, f32 rotation);
void btlUnitPartsBlurControl(BattleUnitPart* part, s32 unused, Mtx orientation, GXColor color, f32 rotation);
f32 getGravityDispOffset(BattleUnitPart* part);
f32 getFloatDispOffset(BattleUnitPart* part);
void _btlDispTex4(s32 id, Vec trans, Vec scale, Vec rot, Vec pos2, Vec scale2, GXColor color);
void btlDispAnimeSpeed(BattleUnitPart* part, f32 speed);
void gravityOffsetControl(BattleUnitPart* part);
void floatOffsetControl(BattleUnitPart* part);
void statusPoseControl(BattleUnitPart* part);
void BattleStockExpDisp(void);
void btlUnitStolenItemDisp(CameraId cameraId, void* param);
void btlUnitItemDisp2(CameraId cameraId, BattleUnit* unit);
