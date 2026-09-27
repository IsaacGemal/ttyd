#include <string.h>

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include <dolphin/gx.h>

#include "driver/animdrv.h"
#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_pouch.h"
#include "party.h"
#include "evt/evt_case.h"
#include "eff/eff_torch.h"
#include "eff/eff_fire.h"

// Prototypes not yet in headers.
EVT_DEF(evt_ride_airport);
EVT_DEF(evt_nonride_airport);

// .rodata
const char str_802C1F08[12] = "p_hikohki";
const char str_802C1F14[8] = "p_hune";
const char str_802C1F1C[12] = "extparty";

// .sdata2
const u32 dat_80419958 = 0xFF;

// .sbss2: the default .sbss2 is only used for compiler-generated pool
// templates, so pair it with a small-data section and place these by hand.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80422fbc;
__declspec(section ".sdata") GXColor unk_80422fb8;
__declspec(section ".sdata") GXColor unk_80422fb4;
__declspec(section ".sdata") GXColor unk_80422fb0;

USER_FUNC(evt_mapobj_trans) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 name = evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);

    if (type == 0) {
        mapObjTranslate((const char*)name, x, y, z);
    }
    else {
        mapObjTranslate((const char*)name, x, y, z);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_rotate) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 name = evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);

    if (type == 0) {
        mapObjRotate((const char*)name, x, y, z);
    }
    else {
        mapObjRotate((const char*)name, x, y, z);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_scale) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 name = evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);

    if (type == 0) {
        mapObjScale((const char*)name, x, y, z);
    }
    else {
        mapObjScale((const char*)name, x, y, z);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_set_fog) {
    s32* args = event->args;
    s32 color = evtGetValue(event, args[0]);
    f32 near = evtGetFloat(event, args[1]);
    f32 far = evtGetFloat(event, args[2]);
    s32 r = evtGetValue(event, args[3]);
    s32 g = evtGetValue(event, args[4]);
    s32 b = evtGetValue(event, args[5]);
    GXColor c;
    GXColor color2;

    color2 = *(GXColor*)&dat_80419958;
    color2.r = r;
    color2.g = g;
    color2.b = b;
    c = color2;
    mapSetFog(color, near, far, (u32*)&c);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_get_fog) {
    s32* args = event->args;
    u32 color;
    f32 near;
    f32 far;
    GXColor color2;

    mapGetFog(&color, &near, &far, (u32*)&color2);
    evtSetValue(event, args[0], color);
    evtSetFloat(event, args[1], near);
    evtSetFloat(event, args[2], far);
    evtSetValue(event, args[3], color2.r);
    evtSetValue(event, args[4], color2.g);
    evtSetValue(event, args[5], color2.b);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_fog_onoff) {
    s32* args = event->args;

    if (evtGetValue(event, args[0])) {
        mapFogOn();
    }
    else {
        mapFogOff();
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_set_blend) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 r = evtGetValue(event, args[1]);
    s32 g = evtGetValue(event, args[2]);
    s32 b = evtGetValue(event, args[3]);
    s32 a = evtGetValue(event, args[4]);

    if (type == 0) {
        GXColor c;
        GXColor color;

        color = *(GXColor*)&unk_80422fb0;
        color.r = r;
        color.g = g;
        color.b = b;
        color.a = a;
        c = color;
        mapSetBlend(&c);
    }
    else {
        GXColor c;
        GXColor color;

        color = *(GXColor*)&unk_80422fb4;
        color.r = r;
        color.g = g;
        color.b = b;
        color.a = a;
        c = color;
        mapSetBlend2(&c);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_blend_off) {
    s32* args = event->args;

    if (evtGetValue(event, args[0]) == 0) {
        mapBlendOff();
    }
    else {
        mapBlendOff2();
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_blend_set_flag) {
    s32* args = event->args;
    s32 onoff = evtGetValue(event, args[0]);
    s32 name = evtGetValue(event, args[1]);
    s32 flag = evtGetValue(event, args[2]);

    if (strcmp((const char*)name, "mario") == 0) {
        MarioWork* mario = marioGetPtr();
        if (onoff == 1) {
            if (mario->paperAnimGroupId[0] != -1) {
                animPoseSetMaterialFlagOn(mario->paperAnimGroupId[0], flag);
            }
            if (mario->paperAnimGroupId[1] != -1) {
                animPoseSetMaterialFlagOn(mario->paperAnimGroupId[1], flag);
            }
            if (mario->paperAnimGroupId[2] != -1) {
                animPoseSetMaterialFlagOn(mario->paperAnimGroupId[2], flag);
            }
        }
        else {
            if (mario->paperAnimGroupId[0] != -1) {
                animPoseSetMaterialFlagOff(mario->paperAnimGroupId[0], flag);
            }
            if (mario->paperAnimGroupId[1] != -1) {
                animPoseSetMaterialFlagOff(mario->paperAnimGroupId[1], flag);
            }
            if (mario->paperAnimGroupId[2] != -1) {
                animPoseSetMaterialFlagOff(mario->paperAnimGroupId[2], flag);
            }
        }
    }
    else if (strcmp((const char*)name, "party") == 0) {
        PartyEntry* party = partyGetPtr(marioGetPartyId());
        if (onoff == 1) {
            animPoseSetMaterialFlagOn(party->paperAnimGroup, flag);
        }
        else {
            animPoseSetMaterialFlagOff(party->paperAnimGroup, flag);
        }
    }
    else if (strcmp((const char*)name, str_802C1F1C) == 0) {
        PartyEntry* party = partyGetPtr(marioGetExtraPartyId());
        if (onoff == 1) {
            animPoseSetMaterialFlagOn(party->paperAnimGroup, flag);
        }
        else {
            animPoseSetMaterialFlagOff(party->paperAnimGroup, flag);
        }
    }
    else {
        NpcEntry* npc = evtNpcNameToPtr(event, (const char*)name);
        if (onoff == 1) {
            animPoseSetMaterialFlagOn(npc->poseId, flag);
        }
        else {
            animPoseSetMaterialFlagOff(npc->poseId, flag);
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_blend_set_mobj_flag) {
    s32* args = event->args;
    s32 onoff = evtGetValue(event, args[0]);
    s32 name = evtGetValue(event, args[1]);
    s32 flag = evtGetValue(event, args[2]);
    MapObjectEntry* mobj = mobjNameToPtr((const char*)name);

    if (onoff == 1) {
        animPoseSetMaterialFlagOn(mobj->poseId, flag);
    }
    else {
        animPoseSetMaterialFlagOff(mobj->poseId, flag);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_color) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 name = evtGetValue(event, args[1]);
    u8 r = evtGetValue(event, args[2]);
    u8 g = evtGetValue(event, args[3]);
    u8 b = evtGetValue(event, args[4]);
    u8 a = evtGetValue(event, args[5]);

    if (type == 0) {
        GXColor c;
        GXColor color;

        color = *(GXColor*)&unk_80422fb8;
        color.r = r;
        color.g = g;
        color.b = b;
        color.a = a;
        c = color;
        mapObjSetColor((const char*)name, &c);
    }
    else {
        GXColor c;
        GXColor color;

        color = *(GXColor*)&unk_80422fbc;
        color.r = r;
        color.g = g;
        color.b = b;
        color.a = a;
        c = color;
        mapGrpSetColor((const char*)name, &c);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_playanim) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 a2 = evtGetValue(event, args[1]);
    s32 a3 = evtGetValue(event, args[2]);

    mapPlayAnimationLv((const char*)name, a2, a3);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_checkanim) {
    s32* args = event->args;
    BOOL valid;
    f32 rate;

    mapCheckAnimation((const char*)evtGetValue(event, args[0]), &valid, &rate);
    evtSetValue(event, args[1], valid);
    evtSetValue(event, args[2], (s32)rate);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_pauseanim) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 name = evtGetValue(event, args[1]);

    if (type) {
        mapPauseAnimationAll();
    }
    else {
        mapPauseAnimation((const char*)name);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_replayanim) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 name = evtGetValue(event, args[1]);

    if (type) {
        mapReStartAnimationAll();
    }
    else {
        mapReStartAnimation((const char*)name);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_set_playrate) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 rate = evtGetFloat(event, args[1]);

    mapSetPlayRate((const char*)name, rate);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_flag_onoff) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 onoff = evtGetValue(event, args[1]);
    s32 name = evtGetValue(event, args[2]);
    s32 flag = args[3];

    if (type == 0) {
        if (onoff == 0) {
            mapObjFlagOff((const char*)name, flag);
        }
        else {
            mapObjFlagOn((const char*)name, flag);
        }
    }
    else {
        if (onoff == 0) {
            mapGrpFlagOff((const char*)name, flag);
        }
        else {
            mapGrpFlagOn((const char*)name, flag);
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_set_offscreen) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 name = evtGetValue(event, args[1]);
    s32 offName = evtGetValue(event, args[2]);

    if (type == 0) {
        mapObjSetOffScreen((const char*)name, (const char*)offName);
    }
    else {
        mapGrpSetOffScreen((const char*)name, (const char*)offName);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_clear_offscreen) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 name = evtGetValue(event, args[1]);

    if (type == 0) {
        mapObjClearOffScreen((const char*)name);
    }
    else {
        mapGrpClearOffScreen((const char*)name);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mapobj_get_position) {
    s32* args = event->args;
    Vec pos;
    s32 name = evtGetValue(event, args[0]);

    mapObjGetPos((const char*)name, &pos);
    evtSetValue(event, args[1], FLOAT(pos.x));
    evtSetValue(event, args[2], FLOAT(pos.y));
    evtSetValue(event, args[3], FLOAT(pos.z));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_set_tevcallback) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);
    s32 cb = evtGetValue(event, args[1]);

    mapSetTevCallback(idx, (void*)cb);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_set_flush_onoff) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 onoff = evtGetValue(event, args[1]);
    s32 name = evtGetValue(event, args[2]);

    if (type == 0) {
        if (onoff) {
            mapObjFlushOn((const char*)name);
        }
        else {
            mapObjFlushOff((const char*)name);
        }
    }
    else {
        if (onoff) {
            mapGrpFlushOn((const char*)name);
        }
        else {
            mapGrpFlushOff((const char*)name);
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_set_flush_color) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    s32 name = evtGetValue(event, args[1]);
    s32 r = evtGetValue(event, args[2]);
    s32 g = evtGetValue(event, args[3]);
    s32 b = evtGetValue(event, args[4]);
    s32 a = evtGetValue(event, args[5]);

    if (type == 0) {
        mapObjSetFlushColor((const char*)name, r, g, b, a);
    }
    else {
        mapGrpSetFlushColor((const char*)name, r, g, b, a);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_get_flush_color) {
    s32* args = event->args;
    u8 r, g, b, a;

    mapObjGetFlushColor((const char*)evtGetValue(event, args[0]), &r, &g, &b, &a);
    evtSetValue(event, args[1], r);
    evtSetValue(event, args[2], g);
    evtSetValue(event, args[3], b);
    evtSetValue(event, args[4], a);
    return EVT_RETURN_DONE;
}

static USER_FUNC(check) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    evtSetValue(event, args[0], 0);
    if (mario->hitobjStandOn != NULL
        && strcmp((const char*)event->lwData[1], hitGetName((HitEntry*)mario->hitobjStandOn)) == 0) {
        evtSetValue(event, args[0], 1);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(check2) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    evtSetValue(event, args[0], 0);
    if (mario->hitobjStandOn != NULL
        && strcmp((const char*)event->lwData[1], hitGetName((HitEntry*)mario->hitobjStandOn)) != 0) {
        evtSetValue(event, args[0], 1);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_entry_airport_harbor) {
    s32* args = event->args;
    s32 mode = evtGetValue(event, args[0]);
    s32 name = evtGetValue(event, args[1]);
    s32 type = evtGetValue(event, args[2]);
    s32 data[16];

    mapGetMapObj((const char*)name);
    switch (mode) {
    case 0:
        if (animGroupBaseAsync(str_802C1F08, 0, NULL) == 0) {
            return 0;
        }
        if ((s32)pouchCheckItem(4) != 1) {
            break;
        }
        data[0] = name;
        data[1] = type;
        evtRunCaseEntry(0, 0, type, 0, evt_ride_airport, data);
        data[0] = name;
        data[1] = type;
        evtRunCaseEntry(1, 0, type, 0, evt_nonride_airport, data);
        break;
    case 1:
        if (animGroupBaseAsync(str_802C1F14, 0, NULL) == 0) {
            return 0;
        }
        if ((s32)pouchCheckItem(5) != 1) {
            break;
        }
        data[0] = name;
        data[1] = type;
        evtRunCaseEntry(0, 0, type, 0, evt_ride_airport, data);
        data[0] = name;
        data[1] = type;
        evtRunCaseEntry(1, 0, type, 0, evt_nonride_airport, data);
        break;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_map_replace_mapobj) {
    s32* args = event->args;
    Vec pos;
    s32 name = evtGetValue(event, args[0]);
    s32 type = evtGetValue(event, args[1]);

    mapObjGetPos((const char*)name, &pos);
    mapObjFlagOn((const char*)name, 0x4000);
    switch (type) {
    case 0:
        effTorchEntry(0, pos.x, pos.y - 3.0f, pos.z, 0.6f);
        break;
    case 1: {
        EffectEntry* eff = effFireEntry(5, 0, pos.x, pos.y, pos.z, 0.4f);
        *(s32*)((u8*)eff->userdata + 0x88) = 1;
        break;
    }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(N_evt_mapdisp_onoff) {
    s32* args = event->args;

    if (evtGetValue(event, args[0])) {
        N_mapDispOn();
    }
    else {
        N_mapDispOff();
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_ride_airport)
CALL(evt_mapobj_flag_onoff, 1, 0, LW(0), 0x100)
LABEL(0)
CALL(evt_map_get_flush_color, LW(0), LW(10), LW(11), LW(12), LW(13))
ADD(LW(13), 2)
IF_INT_GREATER(LW(13), 100)
SET_VALUE(LW(13), 100)
END_IF
CALL(evt_map_set_flush_color, 1, LW(0), 0xF0, 0xF0, 0x64, LW(13))
CALL(evt_map_set_flush_onoff, 1, 1, LW(0))
CALL(check, LW(10))
IF_INT_EQUAL(LW(10), 1)
WAIT_FRAMES(1)
GOTO(0)
END_IF
RETURN
EVT_END()

EVT_BEGIN(evt_nonride_airport)
LABEL(0)
CALL(evt_map_get_flush_color, LW(0), LW(10), LW(11), LW(12), LW(13))
SUBTRACT(LW(13), 2)
IF_INT_LESS(LW(13), 0)
SET_VALUE(LW(13), 0)
END_IF
CALL(evt_map_set_flush_color, 1, LW(0), 0xF0, 0xF0, 0x64, LW(13))
CALL(check2, LW(10))
IF_INT_EQUAL(LW(10), 1)
WAIT_FRAMES(1)
GOTO(0)
END_IF
RETURN
EVT_END()
