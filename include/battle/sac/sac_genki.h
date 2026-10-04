#pragma once

#include "driver/camdrv.h"

#include <dolphin/types.h>

extern const char str_80300470[];
extern const char str_8030048C[];
extern const char str_80300494[];
void main_star0(s32 idx);
void main_star1(void);
void main_weapon(s32 idx);
void main_mario(void);
void main_target(void);
void main_object(s32 idx);
void object_entry(void);
void weapon_entry(void);
s32 object_get_num(void);
