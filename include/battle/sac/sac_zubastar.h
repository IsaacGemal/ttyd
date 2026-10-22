#pragma once

#include <dolphin/mtx.h>
#include <dolphin/types.h>

typedef struct BattleUnit BattleUnit;
typedef struct BattleWeapon BattleWeapon;
typedef struct BattleWorkUnitPart BattleUnitPart;

typedef struct ZubaStarWork ZubaStarWork;

extern const char str_SFX_BTL_SAC_ZUBA_STAR2[];
extern const char str_SFX_BTL_SAC_ZUBA_STAR1[];
extern const char str_btl_wn_sac_zubastar[];

ZubaStarWork* GetZubaStarPtr(void);
void zubastar_create_takaku(s32 count, Vec* points);
void zubastar_bunkatu(Vec* a, Vec* b);
s32 zubastar_get_kouten(f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, f32 f6, f32 f7, f32 f8, f32* out1, f32* out2);
void zubaStarDispStar(CameraId cameraId, void* param);
void zubastar_capture(CameraId cameraId, void* param);
void zubastar_disp_bomb(CameraId cameraId, void* param);
void zubastar_disp2D(CameraId cameraId, void* param);
u32 weaponGetPower_ZubaStar(BattleUnit* unit1, BattleWeapon* weapon, BattleUnit* unit2, BattleUnitPart* part);
