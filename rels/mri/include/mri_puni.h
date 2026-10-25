#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "evt/evt_cmd.h"
#include "driver/camdrv.h"
#include "punidrv.h"

#define PUNI_DATA_MAX 211
#define PUNI_TEX_INFO_MAX 200

// PuniTarget flags. Only ACTIVE entries within 32 units of height are searched (puniTargetSearch).
#define PUNI_TARGET_ACTIVE  0x1
#define PUNI_TARGET_ATTRACT 0x2 // gathering point: puniModeMass walks toward it, puniModeFunbari jumps at it
#define PUNI_TARGET_REPEL   0x4 // threat: puniModeAway runs from it (puni_piders_check_func marks the Piders)

// One entry of PuniWork's target table at 0x374.
typedef struct PuniTarget {
    s32 flags;             // 0x0, PUNI_TARGET_*
    Vec position;          // 0x4
    struct HitEntry* hit;  // 0x10, evt_target_set_soko; puniModeFall matches it against the landing hit
} PuniTarget; // 0x14

// PuniData.statusFlags
#define PUNI_STATUS_HEAL_ICON 0x1  // heal icon up (puniParaRecover, timed by healIconTime)
#define PUNI_STATUS_STUN_ICON 0x2  // stun icon 0x14D circles the puni (puniModePiyori)
#define PUNI_STATUS_AWAY_MOVE 0x4  // stepping along awayDir (puniModeAway, puniModeWind); cleared each frame
#define PUNI_STATUS_ON_SLOPE  0x8  // on the mri_16 slope (_ymode_mri_16_00/01)
#define PUNI_STATUS_DEFEATED  0x10 // hp reached 0 (puniParaDamage); puni_puni_main turns it into PUNI_MODE_DEAD
#define PUNI_STATUS_SOLID     0x20 // treated as a fixed obstacle by puniOption

// PuniData.mode, one per unk_mri_00012d58 case.
enum PuniMode {
    PUNI_MODE_NORMAL = 0,
    PUNI_MODE_PARENTPRE = 1, // attach to a parent (searchParent in the dispatcher), then PARENT
    PUNI_MODE_PARENT = 2,    // follow the parent in the swarm tree
    PUNI_MODE_SEARCH = 3,
    PUNI_MODE_MASS = 4,      // gather at the nearest PUNI_TARGET_ATTRACT
    PUNI_MODE_AWAYJUMP = 5,
    PUNI_MODE_AWAY = 6,      // run from the nearest PUNI_TARGET_REPEL
    PUNI_MODE_ATTACK = 7,    // puniModeAttack's logic, written inline in the dispatcher
    PUNI_MODE_ESCAPE = 8,
    PUNI_MODE_DAMAGE = 9,
    PUNI_MODE_PIYORI = 10,   // stunned
    PUNI_MODE_RECOVER = 11,
    PUNI_MODE_DEAD = 12,
    PUNI_MODE_WIND = 13,     // blown by Flurrie (windPower over 10)
    PUNI_MODE_EXITED = 14,   // evt_puni_exit; no dispatcher case, so the puni does nothing
    PUNI_MODE_PREHAGURE = 15,
    PUNI_MODE_HAGURE = 16,   // strayed from the swarm
    PUNI_MODE_FALL = 17,
    PUNI_MODE_PREUPDRAFT = 18,
    PUNI_MODE_UPDRAFT = 19,
    PUNI_MODE_WONDER = 20,
    PUNI_MODE_FUNBARI = 21,
    PUNI_MODE_WATER = 22,
    PUNI_MODE_DODAI = 23,
    PUNI_MODE_TRAP = 24,
    PUNI_MODE_ENTER = 25,
    PUNI_MODE_EXIT = 26,
    PUNI_MODE_POINT = 27,
    PUNI_MODE_DOKAN = 28,
    PUNI_MODE_CUSTOM = 29,   // calls modeFunc
    PUNI_MODE_NONE = 30,     // no dispatcher case; Mario's entry (Punidat[0]) and finished exits
};

