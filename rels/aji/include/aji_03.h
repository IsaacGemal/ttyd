#pragma once

#include "driver/camdrv.h"
#include "evt/evt_cmd.h"

USER_FUNC(evt_sousa_icon);
void icon_disp(CameraId camId, void* param);
USER_FUNC(evt_itemNameToItemID);
USER_FUNC(item_chk);
EVT_DEF(evt_kaitentou);
EVT_DEF(evt_kaitentou2);
EVT_DEF(ufo_hand);
EVT_DEF(move_left);
EVT_DEF(move_left2);
EVT_DEF(move_up);
EVT_DEF(move_up2);
EVT_DEF(hand_success);
EVT_DEF(hand_fail);
EVT_DEF(hand_release);
EVT_DEF(ufo_main);
EVT_DEF(evt_ufo_init);
EVT_DEF(evt_zenmai);
EVT_DEF(blink_sw);
EVT_DEF(evt_ufo_sousaban);
EVT_DEF(evt_sw_reinit);
EVT_DEF(evt_sw0);
EVT_DEF(evt_sw1);
EVT_DEF(evt_sw2);
EVT_DEF(badge_evt);
EVT_DEF(badge_evt2);
EVT_DEF(badge_evt3);
EVT_DEF(aji_03_init_evt);
