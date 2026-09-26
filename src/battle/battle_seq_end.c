#include "battle/battle_seq_end.h"
#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "battle/battle_stage.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "mario/mario_pouch.h"
#include "system.h"
#include <string.h>
#include "battle/battle_sub.h"

// Prototypes for functions that are not declared in a shared header.
BattleSpotWork* BatSpotGetPtr(void);
void BattleAudience_Case_GreatVictory(void);
void BattleAudience_Case_FastVictory(f32 value);
void BattleAudienceSoundWhistle(void);
BattleUnit* BtlUnit_GetGuardKouraPtr(BattleUnit* unit);
void mapGrpFlagOn(const char* name, u32 flag);
void FontDrawString(const char* str, f32 x, f32 y);
void FontDrawMessage(s32 x, s32 y, const char* msg);
extern int sprintf(char* str, const char* format, ...);

void _GetExpIcon_Main(BattleWork* wp);
void _LvupParamConfirmDisp(CameraId camId, void* param);
void L__LvupParamHelpMsgDisp(CameraId camId, void* param);
void _lvup_select_object_disp(CameraId camId, void* param);
void btlseqEnd_DispMain(CameraId camId, BattleWork* wp);
void effPokopiPchargeN64Entry(f32 x, f32 y, f32 z, f32 scale, s32 a, s32 b);

extern s32 btlevtcmd_RunDataEventChild[];
s32 _check_guard_koura(EventEntry* event, BOOL isFirstCall);

//.data
RankUpData _rank_up_data[] = {
    {1, 0, "msg_menu_mario_name_0", "butai_01"},
    {10, 1, "msg_menu_mario_name_1", "butai_01"},
    {20, 2, "msg_menu_mario_name_2", "butai_02"},
    {30, 3, "msg_menu_mario_name_3", "butai_03"},
    {0xFFFF, 0, NULL, NULL},
};

s32 _koura_delete_event[] = {
    0x0003005B, (s32)_check_guard_koura, 0xFFFFFFFD, 0xFE363C8A,
    0x00020019, 0xFE363C8A, 0xFFFFFFFF, 0x0003005B,
    (s32)btlevtcmd_RunDataEventChild, 0xFE363C8A, 0x0000003E,
    0x00000021, 0x00000002, 0x00000001,
};

#pragma dont_inline on
RankUpData* _get_rank_data(s32 level) {
    RankUpData* rank;

    for (rank = _rank_up_data; level != rank->startLevel; rank++) {
        if (!rank->rankUpMessage) {
            return NULL;
        }
    }
    return rank;
}
#pragma dont_inline reset

const char* BattleGetRankNameLabel(s32 level) {
    RankUpData* rank = _rank_up_data;
    RankUpData* found = NULL;

    while (level >= rank->startLevel) {
        if (rank->startLevel < 0) {
            break;
        }
        found = rank;
        rank++;
    }
    if (found == NULL) {
        return NULL;
    }
    return found->rankUpName;
}

s32 _keta_get(s32 value) {
    s32 digits = 0;

    if (value < 0) {
        value *= -1;
    }
    if (value == 0) {
        return 1;
    }
    while (value > 0) {
        value /= 10;
        digits++;
    }
    return digits;
}

s32 _get_rank_up_msg(EventEntry* event, BOOL isFirstCall) {
    s32 idx = event->args[0];
    PouchData* pouch = pouchGetPtr();
    RankUpData* rank = _get_rank_data(pouch->level);

    evtSetValue(event, idx, (s32)rank->rankUpMessage);
    return 2;
}

