#pragma once

#include <dolphin/mtx.h>
#include "driver/dispdrv.h"

#pragma enumsalwaysint off
typedef enum MarioMotion {
    MARIO_MOTION_STAY,         // 0x0
    MARIO_MOTION_WALK,         // 0x1
    MARIO_MOTION_DASH,         // 0x2
    MARIO_MOTION_JUMP,         // 0x3
    MARIO_MOTION_JUMP_NPC,     // 0x4
    MARIO_MOTION_JUMP_SW,      // 0x5
    MARIO_MOTION_JUMP_STAND,   // 0x6
    MARIO_MOTION_JUMP_2,       // 0x7
    MARIO_MOTION_JUMP_3,       // 0x8
    MARIO_MOTION_JUMP_SMALL,   // 0x9
    MARIO_MOTION_FALL,         // 0xA
    MARIO_MOTION_FALL_2,       // 0xB
    MARIO_MOTION_UPSTAIRS,     // 0xC
    MARIO_MOTION_LAND,         // 0xD
    MARIO_MOTION_TALK,         // 0xE
    MARIO_MOTION_GET_ITEM,     // 0xF
    MARIO_MOTION_HIP,          // 0x10
    MARIO_MOTION_HIP_2,        // 0x11
    MARIO_MOTION_HAMMER,       // 0x12
    MARIO_MOTION_HAMMER_2,     // 0x13
    MARIO_MOTION_JABARA,       // 0x14
    MARIO_MOTION_SLIT,         // 0x15
    MARIO_MOTION_ROLL,         // 0x16
    MARIO_MOTION_KAZE,         // 0x17
    MARIO_MOTION_PLANE,        // 0x18
    MARIO_MOTION_SHIP,         // 0x19
    MARIO_MOTION_YOSHI,        // 0x1A
    MARIO_MOTION_CLOUD,        // 0x1B
    MARIO_MOTION_VIVIAN,       // 0x1C
    MARIO_MOTION_DOKAN,        // 0x1D
    MARIO_MOTION_GRASP,        // 0x1E
    MARIO_MOTION_DAMAGE,       // 0x1F
    MARIO_MOTION_DAMAGE_TOGE,  // 0x20
    MARIO_MOTION_BOTTOMLESS,   // 0x21
    MARIO_MOTION_FORCE_RESET,  // 0x22
    MARIO_MOTION_SHADOW,       // 0x23
    MARIO_MOTION_PARTY_USE,    // 0x24
    MARIO_MOTION_KPA_SWIM,     // 0x25
    MARIO_MOTION_KPA_POW_UP,   // 0x26
    MARIO_MOTION_KPA_POW_DOWN, // 0x27
    MARIO_MOTION_DUMMY,        // 0x28
    MARIO_MOTION_MOTION_MAX = 0xFFFF
} MarioMotion;
#pragma enumsalwaysint on

#pragma enumsalwaysint off
typedef enum MarioFlags {
    MARIO_FLAG_ACTIVE = (1 << 0),
    MARIO_FLAG_CTRL_OFF = (1 << 1),
    MARIO_FLAG_CTRL_OFF_2 = (1 << 2),
    MARIO_FLAG_KEY_OFF = (1 << 3),
    MARIO_FLAG_FORCED_MOVEMENT = (1 << 5),
    MARIO_FLAG_USE_FORCE_DIRECTION = (1 << 6),
    MARIO_FLAG_STOP_AIR_MOVEMENT = (1 << 7),
    MARIO_FLAG_FLOORS_DISABLED = (1 << 8),
    MARIO_FLAG_FORCE_WALL = (1 << 9),
    MARIO_FLAG_WALLS_DISABLED = (1 << 10),
    MARIO_FLAG_HAMMER_HIT_WALL = (1 << 11),
    MARIO_FLAG_PRE_BATTLE_PHYSICS = (1 << 12),
    MARIO_FLAG_IS_CARRYING_BOBBERY = (1 << 13),
    MARIO_FLAG_ITEM_GET_DISABLED = (1 << 14),
    MARIO_FLAG_IS_PUSHING = (1 << 15),
    MARIO_FLAG_IS_JUMPING = (1 << 16),
    MARIO_FLAG_IS_FALLING = (1 << 17),
    MARIO_FLAG_IS_STEPPING = (1 << 18),
    MARIO_FLAG_HAS_INPUT_JUMP = (1 << 19),
    MARIO_FLAG_PAPER_MODE = (1 << 20),
    MARIO_FLAG_GONE_THROUGH_BARS = (1 << 21),
    MARIO_FLAG_PARTY_WALK_DISABLED = (1 << 22),
    MARIO_FLAG_SLIT_ABILITY_DISABLED = (1 << 23),
    MARIO_FLAG_TUBE_MODE = (1 << 24),
    MARIO_FLAG_BG_MODE = (1 << 25),
    MARIO_FLAG_IN_SHALLOW_WATER = (1 << 26),
    MARIO_FLAG_IS_TOUCHING_CEILING = (1 << 27),
    MARIO_FLAG_CHG_MOT_DISABLED = (1 << 28),
    MARIO_FLAG_IS_SHADOW_MARIO = (1 << 30),
    MARIO_FLAG_IS_8_BIT = (1 << 31)
} MarioFlags;
#pragma enumsalwaysint on

