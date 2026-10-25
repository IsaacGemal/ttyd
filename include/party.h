#pragma once

#include "mario/mario.h"
#include <dolphin/types.h>

#pragma enumsalwaysint off
typedef enum PartyMember {
    PARTY_MEMBER_NONE,      // 0
    PARTY_MEMBER_GOOMBELLA, // 1
    PARTY_MEMBER_KOOPS,     // 2
    PARTY_MEMBER_BOBBERY,   // 3
    PARTY_MEMBER_YOSHI,     // 4
    PARTY_MEMBER_FLURRIE,   // 5
    PARTY_MEMBER_VIVIAN,    // 6
    PARTY_MEMBER_MS_MOWZ,   // 7
#define MAIN_PARTY_MEMBER_MAX PARTY_MEMBER_MS_MOWZ + 1
    PARTY_MEMBER_EGG,                // 8
    PARTY_MEMBER_FLAVIO,             // 9
    PARTY_MEMBER_PUNIO,              // 10
    PARTY_MEMBER_FRANKLY,            // 11
    PARTY_MEMBER_CRAW,               // 12
    PARTY_MEMBER_GOOMBELLA_FOLLOWER, // 13
    PARTY_MEMBER_KOOPS_FOLLOWER,     // 14
    PARTY_MEMBER_BOBBERY_FOLLOWER,   // 15
    PARTY_MEMBER_YOSHI_FOLLOWER,     // 16
    PARTY_MEMBER_FLURRIE_FOLLOWER,   // 17
    PARTY_MEMBER_VIVIAN_FOLLOWER,    // 18
    PARTY_MEMBER_MS_MOWZ_FOLLOWER    // 19
} PartyMember;
#pragma enumsalwaysint on

#pragma enumsalwaysint off
typedef enum PartyFlags {
    PARTY_FLAG_IS_ACTIVE = (1 << 0),
    PARTY_FLAG_IS_GROUNDED = (1 << 1),
    PARTY_FLAG_IS_FOLLOWER = (1 << 2),
    PARTY_FLAG_IS_PARTNER = (1 << 3),
    PARTY_FLAG_IS_JUMPING = (1 << 4),
    PARTY_FLAG_IS_FALLING = (1 << 5),
    PARTY_FLAG_IS_STEPPING = (1 << 6),
    PARTY_FLAG_IN_SHALLOW_WATER = (1 << 7),
    PARTY_FLAG_IS_BEING_USED = (1 << 8),
    PARTY_FLAG_PAPER_MODE = (1 << 12),
    PARTY_FLAG_MOT_SLIT_FLOOR = (1 << 13),
    PARTY_FLAG_IS_RIDING_PLANE = (1 << 16),
    PARTY_FLAG_IS_RIDING_SHIP = (1 << 17),
    PARTY_FLAG_IS_BEHIND_MARIO = (1 << 19),
    PARTY_FLAG_SLEEP_ON = (1 << 22),
    PARTY_FLAG_BG_MODE = (1 << 23),
    PARTY_FLAG_DISABLE_FRONT_WALL_CHK = (1 << 24),
    PARTY_FLAG_FORCE_FALL = (1 << 25),
    PARTY_FLAG_DISABLE_FLOORS = (1 << 26),
    PARTY_FLAG_RUN_OFF = (1 << 28),
    PARTY_FLAG_CTRL_OFF = (1 << 29),
    PARTY_FLAG_CTRL_OFF_2 = (1 << 30),
    PARTY_FLAG_IS_EVT_USE = (1 << 31)
} PartyFlags;
#pragma enumsalwaysint on

