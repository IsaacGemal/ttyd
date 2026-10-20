#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "manager/evtmgr.h"
#include "manager/filemgr.h"
#include "mario/mario_pouch.h"

#pragma enumsalwaysint off
typedef enum AudienceMemberType {
    AUDIENCE_TOAD,          // 0x0, 0
    AUDIENCE_XNAUT,         // 0x1, 1
    AUDIENCE_BOO,           // 0x2, 2
    AUDIENCE_HAMMER_BRO,    // 0x3, 3
    AUDIENCE_DULL_BONES,    // 0x4, 4
    AUDIENCE_SHY_GUY,       // 0x5, 5
    AUDIENCE_CRAZY_DAYZEE,  // 0x6, 6
    AUDIENCE_PUNI,          // 0x7, 7
    AUDIENCE_KOOPA,         // 0x8, 8
    AUDIENCE_BULKY_BOBOMB,  // 0x9, 9
    AUDIENCE_GOOMBA,        // 0xA, 10
    AUDIENCE_PIRANHA_PLANT, // 0xB, 11
    AUDIENCE_LUIGI,         // 0xC, 12
    AUDIENCE_MAX            // 0xD, 13
} AudienceMemberType;
#pragma enumsalwaysint on

typedef struct StarPowerInfo {
    struct BattleWeapon* mWeapon; // 0x0
    f32 mSpAcSuccessMultiplier;   // 0x4
    s8 mStylishCommandMultiplier; // 0x8
    s8 field_0x9;                 // 0x9
    s8 mBingoSlotChance;          // 0xA
    s8 field_0xB;                 // 0xB
} StarPowerInfo;

typedef struct BattleAudienceMember {
    s32 flags;                     // 0x0
    s32 field_0x4;                 // 0x4
    u8 field_0x8[0x19 - 0x8];      // 0x8
    u8 status;                     // 0x19
    u8 texIndex;                   // 0x1A, texture index for the current animation frame (TEXGetGXTexObjFromPalette)
    u8 type;                       // 0x1B, AudienceMemberType
    s16 itemId;                    // 0x1C, -1 is invalid
    u8 field_0x1E[0x20 - 0x1E];    // 0x1E
    s32** animTable;               // 0x20, set in BattleAudience_Entry_Sub
    s32* animCur;                  // 0x24, animation in use (BattleAudience_SetAnim)
    s32* animPos;                  // 0x28, cursor in the command list of animCur
    s32 animWait;                  // 0x2C, frames left before the next command runs
    u8 field_0x30[0x40 - 0x30];    // 0x30
    u8 animDepth;                  // 0x40, index into the 0x34 table of the animation script
    u8 colorR;                     // 0x41
    u8 colorG;                     // 0x42
    u8 colorB;                     // 0x43
    u8 colorA;                     // 0x44, 0xFF after a reset
    u8 field_0x45[0x48 - 0x45];    // 0x45
    Vec position;                  // 0x48
    Vec scale;                     // 0x54, drawn scale: baseSize times the texture size (BattleAudienceDispAudience)
    Vec baseSize;                  // 0x60, reset to 1; kinopio squashes and stretches x and y
    Vec drawOffset;                // 0x6C, added to the quad corners (BattleAudienceDispAudience); kinopio sways x and lifts y
    Vec velocity;                  // 0x78, added to position each frame (kinopio); reset to 0
    Vec accel;                     // 0x84, added to velocity each frame (kinopio); reset to 0
    Vec normal;                    // 0x90, rotated by the member's matrix in the draw code (0,0,-1) after a reset
    Vec mRotation;                 // 0x9C
    Vec mRotationOffset;           // 0xA8
    Vec mHomePosition;             // 0xB4
    u8 field_0xC0[0xCC - 0xC0];    // 0xC0
    Vec jumpOffset;                // 0xCC, y is the hop height (clamped to 0 at the bottom)
    u8 field_0xD8[0xDC - 0xD8];    // 0xD8
    f32 jumpVelY;                  // 0xDC, added to jumpOffset.y each frame
    u8 field_0xE0[0xE4 - 0xE0];    // 0xE0
    f32 jumpAccelY;                // 0xE4, added to jumpVelY each frame
    f32 field_0xE8;                // 0xE8
    u8 field_0xEC[0xF8 - 0xEC];    // 0xEC
    s32 interpFramesLeft;          // 0xF8
    s32 interpFramesTotal;         // 0xFC
    f32 startPosX;                 // 0x100
    f32 targetPosX;                // 0x104
    f32 startPosY;                 // 0x108
    f32 targetPosY;                // 0x10C
    f32 startPosZ;                 // 0x110
    f32 targetPosZ;                // 0x114
    Vec interpDelta;               // 0x118
    f32 interpGravity;             // 0x124
    s32 interpType;                // 0x128
    union {
        s32 timer;                 // 0x12C, countdown: BULKY_BOBOMB fuse and KOOPA (battle_audience.c sets 5); kinopio shell clears it
        s32 targetId;              // 0x12C, member index a Pakkun eats (kinopio)
        s32 evtId;                 // 0x12C, evt id polled with evtCheckID (fall objects, nozzle work; kinopio)
        f32 bobAngle;              // 0x12C, radians, AUDIENCE_BOO bob phase (kinopio)
    };
    f32 bobOffsetY;                // 0x130, 5 * sin(bobAngle) for AUDIENCE_BOO; added to the quad's y in the draw code
} BattleAudienceMember;

