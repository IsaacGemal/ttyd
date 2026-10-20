#pragma once

#include "unit_boss_gundan_zako_common.h"

// Six f32 offsets indexed by case in zakogun_ext_main1's state-0x14 handling.
typedef struct ZakoOffsetTable {
    f32 v[6];
} ZakoOffsetTable;

extern const ZakoOffsetTable dat_muj_00012fdc;

USER_FUNC(zakogun_end_sukima);
USER_FUNC(zakogun_start_sukima);
USER_FUNC(zakogun_escape1);
USER_FUNC(zakogun_dead1);
USER_FUNC(zakogun_attack1);
void zakogun_ext_main1(void);
USER_FUNC(zakogun_ext_init1);
void zakogun_ext_disp(CameraId cameraId, void* param);
void zakogun_ext_main(void);
void zakogun_ext_init(void);
void zakogun_ext_main3(void);
EVT_DEF(damage_event_kaiten_hammer);
EVT_DEF(zakogun_entry);
