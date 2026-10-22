// sound.h -> musyx.h includes the SDK <math.h>, whose `extern inline` sqrtf/sqrt emit four 0.5/3.0
// localstatics into .rodata that the original unit does not have (shifting every string by 0x20).
// Suppress it and take the out-of-line MSL math ABI from math_ext.h instead.
#define _MATH_H_
#include "math_ext.h"
#include "sequence/seq_load.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "driver/windowdrv.h"
#include "manager/fontmgr.h"
void FontDrawColor(GXColor color);
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

// The original reaches its .rodata and .data through the compiler's pool bases (r30 = .rodata+0,
// loadMain's r29 = .data+0, which reaches win_dt/win_dt2/win_dt3 at +0x40/+0x13c/+0x200).
#pragma pool_data on

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") f32 lbl_80349DE8[2] = { -1.0f, 1.0f };

// File-select state. Bits of `flags`: 0x01 option menu open (optionsFade ramps toward 1 while set);
// 0x10 all four slots empty (delete row disabled); 0x20 no slot holds a usable save (copy row disabled);
// 0x40 a memcard event is running (menuEvt); 0x80 a delete/copy was carried out (event result 1), so state 1
// reruns the card check.
// `step` is the menu state; `cursor` is an index into win_dt: 0-3 save slots, 4 delete, 5 copy, 6 title,
// 7 sound option, 8 rumble option. The sound and rumble menus keep a cursor and the committed setting:
// soundCursor 0 mono, 1 stereo, 2 surround; rumbleCursor 0 on, 1 off.
// prevCardCode is last frame's cardGetCode(); button input is ignored when it changes (card inserted or removed).
// copySrc/copyDst are the copy event's source and destination (lwData[0], [1]). cursorX/Y ease toward
// cursorTargetX/Y. printWindowId is the window id copied into the card-check event (seqLoadPrepCardRecheck sets it).
typedef struct LoadWork {
    u16 flags; //0x0
    u8 pad_0x2[0x4 - 0x2];
    FileEntry* file; //0x4
    f32 cursorX; //0x8
    f32 cursorY; //0xC
    f32 cursorTargetX; //0x10
    f32 cursorTargetY; //0x14
    s32 step; //0x18
    s32 prevCardCode; //0x1C
    s32 cursor; //0x20
    s32 soundCursor; //0x24
    s32 rumbleCursor; //0x28
    f32 optionsFade; //0x2C
    s32 copySrc; //0x30
    s32 copyDst; //0x34
    const char* message; //0x38
    EventEntry* menuEvt; //0x3C
    s32 soundSaved; //0x40
    s32 rumbleSaved; //0x44
    s32 printWindowId; //0x48
} LoadWork;

// One entry per box. texId is the picture (10-13 the save boxes, 16-20 the buttons); texId2 the second
// picture on the button rows (30/31). navUp/Down/Left/Right are the next box for the 0x1000/0x2000/0x4000/
// 0x8000 direction bits (slot 2 is below slot 0, so 0x2000 is down). The cursor sits at (x + cursorOffX, y + cursorOffY).
typedef struct WinDt {
    f32 x; //0x0
    f32 y; //0x4
    f32 cursorOffX; //0x8
    f32 cursorOffY; //0xC
    s32 texId; //0x10
    s32 texId2; //0x14
    u8 navUp; //0x18
    u8 navDown; //0x19
    u8 navLeft; //0x1A
    u8 navRight; //0x1B
} WinDt;

#define LOAD_TEX_PALETTE() ((TEXPalette*)*wp->file->data)

static LoadWork work;
static LoadWork* wp = &work;

extern s32 evt_memcard_check[];
extern s32 evt_memcard_start[];
extern s32 evt_memcard_delete[];
extern s32 evt_memcard_copy[];
extern s32 evt_memcard_save[];
u32 gap_80349DF0[14] = {
    0xBF800000, 0x3F800000, 0x3F800000, 0xBF800000, 0x3F800000, 0xBF800000, 0xBF800000, 0x3F800000,
    0xBF800000, 0x3F800000, 0x3F800000, 0xBF800000, 0x3F800000, 0xBF800000,
};
WinDt win_dt[9] = {
    { -140.0f, 75.0f, -120.0f, 60.0f, 13, 0, 7, 2, 1, 1 },
    { 140.0f, 75.0f, -120.0f, 60.0f, 12, 0, 8, 3, 0, 0 },
    { -140.0f, -65.0f, -120.0f, 60.0f, 11, 0, 0, 4, 3, 3 },
    { 140.0f, -65.0f, -120.0f, 60.0f, 10, 0, 1, 5, 2, 2 },
    { -200.0f, -150.0f, -80.0f, 0.0f, 16, 30, 2, 7, 6, 5 },
    { 0.0f, -150.0f, -80.0f, 0.0f, 17, 30, 2, 7, 4, 6 },
    { 200.0f, -150.0f, -80.0f, 0.0f, 18, 30, 3, 8, 5, 4 },
    { -130.0f, -190.0f, -100.0f, 0.0f, 19, 31, 4, 0, 8, 8 },
    { 130.0f, -190.0f, -100.0f, 0.0f, 20, 31, 5, 1, 7, 7 },
};

WinDt win_dt2[7] = {
    { -140.0f, 75.0f, -120.0f, 60.0f, 13, 0, 6, 2, 1, 1 },
    { 140.0f, 75.0f, -120.0f, 60.0f, 12, 0, 6, 3, 0, 0 },
    { -140.0f, -65.0f, -120.0f, 60.0f, 11, 0, 0, 6, 3, 3 },
    { 140.0f, -65.0f, -120.0f, 60.0f, 10, 0, 1, 6, 2, 2 },
    { -200.0f, -150.0f, -80.0f, 0.0f, 16, 30, 2, 7, 6, 5 },
    { 0.0f, -150.0f, -80.0f, 0.0f, 17, 30, 2, 7, 4, 6 },
    { 200.0f, -150.0f, -80.0f, 0.0f, 18, 30, 3, 1, 6, 6 },
};

