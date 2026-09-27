#include "unit/party/unit_party_vivian.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/msgdrv.h"
#include "pmario_sound.h"
#include "system.h"
#include "math_ext.h"
#include "mario/mario_party.h"
#include <dolphin/mtx.h>
#include <string.h>

// sdata2 float constants live in this file's .sdata2 (see the original's data).
extern const f32 float_0_8041de8c;
extern const f32 float_1p2_8041de90;
extern const f32 float_1_8041de94;
extern const f32 float_10_8041de98;
extern const f32 float_16_8041de9c;
extern const f32 float_5_8041dea0;
extern const f32 float_neg30_8041dea4;
extern const f32 float_20_8041dea8;
extern const f32 float_2_8041deac;
extern const f32 float_8_8041deb0;
extern const f32 float_500_8041deb4;
extern const f32 float_200_8041deb8;
extern const f32 float_0p66667_8041debc;

extern u8 lbl_80388868[]; // this unit's .data weapon tables
const f64 double_to_int_802f8010 = 4503601774854144.0;

// strings used by the majo battle events
// (defined in this file's .rodata in the original)
extern const char str_802F7C30[]; // this file's rodata base, original strings are pool-relative
extern const char str_802F8018[]; // "p_bibi"
extern const char str_802F8020[]; // "PM_B_1"
extern const char str_802F8028[]; // "SFX_BTL_VIVIAN_SHADOW1_2"
extern const char str_802F8044[]; // "SFX_BTL_VIVIAN_SHADOW1_1"
extern const char str_802F8060[]; // "PM_B_2"

// The original compiler emitted this file's string literals into one pool and referenced them
// relative to the pool base. We are not defining the rest of the pool yet. The natural form is
// the named string.
#ifdef MUST_MATCH
#define STR_P_BIBI (base + 0x3E8)
#define STR_PM_B_1 (base + 0x3F0)
#define STR_SHADOW1_2 (base + 0x3F8)
#define STR_SHADOW1_1 (base + 0x414)
#define STR_PM_B_2 (base + 0x430)
#else
#define STR_P_BIBI str_802F8018
#define STR_PM_B_1 str_802F8020
#define STR_SHADOW1_2 str_802F8028
#define STR_SHADOW1_1 str_802F8044
#define STR_PM_B_2 str_802F8060
#endif

USER_FUNC(_disp_heart_entry_stop_check) {
    evtSetValue(event, event->args[0], *(s32*)((u8*)event + 0x8C));
    return EVT_RETURN_DONE;
}

USER_FUNC(_disp_heart_entry_stop) {
    EventEntry* entry = evtGetPtrID(evtGetValue(event, event->args[0]));
    *(s32*)((u8*)entry + 0x8C) = 1;
    return EVT_RETURN_DONE;
}

// NONMATCHING (85.86%): identical instruction sequence; only the stack slot chosen for the temporary
// second icon position differs (target reuses 0x20, ours allocates an extra Vec at 0x38).
void _disp_heart(CameraId cameraId, void* param) {
    EventEntry* event;
    f32 scale;
    s32 a;
    s32 b;
    BattleUnit* unit;
    BattleUnitPart* part;
    Vec iconPos;
    Vec pos;

    if (param == NULL) {
        return;
    }
    event = param;
    if ((BattleUnit*)event->userdata[0] == NULL) {
        return;
    }
    a = event->userdata[1];
    b = event->userdata[2];
    if (a <= b) {
        scale = intplGetValue(5, a, b, float_0_8041de8c, float_1p2_8041de90);
    } else if (a <= (s32)event->field_84) {
        scale = intplGetValue(0xB, a - b, (s32)event->field_84 - b, float_1p2_8041de90, float_1_8041de94);
    } else {
        scale = float_1_8041de94;
    }

    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, event->field_0x90[0]));
    part = BtlUnit_GetPartsPtr(unit, event->field_0x90[1]);
    BtlUnit_GetHitPos(unit, part, &pos.x, &pos.y, &pos.z);
    pos.z += float_10_8041de98;
    iconPos = pos;
    iconPos.y += float_16_8041de9c * scale;
    iconPos.z += float_5_8041dea0;
    iconDispGx(pos, 0, 0x1F2, scale);
    iconDispGx(iconPos, 0, 0x189, scale);
}

