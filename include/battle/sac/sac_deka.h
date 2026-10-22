#pragma once

#include "driver/camdrv.h"

typedef struct BattleUnit BattleUnit;
typedef struct BattleWeapon BattleWeapon;
typedef struct BattleWorkUnitPart BattleUnitPart;

#include <dolphin/types.h>
void main_enemy_sub(BattleUnit* unit, s32* count);
void yuka_disp(CameraId cameraId, void* param);
void yuka_capture(CameraId cameraId);
void create_timing(void);
void main_timing(s32 idx);
void main_enemy(void);
void main_gauge(void);
u32 weaponGetPower_Deka(BattleUnit* unit1, BattleWeapon* weapon, BattleUnit* unit2, BattleUnitPart* part);