WinDt win_dt3[4] = {
    { -140.0f, 75.0f, -120.0f, 60.0f, 13, 0, 2, 2, 1, 1 },
    { 140.0f, 75.0f, -120.0f, 60.0f, 12, 0, 3, 3, 0, 0 },
    { -140.0f, -65.0f, -120.0f, 60.0f, 11, 0, 0, 0, 3, 3 },
    { 140.0f, -65.0f, -120.0f, 60.0f, 10, 0, 1, 1, 2, 2 },
};

// Defined after loadDraw, whose compound-literal templates come first in .rodata.
extern const char animgroup_OFF_f_in_out_mario[];
extern const char animgroup_OFF_f_in_out_kuppa[];
extern const char animgroup_OFF_f_in_out_peach[];
extern const char map_aji_10[];
extern const char map_kpa_00[];
extern const char map_nok_01[];
extern const char bero_nokotarou[];
extern const char map_hei_00[];
extern const char map_mri_00[];
extern const char map_mri_01[];
extern const char map_tou_02[];
extern const char map_usu_00[];
extern const char map_gor_04[];
extern const char map_muj_12[];
extern const char map_muj_00[];
extern const char map_pik_03[];
extern const char map_gor_01[];
extern const char bero_resha_return[];
extern const char map_pik_01[];
extern const char map_aji_18[];
extern const char bero_e_bero[];
extern const char map_gor_00[];
extern const char map_jin_04[];
extern const char map_aaa_00[];
extern const char bero_prologue[];
extern const f64 double_480_802ed948;
extern const f64 double_32_802ed950;
extern const f64 double_544_802ed958;

s32 stone_id[] = { 0x197, 0x198, 0x19A, 0x19C, 0x19D, 0x199, 0x19B };

static EVT_BEGIN(evt_shake)
    CALL(evt_sub_rumble_onoff, 0, 0)
    WAIT_MS(150)
    CALL(evt_sub_rumble_onoff, 1, 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_continue)
    CALL(evt_sub_animgroup_async, PTR((char*)animgroup_OFF_f_in_out_mario))
    CALL(evt_sub_animgroup_async, PTR((char*)animgroup_OFF_f_in_out_kuppa))
    CALL(evt_sub_animgroup_async, PTR((char*)animgroup_OFF_f_in_out_peach))
    RETURN
EVT_END()

// The last call in evt_memcard_delete/copy once the card was written (or the write missed): clears the fade and
// keeps the message window for the card check that state 1 reruns (flags 0x80).
s32 seqLoadPrepCardRecheck(EventEntry* event, BOOL isFirstCall) {
    fadeReset(4);
    wp->printWindowId = event->printWindowId;
    return 2;
}

void seq_loadInit(SequenceWork* workArg) {
    memset(wp, 0, sizeof(LoadWork));
}

void seq_loadExit(SequenceWork* workArg) {
    if (wp->file != NULL) {
        fileFree(wp->file);
    }
    wp->file = NULL;
}

