#include "battle/battle_camera.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "driver/camdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"
#include <string.h>
#include "math_ext.h"


extern BattleWork *_battleWorkPointer;
const Vec vec3_802efc40 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802efc4c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802efc58 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802efc64 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802efc70 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802efc7c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802efc88 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802efc94 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802efca0 = { 0.0f, 0.0f, 0.0f };

// prototypes for the event wrappers

// this file uses -O4,s
void battleCameraInit(void) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera;

  camera = &wp->cameraWork;
  memset(camera, 0, sizeof(BattleCameraWork));
  camera->flags = 0;
  switch (wp->fbatInfo->partyId) {
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
    camera->mode = 15;
    break;
  case 9:
    camera->mode = 16;
    break;
  case 0:
    camera->mode = 14;
    break;
  }
  camera->priLimit = 0;
  camera->zoom = 0.0f;
  camera->moveSpeedLv = 1;
  camera->zoomSpeedLv = 1;
  camera->position = (Vec){0.0f, 0.0f, 0.0f};
  camera->unk4 = 0;
}

// The event wrappers call these helpers as real functions in the original
// (not inlined); keep inlining off across the small API surface.
#pragma dont_inline on

USER_FUNC(evt_btl_camera_wait_move_end) {
  if (battleCameraGetPosMoveSpeed() >= 2.0f) {
    return EVT_RETURN_BLOCK;
  }
  return EVT_RETURN_DONE;
}