typedef struct MriPuniAnimSet {
    s16* anims[10];      // 0x00, texId sequences, one per PuniData.animIdx (puniAnimation)
    s16 frameCounts[10]; // 0x28, entries before each sequence's terminator
    s16 terminators[10]; // 0x3C, the terminator: -1 loops, -2 holds the last frame
} MriPuniAnimSet; // 0x50

typedef struct PuniWork {
    s32 systemMode;                     // 0x0, evt_puni_get/set_systemmode
    MriPuniAnimSet animSet[2];          // 0x4
    char puniNames[4][10][16];          // 0xA4, four 10-entry name tables
    u32 flags;                          // 0x324
    s32 area;                           // 0x328, set by evt_puni_setup (args[0] + 1), also written to SW 0x299
    f32 levelY[4];                      // 0x32C, the four heights puniSetYPosition chooses between
    s32 yMode;                          // 0x33C, switch in puniSetYPosition
    s32 mapKind;                        // 0x340, which puniMapdata_*_getidx to use: 0 none, 1 MRI_06, 2 MRI_09, 3 MRI_16
    Vec respawnPos;                     // 0x344
    f32 bandTop0;                       // 0x350
    f32 bandBottom0;                    // 0x354
    f32 bandTop1;                       // 0x358
    f32 bandBottom1;                    // 0x35C
    struct PuniData* marioPuni;         // 0x360, Punidat[0], the entry that tracks Mario
    struct PuniData* switchPuni[4];     // 0x364, the puni bound to SW 0xB3C..0xB3F
    PuniTarget targets[10];             // 0x374
    s32 phase;                          // 0x43C, set to 2/3/5 by mode changes, 0 when idle
    s32 prevPhase;                      // 0x440
    BOOL showPuniCounter;               // 0x444
    BOOL blinkCount;                    // 0x448, puni_disp_info_sub sets it while count != total; the count then blinks
    s32 puniCounterAlpha;               // 0x44C
    s32 randSeedA;                      // 0x450, puni_rnd / puniQrnd state
    s32 randSeedB;                      // 0x454
    u32 sfx;                            // 0x458, an SFX handle evt_puni_force_se_off stops; nothing in mri sets it
    s32 soundNum;                       // 0x45C, puniSound's count
    s32 soundLevel;                     // 0x460, 0..6 bucket of soundNum
    u8 unk464[0x468 - 0x464];           // 0x464
    u32 moveSfx;                        // 0x468, SFX_STG2_TOGE_MOVE handle
    s32 moveCount;                      // 0x46C
    s32 moveLevel;                      // 0x470, 0..3 bucket of moveCount
    s32 prevMoveLevel;                  // 0x474
    u32 togettiSfx;                     // 0x478, SFX_STG2_TOGETTI_MOVE1 handle
    s32 field_0x47C;                    // 0x47C
    Vec arrivePos;                      // 0x480
    Vec arrivePos2;                     // 0x48C
    u8 unk498[0x4B0 - 0x498];           // 0x498
    f32 arriveDistSq;                   // 0x4B0, squared radius around arrivePos
    f32 arriveDistSq2;                  // 0x4B4, squared radius around arrivePos2
    u8 unk4B8[0x4C0 - 0x4B8];           // 0x4B8
    s32 fightSfxCount0;                 // 0x4C0, SFX_STG2_FIGHT_TOGE1 plays
    s32 fightSfxCount1;                 // 0x4C4, SFX_STG2_FIGHT_TOGE2 plays
    u8 unk4C8[0x4CC - 0x4C8];           // 0x4C8
    s32 windDisabled;                   // 0x4CC, evt_puni_winddisable_onoff; nonzero skips the cloudGetBreathPower wind
} PuniWork;  // 0x4D0

