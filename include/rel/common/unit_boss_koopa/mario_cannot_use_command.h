// mario_cannot_use_command, party_cannot_use_command and _icon_jump_icon (see rel/common/unit_boss_koopa.h). Included
// where they sit in the unit's function order (it sets the .text layout); the unit declares `cmd_disable_string[5]`
// before it.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "driver/dispdrv.h"
#include "evt/evt_cmd.h"
#include "rel/common/unit_boss_koopa.h"
#include "system.h"

static USER_FUNC(mario_cannot_use_command) {
    BattleWork* work = _battleWorkPointer;
    s32 type = irand(2);
    s32* args = event->args;
    s32 arg = args[0];
    s32 value = 0;

    switch (type) {
        case 0:
            work->field_19038[0] = 3;
            value = 0x146;
            break;
        case 1:
            work->field_19038[1] = 3;
            value = 0x149;
            break;
        case 2:
            work->field_19038[2] = 3;
            value = 0x14C;
            break;
    }
    event->lwData[0] = (s32)cmd_disable_string[type];
    evtSetValue(event, arg, value);
    return EVT_RETURN_DONE;
}

static USER_FUNC(party_cannot_use_command) {
    BattleWork* work = _battleWorkPointer;
    s32 type = irand(1);
    s32* args = event->args;
    s32 arg = args[0];
    s32 value = 0;

    switch (type) {
        case 0:
            work->field_19038[3] = 3;
            value = 0x150;
            break;
        case 1:
            work->field_19038[4] = 3;
            value = 0x14C;
            break;
    }
    event->lwData[0] = (s32)cmd_disable_string[type + 3];
    evtSetValue(event, arg, value);
    return EVT_RETURN_DONE;
}

static USER_FUNC(_icon_jump_icon) {
    IconJump* obj;
    s32* args;
    f32 x1;
    f32 y1;
    f32 z1;
    f32 x2;
    f32 y2;
    f32 z2;
    f32 accel;
    s32 frames;
    s32 icon;

    args = event->args;
    icon = evtGetValue(event, args[0]);
    x1 = evtGetFloat(event, args[1]);
    y1 = evtGetFloat(event, args[2]);
    z1 = evtGetFloat(event, args[3]);
    x2 = evtGetFloat(event, args[4]);
    y2 = evtGetFloat(event, args[5]);
    z2 = evtGetFloat(event, args[6]);
    accel = evtGetFloat(event, args[7]);
    frames = evtGetValue(event, args[8]);

    if (isFirstCall) {
        Vec pos;

        event->userdata[0] = (s32)BattleAlloc(sizeof(IconJump));
        obj = (IconJump*)event->userdata[0];
        obj->target.x = x2;
        obj->target.y = y2;
        obj->target.z = z2;
        pos.x = x1;
        pos.y = y1;
        pos.z = z1;
        obj->pos = pos;
        obj->start = pos;
        obj->fallAccel = accel;
        obj->frames = frames;
        // /2.0f (not *0.5f) makes MWCC put the float constant in f3 and the
        // int->float bias in f4, matching the original's register allocation.
        obj->vy = obj->fallAccel * obj->frames / 2.0f + (obj->target.y - obj->start.y) / obj->frames;
        obj->speed = distABf(obj->start.x, obj->start.z, obj->target.x, obj->target.z) / obj->frames;
        event->userdata[1] = icon;
        event->userdata[2] = 0;
    }

    obj = (IconJump*)event->userdata[0];
    if (event->userdata[2] != 0) {
        BattleFree(obj);
        return EVT_RETURN_DONE;
    }

    obj->pos.y += obj->vy;
    obj->vy -= obj->fallAccel;
    if (obj->vy < 0.0f && obj->pos.y < obj->target.y) {
        obj->pos.y = obj->target.y;
    }
    btlMovePos(&obj->pos.x, &obj->pos.z, obj->speed, angleABf(obj->start.x, obj->start.z, obj->target.x, obj->target.z));
    obj->frames--;
    if (obj->frames <= 0) {
        event->userdata[2] = 1;
        obj->pos = obj->target;
    }
    dispEntry(CAMERA_3D, 900.0f, 1, _disp_icon, event);
    return EVT_RETURN_BLOCK;
}