void battleCameraMoveTo(s32 id, s32 a, f32* p1, f32* p2, f32* p3, f32* p4,
                        f32* p5, f32* p6, f32 f1, f32 f2, f32 f3, f32 f4,
                        f32 f5, f32 f6) {
  CameraEntry* cam = camGetPtr(4);
  BattleWork* wp = _battleWorkPointer;
  BattleUnit* mario = BattleGetMarioPtr(wp);
  f32 limit;

  if (wp->cameraWork.moveSpeedLv < 0 || wp->cameraWork.moveSpeedLv >= 5) {
    wp->cameraWork.moveSpeedLv = 1;
  }
  if (wp->cameraWork.zoomSpeedLv < 0 || wp->cameraWork.zoomSpeedLv >= 6) {
    wp->cameraWork.zoomSpeedLv = 1;
  }

  if (id == 1) {
    if (wp->flags & 0x100) {
// MUST_MATCH: with the literal `+= 0.0f` below MWCC folds the two adds away and assigns
// the temporaries to different registers; the named zero/tmp keep the original schedule.
#ifdef MUST_MATCH
      f32 d = mario->mHomePosition.z;
      f32 zero = 0.0f;
      f32 tmp = 0.25f * -d;
      f1 += zero;
      f3 += d;
      f4 += zero;
      f6 += d;
      f2 += tmp;
      f5 += tmp;
#else
      f32 d = mario->mHomePosition.z;
      f1 += 0.0f;
      f3 += d;
      f4 += 0.0f;
      f6 += d;
      f2 += 0.25f * -d;
      f5 += 0.25f * -d;
#endif
    }
    f1 += wp->cameraWork.position.x;
    f2 += wp->cameraWork.position.y;
    f3 += wp->cameraWork.position.z;
    f4 += wp->cameraWork.position.x;
    f5 += wp->cameraWork.position.y;
    f6 += wp->cameraWork.position.z;
  }

  // MUST_MATCH: / 2.0f (not * 0.5f) makes MWCC schedule the subtraction before the halving.
  limit = (600.0f - f3) / 2.0f;
  if (limit < 0.0f) {
    limit = 0.0f;
  }

  if (a == 1) {
    if (f1 > 100.0f + limit) {
      f1 = 100.0f + limit;
    }
    if (f1 < -100.0f - limit) {
      f1 = -100.0f - limit;
    }
    if (f4 > 100.0f + limit) {
      f4 = 100.0f + limit;
    }
    if (f4 < -100.0f - limit) {
      f4 = -100.0f - limit;
    }
  }

  switch (wp->cameraWork.moveSpeedLv) {
  case 0:
    *p1 = (f1 - cam->cameraPos.x) / 1.5f;
    *p2 = (f2 - cam->cameraPos.y) / 1.5f;
    *p4 = (f4 - cam->cameraAt.x) / 1.5f;
    *p5 = (f5 - cam->cameraAt.y) / 1.5f;
    cam->cameraPos.x += *p1;
    cam->cameraPos.y += *p2;
    cam->cameraAt.x += *p4;
    cam->cameraAt.y += *p5;
    cam->cameraUp.x = 0.0f;
    cam->cameraUp.y = 1.0f;
    cam->cameraUp.z = 0.0f;
    break;
  case 1:
    *p1 = (f1 - cam->cameraPos.x) / 15.0f;
    *p2 = (f2 - cam->cameraPos.y) / 15.0f;
    *p4 = (f4 - cam->cameraAt.x) / 15.0f;
    *p5 = (f5 - cam->cameraAt.y) / 15.0f;
    cam->cameraPos.x += *p1;
    cam->cameraPos.y += *p2;
    cam->cameraPos.z += *p3;
    cam->cameraAt.x += *p4;
    cam->cameraAt.y += *p5;
    cam->cameraUp.x = 0.0f;
    cam->cameraUp.y = 1.0f;
    cam->cameraUp.z = 0.0f;
    break;
  case 2:
    *p1 = (f1 - cam->cameraPos.x) / 5.0f;
    *p2 = (f2 - cam->cameraPos.y) / 5.0f;
    *p4 = (f4 - cam->cameraAt.x) / 5.0f;
    *p5 = (f5 - cam->cameraAt.y) / 5.0f;
    cam->cameraPos.x += *p1;
    cam->cameraPos.y += *p2;
    cam->cameraAt.x += *p4;
    cam->cameraAt.y += *p5;
    cam->cameraUp.x = 0.0f;
    cam->cameraUp.y = 1.0f;
    cam->cameraUp.z = 0.0f;
    break;
  case 3:
    *p1 = f1 - cam->cameraPos.x;
    *p2 = f2 - cam->cameraPos.y;
    *p4 = f4 - cam->cameraAt.x;
    *p5 = f5 - cam->cameraAt.y;
    cam->cameraPos.x += *p1;
    cam->cameraPos.y += *p2;
    cam->cameraAt.x += *p4;
    cam->cameraAt.y += *p5;
    cam->cameraUp.x = 0.0f;
    cam->cameraUp.y = 1.0f;
    cam->cameraUp.z = 0.0f;
    break;
  case 4:
    *p1 = (f1 - cam->cameraPos.x) / 200.0f;
    *p2 = (f2 - cam->cameraPos.y) / 200.0f;
    *p4 = (f4 - cam->cameraAt.x) / 200.0f;
    *p5 = (f5 - cam->cameraAt.y) / 200.0f;
    cam->cameraPos.x += *p1;
    cam->cameraPos.y += *p2;
    cam->cameraAt.x += *p4;
    cam->cameraAt.y += *p5;
    cam->cameraUp.x = 0.0f;
    cam->cameraUp.y = 1.0f;
    cam->cameraUp.z = 0.0f;
    break;
  case 5:
    *p1 = (f1 - cam->cameraPos.x) / 100.0f;
    *p2 = (f2 - cam->cameraPos.y) / 100.0f;
    *p4 = (f4 - cam->cameraAt.x) / 100.0f;
    *p5 = (f5 - cam->cameraAt.y) / 100.0f;
    cam->cameraPos.x += *p1;
    cam->cameraPos.y += *p2;
    cam->cameraAt.x += *p4;
    cam->cameraAt.y += *p5;
    cam->cameraUp.x = 0.0f;
    cam->cameraUp.y = 1.0f;
    cam->cameraUp.z = 0.0f;
    break;
  }

  switch (wp->cameraWork.zoomSpeedLv) {
  case 0:
    *p3 = (f3 - cam->cameraPos.z) / 1.5f;
    *p6 = (f6 - cam->cameraAt.z) / 1.5f;
    cam->cameraPos.z += *p3;
    cam->cameraAt.z += *p6;
    break;
  case 1:
    *p3 = (f3 - cam->cameraPos.z) / 15.0f;
    *p6 = (f6 - cam->cameraAt.z) / 15.0f;
    cam->cameraPos.z += *p3;
    cam->cameraAt.z += *p6;
    break;
  case 2:
    *p3 = (f3 - cam->cameraPos.z) / 5.0f;
    *p6 = (f6 - cam->cameraAt.z) / 5.0f;
    cam->cameraPos.z += *p3;
    cam->cameraAt.z += *p6;
    break;
  case 3:
    *p3 = f3 - cam->cameraPos.z;
    *p6 = f6 - cam->cameraAt.z;
    cam->cameraPos.z += *p3;
    cam->cameraAt.z += *p6;
    break;
  case 4:
    *p3 = (f3 - cam->cameraPos.z) / 200.0f;
    *p6 = (f6 - cam->cameraAt.z) / 200.0f;
    cam->cameraPos.z += *p3;
    cam->cameraAt.z += *p6;
    break;
  case 5:
    *p3 = (f3 - cam->cameraPos.z) / 100.0f;
    *p6 = (f6 - cam->cameraAt.z) / 100.0f;
    cam->cameraPos.z += *p3;
    cam->cameraAt.z += *p6;
    break;
  }
}

