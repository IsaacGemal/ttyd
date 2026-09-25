#include "battle/battle_seq_end.h"
#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "battle/battle_stage.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/windowdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "mario/mario_pouch.h"
#include "system.h"
#include <string.h>

// Prototypes for functions that are not declared in a shared header.
BattleSpotWork* BatSpotGetPtr(void);
void BattleAudience_Case_GreatVictory(void);
void BattleAudience_Case_FastVictory(f32 value);
void BattleAudienceSoundWhistle(void);
BattleUnit* BtlUnit_GetGuardKouraPtr(BattleUnit* unit);
void mapGrpFlagOn(const char* name, u32 flag);
void FontDrawString(const char* str, f32 x, f32 y);
extern int sprintf(char* str, const char* format, ...);

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

#pragma optimize_for_size on
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
#pragma optimize_for_size off

s32 _get_rank_up_msg(EventEntry* event, BOOL isFirstCall) {
    s32 idx = event->args[0];
    PouchData* pouch = pouchGetPtr();
    RankUpData* rank = _get_rank_data(pouch->level);

    evtSetValue(event, idx, (s32)rank->rankUpMessage);
    return 2;
}

s32 _check_guard_koura(EventEntry* event, BOOL isFirstCall) {
    EventEntry* e = event;
    s32* args = e->args;
    BattleWork* wp = _battleWorkPointer;
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
    if (*(s32*)((u8*)work + 0x0) >= 0x0700001C) {
        return;
    }
    if (*(s32*)((u8*)work + 0x0) >= 0x07000007) {
        if (--*(s32*)((u8*)work + 0x2A0) < 0) {
            *(s32*)((u8*)work + 0x2A0) = irand(0x78) + 0x3C;
            BattleAudienceSoundWhistle();
        }
    } else {
        return;
    }
}

void _ExecAllUnitBattleEndEvent(void) {
    s32 i;

    for (i = 0; i < 0x40; i++) {
        BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, i);

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

s32 _lvup_disp_flag_onoff(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    s32 flags = evtGetValue(event, args[0]);

    if (flags != 0) {
        *(u32*)((u8*)work + 0x30) |= args[1];
    } else {
        *(u32*)((u8*)work + 0x30) &= ~args[1];
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

#pragma optimize_for_size on
s32 _lvup_select_obj_lvup_param_disp(EventEntry* event, BOOL isFirstCall) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;
    s32 i;

    for (i = 0; i < 3; i++) {
        ((u8*)work)[i * 0x34 + 0x228] = 1;
    }
    return 2;
}
#pragma optimize_for_size off

s32 _lvup_select_obj_disp_on(EventEntry* event, BOOL isFirstCall) {
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;

    *(u32*)((u8*)work + 0x30) |= 8;
    return 2;
}

#pragma optimize_for_size on
s32 _set_confetti_control_evtid(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    BattleSeqEndWork* work = _battleWorkPointer->seqEndWork;

    *(s32*)((u8*)work + 0x2A4) = evtGetValue(event, args[0]);
    return 2;
}
#pragma optimize_for_size off

void _GetExpIcon_Disp(CameraId camId, void* param) {
    GXColor color = {0xFF, 0xFF, 0xFF, 0x80};

    windowDispGX_Waku_col(0, color, -76.9f, 21.0f, 165.8f, 24.2f, 10.0f);
}

void _MarioExpDisp(CameraId camId, void* param) {
    BattleWork* wp = param;
    BattleSeqEndWork* work = wp->seqEndWork;
    Vec pos;
    char buf[16];
    s32 len;

    pouchGetPtr();
    pos.x = 200.0f;
    pos.y = -200.0f;
    pos.z = 0.0f;
    if (work != NULL) {
        GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};

        sprintf(buf, "%2d", *(s16*)((u8*)work + 0xA));
        len = strlen(buf);
        windowDispGX_Waku_col(0, color, 180.0f, -160.0f, 12.5f * (f32)len + 65.0f, 32.0f, 10.0f);
        iconDispGx(pos, 0x10, 0x18C, 1.0f);
        FontDrawStart();
        FontDrawString(buf, 225.0f, -163.0f);
    }
}

void btlseqEnd_DispInit(BattleWork* wp) {
    *(u32*)((u8*)wp->seqEndWork + 0x30) = 0;
}

void _ac_help_disp(CameraId camId, void* param) {

}

void _lvup_select_object_disp(CameraId camId, void* param) {

}