typedef struct BattleAudienceItem {
    u32 flags;               // 0x0, bit 0 active, bit 1 drawn
    s32 state;               // 0x4, step of BattleAudienceItemCtrlProcess's switch
    s32 timer;               // 0x8, frames left in the current state (set to the flight length on launch)
    s32 memberId;            // 0xC, audience member it was thrown at, -1 invalid
    ItemType mItemType;      // 0x10
    Vec pos;                 // 0x14
    f32 scale;               // 0x20, x and y scale of the icon
    f32 angleZ;              // 0x24, z rotation in degrees
    Vec vel;                 // 0x28, per-frame velocity, accel is added to it
    Vec accel;               // 0x34
    BattleUnit* target;      // 0x40, the unit the item is thrown toward
    s32 hitResult;           // 0x44, nonzero enters the outcome branch of case 3 (2 goes straight to case 4)
} BattleAudienceItem;

typedef struct BattleAudienceStar {
    u32 flags;                  // 0x0, bit 0 set while the star slot is in use
    s32 state;                  // 0x4, step of the switch in BattleAudienceApSrcCtrlProcess
    Vec pos;                    // 0x8, drawn at pos.x/pos.y in BattleAudienceDispApSrc
    Vec randOffset;             // 0x14, random values set by BattleAudienceApSrcEntry
    Vec startPos;               // 0x20
    Vec endPos;                 // 0x2C
    Vec scale;                  // 0x38, used by PSMTXScale in BattleAudienceDispApSrc
    s32 frame;                  // 0x44
    s32 duration;               // 0x48, frames for the current state
    f32 radius;                 // 0x4C
    f32 radiusStart;            // 0x50
    f32 angle;                  // 0x54, radians
    f32 angleStep;              // 0x58, radians per frame
    u8 alpha;                   // 0x5C
    u8 field_0x5D[0x60 - 0x5D]; // 0x5D
} BattleAudienceStar;

// TODO: verify variable names
typedef struct BattleAudienceSound {
    s32 flags;                     // 0x0, bit 0 set while the sound is in use
    s32 sfxId;                     // 0x4, from BattleAudienceSound1/2, -1 when it failed to start
    u8 variant;                    // 0x8, 1-8, chosen by the caller before the sound starts
    u8 field_0x9[0xC - 0x9];       // 0x9
    s32 timeLeft;                  // 0xC, frames left, -1 when untimed; fades over the last mFadeoutTime frames
    s32 mFadeoutTime;              // 0x10
    s32 mForceFadeTimer;           // 0x14
    s32 mForceFadeLength;          // 0x18
    u8 mBaseSfxVolume;             // 0x1C
    u8 mCurrentBaseSfxVolume;      // 0x1D
    u8 mVolumeMultiplier;          // 0x1E
    u8 mFadeStartVolumeMultiplier; // 0x1F
    u8 mFadeEndVolumeMultiplier;   // 0x20
    u8 field_0x21[3];              // 0x21, TODO verify padding
} BattleAudienceSound;

