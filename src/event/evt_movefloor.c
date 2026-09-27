#include <string.h>

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>

#include "driver/mapdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"

// Prototypes not yet in headers.

// Moving floor work structure. Not in a header yet.
typedef struct MovingFloorWork {
    /* 0x00 */ u32 flags;
    /* 0x04 */ s32 kind;
    /* 0x08 */ f32 speed;
    /* 0x0C */ f32 field_0x0C;
    /* 0x10 */ f32 field_0x10;
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ OSTime field_0x18;
    /* 0x20 */ s32 mode;
    /* 0x24 */ s32 direction;
    /* 0x28 */ s32 index;
    /* 0x2C */ Vec start;
    /* 0x38 */ Vec end;
    /* 0x44 */ Vec pos;
    /* 0x50 */ Vec prev;
    /* 0x5C */ f32 offsetY;
    /* 0x60 */ f32 prevOffsetY;
    /* 0x64 */ f32 time;
    /* 0x68 */ s32 field_0x68[3];
    /* 0x74 */ s32 field_0x74;
    /* 0x78 */ u32 field_0x78;
    /* 0x7C */ u32 seId;
    /* 0x80 */ f32 field_0x80;
    /* 0x84 */ f32 turn;
} MovingFloorWork;

// Compiler-pool float/double constants named after their original addresses.
f32 float_0_80420224 = 0.0f;
f32 float_neg1_80420228 = -1.0f;
f32 float_500_8042022c = 500.0f;
f32 float_3_80420230 = 3.0f;
f32 float_5p625_80420234 = 5.625f;
f32 float_360_80420238 = 360.0f;
f32 float_1000_8042023c = 1000.0f;
f32 float_0p5_80420240 = 0.5f;
f32 float_1_80420244 = 1.0f;

// Compiler-pool integer-conversion constants used by the (f32)<s32>/<u32> casts.
f64 double_to_int_802fb9f0 = 4503601774854144.0;
f64 double_to_int_mask_802fb9f8 = 4503599627370496.0;

// .sdata2 string.
const char str_80420220[4] = "yuu";

// .rodata strings. Shift-JIS bytes kept as octal escapes.
const char str_802FB9A8[24] = "\216\251\223\256\215\355\217\234\203\140\203F\203b\203N\221\322\202\277";
const char str_802FB9C0[16] = "\216\251\223\256\215\355\217\234\212\256\227\271";
const char str_802FB9D0[28] = "SFX_STG1_TIK_FLOOR_MOVE1";

