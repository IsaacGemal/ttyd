#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(moveSwitch);
void getMri02Yuka(void);  // TODO: real signature
USER_FUNC(setPuniWeight);
USER_FUNC(getSwitch);
EVT_DEF(punipuni_switch);
EVT_DEF(dokan_event);
EVT_DEF(dokan_init);
EVT_DEF(mri_02_init_evt);