typedef struct BattleAudienceWindow {
    u8 mActive;                 // 0x0, TODO "bool"?
    u8 field_0x1[0x8 - 0x1];    // 0x1
    f32 posX;                   // 0x8, from slide: -500 closed, -280 open
    f32 posY;                   // 0xC, -160 each frame
    u8 field_0x10[0x14 - 0x10]; // 0x10
    s32 slide;                  // 0x14, 0-15, moves toward open while mActive
    f32 count;                  // 0x18, number to display for audience count
} BattleAudienceWindow;

/*
typedef struct BattleAudience {
        s32 flags; //0x0
        EventEntry* evt; //0x4
        u8 field_0x8[0xC - 0x8]; //0x8
        FileEntry* normalTex; //0xC
        FileEntry* guestTex[2]; //0x10
        AudienceMemberType guestType[2]; //0x18
        u8 field_20[0x1BC - 0x20]; //0x20
        BattleAudienceMember members[200]; //0x1BC
        BattleAudienceItem items[100]; //0xF25C
        BattleAudienceStar starpower[100]; //0x10E7C
        BattleAudienceSound sounds[24]; //0x133FC
        BattleAudienceWindow window; //0x1375C
        f32 mAudienceDeltaMultiplier; //0x13778
        f32 mTargetAudienceCount; //0x1377C
        f32 mAddedAudienceCount; //013780
        s32 mCurrentAudienceIntCount; //0x13784, TODO verify name
        s32 mCurrentAudienceIntCountRight; //0x13788, TODO verify name
        s32 mCurrentAudienceIntCountLeft; //0x1378C, TODO verify name
        s32 mMaxAudience; //0x13790
        u8 field_0x13794[0x137C8 - 0x13794]; //0x13794
        StarPowerInfo* impendingBonuses; //0x137C8
        s32 crowdPleasedStreak; //0x137CC
        s32 crowdDispleasedStreak; //0x137D0
        s32 mNumStylishCommandsThisAttack; //0x137D4
        u8 field_0x137D8[0x137DC - 0x137D8]; //0x137D8
        s32 mCheckPhaseReactionState; //0x137DC, TODO: rename
        u8 field_0x137E0[0x138BC - 0x137E0]; //0x137E0
        s32 mItemOnMemberId; //0x138BC, TODO better name
        Vec mItemOnMemberPos; //0x138C0, TODO better name?
        u8 field_0x138CC[0x13908 - 0x138CC]; //0x138CC
        s32 mTurnEndPhaseEventChance; //0x13908, TODO verify name
        u8 field_0x1390C[0x13910 - 0x1390C]; //0x1390C
        BOOL excited; //0x13910, if toads are cheering
} BattleAudience;*/

