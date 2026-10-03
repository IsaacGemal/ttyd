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

// Prologue/epilogue (frame 0x100, stmw r25), the stack layout, every call and every rodata reference all match.
// What differs is FP/GP register allocation.
// NONMATCHING (98.18%): register allocation.
void battleCameraMain(void) {
  CameraEntry* cam;
  BattleWork* wp;
  BattleAudience* audience;
  BattleUnit* unit;
  BattleUnitPart* part;
  BattleUnit* unit2;
  BattleUnitPart* part2;
  PouchData* pouch;
  BattleAudienceMember* member;
  Vec vv;
  Vec hv;
  f32 o1, o2, o3, o4, o5, o6;
  f32 t1, t2, t3;
  f32 zb, zc;
  f32 zero;
  char name[0x40];

  cam = camGetPtr(CAMERA_3D);
  wp = _battleWorkPointer;
  audience = BattleAudienceBaseGetPtr();
  zero = 0.0f;
  zc = 0.0f; // the original's f30 starts at 0.0 (prologue `lfs f30`): zc when rank is outside 0..3
  (void)BattleGetMarioPtr(wp);
  o1 = zero;
  o2 = zero;
  o3 = zero;
  o4 = zero;
  o5 = zero;
  o6 = zero;

  if (wp->cameraWork.mode != 3) {
    wp->cameraWork.unk84 = 0;
  }

  cam->cameraPos.x = *(f32*)((f32*)&wp->cameraWork + 11);
  cam->cameraPos.y = *(f32*)((f32*)&wp->cameraWork + 12);
  cam->cameraPos.z = *(f32*)((f32*)&wp->cameraWork + 13);
  cam->cameraAt.x = *(f32*)((f32*)&wp->cameraWork + 17);
  cam->cameraAt.y = *(f32*)((f32*)&wp->cameraWork + 18);
  cam->cameraAt.z = *(f32*)((f32*)&wp->cameraWork + 19);
  *(f32*)((f32*)&wp->cameraWork + 14) = cam->cameraPos.x;
  *(f32*)((f32*)&wp->cameraWork + 15) = cam->cameraPos.y;
  *(f32*)((f32*)&wp->cameraWork + 16) = cam->cameraPos.z;
  *(f32*)((f32*)&wp->cameraWork + 20) = cam->cameraAt.x;
  *(f32*)((f32*)&wp->cameraWork + 21) = cam->cameraAt.y;
  *(f32*)((f32*)&wp->cameraWork + 22) = cam->cameraAt.z;

  switch (wp->cameraWork.mode) {
  case 0:
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, 0.0f, 110.0f, 750.0f, 0.0f, 60.0f, 0.0f);
    strcpy(name, "\202\251\202\337\202\347\201@\202\305\202\323\202\247\202\351\202\306");
    break;
  case 1:
    zb = 0.5f * wp->cameraWork.zoom;
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, 0.0f, 110.0f, 750.0f - zb, 0.0f, 60.0f, -zb);
    strcpy(name, "\202\251\202\337\202\347\201@\202\305\202\323\202\247\202\351\202\306\203Y\201[\203\200");
    break;
  case 2:
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, 0.0f, 160.0f, 750.0f, 0.0f, 110.0f, 0.0f);
    strcpy(name, "\202\251\202\337\202\347\201@\202\251\201[\202\273\202\351\202\252\202\244\202\246\202\311");
    /* fallthrough */
  case 3: {
    if (wp->cameraWork.unk84 > 0) {
      s16 save = wp->cameraWork.moveSpeedLv;

      wp->cameraWork.unk84--;
      vv.x = intplGetValue(wp->cameraWork.unk80, wp->cameraWork.unk88 - wp->cameraWork.unk84, wp->cameraWork.unk88,
                         wp->cameraWork.unk8C.x, wp->cameraWork.unk98.x);
      vv.y = intplGetValue(wp->cameraWork.unk80, wp->cameraWork.unk88 - wp->cameraWork.unk84, wp->cameraWork.unk88,
                         wp->cameraWork.unk8C.y, wp->cameraWork.unk98.y);
      vv.z = intplGetValue(wp->cameraWork.unk80, wp->cameraWork.unk88 - wp->cameraWork.unk84, wp->cameraWork.unk88,
                         wp->cameraWork.unk8C.z, wp->cameraWork.unk98.z);
      t1 = intplGetValue(wp->cameraWork.unk80, wp->cameraWork.unk88 - wp->cameraWork.unk84, wp->cameraWork.unk88,
                         wp->cameraWork.unkA4.x, wp->cameraWork.unkB0.x);
      t2 = intplGetValue(wp->cameraWork.unk80, wp->cameraWork.unk88 - wp->cameraWork.unk84, wp->cameraWork.unk88,
                         wp->cameraWork.unkA4.y, wp->cameraWork.unkB0.y);
      t3 = intplGetValue(wp->cameraWork.unk80, wp->cameraWork.unk88 - wp->cameraWork.unk84, wp->cameraWork.unk88,
                         wp->cameraWork.unkA4.z, wp->cameraWork.unkB0.z);
      wp->cameraWork.moveSpeedLv = 3;
      wp->cameraWork.zoomSpeedLv = 3;
      battleCameraMoveTo(0, 0, &o1, &o2, &o3, &o4, &o5, &o6, vv.x, vv.y, vv.z, t1, t2, t3);
      wp->cameraWork.moveSpeedLv = save;
    }
    strcpy(name, "\202\251\202\337\202\347\201@\202\323\202\350\201[");
    break;
  }
  case 4:
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, 0.0f, 110.0f, -750.0f, 0.0f, 60.0f, 0.0f);
    strcpy(name, "\202\251\202\337\202\347\201@\202\324\202\275\202\242\202\244\202\347");
    break;
  case 5:
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, 0.0f, 400.0f, -617.0f, 0.0f, 60.0f, 0.0f);
    strcpy(name, "\202\251\202\337\202\347\201@\202\324\202\275\202\242\202\244\202\347\202Q");
    break;
  case 6:
    pouch = pouchGetPtr();
    switch (pouch->rank) {
    case 0:
      zc = 750.0f;
      break;
    case 1:
      zc = 830.0f;
      break;
    case 2:
      zc = 920.0f;
      break;
    case 3:
      zc = 1000.0f;
      break;
    }
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, 0.0f, 110.0f, zc, 0.0f, 60.0f, 0.0f);
    strcpy(name, "\202\251\202\337\202\347\201@\202\251\202\361\202\253\202\341\202\255");
    break;
  case 7: {
    f32 w;
    f32 dz;
    f32 cvy;
    f32 off;
    BattleUnit* u;

    u = BattleGetUnitPtr(wp, wp->cameraWork.unk10);
    if (u == NULL) {
      btl_camera_set_mode(0, 0);
      break;
    }
    part = BattleGetUnitPartsPtr(wp->cameraWork.unk10, wp->cameraWork.unk18);
    BtlUnit_GetPos(u, &vv.x, &vv.y, &vv.z);
    {
      f32 hh = (f32)BtlUnit_GetHeight(u) - 50.0f;
      hh *= 0.5f;
      vv.y += hh;
    }
    w = (f32)BtlUnit_GetWidth(u) - 24.0f;
    if (w < 0.0f) {
      w = 0.0f;
    }
    dz = wp->cameraWork.zoom;
    dz = dz - 1.5f * w;
    cvy = vv.y;
    off = -dz / 9.0f;
    if (cvy > 100.0f) {
      cvy = 100.0f;
    }
    if (vv.y < 0.0f) {
      cvy = 0.0f;
    }
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, vv.x, 110.0f + cvy + off, 750.0f - dz, vv.x,
                       60.0f + cvy + off, -dz);
    strcpy(name,
           "\202\251\202\337\202\347\201@\202\344\202\311\202\301\202\306\202\302\202\242\202\321");
    break;
  }
  case 8: {
    f32 wa;
    f32 num;
    f32 f9;
    f32 dxc;
    f32 xv;
    f32 f7;
    f32 t;
    f32 v;
    f32 f11;
    f32 base;
    f32 wb;

    unit = BattleGetUnitPtr(wp, wp->cameraWork.unk10);
    unit2 = BattleGetUnitPtr(wp, wp->cameraWork.unk14);
    if (unit == NULL || unit2 == NULL) {
      btl_camera_set_mode(0, 0);
      break;
    }
    part = BattleGetUnitPartsPtr(wp->cameraWork.unk10, wp->cameraWork.unk18);
    part = BattleGetUnitPartsPtr(wp->cameraWork.unk14, wp->cameraWork.unk1C);
    BtlUnit_GetPos(unit, &vv.x, &vv.y, &vv.z);
    BtlUnit_GetHitPos(unit2, part, &hv.x, &hv.y, &hv.z);
    wa = (f32)BtlUnit_GetWidth(unit) - 24.0f;
    wb = (f32)BtlUnit_GetWidth(unit2) - 24.0f;
    if (wa < 0.0f) {
      wa = 0.0f;
    }
    if (wb < 0.0f) {
      wb = 0.0f;
    }
    dxc = wp->cameraWork.unkC8.x - wp->cameraWork.unkBC.x;
    num = hv.x - vv.x;
    if (dxc == 0.0f) {
      t = 0.5f;
    } else {
      t = num / dxc;
    }
    if (t > 1.0f) {
      t = 1.0f;
    }
    if (t < 0.5f) {
      t = 0.5f;
    }
    base = 2.0f * wp->cameraWork.zoom;
    base -= 1.5f * (wa + wb);
    v = (1.0f - t) * base;
    xv = (1.0f - t) * wp->cameraWork.unkC8.x;
    f11 = -v / 10.0f;
    f9 = (wp->cameraWork.unkC8.y / 80.0f) * (vv.y - wp->cameraWork.unkC8.y);
    if (f9 > 100.0f) {
      f9 = 100.0f;
    }
    if (f9 < 0.0f) {
      f9 = 0.0f;
    }
    f7 = 20.0 * -(1.0 - t) + f9;
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, xv, 110.0f + f7 + f11, 750.0f - v,
                       xv, 60.0f + f7 + f11, -v);
    strcpy(name,
           "\202\251\202\337\202\347\201@\202Q\202\344\202\311\202\301\202\306\202\251\202\361\202\302\202\242\202\321");
    break;
  }
  case 9: {
    f32 w;
    f32 base;
    f32 off;
    f32 yc;
    f32 mid;
    f32 xc;
    BattleUnitPart* pt;

    unit = BattleGetUnitPtr(wp, wp->cameraWork.unk10);
    unit2 = BattleGetUnitPtr(wp, wp->cameraWork.unk14);
    if (unit == NULL || unit2 == NULL) {
      btl_camera_set_mode(0, 0);
      break;
    }
    pt = BattleGetUnitPartsPtr(wp->cameraWork.unk10, wp->cameraWork.unk18);
    pt = BattleGetUnitPartsPtr(wp->cameraWork.unk14, wp->cameraWork.unk1C);
    BtlUnit_GetPos(unit, &vv.x, &vv.y, &vv.z);
    BtlUnit_GetHitPos(unit2, pt, &hv.x, &hv.y, &hv.z);
    w = (f32)BtlUnit_GetWidth(unit) - 24.0f;
    (void)BtlUnit_GetWidth(unit2);
    if (w < 0.0f) {
      w = 0.0f;
    }
    base = 2.0f * wp->cameraWork.zoom;
    base -= 1.5f * w;
    // The original only writes yc in the two clamp arms; an in-range midpoint leaves it as it was.
    mid = 0.5f * (vv.y + hv.y);
    off = -base / 10.0f;
    if (mid > 50.0f) {
      yc = 50.0f;
    }
    if (mid < 0.0f) {
      yc = 0.0f;
    }
    xc = 0.5f * (vv.x + hv.x);
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, xc, 110.0f + yc + off, 750.0f - base, xc,
                       60.0f + yc + off, -base);
    strcpy(name,
           "\202\251\202\337\202\347\201@\202\344\202\311\202\301\202\306\202\302\202\242\202\321");
    break;
  }
  case 10: {
    f32 w;
    f32 dz;
    f32 cvy;
    f32 off;

    unit = BattleGetUnitPtr(wp, wp->cameraWork.unk10);
    if (unit == NULL) {
      btl_camera_set_mode(0, 0);
      break;
    }
    part = BattleGetUnitPartsPtr(wp->cameraWork.unk10, wp->cameraWork.unk18);
    if (part == NULL) {
      btl_camera_set_mode(0, 0);
      break;
    }
    BtlUnit_GetHitPos(unit, part, &vv.x, &vv.y, &vv.z);
    {
      f32 hh = (f32)BtlUnit_GetHeight(unit) - 50.0f;
      hh *= 0.5f;
      vv.y += hh;
    }
    w = (f32)BtlUnit_GetWidth(unit) - 24.0f;
    if (w < 0.0f) {
      w = 0.0f;
    }
    dz = wp->cameraWork.zoom;
    dz = dz - 1.5f * w;
    cvy = vv.y;
    off = -dz / 9.0f;
    if (cvy > 100.0f) {
      cvy = 100.0f;
    }
    if (vv.y < 0.0f) {
      cvy = 0.0f;
    }
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, vv.x, 110.0f + cvy + off, 750.0f - dz, vv.x,
                       60.0f + cvy + off, -dz);
    strcpy(name, "\202\251\202\337\202\347\201@\202\317\201[\202\302\202\302\202\242\202\321");
    break;
  }
  case 11: {
    f32 wa;
    f32 num;
    f32 f9;
    f32 dxc;
    f32 xv;
    f32 f7;
    f32 t;
    f32 v;
    f32 f11;
    f32 base;
    f32 wb;

    unit = BattleGetUnitPtr(wp, wp->cameraWork.unk10);
    unit2 = BattleGetUnitPtr(wp, wp->cameraWork.unk14);
    if (unit == NULL || unit2 == NULL) {
      btl_camera_set_mode(0, 0);
      break;
    }
    part = BattleGetUnitPartsPtr(wp->cameraWork.unk10, wp->cameraWork.unk18);
    part2 = BattleGetUnitPartsPtr(wp->cameraWork.unk14, wp->cameraWork.unk1C);
    if (part == NULL || part2 == NULL) {
      btl_camera_set_mode(0, 0);
      break;
    }
    BtlUnit_GetPartsWorldPos(part, &vv.x, &vv.y, &vv.z);
    BtlUnit_GetHitPos(unit2, part2, &hv.x, &hv.y, &hv.z);
    wa = (f32)BtlUnit_GetWidth(unit) - 24.0f;
    wb = (f32)BtlUnit_GetWidth(unit2) - 24.0f;
    if (wa < 0.0f) {
      wa = 0.0f;
    }
    if (wb < 0.0f) {
      wb = 0.0f;
    }
    dxc = wp->cameraWork.unkC8.x - wp->cameraWork.unkBC.x;
    num = hv.x - vv.x;
    if (dxc == 0.0f) {
      dxc = 1e-05f;
    }
    t = num / dxc;
    if (t > 1.0f) {
      t = 1.0f;
    }
    if (t < 0.5f) {
      t = 0.5f;
    }
    base = 2.0f * wp->cameraWork.zoom;
    base -= 1.5f * (wa + wb);
    v = (1.0f - t) * base;
    xv = (1.0f - t) * wp->cameraWork.unkC8.x;
    f11 = -v / 10.0f;
    f9 = (wp->cameraWork.unkC8.y / 80.0f) * (vv.y - wp->cameraWork.unkC8.y);
    if (f9 > 100.0f) {
      f9 = 100.0f;
    }
    if (f9 < 0.0f) {
      f9 = 0.0f;
    }
    f7 = 20.0 * -(1.0 - t) + f9;
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, xv, 110.0f + f7 + f11, 750.0f - v,
                       xv, 60.0f + f7 + f11, -v);
    strcpy(name,
           "\202\251\202\337\202\347\201@\202Q\202\317\201[\202\302\202\251\202\361\202\302\202\242\202\321");
    break;
  }
  case 12:
    member = &audience->members[wp->cameraWork.unk20];
    battleCameraMoveTo(1, 0, &o1, &o2, &o3, &o4, &o5, &o6, member->position.x, 200.0f,
                       400.0f + member->position.z, member->position.x, 0.0f, member->position.z);
    strcpy(name,
           "\202\251\202\337\202\347\201@\202\251\202\361\202\253\202\341\202\255\202\302\202\242\202\321");
    break;
  case 13: {
    f32 w;
    f32 cy;
    f32 t;
    f32 v;
    f32 dx;
    f32 dxn;
    f32 zz;
    f32 u;
    f32 off;

    unit = BattleGetUnitPtr(wp, wp->cameraWork.unk10);
    if (unit == NULL) {
      btl_camera_set_mode(0, 0);
      break;
    }
    part = BattleGetUnitPartsPtr(wp->cameraWork.unk10, wp->cameraWork.unk18);
    if (part == NULL) {
      btl_camera_set_mode(0, 0);
      break;
    }
    wp->cameraWork.unk20 = 10;
    dx = wp->cameraWork.unkC8.x - wp->cameraWork.unkBC.x;
    BtlUnit_GetPos(unit, &vv.x, &vv.y, &vv.z);
    dxn = wp->cameraWork.unkC8.x - vv.x;
    if (dx == 0.0f) {
      t = 0.5f;
    } else {
      t = dxn / dx;
    }
    if (t > 1.0f) {
      t = 1.0f;
    }
    if (t < 0.5f) {
      t = 0.5f;
    }
    u = 1.0f - t;
    cy = vv.y - wp->cameraWork.unkBC.y - 100.0f;
    zz = 2.0f * wp->cameraWork.zoom;
    v = u * wp->cameraWork.unkC8.x;
    if (cy > 0.0f) {
      cy = 0.0f;
    }
    if (cy < -30.0f) {
      cy = -30.0f;
    }
    off = -(1.0 - t) * (125.0f - cy);
    w = u * zz;
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, v, 110.0f + off, 750.0f - w, v, 60.0f + off,
                       -w);
    strcpy(name,
           "\202\251\202\337\202\347\201@\202\344\202\311\202\301\202\306\202\251\202\361\202\253\202\341\202\255\202\251\202\361\202\302\202\242\202\321");
    break;
  }
  case 14:
    wp->cameraWork.unk4++;
    if (wp->cameraWork.unk4 == 1) {
      pouch = pouchGetPtr();
      switch (pouch->rank) {
      case 0:
        zc = 750.0f;
        break;
      case 1:
        zc = 830.0f;
        break;
      case 2:
        zc = 920.0f;
        break;
      case 3:
        zc = 1000.0f;
        break;
      }
      wp->cameraWork.moveSpeedLv = 3;
      wp->cameraWork.zoomSpeedLv = 3;
      battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, 0.0f, 110.0f, zc, 0.0f, 60.0f, 0.0f);
    }
    if (wp->cameraWork.unk4 == 10) {
      wp->cameraWork.unk4 = 0;
      btl_camera_set_mode(0, 3);
      btl_camera_set_moveto(0, 120, 4, 0.0f, 110.0f, 750.0f, 0.0f, 60.0f, 0.0f);
    }
    strcpy(name,
           "\202\251\202\337\202\347\201@\202\323\202\302\202\244\202\311\202\315\202\242\202\301\202\275");
    break;
  case 15:
    wp->cameraWork.moveSpeedLv = 3;
    wp->cameraWork.zoomSpeedLv = 3;
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, 0.0f, 110.0f, 500.0f, 0.0f, 60.0f, 0.0f);
    strcpy(name,
           "\202\251\202\337\202\347\201@\202\244\202\334\202\255\202\315\202\242\202\301\202\275");
    break;
  case 16:
    wp->cameraWork.moveSpeedLv = 3;
    wp->cameraWork.zoomSpeedLv = 3;
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, -100.0f, 110.0f, 500.0f, -100.0f, 60.0f,
                       0.0f);
    strcpy(name,
           "\202\251\202\337\202\347\201@\202\276\202\337\202\311\202\315\202\242\202\301\202\275");
    break;
  case 17:
  case 18: {
    f32 mx;
    f32 my;
    f32 mz;
    f32 h1x, h1y, h1z, h2x, h2y, h2z;
    BattleUnit* u;
    BattleUnit* u2;

    u = BattleGetMarioPtr(wp);
    u2 = BattleGetPartyPtr(wp);
    if (u2 != NULL) {
      BtlUnit_GetHomePos(u, &h1x, &h1y, &h1z);
      BtlUnit_GetHomePos(u2, &h2x, &h2y, &h2z);
      mx = 0.5f * (h1x + h2x);
      my = 0.5f * (h1y + h2y);
      mz = 0.5f * (h1z + h2z);
      if (wp->cameraWork.mode == 18) {
        my -= 20.0f;
      }
    } else {
      BtlUnit_GetHomePos(u, &h1x, &h1y, &h1z);
      mx = h1x;
      my = h1y;
      mz = h1z;
    }
    wp->cameraWork.moveSpeedLv = 1;
    wp->cameraWork.zoomSpeedLv = 1;
    battleCameraMoveTo(0, 1, &o1, &o2, &o3, &o4, &o5, &o6, mx, 100.0f + my, 350.0f + mz, mx,
                       50.0f + my, 0.0f);
    strcpy(name, "\202\251\202\337\202\347\201@\202\265\202\345\202\244\202\350");
    break;
  }
  case 19:
    strcpy(name, "\202\251\202\337\202\347\201@\202\315\202\242\202\332\202\255");
    break;
  case 20:
    wp->cameraWork.moveSpeedLv = 2;
    wp->cameraWork.zoomSpeedLv = 2;
    battleCameraMoveTo(1, 1, &o1, &o2, &o3, &o4, &o5, &o6, -100.0f, 100.0f, 400.0f, -100.0f, 50.0f,
                       0.0f);
    strcpy(name, "\202\251\202\337\202\347\201@\202\311\202\260\202\304\202\351");
    break;
  default:
    strcpy(name, "\202\251\202\337\202\347\201@\202\250\202\251\202\265\202\242");
    break;
  }

  if (cam->cameraPos.z >= 1000.0f) {
    cam->cameraPos.z = 1000.0f;
  }
  if (wp->cameraWork.flags & 4) {
    cam->cameraPos.x = *(f32*)((f32*)&wp->cameraWork + 14);
    cam->cameraAt.x = *(f32*)((f32*)&wp->cameraWork + 20);
    o1 = 0.0f;
    o4 = 0.0f;
  }
  if (wp->cameraWork.flags & 8) {
    cam->cameraPos.y = *(f32*)((f32*)&wp->cameraWork + 15);
    cam->cameraAt.y = *(f32*)((f32*)&wp->cameraWork + 21);
    o2 = 0.0f;
    o5 = 0.0f;
  }
  if (wp->cameraWork.flags & 0x10) {
    cam->cameraPos.z = *(f32*)((f32*)&wp->cameraWork + 16);
    cam->cameraAt.z = *(f32*)((f32*)&wp->cameraWork + 22);
    o3 = 0.0f;
    o6 = 0.0f;
  }
  *(f32*)((f32*)&wp->cameraWork + 11) = cam->cameraPos.x;
  *(f32*)((f32*)&wp->cameraWork + 12) = cam->cameraPos.y;
  *(f32*)((f32*)&wp->cameraWork + 13) = cam->cameraPos.z;
  *(f32*)((f32*)&wp->cameraWork + 17) = cam->cameraAt.x;
  *(f32*)((f32*)&wp->cameraWork + 18) = cam->cameraAt.y;
  *(f32*)((f32*)&wp->cameraWork + 19) = cam->cameraAt.z;
  {
    Vec tmp = vec3_802efc4c;
    tmp.x = o1;
    tmp.y = o2;
    tmp.z = o3;
    *(Vec*)((f32*)&wp->cameraWork + 26) = tmp;
  }
  {
    Vec tmp2 = vec3_802efc58;
    tmp2.x = o4;
    tmp2.y = o5;
    tmp2.z = o6;
    *(Vec*)((f32*)&wp->cameraWork + 29) = tmp2;
  }
  if (!(wp->cameraWork.flags & 1) && !(wp->cameraWork.flags & 2)) {
    padRumbleOff(0);
  }
  if (wp->cameraWork.flags & 0x40) {
    padRumbleOff(0);
  }
  if (wp->cameraWork.flags & 1) {
    s16 saveMove = wp->cameraWork.moveSpeedLv;
    s16 saveZoom = wp->cameraWork.zoomSpeedLv;
    s32 odd;

    odd = (wp->cameraWork.unkDC - wp->cameraWork.unkE4) & 2;
    wp->cameraWork.moveSpeedLv = 3;
    wp->cameraWork.zoomSpeedLv = 3;
    *(f32*)&wp->cameraWork.unkFC[0] = intplGetValue(wp->cameraWork.unkD4, (wp->cameraWork.unkDC - wp->cameraWork.unkE4) / 2, wp->cameraWork.unkDC / 2, wp->cameraWork.unkEC, wp->cameraWork.unkF0);
    if (wp->cameraWork.unkE4 > 0) {
      if (odd == 0) {
        battleCameraMoveTo(0, 0, &o1, &o2, &o3, &o4, &o5, &o6, cam->cameraPos.x + *(f32*)&wp->cameraWork.unkFC[0],
                           cam->cameraPos.y, cam->cameraPos.z, cam->cameraAt.x + *(f32*)&wp->cameraWork.unkFC[0], cam->cameraAt.y,
                           cam->cameraAt.z);
      } else {
        battleCameraMoveTo(0, 0, &o1, &o2, &o3, &o4, &o5, &o6, cam->cameraPos.x - *(f32*)&wp->cameraWork.unkFC[0],
                           cam->cameraPos.y, cam->cameraPos.z, cam->cameraAt.x - *(f32*)&wp->cameraWork.unkFC[0], cam->cameraAt.y,
                           cam->cameraAt.z);
      }
    } else if (wp->cameraWork.unkE4 == 0) {
      wp->cameraWork.flags &= ~1;
    }
    wp->cameraWork.moveSpeedLv = saveMove;
    wp->cameraWork.zoomSpeedLv = saveZoom;
    wp->cameraWork.unkE4--;
  }
  if (wp->cameraWork.flags & 2) {
    s16 saveMove = wp->cameraWork.moveSpeedLv;
    s16 saveZoom = wp->cameraWork.zoomSpeedLv;
    s32 odd;

    odd = (wp->cameraWork.unkE0 - wp->cameraWork.unkE8) & 2;
    wp->cameraWork.moveSpeedLv = 3;
    wp->cameraWork.zoomSpeedLv = 3;
    *(f32*)&wp->cameraWork.unkFC[4] = intplGetValue(wp->cameraWork.unkD8, (wp->cameraWork.unkE0 - wp->cameraWork.unkE8) / 2, wp->cameraWork.unkE0 / 2, wp->cameraWork.unkF4, wp->cameraWork.unkF8);
    if (wp->cameraWork.unkE8 > 0) {
      if (odd == 0) {
        battleCameraMoveTo(0, 0, &o1, &o2, &o3, &o4, &o5, &o6, cam->cameraPos.x, cam->cameraPos.y + *(f32*)&wp->cameraWork.unkFC[4],
                           cam->cameraPos.z, cam->cameraAt.x, cam->cameraAt.y + *(f32*)&wp->cameraWork.unkFC[4], cam->cameraAt.z);
      } else {
        battleCameraMoveTo(0, 0, &o1, &o2, &o3, &o4, &o5, &o6, cam->cameraPos.x, cam->cameraPos.y - *(f32*)&wp->cameraWork.unkFC[4],
                           cam->cameraPos.z, cam->cameraAt.x, cam->cameraAt.y - *(f32*)&wp->cameraWork.unkFC[4], cam->cameraAt.z);
      }
    } else if (wp->cameraWork.unkE8 == 0) {
      wp->cameraWork.flags &= ~2;
    }
    wp->cameraWork.moveSpeedLv = saveMove;
    wp->cameraWork.zoomSpeedLv = saveZoom;
    wp->cameraWork.unkE8--;
  }
}

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

