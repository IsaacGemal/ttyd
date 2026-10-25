#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/mtx.h>

typedef struct BattleCameraWork {
    s32 flags;              // 0x00
    s32 modeFrames;         // 0x04, mode 14 counts its frames here (moves on at 10); btl_camera_set_mode clears it
    s32 priLimit;           // 0x08
    s32 mode;               // 0x0C
    s32 homingUnitId;       // 0x10, the unit evt_btl_camera_set_homing_unit* set; the homing modes follow it
    s32 homingUnitId2;      // 0x14, the second unit (their type2/partId2)
    s32 homingPartId;       // 0x18, homingUnitId's part
    s32 homingPartId2;      // 0x1C, homingUnitId2's part
    s32 homingMemberId;     // 0x20, evt_btl_camera_set_homing_unit_audience's member (mode 12); mode 13 overwrites it with 10
    f32 zoom;               // 0x24
    s16 moveSpeedLv;        // 0x28
    s16 zoomSpeedLv;        // 0x2A
    Vec cameraPos;          // 0x2C, the camera's cameraPos as battleCameraMain last set it; copied back at its start
    Vec prevCameraPos;      // 0x38, cameraPos at the start of battleCameraMain (restored per axis by flags 4/8/0x10)
    Vec cameraAt;           // 0x44, the camera's cameraAt as battleCameraMain last set it; copied back at its start
    Vec prevCameraAt;       // 0x50, cameraAt at the start of battleCameraMain (restored with prevCameraPos)
    Vec position;           // 0x5C
    Vec posVel;             // 0x68, battleCameraMoveTo's per-frame cameraPos step (battleCameraGetPosMoveSpeed)
    Vec atVel;              // 0x74, and its cameraAt step
    s32 moveToMode;         // 0x80, intplGetValue mode of a btl_camera_set_moveto move (battleCameraMain mode 3 runs it)
    s32 moveToTimer;        // 0x84, frames left of that move
    s32 moveToFrames;       // 0x88, its length in frames
    Vec moveToPosFrom;      // 0x8C, cameraPos when the move was set
    Vec moveToPosTo;        // 0x98, the cameraPos it moves to
    Vec moveToAtFrom;       // 0xA4, cameraAt when the move was set
    Vec moveToAtTo;         // 0xB0, the cameraAt it moves to
    Vec homingPos;          // 0xBC, homingUnitId's home position (BtlUnit_GetHomePos) when the homing was set;
                            // the _audience setter stores its BtlUnit_GetPos
    Vec homingPos2;         // 0xC8, homingUnitId2's hit position (or the audience member's) then
    s32 shakeXMode;         // 0xD4, intplGetValue mode of the btl_camera_shake_w offset
    s32 shakeYMode;         // 0xD8, intplGetValue mode of the btl_camera_shake_h offset
    s32 shakeXFrames;       // 0xDC, length of the shake_w shake in frames
    s32 shakeYFrames;       // 0xE0, length of the shake_h shake in frames
    s32 shakeXTimer;        // 0xE4, frames left of the shake_w shake; flag 1 clears at 0
    s32 shakeYTimer;        // 0xE8, frames left of the shake_h shake; flag 2 clears at 0
    f32 shakeXFrom;         // 0xEC, shake_w offset at the start
    f32 shakeXTo;           // 0xF0, and at the end
    f32 shakeYFrom;         // 0xF4, shake_h offset at the start
    f32 shakeYTo;           // 0xF8, and at the end
    f32 shakeOffsetX;       // 0xFC, the current x offset of a btl_camera_shake_w shake (flag 1)
    f32 shakeOffsetY;       // 0x100, the current y offset of a btl_camera_shake_h shake (flag 2)
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
