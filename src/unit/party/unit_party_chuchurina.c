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

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_8037EC10[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };

USER_FUNC(_chuchu_make_extra_work_area) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    *(void**)((u8*)unit + 0x310) = BattleAlloc(0xC0);
    return EVT_RETURN_DONE;
}

extern BattleWeapon partyWeapon_ChuchurinaNormalAttackLeftLast;
extern BattleWeapon partyWeapon_ChuchurinaItemSteal;
extern BattleWeapon partyWeapon_ChuchurinaMadowaseAttack;
extern BattleWeapon partyWeapon_ChuchurinaKiss;

#define TECH_MENU_ENTRY(table)                                                        \
    cmd->mWeaponTable[*count].mIndex = -1;                                            \
    cmd->mWeaponTable[*count].mItem = 0;                                              \
    cmd->mWeaponTable[*count].mWeaponParams = (BattleWeapon*)(table);                 \
    cmd->mWeaponTable[*count].field_0x4 = 0;                                          \
    cmd->mWeaponTable[*count].mIcon =                                                 \
        *(u16*)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 4);                    \
    cmd->mWeaponTable[*count].mName =                                                 \
        (char*)msgSearch(*(char**)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 0)); \
    (*count)++;

// The original addresses the four weapons from one base register (addi r30, r5, lbl_8037EC10@l, then
// +0x6A0/+0x8E0/+0x820/+0x9A0); naming each weapon directly does not reproduce that.
static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count) {
#ifdef MUST_MATCH
    u8* const weaponBase = lbl_8037EC10;
#endif
    s32 techLv = partyGetTechLv(
        BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

#ifdef MUST_MATCH
    TECH_MENU_ENTRY(weaponBase + 0x6A0);  // partyWeapon_ChuchurinaNormalAttackLeftLast
    if (techLv >= 0) {
        TECH_MENU_ENTRY(weaponBase + 0x8E0); // partyWeapon_ChuchurinaItemSteal
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(weaponBase + 0x820); // partyWeapon_ChuchurinaMadowaseAttack
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(weaponBase + 0x9A0); // partyWeapon_ChuchurinaKiss
    }
#else
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
#endif
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
    area = *(BattleWeapon**)((u8*)unit + 0x310);
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
    chance = 0x50 - ((u8)work->actionCommands.mAcDifficulty - 3) * 0x14;
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
// (r27). Carrying both in one reused s32 gives MWCC the target colouring; the natural
// u16 local is under #else.
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
            while ((pick -= list->holdWeight) >= 0) {
                list++;
            }
            return list->type;
        } else {
            return 0;
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

// The target keeps the stolen item in the same register as the event arg id (r27). The
// reused-s32 form under MUST_MATCH colours event r26 and the args/id chain r28, but still
// splits the item to r30; the natural u16 form (#else) scores 94.04% with 7 saved regs.
// Decl/statement order and opt_lifetimes/opt_propagation/dead-assignment/optimize_for_size
// toggles, a list-parameter helper and a single-write helper were all tried.
// NONMATCHING (94.84%): callee-saved map only (target event r26, args/id/item/list r27,
// unit r28, out r29, work r30, flag r31).
#ifdef MUST_MATCH
USER_FUNC(_chuchu_item_steal) {
    s32 out;
    s32 id;
    s32 flag;
    s32* args;
    BattleUnit* unit;
    BattleWork* work;

    work = _battleWorkPointer;
    args = event->args;
    id = evtGetValue(event, args[0]);
    out = args[1];
    flag = evtGetValue(event, args[2]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, id));
    id = (u16)unit->heldItem;
    if ((u16)id == 0) {
        id = _chuchu_pick_drop(unit);
        if ((u16)id == 0) {
            id = 0x79;
        }
    }
#else
// NONMATCHING (94.84%): natural form; see the note above (callee-saved map only).
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
    if (pouchGetItem((u16)id) == 0) {
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
            work->fbatInfo->setup->heldItems[unit->groupSetupId] = 0;
            work->fbatInfo->setup->stolenItems[unit->groupSetupId] = 0;
        }
    } else if (unit->groupSetupId >= 0) {
        work->fbatInfo->setup->heldItems[unit->groupSetupId] = 0;
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
#ifdef MUST_MATCH
    evtSetValue(event, out, (u16)id);
#else
    evtSetValue(event, out, item);
#endif
    return EVT_RETURN_DONE;
}

// .sbss:0x0 | 0x80418A28
s32 mono_alpha;

void mono_capture(CameraId cameraId, void* param) {
    BattleUnit* unit = param;
    BattleUnitPart* parts = BtlUnit_GetPartsPtr(unit, 1);

    *(smartEntry**)((u8*)unit + 0x318) = smartAlloc(GXGetTexBufferSize(0x260, 0x1E0, 1, 0, 0), 1);
    GXSetTexCopySrc(0, 0, 0x260, 0x1E0);
    GXSetTexCopyDst(0x260, 0x1E0, 1, 0);
    GXCopyTex((*(smartEntry**)((u8*)unit + 0x318))->address, 0);
    GXPixModeSync();
    *(u32*)((u8*)parts + 0x204) &= ~0x100;
}

// opt_common_subs on folds the repeated quad vertex constants into registers; the original reloads each.
#pragma opt_common_subs off

// Building the colour in an inline keeps the GXColor temporary in the slot the original uses.
static inline GXColor monoColor(u8 alpha) {
    GXColor color = *(GXColor*)&dat_8041ddb8;
    color.a = alpha;
    return color;
}

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
    entry = *(smartEntry**)((u8*)unit + 0x318);
    tex = NULL;
    if (entry != NULL) {
        tex = entry->address;
    }
    if (tex == NULL) {
        return;
    }
    GXInitTexObj(&texObj, tex, 0x260, 0x1E0, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, float_0_8041ddc0, float_0_8041ddc0, float_0_8041ddc0, GX_FALSE, GX_FALSE, GX_ANISO_1);
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
    GXPosition3f32(float_neg304_8041ddc4, float_240_8041ddc8, float_0_8041ddc0);
    GXTexCoord2f32(float_0_8041ddc0, float_0_8041ddc0);
    GXPosition3f32(float_304_8041ddcc, float_240_8041ddc8, float_0_8041ddc0);
    GXTexCoord2f32(float_1_8041ddd0, float_0_8041ddc0);
    GXPosition3f32(float_304_8041ddcc, float_neg240_8041ddd4, float_0_8041ddc0);
    GXTexCoord2f32(float_1_8041ddd0, float_1_8041ddd0);
    GXPosition3f32(float_neg304_8041ddc4, float_neg240_8041ddd4, float_0_8041ddc0);
    GXTexCoord2f32(float_0_8041ddc0, float_1_8041ddd0);
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
    dispEntry(CAMERA_2D, float_910_8041ddbc, 0, mono_disp, unit);
    return EVT_RETURN_BLOCK;
}

USER_FUNC(mono_capture_event) {
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_GetPartsPtr(unit, 1);
    dispEntry(CAMERA_2D, float_910_8041ddbc, 0, mono_capture, unit);
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
        if (*(smartEntry**)((u8*)unit + 0x318) != NULL) {
            smartFree(*(smartEntry**)((u8*)unit + 0x318));
        }
        *(smartEntry**)((u8*)unit + 0x318) = NULL;
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}