f32 battleCameraGetPosMoveSpeed(void) {
  BattleCameraWork *camera = &_battleWorkPointer->cameraWork;
  f32 x = camera->unk68.x;
  f32 y = camera->unk68.y;
  f32 z = camera->unk68.z;
  return (f32)sqrt((x * x) + (y * y) + (z * z));
}

void btl_camera_set_prilimit(s32 limit) {
  _battleWorkPointer->cameraWork.priLimit = limit;
}

USER_FUNC(evt_btl_camera_set_prilimit) {
  s32 *args = event->args;
  btl_camera_set_prilimit(evtGetValue(event, args[0]));
  return EVT_RETURN_DONE;
}

void btl_camera_off_posoffset_manual(s32 id) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera = &wp->cameraWork;

  if (id < camera->priLimit) {
    return;
  }
  camera->flags &= ~0x20;
  camera->position.x = 0.0f;
  camera->position.y = 0.0f;
  camera->position.z = 0.0f;
}

USER_FUNC(evt_btl_camera_off_posoffset_manual) {
  s32 *args = event->args;
  btl_camera_off_posoffset_manual(evtGetValue(event, args[0]));
  return EVT_RETURN_DONE;
}

void btl_camera_set_posoffset(s32 id, f32 x, f32 y, f32 z) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera = &wp->cameraWork;

  if (id < camera->priLimit) {
    return;
  }
  camera->flags |= 0x20;
  camera->position.x = x;
  camera->position.y = y;
  camera->position.z = z;
}

USER_FUNC(evt_btl_camera_set_posoffset) {
  s32 *args = event->args;
  s32 id = evtGetValue(event, args[0]);
  f32 x = evtGetFloat(event, args[1]);
  f32 y = evtGetFloat(event, args[2]);
  f32 z = evtGetFloat(event, args[3]);

  btl_camera_set_posoffset(id, x, y, z);
  return EVT_RETURN_DONE;
}

void btl_camera_nomove_z_onoff(s32 id, s32 on) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera = &wp->cameraWork;

  if (id < camera->priLimit) {
    return;
  }
  if (on == 1) {
    camera->flags |= 0x10;
    return;
  }
  camera->flags &= ~0x10;
}

USER_FUNC(evt_btl_camera_nomove_z_onoff) {
  s32 *args = event->args;
  s32 id = evtGetValue(event, args[0]);
  s32 on = evtGetValue(event, args[1]);

  btl_camera_nomove_z_onoff(id, on);
  return EVT_RETURN_DONE;
}

void btl_camera_nomove_y_onoff(s32 id, s32 on) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera = &wp->cameraWork;

  if (id < camera->priLimit) {
    return;
  }
  if (on == 1) {
    camera->flags |= 0x08;
    return;
  }
  camera->flags &= ~0x08;
}

USER_FUNC(evt_btl_camera_nomove_y_onoff) {
  s32 *args = event->args;
  s32 id = evtGetValue(event, args[0]);
  s32 on = evtGetValue(event, args[1]);

  btl_camera_nomove_y_onoff(id, on);
  return EVT_RETURN_DONE;
}

void btl_camera_nomove_x_onoff(s32 id, s32 on) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera = &wp->cameraWork;

  if (id < camera->priLimit) {
    return;
  }
  if (on == 1) {
    camera->flags |= 0x04;
    return;
  }
  camera->flags &= ~0x04;
}

USER_FUNC(evt_btl_camera_nomove_x_onoff) {
  s32 *args = event->args;
  s32 id = evtGetValue(event, args[0]);
  s32 on = evtGetValue(event, args[1]);

  btl_camera_nomove_x_onoff(id, on);
  return EVT_RETURN_DONE;
}

