#pragma once

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include "battle/battle_unit.h"
#include "mario/mario_pouch.h"

#define NPC_FIELD_MAX_COUNT 36
#define NPC_BATTLE_MAX_COUNT 16

typedef enum NpcTerritoryType {
    NPC_TERRITORY_TYPE_NOTHING = 0,
    NPC_TERRITORY_TYPE_CIRCLE = 1,
    NPC_TERRITORY_TYPE_SQUARE = 2
} NpcTerritoryType;

typedef enum NpcEventType {
    NPC_EVENT_TYPE_INIT = 0,   // initEvent
    NPC_EVENT_TYPE_MOVE = 1,   // moveEvent
    NPC_EVENT_TYPE_FIND = 2,   // findEvent
    NPC_EVENT_TYPE_LOST = 3,   // lostEvent
    NPC_EVENT_TYPE_RETURN = 4, // returnEvent
    NPC_EVENT_TYPE_BLOW = 5,   // blowEvent
    NPC_EVENT_TYPE_TALK = 6,   // talkEvt, fbat talk
    NPC_EVENT_TYPE_DEAD = 8    // deadEvent
} NpcEventType;

typedef struct NpcTerritoryInfo {
    const char* description; // 0x0
    NpcTerritoryType type;   // 0x4
} NpcTerritoryInfo;

typedef struct NpcDeadInfo {
    char mapName[16]; // 0x0
    s32 mask;         // 0x10
} NpcDeadInfo;

// TODO: move this somewhere better?
typedef struct BattleGroupSetup {
    s32 numEnemies;             // 0x0
    BattleUnitSetup* enemyData; // 0x4
    s32 field_0x8;              // 0x8
    s32 field_0xC;              // 0xC
    s32 field_0x10;             // 0x10
    s32* heartDropParams;       // 0x14, battle_heart_drop_param_default*
    s32* flowerDropParams;      // 0x18, battle_flower_drop_param_default*
    s32 field_0x1C;             // 0x1C
} BattleGroupSetup;

// One entry of an area's battle setup list (btlsetup_*_tbl); npcSetBattleInfo finds one by name and hands it to
// npcSetupBattleInfo.
typedef struct BattleSetupData {
    const char* name;                         // 0x0
    const char* mapName;                      // 0x4, a map name where set (aji_14 for the Magnum Battender fight)
    s32 fixDropItem;                          // 0x8, copied to NpcBattleInfo.fixDropItem
    s32 flag;                                 // 0xC, a GSWF: while it is set, onLoadouts is used
    struct NpcBattleSetupEntry* onLoadouts;   // 0x10, btlgrpsel_*_on
    struct NpcBattleSetupEntry* offLoadouts;  // 0x14, btlgrpsel_*_off
    s32 flags;                                // 0x18, NpcBattleFlags, copied to NpcBattleInfo.flags
    s32 audienceMode;                         // 0x1C, copied to NpcBattleInfo.audienceMode; outside 1-2, audienceRanges is rolled
    s8 audienceRanges[16][2];                 // 0x20, min/max pairs, each rolled into NpcBattleInfo.audienceWeights
    const char* music;                        // 0x40, copied to NpcBattleInfo.music
} BattleSetupData;

// NpcBattleInfo.flags bits (copied from BattleSetupData.flags; jon's trap events add the Jump, Hammer and
// partner-switch bits). Bits 0x20, 0x100000 and 0x20000000 are still unnamed.
typedef enum NpcBattleFlags {
    NPC_BATTLE_NO_DROP_COIN = 0x1,               // evt_npc_get_drop_coin returns 0
    NPC_BATTLE_NO_DROP_HEART = 0x2,              // evt_npc_get_drop_heart returns 0
    NPC_BATTLE_NO_DROP_FLOWER = 0x4,             // evt_npc_get_drop_flower returns 0
    NPC_BATTLE_NO_DROP_ITEM = 0x8,               // evt_npc_get_drop_item returns 0
    NPC_BATTLE_NO_ESCAPE = 0x10,                 // the Run Away operation is disabled; btlseqFirstAct also runs the wall-close check
    NPC_BATTLE_LOSS_RETURNS_TO_FIELD = 0x40,     // a loss leaves Mario at 1 HP in the field instead of game over
    NPC_BATTLE_COIN_BONUS = 0x100,               // fbatBattleMode adds 1 to the coin multiplier
    NPC_BATTLE_NO_JUMP = 0x01000000,             // the Jump action is disabled
    NPC_BATTLE_NO_HAMMER = 0x02000000,           // the Hammer action is disabled
    NPC_BATTLE_NO_PARTY_CHANGE = 0x04000000,     // the change-partner operation is disabled
    NPC_BATTLE_NO_FIRST_STRIKE_WIN = 0x10000000, // Bump Attack and First Attack don't win the battle outright
} NpcBattleFlags;