// PartyEntry.trigFlags bits that have one known meaning; the others stay numbers at their users.
typedef enum PartyTrigFlags {
    PARTY_TRIG_FLAG_CHG_PAPER_MOT = (1 << 0),  // allPartySlitOn/partyChgMotSlitFloor; party_slit/party_roll start on it
    PARTY_TRIG_FLAG_CHG_MOT = (1 << 1),        // partyChgMot; the party_motion_* routine's first frame
    PARTY_TRIG_FLAG_CHG_MOVE_MODE = (1 << 2),  // partyChgMoveMode; the party_move_mode_* routine's first frame
    PARTY_TRIG_FLAG_CHG_RUN_MODE = (1 << 3),   // partyChgRunMode; the run mode's init (use, move, bye, hello)
    PARTY_TRIG_FLAG_CHG_POSE = (1 << 13),      // partyChgPose; partyPreDisp reloads the pose
    PARTY_TRIG_FLAG_CHG_PAPER = (1 << 14),     // partyChgPaper; partyPreDisp reloads the paper pose
    PARTY_TRIG_FLAG_FOLLOW_CLOSE = (1 << 17),  // N_partyFollowCloseOn/Off
    PARTY_TRIG_FLAG_SPLASH = (1 << 24)         // set on water (attr 0x200) when splashTimer is 0; party_move splashes
} PartyTrigFlags;

#pragma enumsalwaysint off
typedef enum PartySlotId {
    PARTY_SLOT_PARTY = 0,    // 0x0
    PARTY_SLOT_FOLLOWER = 1, // 0x1
    PARTY_SLOT_NONE = -1
} PartySlotId;
#pragma enumsalwaysint on

