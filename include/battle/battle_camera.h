#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/mtx.h>

typedef struct BattleCameraWork {
    s32 flags;              // 0x00
    s32 unk4;               // 0x04
    s32 priLimit;           // 0x08
    s32 mode;               // 0x0C
    s32 unk10;              // 0x10
    s32 unk14;              // 0x14
    s32 unk18;              // 0x18
    s32 unk1C;              // 0x1C
    s32 unk20;              // 0x20
    f32 zoom;               // 0x24
    s16 moveSpeedLv;        // 0x28
    s16 zoomSpeedLv;        // 0x2A
    Vec field_0x2C;         // 0x2C, cameraPos as battleCameraMain last set it
    Vec field_0x38;         // 0x38, cameraPos at the start of battleCameraMain (restored per axis by flags 4/8/0x10)
    Vec field_0x44;         // 0x44, cameraAt as battleCameraMain last set it
    Vec field_0x50;         // 0x50, cameraAt at the start of battleCameraMain (restored with field_0x38)
    Vec position;           // 0x5C
    Vec unk68;              // 0x68 (speed / delta)
    Vec field_0x74;         // 0x74, battleCameraMain's o4-o6 (unk68 gets o1-o3)
    s32 unk80;              // 0x80
    s32 unk84;              // 0x84
    s32 unk88;              // 0x88
    Vec unk8C;              // 0x8C
    Vec unk98;              // 0x98
    Vec unkA4;              // 0xA4
    Vec unkB0;              // 0xB0
    Vec unkBC;              // 0xBC
    Vec unkC8;              // 0xC8
    s32 unkD4;              // 0xD4
    s32 unkD8;              // 0xD8
    s32 unkDC;              // 0xDC
    s32 unkE0;              // 0xE0
    s32 unkE4;              // 0xE4
    s32 unkE8;              // 0xE8
    f32 unkEC;              // 0xEC
    f32 unkF0;              // 0xF0
    f32 unkF4;              // 0xF4
    f32 unkF8;              // 0xF8
    f32 field_0xFC;         // 0xFC, the current x offset of a btl_camera_shake_w shake (flag 1)
    f32 field_0x100;        // 0x100, the current y offset of a btl_camera_shake_h shake (flag 2)
} BattleCameraWork;

void battleCameraInit(void);

void battleCameraMain(void);
void battleCameraMoveTo(s32 id, s32 a, f32* p1, f32* p2, f32* p3, f32* p4, f32* p5, f32* p6, f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, f32 f6);
void btl_camera_shake_h(s32 id, s32 a, s32 b, f32 x, f32 y);
void btl_camera_set_mode(s32 id, s32 mode);
void btl_camera_set_moveSpeedLv(s32 id, s32 lv);
void btl_camera_set_moveto(s32 id, s32 a, s32 b, f32 f1, f32 f2, f32 f3, f32 f4,
    f32 f5, f32 f6);
void btl_camera_off_posoffset_manual(s32 id);
void btl_camera_set_posoffset(s32 id, f32 x, f32 y, f32 z);
void btl_camera_nomove_x_onoff(s32 id, s32 on);
void btl_camera_nomove_y_onoff(s32 id, s32 on);
void btl_camera_nomove_z_onoff(s32 id, s32 on);
void btl_camera_noshake(s32 id);
void btl_camera_shake_w(s32 id, s32 a, s32 b, f32 x, f32 y);
void btl_camera_set_zoomSpeedLv(s32 id, s32 lv);
void btl_camera_add_zoom(s32 id, f32 z);
void btl_camera_set_zoom(s32 id, f32 z);
void btl_camera_set_prilimit(s32 limit);
f32 battleCameraGetPosMoveSpeed(void);
USER_FUNC(evt_btl_camera_wait_move_end);
USER_FUNC(evt_btl_camera_off_posoffset_manual);
USER_FUNC(evt_btl_camera_set_mode);
USER_FUNC(evt_btl_camera_set_homing_unit);
USER_FUNC(evt_btl_camera_set_zoom);
USER_FUNC(evt_btl_camera_set_moveSpeedLv);
USER_FUNC(evt_btl_camera_set_zoomSpeedLv);
USER_FUNC(evt_btl_camera_set_mode);
USER_FUNC(evt_btl_camera_set_homing_unit);
USER_FUNC(evt_btl_camera_set_moveSpeedLv);
USER_FUNC(evt_btl_camera_set_zoom);
USER_FUNC(evt_btl_camera_shake_h);
USER_FUNC(evt_btl_camera_add_zoom);

USER_FUNC(evt_btl_camera_nomove_y_onoff);
USER_FUNC(evt_btl_camera_noshake);
USER_FUNC(evt_btl_camera_set_homing_unitparts);
USER_FUNC(evt_btl_camera_set_moveto);
USER_FUNC(evt_btl_camera_set_posoffset);
USER_FUNC(evt_btl_camera_set_prilimit);
USER_FUNC(evt_btl_camera_shake_w);
USER_FUNC(evt_btl_camera_nomove_x_onoff);
USER_FUNC(evt_btl_camera_nomove_z_onoff);
USER_FUNC(evt_btl_camera_set_homing_unit_audience);
// 2.0f in the unit's .sdata2; used by name so the constant stays first in that section.