s32 _check_guard_koura(EventEntry* event, BOOL isFirstCall) {
    EventEntry* e = event;
    BattleWork* wp = _battleWorkPointer;
    s32* args = e->args;
    s32 type = evtGetValue(e, args[0]);
    s32 outIdx = args[1];
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(e, type));

    if (!(unit->mAttributeFlags & 0x20)) {
        evtSetValue(e, outIdx, -1);
        return 2;
    }
    {
        BattleUnit* koura = BtlUnit_GetGuardKouraPtr(unit);

        if (koura == NULL) {
            evtSetValue(e, outIdx, -1);
            return 2;
        }
        evtSetValue(e, outIdx, koura->unitId);
    }
    return 2;
}

void _audience_Whistle_control(void) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;

    if (work == NULL) {
        return;
    }
    if (*(s32*)((u8*)work + 0x0) < 0x0700001C) {
        if (*(s32*)((u8*)work + 0x0) >= 0x07000007) {
            if (--*(s32*)((u8*)work + 0x2A0) < 0) {
                *(s32*)((u8*)work + 0x2A0) = irand(0x78) + 0x3C;
                BattleAudienceSoundWhistle();
            }
        }
    }
}

// NONMATCHING (96.54%): only the intplGetValue 0.0f float-constant load is scheduled after lwz r4/r5;
// the original loads it first.
s32 _lvup_spot_off(EventEntry* event, BOOL isFirstCall) {
    BattleSpotWork* spot = BatSpotGetPtr();
    s32 i;
    s32* args = event->args;

    if (isFirstCall) {
        event->userdata[0] = evtGetValue(event, args[0]);
        event->userdata[1] = event->userdata[0];
        event->userdata[2] = *((u8*)spot->entries + 0x2B);
    }
    event->userdata[1]--;
    for (i = 0; i < 5; i++) {
        BattleSpotEntry* entry = &spot->entries[i];
        GXColor color = {0xFF, 0xFF, 0xFF, 0x00};

        ((u8*)&color)[3] = (s32)intplGetValue(2, event->userdata[1], event->userdata[0], 0.0f, (f32)event->userdata[2]);
        entry->field_28 = *(u32*)&color;
        if (((u8*)&entry->field_28)[3] == 0) {
            entry->flags &= ~2;
        }
    }
    return (event->userdata[1] > 0) ? 0 : 2;
}

// NONMATCHING (98.19%): rebuilt from the asm. Body matches; only the .rodata constant base/offsets differ.
s32 _lvup_spot_on(EventEntry* event, BOOL isFirstCall) {
    BattleSpotWork* spot = BatSpotGetPtr();
    BattleSpotEntry* entry;
    s32 code;

    s32* args = event->args;
    spot->skipRender = TRUE;
    code = evtGetValue(event, args[0]);
    entry = &spot->entries[code];

    switch (code) {
    case 0:
        entry->field_4 = (Vec){25.0f * (f32)code + -50.0f, 25.0f, -50.0f};
        break;
    case 1:
        entry->field_4 = (Vec){25.0f * (f32)code + -50.0f, -10.0f, -50.0f};
        break;
    case 2:
        entry->field_4 = (Vec){25.0f * (f32)code + -50.0f, 25.0f, -50.0f};
        break;
    case 3:
        entry->field_4 = (Vec){25.0f * (f32)code + -50.0f, -10.0f, -50.0f};
        break;
    case 4:
        entry->field_4 = (Vec){25.0f * (f32)code + -50.0f, 25.0f, -50.0f};
        break;
    }
    entry->field_10 = (Vec){0.0f, 0.0f, 0.0f};
    entry->field_1C = (Vec){15.0f, 15.0f, 150.0f};
    entry->field_28 = 0xFFFFFFFF;
    entry->flags |= 2;
    return 2;
}

