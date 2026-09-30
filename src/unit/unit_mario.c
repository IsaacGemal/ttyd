#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "battle/battle_sub.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "driver/animdrv.h"
#include "dolphin/gx/GXTev.h"
#include "driver/mapdrv.h"
#include "unit/unit_mario.h"
static s32 _mario_makkuro_set(EventEntry* event, BOOL isFirstCall);  // local to this file in the original
static s32 _get_mario_hammer_lv(EventEntry* event, BOOL isFirstCall);  // local to this file in the original

//local prototypes inherited from headers that do not declare these yet

//strings and constants are defined with the rest of the file's .rodata/.sdata2
extern const char str_802F3780[];
extern const char str_802F3A60[];
extern const char str_802F3A78[];
extern GXColor dat_8041cc98;

// The original compiler emitted all of this file's string literals into one pool and referenced
// the b_mario_* anim names relative to the pool base (str_802F3780). We are not defining the rest
// of the pool yet, so reproduce the pool-relative addressing; the natural form is the named string.
#ifdef MUST_MATCH
#define ANIM_L (base + 0x2BC)
#define ANIM_W (base + 0x2C8)
#define ANIM_WL (base + 0x2D4)
#else
#define ANIM_L "b_mario_l"
#define ANIM_W "b_mario_w"
#define ANIM_WL "b_mario_wl"
#endif

//param block passed to the TEV callback registered by _mario_makkuro_set
typedef struct TevCallbackParam {
    u8 pad[0xC];
    s32 field_C;  // 0xC
    s32 field_10; // 0x10
    s32 field_14; // 0x14
    s32 field_18; // 0x18
} TevCallbackParam;

static void callback(TevCallbackParam* param);


USER_FUNC(_get_mario_hammer_lv) {
    s32 arg = event->args[0];

    evtSetValue(event, arg, pouchGetPtr()->hammerLevel);
    return EVT_RETURN_DONE;
}

// The named `args` temp plus opt_propagation off are both required: with propagation on (or with
// event->args[0] used directly) MWCC puts the string-pool base temp in r6 and event->args in r5;
// the original has the pool base in r5 and event->args in r6.
#pragma opt_propagation off
s32 _mario_super_emblem_anim_set(EventEntry* event, s32 mode) {
    const char* base = str_802F3780;
    s32* args = event->args;
    const char* anim;
    BattleWork* work;
    s32 emblem;
    s32 id;

    work = _battleWorkPointer;
    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);

    BattleGetUnitPtr(work, id);

    emblem = 0;
    if (pouchEquipCheckBadge(ITEM_L_EMBLEM)) {
        emblem = 1;
    }
    if (pouchEquipCheckBadge(ITEM_W_EMBLEM)) {
        emblem += 2;
    }

    switch (emblem) {
    case 1:
        anim = ANIM_L;
        break;
    case 2:
        anim = ANIM_W;
        break;
    case 3:
        anim = ANIM_WL;
        break;
    default:
        anim = NULL;
        break;
    }

    if (mode != 0) {
        BattleUnitPart* part;
        if (anim == NULL) {
            return EVT_RETURN_DONE;
        }
        part = BattleGetUnitPartsPtr(id, 1);
        animPoseGetAnimPosePtr(*(s32*)part->field_0x1C0);
        if (*(s32*)part->field_0x1C0 != -1) {
            animPoseRelease(*(s32*)part->field_0x1C0);
            *(s32*)part->field_0x1C0 = -1;
        }
        part = BattleGetUnitPartsPtr(id, 2);
        animPoseGetAnimPosePtr(*(s32*)part->field_0x1C0);
        if (*(s32*)part->field_0x1C0 != -1) {
            animPoseRelease(*(s32*)part->field_0x1C0);
            *(s32*)part->field_0x1C0 = -1;
        }
        part = BattleGetUnitPartsPtr(id, 3);
        animPoseGetAnimPosePtr(*(s32*)part->field_0x1C0);
        if (*(s32*)part->field_0x1C0 != -1) {
            animPoseRelease(*(s32*)part->field_0x1C0);
            *(s32*)part->field_0x1C0 = -1;
        }
    }

    if (animEffectAsync(anim, 1)) {
        BattleUnitPart* part;
        part = BattleGetUnitPartsPtr(id, 1);
        *(s32*)part->field_0x1C0 = animPoseEntry(anim, 1);
        part = BattleGetUnitPartsPtr(id, 2);
        *(s32*)part->field_0x1C0 = animPoseEntry(anim, 1);
        part = BattleGetUnitPartsPtr(id, 3);
        *(s32*)part->field_0x1C0 = animPoseEntry(anim, 1);
        return EVT_RETURN_DONE;
    }

    return 0;
}
#pragma opt_propagation reset

static s32 _mario_makkuro_set(EventEntry* event, BOOL isFirstCall) {
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);

    if (mario != NULL) {
        mapSetTevCallback(0xB, callback);
        animPoseSetMaterialFlagOn(*(s32*)BtlUnit_GetPartsPtr(mario, 1)->field_0x1C0, 0xB000000);
        animPoseSetMaterialFlagOn(*(s32*)BtlUnit_GetPartsPtr(mario, 2)->field_0x1C0, 0xB000000);
        animPoseSetMaterialFlagOn(*(s32*)BtlUnit_GetPartsPtr(mario, 3)->field_0x1C0, 0xB000000);
    }
    return EVT_RETURN_DONE;
}

void faker_mario_makkuro_set(void) {
    mapSetTevCallback(0xB, callback);
}

static void callback(TevCallbackParam* param) {
    GXColor color = dat_8041cc98;
    s32 stage = param->field_C;
    s32 field_10 = param->field_10;
    s32 field_14 = param->field_14;
    s32 field_18 = param->field_18;

    GXSetTevColor(GX_TEVREG0, color);
    GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);

    param->field_C = stage + 1;
    param->field_14 = field_14;
    param->field_18 = field_18;
    param->field_10 = field_10;
}

s32 mario_pinch_pose_sound_callback(BattleUnit* unit) {
    s32 result;
    if (unit->work[8] & 1) {
        result = BtlUnit_snd_se(unit, str_802F3A60, 0xF1194D80, 0);
    }
    else {
        result = BtlUnit_snd_se(unit, str_802F3A78, 0xF1194D80, 0);
    }
    unit->work[8] = unit->work[8] + 1;
    return result;
}

s32 _get_local_frame(EventEntry* event, BOOL isFirstCall) {
    s32 arg = event->args[0];
    AnimationPose* pose =
        animPoseGetAnimPosePtr(*(s32*)BattleGetUnitPartsPtr(BattleTransID(event, -2), 1)->field_0x1C0);

    evtSetValue(event, arg, (s32)*(f32*)(pose->field_0x20 + 4));
    return EVT_RETURN_DONE;
}