USER_FUNC(_disp_heart_entry) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 id;
    s32 frame;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    if (isFirstCall) {
        event->userdata[0] = (s32)unit;
        event->userdata[1] = 0;
        event->userdata[2] = evtGetValue(event, args[0]);
        event->field_84 = evtGetValue(event, args[1]);
        *(s32*)&event->wInterpRelatedTime = evtGetValue(event, args[2]);
        *(s32*)((u8*)event + 0x8C) = 0;
        event->field_0x90[0] = evtGetValue(event, args[3]);
        event->field_0x90[1] = evtGetValue(event, args[4]);
    }
    if (*(s32*)((u8*)event + 0x8C) != 0) {
        return EVT_RETURN_DONE;
    }
    event->userdata[1]++;
    dispEntry(CAMERA_3D, float_0_8041de8c, 1, _disp_heart, event);
    frame = *(s32*)&event->wInterpRelatedTime;
    if (event->userdata[1] < frame) {
        return 0;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_make_kagenuke_weapon) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 arg0;
    void* area;
    s32 value;
    s32 value2;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    area = *(void**)((u8*)unit + 0x310);
    arg0 = args[0];
    value = evtGetValue(event, arg0);
    value2 = evtGetValue(event, args[1]);
    memcpy(area, (void*)value, 0xC0);
    *(u8*)((u8*)area + 0x90) = value2;
    evtSetValue(event, arg0, (s32)area);
    return EVT_RETURN_DONE;
}

// NONMATCHING (76.09%): first draft.
USER_FUNC(unk_8017d03c) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit1;
    BattleUnit* unit2;
    f32 sx;
    f32 sy;
    f32 sz;
    f32 scale1;
    f32 k54;
    f32 k58;
    f32 d1;
    f32 d2;
    s32 t;
    s32 u;
    s32 v0;
    s32 v1;
    s32 v2;
    s32 v3;
    s32 outX;
    s32 outY;
    s32 outZ;
    f32 floor;

    v0 = evtGetValue(event, args[0]);
    v1 = evtGetValue(event, args[1]);
    v2 = evtGetValue(event, args[2]);
    v3 = evtGetValue(event, args[3]);

    unit1 = BattleGetUnitPtr(work, BattleTransID(event, -2));
    unit2 = BattleGetUnitPtr(work, BattleTransID(event, v0));
    BtlUnit_GetScale(unit2, &sx, &sy, &sz);

    scale1 = unit1->mSizeMultiplier;
    k54 = *(f32*)((u8*)unit2->mKindParams + 0x54);
    k58 = *(f32*)((u8*)unit2->mKindParams + 0x58);
    d1 = (f32)unit1->faceDirection;
    d2 = (f32)unit2->faceDirection;
    t = (s32)(d2 * k54 * sx);
    u = (s32)(k58 * sz);
    outX = (s32)((f32)v1 + ((float_neg30_8041dea4 * scale1) * d1 + (f32)t));
    outY = (s32)((f32)v2 - float_20_8041dea8 * scale1);
    outZ = v3 + u;
    floor = BattleGetFloorHeight(work, (f32)outX, (f32)outY, (f32)outZ);
    if (floor > (f32)outY) {
        outY = (s32)floor;
    }
    evtSetValue(event, args[1], outX);
    evtSetValue(event, args[2], outY);
    evtSetValue(event, args[3], outZ);
    return EVT_RETURN_DONE;
}