typedef struct PartyEntry {
    u32 flags;                   // 0x0
    u32 flags2;                  // 0x4
    u32 trigFlags;               // 0x8, PartyTrigFlags: one-shot requests, as MarioWork.trigFlags (partyChgMot,
                                 //      partyChgMoveMode, partyChgRunMode, partyChgPose and partyChgPaper set a bit, the code
                                 //      they start tests and clears it), and party_motion's follow and splash states
    s32 paperAnimGroup;          // 0xC
    s32 field_0x10;              // 0x10
    s32 paperPoseId;             // 0x14
    char* pose;                  // 0x18, partyChgPose
    char* paper;                 // 0x1C, partyChgPaper
    u16 poseTime;                // 0x20, partyPreDisp's animPoseSetLocalTime frame; zeroed with each pose change, Mario's
                                 //       poseTime while he rides Yoshi
    u8 pad_0x22[0x24 - 0x22];    // 0x22
    s32 timer;                   // 0x24, frame countdown: `--timer <= 0` checks, reset to 0x14/0x1E/0x3C/0x41
    s16 forceMoveFrames;         // 0x28, partySetForceMove's frame count; partyForceWalk counts it down and restores
                                 //       the previous run mode at 0; nonzero also picks the forced-walk speeds
    s16 mutekiTime;              // 0x2A, marioSetMutekiTime copies Mario's into it
    s8 kemuriCount;              // 0x2C, party_kemuri: counts walking frames, a dust puff each time it passes 6
    s8 stepSfxCount;             // 0x2D, partyStepSfx: counts frames, footstep sounds at its lo/hi marks
    s8 dokanType;                // 0x2E, partyDokanEnd's copy of Mario's dokanType; partyDokanMain picks the exit by it
    s8 currentSlotId;            // 0x2F
    s8 padChan;                  // 0x30, pad index partyUpdateKeyData reads the GlobalWork pad arrays with
    s8 currentMemberId;          // 0x31, PartyMembers
    u8 isFlyer;                  // 0x32, 1 for Flurrie and Vivian (their init_funcs): party_move and party_motion take the
                                 //       fly routines, party_kemuri raises no dust, partyStepSfx plays the float sound
    u8 partnerType;              // 0x33, PartyMember of the init_func that set the entry up (kuribo_init's for Goombella and
                                 //       every NPC follower); fbatHitCheck tests Koops' before nokonokoGetStatus, and Bobbery's
    u8 runMode;                  // 0x34, partyChgRunMode
    u8 prevRunMode;              // 0x35, runMode before partyChgRunMode; unk_800ba2dc returns to it
    u8 mot;                      // 0x36, partyChgMot
    u8 paperMot;                 // 0x37, paper motion to run: 1 party_slit, 2 party_roll, 0 none
    u8 motionStep;               // 0x38, switch selector of the party_motion_* routines
    u8 useMotionId;              // 0x39
    u8 paperMotStep;             // 0x3A, switch selector of party_slit and party_roll; allPartySlitOff moves it to 0x28
    u8 moveMode;                 // 0x3B, partyChgMoveMode
    u8 pad_0x3C;                 // 0x3C
    u8 field_0x3D;               // 0x3D, set to the partyMoveBehindMario/BesideMario mode, cleared by party_move and cloud
    u8 field_0x3E;               // 0x3E, zeroed only (party_move)
    s8 field_0x3F;               // 0x3F, compared with 1, 2 and 3 in party_move, then zeroed
    f32 homingDist;              // 0x40, evt_party_set_homing_dist; distance from Mario at which a standing partner
                                 // starts walking after him (party_motion_stay)
    u32 field_0x44;              // 0x44, set to 0x10 beside homingDist (party_move)
    u8 pad_0x48[0x4C - 0x48];    // 0x48
    s8 splashTimer;              // 0x4C, frames before water (attr 0x200) can raise another splash: set to 0x14 at the splash,
                                 //       the floor checks raise trigFlags 0x1000000 only at 0
    s8 wallHitCount;             // 0x4D, party_motion_homing_walk: frames blocked by a front wall; past 8 it re-tests the floor ahead
    u8 noFloorCount;             // 0x4E, frames the partner walked with no floor under it; party_motion_stay turns it toward Mario past 10
    s8 tailFlags;                // 0x4F, Vivian's tail: bit 0 set by L_partyVivianTailStop, cleared by ..Start, tested by
                                 //       N_vivianDisplayTailCheck
    u8 pad_0x50;                 // 0x50
    s8 followPartner;            // 0x51, 1 while partyRecordFootmark records the other partner's positions instead of Mario's
                                 //       (set when the trail is reset, cleared with partyClearFootmark2)
    s16 touchCount;              // 0x52, contact counter partyMarioHosei returns: up to 100 while touching Mario, decays after
    f32 moveAngle;               // 0x54, heading passed to movePos
    Vec position;                // 0x58
    Vec dispRot;                 // 0x64, display rotation in degrees, as MarioWork's dispRot: party.c's draw matrix rotates by z then x, y is not read; z is revise360'd every frame while rolling (p_roll)
    Vec scale;                   // 0x70, party_scale (usu_00)
    Vec dispOffset;              // 0x7C, added to position for the draw matrix (party.c), y bobbed by cloud and Vivian
    Vec spinPivot;               // 0x88, pivot of the roll spin (party.c translates by -spinPivot before the z and x rotations, +spinPivot after)
    Vec targetPos;               // 0x94, destination of the appear/jump/fly routines; distABf'd against position
    f32 velX;                    // 0xA0, x of the push velocity (partyMarioHosei sets it, partyGetMoveDirSpd adds it to moveDelta.x)
    u8 pad_0xA4[0xA8 - 0xA4];    // 0xA4
    f32 velZ;                    // 0xA8, z of the push velocity (partyMarioHosei: f30 * -cos)
    Vec moveDelta;               // 0xAC, per-frame offset added to position: set by evt_party_set_hosei_xyz, mario->riderDelta in yoshi_use, zeroed in driveParty
    Vec shadowPos;               // 0xB8, partyPreDisp's shadowEntryMode position (y from a floor hit check) and
                                 //       animPoseVivianMain's; Vivian's trails position by up to 22
    Vec field_0xC4;              // 0xC4, partyGroundSlope's x/z tilt angles of the floor normal, zeroed by partySearchGround
    u8 pad_0xD0[0xE0 - 0xD0];    // 0xD0
    f32 groundSlope;             // 0xE0, partyGroundSlope result (party_hit.c), compared with 30.0f
    f32 groundY;                 // 0xE4, floor height saved by partySearchGround
    f32 groundGap;               // 0xE8, partySearchGround: -1 with no floor found, the gap down to a floor below the step-down
                                 //       range, 0 when it takes a floor; partyMoveMain leaves the floor while it is nonzero
    f32 frontWallDist;           // 0xEC, partySearchFrontWall: distance to the wall it hit, -1 with none; kuribo_use
                                 //       compares it with the distance to Mario
    f32 height;                  // 0xF0, height above position.y (partyGetHeight, Yoshi's head offset)
    f32 radius;                  // 0xF4, collision radius
    f32 playerTargetYaw;         // 0xF8, copied from player->targetYaw in driveParty
    f32 moveDir;                 // 0xFC, direction of motion, as MarioWork.moveDir (copied from it at entry, dokan and ride
                                 //       end; the move routines copy dir into it): partyMakeDispDir faces by it, L_getStick
                                 //       returns it
    f32 dir;                     // 0x100
    f32 speed;                   // 0x104
    f32 speed2;                  // 0x108
    f32 dispDir;                 // 0x10C, evt_party_get_dispdir
    f32 dispReqDir;              // 0x110, as MarioWork.dispReqDir (mostly copied from it): partyMakeDispDir turns dispDir
                                 //        toward it; yoshi_use hands it back to Mario
    f32 jumpPara1;               // 0x114
    f32 jumpPara2;               // 0x118
    f32 jumpPara3;               // 0x11C
    f32 stepUpY;                 // 0x120, y of the step ahead (partyChkFrontStep); party_motion_upstairs and Yoshi's step
                                 //        motion climb to it in 3 frames
    f32 spinSpeed;               // 0x124, degrees added each frame to dispDir or dispRot.x (partyDokanMain); the ride-on jump
                                 // spins dispDir by it, raising it by 2 up to 30
    f32 targetDist;              // 0x128, distance to targetPos, set before the fly/jump step count
    struct HitEntry* frontWallHit; // 0x12C, partySearchFrontWall's hit, as MarioWork.hitobjFront (Yoshi's checkWall sets both);
                                   //        the move routines test it for attr 0x800, party_slit for 0x2000
    struct HitEntry* frontHit;   // 0x130, check_front result
    struct HitEntry* pushWallHit; // 0x134, the same wall when the partner was moving into it (speed != 0), as
                                  //        MarioWork.hitobjPush; party_motion_behind_mario stops at it
    struct HitEntry* groundHit;  // 0x138, partySearchGround result
    struct HitEntry* field_0x13C; // 0x13C, takes the old groundHit (party_yoshi.c, evt_party_jump_pos); party.c clears it
    struct HitEntry* headHit;    // 0x140, partySearchHead result
    void* field_0x144;           // 0x144, aji_13 tests it for NULL before the groundHit group check; party.c clears it
    u32 field_0x148;             // 0x148
    struct HitEntry* field_0x14C; // 0x14C, set to the ground hit when its attribute has bit 0x40000000 (party_motion.c)
    u16 button;                  // 0x150, held buttons (partyUpdateKeyData)
    u16 buttonNew;               // 0x152, pressed this frame
    u16 buttonRepeat;            // 0x154
    u16 buttonUp;                // 0x156, released this frame
    s8 stickX;                   // 0x158
    s8 stickY;                   // 0x159, stored negated
    s8 substickX;                // 0x15A
    s8 substickY;                // 0x15B, stored negated
    u8 triggerLeft;              // 0x15C
    u8 triggerRight;             // 0x15D
    u8 unusedInput[2];           // 0x15E, cleared with the other input copies (L_partyClearCont, partyUpdateKeyData), never
                                 // set or read; as MarioWork.unusedInput
    MarioWork* playerPtr;        // 0x160
    s32 camId;                   // 0x164
    s32 yoshiPsndSFXId;          // 0x168
    s32 evtUseMsec;              // 0x16C, length of an event-started use in ms (evt_party_cloud_breathout's argument, 5000 from
                                 //        evt_party_nokotaro_hold): cloud's useTimer is sysMsec2Frame of it, Koops holds while > 0
    union {
        void* useStruct;                     // 0x170
        struct BomheiWork* bomheiWork;       // 0x170, party_bomhei
        struct PartyCloud* partyCloud;       // 0x170, party_cloud
        struct PartyYoshiUse* partyYoshiUse; // 0x170, party_yoshi
    };
    // 0x174-0x187: scratch that each partner uses for its own state; read the member for the active partner.
    union {
        struct {                       // generic jump (party.c partyRideMain case 20-21, evt_party_jump_pos)
            f32 jumpAccel;             // 0x174, added to jumpPara1 each frame
        } jump;
        struct {                       // Yoshi (party_yoshi.c jump routines)
            f32 jumpVel;               // 0x174, added to jumpPara3 each frame
            f32 jumpAccel;             // 0x178, accumulates jumpPara2 each frame, added to jumpVel
        } yoshi;
        struct {                       // Bobbery (party_bomhei.c)
            u8 pad_0x174[4];           // 0x174
            s16 motionTimer;           // 0x178, countdown in mario_controll (states 0x28, 0x1F, 0x20, 0x24)
            u8 marioMotion;            // 0x17A, mario_controll state (0xA, 0x14-0x15, 0x1E-0x24, 0x28-0x29, 0x64-0x66)
            u8 syncFrame;              // 0x17B, index into m_w_7_sync and the M_W_7 pose frame (state 0x15)
            s32 field_0x17C;           // 0x17C, set to -1 at init, never read
            s32 countdownSfxId;        // 0x180, sound 0x93C started at the 2500 ms mark, stopped at the splash (0x28)
            s32 alarmSfxId;            // 0x184, sound 0x93D started in state 0x32, stopped at bye
        } bobbery;
        struct {                       // Flurrie (party_cloud.c; cloud_move bobs the body with bobAngle)
            s32 field_0x174;           // 0x174, zeroed at init only
            struct EffectEntry* breathFx; // 0x178, effKumokumoN64Entry effect while the breath is held, NULL otherwise
            f32 bobAngle;              // 0x17C, degrees, revise360'd by bobStep each frame
            f32 bobStep;               // 0x180, 4.25 per frame
        } flurrie;
        struct {                       // Koops (party_nokonoko.c)
            struct ItemEntry* heldItem; // 0x174, item picked up by the shell search, returned by nokonoko_use_post
            f32 slideDecay;            // 0x178, set to 50.0f when a slide starts, -2.2 per frame down to 0
            s16 dashing;               // 0x17C, 1 while running (useMotionId 0x14-0x15): faster speed cap, 0x50 motion
            s16 moveFlags;             // 0x17E, bit 0x1 set during an event with evtUseMsec > 0, bit 0x2 set by ccfloor
            s32 field_0x180;           // 0x180, zeroed only
        } koops;
        struct {                       // Vivian (party_vivian.c)
            f32 charge;                // 0x174, 10.0f at rest, +1 per frame to 20 while held (party.c shadow size)
            u8 pad_0x178[4];           // 0x178
            f32 moveSpeed;             // 0x17C, movePos speed toward Mario, -13 to 0
            u8 pad_0x180[2];           // 0x180
            s16 poseTime;              // 0x182, set to 0x18, fed to animPoseSetLocalTime
            struct EffectEntry* ripple; // 0x184, ripple effect (effRippleEntry), NULL when gone
        } vivian;
    } tail;
} PartyEntry;

