#pragma once

#include "evt/evt_cmd.h"

extern s32 evt_emergency[];

USER_FUNC(check_kou_evt);
USER_FUNC(dot_mario);
EVT_DEF(peach_evt_bero_out1);
EVT_DEF(peach_evt_bero_out2);
EVT_DEF(evt_kou1);
EVT_DEF(evt_kou2);
EVT_DEF(evt_rokka_open);
EVT_DEF(evt_rokka_close);
EVT_DEF(evt_rokka_init);
EVT_DEF(peach_evt_fuku_g);
EVT_DEF(peach_evt_ka_open);
EVT_DEF(peach_evt_ka_close);
EVT_DEF(peach_evt_kou_in);
EVT_DEF(peach_evt_kou_out);
EVT_DEF(peach_evt_kou1_in);
EVT_DEF(peach_evt_kou1_out);
EVT_DEF(peach_evt_kou2_in);
EVT_DEF(peach_evt_kou2_out);
EVT_DEF(s_bero_out);
EVT_DEF(aji_09_init_evt);
