#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/types.h>
void name_callback_sub(const char* name, void* script, s32 recurse);

USER_FUNC(L_evt_hitobj_flag_onoff);
USER_FUNC(evt_hit_bind_mapobj);
USER_FUNC(evt_hit_bind_update);
USER_FUNC(evt_hit_damage_return_set);
USER_FUNC(evt_hit_get_position);
USER_FUNC(evt_hitobj_attr_onoff);
USER_FUNC(evt_hitobj_name_callback);
USER_FUNC(evt_hitobj_onoff);
