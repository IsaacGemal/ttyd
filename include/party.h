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
    u32 field_8;                 // 0x8
    s32 paperAnimGroup;          // 0xC
    s32 field_10;                // 0x10
    s32 paperPoseId;             // 0x14
    char* field_18;              // 0x18
    u32 field_1C;                // 0x1C, unsigned
    u16 field_20;                // 0x20
    u8 field_22[0x28 - 0x22];    // 0x22
    s16 field_28;                // 0x28
    s16 field_2A;                // 0x2A
    u8 field_2C;                 // 0x2C
    s8 field_2D;                 // 0x2D
    u8 field_2E;                 // 0x2E
    s8 currentSlotId;            // 0x2F
    u8 field_30;                 // 0x30
    s8 currentMemberId;          // 0x31, PartyMembers
    u8 field_32;                 // 0x32
    u8 field_33;                 // 0x33
    u8 field_34;                 // 0x34
    u8 field_35;                 // 0x35
    u8 field_36;                 // 0x36
    u8 field_37[0x39 - 0x37];    // 0x37
    u8 useMotionId;              // 0x39
    u8 field_3A;                 // 0x3A
    u8 field_3B;                 // 0x3B
    u8 field_3C[0x4C - 0x3C];    // 0x3C
    s8 field_4C;                 // 0x4C
    u8 field_4D[0x58 - 0x4D];    // 0x4D
    Vec position;                // 0x58
    u8 field_64[0x70 - 0x64];    // 0x64
    Vec field_70;                // 0x70
    u8 field_7C[0xB8 - 0x7C];    // 0x7C
    Vec field_B8;                // 0xB8
    u8 field_C4[0xE4 - 0xC4];    // 0xC4
    f32 field_E4;                // 0xE4
    u8 field_E8[0xF4 - 0xE8];    // 0xE8
    f32 field_F4;                // 0xF4
    u8 field_F8[0xFC - 0xF8];    // 0xF8
    f32 field_FC;                // 0xFC
    f32 dir;                     // 0x100
    f32 speed;                   // 0x104
    f32 speed2;                  // 0x108
    f32 field_10C;               // 0x10C
    f32 field_110;               // 0x110
    f32 jumpPara1;               // 0x114
    f32 jumpPara2;               // 0x118
    f32 jumpPara3;               // 0x11C
    u8 field_120[0x12C - 0x120]; // 0x120
    void* field_12C;             // 0x12C
    u8 field_130[0x134 - 0x130]; // 0x130
    void* field_134;             // 0x134
    void* field_138;             // 0x138
    u8 field_13C[0x140 - 0x13C]; // 0x13C
    void* field_140;             // 0x140
    u8 field_144[0x160 - 0x144]; // 0x144
    MarioWork* playerPtr;        // 0x160
    s32 camId;                   // 0x164
    s32 yoshiPsndSFXId;          // 0x168
    s32 unk16C;                  // 0x16C
    void* useStruct;             // 0x170
    u8 field_174[0x188 - 0x174]; // 0x174
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
s32 partyGoodbye(s32 memberId);
s32 partyEntry2(s32 memberId);
s32 partyEntryPos(s32 memberId, f32 x, f32 y, f32 z);
void partyKeyOn(void);
void partyKeyOff(void);
void partyMain(void);
void allPartyForceRideOn(void);
void allPartyRidePlane(void);
void partyDokanEnd(s32 index, s32 unk);
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
s32 partyHelloMain(PartyEntry* party);
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
    void* init_func;  // 1
    void* move_func;  // 2
    void* use_func;   // 3
    void* post_func;  // 4
    void* bye_func;   // 5
    void* unk_6_func; // 6
    void* unk_7_func; // 7
    void* unk_8_func; // 8
    void* exit_func;  // 9
} PartyData;
void partySetFamicomMode(PartyEntry* party, s32 mode);
extern s32 partyCtrlNo;
extern char* partyPoseTbl[20][15];
extern const Vec vec3_802cced4;
extern const Vec vec3_802cce5c;
extern const Vec vec3_802cce68;
extern const Vec vec3_802cce74;
extern const Vec vec3_802cce80;
extern char str_802CCF08[];
extern const f32 float_neg6p8_8041b0d8;
extern const f32 float_6p8_8041b0dc;
extern const f32 float_1p2_8041b0e0;
extern const f32 float_2_8041b0e4;
extern const f32 float_1_8041b0e8;
extern const f32 float_90_8041b0f0;
extern const f32 float_270_8041b0f4;
extern const f32 float_deg2rad_8041b0f8;
extern const f32 float_3_8041b104;
extern const f32 float_3p5_8041b108;
extern const f32 float_4_8041b10c;
extern const f32 float_0p75_8041b110;
s32 partyDokanInit(PartyEntry* party);
s32 partyDokanMain(PartyEntry* party);
void party_damage_return(PartyEntry* party);
