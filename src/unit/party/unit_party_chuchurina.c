#include <dolphin/types.h>
#include "unit/party/unit_party_chuchurina.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"
#include "battle/battle_disp.h"
#include "driver/dispdrv.h"
#include "driver/msgdrv.h"
#include "mario/mario_party.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <string.h>
#include "battle/battle_attack_audience.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_icon.h"
#include "battle/battle_message.h"
#include "battle/battle_seq_command.h"
#include "eff/eff_funemizu.h"
#include "eff/n64/eff_kameki_tornade_n64.h"
#include "evt/evt_audience.h"
#include "evt/evt_eff.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "party/party_motion.h"
#include "battle/battle_weapon_power.h"
#include "battle/battle_database_common.h"

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_8037EC10[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };

USER_FUNC(_chuchu_make_extra_work_area) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    unit->mExtraWork = BattleAlloc(0xC0);
    return EVT_RETURN_DONE;
}

#define TECH_MENU_ENTRY(table)                                                        \
    cmd->weaponTable[*count].mIndex = -1;                                            \
    cmd->weaponTable[*count].mItem = 0;                                              \
    cmd->weaponTable[*count].mWeaponParams = (table);                                \
    cmd->weaponTable[*count].mEnabled = 0;                                           \
    cmd->weaponTable[*count].mIcon =                                                 \
        cmd->weaponTable[*count].mWeaponParams->icon;                                \
    cmd->weaponTable[*count].mName =                                                 \
        msgSearch(cmd->weaponTable[*count].mWeaponParams->name);                      \
    (*count)++;