// sin table, values are the raw f32 bit patterns.
static u32 sintbl[450] = {
	0x00000000, 0x3C8EF77F, 0x3D0EF241, 0x3D565E46, 0x3D8EDC3C, 0x3DB27ED8,
	0x3DD612C7, 0x3DF99674, 0x3E0E835E, 0x3E20303C, 0x3E31D0C8, 0x3E43636F,
	0x3E54E6E2, 0x3E66598E, 0x3E77BA67, 0x3E8483ED, 0x3E8D204B, 0x3E95B1C8,
	0x3E9E377A, 0x3EA6B0D9, 0x3EAF1D3F, 0x3EB77C03, 0x3EBFCC7D, 0x3EC80DE5,
	0x3ED03FD5, 0x3ED86163, 0x3EE0722A, 0x3EE87161, 0x3EF05E81, 0x3EF83904,
	0x3F000000, 0x3F03D988, 0x3F07A8C6, 0x3F0B6D76, 0x3F0F2746, 0x3F12D5E0,
	0x3F167914, 0x3F1A108C, 0x3F1D9BF6, 0x3F211B1E, 0x3F248DC1, 0x3F27F37C,
	0x3F2B4C2B, 0x3F2E976C, 0x3F31D51B, 0x3F3504F7, 0x3F3826AB, 0x3F3B3A04,
	0x3F3E3EC0, 0x3F41349C, 0x3F441B76, 0x3F46F30A, 0x3F49BB17, 0x3F4C7358,
	0x3F4F1BBD, 0x3F51B3F2, 0x3F543BC5, 0x3F56B314, 0x3F5919AC, 0x3F5B6F4C,
	0x3F5DB3D0, 0x3F5FE719, 0x3F6208D1, 0x3F6418F8, 0x3F66175D, 0x3F6803CD,
	0x3F69DE16, 0x3F6BA637, 0x3F6D5BEE, 0x3F6EFF19, 0x3F708FB8, 0x3F720D78,
	0x3F737868, 0x3F74D068, 0x3F761544, 0x3F7746ED, 0x3F786552, 0x3F797050,
	0x3F7A67E8, 0x3F7B4BE8, 0x3F7C1C61, 0x3F7CD91F, 0x3F7D8234, 0x3F7E177F,
	0x3F7E98FE, 0x3F7F06A3, 0x3F7F605B, 0x3F7FA637, 0x3F7FD817, 0x3F7FF60A,
	0x3F800000, 0x3F7FF60A, 0x3F7FD817, 0x3F7FA637, 0x3F7F605B, 0x3F7F06A3,
	0x3F7E98FE, 0x3F7E177F, 0x3F7D8234, 0x3F7CD91F, 0x3F7C1C61, 0x3F7B4BE8,
	0x3F7A67E8, 0x3F797050, 0x3F786552, 0x3F7746ED, 0x3F761544, 0x3F74D068,
	0x3F737879, 0x3F720D88, 0x3F708FB8, 0x3F6EFF2A, 0x3F6D5BEE, 0x3F6BA637,
	0x3F69DE27, 0x3F6803CD, 0x3F66175D, 0x3F641909, 0x3F6208E1, 0x3F5FE719,
	0x3F5DB3E1, 0x3F5B6F5D, 0x3F5919AC, 0x3F56B325, 0x3F543BD6, 0x3F51B3F2,
	0x3F4F1BBD, 0x3F4C7369, 0x3F49BB17, 0x3F46F30A, 0x3F441B86, 0x3F4134AD,
	0x3F3E3EC0, 0x3F3B3A04, 0x3F3826AB, 0x3F3504F7, 0x3F31D52C, 0x3F2E977D,
	0x3F2B4C2B, 0x3F27F37C, 0x3F248DC1, 0x3F211B2E, 0x3F1D9C06, 0x3F1A108C,
	0x3F167925, 0x3F12D5F1, 0x3F0F2746, 0x3F0B6D76, 0x3F07A8D6, 0x3F03D999,
	0x3F000000, 0x3EF83904, 0x3EF05EA2, 0x3EE87182, 0x3EE0724B, 0x3ED86184,
	0x3ED03FD5, 0x3EC80E06, 0x3EBFCC7D, 0x3EB77C24, 0x3EAF1D60, 0x3EA6B0FB,
	0x3E9E377A, 0x3E95B1C8, 0x3E8D206D, 0x3E84840E, 0x3E77BAAA, 0x3E6659D1,
	0x3E54E6E2, 0x3E43636F, 0x3E31D10B, 0x3E20307F, 0x3E0E83A1, 0x3DF996FB,
	0x3DD6134D, 0x3DB27ED8, 0x3D8EDCC2, 0x3D565F52, 0x3D0EF34D, 0x3C8EF998,
	0x358637BD, 0xBC8EF77F, 0xBD0EF241, 0xBD565D3A, 0xBD8EDC3C, 0xBDB27E52,
	0xBDD612C7, 0xBDF99674, 0xBE0E835E, 0xBE20303C, 0xBE31D0C8, 0xBE43632C,
	0xBE54E69F, 0xBE66594B, 0xBE77BA23, 0xBE8483ED, 0xBE8D204B, 0xBE95B1A7,
	0xBE9E3758, 0xBEA6B0D9, 0xBEAF1D1D, 0xBEB77BE1, 0xBEBFCC5C, 0xBEC80DC3,
	0xBED03FB3, 0xBED86163, 0xBEE0722A, 0xBEE87161, 0xBEF05E81, 0xBEF838E3,
	0xBEFFFFDE, 0xBF03D977, 0xBF07A8C6, 0xBF0B6D76, 0xBF0F2735, 0xBF12D5E0,
	0xBF167914, 0xBF1A107B, 0xBF1D9BF6, 0xBF211B1E, 0xBF248DB0, 0xBF27F36B,
	0xBF2B4C1B, 0xBF2E976C, 0xBF31D51B, 0xBF3504E6, 0xBF38269A, 0xBF3B39F3,
	0xBF3E3EAF, 0xBF41349C, 0xBF441B76, 0xBF46F2FA, 0xBF49BB06, 0xBF4C7358,
	0xBF4F1BAC, 0xBF51B3F2, 0xBF543BC5, 0xBF56B314, 0xBF5919AC, 0xBF5B6F4C,
	0xBF5DB3D0, 0xBF5FE708, 0xBF6208D1, 0xBF6418F8, 0xBF66175D, 0xBF6803BC,
	0xBF69DE16, 0xBF6BA626, 0xBF6D5BDD, 0xBF6EFF19, 0xBF708FA8, 0xBF720D78,
	0xBF737868, 0xBF74D057, 0xBF761533, 0xBF7746ED, 0xBF786541, 0xBF797050,
	0xBF7A67D7, 0xBF7B4BE8, 0xBF7C1C61, 0xBF7CD91F, 0xBF7D8234, 0xBF7E177F,
	0xBF7E98FE, 0xBF7F06A3, 0xBF7F605B, 0xBF7FA626, 0xBF7FD817, 0xBF7FF60A,
	0xBF800000, 0xBF7FF60A, 0xBF7FD817, 0xBF7FA637, 0xBF7F605B, 0xBF7F06A3,
	0xBF7E98FE, 0xBF7E177F, 0xBF7D8234, 0xBF7CD91F, 0xBF7C1C61, 0xBF7B4BE8,
	0xBF7A67E8, 0xBF797050, 0xBF786552, 0xBF7746ED, 0xBF761544, 0xBF74D068,
	0xBF737879, 0xBF720D88, 0xBF708FB8, 0xBF6EFF2A, 0xBF6D5BEE, 0xBF6BA637,
	0xBF69DE27, 0xBF6803CD, 0xBF66176E, 0xBF641909, 0xBF6208E1, 0xBF5FE719,
	0xBF5DB3E1, 0xBF5B6F5D, 0xBF5919BD, 0xBF56B325, 0xBF543BD6, 0xBF51B403,
	0xBF4F1BCE, 0xBF4C7369, 0xBF49BB17, 0xBF46F31B, 0xBF441B86, 0xBF4134AD,
	0xBF3E3ED1, 0xBF3B3A04, 0xBF3826BB, 0xBF350508, 0xBF31D52C, 0xBF2E977D,
	0xBF2B4C3C, 0xBF27F38C, 0xBF248DC1, 0xBF211B2E, 0xBF1D9C06, 0xBF1A109D,
	0xBF167925, 0xBF12D5F1, 0xBF0F2757, 0xBF0B6D87, 0xBF07A8D6, 0xBF03D999,
	0xBF000011, 0xBEF83926, 0xBEF05EC4, 0xBEE87182, 0xBEE0724B, 0xBED86184,
	0xBED03FD5, 0xBEC80E06, 0xBEBFCC9F, 0xBEB77C24, 0xBEAF1D82, 0xBEA6B0FB,
	0xBE9E379B, 0xBE95B1EA, 0xBE8D206D, 0xBE84840E, 0xBE77BAAA, 0xBE6659D1,
	0xBE54E725, 0xBE4363B2, 0xBE31D10B, 0xBE20307F, 0xBE0E83E4, 0xBDF99781,
	0xBDD613D3, 0xBDB27F5F, 0xBD8EDD48, 0xBD565F52, 0xBD0EF45A, 0xBC8EFBB1,
	0xB58637BD, 0x3C8EF566, 0x3D0EF241, 0x3D565D3A, 0x3D8EDBB6, 0x3DB27DCC,
	0x3DD61240, 0x3DF995EE, 0x3E0E831B, 0x3E202FF9, 0x3E31D085, 0x3E43632C,
	0x3E54E65C, 0x3E66594B, 0x3E77BA23, 0x3E8483CB, 0x3E8D2029, 0x3E95B1A7,
	0x3E9E3737, 0x3EA6B0B8, 0x3EAF1D1D, 0x3EB77BE1, 0x3EBFCC3A, 0x3EC80DC3,
	0x3ED03FB3, 0x3ED86141, 0x3EE07208, 0x3EE87161, 0x3EF05E81, 0x3EF838E3,
	0x3EFFFFDE, 0x3F03D977, 0x3F07A8B5, 0x3F0B6D65, 0x3F0F2724, 0x3F12D5D0,
	0x3F167903, 0x3F1A107B, 0x3F1D9BF6, 0x3F211B0D, 0x3F248DB0, 0x3F27F36B,
	0x3F2B4C1B, 0x3F2E975B, 0x3F31D50B, 0x3F3504D5, 0x3F38269A, 0x3F3B39F3,
	0x3F3E3EAF, 0x3F41349C, 0x3F441B76, 0x3F46F2FA, 0x3F49BB06, 0x3F4C7358,
	0x3F4F1BAC, 0x3F51B3E1, 0x3F543BC5, 0x3F56B314, 0x3F59199C, 0x3F5B6F3B,
	0x3F5DB3D0, 0x3F5FE708, 0x3F6208D1, 0x3F6418F8, 0x3F66174C, 0x3F6803BC,
	0x3F69DE16, 0x3F6BA626, 0x3F6D5BDD, 0x3F6EFF19, 0x3F708FA8, 0x3F720D78,
	0x3F737868, 0x3F74D057, 0x3F761533, 0x3F7746DC, 0x3F786541, 0x3F797050,
	0x3F7A67D7, 0x3F7B4BE8, 0x3F7C1C61, 0x3F7CD91F, 0x3F7D8234, 0x3F7E177F,
	0x3F7E98FE, 0x3F7F06A3, 0x3F7F605B, 0x3F7FA626, 0x3F7FD817, 0x3F7FF60A,
};