typedef struct NpcBattleInfo {
    struct BattleStageData* stageData; // 0x0
    s32 flags;                         // 0x4, NpcBattleFlags, copied from BattleSetupData.flags
    s32 fixDropItem;                   // 0x8, copied from BattleSetupData.fixDropItem; an item the dead event always drops when nonzero (evt_npc_get_drop_fixitem)
    ItemType itemDropped;              // 0xC
    s32 heartsDropped;                 // 0x10
    s32 flowersDropped;                // 0x14
    BattleGroupSetup* groupSetup;      // 0x18
    ItemType heldItems[8];             // 0x1C
    ItemDropData* itemDropLists[8];    // 0x3C
    ItemType stolenItems[8];           // 0x5C
    ItemType recoveredItems[8];        // 0x7C
    void* btlSetupWork[4];             // 0x9C
    s32 audienceMode;                  // 0xAC, copied from BattleSetupData.audienceMode; battle_audience switches on it
    s8 audienceWeights[16];            // 0xB0, rolled from BattleSetupData.audienceRanges; battle_audience reads kinds 0-11
    const char* music;                 // 0xC0, copied from BattleSetupData.music
    u8 battleRules[3];                 // 0xC4, set by evt_npc_set_battle_rule (Glitz Pit): rule kind (the _rule_msg_table index),
                                       // its parameter (printed by _rule_disp), a third byte; seq_battle copies them to FieldBattleInfo
    u8 field_C7[0xC8 - 0xC7];          // 0xC7
} NpcBattleInfo;

// The 0xC-byte entries of the weighted battle setup list selected by npcSetupBattleInfo.
typedef struct NpcBattleSetupEntry {
    s32 weight;              // 0x0
    BattleGroupSetup* setup; // 0x4
    struct BattleStageData* stageData; // 0x8
} NpcBattleSetupEntry;

typedef struct NpcTribeInfo {
    const char* nameJp;           // 0x0
    const char* modelName;        // 0x4
    const char* initialAnim;      // 0x8
    const char* stopAnimation;    // 0xC
    const char* stayAnim;         // 0x10
    const char* talkAnim;         // 0x14
    const char* walkAnimation;    // 0x18
    const char* runAnimation;     // 0x1C
    const char* damageAnimation;  // 0x20
    const char* confuseAnimation; // 0x24
    f32 width;                    // 0x28, TODO double check
    f32 height;                   // 0x2C
    f32 shadowOffsetX;            // 0x30
    f32 shadowOffsetY;            // 0x34
    f32 shadowOffsetZ;            // 0x38
    f32 runStartSpeed;            // 0x3C
    const char* footstepSfxA;     // 0x40
    const char* footstepSfxB;     // 0x44
    u16 footstepStride;           // 0x48
    const char* jumpSfx;          // 0x4C
    const char* landSfx;          // 0x50
} NpcTribeInfo;

