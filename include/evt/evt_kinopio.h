#pragma once

#include "evt/evt_cmd.h"

// An inn's setup, handed to evt_kinopio_setup by the map
typedef struct KinopioData {
    s32 field_0;        // 0x00
    f32 field_4;        // 0x04
    f32 field_8;        // 0x08
    f32 field_C;        // 0x0C
    s32 ryoukin;        // 0x10
    f32 bedX;           // 0x14
    f32 bedY;           // 0x18
    f32 bedZ;           // 0x1C
    f32 bedsideX;       // 0x20
    f32 bedsideY;       // 0x24
    f32 bedsideZ;       // 0x28
    f32 camX;           // 0x2C
    f32 camY;           // 0x30
    f32 camZ;           // 0x34
    f32 camX2;          // 0x38
    f32 camY2;          // 0x3C
    f32 camZ2;          // 0x40
    f32 beddeg;         // 0x44
    f32 npctalkX;       // 0x48
    f32 npctalkY;       // 0x4C
    f32 npctalkZ;       // 0x50
    const char* npcname; // 0x54
    const char* msgs[14]; // 0x58 (0x58..0x8F)
    s32* to_bed_evt;    // 0x90
    s32* from_bed_evt;  // 0x94
} KinopioData;

USER_FUNC(N_cameraman_on);
USER_FUNC(breakfast);
USER_FUNC(coin_chk);
USER_FUNC(get_bed);
USER_FUNC(get_beddeg);
USER_FUNC(get_bedside);
USER_FUNC(get_cam);
USER_FUNC(get_from_bed_evt);
USER_FUNC(get_npcname);
USER_FUNC(get_npctalk);
USER_FUNC(get_ryoukin);
USER_FUNC(get_to_bed_evt);
USER_FUNC(msg_no);
USER_FUNC(offset);
USER_FUNC(power_chk);
USER_FUNC(snd_off);
USER_FUNC(snd_on);

USER_FUNC(evt_kinopio_setup);