#pragma enumsalwaysint off
typedef enum MarioTrigFlags {
	MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION = (1 << 0),
	MARIO_TRIG_FLAG_USE_PARTNER = (1 << 1),
	MARIO_TRIG_FLAG_SHIP_PLANE_EVT_ON = (1 << 2),
	MARIO_TRIG_FLAG_DISP_DIR_CORRECTION = (1 << 8),
	MARIO_TRIG_FLAG_DISP_DIR_LEFT = (1 << 9),
	MARIO_TRIG_FLAG_DISP_DIR_RIGHT = (1 << 10),
	MARIO_TRIG_FLAG_CHG_ANIM = (1 << 12),
	MARIO_TRIG_FLAG_CHG_PAPER_ANIM = (1 << 14),
	MARIO_TRIG_FLAG_SET_JUMP_PARAMS = (1 << 16),
	MARIO_TRIG_FLAG_DISABLE_JUMP_KEMURI = (1 << 17),
	MARIO_TRIG_FLAG_JABARA_CHK = (1 << 20),
	MARIO_TRIG_FLAG_JABARA_MOVE_CHK = (1 << 21),
	MARIO_TRIG_FLAG_START_JABARA_SPIN = (1 << 22),
	MARIO_TRIG_FLAG_DISABLE_UNUSED_INPUT = (1 << 24),
	MARIO_TRIG_FLAG_DISABLE_TRIGGERS = (1 << 25),
	MARIO_TRIG_FLAG_DISABLE_SUB_STICK = (1 << 26),
	MARIO_TRIG_FLAG_DISABLE_STICK = (1 << 27),
	MARIO_TRIG_FLAG_DISABLE_PARTY_SLIT = (1 << 30)
} MarioTrigFlags;
#pragma enumsalwaysint on

#pragma enumsalwaysint off
typedef enum PeachFlags {
    PEACH_FLAG_AFTERSHOWER = (1 << 4),
    PEACH_FLAG_INVISIBLE = (1 << 5),
    PEACH_FLAG_HAPPY = (1 << 6),
    PEACH_FLAG_UNEASY = (1 << 7),
    PEACH_FLAG_ANGRY = (1 << 8),
    PEACH_FLAG_ORDINARY = (1 << 9),
    PEACH_FLAG_CARRY = (1 << 10),
    PEACH_FLAG_STS_MASK = 0x7F0,
    PEACH_FLAG_TRANSFORMED = (1 << 13)
} PeachFlags;
#pragma enumsalwaysint on

// TODO: US struct is bigger, 0x2F8 vs 0x2E0
struct KpaWork;