// NONMATCHING (99.76%): instruction stream matches; the only difference is the .rodata constant base and
// offsets (orig bases on str_802FDDF0 + 0x1f0), which depend on the whole file's data layout.
s32 _lvup_select_object_init(EventEntry* event, BOOL isFirstCall) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    PouchData* pouch = pouchGetPtr();

    *(Vec*)((u8*)work + 0x200) = (Vec){-80.0f, 105.0f, 0.0f};
    *(Vec*)((u8*)work + 0x20C) = (Vec){0.5f, 0.5f, 1.0f};
    *(Vec*)((u8*)work + 0x234) = (Vec){0.0f, 105.0f, 0.0f};
    *(Vec*)((u8*)work + 0x240) = (Vec){0.5f, 0.5f, 1.0f};
    *(Vec*)((u8*)work + 0x268) = (Vec){80.0f, 105.0f, 0.0f};
    *(Vec*)((u8*)work + 0x274) = (Vec){0.5f, 0.5f, 1.0f};

    ((u8*)work)[0x228] = 0;
    *(GXColor*)((u8*)work + 0x230) = (GXColor){0xFF, 0xFF, 0xFF, 0xC8};
    *(s16*)((u8*)work + 0x224) = pouch->baseMaxHP;
    *(Vec*)((u8*)work + 0x218) = (Vec){0.0f, 0.0f, 0.0f};
    if (*(u16*)((u8*)work + 0x224) >= 0xC8) {
        *(u16*)((u8*)work + 0x226) = *(u16*)((u8*)work + 0x224);
    } else {
        *(u16*)((u8*)work + 0x226) = *(u16*)((u8*)work + 0x224) + 5;
    }

    ((u8*)work)[0x25C] = 0;
    *(GXColor*)((u8*)work + 0x264) = (GXColor){0xFF, 0xFF, 0xFF, 0xC8};
    *(s16*)((u8*)work + 0x258) = pouch->baseMaxFP;
    *(Vec*)((u8*)work + 0x24C) = (Vec){0.0f, 0.0f, 0.0f};
    if (*(u16*)((u8*)work + 0x258) >= 0xC8) {
        *(u16*)((u8*)work + 0x25A) = *(u16*)((u8*)work + 0x258);
    } else {
        *(u16*)((u8*)work + 0x25A) = *(u16*)((u8*)work + 0x258) + 5;
    }

    ((u8*)work)[0x290] = 0;
    *(GXColor*)((u8*)work + 0x298) = (GXColor){0xFF, 0xFF, 0xFF, 0xC8};
    *(s16*)((u8*)work + 0x28C) = pouch->maximumBP;
    *(Vec*)((u8*)work + 0x280) = (Vec){0.0f, 0.0f, 0.0f};
    if (*(u16*)((u8*)work + 0x28C) >= 99) {
        *(u16*)((u8*)work + 0x28E) = *(u16*)((u8*)work + 0x28C);
    } else {
        *(u16*)((u8*)work + 0x28E) = *(u16*)((u8*)work + 0x28C) + 3;
    }

    *(u32*)((u8*)work + 0x30) |= 8;
    return 2;
}

// NONMATCHING (98.32%): mwcc schedules the lfs f2 (2160.0f) load after lwz r4/r5; the original loads
// lfs f1, 0.0f then lfs f2, 2160.0f before loading the int args.
s32 _lvup_object_select_move(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;
    BattleSeqEndWork* work = wp->seqEndWork;
    s32 selected = *(s32*)((u8*)work + 0x10);
    BattleUnit* mario = BattleGetMarioPtr(wp);
    u8* entry = (u8*)work + 0x200 + selected * 0x34;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = 0x3C;
        event->userdata[2] = (s32)*(f32*)(entry + 0x0);
        event->field_84 = (s32)*(f32*)(entry + 0x8);
        *(s32*)((u8*)event + 0x88) = (s32)mario->mPosition.x;
        *(s32*)((u8*)event + 0x8C) = (s32)mario->mPosition.z;
    }
    event->userdata[0]++;
    if (event->userdata[0] < event->userdata[1]) {
        *(f32*)(entry + 0x0) = intplGetValue(5, event->userdata[0], event->userdata[1], (f32)(s32)event->userdata[2], (f32)*(s32*)((u8*)event + 0x88));
        *(f32*)(entry + 0x8) = intplGetValue(5, event->userdata[0], event->userdata[1], (f32)(s32)event->field_84, (f32)*(s32*)((u8*)event + 0x8C));
        *(f32*)(entry + 0x1C) = reviseAngle(intplGetValue(5, event->userdata[0], event->userdata[1], 0.0f, 2160.0f));
        return 0;
    }
    *(f32*)(entry + 0x0) = (f32)*(s32*)((u8*)event + 0x88);
    *(f32*)(entry + 0x8) = (f32)*(s32*)((u8*)event + 0x8C);
    return 2;
}

