#pragma once

#include "driver/camdrv.h"

#include <dolphin/types.h>

extern const char str_SFX_BTL_SAC_HEART_SHOOT2[];
extern const char str_M_C_2A[];
extern const char str_M_C_2B[];
void main_star0(s32 idx);
void main_star1(void);
void main_weapon(s32 idx);
void main_mario(void);
void main_target(void);
void main_object(s32 idx);
void object_entry(void);
void weapon_entry(void);
s32 object_get_num(void);