typedef struct MarioWork {
    u32 flags;                     // 0x0
    s32 dispFlags;                 // 0x4
    u32 field_0x8;                 // 0x8
    u32 trigFlags;                 // 0xC
    s32 peachFlags;                // 0x10, PeachFlags (peach.c, mario.c)
    s32 kpaStageFlags;             // 0x14, koopa stage bits from kpaPreInit's map: 1 the 2D stages kpa_02..06 (kpaMain runs
                                   //       kpa2DMain; kpaGetStageViewType returns !bit), 2 kpa_03 (kpaGetStageType)
    const char* animName;          // 0x18
    const char* paperAnimName;     // 0x1C
    s32 field_0x20;                // 0x20
    u8 field_0x24[0x28 - 0x24];    // 0x24
    u16 poseTime;                  // 0x28, paper pose frame passed to animPoseSetLocalTime; every reader loads it unsigned
    u8 field_0x2A[0x2E - 0x2A];    // 0x2A
    MarioMotion currMotionId;      // 0x2E
    MarioMotion prevMotionId;      // 0x30, TODO check
    u8 field_0x32[0x34 - 0x32];    // 0x32
    u32 frameCount;                // 0x34, bumped at the end of every marioMain/peach/koopa main step; marioUpdateBlurPos records on odd counts
    s8 ctrlOffCount;               // 0x38, marioCtrlOff nesting count (flag 0x2); marioCtrlOn returns it to 0
    s8 keyOffCount;                // 0x39, marioKeyOff nesting count (flag 0x8); marioKeyOn returns it to 0
    s8 hammerSfxIdx;               // 0x3A, mot_hammer: index of the badge/sound pair in se_data, stepped by 2 modulo 10
    s8 partyUseTimer;              // 0x3B, set to 5 when the X press starts a partner use, cleared when it ends (mario_party, mario_motion)
    s8 characterId;                // 0x3C
    s8 colorId;                    // 0x3D
    s8 dokanType;                  // 0x3E, evt_mario_paper_pose_dokan's type, set before marioChgMot(MARIO_MOTION_DOKAN);
                                   //       mot_dokan and partyDokanInit switch on it
    u8 wallPushCount;              // 0x3F, frames pressed against a (non-bero) front wall, capped at 20: at 20 the wall becomes
                                   //       hitobjPush (marioCheckWall); marioSearchFrontWall_staying clears it
	s8 wMotionTimer;               // 0x40
    s8 rubCount;                   // 0x41, stick rubs counted by marioGetRub (capped at 20) while charging: mot_jabara 2 and 3,
                                   //       mot_hammer2 2, 3 and 4 raise chargeLevel
    s8 chargeLevel;                // 0x42, charge reached: mot_jabara 0, 2 or 3 (picks the launch's jump integrator values),
                                   //       mot_hammer2 1-3 (picks the release pose and swing)
    s8 dispHalfTurn;               // 0x43, 0/1 half-turn flag: mot_roll adds 180 to its display target, mot_hammer sets it from marioGetRub's side
    s32 currSubMotionId;           // 0x44
    s32 multiTimer;                // 0x48, ???
	s16 mutekiTime;                // 0x4C, marioSetMutekiTime (frames)
	s16 forceMoveTimer;            // 0x4E
    s16 airTimer;                  // 0x50
    s16 airTimerPeak;              // 0x52, airTimer when the jump peaks (marioJump, koopa_motion), cleared on landing; marioBoots and marioChkHipAttack test it
    s16 airTimerPrev;              // 0x54, airTimer copied at the end of the frame (mario.c, read by mario_kemuri)
    u8 field_0x56[0x58 - 0x56];    // 0x56
    s16 field_0x58;                // 0x58
    s16 stayTimer;                 // 0x5A, mot_stay: frames standing (cleared on entry); after a second the idle poses are chosen
    s16 camZoomOffDelay;           // 0x5C, frames until marioCamZoomOff ends the zoom (marioCamZoomOffReq sets 32)
    s16 camZoomOffTimer;           // 0x5E, frames of the zoom-out evt_cam3d_evt_off runs; dispFlags 0x100000 clears at 0 (marioMain)
    s16 unk60;                     // 0x60
    s16 camZoomOffTime;            // 0x62, length of that zoom-out in 10 ms units (marioCamZoomOffReq 220, marioCamZoomOffReq2 msec/10)
    f32 camZoomWeight;             // 0x64, camdrv's zoom blend: scale = weight * camZoom + (1 - weight); copied from camZoomWeightCur
    f32 camZoom;                   // 0x68, camdrv's zoom factor (marioResetCamShift: 2)
    Vec camZoomFocusOfs;           // 0x6C, camdrv zooms on position + this
    s16 noHitEnemyCt;              // 0x78, koopa frames left without enemy hits: kpaPowDown sets sysMsec2Frame(3000), kpaNoHitEnemyCheck returns it, kpa_nohitenemy_ct_clear zeroes it
    s16 itemGetDisableTimer;       // 0x7A, frames left on the MARIO_FLAG_ITEM_GET_DISABLED block: marioItemGetDisable sets 20, marioItemGetChk counts down and clears the flag
    f32 jumpVelY;                  // 0x7C, per-frame Y step added to position.y (evt_mario, mot_roll, mot_ship)
    f32 jumpAccelY;                // 0x80, added to jumpVelY each frame
    f32 jumpJerkY;                 // 0x84, added to jumpAccelY each frame
    f32 jumpJerkStepY;             // 0x88, added to jumpJerkY each frame (0 unless a jump sets it)
    Vec position;                  // 0x8C
    Vec dispOfs;                   // 0x98, display offset: marioDisp draws at position + dispOfs + dispOfs2 (the blur copies use blurPos)
    Vec dispOfs2;                  // 0xA4, second display offset, summed with dispOfs (mario.c blur position)
    Vec dispPivot;                 // 0xB0, display translation: -pivot before the display rotations, +pivot after them
    Vec dispRot;                   // 0xBC, display rotation in degrees (x, z); y is not read: the y rotation comes from dispDir - targetYaw
    Vec scale;                     // 0xC8
    Vec lastGroundPos;             // 0xD4, position copied whenever airTimer is 0 (mot_jump, mot_roll)
    Vec field_0xE0;                // 0xE0, bgTmp source in mot_damage
    Vec camFollowPos;              // 0xEC, the point the 3D camera follows: camdrv copies x/z into marioPos and eases y at camFollowRate
    Vec camCollision;              // 0xF8, marioUpdateCamCollision
    u8 unk104[0x110 - 0x104];      // 0x104
    Vec prevPosition;              // 0x110, copy of position taken each frame (mario.c, peach.c); party_vivian reads its y
    Vec jumpFromPos;               // 0x11C, position at takeoff (mot_jump, mot_stay, mot_jabara, mot_roll): landings compare y with it,
                                   //       evt_mario and mot_ship measure the jump distance from it, mario_hit clamps to its y
    f32 field_0x128;               // 0x128, distance from field_0x11C at landing (evt_mario)
    f32 peakYpos;                  // 0x12C, marioSetPeakYpos
    f32 rollScaleY;                // 0x130, Y scale of the roll squash/stretch state machine (mot_roll, mario.c PSMTXScale)
    CameraId camId;                // 0x134
    f32 camShiftSideTarget;        // 0x138, camera shift sideways from Mario (marioResetCamShift; mot_plane, party_cloud)
    f32 camShiftSideCur;           // 0x13C, eased toward camShiftSideTarget at camShiftRate.x (marioUpdateCam)
    f32 camShiftUpTarget;          // 0x140, camera shift up (marioResetCamShift; mot_plane)
    f32 camShiftUpCur;             // 0x144, eased toward camShiftUpTarget at camShiftRate.y
    f32 camZoomWeightTarget;       // 0x148, zoom weight wanted (marioResetCamShift; mot_jabara, mot_roll, mot_plane, mot_slit)
    f32 camZoomWeightCur;          // 0x14C, eased toward camZoomWeightTarget at camShiftRate.z, published as camZoomWeight
    Vec camShiftRate;              // 0x150, easing rates of the three above (marioResetCamShiftRate)
    f32 camFollowRate;             // 0x15C
    f32 prevCamPosX;               // 0x160, camera x at the last marioCamMain (mario_cam sets dispFlags 0x20 while it is unchanged)
    f32 field_0x164;               // 0x164, prevPosition.y - position.y, stored by mario_hit (marioChkGndRollProbe); no reader in the tree
    s32 screenX;                   // 0x168, marioGetScreenPos (GXProject sx, sy, sz) of position, each frame (marioMove);
    s32 screenY;                   // 0x16C, marioChkInScreen tests x and y
    s32 screenZ;                   // 0x170
    Vec riderDelta;                // 0x174, movement of the object Mario stands on (evt_mapgrp_translate2)
	f32 baseSpeed;                 // 0x180, same on US
    f32 mBaseWalkSpeed;            // 0x184
    f32 mBaseDashSpeed;            // 0x188, same on US
    f32 field_0x18C;               // 0x18C
	f32 controlStickSensitivity;   // 0x190
	f32 controlStickAngle;         // 0x194
    f32 targetYaw;                 // 0x198
    f32 moveDir;                   // 0x19C, move direction (toMovedir), set beside directionView; mario_hit sets it to wallYaw on wall contact
	f32 directionView;             // 0x1A0
	f32 forceDir;                  // 0x1A4, evt_mario_set_force_dir (with MARIO_FLAG_USE_FORCE_DIRECTION)
    f32 dispDir;                   // 0x1A8, evt_mario_get_dispdir
    f32 dispReqDir;                // 0x1AC, requested display direction (get_mario_dispReqDir; party moves aim by it)
    u8 field_0x1B0[0x1B4 - 0x1B0]; // 0x1B0
    Vec wPlayerCollisionBox;       // 0x1B4 JP, 0x1B8 US
    f32 floorY;                    // 0x1C0, floor height under Mario from searchUnder (-3000 when none; shadows)
    f32 stepUpY;                   // 0x1C4, top of the 5-11 unit step marioChkFrontStep found (mario_hit); MARIO_MOTION_UPSTAIRS
                                   //       (mot_jump) and mot_roll aim jumpVelY at it
    f32 wallDist;                  // 0x1C8, largest front-wall distance held for wallHoldTimer frames (marioCheckWall)
    f32 wallYaw;                   // 0x1CC, the wall angle that goes with wallDist (marioCheckWall)
    s32 wallHoldTimer;             // 0x1D0, counts down from 14 before wallDist can drop (marioCheckWall)
    void* touchNpc;                // 0x1D4, returned by marioGetTouchNpcPtr
    struct NpcEntry* sideHitNpc;   // 0x1D8, NPC whose side Mario pushes into (npcCheckHitMarioSide); npcMain clears it each frame;
                                   //       mot_roll damps the roll while it is set
    struct HitEntry* actionHitObj; // 0x1DC, front hit for an A press: mario_hit sets it on the press frame, mario_motion's action check reads it (mario_balloon, party_yoshi and mot_ship touch it raw)
    struct HitEntry* hitobjFront;  // 0x1E0, front-wall HitEntry from searchFrontWall (marioCheckWall, mot_roll)
	struct HitEntry* hitobjStandOn; // 0x1E4, 0x1E8 US HitEntry
	struct HitEntry* hitobjJumpFrom; // 0x1E8, 0x1EC US HitEntry
	struct HitEntry* hitobjPush;   // 0x1EC JP, 0x1F0 US HitEntry
    struct HitEntry* hitobjWall;   // 0x1F0, front-wall HitEntry mario_hit's wall checks store on either branch (also when hitobjPush is not set), party_yoshi's too; unlike hitobjFront never swapped for hitobjBalloon; read by marioChkToge, mot_roll, mot_ship, koopa_motion and party_yoshi
    struct HitEntry* hitobjHead;   // 0x1F4, marioSearchHead result
    struct HitEntry* hitobjUnder;  // 0x1F8, marioSearchUnder result (mario_hit)
    struct HitEntry* hitobjHammer; // 0x1FC, hit the hammer struck: mot_hammer copies its hitEstimate target (0x200) at impact and the case-0x14 front ray stores its hit; cleared at each marioMotion; mobjdrv and casedrv test it for hammer triggers
    struct HitEntry* hitobjEstimate; // 0x200, mot_hammer: hitEstimate's target at the swing start, copied to hitobjHammer at impact
    struct HitEntry* hitobjDamageReturn; // 0x204, hit whose damage return position mot_damage uses
    struct HitEntry* hitobjStandOnPrev; // 0x208, hitobjStandOn copied at the end of the frame (mario.c)
    struct HitEntry* hitobjBalloon; // 0x20C, balloon hit from the front-wall search
    f32 frontWallDist;             // 0x210, searchFrontWall distance, reset to 1000 each check (marioCheckWall, dou10_yoko_yari)
    f32 slopeAngle;                // 0x214, angle of the ground normal from vertical (mario_hit, mot_roll)
    f32 slopeTiltX;                // 0x218, tilt about X: gndCentiPlain of the normal's z/y (mario_hit, mot_roll)
    f32 slopeTiltY;                // 0x21C, zeroed beside the other two (mario_hit)
    f32 slopeTiltZ;                // 0x220, tilt about Z: gndCentiPlain of the normal's x/y (mario_hit, mot_roll)
	f32 playerGravity;             // 0x224 JP, 228 US
    s32 paperAnimGroupId[3];       // 0x228 JP, 0x22C US
    u8 field_0x234[0x23C - 0x234]; // 0x234
    s32 paperPoseId;               // 0x23C JP, 0x240 US
    u8 field_0x240[0x241 - 0x240]; // 0x240
    s8 currPartySlotId[2];         // 0x241 JP, 0x245 US
    s8 prevPartyMemberId[2];       // 0x243 JP, 0x247 US
    u8 field_0x245;                // 0x245
    u16 buttonHeld;                // 0x246, GC pad button bits copied from gp->button by marioUpdateKeyData
    u16 buttonNew;                 // 0x248, gp->buttonNew; mot_roll also ORs 0x100/0x200 into it
    u16 buttonRepeat;              // 0x24A, gp->buttonRepeat
    u16 buttonUp;                  // 0x24C, gp->buttonUp
	s8 wStickDir1;                 // 0x24E JP, 0x252 US
	s8 wStickDir2;                 // 0x24F JP, 0x253 US
    s8 substickX;                  // 0x250, gp->substickX
    s8 substickY;                  // 0x251, -gp->substickY
    u8 triggerLeft;                // 0x252, gp->triggerLeft
    u8 triggerRight;               // 0x253, gp->triggerRight
    u8 unusedInput[0x258 - 0x254]; // 0x254, cleared with the MARIO_TRIG_FLAG_DISABLE_UNUSED_INPUT case
    u32 padDirs;                   // 0x258, DemoPad[0].dirs this frame
    u32 padDirsPressed;            // 0x25C, dirs newly set since the previous frame
    struct EffectEntry* balloonEff; // 0x260, the talk balloon effEntry (effMarioBalloonEntry, effDelete; mario_balloon)
    struct NpcEntry* balloonNpc;   // 0x264, the NPC that balloon belongs to (fbatNpcTalkCheck's result; mario_balloon)
    s32 slitFloorValid;            // 0x268, nonzero while slitFloorPos is set (mot_slit: cleared by marioClearSlitFloor, set in slit state 0x5A)
    Vec slitFloorPos;              // 0x26C, floor point marioGetSlitFloorPos returns (copied from position in slit state 0x5A)
    s32 sfxIds[3];                 // 0x278, sound handles, 0xFFFFFFFF when none (mot_jump, mot_stay, peach, marioSoundInit)
    u32 inputMask;                 // 0x284, button mask set from evt_mario; 0 means unmasked (marioUpdateKeyData)
    union {
        void* motStruct;                         // 0x288, motion userdata, TODO rename?
        struct JabaraWork* jabaraWork;           // 0x288, mot_jabara
        struct MotionPlaneData* motionPlaneData; // 0x288, mot_plane
        struct MotRoll* motRoll;                 // 0x288, mot_roll
        struct MotionShipData* motionShipData;   // 0x288, mot_ship
    };
    struct KpaWork* koopaWork;     // 0x28C, the koopa character's work (koopa.h, KPA); marioReInit keeps it across its memset
    f32 dirEventStart;             // 0x290, start yaw of marioSetDirEventMain
    f32 dirEventTarget;            // 0x294, target yaw is 270 minus this
    s32 dirEventMsec;              // 0x298
    s32 dirEventFrame;             // 0x29C, frames elapsed
    // 0x2A0-0x2BC: per-motion scratch words; each motion gives them its own type (the union members below).
    union {
        f32 spinSpeed;             // 0x2A0, added to dispDir each frame (mario_motion spin, mot_hip, evt_mario)
        f32 damageRootY;           // 0x2A0, surface height of the damage floor fallen into: splash height, start of the return arc (set_damage_root_ypos, mot_damage)
        struct {
            u16 kpaPoseCounter;    // 0x2A0, koopa_motion: frame counter, wraps past 0x50; half of it is the pose time
            u16 field_0x2A2;       // 0x2A2, koopa_motion: cleared beside kpaPoseCounter
        };
    };
    union {
        f32 field_0x2A4;           // 0x2A4, f32 in party_yoshi and mot_dokan
        f32 planeTurnRate;         // 0x2A4, mot_plane: eased turn rate added to dispDir each frame
        f32 hammerDir;             // 0x2A4, mot_hammer: swing direction; dispDir and moveDir are set from it
        f32 jabaraDir;             // 0x2A4, mot_jabara: dispReqDir at the launch, restored to dispDir/dispReqDir on landing
        s32 motionSfxId0;          // 0x2A4, sound handle in mot_hip (-1 when none)
        s32 paperAnimeTime;        // 0x2A4, mot_ship: marioSetPaperAnimeLocalTime argument (0x58 at reinit, counted down through the landing)
        u32 rollFlags;             // 0x2A4, mot_roll flag word (0x10, 0x20, 0x100, 0x200, 0x1000, 0x4000, 0x10000)
        s32 slitPushCount;         // 0x2A4, mot_slit: counter of frames pressed into an attr 0x2000 hit (state 0x50 at sysMsec2Frame(0x32)); also reset on entry
    };
    union {
        f32 wCamVal1;              // 0x2A8, f32 step in mot_plane, mot_slit, mot_dokan, mot_hammer, mot_damage; party_yoshi keeps the flutter base y here
        s32 motionSfxId1;          // 0x2A8, sound handle in mot_hip and mot_roll (-1 when none)
        s32 jabaraHitY;            // 0x2A8, y of the hit mot_jabara lands on (the hit-depth jump)
        s32 hammerSwingType;       // 0x2A8, mot_hammer: swing type stored at the start, switched on at impact for the effect offset
    };
    union {
        f32 field_0x2AC;           // 0x2AC, f32 in mot_plane, mot_damage, mot_slit and mot_bottomless (s32 0 in mot_roll/mot_jabara resets)
        f32 slitPrevY;             // 0x2AC, mot_slit: position.y at the last slit state 10 frame (a change calls slitResetBounce)
    };
    union {
        f32 field_0x2B0;           // 0x2B0, f32 in party_yoshi and mario_motion
        f32 jumpRiseY;             // 0x2B0, mot_jump: height risen since takeoff (jumpVelY summed), checked against jumpHeight
        f32 hammer2AnimTime;       // 0x2B0, mot_hammer2: charge pose time (animPoseSetLocalTime gets it / 6), capped per charge level
        f32 shipDispStepY;         // 0x2B0, mot_ship: per-frame step added to dispOfs.y
        s32 stayJabaraWait;        // 0x2B0, mot_stay: frames standing, capped at 10; marioChkJabara is tried from 10 on
        f32 jabaraSpeed;           // 0x2B0, mot_jabara: angle step added to dispRot.x (its state machine raises and lowers it)
    };
    union {
        s32 sleepTimer;            // 0x2B4, mot_stay and peach_stay: frames asleep (cleared when the sleep pose starts, capped at
                                   //        sysMsec2Frame(10000)); marioChkDeepSleep tests the cap; f32 in party_yoshi
        f32 jabaraAccel;           // 0x2B4, mot_jabara: step added to jabaraSpeed each frame
        s16 hammer2Flags;          // 0x2B4, mot_hammer2: flag word (0x1, 0x10, 0x1000, 0x2000), cleared on entry
        struct {
            s16 slitTurnTimer;     // 0x2B4, mot_slit: set to 6 when a turn starts, counts down in state 0x15
            s16 slitStillCount;    // 0x2B6, mot_slit: frames with the stick neutral in state 0x14 (reaches 6 to leave it), cleared when the stick moves
        };
    };
    union {
        f32 field_0x2B8;           // 0x2B8
        f32 jumpHeight;            // 0x2B8, marioGetJumpHeight/marioChgJumpStandMotion (jump stand)
        s32 stayAHoldTimer;        // 0x2B8, mot_stay: frames A has been held (0 when released); from 10 the stick no longer walks
        s32 motionSfxId2;          // 0x2B8, sound handle in mot_hammer (-1 when none)
        struct {
            s16 jabaraTimer;       // 0x2B8, mot_jabara: frames left in the rising swing (swingMain adds unk4 each frame, then unk8 at 0)
            s8 jabaraState;        // 0x2BA, mot_jabara: swingMain's state switch (0 idle, up to 15)
            s8 jabaraAnimTime;     // 0x2BB, mot_jabara: local time passed to animPoseSetLocalTime for the paper animation
        };
        struct {
            s16 slitFlags;         // 0x2B8, mot_slit: bit 0 set when the walk speed reaches the dash cap (slitCapSpeed), cleared on steering
            s16 slitTurnSide;      // 0x2BA, mot_slit: 1 or 2, the side of the turn (sets dispDir to 260 or 280)
        };
    };
    u8 field_0x2BC[4];             // 0x2BC
    s16 coinDropTimer;             // 0x2C0, frames left dropping coins after fleeing a field battle: npcdrv sets sysMsec2Frame(3000) with marioSetMutekiTime(3000) on battle result 4 (escaped); mario.c drops a coin every 30 frames while it runs
    u8 field_0x2C2[0x2C4 - 0x2C2]; // 0x2C2
    f32 kpaSpeed;                  // 0x2C4, signed walk/swim speed (koopa_motion, koopa.c reset)
    u32 reloadMapOnBottomless;     // 0x2C8, set by N_marioReloadMapOnBottomlessOn/Off
    Vec bottomlessResetPosition;   // 0x2CC JP, 0x2E4 US
    s16 slitState;                 // 0x2D8, mot_slit state machine step (0x14 idle, 0x50-0x5E dash and drop, 0x64+ turns and slides; >= 100 ends the slit)
    s16 slitPhaseTimer;            // 0x2DA, frames left in the 0x65, 0x6F and 0x79 phases (set to 30 on entry)
    s32 slitTimer;                 // 0x2DC, frame counter for states 0x2, 0x50-0x51, 0x5A-0x5C (set on entry, counts down to 0)
} MarioWork;                       // size 0x2E0 on Japan