s32 _lvup_object_decide_bound(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;
    BattleSeqEndWork* work = wp->seqEndWork;
    s32 selected = *(s32*)((u8*)work + 0x10);
    u8* entry = (u8*)work + 0x200 + selected * 0x34;
    s32 half;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = (s32)*(f32*)(entry + 0x4);
        event->userdata[2] = 0x10;
        event->field_84 = 0x1E;
        entry[0x28] = 2;
    }
    event->userdata[0]++;
    if (event->userdata[0] < event->userdata[2]) {
        half = event->userdata[2] / 2;
        if (event->userdata[0] < half) {
            *(f32*)(entry + 0x4) = (f32)event->userdata[1] + intplGetValue(5, event->userdata[0], half, 0.0f, (f32)(s32)event->field_84);
        } else {
            *(f32*)(entry + 0x4) = (f32)event->userdata[1] + intplGetValue(5, event->userdata[0] - half, half, (f32)(s32)event->field_84, 0.0f);
        }
        return 0;
    }
    *(f32*)(entry + 0x4) = (f32)event->userdata[1];
    entry[0x28] = 4;
    effPokopiPchargeN64Entry(*(f32*)(entry + 0x0), *(f32*)(entry + 0x4), *(f32*)(entry + 0x8), 1.5f, 3, 0x78);
    return 2;
}

s32 _lvup_object_noselect_delete(EventEntry* event, BOOL isFirstCall) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    s32 selected = *(s32*)((u8*)work + 0x10);
    s32 i;

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[2] = 0x1E;
        for (i = 0; i < *(s32*)((u8*)work + 0x18); i++) {
            if (i != selected) {
                ((u8*)work)[i * 0x34 + 0x228] = 3;
            }
        }
    }
    event->userdata[0]++;
    if (event->userdata[0] < event->userdata[2]) {
        for (i = 0; i < *(s32*)((u8*)work + 0x18); i++) {
            if (i != selected) {
                ((u8*)work)[i * 0x34 + 0x233] = (u8)(s32)intplGetValue(5, event->userdata[0], event->userdata[2], 255.0f, 0.0f);
            }
        }
        return 0;
    }
    for (i = 0; i < *(s32*)((u8*)work + 0x18); i++) {
        if (i != selected) {
            ((u8*)work)[i * 0x34 + 0x233] = 0;
        }
    }
    return 2;
}

s32 _disable_enemy_unit_disp(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;
    s32 i;

    for (i = 0; i < 0x40; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);
        BattleUnitType type;

        if (unit == NULL) {
            continue;
        }
        if ((u32)unit->flags & 0x80000000) {
            continue;
        }
        type = unit->currentKind;
        if (type == 0xDC) {
            continue;
        }
        if (type == 0xDE) {
            continue;
        }
        if (type >= 0xE0 && type < 0xE7 && unit->mAlliance == 0 && type != 0xE5) {
            continue;
        }
        {
            void* data = BtlUnit_GetData(unit, 0x42);

            if (data != NULL) {
                EventEntry* e = evtEntry(data, 0xA, 0);
                e->unitId = unit->unitId;
            } else if (unit->currentKind != 0xE5) {
                unit->mAttributeFlags |= 0x21000000;
            }
        }
    }
    {
        BattleStageObject* obj = BattleSearchObjectPtr("if_body");

        if (obj != NULL) {
            *(u8*)((u8*)wp + 0x180DC) |= 0x20;
            obj->mFlags |= 1;
            mapGrpFlagOn("if_body", 1);
            mapGrpFlagOn("if_wire", 1);
        }
    }
    return 2;
}