// Wrapping the call in this inline changes MWCC's scheduling of the return-value save
// (direct `mr r26, r3` instead of the r0 spill) without changing the emitted call.
static inline BattleUnit *getUnitPtr(BattleWork *wp, s32 id) {
  return BattleGetUnitPtr(wp, id);
}

USER_FUNC(evt_btl_camera_set_homing_unitparts) {
  BattleWork *wp;
  BattleCameraWork *camera;
  BattleUnit *unit;
  BattleUnitPart *part;
  s32 *args = event->args;
  s32 priority = evtGetValue(event, args[0]);
  s32 type = evtGetValue(event, args[1]);
  s32 partId = evtGetValue(event, args[2]);
  s32 type2 = evtGetValue(event, args[3]);
  s32 partId2 = evtGetValue(event, args[4]);
  Vec pos;
  Vec pos2;
  Vec home;
  Vec hit;

  wp = _battleWorkPointer;
  camera = &wp->cameraWork;
  if (priority < camera->priLimit) {
    return EVT_RETURN_DONE;
  }
  camera->unk10 = BattleTransID(event, type);
  camera->unk18 = partId;
  unit = getUnitPtr(wp, camera->unk10);
  part = BattleGetUnitPartsPtr(camera->unk10, camera->unk18);
  BtlUnit_GetHomePos(unit, &pos.x, &pos.y, &pos.z);
  home = vec3_802efc7c;
  home.x = pos.x;
  home.y = pos.y;
  home.z = pos.z;
  camera->unkBC = home;
  if (type2 != -1) {
    camera->unk14 = BattleTransID(event, type2);
    camera->unk1C = partId2;
    unit = getUnitPtr(wp, camera->unk14);
    part = BattleGetUnitPartsPtr(camera->unk14, camera->unk1C);
    BtlUnit_GetHitPos(unit, part, &pos2.x, &pos2.y, &pos2.z);
    hit = vec3_802efc88;
    hit.x = pos2.x;
    hit.y = pos2.y;
    hit.z = pos2.z;
    camera->unkC8 = hit;
  }
  return EVT_RETURN_DONE;
}

USER_FUNC(evt_btl_camera_set_homing_unit_audience) {
  s32 *args = event->args;
  s32 priority, type, partId, memberId;
  BattleWork *wp;
  BattleCameraWork *camera;
  BattleAudience *audience;
  BattleAudienceMember *member;
  BattleUnit *unit;
  BattleUnitPart *part;
  Vec pos;
  Vec pos2;
  Vec home;
  Vec hit;

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
  BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
  BtlUnit_GetHitPos(unit, part, &pos2.x, &pos2.y, &pos2.z);
  home = vec3_802efc94;
  home.x = pos.x;
  home.y = pos.y;
  home.z = pos.z;
  camera->unkBC = home;
  camera->unk20 = memberId;
  member = &audience->members[memberId];
  hit = (Vec){member->position.x, member->position.y, member->position.z};
  camera->unkC8 = hit;
  return EVT_RETURN_DONE;
}

#pragma dont_inline reset