USER_FUNC(evt_moving_floor_delete) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    EventEntry* entry = evtGetPtrID(id);
    MovingFloorWork* work = (MovingFloorWork*)entry->lwData[9];

    if (work->field_0x78 == 0 && work->seId != 0) {
        psndSFXOff(work->seId);
        work->seId = 0;
    }
    if (work != NULL) {
        _mapFree(work);
        entry->lwData[9] = 0;
    }
    if (evtCheckID(id)) {
        evtDeleteID(id);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(moving_floor_halt_se) {
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, event->args[0]);

    work->field_0x78 = 1;
    return EVT_RETURN_DONE;
}

USER_FUNC(moving_floor_stop_se) {
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, event->args[0]);

    if (work->field_0x78 == 0 && work->seId != 0) {
        psndSFXOff(work->seId);
        work->seId = 0;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(moving_floor_set_se) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    Vec pos;
    MapObject* obj;
    s32 flag;

    if (work->field_0x78 != 0) {
        return EVT_RETURN_DONE;
    }
    if (work->seId == 0) {
        mapObjGetPos(name, &pos);
        obj = mapGetMapObj(name);
        if (obj == NULL) {
            mapErrorEntry(0, name);
            return EVT_RETURN_DONE;
        }
        flag = 0;
        if ((psndGetFlag() & 0x100) && !(*(u32*)((u8*)obj + 0x0) & 0x1800)) {
            flag = 0x10000;
        }
        work->seId = psndSFXOn_3D((const char*)(flag | 0x424), &pos);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(moving_floor_reverse) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, args[0]);
    f32 t;

    if (work->speed >= float_0_80420224) {
        return EVT_RETURN_DONE;
    }
    work->speed = work->speed * float_neg1_80420228;
    t = (f32)work->field_0x68[work->index] * work->time;
    work->field_0x10 = t - work->field_0x10;
    work->direction ^= 1;
    return EVT_RETURN_DONE;
}

