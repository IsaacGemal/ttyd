#include "sequence/seq_load.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "driver/windowdrv.h"
#include "manager/fontmgr.h"
#include "sdk/texPalette.h"
#include "win/win_main.h"
#include "window/win_mario.h"
#include <stdio.h>
#include "countdown.h"
#include "driver/fadedrv.h"
#include "driver/msgdrv.h"
#include "driver/seqdrv.h"
#include "driver/swdrv.h"
#include "event.h"
#include "evt/evt_sub.h"
#include "manager/cardmgr.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/filemgr.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include "nameent.h"
#include "pmario_sound.h"
#include "sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include <string.h>
#include <math.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") f32 lbl_80349DE8[2] = { -1.0f, 1.0f };

typedef struct LoadWork {
    u16 field_0x0; //0x0
    u8 field_0x2[0x4 - 0x2];
    FileEntry* file; //0x4
    f32 field_0x8; //0x8
    f32 field_0xC; //0xC
    f32 field_0x10; //0x10
    f32 field_0x14; //0x14
    s32 field_0x18; //0x18
    s32 field_0x1C; //0x1C
    s32 field_0x20; //0x20
    s32 field_0x24; //0x24
    s32 field_0x28; //0x28
    f32 field_0x2C; //0x2C
    s32 field_0x30; //0x30
    s32 field_0x34; //0x34
    s32 field_0x38; //0x38
    EventEntry* field_0x3C; //0x3C
    s32 field_0x40; //0x40
    s32 field_0x44; //0x44
    s32 field_0x48; //0x48
} LoadWork;

typedef struct WinDt {
    f32 x; //0x0
    f32 y; //0x4
    f32 z; //0x8
    f32 w; //0xC
    s32 unk10; //0x10
    s32 unk14; //0x14
    u8 b18; //0x18
    u8 b19; //0x19
    u8 b1a; //0x1A
    u8 b1b; //0x1B
} WinDt;

// loadDraw copies the 16-entry angle table to the stack in one 64-byte block before indexing it;
// a struct assignment reproduces that copy (an array typedef cannot be assigned).
struct LoadAngleTable
{
    f32 v[16];
};

#ifdef MUST_MATCH
// loadMain reaches the three window tables from one base (the object before them, lbl_80349DE8, plus 0x40/0x13c/0x200),
// which is what the original's code does
#define WDT_TABLE ((WinDt*)(base2 + 0x40))
#define WDT2_TABLE ((WinDt*)(base2 + 0x13c))
#define WDT3_TABLE ((WinDt*)(base2 + 0x200))
#define WDT(i)  (wd = WDT_TABLE, wd[i])
#define WDT2(i) (wd = WDT2_TABLE, wd[i])
#define WDT3(i) (wd = WDT3_TABLE, wd[i])
#else
#define WDT_TABLE ((WinDt*)win_dt)
#define WDT2_TABLE ((WinDt*)win_dt2)
#define WDT3_TABLE ((WinDt*)win_dt3)
#define WDT(i)  (((WinDt*)win_dt)[i])
#define WDT2(i) (((WinDt*)win_dt2)[i])
#define WDT3(i) (((WinDt*)win_dt3)[i])
#endif

static LoadWork work;
static LoadWork* wp = &work;

extern s32 evt_memcard_check[];
extern s32 evt_memcard_start[];
extern s32 evt_memcard_delete[];
extern s32 evt_memcard_copy[];
extern s32 evt_memcard_save[];
#ifdef MUST_MATCH
#endif
u32 gap_80349DF0[14] = {
    0xBF800000, 0x3F800000, 0x3F800000, 0xBF800000, 0x3F800000, 0xBF800000, 0xBF800000, 0x3F800000,
    0xBF800000, 0x3F800000, 0x3F800000, 0xBF800000, 0x3F800000, 0xBF800000,
};
u32 win_dt[9][7] = {
    { 0xC30C0000, 0x42960000, 0xC2F00000, 0x42700000, 0x0000000D, 0x00000000, 0x07020101 },
    { 0x430C0000, 0x42960000, 0xC2F00000, 0x42700000, 0x0000000C, 0x00000000, 0x08030000 },
    { 0xC30C0000, 0xC2820000, 0xC2F00000, 0x42700000, 0x0000000B, 0x00000000, 0x00040303 },
    { 0x430C0000, 0xC2820000, 0xC2F00000, 0x42700000, 0x0000000A, 0x00000000, 0x01050202 },
    { 0xC3480000, 0xC3160000, 0xC2A00000, 0x00000000, 0x00000010, 0x0000001E, 0x02070605 },
    { 0x00000000, 0xC3160000, 0xC2A00000, 0x00000000, 0x00000011, 0x0000001E, 0x02070406 },
    { 0x43480000, 0xC3160000, 0xC2A00000, 0x00000000, 0x00000012, 0x0000001E, 0x03080504 },
    { 0xC3020000, 0xC33E0000, 0xC2C80000, 0x00000000, 0x00000013, 0x0000001F, 0x04000808 },
    { 0x43020000, 0xC33E0000, 0xC2C80000, 0x00000000, 0x00000014, 0x0000001F, 0x05010707 },
};

u32 win_dt2[7][7] = {
    { 0xC30C0000, 0x42960000, 0xC2F00000, 0x42700000, 0x0000000D, 0x00000000, 0x06020101 },
    { 0x430C0000, 0x42960000, 0xC2F00000, 0x42700000, 0x0000000C, 0x00000000, 0x06030000 },
    { 0xC30C0000, 0xC2820000, 0xC2F00000, 0x42700000, 0x0000000B, 0x00000000, 0x00060303 },
    { 0x430C0000, 0xC2820000, 0xC2F00000, 0x42700000, 0x0000000A, 0x00000000, 0x01060202 },
    { 0xC3480000, 0xC3160000, 0xC2A00000, 0x00000000, 0x00000010, 0x0000001E, 0x02070605 },
    { 0x00000000, 0xC3160000, 0xC2A00000, 0x00000000, 0x00000011, 0x0000001E, 0x02070406 },
    { 0x43480000, 0xC3160000, 0xC2A00000, 0x00000000, 0x00000012, 0x0000001E, 0x03010606 },
};

u32 win_dt3[4][7] = {
    { 0xC30C0000, 0x42960000, 0xC2F00000, 0x42700000, 0x0000000D, 0x00000000, 0x02020101 },
    { 0x430C0000, 0x42960000, 0xC2F00000, 0x42700000, 0x0000000C, 0x00000000, 0x03030000 },
    { 0xC30C0000, 0xC2820000, 0xC2F00000, 0x42700000, 0x0000000B, 0x00000000, 0x00000303 },
    { 0x430C0000, 0xC2820000, 0xC2F00000, 0x42700000, 0x0000000A, 0x00000000, 0x01010202 },
};