static void __makeTechMenuFunc(BattleCommand* cmd, s32* count) {
    s32 techLv = partyGetTechLv(
        BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

    TECH_MENU_ENTRY(&partyWeapon_ChuchurinaNormalAttackLeftLast);
    if (techLv >= 0) {
        TECH_MENU_ENTRY(&partyWeapon_ChuchurinaItemSteal);
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(&partyWeapon_ChuchurinaMadowaseAttack);
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(&partyWeapon_ChuchurinaKiss);
    }
}

// The declaration order below is what gives the original's callee-saved allocation
// (event r29, args r27, work r28, v0 r26, v1 r25, outX/outY/outZ r31/r30/r26);
// this function's asm is identical to _get_bomb_hit_position in unit_party_sanders.c.
USER_FUNC(_get_binta_hit_position) {
    BattleUnit* unit;
    BattleWork* work = _battleWorkPointer;
    Vec pos;
    s32* args = event->args;
    s32 v0;
    s32 v1;
    s32 dir;
    s32 outZ;
    s32 outX;
    s32 outY;

    v0 = evtGetValue(event, args[0]);
    v1 = evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    outX = args[3];
    outY = args[4];
    outZ = args[5];
    BattleGetUnitPtr(work, BattleTransID(event, v0));
    unit = BattleGetUnitPtr(work, BattleTransID(event, v1));
    dir = unit->faceDirection;
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    pos.x += (f32)dir * (unit->bintaHitOffset.x * unit->mSizeMultiplier);
    pos.y += unit->bintaHitOffset.y * unit->mSizeMultiplier;
    pos.z += unit->bintaHitOffset.z * unit->mSizeMultiplier;
    evtSetFloat(event, outX, pos.x);
    evtSetFloat(event, outY, pos.y);
    evtSetFloat(event, outZ, pos.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(_make_madowase_weapon) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 id, src, val;
    s32 dst;
    BattleWeapon* area;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    dst = args[0];
    area = unit->mExtraWork;
    src = evtGetValue(event, dst);
    val = evtGetValue(event, args[1]);
    memcpy(area, (void*)src, 0xC0);
    area->dizzyChance = val;
    evtSetValue(event, dst, (s32)area);
    return EVT_RETURN_DONE;
}

USER_FUNC(_get_itemsteal_param) {
    BattleWork* work;
    s32* args = event->args;
    BattleUnit* unit;
    s32 id, a, b, chance, steal;

    id = evtGetValue(event, args[0]);
    a = args[1];
    work = _battleWorkPointer;
    b = args[2];
    unit = BattleGetUnitPtr(work, BattleTransID(event, id));
    chance = 0x50 - (work->actionCommands.mAcDifficulty - 3) * 0x14;
    if (chance <= 5) {
        chance = 5;
    }
    steal = 100 - unit->mKindParams->itemStealParam;
    evtSetValue(event, a, chance);
    evtSetValue(event, b, steal);
    return EVT_RETURN_DONE;
}

#ifdef MUST_MATCH
// Match-only: retail keeps the stolen item in the same register as the event arg id
// (r27). Carrying both in one reused s32 gives MWCC the target colouring. The helper
// returns the value (instead of writing through an out pointer) and its pick loop is
// written condition-first: a plain `while` rotates the body ahead of the test, while the
// `for (;;)` form emits the original's `subf.; bge body; lwz list->type; b done` layout.
// The natural out-pointer/u16 form is under #else.
static inline ItemType _chuchu_pick_drop(BattleUnit* unit) {
    ItemDropData* list;
    ItemDropData* d;
    s32 sum;
    s32 pick;
    if (unit->dropTable != NULL) {
        irand(100);
    }
    list = unit->dropTable;
    if (list == NULL) {
        return 0;
    } else if (list != NULL) {
        d = list;
        sum = 0;
        while (d->holdWeight > 0 || d->type != 0) {
            sum += d->holdWeight;
            d++;
        }
        if (sum > 0) {
            pick = irand(sum);
            for (;;) {
                pick -= list->holdWeight;
                if (pick < 0) {
                    return list->type;
                }
                list++;
            }
        }
    }
    return 0;
}
#else
static inline void _chuchu_pick_drop(BattleUnit* unit, u16* out) {
    ItemDropData* list;
    ItemDropData* d;
    s32 sum;
    s32 pick;
    if (unit->dropTable != NULL) {
        irand(100);
    }
    list = unit->dropTable;
    if (list == NULL) {
        *out = 0;
    } else if (list != NULL) {
        d = list;
        sum = 0;
        while (d->holdWeight > 0 || d->type != 0) {
            sum += d->holdWeight;
            d++;
        }
        if (sum > 0) {
            pick = irand(sum);
            while ((pick -= list->holdWeight) >= 0) {
                list++;
            }
            *out = list->type;
        } else {
            *out = 0;
        }
    }
}
#endif

#ifdef MUST_MATCH
// Match-only: routing these three loads/calls through inline getters is what fixes the
// callee-saved colouring. MWCC's allocator follows the IR symbol order, and the direct
// forms colour work r27 / args r28 / item r30; the getters give the original's map
// (event r26, args/id/item/list r27, unit r28, out r29, work r30, flag r31).
static inline BattleWork* _chuchu_get_work(void) { return _battleWorkPointer; }
static inline s32 _chuchu_get_out(s32* args) { return args[1]; }
static inline BattleUnit* _chuchu_get_unit(BattleWork* work, s32 id) { return BattleGetUnitPtr(work, id); }
#endif
// The target holds work->fbatInfo->setup (0xC(r3)) in r5 across the two stores of the
// first branch; naming it in a local (below) reproduces that (no CSE without it).
// The `else if (list != NULL)` in the helper is what emits the target's redundant `beq`
// over the count loop, and the inner `else { return 0; }` must be omitted so the sum<=0
// path falls through to a single shared `return 0` (with it, MWCC emits a second block).
// The stolen item is masked once into a `u16 item` local used for pouchGetItem and the
// final evtSetValue, while the range test keeps reading `(u16)id`; that reproduces the
// original's cached `clrlwi r31, r27, 16`.
#ifdef MUST_MATCH
USER_FUNC(_chuchu_item_steal) {
    s32 out;
    s32 id;
    s32 flag;
    s32* args;
    BattleUnit* unit;
    BattleWork* work;
    u16 item;

    work = _chuchu_get_work();
    args = event->args;
    id = evtGetValue(event, args[0]);
    out = _chuchu_get_out(args);
    flag = evtGetValue(event, args[2]);
    unit = _chuchu_get_unit(_battleWorkPointer, BattleTransID(event, id));
    id = (u16)unit->heldItem;
    if ((u16)id == 0) {
        id = (u16)_chuchu_pick_drop(unit);
        if ((u16)id == 0) {
            id = 0x79;
        }
    }
#else
USER_FUNC(_chuchu_item_steal) {
    u16 item;
    BattleWork* work;
    s32 id, out, flag;
    s32* args;
    BattleUnit* unit;

    work = _battleWorkPointer;
    args = event->args;
    id = evtGetValue(event, args[0]);
    out = args[1];
    flag = evtGetValue(event, args[2]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, id));
    item = unit->heldItem;
    if (item == 0) {
        _chuchu_pick_drop(unit, &item);
        if (item == 0) {
            item = 0x79;
        }
    }
#endif
    if (!(flag & 2)) {
        evtSetValue(event, out, 0);
        return EVT_RETURN_DONE;
    }
#ifdef MUST_MATCH
    item = (u16)id;
    if (pouchGetItem(item) == 0) {
#else
    if (pouchGetItem(item) == 0) {
#endif
        evtSetValue(event, out, 0);
        return EVT_RETURN_DONE;
    }
    unit->heldItem = 0;
    if (BtlUnit_CheckUnitFlag(unit, 0x40000000)) {
        BtlUnit_OffUnitFlag(unit, 0x40000000);
        if (unit->groupSetupId >= 0) {
            NpcBattleInfo* setup = work->fbatInfo->setup;
            setup->heldItems[unit->groupSetupId] = 0;
            setup->stolenItems[unit->groupSetupId] = 0;
        }
    } else if (unit->groupSetupId >= 0) {
        NpcBattleInfo* setup = work->fbatInfo->setup;
        setup->heldItems[unit->groupSetupId] = 0;
    }
#ifdef MUST_MATCH
    if ((u16)id >= 0xF0 && (u16)id < 0x153) {
#else
    if (item >= 0xF0 && item < 0x153) {
#endif
        if (unit->currentKind == 0xDE) {
            BtlUnit_EquipItem(unit, 3, 0);
        } else if (unit->currentKind >= 0xE0 && unit->currentKind < 0xE7) {
            BtlUnit_EquipItem(unit, 5, 0);
        } else {
            BtlUnit_EquipItem(unit, 1, 0);
        }
    }
    evtSetValue(event, out, item);
    return EVT_RETURN_DONE;
}

// .sbss:0x0 | 0x80418A28
s32 mono_alpha;

void mono_capture(CameraId cameraId, void* param) {
    BattleUnit* unit = param;
    BattleUnitPart* parts = BtlUnit_GetPartsPtr(unit, 1);

    unit->mSmartEntry = smartAlloc(GXGetTexBufferSize(0x260, 0x1E0, 1, 0, 0), 1);
    GXSetTexCopySrc(0, 0, 0x260, 0x1E0);
    GXSetTexCopyDst(0x260, 0x1E0, 1, 0);
    GXCopyTex(unit->mSmartEntry->address, 0);
    GXPixModeSync();
    parts->partFlags &= ~0x100;
}

// Building the colour in an inline keeps the GXColor temporary in the slot the original uses.
static inline GXColor monoColor(u8 alpha) {
    GXColor color = *(GXColor*)&dat_8041ddb8;
    color.a = alpha;
    return color;
}

// opt_common_subs on folds the repeated quad vertex constants into registers; the original reloads each.
#pragma opt_common_subs off
void mono_disp(CameraId cameraId, void* param) {
    BattleUnit* unit = param;
    CameraEntry* camera;
    GXTexObj texObj;
    smartEntry* entry;
    void* tex;
    BattleUnit* party;
    BattleUnitPart* parts;
    CameraId prev;

    camera = camGetPtr(cameraId);
    entry = unit->mSmartEntry;
    tex = NULL;
    if (entry != NULL) {
        tex = entry->address;
    }
    if (tex == NULL) {
        return;
    }
    GXInitTexObj(&texObj, tex, 0x260, 0x1E0, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXLoadPosMtxImm(camera->view, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetTevColor(GX_TEVREG0, monoColor(mono_alpha));
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-304.0f, 240.0f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(304.0f, 240.0f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(304.0f, -240.0f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(-304.0f, -240.0f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    party = BattleGetPartyPtr(_battleWorkPointer);
    if (party != NULL) {
        parts = BtlUnit_GetPartsPtr(party, 1);
        prev = camGetCurNo();
        camSetCurNo(CAMERA_3D);
        btlUnitPartsDisp(CAMERA_3D, parts);
        camSetCurNo(prev);
    }
}
#pragma opt_common_subs reset

USER_FUNC(mono_main) {
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    dispEntry(CAMERA_2D, 910.0f, 0, mono_disp, unit);
    return EVT_RETURN_BLOCK;
}

USER_FUNC(mono_capture_event) {
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_GetPartsPtr(unit, 1);
    dispEntry(CAMERA_2D, 910.0f, 0, mono_capture, unit);
    return EVT_RETURN_DONE;
}

USER_FUNC(mono_on) {
    if (isFirstCall) {
        mono_alpha = 0;
    }
    mono_alpha += 10;
    if (mono_alpha > 0xFF) {
        mono_alpha = 0xFF;
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(mono_off) {
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    mono_alpha -= 10;
    if (mono_alpha < 0) {
        mono_alpha = 0;
        BtlUnit_GetPartsPtr(unit, 1);
        if (unit->mSmartEntry != NULL) {
            smartFree(unit->mSmartEntry);
        }
        unit->mSmartEntry = NULL;
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

#pragma force_active on
static s32 init_event[48];
static s32 damage_event[16];
s32 partyChuchurinaAttack_NormalAttack[1590];
s32 partyChuchurinaAttack_MadowaseAttack[2020];
s32 partyChuchurinaAttack_ItemSteal[788];
s32 partyChuchurinaAttack_Kiss[568];
s32 chuchurina_appeal_event[86];
static s32 party_win_reaction[18];
static s32 attack_audience_event[204];
static u32 defence[2];
static u32 defence_attr[2];
static u32 regist[6];
static s32 attack_event[2];
static s32 wait_event[6];
static s32 main_evt[4];

// clang-format off
// TODO: type
static __declspec(section ".data") u32 negone_one_374[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_8037EC20[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_373[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_8037EC30[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_360[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_8037EC40[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_359[2] = {
    0x3F800000, 0xBF800000,
};

BattleDispPose pose_table_chuchurina_stay[24] = {
    { 0x09, "PCH_D_4" },
    { 0x02, "PCH_D_4" },
    { 0x03, "PCH_X_1" },
    { 0x04, "PCH_D_2" },
    { 0x05, "PCH_X_1" },
    { 0x01, "PCH_N_1" },
    { 0x1B, "PCH_D_3" },
    { 0x1C, "PCH_S_1" },
    { 0x25, "PCH_G_1" },
    { 0x1F, "PCH_G_1" },
    { 0x27, "PCH_D_1" },
    { 0x28, "PCH_W_1" },
    { 0x2A, "PCH_R_1" },
    { 0x39, "PCH_D_2" },
    { 0x38, "PCH_D_2" },
    { 0x3C, "PCH_D_4" },
    { 0x3D, "PCH_D_2" },
    { 0x41, "PCH_T_1" },
    { 0x34, "PCH_I_1" },
    { 0x43, "PCH_C_1" },
    { 0x40, "PCH_X_1" },
    { 0x31, "PCH_O_1" },
    { 0x2D, "PCH_G_1" },
    { 0x45, "PCH_S_1" },
};


static BattleDataEntry data_table[8] = {
    { 0x05, chuchurina_appeal_event },
    { 0x06, attack_audience_event },
    { 0x01, __makeTechMenuFunc },
    { 0x30, btldefaultevt_Dummy },
    { 0x40, party_win_reaction },
    { 0x07, subset_attack_ready_jump_event },
    { 0x2F, btldefaultevt_avoid_counter_event },
    { 0x00, NULL },
};

BattleUnitKindPart unitpartsdata_Party_Chuchurina[4] = {
    {
        1,                                    // partNum
        "btl_un_chuchurina",                  // partName
        "c_tyutyu",                           // modelName
        { 0.0f, 0.0f, 0.0f },                 // positionOffset
        { 16.0f, 24.0f, 5.0f },               // hitBaseOffset
        { 0.0f, 24.0f, 0.0f },                // hitCursorBaseOffset
        20,                                   // shadowSize
        24,                                   // field_0x32
        255,                                  // baseAlpha
        (BattleUnitDefense*)defence,          // defense
        (BattleUnitDefenseAttr*)defence_attr, // defenseAttr
        0x00000009,                           // attributes
        0,                                    // counterAttributes
        pose_table_chuchurina_stay,           // poseTable
    },
    {
        2,                                    // partNum
        "btl_un_chuchurina",                  // partName
        "c_tyutyu",                           // modelName
        { 0.0f, 0.0f, 0.0f },                 // positionOffset
        { 16.0f, 24.0f, 5.0f },               // hitBaseOffset
        { 0.0f, 24.0f, 0.0f },                // hitCursorBaseOffset
        20,                                   // shadowSize
        24,                                   // field_0x32
        255,                                  // baseAlpha
        (BattleUnitDefense*)defence,          // defense
        (BattleUnitDefenseAttr*)defence_attr, // defenseAttr
        0x13010000,                           // attributes
        0,                                    // counterAttributes
        pose_table_chuchurina_stay,           // poseTable
    },
    {
        3,                                    // partNum
        "btl_un_chuchurina",                  // partName
        "c_tyutyu",                           // modelName
        { 0.0f, 0.0f, 0.0f },                 // positionOffset
        { 16.0f, 24.0f, 5.0f },               // hitBaseOffset
        { 0.0f, 24.0f, 0.0f },                // hitCursorBaseOffset
        20,                                   // shadowSize
        24,                                   // field_0x32
        255,                                  // baseAlpha
        (BattleUnitDefense*)defence,          // defense
        (BattleUnitDefenseAttr*)defence_attr, // defenseAttr
        0x13010000,                           // attributes
        0,                                    // counterAttributes
        pose_table_chuchurina_stay,           // poseTable
    },
    {
        4,                                    // partNum
        "btl_un_chuchurina",                  // partName
        "c_tyutyu",                           // modelName
        { 0.0f, 0.0f, 0.0f },                 // positionOffset
        { 16.0f, 24.0f, 5.0f },               // hitBaseOffset
        { 0.0f, 24.0f, 0.0f },                // hitCursorBaseOffset
        20,                                   // shadowSize
        24,                                   // field_0x32
        255,                                  // baseAlpha
        (BattleUnitDefense*)defence,          // defense
        (BattleUnitDefenseAttr*)defence_attr, // defenseAttr
        0x13010000,                           // attributes
        0,                                    // counterAttributes
        pose_table_chuchurina_stay,           // poseTable
    },
};

BattleUnitKind unitdata_Party_Chuchurina = {
    UNIT_MS_MOWZ,                    // mUnitType
    "btl_un_chuchurina",             // name
    10,                              // maxHp
    5,                               // maxFp
    5,                               // mDangerHP
    1,                               // mPerilHP
    1,                               // mLevel
    0,                               // mBonusExp
    0,                               // bonusCoinRolls
    0,                               // bonusCoinRate
    0,                               // baseCoin
    100,                             // escapeRate
    9,                               // renzokuSoftCap
    20,                              // width
    33,                              // height
    { 10, 22 },                      // statusIconOffset
    { 0.0f, 12.0f, 0.0f },           // centerOffset
    { 0, 0 },                        // hpGaugeOffset
    { 0.0f, 30.0f, 0.0f },           // talkTogeBaseOffset
    { 0.0f, 30.0f, 0.0f },           // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },            // burnFlameOffset
    0.0f,                            // kagenukeOffsetX
    0.0f,                            // kagenukeOffsetZ
    { 10.0f, 0.0f, 0.0f },           // bintaHitOffset
    { 10.0f, 21.45f, 0.0f },         // kissHitOffset
    { 0.0f, 0.0f, 0.0f },            // cutBaseOffset
    0.0f,                            // cutWidth
    0.0f,                            // cutHeight
    0,                               // turnOrder
    0,                               // turnOrderVariance
    -1,                              // swallowRate
    0,                               // swallowAttribute
    100,                             // shotDamageRate
    20,                              // itemStealParam
    { 0.0f, 0.0f, 0.0f },            // starPointDispOffset
    "SFX_PARTY_DAMAGE1",             // damageSfxName
    "SFX_MARIO_DAMAGE_FIRE1",        // fireDamageSfxName
    "SFX_MARIO_DAMAGE_ICE1",         // iceDamageSfxName
    "SFX_MARIO_DAMAGE_BIRIBIRI1",    // electricDamageSfxName
    0x02000000,                      // attributeFlags
    (BattleVulnerableStatus*)regist, // vulnStatus
    4,                               // numParts
    unitpartsdata_Party_Chuchurina,  // mParts
    init_event,                      // initEvtScript
    data_table,                      // dataTable
};

static EVT_BEGIN(battle_entry_event)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    WAIT_FRAMES(40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_PARTY_BATTLE_SUBMOVE1L"), STRING("SFX_PARTY_BATTLE_SUBMOVE1R"), 0, 7, 7, 0, 0)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -2, 0)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()


static EVT_BEGIN(init_event)
    CALL(_chuchu_make_extra_work_area)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 15, 13)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 7, 7)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), STRING("SFX_PARTY_BATTLE_LANDING1"))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(battle_entry_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

BattleWeapon partyWeapon_ChuchurinaNormalAttackLeft = {
    "btl_wn_pcr_normal",                      // name
    336,                                      // icon
    ITEM_NULL,                                // item
    "msg_pch_binta",                          // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    NULL,                                     // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    21,                                       // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_binta",                           // acHelpMsg
    0x00034047,                               // special
    0x00000202,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    partyChuchurinaAttack_NormalAttack,       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 2, 3, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon partyWeapon_ChuchurinaNormalAttackRight = {
    "btl_wn_pcr_normal",                      // name
    336,                                      // icon
    ITEM_NULL,                                // item
    "msg_pch_binta",                          // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    NULL,                                     // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    22,                                       // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_binta",                           // acHelpMsg
    0x00034047,                               // special
    0x00000202,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    partyChuchurinaAttack_NormalAttack,       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 2, 3, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon partyWeapon_ChuchurinaNormalAttackLeftLast = {
    "btl_wn_pcr_normal",                      // name
    336,                                      // icon
    ITEM_NULL,                                // item
    "msg_pch_binta",                          // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetACOutputParam,                   // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    23,                                       // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_binta",                           // acHelpMsg
    0x00034047,                               // special
    0x00000002,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    partyChuchurinaAttack_NormalAttack,       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 2, 2, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon partyWeapon_ChuchurinaNormalAttackRightLast = {
    "btl_wn_pcr_normal",                      // name
    336,                                      // icon
    ITEM_NULL,                                // item
    "msg_pch_binta",                          // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetACOutputParam,                   // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    24,                                       // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_binta",                           // acHelpMsg
    0x00034047,                               // special
    0x00000002,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    partyChuchurinaAttack_NormalAttack,       // mAttackEvtScript
    { 0, 0, 0, 100, 0, 2, 2, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon partyWeapon_ChuchurinaMadowaseAttack = {
    "btl_wn_pcr_madowase",                    // name
    338,                                      // icon
    ITEM_NULL,                                // item
    "msg_pch_madowaseru",                     // description
    100,                                      // baseAccuracy
    3,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    3,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_madowaseru",                      // acHelpMsg
    0x00020000,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    100,                                      // dizzyChance
    3,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    partyChuchurinaAttack_MadowaseAttack,     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 5, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon partyWeapon_ChuchurinaItemSteal = {
    "btl_wn_pcr_steal",                       // name
    337,                                      // icon
    ITEM_NULL,                                // item
    "msg_pch_heart_catch",                    // description
    100,                                      // baseAccuracy
    2,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    20,                                       // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_heart_catch",                     // acHelpMsg
    0x00020000,                               // special
    0x00000202,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    partyChuchurinaAttack_ItemSteal,          // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 5, 0, 0, 0, 0, 0 }, // stageEffect
};

BattleWeapon partyWeapon_ChuchurinaKiss = {
    "btl_wn_pcr_kiss",                        // name
    339,                                      // icon
    ITEM_NULL,                                // item
    "msg_pch_kiss",                           // description
    100,                                      // baseAccuracy
    10,                                       // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101160,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_chuchurina_kiss",                 // acHelpMsg
    0x00020008,                               // special
    0x000007FF,                               // counterResistanceFlags
    (u32)lbl_80003500,                        // targetWeightingFlags: the flags 0x80003500, which dtk reads as
                                              // this .init label; as a number the unit's data score drops
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    partyChuchurinaAttack_Kiss,               // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 5, 0, 0, 0, 0, 0 }, // stageEffect
};


EVT_BEGIN(partyChuchurinaAttack_NormalAttack)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    IF_INT_EQUAL(LW(5), 5)
        CALL(btlevtcmd_GetStatusMg, LW(3), LW(6))
        MULTIPLY_FLOAT(LW(6), 40)
        CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    END_IF
    CALL(btlevtcmd_GetStatusMg, -2, LW(5))
    MULTIPLY_FLOAT(LW(5), 12)
    CALL(_get_binta_hit_position, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(5))
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    ADD(LW(2), 5)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_A1_1"))
    BEGIN_SIBLING_EVENT_GET_ID(LW(0))
        CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
        CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
        CALL(btlevtcmd_AcGetDifficulty, LW(0))
        SUBTRACT(LW(0), 3)
        SWITCH(LW(0))
            CASE_LESS_OR_EQUAL(-3)
            SET_VALUE(LW(1), 25)
            SET_VALUE(LW(2), 30)
            CASE_EQUAL(-2)
            SET_VALUE(LW(1), 20)
            SET_VALUE(LW(2), 30)
            CASE_EQUAL(-1)
            SET_VALUE(LW(1), 17)
            SET_VALUE(LW(2), 30)
            CASE_EQUAL(0)
            SET_VALUE(LW(1), 15)
            SET_VALUE(LW(2), 30)
            CASE_EQUAL(1)
            SET_VALUE(LW(1), 14)
            SET_VALUE(LW(2), 30)
            CASE_EQUAL(2)
            SET_VALUE(LW(1), 13)
            SET_VALUE(LW(2), 30)
            CASE_DEFAULT
            SET_VALUE(LW(1), 12)
            SET_VALUE(LW(2), 30)
        END_SWITCH
        CALL(btlevtcmd_GetPartyTechLv, -2, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AcSetParamAll, 14, 210, 178, LW(1), LW(2), 34, 35, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AcSetGaugeParam, 34, 68, 100, 100)
            CALL(btlevtcmd_AcSetFlag, 64)
            CASE_EQUAL(1)
            CALL(btlevtcmd_AcSetParamAll, 14, 210, 178, LW(1), LW(2), 50, 51, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AcSetGaugeParam, 34, 68, 100, 100)
            CALL(btlevtcmd_AcSetFlag, 64)
            CASE_DEFAULT
            CALL(btlevtcmd_AcSetParamAll, 14, 210, 178, LW(1), LW(2), 100, 100, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AcSetGaugeParam, 100, 100, 100, 100)
            CALL(btlevtcmd_AcSetFlag, 64)
        END_SWITCH
        CALL(btlevtcmd_SetupAC, -2, 6, 1, 0)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_StartAC, 1)
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(0))
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        SET_VALUE(LW(6), 0)
        SET_VALUE(LW(5), 5)
        LOOP(LW(5))
            ADD(LW(6), 1)
            SET_VALUE(LW(0), LW(6))
            DIVIDE(LW(0), 2)
            MODULO(LW(0), 2)
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_SetScale, -2, FLOAT(1.05f), FLOAT(1.05f), FLOAT(1.0f))
            ELSE
                CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
            END_IF
            WAIT_FRAMES(1)
            SET_VALUE(LW(5), 5)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        SET_VALUE(LW(5), 5)
        LOOP(LW(5))
            CALL(btlevtcmd_AcGetOutputParam, 1, LW(0))
            SET_VALUE(LW(1), 255)
            SET_VALUE(LW(2), 190)
            MULTIPLY(LW(2), LW(0))
            DIVIDE(LW(2), 100)
            SUBTRACT(LW(1), LW(2))
            CALL(btlevtcmd_SetRGB, -2, 1, 255, LW(1), LW(1))
            WAIT_FRAMES(1)
            SET_VALUE(LW(5), 5)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_ResultAC)
    DELETE_EVENT(LW(14))
    DELETE_EVENT(LW(15))
    CALL(btlevtcmd_GetPartyTechLv, -2, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(btlevtcmd_SetUnitWork, -2, 5, 4)
        CASE_EQUAL(1)
        CALL(btlevtcmd_SetUnitWork, -2, 5, 3)
        CASE_DEFAULT
        CALL(btlevtcmd_SetUnitWork, -2, 5, 2)
    END_SWITCH
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(btlevtcmd_SetRGB, -2, 1, 255, 255, 255)
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        SET_VALUE(LW(8), -75)
        CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, LW(0), EVTDAT_POINTER_MAX)
        IF_INT_GREATER_OR_EQUAL(LW(0), 180)
            SUBTRACT(LW(0), 360)
        END_IF
        CALL(evt_sub_intpl_init, 5, LW(0), LW(8), 8)
        LOOP(8)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetRotate, -2, 0, LW(0), 0)
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LW(8), -60)
        SET_VALUE(LW(9), -75)
        CALL(evt_sub_intpl_init, 11, LW(9), LW(8), 4)
        LOOP(4)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetRotate, -2, 0, LW(0), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(14))
    SET_VALUE(LW(13), 18)
    SET_VALUE(LW(10), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 4, 0)
    LABEL(10)
    ADD(LW(10), 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_A1_2"))
    IF_INT_GREATER_OR_EQUAL(LW(10), 2)
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 1048832, LW(5))
    ELSE
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    END_IF
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(6)
        CASE_OR_EQUAL(4)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
        WAIT_MS(1000)
    END_SWITCH
    IF_INT_NOT_EQUAL(LW(5), 1)
        DELETE_EVENT(LW(14))
        SET_VALUE(LW(14), 0)
        CALL(btlevtcmd_StopAC)
        IF_INT_EQUAL(LW(5), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(5), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(5), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        LOOP(20)
            CALL(btlevtcmd_AddRotate, -2, 0, 18, 0)
            WAIT_FRAMES(1)
        END_LOOP
        LOOP(36)
            CALL(btlevtcmd_AddRotate, -2, 0, 10, 0)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_A2_4"))
        WAIT_FRAMES(52)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        GOTO(95)
    END_IF
    SET_VALUE(LW(0), LW(10))
    MODULO(LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(12), PTR(&partyWeapon_ChuchurinaNormalAttackRight))
    ELSE
        SET_VALUE(LW(12), PTR(&partyWeapon_ChuchurinaNormalAttackLeft))
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 4, LW(15))
    IF_INT_NOT_EQUAL(LW(15), 0)
        DELETE_EVENT(LW(15))
        CALL(btlevtcmd_SetUnitWork, -2, 4, 0)
    END_IF
    IF_INT_NOT_EQUAL(LW(14), 0)
        DELETE_EVENT(LW(14))
        SET_VALUE(LW(14), 0)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(10), LW(0))
        GOTO(20)
    ELSE
        CALL(btlevtcmd_GetResultAC, LW(6))
        IF_BITS_CLEAR(LW(6), 2)
            GOTO(20)
        END_IF
    END_IF
    GOTO(21)
    LABEL(20)
    IF_INT_EQUAL(LW(12), PTR(&partyWeapon_ChuchurinaNormalAttackLeft))
        SET_VALUE(LW(12), PTR(&partyWeapon_ChuchurinaNormalAttackLeftLast))
    ELSE
        SET_VALUE(LW(12), PTR(&partyWeapon_ChuchurinaNormalAttackRightLast))
    END_IF
    LABEL(21)
    CALL(btlevtcmd_PreCheckCounter, -2, LW(3), LW(4), LW(12), 256, LW(0))
    SWITCH(LW(0))
        CASE_OR_EQUAL(7)
        CASE_OR_EQUAL(8)
        CASE_OR_EQUAL(9)
        CASE_OR_EQUAL(10)
        CASE_OR_EQUAL(11)
        CASE_OR_EQUAL(12)
        CASE_OR_EQUAL(13)
        CASE_OR_EQUAL(14)
        CASE_OR_EQUAL(15)
        CASE_OR_EQUAL(16)
        CASE_OR_EQUAL(17)
        IF_INT_EQUAL(LW(12), PTR(&partyWeapon_ChuchurinaNormalAttackLeft))
            SET_VALUE(LW(12), PTR(&partyWeapon_ChuchurinaNormalAttackLeftLast))
        ELSE
            SET_VALUE(LW(12), PTR(&partyWeapon_ChuchurinaNormalAttackRightLast))
        END_IF
        BEGIN_SIBLING_EVENT_GET_ID(LW(15))
            SET_VALUE(LW(7), LW(13))
            CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, LW(0), EVTDAT_POINTER_MAX)
            IF_INT_GREATER_OR_EQUAL(LW(0), 180)
                SUBTRACT(LW(0), 360)
                CALL(evt_sub_intpl_init, 11, LW(0), 0, 4)
                LOOP(4)
                    CALL(evt_sub_intpl_get_value)
                    CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
                    WAIT_FRAMES(1)
                    IF_INT_EQUAL(LW(1), 1)
                        BREAK_LOOP
                    END_IF
                END_LOOP
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
            END_SIBLING_EVENT
            CALL(btlevtcmd_WaitEventEnd, LW(15))
            CALL(btlevtcmd_GetResultAC, LW(6))
            IF_BITS_SET(LW(6), 2)
                CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131328, LW(5))
            ELSE
                CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
            END_IF
            RETURN
            CASE_END
        END_SWITCH
        CALL(evt_btl_camera_shake_h, 0, 2, 0, 12, 13)
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(10), LW(0))
            CALL(evt_snd_sfxon, STRING("SFX_BTL_CHUCHURINA_ATTACK3"), 0)
            IF_BITS_SET(LW(6), 2)
                CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131328, LW(5))
            ELSE
                CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
            END_IF
        ELSE
            SET_VALUE(LW(0), LW(10))
            AND(LW(0), 1)
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_snd_sfxon, STRING("SFX_BTL_CHUCHURINA_ATTACK2"), 0)
            ELSE
                CALL(evt_snd_sfxon, STRING("SFX_BTL_CHUCHURINA_ATTACK1"), 0)
            END_IF
            IF_BITS_SET(LW(6), 2)
                CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131072, LW(5))
            ELSE
                CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
            END_IF
        END_IF
        CALL(btlevtcmd_GetResultAC, LW(6))
        IF_BITS_SET(LW(6), 2)
            CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
            SUBTRACT(LW(0), 2)
            IF_INT_GREATER_OR_EQUAL(LW(0), 0)
                CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(0), LW(11))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_ACSuccessEffect, LW(11), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(11))
            ELSE
                CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
            END_IF
        ELSE
            CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
        END_IF
        IF_BITS_SET(LW(6), 2)
            SWITCH(LW(10))
                CASE_LESS_OR_EQUAL(2)
                BEGIN_SIBLING_EVENT_GET_ID(LW(15))
                    SET_VALUE(LW(7), LW(13))
                    IF_INT_GREATER_OR_EQUAL(LW(7), 0)
                        SET_VALUE(LW(8), 75)
                    ELSE
                        SET_VALUE(LW(8), -75)
                    END_IF
                    CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, LW(0), EVTDAT_POINTER_MAX)
                    IF_INT_GREATER_OR_EQUAL(LW(0), 180)
                        SUBTRACT(LW(0), 360)
                    END_IF
                    CALL(evt_sub_intpl_init, 5, LW(0), LW(8), 14)
                    LOOP(14)
                        CALL(evt_sub_intpl_get_value)
                        CALL(btlevtcmd_SetRotate, -2, 0, LW(0), 0)
                        WAIT_FRAMES(1)
                    END_LOOP
                    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
                    IF_INT_LESS(LW(10), LW(0))
                        SET_VALUE(LW(7), LW(13))
                        IF_INT_GREATER_OR_EQUAL(LW(7), 0)
                            SET_VALUE(LW(8), 75)
                            SET_VALUE(LW(9), 75)
                        ELSE
                            SET_VALUE(LW(8), -75)
                            SET_VALUE(LW(9), -75)
                        END_IF
                        CALL(evt_sub_intpl_init, 11, LW(9), LW(8), 6)
                        LOOP(6)
                            CALL(evt_sub_intpl_get_value)
                            CALL(btlevtcmd_SetRotate, -2, 0, LW(0), 0)
                            WAIT_FRAMES(1)
                        END_LOOP
                    ELSE
                        SET_VALUE(LW(7), LW(13))
                        IF_INT_GREATER_OR_EQUAL(LW(7), 0)
                            SET_VALUE(LW(8), 0)
                            SET_VALUE(LW(9), 75)
                        ELSE
                            SET_VALUE(LW(8), 0)
                            SET_VALUE(LW(9), -75)
                        END_IF
                        CALL(evt_sub_intpl_init, 11, LW(9), LW(8), 12)
                        LOOP(12)
                            CALL(evt_sub_intpl_get_value)
                            CALL(btlevtcmd_SetRotate, -2, 0, LW(0), 0)
                            WAIT_FRAMES(1)
                        END_LOOP
                    END_IF
                END_SIBLING_EVENT
                CALL(btlevtcmd_WaitEventEnd, LW(15))
                CASE_DEFAULT
                BEGIN_SIBLING_EVENT_GET_ID(LW(15))
                    CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, LW(8), EVTDAT_POINTER_MAX)
                    SET_VALUE(LW(7), 360)
                    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
                    IF_INT_GREATER_OR_EQUAL(LW(10), LW(0))
                        SET_VALUE(LW(0), LW(8))
                        IF_INT_GREATER_OR_EQUAL(LW(8), 180)
                            SUBTRACT(LW(0), 360)
                        END_IF
                        IF_INT_GREATER_OR_EQUAL(LW(13), 0)
                            SUBTRACT(LW(7), LW(0))
                        ELSE
                            ADD(LW(7), LW(0))
                        END_IF
                    ELSE
                        ADD(LW(7), 60)
                        SET_VALUE(LW(0), LW(8))
                        IF_INT_GREATER_OR_EQUAL(LW(8), 180)
                            SUBTRACT(LW(0), 360)
                        END_IF
                        IF_INT_GREATER_OR_EQUAL(LW(13), 0)
                            SUBTRACT(LW(7), LW(0))
                        ELSE
                            ADD(LW(7), LW(0))
                        END_IF
                    END_IF
                    IF_INT_LESS(LW(13), 0)
                        MULTIPLY(LW(7), -1)
                    END_IF
                    CALL(evt_sub_intpl_init, 5, 0, LW(7), 20)
                    LOOP(20)
                        CALL(evt_sub_intpl_get_value)
                        ADD(LW(0), LW(8))
                        CALL(btlevtcmd_SetRotate, -2, 0, LW(0), 0)
                        WAIT_FRAMES(1)
                    END_LOOP
                END_SIBLING_EVENT
                CALL(btlevtcmd_SetUnitWork, -2, 4, LW(15))
                WAIT_FRAMES(10)
            END_SWITCH
        ELSE
            BEGIN_SIBLING_EVENT_GET_ID(LW(15))
                CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, LW(8), EVTDAT_POINTER_MAX)
                SET_VALUE(LW(7), 720)
                IF_INT_GREATER_OR_EQUAL(LW(10), 1)
                    SET_VALUE(LW(0), LW(8))
                    IF_INT_GREATER_OR_EQUAL(LW(8), 180)
                        SUBTRACT(LW(0), 360)
                    END_IF
                    SUBTRACT(LW(7), LW(0))
                END_IF
                IF_INT_LESS(LW(13), 0)
                    MULTIPLY(LW(7), -1)
                END_IF
                CALL(evt_sub_intpl_init, 0, 0, LW(7), 60)
                LOOP(60)
                    CALL(evt_sub_intpl_get_value)
                    ADD(LW(0), LW(8))
                    CALL(btlevtcmd_SetRotate, -2, 0, LW(0), 0)
                    WAIT_FRAMES(1)
                END_LOOP
            END_SIBLING_EVENT
            CALL(btlevtcmd_WaitEventEnd, LW(15))
            CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_X_1"))
            WAIT_FRAMES(60)
            CALL(btlevtcmd_StopAC)
            CALL(evt_audience_ap_recovery)
            CALL(btlevtcmd_InviteApInfoReport)
            GOTO(95)
        END_IF
        MULTIPLY(LW(13), -1)
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        IF_INT_LESS(LW(10), LW(0))
            GOTO(10)
        END_IF
        LABEL(90)
        CALL(btlevtcmd_StopAC)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_A1_3"))
        WAIT_FRAMES(40)
        LABEL(95)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), 0, LW(14))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        BEGIN_SIBLING_EVENT_GET_ID(LW(15))
            CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), LW(14), -1, 0)
        END_SIBLING_EVENT
        SET_VALUE(LW(11), LW(14))
        SUBTRACT(LW(11), 11)
        SUBTRACT(LW(14), 1)
        IF_INT_GREATER(LW(14), LW(11))
            SET_VALUE(LW(13), LW(14))
            SUBTRACT(LW(13), 10)
            IF_BITS_SET(LW(6), 2)
                CALL(btlevtcmd_ACRStart, -2, 0, LW(13), LW(14), 0)
                CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
                IF_INT_GREATER_OR_EQUAL(LW(6), 2)
                    DELETE_EVENT(LW(15))
                    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
                    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 30)
                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_O_1"))
                    CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
                    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
                    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
                    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
                    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
                    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
                    CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), LW(2))
                ELSE
                    CALL(btlevtcmd_WaitEventEnd, LW(15))
                END_IF
            ELSE
                CALL(btlevtcmd_WaitEventEnd, LW(15))
            END_IF
        ELSE
            CALL(btlevtcmd_WaitEventEnd, LW(15))
        END_IF
        LABEL(99)
        CALL(btlevtcmd_ResetFaceDirection, -2)
        CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