typedef struct PuniData {
    PuniPose* pose;                     // 0x0, the punidrv pose this entry drives (puni_puni_init)
    s32 flags;                          // 0x4
    u32 statusFlags;                    // 0x8, PUNI_STATUS_*
    u16 mode;                           // 0xC, PuniMode
    u16 prevMode;                       // 0xE, PuniMode to return to after an interruption such as PUNI_MODE_WIND
    u8 unk10[0x114 - 0x10];             // 0x10
    u16 state;                          // 0x114
    s16 angle;                          // 0x116
    s16 treeId;                         // 0x118, swarm tree the puni belongs to, copied from the parent (searchParent); -1 when unlinked
    s16 depth;                          // 0x11A, parent depth + 1 (searchParent); PuniDokuritsu and _daime_change decrement it
    u16 animIdx;                        // 0x11C
    u16 prevAnimIdx;                    // 0x11E
    u8 animDebounce;                    // 0x120, frames a new animIdx waits before it sticks (puniModeParent, counts to 8)
    s8 healIconTime;                    // 0x121, frames the heal icon (statusFlags 0x1) has been up; cleared after 30 (evt_icon_disp_func)
    u8 unk122[0x124 - 0x122];           // 0x122
    s32 animTime;                       // 0x124
    s32 timer;                          // 0x128
    Vec pos;                            // 0x12C
    Vec prevPos;                        // 0x138, pos at the end of the last frame (puni_puni_main)
    Vec dispOffset;                     // 0x144, added to pos for the drawn position (puniAnimation)
    Vec waterPos;                       // 0x150, where puniModeWind_sub hit water; puniModeWater keeps y as the surface and sets x/z to the landing point
    Vec vel;                            // 0x15C
    Vec awayDir;                        // 0x168, escape direction; -hitNormal after a wall or water hit
    f32 maxSpeed;                       // 0x174, clamp on the vel magnitude (puniMoveLimitedNormal, puniMoveDecel)
    f32 gravity;                        // 0x178
    u8 unk17C[0x180 - 0x17C];           // 0x17C
    f32 direction;                      // 0x180
    f32 rotZ;                           // 0x184, npc->rotation.z / PuniPose rotZ; evt_puni_korokoro spins it
    f32 rotOffsetY;                     // 0x188, npc->rotationOffset.y / PuniPose rotOffsetY, the spin pivot (8.0 while korokoro spins)
    f32 scale;                          // 0x18C
    f32 radius;                         // 0x190, body radius: two punis touch at radius + radius (puniOption); Away and Wind also use it as the step length along awayDir
    f32 spacing;                        // 0x194, puniOption: two punis that touched push apart out to radii + reach + spacing; the push latch clears once nothing is within radii + reach
    f32 reach;                          // 0x198, see spacing; puniModeParent pulls toward the parent past radii + spacing + reach
    f32 attackRange;                    // 0x19C, an attacker closes in until within the target's attackRange, then calls puniParaAttack (10.0)
    f32 windPower;                      // 0x1A0, cloudGetBreathPower result; over 10 blows the puni into PUNI_MODE_WIND
    f32 marioDist;                      // 0x1A4, distance to Mario, 1000 when more than 32 units apart in height; puniAnimation offsets the PARENT animation phase by marioDist / 8
    f32 parentDist;                     // 0x1A8, distance to the parent, set each frame in puniModeParent
    f32 speed;                          // 0x1AC, |pos - prevPos|, distance moved last frame (puni_puni_main)
    f32 parentDistPrev;                 // 0x1B0, parentDist from the previous frame
    s32 nearAttractIdx;                 // 0x1B4, nearest PUNI_TARGET_ATTRACT entry in Punigp->targets (puniTargetSearch), -1 for none
    f32 nearAttractDistSq;              // 0x1B8, squared distance to it
    s32 nearRepelIdx;                   // 0x1BC, nearest PUNI_TARGET_REPEL entry, -1 for none
    f32 nearRepelDistSq;                // 0x1C0, squared distance to it
    u8 unk1C4[0x1C8 - 0x1C4];           // 0x1C4
    OSTime modeStartTime;               // 0x1C8
    struct PuniData* parentPuni;        // 0x1D0
    struct PuniData* leftPuni;          // 0x1D4
    struct PuniData* rightPuni;         // 0x1D8
    struct PuniData* target;            // 0x1DC, fight opponent, or Punigp->marioPuni while blown
    s32 subState;                       // 0x1E0
    s32 counter;                        // 0x1E4, per-mode scratch: Mass push frames (to 30), Awayjump's second-animation latch, Hagure's random 5..10 repeat count
    s32 turnsLeft;                      // 0x1E8, puniModeHagure: turns left (from counter); odd faces 90, even 270
    f32 height;                         // 0x1EC
    f32 vertVel;                        // 0x1F0
    f32 jumpVelStart;                   // 0x1F4
    Vec accel;                          // 0x1F8
    Vec moveTarget;                     // 0x204, destination of evt_puni_move_position (x and z; y keeps the start height)
    struct HitEntry* hitVel;            // 0x210, HitEntry from puniMoveAtari's first check, along vel
    struct HitEntry* hitDir;            // 0x214, HitEntry from puniMoveAtari's second check, from the start point
    struct EffectEntry* effect;         // 0x218
    u8 unk21C[0x2AC - 0x21C];           // 0x21C
    Vec fukidashiPos;                   // 0x2AC, puniModeHagure's fukidashi reads it as the position of a MarioWork at 0x220 (y -1000 hides it)
    u8 unk2B8[0x560 - 0x2B8];           // 0x2B8
    Vec hitNormal;                      // 0x560, surface normal of the last puniMoveAtari hit
    void (*modeFunc)(struct PuniData*); // 0x56C, PUNI_MODE_CUSTOM handler (puniModeCustom), set by the map RELs
    void (*moveFunc)(struct PuniData*); // 0x570, per-puni move callback (puniMoveCustom), set by the map RELs
    s16 hp;                             // 0x574, puniParaRecover, puniParaDamage
    s16 maxHp;                          // 0x576, the danger and self checks divide hp by it
    s16 hpRegen;                        // 0x578, hp added per regen tick (puniParaRecover)
    s16 regenTimer;                     // 0x57A, frames until the next regen tick; reset while moving, blown or stunned
    s16 regenInterval;                  // 0x57C, regenTimer reloads to this (60 frames)
    s16 attack;                         // 0x57E, the loser of puniParaAttack takes the winner's attack - its own defense
    s16 defense;                        // 0x580
    s16 field_0x582;                    // 0x582, set to 10 at init, never read here
    s16 damage;                         // 0x584, pending damage, applied in puniParaDamage
    s16 foughtThisFrame;                // 0x586, set by puniParaAttack on both sides; puni_puni_main clears it each frame
    s16 fightWeight;                    // 0x588, puniParaAttack: the attacker wins with odds own / (own + defender's) (50)
    s16 chaseWeight;                    // 0x58A, puniParaAttack against a PUNI_MODE_ESCAPE defender: odds own / (own + 100 - defender's) (70)
    s32 recoverType;                    // 0x58C, puniPositionCheck result that sent the puni to puniPositionRecover
    Vec lostPos;                        // 0x590, pos before puniPositionRecover
    Vec recoveredPos;                   // 0x59C, pos after puniPositionRecover
} PuniData;  // 0x5A8

