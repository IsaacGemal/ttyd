#ifdef MUST_MATCH
// The SDK math.h declares `extern inline` float sqrtf/sqrt whose static _half/_three
// doubles are emitted at parse time and duplicate the ones calc3D pools; skip that
// header and take the out-of-line declarations from math_ext.h instead.
#define _MATH_H_
#endif
#include "pmario_sound.h"
#include "matching.h"
#include "sound.h"
#include "system.h"
#include "manager/evtmgr_cmd.h"
#include "driver/seqdrv.h"
#include "mario/mariost.h"
#include <string.h>
#include <dolphin/os.h>
#include <math.h>
#ifdef MUST_MATCH
#include "math_ext.h"
#endif
#pragma pool_data on // the original addresses its .rodata through one base register
//TODO: put this in a folder?

//.data
// psbgmlist is the 262-entry BGM table: each entry is two name/flags pairs.
BGMListEntry psbgmlist[262] = {
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_TITLE1", 0x80000000, "sound/stream/sys_tit1_32k", 0x58400101 },
    { "BGM_DEMO1", 0x80000000, "sound/stream/sys_demo1_32k", 0x5A400001 },
    { "BGM_DEMO2", 0x80000000, "sound/stream/sys_demo2_32k", 0x4D400001 },
    { "BGM_DIGEST1", 0x80000000, "sound/stream/sys_dig1_32k", 0x53400001 },
    { "BGM_FILE_MENU1", 0x80000000, "sound/stream/sys_mmc1_32k", 0x58400101 },
    { "BGM_OPENING1", 0x80000000, "sound/stream/sys_opn1_32k", 0x56400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_ENDING1", 0x80000000, "sound/stream/evt_ed1_32k", 0x50400001 },
    { "BGM_ENDING2", 0x80000000, "sound/stream/evt_ed2_32k", 0x43400001 },
    { "BGM_ENDING3", 0x80000000, "sound/stream/evt_ed3_32k", 0x55400001 },
    { "BGM_ENDING4", 0x80000000, "sound/stream/evt_ed4_32k", 0x4B400001 },
    { "BGM_ENDING5", 0x80000000, "sound/stream/evt_ed5_32k", 0x4B400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_STAFF_ROLL1", 0x80000000, "sound/stream/sys_stf1_32k", 0x5F400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_M_STG0_GOR1", 0x80000000, "sound/stream/stg_gor2_32k", 0x3C400101 },
    { "BGM_M_STG0_TIK1", 0x80000000, "sound/stream/stg_tik2_32k", 0x3C400201 },
    { "BGM_M_STG0_TIK2", 0x80000000, "sound/stream/stg_tik1_32k", 0x5A400301 },
    { "BGM_M_STG0_CSN1", 0x80000000, "sound/stream/stg_gor1_32k", 0x50400401 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_M_STG1_HEI1", 0x80000000, "sound/stream/stg_dor1_32k", 0x46400501 },
    { "BGM_M_STG1_NOK1", 0x80000000, "sound/stream/stg_nok1_32k", 0x53400601 },
    { "BGM_M_STG1_TORIDE1", 0x80000000, "sound/stream/stg_stn1_32k", 0x46400701 },
    { "BGM_M_STG1_GON1", 0x80000000, "sound/stream/stg_gnb1_32k", 0x4E400801 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_M_STG2_WIN1", 0x80000000, "sound/stream/stg_fsg1_32k", 0x46400901 },
    { "BGM_M_STG2_MRI1", 0x80000000, "sound/stream/stg_okk1_32k", 0x5A400A01 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_M_STG3_URN1", 0x80000000, "sound/stream/stg_urg11_32k", 0x5A400B01 },
    { "BGM_M_STG3_TOU1", 0x80000000, "sound/stream/stg_tgm1_32k", 0x50400C01 },
    { "BGM_M_STG3_TOU2", 0x80000000, "sound/stream/stg_tog1_32k", 0x4D400D01 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_M_STG4_USU1", 0x80000000, "sound/stream/stg_ugt1_32k", 0x53400E01 },
    { "BGM_M_STG4_GRA1", 0x80000000, "sound/stream/stg_gra1_32k", 0x55400F01 },
    { "BGM_M_STG4_JIN1", 0x80000000, "sound/stream/stg_dim1_32k", 0x55401001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_M_STG5_MUJ1", 0x80000000, "sound/stream/stg_mjn1_32k", 0x55401101 },
    { "BGM_M_STG5_DOU1", 0x80000000, "sound/stream/stg_duk1_32k", 0x5A401201 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_M_STG6_RSH1", 0x80000000, "sound/stream/stg_rsh_a1_32k", 0x46401301 },
    { "BGM_M_STG6_RSH2", 0x80000000, "sound/stream/stg_rsh_b1_32k", 0x46401401 },
    { "BGM_M_STG6_RSH3", 0x80000000, "sound/stream/stg_rsh_c1_32k", 0x46401501 },
    { "BGM_M_STG6_HOM1", 0x80000000, "sound/stream/stg_hom1_32k", 0x46401601 },
    { "BGM_M_STG6_EKI1", 0x80000000, "sound/stream/stg_eki1_32k", 0x46401701 },
    { "BGM_M_STG6_PIK1", 0x80000000, "sound/stream/stg_pik1_32k", 0x46401801 },
    { "BGM_M_STG6_PIK2", 0x80000000, "sound/stream/stg_snd1_32k", 0x46401901 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_M_STG7_BOM1", 0x80000000, "sound/stream/stg_bmm1_32k", 0x50401A01 },
    { "BGM_M_STG7_MON1", 0x80000000, "sound/stream/stg_gtm1_32k", 0x5A401B01 },
    { "BGM_M_STG7_DUN1", 0x80000000, "sound/stream/stg_tkd1_32k", 0x55401C01 },
    { "BGM_M_STG7_DUN2", 0x80000000, "sound/stream/stg_tkd2_32k", 0x46401D01 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_M_STG8_LST1", 0x80000000, "sound/stream/stg_ldn1_32k", 0x5A501E01 },
    { "BGM_M_STG8_LST2", 0x80000000, "sound/stream/stg_ldn2_32k", 0x5A501F01 },
    { "BGM_M_STG8_LST3", 0x80000000, "sound/stream/stg_ldn3_32k", 0x5A502001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_M_STGK_CASTLE1", 0x80000000, "sound/stream/stg_kpa1_32k", 0x50402101 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_STG0_GOR1", 0x80000000, "sound/stream/stg_gor2_32k", 0x46400000 },
    { "BGM_STG0_TIK1", 0x80000000, "sound/stream/stg_tik2_32k", 0x39400000 },
    { "BGM_STG0_TIK2", 0x80000000, "sound/stream/stg_tik3_32k", 0x5A400000 },
    { "BGM_STG0_CSN1", 0x80000000, "sound/stream/stg_gor1_32k", 0x4B400000 },
    { "BGM_STG0_100DN1", 0x80000000, "sound/stream/stg_tik1_32k", 0x32400000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_STG1_HEI1", 0x80000000, "sound/stream/stg_dor1_32k", 0x46400000 },
    { "BGM_STG1_NOK1", 0x80000000, "sound/stream/stg_nok1_32k", 0x57400000 },
    { "BGM_STG1_TORIDE1", 0x80000000, "sound/stream/stg_stn1_32k", 0x3F400000 },
    { "BGM_STG1_GON1", 0x80000000, "sound/stream/stg_gnb1_32k", 0x44400000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_STG2_WIN1", 0x80000000, "sound/stream/stg_fsg1_32k", 0x46400000 },
    { "BGM_STG2_MRI1", 0x80000000, "sound/stream/stg_okk1_32k", 0x4B400000 },
    { "BGM_STG2_MRI2", 0x80000000, "sound/stream/evt_pun1_32k", 0x48400000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_STG3_URN1", 0x80000000, "sound/stream/stg_urg11_32k", 0x50400000 },
    { "BGM_STG3_TOU1", 0x80000000, "sound/stream/stg_tog1_32k", 0x4D400000 },
    { "BGM_STG3_TOU2", 0x80000000, "sound/stream/stg_tgm1_32k", 0x50400000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_STG4_USU1", 0x80000000, "sound/stream/stg_ugt1_32k", 0x3E400000 },
    { "BGM_STG4_GRA1", 0x80000000, "sound/stream/stg_gra1_32k", 0x2D400000 },
    { "BGM_STG4_JIN1", 0x80000000, "sound/stream/stg_dim1_32k", 0x55400000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_STG5_MUJ1", 0x80000000, "sound/stream/stg_mjn1_32k", 0x55400000 },
    { "BGM_STG5_DOU1", 0x80000000, "sound/stream/stg_duk1_32k", 0x5A400000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_STG6_RSH1", 0x80000000, "sound/stream/stg_rsh_a1_32k", 0x46400000 },
    { "BGM_STG6_RSH2", 0x80000000, "sound/stream/stg_rsh_b1_32k", 0x46400000 },
    { "BGM_STG6_RSH3", 0x80000000, "sound/stream/stg_rsh_c1_32k", 0x46400000 },
    { "BGM_STG6_HOM1", 0x80000000, "sound/stream/stg_hom1_32k", 0x46400000 },
    { "BGM_STG6_EKI1", 0x80000000, "sound/stream/stg_eki1_32k", 0x46400000 },
    { "BGM_STG6_PIK1", 0x80000000, "sound/stream/stg_pik1_32k", 0x46400000 },
    { "BGM_STG6_PIK2", 0x80000000, "sound/stream/stg_snd1_32k", 0x46400000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_STG7_BOM1", 0x80000000, "sound/stream/stg_bmm1_32k", 0x4B400000 },
    { "BGM_STG7_MON1", 0x80000000, "sound/stream/stg_gtm1_32k", 0x4B400000 },
    { "BGM_STG7_DUN1", 0x80000000, "sound/stream/stg_tkd1_32k", 0x55400000 },
    { "BGM_STG7_DUN2", 0x80000000, "sound/stream/stg_tkd2_32k", 0x49400000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_STG8_LST1", 0x80000000, "sound/stream/stg_ldn1_32k", 0x55500000 },
    { "BGM_STG8_LST2", 0x80000000, "sound/stream/stg_ldn2_32k", 0x5A500000 },
    { "BGM_STG8_LST3", 0x80000000, "sound/stream/stg_ldn3_32k", 0x5D500000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_STGK_CASTLE1", 0x80000000, "sound/stream/stg_kpa1_32k", 0x4B400000 },
    { "BGM_STGK_FIELD1", 0x80000000, "sound/stream/stgk_fld1_32k", 0x5F400000 },
    { "BGM_STGK_FIELD2", 0x80000000, "sound/stream/stg_kpa3_32k", 0x5A400000 },
    { "BGM_STGK_FIELD3", 0x80000000, "sound/stream/stgk_fld2_32k", 0x55400000 },
    { "BGM_STGK_FIELD4", 0x80000000, "sound/stream/stgk_fld3_32k", 0x5A400000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_STGK_FIELD1_UP1", 0x80000000, "sound/stream/stgk_fld1_up1_32k", 0x5F400000 },
    { "BGM_STGK_FIELD1_UP2", 0x80000000, "sound/stream/stgk_fld1_up2_32k", 0x5A400000 },
    { "BGM_STGK_FIELD3_UP1", 0x80000000, "sound/stream/stgk_fld2_up1_32k", 0x55400000 },
    { "BGM_STGK_FIELD4_UP1", 0x80000000, "sound/stream/stgk_fld3_up1_32k", 0x5A400000 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_ZAKO_BATTLE1", 0x80000000, "sound/stream/btl_zak1_32k", 0x4B400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_KOBOSS_BATTLE1", 0x80000000, "sound/stream/btl_kbs1_32k", 0x4B400001 },
    { "BGM_CHUBOSS_BATTLE1", 0x80000000, "sound/stream/btl_cbs1_32k", 0x4D400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_BOSS_STG1_GONBABA1", 0x80000000, "sound/stream/btl_gnb1_32k", 0x5A400001 },
    { "BGM_BOSS_STG3_3RDLEADER1", 0x80000000, "sound/stream/btl_mag1_32k", 0x51400001 },
    { "BGM_BOSS_STG3_3WOMEN1", 0x80000000, "sound/stream/btl_wom1_32k", 0x54400001 },
    { "BGM_BOSS_STG3_CHAMP1", 0x80000000, "sound/stream/btl_cap1_32k", 0x55400001 },
    { "BGM_BOSS_STG3_GANCE1", 0x80000000, "sound/stream/btl_gns1_32k", 0x5C400001 },
    { "BGM_BOSS_STG4_RUNPELL1", 0x80000000, "sound/stream/btl_rpl1_32k", 0x5A400001 },
    { "BGM_BOSS_STG4_FAKEMARIO1", 0x80000000, "sound/stream/btl_rpl1_32k", 0x5A400001 },
    { "BGM_BOSS_STG5_KORTESS1", 0x80000000, "sound/stream/btl_krt1_32k", 0x5A400001 },
    { "BGM_BOSS_STG5_SHIP1", 0x80000000, "sound/stream/btl_sif1_32k", 0x5A400001 },
    { "BGM_BOSS_STG6_GREAT_MOAMOA1", 0x80000000, "sound/stream/btl_gmm1_32k", 0x5D400001 },
    { "BGM_BOSS_STG8_3RDHEAD1", 0x80000000, "sound/stream/btl_3dh1_32k", 0x5A400001 },
    { "BGM_BOSS_STG8_WITCH", 0x80000000, "sound/stream/btl_wit1_32k", 0x5A400001 },
    { "BGM_BOSS_STG8_KOOPA", 0x80000000, "sound/stream/btl_kpa3_32k", 0x4B400001 },
    { "BGM_BOSS_STG8_LAST_BOSS1", 0x80000000, "sound/stream/btl_bpc1_32k", 0x5E400001 },
    { "BGM_BOSS_STG8_LAST_BOSS2", 0x80000000, "sound/stream/btl_wth3_32k", 0x5F400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_BATTLE_WIN1", 0x80000000, "sound/stream/btl_win1_32k", 0x50400001 },
    { "BGM_BATTLE_WIN2", 0x80000000, "sound/stream/btl_win2_32k", 0x50400001 },
    { "BGM_BATTLE_WIN3", 0x80000000, "sound/stream/btl_win3_32k", 0x50400001 },
    { "BGM_BATTLE_LOSE1", 0x80000000, "sound/stream/btl_die1_32k", 0x50400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_EVT_LECTURE1", 0x80000000, "sound/stream/evt_lec1_32k", 0x49400001 },
    { "BGM_EVT_PERAMAJIN1", 0x80000000, "sound/stream/evt_prm1_32k", 0x53400001 },
    { "BGM_EVT_NOK1", 0x80000000, "sound/stream/evt_nok1_32k", 0x64400000 },
    { "BGM_EVT_GONBABA_FLY1", 0x80000000, "sound/stream/evt_gnb1_32k", 0x69400001 },
    { "BGM_EVT_GONBABA_FLY2", 0x80000000, "sound/stream/evt_gnb4_32k", 0x5A400001 },
    { "BGM_EVT_CASTLE1", 0x80000000, "sound/stream/evt_gnb3_32k", 0x50400001 },
    { "BGM_EVT_QUIZ1", 0x80000000, "sound/stream/evt_qiz1_32k", 0x55400001 },
    { "BGM_EVT_QUIZ2", 0x80000000, "sound/stream/evt_qiz2_32k", 0x50400001 },
    { "BGM_EVT_DANGER1", 0x80000000, "sound/stream/evt_kik1_32k", 0x55400001 },
    { "BGM_EVT_DANGER2", 0x80000000, "sound/stream/evt_kik2_32k", 0x50400001 },
    { "BGM_EVT_DANGER3", 0x80000000, "sound/stream/evt_kik3_32k", 0x50400001 },
    { "BGM_EVT_DANGER4", 0x80000000, "sound/stream/evt_kik4_32k", 0x53400001 },
    { "BGM_EVT_HAPPY1", 0x80000000, "sound/stream/evt_hpy1_32k", 0x5A400001 },
    { "BGM_EVT_CYUCYURINA1", 0x80000000, "sound/stream/evt_ccr1_32k", 0x46400001 },
    { "BGM_EVT_NOKO_SAIKAI1", 0x80000000, "sound/stream/evt_nsk1_32k", 0x46400001 },
    { "BGM_EVT_USU1", 0x80000000, "sound/stream/evt_usg1_32k", 0x5A400001 },
    { "BGM_EVT_JIN1", 0x80000000, "sound/stream/evt_dim1_32k", 0x5A400001 },
    { "BGM_EVT_SAILING1", 0x80000000, "sound/stream/evt_kou1_32k", 0x4E400001 },
    { "BGM_EVT_MINIGAME1", 0x80000000, "sound/stream/btl_zak3_32k", 0x50400001 },
    { "BGM_EVT_FRANKURI1", 0x80000000, "sound/stream/evt_fkh1_32k", 0x50400001 },
    { "BGM_EVT_3RD_ENEMY", 0x80000000, "sound/stream/evt_3rd1_32k", 0x4D400001 },
    { "BGM_EVT_REST1", 0x80000000, "sound/stream/evt_rst1_32k", 0x5A400001 },
    { "BGM_EVT_POWAN1", 0x80000000, "sound/stream/evt_pwn1_32k", 0x46400001 },
    { "BGM_EVT_STG6_PIK1", 0x80000000, "sound/stream/evt_pik1_32k", 0x5A400000 },
    { "BGM_EVT_STG2_MORI1", 0x80000000, "sound/stream/evt_fsg1_32k", 0x5A400001 },
    { "BGM_EVT_STG2_TAIJU1", 0x80000000, "sound/stream/evt_okk1_32k", 0x5A400001 },
    { "BGM_EVT_WITCH1", 0x80000000, "sound/stream/evt_wit1_32k", 0x52400001 },
    { "BGM_EVT_CLOUDA1", 0x80000000, "sound/stream/evt_cld1_32k", 0x4B400001 },
    { "BGM_EVT_STG2_PUNI1", 0x80000000, "sound/stream/evt_pun1_32k", 0x46400001 },
    { "BGM_EVT_STG2_PUNITOGE1", 0x80000000, "sound/stream/evt_pvt1_32k", 0x50400001 },
    { "BGM_EVT_STG3_URON1", 0x80000000, "sound/stream/evt_urn1_32k", 0x5A400000 },
    { "BGM_EVT_STG3_TOUGI1", 0x80000000, "sound/stream/evt_tug1_32k", 0x5A400000 },
    { "BGM_EVT_STG3_EGG1", 0x80000000, "sound/stream/evt_egg1_32k", 0x4B400001 },
    { "BGM_EVT_STG3_GANCE1", 0x80000000, "sound/stream/evt_gns1_32k", 0x48400001 },
    { "BGM_EVT_STG3_GANCE2", 0x80000000, "sound/stream/evt_gns2_32k", 0x4B400001 },
    { "BGM_EVT_STG4_FAKEMARIO1", 0x80000000, "sound/stream/evt_fmo1_32k", 0x5A400001 },
    { "BGM_EVT_STG5_GHOST1", 0x80000000, "sound/stream/evt_elmos_app1_32k", 0x50400001 },
    { "BGM_EVT_STG5_KORTESS1", 0x80000000, "sound/stream/evt_kts1_32k", 0x54400001 },
    { "BGM_EVT_STG5_VS_3RD_ENEMY", 0x80000000, "sound/stream/evt_v3d1_32k", 0x5A400001 },
    { "BGM_EVT_STG7_HUGE_GUN1", 0x80000000, "sound/stream/evt_huge_gun1_32k", 0x62400001 },
    { "BGM_EVT_MAGNUM_APP1", 0x80000000, "sound/stream/evt_mag1_32k", 0x4D400001 },
    { "BGM_EVT_KUPPA1", 0x80000000, "sound/stream/evt_kpa1_32k", 0x50400000 },
    { "BGM_EVT_KUPPA2", 0x80000000, "sound/stream/evt_kpa1_32k", 0x50400001 },
    { "BGM_EVT_STG8_3RDHEAD1", 0x80000000, "sound/stream/evt_3dh1_32k", 0x50400001 },
    { "BGM_EVT_STG8_WITCH", 0x80000000, "sound/stream/evt_wit2_32k", 0x58400001 },
    { "BGM_EVT_COOKING1", 0x80000000, "sound/stream/evt_cok1_32k", 0x53400001 },
    { "BGM_EVT_STAGE_CLEAR1", 0x80000000, "sound/stream/evt_stc1_32k", 0x62400001 },
    { "BGM_EVT_ORGEL1", 0x80000000, "sound/stream/evt_org1_32k", 0x3E400001 },
    { "BGM_EVT_RUIJI1", 0x80000000, "sound/stream/evt_rui1_32k", 0x50400001 },
    { "BGM_EVT_PEACH_WALTZ1", 0x80000000, "sound/stream/evt_wlt1_32k", 0x4B400001 },
    { "BGM_EVT_MARIO_HOUSE1", 0x80000000, "sound/stream/evt_mri1_32k", 0x50400001 },
    { "BGM_EVT_MARIO_HOUSE2", 0x80000000, "sound/stream/evt_mri2_32k", 0x37400001 },
    { "BGM_EVT_PEACH1", 0x80000000, "sound/stream/evt_peh1_32k", 0x4D400001 },
    { "BGM_EVT_STG4_TRAIN1", 0x80000000, "sound/stream/evt_rit1_32k", 0x5A400001 },
    { "BGM_EVT_DOT_MARIO1", 0x80000000, "sound/stream/evt_dot1_32k", 0x46400001 },
    { "BGM_EVT_STG3_AIRSHIP1", 0x80000000, "sound/stream/evt_air1_32k", 0x55400001 },
    { "BGM_EVT_STG8_VOICE1", 0x80000000, "sound/stream/evt_vce1_32k", 0x55400001 },
    { "BGM_EVT_STG5_GHOST_SHIP1", 0x80000000, "sound/stream/evt_gstship1_32k", 0x55400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_FF_GET_STARSTONE1", 0x80000000, "sound/stream/ff_stg1_32k", 0x50400001 },
    { "BGM_FF_GET_KEYITEM1", 0x80000000, "sound/stream/ff_get2_32k", 0x50400001 },
    { "BGM_FF_GET_BADGE1", 0x80000000, "sound/stream/ff_get3_32k", 0x50400001 },
    { "BGM_FF_GET_ITEM1", 0x80000000, "sound/stream/ff_get4_32k", 0x50400001 },
    { "BGM_FF_GET_PARTY1", 0x80000000, "sound/stream/ff_get8_32k", 0x50400001 },
    { "BGM_FF_GET_STARPIECE1", 0x80000000, "sound/stream/ff_get6_32k", 0x50400001 },
    { "BGM_FF_GET_PERA1", 0x80000000, "sound/stream/ff_get7_32k", 0x50400001 },
    { "BGM_FF_GET_IMPORTANT_ITEM1", 0x80000000, "sound/stream/ff_get8_32k", 0x50400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_FF_STG_START1", 0x80000000, "sound/stream/ff_stt1_32k", 0x4B400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_FF_ENV_SURPRISED1", 0x80000000, "sound/stream/ff_spr1_32k", 0x55400001 },
    { "BGM_FF_ENV_SURPRISED2", 0x80000000, "sound/stream/ff_spr2_32k", 0x50400001 },
    { "BGM_FF_ENV_SURPRISED3", 0x80000000, "sound/stream/ff_spr3_32k", 0x50400001 },
    { "BGM_FF_ENV_ADMIRE1", 0x80000000, "sound/stream/ff_adm1_32k", 0x50400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_FF_ENV_SUCCESS1", 0x80000000, "sound/stream/ff_scs1_32k", 0x50400001 },
    { "BGM_FF_ENV_SUCCESS2", 0x80000000, "sound/stream/ff_scs1_32k", 0x50400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_FF_GO_SLEEPING1", 0x80000000, "sound/stream/ff_slp1_32k", 0x46400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_FF_ENV_CRISIS1", 0x80000000, "sound/stream/ff_crs1_32k", 0x50400001 },
    { "BGM_FF_ENV_TOU1", 0x80000000, "sound/stream/ff_tou1_32k", 0x57400001 },
    { "BGM_FF_ENV_TOU2", 0x80000000, "sound/stream/ff_tou2_32k", 0x52400001 },
    { "BGM_FF_ENV_TOU3", 0x80000000, "sound/stream/ff_tou3_32k", 0x5C400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_FF_ENV_RPL_APPEAR1", 0x80000000, "sound/stream/ff_r_apr2_32k", 0x54400001 },
    { "BGM_FF_ENV_SHIP_APPEAR1", 0x80000000, "sound/stream/ff_s_apr1_32k", 0x5A400001 },
    { "BGM_FF_ENV_WITCH_APPEAR1", 0x80000000, "sound/stream/ff_w_apr1_32k", 0x50400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_FF_ENV_BOKE1", 0x80000000, "sound/stream/ff_bok1_32k", 0x50400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_FF_ENV_MAP1", 0x80000000, "sound/stream/ff_map1_32k", 0x5A400001 },
    { "BGM_FF_ENV_OPEN_DOOR1", 0x80000000, "sound/stream/ff_dor1_32k", 0x5A400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_FF_KUPPA_GOAL1", 0x80000000, "sound/stream/ff_kpa1_32k", 0x5A400001 },
    { "BGM_FF_KUPPA_DOWN1", 0x80000000, "sound/stream/ff_kpa2_32k", 0x5A400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_FF_PEACH_DISCSET1", 0x80000000, "sound/stream/ff_disc_set1_32k", 0x50400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_FF_POWERUP1", 0x80000000, "sound/stream/ff_pow1_32k", 0x64400001 },
    { "BGM_FF_POWERDOWN1", 0x80000000, "sound/stream/ff_pow1_32k", 0x64400001 },
    { "", 0x00000000, "", 0x00000000 },
    { "BGM_FF_MAIL_RECEPTION1", 0x80000000, "sound/stream/ff_mail_chakusin1_32k", 0x46400001 },
    { "BGM_FF_MAIL_RECEPTION2", 0x80000000, "sound/stream/ff_mail_chakusin2_32k", 0x46400001 },
    { "BGM_FF_MAIL_RECEPTION3", 0x80000000, "sound/stream/ff_mail_chakusin3_32k", 0x46400001 },
};

//silent_tbl
__declspec(section ".sdata") s32 silent_tbl[10] = {
	10, 20, 30, 40, 50, 60, 70, 80, 90, 100
};

//psenvlist

SoundEffectEntry pssfxlist[4242] = {
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_CURSOR_MOVE1", 0x2000047C, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CURSOR_MOVE2", 0x2000047C, "", 85, 64, 0, 0, 0, 0, 0xAAA},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PRESS_START1", 0x2000026D, "", 90, 64, 0, 40, 0, 0, 0x0},
	{"SFX_FILE_CURSOR_MOVE1", 0x20000480, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_FILE_MOJI_SET1", 0x20000483, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_FILE_MOJI_DELETE1", 0x20000482, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_FILE_FONT_CHANGE1", 0x20000481, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FILE_CURSOR_MOVE2", 0x20000000, "SE1_CURSOR3", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_CURSOR_OK1", 0x2000047E, "SE1_SELECT1", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CURSOR_CANCEL1", 0x2000047B, "SE1_SELECT2", 75, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CURSOR_NOT_CHOICE1", 0x20000266, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_NORMAL_OPEN1", 0x2000048D, "", 55, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_NORMAL_CLOSE1", 0x2000048C, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_BOSS_OPEN1", 0x20000473, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_BOSS_CLOSE1", 0x20000472, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_WHISPER_OPEN1", 0x2000049A, "", 30, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_WHISPER_CLOSE1", 0x20000499, "", 30, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_SYSTEM_OPEN1", 0x20000496, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_SYSTEM_CLOSE1", 0x20000495, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_BROADCAST_OPEN1", 0x20000475, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_BROADCAST_CLOSE1", 0x20000474, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_SIGNBOARD_OPEN1", 0x2000048E, "", 65, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_SIGNBOARD_CLOSE1", 0x2000048F, "", 65, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_LOGBOOK_OPEN1", 0x20000487, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_LOGBOOK_CLOSE1", 0x20000486, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_TEC_OPEN1", 0x20000498, "", 65, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_TEC_CLOSE1", 0x20000497, "", 65, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_WITCH_OPEN1", 0x2000049C, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_WITCH_CLOSE1", 0x2000049B, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WINDOW_TELESA_OPEN1", 0x2000026E, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STATUS_WINDOW_OPEN1", 0x20000491, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STATUS_WINDOW_CLOSE1", 0x20000490, "SE1_STATUS_W_CLOSE1", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ITEM_WINDOW_OPEN1", 0x2000003F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_WINDOW_OPEN1", 0x2000003F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STATUS_GAUGE_OPEN1", 0x20000040, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STATUS_GAUGE_CLOSE1", 0x2000003E, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STATUS_WINDOW_CURSOL_MOVE1", 0x2000047C, "SE1_CLICK1", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STATUS_WINDOW_SLIDE1", 0x20000492, "", 55, 64, 0, 0, 0, 15, 0x0},
	{"SFX_STATUS_WINDOW_SLIDE2", 0x20000493, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STATUS_WINDOW_CURSOL_OK1", 0x2000047E, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STATUS_WINDOW_CURSOL_CANCEL1", 0x2000047B, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STATUS_WINDOW_TABI_MOVE1", 0x20000494, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STATUS_WINDOW_MAIL_MOVE1", 0x20000222, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STATUS_WINDOW_SSTONE_MOVE1", 0x2000026F, "", 90, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BADGE_PUT1", 0x20000470, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BADGE_REMOVE1", 0x20000471, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MESSAGE_WINDOW_OPEN1", 0x20000000, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MESSAGE_WINDOW_CLOSE1", 0x20000000, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MESSAGE_WAIT1", 0x20000267, "SE1_KEY_WAIT1", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MESSAGE_QUESTION1", 0x2000026A, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MESSAGE_SAVE_OK1", 0x2000026B, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MESSAGE_B1", 0x20000000, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MESSAGE_Z1", 0x2000026C, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MESSAGE_R1", 0x20000000, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MESSAGE1_1", 0x20000268, "", 45, 64, 0, 0, 50, 0, 0x0},
	{"SFX_MESSAGE1_2", 0x20000269, "", 45, 64, 0, 0, 50, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MESSAGE2_1", 0x20000000, "SE1_VOICE_NPC1", 0, 64, 0, 0, 1, 0, 0x0},
	{"SFX_MESSAGE2_2", 0x20000000, "SE1_VOICE_NPC2", 0, 64, 0, 0, 1, 0, 0x0},
	{"SFX_MESSAGE2_3", 0x20000000, "SE1_VOICE_NPC3", 0, 64, 0, 0, 1, 0, 0x0},
	{"SFX_MESSAGE2_4", 0x20000000, "SE1_VOICE_NPC4", 0, 64, 0, 0, 1, 0, 0x0},
	{"SFX_MESSAGE2_5", 0x20000000, "SE1_VOICE_NPC5", 0, 64, 0, 0, 1, 0, 0x0},
	{"SFX_MESSAGE2_6", 0x20000000, "SE1_VOICE_NPC6", 0, 64, 0, 0, 1, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MESSAGE3_1", 0x20000000, "SE1_VOICE_CHURINA1", 0, 64, 0, 0, 1, 0, 0x0},
	{"SFX_MESSAGE3_2", 0x20000000, "SE1_VOICE_CHURINA2", 0, 64, 0, 0, 1, 0, 0x0},
	{"SFX_MESSAGE3_3", 0x20000000, "SE1_VOICE_CHURINA3", 0, 64, 0, 0, 1, 0, 0x0},
	{"SFX_MESSAGE3_4", 0x20000000, "SE1_VOICE_CHURINA4", 0, 64, 0, 0, 1, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MESSAGE4_1", 0x20000000, "SE1_DUMMY", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MESSAGE5_1", 0x20000000, "SE1_VOICE_ENEMY_HEAVY1", 0, 64, 0, 0, 1, 0, 0x0},
	{"SFX_MESSAGE5_2", 0x20000000, "SE1_VOICE_ENEMY_HEAVY2", 0, 64, 0, 0, 1, 0, 0x0},
	{"SFX_MESSAGE5_3", 0x20000000, "SE1_VOICE_ENEMY_HEAVY3", 0, 64, 0, 0, 1, 0, 0x0},
	{"SFX_MESSAGE5_4", 0x20000000, "SE1_VOICE_ENEMY_HEAVY2", 0, 64, 0, 0, 1, 0, 0x0},
	{"SFX_MESSAGE5_5", 0x20000000, "SE1_VOICE_ENEMY_HEAVY5", 0, 64, 0, 0, 1, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_COUNT_COIN1", 0x2000047A, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BIKKURI_MARK1", 0x20000265, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_QUESTION_MARK1", 0x2000003C, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_GURUGURU_MARK1", 0x20000131, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_TENTEN_MARK1", 0x20000000, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_WIPE_TO_PEACH1", 0x20000000, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WIPE_TO_KOOPA1", 0x20000000, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_WIPE_TO_MARIO1", 0x20000000, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACTION1", 0x20000000, "SE1_VOICE_M_ACTION1", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACTION2", 0x20000000, "SE1_VOICE_M_ACTION2", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACTION3", 0x20000000, "SE1_VOICE_M_ACTION3", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACTION4", 0x20000000, "SE1_VOICE_M_ACTION4", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACTION5", 0x20000000, "SE1_VOICE_M_ACTION5", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACTION6", 0x20000000, "SE1_VOICE_M_ACTION6", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACTION7", 0x20000000, "SE1_VOICE_M_ACTION7", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACTION8", 0x20000000, "SE1_VOICE_M_ACTION8", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACTION9", 0x20000000, "SE1_VOICE_M_ACTION9", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACTION10", 0x20000000, "SE1_VOICE_M_ACTION10", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACTION11", 0x20000000, "SE1_VOICE_M_ACTION11", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACTION12", 0x20000000, "SE1_VOICE_M_ACTION12", 127, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACROBAT1", 0x20000000, "SE1_VOICE_M_ACROBAT1", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACROBAT2", 0x20000000, "SE1_VOICE_M_ACROBAT2", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACROBAT3", 0x20000000, "SE1_VOICE_M_ACROBAT3", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACROBAT4", 0x20000000, "SE1_VOICE_M_ACROBAT4", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACROBAT5", 0x20000000, "SE1_VOICE_M_ACROBAT5", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACROBAT6", 0x20000000, "SE1_VOICE_M_ACROBAT6", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACROBAT7", 0x20000000, "SE1_VOICE_M_ACROBAT7", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACROBAT8", 0x20000000, "SE1_VOICE_M_ACROBAT8", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACROBAT9", 0x20000000, "SE1_VOICE_M_ACROBAT9", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACROBAT10", 0x20000000, "SE1_VOICE_M_ACROBAT10", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACROBAT11", 0x20000000, "SE1_VOICE_M_ACROBAT11", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACROBAT12", 0x20000000, "SE1_VOICE_M_ACROBAT12", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_ACROBAT13", 0x20000000, "SE1_VOICE_M_ACROBAT13", 127, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_REST1", 0x20000000, "SE1_VOICE_M_REST1", 70, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_DAMAGE_NORMAL1", 0x20000000, "SE1_VOICE_M_DAMAGE_NORMAL1", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_DAMAGE_NORMAL2", 0x20000000, "SE1_VOICE_M_DAMAGE_NORMAL2", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_DAMAGE_NORMAL3", 0x20000000, "SE1_VOICE_M_DAMAGE_NORMAL3", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_DAMAGE_BIG1", 0x20000000, "SE1_VOICE_M_DAMAGE_BIG1", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_DAMAGE_BIG2", 0x20000000, "SE1_VOICE_M_DAMAGE_BIG2", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_DAMAGE_BIG3", 0x20000000, "SE1_VOICE_M_DAMAGE_BIG3", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_DAMAGE_BIG4", 0x20000000, "SE1_VOICE_M_DAMAGE_BIG4", 127, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_DAMAGE_DOWN1", 0x20000000, "SE1_VOICE_M_DAMAGE_DOWN1", 115, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_GAJIGAJI1", 0x20000000, "SE1_VOICE_M_GAJIGAJI1", 127, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_KUPPA_ANGRY1", 0x20000000, "SE1_VOICE_K_ANGRY1", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_KUPPA_CALL_MARIO1", 0x20000000, "SE1_VOICE_K_CALL_MARIO1", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_KUPPA_CALL_MARIO2", 0x20000000, "SE1_VOICE_K_CALL_MARIO2", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_KUPPA_FALLIN1", 0x20000000, "SE1_VOICE_K_FALLIN1", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_KUPPA_LAUGH1", 0x20000000, "SE1_VOICE_K_LAUGH1", 127, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_CALL_MARIO1", 0x20000000, "SE1_VOICE_P_CALL_MARIO1", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_CALL_MARIO2", 0x20000000, "SE1_VOICE_P_CALL_MARIO2", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_CALL_MARIO3", 0x20000000, "SE1_VOICE_P_CALL_MARIO3", 127, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_FLD_JUMP1", 0x2000028F, "SE2_VOICE_M_FLD_JUMP1", 107, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_FLD_JUMP2", 0x20000290, "SE2_VOICE_M_FLD_JUMP2", 107, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_FLD_JUMP3", 0x200002A6, "", 107, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_FLD_JUMP4", 0x200002A7, "", 107, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_FLD_JUMP5", 0x20000292, "SE2_VOICE_M_FLD_JUMP5", 107, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_BATTLE_JUMP1", 0x200002AF, "", 117, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC1_1", 0x2000028E, "", 119, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC1_2", 0x2000028F, "", 112, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC1_3", 0x20000290, "", 122, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC1_4", 0x20000291, "", 122, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC1_5", 0x20000292, "", 122, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC2_1", 0x20000293, "", 122, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC2_2", 0x20000294, "", 122, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC2_3", 0x20000295, "", 117, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC2_4", 0x20000296, "", 112, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC2_5", 0x20000297, "", 117, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC3_1", 0x20000298, "", 117, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC3_2", 0x20000299, "", 117, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC3_3", 0x2000029A, "", 117, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC3_4", 0x2000029B, "", 117, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AC3_5", 0x2000029C, "", 119, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_AWAKE1", 0x2000029D, "", 72, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_DAMAGED1", 0x2000029E, "", 117, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_DAMAGED2", 0x2000029F, "", 97, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_DOWN1", 0x200002A0, "", 97, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_FIND1", 0x200002A1, "", 72, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_FIND1_2", 0x200002A2, "", 82, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_FIND1_3", 0x200002A3, "", 77, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_HAND_UP1_1", 0x200002A9, "", 87, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_HAND_UP1_2", 0x200002AA, "", 87, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_HAND_UP1_3", 0x200002AB, "", 82, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_HAND_UP1_4", 0x200002AC, "", 72, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_HAPPY1", 0x200002AD, "", 97, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_HAPPY2", 0x200002AE, "", 107, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_LAND_DAMAGE1", 0x200002B0, "", 97, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_LAND_DAMAGE2", 0x200002B1, "", 102, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_NO1", 0x200002B2, "", 77, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_NO2", 0x200002B3, "", 77, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_NOD1", 0x200002B4, "", 67, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_NOD1_2", 0x200002B5, "", 82, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_NOD1_3", 0x200002B6, "", 72, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_NOD1_4", 0x200002B7, "", 77, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_NOD1_5", 0x200002B8, "", 77, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_NOD1_6", 0x200002B9, "", 67, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_PAUSE1", 0x200002BA, "", 109, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_PAUSE2", 0x200002BB, "", 119, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_PAUSE3", 0x200002BC, "", 109, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_POWER1", 0x200002BD, "", 87, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_POWER2", 0x200002BE, "", 77, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_QUESTION1", 0x200002BF, "", 72, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_QUESTION1_2", 0x200002C0, "", 67, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_RELIEF1", 0x200002C1, "", 77, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_RELIEF2", 0x200002C2, "", 82, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_SLEEP1", 0x200002C3, "", 69, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_SURPRISED1", 0x200002C4, "", 82, 64, 1, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_SURPRISED1_2", 0x200002C5, "", 92, 64, 1, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_SURPRISED2", 0x200002C6, "", 87, 64, 1, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_SURPRISED2_2", 0x200002C7, "", 92, 64, 1, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_SURPRISED2_3", 0x200002C8, "", 82, 64, 1, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_THINK1", 0x200002C9, "", 72, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_THINK1_2", 0x200002CA, "", 67, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_MARIO_YAWN1", 0x200002CB, "", 87, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_HAMMING1", 0x200002D8, "", 95, 64, 0, 50, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_HAMMING2", 0x200002D9, "", 90, 64, 0, 50, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_HAMMING3", 0x200002DA, "", 90, 64, 0, 50, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_HAPPY1_1", 0x200002DB, "", 95, 64, 0, 15, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_HAPPY1_2", 0x200002DC, "", 95, 64, 0, 15, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_HAPPY2_1", 0x200002DD, "", 95, 64, 0, 15, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_HAPPY2_2", 0x200002DE, "", 90, 64, 0, 15, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_LAUGH1_1", 0x200002DF, "", 95, 64, 0, 15, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_LAUGH1_2", 0x200002E0, "", 95, 64, 0, 15, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_LAUGH2_1", 0x200002E1, "", 85, 64, 0, 15, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_LAUGH2_2", 0x200002E2, "", 85, 64, 0, 15, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_QUESTION1", 0x200002E3, "", 75, 64, 0, 15, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_SAD1_1", 0x200002E4, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_SAD1_2", 0x200002E5, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_SAD2_1", 0x200002E6, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_SAD2_2", 0x200002E7, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_SURPRISED1_1", 0x200002E8, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_SURPRISED1_2", 0x200002E9, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_SURPRISED1_3", 0x200002EA, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_SURPRISED2_1", 0x200002EB, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_VOICE_PEACH_SURPRISED2_2", 0x200002EC, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_ANGRY1_1", 0x20000270, "", 92, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_ANGRY1_2", 0x20000271, "", 92, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_ANGRY2_1", 0x20000272, "", 87, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_ANGRY2_2", 0x20000273, "", 87, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_HAPPY1_1", 0x20000275, "", 87, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_HAPPY1_2", 0x20000276, "", 87, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_HAPPY2_1", 0x20000277, "", 77, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_HAPPY2_2", 0x20000278, "", 82, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_LAUGH1_1", 0x20000279, "", 87, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_LAUGH1_2", 0x2000027A, "", 87, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_LAUGH2_1", 0x2000027B, "", 87, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_LAUGH2_2", 0x2000027C, "", 87, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_QUESTION1", 0x2000027D, "", 77, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_SATIATED1_1", 0x2000027E, "", 82, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_SATIATED1_2", 0x2000027F, "", 72, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_SATIATED2_1", 0x20000280, "", 77, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_SATIATED2_2", 0x20000281, "", 77, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_SURPRISED1_1", 0x20000282, "", 82, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_SURPRISED1_2", 0x20000283, "", 77, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_SURPRISED2_1", 0x20000284, "", 82, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_SURPRISED2_2", 0x20000285, "", 77, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_THINK1_1", 0x20000286, "", 77, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_THINK1_2", 0x20000287, "", 72, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_THINK2_1", 0x20000288, "", 77, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_THINK2_2", 0x20000289, "", 82, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_VEXATIOUS1_1", 0x2000028A, "", 92, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_VEXATIOUS1_2", 0x2000028B, "", 87, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_VEXATIOUS2_1", 0x2000028C, "", 87, 64, 0, 20, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_VEXATIOUS2_2", 0x2000028D, "", 82, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_KOOPA_FIRE1", 0x20000274, "", 92, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_VOICE_NM_HAPPY1", 0x200002D2, "", 107, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_NM_HAPPY2", 0x200002D1, "", 127, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_NM_LAND_DAMAGE2", 0x200002D3, "", 112, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_NM_SURPRISED1", 0x200002D4, "", 92, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_NM_SURPRISED1_2", 0x200002D5, "", 102, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_NM_SURPRISED2", 0x200002D6, "", 97, 64, 0, 1, 0, 0, 0x0},
	{"SFX_VOICE_NM_SURPRISED2_3", 0x200002D7, "", 92, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_MOVE1L", 0x20000399, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_MOVE1R", 0x2000039A, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_MOVE2L", 0x2000039D, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_MOVE2R", 0x2000039E, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_MOVE3L", 0x2000039F, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_MOVE3R", 0x200003A0, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_MOVE4L", 0x2000039B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_MOVE4R", 0x2000039C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_MOVE5L", 0x20000395, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_MOVE5R", 0x20000396, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_MOVE6L", 0x2000022B, "SE4_FLD_M_WALK_METAL1", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_MOVE6R", 0x2000022C, "SE4_FLD_M_WALK_METAL2", 70, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_JUMP1", 0x2000021E, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_LANDING1", 0x20000387, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_LANDING2", 0x20000389, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_LANDING3", 0x2000038A, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_LANDING4", 0x20000388, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_LANDING5", 0x20000385, "", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_LANDING6", 0x20000221, "SE4_FLD_M_LANDING_METAL1", 70, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HEAD1", 0x20000034, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HEAD1_BLOCK1", 0x20000034, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HIP_DROP1", 0x2000037D, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HIO_DROP2", 0x2000037E, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_FURU1", 0x20000382, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_FURU2", 0x20000383, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_FURU3", 0x20000384, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_DON1", 0x2000037F, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_DON2", 0x20000380, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_DON3", 0x20000381, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_WOOD_DON1", 0x200002ED, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_KAITEN1", 0x2000021F, "SE4_FLD_M_HMMR_KAITEN1", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_KAITEN2", 0x20000220, "SE4_FLD_M_HMMR_KAITEN2", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_PIKKYO_R1", 0x20000259, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_PIKKYO_G1", 0x20000256, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_PIKKYO_B1", 0x20000257, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_PIKKYO_Y1", 0x2000025A, "", 105, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_PIKKYO_P1", 0x20000258, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_MISS_BLOCK1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_STAMP1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_FALL1", 0x20000000, "SE4_FLD_M_FALL1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_FALL2", 0x20000000, "SE4_FLD_M_FALL2", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_FALL3", 0x20000000, "SE4_FLD_M_FALL3", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_FURAFURA1", 0x20000000, "SE4_FLD_M_FURAFURA1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_DOWN1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_DOKAN1", 0x200003A1, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_CHANGE_SLIT1", 0x2000038B, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_CHANGE_SLIT2", 0x2000038E, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_CHANGE_ROLL1", 0x2000038F, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_CHANGE_ROLL2", 0x20000390, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_ROLL_MOVE1", 0x20000391, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_ROLL_JUMP1", 0x2000021E, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_AIRPLANE_JUMP1", 0x20000226, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_AIRPLANE1", 0x20000224, "SE4_FLD_M_PAPER_ACT6", 100, 64, 0, 1, 0, 10, 0x0},
	{"SFX_MARIO_AIRPLANE2", 0x20000393, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_AIRPLANE3", 0x20000227, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_BELLOWS1", 0x20000000, "SE4_M_PAPER_ACT81", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_BELLOWS1_1", 0x200003C8, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_BELLOWS1_2", 0x200003C9, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_BELLOWS1_3", 0x200003CA, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_BELLOWS2", 0x20000394, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_BELLOWS_CATCH1", 0x2000021C, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_BELLOWS_MOVE1", 0x2000021D, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_BELLOWS3", 0x20000316, "SE4_FLD_M_PAPER_ACT10", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_BELLOWS4", 0x2000038D, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_BELLOWS5", 0x200003C5, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_BELLOWS6", 0x200003C6, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_BELLOWS7", 0x200003C7, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_SHIP_JUMP1", 0x20000228, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_SHIP1_1", 0x20000224, "SE2_FLD_M_PAPER_ACT6", 100, 64, 0, 1, 0, 15, 0x0},
	{"SFX_MARIO_SHIP1_2", 0x20000389, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_SHIP1", 0x2000022A, "SE4_FLD_M_PAPER_ACT13", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_SHIP2", 0x20000225, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_SHIP3", 0x20000229, "", 100, 64, 0, 1, 0, 55, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_FALL_WATER1", 0x2000021B, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_LEAP_WATER1", 0x2000037B, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_GAJIGAJI1", 0x2000037C, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_FALL_CRUSH1", 0x2000021A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_EAT1", 0x20000000, "SE2_FLD_M_EAT1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_SET_KEY1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_BIG_JUMP1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_BREAK_BOARD1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_BREAK_BOX1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_POWER_UP1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_POWER_DOWN1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_LIFT1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_PUT1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_THROW1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_EVT_ACTION1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_SPIT1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_OPEN_LETTER1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MARIO_KISS1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_SAVE1", 0x2000026A, "SE4_OBJ_BLOCK_SAVE1", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_S_FLOAT_MOVE1", 0x200003CF, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_S_FLOAT_DOWN1", 0x200003D0, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_B_FLOAT_MOVE1", 0x200003CD, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_B_FLOAT_DOWN1", 0x200003CE, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_B_FLOAT_STOP1", 0x2000025B, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_SWITCH1", 0x200003CC, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_UNBREAK1", 0x200003D8, "", 100, 60, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_BREAK1", 0x200003D6, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_LEVEL_BLOCK_BREAK1", 0x20000000, "SE2_WOOD_BREAK1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_QUESTION1", 0x200003D3, "", 100, 60, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_BADGE1", 0x200003CB, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_ITEM_APPEAR1", 0x200003E8, "", 100, 60, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_TEN_COUNT_HIT1", 0x2000047A, "SE4_OBJ_BLOCK_TEN_HIT1", 60, 60, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_TEN_COUNT_COIN1", 0x2000047A, "SE4_OBJ_BLOCK_TEN_COIN1", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_POWER_SHINE1", 0x200003D2, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_TIMER_SWITCH_HIT1", 0x200003F5, "", 100, 60, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_TIMER_SWITCH_LIGHT1", 0x200003F6, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_OJAMA1", 0x200003D9, "", 100, 60, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_OJAMA2", 0x200003D1, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_OJAMA_SMALL1", 0x20000255, "", 110, 60, 0, 1, 0, 0, 0xAAA},
	{"SFX_MOBJ_BLOCK_OJAMA_MID1", 0x20000255, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_BLOCK_OJAMA_BIG1", 0x20000254, "", 120, 68, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_WALL_BREAK1", 0x20000000, "SE2_ROCK_BREAK1", 0, 60, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_FIREBAR_ROUND1", 0x200003E1, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_DOKAN_APPEAR1", 0x200003DF, "", 100, 64, 0, 1, 0, 0, 0xF800},
	{"SFX_DOKAN_GOOUT1", 0x200003E0, "", 100, 64, 0, 1, 0, 0, 0xF800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_SWITCH_ON1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_BIKKURI_SWITCH_ON1", 0x200003EF, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_RIDE_SWITCH_ON1", 0x200003F0, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_RIDE_SWITCH_ON2", 0x200003F1, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_RIDE_SWITCH_ON3", 0x200003F2, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_RIDE_SWITCH_ON4", 0x200003F3, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_RIDE_SWITCH_ON5", 0x200003F4, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_HP_RECOVER_HIT1", 0x200003D5, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_HP_RECOVER_HEART1", 0x200003D4, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_HP_RECOVER_SHINE1", 0x20000158, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_HP_RETURN1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_FP_RECOVER1", 0x20000000, "SE2_BTL_S_RECOVER_SHINE1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_TREASURE_BOX_SWING1", 0x200003DD, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_TREASURE_BOX_OPEN1", 0x200003DC, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_TREASURE_BOX_SWING2", 0x200003DB, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_TREASURE_BOX_OPEN2", 0x200003DA, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_LOCK_SWING1", 0x200003ED, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_UNLOCK1", 0x200003EC, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_UNLOCK1_LEAPED1", 0x20000041, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_BREAK_FLOOR1", 0x200003E2, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_BREAK_WOODBOX1", 0x20000000, "SE4_OBJ_BOX_WOOD_BREAK1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_JUMP_STAND1", 0x200003EA, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_JUMP_STAND2", 0x200003EB, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_KURURIN_FLOOR_HOP1", 0x200003E5, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_KURURIN_FLOOR_LEAP1", 0x200003E4, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_KURURIN_FLOOR_CLOSE1", 0x200003E3, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_FLOOR_TRAP1", 0x200003E6, "", 100, 64, 0, 50, 0, 0, 0x0},
	{"SFX_MOBJ_FLOOR_TRAP2", 0x200003E7, "", 100, 64, 0, 50, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_POW_BLOCK1", 0x2000003B, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MOBJ_BLACKBOX_SWING1", 0x20000253, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_BLACKBOX_OPEN1", 0x200003DA, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_PERAMAJIN_OUT1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MOBJ_PERAMAJIN_OUT2", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_WALL_BREAK1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ITEM_MARIO_USE1", 0x20000425, "btl_e_item_use1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ITEM_PARTY_USE1", 0x20000425, "btl_e_item_use1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ITEM_RECOVERY_SHINE1", 0x200003D4, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_COIN_GET1", 0x2000047A, "SE4_OBJ_BLOCK_TEN_COIN1", 60, 64, 0, 10, 0, 0, 0x0},
	{"SFX_COIN_LEAPED1", 0x20000247, "", 90, 64, 0, 10, 0, 0, 0x0},
	{"SFX_HEART_GET1", 0x20000308, "", 90, 64, 0, 10, 0, 0, 0x0},
	{"SFX_HEART_LEAPED1", 0x200003E9, "", 50, 64, 0, 10, 0, 0, 0x555},
	{"SFX_FLOWER_GET1", 0x20000304, "", 90, 64, 0, 10, 0, 0, 0x0},
	{"SFX_FLOWER_LEAPED1", 0x200003E9, "", 50, 64, 0, 10, 0, 0, 0x555},
	{"SFX_STAR_PIECE_LEAPED1", 0x2000003D, "", 100, 64, 0, 10, 0, 0, 0x800},
	{"SFX_ITEM_LEAPED1", 0x200003E9, "", 90, 64, 0, 10, 0, 0, 0x0},
	{"SFX_BADGE_LEAPED1", 0x200003E9, "SE1_OTOSITA1", 90, 64, 0, 10, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ITEM_HORROR1", 0x20000035, "", 115, 64, 0, 1, 0, 0, 0x0},
	{"SFX_ITEM_HORROR2", 0x20000036, "", 115, 64, 0, 1, 0, 0, 0x0},
	{"SFX_ITEM_GURUGURU1", 0x20000037, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_ITEM_FIRE_FLOWER1", 0x20000248, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_ITEM_ICE_WIND1", 0x2000024A, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_ITEM_KIRAKIRA_FALL1", 0x2000024B, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_ITEM_ICE_THUNDER_FALL1", 0x20000250, "", 110, 64, 0, 20, 0, 0, 0x0},
	{"SFX_ITEM_QUAKE1", 0x200001F1, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_ITEM_SLEEP1", 0x2000024D, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_ITEM_SLEEP2", 0x2000024E, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_ITEM_POW_APPEAR1", 0x2000019F, "", 120, 64, 0, 1, 0, 0, 0x555},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ITEM_WHAT1", 0x2000019F, "", 120, 64, 0, 1, 0, 0, 0x555},
	{"SFX_ITEM_WHAT2", 0x40045A5A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_ITEM_WHAT3", 0x2000041A, "btl_e_fall1_22k", 100, 64, 0, 1, 0, 0, 0xFAAB},
	{"SFX_ITEM_WHAT4", 0x200003CC, "", 100, 64, 0, 1, 0, 0, 0x800},
	{"SFX_ITEM_WHAT5", 0x20000224, "", 100, 64, 0, 1, 0, 0, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ITEM_RANK_UP_ENEMY1", 0x2000024C, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_ITEM_STOP_WATCH1", 0x2000024F, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_ITEM_HP_FP_CHANGE1", 0x2000046F, "", 90, 64, 0, 1, 0, 40, 0x0},
	{"SFX_ITEM_HP_FP_CHANGE2", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ITEM_COOKING_THROW1", 0x200000AD, "", 70, 64, 0, 10, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ITEM_COOK_LAST1", 0x20000114, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_ITEM_COOK_LAST2", 0x200001EF, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_ITEM_COOK_LAST3", 0x20000250, "", 100, 64, 0, 60, 0, 0, 0xF800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_GRASS_SHAKE1", 0x200003B6, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_GRASS_SHAKE2", 0x20000000, "SE1_GRASS_SHAKE1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_HIT_TREE1", 0x200003B8, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_HIT_TREE1_LEAVES1", 0x200003B7, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_DOOR_OPEN1", 0x200003A3, "", 105, 64, 0, 5, 0, 0, 0xFD55},
	{"SFX_DOOR_OPEN_WOOD1", 0x200003A6, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_OPEN_IRON2", 0x200003A7, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_OPEN_WOOD2", 0x200003AC, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_OPEN_GRILLE1", 0x200003A5, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_OPEN_SLIDE1", 0x20000230, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_OPEN_SLIDE1_2", 0x20000230, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_OPEN_SLIDE1_3", 0x20000230, "", 100, 64, 0, 5, 0, 0, 0xFAAB},
	{"SFX_DOOR_OPEN_TURN1", 0x20000000, "SE4_FLD_O_DOOR_OPEN_TURN1", 0, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_OPEN_ELEC1", 0x200003A4, "", 110, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_OPEN_STONE1", 0x200003AB, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_OPEN_WOOD3", 0x200003AD, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_OPEN_CASTLE1", 0x2000022D, "", 80, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_OPEN_CASTLE2", 0x2000022E, "", 80, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_OPEN_METAL1", 0x2000022F, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_FENCE_OPEN1", 0x2000023B, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_OPEN_ELEC2_1", 0x20000238, "", 80, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_OPEN_ELEC2_2", 0x20000239, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_DOOR_FALL1", 0x200003A2, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_DOOR_SHUT1", 0x200003AE, "", 100, 64, 0, 5, 0, 12, 0x0},
	{"SFX_DOOR_SHUT_IRON1", 0x200003B1, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_SHUT_IRON2", 0x200003B2, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_SHUT_GRILLE1", 0x200003B0, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_SHUT_SLIDE1", 0x20000235, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_SHUT_SLIDE1_2", 0x20000235, "", 100, 64, 0, 5, 0, 10, 0x0},
	{"SFX_DOOR_SHUT_SLIDE1_3", 0x20000235, "", 100, 64, 0, 5, 0, 0, 0xFAAB},
	{"SFX_DOOR_SHUT_ELEC1", 0x200003AF, "", 110, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_SHUT_WOOD1", 0x20000237, "SE4_FLD_O_DOOR_SHUT_STONE1", 80, 64, 0, 5, 0, 10, 0x0},
	{"SFX_DOOR_SHUT_WOOD2", 0x200003B5, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_SHUT_CASTLE1", 0x20000232, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_SHUT_CASTLE2", 0x20000233, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_SHUT_METAL1", 0x20000234, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_SHUT_WOOD3", 0x20000237, "", 90, 64, 0, 5, 0, 0, 0x0},
	{"SFX_DOOR_SHUT_ELEC2", 0x2000023A, "", 100, 64, 0, 5, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_SLIDE_DOOR1_UP", 0x200003A9, "", 100, 64, 0, 10, 0, 0, 0x0},
	{"SFX_SLIDE_DOOR1_DOWN", 0x200003AA, "", 100, 64, 0, 10, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_GLASS_DOOR1_OPEN1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_GLASS_DOOR1_CLOSE1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_DOOR_DOWN1", 0x200003A2, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_HOUSE_DOWN1", 0x2000023C, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_HOUSE_OPEN1", 0x2000023D, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_HOUSE_SHUT1", 0x2000023E, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_WALL_MOVE1", 0x20000492, "", 100, 64, 0, 1, 0, 0, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_WALK_BRIDGE1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STONE_MOVE1", 0x20000264, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MAIL_RECEPTION1", 0x20000222, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_MAIL_RECEPTION2", 0x20000223, "", 110, 64, 0, 1, 0, 5, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_STARSTONE_SHINE1", 0x20000172, "", 55, 64, 0, 10, 0, 0, 0x0},
	{"SFX_EVT_STARSTONE_JUMP1", 0x2000003D, "SE2_EVT_1_STARSTONE_JUMP1", 100, 64, 0, 10, 0, 0, 0x0},
	{"SFX_EVT_STARSTONE_MOVE1", 0x20000000, "SE2_EVT_1_STARSTONE_MOVE1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_SE2_EVT_STARSTONE_GET1", 0x40043434, "", 85, 64, 0, 1, 0, 3, 0x0},
	{"SFX_EVT_STARSTONE_WHITEOUT1", 0x20000218, "", 60, 64, 0, 70, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_WITCH_SHADOW_SPREAD1", 0x20000244, "", 90, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_MAJO_SHADOW1", 0x20000242, "SE4_BOS_MAJO_SHADOW1", 85, 64, 0, 1, 0, 0, 0x555},
	{"SFX_EVT_MAJO_SHADOW2", 0x200002F1, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_MARI_SHADOW1", 0x20000242, "SE4_BOS_MARI_SHADOW1", 85, 64, 0, 1, 0, 0, 0xFAAB},
	{"SFX_EVT_MARI_SHADOW2", 0x200002F3, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_VIVI_JUMP1", 0x200003BA, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_MAJO_JUMP1", 0x200002F1, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_MARI_JUMP1", 0x200002F3, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_NPC_BIKKURI_MARK1", 0x20000265, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_NPC_QUESTION_MARK1", 0x2000003C, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_MAP_KAKAGERU1", 0x40041B1B, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_MAP_KAKAGERU1_2", 0x40044141, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_MAP_NEXT_APPEAR1", 0x20000209, "", 80, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_MAP_STG1_APPEAR1", 0x40045B5B, "", 90, 64, 0, 1, 0, 2, 0x0},
	{"SFX_EVT_MAP_STG2_APPEAR1", 0x40045C5C, "", 90, 64, 0, 1, 0, 3, 0x0},
	{"SFX_EVT_MAP_STG3_APPEAR1", 0x40045D5D, "", 90, 64, 0, 1, 0, 2, 0x0},
	{"SFX_EVT_MAP_STG4_APPEAR1", 0x40045E5E, "", 90, 64, 0, 1, 0, 7, 0x0},
	{"SFX_EVT_MAP_STG5_APPEAR1", 0x40045F5F, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_MAP_STG6_APPEAR1", 0x40046060, "", 90, 64, 0, 1, 0, 6, 0x0},
	{"SFX_EVT_MAP_STG7_APPEAR1", 0x40046161, "", 90, 64, 0, 1, 0, 2, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_MAP_NEXT_APPEAR2", 0x20000000, "SE2_EVT_MAP_NEXT_APPEAR2", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_MAP_NEXT_APPEAR2_2", 0x20000000, "SE2_EVT_MAP_NEXT_APPEAR2_2", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_MAP_NEXT_APPEAR2_3", 0x20000000, "SE2_EVT_MAP_NEXT_APPEAR2_3", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_MAP_KAKAGERU2", 0x40043535, "", 95, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_MAP_KAKAGERU2_2", 0x40043636, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_MAP_KAKAGERU2_3", 0x40044141, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_MAP_KAKAGERU3_1", 0x40044242, "", 95, 64, 0, 1, 0, 15, 0x0},
	{"SFX_EVT_MAP_KAKAGERU3_2", 0x40044343, "", 100, 64, 0, 1, 0, 15, 0x0},
	{"SFX_EVT_MAP_KAKAGERU3_3", 0x40044444, "", 85, 64, 0, 1, 0, 10, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_STAGE_CLEAR1", 0x20000150, "", 50, 64, 0, 0, 0, 6, 0x555},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_PERAMAJIN_LIGHT1", 0x40043737, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_PERAMAJIN_BLINKING1", 0x20000249, "", 50, 64, 0, 1, 0, 0, 0xAAA},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_LECTUREN_BLINKING1", 0x20000249, "", 50, 64, 0, 1, 0, 0, 0xAAA},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_LECTURE_BLOCK_FALL1", 0x200000AD, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_LECTURE_BLOCK_DOWN1", 0x200003BC, "", 100, 64, 0, 1, 0, 0, 0xF800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_PARTY_MOVE1L", 0x200003BF, "", 45, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_PARTY_MOVE1R", 0x200003C0, "", 45, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_PARTY_JUMP1", 0x200003BA, "", 40, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_PARTY_LANDING1", 0x200003BB, "", 40, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_PARTY_HANG1", 0x200003BE, "", 30, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_LUIGI_MOVE1L", 0x20000399, "", 65, 64, 0, 1, 0, 0, 0xFD55},
	{"SFX_FLD_NPC_LUIGI_MOVE1R", 0x2000039A, "", 65, 64, 0, 1, 0, 0, 0xFD55},
	{"SFX_FLD_NPC_LUIGI_JUMP1", 0x2000021E, "", 75, 64, 0, 1, 0, 0, 0xFD55},
	{"SFX_FLD_NPC_LUIGI_LANDING1", 0x20000387, "", 65, 64, 0, 1, 0, 0, 0xFD55},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_KURI_MOVE1L", 0x20000440, "btl_e_move_walk1l_22k", 45, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_KURI_MOVE1R", 0x20000441, "btl_e_move_walk1r_22k", 45, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_KINOPIO_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_KINOPIO_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_KINOPIO_S_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x800},
	{"SFX_FLD_NPC_KINOPIO_S_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_NOKO_MOVE1L", 0x200004F4, "btl_e_noko_move3l_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_NOKO_MOVE1R", 0x200004F5, "btl_e_noko_move3r_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_BOMB_MOVE1", 0x200003F9, "btl_e_bomb_move2_22k", 20, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_FLOWER_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_FLOWER_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_POWER_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_POWER_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_STARMANIA_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_STARMANIA_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_INFO_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_INFO_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_BOTTAKURU_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_BOTTAKURU_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_CHUSAN_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_CHUSAN_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_KIZA_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_KIZA_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_MOKOMOKO_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_MOKOMOKO_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_NAZO_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_NAZO_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_THIEF_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_THIEF_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_MONTE_MOVE1L", 0x200004C6, "btl_e_borodo_move1l_22k", 55, 64, 0, 1, 0, 0, 0xFAAB},
	{"SFX_FLD_NPC_MONTE_MOVE1R", 0x200004C7, "btl_e_borodo_move1r_22k", 55, 64, 0, 1, 0, 0, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_MASTER_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_MASTER_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_STG4_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_STG4_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_STG4_S_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x800},
	{"SFX_FLD_NPC_STG4_S_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_MONBAN_MOVE1L", 0x200004E6, "btl_e_monban_move2ｌ_22k", 40, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_MONBAN_MOVE1R", 0x200004E7, "btl_e_monban_move2ｒ_22k", 40, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_ROTEN_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_ROTEN_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_JUGEM_MOVE1", 0x200003BE, "", 30, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_PENGUIN_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_PENGUIN_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_YOSHI_MOVE1L", 0x200004F4, "btl_e_noko_move3l_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_YOSHI_MOVE1R", 0x200004F5, "btl_e_noko_move3r_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_BORODO_MOVE1L", 0x200004C6, "btl_e_borodo_move1l_22k", 40, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_BORODO_MOVE1R", 0x200004C7, "btl_e_borodo_move1r_22k", 40, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_PUKUPUKUI_MOVE1", 0x200003BE, "", 30, 64, 0, 1, 0, 0, 0x2AB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_MET_MOVE1L", 0x200004E0, "btl_e_met_move1l_22k", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_MET_MOVE1R", 0x200004E1, "btl_e_met_move1r_22k", 50, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_CHORO_WAIT1", 0x2000040B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_CHORO_WAIT2", 0x2000040C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_LANDING1", 0x2000020A, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_NPC_JUMP1", 0x200004FF, "btl_e_pata_move2_22k", 65, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_MAJO_HANG1", 0x200003BE, "", 30, 64, 0, 1, 0, 0, 0x2AB},
	{"SFX_FLD_NPC_MARI_HANG1", 0x200003BE, "", 30, 64, 0, 1, 0, 0, 0xFD55},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_SYURYO_MOVE1L", 0x200004B0, "btl_e_3rd_walkl1_22k", 70, 64, 0, 1, 0, 0, 0xF800},
	{"SFX_FLD_NPC_SYURYO_MOVE1R", 0x200004B1, "btl_e_3rd_walkr1_22k", 70, 64, 0, 1, 0, 0, 0xF800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_KANBU_MOVE1L", 0x200004B0, "btl_e_3rd_walkl1_22k", 70, 64, 0, 1, 0, 0, 0xFD55},
	{"SFX_FLD_NPC_KANBU_MOVE1R", 0x200004B1, "btl_e_3rd_walkr1_22k", 70, 64, 0, 1, 0, 0, 0xFD55},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_NPC_KANBU_JUMP1", 0x20000426, "btl_e_jump_normal1_22k", 70, 64, 0, 1, 0, 0, 0xFD55},
	{"SFX_FLD_NPC_KANBU_LANDING1", 0x20000435, "btl_e_3rd_landing1_22k", 70, 64, 0, 1, 0, 0, 0xFD55},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF_STG_START_CURTAIN_UP1", 0x20000554, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_STG_START_CURTAIN_DOWN1", 0x20000553, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_BTL_START_CURTAIN_UP1", 0x20000545, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_BTL_START_CURTAIN_DOWN1", 0x20000544, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_BTL_KORTESS_CHANGE1", 0x20000542, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_BTL_KORTESS_CHANGE2", 0x20000543, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF_KUPPA_WALL_BREAK1", 0x2000054A, "SE6_OFF_KUPPA_WALL_BREAK1", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF_PARTY_LECTURE1_1", 0x40044040, "", 95, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_PARTY_LECTURE1", 0x40041717, "", 75, 64, 0, 1, 0, 20, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF_MAP_BECOME_OLD1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_SHIP_REVERSE1", 0x2000054F, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_FENCE_OPEN1", 0x20000549, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_FENCE_FALL1", 0x20000548, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF_STEP_APPEAR1", 0x20000550, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_STEP_APPEAR2", 0x20000551, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_STEP_APPEAR3", 0x20000552, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF0_STEP_APPEAR1_2", 0x40041616, "", 115, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF1_ROCK_BREAK1", 0x20000536, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_PERA1", 0x2000046D, "btl_e_bomb_move1_22k", 80, 64, 0, 1, 0, 0, 0x2AB},
	{"SFX_OFF1_PUZZLE1", 0x20000533, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF1_PUZZLE2", 0x20000534, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF1_PUZZLE_MOVE1", 0x20000535, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_PERA_STEP1", 0x40043D3D, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_PERA_STEP2", 0x40043E3E, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_PERA2", 0x40043F3F, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF1_PERA_BREAK1", 0x40046666, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF1_PERA_FALL1", 0x20000538, "", 85, 64, 0, 1, 0, 0, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF2_ENTER_APPEAR1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF2_DOOR_GRID_SHUT1", 0x20000539, "", 100, 64, 0, 1, 0, 10, 0x0},
	{"SFX_OFF2_DOKAN_APPEAR1", 0x20000537, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF2_DOKAN_APPEAR2", 0x20000538, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF2_SLOT_GOOUT1", 0x2000053B, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF2_SLOT_GOOUT2", 0x2000053B, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF2_SLOT_GOOUT3", 0x2000053B, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF2_SLOT_GOOUT4", 0x2000053B, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF2_TORIDE_BREAK2", 0x2000053C, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF2_BOSS_APPEAR1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF3_VIEW_ROOM1", 0x40097B7B, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF3_VIEW_ROOM2", 0x40097C7C, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF3_PERA_KAIDAN_APPEAR1", 0x2000046B, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF3_PERA_KAIDAN_APPEAR2", 0x2000023D, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF3_NAME_ENTRY1", 0x40097777, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF3_NAME_ENTRY2", 0x40097878, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF3_MAP_CHANGE1", 0x2000053E, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF4_LANPEL_APPEAR1", 0x2000046C, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF4_NAME_ENTRY1", 0x40097777, "", 127, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF4_NAME_ENTRY2", 0x40097878, "", 127, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF5_OBJ_ROCK_BREAK1", 0x20000555, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF5_SHIP_APPEAR1", 0x2000053D, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF5_SHIP_REVERSE1", 0x2000054F, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF6_ROOM_IN1", 0x2000053E, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF6_ROOM_OUT1", 0x2000053F, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF6_SHOP_IN1", 0x20000540, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF6_SHOP_OUT1", 0x20000541, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF7_KAIDAN_APPEAR1", 0x40097F7F, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF7_PASSWORD_ENTRY1", 0x40097777, "", 127, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF7_PASSWORD_ENTRY2", 0x40097878, "", 127, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF7_PUZZLE1", 0x20000533, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF7_PUZZLE2", 0x20000534, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF_CLAUD_WIND2", 0x20000546, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CLAUD_WIND3", 0x20000547, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF_BLOCK_BREAK1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_WALL_BREAK1", 0x20000555, "", 115, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF_PERAMAJIN_APPEAR1", 0x2000054D, "", 85, 64, 0, 1, 0, 6, 0x0},
	{"SFX_OFF_PERAMAJIN_COME1", 0x400B8585, "", 105, 64, 0, 1, 0, 10, 0x0},
	{"SFX_OFF_PERAMAJIN_GOOUT1", 0x400B8686, "", 100, 64, 0, 1, 0, 10, 0x0},
	{"SFX_OFF_PERAMAJIN_GOOUT2", 0x2000054E, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF_HATENA_APPEAR1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_HATENA_END1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_HATENA_TURN1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP2", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP3", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP4", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP5", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP6", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP7", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP8", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP_DOKAN1", 0x40097D7D, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP_DOKAN2", 0x40097D7D, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP_DOKAN3", 0x40097D7D, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP_DOKAN4", 0x40097D7D, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP_DOKAN5", 0x40097E7E, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP_DOKAN6", 0x40097E7E, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP_DOKAN7", 0x40097E7E, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_MAP_DOKAN8", 0x40097E7E, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF_CHANGE_STORY1", 0x40098383, "", 85, 64, 0, 1, 0, 7, 0x0},
	{"SFX_OFF_CHANGE_STORY2", 0x40098484, "", 85, 64, 0, 1, 0, 2, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_OFF_MARIO_SLEEP1", 0x2000054B, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_MARIO_SLEEP2", 0x2000054C, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_OFF_MARIO_AWAKE1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_LUIGI_MOVE1L", 0x20000000, "SE4_FLD_M_WALK_NORMAL1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_LUIGI_MOVE1R", 0x20000000, "SE4_FLD_M_WALK_NORMAL2", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_OPN_BOOK_OPEN1", 0x2000020D, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_OPN_PAGE_OPEN1", 0x2000020E, "", 100, 64, 0, 60, 0, 15, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_OPN_PARETTA_MOVE1", 0x200004FE, "btl_e_pata_move1_22k", 95, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_POST_SHAKE1", 0x2000020F, "", 115, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_OPN_SHIP_MOVE1", 0x20000160, "", 95, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_OPN_SHIP_CREAK1", 0x20000076, "", 50, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_SHIP_MOVE1", 0x20000160, "", 95, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG0_SHIP_JUMP1", 0x20000556, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG0_SHIP_JUMP2", 0x20000505, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_SHIP_LANDING1", 0x20000557, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_3RD_TOGETHER1", 0x2000015B, "", 75, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG0_3RD_TOGETHER1L", 0x200004B0, "btl_e_3rd_walkl1_22k", 90, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG0_3RD_TOGETHER1R", 0x200004B1, "btl_e_3rd_walkr1_22k", 90, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_3RD_TOGETHER1_2", 0x40041515, "", 80, 64, 0, 20, 0, 10, 0x0},
	{"SFX_STG0_3RD_ATTACK1", 0x200004C9, "", 70, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG0_3RD_ATTACK2", 0x20000369, "", 80, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG0_3RD_ATTACK3", 0x20000412, "", 85, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG0_3RD_QUESTION_MARK1", 0x2000003C, "", 60, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_KURI_KISS1", 0x2000033A, "", 95, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_KURI_KISS2", 0x20000309, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_THIEF_FIGHT1", 0x20000316, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_THIEF_FIGHT2", 0x2000052B, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_THIEF_DOWN1", 0x20000414, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_BORODO_RUN1", 0x200004C9, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_BORODO_STEAL1", 0x200004C2, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_MARIO_ROUND1", 0x2000052B, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_MARIO_FALL_CRUSH1", 0x2000021A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_KURI_MOVE1L", 0x20000440, "btl_e_move_walk1l_22k", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_KURI_MOVE1R", 0x20000441, "btl_e_move_walk1r_22k", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_KURI_JUMP1", 0x20000426, "btl_e_jump_normal1_22k", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_KURI_ESCAPE1", 0x20000364, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_FENCE_OPEN1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_FENCE_OPEN2", 0x20000000, "SE2_AUD_THROW_OBJ2", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_MAP_APPEAR1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_MOBJ_APPEAR1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_KURIHAKASE_MOVE1L", 0x20000000, "SE5_BTL_E_MOVE_WALK1L", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_KURIHAKASE_MOVE1R", 0x20000000, "SE5_BTL_E_MOVE_WALK1R", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_PUNI_MOVE1", 0x2000017A, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_STARSTONE_PUTOUT1", 0x20000000, "SE2_BTL_SAC_START3", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_STARSTONE_PUTOUT2", 0x20000000, "SE2_EVT_SSTONE_PUTOUT2", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_STARSTONE_EFFECT1", 0x20000000, "SE2_BTL_SAC_START1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_STARSTONE_EFFECT2", 0x20000000, "SE2_EVT_SSTONE_EFFECT2", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_STARSTONE_EFFECT3", 0x20000000, "SE2_EVT_SSTONE_EFFECT3", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_STARSTONE_EFFECT4", 0x20000000, "SE2_EVT_5_ROCK_EYE_SHINE1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_STARSTONE_EFFECT5", 0x20000000, "SE2_EVT_1_STARSTONE_SHINE1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_STARSTONE_EFFECT6", 0x20000000, "SE2_EVT_SSTONE_EFFECT6", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_STARSTONE_EFFECT7", 0x20000000, "SE2_EVT_SSTONE_EFFECT7", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_MAP_PUTOUT1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_MAP_DOWN1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_SHUT_BOOK1", 0x20000161, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_OPEN_BOOK1", 0x2000015C, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_PUT_BOOK1", 0x20000161, "", 85, 64, 0, 1, 0, 0, 0xFAAB},
	{"SFX_STG0_LENS_BREAK1", 0x2000015F, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_GESO_LEG_MOVE1", 0x2000015E, "", 50, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG0_GESO_LEG_MOVE2", 0x2000021B, "", 90, 64, 0, 20, 0, 0, 0x800},
	{"SFX_STG0_GESO_JUMP1", 0x200000D5, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG0_GESO_LANDING1", 0x2000015D, "", 110, 64, 1, 20, 0, 0, 0x0},
	{"SFX_STG0_GESO_APPEAR2", 0x40043838, "", 110, 64, 1, 20, 0, 0, 0x0},
	{"SFX_STG0_GESO_SHAKE1", 0x2000015E, "", 50, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG0_GESO_CRASH1", 0x2000054A, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG0_ROAD_APPEAR1", 0x20000556, "", 80, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_GESO_WAIT1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_GESO_FALL1", 0x20000000, "SE2_AUD_THROW_OBJ1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_KINOBROS_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_KINOBROS_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_LOAD_APPEAR1", 0x200003E5, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_TIK_FLOOR_MOVE1", 0x20000052, "", 60, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_AIR_SHIP_DRIVE1", 0x20000053, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_KURAGARI_PEN1", 0x40041D1D, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_MARIO_ROLL_DOWN1", 0x20000552, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_MARIO_ROLL_DOWN2", 0x2000016A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_SHIP_MOVE1", 0x20000076, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_SHIP_MAST1", 0x2000052F, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_WAVE1", 0x2000000F, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_SHIP_MOVE2", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_TRAIN_BELL1", 0x200001C8, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_TRAIN_MOVE1", 0x40010000, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_TRAIN_MOVE2", 0x40010101, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_TBOX_FALL1", 0x2000037A, "", 70, 64, 0, 1, 0, 0, 0x1000},
	{"SFX_STG6_TBOX_FALL2", 0x20000163, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_PASS_ACCESS1", 0x20000000, "SE2_EVT_6_PASS_ACCESS1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_PASS_OK1", 0x20000000, "SE2_EVT_7_ELEVATOR_KEY1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_PASS_NG1", 0x20000000, "SE2_EVT_6_PASS_NG1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_DOOR_LAST_OPEN1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_MONBAN_ESCAPE1", 0x20000364, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_GNB_ROAR1", 0x2000004B, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG1_GNB_ROAR2", 0x2000004B, "", 127, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG1_GNB_FLY1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_GNB_ENV1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_GNB_STEP1", 0x20000163, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG1_GNB_DOWN1", 0x200000E0, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG1_GNB_DOWN2", 0x2000016B, "", 110, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG1_GNB_DOWN3", 0x20000162, "", 50, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_QUAKE1", 0x20000168, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_STG1_QUAKE2", 0x2000016A, "", 105, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_QUAKE3", 0x20000169, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_STG1_QUAKE4", 0x2000016B, "", 80, 64, 0, 15, 0, 0, 0x0},
	{"SFX_STG1_QUAKE5", 0x20000168, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_STG1_QUAKE6", 0x20000168, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_STG1_DSN_ROUND1", 0x200003A9, "", 50, 64, 0, 15, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_SINNOSUKE_JUMP1", 0x2000050C, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_SINNOSUKE_LANDING1", 0x2000016A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_TOGEDA_JUMP1", 0x2000050C, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_TOGEDA_LANDING1", 0x2000016A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_SINNOSUKE_LANDING2", 0x2000016A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_SINNOSUKE_LANDING3", 0x20000417, "", 110, 64, 0, 0, 0, 0, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_GOOD_SOUND1", 0x2000016E, "", 95, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BAD_SOUND1", 0x2000016C, "", 95, 64, 0, 1, 0, 0, 0x0},
	{"SFX_GOOD_SOUND2", 0x2000016F, "", 95, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BAD_SOUND2", 0x2000016D, "", 95, 64, 0, 1, 0, 0, 0x0},
	{"SFX_QUIZ_START1", 0x20000170, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_TOGE_FLOAR_UP1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_CHORO_FALL1", 0x2000040C, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_CHORO_G_WAIT1", 0x2000040B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_CHORO_G_WAIT2", 0x2000040C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_CHORO_MOVE1", 0x20000407, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_CHORO_ESCAPE1", 0x20000407, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_GRID_UP1", 0x200003AB, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG1_GRID_UP2", 0x20000165, "", 70, 64, 0, 25, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_NOKORIN_MOVE1", 0x2000036A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_NOKORIN_MOVE2", 0x2000036B, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_STONE_SET1", 0x20000173, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_ROCK_MOVE1", 0x200003A9, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG1ROCK_QUAKE1", 0x20000168, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG1ROCK_FONT_APPEAR1", 0x200001DC, "SE2_EVT_3_TOUROKU_COM1", 100, 64, 0, 25, 0, 5, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX__STG1_ROCK_BREAK1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_HONE_ACCESS1", 0x2000052C, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_HONE_DOWN1", 0x20000000, "SE2_AUD_THROW_OBJ2", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_HONE_DOWN1_2", 0x200000C6, "", 110, 64, 0, 1, 0, 20, 0x0},
	{"SFX_STG1_HONE_LANDING1", 0x200004F5, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_HONE__MOVE1", 0x40041212, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_HONE_FLY1", 0x20000166, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_HONE_ESCAPE1", 0x40043939, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX__STG1_HONE__MOVE1", 0x20000000, "SE6_BTL_E_NOKO_MOVE3L", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX__STG1_HONE__MOVE2", 0x20000000, "SE6_BTL_E_NOKO_MOVE3R", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_DOOR_GRID_OPEN1", 0x200003AB, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG1_DOOR_GRID_SHUT1", 0x20000164, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG1_DOOR_GRID_DOWN1", 0x200003B4, "", 105, 64, 0, 25, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_CHURINA_MOVE1L", 0x200003BF, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_CHURINA_MOVE1R", 0x200003C0, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_CHURINA_LANDING1", 0x200003BB, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_CHURINA_KISS1", 0x2000033A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_CHURINA_KISS2", 0x20000309, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_CHURINA_JUMP1", 0x200003BA, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_CHURINA_ESCAPE1", 0x20000535, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_NOKOPAPA_OUT1_1", 0x2000052D, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG1_NOKOPAPA_OUT1_2", 0x2000052E, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG1_NOKOPAPA_OUT1_3", 0x200000DB, "", 100, 64, 0, 20, 0, 10, 0x0},
	{"SFX_STG1_NOKOPAPA_OUT2", 0x200004D2, "", 70, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG1_NOKOPAPA_OUT3", 0x20000521, "SE4_EVT_1_NOKOPAPA_OUT3", 50, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG1_NOKOPAPA_LANDING1", 0x20000506, "btl_e_landing_hard1_22k", 65, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX__STG1_PERA1", 0x20000000, "SE2_EVT_1_PERA_BRIDGE1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX__STG1_PERA2", 0x20000000, "SE2_EVT_1_PERA_BRIDGE2", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX__STG1_PERA_STEP1", 0x20000000, "SE2_EVT_1_PERA_STEP1", 0, 64, 0, 40, 0, 0, 0x0},
	{"SFX_STG1_QUAK_PERA1", 0x2000016A, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_TOGE_UP1", 0x20000174, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_STARSTONE_SHINE1", 0x20000000, "SE2_EVT_1_STARSTONE_SHINE1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG1_STARSTONE_JUMP1", 0x20000000, "SE2_EVT_1_STARSTONE_JUMP1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_ROAD_APPEAR1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_VIVI_NECKLACE1", 0x200004A9, "btl_e_3rd_hoist1_22k", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_MAJO_NECKLACE_STEAL1", 0x200000AD, "", 70, 64, 0, 1, 0, 0, 0x800},
	{"SFX_STG2_MAJO_NECKLACE_STEAL2", 0x200004A4, "btl_e_3rd_cloth1_22k", 70, 64, 0, 1, 0, 15, 0xD55},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_SISTERS_ESCAPE1", 0x20000364, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_SISTERS_ESCAPE2", 0x2000036D, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_STEP_EXTEND1", 0x200003CF, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_CRAUD_LIGHT1", 0x20000057, "", 50, 64, 0, 10, 0, 0, 0x0},
	{"SFX_STG2_CRAUD_LIGHT2", 0x20000057, "", 50, 64, 0, 10, 0, 0, 0x0},
	{"SFX_STG2_CRAUD_FLOWER1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_CRAUD_WINK1", 0x2000036C, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_STG2_CRAUD_KISS1", 0x40077373, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_CRAUD_ATTACK1", 0x20000332, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_LOCK_OPEN1", 0x200003EC, "", 105, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_PUNIDAMA_SET1", 0x20000173, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_PUNI_MOVE1", 0x2000017A, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_PUNI_MOVE2", 0x20000000, "SE2_EVT_2_PUNI_MOVE2", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_PUNI_MOVE3", 0x20000000, "SE2_EVT_2_PUNI_MOVE3", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_TOGE_MOVE1", 0x20000186, "", 45, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_TOGE_MOVE2", 0x20000187, "", 45, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_TOGE_MOVE3", 0x20000188, "", 45, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_TOGE_JUMP1", 0x20000185, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_TOGETTI_MOVE1", 0x20000186, "", 40, 64, 0, 1, 0, 0, 0x555},
	{"SFX_STG2_TOGETTI_JUMP1", 0x20000185, "", 40, 64, 0, 1, 0, 0, 0x555},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_PUNISWITCH_RIDE1", 0x20000072, "", 60, 64, 0, 50, 0, 0, 0x800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_PUNI_MOVE2_1", 0x2000017B, "", 40, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_PUNI_MOVE2_2", 0x2000017C, "", 40, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_PUNI_MOVE2_3", 0x2000017D, "", 40, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_PUNI_MOVE2_4", 0x2000017E, "", 40, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_PUNI_MOVE2_5", 0x2000017F, "", 40, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_PUNI_MOVE2_6", 0x20000180, "", 40, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_PUNI_JUMP1", 0x20000179, "", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_PUNI_HIDE1", 0x20000178, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_PUNIKO_KINOKO1", 0x200004A9, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_PUNI_EAT1", 0x20000219, "", 50, 64, 0, 1, 0, 0, 0x555},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_DOKAN_QUAKE1", 0x2000016B, "", 100, 64, 0, 40, 0, 0, 0x0},
	{"SFX_STG2_DOKAN_APPEAR1", 0x200003DF, "", 100, 64, 0, 1, 0, 0, 0xF800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_CHURINA_JUMP1", 0x200003BA, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_CHURINA_LANDING1", 0x200003BB, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_CHURINA_ATTACK1", 0x20000411, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_CHURINA_ESCAPE1", 0x20000535, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_CHURINA_KISS1", 0x200003B9, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_CHURINA_KISS2", 0x20000309, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_3RD_DOWN1", 0x20000414, "", 85, 64, 0, 1, 0, 10, 0x0},
	{"SFX_STG2_3RD_WAKE1", 0x20000426, "btl_e_jump_normal1_22k", 65, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_DOOR_GRID_OPEN1", 0x20000164, "", 100, 64, 0, 40, 0, 0, 0xF800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_CHOUROU_PUNIDAMA1", 0x200004A9, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_CHOUROU_CALL_PUNI1", 0x200000AF, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_SWITCH_COUNTER1", 0x20000184, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_SWITCH_SHINE1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_SWITCH_ON1", 0x20000168, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_SWITCH_OFF1", 0x20000168, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_SWITCH_QUAKE1", 0x2000016B, "", 100, 64, 0, 40, 0, 0, 0x0},
	{"SFX_STG2_SOAPBALL_MOVE1", 0x20000183, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_SOAPBALL_BREAK1", 0x20000182, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_SIREN1", 0x20000181, "", 90, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG2_FIGHT_TOGE1", 0x2000036D, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_FIGHT_TOGE2", 0x2000036E, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_FIGHT_TOGE3", 0x20000413, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_TORIDE_BREAK1", 0x2000016A, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX__STG2_TORIDE_BREAK2", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX__STG2_DOOR_GRID_SHUT1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_DOOR_GRID_DOWN1", 0x2000053A, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_3RD_FALL1", 0x200004DD, "btl_e_met_fall1_22k", 70, 64, 0, 1, 0, 0, 0xFAAB},
	{"SFX_STG2_3RD_LANDING1", 0x200004B0, "btl_e_3rd_walkl1_22k", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_3RD_MOVE1L", 0x20000000, "SE6_BTL_E_3RD_WALKL1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_3RD_MOVE1R", 0x20000000, "SE6_BTL_E_3RD_WALKR1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_KANBU_FALL1", 0x200004DD, "btl_e_met_fall1_22k", 70, 64, 0, 1, 0, 0, 0xF800},
	{"SFX_STG2_KANBU_LANDING1", 0x200004B0, "", 90, 64, 0, 1, 0, 0, 0xFD55},
	{"SFX_STG2_KANBU_MOVE1L", 0x20000000, "SE6_BTL_E_3RD_WALKL1", 0, 64, 0, 1, 0, 0, 0xFD55},
	{"SFX_STG2_KANBU_MOVE1R", 0x20000000, "SE6_BTL_E_3RD_WALKR1", 0, 64, 0, 1, 0, 0, 0xFD55},
	{"SFX_STG2_3RD1_MISS1", 0x200004AC, "btl_e_3rd_miss1_22k", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX__STG2_DOKAN_APPEAR1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX__STG2_DOKAN_APPEAR2", 0x20000000, "SE2_AUD_THROW_OBJ2", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_SLOT_TURN1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX__STG2_SLOT_GOOUT1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_QUAKE1", 0x20000168, "", 100, 64, 0, 40, 0, 0, 0x0},
	{"SFX_STG2_QUAKE2", 0x20000168, "", 100, 64, 0, 40, 0, 0, 0x0},
	{"SFX_STG2_WATER_FLOW1", 0x20000004, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_STG2_FLOOR_MOVE1", 0x20000052, "", 60, 64, 0, 1, 0, 0, 0xFAAB},
	{"SFX_STG2_FLOOR_STOP1", 0x2000016A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_LIGHT1", 0x40043B3B, "", 90, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG2_STARSTONE_APPEAR1", 0x40043C3C, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_KANBU_STARSTONE_GET1", 0x20000176, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_BOMB_SWITCH_APPEAR1", 0x200004A9, "btl_e_3rd_hoist1_22k", 75, 64, 0, 1, 0, 10, 0x0},
	{"SFX_STG2_BOMB_SWITCH_AWAY1", 0x200004A4, "btl_e_3rd_cloth1_22k", 65, 64, 0, 0, 0, 5, 0x0},
	{"SFX_STG2_DOKAN_FALL1", 0x20000552, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_BOMB_TIMER_SET1", 0x20000173, "", 100, 64, 0, 1, 0, 0, 0x555},
	{"SFX_STG2_BOMB_TIMER1", 0x2000034A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_DOOR_UNLOCK", 0x200001D3, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_DOOR_LAMP", 0x2000019E, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_CHOUROU_BIG1", 0x200002F8, "", 100, 64, 0, 1, 0, 0, 0x555},
	{"SFX_STG2_CHOUROU_WAIST1", 0x20000175, "", 127, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_KANBU_SMALL1", 0x200002FF, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX__STG2_BOSS_APPEAR1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_BOSS_EXPLOSION1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_BOSS_APPEAR1", 0x20000057, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_BOSS_APPEAR2", 0x20000167, "", 127, 64, 0, 1, 0, 0, 0xFAAB},
	{"SFX_STG2_BOSS_LIGHT1", 0x40043737, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_BOSS_SET1", 0x200001DA, "", 100, 64, 0, 1, 0, 0, 0x555},
	{"SFX_STG2_BOSS_SET2", 0x40046464, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_BOSS_LANDING1", 0x20000103, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_BOSS_MOVE1L", 0x20000106, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG2_BOSS_MOVE1R", 0x20000106, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG2_3RD_EXPLSION1", 0x200002F5, "", 85, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG2_3RD_EXPLSION2", 0x40044A4A, "", 90, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG2_3RD_EXPLSION3", 0x200001C9, "", 127, 64, 0, 40, 0, 0, 0x0},
	{"SFX_STG2_KANBU_ROUND1", 0x200004A2, "btl_e_3rd_att1_22k", 100, 64, 0, 1, 0, 0, 0xFD55},
	{"SFX_STG2_KANBU_STICK1", 0x20000177, "", 100, 64, 0, 1, 0, 0, 0xFD55},
	{"SFX_STG2_KANBU_ESCAPE1", 0x20000364, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_AIR_SHIP_DRIVE1", 0x20000053, "", 60, 64, 0, 1, 0, 0, 0x555},
	{"SFX_STG3_AIR_SHIP_DRIVE1_2", 0x2000019B, "", 65, 64, 0, 1, 0, 0, 0x555},
	{"SFX_STG3_AIR_SHIP_DRIVE2", 0x40042A2A, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_AIR_SHIP_DRIVE3", 0x40042B2B, "", 60, 64, 0, 1, 0, 30, 0x0},
	{"SFX_STG3_AIR_SHIP_DRIVE4", 0x40042C2C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_AIR_SHIP_DRIVE5", 0x40042D2D, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_AIR_SHIP_REVERSE1", 0x2000054F, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_AIR_SHIP_STOP1", 0x20000000, "SE2_AMB_AIRSHIP_IDOL1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_AIR_SHIP_GONDOLA_MOVE1", 0x200001BC, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_AIR_SHIP_GONDOLA_DOOR1", 0x200003A6, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_KAIDAN_APPEAR1", 0x2000046B, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_TELEPHONE1", 0x2000019C, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_AUTO_DOOR1", 0x2000018C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX__STG3_AUDIENCE1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX__STG3_AUDIENCE2", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX__STG3_AUDIENCE3", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX__STG3_AUDIENCE_FAN1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX__STG3_AUDIENCE_FAN2", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX__STG3_AUDIENCE_FAN3", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_AUDIENCE_FAN1", 0x40041313, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_AUDIENCE_FAN_FLY1", 0x20000166, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_BLED_MOVE1L", 0x200000BF, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_BLED_MOVE1R", 0x200000C0, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_BLED_JUMP1", 0x200000BD, "", 115, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_BLED_ATTACK1", 0x2000040F, "", 115, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_TOGENOKO_DOWN1", 0x200004F3, "btl_e_noko_move2_22k", 120, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_WIN_AUDIENCE1", 0x2000004F, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_WIN_AUDIENCE2", 0x2000004F, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_FIGHTING1", 0x20000399, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_FIGHTING2", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_FIGHTING3", 0x20000316, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_KURI_ATTACK1", 0x20000426, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_KURI_FURAFURA1", 0x20000131, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_GONG1", 0x20000194, "", 110, 64, 1, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_CHAMP_CRITICAL1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_HIKIDASI1", 0x20000000, "SE2_EVT_3_HIKIDASI1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_DRAWER_OPEN1", 0x2000018E, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_DRAWER_SHUT1", 0x2000018F, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_TOUROKU_COM1", 0x20000070, "", 70, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG3_TOUROKU_COM2", 0x200001DC, "SE2_EVT_3_TOUROKU_COM1", 70, 64, 0, 40, 0, 0, 0x800},
	{"SFX_STG3_LOCKER_OPEN1", 0x2000019A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_LOCKER_CLOSE1", 0x20000199, "", 100, 64, 0, 1, 0, 20, 0x0},
	{"SFX_STG3_TOILET1", 0x2000019D, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG3_DRAINING1", 0x40041414, "", 90, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG3_FLY1", 0x20000191, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX__STG3_NAME_ENTRY1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_DUCT_KOUSHI_HIT1", 0x20000190, "SE2_EVT_3_AIRDUCT_HIT1", 110, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG3_DUCT_KOUSHI_FALL1", 0x20000143, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_YOSH_EGG_JUMP1", 0x200003C1, "", 127, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_YOSH_EGG_JUMP2", 0x200003C1, "", 65, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_EAT_CAKE1", 0x20000219, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_AIRDUCT_SHAKE1", 0x2000018B, "", 70, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_DOOR_NOTOPEN1", 0x2000018D, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_BLED_DIE1", 0x200000B9, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_GROUND_SHAKE1", 0x2000016B, "", 100, 64, 0, 40, 0, 0, 0x0},
	{"SFX_STG3_RING_DOWN1", 0x2000025B, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG3_ABSORB_DEVICE_UP1", 0x200001CD, "", 80, 64, 0, 20, 0, 0, 0x2AB},
	{"SFX_STG3_ABSORB_DEVICE_UP_STOP1", 0x2000025B, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG3_ABSORB_DEVICE1", 0x20000189, "", 80, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG3_ABSORB_DEVICE2", 0x2000018A, "", 80, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG3_GANSU_BIG1", 0x20000192, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG3_GANSU_BIG2", 0x20000193, "", 110, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_STARSTONE_ROUND1", 0x40044545, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG3_KINOSICHOFU_LANDING1", 0x2000020A, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_STARSTONE_ROUND2", 0x40044646, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX__STG3_PERA_KAIDAN_APPEAR1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_GUARDMAN_MOVE1L", 0x20000399, "", 70, 64, 0, 1, 0, 0, 0xFAAB},
	{"SFX_STG3_GUARDMAN_MOVE1R", 0x2000039A, "", 70, 64, 0, 1, 0, 0, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_GANSU_MOVE1L", 0x200000BF, "", 70, 64, 0, 1, 0, 0, 0x555},
	{"SFX_STG3_GANSU_MOVE1R", 0x200000C0, "", 70, 64, 0, 1, 0, 0, 0x555},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_BLED_MOVE1L_", 0x20000000, "SE2_BOS_GANSU_MOVE1L", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_BLED_MOVE1R_", 0x20000000, "SE2_BOS_GANSU_MOVE1R", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_KINOSIKOWA_MOVE1L", 0x20000197, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_KINOSIKOWA_MOVE1R", 0x20000198, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_KINOSIKOWA_LANDING1", 0x20000196, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_KINOSIKOWA_JUMP1", 0x200004FF, "btl_e_pata_move2_22k", 50, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_HOTDOG_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_HOTDOG_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_MICHAEL_MOVE1L", 0x20000000, "SE6_BTL_E_NOKO_MOVE3L", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_MICHAEL_MOVE1R", 0x20000000, "SE6_BTL_E_NOKO_MOVE3R", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_BLOTZ_MOVE1L", 0x20000000, "SE6_BTL_E_BORODO_MOVE1L", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_BLOTZ_MOVE1R", 0x20000000, "SE6_BTL_E_BORODO_MOVE1R", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_POGU_MOVE1", 0x20000000, "SE6_BTL_E_BASA_MOVE1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_ISIN_MOVE1L", 0x20000417, "btl_e_emon_walkl1_22k", 100, 64, 0, 0, 0, 0, 0xF800},
	{"SFX_STG3_ISIN_MOVE1R", 0x20000418, "btl_e_emon_walkr2_22k", 100, 64, 0, 0, 0, 0, 0xF800},
	{"SFX_STG3_ISIN_JUMP1", 0x20000195, "SE3_EVT_K_JUMP1", 100, 64, 0, 0, 0, 0, 0xFD55},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_YOSHI_MOVE1L", 0x20000000, "SE4_FLD_P_WALK_NORMAL1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_YOSHI_MOVE1R", 0x20000000, "SE4_FLD_P_WALK_NORMAL2", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_KOOPA_MOVE1L", 0x20000049, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_KOOPA_MOVE1R", 0x2000004A, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_KOOPA_JUMP1", 0x20000024, "evt_k_jump1_16k", 110, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_CHURINA_MOVE1L", 0x20000000, "SE4_FLD_P_WALK_NORMAL1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_CHURINA_MOVE1R", 0x20000000, "SE4_FLD_P_WALK_NORMAL2", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_CHURINA_LANDING1", 0x20000000, "SE4_FLD_P_LANDING1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_CHURINA_KISS1", 0x2000033A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_CHURINA_KISS2", 0x20000309, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_CHURINA_JUMP1", 0x20000000, "SE4_FLD_P_JUMP1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_NPC_MOVE1L", 0x2000020B, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_NPC_MOVE1R", 0x2000020C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_TEMPLE_BELL1", 0x20000048, "", 115, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_CHANGE_PIG1", 0x200001A0, "", 115, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_VOICE_PIG1", 0x200001A7, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_FENCE_OPEN1", 0x2000023B, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_CROW_HOWL1", 0x200001A1, "", 90, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_FRY_STRAW1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_MOVE_ROCK1", 0x200003A9, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_BOX_SWING1", 0x2000015E, "", 50, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_BOX_OPEN1", 0x200003DC, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_TELESA_VOICE1", 0x20000000, "SE6_EVT_4_TELESA_VOICE1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_TELESA_APPEAR1", 0x40041818, "", 90, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_TELESA_SURPRISED1", 0x20000467, "", 110, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_TELESA_APPEAR1_2", 0x2000045A, "", 90, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_TELESA_APPEAR2", 0x40040D0D, "", 117, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_TELESA_TOGETHER1", 0x2000045A, "", 60, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_TELESA_LIGHT1", 0x200001A6, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_TELESA_A_APPEAR1", 0x2000019F, "SE2_EVT_4_ATMTELESA_APPEAR1", 120, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_TELESA_ESCAPE1", 0x20000166, "", 90, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_MARIO_DOWN1", 0x20000413, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_MARIO_DOWN2", 0x20000414, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_MOVE_PEDESTAL1", 0x200003A9, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_MOVE_PEDESTAL2", 0x2000016B, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_TBOX_OPEN1", 0x200003DC, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_KAIDAN_MOVE1", 0x2000016B, "", 120, 64, 0, 30, 0, 30, 0x0},
	{"SFX_STG4_CHANGE_LANPEL1", 0x200001A0, "", 110, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_GRID_MOVE1", 0x200001A4, "", 105, 64, 0, 50, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_GATE_CLOSED1", 0x200001A2, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_GATE_OPEN1", 0x200003A7, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG4_GATE_LOCK_FALL1", 0x200001A3, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_VIVIAN_KISS1", 0x2000033A, "", 95, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_VIVIAN_KISS2", 0x20000309, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_LANPEL_SHOCK1", 0x200000DE, "", 100, 64, 0, 1, 0, 0, 0x800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_LANPEL_M_JUMP1", 0x200002CD, "", 115, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_LANPEL_M_MOVE1L", 0x20000399, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_LANPEL_M_MOVE1R", 0x2000039A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_LANPEL_M_DOWN1", 0x2000021A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_LANPEL_JUMP1", 0x200004FF, "btl_e_pata_move2_22k", 65, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_LANPEL_JUMP2", 0x2000021A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_LANPEL_M_LANDING1", 0x200004DE, "btl_e_met_jump1_22k", 60, 64, 0, 0, 0, 0, 0x555},
	{"SFX_STG4_LANPEL_M_FALL1", 0x2000037A, "", 10, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG4_MARIO_ROUND1", 0x2000052B, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_MARIO_RETURN1_1", 0x40041A1A, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG4_MARIO_RETURN1", 0x200001A0, "", 90, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_LANPEL_MOVE1L", 0x2000055D, "btl_e_met_move1l_22k", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_LANPEL_MOVE1R", 0x2000055E, "btl_e_met_move1r_22k", 70, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_LANPEL_ESCAPE1", 0x20000364, "", 105, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_SHIP_STARTING1", 0x40042E2E, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_SHIP_ARRIVAL1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_SHIP_STARTING1_2", 0x40042F2F, "", 105, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_SHIP_STARTING1_3", 0x40043030, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_SHIP_STARTING1_4", 0x40043131, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_SHIP_ARRIVAL1_3", 0x40043232, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_SHIP_ARRIVAL1_4", 0x40043333, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_MARCO_MOVE1L", 0x2000020B, "", 70, 64, 0, 1, 0, 0, 0xFAAB},
	{"SFX_STG5_MARCO_MOVE1R", 0x2000020C, "", 70, 64, 0, 1, 0, 0, 0xFAAB},
	{"SFX_STG5_MARCO_LANDING1", 0x2000020A, "", 60, 64, 0, 0, 0, 0, 0xFAAB},
	{"SFX_STG5_MARCO_JUMP1", 0x200004FF, "btl_e_pata_move2_22k", 70, 64, 0, 0, 0, 0, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_KANBU_GLASSES_MOVE1L", 0x200004B0, "", 100, 64, 0, 1, 0, 0, 0xFD55},
	{"SFX_STG5_KANBU_GLASSES_MOVE1R", 0x200004B1, "", 100, 64, 0, 1, 0, 0, 0xFD55},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_SAILER_MOVE1", 0x200003F9, "btl_e_bomb_move2_22k", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_SAILER_FART1", 0x200001AA, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_SUN_MOON_CHANGE1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_SAILER_VS_MARCO1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_BRIDGE_CREAK1", 0x20000076, "", 50, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_COCONUTS_FALL1", 0x20000387, "", 70, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_NPC_FALL1", 0x2000021B, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_ELMOS_PICK1", 0x2000023D, "", 100, 64, 0, 1, 0, 0, 0x800},
	{"SFX_STG5_NPC_ESCAPE1", 0x20000364, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_SHIP_QUAKE1", 0x200001F1, "", 70, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_SHIP_SINK1", 0x200001B5, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_ELMOS_APPEAR1_2", 0x40020909, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_ELMOS_APPEAR2", 0x40020A0A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_ELMOS_MOVE1", 0x200004CD, "btl_e_bubble_move1_22k", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_ELMOS_MOVE1_2", 0x20000559, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_MARCO_ESCAPE1", 0x20000364, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_THUNDERS_THROW1", 0x200000F4, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_THUNDERS_DAMAGED1", 0x20000337, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_THUNDERS_FALL1", 0x20000379, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_THUNDERS_DOWN1", 0x200001B7, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_THUNDERS_HIT1", 0x200001B7, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_OBJ_TREE_SHAKE1", 0x20000375, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_OBJ_TREE_SHAKE2", 0x2000006B, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_OBJ_LEAF_SHAKE1", 0x2000036F, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_JUWEL_SET1", 0x20000173, "", 85, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_ROCK_EYE_SHINE1", 0x200001F4, "", 100, 64, 0, 20, 0, 0, 0x555},
	{"SFX_STG5_ROCK_EYE_SHINE2", 0x200001F4, "", 70, 64, 0, 20, 0, 0, 0xAAA},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_OBJ_BIG_ROCK_HIT1", 0x20000370, "", 115, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_OBJ_BIG_ROCK_JUMP1", 0x20000371, "", 115, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_OBJ_SMALL_ROCK_HIT1", 0x20000372, "", 115, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_OBJ_SMALL_ROCK_JUMP1", 0x20000373, "", 115, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_QUAKE1", 0x200001F1, "", 105, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_OBJ_ROCK_WINDOW1", 0x20000374, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_OBJ_ROCK_BREAK1", 0x200003EE, "", 115, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_JUWEL_FALL1", 0x200000AD, "", 50, 64, 0, 1, 0, 0, 0x800},
	{"SFX_STG5_JUWEL_LANDING1", 0x200003E9, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_STEP_UP1", 0x20000065, "", 70, 64, 0, 20, 0, 0, 0x555},
	{"SFX_STG5_STEP_FALL1", 0x20000065, "", 70, 64, 0, 20, 0, 0, 0x555},
	{"SFX_STG5_STEP_DOWN1", 0x2000016A, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_STEP_FLOAT1", 0x200001AB, "", 70, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_STEP_FLOAT2", 0x200001B6, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_FLOODGATE_SHAKE1", 0x20000169, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_FLOODGATE_UP1", 0x200001AC, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_FLOODGATE_UP2", 0x200003AA, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_BARREL_MOVE1", 0x200001A9, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_TOGE_APPEAR1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_TOGE_IN1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_TOGE_APPEAR2", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_TOGE_IN2", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_MINIGAME_SHIP_SINK1", 0x40044747, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_MINIGAME_SHIP_FLOAT1", 0x20000556, "", 80, 64, 0, 1, 0, 30, 0x555},
	{"SFX_STG5_MINIGAME_WAVE1", 0x2000000F, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_MINIGAME_WAVE2", 0x20000000, "SE6_STG0_SHIP_LANDING1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_BARREL_ON1", 0x200001A9, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_SHIP_ON1", 0x200001A9, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_SHIP_SHAKE1", 0x20000076, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_KINOPIO_MOVE1", 0x20000000, "SE2_AUD_RUN1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_KINOPIO_MOVE2", 0x20000000, "SE4_FLD_P_JUMP1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_KINOPIO_MOVE1_2", 0x400B8787, "", 70, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_BARREL_APPEAR1", 0x200001A9, "SE6_STG0_SHIP_LANDING1", 100, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_SHIP_KOR_SHAKE1", 0x200001B2, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_KOR_APPEAR1", 0x200001AD, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_KOR_APPEAR2", 0x200001AE, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_KOR_GOOUT1", 0x200001AF, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_KOR_APPEAR_LIGHT1", 0x40044848, "", 70, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_SHIP_SHAKE2", 0x200001F1, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_KOR_HEAD_MOVE1", 0x200001B0, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG5_KOR_HEAD_MOVE2", 0x200001B1, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_KINOPIO_MOVE3", 0x400B8E8E, "", 70, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_MARCO_SHAKE1", 0x200001B4, "", 30, 64, 0, 1, 0, 0, 0x555},
	{"SFX_STG5_JUWEL_MOVE1", 0x200004B9, "", 50, 64, 0, 30, 0, 0, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_KOR_SHIP_MOVE1", 0x200001B3, "", 80, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_3RD_SHIP_MOVE1", 0x200001A8, "", 80, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_ELMOS_APPEAR3", 0x40020909, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_CANNON_FIRING1", 0x20000042, "", 120, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_CANNON_SEA1", 0x20000043, "", 120, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_ELMOS_VOICE1", 0x20000044, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_ELMOS_APPEAR1", 0x20000045, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_SHIP_IMPACT1", 0x20000046, "", 85, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_SHIP_MOVE1", 0x20000047, "", 70, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_KORTESS_VOICE1", 0x20000044, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_KORTESS_TURN1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_ELM_VS_3RD1", 0x400B8888, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_KANBU_TURN1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_SHIP_KOL_MOVE1", 0x20000076, "", 95, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_SHIP_KOL_MAST1", 0x2000052F, "", 95, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG5_WAVE1", 0x2000000F, "", 95, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_KOBURON_OUT1", 0x200004F8, "btl_e_pakkun_move1_22k", 100, 64, 0, 1, 0, 0, 0x555},
	{"SFX_STG5_KOBURON_IN1", 0x200004F9, "btl_e_pakkun_move2_22k", 70, 64, 0, 1, 0, 0, 0x555},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_ROSE_MOVE1L", 0x20000197, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_ROSE_MOVE1R", 0x20000198, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_TRAIN_BELL1", 0x200001C8, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_TRAIN_MOVE1", 0x40010000, "", 120, 64, 0, 1, 0, 30, 0x0},
	{"SFX_STG6_TRAIN_MOVE1_2", 0x40010202, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_TRAIN_MOVE1_2_2", 0x40010606, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_TRAIN_MOVE1_2_3", 0x40010303, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_TRAIN_MOVE1_3", 0x40010303, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_TRAIN_MOVE2", 0x40010101, "", 120, 64, 0, 1, 0, 10, 0x0},
	{"SFX_STG6_TRAIN_MOVE2_2", 0x40010404, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_TRAIN_MOVE2_2_2", 0x40010707, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_TRAIN_MOVE2_3", 0x40010505, "", 120, 64, 0, 1, 0, 30, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_TRAIN_MOVE3", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_TRAIN_CHIME1", 0x40040C0C, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_TRAIN_HANDLE_MOVE1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_DRIVER_SIGN1", 0x40041D1D, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_GHOST_SHAKE1", 0x200000DE, "", 100, 64, 0, 1, 0, 0, 0xF800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_DOUBTFUL_POINT1", 0x200001BB, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_GHOST_APPEAR1", 0x20000045, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_GHOST_PASSAWAY1", 0x200001BE, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_GHOST_RETURN1", 0x200001BE, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_TRON_APPEAR1", 0x20000531, "", 100, 64, 0, 1, 0, 20, 0x0},
	{"SFX_STG6_TRON_JUMP1", 0x200004FF, "btl_e_pata_move2_22k", 65, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_TRON_DAMAGED1", 0x20000337, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_LANPEL_CHANGE1", 0x200001A0, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_LANPEL_JUMP1", 0x200004FF, "btl_e_pata_move2_22k", 65, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_PERA_KAI_DOWN1", 0x40098080, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_PERA_KAI_DOWN2", 0x2000046B, "", 110, 64, 0, 25, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_BOX_OPEN1", 0x2000023D, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_OBJ_SWITCH_ON1", 0x200003EF, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_OBJ_SWITCH_SMALL1", 0x20000443, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_OBJ_SWITCH_JUMP1", 0x20000516, "", 85, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_PERA_KAI_OPEN1", 0x200001C4, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_PERA_KAI_OPEN1_2", 0x2000023C, "", 110, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_PERA_KAI_OPEN2", 0x2000046B, "", 110, 64, 0, 25, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_ELEVATOR_KEY1", 0x20000184, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_ELEVATOR_BELL1", 0x20000140, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_ELEVATOR_OPEN1", 0x200001BD, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_ELEVATOR_CLOSE1", 0x200001BD, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_ELEVATOR_SHAKE1", 0x20000376, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_ELEVATOR_MOVE1", 0x200001BC, "", 100, 64, 0, 25, 0, 45, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_MOAMOA_VOICE1", 0x200001C0, "", 60, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_MOAMOA_VOICE2", 0x40041E1E, "", 60, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_MOAMOA_DAMAGED1", 0x20000166, "", 90, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_MOA_TOGETHER_MOVE1", 0x40041F1F, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_MOA_TOGETHER_MOVE2", 0x200001C2, "", 100, 64, 0, 30, 0, 0, 0xFAAB},
	{"SFX_STG6_MOAMOA_TOGETHER1", 0x20000000, "SE2_EVT_6_MOAMOA_G_TOGETHER1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_MOAMOA_G_LEG1", 0x200001C1, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG6_MOAMOA_G_LEG2", 0x200001C2, "", 110, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG6_MOAMOA_G_LEG3", 0x200001C3, "", 110, 64, 0, 60, 0, 0, 0x0},
	{"SFX_STG6_MOAMOA_G_LEG4", 0x200001BF, "", 110, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG6_MOAMOA_G_APPEAR1", 0x20000000, "SE2_EVT_3_GANSU_BIG1", 0, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG6_MOAMOA_G_APPEAR2", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_MOAMOA_G_MOVE1", 0x200001BF, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_MOAMOA_G_ESCAPE1", 0x40042020, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_SWITCH_MOVE1", 0x200001C7, "", 100, 64, 0, 25, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_ORIGAMI_APPEAR1", 0x40098181, "SE5_EVT_6_ORIGAMI_KAIDAN_APPEAR2", 100, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_ORIGAMI_APPEAR2", 0x2000023C, "", 70, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG6_OBJ_SWITCH_APPEAR1", 0x200001C6, "", 127, 64, 0, 25, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_BRIDGE_MOVE1", 0x200001B8, "", 110, 64, 0, 40, 0, 0, 0x0},
	{"SFX_STG6_BRIDGE_MOVE2", 0x200001B9, "", 110, 64, 0, 40, 0, 0, 0x0},
	{"SFX_STG6_CROSSING_OPEN1", 0x200001BA, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_MAP_CHANGE1", 0x20000540, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG6_MAP_CHANGE2", 0x20000541, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_PIK_ELEVATOR_DOOR1", 0x200001BA, "", 70, 64, 0, 1, 0, 0, 0xFAAB},
	{"SFX_STG6_PIK_ELEVATOR_MOVE1", 0x200001BC, "", 70, 64, 0, 1, 0, 15, 0xFD55},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_STARATONE_LIGHT1", 0x20000209, "", 70, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_CURTAIN_UP1", 0x2000014F, "", 50, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_WHITE_DAMAGED1", 0x2000040F, "btl_e_damaged_hard1_22k", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_WHITE_NICE1", 0x20000489, "", 110, 64, 0, 1, 0, 8, 0x0},
	{"SFX_STG7_WHITE_GOOD1", 0x20000484, "", 110, 64, 0, 1, 0, 8, 0x0},
	{"SFX_STG7_WHITE_ROUND1", 0x200003FE, "btl_e_boome_att3_22k", 90, 64, 0, 25, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_FLOOR_QUAKE1", 0x200001F1, "", 90, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_FLOOR_QUAKE2", 0x200001D7, "", 90, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_FLOOR_OPEN1", 0x200001D5, "", 70, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_MARIO_FALL1", 0x2000037A, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_FLOOR_OPEN2", 0x200001D6, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_FLOOR_DOWN1", 0x2000016B, "", 110, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_CANNON_SYS_ACTIVE1", 0x200001CC, "", 100, 64, 0, 50, 0, 0, 0x0},
	{"SFX_STG7_CANNON_SYS_ACTIVE2", 0x200001CB, "", 60, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_SHUTTER_OPEN1", 0x200001DA, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_CANNON_SIREN1", 0x20000181, "", 60, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_CANNON_UP1", 0x200001CD, "", 90, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_CANNON_STOP1", 0x200001CA, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_CANNON_MOVE1", 0x200001CD, "", 90, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_CANNON_STOP2", 0x200001D7, "", 90, 64, 0, 20, 0, 0, 0x555},
	{"SFX_STG7_HATCH_OPEN1", 0x200001D8, "", 80, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_HATCH_SHUT1", 0x200001D8, "", 80, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_CANNON_DISCHARGE1", 0x200001C9, "", 127, 64, 0, 20, 0, 0, 0xFAAB},
	{"SFX_STG7_BOMB_TOGETHER1", 0x40098282, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_BOMB_TOGETHER1_2", 0x40098282, "", 50, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_BOMB_TOGETHER2", 0x200003F8, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_BOMB_FALL1", 0x20000377, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_MARIO_FLY1", 0x200000AD, "", 55, 64, 0, 40, 0, 0, 0x1000},
	{"SFX_STG7_MARIO_LIGHT1", 0x200001F4, "", 100, 64, 0, 70, 0, 0, 0x555},
	{"SFX_STG7_MARIO_MOON_FALL1", 0x20000177, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_MARIO_SHAKE1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_ELECTRIC_SHOCK1", 0x200002F5, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_ELEVATOR_KEY1", 0x200001D3, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_ELEVATOR_ACCESS1", 0x200001C5, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_ELEVATOR_APPEAR1", 0x200001D0, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_ELEVATOR_OPEN1", 0x200001D1, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_ELEVATOR_OPEN2", 0x200001D2, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_ELEVATOR_CLOSE1", 0x200001D1, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_ELEVATOR_CLOSE2", 0x200001D2, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_ELEVATOR_MOVE1", 0x200001D4, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_DOOR_UNLOCK1", 0x200001D3, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_DOOR_LAMP1", 0x2000019E, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_CRANE_BUTTON_PUSH1", 0x20000173, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_CRANE_ACTIVE1", 0x200001CF, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_CRANE_MOVE1", 0x2000025F, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_CRANE_SIREN1", 0x20000181, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_SWITCH_WARP1", 0x20000173, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_WARP_LIGHT1", 0x200001E3, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_WARP1", 0x200001DF, "", 100, 64, 0, 1, 0, 15, 0x0},
	{"SFX_STG7_WARP2", 0x200001E1, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_WARP1_2", 0x200001E0, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_LOCKER_OPEN1", 0x2000019A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_LOCKER_CLOSE1", 0x20000199, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_CURTAIN_OPEN1", 0x20000385, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_CURTAIN_CLOSE1", 0x20000385, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_DSN_AWAKE1", 0x200001CC, "", 100, 64, 0, 50, 0, 0, 0x555},
	{"SFX_STG7_DSN_ROUND1", 0x200003A9, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_QUAKE1", 0x20000168, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_QUAKE2", 0x2000016A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_GOOD_SOUND1", 0x2000016E, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_BAD_SOUND1", 0x2000016C, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_GOOD_SOUND2", 0x2000016F, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_BAD_SOUND2", 0x2000016D, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_QUIZ_START1", 0x20000170, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_BOSS_ACTIVE1", 0x20000107, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_BOSS_LANDING1", 0x20000103, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_BOSS_MOVE1L", 0x20000106, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_BOSS_MOVE1R", 0x20000106, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_BOSS_JUMP1", 0x20000100, "btl_e_elmos_charge1_22k", 90, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_3RD_EXPLSION1", 0x200002F5, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_3RD_EXPLSION2", 0x40044A4A, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_3RD_EXPLSION3", 0x200001C9, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG7_3RD_FLY1", 0x200000AD, "", 50, 64, 0, 70, 0, 0, 0x1000},
	{"SFX_STG7_3RD_LIGHT1", 0x200001F4, "", 110, 64, 0, 70, 0, 0, 0x555},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_MOVEFLOOR_MOVE1", 0x200001D9, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_CONVEYER_ACTIVE1", 0x200001CE, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_CONVEYER_MOVE1", 0x20000263, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_BOSS_EXPLOSION1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_ALART_WARNING1", 0x200001DB, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_TEC_ACTIVE1", 0x200001DC, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_TEC_ACTIVE2", 0x2000001A, "evt_p_tec_active1_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_TEC_SANDSTORM1", 0x200001DD, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_TEC_SHUTDOWN1", 0x200001DE, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_TEC_LIGHT1", 0x200001F4, "", 100, 64, 0, 70, 0, 0, 0x0},
	{"SFX_STG7_TEC_LIGHT2", 0x40044A4A, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_TOGE_APPEAR1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_TOGE_IN1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_KARON_APPEAR1", 0x2000052C, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_KARON_DOWN1", 0x200000C6, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_KARON_LANDING1", 0x200004F5, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_KARON_MOVE1", 0x40041212, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_KARON_FLY1", 0x20000166, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_KARON_ESCAPE1", 0x20000000, "SE2_EVT_8_KARON_ESCAPE1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_DOOR_NG_OPEN1", 0x200001EC, "", 100, 64, 0, 1, 0, 0, 0x555},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_GNB_ROAR1", 0x2000004B, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG8_GNB_STEP1", 0x20000163, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_GNB_DOWN1", 0x200000E0, "", 127, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_GNB_DOWN2", 0x2000016B, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_GNB_DOWN3", 0x20000162, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_GNB_THROAT_MOVE1", 0x2000052D, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_GMB_MOUTH_MOVE1", 0x2000052E, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_GMB_MOUTH_OPEN1", 0x200000DB, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_TBOX_JUMP1", 0x200004D2, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_TBOX_LANDING1", 0x2000038A, "", 100, 64, 0, 1, 0, 0, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_QUAKE2", 0x20000168, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG8_PORT_APPEAR1", 0x2000046B, "", 110, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG8_PORT_APPEAR2", 0x2000046B, "", 110, 64, 0, 20, 0, 0, 0xF800},
	{"SFX_STG8_KEYSTAND_APPEAR1", 0x200001EF, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG8_KEYSTAND_APPEAR2", 0x200001F0, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG8_TBOX_APPEAR1", 0x200001EF, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG8_TBOX_APPEAR2", 0x200001F0, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG8_STAR_SET1", 0x20000173, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_SWITCH_BIKKURI_APPEAR1", 0x200001A0, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_KEY_SET1", 0x200003EC, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG8_KEYSTAND_SHAKE1", 0x20000168, "", 100, 64, 0, 20, 0, 0, 0x800},
	{"SFX_STG8_KEYSTAND_ROUND1", 0x200003A9, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG8_CELESTIAL_SHAKE1", 0x20000168, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG8_CELESTIAL_MOVE1", 0x40042929, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG8_CELESTIAL_MOVE2", 0x200001EB, "", 85, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_MAP_MOVE1", 0x2000025B, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_RANPEL_TRANSFORM1", 0x200001A0, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_WITCH_PIYO1", 0x200001F6, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_FLOOR_MOVE1", 0x20000052, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_TBOX_APPEAR3", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_SWITCH_APPEAR1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_FLOOR_ROUND_START1", 0x20000263, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG8_STAR_LIGHTUP1", 0x200001F4, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG8_STAR_LIGHTUP2", 0x200001F5, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_QUAKE3", 0x20000168, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_PEACH_APPEAR1", 0x40042121, "", 90, 64, 0, 15, 0, 0, 0x0},
	{"SFX_STG8_SHURYO_THUNDER1", 0x20000250, "", 110, 64, 0, 25, 0, 0, 0x0},
	{"SFX_STG8_SHURYO_THUNDER2", 0x20000314, "", 60, 64, 0, 1, 0, 0, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_KOOPA_QUAKE1", 0x200001E9, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG8_CEILING_BREAK1", 0x200001E8, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_CEILING_BREAK2", 0x200001EA, "", 127, 64, 0, 1, 0, 30, 0x0},
	{"SFX_STG8_KOOPA_FALL1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_KOOPA_STRIKE1", 0x20000552, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_KOOPA_STRIKE2", 0x20000260, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_KOOPA_PIYO1", 0x200001F6, "", 35, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_SHURYO_STUFF_PISECE1", 0x20000428, "btl_e_kameku_magic3_22k", 90, 64, 0, 0, 0, 10, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_CANDLE_OFF1", 0x200001E6, "", 80, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG8_CANDLE_DARK_ON1", 0x200001E5, "", 80, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG8_GRAVE_LIGHT1", 0x200001ED, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_GRAVE_SHAKE1", 0x200001EE, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_GRAVE_MOVE1", 0x200003A9, "", 110, 64, 0, 1, 0, 0, 0xF800},
	{"SFX_STG8_GRAVE_DOWN1", 0x2000025B, "", 127, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_GRAVE_LIGHT2", 0x40042222, "", 105, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_QUEEN_APPEAR1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_QUEEN_APPEAR2", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_WORLD_DARK1", 0x40042323, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_QUEEN_MOVE1", 0x40042525, "", 105, 64, 0, 1, 0, 10, 0x0},
	{"SFX_STG8_QUEEN_MOVE2", 0x40042626, "", 105, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_SHURYO_HEAD_LEAPED1", 0x2000003D, "", 100, 64, 0, 1, 0, 0, 0xF556},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_THUNDER1", 0x20000250, "", 100, 64, 0, 45, 0, 0, 0xF800},
	{"SFX_STG8_LIGHT1", 0x40042424, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_LIGHT2", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_QUAKE1", 0x200001F1, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_STARSTONE_SHINE1", 0x200001F3, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_STG8_STARSTONE_ROUND1", 0x40044D4D, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_STARSTONE_ROUND2", 0x40044E4E, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_STARSTONE_EFFECT1", 0x40044F4F, "", 97, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_QUEEN_SHINE1", 0x20000172, "", 55, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG8_QUEEN_SHINE2", 0x40045050, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_QUEEN_SHINE2_2", 0x40046D6D, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_QUEEN_SHINE2_3", 0x40046E6E, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_QUEEN_SHINE3", 0x40045151, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_QUEEN_SHINE4", 0x200001F2, "", 127, 64, 0, 60, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_PEACH_SHINE1", 0x40045252, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_MARIO_SHINE1", 0x40045353, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_QUEEN_DIE1", 0x40042727, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG8_QUEEN_DIE2", 0x20000250, "", 90, 64, 0, 60, 0, 0, 0x800},
	{"SFX_STG8_QUEEN_DIE3", 0x40042828, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_CANDLE_DARK_OFF1", 0x200001E4, "", 85, 64, 0, 30, 0, 0, 0x0},
	{"SFX_STG8_CANDLE_ON1", 0x200001E7, "", 85, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG8_KURI_TBOX_LANDING1", 0x2000038A, "", 85, 64, 0, 1, 0, 0, 0xF556},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_MOVE1L", 0x20000215, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_MOVE1R", 0x20000216, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_LANDING1", 0x20000214, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_3RD_MOVE1L", 0x200004B0, "btl_e_3rd_walkl1_22k", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_3RD_MOVE1R", 0x200004B1, "btl_e_3rd_walkr1_22k", 85, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_KANBU_MOVE1L", 0x200004B0, "", 100, 64, 0, 1, 0, 0, 0xFD55},
	{"SFX_PEACH_KANBU_MOVE1R", 0x200004B1, "", 100, 64, 0, 1, 0, 0, 0xFD55},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_SHURYO_MOVE1L", 0x200004B0, "btl_e_3rd_walkl1_22k", 85, 64, 0, 1, 0, 0, 0xFAAB},
	{"SFX_PEACH_SHURYO_MOVE1R", 0x200004B1, "btl_e_3rd_walkr1_22k", 85, 64, 0, 1, 0, 0, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_3RD_BATTEN1", 0x20000210, "", 70, 64, 0, 1, 0, 5, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_PIC_BIRIBIRI1", 0x200002F5, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_PIC_CHANGE1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_PIC_CHANGE2", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_SCREEN_OPEN1", 0x20000070, "", 100, 64, 0, 60, 0, 0, 0xFD55},
	{"SFX_PEACH_SCREEN_SHUT1", 0x20000071, "", 100, 64, 0, 60, 0, 0, 0xFD55},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_SHOWER1", 0x20000018, "evt_p_shower1_16k", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_SHOWER2", 0x20000017, "evt_p_shower2_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_HANG_CLOTH1", 0x20000019, "evt_p_hang_cloth1_16k", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_TEC_ACTIVE1", 0x2000001A, "evt_p_tec_active1_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_KEYBOARD1", 0x20000213, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_MAIL_SOUSIN1", 0x2000001B, "evt_p_mail_send1_16k", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_MAIL_SOUSIN2", 0x2000001C, "evt_p_mail_send2_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_DANCE_APPEAR1", 0x2000001D, "evt_p_dance_appear1_16k", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_DANCE_TURN1", 0x2000001E, "evt_p_dance_turn1_16k", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_DANCE_OK1", 0x2000001F, "evt_p_dance_ok1_16k", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_DANCE_NG1", 0x20000020, "evt_p_dance_ng1_16k", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_3RD_P_MOVE1L", 0x200004B0, "btl_e_3rd_walkl1_22k", 85, 64, 0, 1, 0, 0, 0x2AB},
	{"SFX_PEACH_3RD_P_MOVE1R", 0x200004B1, "btl_e_3rd_walkr1_22k", 85, 64, 0, 1, 0, 0, 0x2AB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_ELEVATOR_KEY1", 0x20000000, "SE2_EVT_7_ELEVATOR_KEY1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_ELEVATOR_ACCESS1", 0x200001C5, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_ELEVATOR_APPEAR1", 0x200001D0, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_ELEVATOR_OPEN1", 0x200001D1, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_ELEVATOR_OPEN2", 0x200001D2, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_ELEVATOR_CLOSE1", 0x200001D1, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_ELEVATOR_CLOSE2", 0x200001D2, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_ELEVATOR_MOVE1", 0x200001D4, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_LOCKER_OPEN1", 0x2000019A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_LOCKER_CLOSE1", 0x20000199, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_CURTAIN_OPEN1", 0x20000385, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_CURTAIN_CLOSE1", 0x20000385, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_CHANGE_CLOTH1", 0x20000021, "evt_p_change_cloth1_16k", 70, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_MACHINE_OPEN1", 0x200001D5, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_MACHINE_SWITCH1", 0x20000173, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_MACHINE_SIREN1", 0x20000217, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_MACHINE_BEAKER1", 0x200001D0, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_MACHINE_MOVE1", 0x20000263, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_MACHINE_MEDICINE1", 0x20000022, "evt_p_medicine1_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_MACHINE_HOT1", 0x20000028, "evt_p_medicine_hot1_16k", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_TOUMEI1", 0x200001A0, "", 127, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_TOUMEI2", 0x200001A0, "", 127, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_DRINK_MEDICINE1", 0x200004A6, "btl_e_3rd_drink1_22k", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_MINIMINI1", 0x200002FF, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_DEKADEKA1", 0x200002F8, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_BAREFOOT_MOVE1L", 0x20000211, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_BAREFOOT_MOVE1R", 0x20000212, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_BAREFOOT_LANDING1", 0x20000212, "", 50, 64, 0, 1, 0, 0, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_DISC_SET1", 0x20000027, "evt_p_disc_set1_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_MONITOR_ON1", 0x20000023, "evt_p_monitor_on1_16k", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PEACH_MONITOR_OFF1", 0x20000023, "evt_p_monitor_off1_16k", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PEACH_TEC_BUTTON1", 0x20000173, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_MOVE1", 0x20000049, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_MOVE2", 0x2000004A, "", 75, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_ENEMY_DIE1", 0x2000038D, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_LANDING1", 0x2000004C, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_JUMP1", 0x20000024, "evt_k_jump1_16k", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_HIP_DROP1", 0x20000025, "evt_k_hipdrop1_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_HIP_DROP2", 0x20000026, "evt_k_hipdrop2_16k", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_FIRE_BREATH1", 0x20000202, "SE2_BTL_STG_FIRE1", 110, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_BIG1", 0x2000024C, "SE3_EVT_K_BIG1", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_SMALL1", 0x200003A1, "SE3_EVT_K_SMALL1", 110, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_WATER_UP1", 0x2000002A, "evt_k_water_up1_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_WATER_FALL1", 0x20000029, "evt_k_water_fall1_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_SWIM1", 0x2000002B, "evt_k_swim1_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_POST_JUMP1", 0x2000002C, "evt_k_post_jump1_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_POST_SLIDE1", 0x2000002D, "evt_k_post_slide1_16k", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_STRUGGLE1", 0x2000002E, "evt_k_struggle1_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_GOALIN1", 0x20000150, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_FIREWORKS1", 0x2000013D, "", 100, 64, 0, 10, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_3RD_WATER_FALL1", 0x2000021B, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_GESSO_MOVE1", 0x2000002F, "evt_k_gesso_move1_16k", 90, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_DOKAN_BREAK1", 0x20000378, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_POST_THROW1", 0x20000208, "", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_MEAT_APPEAR1", 0x200003DF, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_COUNTER1", 0x20000030, "evt_k_counter1_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_ANGRY1", 0x2000016A, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_ANGRY2", 0x20000049, "", 90, 64, 0, 1, 0, 0, 0xF800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_WALL_BREAK1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_AIRSHIP_MOVE1", 0x20000031, "evt_k_airship_move1_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_KAMEBABA_MOVE1L", 0x20000206, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_KAMEBABA_MOVE1R", 0x20000207, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_KAMEBABA_FLY1", 0x20000032, "evt_k_kamebaba_fly1_16k", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_KAMEBABA_JUMP1", 0x20000204, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_KAMEBABA_LANDING1", 0x20000205, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_NOKO_ESCAPE1", 0x20000364, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_FIND_PEACH1", 0x200003D2, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_NOKO_JUMP1", 0x200004F1, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_NOKO_POSTER1", 0x2000038B, "", 100, 64, 0, 1, 0, 15, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_NOKO_MOVE1L", 0x200004F4, "btl_e_noko_move3l_22k", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_NOKO_MOVE1R", 0x200004F5, "btl_e_noko_move3r_22k", 90, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_PUNI_MOVE2_1", 0x2000017B, "", 30, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_PUNI_MOVE2_2", 0x2000017C, "", 30, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_PUNI_MOVE2_3", 0x2000017D, "", 30, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_PUNI_MOVE2_4", 0x2000017E, "", 30, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_PUNI_MOVE2_5", 0x2000017F, "", 30, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_PUNI_MOVE2_6", 0x20000180, "", 30, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_PUKU_ESCAPE1", 0x20000364, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_AIRSHIP_BIG1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_AIRSHIP_BREAK1", 0x20000314, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_AIRSHIP_FALL1", 0x20000200, "", 50, 64, 0, 60, 0, 0, 0x0},
	{"SFX_KUPPA_AIRSHIP_DOWN_SEA1", 0x20000201, "SE6_STG0_SHIP_LANDING1", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_KAMEBABA_ROUND1", 0x200001F6, "", 35, 64, 0, 1, 0, 0, 0x555},
	{"SFX_KUPPA_KAMEBABA_FALL1", 0x2000041A, "btl_e_fall1_22k", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_KAMEBABA_DOWN1", 0x20000203, "", 70, 64, 0, 1, 0, 5, 0x0},
	{"SFX_KUPPA_KAMEBABA_KOGE1", 0x20000314, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_WATER_OUT1", 0x20000201, "SE6_STG0_SHIP_LANDING1", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_WATER_IN1", 0x200000B9, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_3RD_GRASS_SHAKE1", 0x200003B6, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_3RD_SHUGOU1", 0x2000050F, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_3RD_MOVE1", 0x200004B0, "", 80, 64, 0, 20, 0, 0, 0x0},
	{"SFX_KUPPA_3RD_MOVE2", 0x200004B1, "", 80, 64, 0, 20, 0, 0, 0x0},
	{"SFX_KUPPA_KAMEBABA_SHUGOU1", 0x2000050F, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_TOGENOKO_MOVE1", 0x20000514, "", 80, 64, 0, 20, 0, 0, 0x0},
	{"SFX_KUPPA_TOGENOKO_MOVE2", 0x20000515, "", 80, 64, 0, 20, 0, 0, 0x0},
	{"SFX_KUPPA_TAIGUN_MOVE1", 0x40044949, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_KUPPA_ITIKORO_APPEAR1", 0x200004A9, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_ITIKORO_SHINE1", 0x2000032E, "", 70, 64, 0, 50, 0, 0, 0x0},
	{"SFX_KUPPA_ITIKORO_FLY1", 0x200004AF, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_ITIKORO_HIT1", 0x20000142, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_ITIKORO_JUMP1", 0x200003E9, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_ITIKORO_BIRIBIRI1", 0x200002F5, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_ITIKORO_LIGHT1", 0x40044A4A, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_ITIKORO_EXPLOSION1", 0x200001C9, "", 127, 64, 0, 60, 0, 0, 0xF800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_PEAPLE_ESCAPE1", 0x20000364, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_STONE_SHINE1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_BRED_APPEAR1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_BRED_TACKLE1", 0x200000BD, "", 120, 64, 0, 0, 0, 0, 0xF800},
	{"SFX_KUPPA_BRED_TACKLE_HIT1", 0x20000552, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_STONE_FALL1", 0x200000AE, "", 60, 64, 0, 1, 0, 0, 0x800},
	{"SFX_KUPPA_STONE_HIT1", 0x20000142, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_STONE_BREAK1", 0x20000141, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_KONARIKIN_ESCAPE1", 0x20000364, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_KUPPA_STONE_DOWN1", 0x200003E9, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_KUPPA_PKURI_GLASS_BREAK1", 0x200004A5, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_KUPPA_PKURI_LANDING1", 0x20000435, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_EPILOGUE_KOR_SHIP_MOVE1", 0x200001B3, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_EPILOGUE_3RD_BATTEN1", 0x20000210, "", 65, 64, 0, 1, 0, 5, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_EPILOGUE_KINOJII_KNOCK1", 0x200003A0, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_EPILOGUE_BOOK_SHUT1", 0x20000161, "", 95, 64, 0, 60, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG0_AMB_WATERDROP1", 0x2000007C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG0_AMB_WATERDROP2", 0x2000007C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_AMB_WATERDROP1", 0x20000000, "SE2_AMB_WATERDROP1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG1_AMB_KANBAN1", 0x20000009, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_ENV_KANBAN1", 0x20000000, "SE3_AMB_BILLBOARD1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_BOMB1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_AMB_OBJ_SCREEN1", 0x20000070, "", 60, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG3_AMB_OBJ_SCREEN2", 0x20000071, "", 60, 64, 0, 20, 0, 0, 0x0},
	{"SFX_STG3_AMB_KANBAN1", 0x20000009, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG3_AMB_WATERDROP1", 0x2000007C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_AMB_OBJ_SCREEN1", 0x20000000, "SE2_AMB_OBJ_SCREEN1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_AMB_OBJ_SCREEN2", 0x20000000, "SE2_AMB_OBJ_SCREEN2", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_AMB_CAMERA1", 0x20000000, "SE2_AMB_CAMERA1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG4_AMB_WATERDROP1", 0x2000007C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG5_AMB_POT1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_AMB_POT2", 0x20000000, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_AMB_WATERDROP2", 0x2000007C, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG6_AMB_WEATHERCOCK1", 0x20000081, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG5_AMB_WATERDROP1", 0x2000007C, "", 60, 64, 0, 1, 0, 0, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG7_AMB_WINDVANE1", 0x20000081, "", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_AMB_CEILING_MOVE1", 0x20000009, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_STG7_AMB_GEAR1", 0x20000064, "", 60, 64, 0, 15, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_GAME_TELESA_MOVE1", 0x2000045B, "btl_e_telesa_toumei1_22k.aif", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_GAME_TELESA_MOVE2", 0x2000045A, "btl_e_telesa_toumei1_22k.aif", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_GAME_MONTE_COUNT1", 0x2000047A, "", 50, 64, 0, 1, 0, 0, 0xFD55},
	{"SFX_EVT_GAME_MONTE_LEAPED1", 0x20000247, "", 55, 64, 0, 1, 0, 0, 0xFD55},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_GAME_SLOT_ROUND1", 0x40097979, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_GAME_SLOT_STOP1", 0x20000173, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_GAME_SLOT_SUCCESS1", 0x40097A7A, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_GAME_COUNT1", 0x2000035E, "", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_GAME_START1", 0x2000035D, "", 95, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_GAME_AUDIENCE1", 0x2000004E, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_GAME_AUDIENCE2", 0x2000004F, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_GAME_RESULT_COUNT1", 0x20000030, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_GAME_BOAT_WATER1", 0x20000062, "", 40, 64, 0, 1, 0, 0, 0xFD55},
	{"SFX_EVT_GAME_BOAT_BARREL1", 0x2000039E, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_GAME_BOAT_UZUSIO1", 0x40046565, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_GAME_BOAT_UZUSIO2", 0x40044747, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_GAME_SLIT_NPC_MOVE1L", 0x2000020B, "", 50, 64, 0, 1, 100, 0, 0x0},
	{"SFX_EVT_GAME_SLIT_NPC_MOVE1R", 0x2000020C, "", 50, 64, 0, 1, 100, 0, 0x0},
	{"SFX_EVT_GAME_SLIT_NPC_JUMP1", 0x200004FF, "btl_e_pata_move2_22k", 50, 64, 0, 1, 90, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_LOTTERY_BOMB_BURST1", 0x20000126, "btl_e_heavyb_att3_22k", 120, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_LOTTERY_BOMB_BURST2", 0x2000037B, "", 110, 64, 0, 1, 0, 0, 0xF800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_FORTUNE_CARD_MOVE1", 0x40040E0E, "", 80, 64, 0, 1, 0, 25, 0x0},
	{"SFX_EVT_FORTUNE_CARD_MOVE2", 0x200001F8, "", 120, 64, 0, 1, 0, 20, 0x0},
	{"SFX_EVT_FORTUNE_CARD_MOVE3", 0x40040F0F, "", 80, 64, 0, 1, 0, 20, 0x0},
	{"SFX_EVT_FORTUNE_CARD_MOVE4", 0x40041010, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_FORTUNE_CARD_MOVE5", 0x40041111, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_INCANT_LIGHT1", 0x200001FF, "", 90, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_INCANT_CUBE1", 0x200001F9, "", 100, 64, 0, 60, 0, 15, 0x0},
	{"SFX_EVT_INCANT_CUBE2", 0x200001FA, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_INCANT_CUBE3", 0x200001FB, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_INCANT_CUBE4", 0x40046F6F, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_INCANT_CUBE4_2", 0x200001FC, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_INCANT_CUBE5", 0x200001FD, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_INCANT_CUBE6", 0x2000023D, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_INCANT_CUBE7", 0x200001FE, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_FLD_INCANT_CUBE1", 0x200001F9, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_FLD_INCANT_CUBE2", 0x200001FA, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_FLD_INCANT_CUBE3", 0x40047070, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_FLD_INCANT_CUBE4", 0x2000023D, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_FLD_INCANT_CUBE5", 0x200001FE, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_BTL_INCANT_CUBE1", 0x200001F9, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_BTL_INCANT_CUBE2", 0x200001FA, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_BTL_INCANT_CUBE3", 0x40047070, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_BTL_INCANT_CUBE4", 0x2000023D, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_BTL_INCANT_CUBE5", 0x200001FE, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_POWERUP1", 0x20000000, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_EVT_POWERUDOWN1", 0x20000000, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_REQUEST_CHURINA_KISS1", 0x2000033A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_REQUEST_DAMAGED1", 0x20000411, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_COOK_COOKING1", 0x2000007E, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_COOK_COOKING2", 0x40077474, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_EVT_COOK_COOKING3", 0x200001F7, "", 125, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_COOK_FIRE1", 0x20000314, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_COOK_LIGHT1", 0x200000CE, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_COOK_EXPLOSION1", 0x20000126, "SE4_BTL_P_THUNDERS_ATT3", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_IN1", 0x20000155, "SE2_PARTY_IN2", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_OUT1", 0x20000360, "SE2_PARTY_OUT2", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_CHANGE1", 0x20000000, "SE2_PARTY_CHANGE1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_MOVE1L", 0x200003BF, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PARTY_MOVE1R", 0x200003C0, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PARTY_MOVE2", 0x200003BE, "", 30, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PARTY_JUMP1", 0x200003BA, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PARTY_LANDING1", 0x200003BB, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_PNK_KOURA1", 0x20000341, "SE2_KAMEKI_FLY2", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PARTY_PNK_KOURA_BACK1", 0x20000251, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PARTY_PNK_KOURA_WALL1", 0x200003BC, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PARTY_PNK_LEAP1", 0x200003BD, "SE2_KAMEKI_LEAP1", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PARTY_PNK_HOLD1", 0x20000340, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_CLAUD_WIND1", 0x20000334, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX__PARTY_CLAUD_WIND2", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX__PARTY_CLAUD_WIND3", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_VIVIAN_SHADOW1_1", 0x20000244, "", 90, 64, 0, 20, 0, 0, 0x0},
	{"SFX_PARTY_VIVIAN_SHADOW1_2", 0x20000243, "", 100, 64, 0, 10, 0, 0, 0x0},
	{"SFX_PARTY_VIVIAN_SHADOW1", 0x20000242, "SE4_BTL_P_VIVIAN_ATT1", 95, 64, 0, 10, 0, 0, 0x0},
	{"SFX_PARTY_VIVIAN_SHADOW2", 0x2000034D, "", 90, 64, 0, 10, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_PBM_THROW1", 0x200004FF, "", 90, 64, 0, 0, 0, 0, 0xFAAB},
	{"SFX_PARTY_PBM_DOWN1", 0x200001B7, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PARTY_PBM_BOMB1", 0x20000126, "SE4_BTL_P_THUNDERS_ATT3", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PARTY_PBM_FUSE1", 0x2000034A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PARTY_PBM_FUSE2", 0x20000127, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_YOSH_RIDE1", 0x20000246, "SE4_FLD_P_YOSHI_RIDE1", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PARTY_YOSH_MOVE1", 0x200003C3, "", 85, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PARTY_YOSH_JUMP1", 0x20000245, "SE4_FLD_P_YOSHI_JUMP1", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_PARTY_YOSH_EGG1", 0x200003C1, "", 50, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_CHURINA_SMELL1", 0x20000330, "", 85, 64, 0, 1, 0, 0, 0x555},
	{"SFX_PARTY_CHURINA_SMELL_JUMP1", 0x200003BA, "", 70, 64, 0, 1, 0, 0, 0x555},
	{"SFX_PARTY_CHURINA_KISS1", 0x200003B9, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_FLD_MARIO_DAMAGED1", 0x20000316, "", 100, 64, 1, 0, 0, 0, 0x0},
	{"SFX_BTL_FLD_ENEMY_DAMAGED1", 0x20000411, "btl_e_damaged1_normal1_22k", 100, 64, 1, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_POS_CHANGE1", 0x2000025C, "SE4_BTL_S_CHANGE_POSITION1", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_UP_SCREEN1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_BATTLE_MOVE1L", 0x20000399, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_BATTLE_MOVE1R", 0x2000039A, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_BATTLE_JUMP1", 0x2000021E, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_JUMP_LANDING1", 0x20000387, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_HOLD1", 0x2000012B, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_HOLD2", 0x2000012C, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_HOLD3", 0x2000012D, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_SWING1", 0x20000382, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_SWING2", 0x20000383, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_SWING3", 0x20000384, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_MARIO_EAT1", 0x20000000, "SE2_FLD_M_EAT1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ATTACK_MISS1", 0x20000310, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ATTACK_MISS2", 0x20000311, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ATTACK_MISS3", 0x20000312, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_DAMAGE_PRESS1", 0x200003C8, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AC_OK1", 0x20000489, "SE4_BTL_M_AC_SUCCESS1", 110, 64, 0, 0, 0, 8, 0x0},
	{"SFX_AC_FAIL1", 0x20000000, "SE4_BTL_S_AC_FAIL1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_MARIO_FALL_CRUSH1", 0x2000021A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ACROBAT_START1", 0x2000011F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ACROBAT_MISS1", 0x2000030F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ACROBAT_FINISH1", 0x2000030E, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ACROBAT_KAITEN1", 0x20000000, "SE4_BTL_M_ACROBAT_KAITEN1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ACROBAT_KAITEN2", 0x20000000, "SE4_BTL_M_ACROBAT_KAITEN2", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ACROBAT_TRICK1", 0x20000000, "SE4_BTL_M_ACROBAT_TRICK1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ACROBAT_TRICK2", 0x20000000, "SE4_BTL_M_ACROBAT_TRICK2", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ACROBAT_TRICK3", 0x20000000, "SE4_BTL_M_ACROBAT_TRICK3", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ACROBAT_SHAKE1", 0x20000000, "SE4_BTL_M_ACROBAT_SHAKE1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_JUMP_TUGITUGI1", 0x20000326, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_JUMP_KURURIN1", 0x2000037D, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_JUMP_JABARA1", 0x200003C8, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_JUMP_JABARA2", 0x200003C9, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_JUMP_JABARA3", 0x200003CA, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_JUMP_JABARA4", 0x20000394, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_JUMP_JABARA5", 0x2000037D, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_JUMP_TATUMAKI1", 0x2000012E, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_JUMP_TATUMAKI2", 0x2000012F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_JUMP_GATUN1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_JUMP_DOKAN1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_JUMP_CONTINUE1", 0x2000031C, "", 0, 64, 0, 0, 0, 10, 0x0},
	{"SFX_BTL_JUMP_CONTINUE2", 0x2000031D, "", 100, 64, 0, 0, 0, 10, 0x0},
	{"SFX_BTL_JUMP_CONTINUE3", 0x2000031E, "", 100, 64, 0, 0, 0, 10, 0x0},
	{"SFX_BTL_JUMP_CONTINUE4", 0x2000031F, "", 100, 64, 0, 0, 0, 10, 0x0},
	{"SFX_BTL_JUMP_CONTINUE5", 0x20000320, "", 100, 64, 0, 0, 0, 10, 0x0},
	{"SFX_BTL_JUMP_CONTINUE6", 0x20000321, "", 100, 64, 0, 0, 0, 10, 0x0},
	{"SFX_BTL_JUMP_CONTINUE7", 0x20000322, "", 100, 64, 0, 0, 0, 10, 0x0},
	{"SFX_BTL_JUMP_CONTINUE8", 0x20000323, "", 100, 64, 0, 0, 0, 10, 0x0},
	{"SFX_BTL_JUMP_CONTINUE9", 0x20000324, "", 100, 64, 0, 0, 0, 10, 0x0},
	{"SFX_BTL_JUMP_CONTINUE10", 0x20000325, "", 100, 64, 0, 0, 0, 10, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_MARIO_HAMMER_PIKKYO_R1", 0x20000259, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BTL_MARIO_HAMMER_PIKKYO_G1", 0x20000256, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BTL_MARIO_HAMMER_PIKKYO_B1", 0x20000257, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BTL_MARIO_HAMMER_PIKKYO_Y1", 0x2000025A, "", 105, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BTL_MARIO_HAMMER_PIKKYO_P1", 0x20000258, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_MARIO_HAMMER_SHINE1", 0x20000209, "", 80, 64, 0, 60, 0, 0, 0x555},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_ATTACK_EARTHQUAKE1", 0x20000319, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_EARTHQUAKE1", 0x20000327, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_ATTACK_ULTRAQUAKE1", 0x2000031B, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_ULTRAQUAKE1", 0x20000328, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_HIT_GROUND1", 0x2000037F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_HIT_GROUND2", 0x20000380, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_HIT_GROUND3", 0x20000381, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_NAGERU1", 0x20000318, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_TURANUKI1", 0x2000031A, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_MARIO_HAMMER_KAITEN1", 0x2000021F, "", 80, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BTL_MARIO_HAMMER_KAITEN2", 0x20000220, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_ULTRA1", 0x2000021F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_ULTRA2", 0x20000220, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_MARIO_HAMMER_FIRE1", 0x2000021F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_MARIO_HAMMER_FIRE2", 0x20000220, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_MARIO_HAMMER_FIRE3", 0x20000314, "", 100, 64, 0, 0, 0, 0, 0xFD55},
	{"SFX_BTL_MARIO_HAMMER_FIRE4", 0x200004CA, "btl_e_elmos_att1_22k", 110, 64, 0, 0, 0, 0, 0xFD55},
	{"SFX_BTL_MARIO_HAMMER_FIRE5", 0x200004CC, "", 110, 64, 0, 0, 0, 0, 0xFD55},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_LUCKY1", 0x20000000, "SE4_BTL_S_LUCKY1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_PINCH1", 0x2000003A, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_DANGER1", 0x20000038, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_TURN1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_JUMP_FUNYA1", 0x20000000, "SE4_BTL_M_JUMP_FUNYA1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_CONFUSE1", 0x20000000, "SE4_BTL_M_HMMR_CONFUSE1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_RETURN1", 0x20000000, "SE4_BTL_M_HMMR_RETURN1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_HAMMER_THROUGH1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_BRUSH_DUST1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_STAMP_THORN1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_STAMP_THORN_HURT1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_DAMAGE_KURUKURU1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_DAMAGE_PATAN1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_BREAK_ICE1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_GUARD1_DUMMY", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_POWER_UP1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_POWER_DOWN1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_PEKE_MARK1", 0x20000000, "SE1_PEKE_MARK1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NO_ACTION1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CURSOR_MOVE1", 0x20000477, "SE1_SELECT5", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CURSOR_MOVE2", 0x20000478, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CURSOR_OK1", 0x20000479, "", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CURSOR_CANCEL1", 0x20000476, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_PARTY_IN1", 0x20000155, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_PARTY_CHANGE1", 0x20000360, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_CHARGE1", 0x20000361, "", 100, 64, 0, 0, 0, 0, 0xFD55},
	{"SFX_MARIO_SUPER_CHARGE1", 0x20000362, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_APPEAL1", 0x2000035F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_AVOID1", 0x20000313, "", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_MARIO_PARTY_CHANGE1", 0x2000025C, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_MARIO_DEFENCE1", 0x20000361, "", 100, 64, 0, 0, 0, 0, 0xFD55},
	{"SFX_BTL_MARIO_DEFENCE2", 0x2000012A, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_BATTLE_STAMP1", 0x2000055A, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_RUN_AWAY1", 0x20000364, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_RUN_AWAY_MISS1", 0x20000365, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_DAMAGE1", 0x20000316, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_DAMAGE_FIRE1", 0x2000040E, "SE4_BTL_M_DAMAGE_FIRE1", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_DAMAGE_ICE1", 0x20000410, "SE4_BTL_M_DAMAGE_ICE1", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_DAMAGE_BIRIBIRI1", 0x200002F5, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_DAMAGE_FP1", 0x20000129, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_POISON1", 0x20000300, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_SLEEP1", 0x2000030A, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_STOP1", 0x2000030C, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_GURUGURU1", 0x200002FC, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_CONFUSE1", 0x200002F7, "", 115, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_BIRIBIRI1", 0x200002F5, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_YOKEYOKE1", 0x2000030D, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_FIRE1", 0x2000040E, "SE4_BTL_M_DAMAGE_FIRE1", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_ICE1", 0x20000410, "SE4_BTL_M_DAMAGE_ICE1", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_DEKADEKA1", 0x200002F8, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_MINIMINI1", 0x200002FF, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_TUYOTUYO1", 0x20000124, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_YOWAYOWA1", 0x20000125, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_KATIKATI1", 0x200002FE, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_FUNYA1", 0x200002FB, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_CHARGE1", 0x20000122, "", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_TOUMEI1", 0x20000128, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_FAST1", 0x200002FA, "", 97, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_SLOW1", 0x2000030B, "", 97, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_COUNTER1", 0x20000123, "", 115, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_REST_HP_SLOW1", 0x20000308, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_REST_FP_SLOW1", 0x20000304, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_ALLERGY1", 0x200002F4, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_BREAK_ICE1", 0x20000315, "SE2_BTL_STG_DMG_GLASS1", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_DOWN1", 0x2000021A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_CONDITION_DIE1", 0x20000000, "SE4_BTL_C_DIE1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_COMEBACK1", 0x20000000, "SE4_BTL_C_COME_BACK1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_TURN_THROUGH1", 0x20000368, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_INSIDE", 0x200002FD, "", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_AWATERU1", 0x20000000, "SE4_BTL_M_AWATERU1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_FIND1", 0x20000265, "SE4_BTL_M_FIND1", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_QUESTION1", 0x2000003C, "SE4_BTL_M_QUESTION1", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_SHADOW_HIDE1", 0x20000000, "SE4_BTL_C_SHADOW_HIDE2", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_CONDITION_SHADOW_HIDE2", 0x20000000, "SE4_BTL_C_SHADOW_HIDE3", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_HP_RECOVER_SMALL1", 0x20000309, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_HP_RECOVER_MID1", 0x20000307, "", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_HP_RECOVER_BIG1", 0x20000306, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_HP_RECOVER_SHINE1", 0x20000158, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_HP_RECOVER_SHINE2", 0x20000159, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_HP_RECOVER_SHINE3", 0x2000015A, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_FP_RECOVER_SMALL1", 0x20000305, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_FP_RECOVER_MID1", 0x20000303, "", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_FP_RECOVER_BIG1", 0x20000302, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_FP_RECOVER_SHINE1", 0x20000158, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_FP_RECOVER_SHINE2", 0x20000159, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_FP_RECOVER_SHINE3", 0x2000015A, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_REST_CONDITION1", 0x20000301, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SYS_AP_STAR1", 0x20000172, "SE2_BTL_SYS_AP_UP1", 105, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX__BTL_SYS_AP_STAR1", 0x20000000, "SE2_BTL_SYS_STAR_TOGETHER1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX__BTL_SYS_AP_UP1", 0x20000000, "SE2_BTL_SYS_AP_UP1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX__BTL_SYS_AP_DOWN1", 0x20000000, "SE2_BTL_SYS_AP_DOWN1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_SYSTEM_NICE_25PERCENT1", 0x2000048A, "", 110, 64, 0, 0, 0, 8, 0x0},
	{"SFX_SYSTEM_NICE_50PERCENT1", 0x2000048B, "", 110, 64, 0, 0, 0, 8, 0x0},
	{"SFX_SYSTEM_NICE1", 0x20000489, "", 110, 64, 0, 0, 0, 8, 0x0},
	{"SFX_SYSTEM_GOOD1", 0x20000484, "", 110, 64, 0, 0, 0, 8, 0x0},
	{"SFX_SYSTEM_GREAT1", 0x20000485, "", 115, 64, 0, 0, 0, 8, 0x0},
	{"SFX_SYSTEM_WONDERFUL1", 0x2000049D, "", 115, 64, 0, 0, 0, 8, 0x0},
	{"SFX_SYSTEM_EXCELLENT1", 0x2000047F, "", 115, 64, 0, 0, 0, 8, 0x0},
	{"SFX_SYSTEM_NOHIT1", 0x20000000, "", 0, 64, 0, 0, 0, 8, 0x0},
	{"SFX_SYSTEM_LUCKY1", 0x20000488, "", 100, 64, 0, 0, 0, 8, 0x0},
	{"SFX_SYSTEM_ACROBAT_IN1", 0x2000046F, "", 100, 64, 0, 0, 0, 8, 0x0},
	{"SFX_SYSTEM_ACROBAT1", 0x2000046E, "", 80, 64, 0, 0, 0, 8, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_MARIO_GUARD1", 0x20000317, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_STAR_POINT_GET1", 0x20000367, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_NOT_CHOICE1", 0x20000266, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AC_PI1", 0x2000035E, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_AC_PONE1", 0x2000035D, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_AC_POWER_GAUGE1", 0x20000154, "SE4_BTL_S_AC_METER1", 70, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AC_COMMAND_OK1", 0x2000001F, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_AC_COMMAND_NG1", 0x20000020, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AC_CURSOR_MOVE1", 0x20000156, "", 80, 64, 0, 0, 0, 0, 0x555},
	{"SFX_AC_GAUGE_MOVE1", 0x20000157, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_AC_BUTTON_PUSH1", 0x2000035E, "", 60, 64, 0, 0, 0, 0, 0x800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_SPECIAL_AC_PI1", 0x20000000, "SE4_BTL_S_AC_S_START1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_SPECIAL_AC_PONE1", 0x20000000, "SE4_BTL_S_AC_S_END1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_AC_SPECIAL1", 0x20000000, "SE4_BTL_S_AC_S_METER1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SLOT_START1", 0x40097979, "", 75, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SLOT_SUCCESS1", 0x40097A7A, "", 75, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_LVUP_CURSOR1", 0x20000477, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_LVUP_BLOCK_MOVE1", 0x20000153, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_LVUP_BLOCK_HIT1", 0x20000152, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SYS_BREAK_FAILRE1", 0x2000016C, "", 60, 64, 0, 0, 0, 0, 0x555},
	{"SFX_BTL_SYS_BREAK1", 0x20000150, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SYS_CAUTION_THROW1", 0x20000151, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SYS_LEVELUP_FALL1", 0x20000150, "", 80, 64, 0, 0, 0, 0, 0x555},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_START1", 0x40041C1C, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_START2", 0x20000139, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_START3", 0x2000013A, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_SUCCESS1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_HEART_SHOOT1", 0x20000137, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_HEART_SHOOT2", 0x200004FF, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_HEART_SHOOT3", 0x20000308, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_HEART_SHOOT4", 0x20000304, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_HEART_SHOOT5", 0x20000307, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_HEART_SHOOT6", 0x20000303, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_HEART_SHOOT7", 0x20000020, "", 107, 64, 0, 0, 0, 0, 0x555},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_DOKKAN1_1", 0x20000154, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_DOKKAN1_2", 0x20000133, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_DOKKAN1_3", 0x20000134, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_DOKKAN1_4", 0x20000135, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_DOKKAN1_5", 0x20000136, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_DOKKAN2", 0x2000001F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_DOKKAN3", 0x20000020, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_DOKKAN4", 0x200002F8, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_DOKKAN5", 0x200000D7, "", 120, 64, 0, 0, 0, 0, 0x555},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_TIME1", 0x20000154, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_TIME2", 0x200001B7, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_TIME3", 0x200000AD, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_TIME4", 0x200002F8, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_TIME5", 0x20000126, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_TIME6", 0x20000000, "SE2_BTL_SAC_TIME6", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_BODY1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_BODY2", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_BODY3", 0x20000478, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_BODY4", 0x2000001F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_BODY5", 0x20000020, "", 107, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_BODY6", 0x20000132, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_KAKONDEPON1", 0x20000138, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_KAKONDEPON2", 0x20000536, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_SUKKIRI1", 0x2000001F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_SUKKIRI2", 0x20000020, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_SUKKIRI3", 0x2000013A, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_SUKKIRI4", 0x2000054A, "", 90, 64, 0, 0, 0, 0, 0x800},
	{"SFX_BTL_SAC_SUKKIRI4_2", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_SUKKIRI5", 0x40044848, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_ZUBA_STAR1", 0x20000154, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_ZUBA_STAR2", 0x2000013C, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SAC_ZUBA_STAR3", 0x2000053C, "", 105, 64, 0, 50, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_BATTLE_SUBMOVE1L", 0x200003BF, "", 50, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_BATTLE_SUBMOVE1R", 0x200003C0, "", 50, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_BATTLE_SUBHANG1", 0x200003BE, "", 50, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_BATTLE_MOVE1L", 0x200003BF, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_BATTLE_MOVE1R", 0x200003C0, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_BATTLE_HANG1", 0x200003BE, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_BATTLE_JUMP1", 0x200003BA, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_BATTLE_LANDING1", 0x200003BB, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_BATTLE_DIE1", 0x20000130, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_DAMAGE1", 0x20000337, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_MOJIMOJI1", 0x2000033F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_PARTY_KYOROKYORO1", 0x2000033E, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_KURI_ATTACK1", 0x20000338, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_KURI_ATTACK2", 0x20000339, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_KURI_KISS1", 0x2000033A, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_KURI_CHEER_KISS1", 0x40077676, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_KURI_SQUAT1", 0x2000033D, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_KURI_LEARNED1", 0x2000033B, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_KURI_LEARNED2", 0x2000033C, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_KURI_FAIL_LEARNED1", 0x20000131, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NOKO_ATTACK1", 0x20000340, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NOKO_ATTACK2", 0x20000341, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NOKO_SHUBIBIN1", 0x20000342, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NOKO_SHUBIBIN2", 0x20000343, "", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NOKO_IPPATSU1", 0x20000344, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NOKO_IPPATSU2", 0x20000345, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NOKO_GUARD1", 0x20000346, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NOKO_GUARD2", 0x20000144, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NOKO_GUARD3", 0x2000023F, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NOKO_GUARD4", 0x20000240, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CLAUD_ATTACK1", 0x20000331, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CLAUD_ATTACK2", 0x20000332, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CLAUD_BREATH1", 0x20000333, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CLAUD_BREATH2", 0x20000334, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CLAUD_BREATH3", 0x20000335, "", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CLAUD_KISS1", 0x40077575, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CLAUD_KISS2", 0x20000336, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CLAUD_HIDE1", 0x200001B4, "", 60, 64, 0, 0, 0, 0, 0x800},
	{"SFX_BTL_CLAUD_HIDE2", 0x200004A8, "btl_e_3rd_fumble1_22k", 110, 64, 0, 0, 0, 0, 0xD55},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_VIVIAN_ATTACK1", 0x20000242, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_VIVIAN_ATTACK2", 0x2000034D, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_VIVIAN_ATTACK3", 0x2000034E, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_VIVIAN_ATTACK4", 0x2000034F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_VIVIAN_MAGIC1", 0x20000350, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_VIVIAN_MEROMERO1", 0x2000033A, "", 115, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_VIVIAN_SHADOW1_1", 0x20000244, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BTL_VIVIAN_SHADOW1_2", 0x20000243, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_THUNDERS_ATTACK1", 0x20000347, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_THUNDERS_ATTACK2", 0x20000127, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_THUNDERS_ATTACK2_2", 0x20000348, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_THUNDERS_ATTACK3", 0x20000126, "SE4_BTL_P_THUNDERS_ATT3", 100, 64, 0, 0, 0, 0, 0x555},
	{"SFX_BTL_THUNDERS_ATTACK4", 0x20000349, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_THUNDERS_BOMB1", 0x2000034A, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_THUNDERS_BOMB2", 0x200003F7, "", 110, 64, 0, 0, 0, 0, 0x2AB},
	{"SFX_BTL_THUNDERS_COUNTER1", 0x2000034B, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_THUNDERS_DOWN1", 0x200001B7, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_EGG1", 0x20000352, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_EGG2", 0x20000353, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_EGG3", 0x200004FF, "", 80, 64, 0, 0, 0, 0, 0xF800},
	{"SFX_BTL_YOSHI_SWALLOW1", 0x20000354, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_SWALLOW2", 0x20000355, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_SWALLOW3", 0x20000356, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_SWALLOW4", 0x20000357, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_SWALLOW5", 0x20000358, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_SWALLOW6", 0x20000359, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_SWALLOW7", 0x2000035A, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_ATTACK1", 0x20000351, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_TAIGUN1", 0x2000035B, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_TAIGUN2", 0x20000000, "SE4_BTL_P_YOSH_TAIGUN2", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_TAIGUN3", 0x20000000, "SE4_BTL_P_YOSH_TAIGUN3", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_TAIGUN4", 0x20000000, "SE4_BTL_P_YOSH_TAIGUN4", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_TAIGUN5", 0x20000000, "SE4_BTL_P_YOSH_TAIGUN5", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YOSHI_TAIGUN6", 0x20000000, "SE4_BTL_P_YOSH_TAIGUN6", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CHUCHURINA_ATTACK1", 0x20000329, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CHUCHURINA_ATTACK2", 0x2000032A, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CHUCHURINA_ATTACK3", 0x2000032B, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CHURINA_CONFUSE1_2", 0x20000226, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CHURINA_CONFUSE1", 0x2000032C, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BTL_CHURINA_CONFUSE2", 0x20000000, "SE4_BTL_P_CHURINA_CONFUSE2", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CHURINA_CONFUSE_MOVE1L", 0x200003BF, "", 60, 64, 0, 0, 0, 0, 0x555},
	{"SFX_BTL_CHURINA_CONFUSE_MOVE1R", 0x200003C0, "", 60, 64, 0, 0, 0, 0, 0x555},
	{"SFX_BTL_CHURINA_CONFUSE_JUMP1", 0x200003BA, "", 60, 64, 0, 0, 0, 0, 0x555},
	{"SFX_BTL_CHURINA_CONFUSE_LANDING1", 0x200003BB, "", 60, 64, 0, 0, 0, 0, 0x555},
	{"SFX_BTL_CHURINA_EYE1", 0x2000032E, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CHURINA_HEARTCATCH1", 0x2000032F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CHURINA_SMELL1", 0x20000330, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CHURINA_KISS1", 0x200003B9, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENEMY_DIE1", 0x20000414, "SE1_ENEMY_GO_OUT2", 55, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_HONENOKO_MOVE1L", 0x200004F4, "", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_HONENOKO_MOVE1R", 0x200004F5, "", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_HONENOKO_JUMP1", 0x200004F1, "btl_e_noko_jump1_22k", 57, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_HONENOKO_LANDING1", 0x20000506, "btl_e_landing_hard1_22k", 55, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_KARON_BONE_THROW1", 0x20000505, "btl_e_bone_throw1_22k", 57, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_KILLER_CANNON1", 0x2000041D, "btl_e_firling1_22k", 120, 64, 0, 80, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_KUMO_MOVE1", 0x200004EF, "btl_e_move_float4_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_KURI_MOVE1L", 0x20000440, "btl_e_move_walk1l_22k", 45, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_KURI_MOVE1R", 0x20000441, "btl_e_move_walk1r_22k", 45, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_KURI_JUMP1", 0x20000426, "btl_e_jump_normal1_22k", 40, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_PATAKURI_MOVE1", 0x2000044A, "btl_e_pkuri_move1_22k", 45, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_SAMBO_MOVE1", 0x2000044F, "btl_e_sambo_move1_22k", 80, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_EMON_MOVE1L", 0x20000417, "btl_e_emon_walkl1_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_EMON_MOVE1R", 0x20000418, "btl_e_emon_walkr2_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_EMON_JUMP1", 0x20000195, "", 55, 64, 0, 1, 0, 0, 0x800},
	{"SFX_FLD_ENM_EMON_LANDING1", 0x20000417, "btl_e_emon_walkl1_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_3RD1_MOVE1L", 0x200004B0, "btl_e_3rd_walkl1_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_3RD1_MOVE1R", 0x200004B1, "btl_e_3rd_walkr1_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_3RD1_JUMP1", 0x20000426, "btl_e_jump_normal1_22k", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_3RD1_LANDING1", 0x20000435, "btl_e_3rd_landing1_22k", 55, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_CHORO1_MOVE1", 0x20000407, "btl_e_choro_move1_22k", 45, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_TELESA_MOVE1", 0x2000045B, "btl_e_telesa_move1_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_NOKO_JUMP1", 0x200004F1, "btl_e_noko_jump1_22k", 57, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_NOKO_MOVE1", 0x200004F2, "btl_e_noko_move1_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_NOKO_MOVE2", 0x20000000, "SE6_BTL_E_NOKO_MOVE2", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_NOKO_SHELL1", 0x200004F6, "btl_e_noko_shell1_22k", 60, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_NOKO_MOVE3L", 0x200004F4, "btl_e_noko_move3l_22k", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_NOKO_MOVE3R", 0x200004F5, "btl_e_noko_move3r_22k", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_NOKO_JUMP2", 0x200004F1, "btl_e_noko_jump1_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_NOKO_LANDING1", 0x20000506, "btl_e_landing_hard1_22k", 55, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_PATA_MOVE1", 0x200004FE, "btl_e_pata_move1_22k", 57, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_PATA_MOVE2", 0x200004FF, "btl_e_pata_move2_22k", 57, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_PATA_MOVE3", 0x20000500, "btl_e_pata_move3_22k", 57, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_PATA_ATTACK1", 0x20000000, "SE6_BTL_E_PATA_SWING1", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_PAIDA_MOVE1", 0x20000444, "btl_e_paida_move1_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_PAIDA_MOVE2", 0x20000444, "btl_e_paida_move1_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_BASA_MOVE1", 0x200004BF, "btl_e_basa_move1_22k", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_BUBBLE_MOVE1", 0x200004CD, "btl_e_bubble_move1_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_BARRI_ATTACK1", 0x200004B5, "btl_e_barri_att3_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_BARRI_ATTACK2", 0x20000000, "SE6_BTL_E_BARRI_ATT7", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_PANZI_MOVE1L", 0x2000020B, "", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_PANZI_MOVE1R", 0x2000020C, "", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_PANZI_MOVE2", 0x20000447, "btl_panzi_escape1_1_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_HANNYA_MOVE1", 0x200004D1, "btl_e_hannya_move1_22k", 60, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_BROS_MOVE1L", 0x200004F4, "btl_e_noko_move3l_22k", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_BROS_MOVE1R", 0x200004F5, "btl_e_noko_move3r_22k", 50, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_PAKKUN_MOVE1", 0x200004F8, "btl_e_pakkun_move1_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_PAKKUN_MOVE2", 0x200004F9, "btl_e_pakkun_move2_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_PAKKUN_ATTACK1", 0x200000A3, "aud_paku_eat1_22k", 57, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_BOMB_MOVE1", 0x200003F9, "btl_e_bomb_move2_22k", 48, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_BOMB_JUMP1", 0x200003F8, "btl_e_bomb_move1_22k", 40, 64, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_BOMB_LANDING1", 0x200003F9, "btl_e_bomb_move2_22k", 48, 64, 0, 1, 0, 0, 0x555},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_BORODO_MOVE1L", 0x200004C6, "btl_e_borodo_move1l_22k", 40, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_BORODO_MOVE1R", 0x200004C7, "btl_e_borodo_move1r_22k", 40, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_BORODO_JUMP1", 0x200004F1, "btl_e_noko_jump1_22k", 57, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_BORODO_LANDING1", 0x200004C6, "btl_e_borodo_move1l_22k", 40, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_MAHO_MOVE1", 0x200004EF, "btl_e_move_float4_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_MET_MOVE1L", 0x200004E0, "btl_e_met_move1l_22k", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_MET_MOVE1R", 0x200004E1, "btl_e_met_move1r_22k", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_MET_ATTACK1", 0x200004DA, "btl_e_met_att1_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_MET_ATTACK2", 0x20000000, "SE6_BTL_E_MET_ATT2", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_MET_MOVE2", 0x20000000, "SE6_BTL_E_MET_MOVE2", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_PATAMET_MOVE1", 0x200004FD, "btl_e_patamet_wait1_22k", 45, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_PATAMET_MOVE2", 0x200004FC, "btl_e_patamet_move2_22k", 62, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_WAN_MOVE1", 0x20000463, "btl_e_wan_wait2_22k", 52, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_WAN_MOVE2", 0x20000464, "btl_e_wan_wait3_22k", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_WAN_MOVE3", 0x20000000, "SE5_BTL_E_WAN_WAIT4", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_WAN_LANDING1", 0x20000461, "btl_e_wan_landing1_22k", 50, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_WAN_ATTACK2", 0x2000045F, "btl_e_wan_att2_22k", 52, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_KAMEKU_MOVE1L", 0x2000020B, "", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_KAMEKU_MOVE1R", 0x2000020C, "", 55, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_KAMEKU_JUMP1", 0x200004F1, "btl_e_noko_jump1_22k", 57, 64, 0, 1, 0, 0, 0x0},
	{"SFX_FLD_ENM_KAMEKU_LANDING1", 0x200004C6, "btl_e_borodo_move1l_22k", 40, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_FLD_ENM_JUGEM_MOVE1", 0x200004EF, "btl_e_move_float4_22k", 55, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ENEMY_DIE1_1", 0x20000413, "btl_e_die1_1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ENEMY_DIE1_2", 0x20000414, "btl_e_die1_2_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_DAMAGE_FIRE1", 0x2000040E, "btl_e_damaged_fire1_22k", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_DAMAGE_ICE1", 0x20000410, "btl_e_damaged_ice1_22k", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_DAMAGE_BIRIBIRI1", 0x200002F5, "btl_e_damaged_elec1_22k", 95, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_ENEMY_FIND_MARIO1", 0x20000000, "SE2_FIND_MARIO2", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_ITEM_USE1", 0x20000425, "btl_e_item_use1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_INSIDE1", 0x2000045C, "btl_e_turned1_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_INSIDE2", 0x2000045D, "btl_e_turned2_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MAGIC1", 0x20000000, "btl_e_magic1_22k", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_FALL1", 0x2000041A, "btl_e_fall1_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_ESCAPE1", 0x20000419, "btl_e_escape1_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_DAMAGED_PLIABLE1", 0x20000412, "btl_e_damaged_pliable1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHARGE1", 0x20000361, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HONENOKO_SEND_GROUP1", 0x20000505, "btl_e_throw_poi1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HONENOKO_SEND_GROUP2", 0x20000506, "btl_e_throw_pote1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HONENOKO_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HONENOKO_CRUMBLE1", 0x200004CF, "btl_e_crumble1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HONENOKO_UNITE1", 0x20000524, "btl_e_unite1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HONENOKO_UNITE2", 0x20000525, "btl_e_unite2_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HONENOKO_LANDING1", 0x20000506, "btl_e_landing_hard1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HONENOKO_BONE_THROW1", 0x20000504, "btl_e_throw_long1_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HONENOKO_BONE_THROW2", 0x200004C0, "btl_e_bone_throw1_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HONENOKO_BONE_THROW3", 0x200004C0, "btl_e_bone_throw1_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KILLER_MOVE1", 0x20000433, "btl_e_killer_move1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KILLER_ATTACK1", 0x20000430, "btl_e_killer_att1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KILLER_CHARGE1", 0x20000432, "btl_e_killer_charge1_22k", 95, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KILLER_DAMAGED1", 0x2000040F, "btl_e_damaged_hard1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KILLER_CANNON1", 0x2000041D, "btl_e_firling1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KILLER_CANNON2", 0x2000041D, "btl_e_firling1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KILLER_CANNON3", 0x2000041D, "btl_e_firling1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KILLER_CANNON4", 0x2000041D, "btl_e_firling1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KILLER_CANNON_CHARGE1", 0x20000431, "btl_e_charge1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KILLER_CANON_DAMAGED1", 0x2000040F, "btl_e_damaged_hard1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KUMO_MOVE1", 0x200004EC, "btl_e_move_float1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KUMO_MOVE2", 0x200004ED, "btl_e_move_float2_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KUMO_MOVE3", 0x200004EE, "btl_e_move_float3_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KUMO_MOVE4", 0x200004EF, "btl_e_move_float4_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KUMO_MOVE5", 0x200004EF, "btl_e_move_float5_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KUMO_ATTACK1", 0x200004D9, "btl_e_charge1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KUMO_DAMAGED1", 0x20000412, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KUMO_CHARGE1", 0x200004D9, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KUMO_CHARGE_ATTACK1", 0x200002F5, "エフェクト雷の音", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_DOKU_KUMO_CHARGE1", 0x200004D9, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_DOKU_KUMO_CHARGE_ATTACK1", 0x20000300, "se4_btl_c_poison1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BLIZ_KUMO_CHARGE1", 0x200004D9, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BLIZ_KUMO_CHARGE_ATTACK1", 0x20000334, "se4_btl_p_clau_breath2_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KURI_JUMP1", 0x20000426, "btl_e_jump_normal1_22k", 70, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KURI_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KURI_LANDING1", 0x20000435, "btl_e_landing_normal1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KURI_MOVE1", 0x20000440, "btl_e_move_walk1l_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KURI_MOVE2", 0x20000441, "btl_e_move_walk1r_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KURI_LANDING_MISS1", 0x20000434, "btl_e_landing_miss1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEKURI_MOVE1", 0x20000440, "btl_e_move_walk1l_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEKURI_MOVE2", 0x20000441, "btl_e_move_walk1r_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEKURI_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATAKURI_MOVE1", 0x2000044A, "btl_e_pkuri_move1_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATAKURI_MOVE2", 0x2000044B, "btl_e_pkuri_move2_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATAKURI_MOVE3", 0x2000044C, "btl_e_pkuri_move3_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATAKURI_WAIT1", 0x2000044A, "btl_e_pkuri_move1_22k", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATAKURI_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_SAMBO_MOVE1", 0x2000044F, "btl_e_sambo_move1_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_SAMBO_ATTACK1", 0x20000458, "btl_e_charge1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_SAMBO_ATTACK3", 0x20000458, "btl_e_charge1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_SAMBO_ATTACK4", 0x2000044E, "btl_e_att_hard1_22k", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_SAMBO_ATTACK5", 0x20000452, "btl_e_sambo_throw1_22k", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_SAMBO_SEND_GROUP1_3", 0x20000456, "btl_e_swing1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_SAMBO_SEND_GROUP1_2", 0x20000455, "btl_e_swing1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_SAMBO_SEND_GROUP1_1", 0x20000454, "btl_e_swing1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_SAMBO_SEND_GROUP2", 0x20000457, "btl_e_swing2_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_SAMBO_SEND_GROUP3", 0x2000044D, "btl_e_pullout1_22k", 95, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_SAMBO_MOVE2", 0x20000450, "btl_e_jump_short1_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_SAMBO_MOVE3", 0x20000451, "btl_e_jump_normal1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_SAMBO_ATTACK6", 0x2000044E, "btl_e_att_hard1_22k", 95, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_SAMBO_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_EMON_MOVE1", 0x20000417, "btl_e_emon_walkl1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_EMON_MOVE2", 0x20000418, "btl_e_emon_walkr2_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_EMON_MOVE3", 0x20000415, "btl_e_charge1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_EMON_DASH1", 0x20000416, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_EMON_LANDING1", 0x20000417, "", 100, 64, 0, 0, 0, 0, 0xFAAB},
	{"SFX_ENM_EMON_JUMP1", 0x20000195, "", 100, 64, 0, 0, 0, 0, 0x800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_EMON_DAMAGED1", 0x2000040F, "btl_e_damaged1_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD1_MOVE1", 0x200004B0, "btl_e_3rd_walkl1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD1_MOVE2", 0x200004B1, "btl_e_3rd_walkr1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD1_MOVE3", 0x200004AE, "btl_e_3rd_swing1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD1_ATTACK1", 0x200004A2, "btl_e_3rd_att1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD1_ATTACK2", 0x200004AB, "btl_e_3rd_jump1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD1_ATTACK3", 0x200004A3, "btl_e_3rd_att2_22k", 115, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD1_BOTTLE1", 0x200004A8, "btl_e_3rd_fumble1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD1_BOTTLE2", 0x200004A9, "btl_e_3rd_hoist1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD1_BOTTLE3", 0x200004A4, "btl_e_3rd_cloth1_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD1_BOTTLE4", 0x200004A6, "btl_e_3rd_drink1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD1_BOTTLE5", 0x200004A7, "btl_e_3rd_drink2_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD1_MISS1", 0x200004AC, "btl_e_3rd_miss1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD1_DAMAGED1", 0x20000411, "btl_e_3rd_damaged1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD1_LANDING1", 0x20000435, "btl_e_3rd_landing1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD2_ATTACK1", 0x200004A8, "btl_e_3rd_fumble1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD2_ATTACK2", 0x200004A9, "btl_e_3rd_hoist1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD2_ATTACK3", 0x200004AA, "btl_e_3rd_hoist2_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD2_ATTACK4", 0x200004AF, "btl_e_3rd_throw1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD2_ATTACK5", 0x20000000, "btl_e_3rd_totter1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD2_BOTTLE3", 0x200004A4, "btl_e_3rd_cloth1_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD2_BOTTLE4", 0x200004A6, "btl_e_3rd_drink1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD2_BOTTLE5", 0x200004A7, "btl_e_3rd_drink2_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD2_SHAKE1", 0x200004AD, "btl_e_3rd_shake1_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD2_CRASH1", 0x200004A5, "btl_e_3rd_crash1_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_3RD2_DAMAGED1", 0x20000411, "btl_e_3rd_damaged1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO1_MOVE1", 0x20000407, "btl_e_choro_move1_22k", 75, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO1_ATTACK1_1", 0x20000401, "btl_e_choro_att1_1_22k", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO1_ATTACK1_2", 0x20000402, "btl_e_choro_att1_2_22k", 65, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO1_ATTACK2", 0x20000403, "btl_e_choro_att2_22k", 65, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO1_ATTACK3", 0x20000408, "btl_e_choro_move2_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO1_WAIT1", 0x2000040B, "btl_e_choro_wait1_22k", 55, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO1_WAIT2", 0x2000040C, "btl_e_choro_wait2_22k", 55, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO1_LANDING1", 0x20000406, "btl_e_choro_landing1_22k", 55, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO1_DAMAGED1", 0x20000404, "btl_e_choro_damaged1_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO2_MOVE1", 0x20000407, "btl_e_choro_move1_22k", 75, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO2_ATTACK1_1", 0x20000401, "btl_e_choro_att1_1_22k", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO2_ATTACK1_2", 0x20000402, "btl_e_choro_att1_2_22k", 65, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO2_ATTACK2", 0x20000403, "btl_e_choro_att2_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO2_ATTACK3", 0x20000408, "btl_e_choro_move2_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO2_SPLIT1", 0x20000409, "btl_e_choro_turn1_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO2_SPLIT1_2", 0x2000040A, "btl_e_choro_turn2_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO2_SPLIT2", 0x20000405, "btl_e_choro_jump1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO2_MAGIC1", 0x20000250, "", 110, 64, 0, 0, 0, 0, 0x2AB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO2_WAIT1", 0x2000040B, "btl_e_choro_wait1_22k", 55, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO2_WAIT2", 0x2000040C, "btl_e_choro_wait2_22k", 55, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO2_LANDING1", 0x20000406, "btl_e_choro_landing1_22k", 55, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_CHORO2_DAMAGED1", 0x20000404, "btl_e_choro_damaged1_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TELESA_MOVE1", 0x2000045B, "btl_e_telesa_move1_22k", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_ENM_TELESA_MOVE2", 0x2000045A, "btl_e_telesa_hide1_22k", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_ENM_TELESA_MOVE3", 0x2000045B, "btl_e_telesa_toumei1_22k.aif", 80, 64, 0, 30, 0, 0, 0x0},
	{"SFX_ENM_TELESA_MOVE4", 0x2000045A, "btl_e_telesa_toumei1_22k.aif", 80, 64, 0, 30, 0, 0, 0x0},
	{"SFX_ENM_TELESA_ATTACK1", 0x20000459, "btl_e_telesa_att1_22k", 100, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TELESA_DAMAGED1", 0x20000411, "btl_e_telesa_damaged1_22k", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEDA_MOVE1", 0x20000508, "btl_e_togeda_move1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEDA_MOVE2", 0x20000509, "btl_e_togeda_move2_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEDA_MOVE3", 0x2000050A, "btl_e_togeda_move3_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEDA_MOVE4", 0x2000050B, "btl_e_togeda_move4_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEDA_MOVE5", 0x2000050C, "btl_e_togeda_move5_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEDA_MOVE6", 0x2000050D, "btl_e_togeda_move6_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEDA_DAMAGED1", 0x20000507, "btl_e_togeda_damaged1_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_NOKO_JUMP1", 0x200004F1, "btl_e_noko_jump1_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_NOKO_MOVE1", 0x200004F2, "btl_e_noko_move1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_NOKO_MOVE2", 0x200004F3, "btl_e_noko_move2_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_NOKO_SHELL1", 0x200004F6, "btl_e_noko_shell1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_NOKO_MOVE3L", 0x200004F4, "btl_e_noko_move3l_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_NOKO_MOVE3R", 0x200004F5, "btl_e_noko_move3r_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_NOKO_DAMAGED1", 0x2000040F, "btl_e_damaged_hard1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_URANOKO_MOVE1", 0x20000526, "btl_e_uranoko_att1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_URANOKO_MOVE2", 0x20000527, "btl_e_uranoko_att2_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_URANOKO_MOVE3", 0x20000528, "btl_e_uranoko_att3_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_URANOKO_MOVE4", 0x20000529, "btl_e_uranoko_att4_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGENOKO_MOVE1", 0x20000512, "btl_e_togenoko_jump1_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGENOKO_MOVE2", 0x20000513, "btl_e_togenoko_move1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGENOKO_ATTACK1", 0x20000528, "btl_e_uranoko_att3_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGENOKO_ATTACK2", 0x20000529, "btl_e_uranoko_att4_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGENOKO_SHELL1", 0x20000518, "btl_e_togenoko_shell1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGENOKO_CALL1", 0x2000050F, "btl_e_togenoko_call1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGENOKO_CHARGE1", 0x20000510, "btl_e_togenoko_charge1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGENOKO_MOVE3", 0x20000514, "btl_e_togenoko_move2_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGENOKO_MOVE4", 0x20000515, "btl_e_togenoko_move3_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGENOKO_SHELL2", 0x20000519, "btl_e_togenoko_shell2_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGENOKO_MOVE5", 0x20000516, "btl_e_togenoko_move4_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGENOKO_MOVE6", 0x20000517, "btl_e_togenoko_move5_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGENOKO_DAMAGED1", 0x2000040F, "btl_e_damaged_hard1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATA_MOVE1", 0x200004FE, "btl_e_pata_move1_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATA_MOVE2", 0x200004FF, "btl_e_pata_move2_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATA_MOVE3", 0x20000500, "btl_e_pata_move3_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATA_ATTACK1", 0x20000501, "btl_e_pata_swing1_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATA_WAIT1", 0x20000502, "btl_e_pata_wait1_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATA_DAMAGED1", 0x2000040F, "btl_e_damaged_hard1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PAIDA_MOVE1", 0x20000444, "btl_e_paida_move1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PAIDA_ATTACK1", 0x20000442, "btl_e_paida_att1_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PAIDA_ATTACK2", 0x20000443, "btl_e_paida_att2_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PAIDA_MOVE2", 0x20000444, "btl_e_paida_move1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PAIDA_DAMAGED1", 0x20000411, "btl_e_damaged1_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BASA_MOVE1", 0x200004BF, "btl_e_basa_move1_22k", 120, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BASA_SUCK1", 0x20000503, "btl_e_basa_suck1_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BASA_WAIT1", 0x200004BF, "btl_e_basa_move1_22k", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BASA_DAMAGED1", 0x20000411, "btl_e_damaged1_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BUBBLE_MOVE1", 0x200004CD, "btl_e_bubble_move1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BUBBLE_MOVE2", 0x200004CE, "btl_e_bubble_move1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BUBBLE_SPLIT1", 0x200004CC, "btl_e_bubble_split1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BUBBLE_ATTACK2", 0x200004CA, "btl_e_elmos_att1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BUBBLE_ATTACK3", 0x20000314, "炎のエフェクトの音", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BUBBLE_ATTACK4", 0x200004CB, "btl_e_elmos_charge1_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BUBBLE_FIRE1", 0x200004CC, "btl_e_bubble_fire1_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BUBBLE_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_ATTACK1", 0x200004B2, "btl_e_barri_att1_22k", 90, 64, 0, 0, 0, 27, 0x0},
	{"SFX_ENM_BARRI_ATTACK2", 0x200004B4, "btl_e_barri_att2_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_ATTACK3", 0x200004B5, "btl_e_barri_att3_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_ATTACK4", 0x200004B6, "btl_e_barri_att4_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_ATTACK5", 0x200004B7, "btl_e_barri_att5_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_ATTACK6", 0x200004B8, "btl_e_barri_att6_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_ATTACK7", 0x200004B9, "btl_e_barri_att7_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_ATTACK8", 0x200004BA, "btl_e_barri_att8_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_ATTACK9", 0x200004BB, "btl_e_barri_att9_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_ATTACK10", 0x200004B3, "btl_e_barri_att10_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_BARRIER1", 0x200004BC, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_BARRIER_BREAK1", 0x20000141, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_SPLIT1", 0x200004BD, "btl_e_barri_split1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_SPLIT2", 0x200004BE, "btl_e_barri_split2_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PUTIBARRI_MOVE1", 0x20000000, "btl_e_barri_move1_22k", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BARRI_DAMAGED2", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PANSY_ATTACK1", 0x20000445, "btl_e_panzi_attack1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PANSY_ESCAPE1_1", 0x20000447, "btl_panzi_escape1_1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PANSY_ESCAPE1_2", 0x20000448, "btl_panzi_escape1_2_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PANSY_ESCAPE2", 0x20000449, "btl_panzi_escape2_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PANSY_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HANNYA_MOVE1", 0x200004D1, "btl_e_hannya_move1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HANNYA_MOVE2", 0x200004D2, "btl_e_hannya_move2_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HANNYA_MOVE3", 0x200004D3, "btl_e_hannya_move3_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HANNYA_CHARGE1", 0x200004D0, "btl_e_hannya_charge1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HANNYA_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HUMMER_ATTACK1", 0x20000422, "btl_e_hummer_att1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HUMMER_ATTACK2", 0x20000423, "btl_e_hummer_att2_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HUMMER_ATTACK3", 0x20000424, "btl_e_hummer_att3_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HUMMER_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BOOME_ATTACK1", 0x200003FC, "btl_e_boome_att1_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BOOME_ATTACK2", 0x200003FD, "btl_e_boome_att2_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BOOME_ATTACK3", 0x200003FE, "btl_e_boome_att3_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BOOME_ATTACK4", 0x200003FF, "btl_e_boome_att4_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BOOME_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_FIREB_ATTACK1", 0x2000041B, "btl_e_fireb_att1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_FIREB_ATTACK2", 0x2000041C, "btl_e_fireb_att2_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_FIREB_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PAKKUN_MOVE1", 0x200004F8, "btl_e_pakkun_move1_22k", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PAKKUN_MOVE2", 0x200004F9, "btl_e_pakkun_move2_22k", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PAKKUN_ATTACK1", 0x200000A3, "aud_paku_eat1_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PAKKUN_MOUTH1", 0x200004F7, "btl_e_pakkun_mouth1_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PAKKUN_POISON1", 0x200004FA, "btl_e_pakkun_poison1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PAKKUN_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HEAVYB_ATTACK1", 0x2000041E, "btl_e_heavyb_att1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HEAVYB_ATTACK2", 0x2000041F, "btl_e_bomb_spark1_22k", 120, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HEAVYB_ATTACK3", 0x20000126, "btl_e_heavyb_att3_22k", 120, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HEAVYB_WAIT1", 0x20000127, "btl_e_bomb_ready1_22k", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HEAVYB_WAIT2", 0x20000420, "btl_e_heavyb_fuse1_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HEAVYB_WAIT3", 0x20000421, "btl_e_heavyb_wait2_22k", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_HEAVYB_DAMAGED1", 0x2000040F, "btl_e_damaged_hard1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BOMB_MOVE1", 0x200003F8, "btl_e_bomb_move1_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BOMB_MOVE2", 0x200003F9, "btl_e_bomb_move2_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BOMB_MOVE3", 0x200003FA, "btl_e_bomb_move3_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BOMB_MOVE4", 0x200003FB, "btl_e_bomb_move4_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BOMB_SPARK1", 0x2000041F, "btl_e_bomb_spark1_22k", 120, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BOMB_ATTACK2", 0x200003F7, "btl_e_bomb_att1_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BOMB_WAIT1", 0x20000127, "btl_e_bomb_ready1_22k", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BOMB_DAMAGED1", 0x2000040F, "btl_e_damaged_hard1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BORODO_MOVE1L", 0x200004C6, "btl_e_borodo_move1l_22k", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BORODO_MOVE1R", 0x200004C7, "btl_e_borodo_move1r_22k", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BORODO_INCLINE1", 0x200004C3, "btl_e_borodo_incline1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BORODO_SUCCESS1", 0x200004C8, "btl_e_borodo_success1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BORODO_WAIT1", 0x200004C9, "btl_e_borodo_wait1_22k", 60, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BORODO_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BORODO_ATT1", 0x200004C1, "btl_e_borodo_att1_22k", 115, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BORODO_LAUGH1", 0x200004C5, "btl_e_borodo_laugh1_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BORODO_JUMP1", 0x200004C4, "btl_e_borodo_jump1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_BORODO_ESCAPE1", 0x200004C2, "btl_e_borodo_escape1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MAHO_CHARGE1", 0x20000439, "btl_e_charge1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MAHO_MAGIC1", 0x2000043A, "btl_e_maho_magic1_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MAHO_MAGIC2", 0x2000043B, "btl_e_maho_magic2_22k", 65, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MAHO_MAGIC3", 0x2000043C, "btl_e_maho_magic3_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MAHO_MAGIC4", 0x2000043D, "btl_e_maho_magic4_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MAHO_ATTACK1", 0x20000436, "btl_e_maho_att1_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MAHO_ATTACK1_2", 0x20000437, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MAHO_ATTACK2", 0x20000438, "btl_e_maho_att2_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MAHO_OFFSHOOT1", 0x2000042E, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MAHO_OFFSHOOT2", 0x2000042F, "", 95, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MAHO_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MET_ATTACK1", 0x200004DA, "btl_e_met_att1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MET_ATTACK2", 0x200004DB, "btl_e_met_att2_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MET_MOVE1L", 0x200004E0, "btl_e_met_move1l_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MET_MOVE1R", 0x200004E1, "btl_e_met_move1r_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MET_MOVE2", 0x200004E2, "btl_e_met_move2_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MET_FALL1", 0x200004DD, "btl_e_met_fall1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MET_JUMP1", 0x200004DE, "btl_e_met_jump1_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MET_DAMAGED1", 0x2000040F, "btl_e_damaged_hard1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MET_LANDING1", 0x200004DF, "btl_e_met_landing1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MET_SHELL1", 0x200004E3, "btl_e_met_shell1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEMET_LANDING1", 0x2000050E, "btl_e_togemet_landing1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATAMET_MOVE1", 0x200004FB, "btl_e_patamet_move1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATAMET_MOVE2", 0x200004FC, "btl_e_patamet_move2_22k", 115, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATAMET_ATTACK1", 0x20000000, "btl_e_patamet_att1_22k", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_PATAMET_WAIT1", 0x200004FD, "btl_e_patamet_wait1_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_WAN_ATTACK1", 0x2000045E, "btl_e_wan_att1_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_WAN_ATTACK2", 0x2000045F, "btl_e_wan_att2_22k", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_WAN_ATTACK3", 0x20000460, "btl_e_wan_att3_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_WAN_WAIT1", 0x20000462, "btl_e_wan_wait1_22k", 10, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_WAN_WAIT2", 0x20000463, "btl_e_wan_wait2_22k", 95, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_WAN_WAIT3", 0x20000464, "btl_e_wan_wait3_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_WAN_WAIT4", 0x20000465, "btl_e_wan_wait4_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_WAN_DAMAGED1", 0x2000040F, "btl_e_damaged_hard1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_WAN_LANDING1", 0x20000461, "btl_e_wan_landing1_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MONBAN_MOVE1", 0x200004E5, "btl_e_monban_move1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MONBAN_MOVE2", 0x20000000, "btl_e_monban_move2_22k", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MONBAN_MOVE2L", 0x200004E6, "btl_e_monban_move2ｌ_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MONBAN_MOVE2R", 0x200004E7, "btl_e_monban_move2ｒ_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MONBAN_MOVE3", 0x200004E8, "btl_e_monban_move3_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MONBAN_THROW1", 0x200004E9, "btl_e_monban_throw1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MONBAN_THROW2", 0x200004EA, "btl_e_monban_throw2_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MONBAN_LAUGH1", 0x200004E4, "btl_e_monban_raugh_22k", 75, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MONBAN_WAIT1", 0x200004EB, "btl_e_monban_wait1_22k", 70, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_MONBAN_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KAMEKU_MAGIC1", 0x20000427, "btl_e_kameku_magic1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KAMEKU_MAGIC2", 0x20000000, "btl_e_kameku_magic2_22k", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KAMEKU_MAGIC3", 0x20000428, "btl_e_kameku_magic3_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KAMEKU_MAGIC3_2", 0x20000429, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KAMEKU_MAGIC4", 0x2000042A, "btl_e_kameku_magic4_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KAMEKU_MAGIC6", 0x2000042B, "btl_e_kameku_magic6_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KAMEKU_MAGIC7", 0x2000042C, "btl_e_kameku_magic7_22k", 85, 64, 0, 0, 0, 0, 0x555},
	{"SFX_ENM_KAMEKU_MAGIC8", 0x2000042D, "btl_e_kameku_magic7_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KAMEKU_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KAMEKU_OFFSHOOT1", 0x2000042E, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_KAMEKU_OFFSHOOT2", 0x2000042F, "", 95, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_JUGEM_ATTACK1", 0x200004D4, "btl_e_jugem_att1_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_JUGEM_ATTACK2", 0x200004D5, "btl_e_jugem_att2_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_JUGEM_THROW1", 0x200004D7, "btl_e_jugem_throw1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_JUGEM_THROW2", 0x200004D8, "btl_e_jugem_throw2_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_JUGEM_HOLD1", 0x200004D5, "btl_e_jugem_att2_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_JUGEMU_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_JUGEM_REBOUND1", 0x200004D6, "btl_e_jugem_rebound1_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_JUGEM_JUMP1", 0x2000051A, "btl_e_togezo_jump1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEZO_TURN1", 0x20000522, "btl_e_togezo_turn1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEZO_TURN2", 0x20000523, "btl_e_togezo_turn2_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEZO_JUMP1", 0x2000051A, "btl_e_togezo_jump1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEZO_ROLL1", 0x2000051D, "btl_e_togezo_roll1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEZO_ROLL2", 0x2000051E, "btl_e_togezo_roll2_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEZO_ROLL3", 0x2000051F, "btl_e_togezo_roll3_22k", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEZO_ROLL4", 0x20000520, "btl_e_togezo_roll4_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEZO_ROLL5", 0x20000521, "btl_e_togezo_roll5_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEZO_MOVE1L", 0x2000051B, "btl_e_togezo_move1ｌ_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEZO_MOVE1R", 0x2000051C, "btl_e_togezo_move1ｒ_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"SFX_ENM_TOGEZO_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KANBU_MOVE1L", 0x200004B0, "", 105, 64, 0, 0, 0, 0, 0xFD55},
	{"SFX_BOSS_KANBU_MOVE1R", 0x200004B1, "", 105, 64, 0, 0, 0, 0, 0xFD55},
	{"SFX_BOSS_KANBU_JUMP1", 0x200004AB, "", 105, 64, 0, 0, 0, 0, 0xFD55},
	{"SFX_BOSS_KANBU_HIP_ATTACK1", 0x200004A3, "", 105, 64, 0, 0, 0, 0, 0xFD55},
	{"SFX_BOSS_KANBU_HIP_ATTACK2", 0x200004AE, "", 105, 64, 0, 0, 0, 0, 0xFD55},
	{"SFX_BOSS_KANBU_CALL_BACK1", 0x2000050F, "", 105, 64, 0, 0, 0, 0, 0xFD55},
	{"SFX_BOSS_KANBU_CALL_ON1", 0x2000050F, "", 105, 64, 0, 0, 0, 0, 0xFD55},
	{"SFX_BOSS_KANBU_GUARD1", 0x200000FB, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KANBU_GUARD2", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KANBU_DAMAGED1", 0x20000411, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GESSO_APPEAR1", 0x200000D2, "", 100, 64, 0, 15, 0, 20, 0x0},
	{"SFX_BOSS_GESSO_WINK1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GESSO_ARM_APPEAR1", 0x200000D3, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_BOSS_GESSO_ARM_GOOUT1", 0x20000120, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_BOSS_GESSO_ARM_MOVE1", 0x200004A1, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_BOSS_GESSO_ATTACK1", 0x2000044C, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_BOSS_GESSO_ATTACK2", 0x2000044C, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_BOSS_GESSO_FALL1", 0x20000552, "", 115, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BOSS_GESSO_DOWN1", 0x2000016A, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_BOSS_GESSO_WAKE1", 0x200000D5, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_BOSS_GESSO_WAKE2", 0x20000121, "", 100, 64, 0, 15, 0, 0, 0x0},
	{"SFX_BOSS_GESSO_UP1", 0x200000D5, "", 110, 64, 0, 15, 0, 0, 0x0},
	{"SFX_BOSS_GESSO_SUMI1", 0x20000004, "SE2_BOS_GESSO_SUMI1", 115, 64, 0, 15, 0, 0, 0x555},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GESSO_DAMAGED1", 0x20000411, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_G_CHORO_MOVE1", 0x20000407, "btl_e_choro_move1_22k", 75, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_G_CHORO_ATTACK1", 0x20000407, "btl_e_choro_move1_22k", 75, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_G_CHORO_WAIT1", 0x2000040B, "btl_e_choro_wait1_22k", 65, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_G_CHORO_WAIT2", 0x2000040C, "btl_e_choro_wait2_22k", 65, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_G_CHORO_LANDING1", 0x20000406, "btl_e_choro_landing1_22k", 65, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_G_CHORO_DAMAGED1", 0x20000404, "btl_e_choro_damaged1_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_TEAM_CHORO_FALL1", 0x20000407, "", 80, 64, 0, 0, 100, 0, 0x0},
	{"SFX_BOSS_TEAM_CHORO_FALL2", 0x200000C6, "", 120, 64, 0, 0, 100, 10, 0x0},
	{"SFX_BOSS_TEAM_CHORO_TOGETHER1", 0x20000407, "", 60, 64, 0, 0, 100, 0, 0x0},
	{"SFX_BOSS_TEAM_CHORO_WAIT1", 0x20000407, "", 60, 64, 0, 0, 100, 0, 0x0},
	{"SFX_BOSS_TEAM_CHORO_MOVE1", 0x20000407, "", 60, 64, 0, 0, 100, 0, 0x0},
	{"SFX_BOSS_TEAM_CHORO_ATTACK1", 0x20000407, "", 100, 64, 0, 0, 100, 0, 0x0},
	{"SFX_BOSS_TEAM_CHORO_DAMAGED1", 0x20000404, "", 85, 64, 0, 0, 100, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GNB_APPEAR1", 0x200000D7, "SE2_BOS_GNB_STEP1", 120, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_GNB_WAIT1", 0x20000000, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_GNB_NECK_UP1", 0x200000E6, "", 110, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_GNB_BREATH_FIRE1", 0x200000E4, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_GNB_BREATH_POISON1", 0x200000E7, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_GNB_BREATH_COLD1", 0x200000DD, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_GNB_BREATH_CHANGE1", 0x200000DC, "", 95, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_GNB_MOVE1", 0x200000D7, "SE2_BOS_GNB_MOVE1", 110, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_GNB_ARM_UP1", 0x200000D9, "", 110, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_GNB_ARM_DOWN1", 0x200000D8, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_GNB_BITE1", 0x200000DA, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_GNB_BITE2", 0x200000DB, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_GNB_CHARGE1", 0x200000E6, "", 100, 64, 0, 20, 0, 0, 0x555},
	{"SFX_BOSS_GNB_DOWN1", 0x200000DE, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_GNB_DOWN2", 0x200000DF, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_GNB_DOWN3", 0x200000E0, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GNB_EAT_APPEAR1", 0x200000E2, "", 120, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BOSS_GNB_EAT1", 0x200000E3, "SE2_BOS_GNB_EAT1", 90, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GNB_DAMAGED1", 0x20000411, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MAJO_SHADOW1", 0x20000242, "SE4_BOS_MAJO_SHADOW1", 110, 64, 0, 0, 0, 0, 0x555},
	{"SFX_BOSS_MAJO_SHADOW2", 0x200002F1, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MAJO_LAUGH1", 0x20000000, "SE2_BOS_MAJO_LAUGH1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MAJO_PUSH1", 0x200000F9, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MAJO_PUSH2", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MAJO_FINGER1", 0x20000000, "SE2_BOS_MAJO_FINGER1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MAJO_STOOP1", 0x20000000, "SE2_BOS_MAJO_FINGER1", 0, 64, 0, 0, 0, 5, 0x0},
	{"SFX_BOSS_MAJO_HAND_OPEN1", 0x200000F9, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MAJO_ATTACK_ICE1", 0x2000024A, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MAJO_DAMAGED1", 0x20000412, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MARI_SHADOW1", 0x20000242, "SE4_BOS_MARI_SHADOW1", 110, 64, 0, 0, 0, 0, 0xFAAB},
	{"SFX_BOSS_MARI_SHADOW2", 0x200002F3, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MARI_HAND_CLAP1", 0x200000FA, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MARI_HAND_CLAP2", 0x20000000, "SE2_BOS_MARI_HAND_CLAP2", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MARI_POWER1", 0x200000FA, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MARI_HAND_OPEN1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MARI_ARM_FOLD1", 0x200000FB, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MARI_CHARGE1", 0x200000FA, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MARI_ATTACK_THUNDER1", 0x20000250, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MARI_DAMAGED1", 0x20000412, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_VIVI_SHADOW1", 0x20000242, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_VIVI_SHADOW2", 0x2000034D, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_VIVI_ATTACK4", 0x2000034F, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_VIVI_MAGIC1", 0x20000350, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_VIVI_DAMAGED1", 0x20000412, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_PUNCH_ACTIVE1", 0x20000107, "", 100, 64, 0, 50, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_PUNCH_STOP1", 0x20000108, "", 100, 64, 0, 50, 0, 0, 0x800},
	{"SFX_BOSS_MBTN_PUNCH_IGNITE1", 0x2000041D, "btl_e_firling1_22k", 120, 64, 0, 50, 0, 0, 0xFAAB},
	{"SFX_BOSS_MBTN_PUNCH_FLY1", 0x20000433, "btl_e_killer_move1_22k", 110, 64, 0, 50, 0, 0, 0x800},
	{"SFX_BOSS_MBTN_PUNCH_HIT1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_MOVE1", 0x20000106, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_LEG_UP1", 0x20000105, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_LEG_PERPLEX1", 0x20000104, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_STEPON1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_QUAKE1", 0x200000D7, "SE2_BOS_GNB_STEP1", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_CAILING1", 0x200001E8, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_DAMAGED1", 0x2000040F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_DRILL_ATTACK1", 0x200000FF, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_DRILL_ATTACK2", 0x20000100, "btl_e_elmos_charge1_22k", 110, 64, 0, 0, 0, 15, 0x0},
	{"SFX_BOSS_MBTN_DRILL_ATTACK3", 0x20000101, "btl_e_uranoko_att3_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_BOOME_DIVIDE1", 0x20000508, "btl_e_togeda_move1_22k", 110, 64, 0, 0, 0, 0, 0xF800},
	{"SFX_BOSS_MBTN_BOOME_ROUND1", 0x200000FE, "btl_e_barri_att8_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_BOOME_ACTIVE1", 0x200000FC, "btl_e_barri_split2_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_BOOME_MOVE1", 0x200000FD, "btl_e_uranoko_att3_22k", 115, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_JUMP1", 0x20000100, "btl_e_elmos_charge1_22k", 90, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_FALL1", 0x20000102, "btl_e_elmos_att1_22k", 110, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_MOTOR_ACTIVE1", 0x200001D8, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_INHALE1", 0x20000053, "", 120, 64, 0, 50, 0, 0, 0x555},
	{"SFX_BOSS_MBTN_CHEST_OPEN1", 0x20000508, "btl_e_togeda_move1_22k", 110, 64, 0, 0, 0, 0, 0xFAAB},
	{"SFX_BOSS_MBTN_SEARCH1", 0x40046262, "", 80, 64, 0, 0, 0, 0, 0x555},
	{"SFX_BOSS_MBTN_LOCKON1", 0x40046363, "btl_e_barri_att6_22k", 80, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MBTN_MACHINEGUN1", 0x2000041D, "btl_e_firling1_22k", 110, 64, 0, 50, 0, 0, 0x800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_ISIN_MOVE1", 0x20000417, "btl_e_emon_walkl1_22k", 100, 64, 0, 0, 0, 0, 0xF800},
	{"SFX_BOSS_ISIN_MOVE2", 0x20000418, "btl_e_emon_walkr2_22k", 100, 64, 0, 0, 0, 0, 0xF800},
	{"SFX_BOSS_ISIN_MOVE3", 0x20000415, "btl_e_charge1_22k", 100, 64, 0, 0, 0, 0, 0xF800},
	{"SFX_BOSS_ISIN_JUMP1", 0x20000195, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_ISIN_DAMAGED1", 0x2000040F, "btl_e_damaged1_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BLED_JUMP1", 0x200000BE, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BLED_GRASP_CAILING1", 0x200004A0, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BLED_CHOP_ROUND1", 0x200000C1, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BLED_CHOP_LANDING1", 0x200000BF, "", 100, 64, 0, 0, 0, 0, 0xFAAB},
	{"SFX_BOSS_BLED_CHOP_FLY1", 0x200000BD, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BLED_CHOP_ATTACK1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BLED_MOVE1L", 0x200000BF, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BLED_MOVE1R", 0x200000C0, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BLED_LANDING1", 0x200000BF, "", 100, 64, 0, 0, 0, 0, 0xFAAB},
	{"SFX_BOSS_BLED_MOVE_CAILING1", 0x200001B8, "", 100, 64, 0, 10, 0, 0, 0x555},
	{"SFX_BOSS_BLED_DROP_KICK1", 0x200000BD, "", 120, 64, 0, 0, 0, 0, 0xF800},
	{"SFX_BOSS_BLED_FUMI_FALL1", 0x200000BC, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BLED_FUMI_STEPON1", 0x200000BB, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BLED_SHAKE_CAILING1", 0x200000C2, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BLED_EAT1", 0x200000BA, "", 110, 64, 0, 0, 0, 5, 0x0},
	{"SFX_BOSS_BLED_LAUGH1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BLED_DIE1", 0x200000B9, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BLED_DAMAGED1", 0x20000411, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_PAUSING1", 0x200000CF, "", 107, 64, 0, 0, 0, 0, 0xFAAB},
	{"SFX_BOSS_GANSU_PAUSING2", 0x200000CE, "", 125, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_PAUSING3", 0x200000CE, "", 125, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_PAUSING4", 0x200000CE, "", 125, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_PAUSING5", 0x200000CE, "", 125, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_PAUSING6", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_PAUSING7", 0x200000D0, "", 125, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_PAUSING8", 0x200000D1, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_PAUSING9", 0x200000D1, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_JUMP1", 0x200000C1, "", 120, 64, 0, 0, 0, 0, 0xFAAB},
	{"SFX_BOSS_GANSU_BIG1", 0x200000C8, "", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_BIG2", 0x200000C9, "", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_BIG3", 0x200000CF, "SE2_BOS_GANSU_BIG3", 120, 64, 0, 0, 0, 0, 0xF556},
	{"SFX_BOSS_GANSU_FIST1", 0x200000CC, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_ATTACK1", 0x200000C7, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_ARM_SHAKE1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_HIPDROP1", 0x200000D7, "SE2_BOS_GANSU_HIPDROP1", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_DOWN1", 0x200000B9, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_MOVE1L", 0x200000BF, "", 115, 64, 0, 1, 0, 0, 0xFAAB},
	{"SFX_BOSS_GANSU_MOVE1R", 0x200000C0, "", 115, 64, 0, 1, 0, 0, 0xFAAB},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GANSU_DAMAGED1", 0x20000411, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_RNPL_FLY1", 0x2000055C, "", 90, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_RNPL_STOP1", 0x20000000, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_RNPL_FALL1", 0x2000055B, "", 107, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_RNPL_DOWN1", 0x200004AC, "btl_e_3rd_miss1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_RNPL_LAUGH1", 0x20000119, "", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_RNPL_ARM_UP1", 0x20000118, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_RNPL_EYE_SHINE1", 0x200001F4, "", 110, 64, 0, 20, 0, 0, 0x555},
	{"SFX_BOSS_RNPL_TRANSFORM1", 0x200004BC, "SE2_BOS_RNPL_TRANSFORM1", 75, 64, 0, 0, 0, 8, 0x0},
	{"SFX_BOSS_RNPL_TRANSFORM2", 0x2000011B, "", 75, 64, 0, 0, 0, 8, 0x0},
	{"SFX_BOSS_RNPL_TRANSFORM3", 0x2000011B, "", 105, 64, 0, 0, 0, 8, 0xFAAB},
	{"SFX_BOSS_RNPL_TRANSFORM4", 0x200001A0, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_RNPL_LANDING1", 0x2000055F, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_RNPL_MOVE1L", 0x2000055D, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_RNPL_MOVE1R", 0x2000055E, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_RNPLTRANSFORM5", 0x200002D2, "", 95, 64, 0, 0, 0, 20, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_RNPL_DAMAGED1", 0x20000411, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_VOICE_NM_AC1_1", 0x200002CC, "", 125, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_VOICE_NM_AC1_2", 0x200002CD, "", 115, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_VOICE_NM_AC1_5", 0x200002CE, "", 125, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_VOICE_NM_AC2_1", 0x200002CF, "", 125, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_VOICE_NM_AC2_2", 0x200002D0, "", 125, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_VOICE_NM_AC2_4", 0x200002D1, "", 115, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_NM_MOVE1L", 0x20000399, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_NM_MOVE1R", 0x2000039A, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_NM_LANDING1", 0x20000387, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_NM_DOWN1", 0x2000021A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_NM_JUMP1", 0x2000021E, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_NM_HAMMER_FURU2", 0x20000383, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_NM_HAMMER_DON2", 0x20000380, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_NM_HAMMER_KAITEN1", 0x20000000, "SE4_FLD_M_HMMR_KAITEN1", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_NM_HAMMER_KAITEN2", 0x20000000, "SE4_FLD_M_HMMR_KAITEN2", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_NM_JUMP_KURURIN1", 0x2000037D, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_NM_DOWN2", 0x2000021A, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_NM_DOWN3", 0x20000130, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_ATMTLSA_MOVE1", 0x2000049F, "SE2_BOS_ATMTLSA_MOVE1", 115, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_ATMTLSA_MOVE2", 0x2000049F, "SE2_BOS_ATMTLSA_MOVE2", 115, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_ATMTLSA_MOVE3", 0x20000000, "SE2_BOS_ATMTLSA_STEPON1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_ATMTLSA_FACE_HIDE1", 0x2000049E, "btl_e_telesa_hide1_22k", 115, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_ATMTLSA_SURPRISED1", 0x20000467, "", 115, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_ATMTLSA_DIVIDE1", 0x2000045A, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_ATMTLSA_S_MOVE1", 0x2000045B, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_ATMTLSA_S_ROUND1", 0x200000B6, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_ATMTLSA_SURPRISED2", 0x20000459, "", 107, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_ATMTLSA_BLOW1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_ATMTLSA_BLOW2", 0x200001A5, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_ATMTLSA_WHISTLE1", 0x200000AF, "", 105, 64, 0, 0, 0, 0, 0x555},
	{"SFX_BOSS_ATMTLSA_UNION1", 0x200000B8, "", 127, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_ATMTLSA_DAMAGED1", 0x20000411, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KOR_SWORD_SHAKE1", 0x200000F6, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_SWORD_READY1", 0x200000ED, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_SWORD_STICK1", 0x200000F4, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_SWORD_FALL1", 0x40041919, "SE2_BOS_KOR_SWORD_FALL2", 75, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_HEAD_ATTACK1", 0x200000ED, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_HEAD_ATTACK2", 0x200000EE, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_MOVE1", 0x200000F2, "", 105, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_MOVE2", 0x200000F1, "", 105, 64, 0, 20, 0, 0, 0xFAAB},
	{"SFX_BOSS_KOR_WAIT1", 0x200000F8, "", 75, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_INHALE1", 0x200000EF, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_BONE_FLY1", 0x200000EC, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_RIB_COUNTER1", 0x200000F0, "", 127, 64, 0, 20, 0, 10, 0x0},
	{"SFX_BOSS_KOR_BONE_FALL1", 0x200000EB, "", 110, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_MOVE3", 0x200000F2, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_SWORD_DANCE1", 0x200000F4, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_SWORD_DANCE2", 0x200000F5, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_SWORD_DANCE3", 0x200000F6, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_BITE1", 0x200000EA, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_ROUND1", 0x200000F3, "", 127, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_SWORD_FALL2", 0x200000F7, "", 110, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_KOR_INHALE2", 0x40046767, "", 110, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KOR_DAMAGED1", 0x2000040F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_TEAM_3RD_MOVE1L", 0x200004B0, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_TEAM_3RD_MOVE1R", 0x200004B1, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_TEAM_3RD_ATTACK1", 0x200004AB, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_TEAM_3RD_ATTACK1_2", 0x200004AC, "btl_e_3rd_miss1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_TEAM_3RD_WAKE1", 0x200004AE, "", 100, 64, 0, 0, 0, 0, 0x555},
	{"SFX_BOSS_TEAM_3RD_MOVE_CAILING1", 0x200001B8, "", 110, 64, 0, 10, 0, 0, 0x555},
	{"SFX_BOSS_TEAM_3RD_MOVE_CAILING2", 0x400B8B8B, "", 95, 64, 0, 0, 0, 20, 0x0},
	{"SFX_BOSS_TEAM_3RD_MOVE_CAILING3", 0x200000C2, "", 110, 64, 0, 20, 0, 0, 0xFAAB},
	{"SFX_BOSS_TEAM_3RD_ATTACK2", 0x20000558, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_TEAM_3RD_MOVE2", 0x400B8A8A, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_TEAM_3RD_ADHERE1", 0x400B8989, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_TEAM_3RD_ADHERE2", 0x400B8C8C, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_TEAM_3RD_ROUND1", 0x20000261, "", 105, 64, 0, 10, 0, 20, 0x800},
	{"SFX_BOSS_TEAM_3RD_ESCAPE1", 0x400B8D8D, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_TEAM_3RD_DAMAGED1", 0x20000411, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MOA_G_ARM_MOVE1", 0x20000109, "", 95, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BOSS_MOA_G_ARM_MOVE2", 0x2000010A, "", 107, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BOSS_MOA_G_ARM_MOVE3", 0x2000010C, "", 107, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BOSS_MOA_G_ARM_MOVE2_2", 0x2000010B, "", 107, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BOSS_MOA_G_ARM_MOVE3_2", 0x2000010D, "", 107, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BOSS_MOA_G_ARM_MOVE4", 0x2000010E, "", 107, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BOSS_MOA_G_ARM_MOVE5", 0x2000010F, "", 107, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BOSS_MOA_G_ARM_DOWN1", 0x40045555, "", 110, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BOSS_MOA_G_ARM_DOWN2", 0x40045656, "", 107, 64, 0, 30, 0, 10, 0x0},
	{"SFX_BOSS_MOA_G_BIG1", 0x40045858, "", 90, 64, 0, 30, 0, 5, 0x0},
	{"SFX_BOSS_MOA_G_BIG2", 0x200004D9, "btl_e_charge1_22k", 90, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BOSS_MOA_G_ARM_RESTORE1", 0x40045959, "", 120, 64, 0, 60, 0, 0, 0x0},
	{"SFX_BOSS_MOA_G_ARM_RESTORE2", 0x40045454, "", 120, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BOSS_MOA_G_LUMP1", 0x40045555, "", 110, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BOSS_MOA_G_EXPLOSION1", 0x40045757, "", 110, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_MOA_G_DAMAGED1", 0x20000411, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KOOPA_JUMP1", 0x20000024, "", 127, 64, 0, 0, 0, 10, 0x0},
	{"SFX_BOSS_KOOPA_HIP_ATTACK1", 0x20000026, "", 120, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KOOPA_MOVE1L", 0x20000049, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KOOPA_MOVE1R", 0x2000004A, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KOOPA_ROAR1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KOOPA_BITE1", 0x200000DA, "", 100, 64, 0, 0, 0, 0, 0x555},
	{"SFX_BOSS_KOOPA_BITE2", 0x200000DB, "", 100, 64, 0, 0, 0, 0, 0x555},
	{"SFX_BOSS_KOOPA_BITE3", 0x200000DB, "", 107, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KOOPA_FIRE_BREATH1", 0x20000202, "SE2_BTL_STG_FIRE1", 127, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KOOPA_DAMAGED1", 0x20000411, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KAMEBABA_MAGIC1", 0x20000427, "btl_e_kameku_magic1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KAMEBABA_MAGIC2", 0x200001F4, "", 100, 64, 0, 70, 0, 0, 0x555},
	{"SFX_BOSS_KAMEBABA_MAGIC3", 0x20000428, "btl_e_kameku_magic3_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KAMEBABA_MAGIC3_2", 0x20000429, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KAMEBABA_MAGIC4", 0x2000042A, "btl_e_kameku_magic4_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KAMEBABA_MAGIC6", 0x2000042B, "btl_e_kameku_magic6_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KAMEBABA_MAGIC7", 0x2000042C, "btl_e_kameku_magic7_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KAMEBABA_MAGIC8", 0x2000042D, "btl_e_kameku_magic7_22k", 85, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KAMEBABA_MOVE1L", 0x20000206, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_KAMEBABA_MOVE1R", 0x20000207, "", 70, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_KAMEBABA_MOVE2", 0x20000032, "evt_k_kamebaba_fly1_16k", 105, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KAMEBABA_FALL1", 0x20000000, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_KAMEBABA_DOWN1", 0x20000203, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_KAMEBABA_DAMAGED1", 0x20000411, "btl_e_damaged_normal1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SRYO_STUFF_PISECE1", 0x20000428, "btl_e_kameku_magic3_22k", 90, 64, 0, 0, 0, 5, 0x0},
	{"SFX_BOSS_SRYO_STUFF_PUTOUT1", 0x20000427, "btl_e_kameku_magic1_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SRYO_STUFF_PUTOUT2", 0x2000011E, "", 127, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SRYO_THUNDER1", 0x20000250, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SRYO_FIRE1", 0x20000202, "SE2_BTL_STG_FIRE1", 115, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SRYO_ICE1", 0x2000011D, "", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SRYO_STOP1", 0x2000024F, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SRYO_BARRIER_BREAK1", 0x20000141, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SRYO_LIGHT_ROUND1", 0x200004BC, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SRYO_SPLIT1", 0x200004BD, "btl_e_barri_split1_22k", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SRYO_SPLIT2", 0x200004BE, "btl_e_barri_split2_22k", 110, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SATELLITE_ATTACK1", 0x2000044C, "btl_e_pkuri_move3_22k", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SATELLITE_ATTACK2", 0x2000044C, "btl_e_pkuri_move3_22k", 105, 64, 0, 0, 0, 0, 0xFAAB},
	{"SFX_BOSS_SATELLITE_DAMAGED1", 0x2000040F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SRYO_DAMAGED1", 0x20000411, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SRYO_MOVE1L", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_SRYO_MOVE1R", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BPCH_THUNDER1", 0x20000250, "", 115, 64, 0, 45, 0, 0, 0xF800},
	{"SFX_BOSS_BPCH_DRAIN1", 0x20000114, "", 110, 64, 0, 45, 0, 0, 0x0},
	{"SFX_BOSS_BPCH_DRAIN2", 0x200001F0, "", 115, 64, 0, 45, 0, 0, 0xD55},
	{"SFX_BOSS_BPCH_DARK_POWER1", 0x200000C3, "", 110, 64, 0, 45, 0, 0, 0x0},
	{"SFX_BOSS_BPCH_HAND_UP1", 0x200000C4, "", 110, 64, 0, 45, 0, 0, 0x0},
	{"SFX_BOSS_BPCH_QWEN_APPEAR1", 0x200000C5, "", 80, 64, 0, 45, 0, 0, 0x0},
	{"SFX_BOSS_BPCH_HAND_APPEAR1", 0x40046868, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_BPCH_HAND_IN1", 0x20000000, "", 0, 64, 0, 60, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BPCH_QWEN_APPEAR2", 0x40046969, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BPCH_DAMAGED1", 0x20000411, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_EYE_SHINE1", 0x200001F4, "", 110, 64, 0, 45, 0, 0, 0xFD55},
	{"SFX_BOSS_QWEN_THUNDER1", 0x20000250, "", 115, 64, 0, 45, 0, 0, 0xF800},
	{"SFX_BOSS_QWEN_DARK_POWER1", 0x20000112, "", 110, 64, 0, 45, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_DARK_CHERGE1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_DARK_WAVE1", 0x40046A6A, "", 115, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_DARK_WAVE2", 0x200002F5, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_DARK_WAVE3", 0x20000113, "", 115, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_BREATH1", 0x20000110, "", 115, 64, 0, 45, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_BREATH2", 0x20000111, "", 90, 64, 0, 70, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_DRAIN1", 0x20000114, "", 110, 64, 0, 45, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_DRAIN2", 0x200001F0, "", 115, 64, 0, 45, 0, 0, 0xD55},
	{"SFX_BOSS_QWEN_HAND_UP1", 0x20000117, "", 110, 64, 0, 45, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_HAND_DOWN1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_HANDS_MOVE1", 0x20000116, "", 110, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BOSS_QWENH_HAND_APPEAR1", 0x40046868, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_HAND_IN1", 0x20000000, "", 0, 64, 0, 50, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_HAND_APPEAR1_2", 0x40046B6B, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_HAND_GOOUT1", 0x20000115, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_QWENH_HAND_APPEAR2", 0x40046C6C, "", 110, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_QWEN_DAMAGED1", 0x20000411, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AUDIENCE_BOO_ALLY1", 0x20000085, "", 110, 64, 0, 10, 30, 0, 0x0},
	{"SFX_AUDIENCE_BOO_ALLY2", 0x20000086, "", 110, 64, 0, 10, 30, 0, 0x0},
	{"SFX_AUDIENCE_BOO_ALLY3", 0x20000087, "", 110, 64, 0, 10, 30, 0, 0x0},
	{"SFX_AUDIENCE_BOO_ALLY4", 0x20000088, "", 110, 64, 0, 10, 30, 0, 0x0},
	{"SFX_AUDIENCE_BOO_ENEMY1", 0x20000089, "", 110, 64, 0, 10, 30, 0, 0x0},
	{"SFX_AUDIENCE_BOO_ENEMY2", 0x2000008A, "", 110, 64, 0, 10, 30, 0, 0x0},
	{"SFX_AUDIENCE_BOO_ENEMY3", 0x2000008B, "", 110, 64, 0, 10, 30, 0, 0x0},
	{"SFX_AUDIENCE_BOO_ENEMY4", 0x2000008C, "", 110, 64, 0, 10, 30, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AUDIENCE_CLAPPING1", 0x20000099, "", 110, 64, 0, 0, 30, 0, 0x0},
	{"SFX_AUDIENCE_CLAPPING2", 0x2000009A, "", 110, 64, 0, 0, 30, 0, 0x0},
	{"SFX_AUDIENCE_CLAPPING3", 0x2000009B, "", 110, 64, 0, 0, 30, 0, 0x0},
	{"SFX_AUDIENCE_CLAPPING4", 0x2000009C, "", 110, 64, 0, 0, 30, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AUDIENCE_NOISY1", 0x2000009F, "", 107, 64, 0, 0, 30, 0, 0x0},
	{"SFX_AUDIENCE_NOISY2", 0x200000A0, "", 107, 64, 0, 0, 30, 0, 0x0},
	{"SFX_AUDIENCE_NOISY3", 0x200000A1, "", 107, 64, 0, 0, 30, 0, 0x0},
	{"SFX_AUDIENCE_NOISY4", 0x200000A2, "", 107, 64, 0, 0, 30, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AUDIENCE_CHEER_ALLY1", 0x20000091, "", 112, 64, 0, 0, 20, 0, 0x0},
	{"SFX_AUDIENCE_CHEER_ALLY2", 0x20000092, "", 112, 64, 0, 0, 20, 0, 0x0},
	{"SFX_AUDIENCE_CHEER_ALLY3", 0x20000093, "", 112, 64, 0, 0, 20, 0, 0x0},
	{"SFX_AUDIENCE_CHEER_ALLY4", 0x20000094, "", 112, 64, 0, 0, 20, 0, 0x0},
	{"SFX_AUDIENCE_CHEER_ENEMY1", 0x20000095, "", 112, 64, 0, 0, 20, 0, 0x0},
	{"SFX_AUDIENCE_CHEER_ENEMY2", 0x20000096, "", 112, 64, 0, 0, 20, 0, 0x0},
	{"SFX_AUDIENCE_CHEER_ENEMY3", 0x20000097, "", 112, 64, 0, 0, 20, 0, 0x0},
	{"SFX_AUDIENCE_CHEER_ENEMY4", 0x20000098, "", 112, 64, 0, 0, 20, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AUDIENCE_WHISTLE1", 0x200000AF, "", 100, 64, 0, 10, 10, 0, 0x0},
	{"SFX_AUDIENCE_WHISTLE2", 0x200000B0, "", 100, 64, 0, 10, 10, 0, 0x0},
	{"SFX_AUDIENCE_WHISTLE3", 0x200000B1, "", 100, 64, 0, 10, 10, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AUDIENCE_CALL_ALLY1", 0x2000008D, "", 110, 64, 0, 10, 20, 0, 0x0},
	{"SFX_AUDIENCE_CALL_ALLY2", 0x2000008E, "", 110, 64, 0, 10, 20, 0, 0x0},
	{"SFX_AUDIENCE_CALL_ENEMY1", 0x2000008F, "", 110, 64, 0, 10, 20, 0, 0x0},
	{"SFX_AUDIENCE_CALL_ENEMY2", 0x20000090, "", 110, 64, 0, 10, 20, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AUDIENCE_HANDBEAT1", 0x2000009D, "", 110, 64, 0, 50, 10, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AUDIENCE_PANIC_SCREAM1", 0x40047171, "SE2_AUD_PANIC_SCRMLP2", 105, 64, 0, 0, 0, 0, 0x0},
	{"SFX_AUDIENCE_PANIC_SCREAM2", 0x200000A4, "", 70, 64, 0, 10, 0, 0, 0x0},
	{"SFX_AUDIENCE_PANIC_SCREAM3", 0x200000A5, "", 70, 64, 0, 10, 0, 0, 0x0},
	{"SFX_AUDIENCE_PANIC_RUN1", 0x200000A7, "", 70, 64, 0, 10, 0, 0, 0x0},
	{"SFX_AUDIENCE_PANIC_RUN2", 0x200000A7, "", 60, 64, 0, 10, 0, 0, 0xFAAB},
	{"SFX_AUDIENCE_THROW_OBJECT1", 0x200000AD, "", 70, 64, 0, 10, 30, 0, 0x555},
	{"SFX_AUDIENCE_THROW_OBJECT2", 0x200000AE, "", 70, 64, 0, 10, 30, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AUDIENCE_TELESA_INVISIBLE1", 0x20000128, "", 70, 64, 0, 10, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AUDIENCE_RUN1", 0x200000A7, "", 60, 64, 0, 0, 0, 0, 0x0},
	{"SFX_AUDIENCE_RUN2", 0x200000A7, "", 60, 64, 0, 0, 0, 0, 0xFAAB},
	{"SFX_AUDIENCE_RUN3", 0x200000A8, "", 50, 64, 0, 10, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AUDIENCE_GNB_PANIC1", 0x40043A3A, "", 105, 64, 0, 10, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AUDIENCE_SLEEP1", 0x200000AA, "", 70, 64, 0, 10, 40, 0, 0x0},
	{"SFX_AUDIENCE_SLEEP2", 0x200000AB, "", 70, 64, 0, 10, 40, 0, 0x0},
	{"SFX_AUDIENCE_SLEEP3", 0x200000AC, "", 70, 64, 0, 10, 40, 0, 0x0},
	{"SFX_AUDIENCE_ZZZ1", 0x200000B2, "", 70, 64, 0, 10, 40, 0, 0x0},
	{"SFX_AUDIENCE_PANSY_SINGING1", 0x20000445, "", 70, 64, 0, 10, 40, 0, 0x0},
	{"SFX_AUDIENCE_SHELL_TRANSFORM1", 0x200004F6, "SE2_AUD_SHELL_TRFM1", 70, 64, 0, 10, 40, 0, 0x0},
	{"SFX_AUDIENCE_BOMB_READY1", 0x20000127, "", 70, 64, 0, 0, 40, 0, 0x0},
	{"SFX_AUDIENCE_BOMB_BURST1", 0x20000126, "", 90, 64, 0, 10, 40, 0, 0x0},
	{"SFX_AUDIENCE_PAKU_EAT1", 0x200000A3, "", 70, 64, 0, 10, 40, 0, 0x0},
	{"SFX_AUDIENCE_HEIHO_MOVE1", 0x2000009E, "", 70, 64, 0, 0, 40, 0, 0x0},
	{"SFX_AUDIENCE_TELESA_MOVE1", 0x2000045B, "btl_e_telesa_move1_22k", 70, 64, 0, 10, 40, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SMOKE1", 0x2000014F, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_STAGE_BURST1", 0x2000013D, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BTL_STAGE_FIRE1", 0x20000147, "", 100, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_STAGE_DRYICE1", 0x20000000, "", 0, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BTL_STAGE_DROP_OBJECT1", 0x20000144, "", 80, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_STAGE_DAMAGE_NORMAL1", 0x20000142, "", 80, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BTL_STAGE_DAMAGE_GRASS1", 0x20000141, "", 80, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BTL_STAGE_DAMAGE_TUB1", 0x20000143, "", 80, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BTL_STAGE_DAMAGE_CHIN1", 0x20000140, "", 80, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BTL_STAGE_DAMAGE_FORK1", 0x2000013E, "", 80, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BTL_STAGE_FALL_BUG1", 0x20000145, "", 80, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BTL_STAGE_FALL_BUG2", 0x20000146, "", 80, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BTL_STAGE_FALL_WATER1", 0x2000007E, "", 90, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_STAGE_FALL_METEOR1", 0x20000260, "", 127, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BTL_STAGE_DAMAGE_METEOR2", 0x20000261, "", 110, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_STAGE_FALL_KOOPA1", 0x20000260, "", 127, 64, 0, 30, 0, 0, 0x0},
	{"SFX_BTL_STAGE_DAMAGE_LIGHT1", 0x2000013F, "", 70, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_DOWN1", 0x20000149, "", 95, 64, 0, 10, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_DOWN2", 0x20000148, "", 95, 64, 0, 10, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_DOWN3", 0x2000014A, "", 95, 64, 0, 10, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_DOWN_COMBI1", 0x20000000, "SE2_BTL_STG_SET_DOWN2", 0, 64, 0, 10, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_DOWN_COMBI2", 0x20000000, "SE2_BTL_STG_SET_DOWN3", 0, 64, 0, 10, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_FALL1", 0x2000014B, "", 70, 64, 0, 10, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_FALL2", 0x2000014C, "", 70, 64, 0, 10, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_FALL3", 0x2000014D, "", 70, 64, 0, 10, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_FALL_COMBI1", 0x20000000, "SE2_BTL_STG_SET_FALL1", 0, 64, 0, 10, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_FALL_COMBI2", 0x20000000, "SE2_BTL_STG_SET_FALL3", 0, 64, 0, 10, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_SHAKE1", 0x2000014E, "", 70, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_MOVE1", 0x20000000, "SE2_STG_SET_MOVE1", 0, 64, 0, 10, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_MOVE_FINISH1", 0x20000000, "SE2_STG_SET_MV_FSH1", 0, 64, 0, 10, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_TURN1", 0x20000000, "SE2_STG_SET_TURN1", 0, 64, 0, 10, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_TURN_FINISH1", 0x20000000, "SE2_STG_SET_TRN_FSH1", 0, 64, 0, 10, 0, 0, 0x0},
	{"SFX_BTL_STAGE_CURTAIN_RISE", 0x20000000, "SE2_STG_CERTAIN_RISE1", 0, 64, 0, 10, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_STAGE_SET_WALL_DOWN1", 0x20000000, "", 0, 64, 0, 10, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_KUR_ATTACK1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_TGK_ATTACK1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_PTK_ATTACK1", 0x20000000, "SE1_ENEMY_PATK_ATT_A1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_PTK_ATTACK1B", 0x20000000, "SE1_ENEMY_PATK_ATT_B1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NKT_ATTACK1", 0x20000000, "SE1_ENEMY_NOKO_KAITEN1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NKT_ATTACK2", 0x20000000, "SE1_ENEMY_NOKO_SHOT1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NKT_REVERSE1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_NKT_WAKE1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_PATA_ATTACK1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SIS_ATTACK1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CBN_JUMP1", 0x20000000, "SE2_CHOROBON2", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CBN_MOVE1", 0x20000000, "SE1_ENEMY_CHORO_MOVE1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CBN_ATTACK1", 0x20000000, "SE1_ENEMY_CHORO_ATT_A1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CBN_ATTACK1B", 0x20000000, "SE1_ENEMY_CHORO_ATT_B1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CBN_ADD1", 0x20000000, "SE1_ENEMY_CHORO_FUERU1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CBN_ATTACK2", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_HNN_CHARGE1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_HNN_ATTACK1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_TGD_MOVE1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_TGD_ATTACK1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_KRN_ATTACK1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_KRN_ADD1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_KRN_ADD2", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_KRN_DIE1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_GNB_MOVE1", 0x20000000, "SE2_BTL_GNB_MOVE1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_GNB_UP1", 0x20000000, "SE2_BTL_GNB_UP1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_GNB_STEP1", 0x20000000, "SE2_BTL_GNB_STEP1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_GNB_EAT1", 0x20000000, "SE2_BTL_GNB_EAT1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_GNB_ROAR1", 0x20000000, "SE3_GNB_ROAR1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_GNB_FIRE1", 0x20000000, "SE1_FIRE1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SNB_ATTACK1", 0x20000000, "SE2_BTL_SMB_UP1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SNB_ATTACK2", 0x20000000, "SE1_SND_HIT1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SNB_ATTACK3", 0x20000000, "SE2_BTL_SMB_ATTACK3", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SNB_ATTACK4", 0x20000000, "SE2_BTL_SMB_ATTACK2", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SNB_ADD1", 0x20000000, "SE2_BTL_SMB_ADD1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SNB_ADD1_2", 0x20000000, "SE2_BTL_SMB_ADD1_2", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SNB_ADD1_3", 0x20000000, "SE2_BTL_SMB_ADD1_3", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SNB_ADD2", 0x20000000, "SE2_BTL_SMB_AP3", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SNB_H_MOVE1", 0x20000000, "SE2_BTL_SMB_MV1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_SNB_H_ATTACK1", 0x20000000, "SE2_BTL_SMB_ATTACK1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_WAN_STANDBY1", 0x20000000, "SE1_IWAWAN_GON1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_WAN_ATTACK1", 0x20000000, "SE1_IWAWAN_KUSARI1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_WAN_ATTACK2", 0x20000000, "SE1_IWAWAN_KAMU1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_WAN_STANDBY2", 0x20000000, "SE1_IWAWAN_GON1", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_WAN_STANDBY3", 0x20000000, "SE1_IWAWAN_KUSARI1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_CYU_ATTACK1", 0x20000000, "SE2_BSACHU_ATTACK1", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YWL_1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YWL_2", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_YWL_3", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_TYL_1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_TYL_2", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_TYL_3", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_MOA_1", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_MOA_2", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"SFX_BTL_MOA_3", 0x20000000, "", 0, 64, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_NOIZE1", 0x20000001, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_NOIZE2", 0x20000008, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_TEST_SE2", 0x20000000, "SE1_GRASS_SHAKE1", 0, 64, 0, 20, 0, 0, 0x0},
	{"SFX_TEST_SE3", 0x20000000, "SE1_TREE_SHAKE1", 0, 64, 0, 20, 0, 0, 0x0},
	{"SFX_TEST_SE4", 0x20000000, "SE1_NORMAL_SPIN1", 0, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_RIVER1", 0x20000002, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_RIVER1_2", 0x20000002, "", 100, 64, 0, 20, 0, 20, 0x2AB},
	{"SFX_AMB_RIVER1_3", 0x20000002, "", 100, 64, 0, 20, 0, 40, 0x555},
	{"SFX_AMB_RIVER2", 0x20000010, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_RIVER3", 0x20000002, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_SEA1", 0x20000003, "", 100, 64, 0, 20, 0, 0, 0x2AB},
	{"SFX_AMB_SEA2", 0x2000000F, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_ENV_SHIP1", 0x20000004, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_ENV_SHIP2", 0x20000012, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_ENV_SEA_GULL1", 0x20000000, "", 0, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_SHIP_CREAK1", 0x20000076, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_JUNGLE1", 0x20000011, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_JUNGLE2", 0x20000000, "", 0, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_JUNGLE3", 0x20000000, "", 0, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_CAVE1", 0x20000013, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_CAVE2", 0x20000000, "", 0, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_TRAIN1", 0x20000005, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_TRAIN2", 0x20000006, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_ENV_TABLE2A", 0x20000014, "", 100, 64, 0, 20, 0, 0, 0x2AB},
	{"SFX_ENV_TABLE2B", 0x20000015, "", 100, 64, 0, 20, 0, 0, 0x2AB},
	{"SFX_ENV_WHEEL1", 0x20000000, "", 0, 64, 0, 20, 0, 0, 0x0},
	{"SFX_ENV_WHEEL2", 0x20000000, "", 0, 64, 0, 20, 0, 0, 0x0},
	{"SFX_ENV_MOAMOA1", 0x20000000, "", 0, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_RUINS1", 0x20000000, "", 0, 64, 0, 20, 0, 0, 0x0},
	{"SFX_ENV_WATER1", 0x2000000A, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_ENV_WATER2", 0x20000000, "", 0, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_WIND1", 0x20000007, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_WIND2", 0x2000000B, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_WIND3", 0x20000016, "", 40, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_BIRD1", 0x2000000C, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_BIRD2", 0x2000000D, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_BIRD3", 0x2000000E, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_BIRD4", 0x200001A1, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_BIRD5", 0x200001A1, "", 100, 64, 0, 20, 0, 0, 0xFAAB},
	{"SFX_BGM_RSH1", 0x20000000, "", 0, 64, 0, 20, 0, 0, 0x0},
	{"SFX_BGM_RSH2", 0x20000000, "", 0, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_NIGHT1", 0x20000000, "SE1_AMB_NIGHT1", 0, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_NIGHT2", 0x20000000, "SE1_AMB_NIGHT2", 0, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_WOLF", 0x20000000, "", 0, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_DUMMY", 0x20000000, "SE2_AMB_DUMMY", 0, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_COMPUTER1", 0x20000058, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_COMPUTER2", 0x20000059, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_AIRSHIP_IDOL1", 0x20000053, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_TRAIN_IDOL1", 0x2000007B, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_WATER3", 0x20000010, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_PAD1", 0x20000073, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_AMB_PAD2", 0x20000000, "SE2_AMB_PAD2", 0, 64, 0, 60, 0, 0, 0x0},
	{"SFX_AMB_PAD3", 0x20000000, "SE2_AMB_PAD3", 0, 64, 0, 60, 0, 0, 0x0},
	{"SFX_AMB_LEAF1", 0x2000006B, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_AMB_LEAF2", 0x2000006C, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_AMB_FOREST1", 0x20000061, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_AMB_WATER_WOOD1", 0x2000007D, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_AMB_WATER_WOOD2", 0x2000007E, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_AMB_LIGHT1", 0x2000006D, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_OBJ_WELCOME1", 0x20000072, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_NOISY1", 0x20000050, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_AUDIENCE1", 0x2000004E, "", 120, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_AUDIENCE2", 0x2000004F, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_AIRDUCT1", 0x20000052, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_SEWAGE1", 0x20000000, "", 0, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_WOLF_HOWL1", 0x20000084, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_FIRE1", 0x20000000, "SE2_AMB_FIRE1", 0, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_EKI_CREAK1", 0x2000005A, "", 100, 64, 0, 40, 0, 0, 0x0},
	{"SFX_AMB_EKI_CREAK2", 0x2000005B, "", 100, 64, 0, 40, 0, 0, 0x0},
	{"SFX_AMB_EKI_CREAK3", 0x2000005C, "", 100, 64, 0, 40, 0, 0, 0x0},
	{"SFX_AMB_EKI_WIND1", 0x2000005E, "", 100, 64, 0, 40, 0, 0, 0x0},
	{"SFX_AMB_EKI_WIND2", 0x2000005F, "", 100, 64, 0, 40, 0, 0, 0x0},
	{"SFX_AMB_EKI_WIND3", 0x2000005E, "", 100, 64, 0, 50, 0, 0, 0xF800},
	{"SFX_AMB_EKI_WIND4", 0x2000005F, "", 100, 64, 0, 50, 0, 0, 0xF800},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_GEAR1", 0x20000064, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_GEAR2", 0x20000065, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_GEAR3", 0x20000066, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_GEAR4", 0x20000067, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_GEAR5", 0x20000068, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_WIND4", 0x2000007F, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_WIND5", 0x20000080, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_WIND6", 0x2000007F, "", 100, 64, 0, 20, 0, 0, 0xF000},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_WINDVANE1", 0x20000081, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_MOON1", 0x20000000, "SE2_AMB_MOON1", 0, 64, 0, 50, 0, 0, 0x1000},
	{"SFX_AMB_MOON2", 0x2000006F, "", 100, 64, 0, 50, 0, 0, 0x0},
	{"SFX_AMB_AJI_LIGHT1", 0x20000054, "", 100, 64, 0, 50, 0, 0, 0x0},
	{"SFX_AMB_METEOR1", 0x2000006E, "", 100, 64, 0, 50, 0, 0, 0x0},
	{"SFX_AMB_AIR1", 0x20000051, "", 100, 64, 0, 30, 0, 0, 0x0},
	{"SFX_AMB_FACTORY1", 0x20000060, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_LAS1", 0x20000069, "", 100, 64, 0, 50, 0, 0, 0x0},
	{"SFX_AMB_LAS2", 0x2000006A, "", 100, 64, 0, 50, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_FOUNTAIN1", 0x20000062, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_FOUNTAIN2", 0x20000063, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_SINDEN1", 0x20000077, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_AMB_SINDEN2", 0x20000078, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_AMB_SINDEN3", 0x20000079, "", 100, 64, 0, 80, 0, 0, 0x0},
	{"SFX_AMB_SINDEN4", 0x20000079, "", 100, 64, 0, 80, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_100DAN1", 0x2000004D, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"SFX_AMB_DIM1", 0x2000005D, "", 100, 64, 0, 60, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_WIND_GNB1", 0x20000082, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_WIND_GNB2", 0x20000083, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_AMB_RIVER_NOK1", 0x20000074, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"SFX_AMB_RIVER_NOK2", 0x20000075, "", 100, 64, 0, 20, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_STG3_GANSU_JUMP1", 0x200000C1, "", 0, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_GNB_MEGABREATH1", 0x200002EE, "", 120, 64, 0, 20, 0, 0, 0xF000},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_50_HOTEL_SHUTTER1", 0x200002EF, "", 90, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_EVT_MAP_KAKAGERU1_3", 0x40047272, "", 100, 64, 0, 1, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"SFX_BOSS_BPCH_DIE1", 0x200001EF, "", 110, 64, 0, 60, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
	{"", 0x00000000, "", 0, 0, 0, 0, 0, 0, 0x0},
};

SoundEnvList psenvlist[325] = {
    { 0x0001, 0x102E, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0001, 0x102F, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0001, 0x1030, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0001, 0x1045, 65, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0001, 0x1046, 65, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0002, 0x1013, 75, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0003, 0x1013, 45, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0004, 0x102A, 45, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0005, 0x102A, 45, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0005, 0x1083, 95, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x43C80000, 20, 50, 18 },
    { 0x0006, 0x102A, 50, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0006, 0x1076, 80, 0, 1, 2, 1, 0x43AF0000, 0x00000000, 0x43C80000, 20, 50, 7 },
    { 0x0007, 0x103E, 95, 0, 1, 2, 1, 0x43B18000, 0x42F00000, 0xC3B00000, 20, 50, 18 },
    { 0x0007, 0x103F, 95, 0, 1, 2, 1, 0x42480000, 0x42C80000, 0xC1A00000, 20, 50, 18 },
    { 0x0007, 0x102A, 45, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0008, 0x1013, 85, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0009, 0x1009, 10, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x000A, 0x1082, 40, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x442F0000, 20, 50, 18 },
    { 0x000A, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x000B, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x000B, 0x103C, 45, 0, 3, 2, 1, 0xC3C60000, 0x41A00000, 0xC35C0000, 20, 50, 18 },
    { 0x000C, 0x1082, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x000C, 0x1009, 10, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x000D, 0x1009, 10, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x000E, 0x1009, 10, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x000F, 0x1009, 10, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x000F, 0x1082, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0010, 0x107C, 65, 0, 0, 3, 4, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0011, 0x1045, 60, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0011, 0x1046, 60, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0011, 0x102E, 60, 0, 0, 8, 4, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0011, 0x102F, 60, 0, 0, 8, 4, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0011, 0x1030, 60, 0, 0, 8, 4, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0012, 0x1045, 65, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0012, 0x1046, 65, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0012, 0x1083, 125, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x43480000, 20, 50, 18 },
    { 0x0012, 0x102E, 60, 0, 0, 8, 4, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0012, 0x102F, 60, 0, 0, 8, 4, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0012, 0x1030, 60, 0, 0, 8, 4, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0013, 0x1082, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0013, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0014, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0015, 0x1046, 65, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0015, 0x100E, 85, 0, 1, 2, 1, 0xC3130000, 0x00000000, 0x44078000, 20, 50, 18 },
    { 0x0015, 0x1083, 85, 0, 1, 2, 1, 0xC3890000, 0x00000000, 0x43EE8000, 20, 50, 18 },
    { 0x0015, 0x1010, 85, 0, 1, 2, 1, 0x43848000, 0x00000000, 0x43FA0000, 20, 50, 18 },
    { 0x0015, 0x102E, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0015, 0x102F, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0015, 0x1030, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0016, 0x1045, 65, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0016, 0x1046, 65, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0016, 0x1083, 90, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x43A80000, 20, 50, 18 },
    { 0x0016, 0x102E, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0016, 0x102F, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0016, 0x1030, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0017, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0018, 0x1082, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0018, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0019, 0x107F, 95, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0019, 0x1080, 70, 0, 0, 7, 5, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x001A, 0x107F, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001A, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001A, 0x1080, 40, 0, 0, 7, 5, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x001B, 0x1042, 35, 0, 0, 8, 5, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001B, 0x1045, 60, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001B, 0x1046, 60, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001B, 0x1047, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001C, 0x1009, 10, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001D, 0x1048, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001D, 0x1045, 55, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001D, 0x1047, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001E, 0x1042, 35, 0, 0, 8, 5, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001E, 0x1045, 55, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001E, 0x1049, 65, 0, 1, 2, 1, 0xC3FA0000, 0x00000000, 0xC3140000, 20, 50, 8 },
    { 0x001E, 0x1049, 65, 0, 1, 2, 1, 0xC35A0000, 0x00000000, 0xC3930000, 20, 50, 8 },
    { 0x001E, 0x1047, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001F, 0x1048, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001F, 0x1045, 50, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x001F, 0x1049, 65, 0, 1, 2, 1, 0x42DC0000, 0x00000000, 0xC2480000, 20, 50, 8 },
    { 0x001F, 0x1047, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0020, 0x1048, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0020, 0x1045, 50, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0020, 0x104A, 45, 0, 1, 2, 1, 0xC2500000, 0x00000000, 0x42440000, 20, 50, 8 },
    { 0x0020, 0x1047, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0021, 0x1048, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0021, 0x1045, 50, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0021, 0x104A, 45, 0, 1, 2, 1, 0xC2920000, 0x00000000, 0x420C0000, 20, 50, 8 },
    { 0x0021, 0x1047, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0022, 0x1048, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0022, 0x1045, 50, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0022, 0x1049, 65, 0, 1, 2, 1, 0x00000000, 0xC37A0000, 0x41B00000, 20, 50, 18 },
    { 0x0022, 0x1047, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0023, 0x1048, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0023, 0x1045, 50, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0023, 0x1049, 65, 0, 1, 2, 1, 0xC2D80000, 0x00000000, 0xC30C0000, 20, 50, 8 },
    { 0x0023, 0x1047, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0024, 0x1048, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0024, 0x1045, 50, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0024, 0x1049, 65, 0, 1, 2, 1, 0x42D60000, 0x00000000, 0xC31E0000, 20, 50, 8 },
    { 0x0024, 0x1047, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0025, 0x1048, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0025, 0x1045, 50, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0025, 0x1047, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0026, 0x1048, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0026, 0x100E, 80, 0, 1, 2, 1, 0xC1700000, 0x42540000, 0x42AA0000, 20, 50, 10 },
    { 0x0026, 0x1047, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0027, 0x103E, 75, 0, 1, 2, 1, 0xC1700000, 0x42540000, 0x42AA0000, 20, 50, 18 },
    { 0x0027, 0x102A, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0027, 0x104D, 95, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0028, 0x104D, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0029, 0x104E, 90, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x002A, 0x104E, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x002B, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x002C, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x002D, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x002D, 0x1051, 85, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x002E, 0x102A, 70, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x002F, 0x1065, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x002F, 0x1053, 100, 0, 0, 6, 4, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x002F, 0x105B, 120, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x002F, 0x105C, 120, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x0030, 0x1067, 70, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0030, 0x1053, 100, 0, 0, 6, 4, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0030, 0x1045, 45, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0030, 0x105B, 127, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x0030, 0x105C, 127, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x0031, 0x100A, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0031, 0x107D, 60, 0, 0, 3, 7, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0032, 0x1045, 65, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0032, 0x1067, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0032, 0x105B, 120, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x0032, 0x105C, 120, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x0032, 0x107D, 40, 0, 0, 3, 7, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0033, 0x100A, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0033, 0x107D, 60, 0, 0, 3, 7, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0034, 0x1013, 75, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0034, 0x1016, 70, 0, 1, 2, 1, 0x42C80000, 0x00000000, 0xC2C80000, 20, 50, 0 },
    { 0x0035, 0x1016, 70, 0, 1, 2, 1, 0x42C80000, 0x00000000, 0xC2C80000, 20, 50, 0 },
    { 0x0035, 0x1013, 75, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0036, 0x1014, 85, 0, 1, 2, 1, 0x43918000, 0xC1700000, 0x44160000, 20, 50, 18 },
    { 0x0036, 0x1013, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0037, 0x1014, 75, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x44480000, 20, 50, 18 },
    { 0x0037, 0x1013, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0038, 0x1019, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0038, 0x1083, 100, 0, 1, 2, 1, 0x431A0000, 0x00000000, 0x42B00000, 20, 50, 12 },
    { 0x0039, 0x1019, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x003A, 0x1013, 75, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x003A, 0x102A, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x003A, 0x1014, 75, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x003B, 0x1019, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x003B, 0x1083, 80, 0, 1, 2, 1, 0x44110000, 0x00000000, 0x42E20000, 20, 50, 12 },
    { 0x003C, 0x1016, 65, 0, 1, 2, 1, 0xC2B40000, 0x00000000, 0xC2480000, 20, 50, 18 },
    { 0x003C, 0x1013, 45, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x003D, 0x1016, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x003D, 0x1013, 35, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x003E, 0x101D, 70, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x44480000, 20, 50, 20 },
    { 0x003E, 0x1082, 55, 0, 1, 2, 1, 0xC2C80000, 0x00000000, 0x430C0000, 20, 50, 18 },
    { 0x003E, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x003F, 0x101D, 75, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x44480000, 20, 50, 20 },
    { 0x003F, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0040, 0x101D, 70, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x44480000, 20, 50, 20 },
    { 0x0040, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0040, 0x1013, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0041, 0x101D, 70, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x44480000, 20, 50, 20 },
    { 0x0041, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0041, 0x1013, 45, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0041, 0x1015, 115, 0, 1, 2, 1, 0x00000000, 0x00000000, 0xC2C80000, 20, 50, 12 },
    { 0x0042, 0x101D, 70, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x44480000, 20, 50, 20 },
    { 0x0042, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0042, 0x1082, 55, 0, 1, 2, 1, 0x00000000, 0x438C0000, 0x00000000, 20, 50, 12 },
    { 0x0042, 0x1015, 65, 0, 1, 2, 1, 0x42FA0000, 0x438C0000, 0xC3480000, 20, 50, 18 },
    { 0x0043, 0x101D, 70, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x44480000, 20, 50, 20 },
    { 0x0043, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0043, 0x1082, 45, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0044, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0044, 0x1013, 40, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0044, 0x1015, 35, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0045, 0x101D, 70, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x44480000, 20, 50, 20 },
    { 0x0045, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0045, 0x1013, 45, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0045, 0x1015, 95, 0, 1, 2, 1, 0x00000000, 0x00000000, 0xC3480000, 20, 50, 18 },
    { 0x0046, 0x101D, 80, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x44480000, 20, 50, 20 },
    { 0x0046, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0047, 0x101D, 70, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x44480000, 20, 50, 20 },
    { 0x0047, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0047, 0x1013, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0048, 0x101D, 70, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x44480000, 20, 50, 20 },
    { 0x0048, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0048, 0x1013, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0048, 0x1016, 65, 0, 1, 2, 1, 0x42A00000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0049, 0x101D, 70, 0, 1, 2, 1, 0x00000000, 0x00000000, 0x44480000, 20, 50, 20 },
    { 0x0049, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0049, 0x1013, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0049, 0x1014, 45, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0049, 0x1016, 50, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x004A, 0x101F, 75, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x004B, 0x101F, 75, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x004B, 0x1021, 65, 0, 2, 2, 1, 0x43BB0000, 0x41F00000, 0xC3650000, 20, 50, 8 },
    { 0x004B, 0x1022, 65, 0, 2, 2, 1, 0x43440000, 0x41F00000, 0xC3650000, 20, 50, 8 },
    { 0x004C, 0x101F, 80, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x004D, 0x101F, 80, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x004D, 0x103F, 60, 0, 1, 2, 1, 0xC29A0000, 0x00000000, 0xC34B0000, 20, 50, 18 },
    { 0x004E, 0x1020, 100, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x004F, 0x1031, 85, 0, 0, 6, 1, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x004F, 0x1032, 85, 0, 0, 6, 1, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x004F, 0x102A, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0050, 0x1056, 75, 0, 0, 6, 4, 0x00000000, 0x00000000, 0x00000000, 80, 50, 18 },
    { 0x0050, 0x1057, 75, 0, 0, 6, 4, 0x00000000, 0x00000000, 0x00000000, 80, 50, 18 },
    { 0x0050, 0x1058, 75, 0, 0, 6, 4, 0x00000000, 0x00000000, 0x00000000, 80, 50, 18 },
    { 0x0050, 0x1059, 105, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x0050, 0x105A, 105, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x0050, 0x1065, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0051, 0x1056, 75, 0, 0, 6, 4, 0x00000000, 0x00000000, 0x00000000, 80, 50, 18 },
    { 0x0051, 0x1057, 75, 0, 0, 6, 4, 0x00000000, 0x00000000, 0x00000000, 80, 50, 18 },
    { 0x0051, 0x1058, 75, 0, 0, 6, 4, 0x00000000, 0x00000000, 0x00000000, 80, 50, 18 },
    { 0x0051, 0x1059, 105, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0051, 0x105A, 105, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0051, 0x1065, 50, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0051, 0x105F, 35, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0052, 0x105F, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0052, 0x1060, 75, 0, 1, 2, 1, 0xC2A60000, 0x430C0000, 0x42880000, 20, 50, 18 },
    { 0x0052, 0x1061, 75, 0, 1, 2, 1, 0xC2F60000, 0x43420000, 0xC0000000, 20, 50, 18 },
    { 0x0052, 0x1062, 75, 0, 1, 2, 1, 0x42780000, 0x430C0000, 0x42040000, 20, 50, 18 },
    { 0x0052, 0x1063, 75, 0, 1, 2, 1, 0x43420000, 0x430C0000, 0x42040000, 20, 50, 18 },
    { 0x0052, 0x1060, 75, 0, 1, 2, 1, 0x43C68000, 0x430C0000, 0xC0C00000, 20, 50, 18 },
    { 0x0053, 0x1031, 105, 0, 0, 6, 4, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x0053, 0x1032, 105, 0, 0, 6, 4, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x0053, 0x102A, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0054, 0x102E, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0054, 0x102F, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0054, 0x1030, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0055, 0x1076, 80, 0, 1, 2, 1, 0xC3270000, 0x00000000, 0x43270000, 20, 50, 15 },
    { 0x0055, 0x102E, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0055, 0x102F, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0055, 0x1030, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0056, 0x1076, 80, 0, 1, 2, 1, 0xC2860000, 0x00000000, 0x43AB8000, 20, 50, 15 },
    { 0x0056, 0x102E, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0056, 0x102F, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0056, 0x1030, 65, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0057, 0x1077, 30, 0, 0, 2, 4, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x0057, 0x1079, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0058, 0x1078, 30, 0, 0, 2, 4, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x0058, 0x107A, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0059, 0x1066, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0059, 0x1059, 90, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0059, 0x105A, 90, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x005A, 0x1066, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x005A, 0x1059, 90, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x005A, 0x105A, 90, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x005A, 0x1069, 70, 0, 1, 2, 1, 0xC3AB8000, 0x42A00000, 0xC38C0000, 20, 50, 6 },
    { 0x005A, 0x1069, 70, 0, 1, 2, 1, 0x43AB8000, 0x42A00000, 0xC38C0000, 20, 50, 6 },
    { 0x005B, 0x1066, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x005B, 0x1059, 90, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x005B, 0x105A, 90, 0, 0, 5, 5, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x005B, 0x1069, 70, 0, 1, 2, 1, 0x42F00000, 0x42A00000, 0xC3480000, 20, 50, 6 },
    { 0x005C, 0x106C, 85, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x005D, 0x106D, 85, 0, 1, 2, 1, 0x433D0000, 0x42960000, 0xC3FA8000, 20, 50, 18 },
    { 0x005D, 0x106C, 70, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x005E, 0x106E, 30, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x005E, 0x106C, 50, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x005F, 0x106C, 70, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0060, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0060, 0x106F, 70, 0, 0, 3, 2, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0060, 0x1070, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0061, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0061, 0x1070, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0062, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0062, 0x1070, 75, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0063, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0063, 0x1070, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0063, 0x103D, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0064, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0064, 0x1070, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0064, 0x103C, 95, 0, 1, 2, 1, 0x437E0000, 0x42200000, 0x3F800000, 20, 50, 12 },
    { 0x0065, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0065, 0x1070, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0065, 0x1048, 45, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0066, 0x1009, 20, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0066, 0x1070, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0066, 0x105F, 50, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0067, 0x1009, 15, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0067, 0x1072, 90, 0, 2, 5, 7, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0068, 0x1009, 15, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0068, 0x1082, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0068, 0x1072, 90, 0, 2, 5, 7, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0069, 0x1009, 15, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0069, 0x1082, 75, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0069, 0x100E, 115, 0, 1, 2, 1, 0xC3E60000, 0xC1700000, 0x43160000, 20, 50, 18 },
    { 0x0069, 0x1083, 115, 0, 1, 2, 1, 0x00000000, 0xC1700000, 0x43820000, 20, 50, 18 },
    { 0x0069, 0x1010, 115, 0, 1, 2, 1, 0x43E60000, 0xC1700000, 0x43160000, 20, 50, 18 },
    { 0x0069, 0x1072, 90, 0, 2, 5, 7, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x006A, 0x1009, 15, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x006B, 0x105F, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x006B, 0x1060, 75, 0, 1, 2, 1, 0x43AC8000, 0x42340000, 0xC3310000, 20, 50, 18 },
    { 0x006B, 0x1060, 75, 0, 1, 2, 1, 0xC3B40000, 0x42AA0000, 0xC3200000, 20, 50, 18 },
    { 0x006C, 0x105F, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x006C, 0x1060, 75, 0, 1, 2, 1, 0xC40D0000, 0x43660000, 0xC3200000, 20, 50, 18 },
    { 0x006C, 0x1060, 75, 0, 1, 2, 1, 0x43400000, 0xC2D80000, 0xC3240000, 20, 50, 18 },
    { 0x006C, 0x1063, 75, 0, 1, 2, 1, 0xC3870000, 0xC40E8000, 0xC2140000, 20, 50, 18 },
    { 0x006C, 0x1060, 75, 0, 1, 2, 1, 0x439C0000, 0xC4004000, 0x40C00000, 20, 50, 18 },
    { 0x006D, 0x105F, 55, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x006D, 0x1060, 75, 0, 1, 2, 1, 0xC3E20000, 0xC1B00000, 0xC30C0000, 20, 50, 18 },
    { 0x006D, 0x1060, 75, 0, 1, 2, 1, 0x43720000, 0xC3210000, 0xC3180000, 20, 50, 18 },
    { 0x006D, 0x1063, 75, 0, 1, 2, 1, 0x4417C000, 0xC3C50000, 0xC3250000, 20, 50, 18 },
    { 0x006E, 0x1009, 15, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x006F, 0x1009, 15, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x006F, 0x1073, 60, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0070, 0x1067, 85, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0070, 0x107F, 95, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0070, 0x1080, 70, 0, 0, 7, 5, 0x00000000, 0x00000000, 0x00000000, 50, 50, 18 },
    { 0x0071, 0x1013, 75, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0072, 0x1016, 65, 0, 1, 2, 1, 0x42C80000, 0x00000000, 0xC2C80000, 20, 50, 0 },
    { 0x0072, 0x1013, 65, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0073, 0x1013, 75, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0073, 0x1016, 70, 0, 1, 2, 1, 0x42C80000, 0x00000000, 0xC2C80000, 20, 50, 0 },
    { 0x0074, 0x102E, 40, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0074, 0x102F, 40, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0074, 0x1030, 40, 0, 0, 7, 3, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0075, 0x1045, 75, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0075, 0x1046, 75, 0, 0, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0075, 0x1083, 125, 0, 3, 2, 1, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0075, 0x102E, 70, 0, 0, 8, 4, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0075, 0x102F, 70, 0, 0, 8, 4, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0075, 0x1030, 70, 0, 0, 8, 4, 0x00000000, 0x00000000, 0x00000000, 20, 50, 18 },
    { 0x0076, 0x0000, 0, 0, 0, 0, 0, 0x00000000, 0x00000000, 0x00000000, 0, 0, 0 },
};

const char* psenvlistname[] = {
	"",
	"ENV_OPN_AAA1",
	"ENV_OPN_MUJ1",
	"ENV_STG0_GOR0", "ENV_STG0_GOR1", "ENV_STG0_GOR3", "ENV_STG0_GOR4", "ENV_STG0_GOR5", "ENV_STG0_GOR6",
	"ENV_STG0_TIK1", "ENV_STG0_TIK2", "ENV_STG0_TIK3", "ENV_STG0_TIK4", "ENV_STG0_TIK5", "ENV_STG0_TIK6", "ENV_STG0_TIK7",
	"ENV_STG0_DAN1",
	"ENV_STG1_HEI1", "ENV_STG1_HEI2", "ENV_STG1_HEI3", "ENV_STG1_HEI4",
	"ENV_STG1_NOK1", "ENV_STG1_NOK2",
	"ENV_STG1_GON1", "ENV_STG1_GON2", "ENV_STG1_GON3", "ENV_STG1_GON4",
	"ENV_STG2_WIN1", "ENV_STG2_WIN2",
	"ENV_STG2_MRI1", "ENV_STG2_MRI2", "ENV_STG2_MRI3", "ENV_STG2_MRI4", "ENV_STG2_MRI5",
	"ENV_STG2_MRI6", "ENV_STG2_MRI7", "ENV_STG2_MRI8", "ENV_STG2_MRI9", "ENV_STG2_MRI10",
	"ENV_STG3_TOU1", "ENV_STG3_TOU2", "ENV_STG3_TOU3", "ENV_STG3_TOU4",
	"ENV_STG3_TOU5", "ENV_STG3_TOU6", "ENV_STG3_TOU7", "ENV_STG3_TOU8",
	"ENV_STG4_USU1",
	"ENV_STG4_GRA1",
	"ENV_STG4_JIN1", "ENV_STG4_JIN2", "ENV_STG4_JIN3",
	"ENV_STG5_MUJ1_1", "ENV_STG5_MUJ1_2", "ENV_STG5_MUJ2", "ENV_STG5_MUJ3", "ENV_STG5_MUJ4",
	"ENV_STG5_MUJ5", "ENV_STG5_MUJ6", "ENV_STG5_MUJ7", "ENV_STG5_MUJ8", "ENV_STG5_MUJ9",
	"ENV_STG5_DOU1", "ENV_STG5_DOU2", "ENV_STG5_DOU3", "ENV_STG5_DOU4", "ENV_STG5_DOU5", "ENV_STG5_DOU6",
	"ENV_STG5_DOU7", "ENV_STG5_DOU8", "ENV_STG5_DOU9", "ENV_STG5_DOU10", "ENV_STG5_DOU11", "ENV_STG5_DOU12",
	"ENV_STG6_RSH1", "ENV_STG6_RSH1_2", "ENV_STG6_RSH1_3", "ENV_STG6_RSH1_4", "ENV_STG6_RSH2",
	"ENV_STG6_EKI1", "ENV_STG6_EKI2", "ENV_STG6_EKI2_2", "ENV_STG6_EKI3", "ENV_STG6_EKI4",
	"ENV_STG6_PIK1", "ENV_STG6_PIK1_1", "ENV_STG6_PIK1_2", "ENV_STG6_PIK2", "ENV_STG6_PIK3",
	"ENV_STG7_BOM1", "ENV_STG7_BOM2", "ENV_STG7_BOM3",
	"ENV_STG7_MOO1", "ENV_STG7_MOO2", "ENV_STG7_MOO3", "ENV_STG7_MOO4",
	"ENV_STG7_AJI1", "ENV_STG7_AJI2", "ENV_STG7_AJI3", "ENV_STG7_AJI4", "ENV_STG7_AJI5", "ENV_STG7_AJI6", "ENV_STG7_AJI7",
	"ENV_STG8_LAS1", "ENV_STG8_LAS2", "ENV_STG8_LAS3", "ENV_STG8_LAS4", "ENV_STG8_LAS5",
	"ENV_STG8_LAS6", "ENV_STG8_LAS7", "ENV_STG8_LAS8", "ENV_STG8_LAS9",
	"ENV_KOOPA_KPA1",
	"ENV_END_MUJ1", "ENV_END_MUJ2", "ENV_END_MUJ3",
	"ENV_END_AAA1",
	"ENV_SYS_FL1"
};

// Debug database path strings the original keeps in .rodata but nothing references.
#pragma force_active on
const char str_s_sound_proj_pmario_sound_bgm_txt_db[40] = "%s/sound/proj/pmario_sound_bgm_txt.db";
const char str_s_sound_proj_pmario_sound_env_txt_db[40] = "%s/sound/proj/pmario_sound_env_txt.db";
const char str_s_sound_proj_pmario_sound_sfx_txt_db[40] = "%s/sound/proj/pmario_sound_sfx_txt.db";
#pragma force_active reset

static f32 angleABTBL[] = {
	1.0f, 1.273187f, 1.27303f, 1.272768f, 1.272402f, 1.271932f, 1.271358f,
	1.270681f, 1.269902f, 1.269021f, 1.268038f, 1.266956f, 1.265774f, 1.2644939f,
	1.263116f, 1.2616431f, 1.260075f, 1.258413f, 1.256659f, 1.254815f, 1.2528811f,
	1.250859f, 1.248752f, 1.24656f, 1.244285f, 1.2419291f, 1.239494f, 1.236981f,
	1.234393f, 1.2317311f, 1.228997f, 1.226192f, 1.22332f, 1.220382f, 1.217379f,
	1.2143151f, 1.211189f, 1.208006f, 1.204766f, 1.201471f, 1.1981241f, 1.1947269f,
	1.191281f, 1.1877871f, 1.18425f, 1.180669f, 1.177047f, 1.173386f, 1.169687f,
	1.165952f, 1.162184f, 1.158384f, 1.1545531f, 1.1506931f, 1.146806f, 1.142893f,
	1.138957f, 1.134998f, 1.131018f, 1.127019f, 1.1230021f, 1.118969f, 1.11492f,
	1.110858f, 1.106783f, 1.102697f, 1.098601f, 1.094496f, 1.090384f, 1.086266f,
	1.082142f, 1.078014f, 1.0738831f, 1.06975f, 1.065616f, 1.061482f, 1.057348f,
	1.053216f, 1.049087f, 1.04496f, 1.040838f, 1.036721f, 1.0326101f, 1.028504f,
	1.024406f, 1.020316f, 1.016234f, 1.0121599f, 1.0080971f, 1.004043f, 1.0f
};

//psbgmlist

//.bss
PaperSoundBGM psbgm[2];
PaperSoundEffect pssfx[40];
PaperSoundEnv psenv[16];
PaperSoundWork psnd;
u8 _buf[0x80];

//.sbss (definitions follow the deferred functions)
extern BOOL _psndPushGroup_trg;
extern BOOL init_f;

//local definitions

//local declarations for symbols without a header prototype

// same layout as PaperSoundEffect but with unk6 as u16 so loads are lhz, not lha
typedef struct PaperSoundEffectU16_s {
    s32 unk0;
    u16 unk4;
    u16 unk6;
    u8 _pad[0x28 - 0x8];
} PaperSoundEffectU16;

extern int __float_nan[]; // no project header declares it (MSL float.h is off the include path)
typedef struct PaperSoundEnvWork {
	u8 unk0;   // 0x0
	u8 unk1;   // 0x1
	s16 unk2;  // 0x2
	s16 unk4;  // 0x4
	u8 unk6;   // 0x6
	u8 unk7;   // 0x7
	s16 unk8;  // 0x8
	s16 unkA;  // 0xa
} PaperSoundEnvWork;

// Prototypes for the forward references the original function order needs.
static inline s32 psndSFXFindSSChannel(void);
static inline s32 psndSFXFindSongChannel(void);

static f32 angleABf(f32 x1, f32 y1, f32 x2, f32 y2) {
	f32 xdiff, ydiff;
	f32 xabs, yabs;
	f32 v8;
	f32 v9;
	int v10;
	f32 v13;
	f32 v15;
	f32 test;

	xdiff = x2 - x1;
	ydiff = y2 - y1;
	xabs = __fabsf(xdiff);
	yabs = __fabsf(ydiff);
	if (xabs > yabs) {
		v8 = 45.0f * (yabs / xabs);
		v9 = 2.0f * v8;
		if (v9 >= 0.0f) {
			v10 = (int)(0.5 + v9);
		}
		else {
			v10 = -(int)(0.5 - v9);
		}
		test = v8 * angleABTBL[v10];
		if (xdiff >= 0.0f) {
			if (ydiff >= 0.0f) {
				v13 = 90.0f + test;
			}
			else {
				v13 = 90.0f - test;
			}
		}
		else {
			if (ydiff >= 0.0f) {
				v13 = 270.0f - test;
			}
			else {
				v13 = 270.0f + test;
			}
		}
	}
	else {
		if (yabs == 0.0f) {
			return 0.0f;
		}
		else {
			v15 = 45.0f * (xabs / yabs);
			v9 = 2.0f * v15;
			if (v9 >= 0.0f) {
				v10 = (int)(0.5 + v9);
			}
			else {
				v10 = -(int)(0.5 - v9);
			}
			test = v15 * angleABTBL[v10];
			if (ydiff >= 0.0f) {
				if (xdiff >= 0.0f) {
					v13 = 180.0f - test;
				}
				else {
					v13 = 180.0f + test;
				}
			}
			else {
				if (xdiff >= 0.0f) {
					v13 = 0.0f + test;
				}
				else {
					v13 = 360.0f - test;
				}
			}
		}
	}
	return v13;
}

//calc3D, returns the first bit of SND_3DINFO (vol, pan, span, unused)
// MWCC's inline for the MSL float sqrt: bit-classifies x and Newton-refines
// the reciprocal square root; a negative input becomes NaN.
static inline s32 psndFpClassifyf(f32 x) {
	switch (*(s32*)&x & 0x7F800000) {
	case 0x7F800000:
		if (*(s32*)&x & 0x007FFFFF) {
			return 1;
		}
		return 2;
	case 0:
		if (*(s32*)&x & 0x007FFFFF) {
			return 5;
		}
		return 3;
	default:
		return 4;
	}
}

static inline f32 psndSqrtf(f32 x) {
	const f64 _half = .5;
	const f64 _three = 3.0;

	if (x > 0.0f) {
		f64 guess = __frsqrte(x);

		guess = _half * guess * (_three - guess * guess * x);
		guess = _half * guess * (_three - guess * guess * x);
		guess = _half * guess * (_three - guess * guess * x);
		return x * guess;
	} else if (x < 0.0) {
		return *(f32*)__float_nan;
	} else if (psndFpClassifyf(x) == 1) {
		return *(f32*)__float_nan;
	}
	return x;
}

// -O4,p already emits the target's size-optimised stmw/lmw prologue and, with the SDK math.h
// suppressed (see the top of the file), the target's .rodata base register that the
// `lfd ...,(rBase)` loads need, so no pragma is needed. Declaring `ps` uninitialised and setting
// it from &psnd only after `pos = *position` (not in the initialiser) is what puts ps in r27 and
// the base in r31 like the target; written in the initialiser the whole callee-saved basis
// rotates by one (vol/span/pan/base/ps). `pan2` takes the float conversion directly (an `(s32)`
// cast adds a truncated copy the compares then use); `angOut` keeps its s32 temp `angOut2`.
// Needed: the original used the size optimiser for this function; without it the
// callee-saved GPR set and the immediate materialisation differ from the target.
// Signed angle from the listener's facing to the source, as a fraction of a half turn (-1..1).
static inline f32 psndRelAngle(f32 ang, f32 dir) {
	if (__fabsf(ang - dir) >= 180.0f) {
		if (ang < dir) {
			ang += 360.0f;
		}
		else {
			ang -= 360.0f;
		}
	}
	return (ang - dir) / 180.0f;
}

// `rel` coming out of psndRelAngle gives it f31 and `dist` f30, as the target.
u32 calc3D(u32 flags, Vec* position, u8 volume, u16 span) {
	static u8 PAN[4];
	Vec pos;
	f32 ang;
	f32 dir;
	f32 rel;
	f32 dist;
	s16 pan;
	s16 pan2;
	s16 angOut;
	PaperSoundWork* ps;

	pos = *position;
	ps = &psnd;
	if (flags & 0x01000000) {
		pos.x += ps->position.x;
		pos.y += ps->position.y;
		pos.z += ps->position.z;
	}
	if (flags & 0x02000000) {
		pos = ps->position;
	}
	dir = psnd.direction;
#ifdef MUST_MATCH
	// The target reloads pos.z for the distance computation after this call. Written as a plain
	// field read MWCC CSEs it into the argument's load and keeps it live in f31, so force the
	// reload the target has; the else form is the natural source.
	ang = angleABf(ps->position.x, ps->position.z, pos.x, *(volatile f32*)&pos.z);
#else
	ang = angleABf(ps->position.x, ps->position.z, pos.x, pos.z);
#endif
	rel = psndRelAngle(ang, dir);
	pan2 = 127.0f * rel + 127.0f;
	if (pan2 < 0x40) {
		pan2 = 0x7F - pan2;
	}
	else if (pan2 > 0xC4) {
		pan2 = 0xC4 - (pan2 - 0xC4);
	}
	pan = pan2 - 0x40;
	if ((s16)pan < 0) {
		pan = 0;
	}
	if ((s16)pan > 0x7F) {
		pan = 0x7F;
	}
	{
		f32 dx = pos.x - ps->position.x;
		f32 dy = pos.y - ps->position.y;
		f32 dz = pos.z - ps->position.z;

		dist = psndSqrtf(dx * dx + dy * dy + dz * dz);
	}
	{
		f32 cs = (f32)cos((f32)(3.14 * rel));
		f32 a = __fabsf(dist * cs);
		s32 r;
		f32 t;

		if (a > 2048.0f) {
			a = 2048.0f;
		}
		t = 64.0f * a / 2048.0f;
		if (__fabsf(rel) < 0.5f) {
			r = (s32)(64.0f - t);
			if ((s16)r < 1) {
				r = 1;
			}
		}
		else {
			r = (s32)(64.0f + t);
			if ((s16)r > 0x7F) {
				r = 0x7F;
			}
		}
		r = (s16)r;
		if (r <= 0x40) {
			f32 s = (f32)sin(6.2831855f * (f32)((0x40 - r) * 0x5A / 0x40) / 360.0f);
			angOut = 64.0f - 64.0f * s;
		}
		else {
			f32 s = (f32)sin(6.2831855f * (f32)((r - 0x40) * 0x5A / 0x40) / 360.0f);
			angOut = 64.0f + 64.0f * s;
		}
	}
	if (angOut < 0) {
		angOut = 0;
	}
	if (angOut > 0x7F) {
		angOut = 0x7F;
	}
	if (span != 0) {
		f32 d = dist - 200.0f;

		if (d > (f32)(span * 100)) {
			d = (f32)(span * 100);
		}
		if (d < 0.0f) {
			d = 0.0f;
		}
		PAN[0] = (s32)((f32)volume * (127.0f * ((f32)(span * 100) - d) / (f32)(span * 100)) / 127.0f);
	}
	else {
		PAN[0] = volume;
	}
	if (PAN[0] > 0x7F) {
		PAN[0] = 0x7F;
	}
	PAN[1] = pan;
	PAN[2] = angOut;
	return *(u32*)PAN;
}

s32 searchPSSFXList(const char* name) {
	SoundEffectEntry* entry = pssfxlist;
	int i;

	if ((s32)name >= 0) { //address, return
		return (s32)name;
	}
	for (i = 0; i < 4242; i++) {
		if (!strcmp(entry->name, name)) {
			break;
		}
		entry++;
	}
	if (i >= 4242) {
		i = 0;
	}
	return i;
}

void psndInit(void) { //1:1
    PaperSoundEnv* temp_r6;
    PaperSoundEffect* var_r6;
    int i;

    SoundInit();
    for (i = 0; i < 2; i++) {
        psbgm[i].unk0 = -1;
        psbgm[i].unk20 = 0;
    }
    for (i = 0; i < 40; i++) {
        var_r6 = &pssfx[i];
        var_r6->unk0 = -1;
        var_r6->unk18 = -1;
        var_r6->unk6 = 0;
    }
    for (i = 0; i < 16; i++) {
        temp_r6 = &psenv[i];
        temp_r6->unk4 = 0;
    }
    psnd.unk18 = 0;
    psnd.unk0 = 0;
    psnd.unk1A = 0;
    psnd.unkC = 0;
    psnd.direction = 0.0f;
    psnd.unk50 = 0;
    psnd.unk54 = 0;
    psnd.unk56 = 0;
    psnd.unk5C = 0;
    psnd.unk5A = 0x64;
    psnd.unk59 = 0x64;
    psnd.unk58 = 0x64;
    strcpy(psnd.unk40, "??????");
    SoundLoadDVD2("sound/proj/pmario");
    SoundSLibLoadDVD("sound/proj/pmario");
}

void psndMain(void) { //1:1
    u8 temp_r3;

    if (init_f) {
        if (psnd.unk5C) {
            psnd.unk5C--;
        }
        psndBGMMain();
        psndENVMain();
        psndSFXMain();
        SoundMain();
    }
}

void psndMainInt(void) {
	if (init_f) {
		SoundMainInt();
	}
}

void psndExit(void) {
	SoundDropData();
	DVDMgrSetupCallback(NULL);
	init_f = FALSE;
}

BOOL psndPushGroup(void) {
	static s8 tbl[4];
	static s32 group;
	if (_psndPushGroup_trg) {
		return TRUE;
	}
	if (group > 11) {
		init_f = TRUE;
		SoundLoadDVD2Free();
		return FALSE;
	}
	_psndPushGroup_trg = TRUE;
	((u8*)tbl)[0] = group;
	((u8*)tbl)[1] = 0xFF;
	SoundLoadDVD2PushGroup((u8*)tbl);
	_psndPushGroup_trg = FALSE;
	group++;
	return TRUE;
}

void psndSetPosDirListener(Vec* position, f32 direction) {  //1:1
    psnd.position = *position;
    psnd.direction = direction;
}

void psndStopAllFadeOut(void) {
	u8* p = (u8*)&psnd;
	int i;

	psndBGMOff_f_d(0x200, 0x3E8, 0);
	psndBGMOff_f_d(0x201, 0x3E8, 0);
	psndENVOff(0x200);
	psndENVOff(0x201);
	for (i = 0; i < 40; i++) {
		if ((u32)((PaperSoundEffectU16*)&pssfx[i])->unk0 != -1) {
			((PaperSoundEffectU16*)&pssfx[i])->unk6 |= 0x40;
		}
	}
	p[0x24] = 2;
	*(s16*)(p + 0x26) = 0x2D;
	*(s16*)(p + 0x28) = 0x2D;
}

void psndSetReverb(s32 mode) {
	switch (mode) {
	case 0:
		*(u16*)((u8*)&psnd + 0x52) = 0;
		break;
	case 1:
		*(u16*)((u8*)&psnd + 0x52) = 0x14;
		break;
	case 2:
		*(u16*)((u8*)&psnd + 0x52) = 0x28;
		break;
	case 3:
		*(u16*)((u8*)&psnd + 0x52) = 0x3C;
		break;
	}
}

void psndSetFlag(s32 flag) { //1:1 except maybe param
    psnd.unk56 |= flag;
    if (psnd.unk56 & 1) {
        psndBGMOff_f_d(0, 0x3E8, 0);
        psndBGMOff_f_d(1, 0x3E8, 0);
    }
    if (psnd.unk56 & 2) {
        psndENVOff(0);
        psndENVOff(1);
    }
}

void psndClearFlag(s32 flag) { //1:1 except maybe param
	psnd.unk56 &= ~flag;
}

//functions below were not present in the placeholder file
u16 psndGetFlag(void) {
	return psnd.unk56;
}

// The loop lives in its own inline so MWCC folds the pssfx base address straight
// into the walking pointer (`addi r7, r3, pssfx@l`) instead of copying it.
static inline void psndMapChangeMarkAll(void) {
	PaperSoundEffectU16* sfx = (PaperSoundEffectU16*)&pssfx[0];
	int i;

	for (i = 0; i < 40; sfx++, i++) {
		if ((u32)sfx->unk0 != -1 && !(sfx->unk6 & 0xA0)) {
			sfx->unk6 |= 0x40;
		}
	}
}

void psndMapChange(void) {
	u8* p;

	if (strcmp(psnd.unk40, gp->currentMapName) != 0) {
		*(u16*)((u8*)&psnd + 0x52) = 0;
	}
	strcpy(psnd.unk40, gp->currentMapName);
	p = (u8*)&psnd;
	psnd.unk56 &= ~0x100;
	psndMapChangeMarkAll();
	if (p[0x24] != 2) {
		p[0x24] = 2;
		*(s16*)(p + 0x26) = 0x2D;
		*(s16*)(p + 0x28) = 0x2D;
	}
}

void psndSFXAllOff(void) {
	u8* p = (u8*)&psnd;
	int i;

	for (i = 0; i < 40; i++) {
		PaperSoundEffectU16* sfx = (PaperSoundEffectU16*)&pssfx[i];

		if ((u32)sfx->unk0 != -1 && !(sfx->unk6 & 0xA0)) {
			sfx->unk6 |= 0x40;
		}
	}
	if (p[0x24] != 2) {
		p[0x24] = 2;
		*(s16*)(p + 0x26) = 0x2D;
		*(s16*)(p + 0x28) = 0x2D;
	}
}

void L_psndBGM_stop(void) {
	s32 i;

	psnd.unk56 &= ~0x10;
	for (i = 0; i < 40; i++) {
		if ((u32)pssfx[i].unk0 == -1) continue;
		if (!(((u16*)&pssfx[i])[3] & 0x10)) continue;
		if (((u16*)&pssfx[i])[3] & 0x820) continue;
		psndSFXOff(i);
	}
}

void psndBGM_rate(s32 id, u16 rate) {
	BGMListEntry* entry;
	u32 bgmid;
	PaperSoundBGM* bgm = &psbgm[id & 0xF];
	bgmid = bgm->unk0;

	if ((u32)bgmid == -1) {
		return;
	}
	entry = &psbgmlist[bgmid];
	if (!(entry->flags & 0x80000000)) {
		return;
	}
	SoundSSSetPlayFreqCh(bgm->streamId, rate);
}

void psndBGMMain(void) {
	s32 level;
	u32 active;
	s32 master;
	PaperSoundBGM* cur;
	u8 base;
	u8 range;
	u8 vol;
	u8* work;
	u8* w;
	PaperSoundBGM* bgm;
	s32 i;

	level = 100;
	if ((psnd.unk56 & 0x20) && !(psnd.unk56 & 0x40)) {
		level = 0x41;
	}

	if (seqGetSeq() != SEQ_MAP_CHANGE && (gp->retraceCount & 1)) {
		if (psnd.unk58 < level) {
			psnd.unk58++;
		}
		if (psnd.unk58 > level) {
			psnd.unk58--;
		}
	}

	for (i = 0; i < 2; i++) {
		bgm = &psbgm[i];
		work = (u8*)bgm + 0x20;

		if ((u32)bgm->unk0 == (u32)-1) {
			continue;
		}

		if (SoundSSCheck(bgm->streamId) & 2) {
			SoundSSMuteOff(bgm->streamId);
		}

		if ((u32)bgm->unk0 == (u32)-1) {
			active = 0;
		}
		else if (psbgmlist[bgm->unk0].flags & 0x80000000) {
			if ((u32)SoundSSCheck(bgm->streamId) != 0) {
				active = 1;
			}
			else {
				active = 0;
			}
		}
		else {
			if ((u32)SoundSongCheck(bgm->streamId) == 0) {
				active = 0;
			}
			else {
				active = 1;
			}
		}

		if (active == 0) {
			bgm->unk0 = -1;
			continue;
		}

		if (bgm->scopeTimer != 0 && bgm->scopeTimer != 0xFF) {
			bgm->scopeTimer--;
		}

		master = 100;
		if (bgm->scopeTimer == 0) {
			master = psnd.unk58;
		}

		{
			// MUST_MATCH: the integer cast keeps this pointer separate from `work`,
			// so MWCC holds it in r6 as the target does.
#ifdef MUST_MATCH
			u8* w = (u8*)((u32)bgm + 0x20);
#else
			u8* w = (u8*)bgm + 0x20;
#endif

			if (w[0] == 0) {
				base = bgm->volume;
			}
			else {
				base = bgm->volume * *(s16*)(w + 4) / *(s16*)(w + 2);
			}

			if (w[6] == 0) {
				range = 0x7F;
			}
			else {
				range = *(s16*)(w + 0xA) * 0x7F / *(s16*)(w + 8);
			}
		}

		vol = base * range / 127;

		if ((u32)psbgm[i & 0xF].unk0 != (u32)-1) {
			cur = &psbgm[i & 0xF];
			if (psbgmlist[cur->unk0].flags & 0x80000000) {
				SoundSSSetVolCh(cur->streamId, (vol * master) / 100);
			}
			else {
				SoundSongSetVolCh(cur->streamId, (vol * master) / 100);
			}
		}

		if (work[0] == 1) {
			*(s16*)(work + 4) = *(s16*)(work + 4) + 1;
			if (*(s16*)(work + 4) >= *(s16*)(work + 2)) {
				work[0] = 0;
			}
		}

		if (work[0] == 2) {
			*(s16*)(work + 4) = *(s16*)(work + 4) - 1;
			if (*(s16*)(work + 4) <= 0) {
				if ((u32)bgm->unk0 != (u32)-1) {
					if (psbgmlist[bgm->unk0].flags & 0x80000000) {
						SoundSetFadeTime(0, 100);
						SoundSSFadeoutCh(bgm->streamId);
					}
					else {
						SoundSetFadeTime(0, 100);
						SoundSongFadeinCh(bgm->streamId);
					}
				}
				work[0] = 0;
				bgm->unk0 = -1;
			}
		}

		if (work[0] == 4) {
			s32 v = *(s16*)(work + 4);

			if (v > 0) {
				*(s16*)(work + 4) = v - 1;
			}
		}

		if (work[6] == 8) {
			s32 target = (*(s16*)(work + 8) * work[7]) / 100;

			if (*(s16*)(work + 0xA) > target) {
				if (!(bgm->flags & 0x40)) {
					*(s16*)(work + 0xA) = *(s16*)(work + 0xA) - 1;
				}
				else {
					*(s16*)(work + 0xA) = *(s16*)(work + 0xA) - 5;
					if (*(s16*)(work + 0xA) < (*(s16*)(work + 8) * work[7]) / 100) {
						*(s16*)(work + 0xA) = (*(s16*)(work + 8) * work[7]) / 100;
					}
				}
			}
		}

		if (work[6] == 0x10) {
			if (*(s16*)(work + 0xA) < *(s16*)(work + 8)) {
				if (!(bgm->flags & 0x40)) {
					*(s16*)(work + 0xA) = *(s16*)(work + 0xA) + 1;
				}
				else {
					*(s16*)(work + 0xA) = *(s16*)(work + 0xA) + 5;
					if (*(s16*)(work + 0xA) > *(s16*)(work + 8)) {
						*(s16*)(work + 0xA) = *(s16*)(work + 8);
					}
				}
			}
			else {
				work[6] = 0;
			}
		}
	}
}

BOOL psndBGMOn(u32 r3, char* bgm_name) {
	return psndBGMOn_f_d(r3, (s32)bgm_name, 0x3E8, 0x2EE, 0);
}

BOOL psndBGMOn_f(s32 a, char* b, s32 c, s32 d) {
	return psndBGMOn_f_d(a, (s32)b, c, 0x2EE, d);
}

// Inlined BGM-list name lookup. The inliner reproduces the target's pointer/index colouring
// (namePtr=r27, index=r26); the `name >= 0` early return and the 0x106 wraparound are the original's.
static inline s32 psndBGMFindName(s32 name) {
	BGMListEntry* namePtr;
	s32 index;

	namePtr = psbgmlist;
	if (name >= 0) {
		return name;
	}
	for (index = 0; index < 0x106; index++, namePtr++) {
		if (!strcmp((const char*)namePtr->name, (const char*)name)) {
			break;
		}
	}
	if (index >= 0x106) {
		index = 0;
	}
	return index;
}

// `name` (an id or a name pointer, as psndENVOn_f_d/__psndSFXOn take it) is reused for the resolved id,
// which gives it retail's r24; the early slot pointer `cur` is its own local. The u8 `vol` takes the value
// of the slot's volume store (`vol = bgm->vol = entry->vol`), which gives retail's `lbz r3; stb r3; mr r26,r3`.
s32 psndBGMOn_f_d(s32 a, s32 name, s32 c, s32 d, s32 e) {
	PaperSoundBGM* bgm;
	u8* work;
	s32 reverb;
	BGMListEntry* ptr;
	OSTime time;
	u32 i;
	BGMListEntry* entry;
	u8* cur;
	s32 slot;
	s32 index;
	u8 vol;
	s32 pan;

	if (psnd.unk56 & 1) {
		return 0;
	}

	index = psndBGMFindName(name);

	name = index;
	slot = a & 0xF;
	cur = (u8*)&psbgm[slot] + 0x20;

	if (evtGetValue(0, 0xF5DE0180) >= 0x65 &&
	    (evtGetValue(0, 0xF5DE0180) < 0x66 || evtGetValue(0, 0xF5DE0180) >= 0x6A)) {
		if (strcmp(gp->currentMapName, "mri_00") != 0 && index == 0x4F) {
			name = 0x50;
		}
	}

	if (evtGetValue(0, 0xF5DE0180) >= 0x101 && evtGetValue(0, 0xF5DE0180) < 0x104) {
		if (name == 0x5A || name == 0x5B) {
			name = 0xA4;
		}
	}

	if (evtGetValue(0, 0xF5DE0180) >= 0x6D && evtGetValue(0, 0xF5DE0180) < 0x6F) {
		if (name == 0x4F) {
			name = 0xA4;
		}
		if (name == 0x50) {
			name = 0xA4;
		}
		if (name == 0x7B) {
			return 0;
		}
		if (name == 0x7D) {
			return 0;
		}
		if (name == 0x7E) {
			return 0;
		}
		if (slot == 0 && (a & 0x20) && cur[0] != 4) {
			return 0;
		}
		{
			PaperSoundBGM* bgm0 = &psbgm[0];

			if ((name == 0x90 || name == 0x91) && (u32)bgm0->unk0 == 0xA4) {
				return 0;
			}
			if (name == 0x92 && (u32)bgm0->unk0 == 0xA4) {
				psndBGMOff_f_d(0x400, 0x2EE, 1);
			}
		}
	}

	if (evtGetValue(0, 0xF5DE0180) >= 0x7F && evtGetValue(0, 0xF5DE0180) < 0xB2) {
		if (name == 0x8C) {
			return 0;
		}
		{
			PaperSoundBGM* bgm0 = &psbgm[0];

			if ((name == 0x90 || name == 0x91) && (u32)bgm0->unk0 == 0xC3) {
				return 0;
			}
			if (name == 0 && (u32)bgm0->unk0 == 0xC3 && !(a & 0x20)) {
				return 0;
			}
			if (name == 0x92 && (u32)bgm0->unk0 == 0xC3) {
				psndBGMOff_f_d(0x400, 0x2EE, 1);
			}
		}
	}

	if (evtGetValue(0, 0xF5DE0180) >= 0xA4 && name == 0x53) {
		name = 0x54;
	}

	if (name == 0x90 || name == 0x91) {
		s32 v = psbgm[1].unk0;

		if ((u32)v == 0x80 || (u32)(v - 0x81) <= 0xB || (u32)v == 0x8E || (u32)v == 0x8D) {
			return 0;
		}
	}

	bgm = &psbgm[a & 0xF];
	entry = &psbgmlist[name];
	work = (u8*)bgm + 0x20;

	if (entry->flags == 0 && !(a & 0x20)) {
		return 0;
	}

	SoundSetFadeTime(c & 0xFFFF, c & 0xFFFF);

	if ((u32)bgm->unk0 != (u32)-1) {
		if (!(a & 0x10)) {
			if (a & 0x20) {
				if (psbgmlist[bgm->unk0].flags & 0x80000000) {
					SoundSSContinueCh(bgm->streamId);
				}
				else {
					SoundSongContinueCh(bgm->streamId);
				}
				if (a & 0x100) {
					work[0] = 1;
					*(s16*)(work + 0x2) = (s16)((u32)c / 16.666666f);
					*(s16*)(work + 0x4) = 0;
				}
				else {
					work[0] = 0;
					*(s16*)(work + 0x2) = 1;
					*(s16*)(work + 0x4) = 1;
				}
				if (a & 0x80) {
					if (work[0x6] == 8) {
						work[0x6] = 0x10;
						*(s16*)(work + 0x8) = (s16)((u32)c / 16.666666f);
						*(s16*)(work + 0xA) = (s16)((*(s16*)(work + 0x8) * work[0x7]) / 100);
						if (a & 0x40) {
							bgm->flags |= 0x40;
						}
						else {
							bgm->flags &= ~0x40;
						}
					}
				}
				goto done;
			}
			if (*(u32*)&bgm->unk0 == (u32)name) {
				if (work[0] == 2) {
				}
				else {
					goto done;
				}
			}
			else {
				if (((u8*)&entry->params)[2] != 0 &&
				    ((u8*)&entry->params)[2] == bgm->reverb) {
					goto done;
				}
			}
		}
		if (entry->flags & 0x80000000) {
			if ((u32)(a & 0x10) != 0) {
				SoundSetFadeTime(0, 100);
				SoundSSFadeoutCh(bgm->streamId);
			}
			else {
				SoundSetFadeTime(0, d & 0xFFFF);
				SoundSSFadeoutCh(bgm->streamId);
			}
		}
		else {
			SoundSetFadeTime(0, d & 0xFFFF);
			SoundSongFadeoutCh(bgm->streamId);
		}
	}

	if (name == 0) {
		return 0;
	}

	vol = bgm->volume = ((u8*)&entry->params)[0];
	reverb = ((u8*)&entry->params)[2];
	bgm->unk0 = name;
	pan = ((u8*)&entry->params)[1];
	bgm->flags = a;
	bgm->reverb = reverb;
	bgm->pan = pan;
	if (((u8*)&entry->params)[3] == 1) {
		bgm->scopeTimer = 0xFF;
	}
	else {
		bgm->scopeTimer = 0;
	}
	time = gp->renderTime;
	bgm->startTime = time;

	if (entry->flags & 0x80000000) {
		i = psndSFXFindSSChannel();
		bgm->streamId = i;
		if ((u32)bgm->streamId == (u32)-1) {
			goto no_channel;
		}
		SoundSSPlayCh(bgm->streamId, (u32)entry->streamName);
		SoundSSMuteOn(bgm->streamId);
		SoundSSSetVolCh(bgm->streamId, vol);
		SoundSSSetPanCh(bgm->streamId, pan);
		if ((u32)pan == 0x40) {
			SoundSSSetSrndPanCh(bgm->streamId, 0x40);
		}
		else {
			SoundSSSetSrndPanCh(bgm->streamId, 0);
		}
	}
	else {
		i = psndSFXFindSongChannel();
		bgm->streamId = i;
		if ((u32)bgm->streamId == (u32)-1) {
			goto no_channel;
		}
		SoundSongPlayCh(bgm->streamId, entry->flags & 0x0FFFFFFF);
		SoundSongSetVolCh(bgm->streamId, vol);
	}

	if (a & 0x100) {
		work[0] = 1;
		*(s16*)(work + 0x2) = (s16)((u32)c / 16.666666f);
		*(s16*)(work + 0x4) = 0;
		work[0x6] = 0;
		*(s16*)(work + 0x8) = 1;
		*(s16*)(work + 0xA) = 1;
	}
	else {
		work[0] = 0;
		*(s16*)(work + 0x2) = 1;
		*(s16*)(work + 0x4) = 1;
		work[0x6] = 0;
		*(s16*)(work + 0x8) = 1;
		*(s16*)(work + 0xA) = 1;
	}
	goto done;

no_channel:
	bgm->unk0 = -1;
	return 0;

done:
	psndBGMMain();
	return 1;
}

BOOL psndBGMOff(s32 flags) {
	return psndBGMOff_f_d(flags, 0x3E8, 0);
}

BOOL psndBGMOff_f_d(s32 flags, s32 fadetime, BOOL someswitch) {
    PaperSoundBGM* bgm = &psbgm[flags & 0xF];
    u8* work;
    s32 silent;
    BGMListEntry* entry;

    silent = silent_tbl[(flags >> 12) & 0xF];
    SoundSetFadeTime(fadetime & 0xFFFF, fadetime & 0xFFFF);

    if ((u32)bgm->unk0 == -1) {
        return FALSE;
    }

    if ((u32)someswitch == 0) {
        if (evtGetValue(0, 0xF5DE0180) >= 0x6D && evtGetValue(0, 0xF5DE0180) < 0x6F && (u32)bgm->unk0 == 0xA4) {
            return FALSE;
        }
        if (evtGetValue(0, 0xF5DE0180) >= 0x7F && evtGetValue(0, 0xF5DE0180) < 0xB2 && (u32)psbgm[0].unk0 == 0xC3) {
            if ((u32)bgm->unk0 != 0x92) {
                return FALSE;
            }
            psndBGMOn_f_d(0x120, 0, 0x2EE, 0x2EE, 1);
        }
    }

    work = (u8*)bgm + 0x20;
    entry = &psbgmlist[bgm->unk0];

    if (flags & 0x200) {
        if (work[0] == 2) {
        }
        else if (work[0] != 4) {
            work[0] = 2;
            *(s16*)(work + 0x2) = (s16)((u32)fadetime / 16.666666f);
            *(s16*)(work + 0x4) = *(s16*)(work + 0x2);
        }
        else {
            work[0] = 2;
        }
    }
    else if (flags & 0x400) {
        if (work[0] != 2 && work[0] != 4) {
            work[0] = 4;
            *(s16*)(work + 0x2) = (s16)((u32)fadetime / 16.666666f);
            *(s16*)(work + 0x4) = *(s16*)(work + 0x2);
        }
    }
    else if (flags & 0x800) {
        if (work[6] != 8) {
            work[6] = 8;
            work[7] = silent;
            *(s16*)(work + 0x8) = (s16)((u32)fadetime / 16.666666f);
            *(s16*)(work + 0xA) = *(s16*)(work + 0x8);
            if ((flags & 0x40) || (gp->flags & 1)) {
                bgm->flags |= 0x40;
            }
            else {
                bgm->flags &= ~0x40;
            }
        }
    }
    else {
        if (entry->flags & 0x80000000) {
            SoundSSStopCh(bgm->streamId);
        }
        else {
            SoundSongStopCh(bgm->streamId);
        }
        bgm->unk0 = -1;
    }
    return TRUE;
}

BOOL psndBGMChk(s32 id) {
	s32 val = psbgm[id & 0xF].unk0;

	return (s32)(~((val + 1) | (-1 - val))) >> 31;
}

BOOL psndBGMChkSilent(s32 id) {
	PaperSoundBGM* bgm = &psbgm[id & 0xF];

	if ((u32)bgm->unk0 == -1) {
		return FALSE;
	}
	return bgm->unk21[5] == 8;
}

void psndBGMScope(s32 id, s32 scope) {
	PaperSoundBGM* bgm = &psbgm[id & 0xF];

	if ((u32)bgm->unk0 == -1) {
		return;
	}
	if (scope > 0) {
		bgm->scopeTimer = 0xFF;
	}
	if (scope < 0) {
		bgm->scopeTimer = 0;
	}
	if (scope == 0) {
		bgm->scopeTimer = 0x1E;
	}
}

BOOL psndBGMStartCheck(s32 id) {
	PaperSoundBGM* bgm = &psbgm[id & 0xF];

	if ((u32)bgm->unk0 == -1) {
		return TRUE;
	}
	return (SoundSSCheck(bgm->streamId) >> 3) & 1;
}

s32 psndBGMPlayTime(s32 id) {
	PaperSoundBGM* bgm = &psbgm[id & 0xF];

	if ((u32)bgm->unk0 == -1) {
		return 600000;
	}
	return OSTicksToMilliseconds(gp->renderTime - bgm->startTime);
}

// Inline channel searches: an inline `return` is what makes MWCC emit `bne cont; b found` for the free-channel
// scans (a `goto`/`break` folds to `beq found`).
static inline s32 psndSFXFindSSChannel(void) {
    s32 ch;

    for (ch = 0; (u32)ch < 3; ch++) {
        if ((u32)SoundSSCheck(ch) == 0) return ch;
    }
    for (ch = 0; (u32)ch < 3; ch++) {
        if (SoundSSCheck(ch) & 0x20) {
            SoundSSStopCh(ch);
        }
    }
    for (ch = 0; (u32)ch < 3; ch++) {
        if ((u32)SoundSSCheck(ch) == 0) return ch;
    }
    return -1;
}

static inline s32 psndSFXFindSongChannel(void) {
    s32 ch;

    for (ch = 0; (u32)ch < 4; ch++) {
        if ((u32)SoundSongCheck(ch) == 0) return ch;
    }
    return -1;
}

// The reverb guard reads unk18 through a u32 view, so SoundEfxSetAux1's argument is reloaded as in retail.
// The pending-bit read uses the unsigned record view; keeping it separate from the flag update below
// leaves the two loops' address and counter webs in their original registers. Effect IDs use their native
// u16 width, and each fade reuses vol for the byte mask before multiplying by ratio.
void psndSFXMain(void) {

    SoundEffectEntry* entry;
    s32 target;
    PaperSoundEffectDetail* sfx;
    s32 vol;
    s32 numSounds;
    s32 i;
    u32 u;
    u32 ch;
    u32 res;
    u8* pp;
    s32 ratio;

    ratio = 0;
    pp = (u8*)&psnd;
    for (i = 0; i < 40; i++) {
        sfx = (PaperSoundEffectDetail*)&pssfx[i];
        if ((u32)sfx->unk0 == -1) continue;
        if (sfx->unk6 & 1) continue;
        entry = &pssfxlist[sfx->unk0 & 0x1FFF];
        if (entry->flags & 0x80000000) {
            u = (u32)SoundSSCheck(sfx->unk18);
            if (u == 0) {
                sfx->unk0 = -1;
                sfx->unk18 = -1;
            }
        } else if (entry->flags & 0x40000000) {
            u = (u32)SoundSongCheck(sfx->unk18);
            if (u == 0) {
                sfx->unk0 = -1;
                sfx->unk18 = -1;
            }
        } else {
            u = (u32)SoundEfxCheck(sfx->unk18);
            if (u == 0) {
                sfx->unk0 = -1;
                sfx->unk18 = -1;
            }
        }
    }

    if (pp[0x24] == 2) {
        if (--*(s16*)(pp + 0x28) <= 0) {
            pp[0x24] = 0;
        }
        ratio = *(s16*)(pp + 0x28) * 0x7F / *(s16*)(pp + 0x26);
    }

    target = 100;
    if (psnd.unk56 & 0x20) {
        if (!(psnd.unk56 & 0x40)) {
            target = 120;
        }
    }
    if (seqGetSeq() != SEQ_MAP_CHANGE && (gp->retraceCount & 1)) {
        if (psnd.unk59 < target) {
            psnd.unk59++;
        }
        if (psnd.unk59 > target) {
            psnd.unk59--;
        }
    }

    numSounds = 0;
    for (i = 0; i < 40; i++) {
        sfx = (PaperSoundEffectDetail*)&pssfx[i];
        if ((u32)sfx->unk0 == -1) continue;
        entry = &pssfxlist[sfx->unk0 & 0x1FFF];
        numSounds++;
        if (((PaperSoundEffectU16*)sfx)->unk6 & 1) {
            sfx->unk6 = (sfx->unk6) & ~0x100;
            if (sfx->unkE != 0) {
                sfx->unkE -= 1;
                continue;
            }
            if (entry->flags & 0x80000000) {
                ch = psndSFXFindSSChannel();
                sfx->unk18 = ch;
                if ((u32)sfx->unk18 != -1) {
                    SoundSSPlayCh(sfx->unk18, (u32)entry->streamName);
                    SoundSSSetPanCh(sfx->unk18, sfx->pan);
                    vol = (sfx->vol * sfx->baseVol) / 127 - sfx->unk10;
                    if (vol < 0) vol = 0;
                    if (vol > 0x7F) vol = 0x7F;
                    SoundSSSetVolCh(sfx->unk18, (u8)vol);
                }
            } else if (entry->flags & 0x40000000) {
                ch = psndSFXFindSongChannel();
                sfx->unk18 = ch;
                if ((u32)sfx->unk18 != -1) {
                    SoundSongPlayCh(sfx->unk18, entry->flags & 0x0FFFFFFF);
                    vol = (sfx->vol * sfx->baseVol) / 127 - sfx->unk10;
                    if (vol < 0) vol = 0;
                    if (vol > 0x7F) vol = 0x7F;
                    SoundSongSetVolCh(sfx->unk18, (u8)vol);
                }
            } else {
                if (sfx->unk6 & 2) {
                    vol = (sfx->vol * sfx->baseVol) / 127 - sfx->unk10;
                    if (vol < 0) vol = 0;
                    if (vol > 0x7F) vol = 0x7F;
                    res = calc3D(sfx->unk0, &sfx->position, (u8)vol, sfx->unk12);
                    sfx->unk18 = SoundEfxPlayEx((u16)entry->flags, 0, ((u8*)&res)[0], ((u8*)&res)[1]);
                    if ((u32)sfx->unk18 != -1) {
                        SoundEfxSetVolume(sfx->unk18, ((u8*)&res)[0]);
                        SoundEfxSetSrndPan(sfx->unk18, ((u8*)&res)[2]);
                        SoundEfxSetPitch(sfx->unk18, sfx->unk14);
                    }
                } else {
                    vol = (sfx->vol * sfx->baseVol) / 127 - sfx->unk10;
                    if (vol < 0) vol = 0;
                    if (vol > 0x7F) vol = 0x7F;
                    sfx->unk18 = SoundEfxPlayEx((u16)entry->flags, 0, (u8)vol, sfx->pan);
                    if ((u32)sfx->unk18 != -1) {
                        SoundEfxSetPitch(sfx->unk18, sfx->unk14);
                    }
                }
                if (*(u32*)&sfx->unk18 != -1) {
                    s32 reverb = entry->reverb;

                    if (reverb > 0) {
                        reverb += *(u16*)psnd.unk52;
                    }
                    if (reverb > 0x7F) reverb = 0x7F;
                    SoundEfxSetAux1(sfx->unk18, (u8)reverb);
                }
            }
            if ((u32)sfx->unk18 == -1) {
                sfx->unk0 = -1;
                sfx->unk18 = -1;
                continue;
            }
            sfx->unk6 &= ~1;
        }
        if (sfx->unk6 & 2) {
            vol = (sfx->vol * sfx->baseVol) / 127 - sfx->unk10;
            if (vol < 0) vol = 0;
            if (vol > 0x7F) vol = 0x7F;
            res = calc3D(sfx->unk0, &sfx->position, (u8)vol, sfx->unk12);
            if (entry->flags & 0x80000000) {
                SoundSSSetPanCh(sfx->unk18, ((u8*)&res)[1]);
                vol = ((u8*)&res)[0] * psnd.unk59 / 100;
                if (vol < 0) vol = 0;
                if (vol > 0x7F) vol = 0x7F;
                SoundSSSetVolCh(sfx->unk18, (u8)vol);
                SoundSSSetSrndPanCh(sfx->unk18, ((u8*)&res)[2]);
            } else if (entry->flags & 0x40000000) {
                vol = ((u8*)&res)[0] * psnd.unk59 / 100;
                if (vol < 0) vol = 0;
                if (vol > 0x7F) vol = 0x7F;
                SoundSongSetVolCh(sfx->unk18, (u8)vol);
            } else {
                SoundEfxSetPan(sfx->unk18, ((u8*)&res)[1]);
                vol = ((u8*)&res)[0] * psnd.unk59 / 100;
                if (vol < 0) vol = 0;
                if (vol > 0x7F) vol = 0x7F;
                SoundEfxSetVolume(sfx->unk18, (u8)vol);
                SoundEfxSetSrndPan(sfx->unk18, ((u8*)&res)[2]);
            }
        } else {
            if (entry->flags & 0x80000000) {
                SoundSSSetPanCh(sfx->unk18, sfx->pan);
                vol = (sfx->vol * sfx->baseVol) / 127 - sfx->unk10;
                if (vol < 0) vol = 0;
                if (vol > 0x7F) vol = 0x7F;
                SoundSSSetVolCh(sfx->unk18, (u8)vol);
            } else if (entry->flags & 0x40000000) {
                vol = (sfx->vol * sfx->baseVol) / 127 - sfx->unk10;
                if (vol < 0) vol = 0;
                if (vol > 0x7F) vol = 0x7F;
                SoundSongSetVolCh(sfx->unk18, (u8)vol);
            } else {
                SoundEfxSetPan(sfx->unk18, sfx->pan);
                vol = (sfx->vol * sfx->baseVol) / 127 - sfx->unk10;
                if (vol < 0) vol = 0;
                if (vol > 0x7F) vol = 0x7F;
                SoundEfxSetVolume(sfx->unk18, (u8)vol);
            }
        }
        if (!(sfx->unk6 & 0xA30) && (psnd.unk56 & 0x80)) {
            if (entry->flags & 0x80000000) {
                SoundSSSetVolCh(sfx->unk18, 0);
            } else if (entry->flags & 0x40000000) {
                SoundSongSetVolCh(sfx->unk18, 0);
            } else {
                SoundEfxSetVolume(sfx->unk18, 0);
            }
        }
        if (sfx->unk6 & 0x40) {
            if (entry->flags & 0x80000000) {
                vol = SoundSSGetVolCh(sfx->unk18);
                vol &= 0xFF;
                vol = (vol * ratio) / 127;
                SoundSSSetVolCh(sfx->unk18, (u8)vol);
                if (vol == 0) psndSFXOff(i);
            } else if (entry->flags & 0x40000000) {
                vol = SoundSongGetVolCh(sfx->unk18);
                vol &= 0xFF;
                vol = (vol * ratio) / 127;
                SoundSongSetVolCh(sfx->unk18, (u8)vol);
                if (vol == 0) psndSFXOff(i);
            } else {
                vol = SoundEfxGetVolume(sfx->unk18);
                vol &= 0xFF;
                vol = (vol * ratio) / 127;
                SoundEfxSetVolume(sfx->unk18, (u8)vol);
                if (vol == 0) psndSFXOff(i);
            }
        }
        if ((psnd.unk56 & 0x100) && (sfx->unk6 & 0x400)) {
            if (entry->flags & 0x80000000) {
                SoundSSSetVolCh(sfx->unk18, 0);
            } else if (entry->flags & 0x40000000) {
                SoundSongSetVolCh(sfx->unk18, 0);
            } else {
                SoundEfxSetVolume(sfx->unk18, 0);
            }
        }
    }
    psnd.unk5B[0] = numSounds;
    psnd.unk54 = 0;
}

// The first scan loop reads the id through two views of the slot, so the frontend does not merge the two loads
// into one temp (the backend still does): with the shared temp, &psnd/sfx/i lose retail's r3/r4/r5. The second
// `slot == 0` check sits inside the fallback search, so the first loop's hit branches past it.
s32 __psndSFXOn(s32 name, u8 vol, u8 pan, s32 pitch, Vec* position, s32 a6, s32 flags, s32 a8) {
    s32 sfxid;
    s32 index;
    s32 i;
    s32 j;
    PaperSoundEffect* sfx;
    PaperSoundEffect* slot = 0;
    SoundEffectEntry* entry;
    s32 slotIndex = 0;
    s32 prio;

    index = searchPSSFXList((const char*)name);
    sfxid = index & 0x1FFF;
    entry = &pssfxlist[sfxid];
    if (entry->flags == 0) {
        return -1;
    }
    if ((psnd.unk56 & 4) && a8 == 0) {
        return -1;
    }
    if (sfxid == 0x5B || sfxid == 0x8CA || (u32)(sfxid - 0x1BE) <= 1 || sfxid == 0x205) {
        if (psnd.unk5C != 0) {
            return -1;
        }
        psnd.unk5C = 6;
    }
    if (evtGetValue(0, 0xF5DE0180) <= 2 && sfxid == 0x365) {
        return -1;
    }

    prio = 0;
    for (i = 0; i < 40; i++) {
        sfx = &pssfx[i];
        if ((u32)sfx->unk0 != -1 && (u32)(((PaperSoundEffectU16*)sfx)->unk0 & 0x1FFF) == (u32)sfxid
            && (((u16*)sfx)[3] & 0x100) && prio == 0) {
            *(u16*)((u8*)&psnd + 0x54) += 8;
            prio = *(u16*)((u8*)&psnd + 0x54);
        }
        if ((u32)sfx->unk0 == -1) {
            slot = sfx;
            slotIndex = i;
        }
    }

    if (slot == 0) {
        for (j = 0; j < 40; j++) {
            sfx = &pssfx[j];
            if ((u32)sfx->unk0 == -1) {
                slot = sfx;
                slotIndex = j;
                break;
            }
            if (*(u16*)(sfx->unk8 + 4) > entry->field_0x10) {
                psndSFXOff(j);
                slot = sfx;
                slotIndex = j;
                break;
            }
        }
        if (slot == 0) {
            return -1;
        }
    }

    if (vol == 0xFF) {
        vol = 0x7F;
    }
    if (pan == 0xFF) {
        pan = entry->pan;
    }
    *(u16*)&slot->unk6 = 0x101;
    if (psnd.unk56 & 0x10) {
        *(u16*)&slot->unk6 |= 0x10;
    }
    if (index & 0x04000000) {
        *(u16*)&slot->unk6 |= 0x20;
    }
    if (sfxid == 0x518 || sfxid == 0x742) {
        *(u16*)&slot->unk6 |= 0x280;
    }
    if (entry->field_0xE == 1) {
        *(u16*)&slot->unk6 |= 0x200;
    }
    if (index & 0x10000) {
        *(u16*)&slot->unk6 |= 0x400;
    }
    if (index & 0x20000) {
        *(u16*)&slot->unk6 |= 0x800;
    }
    if (index & 0x40000) {
        *(u16*)&slot->unk6 |= 0x80;
    }

    slot->unk0 = index;
    ((PaperSoundEffectDetail*)slot)->vol = vol;
    ((PaperSoundEffectDetail*)slot)->baseVol = entry->volume;
    ((PaperSoundEffectDetail*)slot)->pan = pan;
    ((PaperSoundEffectDetail*)slot)->unkE = (s16)(s32)((f32)(u16)pitch / 16.666666f + (f32)prio + (f32)entry->field_0x11);
    ((PaperSoundEffectDetail*)slot)->unk10 = (u16)(prio * 5);
    ((PaperSoundEffectDetail*)slot)->reverb = entry->reverb;
    ((PaperSoundEffectDetail*)slot)->unkC = entry->field_0x10;
    ((PaperSoundEffectDetail*)slot)->unk14 = *(s16*)&entry->pitch;
    if (position != 0) {
        ((PaperSoundEffectDetail*)slot)->position = *position;
        if ((u32)a6 == 0) {
            *(u16*)&slot->unk6 |= 2;
        } else {
            *(u16*)&slot->unk6 |= 4;
        }
    }
    ((PaperSoundEffectDetail*)slot)->unk12 = (u16)flags;
    ((PaperSoundEffectDetail*)slot)->unk16 = 0x7D00;
    *(u16*)((u8*)&psnd + 0x50) += 1;
    ((PaperSoundEffectDetail*)slot)->unk4 = *(u16*)((u8*)&psnd + 0x50);
    slotIndex |= *(u16*)((u8*)&psnd + 0x50) << 8;
    return slotIndex;
}

s32 psndSFXOn_(const char* name) {
	return __psndSFXOn((s32)name, 0xFF, 0xFF, 0, 0, 0, 0x10, 1);
}

s32 psndSFXOn(const char* sfx_name) {
	return __psndSFXOn((s32)sfx_name, 0xFF, 0xFF, 0, 0, 0, 0x10, 0);
}

void psndSFXOnVol(s32 a, u8 b) {
	__psndSFXOn(a, b, 0xFF, 0, 0, 0, 0x10, 0);
}

s32 psndSFXOn_3D(const char* name, Vec* position) {
	return __psndSFXOn((s32)name, 0xFF, 0xFF, 0, position, 0, 0x10, 0);
}

s32 psndSFXOnEx_3D(const char* name, Vec* position, u8 vol, u8 pan, s32 pitch, s32 flags) {
	return __psndSFXOn((s32)name, vol, pan, pitch, position, 0, flags, 0);
}

void psndSFX_vol(s32 id, u8 vol) {
	PaperSoundEffect* sfx;

	if ((u32)id == -1) {
		return;
	}
	sfx = &pssfx[id & 0xFF];
	if ((u32)sfx->unk0 == -1) {
		return;
	}
	sfx->unk8[0] = vol;
}

void psndSFX_pit(s32 id, s16 pitch) {
	PaperSoundEffect* sfx;
	SoundEffectEntry* entry;

	if ((u32)id == -1) {
		return;
	}
	sfx = &pssfx[id & 0xFF];
	if ((u32)sfx->unk0 == -1) {
		return;
	}
	if (((u16*)sfx)[3] & 1) {
		((s16*)sfx)[10] = pitch;
	}
	else {
		entry = &pssfxlist[sfx->unk0 & 0x1FFF];
		if (!(entry->flags & 0x80000000)) {
			if (!(entry->flags & 0x80000000)) {
				SoundEfxSetPitch(sfx->unk18, pitch);
			}
		}
	}
}

void psndSFX_pos(s32 id, Vec* position) {
	PaperSoundEffect* sfx;

	if ((u32)id == -1) {
		return;
	}
	sfx = &pssfx[id & 0xFF];
	if ((u32)sfx->unk0 == -1) {
		return;
	}
	*(Vec*)sfx->unk1C = *position;
}

void psndSFX_dist(s32 id, u16 dist) {
	PaperSoundEffect* sfx;

	if ((u32)id == -1) {
		return;
	}
	sfx = &pssfx[id & 0xFF];
	if ((u32)sfx->unk0 == -1) {
		return;
	}
	*(u16*)(sfx->unk8 + 0xA) = dist;
}

// The entry's type flags are read once into `flags`: the standalone code is the same either way, but the
// extra web gives psndSFXOff's inlined copies in psndENV_stop retail's register order.
void psndSFXOff(s32 id) {
	PaperSoundEffect* sfx;
	SoundEffectEntry* entry;
	u32 flags;

	if ((u32)id == -1) {
		return;
	}
	sfx = &pssfx[id & 0xFF];
	if ((u32)sfx->unk0 == -1) {
		return;
	}
	if (((u16*)sfx)[3] & 1) {
		sfx->unk0 = -1;
		sfx->unk18 = -1;
		return;
	}
	entry = &pssfxlist[sfx->unk0 & 0x1FFF];
	if ((u32)sfx->unk18 != -1) {
		flags = entry->flags;
		if (flags & 0x80000000) {
			SoundSSStopCh(sfx->unk18);
		}
		else if (flags & 0x40000000) {
			SoundSongStopCh(sfx->unk18);
		}
		else {
			SoundEfxStop(sfx->unk18);
		}
	}
	sfx->unk0 = -1;
	sfx->unk18 = -1;
}

#pragma opt_propagation off // without it the high-part extract reuses r3, so the sfx pointer lands in r4
s32 psndSFXChk(u32 id) {
	PaperSoundEffectU16* sfx;
	u32 high;
	s32 val;

	if (id == -1) {
		return -1;
	}
	sfx = (PaperSoundEffectU16*)&pssfx[id & 0xFF];
	high = (id >> 8) & 0xFFFF;
	if (high != sfx->unk4) {
		return -1;
	}
	val = sfx->unk0;
	return (s32)(~((val + 1) | (-1 - val))) >> 31;
}
#pragma opt_propagation reset

u8 psndSFX_get_vol(s32 id) {
	PaperSoundEffect* sfx;

	if ((u32)id == -1) {
		return 0;
	}
	sfx = &pssfx[id & 0xFF];
	if ((u32)sfx->unk0 == -1) {
		return 0;
	}
	return sfx->unk8[0];
}

// Each case stops the slot's effect through psndSFXOff, which MWCC inlines (the doubled `!= -1` test is
// the caller's guard plus psndSFXOff's own). The loop indexes psenv, so the walking pointer is a compiler
// induction variable and takes the address directly (a named pointer set to psenv keeps `addi r0; mr`).
void psndENV_stop(u32 id) {
	PaperSoundEnv* env;
	int i;

	for (i = 0; i < 16; i++) {
		env = &psenv[i];
		if (*(u16*)&env->unk4 != id) {
			continue;
		}
		switch (env->unk6[2]) {
		case 0:
			if ((u32)*(s32*)(env->unk6 + 0xE) != -1) {
				psndSFXOff(*(s32*)(env->unk6 + 0xE));
			}
			env->unk4 = 0;
			break;
		case 1:
			if ((u32)*(s32*)(env->unk6 + 0xE) != -1) {
				psndSFXOff(*(s32*)(env->unk6 + 0xE));
			}
			env->unk4 = 0;
			break;
		case 2:
			if ((u32)*(s32*)(env->unk6 + 0xE) != -1) {
				psndSFXOff(*(s32*)(env->unk6 + 0xE));
			}
			env->unk4 = 0;
			break;
		case 3:
			if ((u32)*(s32*)(env->unk6 + 0xE) != -1) {
				psndSFXOff(*(s32*)(env->unk6 + 0xE));
			}
			env->unk4 = 0;
			break;
		}
	}
}

static inline s32 psndENVMasterVol(PaperSoundWork* ps) {
	return ps->unk5A;
}

static inline void psndSFX_lpf(s32 id, u16 lpf) {
	PaperSoundEffect* sfx;
	SoundEffectEntry* entry;
	s32 no;

	if ((u32)id == -1) {
		return;
	}
	sfx = &pssfx[id & 0xFF];
	no = sfx->unk0;
	if ((u32)no == -1) {
		return;
	}
	*(u16*)&sfx->unk8[0xE] = lpf;
	entry = &pssfxlist[no & 0x1FFF];
	if (!(((u16*)sfx)[3] & 1)) {
		if (!(entry->flags & 0x80000000)) {
			if (!(entry->flags & 0x80000000)) {
				SoundEfxSetLPF(sfx->unk18, (s16)lpf);
			}
		}
	}
}

static inline u16 psndENVGetLPF(PaperSoundEnv* env) {
	u16 lpf;

	lpf = 0;
	if (*(u16*)&psnd.unk18 == *(u16*)&env->unk4) {
		lpf = psnd.unk1C[0];
	}
	if (*(u16*)&psnd.unk1A == *(u16*)&env->unk4) {
		lpf = psnd.unk1C[1];
	}
	return lpf;
}

// volume for one env slot: the matching work item (or NULL) is found from env->unk4
static inline u8 psndENVGetVol(PaperSoundEnv* env, PaperSoundWork* ps) {
	u8* work;
	u16 id;
	s16 t;
	s32 x;

	work = NULL;
	id = *(u16*)&env->unk4;
	if (*(u16*)&ps->unk18 == id) {
		work = (u8*)ps;
	}
	if (*(u16*)&ps->unk1A == id) {
		work = (u8*)ps + 0xC;
	}
	if (work == NULL) {
		return (u8)(env->unk6[0] * ps->unk5A / 100);
	}
	if (*(u8*)(work + 6) == 0) {
		t = 0x7F;
	}
	else {
		t = *(s16*)(work + 0xA) * 0x7F / *(s16*)(work + 8);
	}
	if (*(u8*)(work + 0) == 0) {
		x = env->unk6[0] * t / 127;
		return (u8)(ps->unk5A * x / 100);
	}
	x = env->unk6[0] * t / 127;
	// one expression: the /100 is lowered before the scaled volume, and the master volume (a call, so
	// MWCC orders it after the scaled volume) is loaded after the inner divide
	return (u8)(psndENVMasterVol(ps) * (s16)(*(s16*)(work + 4) * x / *(s16*)(work + 2)) / 100);
}

// Both env loops index psenv, and __psndSFXOn's u8 volume lets the inlined psndENVGetVol result go straight
// into r4. The LPF update is psndENVGetLPF plus psndSFX_lpf (built like psndSFX_pit, with its doubled
// SS-flag test), and the last loop's `active` check skips only the fade work, not the LPF slew.
void psndENVMain(void) {
	s32 target;
	u16 span;
	s32 i;
	u8* work;
	u8* ramp;
	s32 sfxId;
	s32 rnd;
	u8 vol;
	f32 f31;
	f32 f30;
	PaperSoundEnv* env;
	PaperSoundEffect* sfx;


	target = 0x64;
	if ((psnd.unk56 & 0x20) && !(psnd.unk56 & 0x40)) {
		target = 0x50;
	}

	if (seqGetSeq() != SEQ_MAP_CHANGE && (gp->retraceCount & 1)) {
		if (psnd.unk5A < target) {
			psnd.unk5A++;
		}
		if (psnd.unk5A > target) {
			psnd.unk5A--;
		}
	}

	for (i = 0; i < 16; i++) {
		env = &psenv[i];
		if (*(u16*)&env->unk4 == 0) {
			continue;
		}
		switch (env->unk6[2]) {
		case 0:
			switch (env->unk6[7]) {
			case 0:
				*(u16*)(env->unk6 + 8) -= 1;
				if (*(u16*)(env->unk6 + 8) == 0) {
					env->unk6[7]++;
				}
				continue;
			case 1:
				*(f32*)(env->unk6 + 0x12) = 0.0f;
				*(f32*)(env->unk6 + 0x16) = 0.0f;
				*(f32*)(env->unk6 + 0x1A) = 0.0f;
				f31 = (f32)irand(0x168);
				rnd = env->unk6[5] * 10 + irand(env->unk6[6] * 10);
				f30 = 6.2831855f * f31 / 360.0f;
				f31 = (f32)rnd;
				*(f32*)(env->unk6 + 0x12) += f31 * (f32)sin(f30);
				*(f32*)(env->unk6 + 0x1A) -= f31 * (f32)cos(f30);
				vol = psndENVGetVol(env, &psnd);
				*(s32*)(env->unk6 + 0xE) = __psndSFXOn(*(u32*)env->unk0 | 0x05000000, vol, 0xFF, 0, (Vec*)(env->unk6 + 0x12), 0, *(u16*)(env->unk6 + 0xA), 0);
				env->unk6[7]++;
				continue;
			case 2:
				sfxId = *(s32*)(env->unk6 + 0xE);
				if ((u32)sfxId != -1) {
					sfx = &pssfx[sfxId & 0xFF];
					if ((u32)sfx->unk18 != -1) {
						continue;
					}
					if (*(u16*)&sfx->unk6 & 1) {
						continue;
					}
				}
				*(s32*)(env->unk6 + 0xE) = -1;
				env->unk6[7] = 0;
				*(u16*)(env->unk6 + 8) = 0;
				switch (env->unk6[2]) {
				case 0:
					*(u16*)(env->unk6 + 8) = env->unk6[4] * 60;
					*(u16*)(env->unk6 + 8) += irand(env->unk6[3]) * 60;
					break;
				case 2:
					*(u16*)(env->unk6 + 8) = env->unk6[4] * 60;
					*(u16*)(env->unk6 + 8) += irand(env->unk6[3]) * 60;
					break;
				case 1:
				case 3:
					break;
				}
				continue;
			default:
				continue;
			}
			break;
		case 1:
			switch (env->unk6[7]) {
			case 0:
				vol = psndENVGetVol(env, &psnd);
				*(s32*)(env->unk6 + 0xE) = __psndSFXOn(*(u32*)env->unk0 | 0x04000000, vol, 0xFF, 0, (Vec*)(env->unk6 + 0x12), 0, *(u16*)(env->unk6 + 0xA), 0);
				env->unk6[7]++;
				continue;
			case 1:
				sfxId = *(s32*)(env->unk6 + 0xE);
				if ((u32)sfxId != -1) {
					sfx = &pssfx[sfxId & 0xFF];
					if ((u32)sfx->unk18 != -1) {
						continue;
					}
					if (*(u16*)&sfx->unk6 & 1) {
						continue;
					}
				}
				*(s32*)(env->unk6 + 0xE) = -1;
				env->unk6[7] = 0;
				*(u16*)(env->unk6 + 8) = 0;
				switch (env->unk6[2]) {
				case 0:
					*(u16*)(env->unk6 + 8) = env->unk6[4] * 60;
					*(u16*)(env->unk6 + 8) += irand(env->unk6[3]) * 60;
					break;
				case 2:
					*(u16*)(env->unk6 + 8) = env->unk6[4] * 60;
					*(u16*)(env->unk6 + 8) += irand(env->unk6[3]) * 60;
					break;
				case 1:
				case 3:
					break;
				}
				continue;
			default:
				continue;
			}
			break;
		case 2:
			switch (env->unk6[7]) {
			case 0:
				*(u16*)(env->unk6 + 8) -= 1;
				if (*(u16*)(env->unk6 + 8) == 0) {
					env->unk6[7]++;
				}
				continue;
			case 1:
				vol = psndENVGetVol(env, &psnd);
				*(s32*)(env->unk6 + 0xE) = __psndSFXOn(*(u32*)env->unk0 | 0x04000000, vol, 0xFF, 0, (Vec*)(env->unk6 + 0x12), 0, *(u16*)(env->unk6 + 0xA), 0);
				env->unk6[7]++;
				continue;
			case 2:
				sfxId = *(s32*)(env->unk6 + 0xE);
				if ((u32)sfxId != -1) {
					sfx = &pssfx[sfxId & 0xFF];
					if ((u32)sfx->unk18 != -1) {
						continue;
					}
					if (*(u16*)&sfx->unk6 & 1) {
						continue;
					}
				}
				*(s32*)(env->unk6 + 0xE) = -1;
				env->unk6[7] = 0;
				*(u16*)(env->unk6 + 8) = 0;
				switch (env->unk6[2]) {
				case 0:
					*(u16*)(env->unk6 + 8) = env->unk6[4] * 60;
					*(u16*)(env->unk6 + 8) += irand(env->unk6[3]) * 60;
					break;
				case 2:
					*(u16*)(env->unk6 + 8) = env->unk6[4] * 60;
					*(u16*)(env->unk6 + 8) += irand(env->unk6[3]) * 60;
					break;
				case 1:
				case 3:
					break;
				}
				continue;
			default:
				continue;
			}
			break;
		case 3:
			switch (env->unk6[7]) {
			case 0:
				vol = psndENVGetVol(env, &psnd);
				*(s32*)(env->unk6 + 0xE) = __psndSFXOn(*(u32*)env->unk0 | 0x06000000, vol, 0xFF, 0, (Vec*)(env->unk6 + 0x12), 0, *(u16*)(env->unk6 + 0xA), 0);
				env->unk6[7]++;
				continue;
			case 1:
				sfxId = *(s32*)(env->unk6 + 0xE);
				if ((u32)sfxId != -1) {
					sfx = &pssfx[sfxId & 0xFF];
					if ((u32)sfx->unk18 != -1) {
						continue;
					}
					if (*(u16*)&sfx->unk6 & 1) {
						continue;
					}
				}
				*(s32*)(env->unk6 + 0xE) = -1;
				env->unk6[7] = 0;
				*(u16*)(env->unk6 + 8) = 0;
				switch (env->unk6[2]) {
				case 0:
					*(u16*)(env->unk6 + 8) = env->unk6[4] * 60;
					*(u16*)(env->unk6 + 8) += irand(env->unk6[3]) * 60;
					break;
				case 2:
					*(u16*)(env->unk6 + 8) = env->unk6[4] * 60;
					*(u16*)(env->unk6 + 8) += irand(env->unk6[3]) * 60;
					break;
				case 1:
				case 3:
					break;
				}
				continue;
			default:
				continue;
			}
			break;
		}
	}

	for (i = 0; i < 16; i++) {
		env = &psenv[i];
		if (*(u16*)&env->unk4 == 0) {
			continue;
		}
		sfxId = *(s32*)(env->unk6 + 0xE);
		if ((u32)sfxId == -1) {
			continue;
		}
		switch (env->unk6[2]) {
		case 0:
			switch (env->unk6[7]) {
			case 2:
			vol = psndENVGetVol(env, &psnd);
#ifdef MUST_MATCH
			sfxId = *(volatile s32*)(env->unk6 + 0xE); // the target reloads env->unk14 here; volatile forces the reload
#else
			sfxId = *(s32*)(env->unk6 + 0xE);
#endif
			if ((u32)sfxId != -1) {
				sfx = &pssfx[sfxId & 0xFF];
				if ((u32)sfx->unk0 != -1) {
					sfx->unk8[0] = vol;
				}
			}
			span = psndENVGetLPF(env);
			psndSFX_lpf(*(s32*)(env->unk6 + 0xE), span);
				break;
			}
			continue;
		case 1:
			switch (env->unk6[7]) {
			case 1:
			vol = psndENVGetVol(env, &psnd);
#ifdef MUST_MATCH
			sfxId = *(volatile s32*)(env->unk6 + 0xE); // the target reloads env->unk14 here; volatile forces the reload
#else
			sfxId = *(s32*)(env->unk6 + 0xE);
#endif
			if ((u32)sfxId != -1) {
				sfx = &pssfx[sfxId & 0xFF];
				if ((u32)sfx->unk0 != -1) {
					sfx->unk8[0] = vol;
				}
			}
			span = psndENVGetLPF(env);
			psndSFX_lpf(*(s32*)(env->unk6 + 0xE), span);
				break;
			}
			continue;
		case 2:
			switch (env->unk6[7]) {
			case 2:
			vol = psndENVGetVol(env, &psnd);
#ifdef MUST_MATCH
			sfxId = *(volatile s32*)(env->unk6 + 0xE); // the target reloads env->unk14 here; volatile forces the reload
#else
			sfxId = *(s32*)(env->unk6 + 0xE);
#endif
			if ((u32)sfxId != -1) {
				sfx = &pssfx[sfxId & 0xFF];
				if ((u32)sfx->unk0 != -1) {
					sfx->unk8[0] = vol;
				}
			}
			span = psndENVGetLPF(env);
			psndSFX_lpf(*(s32*)(env->unk6 + 0xE), span);
				break;
			}
			continue;
		case 3:
			switch (env->unk6[7]) {
			case 1:
			vol = psndENVGetVol(env, &psnd);
#ifdef MUST_MATCH
			sfxId = *(volatile s32*)(env->unk6 + 0xE); // the target reloads env->unk14 here; volatile forces the reload
#else
			sfxId = *(s32*)(env->unk6 + 0xE);
#endif
			if ((u32)sfxId != -1) {
				sfx = &pssfx[sfxId & 0xFF];
				if ((u32)sfx->unk0 != -1) {
					sfx->unk8[0] = vol;
				}
			}
			span = psndENVGetLPF(env);
			psndSFX_lpf(*(s32*)(env->unk6 + 0xE), span);
				break;
			}
			continue;
		default:
			continue;
		}
	}

	for (i = 0; i < 2; i++) {
		ramp = (u8*)&psnd + i * 2;
		if (*(u16*)(ramp + 0x18) != 0) {
			work = (u8*)&psnd + i * 0xC;
			if (*(u8*)(work + 0) == 1) {
				*(s16*)(work + 4) += 1;
				if (*(s16*)(work + 4) >= *(s16*)(work + 2)) {
					*(u8*)(work + 0) = 0;
				}
			}
			if (*(u8*)(work + 0) == 2) {
				*(s16*)(work + 4) -= 1;
				if (*(s16*)(work + 4) <= 0) {
					psndENV_stop(*(u16*)(ramp + 0x18));
					*(u8*)(work + 0) = 0;
				}
			}
			if (*(u8*)(work + 0) == 4) {
				if (*(s16*)(work + 4) > 0) {
					*(s16*)(work + 4) -= 1;
				}
			}
			if (*(u8*)(work + 6) == 8) {
				if (*(s16*)(work + 0xA) > *(s16*)(work + 8) * *(u8*)(work + 7) / 100) {
					*(s16*)(work + 0xA) -= 1;
				}
			}
			if (*(u8*)(work + 6) == 0x10) {
				if (*(s16*)(work + 0xA) < *(s16*)(work + 8)) {
					*(s16*)(work + 0xA) += 1;
				}
				else {
					*(u8*)(work + 6) = 0;
				}
			}
		}
		if (*(u16*)(ramp + 0x1C) < *(u16*)(ramp + 0x20)) {
			*(u16*)(ramp + 0x1C) += 100;
		}
		if (*(u16*)(ramp + 0x1C) > *(u16*)(ramp + 0x20)) {
			*(u16*)(ramp + 0x1C) -= 1000;
			if (*(u16*)(ramp + 0x1C) < *(u16*)(ramp + 0x20)) {
				*(u16*)(ramp + 0x1C) = *(u16*)(ramp + 0x20);
			}
		}
	}
}

BOOL psndENVOn(s32 a, s32 b) {
	return psndENVOn_f_d(a, b, 0x3E8, 0);
}

// per-env work item at psnd + (id & 0xF) * 0xC
// (not in a header; the original's layout is not declared there yet)


static inline PaperSoundEnv* psndENVGetFree(void) {
	s32 i;

	for (i = 0; i < 16; i++) {
		if (*(u16*)&psenv[i].unk4 == 0) {
			return &psenv[i];
		}
	}
	return NULL;
}

static inline s32 psndENVFindName(s32 name) {
	const char** namePtr;
	s32 index;

	namePtr = psenvlistname;
	if (name >= 0) {
		return name;
	}
	for (index = 0; index < 0x76; index++, namePtr++) {
		if (!strcmp(*namePtr, (char*)name)) {
			break;
		}
	}
	if (index >= 0x76) {
		index = 0;
	}
	return index;
}

// name lookup and free-env scan are inlined static helpers; with them index lands
// in r31 and env in r30 as the target, and the u16 slot offset `ofs` declared after `work` puts it in r27 and
// active in r29. The final 0x7D00 stores index the u16 fields the way `active` does (`(u8*)psnd.unk1C + ofs`):
// the cast keeps the frontend from folding both into one `psnd + ofs` temp, so the address is recomputed as in
// retail. The empty `case 1`/`case 3` fix the switch's 2-then-0 tree.
s32 psndENVOn_f_d(s32 id, s32 name, s32 value, s32 flag) {
	s32 index;
	PaperSoundEnv* env;
	u16* active;
	PaperSoundEnvWork* work;
	s32 ofs;
	SoundEnvList* list;
	s32 slot;

	list = psenvlist;
	index = psndENVFindName(name);

	slot = id & 0xF;
	work = (PaperSoundEnvWork*)((u8*)&psnd + slot * 0xC);
	if (psnd.unk56 & 2) {
		return 0;
	}

	ofs = slot * 2;
	active = (u16*)((u8*)&psnd.unk18 + ofs);

	if (active[0] != 0) {
		if (!(id & 0x10)) {
			if (id & 0x20) {
				if (id & 0x100) {
					work->unk0 = 1;
					work->unk2 = (s16)((u32)value / 16.666666f);
					work->unk4 = 0;
				}
				else {
					work->unk0 = 0;
					work->unk2 = 1;
					work->unk4 = 1;
				}
				if (id & 0x80) {
					if (work->unk6 == 8) {
						work->unk6 = 0x10;
						work->unk8 = (s16)((u32)value / 16.666666f);
						work->unkA = (s16)((work->unk8 * work->unk7) / 100);
					}
				}
				goto done;
			}
			if (active[0] == index) {
				if (work->unk0 != 2) {
					goto done;
				}
			}
		}
		psndENV_stop(active[0]);
		active[0] = 0;
	}

	for (;;) {
		if (list->nameId == 0x76) {
			break;
		}
		if (list->nameId == index) {
			env = psndENVGetFree();
			if (env != NULL) {
				env->unk4 = (s16)list->nameId;
				*(u32*)env->unk0 = list->flags;
				env->unk6[0] = list->volume;
				env->unk6[1] = list->field_0x5;
				env->unk6[2] = list->mode;
				env->unk6[3] = list->intervalRand;
				env->unk6[4] = list->interval;
				*(Vec*)(env->unk6 + 0x12) = *(Vec*)&list->posX;
				env->unk6[5] = list->radius;
				env->unk6[6] = list->radiusRand;
				*(u16*)(env->unk6 + 0xA) = list->span;
				env->unk6[7] = 0;
				*(u16*)(env->unk6 + 8) = 0;

				switch (env->unk6[2]) {
				case 0:
					*(u16*)(env->unk6 + 8) = env->unk6[4] * 60;
					*(u16*)(env->unk6 + 8) += irand(env->unk6[3]) * 60;
					break;
				case 1:
					break;
				case 2:
					*(u16*)(env->unk6 + 8) = env->unk6[4] * 60;
					*(u16*)(env->unk6 + 8) += irand(env->unk6[3]) * 60;
					break;
				case 3:
					break;
				default:
					break;
				}
			}
		}
		list++;
	}

	active[0] = index;
	work->unk0 = 0;
	*(u16*)&((u8*)psnd.unk1C)[ofs] = 0x7D00;
	*(u16*)&((u8*)psnd.unk20)[ofs] = 0x7D00;
	if (id & 0x100) {
		work->unk0 = 1;
		work->unk2 = (s16)((u32)value / 16.666666f);
		work->unk4 = 0;
		work->unk8 = 1;
		work->unkA = 1;
		work->unk6 = 0;
		work->unk8 = 1;
		work->unkA = 1;
	}
	else {
		work->unk0 = 0;
		work->unk2 = 1;
		work->unk4 = 1;
		work->unk8 = 1;
		work->unkA = 1;
		work->unk6 = 0;
		work->unk8 = 1;
		work->unkA = 1;
	}

done:
	return 1;
}

#pragma dont_inline on // the original keeps psndENVOff as a real call in psndSetFlag/psndStopAllFadeOut
BOOL psndENVOff(u32 r3) { //probably "id"
	return psndENVOff_f_d(r3, 0x3E8, 0);
}
#pragma dont_inline reset

s32 psndENVOff_f_d(s32 id, s32 value, s32 flag) {
	s32 silent = silent_tbl[(id >> 12) & 0xF];
	u16* active = (u16*)&psnd.unk18 + (id & 0xF);
	PaperSoundEnvWork* work;

	if (active[0] == 0) {
		return 0;
	}
	work = (PaperSoundEnvWork*)((u8*)&psnd + (id & 0xF) * 0xC);

	if (id & 0x200) {
		if (work->unk0 == 2) {
		}
		else if (work->unk0 != 4) {
			work->unk0 = 2;
			work->unk2 = (s16)((u32)value / 16.666666f);
			work->unk4 = work->unk2;
		}
		else {
			work->unk0 = 2;
		}
	}
	else if (id & 0x400) {
		if (work->unk0 != 2 && work->unk0 != 4) {
			work->unk0 = 4;
			work->unk2 = (s16)((u32)value / 16.666666f);
			work->unk4 = work->unk2;
		}
	}
	else if (id & 0x800) {
		work->unk6 = 8;
		work->unk8 = (s16)((u32)value / 16.666666f);
		work->unkA = work->unk8;
		work->unk7 = silent;
	}
	else {
		psndENV_stop(active[0]);
		active[0] = 0;
	}
	return 1;
}

s32 psndENV_LPF(s32 id, u16 freq) {
	if (((u16*)((u8*)&psnd + 0x18))[id & 0xF] == 0) {
		return 0;
	}
	if (freq == 0) {
		freq = 0x7D00;
	}
	((u16*)((u8*)&psnd + 0x20))[id & 0xF] = freq;
	return 1;
}

#ifdef MUST_MATCH
/// @todo reconstructed: uncalled in retail (the linker strips it). Fades env slot `id` to `vol` over `time` ms,
/// as psndENVOff_f_d's 0x800 branch does with the silent level. Generated first in this deferred unit, so its
/// signed int->float conversion pools that double ahead of psndENVOff_f_d's unsigned one, as in retail's .rodata.
s32 psndENV_vol(s32 id, s32 vol, s32 time) {
	u16* active = (u16*)&psnd.unk18 + (id & 0xF);
	PaperSoundEnvWork* work;

	if (active[0] == 0) {
		return 0;
	}
	work = (PaperSoundEnvWork*)((u8*)&psnd + (id & 0xF) * 0xC);
	work->unk6 = 8;
	work->unk8 = (s16)(time / 16.666666f);
	work->unkA = work->unk8;
	work->unk7 = vol;
	return 1;
}
#endif

// Deferred generation emits these in reverse order before the function-local .sbss objects.
BOOL _psndPushGroup_trg;
BOOL init_f;
