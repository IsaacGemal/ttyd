#include "sequence/seq_load.h"
#include "countdown.h"
#include "driver/fadedrv.h"
#include "driver/seqdrv.h"
#include "driver/swdrv.h"
#include "event.h"
#include "manager/cardmgr.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/filemgr.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include <string.h>

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
    u8 field_0x2C[0x3C - 0x2C];
    EventEntry* field_0x3C; //0x3C
    s32 field_0x40; //0x40
    s32 field_0x44; //0x44
    s32 field_0x48; //0x48
} LoadWork;

static LoadWork work;
static LoadWork* wp = &work;

extern s32 evt_memcard_check[];
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

const GXColor dat_8041bd68 = {0, 0, 0, 0xFF};

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