typedef struct PuniMap {
    f32 yMax;       // 0x0, puniMapFindbuff: Mario above this is off the map
    f32 yMin;       // 0x4, puniMapFindbuff: Mario below this is off the map
    s32 width;      // 0x8
    s32 height;     // 0xC
    s32 dataSize;   // 0x10, width * height * 4 bytes (twice what the u16 grid needs)
    s32 hasData;    // 0x14, 0 until puniMapDataSet gets a grid
    void* gridSrc;  // 0x18, the source grid memcpy'd into grid on each refresh
    s32 valid;      // 0x1C, set by puniMapFindbuff; 0 when Mario is off the map
    u16* grid;      // 0x20, cells puniMapFindbuff flood-fills outward from Mario's cell each frame
} PuniMap; // 0x24

typedef struct PuniTexInfo {
    char* name;     // 0x0, animPoseDisp_MakeExtTexture's arguments (puniMakeTexture); NULL ends the table
    char* animName; // 0x4
    f32 frame;      // 0x8
} PuniTexInfo;

USER_FUNC(evt_puni_mem_init);

PuniData* puniGetPunidat(void);
PuniWork* puniGetPunigp(void);
void puniswGetindex(u32 base, s32 index, s32* out, Vec* pos);
void puniswTrapGetindex(s32 index, s32* out);
s32 puniMoveAtari(PuniData* punidat, Vec pos, Vec vel, s32 arg3, f32 mag);
void puniMoveDecel(PuniData* punidat);

