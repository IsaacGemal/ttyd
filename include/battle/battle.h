#pragma once

#include <dolphin/types.h>
#include "battle/battle_ac.h"
#include "battle/battle_detect.h"
#include "battle/battle_audience.h"
#include "battle/battle_camera.h"
#include "battle/battle_database_common.h"
#include "battle/battle_icon.h"
#include "battle/battle_information.h"
#include "battle/battle_pad.h"
#include "battle/battle_seq_command.h"
#include "battle/battle_stage.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_unit.h"
#include "battle/battle_seq_end.h"
#include "mario/mario_pouch.h"

typedef struct BattleWork BattleWork;
typedef struct BattleWorkCommand BattleWorkCommand;
typedef struct BattleAlliance BattleAlliance;
typedef struct BattleWorkCommandAction BattleWorkCommandAction;
typedef struct BattleWorkCommandWeapon BattleWorkCommandWeapon;
typedef struct BattleWorkCommandOperation BattleWorkCommandOperation;
typedef struct BattleWorkCommandParty BattleWorkCommandParty;
typedef struct BattleWorkCommandMultiItem BattleWorkCommandMultiItem;
typedef struct BattleWorkCommandCursor BattleWorkCommandCursor;
typedef struct BattleWorkCommandWindow BattleWorkCommandWindow;

typedef enum BattleFlags {
    BATTLE_ESCAPED = 0x10,      // set by btlevtcmd_InterruptStop(0), the end of btldefaultevt_Escape
    BATTLE_SEQ_END = 0x80000000,
    BATTLE_MAJINAI_END = 0x400000, // set by BattleMajinaiEndCheck when Merlee has no curse uses left
} BattleFlags;

typedef struct PhaseEvtEntry {
    s32 unitIndex; // 0x0
    s32 value;     // 0x4
} PhaseEvtEntry;

typedef struct BattleAfterReactionWork {
    s32 id;      // 0x0
    s32 field_4; // 0x4
} BattleAfterReactionWork;

typedef enum BattleSequence {
    SEQ_UNKNOWN = 0,
    SEQ_INIT = 1,
    SEQ_FIRST_ACT = 2,
    SEQ_TURN = 3,
    SEQ_PHASE = 4,
    SEQ_MOVE = 5,
    SEQ_ACT = 6,
    SEQ_END = 7
} BattleSequence;

// Steps of the SEQ_END sub-sequence (BattleGetSeq(wp, SEQ_END)), run by btlseqEnd in battle_seq_end.c.
typedef enum BattleSeqEndStep {
    SEQ_END_INIT = 0x07000000,             // set the battle result, pick the branch, allocate seqEndWork
    SEQ_END_LOSE_FADE = 0x07000001,        // Mario's side was cleared: fade out, BGM off
    SEQ_END_LOSE_FADE_WAIT = 0x07000002,   // then game over, or SEQ_END_RETURN when the setup allows losing
    SEQ_END_WIN_EVT = 0x07000003,          // won, and the field battle has an end event: start it
    SEQ_END_WIN_EVT_WAIT = 0x07000004,     // wait for it (it can send the battle back to SEQ_TURN)
    SEQ_END_WIN_CLEAR_STATUS = 0x07000005, // unflip the partner, use reserved items, clear statuses
    SEQ_END_WIN_START = 0x07000006,        // units' battle-end events and drops; Merlee's exp curse starts here
    SEQ_END_WIN_MAJINAI_WAIT = 0x07000007, // wait for Merlee's exp-up event
    SEQ_END_EXP_INIT = 0x07000008,         // stock exp, victory camera and BGM
    SEQ_END_EXP_DELAY = 0x07000009,
    SEQ_END_EXP_START = 0x0700000A,        // units' exp events and the point effect
    SEQ_END_EXP_COUNT = 0x0700000B,        // exp ticks into star points
    SEQ_END_EXP_COUNT_END = 0x0700000C,
    SEQ_END_EXP_END_WAIT = 0x0700000D,
    SEQ_END_MAJINAI_END = 0x0700000E,      // Merlee has no uses left: announce it
    SEQ_END_MAJINAI_END_WAIT = 0x0700000F,
    SEQ_END_LVUP_CHECK = 0x07000010,
    SEQ_END_LVUP_CURTAIN = 0x07000011,     // _curtain_close_event
    SEQ_END_LVUP_SELECT = 0x07000012,      // choose HP, FP or BP
    SEQ_END_LVUP_CONFIRM = 0x07000013,     // yes/no prompt; no step leads here
    SEQ_END_LVUP_JUMP_WAIT = 0x07000014,   // _jump_event
    SEQ_END_LVUP_APPLY = 0x07000015,
    SEQ_END_LVUP_CHEER = 0x07000016,
    SEQ_END_RANK_UP = 0x07000017,
    SEQ_END_RANK_UP_WAIT = 0x07000018,
    SEQ_END_WIN_POSE = 0x07000019,         // no level-up: victory voice and pose
    SEQ_END_WIN_POSE_WAIT = 0x0700001A,
    SEQ_END_WIN_POSE_DONE = 0x0700001B,
    SEQ_END_WIN_DONE = 0x0700001C,
    SEQ_END_FADE_OUT = 0x0700001D,
    SEQ_END_FADE_WAIT = 0x0700001E,
    SEQ_END_RETURN = 0x0700001F,           // hand Mario and the party back to the field, free seqEndWork
} BattleSeqEndStep;

