#pragma once

#include <dolphin/types.h>
#include "battle/battle_ac.h"
#include "battle/battle_detect.h"
#include "battle/battle_audience.h"
#include "battle/battle_break_slot.h"
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
    s16 mCurrentHP;          // 0x14
    s16 mMaxHP;              // 0x16
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
    u16 allianceId;     // 0x0
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

// BattleWork.field_180C0: the stage-object work of battle_stage_object.c (layer events, nozzles, fog, falling
// frames and objects). The code indexes it from 0x180C0 (_nozzle_turn, _nozzle_work_wait, _Nozzle_Change_Check),
// and the original reaches the other fields through one pointer to it: wp->field_180C0.x recomputes the address
// for each access, where a BattleWorkField180C0 pointer gives the original's single addis.
typedef struct BattleWorkField180C0 {
    u8 field_0x0[0x4 - 0x0];       // 0x0 (BattleWork 0x180C0)
    Vec field_0x4;                 // 0x4, rotation of the layer 2 objects (_set_mobj_rotate_x sets x)
    Vec field_0x10;                // 0x10, field_0x4 as last applied by BattleStageObjectMain
    u8 stageFlags;                 // 0x1C, bits 0x2/0x4/0x8 set by the layer switches, 0x10 by
                                   // BattleStage_WallCloseCheck, 0x20 by BattleStage_IronFrameFallCheck, init_ceil
                                   // and _disable_enemy_unit_disp (read by ceil_exist); BattleStage_IronFrameFallCheck
                                   // does nothing while 0x20 or 0x40 is set
    u8 field_0x1D[0x20 - 0x1D];    // 0x1D
    s32 field_0x20;                // 0x20, event id of the object turn event (BattleStage_DestroyA1/B)
    s32 field_0x24;                // 0x24, event ids of the stage's layerEvents[0]-[3] (BattleStageObjectInit)
    s32 field_0x28;                // 0x28
    s32 field_0x2C;                // 0x2C
    s32 field_0x30;                // 0x30
    u8* field_0x34;                // 0x34, this rank's block of battle_stage_nozzle_data, NULL at rank 0
    u8* field_0x38;                // 0x38, this rank's block of battle_stage_fall_object_data
    BattleWeapon* weaponAddr[2];   // 0x3C, _nozzle_work_get_weapon_address; points into field_0x44
    BattleWeapon field_0x44[2];    // 0x44, nozzle weapons combined by _nozzle_work_attack_set
    u8 field_0x1C4[3];             // 0x1C4, direction of each nozzle (0-3)
    u8 field_0x1C7[3];             // 0x1C7, direction each nozzle is turning to
    s8 field_0x1CA;                // 0x1CA, nozzle type (-1 until _nozzle_type_init picks one)
    u8 field_0x1CB;                // 0x1CB
    s32 field_0x1CC[3];            // 0x1CC, event id of each nozzle's _nozzle_turn_event
    s32 field_0x1D8;               // 0x1D8, event id of the nozzle work event (BattleStage_NozzleWorkCheck)
    s32 field_0x1DC;               // 0x1DC, fog frames left (BattleFogEndCheck)
    EffectEntry* field_0x1E0;      // 0x1E0, the fog effect (effNozleEntry), deleted when the fog ends
    s32 field_0x1E4;               // 0x1E4, event id of _close_wall_event (BattleStage_WallCloseCheck)
    s32 field_0x1E8;               // 0x1E8, event id of _fall_frame_event (BattleStage_IronFrameFallCheck)
    s32 field_0x1EC;               // 0x1EC, event id of _object_fall_event (BattleStage_ObjectFallCheck)
} BattleWorkField180C0;

// BattleWork.field_18C70: the battle announce message (battle_message.c). btlevtcmd_AnnounceSetParam indexes it from
// 0x18C70, and the original reaches all of it through one pointer.
typedef struct BattleWorkField18C70 {
    s16 field_0x0;                 // 0x0 (BattleWork 0x18C70), btlevtcmd_AnnounceMessage's x
    s16 field_0x2;                 // 0x2, and y
    s32 field_0x4[4];              // 0x4, btlevtcmd_AnnounceSetParam; [0] is the item index of message kind 1
    char field_0x14[0x118 - 0x14]; // 0x14, the formatted message
} BattleWorkField18C70;

// BattleWork.field_19068: forced random outcomes. Nothing in the game writes forceFlags; each bit forces a random
// outcome where it is read. battle_audience.c and battle_break_slot.c read these through one pointer (one addis per
// function), so the original has them in a struct; no indexed access shows where that struct starts, and this one
// starts at the first field any user reads.
typedef struct BattleWorkField19068 {
    u32 forceFlags;                // 0x0 (BattleWork 0x19068): 0x1 object fall, 0x2 the audience gets the falling
                                   // object, 0x10/0x20/0x40 stage destroy A1/A2/B, 0x80 iron frame fall, 0x100 nozzle
                                   // work, 0x200 nozzle turn, 0x10000 audience count = forceAudienceCount, 0x20000
                                   // frequent audience item throws, 0x40000 the 1-in-20 audience kind swap, 0x80000
                                   // bingo reels stop on forceSlotValue
    u8 field_0x4[0xC - 0x4];       // 0x4
    s32 forceAudienceCount;        // 0xC, starting audience (capped at the max) under 0x10000
    s32 forceAudienceKind;         // 0x10, 1-12 fills the audience with one kind; 0 = off
    s32 forceSlotValue;            // 0x14, reel value forced under 0x80000
} BattleWorkField19068;