void btl_camera_noshake(s32 id) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera = &wp->cameraWork;

  if (id < camera->priLimit) {
    return;
  }
  camera->flags &= ~0x1;
  camera->flags &= ~0x2;
}

USER_FUNC(evt_btl_camera_noshake) {
  s32 *args = event->args;
  btl_camera_noshake(evtGetValue(event, args[0]));
  return EVT_RETURN_DONE;
}

void btl_camera_shake_h(s32 id, s32 a, s32 b, f32 x, f32 y) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera = &wp->cameraWork;

  if (id < camera->priLimit) {
    return;
  }
  camera->flags |= 0x2;
  camera->unkF4 = x;
  camera->unkF8 = y;
  camera->unkD8 = b;
  camera->unkE0 = a;
  camera->unkE8 = a;
  if (!(camera->flags & 0x40)) {
    padRumbleOn(0);
  }
}

USER_FUNC(evt_btl_camera_shake_h) {
  s32 *args = event->args;
  s32 id = evtGetValue(event, args[0]);
  f32 x = (f32)evtGetValue(event, args[1]);
  f32 y = (f32)evtGetValue(event, args[2]);
  s32 a = evtGetValue(event, args[3]);
  s32 b = evtGetValue(event, args[4]);

  btl_camera_shake_h(id, a, b, x, y);
  return EVT_RETURN_DONE;
}

void btl_camera_shake_w(s32 id, s32 a, s32 b, f32 x, f32 y) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera = &wp->cameraWork;

  if (id < camera->priLimit) {
    return;
  }
  camera->flags |= 0x1;
  camera->unkEC = x;
  camera->unkF0 = y;
  camera->unkD4 = b;
  camera->unkDC = a;
  camera->unkE4 = a;
  if (!(camera->flags & 0x40)) {
    padRumbleOn(0);
  }
}

USER_FUNC(evt_btl_camera_shake_w) {
  s32 *args = event->args;
  s32 id = evtGetValue(event, args[0]);
  f32 x = (f32)evtGetValue(event, args[1]);
  f32 y = (f32)evtGetValue(event, args[2]);
  s32 a = evtGetValue(event, args[3]);
  s32 b = evtGetValue(event, args[4]);

  btl_camera_shake_w(id, a, b, x, y);
  return EVT_RETURN_DONE;
}

void btl_camera_set_zoomSpeedLv(s32 id, s32 lv) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera = &wp->cameraWork;

  if (id < camera->priLimit) {
    return;
  }
  if (lv >= 0 && lv <= 5) {
    camera->zoomSpeedLv = lv;
    return;
  }
  camera->zoomSpeedLv = 1;
}

USER_FUNC(evt_btl_camera_set_zoomSpeedLv) {
  s32 *args = event->args;
  s32 id = evtGetValue(event, args[0]);
  s32 lv = evtGetValue(event, args[1]);

  btl_camera_set_zoomSpeedLv(id, lv);
  return EVT_RETURN_DONE;
}

void btl_camera_set_moveSpeedLv(s32 id, s32 lv) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera = &wp->cameraWork;

  if (id < camera->priLimit) {
    return;
  }
  if (lv >= 0 && lv <= 5) {
    camera->moveSpeedLv = lv;
  } else {
    camera->moveSpeedLv = 1;
  }
  camera->zoomSpeedLv = camera->moveSpeedLv;
}

USER_FUNC(evt_btl_camera_set_moveSpeedLv) {
  s32 *args = event->args;
  s32 id = evtGetValue(event, args[0]);
  s32 lv = evtGetValue(event, args[1]);

  btl_camera_set_moveSpeedLv(id, lv);
  return EVT_RETURN_DONE;
}

void btl_camera_add_zoom(s32 id, f32 z) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera = &wp->cameraWork;

  if (id < camera->priLimit) {
    return;
  }
  camera->zoom += z;
}

USER_FUNC(evt_btl_camera_add_zoom) {
  s32 *args = event->args;
  s32 id = evtGetValue(event, args[0]);

  btl_camera_add_zoom(id, (s32)evtGetFloat(event, args[1]));
  return EVT_RETURN_DONE;
}

void btl_camera_set_zoom(s32 id, f32 z) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera = &wp->cameraWork;

  if (id < camera->priLimit) {
    return;
  }
  camera->zoom = z;
}