// deprecated ------------------------------------------------------------
struct BattleWorkCommandAction {
    u32 mType;      // 0x0
    BOOL mEnabled;  // 0x4
    char* mName;    // 0x8
    u16 mIcon;      // 0xC
    u8 pad_0xE[2];  // 0xE
    u32 field_0x10; // 0x10, mDisabled?
};

struct BattleWorkCommandWeapon {
    BattleWeapon* mWeaponParams; // 0x0
    u32 field_0x4;               // 0x4
    char* mName;                 // 0x8
    u16 mIcon;                   // 0xC
    u8 pad_0xE[2];               // 0xE
    u32 mIndex;                  // 0x10
    ItemType mItem;              // 0x14
    u32 field_0x18;              // 0x18
};

struct BattleWorkCommandOperation {
    u32 mType;          // 0x0
    BOOL mEnabled;      // 0x4
    u32 field_0x8;      // 0x8, mDisabled?
    char* mName;        // 0xC
    u16 mIcon;          // 0x10
    u8 pad_0x12[2];     // 0x12
    char* mDescription; // 0x14
    u32 mWeaponCost;    // 0x18
};

struct BattleWorkCommandParty {
    BattleUnitType mPartyId; // 0x0
    BOOL mEnabled;           // 0x4
    char* mName;             // 0x8
    u16 mIcon;               // 0xC
    u8 pad_0xE[2];           // 0xE
    char* mDescription;      // 0x10
    u16 mCurrentHP;          // 0x14
    u16 mMaxHP;              // 0x16
};

struct BattleWorkCommandMultiItem {
    BOOL mEnabled;      // 0x0
    char* mName;        // 0x4
    char* mDescription; // 0x8
    u16 mIcon;          // 0xC
    u8 pad_0xE[2];      // 0xE
    u32 mWeaponCost;    // 0x10
    u32 field_0x14;     // 0x14, mDisabled?
};

struct BattleWorkCommandCursor {
    u32 mAbsolutePos; // 0x0
    u32 mRelativePos; // 0x4
    u32 mNumOptions;  // 0x8
};

struct BattleWorkCommandWindow {
    u8 field_0x0[0x124 - 0]; // 0x0
};

// TODO: modularize for more actions/party members?
struct BattleWorkCommand {
    u32 mCurrentState;                             // 0x0
    u32 mCurrentMenu;                              // 0x4
    BattleWorkCommandAction mActionTable[6];       // 0x8
    BattleWorkCommandWeapon mWeaponTable[21];      // 0x80
    BattleWorkCommandOperation mOperationTable[7]; // 0x2CC
    BattleWorkCommandParty mPartyTable[8];         // 0x390
    BattleWorkCommandMultiItem mMultiItemTable[3]; // 0x450
    BattleWorkCommandCursor mCursors[14];          // 0x498
    u8 field_0x540[0x55C - 0x540];                 // 0x540
    BattleWorkCommandWindow* mWindowWork;          // 0x55C
    u8 field_0x560[0x574 - 0x560];                 // 0x560
};
// end deprecated ----------------------------------------------------

