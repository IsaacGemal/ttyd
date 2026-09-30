#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "evt/evt_cmd.h"
#include "driver/camdrv.h"

#define PUNI_DATA_MAX 211
#define PUNI_TEX_INFO_MAX 200

typedef struct PuniWork {
    s32 field_0x0;                      // 0x0
    u8 unk4[0x324 - 0x4];               // 0x4
    s32 field_0x324;                    // 0x324
    s32 unk328;                         // 0x328
    f32 field_0x32C;                    // 0x32C
    f32 field_0x330;                    // 0x330
    f32 field_0x334;                    // 0x334
    f32 field_0x338;                    // 0x338
    s32 field_0x33C;                    // 0x33C
    s32 field_0x340;                    // 0x340
    Vec field_0x344;                    // 0x344
    f32 field_0x350;                    // 0x350
    f32 field_0x354;                    // 0x354
    f32 field_0x358;                    // 0x358
    f32 field_0x35C;                    // 0x35C
    s32 field_0x360;                    // 0x360
    s32 field_0x364;                    // 0x364
    s32 field_0x368;                    // 0x368
    s32 field_0x36C;                    // 0x36C
    s32 field_0x370;                    // 0x370
    s32 field_0x374;                    // 0x374
    Vec field_0x378;                    // 0x378
    s32 field_0x384;                    // 0x384
    u8 unk388[0x43C - 0x388];           // 0x388
    s32 field_0x43C;                    // 0x43C
    s32 field_0x440;                    // 0x440
    BOOL showPuniCounter;               // 0x444
    BOOL unk448;                        // 0x448
    s32 puniCounterAlpha;               // 0x44C
    s32 field_0x450;                    // 0x450
    s32 field_0x454;                    // 0x454
    s32 field_0x458;                    // 0x458
    s32 field_0x45C;                    // 0x45C
    s32 field_0x460;                    // 0x460
    u8 unk464[0x468 - 0x464];           // 0x464
    s32 field_0x468;                    // 0x468
    s32 field_0x46C;                    // 0x46C
    s32 field_0x470;                    // 0x470
    s32 field_0x474;                    // 0x474
    s32 field_0x478;                    // 0x478
    s32 field_0x47C;                    // 0x47C
    Vec field_0x480;                    // 0x480
    Vec field_0x48C;                    // 0x48C
    u8 unk498[0x4B0 - 0x498];           // 0x498
    f32 field_0x4B0;                    // 0x4B0
    f32 field_0x4B4;                    // 0x4B4
    u8 unk4B8[0x4C0 - 0x4B8];           // 0x4B8
    s32 field_0x4C0;                    // 0x4C0
    s32 field_0x4C4;                    // 0x4C4
    u8 unk4C8[0x4CC - 0x4C8];           // 0x4C8
    s32 field_0x4CC;                    // 0x4CC
} PuniWork;  // 0x4D0