PartyEntry* partyGetPtr(s32 index);
s32 partyEntry(s32 memberId);
s32 partyEntryHello(s32 memberId);
PartyEntry* anotherPartyGetPtr(s32 index);
BOOL partyPaperOn(PartyEntry* party, const char* animName);
char* partyChgPoseId(PartyEntry* party, s32 index);
void partyChgPose(PartyEntry* party, char* pose);
void party_set_behind_dir(PartyEntry* party, f32 dir);

void partyCtrlOff(void);
void partyChgPaper(PartyEntry* party, char* paper);
void partyCtrlOn(void);
f32 partyGetHeight(PartyEntry* party);
void partySetForceMove(PartyEntry* party, s32 frames, f32 x, f32 z);
s32 yoshiSetColor(void);
s32 partyKill(s32 memberId);
s32 partyKill2(s32 memberId);
s32 getPartyEntryId(void);
void partyPoseEntry(PartyEntry* party);
s32 partyGoodbye(s32 memberId);
s32 partyEntry2(s32 memberId);
s32 partyEntryPos(s32 memberId, f32 x, f32 y, f32 z);
void partyKeyOn(void);
void partyKeyOff(void);
void partyMain(void);
void allPartyForceRideOn(void);
void allPartyRidePlane(void);
void partyDokanEnd(s32 index, s32 dokanType);
void allPartyRideShip(void);
void allPartyRideOff(void);
void allPartyRideOff2(void);
s32 partyEntry2Pos(s32 memberId, f32 x, f32 y, f32 z);
s32 partyEntry2Hello(s32 memberId);
void partyPreDisp(PartyEntry* party);
void partyMakeDispDir(PartyEntry* party);
void partyDisp(CameraId cameraId, void* param);
void partyChgRunMode(PartyEntry* party, s32 mode);
void partyShadowOff(PartyEntry* party);
BOOL partyChkHerohero(PartyEntry* party);
void partySetCamId(PartyEntry* party, s32 cameraId);
void partyInit(void);
void partyReInit(void);
void partyInitCamId(PartyEntry* party);
void partyRun(PartyEntry* party);
void partyStop(PartyEntry* party);
void unk_800ba2dc(PartyEntry* party);
void partyUsePost(PartyEntry* party);
void partyPaperOff(PartyEntry* party);
void partyPaperLightOff(PartyEntry* party);
void setAnimPoseMaterialFlag(PartyEntry* party);
void partyPaperRelease(PartyEntry* party);
void party_force_ride_yoshi(PartyEntry* party);
void party_ride_yoshi_force_move(void);
void L_partyClearCont(PartyEntry* party);
BOOL partyGoodbyeInit(PartyEntry* party);
BOOL partyGoodbyeMain(PartyEntry* party);
s32 partyRideYoshiMain(PartyEntry* party);
s32 partyEntryMain(s32 memberId);
void N__partyChgRunMode(PartyEntry* party, s32 oldMode, s32 newMode);
void N_partyForceChgRunMode(PartyEntry* party, s32 mode, s32 unk);
void driveParty(PartyEntry* party);
void partyHelloInit(PartyEntry* party);
s32 partyHelloMain(PartyEntry* party);
void party_kemuri(PartyEntry* party);
void getRidePos(PartyEntry* party, Vec* pos);
s32 partyDoWork(void);
void partyHello(PartyEntry* party);
struct HitEntry;
struct HitEntry* check_front(PartyEntry* party);
void mario_controll(PartyEntry* party);
BOOL mario_bomhei_keychk(void);
void bomhei_use_main(PartyEntry* party);
void bomhei_use(PartyEntry* party);
void bomhei_use_post(PartyEntry* party);
void bomhei_exit(PartyEntry* party);
void bomhei_init_800BFBB0(PartyEntry* party);
void bomhei_bye(PartyEntry* party);
s32 partyRideChk(PartyEntry* party);
void partyUpdateKeyData(PartyEntry* party);

typedef struct PartyData {
    char* name;       // 0, TODO: modelname? animname?
    void (*init_func)(PartyEntry* party);  // 1
    void (*move_func)(PartyEntry* party);  // 2
    void (*use_func)(PartyEntry* party);   // 3
    void (*post_func)(PartyEntry* party);  // 4
    void (*bye_func)(PartyEntry* party);   // 5
    void* field_0x18; // 6
    void* field_0x1C; // 7
    void* field_0x20; // 8
    void (*exit_func)(PartyEntry* party);  // 9
} PartyData;
void partySetFamicomMode(PartyEntry* party, s32 mode);
extern s32 partyCtrlNo;
extern char* partyPoseTbl[20][15];
s32 partyDokanInit(PartyEntry* party);
s32 partyDokanMain(PartyEntry* party);
void party_damage_return(PartyEntry* party);
void partyRideMain(PartyEntry* party);
void L_allPartyRideOn(void);
s32 partyGetNum(void);
