#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(aud_target_mark);
USER_FUNC(aud_fire_no_hit);
USER_FUNC(aud_fire_hit);
void aud_fire_eff(void);  // TODO: real signature
void aud_get_pod_pos(void);  // TODO: real signature
USER_FUNC(unk_aji_00007668);
USER_FUNC(aud_suikomi);
USER_FUNC(get_aud_num);
USER_FUNC(aud_jet_end);
USER_FUNC(aud_jet);
USER_FUNC(drill_jet);
USER_FUNC(drill_tor);
EVT_DEF(aud_hit_event);
EVT_DEF(aud_no_hit_event);
EVT_DEF(drill_attack_event);
EVT_DEF(boomerang_attack_event);
EVT_DEF(aud_attack_event);
EVT_DEF(aud_attack_event_sub);