USER_FUNC(moving_floor_getf_xz_scale) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, args[0]);
    f32 v;

    v = evtGetFloat(event, (s32)(work->pos.y + work->offsetY));
    evtSetFloat(event, args[1], (float_500_8042022c - v) / float_500_8042022c);
    return EVT_RETURN_DONE;
}

USER_FUNC(moving_floor_get_stmsec) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, args[0]);

    if (work->flags & 1) {
        evtSetValue(event, args[1], 0);
    } else {
        evtSetValue(event, args[1], work->field_0x74 * 60 / 1000);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(moving_floor_getf_now_bpoint) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, args[0]);

    evtSetFloat(event, args[1], work->pos.x - work->prev.x);
    evtSetFloat(event, args[2], (work->pos.y + work->offsetY) - (work->prev.y + work->prevOffsetY));
    evtSetFloat(event, args[3], work->pos.z - work->prev.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(moving_floor_getf_pos) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, args[0]);
    u32 idx;

    work->prevOffsetY = work->offsetY;
    if (work->kind == 1) {
        idx = (gp->retraceCount + event->eventId * 0x27) & 0x3F;
        work->offsetY = float_3_80420230 * ((f32*)sintbl)[(s32)(float_5p625_80420234 * (f32)idx)];
    } else {
        work->offsetY = float_0_80420224;
    }
    evtSetFloat(event, args[1], work->pos.x);
    evtSetFloat(event, args[2], work->pos.y + work->offsetY);
    evtSetFloat(event, args[3], work->pos.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(moving_floor_update_turn) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, args[0]);

    work->turn += work->field_0x80;
    if (work->turn < 0.0f) {
        work->turn += 360.0f;
    }
    if (work->turn >= 360.0f) {
        work->turn -= 360.0f;
    }
    evtSetFloat(event, args[1], work->turn);
    return EVT_RETURN_DONE;
}