EVT_END()


EVT_BEGIN(partyChuchurinaAttack_MadowaseAttack)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcSetFlag, 41)
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 3)
        SET_VALUE(LW(1), 50)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 4)
        SET_VALUE(LW(1), 80)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 5)
        SET_VALUE(LW(1), 100)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 7)
        SET_VALUE(LW(1), 100)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 8)
        SET_VALUE(LW(1), 110)
        CALL(btlevtcmd_AcSetFlag, 49)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 10)
        SET_VALUE(LW(1), 120)
        CALL(btlevtcmd_AcSetFlag, 49)
        CASE_DEFAULT
        SET_VALUE(LW(0), 12)
        SET_VALUE(LW(1), 130)
        CALL(btlevtcmd_AcSetFlag, 49)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 1, 240, 178, LW(0), LW(1), 100, 1, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetGaugeParam, 34, 68, 100, 100)
    CALL(btlevtcmd_SetupAC, -2, 6, 1, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 1, LW(0), 73, 400, LW(0), 43, 0, 40, 5)
    WAIT_FRAMES(22)
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_StopAC)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_I_1"))
    LOOP(40)
        CALL(btlevtcmd_AddRotate, -2, 0, 36, 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_A4_1"))
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1L"), STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1R"), 0, 15, 13)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1L"), STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1R"), 0, 7, 7)
    CALL(btlevtcmd_SetDiveSound, -2, STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1L"), STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1R"), 0, 7, 7)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_BTL_CHURINA_CONFUSE_JUMP1"), STRING("SFX_BTL_CHURINA_CONFUSE_LANDING1"))
    CALL(btlevtcmd_SetPartsWalkSound, -2, 2, STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1L"), STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1R"), 0, 15, 13)
    CALL(btlevtcmd_SetPartsRunSound, -2, 2, STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1L"), STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1R"), 0, 7, 7)
    CALL(btlevtcmd_SetPartsDiveSound, -2, 2, STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1L"), STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1R"), 0, 7, 7)
    CALL(btlevtcmd_SetPartsJumpSound, -2, 2, STRING("SFX_BTL_CHURINA_CONFUSE_JUMP1"), STRING("SFX_BTL_CHURINA_CONFUSE_LANDING1"))
    CALL(btlevtcmd_SetPartsWalkSound, -2, 3, STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1L"), STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1R"), 0, 15, 13)
    CALL(btlevtcmd_SetPartsRunSound, -2, 3, STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1L"), STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1R"), 0, 7, 7)
    CALL(btlevtcmd_SetPartsDiveSound, -2, 3, STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1L"), STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1R"), 0, 7, 7)
    CALL(btlevtcmd_SetPartsJumpSound, -2, 3, STRING("SFX_BTL_CHURINA_CONFUSE_JUMP1"), STRING("SFX_BTL_CHURINA_CONFUSE_LANDING1"))
    CALL(btlevtcmd_SetPartsWalkSound, -2, 4, STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1L"), STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1R"), 0, 15, 13)
    CALL(btlevtcmd_SetPartsRunSound, -2, 4, STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1L"), STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1R"), 0, 7, 7)
    CALL(btlevtcmd_SetPartsDiveSound, -2, 4, STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1L"), STRING("SFX_BTL_CHURINA_CONFUSE_MOVE1R"), 0, 7, 7)
    CALL(btlevtcmd_SetPartsJumpSound, -2, 4, STRING("SFX_BTL_CHURINA_CONFUSE_JUMP1"), STRING("SFX_BTL_CHURINA_CONFUSE_LANDING1"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 40)
    SUBTRACT(LW(2), 20)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 20)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 180, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("PCH_A3_1"))
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 50331648)
    CALL(evt_eff, STRING(""), STRING("bomb"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CHURINA_CONFUSE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(30)
        CALL(btlevtcmd_SetPartsFallAccel, -2, 2, FLOAT(0.3f))
        CALL(btlevtcmd_GetPartsPos, -2, 2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FallPartsPosition, -2, 2, LW(0), LW(1), LW(2), 25)
        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 2, 43)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    SUBTRACT(LW(2), 10)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 10)
    CALL(btlevtcmd_SetPartsPos, -2, 3, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsRotate, -2, 3, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("PCH_A3_2"))
    CALL(btlevtcmd_OffPartsAttribute, -2, 3, 50331648)
    CALL(evt_eff, STRING(""), STRING("bomb"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(30)
        CALL(btlevtcmd_SetPartsFallAccel, -2, 3, FLOAT(0.3f))
        CALL(btlevtcmd_GetPartsPos, -2, 3, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FallPartsPosition, -2, 3, LW(0), LW(1), LW(2), 25)
        CALL(btlevtcmd_SetPartsRotate, -2, 3, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 3, 43)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 30)
    ADD(LW(2), 10)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), -10)
    CALL(btlevtcmd_SetPartsPos, -2, 4, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsRotate, -2, 4, 0, 180, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("PCH_A3_3"))
    CALL(btlevtcmd_OffPartsAttribute, -2, 4, 50331648)
    CALL(evt_eff, STRING(""), STRING("bomb"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(30)
        CALL(btlevtcmd_SetPartsFallAccel, -2, 4, FLOAT(0.3f))
        CALL(btlevtcmd_GetPartsPos, -2, 4, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FallPartsPosition, -2, 4, LW(0), LW(1), LW(2), 25)
        CALL(btlevtcmd_SetPartsRotate, -2, 4, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 4, 43)
    END_SIBLING_EVENT
    WAIT_FRAMES(85)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 1)
    CALL(btlevtcmd_SetPartsBlur, -2, 1, 255, 255, 255, 255, 100, 100, 100, 50, 1)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 67108864)
    CALL(btlevtcmd_SetPartsBlur, -2, 2, 255, 255, 255, 255, 100, 100, 100, 50, 1)
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 67108864)
    CALL(btlevtcmd_SetPartsBlur, -2, 3, 255, 255, 255, 255, 100, 100, 100, 50, 1)
    CALL(btlevtcmd_OnPartsAttribute, -2, 3, 67108864)
    CALL(btlevtcmd_SetPartsBlur, -2, 4, 255, 255, 255, 255, 100, 100, 100, 50, 1)
    CALL(btlevtcmd_OnPartsAttribute, -2, 4, 67108864)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(40)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetStageSize, LW(5), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        DIVIDE(LW(5), 3)
        SET_VALUE(LW(0), 0)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(5))
        CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), FLOAT(3.0f), LW(7))
        CALL(evt_btl_camera_set_mode, 0, 3)
        CALL(evt_btl_camera_set_moveto, 1, LW(0), 90, 500, LW(0), 60, 0, LW(7), 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
        WAIT_FRAMES(40)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetStageSize, LW(5), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        DIVIDE(LW(5), 3)
        SET_VALUE(LW(0), 0)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(5))
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, 0, 2, 0, -1)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(0), 40)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        SUBTRACT(LW(0), 60)
        CALL(btlevtcmd_SetRotate, -2, 0, 180, 0)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        ADD(LW(0), 40)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        SUBTRACT(LW(0), 60)
        CALL(btlevtcmd_SetRotate, -2, 0, 180, 0)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        ADD(LW(0), 40)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        CALL(evt_eff, STRING(""), STRING("bomb"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 67108864)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(6), 2)
        CALL(btlevtcmd_SetPartsFallAccel, -2, LW(6), FLOAT(0.7f))
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(6), 42)
        CALL(btlevtcmd_GetPartsPos, -2, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetStageSize, LW(5), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        DIVIDE(LW(5), 3)
        SET_VALUE(LW(0), 0)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(5))
        CALL(btlevtcmd_SetPartsMoveSpeed, -2, LW(6), FLOAT(3.0f))
        CALL(btlevtcmd_DivePartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 0, 0, 2, 0, -1)
        CALL(btlevtcmd_GetPartsPos, -2, LW(6), LW(0), LW(1), LW(2))
        ADD(LW(0), 60)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 25, -1)
        SUBTRACT(LW(0), 90)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 180, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 25, -1)
        ADD(LW(0), 60)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 0, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 25, -1)
        SUBTRACT(LW(0), 90)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 180, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 25, -1)
        SUBTRACT(LW(0), 30)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 180, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 25, -1)
        ADD(LW(0), 30)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 0, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 25, -1)
        CALL(evt_eff, STRING(""), STRING("bomb"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 50331648)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 67108864)
        CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(6), 3)
        CALL(btlevtcmd_SetPartsFallAccel, -2, LW(6), FLOAT(0.7f))
        WAIT_FRAMES(20)
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(6), 42)
        CALL(btlevtcmd_GetPartsPos, -2, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetStageSize, LW(5), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        DIVIDE(LW(5), 3)
        SET_VALUE(LW(0), 0)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(5))
        CALL(btlevtcmd_SetPartsMoveSpeed, -2, LW(6), FLOAT(3.0f))
        CALL(btlevtcmd_DivePartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 0, 0, 2, 0, -1)
        CALL(btlevtcmd_GetPartsPos, -2, LW(6), LW(0), LW(1), LW(2))
        SUBTRACT(LW(0), 80)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 180, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 25, -1)
        ADD(LW(0), 160)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 0, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 40, -1)
        SUBTRACT(LW(0), 40)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 180, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 15, -1)
        SUBTRACT(LW(0), 60)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 180, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 20, -1)
        ADD(LW(0), 20)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 0, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 20, -1)
        CALL(evt_eff, STRING(""), STRING("bomb"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        CALL(btlevtcmd_OnPartsAttribute, -2, 3, 50331648)
        CALL(btlevtcmd_OffPartsAttribute, -2, 3, 67108864)
        CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(6), 4)
        CALL(btlevtcmd_SetPartsFallAccel, -2, LW(6), FLOAT(0.7f))
        WAIT_FRAMES(60)
        CALL(btlevtcmd_AnimeChangePoseType, -2, LW(6), 42)
        CALL(btlevtcmd_GetPartsPos, -2, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetStageSize, LW(5), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        DIVIDE(LW(5), 3)
        SET_VALUE(LW(0), 0)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(5))
        CALL(btlevtcmd_SetPartsMoveSpeed, -2, LW(6), FLOAT(3.0f))
        CALL(btlevtcmd_DivePartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 0, 0, 2, 0, -1)
        CALL(btlevtcmd_GetPartsPos, -2, LW(6), LW(0), LW(1), LW(2))
        ADD(LW(0), 30)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 10, -1)
        ADD(LW(0), 30)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 0, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 10, -1)
        SUBTRACT(LW(0), 30)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 180, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 10, -1)
        SUBTRACT(LW(0), 30)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 180, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 10, -1)
        SUBTRACT(LW(0), 30)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 180, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 10, -1)
        SUBTRACT(LW(0), 30)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 180, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 10, -1)
        ADD(LW(0), 30)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 0, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 10, -1)
        SUBTRACT(LW(0), 30)
        CALL(btlevtcmd_SetPartsRotate, -2, LW(6), 0, 180, 0)
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(6), LW(0), LW(1), LW(2), 10, -1)
        CALL(evt_eff, STRING(""), STRING("bomb"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        CALL(btlevtcmd_OnPartsAttribute, -2, 4, 50331648)
        CALL(btlevtcmd_OffPartsAttribute, -2, 4, 67108864)
        CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    END_SIBLING_EVENT
    LABEL(9)
    SET_VALUE(LW(0), 0)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(1))
    ADD(LW(0), LW(1))
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(1))
    ADD(LW(0), LW(1))
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(1))
    ADD(LW(0), LW(1))
    CALL(btlevtcmd_GetUnitWork, -2, 3, LW(1))
    ADD(LW(0), LW(1))
    IF_INT_NOT_EQUAL(LW(0), 0)
        WAIT_FRAMES(1)
        GOTO(9)
    END_IF
    CALL(evt_snd_sfxon, STRING("SFX_BTL_CHURINA_CONFUSE1"), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(15), 0)
    LABEL(10)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(6))
    IF_INT_NOT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        GOTO(50)
    END_IF
    CALL(btlevtcmd_AcGetOutputParam, 1, LW(0))
    CALL(_make_madowase_weapon, LW(12), LW(0))
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131328, LW(5))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        IF_INT_GREATER_OR_EQUAL(LW(6), 0)
            CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
            ADD(LW(15), 1)
            CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
        END_IF
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    END_IF
    LABEL(50)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    IF_INT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
    END_IF
    LABEL(80)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_B_1"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 80)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.15f))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CHURINA_CONFUSE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 5, 0)
        CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
        MULTIPLY(LW(5), 9)
        LOOP(40)
            CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(5))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
        MULTIPLY(LW(5), 36)
        LOOP(20)
            CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(5))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_ACRStart, -2, 45, 59, 59, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 60, -1)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    SWITCH(LW(6))
        CASE_GREATER_OR_EQUAL(2)
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_Y_1"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        WAIT_MS(300)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_AddRotate, -2, 0, 180, 0)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, 0, 2, 0, -1)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CASE_DEFAULT
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(evt_audience_acrobat_notry)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_R_2"))
        CALL(btlevtcmd_AddRotate, -2, 0, 180, 0)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, 0, 2, 0, -1)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    END_SWITCH
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 15, 13)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 7, 7)
    CALL(btlevtcmd_SetDiveSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 7, 7)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), STRING("SFX_PARTY_BATTLE_LANDING1"))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()






