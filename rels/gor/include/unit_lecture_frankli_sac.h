#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"

void spot_square_disp(CameraId cameraId, void* param);
void Quads2D(f32 x0, f32 y0, f32 x1, f32 y1);
USER_FUNC(spot_square);
USER_FUNC(blinkAP);
USER_FUNC(aud_attack_end);
USER_FUNC(aud_attack_init);
USER_FUNC(hammer_on);
USER_FUNC(aud_dec);
USER_FUNC(aud_inc);
USER_FUNC(aud_joy);
USER_FUNC(get_ac_result);
USER_FUNC(get_actkind);
USER_FUNC(_set_actend_event);
EVT_DEF(_test_event);
EVT_DEF(stop_wait_event);
EVT_DEF(actend_event);
