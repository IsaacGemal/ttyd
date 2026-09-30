#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(mist_add_vy);
USER_FUNC(make_mist_name);
USER_FUNC(eff_b_peach_mist);
USER_FUNC(eff_b_peach_ripple);
USER_FUNC(slowturnoff_check);
USER_FUNC(make_koe_pos);
USER_FUNC(make_koe_label);
USER_FUNC(pcharge_set_pos);
USER_FUNC(pcharge_entry);
USER_FUNC(radiation_set_pos);
USER_FUNC(radiation_entry);
USER_FUNC(add_vy);
void b_peach_msgcb(s32 state);
USER_FUNC(b_peach_thunder);
USER_FUNC(all_hikari_onoff);
void map_color_tev(void* work);
USER_FUNC(all_hikari_change_kuro);
USER_FUNC(hikari_onoff);
USER_FUNC(hikari_se);
USER_FUNC(hikari_change);
USER_FUNC(hikari_set_scale);
USER_FUNC(rou_hikari_front_onoff);
void _hikariEntry(const char* name, s32 type, f32 scale);
EVT_DEF(q_kage_init);
EVT_DEF(hikari_off_evt);
EVT_DEF(hikari_on_evt);
EVT_DEF(flush_on);
EVT_DEF(flush_off);
EVT_DEF(last_evt_1);
EVT_DEF(last_evt_2);
EVT_DEF(unk_evt_las_00034ca0);
EVT_DEF(last_evt_3);
EVT_DEF(b_peach_fuwafuwa);
EVT_DEF(last_evt_3_1);
EVT_DEF(koe_evt);
EVT_DEF(majo_kyorokyoro);
EVT_DEF(last_evt_3_2);
EVT_DEF(last_evt_4);
EVT_DEF(shuryolight_delete);
EVT_DEF(las_29_init_evt);

typedef struct MapColorTevWork {
    u8 pad_0x0[0xC];
    s32 field_0xC;
    s32 field_0x10;
    s32 field_0x14;
} MapColorTevWork;