typedef struct NpcEntry {
    s32 flags;                      // 0x0
    u32 reactionFlags;              // 0x4
    char description[32];           // 0x8
    NpcTribeInfo* tribe;            // 0x28
    char currentAnim[32];           // 0x2C
    char stayAnim[32];              // 0x4C
    char talkAnim[32];              // 0x6C
    Vec position;                   // 0x8C, verified
    Vec prevPosition;               // 0x98
    Vec positionHistory[5];         // 0xA4
    Vec scale;                      // 0xE0
    Vec rotation;                   // 0xEC
    Vec rotationOffset;             // 0xF8
    s32 poseId;                     // 0x104
    s32 gait;                       // 0x108, 0 unset, 1 stay, 2 walk, 3 run (picks the dust effect in _kamek_houki_kemuri)
    const char* checkPose;          // 0x10C, tik_02 geso_set_checkpose (geso_checkpose)
    s32 offscreenId;                // 0x110, offscreen layer drawn into (evt_npc_set_offscreen), -1 none
    GXColor color;                  // 0x114
    s32 initEvtId;                  // 0x118
    s32 regularEvtId;               // 0x11C
    void* initEvent;                // 0x120, verified
    void* moveEvent;                // 0x124, verified
    void* talkEvt;                  // 0x128
    void* deadEvent;                // 0x12C, verified
    void* findEvent;                // 0x130, verified
    void* lostEvent;                // 0x134, verified
    void* returnEvent;              // 0x138, verified
    void* blowEvent;                // 0x13C, verified
    u32 interruptFlags;             // 0x140, pending reactions: 1 found Mario, 2 lost, 4 returned, 8 blown, 0x10 dead, 0x10000000 move
    f32 direction;                  // 0x144, angle we're facing at, verified
    f32 savedDirection;             // 0x148, direction kept while the npc is turned toward Mario (hit, noticed, talk), restored after
    f32 width;                      // 0x14C
    f32 height;                     // 0x150
    f32 shadowScale;                // 0x154, multiplies the shadow radius; 1.0 at entry, eased by the evt_npc_majo_disp_* events
    Vec jumpStartPos;               // 0x158
    Vec jumpTargetPos;              // 0x164
    u8 field_170[0x178 - 0x170];    // 0x170
    OSTime airTicks;                // 0x178, ticks under gravity while flags 0x20000 is set; reset on landing
    f32 airTime;                    // 0x180, airTicks in seconds
    u8 field_184[0x188 - 0x184];    // 0x184
    OSTime moveElapsed;             // 0x188, ticks since the current move began; cleared by the move events
    OSTime frameTicks;              // 0x190, ticks since the last update (16 ms if it was over 500 ms)
    OSTime lastUpdateTick;          // 0x198, gp->renderTime at the last update
    f32 moveElapsedSec;             // 0x1A0, moveElapsed in seconds (compared against moveTime)
    f32 moveDt;                     // 0x1A4
    f32 lastUpdateSec;              // 0x1A8, lastUpdateTick in seconds
    f32 moveDir;                    // 0x1AC
    f32 moveSpeed;                  // 0x1B0
    Vec moveOffset;                 // 0x1B4
    f32 moveTime;                   // 0x1C0
    f32 jumpVelY;                   // 0x1C4
    f32 jumpYScale;                 // 0x1C8
    f32 jumpGravity;                // 0x1CC
    f32 maxFallSpeed;               // 0x1D0, caps the fall per second under gravity (halved in water); 0 no cap, set by limitSpd
    s32 jumpFlags;                  // 0x1D4, TODO verify
    s16 footstepToggle;             // 0x1D8
    f32 footstepAccum;              // 0x1DC
    const char* footstepSfxA;       // 0x1E0
    const char* footstepSfxB;       // 0x1E4
    u16 footstepStride;             // 0x1E8
    const char* jumpSfx;            // 0x1EC
    const char* landSfx;            // 0x1F0
    s32 cameraId;                   // 0x1F4, CameraId
    NpcTerritoryType territoryType; // 0x1F8, verified, TODO: create territory substruct?
    Vec territoryBase;              // 0x1FC, verified
    Vec territoryLoiter;            // 0x208, verified
    Vec territoryHoming;            // 0x214, verified
    f32 searchRange;                // 0x220, verified
    f32 searchAngle;                // 0x224, verified
    f32 homingRange;                // 0x228, verified
    f32 homingAngle;                // 0x22C, verified
    NpcBattleInfo battleInfo;       // 0x230
    s32 attackMode;                 // 0x2F8, set by evt_npc_set_attack_mode; nonzero makes a stomp a first strike against Mario
    struct HitEntry* wallHit;       // 0x2FC, wall npcHitCheckSide stopped the last move against, NULL if none
    struct HitEntry* groundHit;     // 0x300, floor npcMoveG last probed under a falling npc, NULL if the probe found none
    void* unitWork[4];              // 0x304
    u8 setupIndex;                  // 0x314, index in the map's npc setup: its bit in fbat deadInfo[0].mask marks it dead
    u8 btlCoinDropCount;            // 0x315
    u8 baseCoin;                    // 0x316, baseCoin of the lead unit kind, the coin drop for battle result 1
    s8 balloonType;                 // 0x317, s8: mario_balloon loads it with lbz; extsb
    s32 fbatHitKind;                // 0x318, fbatHitCheck kind that started the field contact, 0 for a talk (written only)
    u16 koopaEncounterType;         // 0x31C
    u16 hazardImmuneFlags;          // 0x31E, bits 1/2/4/8/0x10 skip the npcGroupDead of that hazard class; set to 0x1F by set_bottle_flag_init
    u16 wFbatHitCheckRelated;       // 0x320
    u8 pad_322[2];                  // 0x322
    struct NpcEntry* prev;          // 0x324
    struct NpcEntry* next;          // 0x328
    struct NpcEntry* master;        // 0x32C
    struct NpcEntry* slaves[4];     // 0x330
} NpcEntry;