typedef struct BattleAudience {
    s32 flags;                         // 0x0
    EventEntry* event;                 // 0x4
    s32 apStarSfxId;                  // 0x8, psnd id of the AP star loop
    FileEntry* normalTex;              // 0xC
    FileEntry* guestTex[2];            // 0x10
    AudienceMemberType guestType[2];   // 0x18
    u8 field_1A[0x1BC - 0x1A];         // 0x1A
    BattleAudienceMember members[200]; // 0x1BC
    BattleAudienceItem items[100];     // 0xF25C
    BattleAudienceStar starpower[100]; // 0x10E7C
    BattleAudienceSound sounds[24];    // 0x133FC
    BattleAudienceWindow window;       // 0x1375C
    f32 mAudienceDeltaMultiplier;      // 0x13778
    f32 mTargetAudienceCount;          // 0x1377C
    f32 mAddedAudienceCount;           // 013780
    s32 mCurrentAudienceIntCount;      // 0x13784, TODO verify name
    s32 mCurrentAudienceIntCountRight; // 0x13788, TODO verify name
    s32 mCurrentAudienceIntCountLeft;  // 0x1378C, TODO verify name
    s32 mMaxAudience;                  // 0x13790
    s32 typeWeights[12];               // 0x13794, per-type weights for picking members (BattleAudienceSettingAudience)
    s32 apGauge;                       // 0x137C4, AP meter: BattleAudienceCheer clamps it to 800 (0x320), the AP star drains it
    StarPowerInfo* impendingBonuses;   // 0x137C8
    s32 crowdPleasedStreak;            // 0x137CC, capped at 2
    s32 crowdDispleasedStreak;         // 0x137D0, capped at 5
    s32 mNumStylishCommandsThisAttack; // 0x137D4
    u8 field_137D8[0x137DC - 0x137D8]; // 0x137D8
    s32 mCheckPhaseReactionState;      // 0x137DC, TODO: rename
    s32 phaseStep;                     // 0x137E0, 0, 5, 10 or 15 through the phase event steps
    s32 presentItemNo;                 // 0x137E4, item being used (BattleAudience_SetPresentItemNo)
    s32 presentItemType;               // 0x137E8
    s32 presentTargetUnitId;           // 0x137EC
    s32 itemThrowCount;                // 0x137F0, items thrown so far by BattleAudienceCtrlProcess
    s32 itemThrowMax;                  // 0x137F4, set by BattleAudienceSetThrowItemMax (0-3, or 10000)
    s32 itemCombo;                     // 0x137F8, caught items in a row (capped at 3)
    u8 field_137FC[0x138BC - 0x137FC]; // 0x137FC, weapon table (BattleWeapon*, see battle_attack_audience.c)
    s32 mItemOnMemberId;               // 0x138BC, TODO better name
    Vec mItemOnMemberPos;              // 0x138C0, TODO better name?
    s32 phaseEvtList[14];              // 0x138CC
    s32 phaseEvtCount;                 // 0x13904, entries in phaseEvtList
    s32 mTurnEndPhaseEventChance;      // 0x13908, TODO verify name
    s32 joyLevel;                      // 0x1390C, 0-4, picks the BattleAudienceJoy_Sub numbers for case 3
    BOOL excited;                      // 0x13910, if toads are cheering
} BattleAudience;

// Struct views used by BattleAudienceSettingAudience so MWCC emits the paired
// lwzu/stwu const-table copy loops (it does not for memcpy).
typedef struct BattleAudienceDeltaMultiplierTable {
    f32 v[65];
} BattleAudienceDeltaMultiplierTable;

typedef struct BattleAudienceTypeWeights {
    s32 v[11];
} BattleAudienceTypeWeights;

// Struct views for the haiti rate tables copied by BattleAudience_NoUsedFCHaitiRand.
typedef struct BattleAudienceHaitiRateTable {
    s32 v[10];
} BattleAudienceHaitiRateTable;

typedef struct BattleAudienceHaitiOffsetTable {
    s32 v[20];
} BattleAudienceHaitiOffsetTable;

extern f32 float_0p5_8041e708;
// .sdata2 constants and the haiti table used by battle_audience.c (data objects of this unit).
extern f32 float_12p5_8041e688;
extern f32 float_304_8041e6cc;
extern f32 float_240_8041e6d0;
extern s32 haiti_data[][10];

