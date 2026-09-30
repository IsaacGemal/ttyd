#pragma once

#include <dolphin/types.h>
#include "driver/dispdrv.h"

void BattleAcHelpInit(void);
void BattleAcHelpMain(void);
void BattleAcHelpSetHelp(const char* msg);
void BattleAcHelpSetDispType(s32 type);
void _ac_help_disp(CameraId camera, void* param);