typedef struct PuniData {
    s32 field_0x0;                      // 0x0
    s32 unk4;                           // 0x4
    s32 field_0x8;                      // 0x8
    u16 unkC;                           // 0xC
    s16 field_0xE;                      // 0xE
    u8 unk10[0x114 - 0x10];             // 0x10
    u16 field_0x114;                    // 0x114
    s16 field_0x116;                    // 0x116
    s16 field_0x118;                    // 0x118
    s16 field_0x11A;                    // 0x11A
    u16 field_0x11C;                    // 0x11C
    s16 field_0x11E;                    // 0x11E
    u8 unk120[0x121 - 0x120];           // 0x120
    u8 field_0x121;                     // 0x121
    u8 unk122[0x124 - 0x122];           // 0x122
    s32 field_0x124;                    // 0x124
    s32 field_0x128;                    // 0x128
    Vec field_0x12C;                    // 0x12C
    Vec field_0x138;                    // 0x138
    f32 field_0x144;                    // 0x144
    f32 field_0x148;                    // 0x148
    f32 field_0x14C;                    // 0x14C
    u8 unk150[0x15C - 0x150];           // 0x150
    Vec field_0x15C;                    // 0x15C
    u8 unk168[0x174 - 0x168];           // 0x168
    f32 field_0x174;                    // 0x174
    u8 unk178[0x180 - 0x178];           // 0x178
    f32 field_0x180;                    // 0x180
    f32 field_0x184;                    // 0x184
    f32 field_0x188;                    // 0x188
    f32 field_0x18C;                    // 0x18C
    f32 field_0x190;                    // 0x190
    f32 field_0x194;                    // 0x194
    f32 field_0x198;                    // 0x198
    u8 unk19C[0x1A0 - 0x19C];           // 0x19C
    f32 field_0x1A0;                    // 0x1A0
    f32 field_0x1A4;                    // 0x1A4
    f32 field_0x1A8;                    // 0x1A8
    f32 field_0x1AC;                    // 0x1AC
    f32 field_0x1B0;                    // 0x1B0
    s32 field_0x1B4;                    // 0x1B4
    f32 field_0x1B8;                    // 0x1B8
    s32 field_0x1BC;                    // 0x1BC
    f32 field_0x1C0;                    // 0x1C0
    u8 unk1C4[0x1C8 - 0x1C4];           // 0x1C4
    s32 field_0x1C8;                    // 0x1C8
    s32 field_0x1CC;                    // 0x1CC
    s32 field_0x1D0;                    // 0x1D0
    s32 field_0x1D4;                    // 0x1D4
    s32 field_0x1D8;                    // 0x1D8
    s32 field_0x1DC;                    // 0x1DC
    u8 unk1E0[0x1E4 - 0x1E0];           // 0x1E0
    s32 field_0x1E4;                    // 0x1E4
    u8 unk1E8[0x1EC - 0x1E8];           // 0x1E8
    f32 field_0x1EC;                    // 0x1EC
    f32 field_0x1F0;                    // 0x1F0
    u8 unk1F4[0x1F8 - 0x1F4];           // 0x1F4
    Vec field_0x1F8;                    // 0x1F8
    Vec field_0x204;                    // 0x204
    s32 field_0x210;                    // 0x210
    s32 field_0x214;                    // 0x214
    s32 field_0x218;                    // 0x218
    u8 unk21C[0x560 - 0x21C];           // 0x21C
    s32 field_0x560;                    // 0x560
    s32 field_0x564;                    // 0x564
    s32 field_0x568;                    // 0x568
    s32 field_0x56C;                    // 0x56C
    s32 field_0x570;                    // 0x570
    s16 field_0x574;                    // 0x574
    s16 field_0x576;                    // 0x576
    s16 field_0x578;                    // 0x578
    s16 field_0x57A;                    // 0x57A
    s16 field_0x57C;                    // 0x57C
    s16 field_0x57E;                    // 0x57E
    s16 field_0x580;                    // 0x580
    s16 field_0x582;                    // 0x582
    s16 field_0x584;                    // 0x584
    s16 field_0x586;                    // 0x586
    s16 field_0x588;                    // 0x588
    s16 field_0x58A;                    // 0x58A
    s32 field_0x58C;                    // 0x58C
    s32 field_0x590;                    // 0x590
    s32 field_0x594;                    // 0x594
    s32 field_0x598;                    // 0x598
    s32 field_0x59C;                    // 0x59C
    s32 field_0x5A0;                    // 0x5A0
    s32 field_0x5A4;                    // 0x5A4
} PuniData;  // 0x5A8

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
void puniMoveAtari(PuniData* punidat, Vec pos, Vec vel, s32 arg3, f32 mag);
void puniMoveDecel(PuniData* punidat);