USER_FUNC(evt_btl_camera_set_zoom) {
  s32 *args = event->args;
  s32 id = evtGetValue(event, args[0]);

  btl_camera_set_zoom(id, (s32)evtGetFloat(event, args[1]));
  return EVT_RETURN_DONE;
}

void btl_camera_set_mode(s32 id, s32 mode) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera = &wp->cameraWork;

  if (id < camera->priLimit) {
    return;
  }
  camera->mode = mode;
  camera->zoom = 0.0f;
  camera->moveSpeedLv = 1;
  camera->zoomSpeedLv = 1;
  camera->unk4 = 0;
  btl_camera_nomove_x_onoff(0, 0);
  btl_camera_nomove_y_onoff(0, 0);
  btl_camera_nomove_z_onoff(0, 0);
  camera->flags &= ~0x20;
}

#pragma dont_inline reset
USER_FUNC(evt_btl_camera_set_mode) {
  s32 *args = event->args;
  s32 id = evtGetValue(event, args[0]);
  s32 mode = evtGetValue(event, args[1]);

  btl_camera_set_mode(id, mode);
  return EVT_RETURN_DONE;
}
#pragma dont_inline on

USER_FUNC(evt_btl_camera_set_moveto) {
  s32 *args = event->args;
  s32 id = evtGetValue(event, args[0]);
  f32 f1 = evtGetFloat(event, args[1]);
  f32 f2 = evtGetFloat(event, args[2]);
  f32 f3 = evtGetFloat(event, args[3]);
  f32 f4 = evtGetFloat(event, args[4]);
  f32 f5 = evtGetFloat(event, args[5]);
  f32 f6 = evtGetFloat(event, args[6]);
  s32 a = evtGetValue(event, args[7]);
  s32 b = evtGetValue(event, args[8]);

  btl_camera_set_moveto(id, a, b, f1, f2, f3, f4, f5, f6);
  return EVT_RETURN_DONE;
}

void btl_camera_set_moveto(s32 id, s32 a, s32 b, f32 f1, f32 f2, f32 f3, f32 f4,
                           f32 f5, f32 f6) {
  BattleWork *wp = _battleWorkPointer;
  BattleCameraWork *camera = &wp->cameraWork;
  CameraEntry *cam = camGetPtr(4);

  if (id < camera->priLimit) {
    return;
  }
  camera->unk8C.x = cam->cameraPos.x;
  camera->unk8C.y = cam->cameraPos.y;
  camera->unk8C.z = cam->cameraPos.z;
  camera->unk98.x = f1;
  camera->unk98.y = f2;
  camera->unk98.z = f3;
  camera->unkA4.x = cam->cameraAt.x;
  camera->unkA4.y = cam->cameraAt.y;
  camera->unkA4.z = cam->cameraAt.z;
  camera->unkB0.x = f4;
  camera->unkB0.y = f5;
  camera->unkB0.z = f6;
  camera->unk84 = a;
  camera->unk88 = a;
  camera->unk80 = b;
}

USER_FUNC(evt_btl_camera_set_homing_unit) {
  BattleWork *wp;
  BattleCameraWork *camera;
  BattleUnit *unit;
  BattleUnit *unit2;
  BattleUnitPart *part;
  s32 *args = event->args;
  s32 priority = evtGetValue(event, args[0]);
  s32 type = evtGetValue(event, args[1]);
  s32 type2 = evtGetValue(event, args[2]);
  Vec pos;
  Vec pos2;
  Vec home;
  Vec hit;

  wp = _battleWorkPointer;
  camera = &wp->cameraWork;
  (void)priority;
  camera->unk10 = BattleTransID(event, type);
  unit = BattleGetUnitPtr(wp, camera->unk10);
  camera->unk18 = BtlUnit_GetBodyPartsId(unit);
  part = BattleGetUnitPartsPtr(camera->unk10, camera->unk18);
  BtlUnit_GetHomePos(unit, &pos.x, &pos.y, &pos.z);
  home = vec3_802efc64;
  home.x = pos.x;
  home.y = pos.y;
  home.z = pos.z;
  camera->unkBC = home;
  if (type2 != -1) {
    camera->unk14 = BattleTransID(event, type2);
    unit2 = BattleGetUnitPtr(wp, camera->unk14);
    camera->unk1C = BtlUnit_GetBodyPartsId(unit2);
    part = BattleGetUnitPartsPtr(camera->unk14, camera->unk1C);
    BtlUnit_GetHitPos(unit2, part, &pos2.x, &pos2.y, &pos2.z);
    hit = vec3_802efc70;
    hit.x = pos2.x;
    hit.y = pos2.y;
    hit.z = pos2.z;
    camera->unkC8 = hit;
  }
  return EVT_RETURN_DONE;
}