USER_FUNC(moving_floor_get_mode) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, args[0]);

    evtSetValue(event, args[1], work->mode);
    return EVT_RETURN_DONE;
}

USER_FUNC(moving_floor_get_kind) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, args[0]);

    evtSetValue(event, args[1], work->kind);
    return EVT_RETURN_DONE;
}

USER_FUNC(moving_floor_get_pause) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, args[0]);

    if (work->flags & 2) {
        evtSetValue(event, args[1], 1);
    } else {
        evtSetValue(event, args[1], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(moving_floor_get_end) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, args[0]);
    s32 a = (s32)work->field_0x10;
    s32 b = (s32)((f32)work->field_0x68[work->index] * work->time);

    if (a >= b) {
        evtSetValue(event, args[1], 1);
    } else {
        evtSetValue(event, args[1], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_moving_floor_main2) {
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, event->args[0]);

    work->field_0x18 = event->lifetime;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_moving_floor_main) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, args[0]);
    s32 tick;
    s32 us;
    f32 dt;
    u32 idx;

    tick = ((s32*)&event->lifetime)[1] - ((s32*)&work->field_0x18)[1];
    us = OSTicksToMicroseconds(tick);
    dt = (f32)us;
    {
        f32 b = dt / 1000.0f;
        f32 a = work->speed * dt / 1000.0f;
        work->field_0x0C += a;
        work->field_0x10 += b;
    }

    work->prev.x = work->pos.x;
    work->prev.y = work->pos.y;
    work->prev.z = work->pos.z;
    work->prevOffsetY = work->offsetY;
    if (work->kind == 1) {
        idx = (gp->retraceCount + event->eventId * 0x27) & 0x3F;
        work->offsetY = float_3_80420230 * ((f32*)sintbl)[(s32)(float_5p625_80420234 * (f32)idx)];
    } else {
        work->offsetY = float_0_80420224;
    }
    ((f32*)&work->pos)[work->index] = work->field_0x0C;
    evtSetFloat(event, args[1], work->pos.x);
    evtSetFloat(event, args[2], work->pos.y + work->offsetY);
    evtSetFloat(event, args[3], work->pos.z);
    return EVT_RETURN_DONE;
}

