#pragma once

#include "evt/evt_cmd.h"

typedef struct MoamoaAnimData {
    const char* name1; // 0x0
    const char* name2; // 0x4
    f32 frame;         // 0x8
} MoamoaAnimData;

USER_FUNC(moamoa_set_bodyScale);
void moamoa_draw(void);  // TODO: real signature
void moamoa_pose(void* pose);
USER_FUNC(moamoa_on);
USER_FUNC(moamoa2_on);
USER_FUNC(moamoa1_on);
USER_FUNC(moamoa_bomb);
USER_FUNC(moamoa_guruguru);
void moamoa_mainUnitIdClear(void);  // TODO: real signature
USER_FUNC(evt_moamoa_mainUnitIdClear);
USER_FUNC(boss_moamoa_check_zenei_kouei);
EVT_DEF(moamoa_zoom);
EVT_DEF(moamoa_tentacl_on);
EVT_DEF(moamoa_mouth_on);
EVT_DEF(MOAMOA_SET_BODYSCALE);