const GXColor dat_8041bd68 = {0, 0, 0, 0xFF};
const GXColor dat_8041bd6c = {0, 0, 0, 0xFF};
const GXColor dat_8041bd70 = {0, 0, 0, 0xFF};
const GXColor dat_8041bd74 = {0, 0, 0, 0xFF};
const GXColor dat_8041bd78 = {0, 0, 0, 0xFF};
const GXColor dat_8041bd7c = {0, 0, 0, 0xFF};
const GXColor dat_8041bd80 = {0, 0, 0, 0xFF};
// Colors/data the loadDraw screen reads by name (the original's .sdata2 block).
const GXColor dat_8041bd84 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_8041bd88 = {0xA0, 0xA0, 0xA0, 0xFF};
const GXColor dat_8041bd8c = {0x80, 0x80, 0x80, 0x80};
const GXColor dat_8041bd90 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_8041bd94 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_8041bd98 = {0xC0, 0xC0, 0xC0, 0xFF};
const GXColor dat_8041bd9c = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_8041bda0 = {0x20, 0x20, 0x20, 0xFF};
const GXColor dat_8041bda4 = {0, 0, 0, 0xFF};
const GXColor dat_8041bda8 = {0, 0, 0, 0xFF};
const GXColor dat_8041bdac = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_8041bdb0 = {0xFF, 0xFF, 0xFF, 0xFF};
const u32 dat_8041bdb4 = 0x40;
const GXColor dat_8041bdb8 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_8041bdbc = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_8041bdc0 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_8041bdc4 = {0xFF, 0xFF, 0xFF, 0xFF};
const u32 dat_8041bdc8 = 0x40;
const GXColor dat_8041bdcc = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_8041bdd0 = {0xFF, 0xFF, 0xFF, 0xFF};
const u32 dat_8041bdd4 = 0x80;
const u32 dat_8041bdd8 = 0x80;
const char str_8041BE34[] = "\202W\202W\000\000";
const char str_8041BE40[] = " \000";
const f32 float_0_8041bde4 = 0.0f;
const f32 float_1_8041bde8 = 1.0f;
const f32 float_neg304_8041bdec = -304.0f;
const f32 float_240_8041bdf0 = 240.0f;
const f32 float_neg240_8041bdf4 = -240.0f;
const f32 float_neg272_8041bdf8 = -272.0f;
const f32 float_272_8041bdfc = 272.0f;
const f32 float_304_8041be00 = 304.0f;
const f32 float_60_8041be04 = 60.0f;
const f32 float_neg250_8041be08 = -250.0f;
const f32 float_200_8041be0c = 200.0f;
const f32 float_500_8041be10 = 500.0f;
const f32 float_40_8041be14 = 40.0f;
const f32 float_20_8041be18 = 20.0f;
const f32 float_0p6_8041be1c = 0.6f;
const f32 float_100_8041be20 = 100.0f;
const f32 float_84_8041be24 = 84.0f;
const f32 float_0p5_8041be28 = 0.5f;
const f32 float_12_8041be2c = 12.0f;
const f32 float_70_8041be30 = 70.0f;
const f32 float_8_8041be44 = 8.0f;
const f32 float_22_8041be48 = 22.0f;
const f32 float_1p5_8041be4c = 1.5f;
const f32 float_106_8041be50 = 106.0f;
const f32 float_52_8041be54 = 52.0f;
const f32 float_0p05_8041be58 = 0.05f;
const f32 float_120_8041be5c = 120.0f;
const f32 float_140_8041be60 = 140.0f;
const f32 float_neg150_8041be64 = -150.0f;
const f32 float_300_8041be68 = 300.0f;

const Vec vec3_802ed670 = {0, 0, 0};
const Vec vec3_802ed67c = {1, 1, 1};
const Vec vec3_802ed688 = {0, 0, 0};
const Vec vec3_802ed694 = {1, 1, 1};
const Vec vec3_802ed6a0 = {0, 0, 0};
const Vec vec3_802ed6ac = {1, 1, 1};
const Vec vec3_802ed6b8 = {0, 0, 0};
const Vec vec3_802ed6c4 = {1, 1, 1};
const Vec vec3_802ed6d0 = {0, 100, 0};
const Vec vec3_802ed6dc = {1, 1, 1};
const Vec vec3_802ed6e8 = {0, 0, 0};
const Vec vec3_802ed6f4 = {1, 1, 1};
const Vec vec3_802ed700 = {-4, 0, 0};
const Vec vec3_802ed70c = {1, 1, 1};
const Vec vec3_802ed718 = {0, 0, 0};
const Vec vec3_802ed724 = {1, 1, 1};
const Vec vec3_802ed730 = {0, 0, 0};
const Vec vec3_802ed73c = {1, 1, 1};
const Vec vec3_802ed748 = {0, 100, 0};
const Vec vec3_802ed754 = {1, 1, 1};
const Vec vec3_802ed760 = {0, 0, 0};
const Vec vec3_802ed76c = {1, 1, 1};
const Vec vec3_802ed778 = {-4, 0, 0};
const Vec vec3_802ed784 = {1, 1, 1};
const Vec vec3_802ed790 = {0, 0, 0};
const Vec vec3_802ed79c = {1, 1, 1};
const u32 dat_802ed7a8[] = {0x00000000, 0xC2B40000, 0xC3340000, 0xC3070000, 0x42B40000, 0x00000000, 0x43070000, 0x43340000, 0x00000000, 0xC2340000, 0x00000000, 0xC2B40000, 0x42340000, 0x00000000, 0x42B40000, 0x00000000};
const Vec vec3_802ed7e8 = {0, 0, 0};
const Vec vec3_802ed7f4 = {1, 1, 1};
const Vec vec3_802ed800 = {0, 0, 0};
const Vec vec3_802ed80c = {1, 1, 1};
const Vec vec3_802ed818 = {0, 0, 0};
const Vec vec3_802ed824 = {1, 1, 1};
const Vec vec3_802ed830 = {0, 0, 0};
const Vec vec3_802ed83c = {1, 1, 1};
const char str_802ED848[] = "OFF_f_in_out_mario\000";
const char str_802ED85C[] = "OFF_f_in_out_kuppa\000";
const char str_802ED870[] = "OFF_f_in_out_peach\000";
const char str_802ED884[] = "aji_10\000";
const char str_802ED88C[] = "kpa_00\000";
const char str_802ED894[] = "nok_01\000";
const char str_802ED89C[] = "nokotarou\000\000";
const char str_802ED8A8[] = "hei_00\000";
const char str_802ED8B0[] = "mri_00\000";
const char str_802ED8B8[] = "mri_01\000";
const char str_802ED8C0[] = "tou_02\000";
const char str_802ED8C8[] = "usu_00\000";
const char str_802ED8D0[] = "gor_04\000";
const char str_802ED8D8[] = "muj_12\000";
const char str_802ED8E0[] = "muj_00\000";
const char str_802ED8E8[] = "pik_03\000";
const char str_802ED8F0[] = "gor_01\000";
const char str_802ED8F8[] = "resha_return\000\000\000";
const char str_802ED908[] = "pik_01\000";
const char str_802ED910[] = "aji_18\000";
const char str_802ED918[] = "e_bero\000";
const char str_802ED920[] = "gor_00\000";
const char str_802ED928[] = "jin_04\000";
const char str_802ED930[] = "aaa_00\000";
const char str_802ED938[] = "prologue\000\000\000";
const u32 gap_04_802ED944_rodata[] = {0x00000000};
const f64 double_480_802ed948 = 480;
const f64 double_32_802ed950 = 32;
const f64 double_544_802ed958 = 544;
const char str_802ED960[] = "\203f\201[\203^%s\000\000\000";
const char str_802ED96C[] = "\202\261\202\355\202\352\202\304\202\242\202\334\202\267\000";
const char str_802ED97C[] = "\203j\203\205\201[\203Q\201[\203\200\000\000\000";
const char str_802ED98C[] = "\202\315\202\266\202\337\202\251\202\347\000";
const char str_802ED998[] = "\203\214\203x\203\213\201@%s\000";
const char str_802ED9A4[] = "\203v\203\214\203C\202\266\202\251\202\361\201@\000";
const u32 gap_04_802ED9B4_rodata[] = {0x00000000};
const f64 double_90_802ed9b8 = 90;
const f64 double_to_int_mask_802ed9c0 = 4503599627370496;
const f64 double_to_int_802ed9c8 = 4503601774854144;
const char str_802ED9D0[] = "msg_savefile_select";
const char str_802ED9E4[] = "msg_savefile_delete_select\000";
const char str_802EDA00[] = "msg_savefile_copy_select\000\000\000";
const char str_802EDA1C[] = "msg_savefile_copy_point";
const char str_802EDA34[] = "%s/w/jp/name.tpl\000\000\000";
const char str_8041BDDC[] = "next";
const f32 float_30_8041be3c = 30.0f;
const f32 float_50_8041be6c = 50.0f;
const f32 float_2_8041be70 = 2.0f;
const f32 float_neg74_8041be74 = -74.0f;
const f32 float_4_8041be78 = 4.0f;
const f32 float_0p25_8041be7c = 0.25f;
const f32 float_0p125_8041be80 = 0.125f;
const f32 float_1000_8041be84 = 1000.0f;