typedef struct NpcFieldEntry {
    char mModelName[64];                // 0x0
    char mAnimName[64];                 // 0x40
    u32 mFlags;                         // 0x80
    f32 wCurrentRenderRotationMinus90Y; // 0x84, TODO better name
    f32 wRotationY;                     // 0x88, TODO better name
    f32 wScaleZ_flip;                   // 0x8C, TODO better name
    f32 wInitToZero4;                   // 0x90, TODO better name
} NpcFieldEntry;

typedef struct NpcWork {
    s32 count;            // 0x0, currently allocated
    s32 total;            // 0x4, maximum allowed
    s32 flags;            // 0x8
    NpcEntry* entries;    // 0xC
    NpcEntry* talkCheckNpc; // 0x10, the NPC Mario can talk to this frame (set in the talk check, read on the talk button)
} NpcWork;

typedef struct FirstStrikeInfo {
    BOOL enabled;        // 0x0
    u32 announceKind;    // 0x4, the attack code passed to fbatSetAttackAnnounce (as FbatHitInfo.kind); 0 when the banner ends
    u32 type;            // 0x8
    u32 timer;           // 0xC, frames since the banner started
    f32 bannerX;         // 0x10
    f32 bannerY;         // 0x14
    const char* msg_tag; // 0x18
    GXColor color;       // 0x1C
} FirstStrikeInfo;

// Battle setup and result passed to the battle (BattleInit keeps it as BattleWork.fbatInfo).
typedef struct FieldBattleInfo {
    u32 mode;                // 0x0, unused
    BattleUnitType party;    // 0x4
    u32 partyId;             // 0x8
    NpcBattleInfo* setup;    // 0xC
    s32 result;              // 0x10
    s32 storeCoin;           // 0x14, coins added by BattleStoreCoin; npcdrv copies it to NpcEntry.btlCoinDropCount
    u8 field_0x18;           // 0x18, battleRules[0] (seq_battle), the rule kind battle_actrecord judges
    u8 ruleKeepResult;       // 0x19, written by battle_actrecord, read by evt_npc_get_battle_rule_keep_result
    u8 field_0x1A;           // 0x1A, battleRules[1] (seq_battle)
    u8 field_0x1B;           // 0x1B, battleRules[2] (seq_battle)
} FieldBattleInfo;

typedef struct FieldBattleData {
    s16 mode;                    // 0x0
    u8 state;                    // 0x2, step of the talk sequence in npcdrv's field battle state machine
    u8 field_3[0x4 - 0x3];       // 0x3
    NpcEntry* talkNpc;           // 0x4
    NpcEntry* target;            // 0x8
    s32 talkEvtId;               // 0xC, event started for talkNpc, waited on in state 1
    s32 firstAttackKind;         // 0x10, how the field contact was made, written by fbatHitCheck: 0x20000-0x80000
                                 // jumps, 0x100000-0x400000 hammers, 0x800000, 0x1000000; tou_03 forces 0x10000000;
                                 // seq_battle turns it into BattleInformationSetFirstAttack kinds 1-7
    u32 field_14;                // 0x14, BattleInformationSetFirstAttack argument for firstAttackKind 0x10000000
                                 // (seq_battle; tou_03 sets 0x10)
    u8 field_18[0x1C - 0x18];    // 0x18
    u32 field_1C;                // 0x1C, bit 0: no first attack (seq_battle; tou_03 clears it)
    FieldBattleInfo field_20;    // 0x20
    void* endEvt;                // 0x3C, script started by battle_seq_end case 0x07000003
    s32 endEvtId;                // 0x40, waited on by case 0x07000004
    s32 field_44;                // 0x44
    s32 majinaiEvtId;            // 0x48, event waited on in state 3 (the Merlee curse coin-up effect)
    NpcDeadInfo deadInfo[64];    // 0x4C
    u8 field_0x54C;              // 0x54C
    u8 sandersBombTrigger;       // 0x54D
    u8 field_54E[0x550 - 0x54E]; // 0x54E
    FirstStrikeInfo firstStrike; // 0x550
    u8 field_570[0x574 - 0x570]; // 0x570
    s32 field_574;               // 0x574
    s32 field_578;               // 0x578
    s32 field_57C;               // 0x57C
} FieldBattleData;