PuniTexInfo* puniGetTex(void);
void evt_icon_disp_func(CameraId camId, void* param);
PuniTexInfo* _make_texbuf(PuniTexInfo* texbuf, s32 count, const char* a3, const char* a4, f32 a5);
USER_FUNC(evt_make_texbuf);
void puniAnimCount(void);  // TODO: real signature
void puniAnimationInit(void);  // TODO: real signature
void puniAnimationInit2(void);  // TODO: real signature
void puniAnimation(void);  // TODO: real signature
void _chukanhosei(void);  // TODO: real signature
void _areacheck(void);  // TODO: real signature
void _ymode_mri_09(void);  // TODO: real signature
void _ymode_mri_16_00(void);  // TODO: real signature
void _ymode_mri_16_01(void);  // TODO: real signature
void puniSetYPosition(void);  // TODO: real signature
void puniMapdata_MRI_06_getidx(void);  // TODO: real signature
void puniMapdata_MRI_06_getpos(void);  // TODO: real signature
void puniMapdata_MRI_09_getidx(void);  // TODO: real signature
void puniMapdata_MRI_09_getpos(void);  // TODO: real signature
void puniMapdata_MRI_16_getidx(void);  // TODO: real signature
void puniMapdata_MRI_16_getpos(void);  // TODO: real signature
void puniMapPos2Blk(void);  // TODO: real signature
void puniMapBlk2Pos(void);  // TODO: real signature
void puniMapGetInfo(void);  // TODO: real signature
void puniMapInit(void);  // TODO: real signature
void puniMapDataSet(void);  // TODO: real signature
void puniMapFindbuff(void);  // TODO: real signature
void puniTargetSearch(void);  // TODO: real signature
void puniswSetindex(void);  // TODO: real signature
void puniswTrapSetindex(void);  // TODO: real signature
void randi(void);  // TODO: real signature
void qrndseed(void);  // TODO: real signature
void qrnd(void);  // TODO: real signature
void puniDeleteEffect(void);  // TODO: real signature
void _daime_change(void);  // TODO: real signature
void PuniSakanobori(void);  // TODO: real signature
void PuniChildHagure(void);  // TODO: real signature
void searchParent(void);  // TODO: real signature
void searchMario(void);  // TODO: real signature
void puniPositionCheck(void);  // TODO: real signature
void puniPositionRecover(void);  // TODO: real signature
void unk_mri_000095f0(void);  // TODO: real signature
void unk_mri_0000980c(void);  // TODO: real signature
void puniMoveLimitedNormal(void);  // TODO: real signature
void puniMoveNormal(void);  // TODO: real signature
void puniMoveSimple(void);  // TODO: real signature
void puniMoveCustom(void);  // TODO: real signature
void puniParaAttack(void);  // TODO: real signature
void puniParaDamage(void);  // TODO: real signature
void puniParaRecover(void);  // TODO: real signature
void puniParaDangerCheck(void);  // TODO: real signature
void puniParaSelfCheck(void);  // TODO: real signature
void puniParaRecoverCheck(void);  // TODO: real signature
void puniSearchRandom(void);  // TODO: real signature
void puniSearchRandom_Mikata(void);  // TODO: real signature
void fn_19_A820(void);  // TODO: real signature
void puniSearchRandom_Teki(void);  // TODO: real signature
void fn_19_A8DC(void);  // TODO: real signature
void searchBattle(void);  // TODO: real signature
void searchMikata(void);  // TODO: real signature
void searchTeki(void);  // TODO: real signature
void searchBattleMikata(void);  // TODO: real signature
void searchBattleTeki(void);  // TODO: real signature
void puniSearch(void);  // TODO: real signature
void puniModeChangeAttack(void);  // TODO: real signature
void puniModeChangeAway(void);  // TODO: real signature
void puniModeChangeFallCheck(void);  // TODO: real signature
void puniFukki(void);  // TODO: real signature
void puniModeALLCheck(void);  // TODO: real signature
void puniModeALLEndCheck(void);  // TODO: real signature
void puniModeNormal(void);  // TODO: real signature
void puniModeParentpre(void);  // TODO: real signature
void puniModeParent(void);  // TODO: real signature
void puniModeSearch(void);  // TODO: real signature
void puniModeMass(void);  // TODO: real signature
void puniModeAwayjump(void);  // TODO: real signature
void puniModeAway_sub(void);  // TODO: real signature
void puniModeAway(void);  // TODO: real signature
void puniModeAttack(void);  // TODO: real signature
void puniModeDamage(void);  // TODO: real signature
void puniModePiyori(void);  // TODO: real signature
void puniModeEscape(void);  // TODO: real signature
void puniModeRecover(void);  // TODO: real signature
void puniModeDead(void);  // TODO: real signature
void puniModeWind_sub(void);  // TODO: real signature
void puniModeWind_kaiten(void);  // TODO: real signature
void puniModeWind(void);  // TODO: real signature
void puniModeFall(void);  // TODO: real signature
void puniModePreupdraft(void);  // TODO: real signature
void puniModeUpdraft(void);  // TODO: real signature
void puniModeWonder(void);  // TODO: real signature
void puniModeFunbari(void);  // TODO: real signature
void puniModeWater(void);  // TODO: real signature
void puniModePrehagure(void);  // TODO: real signature
void puniModeHagure(void);  // TODO: real signature
void puniModeDodai(void);  // TODO: real signature
void puniModeTrap_sub(void);  // TODO: real signature
void puniModeTrap(void);  // TODO: real signature
void puniModeEnterSubInit(void);  // TODO: real signature
void puniModeEnterSub(void);  // TODO: real signature
void puniModeEnter(void);  // TODO: real signature
void puniModeExitSubInit(void);  // TODO: real signature
void puniModeExitSub(void);  // TODO: real signature
void puniModeExit(void);  // TODO: real signature
void puniModePoint(void);  // TODO: real signature
void puniModeDokan(void);  // TODO: real signature
void puniModeCustom(void);  // TODO: real signature
void puni_puni_disp(void);  // TODO: real signature
void puni_puni_shadow_disp(void);  // TODO: real signature
void puni_puni_init(void);  // TODO: real signature
void unk_mri_00012d58(void);  // TODO: real signature
void fn_19_13A90(void);  // TODO: real signature
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
void puni_init_posy(void);  // TODO: real signature
void puni_init_para(void);  // TODO: real signature
void toge_init_para(void);  // TODO: real signature
USER_FUNC(evt_puni_rnd_pos);
USER_FUNC(evt_puni_get_systemmode);
USER_FUNC(evt_puni_set_systemmode);
USER_FUNC(evt_puni_wait_systemmode);
USER_FUNC(evt_puni_debug_info);
USER_FUNC(evt_puni_setup);
USER_FUNC(evt_toge_setup);
void _puniVAtari(void);  // TODO: real signature
void puniHagurePos(void);  // TODO: real signature
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
void npcGetTribeName(void);  // TODO: real signature
USER_FUNC(puni_piders_check_func);
USER_FUNC(puni_disp_info_kassencheck);
void puniCountDisp(void);  // TODO: real signature
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