EVT_BEGIN(partyChuchurinaAttack_ItemSteal)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    SET_VALUE(LW(14), 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&partyWeapon_ChuchurinaItemSteal), 256, LW(5))
    IF_INT_EQUAL(LW(5), 5)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_GetWidth, LW(3), LW(15))
        DIVIDE(LW(15), 2)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(15))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 20)
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&partyWeapon_ChuchurinaItemSteal), 256, LW(5))
        RETURN
    END_IF
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetWidth, LW(3), LW(15))
    MULTIPLY(LW(15), 2)
    DIVIDE(LW(15), 3)
    ADD(LW(15), 10)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(15))
    ADD(LW(2), 8)
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, 1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    WAIT_MS(200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_K_1"))
    WAIT_FRAMES(6)
    IF_INT_NOT_EQUAL(LW(5), 1)
        WAIT_FRAMES(20)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        SWITCH(LW(5))
            CASE_OR_EQUAL(4)
            CASE_END
            CASE_EQUAL(3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
            CASE_EQUAL(6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
            CASE_EQUAL(2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
            CASE_EQUAL(1)
            CASE_DEFAULT
        END_SWITCH
        WAIT_FRAMES(30)
        CALL(btlevtcmd_StopAC)
        GOTO(90)
    END_IF
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&partyWeapon_ChuchurinaItemSteal), 256, LW(5))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, -2, LW(15))
    MULTIPLY(LW(15), 2)
    DIVIDE(LW(15), 3)
    ADD(LW(1), LW(15))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 20)
    CALL(evt_eff, STRING(""), STRING("kiss"), 0, LW(0), LW(1), LW(2), 1, 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CHURINA_KISS1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_OnUnitFlag, LW(3), 33554432)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_Z_1"))
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(btlevtcmd_AnimeFlagOnOff, -2, 1, 256, 1)
    WAIT_FRAMES(1)
    CALL(mono_capture_event)
    SPAWN_EVENT_GET_ID(PTR(main_evt), LW(13))
    BEGIN_INLINE_EVENT
        CALL(mono_on)
    END_INLINE_EVENT
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(_get_itemsteal_param, LW(3), LW(0), LW(1))
    CALL(btlevtcmd_AcSetParamAll, 1, LW(0), 2, 2, LW(1), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 0)
    CALL(btlevtcmd_SetupAC, -2, 15, 1, 0)
    WAIT_FRAMES(22)
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_SET(LW(6), 2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_A4_1"))
    END_IF
    WAIT_FRAMES(35)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_CHURINA_EYE1"), 0)
    WAIT_FRAMES(47)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetWidth, LW(3), LW(15))
    DIVIDE(LW(15), 2)
    ADD(LW(0), LW(15))
    CALL(btlevtcmd_GetHeight, LW(3), LW(15))
    ADD(LW(1), LW(15))
    ADD(LW(1), 5)
    ADD(LW(2), 8)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 16, -1)
    CALL(btlevtcmd_GetResultAC, LW(6))
    CALL(_chuchu_item_steal, LW(3), LW(14), LW(6))
    IF_INT_EQUAL(LW(14), 0)
        CALL(btlevtcmd_JumpContinue, -2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_F_1"))
        WAIT_FRAMES(22)
        BEGIN_INLINE_EVENT
            CALL(mono_off)
            DELETE_EVENT(LW(13))
        END_INLINE_EVENT
        WAIT_FRAMES(20)
        CALL(btlevtcmd_OffUnitFlag, LW(3), 33554432)
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_R_2"))
        CALL(btlevtcmd_AddRotate, -2, 0, 180, 0)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, 0, 2, 0, -1)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
    CALL(evt_snd_sfxon, STRING("SFX_BTL_CHURINA_HEARTCATCH1"), 0)
    CALL(btlevtcmd_OffUnitFlag, LW(3), 33554432)
    BEGIN_INLINE_EVENT
        CALL(mono_off)
        DELETE_EVENT(LW(13))
    END_INLINE_EVENT
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 50)
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_A3_1"))
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 10, 0)
        LOOP(18)
            CALL(btlevtcmd_AddRotate, -2, 0, 0, 20)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    LABEL(90)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    IF_INT_NOT_EQUAL(LW(14), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHeight, -2, LW(5))
        ADD(LW(1), LW(5))
        ADD(LW(1), 5)
        CALL(btlevtcmd_BtlIconEntryItemId, LW(14), LW(0), LW(1), LW(2), LW(8))
        BEGIN_SIBLING_EVENT_GET_ID(LW(15))
            CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
            CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
            CALL(btlevtcmd_AddRotate, -2, 0, 180, 0)
            CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, 0, 2, 0, -1)
            CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        END_SIBLING_EVENT
        LABEL(91)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHeight, -2, LW(5))
        ADD(LW(1), LW(5))
        ADD(LW(1), 5)
        CALL(btlevtcmd_BtlIconSetPosition, LW(8), LW(0), LW(1), LW(2))
        IS_EVENT_RUNNING(LW(15), LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            WAIT_FRAMES(1)
            GOTO(91)
        END_IF
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_A2_2"))
        WAIT_FRAMES(4)
        CALL(btlevtcmd_AnnounceSetParam, 0, LW(14))
        CALL(btlevtcmd_AnnounceMessage, 1, 0, 0, STRING("btl_msg_steal_item_get"), 90)
        CALL(btlevtcmd_BtlIconDelete, LW(8))
    ELSE
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_AddRotate, -2, 0, 180, 0)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, 0, 2, 0, -1)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    END_IF
    LABEL(99)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partyChuchurinaAttack_Kiss)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -2)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), LW(3))
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    END_IF
    CALL_CHILD(unk_evt_803510ec)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CalculateFaceDirection, LW(3), LW(4), -2, -1, 16, LW(15))
        CALL(btlevtcmd_ChangeFaceDirection, LW(3), LW(15))
    END_IF
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 300)
        SET_VALUE(LW(1), 20)
        SET_VALUE(LW(2), 20)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 270)
        SET_VALUE(LW(1), 18)
        SET_VALUE(LW(2), 22)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 240)
        SET_VALUE(LW(1), 17)
        SET_VALUE(LW(2), 24)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 210)
        SET_VALUE(LW(1), 16)
        SET_VALUE(LW(2), 25)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 195)
        SET_VALUE(LW(1), 14)
        SET_VALUE(LW(2), 26)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 180)
        SET_VALUE(LW(1), 13)
        SET_VALUE(LW(2), 27)
        CASE_DEFAULT
        SET_VALUE(LW(0), 150)
        SET_VALUE(LW(1), 12)
        SET_VALUE(LW(2), 30)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 11, LW(0), 178, LW(1), LW(2), 12, 37, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetGaugeParam, 36, 72, 100, 100)
    CALL(btlevtcmd_AcSetFlag, 64)
    CALL(btlevtcmd_SetupAC, -2, 6, 1, 0)
    WAIT_FRAMES(22)
    CALL(btlevtcmd_StartAC, 1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
        CALL(btlevtcmd_GetStatusMg, -2, LW(6))
        CALL(btlevtcmd_GetStatusMg, LW(3), LW(7))
        MULTIPLY_FLOAT(LW(6), 12)
        MULTIPLY_FLOAT(LW(7), 7)
        CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(7))
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 235, -1, 0)
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    END_SIBLING_EVENT
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_AcGetOutputParam, 2, LW(5))
    SWITCH(LW(5))
        CASE_GREATER_OR_EQUAL(10)
        SET_VALUE(LW(5), 2)
        CASE_GREATER_OR_EQUAL(7)
        SET_VALUE(LW(5), 1)
        CASE_GREATER_OR_EQUAL(4)
        SET_VALUE(LW(5), 0)
        CASE_DEFAULT
        SET_VALUE(LW(5), -1)
    END_SWITCH
    IF_INT_GREATER_OR_EQUAL(LW(5), 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetResultPrizeLv, -5, LW(5), LW(6))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
    ELSE
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(6))
    CALL(btlevtcmd_StopAC)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_K_1"))
    WAIT_FRAMES(6)
    IF_INT_GREATER_OR_EQUAL(LW(5), 0)
        CALL_CHILD(unk_evt_803510ec)
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 59)
        END_IF
    END_IF
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    MULTIPLY_FLOAT(LW(6), 16)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(6))
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    MULTIPLY_FLOAT(LW(6), 40)
    ADD(LW(1), LW(6))
    CALL(btlevtcmd_GetFaceDirection, LW(3), LW(6))
    MULTIPLY(LW(6), 45)
    CALL(evt_eff, 0, STRING("kiss"), 0, LW(0), LW(1), LW(2), LW(6), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CHURINA_KISS1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(40)
    CALL(btlevtcmd_GetPartsPos, LW(3), LW(4), LW(12), LW(13), LW(14))
    CALL(btlevtcmd_GetHeight, LW(3), LW(5))
    CALL(btlevtcmd_GetStatusMg, LW(3), LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    ADD(LW(13), LW(5))
    CALL(btlevtcmd_AcGetOutputParam, 2, LW(5))
    CALL(evt_eff, STRING("eff"), STRING("recovery"), 0, LW(12), LW(13), LW(14), LW(5), 0, 0, 0, 0, 0, 0, 0)
    BEGIN_INLINE_EVENT_GET_ID(LW(15))
        WAIT_FRAMES(120)
        CALL(btlevtcmd_RecoverHp, LW(3), LW(4), LW(5))
    END_INLINE_EVENT
    CALL(btlevtcmd_ACRStart, -2, 0, 15, 15, 20)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    SWITCH(LW(6))
        CASE_GREATER_OR_EQUAL(2)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_Y_1"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        WAIT_MS(300)
        CASE_DEFAULT
        CALL(evt_audience_acrobat_notry)
    END_SWITCH
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL_CHILD(unk_evt_803510ec)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_ResetFaceDirection, LW(3))
        CALL(btlevtcmd_StartWaitEvent, LW(3))
    END_IF
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(chuchurina_appeal_event)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_APPEAL1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_O_3"))
    CALL(btlevtcmd_AudienceAppeal, -2)
    CALL(btlevtcmd_ftomsec, 45, LW(0))
    WAIT_MS(LW(0))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    ADD(LW(1), 30)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(14))
    MULTIPLY(LW(14), 45)
    CALL(evt_eff, STRING(""), STRING("kiss"), 0, LW(0), LW(1), LW(2), LW(14), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CHURINA_KISS1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(party_win_reaction)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_Y_1"))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    WAIT_MS(333)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    RETURN
EVT_END()

static EVT_BEGIN(attack_audience_event)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStageSize, LW(3), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(4))
    SET_VALUE(LW(8), LW(3))
    MULTIPLY(LW(8), LW(4))
    MULTIPLY(LW(8), -1)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_MovePosition, -2, LW(8), LW(1), LW(2), 0, -1, 0)
    WAIT_FRAMES(20)
    CALL(_get_attack_aud_no, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStageAudienceAreaSize, LW(4), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
    SET_VALUE(LW(6), LW(4))
    MULTIPLY(LW(6), LW(5))
    MULTIPLY(LW(6), -1)
    CALL(btlevtcmd_SetPos, -2, LW(6), LW(1), LW(2))
    CALL(evt_btl_camera_set_mode, 0, 13)
    CALL(evt_btl_camera_set_homing_unit_audience, 0, -2, 1, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 100)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 24)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_A4_1"))
    CALL(btlevtcmd_ftof, 52, LW(0))
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PCH_Y_1"))
    CALL(btlevtcmd_ftof, 7, LW(0))
    WAIT_FRAMES(LW(0))
    CALL(_attack_aud, LW(3), 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_MS(1500)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_MS(1000)
    END_SIBLING_EVENT
    CALL(btlevtcmd_ftof, 35, LW(0))
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(6), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(8), LW(1), LW(2))
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// TODO: type
static u32 defence[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x64646464, 0x64646464, 0x64646464, 0x64646464,
    0x64646464, 0x64640000,
};

static EVT_BEGIN(attack_event)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()


static EVT_BEGIN(main_evt)
    CALL(mono_main)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off

__declspec(section ".sdata2") u32 dat_8041ddb8 = 0xFFFFFF00;