// NONMATCHING (99.85%): the field_0x0C blend in the tail differs only in float
// register allocation. The original computes `t = 1.0f - ratio` into f1 and
// loads `start` into f2, while this build assigns f2 to `t` and f1 to `start`
// (and therefore emits `fmuls f0, f31, f0` instead of `fmuls f0, f0, f31`).
// Every byte, ordering and arithmetic matches otherwise; the constant pool
// refs (`float_0_*`, `double_to_int_*`, `jumptable_*`) are the same values at
// differing symbol names and do not affect the score.
USER_FUNC(evt_moving_floor_init) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, args[0]);
    s32 reverse;
    f32 ratio;
    f32 delta;

    switch (work->mode) {
    case 0:
        reverse = work->direction;
        ratio = 0.0f;
        work->direction++;
        if (work->direction >= 2) {
            work->direction = 0;
        }
        break;
    case 1:
        reverse = work->direction;
        ratio = 0.0f;
        work->direction++;
        if (work->direction >= 2) {
            work->direction = 0;
        }
        break;
    case 2:
        reverse = work->direction;
        ratio = 0.0f;
        work->direction++;
        if (work->direction >= 2) {
            work->direction = 0;
        }
        break;
    case 3:
        switch (work->direction) {
        case 0:
            work->flags &= ~1;
            work->index = 0;
            work->time = 1.0f;
            reverse = 1;
            ratio = 0.5f;
            break;
        case 1:
            work->flags |= 1;
            work->index = 0;
            work->time = 0.5f;
            reverse = 0;
            ratio = 0.0f;
            break;
        case 2:
            work->flags &= ~1;
            work->index = 2;
            work->time = 1.0f;
            reverse = 1;
            ratio = 0.5f;
            break;
        case 3:
            work->flags |= 1;
            work->index = 2;
            work->time = 0.5f;
            reverse = 0;
            ratio = 0.0f;
            break;
        case 4:
            work->flags &= ~1;
            work->index = 0;
            work->time = 1.0f;
            reverse = 0;
            ratio = 0.5f;
            break;
        case 5:
            work->flags |= 1;
            work->index = 0;
            work->time = 0.5f;
            reverse = 1;
            ratio = 0.0f;
            break;
        case 6:
            work->flags &= ~1;
            work->index = 2;
            work->time = 1.0f;
            reverse = 0;
            ratio = 0.5f;
            break;
        case 7:
            work->flags |= 1;
            work->index = 2;
            work->time = 0.5f;
            reverse = 1;
            ratio = 0.0f;
            break;
        }
        work->direction++;
        if (work->direction >= 8) {
            work->direction = 0;
        }
        break;
    case 4:
        switch (work->direction) {
        case 0:
            work->index = 2;
            work->time = 1.0f;
            reverse = 0;
            ratio = 0.0f;
            break;
        case 1:
            work->index = 0;
            work->time = 1.0f;
            reverse = 0;
            ratio = 0.0f;
            break;
        case 2:
            work->index = 2;
            work->time = 1.0f;
            reverse = 1;
            ratio = 0.0f;
            break;
        case 3:
            work->index = 0;
            work->time = 1.0f;
            reverse = 1;
            ratio = 0.0f;
            break;
        }
        work->direction++;
        if (work->direction >= 4) {
            work->direction = 0;
        }
        break;
    case 5:
        reverse = work->direction;
        ratio = 1.0f;
        break;
    default:
        break;
    }

    if (reverse != 0) {
        delta = ((f32*)&work->start)[work->index] - ((f32*)&work->end)[work->index];
    } else {
        delta = ((f32*)&work->end)[work->index] - ((f32*)&work->start)[work->index];
    }
    work->speed = delta / (f32)work->field_0x68[work->index];
    if (reverse != 0) {
        f32 u = ((f32*)&work->start)[work->index] * ratio;
        f32 t = 1.0f - ratio;
        work->field_0x0C = ((f32*)&work->end)[work->index] * t + u;
    } else {
        f32 u = ((f32*)&work->end)[work->index] * ratio;
        f32 t = 1.0f - ratio;
        work->field_0x0C = ((f32*)&work->start)[work->index] * t + u;
    }
    work->field_0x10 = (f32)work->field_0x68[work->index] * ratio;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_moving_floor_free) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)evtGetValue(event, args[0]);

    if (work == NULL) {
        return EVT_RETURN_DONE;
    }
    _mapFree(work);
    evtSetValue(event, args[0], 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_moving_floor_alloc) {
    s32* args = event->args;
    MovingFloorWork* work = (MovingFloorWork*)_mapAlloc(sizeof(MovingFloorWork));

    memset(work, 0, sizeof(MovingFloorWork));
    work->mode = event->lwData[6];
    work->kind = event->lwData[12];

    switch (work->mode) {
    case 0:
        work->flags &= ~1;
        work->index = 0;
        work->time = 1.0f;
        work->start.x = (f32)event->lwData[2];
        work->end.x = (f32)event->lwData[3];
        work->field_0x68[0] = event->lwData[4];
        work->pos.x = work->start.x;
        work->field_0x80 = (f32)event->lwData[11];
        break;
    case 1:
        work->index = 1;
        work->time = 1.0f;
        work->start.y = (f32)event->lwData[2];
        work->end.y = (f32)event->lwData[3];
        work->field_0x68[1] = event->lwData[4];
        work->pos.y = work->start.y;
        work->field_0x80 = (f32)event->lwData[11];
        break;
    case 2:
        work->index = 2;
        work->time = 1.0f;
        work->start.z = (f32)event->lwData[2];
        work->end.z = (f32)event->lwData[3];
        work->field_0x68[2] = event->lwData[4];
        work->pos.z = work->start.z;
        work->field_0x80 = (f32)event->lwData[11];
        break;
    case 3:
        work->index = 0;
        work->time = 1.0f;
        work->start.x = (f32)event->lwData[2];
        work->end.x = (f32)event->lwData[3];
        work->field_0x68[0] = event->lwData[4];
        work->pos.x = 0.5f * work->start.x + 0.5f * work->end.x;
        work->start.z = (f32)event->lwData[8];
        work->end.z = (f32)event->lwData[9];
        work->field_0x68[2] = event->lwData[10];
        work->pos.z = 0.5f * work->start.z + 0.5f * work->end.z;
        work->field_0x80 = (f32)event->lwData[11];
        break;
    case 4:
        work->index = 2;
        work->time = 1.0f;
        work->start.x = (f32)event->lwData[2];
        work->end.x = (f32)event->lwData[3];
        work->field_0x68[0] = event->lwData[4];
        work->pos.x = work->start.x;
        work->start.z = (f32)event->lwData[8];
        work->end.z = (f32)event->lwData[9];
        work->field_0x68[2] = event->lwData[10];
        work->pos.z = work->start.z;
        work->field_0x80 = (f32)event->lwData[11];
        break;
    case 5:
        work->flags &= ~1;
        work->index = 0;
        work->time = 1.0f;
        work->start.x = (f32)event->lwData[2];
        work->end.x = (f32)event->lwData[3];
        work->field_0x68[0] = event->lwData[4];
        work->pos.x = work->start.x;
        work->field_0x80 = (f32)event->lwData[11];
        break;
    default:
        break;
    }

    work->field_0x74 = event->lwData[5];
    work->prev.x = work->pos.x;
    work->prev.y = work->pos.y;
    work->prev.z = work->pos.z;
    work->prevOffsetY = work->offsetY;
    if (work->kind == 1) {
        u32 idx = (gp->retraceCount + event->eventId * 0x27) & 0x3F;
        work->offsetY = 3.0f * ((f32*)sintbl)[(s32)(5.625f * (f32)idx)];
    } else {
        work->offsetY = 0.0f;
    }
    evtSetValue(event, args[0], (s32)work);
    return EVT_RETURN_DONE;
}