typedef struct BattleWorkActRecord {
    u8 field_0x0[0x24 - 0x0]; // 0x0
} BattleWorkActRecord;

struct BattleAlliance {
    s16 allianceId;     // 0x0
    s8 attackDirection; // 0x2
    // pad 1 byte
    u32 clearConditionMet; // 0x4
};

typedef struct BattleWorkPartyInfo {
    f32 field_0;              // 0x0
    u8 field_4[0x1E];         // 0x4
    u8 field_22[0x24 - 0x22]; // 0x22
    s32 field_24;             // 0x24
} BattleWorkPartyInfo;

struct BattleWork {
    s16 turnCount;                      // 0x0
    u8 field_2[0x4 - 0x2];              // 0x2
    s32 seqUnknownValue;                // 0x4
    BattleAlliance alliances[3];        // 0x8
    BattleUnit* units[64];              // 0x20
    s32 unitIdTable[64];                // 0x120
    PhaseEvtEntry phaseEvtTable[64];    // 0x220, battleMakePhaseEvtTable / btlseqMove
    s32 activeUnitId;                   // 0x420
    s32 field_424;                      // 0x424
    BattleSampling sampling;            // 0x428, enemy target sampling (battle_detect.c)
    u32 flags;                          // 0xEF4
    s32 dispFlags;                      // 0xEF8
    u8 field_EFC[0xF00 - 0xEFC];        // 0xEFC
    s32 stockExp;                       // 0xF00, BattleStoreExp / BattleGetStockExp
    u8 field_F04[0xF0C - 0xF04];        // 0xF04
    s32 seqInitValue;                   // 0xF0C
    s32 seqFirstActValue;               // 0xF10
    s32 seqTurnValue;                   // 0xF14
    s32 seqPhaseValue;                  // 0xF18
    s32 seqMoveValue;                   // 0xF1C
    s32 seqActValue;                    // 0xF20
    s32 seqEndValue;                    // 0xF24
    BattleSeqEndWork* seqEndWork;       // 0xF28
    BattleWorkPad padWork[4];           // 0xF2C
    BattleCommand commandMenu;          // 0x171C
    BattleActions actionCommands;       // 0x1C90
    u8 field_1F9C[0x271C - 0x1F9C];     // 0x1F9C
    s32 mStylishCurFrame;               // 0x271C
    s32 mStylishWindowStart;            // 0x2720
    s32 mStylishWindowEnd;              // 0x2724
    s32 mStylishEndFrame;               // 0x2728
    s32 mStylishUnitId;                 // 0x272C
    s32 mStylishResult;                 // 0x2730
    s32 mStylishEarlyFrames;            // 0xA2734
    FieldBattleInfo* fbatInfo;          // 0x2738
    s32 statusWinScriptOff;             // 0x273C, set by nok unit_act_mario _status_window_off; hides the window
    s32 statusWinLectureOff;            // 0x2740, set by gor _set_lecture_mode (tutorial battle); hides the window
    s32 statusWinSystem;                // 0x2744, BattleStatusWindowSystemOn/Off, StatusWindowOnOff mode 2
    s32 statusWinEvent;                 // 0x2748, BattleStatusWindowEventOn/Off, StatusWindowOnOff mode 0/1
    s32 statusWinAPRecovery;            // 0x274C, BattleStatusWindowAPRecoveryOn/Off
    u8 field_2750[0x2754 - 0x2750];     // 0x2750
    BattleCameraWork cameraWork;        // 0x2754
    BattleAudience audience;            // 0x2858
    u8 field_1616C[0x162A0 - 0x1616C];  // 0x1616C
    BattleWorkPartyInfo partyInfo[7];   // 0x162A0
    u32 tattledUnitTypeFlags[8];        // 0x163B8
    s32 badgeEquippedFlags;             // 0x163D8
    FileEntry* menuTex;                 // 0x163DC
    BattleStage stage;                  // 0x163E0
    u8 field_16F1C[0x16F40 - 0x16F1C];  // 0x16F1C
    BattleAfterReactionWork afterReaction[64]; // 0x16F40
    BattleStageObject stageObjects[32]; // 0x17140
    u8 field_180C0[0x180DC - 0x180C0];  // 0x180C0
    u8 stageFlags;                      // 0x180DC, bits 0x2/0x4/0x8 set by the layer switches (battle_stage_object.c),
                                        // 0x20 set by init_ceil, _disable_enemy_unit_disp and read by ceil_exist
    u8 field_180DD[0x182B0 - 0x180DD];  // 0x180DD
    BattleIcon icons[16];               // 0x182B0
    u8 field_18C70[0x18FE0 - 0x18C70];  // 0x18C70
    s8 alertTick;                       // 0x18FE0
    u8 field_18FE1[0x18FE4 - 0x18FE1];  // 0x18FE1
    StarPowerInfo impendingBonuses;     // 0x18FE4
    u8 field_18FF0[0x19050 - 0x18FF0];  // 0x18FF0
    u32 reserveItems[4];                // 0x19050
    s32 field_19060;                    // 0x19060
    u8 field_19064[0x19088 - 0x19064];  // 0x19064
};
extern BattleWork* _battleWorkPointer;