s32 _get_move_frame(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    Vec a;
    Vec b;
    Vec d;
    f32 speed;
    f32 len;
    s32 frame;
    s32 frameIndex;

    a.x = evtGetFloat(event, args[0]);
    a.y = evtGetFloat(event, args[1]);
    a.z = evtGetFloat(event, args[2]);
    b.x = evtGetFloat(event, args[3]);
    b.y = evtGetFloat(event, args[4]);
    b.z = evtGetFloat(event, args[5]);
    speed = evtGetFloat(event, args[6]);
    frameIndex = args[7];
    PSVECSubtract(&a, &b, &d);
    len = sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
    frame = (s32)(len / speed);
    if (frame > 0xF) {
        frame = 0xF;
    }
    evtSetValue(event, frameIndex, frame);
    return EVT_RETURN_DONE;
}

#define TECH_MENU_ENTRY(table)                                                             \
    cmd->mWeaponTable[*count].mIndex = -1;                                                 \
    cmd->mWeaponTable[*count].mItem = 0;                                                   \
    cmd->mWeaponTable[*count].mWeaponParams = (BattleWeapon*)(table);                      \
    cmd->mWeaponTable[*count].field_0x4 = 0;                                               \
    cmd->mWeaponTable[*count].mIcon =                                                      \
        *(u16*)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 4);                         \
    cmd->mWeaponTable[*count].mName =                                                      \
        (char*)msgSearch(*(char**)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 0));     \
    (*count)++;

static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count) {
    u8* const weaponBase = lbl_80388868;
    s32 techLv = partyGetTechLv(
        BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

    TECH_MENU_ENTRY(weaponBase + 0x830);
    if (techLv >= 0) {
        TECH_MENU_ENTRY(weaponBase + 0x8F0);
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(weaponBase + 0x9B0);
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(weaponBase + 0xA70);
    }
}

USER_FUNC(_vivian_make_extra_work_area) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    *(void**)((u8*)unit + 0x310) = BattleAlloc(0xC0);
    return EVT_RETURN_DONE;
}

// NONMATCHING (91.42%): 5 instructions differ, 5 missing, 5 extra, plus register differences.
USER_FUNC(battle_evt_majo_disp_off) {
#ifdef MUST_MATCH
    const char* const base = str_802F7C30;
#endif
    s32* args = event->args;
    BattleUnit* unit;
    BattleUnitPart* parts;
    s32 pose;
    s32 id;
    s32 v1;
    s32 v2;
    s32 v3;

    id = BattleTransID(event, evtGetValue(event, args[0]));
    v1 = evtGetValue(event, args[1]);
    v2 = evtGetValue(event, args[2]);
    v3 = evtGetValue(event, args[3]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    parts = BattleGetUnitPartsPtr(id, v1);
    pose = *(s32*)parts->field_0x1C0;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = 0;
        animEffectAsync(STR_P_BIBI, 1);
    }

    switch (event->userdata[0]) {
    case 0:
        parts->field_0x78 = intplGetValue(0, event->userdata[1], 30, float_1_8041de94, float_2_8041deac);
        if (v2 != 0) {
            unit->flags |= 0x10000;
        }
        event->userdata[1]++;
        if (event->userdata[1] > 15) {
            event->userdata[0]++;
        }
        break;
    case 1:
        if (event->userdata[1] <= 30) {
            parts->field_0x78 = intplGetValue(0, event->userdata[1], 30, float_1_8041de94, float_2_8041deac);
            event->userdata[1]++;
        }
        if (!animEffectAsync(STR_P_BIBI, 1)) {
            return 0;
        }
        animPoseSetEffect(pose, STR_P_BIBI, 1);
        animPoseSetEffectAnim(pose, STR_PM_B_1, 1);
        animPoseSetLocalTime(animPoseGetAnimPosePtr(pose)->paperId, float_8_8041deb0);
        event->userdata[0]++;
        break;
    case 2:
        if (event->userdata[1] <= 30) {
            parts->field_0x78 = intplGetValue(0, event->userdata[1], 30, float_1_8041de94, float_2_8041deac);
            event->userdata[1]++;
        }
        if (animPoseGetLoopTimes(animPoseGetAnimPosePtr(pose)->paperId) >= float_1_8041de94) {
            psndSFXOn_3D(STR_SHADOW1_2, &unit->mPosition);
            event->userdata[0]++;
            event->userdata[1] = 0;
            if (v3 != 0) {
                parts->attributes |= 0x01000000;
            }
        }
        break;
    case 3:
        parts->field_0x78 = intplGetValue(0, event->userdata[1], 20, float_2_8041deac, float_0_8041de8c);
        event->userdata[1]++;
        if (event->userdata[1] > 20) {
            parts->field_0x78 = float_0_8041de8c;
            unit->mPosition.y -= float_500_8041deb4;
            animPoseSetEffect(pose, NULL, 1);
            animPoseSetPaperAnimGroup(pose, NULL, 1);
            return EVT_RETURN_DONE;
        }
        break;
    }
    return 0;
}