void _check_audience_battle_end_exec(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleGroupSetup* group = wp->fbatInfo->setup->groupSetup;
    s32 firstLevel = group->enemyData[0].mUnitKindParams->mLevel;
    s32 turn;
    s32 sum;
    s32 i;
    s32 level;
    BattleUnitSetup* entry;
    PouchData* pouch;

    if (firstLevel > (pouch = pouchGetPtr())->level + 1) {
        BattleAudience_Case_GreatVictory();
    }
    turn = wp->turnCount;
    pouch = pouchGetPtr();
    sum = 0;
    level = pouch->level + 1;
    entry = group->enemyData;
    for (i = 0; i < group->numEnemies; i++, entry++) {
        sum += entry->mUnitKindParams->mLevel;
    }
    level = sum * 2 / level / 3;
    if (turn < level) {
        BattleAudience_Case_FastVictory((f32)(level - turn));
    }
}

void _ExecAllUnitBattleEndEvent(void) {
    BattleWork* wp;
    s32 i;

    wp = _battleWorkPointer;
    for (i = 0; i < 0x40; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);

        if (unit != NULL) {
            void* data = BtlUnit_GetData(unit, 0x3F);

            if (data != NULL) {
                EventEntry* event = evtEntry(data, 0xA, 0);
                event->unitId = unit->unitId;
            }
        }
    }
}

s32 _lvup_ap_recover_full(void) {
    BattleAudience* audience = BattleAudienceBaseGetPtr();

    audience->excited = TRUE;
    pouchSetAP(pouchGetMaxAP());
    return 2;
}

s32 _nozzle_set_offset(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    f32 offset = evtGetFloat(event, args[1]);
    BattleStageObject* obj = BattleSearchObjectPtr((const char*)id);

    if (obj != NULL) {
        obj->field_0x10.y = offset;
    }
    return 2;
}

void _BattleMarioKouraDelete(void) {
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    EventEntry* event = evtEntry(_koura_delete_event, 0xA, 0);

    event->unitId = mario->unitId;
}

void _GetExpIcon_End(BattleSeqEndWork* work) {
    s32 i;

    *(u32*)((u8*)work + 0x30) &= ~2;
    for (i = 0; i < 0x13; i++) {
        iconDelete((const char*)(*(u32*)((u8*)work + 0x34 + i * 4) + 0x18));
    }
}