void seq_loadMain(SequenceWork* workArg) {
    switch (workArg->state) {
        case 0: {
            BOOL result;
            if (fileAsyncf(FILE_TYPE_TPL, 0, "%s/w/jp/name.tpl\0\0\0", getMarioStDvdRoot()) != 0U) {
                wp->file = fileAllocf(FILE_TYPE_TPL, "%s/w/jp/name.tpl\0\0\0", getMarioStDvdRoot());
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
            wp->menuEvt = evtEntryType(evt_memcard_check, 0, 0, 0);
            if (wp->flags & 0x80) {
                wp->flags &= ~0x80;
                wp->menuEvt->lwData[10] = 1;
                wp->menuEvt->printWindowId = wp->printWindowId;
            }
            workArg->state++;
            break;
        case 2:
            if (!evtCheckID(wp->menuEvt->eventId)) {
                if (evtGetValue(NULL, 0xFAA2B580) != 1) {
                    seqSetSeq(SEQ_TITLE, NULL, NULL);
                    break;
                }
                wp->menuEvt = NULL;
                workArg->state++;
                wp->step = 0;
                wp->prevCardCode = cardGetCode();
                wp->flags &= ~0x40;
                if (OSGetSoundMode() == 0) {
                    SoundSetOutputMode(SND_OUTPUTMODE_MONO);
                    gp->soundSurround = 0;
                    wp->soundSaved = 0;
                } else {
                    SoundSetOutputMode(SND_OUTPUTMODE_STEREO);
                    if (gp->soundSurround == 0) {
                        wp->soundSaved = 1;
                    } else {
                        wp->soundSaved = 2;
                    }
                }
                wp->rumbleSaved = (gp->rumbleEnabled == 0);
                wp->soundCursor = wp->soundSaved;
                wp->rumbleCursor = wp->rumbleSaved;
                wp->cursor = gp->lastFileSlot;
                wp->cursorX = win_dt[wp->cursor].x;
                wp->cursorY = win_dt[wp->cursor].y;
            }
            break;
        case 3: {
            fadeEntry(0x1C, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
            workArg->state++;
            break;
        }
        case 4:
            loadMain(workArg);
            break;
    }
}

void loadMain(SequenceWork* workArg) {
    u32 btn = keyGetButtonTrg(0);
    u32 rep = keyGetButtonRep(0);
    u32 dir = keyGetDirRep(0);
    s32 sel;
    s32 code = cardGetCode();
    MarioWork* mario;
    s32 i;
    u16 flag;
    CardData* card;

    GXColor color;
    f32 bounce;
    s32 mode;
    (void)rep;

    if (cardIsExec() || !fadeIsFinish()) {
        btn = 0;
        dir = 0;
    }
    if (wp->prevCardCode != code) {
        btn = 0;
        dir = 0;
    }
    wp->prevCardCode = code;

    i = 0;
    while (i < 4) {
        card = cardGetFilePtr();
        flag = card->saves[i].data.flags;
        if (!(flag & 1) || (flag & 2)) {
            break;
        }
        i++;
    }
    if (i < 4) {
        wp->flags &= ~0x10;
    } else {
        wp->flags |= 0x10;
    }

    i = 0;
    wp->flags &= ~0x20;
    while (i < 4) {
        card = cardGetFilePtr();
        flag = card->saves[i].data.flags;
        if ((flag & 1) || (flag & 2)) {
            i++;
        } else {
            break;
        }
    }
    if (i >= 4) {
        wp->flags |= 0x20;
    }

    switch (wp->step) {
        case 0:
            wp->step++;
            /* fallthrough */
        case 1:
            wp->message = msgSearch("msg_savefile_select");
            sel = wp->cursor;

            if (dir & 0x1000) {
                wp->cursor = win_dt[sel].navUp;
            } else if (dir & 0x2000) {
                wp->cursor = win_dt[sel].navDown;
            } else if (dir & 0x4000) {
                wp->cursor = win_dt[sel].navLeft;
            } else if (dir & 0x8000) {
                wp->cursor = win_dt[sel].navRight;
            } else if (btn & 0x100) {
                card = cardGetFilePtr();
                flag = *(u16*)((u8*)card + (wp->cursor << 14) + 0x2000);
                switch (wp->cursor) {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                        if (flag & 2) {
                            psndSFXOn((const char*)0x13);
                        } else if (flag & 1) {
                            wp->step = 0x1e;
                            psndSFXOn((const char*)0x12);
                        } else {
                            wp->step = 0x14;
                            psndSFXOn((const char*)0x12);
                        }
                        break;
                    case 4:
                        if (!(wp->flags & 0x10)) {
                            i = 0;
                            while (i < 4) {
                                card = cardGetFilePtr();
                                flag = card->saves[i].data.flags;
                                if (!(flag & 1) || (flag & 2)) {
                                    break;
                                }
                                i++;
                            }
                            if (i < 4) {
                                psndSFXOn((const char*)0x12);
                                wp->cursor = 0;
                                wp->step = 0x28;
                            } else {
                                psndSFXOn((const char*)0x13);
                            }
                        }
                        break;
                    case 5:
                        if (!(wp->flags & 0x20)) {
                            i = 0;
                            while (i < 4) {
                                card = cardGetFilePtr();
                                flag = card->saves[i].data.flags;
                                if ((flag & 2) || (flag & 1)) {
                                    i++;
                                } else {
                                    break;
                                }
                            }
                            if (i < 4) {
                                psndSFXOn((const char*)0x12);
                                wp->cursor = 0;
                                wp->step = 0x32;
                            } else {
                                psndSFXOn((const char*)0x13);
                            }
                        }
                        break;
                    case 6:
                        psndSFXOn((const char*)0x13);
                        wp->step = 0xa;
                        break;
                    case 7:
                        psndSFXOn((const char*)0x12);
                        psndBGMOff(0x800);
                        wp->step = 0x64;
                        break;
                    case 8:
                        psndSFXOn((const char*)0x12);
                        wp->step = 0xc8;
                        break;
                }
            } else if (btn & 0x200) {
                psndSFXOn((const char*)0x13);
                wp->step = 0xa;
            }
            if (sel != wp->cursor) {
                if (wp->cursor < 4) {
                    psndSFXOn((const char*)9);
                } else {
                    psndSFXOn((const char*)5);
                }
            }
            wp->cursorTargetX = win_dt[wp->cursor].x + win_dt[wp->cursor].cursorOffX;
            wp->cursorTargetY = win_dt[wp->cursor].y + win_dt[wp->cursor].cursorOffY;
            break;

        case 0xa:
            fadeEntry(0x1b, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
            wp->step++;
            break;

        case 0xb:
            if (fadeIsFinish()) {
                seqSetSeq(SEQ_TITLE, NULL, NULL);
            }
            break;

        case 0x14:
            wp->flags |= 0x40;
            wp->menuEvt = evtEntryType(evt_memcard_start, 0, 0, 0);
            wp->menuEvt->lwData[0] = wp->cursor;
            wp->step++;
            break;

        case 0x15:
            if (!evtCheckID(wp->menuEvt->eventId)) {
                wp->flags &= ~0x40;
                if (evtGetValue(NULL, 0xFD050F80) == 1) {
                    wp->step++;
                } else {
                    wp->step = 1;
                }
            }
            break;

        case 0x16:
            fadeEntry(0x1b, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
            psndBGMOff_f_d(0x200, 0x7d0, 0);
            wp->step++;
            break;

        case 0x17:
            if (fadeIsFinish()) {
                wp->flags |= 0x40;
                wp->menuEvt = evtEntryType(evt_continue, 0, 0, 0);
                wp->step++;
            }
            break;

        case 0x18:
            if (!evtCheckID(wp->menuEvt->eventId)) {
                wp->flags &= ~0x40;
                continueGame();
            }
            break;

        case 0x1e:
            fadeEntry(0x1b, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
            wp->step++;
            break;

        case 0x1f:
            if (fadeIsFinish()) {
                wp->step++;
            }
            break;

        case 0x20:
            if (nameEntPrepare()) {
                nameEntOn(0);
                fadeEntry(0x1c, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
                wp->step++;
            }
            break;

        case 0x21:
            if (fadeIsFinish() && !nameEntWait()) {
                fadeEntry(0x18, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
                if (!nameEntIsCancel()) {
                    psndBGMOff_f_d(0x200, 0xbb8, 0);
                }
                wp->step++;
            }
            break;

        case 0x22:
            if (fadeIsFinish()) {
                nameEntOff();
                if (nameEntIsCancel()) {
                    fadeEntry(0x1c, 0, (GXColor){ 0x00, 0x00, 0x00, 0xFF });
                    wp->step = 1;
                } else {
                    strcpy(gp->playerName, nameEntGetName());
                    gp->lastFileSlot = wp->cursor;
                    stg0_00_init();
                    gp->saveSlot = wp->cursor;
                    marioPartyKill();
                    mario = marioGetPtr();
                    mario->prevPartyMemberId[0] = 0;
                    mario->prevPartyMemberId[1] = 0;
                    gp->areaFadeInType = 0;
                    gp->areaFadeOutType = 0x19;
                    swInit();
                    swByteSet(0, 0);
                    if (code == 0) {
                        gp->saveSlot = wp->cursor;
                        wp->menuEvt = evtEntryType(evt_memcard_save, 0, 0, 0);
                        wp->menuEvt->lwData[0] = 0x17;
                        wp->step++;
                    } else {
                        seqSetSeq(SEQ_MAP_CHANGE, map_aaa_00, bero_prologue);
                    }
                }
            }
            break;

        case 0x23:
            if (!evtCheckID(wp->menuEvt->eventId)) {
                seqSetSeq(SEQ_MAP_CHANGE, map_aaa_00, bero_prologue);
            }
            break;

        case 0x28:
            wp->message = msgSearch("msg_savefile_delete_select");
            sel = wp->cursor;

            if (dir & 0x1000) {
                wp->cursor = win_dt2[sel].navUp;
            } else if (dir & 0x2000) {
                wp->cursor = win_dt2[sel].navDown;
            } else if (dir & 0x4000) {
                wp->cursor = win_dt2[sel].navLeft;
            } else if (dir & 0x8000) {
                wp->cursor = win_dt2[sel].navRight;
            } else if (btn & 0x100) {
                if (sel < 4) {
                    card = cardGetFilePtr();
                    flag = *(u16*)((u8*)card + (wp->cursor << 14) + 0x2000);
                    if ((flag & 2) || !(flag & 1)) {
                        psndSFXOn((const char*)0x12);
                        wp->step++;
                    } else {
                        psndSFXOn((const char*)0x13);
                    }
                } else {
                    wp->cursor = 4;
                    wp->step = 1;
                    psndSFXOn((const char*)0x13);
                }
            } else if (btn & 0x200) {
                wp->cursor = 4;
                wp->step = 1;
                psndSFXOn((const char*)0x13);
            }
            if (sel != wp->cursor) {
                if (wp->cursor < 4) {
                    psndSFXOn((const char*)9);
                }
            }
            wp->cursorTargetX = win_dt2[wp->cursor].x + win_dt2[wp->cursor].cursorOffX;
            wp->cursorTargetY = win_dt2[wp->cursor].y + win_dt2[wp->cursor].cursorOffY;
            break;

        case 0x29:
            wp->flags |= 0x40;
            wp->menuEvt = evtEntryType(evt_memcard_delete, 0, 0, 0);
            wp->menuEvt->lwData[0] = wp->cursor;
            wp->step++;
            break;

        case 0x2a:
            if (!evtCheckID(wp->menuEvt->eventId)) {
                if (evtGetValue(NULL, 0xFAA2B580) == 0) {
                    wp->flags &= ~0x40;
                    wp->cursor = 4;
                    wp->step = 1;
                } else {
                    wp->flags |= 0x80;
                    workArg->state = 1;
                    return;
                }
            }
            break;

        case 0x32:
            wp->message = msgSearch("msg_savefile_copy_select");
            sel = wp->cursor;

            if (dir & 0x1000) {
                wp->cursor = win_dt2[sel].navUp;
            } else if (dir & 0x2000) {
                wp->cursor = win_dt2[sel].navDown;
            } else if (dir & 0x4000) {
                wp->cursor = win_dt2[sel].navLeft;
            } else if (dir & 0x8000) {
                wp->cursor = win_dt2[sel].navRight;
            } else if (btn & 0x100) {
                if (sel < 4) {
                    card = cardGetFilePtr();
                    flag = *(u16*)((u8*)card + (wp->cursor << 14) + 0x2000);
                    if (!(flag & 2) && !(flag & 1)) {
                        wp->step = 0x33;
                        wp->copySrc = wp->cursor;
                        wp->copyDst = (wp->copySrc + 1) % 4;
                        wp->cursor = wp->copyDst;
                        psndSFXOn((const char*)0x12);
                    }
                } else {
                    wp->cursor = 5;
                    wp->step = 1;
                    psndSFXOn((const char*)0x13);
                }
            } else if (btn & 0x200) {
                wp->cursor = 5;
                wp->step = 1;
                psndSFXOn((const char*)0x13);
            }
            if (sel != wp->cursor) {
                if (wp->cursor < 4) {
                    psndSFXOn((const char*)9);
                }
            }
            wp->cursorTargetX = win_dt2[wp->cursor].x + win_dt2[wp->cursor].cursorOffX;
            wp->cursorTargetY = win_dt2[wp->cursor].y + win_dt2[wp->cursor].cursorOffY;
            break;

        case 0x33:
            wp->message = msgSearch("msg_savefile_copy_point");
            sel = wp->cursor;

            if (dir & 0x1000) {
                do {
                    wp->cursor = win_dt3[wp->cursor].navUp;
                } while (wp->cursor == wp->copySrc);
            } else if (dir & 0x2000) {
                do {
                    wp->cursor = win_dt3[wp->cursor].navDown;
                } while (wp->cursor == wp->copySrc);
            } else if (dir & 0x4000) {
                do {
                    wp->cursor = win_dt3[wp->cursor].navLeft;
                } while (wp->cursor == wp->copySrc);
            } else if (dir & 0x8000) {
                do {
                    wp->cursor = win_dt3[wp->cursor].navRight;
                } while (wp->cursor == wp->copySrc);
            } else if (btn & 0x100) {
                wp->step = 0x34;
                wp->copyDst = wp->cursor;
                psndSFXOn((const char*)0x12);
            } else if (btn & 0x200) {
                wp->cursor = wp->copySrc;
                wp->step = 0x32;
                psndSFXOn((const char*)0x13);
            }
            if (sel != wp->cursor) {
                if (wp->cursor < 4) {
                    psndSFXOn((const char*)9);
                }
            }
            wp->copyDst = wp->cursor;
            wp->cursorTargetX = win_dt2[wp->cursor].x + win_dt2[wp->cursor].cursorOffX;
            wp->cursorTargetY = win_dt2[wp->cursor].y + win_dt2[wp->cursor].cursorOffY;
            break;

        case 0x34:
            wp->flags |= 0x40;
            wp->menuEvt = evtEntryType(evt_memcard_copy, 0, 0, 0);
            wp->menuEvt->lwData[0] = wp->copySrc;
            wp->menuEvt->lwData[1] = wp->copyDst;
            wp->step++;
            break;

        case 0x35:
            if (!evtCheckID(wp->menuEvt->eventId)) {
                if (evtGetValue(NULL, 0xFAA2B580) == 0) {
                    wp->flags &= ~0x40;
                    wp->cursor = 5;
                    wp->step = 1;
                } else {
                    wp->flags |= 0x80;
                    workArg->state = 1;
                    return;
                }
            }
            break;

        case 0x64:
            wp->flags |= 1;
            i = wp->soundCursor; // the cursor before this frame's input
            if (dir & 0x1000) {
                wp->soundCursor = i - 1;
                psndSFXOn((const char*)5);
            } else if (dir & 0x2000) {
                wp->soundCursor = i + 1;
                psndSFXOn((const char*)5);
            } else if (btn & 0x100) {
                switch (i) {
                    case 0:
                        OSSetSoundMode(0);
                        gp->soundSurround = 0;
                        break;
                    case 1:
                        OSSetSoundMode(1);
                        gp->soundSurround = 0;
                        break;
                    case 2:
                        OSSetSoundMode(1);
                        gp->soundSurround = 1;
                        break;
                }
                if (OSGetSoundMode() == 0) {
                    SoundSetOutputMode(SND_OUTPUTMODE_MONO);
                } else {
                    SoundSetOutputMode(SND_OUTPUTMODE_STEREO);
                }
                psndSFXOn((const char*)0x12);
                psndBGMOn(0xa0, NULL);
                psndENVOff(0x200);
                wp->soundSaved = wp->soundCursor;
                wp->flags &= ~1;
                wp->step = 1;
                break;
            } else if (btn & 0x200) {
                psndSFXOn((const char*)0x13);
                psndBGMOn(0xa0, NULL);
                psndENVOff(0x200);
                wp->soundCursor = wp->soundSaved;
                if (OSGetSoundMode() == 0) {
                    SoundSetOutputMode(SND_OUTPUTMODE_MONO);
                } else {
                    SoundSetOutputMode(SND_OUTPUTMODE_STEREO);
                }
                wp->flags &= ~1;
                wp->step = 1;
                break;
            }
            if (wp->soundCursor >= 3) {
                wp->soundCursor = 0;
            }
            if (wp->soundCursor < 0) {
                wp->soundCursor = 2;
            }
            mode = wp->soundCursor;
            if (mode != i) {
                switch (mode) {
                    case 0:
                        SoundSetOutputMode(SND_OUTPUTMODE_MONO);
                        break;
                    case 1:
                        SoundSetOutputMode(SND_OUTPUTMODE_STEREO);
                        break;
                    case 2:
                        SoundSetOutputMode(SND_OUTPUTMODE_SURROUND);
                        break;
                }
                psndENVOn(0x110, 0x75);
            }
            wp->cursorTargetX = -74.0f;
            wp->cursorTargetY = 4.0f + (50.0f - 50.0f * (f32)wp->soundCursor);
            break;

        case 0xc8:
            wp->flags |= 1;
            if (dir & 0x1000) {
                wp->rumbleCursor--;
                psndSFXOn((const char*)5);
            } else if (dir & 0x2000) {
                wp->rumbleCursor++;
                psndSFXOn((const char*)5);
            } else if (btn & 0x100) {
                gp->rumbleEnabled = (wp->rumbleCursor == 0);
                psndSFXOn((const char*)0x12);
                wp->rumbleSaved = wp->rumbleCursor;
                wp->flags &= ~1;
                wp->step = 1;
                if (gp->rumbleEnabled) {
                    evtEntryType(evt_shake, 0, 0, 0);
                }
            } else if (btn & 0x200) {
                psndSFXOn((const char*)0x13);
                wp->rumbleCursor = wp->rumbleSaved;
                wp->flags &= ~1;
                wp->step = 1;
            }
            if (wp->rumbleCursor >= 2) {
                wp->rumbleCursor = 0;
            }
            if (wp->rumbleCursor < 0) {
                wp->rumbleCursor = 1;
            }
            wp->cursorTargetX = -74.0f;
            wp->cursorTargetY = 4.0f + (30.0f - 50.0f * (f32)wp->rumbleCursor);
            break;
    }

    wp->cursorX += (wp->cursorTargetX - wp->cursorX) / 4.0f;
    wp->cursorY += (wp->cursorTargetY - wp->cursorY) / 4.0f;
    bounce = 2.0f * (f32)sin((f32)gp->retraceCount / 8.0f);
    wp->cursorX += __fabsf(bounce);
    dispEntry(4, 1000.0f, 0, (DispCallback)loadDraw, NULL);
}

// loadDraw's frame: the named aggregates are declared in the reverse of the original's slot order
// (viewport, projection, nameBuf, timeBuf) and the icon-block `Mtx mtx` is shared by the first icon
// block and the stone loop. The quad scales are passed as pointers to the named f64 pool objects so
// the `lfd` + `fabs` pair stays at the use instead of being folded to a constant.
static inline void loadDrawQuad(s32 texId, f32 x, f32 y, f32 w, f32 h, const f64* wAbs, const f64* hAbs) {
    GXTexObj texObj;
    Mtx mtx;
    f64 sy;
    f64 sx;
    f32 syf;

    TEXGetGXTexObjFromPalette(LOAD_TEX_PALETTE(), &texObj, texId);
    GXInitTexObjLOD(&texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    sy = __fabs(*hAbs);
    sx = __fabs(*wAbs);
    syf = (f32)sy / (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx, (f32)sx / (f32)GXGetTexObjWidth(&texObj), syf, 1.0f);
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

void loadDraw(void) {
    CameraEntry* cam = camGetPtr(CAMERA_2D);
    GXColor white = { 0xFF, 0xFF, 0xFF, 0xFF };
    GXColor greyA0 = { 0xA0, 0xA0, 0xA0, 0xFF };
    s32 i;
    s32 j;
    WinDt* wd;
    CardData* card;
    CardSaveSlot* slot;
    GXColor grey80 = { 0x80, 0x80, 0x80, 0x80 };
    f32 viewport[6];
    f32 projection[7];
    char nameBuf[0x20];
    char timeBuf[0x100];
    CameraEntry saved;

    if (wp->step >= 0x20 && wp->step < 0x28) {
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
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
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

    loadDrawQuad(0x35, -304.0f, 240.0f, 32.0f, 480.0f,
                 &double_32_802ed950, &double_480_802ed948);
    loadDrawQuad(0x34, -272.0f, 240.0f, 544.0f, 480.0f,
                 &double_544_802ed958, &double_480_802ed948);
    loadDrawQuad(0x36, 272.0f, 240.0f, 32.0f, 480.0f,
                 &double_32_802ed950, &double_480_802ed948);

    // The nine file windows.
    for (i = 0; i < 9; i++) {
        GXColor color = greyA0;
        if (i == wp->cursor) { color = white; }

        wd = &win_dt[i];
        if (wp->step == 0x33 && (i == wp->copySrc || i == wp->copyDst)) {
            color = white;
        }
        if (i == 4 && (wp->flags & 0x10)) {
            color = grey80;
        }
        if (i == 5 && (wp->flags & 0x20)) {
            color = grey80;
        }
        switch (wd->texId) {
        case 0xa:
        case 0xb:
        case 0xc:
        case 0xd:
            color.a = 0xFE;
            winTexInit(LOAD_TEX_PALETTE());
            winTexSet(0xe, (Vec){ wd->x, wd->y, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, color);
            winTexInit(LOAD_TEX_PALETTE());
            winTexSet(wd->texId, (Vec){ wd->x, wd->y + 60.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, color);
            break;
        default:
            winTexInit_x2(LOAD_TEX_PALETTE());
            winTexSet_x2(wd->texId, wd->texId2, (Vec){ wd->x, wd->y, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, color);
            break;
        }
    }

    windowDispGX_Waku_col(-250.0f, 200.0f, 500.0f, 40.0f, 20.0f, 0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    FontDrawStart();
    {
        const char* msg = wp->message;

        FontDrawMessage(-((s32)FontGetMessageWidth(msg)) / 2, 0xc0, msg);
    }

    // The four save files.
    for (i = 0; i < 4; i++) {
        u16 mask;
        u16 flags;
        s32 level;
        OSTime playTime; // save play time: the OSTime that GlobalWork names retraceLocalTime
        s32 gsw0; // story progress, 0 on a new file (shows the start text instead of the play time)
        const char* name;

        card = cardGetFilePtr();
        slot = &card->saves[i];
        flags = slot->data.flags;
        level = slot->data.pouch.level;
        playTime = slot->data.global.retraceLocalTime;
        mask = slot->data.pouch.mStarPowersObtained;
        gsw0 = slot->data.global.mGSW0;
        name = slot->data.global.playerName;
        wd = &win_dt[i];

        FontDrawStart();
        FontDrawColor((GXColor){ 0xC0, 0xC0, 0xC0, 0xFF });
        if (i == wp->cursor) {
            FontDrawColor((GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
        }
        FontDrawScale(0.6f);
        FontDrawEdge();
        sprintf(nameBuf, "\203f\201[\203^%s", winZenkakuStr(i + 1));
        FontDrawString(wd->x - 100.0f, 84.0f + wd->y, nameBuf);
        FontDrawEdgeOff();
        FontDrawScale(1.0f);
        FontDrawColor((GXColor){ 0x20, 0x20, 0x20, 0xFF });
        if (i == wp->cursor) {
            FontDrawColor((GXColor){ 0x00, 0x00, 0x00, 0xFF });
        }

        if (flags & 2) {
            const char* s = "\202\261\202\355\202\352\202\304\202\242\202\334\202\267";

            FontDrawString(wd->x - (f32)FontGetMessageWidth(s) / 2.0f, 12.0f + wd->y, s);
        } else if (flags & 1) {
            const char* s = "\203j\203\205\201[\203Q\201[\203\200";

            FontDrawString(wd->x - (f32)FontGetMessageWidth(s) / 2.0f, 12.0f + wd->y, s);
        } else if (gsw0 == 0) {
            const char* s = "\202\315\202\266\202\337\202\251\202\347";

            FontDrawString(wd->x - (f32)FontGetMessageWidth(s) / 2.0f, 12.0f + wd->y, s);
            if (strlen(name) != 0) {
                FontDrawString(wd->x - (f32)(FontGetMessageWidth(name) >> 1), 70.0f + wd->y, name);
            }
        } else {
            u32 hours;
            u32 mins;
            u64 secs;
            f32 half;
            s64 ticks = playTime;
            Mtx mtx;
            f32 half2;

            FontGetMessageWidth("\202W\202W");
            secs = ticks / (s64)(*(volatile u32*)0x800000F8 >> 2);
            if (secs > 359999) {
                hours = 99;
                mins = 59;
            } else {
                u64 t = secs;
                hours = (u32)(t / 3600);
                t %= 3600;
                mins = (u32)(t / 60);
            }

            if (strlen(name) != 0) {
                FontDrawString(wd->x - (f32)(FontGetMessageWidth(name) >> 1), 70.0f + wd->y, name);
            }
            sprintf(timeBuf, "\203\214\203x\203\213\201@%s", winZenkakuStr(level));
            FontDrawString(wd->x - (f32)FontGetMessageWidth(timeBuf) / 2.0f, 30.0f + wd->y, timeBuf);
            strcpy(timeBuf, "\203v\203\214\203C\202\266\202\251\202\361\201@");
            strcat(timeBuf, winZenkakuPadStr(hours, 2));
            strcat(timeBuf, " ");
            strcat(timeBuf, winZenkakuPadStr(mins, 2));
            {
                f32 w = (f32)FontGetMessageWidth(timeBuf);
                FontDrawString(wd->x - (half = w / 2.0f), wd->y, timeBuf);
            }
            strcpy(timeBuf, "\203v\203\214\203C\202\266\202\251\202\361\201@");
            strcat(timeBuf, winZenkakuPadStr(hours, 2));
            half2 = (f32)FontGetMessageWidth(timeBuf);
            {
                Mtx mtx2;
                GXColor c;

                PSMTXTrans(mtx, 8.0f + (half2 + (wd->x - half)), 1.0f + (wd->y - 22.0f), 0.0f);
                PSMTXScale(mtx2, 1.5f, 1.5f, 1.5f);
                PSMTXConcat(mtx, mtx2, mtx);
                c = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
                iconDispGxCol(mtx, 0, 0x201, c);
            }

            for (j = 0; j < 7; j++) {
                if (mask & (1 << (j + 1))) {
                    GXColor c;

                    PSMTXTrans(mtx, win_dt[i].x - 106.0f + (f32)(j * 0x20), win_dt[i].y - 52.0f, 0.0f);
                    c = (i == wp->cursor) ? white : greyA0;
                    iconDispGxCol(mtx, 0, stone_id[j] & 0xFFFF, c);
                }
            }
        }
    }

    if (wp->flags & 1) {
        wp->optionsFade += 0.05f;
    } else {
        wp->optionsFade -= 0.05f;
    }
    if (wp->optionsFade < 0.0f) {
        wp->optionsFade = 0.0f;
    }
    if (wp->optionsFade > 1.0f) {
        wp->optionsFade = 1.0f;
    }
    nameMaskGX(wp->optionsFade);

    if (wp->step >= 0x64 && wp->step < 0xc8) {
        winTexInit(LOAD_TEX_PALETTE());
        winTexSet(0xf, (Vec){ 0.0f, 0.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
        winTexInit_x2(LOAD_TEX_PALETTE());
        winTexSet_x2(0x13, 0x1f, (Vec){ 0.0f, 100.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });

        for (j = 0; j < 3; j++) {
            if (j == wp->soundCursor) {
                winTexInit(LOAD_TEX_PALETTE());
                winTexSet(0x33, (Vec){ 0.0f, (f32)(0x32 - j * 0x32), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0x00, 0x00, 0x00, 0x40 });
                winTexInit_x2(LOAD_TEX_PALETTE());
                winTexSet_x2(j + 0x15, 0x33, (Vec){ -4.0f, (f32)(0x36 - j * 0x32), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
            } else {
                winTexInit_x2(LOAD_TEX_PALETTE());
                winTexSet_x2(j + 0x15, 0x33, (Vec){ 0.0f, (f32)(0x32 - j * 0x32), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
            }
        }
    }

    if (wp->step >= 0xc8 && wp->step < 0x12c) {
        winTexInit(LOAD_TEX_PALETTE());
        winTexSet(0xf, (Vec){ 0.0f, 0.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
        winTexInit_x2(LOAD_TEX_PALETTE());
        winTexSet_x2(0x14, 0x1f, (Vec){ 0.0f, 100.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });

        for (j = 0; j < 2; j++) {
            if (j == wp->rumbleCursor) {
                winTexInit(LOAD_TEX_PALETTE());
                winTexSet(0x33, (Vec){ 0.0f, (f32)(0x1e - j * 0x32), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0x00, 0x00, 0x00, 0x40 });
                winTexInit_x2(LOAD_TEX_PALETTE());
                winTexSet_x2(j + 0x18, 0x33, (Vec){ -4.0f, (f32)(0x22 - j * 0x32), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
            } else {
                winTexInit_x2(LOAD_TEX_PALETTE());
                winTexSet_x2(j + 0x18, 0x33, (Vec){ 0.0f, (f32)(0x1e - j * 0x32), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
            }
        }
    }

    if (wp->step == 0x33 && wp->copyDst < 4) {
        f32 angles[4][4] = { { 0.0f, -90.0f, -180.0f, -135.0f }, { 90.0f, 0.0f, 135.0f, 180.0f }, { 0.0f, -45.0f, 0.0f, -90.0f }, { 45.0f, 0.0f, 90.0f, 0.0f } };
        f32 f29;
        f32 f30;

        if (!(f32)fmod(angles[wp->copySrc][wp->copyDst], 90.0)) {
            f32 mod = (f32)(wp->copySrc % 2);
            f32 fa = (f32)(wp->copySrc / 2);
            f32 fb = (f32)(wp->copyDst / 2);
            if (fa == fb) {
                f29 = 0.0f;
                f30 = 120.0f - 140.0f * fa;
            } else {
                f29 = 300.0f * mod + -150.0f;
                f30 = 50.0f;
            }
        } else {
            f29 = 0.0f;
            f30 = 50.0f;
        }
        f30 -= 40.0f;

        {
            Mtx mtx;

            winTexInit(LOAD_TEX_PALETTE());
            winTexSetRot(0x1d, (Vec){ 2.0f + f29, f30 - 2.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, angles[wp->copySrc][wp->copyDst], (GXColor){ 0x00, 0x00, 0x00, 0x80 });
            winTexSetRot(0x1d, (Vec){ f29, f30, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, angles[wp->copySrc][wp->copyDst], white);
            (void)mtx;
        }
    }

    if (!(wp->flags & 0x40)) {
        winTexInit(LOAD_TEX_PALETTE());
        winTexSet(0, (Vec){ wp->cursorX + 2.0f, wp->cursorY - 2.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0x00, 0x00, 0x00, 0x80 });
        winTexSet(0, (Vec){ wp->cursorX, wp->cursorY, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white);
    }

    GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
    GXSetProjectionv(projection);
    *camGetCurPtr() = saved;
}

const char animgroup_OFF_f_in_out_mario[] = "OFF_f_in_out_mario\000";
const char animgroup_OFF_f_in_out_kuppa[] = "OFF_f_in_out_kuppa\000";
const char animgroup_OFF_f_in_out_peach[] = "OFF_f_in_out_peach\000";
const char map_aji_10[] = "aji_10\000";
const char map_kpa_00[] = "kpa_00\000";
const char map_nok_01[] = "nok_01\000";
const char bero_nokotarou[] = "nokotarou\000\000";
const char map_hei_00[] = "hei_00\000";
const char map_mri_00[] = "mri_00\000";
const char map_mri_01[] = "mri_01\000";
const char map_tou_02[] = "tou_02\000";
const char map_usu_00[] = "usu_00\000";
const char map_gor_04[] = "gor_04\000";
const char map_muj_12[] = "muj_12\000";
const char map_muj_00[] = "muj_00\000";
const char map_pik_03[] = "pik_03\000";
const char map_gor_01[] = "gor_01\000";
const char bero_resha_return[] = "resha_return\000\000\000";
const char map_pik_01[] = "pik_01\000";
const char map_aji_18[] = "aji_18\000";
const char bero_e_bero[] = "e_bero\000";
const char map_gor_00[] = "gor_00\000";
const char map_jin_04[] = "jin_04\000";
const char map_aaa_00[] = "aaa_00\000";
const char bero_prologue[] = "prologue\000\000\000";
const f64 double_480_802ed948 = 480;
const f64 double_32_802ed950 = 32;
const f64 double_544_802ed958 = 544;

void continueGame(void) {
    MarioWork* mario;

    cardCopy2Main(wp->cursor);
    gp->saveSlot = wp->cursor;
    gp->lastFileSlot = wp->cursor;
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
            seqSetSeq(SEQ_MAP_CHANGE, map_aji_10, NULL);
            break;
        case 2:
            gp->areaFadeInType = 0x3E;
            gp->areaFadeOutType = 0x3A;
            seqSetSeq(SEQ_MAP_CHANGE, map_kpa_00, NULL);
            break;
        case 3:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, map_nok_01, bero_nokotarou);
            break;
        case 4:
            gp->areaFadeInType = 0x3D;
            gp->areaFadeOutType = 0x38;
            seqSetSeq(SEQ_MAP_CHANGE, map_aji_10, NULL);
            break;
        case 5:
            gp->areaFadeInType = 0x3E;
            gp->areaFadeOutType = 0x3A;
            seqSetSeq(SEQ_MAP_CHANGE, map_hei_00, NULL);
            break;
        case 6:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, map_mri_00, NULL);
            break;
        case 7:
            gp->areaFadeInType = 0x3D;
            gp->areaFadeOutType = 0x38;
            seqSetSeq(SEQ_MAP_CHANGE, map_aji_10, NULL);
            break;
        case 8:
            gp->areaFadeInType = 0x3E;
            gp->areaFadeOutType = 0x3A;
            seqSetSeq(SEQ_MAP_CHANGE, map_mri_01, NULL);
            break;
        case 9:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, map_tou_02, NULL);
            break;
        case 10:
            gp->areaFadeInType = 0;
            gp->areaFadeOutType = 0x1C;
            seqSetSeq(SEQ_MAP_CHANGE, map_usu_00, NULL);
            break;
        case 11:
            gp->areaFadeInType = 0x3E;
            gp->areaFadeOutType = 0x3A;
            seqSetSeq(SEQ_MAP_CHANGE, map_gor_04, NULL);
            break;
        case 12:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, map_usu_00, NULL);
            break;
        case 13:
            gp->areaFadeInType = 0;
            gp->areaFadeOutType = 0x1C;
            seqSetSeq(SEQ_MAP_CHANGE, map_muj_12, NULL);
            break;
        case 14:
            gp->areaFadeInType = 0x3E;
            gp->areaFadeOutType = 0x3A;
            seqSetSeq(SEQ_MAP_CHANGE, map_usu_00, NULL);
            break;
        case 15:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, map_muj_00, NULL);
            break;
        case 16:
            gp->areaFadeInType = 0;
            gp->areaFadeOutType = 0x1C;
            seqSetSeq(SEQ_MAP_CHANGE, map_pik_03, "next");
            break;
        case 17:
            gp->areaFadeInType = 0x3E;
            gp->areaFadeOutType = 0x3A;
            seqSetSeq(SEQ_MAP_CHANGE, map_gor_01, NULL);
            break;
        case 18:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, map_gor_04, bero_resha_return);
            break;
        case 19:
            gp->areaFadeInType = 0x3E;
            gp->areaFadeOutType = 0x3A;
            seqSetSeq(SEQ_MAP_CHANGE, map_pik_01, NULL);
            break;
        case 20:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, map_aji_18, bero_e_bero);
            break;
        case 21:
            gp->areaFadeInType = 0x3C;
            gp->areaFadeOutType = 0x36;
            seqSetSeq(SEQ_MAP_CHANGE, map_gor_00, NULL);
            break;
        case 22:
            gp->areaFadeInType = 0;
            gp->areaFadeOutType = 0x1C;
            seqSetSeq(SEQ_MAP_CHANGE, map_jin_04, NULL);
            break;
        case 23:
            stg0_00_init();
            gp->saveSlot = wp->cursor;
            marioPartyKill();
            mario = marioGetPtr();
            mario->prevPartyMemberId[0] = 0;
            mario->prevPartyMemberId[1] = 0;
            gp->areaFadeInType = 0;
            gp->areaFadeOutType = 0x19;
            swInit();
            swByteSet(0, 0);
            seqSetSeq(SEQ_MAP_CHANGE, map_aaa_00, bero_prologue);
            break;
        case 24:
            gp->areaFadeInType = 0x3D;
            gp->areaFadeOutType = 0x38;
            seqSetSeq(SEQ_MAP_CHANGE, map_aji_10, NULL);
            break;
    }
}