void battleMain(void);

void battle_exit(void);
BOOL battle_init(void);
void BattleAfterReactionQueueInit(void);

BattleUnit* BattleGetPartnerPtr(BattleWork* work, BattleUnit* unit);
BattleUnit* BattleGetPartyPtr(BattleWork* wp);
BattleUnit* BattleGetMarioPtr(BattleWork* work);
BattleUnit* BattleGetSystemPtr(BattleWork* work);
BattleUnitPart* BattleGetUnitPartsPtr(s32 index, s32 partNum);
void BattleSetUnitPtr(BattleWork* wp, s32 index, BattleUnit* unit);
BattleUnit* BattleGetUnitPtr(BattleWork* wp, s32 index);
void BattleFree(void* ptr);
BOOL BattlePartyAnimeLoad(BattleUnitType partyId);
BOOL BattleStatusWindowCheck(void);
void* BattleAlloc(u32 size);
void BattleIncSeq(BattleWork* wp, BattleSequence seq);
s32 BattleGetSeq(BattleWork* wp, BattleSequence seq);
void BattleSetSeq(BattleWork* wp, BattleSequence seq, u32 value);
void BattleSetMarioParamToFieldBattle(BattleWork* work);
BOOL Btl_UnitSetup(BattleWork* work);
void BattleEnd(void);
void BattleInit(FieldBattleInfo* info);
BOOL battleSeqEndCheck(void);

struct BattleUnit* BattleChangeParty(BattleWork* wp);
s32 BattleTransPartyId(BattleUnitType kind);

BOOL battleDisableHResetCheck(void);
BOOL BattleMain(void);
void BattlePartyInfoWorkInit(BattleWork* wp);
void BattleCheckUnitBroken(BattleWork* wp);
BattleUnitType BattleTransPartyIdToUnitKind(s32 partyId);
void BattleStoreCoin(BattleWork* wp, s32 coins);
void BattleStoreExp(BattleWork* wp, s32 exp);
s32 BattleGetStockExp(BattleWork* wp);
f32 BattleGetFloorHeight(BattleWork* wp, f32 x, f32 y, f32 z);
void BattleAfterReactionEntry(s32 id, s32 data);
void BattleAfterReactionRelease(s32 id, s32 data);
BOOL BattleAfterReactionMain(void);
s32 BattleMajinaiCheck(void);
void BattleMajinaiDone(void);
void BattleMajinaiEndCheck(void);
void BattleStatusWindowAPRecoveryOn(void);
void BattleStatusWindowAPRecoveryOff(void);
void BattleStatusWindowEventOn(void);
void BattleStatusWindowSystemOn(void);
void BattleStatusWindowEventOff(void);
void BattleStatusWindowSystemOff(void);
void BattleConsumeReserveItem(void);

typedef enum EquipFlags {
    EQUIP_RESET = 1 << 0,   // 0x1, clear/reset equipped badges
    EQUIP_MARIO = 1 << 1,   // 0x2, equip Mario (non-P) badges
    EQUIP_PARTNER = 1 << 2, // 0x4, equip Partner (P) badges
} EquipFlags;
void BtlUnit_EquipItem(BattleUnit* unit, EquipFlags flags, ItemType type);
void _EquipItem(BattleUnit* unit, EquipFlags flags, ItemType type);