// NONMATCHING (93.40%): first-loop float conversion order differs and mwcc folds the original's `y + 0.0f`
// (orig keeps an `fadds f2, y, float_0`) although the value is unchanged.
void _GetExpIcon_Main(BattleWork* wp) {
    BattleSeqEndWork* work = wp->seqEndWork;
    s32 exp = *(s16*)((u8*)work + 0xA);
    s32 i;

    for (i = 0; i < exp / 10; i++) {
        iconFlagOff((const char*)(*(u32*)((u8*)work + 0x34 + i * 4) + 0x18), 2);
        if (*(s32*)((u8*)work + 0x80 + i * 4) <= 0) {
            *(s32*)((u8*)work + 0x80 + i * 4) = 0x40;
        } else {
            *(s32*)((u8*)work + 0x80 + i * 4) += 0x40;
            if (*(s32*)((u8*)work + 0x80 + i * 4) > 0xFF) {
                *(s32*)((u8*)work + 0x80 + i * 4) = 0xFF;
            }
        }
        if (*(s32*)((u8*)work + 0xCC + i * 4) < 0x10) {
            *(s32*)((u8*)work + 0xCC + i * 4) += 1;
        }
        *(f32*)((u8*)work + 0x118 + i * 0xC) =
            (24.0f - 18.0f * (f32)i) + 36.0f * (f32)(*(s16*)((u8*)work + 0xA) / 10);
    }
    for (; i < 10; i++) {
        iconFlagOn((const char*)(*(u32*)((u8*)work + 0x34 + i * 4) + 0x18), 2);
        *(s32*)((u8*)work + 0x80 + i * 4) = 0;
        *(s32*)((u8*)work + 0xCC + i * 4) = 0;
    }
    {
        s32 rem = exp % 10;
        s32 j;

        for (j = 10; j - 10 < rem; j++) {
            iconFlagOff((const char*)(*(u32*)((u8*)work + 0x34 + j * 4) + 0x18), 2);
            if (*(s32*)((u8*)work + 0x80 + j * 4) <= 0) {
                *(s32*)((u8*)work + 0x80 + j * 4) = 0x40;
            } else {
                *(s32*)((u8*)work + 0x80 + j * 4) += 0x40;
                if (*(s32*)((u8*)work + 0x80 + j * 4) > 0xFF) {
                    *(s32*)((u8*)work + 0x80 + j * 4) = 0xFF;
                }
            }
        }
        for (; j < 0x13; j++) {
            iconFlagOn((const char*)(*(u32*)((u8*)work + 0x34 + j * 4) + 0x18), 2);
            *(s32*)((u8*)work + 0x80 + j * 4) = 0;
        }
    }
    for (i = 0; i < 0x13; i++) {
        iconSetAlpha((const char*)(*(u32*)((u8*)work + 0x34 + i * 4) + 0x18),
                     (u8)*(s32*)((u8*)work + 0x80 + i * 4));
        iconSetPos((const char*)(*(u32*)((u8*)work + 0x34 + i * 4) + 0x18),
                   *(f32*)((u8*)work + 0x118 + i * 0xC), 0.0f + *(f32*)((u8*)work + 0x11C + i * 0xC),
                   *(f32*)((u8*)work + 0x120 + i * 0xC));
    }
}

// NONMATCHING (93.80%): second-loop offsets (orig preloads li 0x28 / li 0x78 and increments, ours
// computes i*4 / i*0xc) and some register naming differ.
void _GetExpIcon_Init(BattleSeqEndWork* arg) {
    BattleSeqEndWork* work = arg;
    s32 i;
    char name[0x10];

    for (i = 0; i < 10; i++) {
        sprintf(name, "_Bgsp%d", i);
        iconEntry2D(name, 0x18C);
        *(u32*)((u8*)work + 0x34 + i * 4) = (u32)iconNameToPtr(name);
        iconSetScale(name, 0.9f);
        iconFlagOn(name, 2);
        *(s32*)((u8*)work + 0xCC + i * 4) = 0;
        *(f32*)((u8*)work + 0x118 + i * 0xC) =
            36.0f * (f32)i + (24.0f - 18.0f * (f32)(*(s16*)((u8*)work + 0xA) / 10));
        *(f32*)((u8*)work + 0x11C + i * 0xC) = 25.0f;
        *(f32*)((u8*)work + 0x120 + i * 0xC) = 0.0f;
        iconSetPos(name, *(f32*)((u8*)work + 0x118 + i * 0xC),
                   *(f32*)((u8*)work + 0x11C + i * 0xC), *(f32*)((u8*)work + 0x120 + i * 0xC));
    }
    for (; i < 0x13; i++) {
        sprintf(name, "_Bgsp%d", i);
        iconEntry2D(name, 0x18C);
        *(u32*)((u8*)work + 0x34 + i * 4) = (u32)iconNameToPtr(name);
        iconSetScale(name, 0.45f);
        iconFlagOn(name, 2);
        *(s32*)((u8*)work + 0xCC + i * 4) = 0;
        *(f32*)((u8*)work + 0x118 + i * 0xC) = 86.9f - 16.2f * (f32)(i - 9);
        *(f32*)((u8*)work + 0x11C + i * 0xC) = 0.0f;
        *(f32*)((u8*)work + 0x120 + i * 0xC) = 0.0f;
        iconSetPos(name, *(f32*)((u8*)work + 0x118 + i * 0xC),
                   *(f32*)((u8*)work + 0x11C + i * 0xC), *(f32*)((u8*)work + 0x120 + i * 0xC));
    }
    *(u32*)((u8*)work + 0x30) |= 2;
}