// NONMATCHING (84.85%): 3 instructions differ, 26 missing, 2 extra, plus register differences.
USER_FUNC(battle_evt_majo_disp_on) {
#ifdef MUST_MATCH
    const char* const base = str_802F7C30;
#endif
    s32* args = event->args;
    BattleUnit* unit;
    BattleUnitPart* parts;
    s32 pose;
    s32 id;
    s32 v1;
    s32 v5;
    f32 f2;
    f32 f3;
    f32 f4;

    id = BattleTransID(event, evtGetValue(event, args[0]));
    v1 = evtGetValue(event, args[1]);
    f2 = evtGetFloat(event, args[2]);
    f3 = evtGetFloat(event, args[3]);
    f4 = evtGetFloat(event, args[4]);
    v5 = evtGetValue(event, args[5]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    parts = BattleGetUnitPartsPtr(id, v1);
    pose = *(s32*)parts->field_0x1C0;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = 0;
        unit->mPosition.x = f2;
        unit->mPosition.y = f3 - float_200_8041deb8;
        unit->mPosition.z = f4;
        animEffectAsync(STR_P_BIBI, 1);
    }

    switch (event->userdata[0]) {
    case 0:
        parts->field_0x78 = intplGetValue(0, event->userdata[1], 30, float_0_8041de8c, float_2_8041deac);
        if (event->userdata[1] == 0) {
            psndSFXOn_3D(STR_SHADOW1_1, &unit->mPosition);
        }
        event->userdata[1]++;
        if (event->userdata[1] > 15) {
            event->userdata[0]++;
        }
        break;
    case 1:
        if (event->userdata[1] <= 30) {
            parts->field_0x78 = intplGetValue(0, event->userdata[1], 30, float_0_8041de8c, float_2_8041deac);
            event->userdata[1]++;
        }
        if (!animEffectAsync(STR_P_BIBI, 1)) {
            return 0;
        }
        unit->mPosition.x = f2;
        unit->mPosition.y = f3;
        unit->mPosition.z = f4;
        animPoseSetEffect(pose, STR_P_BIBI, 1);
        animPoseSetEffectAnim(pose, STR_PM_B_2, 1);
        event->userdata[0]++;
        break;
    case 2:
        if (event->userdata[1] <= 30) {
            parts->field_0x78 = intplGetValue(0, event->userdata[1], 30, float_0_8041de8c, float_2_8041deac);
            event->userdata[1]++;
        }
        if (animPoseGetLoopTimes(animPoseGetAnimPosePtr(pose)->paperId) >= float_0p66667_8041debc) {
            animPoseSetPaperAnimGroup(pose, NULL, 1);
            event->userdata[0]++;
            event->userdata[1] = 0;
        }
        break;
    case 3:
        parts->field_0x78 = intplGetValue(0, event->userdata[1], 15, float_2_8041deac, float_1_8041de94);
        event->userdata[1]++;
        if (event->userdata[1] > 15) {
            parts->field_0x78 = float_1_8041de94;
            animPoseSetEffect(pose, NULL, 1);
            if (v5 != 0) {
                unit->flags &= ~0x10000;
            }
            return EVT_RETURN_DONE;
        }
        break;
    }
    return 0;
}