BattleAudience* BattleAudienceBaseGetPtr(void);
BattleAudienceMember* BattleAudienceGetPtr(s32 id);
BattleAudienceItem* BattleAudienceItemGetPtr(s32 id);
BattleAudienceWindow* BattleAudienceWinGetPtr(void);
BattleAudienceSound* BattleAudienceSoundGetPtr(s32 id);
void BattleAudience_Init(void);
void BattleAudience_Main(void);
void BattleAudience_ActInit(void);
void BattleAudience_PerAct(void);
void BattleAudience_Case_FirstAttack_Good(void);
void BattleAudience_Case_FirstAttack_Bad(void);
BOOL BattleAudience_CheckReaction(void);
void BattleAudience_PerPhase(u32 value);
BOOL BattleAudience_CheckReactionPerPhase(void);
void BattleAudience_Disp(void);
void BattleAudience_End(void);
BOOL BattleAudience_Entry(s32 index, AudienceMemberType type, u8 status);
void BattleAudience_Delete(s32 index);
void BattleAudience_SetAnim(s32 id, s32 animId, BOOL force);
void BattleAudience_GetPosition(s32 index, f32* x, f32* y, f32* z);
void BattleAudience_GetHomePosition(s32 index, f32* x, f32* y, f32* z);
void BattleAudience_GetRotate(s32 index, f32* x, f32* y, f32* z);
void BattleAudience_SetPosition(s32 index, f32 x, f32 y, f32 z);
void BattleAudience_SetRotate(s32 index, f32 x, f32 y, f32 z);
void BattleAudience_SetRotateOffset(s32 index, f32 x, f32 y, f32 z);
void BattleAudience_GetItemOn(s32* memberId, f32* x, f32* y, f32* z, ItemType* itemType);
void BattleAudience_GetItemOn2(s32* memberId, f32* x, f32* y, f32* z, ItemType* itemType);
void BattleAudience_SetTarget(s32 index);
void BattleAudience_Attack(s32 index);
void BattleAudience_ChangeStatus(s32 index, s32 status);
BOOL BattleAudience_GetExist(s32 id);
BOOL BattleAudience_GetSysCtrl(s32 index);
BOOL BattleAudience_GetWaiting(s32 index);
s32 BattleAudience_HaitiRandForFallObject(void);
void BattleAudience_Case_FallObject_Aud(s32 index, u8 type);
void BattleAudience_Case_FallObject_Stage(void);
s32 BattleAudience_GetAudienceNum(void);
void BattleAudience_Case_Escape(void);
void BattleAudience_Case_Escape_Bad(void);
void BattleAudience_Case_Escape_Good(void);
FileEntry* tplRead(const char* path);
BOOL check_exe_phase_evt_status(s32 index, s32 type);
void BattleAudienceSettingAudience(void);
void BattleAudienceGuestTPLRead(s32 index, AudienceMemberType member, const char* tplName);
void BattleAudienceCtrlProcess(void);
BOOL BattleAudienceItemOn(s32 index, u16 itemId, s32 numItems);
void BattleAudienceSetThrowItemMax(void);
void BattleAudienceItemCtrlProcess(void);
void BattleAudienceApSrcCtrlProcess(void);
BOOL BattleAudienceApSrcEntry(void);
void BattleAudienceWinCtrlProcess(void);
void BattleAudienceGXInit(void);
void BattleAudienceAnimProcess(void);
void BattleAudienceDispAudience(CameraId camId, void* param);
void BattleAudienceDispItem(CameraId camId, void* param);
void BattleAudienceDispApSrc(CameraId camId, void* param);
void BattleAudienceDispWin(CameraId camId, void* param);
void BattleAudience_Entry_Sub(s32 index, s32 type, u8 status);
BOOL BattleAudience_GetAnimEnd(s32 index);
BOOL BattleAudience_GetFront(s32 index);
BOOL BattleAudience_GetEscapeChangeOK(s32 index);
s32 BattleAudience_GetAudienceNoFromOffset(s32 index, s32 column, s32 row);
s32 BattleAudience_NoUsedHaitiRand(void);
s32 BattleAudience_NoUsedFCHaitiRand(void);
s32 BattleAudience_GetPPAudienceNum(void);
s32 BattleAudience_GetPPAudienceNumKind(s32 type);
s32 BattleAudience_GetPPAudienceNum_R(void);
s32 BattleAudience_GetPPAudienceNum_L(void);
s32 BattleAudience_GetPPAudienceNum_RL_Sub(s32 offset);
BOOL BattleAudience_GetPPAudienceNum_Sub(s32 id);
void BattleAudienceAddTargetNumSub(f32 value);
void BattleAudienceAddTargetNum(f32 value, f32 delta);
void BattleAudienceAddPhaseEvtList(s32 evtId);
void BattleAudienceJoy(s32 a);
void BattleAudienceJoySACLecture(void);
void BattleAudienceJoy_Sub(s32 a, s32 b, s32 c, s32 d);
void BattleAudienceSoundCheer(s32 a, s32 b);
void BattleAudienceSoundBooingKind(s32 kind);
void BattleAudienceSoundBooing(void);
void BattleAudienceSoundGetInfo1(s32 a, s32 b, u8* c, Vec* d);
void BattleAudienceSoundGetInfo2(s32 index, Vec* pos);
s32 BattleAudienceSound1(const char* name, s32 a, s32 b);
s32 BattleAudienceSound2(const char* name, s32 index);
void BattleAudienceSoundStop(s32 index);
void BattleAudienceSoundRun(s32 kind);
void BattleAudienceSoundWhistle(void);
void BattleAudiencePuniAllEscape(void);
void BattleAudienceAddPuni(s32 num);
void BattleAudienceCheer(s32 a);
void BattleAudienceSoundClap(s32 a, s32 b);
s32 BattleAudienceDetectPakkunEatTargetSub2(s32 a, s32 b);
s32 BattleAudienceDetectPakkunEatTargetSub(s32 a, s32 b, s32 c);
s32 BattleAudienceNumToTargetSub(void);
void BattleAudienceSoundNoiseAlways(void);
void BattleAudienceSoundSetVol(s32 index, u8 a2, s32 a3);
void BattleAudienceSoundSetVolAll(u8 a2, s32 a3);
void BattleAudienceNumToTarget(void);
void BattleAudience_WinSetActive(u8 active);
void BattleAudienceSoundMain(void);
s32 BattleAudienceDetectPakkunEatTarget(s32 index);
void BattleAudienceSoundZZZ(s32 index);
void BattleAudienceSoundSing(s32 index);
void BattleAudienceSoundPakkunEat(s32 index);
void BattleAudienceSoundShell(s32 index);
void BattleAudienceSoundBombFire(s32 index);
void BattleAudienceSoundBombIgnite(s32 index, s32 a, s32 b);
void BattleAudienceSoundItemThrow(s32 kind, s32 index);
void BattleAudienceSoundSleep(s32 index);
void BattleAudience_Case_JumpNewRecord(void);
s32 BattleAudience_GetPresentItemNo(void);
s32 BattleAudience_GetPresentTargetUnitId(void);
void BattleAudience_SetPresentItemNo(s32 itemNo);
void BattleAudience_SetPresentItemType(s32 value);
void BattleAudience_SetPresentTargetUnitId(s32 value);
void BattleAudience_Case_GreatVictory(void);
void BattleAudience_Case_FastVictory(f32 value);
void BattleAudience_Case_HaikeiSet(void);
void BattleAudience_Case_MarioDanger(void);
void BattleAudience_Case_MarioPinch(void);
BOOL BattleAudienceSoundCheck(s32 index);
void BattleAudienceSoundHandBeat(void);
void BattleAudienceSoundCallKind(s32 kind);
void BattleAudience_Case_AcrobatNoTry(void);
void BattleAudienceJoyEnding(void);
BattleUnit* BattleAudienceDetectTargetAll(void);
BattleUnit* BattleAudienceDetectTargetPlayer(void);
s32 BattleAudienceAddAudienceNum(s32 num);
void BattleAudienceSoundCheerKind(s32 kind, s32 a, s32 b);
void BattleAudienceSoundWhistleKind(s32 kind);
void BattleAudienceNoiseMain(void);
void BattleAudience_ApRecoveryBuild(StarPowerInfo* info);
void BattleAudience_Case_Appeal(BattleUnit* unit);
s32 BattleAudience_GetPresentItemType(void);
void BattleAudience_Case_AcrobatGood(void);
void BattleAudience_Case_AcrobatBad(void);
void BattleAudience_Case_ActionCommandGood(void);
void BattleAudience_Case_ActionCommandBad(void* arg);
void BattleAudience_Case_TurnEnd(void);
void BattleAudience_Case_Countered(void);
void BattleAudience_Case_EnemyDamage(void);
void BattleAudience_Case_EnemyNoDamage(void);
void BattleAudience_Case_GuardBad(void);
void BattleAudience_Case_GuardGood(void);
void BattleAudience_Case_MarioBigDamage();
void BattleAudience_Case_PartyDown();
void BattleAudience_Case_KillEnemy();
void BattleAudience_Case_FinalAttack();
extern s32 msg_puni_all_enter[9];
extern s32 msg_pansy_sing[9];
extern s32 msg_heavy_bomb_fire[9];
s32 BattleAudienceSound0(const char* name);