PuniTexInfo* puniGetTex(void);
void evt_icon_disp_func(CameraId camId, void* param);
PuniTexInfo* _make_texbuf(PuniTexInfo* texbuf, s32 count, const char* a3, const char* a4, f32 a5);
USER_FUNC(evt_make_texbuf);
void puniAnimCount(s16* anim, s16* count, s16* term);
void puniAnimationInit(s16** anim, s16** anim2);
void puniAnimationInit2(void);
void puniAnimation(PuniData* dat, PuniPose* pose);
f32 _chukanhosei(f32 a, f32 b, f32 c);
s32 _areacheck(f32 x, f32 z, f32 range, f32 cx, f32 cz);
void _ymode_mri_09(PuniData* dat);
void _ymode_mri_16_00(PuniData* puni);
void _ymode_mri_16_01(PuniData* puni);
void puniSetYPosition(PuniData* puni);
void puniMapdata_MRI_06_getidx(s32* outX, s32* outZ, f64 x, f64 z);
void puniMapdata_MRI_06_getpos(s32 idx, s32 sub, f32* outX, f32* outZ);
void puniMapdata_MRI_09_getidx(s32* outX, s32* outZ, f64 x, f64 z);
void puniMapdata_MRI_09_getpos(s32 idx, s32 sub, f32* outX, f32* outZ);
void puniMapdata_MRI_16_getidx(s32* out1, s32* out2, f32 x, f32 z);
void puniMapdata_MRI_16_getpos(s32 a, s32 b, f32* outX, f32* outZ);
void puniMapPos2Blk(s32* out1, s32* out2, f32 x, f32 z);
void puniMapBlk2Pos(s32 a, s32 b, f32* outX, f32* outZ);
u16 puniMapGetInfo(s32 x, s32 y);
void puniMapInit(void);
void puniMapDataSet(s32 w, s32 h, f32 a, f32 b, void* data);
void puniMapFindbuff(Vec* pos);
void puniTargetSearch(PuniData* punidat);
void puniswSetindex(u32 base, s32 index, s32* out, Vec* pos);
void puniswTrapSetindex(s32 index, s32* value);
s32 randi(s32 min, s32 max);
void qrndseed(s32 seed);
s32 qrnd(s32 max);
void puniDeleteEffect(PuniData* puni);
void _daime_change(PuniData* puni);
struct PuniData* PuniSakanobori(struct PuniData* puni);
void PuniChildHagure(PuniData* puni);
s32 searchParent(PuniData* puni);
s32 searchMario(PuniData* puni);
s32 puniPositionCheck(PuniData* puni);
void puniPositionRecover(PuniData* puni);
void unk_mri_000095f0(PuniData* puni, Vec pos, Vec vel, f32 mag);
void unk_mri_0000980c(PuniData* puni, Vec pos, Vec vel, f32 mag);
void puniMoveLimitedNormal(PuniData* puni);
void puniMoveNormal(PuniData* puni);
void puniMoveSimple(PuniData* puni);
void puniMoveCustom(PuniData* puni);
void puniParaAttack(PuniData* puni, PuniData* other);
void puniParaDamage(PuniData* puni);
void puniParaRecover(PuniData* puni);
s32 puniParaDangerCheck(PuniData* puni);
s32 puniParaSelfCheck(PuniData* puni);
s32 puniParaRecoverCheck(PuniData* puni);
PuniData* puniSearchRandom(s32 start, s32 count);
PuniData* puniSearchRandom_Mikata(void);
PuniData* puniSearchRandom_Teki(void);
PuniData* searchBattle(PuniData* puni, s32 start, s32 count, f32 maxDist);
PuniData* searchMikata(PuniData* puni);
PuniData* searchTeki(PuniData* puni);
PuniData* searchBattleMikata(PuniData* puni);
PuniData* searchBattleTeki(PuniData* puni);
void puniSearch(PuniData* puni);
void puniModeChangeAttack(void);  // TODO: real signature
void puniModeChangeAway(void);  // TODO: real signature
void puniModeChangeFallCheck(void);  // TODO: real signature
void puniFukki(PuniData* puni);
void puniModeALLCheck(void);  // TODO: real signature
void puniModeALLEndCheck(PuniData* punidat);
void puniModeNormal(PuniData* punidat);
void puniModeParentpre(PuniData* punidat);
void puniModeParent(PuniData* punidat);
void puniModeSearch(PuniData* puni);
void puniModeMass(PuniData* punidat);
void puniModeAwayjump(PuniData* punidat);
s32 puniModeAway_sub(PuniData* punidat);
void puniModeAway(PuniData* punidat);
void puniModeAttack(PuniData* punidat);
void puniModeDamage(PuniData* punidat);
void puniModePiyori(PuniData* punidat);
void puniModeEscape(PuniData* punidat);
void puniModeRecover(PuniData* punidat);
void puniModeDead(PuniData* punidat);
s32 puniModeWind_sub(PuniData* punidat);
void puniModeWind_kaiten(PuniData* punidat);
void puniModeWind(PuniData* punidat);
void puniModeFall(PuniData* punidat);
void puniModePreupdraft(PuniData* punidat);
void puniModeUpdraft(PuniData* punidat);
void puniModeWonder(PuniData* punidat);
void puniModeFunbari(PuniData* punidat);
void puniModeWater(PuniData* punidat);
void puniModePrehagure(PuniData* puni);
void puniModeHagure(PuniData* puni);
void puniModeDodai(PuniData* puni);
void puniModeTrap_sub(PuniData* puni);
void puniModeTrap(PuniData* puni);
void puniModeEnterSubInit(PuniData* puni);
void puniModeEnterSub(PuniData* puni);
void puniModeEnter(PuniData* puni);
void puniModeExitSubInit(PuniData* puni);
void puniModeExitSub(PuniData* puni);
void puniModeExit(PuniData* punidat);
void puniModePoint(PuniData* punidat);
void puniModeDokan(PuniData* punidat);
void puniModeCustom(PuniData* punidat);
void puni_puni_disp(CameraId cameraId, void* param);
void puni_puni_shadow_disp(CameraId camId, void* param);
void puni_puni_init(void);  // TODO: real signature
void unk_mri_00012d58(PuniData* puni);
void fn_19_13A90(PuniData* puni);
void puniOption(void);  // TODO: real signature
void unk_mri_00014ad4(void);  // TODO: real signature
void puniSound(void);  // TODO: real signature
void puni_puni_main(void);  // TODO: real signature
void puni_puni_dispent(void);  // TODO: real signature
USER_FUNC(evt_puni_init);
USER_FUNC(evt_puni_set_yuka);
USER_FUNC(evt_puni_set_hamari);
USER_FUNC(evt_puni_set_maze);
USER_FUNC(evt_puni_main);
f32 puni_init_posy(f32 y);
void puni_init_para(PuniData* punidat, f32 x, f32 y, f32 z);
void toge_init_para(PuniData* punidat, f32 x, f32 y, f32 z);
USER_FUNC(evt_puni_rnd_pos);
USER_FUNC(evt_puni_get_systemmode);
USER_FUNC(evt_puni_set_systemmode);
USER_FUNC(evt_puni_wait_systemmode);
USER_FUNC(evt_puni_debug_info);
USER_FUNC(evt_puni_setup);
USER_FUNC(evt_toge_setup);
s32 _puniVAtari(PuniData* punidat, Vec* src, Vec* dest);
void puniHagurePos(PuniData* punidat);
USER_FUNC(evt_puni_exit);
USER_FUNC(evt_puni_init_puni);
USER_FUNC(evt_puni_init_teki);
USER_FUNC(evt_puni_get_position);
USER_FUNC(evt_puni_set_position);
USER_FUNC(evt_puni_get_dir);
USER_FUNC(evt_puni_set_dir);
USER_FUNC(evt_puni_get_anim);
USER_FUNC(evt_puni_set_anim);
USER_FUNC(evt_puni_korokoro);
USER_FUNC(evt_puni_move_position);
USER_FUNC(evt_puni_move_position2);
USER_FUNC(evt_puni_jump_position);
USER_FUNC(evt_puni_get_mode);
USER_FUNC(evt_puni_set_mode);
USER_FUNC(evt_puni_setsudan);
USER_FUNC(evt_puni_get_flag);
USER_FUNC(evt_puni_flag_onoff);
USER_FUNC(evt_puni_dokan_onoff);
USER_FUNC(evt_puni_get_lsw_index);
USER_FUNC(evt_puni_set_lsw_index);
USER_FUNC(evt_puni_get_trap_index);
USER_FUNC(evt_puni_set_trap_index);
USER_FUNC(evt_puni_set_ymode);
USER_FUNC(evt_puni_all_fall_check);
USER_FUNC(evt_puni_all_movestop);
USER_FUNC(evt_puni_count);
USER_FUNC(evt_puni_count_area);
USER_FUNC(evt_puni_zenmetsu_check);
USER_FUNC(evt_puni_landon_check);
USER_FUNC(evt_puni_pera_onoff);
USER_FUNC(evt_puni_fade_onoff);
USER_FUNC(evt_puni_attack_on);
USER_FUNC(evt_puni_puninum_onoff);
USER_FUNC(evt_puni_puninum_alpha);
USER_FUNC(evt_puni_winddisable_onoff);
USER_FUNC(evt_puni_force_se_off);
USER_FUNC(evt_puni_all_save);
USER_FUNC(evt_target_flag_onoff);
USER_FUNC(evt_target_set_position);
USER_FUNC(evt_target_set_soko);
const char* npcGetTribeName(NpcTribeInfo* tribe);
USER_FUNC(puni_piders_check_func);
USER_FUNC(puni_disp_info_kassencheck);
void puniCountDisp(CameraId cameraId, void* param);
USER_FUNC(puni_disp_info_sub);
EVT_DEF(puni_piders_check);
EVT_DEF(puni_disp_info);
EVT_DEF(mri_puninum_fadeout);
EVT_DEF(mri_puninum_fadein);
EVT_DEF(mri_puni_position_init);
EVT_DEF(mri_puni_main);
EVT_DEF(mri_puni_init_dvd);
EVT_DEF(mri_puni_init);
EVT_DEF(mri_puni_init_none);
void PuniDokuritsu(PuniData* puni);