s32 _lvup_disp_flag_onoff(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    s32 flags = evtGetValue(event, args[0]);
    u32 mask = args[1];

    if (flags != 0) {
        *(u32*)((u8*)work + 0x30) = *(u32*)((u8*)work + 0x30) | mask;
    } else {
        *(u32*)((u8*)work + 0x30) = *(u32*)((u8*)work + 0x30) & ~mask;
    }
    return 2;
}

s32 _lvup_obj_set_color(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    u8* obj = (u8*)work + 0x200 + evtGetValue(event, args[0]) * 0x34;

    obj[0x30] = evtGetValue(event, args[1]);
    obj[0x31] = evtGetValue(event, args[2]);
    obj[0x32] = evtGetValue(event, args[3]);
    obj[0x33] = evtGetValue(event, args[4]);
    return 2;
}

s32 _lvup_select_obj_lvup_param_disp(EventEntry* event, BOOL isFirstCall) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    s32 i;

    for (i = 0; i < 3; i++) {
        ((u8*)work)[i * 0x34 + 0x228] = 1;
    }
    return 2;
}

s32 _lvup_select_obj_disp_on(EventEntry* event, BOOL isFirstCall) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;

    *(u32*)((u8*)work + 0x30) |= 8;
    return 2;
}

s32 _set_confetti_control_evtid(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 val = args[0];
    BattleSeqEndWork* work = wp->seqEndWork;

    *(s32*)((u8*)work + 0x2A4) = evtGetValue(event, val);
    return 2;
}

void _GetExpIcon_Disp(CameraId camId, void* param) {
    GXColor color = {0xFF, 0xFF, 0xFF, 0x80};

    windowDispGX_Waku_col(0, color, -76.9f, 21.0f, 165.8f, 24.2f, 10.0f);
}

// NONMATCHING (68.47%): rebuilt from the asm; logic is complete. The switch, pouch checks and message
// selection match. Remaining differences are int->float conversion codegen (the original divides the u16
// width with srawi/addze) and instruction scheduling inside the window computation.
void L__LvupParamHelpMsgDisp(CameraId camId, void* param) {
    BattleWork* wp = param;
    BattleSeqEndWork* work = wp->seqEndWork;
    PouchData* pouch;
    const char* msg;
    u16 lines;
    u16 width;
    f32 x;

    pouch = pouchGetPtr();
    switch (*(s32*)((u8*)work + 0x10)) {
    case 0:
        msg = msgSearch("help_btl_levelup_hp");
        if (pouch->baseMaxHP >= 0xC8) {
            msg = msgSearch("helperr_btl_levelup_hp");
        }
        break;
    case 1:
        msg = msgSearch("help_btl_levelup_fp");
        if (pouch->baseMaxFP >= 0xC8) {
            msg = msgSearch("helperr_btl_levelup_fp");
        }
        break;
    case 2:
        msg = msgSearch("help_btl_levelup_bp");
        if (pouch->maximumBP >= 0x63) {
            msg = msgSearch("helperr_btl_levelup_bp");
        }
        break;
    }
    width = FontGetMessageWidthLine(msg, &lines);
    x = (f32)(-(s32)(width / 2));
    windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, x - 20.0f, -145.0f + (f32)(lines * 29),
                          (f32)(width + 40), (f32)((lines + 1) * 29 + 1), 10.0f);
    lines++;
    FontDrawStart_alpha(0xFF);
    FontDrawMessage((s32)x, -148 + (lines - 1) * 29, msg);
}