s32 stone_id[] = { 0x197, 0x198, 0x19A, 0x19C, 0x19D, 0x199, 0x19B };

static EVT_BEGIN(evt_shake)
    CALL(evt_sub_rumble_onoff, 0, 0)
    WAIT_MS(150)
    CALL(evt_sub_rumble_onoff, 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_continue)
    CALL(evt_sub_animgroup_async, PTR((char*)str_802ED848))
    CALL(evt_sub_animgroup_async, PTR((char*)str_802ED85C))
    CALL(evt_sub_animgroup_async, PTR((char*)str_802ED870))
    RETURN
EVT_END()

// The original addresses these .rodata objects as offsets from one base symbol.
#ifdef MUST_MATCH
#define RO(base, off, sym) ((const char*)(base) + (off))
#else
#define RO(base, off, sym) ((const char*)&(sym))
#endif
void continueGame(void) {
    MarioWork* mario;
    const char* base = (const char*)&vec3_802ed670;

    cardCopy2Main(wp->field_0x20);
    *(s32*)(gp->field_0x11BC + 0x14) = wp->field_0x20;
    gp->unk11B8 = wp->field_0x20;
    marioPartyKill();
    mario = marioGetPtr();
    mario->prevPartyMemberId[0] = 0;
    mario->prevPartyMemberId[1] = 0;
    psndSetFlag(4);
    marioPartyEntry(gp->savePartyId[0]);
    marioPartyEntry(gp->savePartyId[1]);
    psndClearFlag(4);
    countDownSaveReStart();
    gp->flags |= 1;


    switch (swByteGet(4)) {
        case 0:
            gp->areaFadeInType = 0;
            gp->areaFadeOutType = 0x1C;
            seqSetSeq(SEQ_MAP_CHANGE, gp->currentMapName, NULL);
            break;
        case 1:
            gp->areaFadeInType = 0x3D;
            gp->areaFadeOutType = 0x38;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x214, str_802ED884), NULL);
            break;
        case 2:
            gp->areaFadeInType = 0x3E;
            gp->areaFadeOutType = 0x3A;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x21C, str_802ED88C), NULL);
            break;
        case 3:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x224, str_802ED894), RO(base, 0x22C, str_802ED89C));
            break;
        case 4:
            gp->areaFadeInType = 0x3D;
            gp->areaFadeOutType = 0x38;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x214, str_802ED884), NULL);
            break;
        case 5:
            gp->areaFadeInType = 0x3E;
            gp->areaFadeOutType = 0x3A;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x238, str_802ED8A8), NULL);
            break;
        case 6:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x240, str_802ED8B0), NULL);
            break;
        case 7:
            gp->areaFadeInType = 0x3D;
            gp->areaFadeOutType = 0x38;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x214, str_802ED884), NULL);
            break;
        case 8:
            gp->areaFadeInType = 0x3E;
            gp->areaFadeOutType = 0x3A;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x248, str_802ED8B8), NULL);
            break;
        case 9:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x250, str_802ED8C0), NULL);
            break;
        case 10:
            gp->areaFadeInType = 0;
            gp->areaFadeOutType = 0x1C;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x258, str_802ED8C8), NULL);
            break;
        case 11:
            gp->areaFadeInType = 0x3E;
            gp->areaFadeOutType = 0x3A;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x260, str_802ED8D0), NULL);
            break;
        case 12:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x258, str_802ED8C8), NULL);
            break;
        case 13:
            gp->areaFadeInType = 0;
            gp->areaFadeOutType = 0x1C;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x268, str_802ED8D8), NULL);
            break;
        case 14:
            gp->areaFadeInType = 0x3E;
            gp->areaFadeOutType = 0x3A;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x258, str_802ED8C8), NULL);
            break;
        case 15:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x270, str_802ED8E0), NULL);
            break;
        case 16:
            gp->areaFadeInType = 0;
            gp->areaFadeOutType = 0x1C;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x278, str_802ED8E8), str_8041BDDC);
            break;
        case 17:
            gp->areaFadeInType = 0x3E;
            gp->areaFadeOutType = 0x3A;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x280, str_802ED8F0), NULL);
            break;
        case 18:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x260, str_802ED8D0), RO(base, 0x288, str_802ED8F8));
            break;
        case 19:
            gp->areaFadeInType = 0x3E;
            gp->areaFadeOutType = 0x3A;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x298, str_802ED908), NULL);
            break;
        case 20:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x2A0, str_802ED910), RO(base, 0x2A8, str_802ED918));
            break;
        case 21:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x2B0, str_802ED920), NULL);
            break;
        case 22:
            gp->areaFadeInType = 0;
            gp->areaFadeOutType = 0x1C;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x2B8, str_802ED928), NULL);
            break;
        case 23:
            stg0_00_init();
            *(s32*)(gp->field_0x11BC + 0x14) = wp->field_0x20;
            marioPartyKill();
            mario = marioGetPtr();
            mario->prevPartyMemberId[0] = 0;
            mario->prevPartyMemberId[1] = 0;
            gp->areaFadeInType = 0;
            gp->areaFadeOutType = 0x19;
            swInit();
            swByteSet(0, 0);
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x2C0, str_802ED930), RO(base, 0x2C8, str_802ED938));
            break;
        case 24:
            gp->areaFadeInType = 0x3D;
            gp->areaFadeOutType = 0x38;
            seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x214, str_802ED884), NULL);
            break;
    }
}
void seq_loadMain(SequenceWork* workArg) {
    switch (workArg->state) {
        case 0: {
            BOOL result;
            if (fileAsyncf(4, 0, str_802EDA34, getMarioStDvdRoot()) != 0U) {
                wp->file = fileAllocf(4, str_802EDA34, getMarioStDvdRoot());
                result = TRUE;
            } else {
                result = FALSE;
            }
            if (result) {
                workArg->state++;
            }
            break;
        }
        case 1:
            wp->field_0x3C = evtEntryType(evt_memcard_check, 0, 0, 0);
            if (wp->field_0x0 & 0x80) {
                wp->field_0x0 &= ~0x80;
                wp->field_0x3C->lwData[10] = 1;
                wp->field_0x3C->printWindowId = wp->field_0x48;
            }
            workArg->state++;
            break;
        case 2:
            if (!evtCheckID(wp->field_0x3C->eventId)) {
                if (evtGetValue(NULL, 0xFAA2B580) != 1) {
                    seqSetSeq(SEQ_TITLE, NULL, NULL);
                    break;
                }
                wp->field_0x3C = NULL;
                workArg->state++;
                wp->field_0x18 = 0;
                wp->field_0x1C = cardGetCode();
                wp->field_0x0 &= ~0x40;
                if (OSGetSoundMode() == 0) {
                    SoundSetOutputMode(0);
                    gp->unk1274 = 0;
                    wp->field_0x40 = 0;
                } else {
                    SoundSetOutputMode(1);
                    if (gp->unk1274 == 0) {
                        wp->field_0x40 = 1;
                    } else {
                        wp->field_0x40 = 2;
                    }
                }
                wp->field_0x44 = (gp->unk1294 == 0);
                wp->field_0x24 = wp->field_0x40;
                wp->field_0x28 = wp->field_0x44;
                wp->field_0x20 = gp->unk11B8;
                wp->field_0x8 = ((f32(*)[7])win_dt)[wp->field_0x20][0];
                wp->field_0xC = ((f32(*)[7])win_dt)[wp->field_0x20][1];
            }
            break;
        case 3: {
            GXColor color = *(GXColor*)&dat_8041bd68;
            fadeEntry(0x1C, 0, color);
            workArg->state++;
            break;
        }
        case 4:
            loadMain(workArg);
            break;
    }
}
void seq_loadExit(SequenceWork* workArg) {
    if (wp->file != NULL) {
        fileFree(wp->file);
    }
    wp->file = NULL;
}
void seq_loadInit(SequenceWork* workArg) {
    memset(wp, 0, sizeof(LoadWork));
}
s32 unk_800f2398(EventEntry* event, BOOL isFirstCall) {
    fadeReset(4);
    wp->field_0x48 = event->printWindowId;
    return 2;
}

