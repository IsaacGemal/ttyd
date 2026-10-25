#pragma once

typedef struct EventEntry EventEntry;

#include "driver/camdrv.h"

typedef struct BattleWork BattleWork;
typedef struct IconEntry IconEntry;

#include <dolphin/types.h>

typedef struct RankUpData {
    s16 startLevel; //0x0, the level you need to reach to be a certain rank
    s16 rankId; //0x2, the index of this rank for lookups
    const char* rankUpName; //0x4, name of the rank for pause menu
    const char* rankUpMessage; //0x8, text box when you rank up
} RankUpData;

typedef struct EffectEntry EffectEntry;

// Bits in BattleSeqEndWork.dispFlags (0x30): which end-of-battle displays run.
typedef enum BattleSeqEndDisp {
    SEQEND_DISP_EXP_ICONS = 0x2,    // _GetExpIcon_Disp; set by _GetExpIcon_Init, cleared by _GetExpIcon_End
    SEQEND_DISP_MARIO_EXP = 0x4,    // _MarioExpDisp
    SEQEND_DISP_LVUP_OBJECTS = 0x8, // _lvup_select_object_disp
    SEQEND_DISP_LVUP_CONFIRM = 0x10,// _LvupParamConfirmDisp
    SEQEND_DISP_LVUP_HELP = 0x20,   // L__LvupParamHelpMsgDisp
} BattleSeqEndDisp;

// One of the three level-up choices (HP, FP, BP). 0x34 bytes, at BattleSeqEndWork.objs.
typedef struct LvupSelectObj {
    Vec pos;       // 0x00
    Vec scale;     // 0x0C
    Vec rot;       // 0x18, y is the spin angle set by _lvup_object_select_move
    u16 value;     // 0x24, current stat (baseMaxHP, baseMaxFP or maximumBP)
    u16 max;       // 0x26, the stat after this level-up (+5, +3, or unchanged past the cap)
    u8 state;      // 0x28, 1 shown, 2 bouncing, 3 dimmed, 4 chosen, 5 chosen and blinking
    u8 blinkTimer; // 0x29, frames until visible toggles (state 5)
    u8 pad[2];     // 0x2A
    s32 visible;   // 0x2C, drawn when nonzero; state 4 starts blinking it off and on
    GXColor color; // 0x30
} LvupSelectObj;

typedef struct BattleSeqEndWork {
    s32 unk0;                // 0x0, zeroed when created; _audience_Whistle_control compares it with SEQ_END_* steps, nothing sets it
    s32 timer;               // 0x4, frame counter for the current end-of-battle step
    s16 stockExp;            // 0x8, exp earned this battle (BattleGetStockExp; doubled when merleeCurseType == 3, capped at 100)
    s16 expRemaining;        // 0xA, exp not yet turned into star points; the exp icons show this
    s32 lvupPending;         // 0xC, set when 100 star points were earned (a level-up is due)
    s32 lvupSelect;          // 0x10, highlighted level-up choice: 0 HP, 1 FP, 2 BP
    s32 unk14;               // 0x14, zeroed when the level-up starts, never read
    s32 lvupCount;           // 0x18, number of level-up choices (3)
    s32 unk1C;               // 0x1C
    s32 unk20;               // 0x20
    s32 confirmSel;          // 0x24, yes (0) / no (1) on the level-up confirm prompt
    s32 unk28;               // 0x28, zeroed when the confirm prompt opens, never read
    s32 confirmCount;        // 0x2C, choices on the confirm prompt (2)
    u32 dispFlags;           // 0x30, SEQEND_DISP_* bits
    IconEntry* iconPtr[0x13]; // 0x34, icon object for each exp star ("_Bgsp%d"), 19 of them
    s32 iconAlpha[0x13];     // 0x80, alpha ramped up while a star is lit
    s32 iconFrames[0x13];    // 0xCC, counts up to 0x10 while a star is lit
    Vec iconPos[0x13];       // 0x118, star positions
    s32 waitEvtId;           // 0x1FC, event the sequence waits on before its next step
    LvupSelectObj objs[3];   // 0x200
    EffectEntry* effect;     // 0x29C, point effect started from stockExp, deleted when the exp count ends
    s32 whistleTimer;        // 0x2A0, frames until the next audience whistle
    s32 confettiEvtId;       // 0x2A4, event id set by _set_confetti_control_evtid
    s32 rankUpEvtId;         // 0x2A8, event id of the rank-up event
} BattleSeqEndWork;
void _GetExpIcon_Main(BattleWork* wp);
void _GetExpIcon_End(BattleSeqEndWork* work);
void _GetExpIcon_Init(BattleSeqEndWork* arg);
void _LvupParamConfirmDisp(CameraId camId, void* param);
void _MarioExpDisp(CameraId camId, void* param);
void _GetExpIcon_Disp(CameraId camId, void* param);
void L__LvupParamHelpMsgDisp(CameraId camId, void* param);
void _lvup_select_object_disp(CameraId camId, void* param);
void btlseqEnd_DispMain(CameraId camId, BattleWork* wp);
void btlseqEnd_DispInit(BattleWork* wp);
void _audience_Whistle_control(void);
void _check_audience_battle_end_exec(void);
void _ExecAllUnitBattleEndEvent(void);
void _BattleMarioKouraDelete(void);
const char* BattleGetRankNameLabel(s32 level);
s32 _lvup_disp_flag_onoff(EventEntry* event, BOOL isFirstCall);
s32 _lvup_select_obj_disp_on(EventEntry* event, BOOL isFirstCall);
s32 _lvup_select_obj_lvup_param_disp(EventEntry* event, BOOL isFirstCall);
s32 _lvup_object_noselect_delete(EventEntry* event, BOOL isFirstCall);
s32 _lvup_object_select_move(EventEntry* event, BOOL isFirstCall);
s32 _lvup_object_decide_bound(EventEntry* event, BOOL isFirstCall);
s32 _lvup_obj_set_color(EventEntry* event, BOOL isFirstCall);
s32 _get_rank_up_msg(EventEntry* event, BOOL isFirstCall);
s32 _nozzle_set_offset(EventEntry* event, BOOL isFirstCall);
s32 _set_confetti_control_evtid(EventEntry* event, BOOL isFirstCall);
s32 _lvup_spot_on(EventEntry* event, BOOL isFirstCall);
s32 _lvup_spot_move(EventEntry* event, BOOL isFirstCall);
s32 _disable_enemy_unit_disp(EventEntry* event, BOOL isFirstCall);
s32 _lvup_select_object_init(EventEntry* event, BOOL isFirstCall);
s32 _lvup_spot_off(EventEntry* event, BOOL isFirstCall);
s32 _lvup_ap_recover_full(void);