// NONMATCHING (56.97%): rebuilt from the asm; logic is complete. Differences: our frame is 0x70 vs the
// original 0x60, so the icon Vec and float-conversion scratch land 0x10 higher; the int->float scheduling
// and a couple of load orderings differ.
void _LvupParamConfirmDisp(CameraId camId, void* param) {
    BattleWork* wp = param;
    BattleSeqEndWork* work = wp->seqEndWork;
    Vec pos;
    char buf[0x14];
    u32 len;

    pouchGetPtr();
    sprintf(buf, msgSearch("btl_msg_lvup_ok"));
    len = strlen(buf);
    windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -30.0f - 6.25f * (f32)len, 30.0f,
                          12.5f * (f32)len + 65.0f, 32.0f, 10.0f);
    windowDispGX_Waku_col(0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF}, -70.0f, -10.0f, 130.0f, 60.0f, 10.0f);
    FontDrawStart();
    FontDrawString(buf, 0.0f - 6.25f * (f32)len, 27.0f);
    FontDrawString(msgSearch("btl_msg_lvup_yes"), -20.0f, -13.0f);
    FontDrawString(msgSearch("btl_msg_lvup_no"), -20.0f, -38.0f);
    pos.x = -40.0f;
    pos.y = (f32)(-50 - *(s32*)((u8*)work + 0x24) * 25);
    pos.z = 0.0f;
    iconDispGx(pos, 0x10, 0x18C, 1.0f);
}

void _MarioExpDisp(CameraId camId, void* param) {
    BattleWork* wp = param;
    BattleSeqEndWork* work = wp->seqEndWork;
    Vec pos;
    GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};
    char buf[8];
    s32 len;

    pouchGetPtr();
    pos.x = 200.0f;
    pos.y = -200.0f;
    pos.z = 0.0f;
    if (work != NULL) {
        sprintf(buf, "%2d", *(s16*)((u8*)work + 0xA));
        len = strlen(buf);
        windowDispGX_Waku_col(0, color, 180.0f, -160.0f, 12.5f * (f32)len + 65.0f, 32.0f, 10.0f);
        iconDispGx(pos, 0x10, 0x18C, 1.0f);
        FontDrawStart();
        FontDrawString(buf, 225.0f, -163.0f);
    }
}

// NONMATCHING (98.87%): callee-saved allocation is swapped: orig camId=r29, param=r31, work=r30; ours
// param=r30, work=r31.
void btlseqEnd_DispMain(CameraId camId, BattleWork* param) {
    BattleSeqEndWork* work = param->seqEndWork;

    if (work == NULL) {
        return;
    }
    if (*(u32*)((u8*)work + 0x30) & 2) {
        _GetExpIcon_Main(param);
        dispEntry(camId, 900.0f, 1, _GetExpIcon_Disp, param);
    }
    if (*(u32*)((u8*)work + 0x30) & 4) {
        dispEntry(camId, 900.0f, 1, _MarioExpDisp, param);
    }
    if (*(u32*)((u8*)work + 0x30) & 8) {
        Vec pos = {0.0f, 0.0f, 0.0f};
        dispEntry(4, dispCalcZ(&pos), 2, _lvup_select_object_disp, param);
    }
    if (*(u32*)((u8*)work + 0x30) & 0x10) {
        dispEntry(camId, 900.0f, 1, _LvupParamConfirmDisp, param);
    }
    if (*(u32*)((u8*)work + 0x30) & 0x20) {
        dispEntry(camId, 900.0f, 1, L__LvupParamHelpMsgDisp, param);
    }
}

void btlseqEnd_DispInit(BattleWork* wp) {
    *(u32*)((u8*)wp->seqEndWork + 0x30) = 0;
}

void _ac_help_disp(CameraId camId, void* param) {

}

void _lvup_select_object_disp(CameraId camId, void* param) {

}
