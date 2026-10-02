#pragma once

#include "evt/evt_cmd.h"
USER_FUNC(gazigazi);
void set_damage_root_ypos(f32 y);
void marioDmgRetQuakeOn(void);
void marioSetSplash(s32 a, Vec* pos);
s32 marioWaitDamageTogeReturnFall(void);
s32 marioChkDamageSink(void);
s32 marioWaitDamageReturnFall(void);