NpcWork* npcGetWorkPtr(void);
void npcReleaseFieldNpc(void);
void npcRecoveryFieldNpc(void);
void npcInit(void);
void npcReset(BOOL inBattle);
s32 npcGetReactionOfLivingBody(BOOL inBattle);
s32 npcEntry(const char* a1, const char* animName);
NpcTribeInfo* npcGetTribe(const char* tribeName);
void npcDelete(NpcEntry* entry);
void npcDeleteGroup(NpcEntry* entry);
void npcMain(void);
NpcEntry* npcNameToPtr(const char* name);
NpcEntry* npcNameToPtr_NoAssert(const char* name);

FieldBattleData* fbatGetPointer(void);
void fbatSetAttackAnnounce(s32 a1);
void fbatSetAttackAnnounceEnable(void);

void npcSetSlave(NpcEntry* npc, NpcEntry* slave, s32 id);
void npcSetBattleInfo(NpcEntry* npc, s32 battleInfoId);
void npcSetupBattleInfo(NpcEntry* npc, BattleSetupData* setup);
void fbatChangeMode(s16 mode);

NpcTerritoryType dbGetDefData(NpcTerritoryInfo* info, const char* description);

void npcTuningRy(NpcEntry* npc, f32 angle);
void npcGetBackItemEntry(NpcEntry* entry);
void npcSetMarioAutoTalkPose(const char* stayPose, const char* talkPose);
s32 npcCalcScore(NpcEntry* entry);
struct HitEntry;
NpcEntry* fbatNpcTalkCheck(void);
s32 npcKoopaModeMobjBoundDeadCheck(struct HitEntry* hit);
void npcSetBtlSetupWork(NpcEntry* npc, s32 index, void* work);
s32 N_fbatPreventMarioEventChk(void);
void npcStopForEvent(void);
void npcStartForEvent(void);
void npcBlurOn(const char* name);
void npcBlurOff(const char* name);
void _npcGroupDead(NpcEntry* entry, u16 type);
void* fbatHitCheck(s32 a1, s32 a2);  // a pointer: fbatEncountCheck reads 0x28 and 0x104 through it
NpcEntry* _npcHitCheckHammerAllMotion(NpcEntry* entry, f32* outDist, f32 radius, f32 angle);
NpcEntry* _npcHitCheckSphere(NpcEntry* entry, f32* outDist, f32 x, f32 y, f32 z, f32 radius);
void npcGroupDead(NpcEntry* entry, u16 type);
void npcDisp_xlu(CameraId cameraId, void* param);
void npcDisp(CameraId camId, void* param);
void npcDisp_offscreen_xlu(CameraId camId, void* param);
void npcDisp_offscreen(CameraId camId, void* param);
void npcDisp_blur(CameraId camId, void* param);
void _npcDeleteGroup(NpcEntry* entry);
void npcStopForOneEvent(NpcEntry* npc);
void npcStartForOneEvent(NpcEntry* npc);
BOOL npcCheckInterrupt(NpcEntry* npc);
s32 npcCheckHitMarioSide(NpcEntry* entry);
void* npcGetBtlSetupWork(NpcEntry* npc, s32 index);
void npcSetLink(NpcEntry* npc, NpcEntry* link);
void npcSetColor(const char* name, GXColor* color);
f32 npcTransRytoFaceDir(NpcEntry* npc);
s32 npcCheckBlow(NpcEntry* npc);
void mtx_setup(NpcEntry* entry, Mtx mtx, s32 history);
void npcSetStayPose(const char* name);
void npcSetTalkPose(const char* name);
void _npcDead(NpcEntry* entry, u16 type);
struct HitEntry* npcHitCheckSide(NpcEntry* entry, f32* outX, f32* outZ, f32* outAngle, f32 dx, f32 dz);
NpcEntry* npcNearDistCheck(f32 x, f32 y, f32 z, f32 radius);
void npcKoopaModeEncountCheck(void);
void npcMoveG(NpcEntry* entry);
void fbatTalkMode(void);
void fbatBattleMode(void);
void _fbatFirstAttackAnnouceDisp(CameraId camId, void* param);
void fbatSandersBombTriggerOn(void);
void npcClearDeadInfo(void);
void fbatEncountCheck(void);
void npcExecAllInitEvt(void);
BOOL npcWaitAllInitEvtEnd(void);
void npcExecAllReglEvt(void);
void npcKoopaWaterCheck(NpcEntry* entry);
void npcKoopaJumpScore(FieldBattleData* data, struct MarioWork* mario);