// BattleWork.field_18FF0: the action-command help (battle_ac_help.c). BattleAcHelpMain reads it through one pointer
// (one addis), so the original has a struct here; it is at least these 8 bytes and starts at or before 0x18FF0.
typedef struct BattleWorkField18FF0 {
    const char* field_0x0;         // 0x0 (BattleWork 0x18FF0), BattleAcHelpSetHelp's message
    s32 field_0x4;                 // 0x4, BattleAcHelpSetDispType's type; 0 hides the help
} BattleWorkField18FF0;

struct BattleWork {
    s16 turnCount;                      // 0x0
    u16 field_2;                        // 0x2, step counter of battle_seq's SEQ_TURN 0x3000009 (burn effects)
    s32 seqUnknownValue;                // 0x4
    BattleAlliance alliances[3];        // 0x8
    BattleUnit* units[64];              // 0x20
    s32 unitIdTable[64];                // 0x120
    PhaseEvtEntry phaseEvtTable[64];    // 0x220, battleMakePhaseEvtTable / btlseqMove
    s32 activeUnitId;                   // 0x420
    s32 lectureUnitId;                  // 0x424, _battle_set_lecture_unit_id (tutorial battles); BattleTransID(-6)
                                        // returns it; -1 until set
    BattleSampling sampling;            // 0x428, enemy target sampling (battle_detect.c)
    u32 flags;                          // 0xEF4
    s32 dispFlags;                      // 0xEF8
    s32 field_EFC;                      // 0xEFC, frame countdown (btlseqFirstAct, btlseqTurn)
    s32 stockExp;                       // 0xF00, BattleStoreExp / BattleGetStockExp
    s32 field_F04;                      // 0xF04, exp shown by _btlStockExpDisp; BattleStockExpDisp counts it up to
                                        // stockExp
    s32 field_F08;                      // 0xF08, BattleStockExpDisp's frame counter for that count
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
    u8 field_1F9C[0x271C - 0x1F9C];     // 0x1F9C, the special-move works that start at actionCommands.field_0x2BC
                                        // (0x1F4C: sac_deka's DekaWork, sac_suki's SukiWork, ...) run on into it
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
    BattleBreakSlot field_1616C;        // 0x1616C, BattleBreakSlotGetPtr
    BattleWorkPartyInfo partyInfo[7];   // 0x162A0
    u32 tattledUnitTypeFlags[8];        // 0x163B8
    s32 badgeEquippedFlags;             // 0x163D8
    FileEntry* menuTex;                 // 0x163DC
    BattleStage stage;                  // 0x163E0
    u8 field_16F1C[0x16F40 - 0x16F1C];  // 0x16F1C, act-record counts for battle_actrecord.c's rule checks (most
                                        // through BtlActRec_AddCount/AddPoint, capped at 100)
    BattleAfterReactionWork afterReaction[64]; // 0x16F40
    BattleStageObject stageObjects[32]; // 0x17140
    BattleWorkField180C0 field_180C0;   // 0x180C0
    BattleIcon icons[16];               // 0x182B0
    BattleWorkField18C70 field_18C70;   // 0x18C70
    BattleStatusChangeMsgWork statusChangeMsg; // 0x18D88
    s8 alertTick;                       // 0x18FE0
    u8 field_18FE1;                     // 0x18FE1, Merlee's mMerleeNextCurseType once it fires: 1 +3 attack (cleared
                                        // at the end of the attack), 2 +3 defense for Mario (this phase), 3 double exp
    u16 field_18FE2;                    // 0x18FE2, frames in battle_seq's 0x200000B wait, capped at 300
    StarPowerInfo impendingBonuses;     // 0x18FE4
    BattleWorkField18FF0 field_18FF0;   // 0x18FF0
    u8 field_18FF8[0x19038 - 0x18FF8];  // 0x18FF8
    s32 field_19038[5];                 // 0x19038, turn counts; while one is > 0 the command menu disables Mario's
                                        // jump, hammer or item, or the partner's tech or item ([0]-[4]); battle_seq
                                        // counts each down, recover_cmd_disable clears all five
    s32 field_1904C;                    // 0x1904C, nanika_luck_event's event id
    u32 reserveItems[4];                // 0x19050
    s32 field_19060;                    // 0x19060, SFX_OFF_BTL_START_CURTAIN_UP1's handle (btlseqFirstAct), -1 when off
    u8 field_19064;                     // 0x19064, 1 when the last btlevtcmd_AudienceDeclareACResult succeeded, else 0
    u8 field_19065[0x19068 - 0x19065];  // 0x19065
    BattleWorkField19068 field_19068;   // 0x19068
    u32 field_19080;                    // 0x19080, bit 0 set by the gor lecture's aud_attack_init/aud_attack_end
    s32 field_19084;                    // 0x19084, aud_attack_init's message window id (msgWindow_Continue)
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