// Materialise win_dt/win_dt2/win_dt3 as base2+0x40/0x13c/0x200 with a separate addi, as the original does,
// instead of letting the compiler fold those displacements into the load.
#pragma opt_propagation off
// NONMATCHING (98.49%): full reconstruction. Remaining diffs are register colouring (sel in r30 vs the
// original's r31, wp/wd in r4/r3 vs r3/r4) and the s32->f32 magic double, where the compiler emits an
// anonymous literal instead of the existing double_to_int_802ed9c8 object (folding float_4/float_30 too).
void loadMain(SequenceWork* workArg) {
#ifdef MUST_MATCH
    const char* base2 = (const char*)&lbl_80349DE8;
#endif
    const char* base = (const char*)&vec3_802ed670;
    u32 btn = keyGetButtonTrg(0);
    u32 rep = keyGetButtonRep(0);
    u32 dir = keyGetDirRep(0);
    s32 sel;
    s32 code = cardGetCode();
    WinDt* wd;
    MarioWork* mario;
    s32 i;
    s32 off;
    u16 flag;
    CardData* card;

    GXColor color;
    (void)rep;

    if (cardIsExec() || !fadeIsFinish()) {
        btn = 0;
        dir = 0;
    }
    if (wp->field_0x1C != code) {
        btn = 0;
        dir = 0;
    }
    wp->field_0x1C = code;

    i = 0;
    off = 0;
    while (i < 4) {
        card = cardGetFilePtr();
        flag = *(u16*)((u8*)card + off + 0x2000);
        if (!(flag & 1) || (flag & 2)) {
            break;
        }
        i++;
        off += 0x4000;
    }
    if (i < 4) {
        wp->field_0x0 &= ~0x10;
    } else {
        wp->field_0x0 |= 0x10;
    }

    i = 0;
    off = 0;
    wp->field_0x0 &= ~0x20;
    while (i < 4) {
        card = cardGetFilePtr();
        flag = *(u16*)((u8*)card + off + 0x2000);
        if ((flag & 1) || (flag & 2)) {
            i++;
            off += 0x4000;
        } else {
            break;
        }
    }
    if (i >= 4) {
        wp->field_0x0 |= 0x20;
    }

    switch (wp->field_0x18) {
        case 0:
            wp->field_0x18++;
            /* fallthrough */
        case 1:
            wp->field_0x38 = (s32)msgSearch(RO(base, 0x360, str_802ED9D0));
            sel = wp->field_0x20;

            if (dir & 0x1000) {
                wd = WDT_TABLE;
                wp->field_0x20 = wd[sel].b18;
            } else if (dir & 0x2000) {
                wp->field_0x20 = WDT(sel).b19;
            } else if (dir & 0x4000) {
                wp->field_0x20 = WDT(sel).b1a;
            } else if (dir & 0x8000) {
                wp->field_0x20 = WDT(sel).b1b;
            } else if (btn & 0x100) {
                card = cardGetFilePtr();
                flag = *(u16*)((u8*)card + (wp->field_0x20 << 14) + 0x2000);
                switch (wp->field_0x20) {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                        if (flag & 2) {
                            psndSFXOn((const char*)0x13);
                        } else if (flag & 1) {
                            wp->field_0x18 = 0x1e;
                            psndSFXOn((const char*)0x12);
                        } else {
                            wp->field_0x18 = 0x14;
                            psndSFXOn((const char*)0x12);
                        }
                        break;
                    case 4:
                        if (!(wp->field_0x0 & 0x10)) {
                            i = 0;
                            off = 0;
                            while (i < 4) {
                                card = cardGetFilePtr();
                                flag = *(u16*)((u8*)card + off + 0x2000);
                                if (!(flag & 1) || (flag & 2)) {
                                    break;
                                }
                                i++;
                                off += 0x4000;
                            }
                            if (i < 4) {
                                psndSFXOn((const char*)0x12);
                                wp->field_0x20 = 0;
                                wp->field_0x18 = 0x28;
                            } else {
                                psndSFXOn((const char*)0x13);
                            }
                        }
                        break;
                    case 5:
                        if (!(wp->field_0x0 & 0x20)) {
                            i = 0;
                            off = 0;
                            while (i < 4) {
                                card = cardGetFilePtr();
                                flag = *(u16*)((u8*)card + off + 0x2000);
                                if ((flag & 2) || (flag & 1)) {
                                    i++;
                                    off += 0x4000;
                                } else {
                                    break;
                                }
                            }
                            if (i < 4) {
                                psndSFXOn((const char*)0x12);
                                wp->field_0x20 = 0;
                                wp->field_0x18 = 0x32;
                            } else {
                                psndSFXOn((const char*)0x13);
                            }
                        }
                        break;
                    case 6:
                        psndSFXOn((const char*)0x13);
                        wp->field_0x18 = 0xa;
                        break;
                    case 7:
                        psndSFXOn((const char*)0x12);
                        psndBGMOff(0x800);
                        wp->field_0x18 = 0x64;
                        break;
                    case 8:
                        psndSFXOn((const char*)0x12);
                        wp->field_0x18 = 0xc8;
                        break;
                }
            } else if (btn & 0x200) {
                psndSFXOn((const char*)0x13);
                wp->field_0x18 = 0xa;
            }
            if (wp->field_0x20 != sel) {
                if (wp->field_0x20 < 4) {
                    psndSFXOn((const char*)9);
                } else {
                    psndSFXOn((const char*)5);
                }
            }
            wd = WDT_TABLE;
            wp->field_0x10 = wd[wp->field_0x20].x + wd[wp->field_0x20].z;
            wp->field_0x14 = wd[wp->field_0x20].y + wd[wp->field_0x20].w;
            break;

        case 0xa:
            fadeEntry(0x1b, 0, *(GXColor*)&dat_8041bd6c);
            wp->field_0x18++;
            break;

        case 0xb:
            if (fadeIsFinish()) {
                seqSetSeq(SEQ_TITLE, NULL, NULL);
            }
            break;

        case 0x14:
            wp->field_0x0 |= 0x40;
            wp->field_0x3C = evtEntryType(evt_memcard_start, 0, 0, 0);
            wp->field_0x3C->lwData[0] = wp->field_0x20;
            wp->field_0x18++;
            break;

        case 0x15:
            if (!evtCheckID(wp->field_0x3C->eventId)) {
                wp->field_0x0 &= ~0x40;
                if (evtGetValue(NULL, 0xFD050F80) == 1) {
                    wp->field_0x18++;
                } else {
                    wp->field_0x18 = 1;
                }
            }
            break;

        case 0x16:
            fadeEntry(0x1b, 0, *(GXColor*)&dat_8041bd70);
            psndBGMOff_f_d(0x200, 0x7d0, 0);
            wp->field_0x18++;
            break;

        case 0x17:
            if (fadeIsFinish()) {
                wp->field_0x0 |= 0x40;
                wp->field_0x3C = evtEntryType(evt_continue, 0, 0, 0);
                wp->field_0x18++;
            }
            break;

        case 0x18:
            if (!evtCheckID(wp->field_0x3C->eventId)) {
                wp->field_0x0 &= ~0x40;
                continueGame();
            }
            break;

        case 0x1e:
            fadeEntry(0x1b, 0, *(GXColor*)&dat_8041bd74);
            wp->field_0x18++;
            break;

        case 0x1f:
            if (fadeIsFinish()) {
                wp->field_0x18++;
            }
            break;

        case 0x20:
            if (nameEntPrepare()) {
                nameEntOn(0);
                fadeEntry(0x1c, 0, *(GXColor*)&dat_8041bd78);
                wp->field_0x18++;
            }
            break;

        case 0x21:
            if (fadeIsFinish() && !nameEntWait()) {
                fadeEntry(0x18, 0, *(GXColor*)&dat_8041bd7c);
                if (!nameEntIsCancel()) {
                    psndBGMOff_f_d(0x200, 0xbb8, 0);
                }
                wp->field_0x18++;
            }
            break;

        case 0x22:
            if (fadeIsFinish()) {
                nameEntOff();
                if (nameEntIsCancel()) {
                    fadeEntry(0x1c, 0, *(GXColor*)&dat_8041bd80);
                    wp->field_0x18 = 1;
                } else {
                    strcpy((char*)gp->field_0x11BC, nameEntGetName());
                    gp->unk11B8 = wp->field_0x20;
                    stg0_00_init();
                    *(s32*)(gp->field_0x11BC + 0x14) = wp->field_0x20;
                    marioPartyKill();
                    mario = marioGetPtr();
                    mario->prevPartyMemberId[0] = 0;
                    mario->prevPartyMemberId[1] = 0;
                    gp->areaFadeInType = 0;
                    gp->areaFadeOutType = 0x19;
                    swInit();
                    swByteSet(0, 0);
                    if (code == 0) {
                        *(s32*)(gp->field_0x11BC + 0x14) = wp->field_0x20;
                        wp->field_0x3C = evtEntryType(evt_memcard_save, 0, 0, 0);
                        wp->field_0x3C->lwData[0] = 0x17;
                        wp->field_0x18++;
                    } else {
                        seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x2c0, str_802ED930), RO(base, 0x2c8, str_802ED938));
                    }
                }
            }
            break;

        case 0x23:
            if (!evtCheckID(wp->field_0x3C->eventId)) {
                seqSetSeq(SEQ_MAP_CHANGE, RO(base, 0x2c0, str_802ED930), RO(base, 0x2c8, str_802ED938));
            }
            break;

        case 0x28:
            wp->field_0x38 = (s32)msgSearch(RO(base, 0x374, str_802ED9E4));
            sel = wp->field_0x20;

            if (dir & 0x1000) {
                wp->field_0x20 = WDT2(sel).b18;
            } else if (dir & 0x2000) {
                wp->field_0x20 = WDT2(sel).b19;
            } else if (dir & 0x4000) {
                wp->field_0x20 = WDT2(sel).b1a;
            } else if (dir & 0x8000) {
                wp->field_0x20 = WDT2(sel).b1b;
            } else if (btn & 0x100) {
                if (sel < 4) {
                    card = cardGetFilePtr();
                    flag = *(u16*)((u8*)card + (wp->field_0x20 << 14) + 0x2000);
                    if ((flag & 2) || !(flag & 1)) {
                        psndSFXOn((const char*)0x12);
                        wp->field_0x18++;
                    } else {
                        psndSFXOn((const char*)0x13);
                    }
                } else {
                    wp->field_0x20 = 4;
                    wp->field_0x18 = 1;
                    psndSFXOn((const char*)0x13);
                }
            } else if (btn & 0x200) {
                wp->field_0x20 = 4;
                wp->field_0x18 = 1;
                psndSFXOn((const char*)0x13);
            }
            if (wp->field_0x20 != sel) {
                if (wp->field_0x20 < 4) {
                    psndSFXOn((const char*)9);
                }
            }
            wd = WDT2_TABLE;
            wp->field_0x10 = wd[wp->field_0x20].x + wd[wp->field_0x20].z;
            wp->field_0x14 = wd[wp->field_0x20].y + wd[wp->field_0x20].w;
            break;

        case 0x29:
            wp->field_0x0 |= 0x40;
            wp->field_0x3C = evtEntryType(evt_memcard_delete, 0, 0, 0);
            wp->field_0x3C->lwData[0] = wp->field_0x20;
            wp->field_0x18++;
            break;

        case 0x2a:
            if (!evtCheckID(wp->field_0x3C->eventId)) {
                if (evtGetValue(NULL, 0xFAA2B580) == 0) {
                    wp->field_0x0 &= ~0x40;
                    wp->field_0x20 = 4;
                    wp->field_0x18 = 1;
                } else {
                    wp->field_0x0 |= 0x80;
                    workArg->state = 1;
                    return;
                }
            }
            break;

        case 0x32:
            wp->field_0x38 = (s32)msgSearch(RO(base, 0x390, str_802EDA00));
            sel = wp->field_0x20;

            if (dir & 0x1000) {
                wp->field_0x20 = WDT2(sel).b18;
            } else if (dir & 0x2000) {
                wp->field_0x20 = WDT2(sel).b19;
            } else if (dir & 0x4000) {
                wp->field_0x20 = WDT2(sel).b1a;
            } else if (dir & 0x8000) {
                wp->field_0x20 = WDT2(sel).b1b;
            } else if (btn & 0x100) {
                if (sel < 4) {
                    card = cardGetFilePtr();
                    flag = *(u16*)((u8*)card + (wp->field_0x20 << 14) + 0x2000);
                    if (!(flag & 2) && !(flag & 1)) {
                        wp->field_0x18 = 0x33;
                        wp->field_0x30 = wp->field_0x20;
                        wp->field_0x34 = (wp->field_0x30 + 1) % 4;
                        wp->field_0x20 = wp->field_0x34;
                        psndSFXOn((const char*)0x12);
                    }
                } else {
                    wp->field_0x20 = 5;
                    wp->field_0x18 = 1;
                    psndSFXOn((const char*)0x13);
                }
            } else if (btn & 0x200) {
                wp->field_0x20 = 5;
                wp->field_0x18 = 1;
                psndSFXOn((const char*)0x13);
            }
            if (wp->field_0x20 != sel) {
                if (wp->field_0x20 < 4) {
                    psndSFXOn((const char*)9);
                }
            }
            wd = WDT2_TABLE;
            wp->field_0x10 = wd[wp->field_0x20].x + wd[wp->field_0x20].z;
            wp->field_0x14 = wd[wp->field_0x20].y + wd[wp->field_0x20].w;
            break;

        case 0x33:
            wp->field_0x38 = (s32)msgSearch(RO(base, 0x3ac, str_802EDA1C));
            sel = wp->field_0x20;

            if (dir & 0x1000) {
                do {
                    { const u8* tbl = (const u8*)WDT3_TABLE; wp->field_0x20 = tbl[wp->field_0x20 * 0x1c + 0x18]; }
                } while (wp->field_0x20 == wp->field_0x30);
            } else if (dir & 0x2000) {
                do {
                    { const u8* tbl = (const u8*)WDT3_TABLE; wp->field_0x20 = tbl[wp->field_0x20 * 0x1c + 0x19]; }
                } while (wp->field_0x20 == wp->field_0x30);
            } else if (dir & 0x4000) {
                do {
                    { const u8* tbl = (const u8*)WDT3_TABLE; wp->field_0x20 = tbl[wp->field_0x20 * 0x1c + 0x1a]; }
                } while (wp->field_0x20 == wp->field_0x30);
            } else if (dir & 0x8000) {
                do {
                    { const u8* tbl = (const u8*)WDT3_TABLE; wp->field_0x20 = tbl[wp->field_0x20 * 0x1c + 0x1b]; }
                } while (wp->field_0x20 == wp->field_0x30);
            } else if (btn & 0x100) {
                wp->field_0x18 = 0x34;
                wp->field_0x34 = wp->field_0x20;
                psndSFXOn((const char*)0x12);
            } else if (btn & 0x200) {
                wp->field_0x20 = wp->field_0x30;
                wp->field_0x18 = 0x32;
                psndSFXOn((const char*)0x13);
            }
            if (wp->field_0x20 != sel) {
                if (wp->field_0x20 < 4) {
                    psndSFXOn((const char*)9);
                }
            }
            wp->field_0x34 = wp->field_0x20;
            wd = WDT2_TABLE;
            wp->field_0x10 = wd[wp->field_0x20].x + wd[wp->field_0x20].z;
            wp->field_0x14 = wd[wp->field_0x20].y + wd[wp->field_0x20].w;
            break;

        case 0x34:
            wp->field_0x0 |= 0x40;
            wp->field_0x3C = evtEntryType(evt_memcard_copy, 0, 0, 0);
            wp->field_0x3C->lwData[0] = wp->field_0x30;
            wp->field_0x3C->lwData[1] = wp->field_0x34;
            wp->field_0x18++;
            break;

        case 0x35:
            if (!evtCheckID(wp->field_0x3C->eventId)) {
                if (evtGetValue(NULL, 0xFAA2B580) == 0) {
                    wp->field_0x0 &= ~0x40;
                    wp->field_0x20 = 5;
                    wp->field_0x18 = 1;
                } else {
                    wp->field_0x0 |= 0x80;
                    workArg->state = 1;
                    return;
                }
            }
            break;

        case 0x64:
            wp->field_0x0 |= 1;
            sel = wp->field_0x24;
            if (dir & 0x1000) {
                wp->field_0x24 = sel - 1;
                psndSFXOn((const char*)5);
            } else if (dir & 0x2000) {
                wp->field_0x24 = sel + 1;
                psndSFXOn((const char*)5);
            } else if (btn & 0x100) {
                switch (sel) {
                    case 0:
                        OSSetSoundMode(0);
                        gp->unk1274 = 0;
                        break;
                    case 1:
                        OSSetSoundMode(1);
                        gp->unk1274 = 0;
                        break;
                    case 2:
                        OSSetSoundMode(1);
                        gp->unk1274 = 1;
                        break;
                }
                if (OSGetSoundMode() == 0) {
                    SoundSetOutputMode(0);
                } else {
                    SoundSetOutputMode(1);
                }
                psndSFXOn((const char*)0x12);
                psndBGMOn(0xa0, NULL);
                psndENVOff(0x200);
                wp->field_0x40 = wp->field_0x24;
                wp->field_0x0 &= ~1;
                wp->field_0x18 = 1;
                break;
            } else if (btn & 0x200) {
                psndSFXOn((const char*)0x13);
                psndBGMOn(0xa0, NULL);
                psndENVOff(0x200);
                wp->field_0x24 = wp->field_0x40;
                if (OSGetSoundMode() == 0) {
                    SoundSetOutputMode(0);
                } else {
                    SoundSetOutputMode(1);
                }
                wp->field_0x0 &= ~1;
                wp->field_0x18 = 1;
                break;
            }
            if (wp->field_0x24 >= 3) {
                wp->field_0x24 = 0;
            }
            if (wp->field_0x24 < 0) {
                wp->field_0x24 = 2;
            }
            if (wp->field_0x24 != sel) {
                switch (wp->field_0x24) {
                    case 0:
                        SoundSetOutputMode(0);
                        break;
                    case 1:
                        SoundSetOutputMode(1);
                        break;
                    case 2:
                        SoundSetOutputMode(2);
                        break;
                }
                psndENVOn(0x110, 0x75);
            }
            wp->field_0x10 = float_neg74_8041be74;
            wp->field_0x14 = float_4_8041be78 + (float_50_8041be6c - float_50_8041be6c * (f32)wp->field_0x24);
            break;

        case 0xc8:
            wp->field_0x0 |= 1;
            if (dir & 0x1000) {
                wp->field_0x28--;
                psndSFXOn((const char*)5);
            } else if (dir & 0x2000) {
                wp->field_0x28++;
                psndSFXOn((const char*)5);
            } else if (btn & 0x100) {
                gp->unk1294 = (wp->field_0x28 == 0);
                psndSFXOn((const char*)0x12);
                wp->field_0x44 = wp->field_0x28;
                wp->field_0x0 &= ~1;
                wp->field_0x18 = 1;
                if (gp->unk1294) {
                    evtEntryType(evt_shake, 0, 0, 0);
                }
            } else if (btn & 0x200) {
                psndSFXOn((const char*)0x13);
                wp->field_0x28 = wp->field_0x44;
                wp->field_0x0 &= ~1;
                wp->field_0x18 = 1;
            }
            if (wp->field_0x28 >= 2) {
                wp->field_0x28 = 0;
            }
            if (wp->field_0x28 < 0) {
                wp->field_0x28 = 1;
            }
            wp->field_0x10 = float_neg74_8041be74;
            wp->field_0x14 = float_4_8041be78 + (float_30_8041be3c - float_50_8041be6c * (f32)wp->field_0x28);
            break;
    }

    wp->field_0x8 += (wp->field_0x10 - wp->field_0x8) * float_0p25_8041be7c;
    wp->field_0xC += (wp->field_0x14 - wp->field_0xC) * float_0p25_8041be7c;
    wp->field_0x8 += fabsf(float_2_8041be70 * (f32)sin((f32)gp->retraceCount * float_0p125_8041be80));
    dispEntry(4, float_1000_8041be84, 0, (DispCallback)loadDraw, NULL);
}
#pragma opt_propagation reset

