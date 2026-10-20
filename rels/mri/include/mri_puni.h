#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "evt/evt_cmd.h"
#include "driver/camdrv.h"
#include "punidrv.h"

#define PUNI_DATA_MAX 211
#define PUNI_TEX_INFO_MAX 200

// One entry of PuniWork's target table at 0x374.
typedef struct PuniTarget {
    s32 flags;                          // 0x0
    Vec position;            // 0x4
    struct HitEntry* hit;    // 0x10
} PuniTarget; // 0x14

typedef struct MriPuniAnimSet {
    s16* unk0[10]; // 0x00
    s16 unk28[10]; // 0x28
    s16 unk3C[10]; // 0x3C
} MriPuniAnimSet; // 0x50

typedef struct PuniWork {
    s32 systemMode;                     // 0x0, evt_puni_get/set_systemmode
    MriPuniAnimSet animSet[2];          // 0x4
    char puniNames[4][10][16];          // 0xA4, four 10-entry name tables
    u32 flags;                          // 0x324
    s32 area;                           // 0x328, set by evt_puni_setup (args[0] + 1), also written to SW 0x299
    f32 levelY[4];                      // 0x32C, the four heights puniSetYPosition chooses between
    s32 yMode;                          // 0x33C, switch in puniSetYPosition
    s32 mapKind;                        // 0x340, 0 none, 1 MRI_06 grid, 2 table-driven grid
    Vec respawnPos;                     // 0x344
    f32 bandTop0;                       // 0x350
    f32 bandBottom0;                    // 0x354
    f32 bandTop1;                       // 0x358
    f32 bandBottom1;                    // 0x35C
    s32 marioPuni;                      // 0x360, the PuniData* of Mario's puni (stored as s32; Punidat[0])
    struct PuniData* switchPuni[4];     // 0x364, the puni bound to SW 0xB3C..0xB3F
    PuniTarget targets[10];             // 0x374
    s32 phase;                          // 0x43C, set to 2/3/5 by mode changes, 0 when idle
    s32 prevPhase;                      // 0x440
    BOOL showPuniCounter;               // 0x444
    BOOL unk448;                        // 0x448
    s32 puniCounterAlpha;               // 0x44C
    s32 randSeedA;                      // 0x450, puni_rnd / puniQrnd state
    s32 randSeedB;                      // 0x454
    u32 field_0x458;                    // 0x458
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
    s32 field_0x0;                      // 0x0
    s32 flags;                          // 0x4
    s32 field_0x8;                      // 0x8
    u16 mode;                           // 0xC
    s16 prevMode;                   // 0xE, mode to return to after an interruption (puniModeNormal and puniModeParent set it before the wind mode 13)
    u8 unk10[0x114 - 0x10];             // 0x10
    u16 state;                          // 0x114
    s16 angle;                          // 0x116
    s16 treeId;                     // 0x118, tree the puni belongs to; -1 when unlinked, copied from the parent (searchParent)
    s16 depth;// 0x11A, searchParent sets parent depth + 1; PuniDokuritsu and _daime_change decrement
    u16 animIdx;                        // 0x11C
    u16 prevAnimIdx;                    // 0x11E
    u8 unk120[0x121 - 0x120];           // 0x120
    u8 field_0x121;                     // 0x121
    u8 unk122[0x124 - 0x122];           // 0x122
    s32 animTime;                       // 0x124
    s32 timer;                          // 0x128
    Vec pos;                            // 0x12C
    Vec field_0x138;                    // 0x138
    f32 field_0x144;                    // 0x144
    f32 field_0x148;                    // 0x148
    f32 field_0x14C;                    // 0x14C
    u8 unk150[0x15C - 0x150];           // 0x150
    Vec vel;                            // 0x15C
    u8 unk168[0x174 - 0x168];           // 0x168
    f32 maxSpeed;                   // 0x174, clamp on the vel magnitude (puniMoveLimitedNormal, puniMoveDecel)
    f32 gravity;                        // 0x178
    u8 unk17C[0x180 - 0x17C];           // 0x17C
    f32 direction;                      // 0x180
    f32 field_0x184;                    // 0x184
    f32 field_0x188;                    // 0x188
    f32 scale;                          // 0x18C
    f32 radius;                     // 0x190, collision radius (puniOption adds two); also scales vel in puniModeAway and puniModeWind
    f32 spacing;                    // 0x194, gap kept beyond radius (puniOption, puniModeParent's leash test)
    f32 reach;                      // 0x198, extra range beyond radius + spacing (puniOption)
    u8 unk19C[0x1A0 - 0x19C];           // 0x19C
    f32 windPower;                  // 0x1A0, cloudGetBreathPower result; over 10 blows the puni into mode 13
    f32 field_0x1A4;                    // 0x1A4
    f32 parentDist;                 // 0x1A8, distance to the parent, set each frame in puniModeParent
    f32 speed;                      // 0x1AC, distance moved last frame (puni_puni_main, from pos - prevPos)
    f32 parentDistPrev;             // 0x1B0, parentDist from the previous frame
    s32 nearFlag2Idx;               // 0x1B4, nearest PuniTarget with flag 2 (puniTargetSearch), -1 for none
    f32 nearFlag2DistSq;            // 0x1B8, squared distance to nearFlag2Idx
    s32 nearFlag4Idx;               // 0x1BC, nearest PuniTarget with flag 4 (puniTargetSearch), -1 for none
    f32 nearFlag4DistSq;            // 0x1C0, squared distance to nearFlag4Idx
    u8 unk1C4[0x1C8 - 0x1C4];           // 0x1C4
    OSTime modeStartTime;               // 0x1C8
    s32 parentPuni;                     // 0x1D0
    s32 leftPuni;                       // 0x1D4
    s32 rightPuni;                      // 0x1D8
    s32 target;                         // 0x1DC
    s32 subState;                       // 0x1E0
    s32 field_0x1E4;                    // 0x1E4
    u8 unk1E8[0x1EC - 0x1E8];           // 0x1E8
    f32 height;                         // 0x1EC
    f32 vertVel;                        // 0x1F0
    f32 jumpVelStart;                   // 0x1F4
    Vec accel;                          // 0x1F8
    Vec field_0x204;                    // 0x204
    struct HitEntry* field_0x210;       // 0x210
    struct HitEntry* field_0x214;       // 0x214
    s32 effect;                         // 0x218
    u8 unk21C[0x560 - 0x21C];           // 0x21C
    Vec field_0x560;                    // 0x560
    s32 field_0x56C;                    // 0x56C
    s32 field_0x570;                    // 0x570
    s16 hp;                         // 0x574, current hit points (puniParaRecover, puniParaDamage)
    s16 maxHp;                      // 0x576, hit points cap; the danger and self checks divide hp by it
    s16 hpRegen;                    // 0x578, hp added per regen tick (puniParaRecover)
    s16 regenTimer;                 // 0x57A, frames until the next regen tick, counts down to 0
    s16 regenInterval;              // 0x57C, regenTimer reloads to this (60 frames)
    s16 attack;                     // 0x57E, attacker's strength (puniParaAttack: attack - defense)
    s16 defense;                    // 0x580, subtracted from the attacker's attack
    s16 field_0x582;                    // 0x582
    s16 damage;                     // 0x584, attack - defense, applied in puniParaDamage
    s16 engaged;                    // 0x586, set once the puni is in a para fight; cleared at init
    s16 hitChance;                  // 0x588, attacker's roll base (50) when the defender is not in mode 8
    s16 hitChanceAlt;               // 0x58A, attacker's roll base (70) when the defender is in mode 8
    s32 field_0x58C;                    // 0x58C
    s32 field_0x590;                    // 0x590
    s32 field_0x594;                    // 0x594
    s32 field_0x598;                    // 0x598
    s32 field_0x59C;                    // 0x59C
    s32 field_0x5A0;                    // 0x5A0
    s32 field_0x5A4;                    // 0x5A4
} PuniData;  // 0x5A8

typedef struct PuniMap {
    f32 yMax;            // 0x0, puniMapFindbuff: pos.y above this is off the map
    f32 yMin;            // 0x4, puniMapFindbuff: pos.y below this is off the map
    s32 width;           // 0x8
    s32 height;          // 0xC
    s32 dataSize;        // 0x10, width * height * 4 bytes
    s32 hasData;         // 0x14, 0 until puniMapDataSet gets a grid
    void* gridSrc;       // 0x18, the source grid memcpy'd into grid on each refresh
    s32 valid;           // 0x1C, set by puniMapFindbuff; 0 when the Puni is off the map
    u16* grid;           // 0x20
} PuniMap; // 0x24

typedef struct PuniTexInfo {
    char* unk0; // 0x0
    char* unk4; // 0x4
    f32 unk8;   // 0x8
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