MarioWork* marioGetPtr(void);
s32 marioGetColor(void);
void marioSetCharMode(s32 mode);
void marioSetFamicomMode(s32 mode);
void marioSetSpec(void);
void marioEntry(void);
void marioItemGetDisable(void);
void marioItemGetChk(void);
BOOL marioItemGetOk(void);
BOOL marioCaseEventValidChk(void);
BOOL mario8005BB80(void);
BOOL unk_8005bb80(void);
BOOL marioCheckMenuDisable(void);
BOOL marioChkKey(void);
BOOL marioChkCtrl(void);
s32 mario8005B5A8(void);
void marioFBattlePrepare(void);
void unk_80058374(void);
void marioFBattlePost(void);
s32 marioCtrlOffChk(void);
s32 marioKeyOffChk(void);
s32 marioCtrlOff(void);
s32 marioCtrlOn(void);
s32 marioCtrlOff2(void);
s32 marioCtrlOn2(void);
s32 marioKeyOff(void);
s32 marioKeyOn(void);
s16 marioSetMutekiTime(s32 msec);
BOOL marioBgmodeChk(void);
void marioBgmodeOn(void);
void marioBgmodeOff(void);
BOOL marioChkSts(s32 flags);
void marioResetHitObj(const char* name);
void marioReset(void);
void marioOfsRotReset(void);
void marioSoundInit(void);
void marioPoseInit(void);
void marioInit(void);
void marioReInit(void);
void marioMain(void);
void marioCtrlOff2Main(void);
void marioMove(void);
void marioMoveMain(void);
BOOL marioChkPushAnime(void);
s32 marioAnimeId(void);
void mario80058374(void);
void marioChgEvtPose(const char* name);
const char* toDotMarioPose(const char* name);
void marioChgPose(const char* name);
void marioChgPoseTime(const char* name, s16 frames);
void marioChgPaper(const char* name);
void marioSetPaperAnimeLocalTime(s32 frames);
BOOL marioPaperOn(const char* name);
void marioPaperOff(void);
void marioPaperLightOn(void);
void marioPaperLightOff(void);
void marioGetScreenPos(Vec position, f32* x, f32* y, f32* z);
BOOL marioChkInScreen(s32 x, s32 y);
f32 marioGetScale(void);
void marioMakeDispDir(void);
void marioRearAnime(void);
void marioPreDisp(void);
void marioDispBlur(CameraId camId, void* param);
void marioDisp(CameraId camId, void* param);
void marioDispBlurSub(s32* unused, s32 blurId);
const char* toRearPose(const char* name);
const char* toFrontPose(const char* name);
s32 N_marioChkUseParty(void);
extern const f32 float_30_80419f70;
extern const f32 float_20_80419fac;