// NONMATCHING (76.47%): complete reconstruction of the save-file load screen. Every call, loop bound,
// rodata object, table read and branch is present and semantically correct. The remaining diffs are
// compiler only: MWCC's stack-frame live-range packing (the frame size matches at 0x890, but the
// name/time buffers and the viewport/projection pair land in different slots), the register colouring
// that follows, and the target's single base-pointer addressing of the .rodata vecs/strings/doubles
// (this file names those objects individually, so MWCC emits lis+addi per use; routing them through
// the RO macro re-colours every register and scores lower). The angle table is copied on the stack
// through a struct assignment (the target does a 16-word block copy before indexing); the name width
// is halved with `>> 1`, which is what makes MWCC fold the shift and mask into the target's `extrwi`;
// the texture-strip scale uses the double __fabs idiom shared with win_root.c's winNameQuad; the
// play-time divide is s64 on the target but emitted as u64 here (one __div2i vs __div2u). Raw offsets
// used for undeclared CardSaveSlot fields are listed in REPORT.md.
// A textured strip quad, the same form as win_root.c's winNameQuad but sourced from the global
// LoadWork. Kept static inline so the three constant-folded calls match the target's double __fabs
// scale idiom and per-call palette reload.
static inline void loadDrawQuad(s32 texId, f32 x, f32 y, f32 w, f32 h) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sy;
    f64 sx;
    f32 syf;

    TEXGetGXTexObjFromPalette(**(TEXPalette***)((u8*)wp->file + 0xA0), &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, float_0_8041bde4, float_0_8041bde4, float_0_8041bde4, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = __fabs(h);
    sx = __fabs(w);
    syf = (f32)sy / (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx, (f32)sx / (f32)GXGetTexObjWidth(&texObj), syf, float_1_8041bde8);
    GXLoadTexMtxImm(mtx, 0x1E, 1);
    GXBegin(0x80, 0, 4);
    GXPosition3f32(x, y, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(x, y - h, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x + w, y - h, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x + w, y, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
}

// Build a by-value Vec from a named base and two coordinates; mirrors win_root.c's winBadgeMkVec so
// each call site gets its own stack temp instead of reusing one local.
static inline Vec loadDrawVec(const Vec* bp, f32 x, f32 y) {
    Vec base = *bp;
    base.x = x;
    base.y = y;
    return base;
}

// Same, for the calls that only move y and keep the base's x/z.
static inline Vec loadDrawVecY(const Vec* bp, f32 y) {
    Vec base = *bp;
    base.y = y;
    return base;
}

// Retail passes these colours by value (MWCC copies the GXColor to a stack temp and passes its
// address to FontDrawColor); the header declares a pointer, so a plain FontDrawColor(&global) does
// not copy. Taking the colour by value forces the copy in both builds.
static inline void loadDrawColor(GXColor color) {
    FontDrawColor(&color);
}

// NONMATCHING (76.47%): see the full note above the helpers; frame live-range packing and one
// s64 divide differ; all calls, loops and rodata are present.
void loadDraw(void) {
    CameraEntry* cam = camGetPtr(CAMERA_2D);
    GXColor white = *(GXColor*)&dat_8041bd84;
    GXColor greyA0 = *(GXColor*)&dat_8041bd88;
    GXColor grey80 = *(GXColor*)&dat_8041bd8c;
    WinDt* wd;
    CardData* card;
    u8* slot;
    char timeBuf[0x70];
    char nameBuf[0x70];
    s32 i;
    s32 j;
    f32 viewport[6];
    f32 projection[7];
    CameraEntry saved;

    if (wp->field_0x18 >= 0x20 && wp->field_0x18 < 0x28) {
        return;
    }

    GXGetViewportv(viewport);
    GXGetProjectionv(projection);
    GXSetProjection(cam->projection, cam->projType);
    saved = *camGetCurPtr();
    *camGetCurPtr() = *cam;

    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(0, 7, 0);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, *(GXColor*)&dat_8041bd90);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(13, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 13, 1, 4, 0);
    GXLoadPosMtxImm(cam->view, 0);
    GXSetCurrentMtx(0);

    loadDrawQuad(0x35, -304.0f, 240.0f, 32.0f, 480.0f);
    loadDrawQuad(0x34, -272.0f, 240.0f, 544.0f, 480.0f);
    loadDrawQuad(0x36, 272.0f, 240.0f, 32.0f, 480.0f);

    // The nine file windows.
    for (i = 0; i < 9; i++) {
        GXColor color = (i == wp->field_0x20) ? white : greyA0;

        wd = &((WinDt*)win_dt)[i];
        if (wp->field_0x18 == 0x33 && (i == wp->field_0x30 || i == wp->field_0x34)) {
            color = white;
        }
        if (i == 4 && (wp->field_0x0 & 0x20)) {
            color = grey80;
        }
        if (i == 5 && (wp->field_0x0 & 0x40)) {
            color = grey80;
        }
        if (wd->unk10 < 0xe && wd->unk10 >= 0xa) {
            color.a = 0xFE;
            winTexInit(**(TEXPalette***)((u8*)wp->file + 0xA0));
            winTexSet(0xe, loadDrawVec(&vec3_802ed670, wd->x, wd->y), vec3_802ed67c, color);
            winTexInit(**(TEXPalette***)((u8*)wp->file + 0xA0));
            winTexSet(wd->unk10, loadDrawVec(&vec3_802ed688, wd->x, wd->y + 60.0f), vec3_802ed694, color);
        } else {
            winTexInit_x2(**(TEXPalette***)((u8*)wp->file + 0xA0));
            winTexSet_x2(wd->unk10, wd->unk14, loadDrawVec(&vec3_802ed6a0, wd->x, wd->y), vec3_802ed6ac, color);
        }
    }

    windowDispGX_Waku_col(0, *(GXColor*)&dat_8041bd94, -250.0f, 200.0f, 500.0f, 40.0f, 20.0f);
    FontDrawStart();
    {
        const char* msg = (const char*)wp->field_0x38;

        FontDrawMessage(-((s32)FontGetMessageWidth(msg)) / 2, 0xc0, msg);
    }

    // The four save files.
    for (i = 0; i < 4; i++) {
        u16 flags;
        u16 mask;
        s32 level;
        s32 field28;
        u32 field2c;
        s32 field17c;
        const char* name;

        card = cardGetFilePtr();
        slot = (u8*)card + i * 0x4000 + 0x2000;
        flags = *(u16*)(slot + 0x0);
        level = *(s16*)(slot + 0x146a);
        field28 = *(u32*)(slot + 0x28);
        field2c = *(u32*)(slot + 0x2c);
        mask = *(u16*)(slot + 0x146c);
        field17c = *(u32*)(slot + 0x17c);
        name = (const char*)(slot + 0x11c4);
        wd = &((WinDt*)win_dt)[i];

        FontDrawStart();
        loadDrawColor(*(GXColor*)&dat_8041bd98);
        if (i == wp->field_0x20) {
            loadDrawColor(*(GXColor*)&dat_8041bd9c);
        }
        FontDrawScale(0.6f);
        FontDrawEdge();
        sprintf(nameBuf, str_802ED998, winZenkakuStr(i + 1));
        FontDrawString(nameBuf, wd->x - 100.0f, 84.0f + wd->y);
        FontDrawEdgeOff();
        FontDrawScale(1.0f);
        loadDrawColor(*(GXColor*)&dat_8041bda0);
        if (i == wp->field_0x20) {
            loadDrawColor(*(GXColor*)&dat_8041bda4);
        }

        if (flags & 2) {
            const char* s = str_802ED96C;

            FontDrawString(s, wd->x - (f32)FontGetMessageWidth(s) * 0.5f, 12.0f + wd->y);
        } else if (flags & 1) {
            const char* s = str_802ED97C;

            FontDrawString(s, wd->x - (f32)FontGetMessageWidth(s) * 0.5f, 12.0f + wd->y);
        } else if (field17c == 0) {
            const char* s = str_802ED98C;

            FontDrawString(s, wd->x - (f32)FontGetMessageWidth(s) * 0.5f, 12.0f + wd->y);
            if (strlen(name) != 0) {
                FontDrawString(name, wd->x - (f32)(FontGetMessageWidth(name) >> 1), 70.0f + wd->y);
            }
        } else {
            u32 divisor = *(volatile u32*)0x800000F8 >> 2;
            u64 ticks = ((u64)field2c << 32) | field28;
            u64 secs;
            u32 hours;
            u32 mins;
            f32 half;
            f32 half2;

            FontGetMessageWidth(str_8041BE34);
            secs = ticks / divisor;
            if (secs > 0x57E3F) {
                hours = 99;
                mins = 59;
            } else {
                hours = (u32)(secs / 3600);
                mins = (u32)((secs % 3600) / 60);
            }

            if (strlen(name) != 0) {
                FontDrawString(name, wd->x - (f32)(FontGetMessageWidth(name) >> 1), 70.0f + wd->y);
            }
            sprintf(timeBuf, str_802ED9A4, winZenkakuStr(level));
            FontDrawString(timeBuf, wd->x - (f32)FontGetMessageWidth(timeBuf) * 0.5f, 30.0f + wd->y);
            strcpy(timeBuf, str_802ED9A4);
            strcat(timeBuf, unk_8016af5c(hours, 2));
            strcat(timeBuf, str_8041BE40);
            strcat(timeBuf, unk_8016af5c(mins, 2));
            half = (f32)FontGetMessageWidth(timeBuf) * 0.5f;
            FontDrawString(timeBuf, wd->x - half, wd->y);
            strcpy(timeBuf, str_802ED9A4);
            strcat(timeBuf, unk_8016af5c(hours, 2));
            half2 = (f32)FontGetMessageWidth(timeBuf);
            {
                Mtx mtx;
                Mtx mtx2;
                GXColor c = *(GXColor*)&dat_8041bda8;

                PSMTXTrans(mtx, 8.0f + (half2 + (wd->x - half)), 1.0f + (wd->y - 22.0f), 0.0f);
                PSMTXScale(mtx2, 1.5f, 1.5f, 1.5f);
                PSMTXConcat(mtx, mtx2, mtx);
                iconDispGxCol(mtx, 0, 0x201, c);
            }
        }

        for (j = 0; j < 7; j++) {
            if (mask & (1 << (j + 1))) {
                GXColor c = (i == wp->field_0x20) ? white : greyA0;
                Mtx mtx;

                PSMTXTrans(mtx, wd->x - 106.0f + (f32)(j * 0x20), wd->y - 52.0f, 0.0f);
                iconDispGxCol(mtx, 0, stone_id[j] & 0xFFFF, c);
            }
        }
    }

    if (wp->field_0x0 & 1) {
        wp->field_0x2C += 0.05f;
    } else {
        wp->field_0x2C -= 0.05f;
    }
    if (wp->field_0x2C < 0.0f) {
        wp->field_0x2C = 0.0f;
    }
    if (wp->field_0x2C > 1.0f) {
        wp->field_0x2C = 1.0f;
    }
    nameMaskGX(wp->field_0x2C);

    if (wp->field_0x18 >= 0x64 && wp->field_0x18 < 0xc8) {
        winTexInit(**(TEXPalette***)((u8*)wp->file + 0xA0));
        winTexSet(0xf, vec3_802ed6b8, vec3_802ed6c4, *(GXColor*)&dat_8041bdac);
        winTexInit_x2(**(TEXPalette***)((u8*)wp->file + 0xA0));
        winTexSet_x2(0x13, 0x1f, vec3_802ed6d0, vec3_802ed6dc, *(GXColor*)&dat_8041bdb0);

        for (j = 0; j < 3; j++) {
            if (wp->field_0x24 == j) {
                winTexInit(**(TEXPalette***)((u8*)wp->file + 0xA0));
                winTexSet(0x33, loadDrawVecY(&vec3_802ed6e8, (f32)(0x32 - j * 0x32)), vec3_802ed6f4, *(GXColor*)&dat_8041bdb4);
                winTexInit_x2(**(TEXPalette***)((u8*)wp->file + 0xA0));
                winTexSet_x2(j + 0x15, 0x33, loadDrawVecY(&vec3_802ed700, (f32)(0x36 - j * 0x32)), vec3_802ed70c, white);
            } else {
                winTexInit_x2(**(TEXPalette***)((u8*)wp->file + 0xA0));
                winTexSet_x2(j + 0x15, 0x33, loadDrawVecY(&vec3_802ed718, (f32)(0x32 - j * 0x32)), vec3_802ed724, white);
            }
        }
    }

    if (wp->field_0x18 >= 0xc8 && wp->field_0x18 < 0x12c) {
        winTexInit(**(TEXPalette***)((u8*)wp->file + 0xA0));
        winTexSet(0xf, vec3_802ed730, vec3_802ed73c, *(GXColor*)&dat_8041bdc0);
        winTexInit_x2(**(TEXPalette***)((u8*)wp->file + 0xA0));
        winTexSet_x2(0x14, 0x1f, vec3_802ed748, vec3_802ed754, *(GXColor*)&dat_8041bdc4);

        for (j = 0; j < 2; j++) {
            if (wp->field_0x28 == j) {
                winTexInit(**(TEXPalette***)((u8*)wp->file + 0xA0));
                winTexSet(0x33, loadDrawVecY(&vec3_802ed760, (f32)(0x1e - j * 0x32)), vec3_802ed76c, *(GXColor*)&dat_8041bdc8);
                winTexInit_x2(**(TEXPalette***)((u8*)wp->file + 0xA0));
                winTexSet_x2(j + 0x18, 0x33, loadDrawVecY(&vec3_802ed778, (f32)(0x22 - j * 0x32)), vec3_802ed784, white);
            } else {
                winTexInit_x2(**(TEXPalette***)((u8*)wp->file + 0xA0));
                winTexSet_x2(j + 0x18, 0x33, loadDrawVecY(&vec3_802ed790, (f32)(0x1e - j * 0x32)), vec3_802ed79c, white);
            }
        }
    }

    if (wp->field_0x18 == 0x33 && wp->field_0x34 < 4) {
        struct LoadAngleTable angles = *(struct LoadAngleTable*)dat_802ed7a8;
        f32 angle = angles.v[wp->field_0x30 * 4 + wp->field_0x34];
        f32 f29;
        f32 f30;

        if (fmod(angle, 90.0) == 0.0f) {
            f32 fa = (f32)(wp->field_0x30 / 2);
            f32 fb = (f32)(wp->field_0x34 / 2);
            if (fa == fb) {
                f29 = 0.0f;
                f30 = 120.0f - 140.0f * fa;
            } else {
                f29 = 300.0f * (f32)(wp->field_0x30 % 2) - 150.0f;
                f30 = 50.0f;
            }
        } else {
            f29 = 0.0f;
            f30 = 50.0f;
        }
        f30 -= 40.0f;

        {
            Mtx mtx;

            winTexInit(**(TEXPalette***)((u8*)wp->file + 0xA0));
            winTexSetRot(0x1d, loadDrawVec(&vec3_802ed7e8, 2.0f + f29, f30 - 2.0f), vec3_802ed7f4, angle, *(GXColor*)&dat_8041bdd4);
            winTexSetRot(0x1d, loadDrawVec(&vec3_802ed800, f29, f30), vec3_802ed80c, angle, white);
            (void)mtx;
        }
    }

    if (!(wp->field_0x0 & 0x40)) {
        winTexInit(**(TEXPalette***)((u8*)wp->file + 0xA0));
        winTexSet(0, loadDrawVec(&vec3_802ed818, wp->field_0x8 + 2.0f, wp->field_0xC - 2.0f), vec3_802ed824, *(GXColor*)&dat_8041bdd8);
        winTexSet(0, loadDrawVec(&vec3_802ed830, wp->field_0x8, wp->field_0xC), vec3_802ed83c, white);
    }

    GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
    GXSetProjectionv(projection);
    *camGetCurPtr() = saved;
}
