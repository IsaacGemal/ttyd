#include "sequence/seq_load.h"
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
    u8 field_0x2C[0x30 - 0x2C];
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

#define WDT(i)  (((WinDt*)(base2 + 0x40))[i])
#define WDT2(i) (((WinDt*)(base2 + 0x13c))[i])
#define WDT3(i) (((WinDt*)(base2 + 0x200))[i])

static LoadWork work;
static LoadWork* wp = &work;

extern s32 evt_memcard_check[];
extern s32 evt_memcard_start[];
extern s32 evt_memcard_delete[];
extern s32 evt_memcard_copy[];
extern s32 evt_memcard_save[];
extern f32 lbl_80349DE8[];
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

static s32 evt_shake[] = {
    0x0003005B, (s32)evt_sub_rumble_onoff, 0x00000000, 0x00000000,
    0x0001000A, 0x00000096,
    0x0003005B, (s32)evt_sub_rumble_onoff, 0x00000001, 0x00000000,
    0x00000002, 0x00000001,
};

s32 evt_continue[] = {
    0x0002005B, (s32)evt_sub_animgroup_async, (s32)(void*)str_802ED848,
    0x0002005B, (s32)evt_sub_animgroup_async, (s32)(void*)str_802ED85C,
    0x0002005B, (s32)evt_sub_animgroup_async, (s32)(void*)str_802ED870,
    0x00000002, 0x00000001,
};

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

// NONMATCHING (96%): full reconstruction; remaining diffs are base+offset materialisation for
// win_dt/win_dt2/win_dt3 accesses (compiler folds 0x40/0x13c/0x200 into the load displacement) and
// the s32->f32 magic double (anonymous @NNN instead of double_to_int_802ed9c8).
void loadMain(SequenceWork* workArg) {
    const char* base2 = (const char*)&lbl_80349DE8;
    const char* base = (const char*)&vec3_802ed670;
    u32 btn = keyGetButtonTrg(0);
    u32 rep = keyGetButtonRep(0);
    u32 dir = keyGetDirRep(0);
    s32 code = cardGetCode();
    s32 sel;
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
                wd = (WinDt*)(base2 + 0x40);
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
            wd = (WinDt*)(base2 + 0x40);
            wp->field_0x10 = wd[wp->field_0x20].x + wd[wp->field_0x20].z;
            wp->field_0x14 = wd[wp->field_0x20].y + wd[wp->field_0x20].w;
            break;

        case 0xa:
            color = *(GXColor*)&dat_8041bd6c;
            fadeEntry(0x1b, 0, color);
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
            color = *(GXColor*)&dat_8041bd70;
            fadeEntry(0x1b, 0, color);
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
            color = *(GXColor*)&dat_8041bd74;
            fadeEntry(0x1b, 0, color);
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
                color = *(GXColor*)&dat_8041bd78;
                fadeEntry(0x1c, 0, color);
                wp->field_0x18++;
            }
            break;

        case 0x21:
            if (fadeIsFinish() && !nameEntWait()) {
                color = *(GXColor*)&dat_8041bd7c;
                fadeEntry(0x18, 0, color);
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
                    color = *(GXColor*)&dat_8041bd80;
                    fadeEntry(0x1c, 0, color);
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
            wp->field_0x10 = WDT2(wp->field_0x20).x + WDT2(wp->field_0x20).z;
            wp->field_0x14 = WDT2(wp->field_0x20).y + WDT2(wp->field_0x20).w;
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
                wp->field_0x20 = WDT3(sel).b18;
            } else if (dir & 0x2000) {
                wp->field_0x20 = WDT3(sel).b19;
            } else if (dir & 0x4000) {
                wp->field_0x20 = WDT3(sel).b1a;
            } else if (dir & 0x8000) {
                wp->field_0x20 = WDT3(sel).b1b;
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
            wp->field_0x10 = WDT3(wp->field_0x20).x + WDT3(wp->field_0x20).z;
            wp->field_0x14 = WDT3(wp->field_0x20).y + WDT3(wp->field_0x20).w;
            break;

        case 0x33:
            wp->field_0x38 = (s32)msgSearch(RO(base, 0x3ac, str_802EDA1C));
            sel = wp->field_0x20;

            if (dir & 0x1000) {
                do {
                    wp->field_0x20 = WDT3(wp->field_0x20).b18;
                } while (wp->field_0x20 == wp->field_0x30);
            } else if (dir & 0x2000) {
                do {
                    wp->field_0x20 = WDT3(wp->field_0x20).b19;
                } while (wp->field_0x20 == wp->field_0x30);
            } else if (dir & 0x4000) {
                do {
                    wp->field_0x20 = WDT3(wp->field_0x20).b1a;
                } while (wp->field_0x20 == wp->field_0x30);
            } else if (dir & 0x8000) {
                do {
                    wp->field_0x20 = WDT3(wp->field_0x20).b1b;
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
            wp->field_0x10 = WDT3(wp->field_0x20).x + WDT3(wp->field_0x20).z;
            wp->field_0x14 = WDT3(wp->field_0x20).y + WDT3(wp->field_0x20).w;
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