// NONMATCHING (95.04%): declaration order (s32 group split; wp/camera/unit/part before the f32s) fixed the
// register allocation. Still differs: the f32 stack groups are reversed (x at 0x1c, x2 at 0x08, the two Vec
// temporaries at 0x2c/0x20; target x 0x2c, x2 0x20, temps 0x14/0x08) and the first block's `unit` spills
// through r0/r25 instead of staying in r26 (3 inserts, 2 deletes). The Vec zero-vector relocations also land
// on anonymous symbols because the file's .rodata vectors (vec3_802efc7c/88) need battleCameraMain, unwritten.
// NONMATCHING
USER_FUNC(evt_btl_camera_set_homing_unitparts) {
  s32 *args;
  BattleWork *wp;
  s32 priority;
  s32 type;
  s32 partId;
  s32 type2;
  s32 partId2;
  BattleCameraWork *camera;
  BattleUnit *unit;
  BattleUnitPart *part;
  f32 x;
  f32 y;
  f32 z;
  f32 x2;
  f32 y2;
  f32 z2;

  args = event->args;
  priority = evtGetValue(event, args[0]);
  type = evtGetValue(event, args[1]);
  partId = evtGetValue(event, args[2]);
  type2 = evtGetValue(event, args[3]);
  partId2 = evtGetValue(event, args[4]);
  wp = _battleWorkPointer;
  camera = &wp->cameraWork;

  if (priority < camera->priLimit) {
    return EVT_RETURN_DONE;
  }
  camera->unk10 = BattleTransID(event, type);
  camera->unk18 = partId;
  unit = BattleGetUnitPtr(wp, camera->unk10);
  part = BattleGetUnitPartsPtr(camera->unk10, camera->unk18);
  BtlUnit_GetHomePos(unit, &x, &y, &z);
  camera->unkBC = (Vec){x, y, z};
  if (type2 != -1) {
    camera->unk14 = BattleTransID(event, type2);
    camera->unk1C = partId2;
    unit = BattleGetUnitPtr(wp, camera->unk14);
    BtlUnit_GetHitPos(unit, BattleGetUnitPartsPtr(camera->unk14, camera->unk1C), &x2, &y2, &z2);
    camera->unkC8 = (Vec){x2, y2, z2};
  }
  return EVT_RETURN_DONE;
}

// NONMATCHING (85.67%): 3 instructions differ, 7 missing, 5 extra, plus register differences.
USER_FUNC(evt_btl_camera_set_homing_unit_audience) {
  s32 *args = event->args;
  s32 priority, type, partId, memberId;
  BattleWork *wp;
  BattleCameraWork *camera;
  BattleAudience *audience;
  BattleUnit *unit;
  BattleUnitPart *part;
  f32 x, y, z;
  f32 x2, y2, z2;

  priority = evtGetValue(event, args[0]);
  type = evtGetValue(event, args[1]);
  partId = evtGetValue(event, args[2]);
  memberId = evtGetValue(event, args[3]);
  wp = _battleWorkPointer;
  camera = &wp->cameraWork;
  audience = BattleAudienceBaseGetPtr();
  if (priority < camera->priLimit) {
    return EVT_RETURN_DONE;
  }
  camera->unk10 = BattleTransID(event, type);
  camera->unk18 = partId;
  unit = BattleGetUnitPtr(wp, camera->unk10);
  part = BattleGetUnitPartsPtr(camera->unk10, camera->unk18);
  BtlUnit_GetPos(unit, &x, &y, &z);
  BtlUnit_GetHitPos(unit, part, &x2, &y2, &z2);
  camera->unkBC = (Vec){x, y, z};
  camera->unk20 = memberId;
  camera->unkC8 = (Vec){audience->members[memberId].position.x,
                        audience->members[memberId].position.y,
                        audience->members[memberId].position.z};
  return EVT_RETURN_DONE;
}

#pragma dont_inline reset
