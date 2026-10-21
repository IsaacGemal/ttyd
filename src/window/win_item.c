#include "window/win_item.h"
#include "window/win_root.h"
#include "data/item_data.h"
#include <dolphin/gx.h>
#include "window/win_main.h"
#include "win/win_main.h"
#include "manager/winmgr.h"
#include "manager/filemgr.h"
#include "manager/fontmgr.h"
#include "driver/msgdrv.h"
#include "mario/mario_pouch.h"
#include "pmario_sound.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include "motion/mot_slit.h"
#include "motion/mot_roll.h"
#include "driver/swdrv.h"
#include "data/mail_data.h"
#include "system.h"
#include <stdio.h>
#include <stdarg.h>

// These .rodata strings are deliberately NOT defined here: defining them would push the
// compiler's anonymous int->float pool doubles behind them (0x958 instead of 0x8f0), which
// breaks the base+offset operands of itemUseDisp2/winItemDisp. They belong inline in the
// functions that use them (owned by the other agents working on this file).
// The pool doubles themselves need no declaration: the compiler emits its own @NNN@ entries.
// str_msg_menu_sort_narabikae ("msg_menu_sort_narabikae") is used as an inline literal in item_disp instead:
// pooling it there puts it at 0x90c, between itemUseDisp's name_mario and winItemMain's three
// strings, so winItemMain's displacements come out right.
extern const char str_msg_menu_mochi_item[];
extern const char str_msg_menu_mochi_daiji[];
extern const char str_s_w_jp_mail_tpl[];
#ifdef MUST_MATCH
// The original reaches these through the luigi_book_1_page1 base register; inline literals pool so that the base folds.
#define STR_MOCHI_ITEM "msg_menu_mochi_item"
#define STR_MOCHI_DAIJI "msg_menu_mochi_daiji"
#define STR_MAIL_TPL "%s/w/jp/mail.tpl\0\0\0"
#else
#define STR_MOCHI_ITEM str_msg_menu_mochi_item
#define STR_MOCHI_DAIJI str_msg_menu_mochi_daiji
#define STR_MAIL_TPL str_s_w_jp_mail_tpl
#endif


// The window draw functions take their Vec and GXColor arguments by value. win_main.h declares the font and
// icon ones with pointers, which compiles to the same callee.
#ifdef MUST_MATCH
// Match-only: called through by-value casts (caller copies plus a pointer, ABI-identical).
#define winFontSetV ((void (*)(Vec, Vec, GXColor, const char*, ...))winFontSet)
#define winFontSetRV ((void (*)(Vec, Vec, GXColor, const char*, ...))winFontSetR)
#define winIconSetV ((void (*)(s32, Vec, Vec, GXColor))winIconSet)
#define winFontSetPitchV ((void (*)(Vec, Vec, GXColor, f32, const char*, ...))winFontSetPitch)
#else
static void winItemFontV(void (*fn)(Vec*, Vec*, GXColor*, const char*, ...), Vec pos, Vec scale,
                         GXColor color, const char* format, va_list args) {
    char text[0x200];

    vsprintf(text, format, args);
    fn(&pos, &scale, &color, "%s", text);
}
static void winFontSetV(Vec pos, Vec scale, GXColor color, const char* format, ...) {
    va_list args;

    va_start(args, format);
    winItemFontV(winFontSet, pos, scale, color, format, args);
    va_end(args);
}
static void winFontSetRV(Vec pos, Vec scale, GXColor color, const char* format, ...) {
    va_list args;

    va_start(args, format);
    winItemFontV(winFontSetR, pos, scale, color, format, args);
    va_end(args);
}
static inline void winIconSetV(s32 id, Vec pos, Vec scale, GXColor color) {
    winIconSet(id, &pos, &scale, &color);
}
static inline void winFontSetPitchV(Vec pos, Vec scale, GXColor color, f32 pitch, const char* text) {
    winFontSetPitch(&pos, scale, &color, pitch, "%s", text);
}
#endif

extern u16 menu_skip_list[18];
extern const char** luigi_book_list[5];
extern WindowPartyDt winPartyDt[];
extern const GXColor dat_8041d46c;
extern const GXColor dat_8041d470;
extern const GXColor dat_8041d474;
extern const GXColor dat_8041d478;
extern const GXColor dat_8041d47c;

extern const GXColor dat_8041d4a4;

// .sdata2 colours used by itemUseDisp (defined at the end of the file).
extern const GXColor dat_8041d480;
extern const GXColor dat_8041d484;
extern const GXColor dat_8041d488;
extern const GXColor dat_8041d48c;
extern const GXColor dat_8041d490;
extern const GXColor dat_8041d494;
extern const GXColor dat_8041d498;
extern const GXColor dat_8041d49c;
extern const GXColor dat_8041d4a0;
extern WindowPartyDt winPartyDt[];

// .rodata prefix of window/win_item: the 60 msg_luigi_book strings. The Vec templates after them
// come from the draw functions' compound literals, in source order.
const char luigi_book_1_page1[24] = "msg_luigi_book1_txt1\000\000\000";

const char luigi_book_1_page2[24] = "msg_luigi_book1_txt2\000\000\000";

const char luigi_book_1_page3[24] = "msg_luigi_book1_txt3\000\000\000";

const char luigi_book_1_page4[24] = "msg_luigi_book1_txt4\000\000\000";

const char luigi_book_1_page5[24] = "msg_luigi_book1_txt5\000\000\000";

const char luigi_book_1_page6[24] = "msg_luigi_book1_txt6\000\000\000";

const char luigi_book_1_page7[24] = "msg_luigi_book1_txt7\000\000\000";

const char luigi_book_1_page8[24] = "msg_luigi_book1_txt8\000\000\000";

const char luigi_book_1_page9[24] = "msg_luigi_book1_txt9\000\000\000";

const char luigi_book_1_page10[24] = "msg_luigi_book1_txt10\000\000";

const char luigi_book_1_page11[24] = "msg_luigi_book1_txt11\000\000";

const char luigi_book_1_page12[24] = "msg_luigi_book1_txt12\000\000";

const char luigi_book_2_page1[24] = "msg_luigi_book2_txt1\000\000\000";

const char luigi_book_2_page2[24] = "msg_luigi_book2_txt2\000\000\000";

const char luigi_book_2_page3[24] = "msg_luigi_book2_txt3\000\000\000";

const char luigi_book_2_page4[24] = "msg_luigi_book2_txt4\000\000\000";

const char luigi_book_2_page5[24] = "msg_luigi_book2_txt5\000\000\000";

const char luigi_book_2_page6[24] = "msg_luigi_book2_txt6\000\000\000";

const char luigi_book_2_page7[24] = "msg_luigi_book2_txt7\000\000\000";

const char luigi_book_2_page8[24] = "msg_luigi_book2_txt8\000\000\000";

const char luigi_book_2_page9[24] = "msg_luigi_book2_txt9\000\000\000";

const char luigi_book_2_page10[24] = "msg_luigi_book2_txt10\000\000";

const char luigi_book_2_page11[24] = "msg_luigi_book2_txt11\000\000";

const char luigi_book_2_page12[24] = "msg_luigi_book2_txt12\000\000";

const char luigi_book_3_page1[24] = "msg_luigi_book3_txt1\000\000\000";

const char luigi_book_3_page2[24] = "msg_luigi_book3_txt2\000\000\000";

const char luigi_book_3_page3[24] = "msg_luigi_book3_txt3\000\000\000";

const char luigi_book_3_page4[24] = "msg_luigi_book3_txt4\000\000\000";

const char luigi_book_3_page5[24] = "msg_luigi_book3_txt5\000\000\000";

const char luigi_book_3_page6[24] = "msg_luigi_book3_txt6\000\000\000";

const char luigi_book_3_page7[24] = "msg_luigi_book3_txt7\000\000\000";

const char luigi_book_3_page8[24] = "msg_luigi_book3_txt8\000\000\000";

const char luigi_book_3_page9[24] = "msg_luigi_book3_txt9\000\000\000";

const char luigi_book_3_page10[24] = "msg_luigi_book3_txt10\000\000";

const char luigi_book_3_page11[24] = "msg_luigi_book3_txt11\000\000";

const char luigi_book_3_page12[24] = "msg_luigi_book3_txt12\000\000";

const char luigi_book_4_page1[24] = "msg_luigi_book4_txt1\000\000\000";

const char luigi_book_4_page2[24] = "msg_luigi_book4_txt2\000\000\000";

const char luigi_book_4_page3[24] = "msg_luigi_book4_txt3\000\000\000";

const char luigi_book_4_page4[24] = "msg_luigi_book4_txt4\000\000\000";

const char luigi_book_4_page5[24] = "msg_luigi_book4_txt5\000\000\000";

const char luigi_book_4_page6[24] = "msg_luigi_book4_txt6\000\000\000";

const char luigi_book_4_page7[24] = "msg_luigi_book4_txt7\000\000\000";

const char luigi_book_4_page8[24] = "msg_luigi_book4_txt8\000\000\000";

const char luigi_book_4_page9[24] = "msg_luigi_book4_txt9\000\000\000";

const char luigi_book_4_page10[24] = "msg_luigi_book4_txt10\000\000";

const char luigi_book_4_page11[24] = "msg_luigi_book4_txt11\000\000";

const char luigi_book_4_page12[24] = "msg_luigi_book4_txt12\000\000";

const char luigi_book_5_page1[24] = "msg_luigi_book5_txt1\000\000\000";

const char luigi_book_5_page2[24] = "msg_luigi_book5_txt2\000\000\000";

const char luigi_book_5_page3[24] = "msg_luigi_book5_txt3\000\000\000";

const char luigi_book_5_page4[24] = "msg_luigi_book5_txt4\000\000\000";

const char luigi_book_5_page5[24] = "msg_luigi_book5_txt5\000\000\000";

const char luigi_book_5_page6[24] = "msg_luigi_book5_txt6\000\000\000";

const char luigi_book_5_page7[24] = "msg_luigi_book5_txt7\000\000\000";

const char luigi_book_5_page8[24] = "msg_luigi_book5_txt8\000\000\000";

const char luigi_book_5_page9[24] = "msg_luigi_book5_txt9\000\000\000";

const char luigi_book_5_page10[24] = "msg_luigi_book5_txt10\000\000";

const char luigi_book_5_page11[24] = "msg_luigi_book5_txt11\000\000";

const char luigi_book_5_page12[24] = "msg_luigi_book5_txt12\000\000";





static WinMgrDesc window_desc[2];

// Item window state machine: the held-item grid, the key-item list, the Luigi book
// (case 100) and the received-mail viewer (cases 200/201/202). Transliterated from the
// original assembly; every call, branch, loop and store is present.
// The original keeps luigi_book_1_page1 in r30 and reaches str_msg_menu_mochi_item/20/38 as r30+0x924/0x938/
// 0x950 (one lis/addi + displacement). Our object pools the string literals used here, in the
// same order (name_mario 0x900, msg_menu_sort_narabikae 0x90c, then the three below), so the
// base register folds correctly. The frame (0x60), the int->float scratch doubles
// (r1+0x30..0x3f), the callee-saved block (stmw r27, r1+0x4c) and the party-pointer list
// (0-based, read as `list[cursor - 1]`) all match, and the function is the same length as the
// original (1180 instructions). The `dirRep & 0xC000` arm's base/value register pair (r5/r4)
// came out swapped until the element was reached through a pointer
// (`s32* e = &list[sel]; idx = *e;`) rather than a repeated subscript, which colours it as the
// original. 100%.
//
// `winItemMailLine` only exists to defeat MWCC's CSE: written inline, the `work + 0x26C` of
// the case-201 call argument is shared with the following `+= 1` / `+= 0xA`, so MWCC
// materialises it into r31 and copies it into r4 (`addi r31, r28, 0x26c` / `mr r4, r31`),
// where the original computes it straight into r4 and uses displacements afterwards. Going
// through a (trivially inlined) function makes the address a separate value and restores the
// original sequence.
static inline u16* winItemMailLine(void* w) { return &((WindowWork*)w)->mailLines; }


void winMakeSkipList(WindowWork* work) {
    s32 i;
    s32 j;

    work->skipCount = 0;
    for (i = 0; i < 0x79; i++) {
        u16 item = pouchKeyItem(i);

        if (item == 0) {
            continue;
        }
        for (j = 0; j < 0x12; j++) {
            if (item == menu_skip_list[j]) {
                break;
            }
        }
        if ((u32)j >= 0x12) {
            work->skipList[work->skipCount] = item;
            work->skipCount++;
        }
    }
}

// The key-list scan walks the skip table with both an index and a pointer; the
// original increments the pointer before the index, which is what settles MWCC's
// register choice (pointer r5, index r3).
void winItemInit(WindowWork* work) {
    s32 i;
    u16 item;
    s32 j;

    work->itemState = 0;
    work->itemTab = 0;
    work->itemSel[0] = 0;
    work->itemScrollRow[0] = 0;
    work->itemSel[1] = 0;
    work->itemScrollRow[1] = 0;
    work->field_0x238 = 320.f;
    work->field_0x234 = 320.f;
    work->field_0x240 = -240.f;
    work->field_0x23C = -240.f;
    work->field_0x248 = 320.f;
    work->field_0x244 = 320.f;
    work->field_0x250 = -240.f;
    work->field_0x24C = -240.f;

    if ((s32)pouchCheckItem(ITEM_MAILBOX_SP) == 0) {
        pouchGetItem(ITEM_MAILBOX_SP);
    }
    work->mailCount = pouchReceiveMailCount();

    work->mailSel = 0;
    work->mailPage = 0;
    work->field_0x264 = 0.0f;
    work->field_0x260 = 0.0f;

    {
        for (i = 0; i < work->mailCount; i++) {
            u8* pouch;
            s32 off;
            work->field_26E[i * 2] = work->mailCount - i - 1;
            pouch = (u8*)pouchGetPtr();
            off = 0x519 + (work->mailCount - i);
            work->field_26E[i * 2 + 1] = pouch[off];
        }
    }

    work->useItemId = 0;
    work->skipCount = 0;
    for (i = 0; i < 0x79; i++) {
        item = pouchKeyItem(i);

        if (item == 0) {
            continue;
        }
        {
            u16* list;
            j = 0;
            list = menu_skip_list;
            while (j < 0x12) {
                if (item == *list) {
                    break;
                }
                list++;
                j++;
            }
        }
        if ((u32)j >= 0x12) {
            work->skipList[work->skipCount] = item;
            work->skipCount++;
        }
    }

    work->useTargetWin[0] = winMgrEntry(&window_desc[0]);
    work->useTargetWin[1] = winMgrEntry(&window_desc[1]);
    winMgrSetPriority(work->useTargetWin[0], -0xC8);
    winMgrSetPriority(work->useTargetWin[1], -0x64);
    fileAsyncf(FILE_TYPE_TPL, NULL, STR_MAIL_TPL, getMarioStDvdRoot());
}

void winItemInit2(WindowWork* work) {
    f32 t;

    work->targetX = -275.0f;
    work->targetY = (f32)(0x7D - work->itemTab * 0x28);

    t = 190.f * (f32)work->itemScrollRow[0];
    work->field_0x22C = t;
    work->field_0x224 = t;

    t = 190.f * (f32)work->itemScrollRow[1];
    work->field_0x230 = t;
    work->field_0x228 = t;

    work->field_0x238 = 320.f;
    work->field_0x234 = 320.f;
    work->field_0x240 = -240.f;
    work->field_0x23C = -240.f;
    work->field_0x248 = 320.f;
    work->field_0x244 = 320.f;
    work->field_0x250 = -240.f;
    work->field_0x24C = -240.f;

    t = (f32)(work->mailSel * 0xC8);
    work->field_0x264 = t;
    work->field_0x260 = t;

    winMsgEntry(work, 0, work->itemTab == 0 ? STR_MOCHI_ITEM : STR_MOCHI_DAIJI, NULL);
    work->useItemId = 0;
}

void winItemExit(WindowWork* work) {
    winMgrDelete(work->useTargetWin[0]);
    winMgrDelete(work->useTargetWin[1]);
}

// 100%: every call, branch, constant, store and control-flow edge matches.
s32 winItemMain(WindowWork* work) {
    s32 sel;
    s32 count;
    s32 rows;
    s32 idx;
    s32 idx2;
    s32 item;
    s32 i;
    s32 n;
    s32 old;
    s32 page;
    WindowPartyDt* list[9];
    s32 leader;

    switch (work->itemState) {
    case 0:
        if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            return -1;
        }
        if (work->buttonTrg & 0x1000) {
            return -2;
        }
        if (work->dirRep & 0x3000) {
            work->itemTab = 1 - work->itemTab;
            psndSFXOn((const char*)0x20005);
        } else if (work->buttonTrg & 0x100) {
            if (work->itemTab == 0) {
                if (pouchGetHaveItemCnt() == 0) {
                    break;
                }
            }
            if (work->itemTab == 1) {
                if (work->skipCount == 0) {
                    break;
                }
            }
            work->itemState = 10;
            psndSFXOn((const char*)0x20012);
            break;
        } else if (work->buttonTrg & 0x10) {
            winSortEntry(work, work->itemTab != 0, -310.f, 150.f);
            work->savedState = work->itemState;
            psndSFXOn((const char*)0x20012);
            work->itemState = 1000;
        }
        work->targetX = -275.0f;
        work->targetY = (f32)(0x7D - work->itemTab * 0x28);
        winMsgEntry(work, 0, work->itemTab == 0 ? STR_MOCHI_ITEM : STR_MOCHI_DAIJI, NULL);
        break;

    case 10:
        if (work->itemTab == 0) {
            count = pouchGetHaveItemCnt();
        } else {
            count = work->skipCount;
        }
        sel = work->itemTab;
        rows = (count + 9) / 10;

        if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            work->itemState = 0;
        }
        if (work->buttonTrg & 0x100) {
            if (work->itemTab == 0) {
                item = pouchHaveItem(work->itemSel[sel]);
                if (itemDataTable[item].usable_locations & 4) {
                    WinMgrEntry* entry;

                    work->useItemId = item;
                    work->useItemSlot = work->itemSel[sel];
                    work->itemState = 300;
                    work->useTargetSel = 0;
                    work->useTimer = 0;
                    psndSFXOn((const char*)0x20012);
                    entry = winMgrGetPtr(work->useTargetWin[0]);
                    winMgrSetSize(work->useTargetWin[0], entry->x, entry->y, entry->width,
                                  work->partyCount * 0x1C + 0x44);
                    winMgrOpen(work->useTargetWin[0]);
                    winMgrOpen(work->useTargetWin[1]);
                }
            } else {
                idx = work->itemSel[sel];
                idx2 = work->skipList[idx];
                item = idx2;
                if ((u32)(item - ITEM_SUPER_LUIGI1) <= 3 || item == ITEM_SUPER_LUIGI5) {
                    const char** book;

                    work->itemState = 100;
                    work->field_0x238 = -170.f;
                    work->field_0x240 = 160.f;
                    work->flags |= 0x8000;
                    book = luigi_book_list[item - ITEM_SUPER_LUIGI1];
                    work->field_0x2C8 = item - ITEM_SUPER_LUIGI1;
                    work->field_0x2D0 = 0;
                    work->field_0x2CC = 0;
                    while (book[work->field_0x2CC] != 0) {
                        work->field_0x2CC++;
                    }
                    work->field_0x2CC = (work->field_0x2CC + 1) / 2;
                    psndSFXOn((const char*)0x20012);
                } else if (item == ITEM_MAILBOX_SP && pouchReceiveMailCount() != 0) {
                    work->itemState = 200;
                    work->field_0x248 = -144.0f;
                    work->field_0x250 = 180.f;
                    psndSFXOn((const char*)0x20012);
                } else if (item == 0x4E) {
                    work->useItemId = item;
                    psndSFXOn((const char*)0x20012);
                    return -2;
                }
            }
        } else if (work->buttonRep & 0x40) {
            work->itemScrollRow[sel] -= 1;
            if (work->itemScrollRow[sel] < 0) {
                work->itemScrollRow[sel] = 0;
            }
            work->itemSel[sel] = work->itemScrollRow[sel] * 10;
            psndSFXOn((const char*)0x20035);
        } else if (work->buttonRep & 0x20) {
            work->itemScrollRow[sel] += 1;
            if (work->itemScrollRow[sel] >= rows) {
                work->itemScrollRow[sel] = rows - 1;
            }
            work->itemSel[sel] = work->itemScrollRow[sel] * 10;
            psndSFXOn((const char*)0x20035);
        } else if (work->dirRep & 0xC000) {
            // A pointer to the element (rather than the array subscript repeated) is what makes
            // MWCC colour the base r5 / value r4 as the original does. It is built from the byte offset:
            // `&work->itemSel[sel]` folds the 0x214 into an lwzx index instead of a load displacement.
            s32* e = &((s32*)((u8*)work + 0x214))[sel];
            idx = *e;
            if (idx % 2 != 0) {
                s32 v = idx - 1;
                if (v >= 0 && v < count) {
                    *e = idx - 1;
                }
            } else {
                s32 v = idx + 1;
                if (v >= 0 && v < count) {
                    *e = idx + 1;
                }
            }
            psndSFXOn((const char*)0x20035);
        } else if (work->dirRep & 0x1000) {
            idx = work->itemSel[sel];
            {
                s32 v = idx - 2;
                if (v >= 0 && v < count) {
                    work->itemSel[sel] = idx - 2;
                }
            }
            work->itemScrollRow[sel] = work->itemSel[sel] / 10;
            psndSFXOn((const char*)0x20035);
        } else if (work->dirRep & 0x2000) {
            idx = work->itemSel[sel];
            {
                s32 v = idx + 2;
                if (v >= 0 && v < count) {
                    work->itemSel[sel] = idx + 2;
                } else if (idx + (idx & 1) < count) {
                    work->itemSel[sel] = count - 1;
                }
            }
            work->itemScrollRow[sel] = work->itemSel[sel] / 10;
            psndSFXOn((const char*)0x20035);
        } else if (work->buttonTrg & 0x10) {
            winSortEntry(work, work->itemTab != 0, -310.f, 150.f);
            work->savedState = work->itemState;
            psndSFXOn((const char*)0x20012);
            work->itemState = 1000;
        } else if (work->buttonTrg & 0x1000) {
            return -2;
        }
        work->targetX = (f32)((work->itemSel[sel] % 10 % 2) * 0xCC - 0x9B);
        work->targetY = (f32)(0x6E - (work->itemSel[sel] % 10 / 2) * 0x26);
        item = (work->itemTab == 0) ? pouchHaveItem(work->itemSel[sel])
                                    : (s32)work->skipList[work->itemSel[sel]];
        winMsgEntry(work, item, itemDataTable[item].description, NULL);
        break;

    case 100:
        if (work->buttonTrg & 0x100) {
            psndSFXOn((const char*)0x20012);
            work->field_0x2D0 += 1;
            if (work->field_0x2D0 >= work->field_0x2CC) {
                work->field_0x2D0 = work->field_0x2CC - 1;
                work->itemState = 10;
                work->field_0x238 = 320.f;
                work->field_0x240 = -240.f;
                work->flags &= ~0x8000;
                work->posX = work->targetX;
                work->posY = work->targetY;
            }
        } else if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            work->field_0x2D0 -= 1;
            if (work->field_0x2D0 < 0) {
                work->field_0x2D0 = 0;
                work->itemState = 10;
                work->field_0x238 = 320.f;
                work->field_0x240 = -240.f;
                work->flags &= ~0x8000;
                work->posX = work->targetX;
                work->posY = work->targetY;
            }
        } else if (work->buttonTrg & 0x1000) {
            work->field_0x238 = 320.f;
            work->field_0x240 = -240.f;
            work->flags &= ~0x8000;
            work->posX = work->targetX;
            work->posY = work->targetY;
            return -2;
        }
        work->targetX = -170.f;
        work->targetY = 150.f;
        break;

    case 200:
        if (work->buttonTrg & 0x100) {
            WinMailEntry* mail;

            if ((s32)pouchCheckMail(work->field_26E[work->mailSel * 2 + 1]) == 1) {
                pouchOpenMail(work->field_26E[work->mailSel * 2 + 1]);
            }
            swSet(0x26);
            psndSFXOn((const char*)0x20036);
            work->itemState = 201;
            mail = mailGetPtr(work->field_26E[work->mailSel * 2 + 1]);
            FontGetMessageWidthLine(msgSearch(mail->body), winItemMailLine(work));
            work->mailLines += 1;
            work->mailScroll = 0;
            if (mail->flags & 0x1000) {
                work->mailLines += 0xA;
            }
        } else if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            work->itemState = 10;
            work->field_0x248 = 320.f;
            work->field_0x250 = -240.f;
        } else if (work->dirRep & 0x1000) {
            old = work->mailSel;
            work->mailSel = old - 1;
            if (work->mailSel < 0) {
                work->mailSel = 0;
            }
            work->mailPage = work->mailSel / 5;
            work->mailScroll = 0;
            if (old != work->mailSel) {
                psndSFXOn((const char*)0x20036);
            }
        } else if (work->dirRep & 0x2000) {
            old = work->mailSel;
            work->mailSel = old + 1;
            if (work->mailSel >= work->mailCount) {
                work->mailSel = work->mailCount - 1;
            }
            work->mailPage = work->mailSel / 5;
            work->mailScroll = 0;
            if (old != work->mailSel) {
                psndSFXOn((const char*)0x20036);
            }
        } else if (work->buttonRep & 0x40) {
            old = work->mailPage;
            work->mailPage = old - 1;
            if (work->mailPage < 0) {
                work->mailPage = 0;
            }
            work->mailSel = work->mailPage * 5;
            if (old != work->mailPage) {
                psndSFXOn((const char*)0x20036);
            }
        } else if (work->buttonRep & 0x20) {
            page = work->mailCount;
            old = work->mailPage;
            work->mailPage = old + 1;
            page = (page + 4) / 5;
            if (work->mailPage >= page) {
                work->mailPage = page - 1;
            }
            work->mailSel = work->mailPage * 5;
            if (old != work->mailPage) {
                psndSFXOn((const char*)0x20036);
            }
        } else if (work->buttonTrg & 0x10) {
            winSortEntry(work, 7, work->field_0x248, work->field_0x250 - 60.f);
            work->savedState = work->itemState;
            psndSFXOn((const char*)0x20012);
            work->itemState = 1000;
        } else if (work->buttonTrg & 0x1000) {
            work->field_0x248 = 320.f;
            work->field_0x250 = -240.f;
            return -2;
        }
        work->targetX = -96.0f;
        work->targetY = 120.f - (f32)((work->mailSel % 5) * 0x28);
        break;

    case 201: {
        WinMailEntry* mail;

        mail = mailGetPtr(work->field_26E[work->mailSel * 2 + 1]);

        if (mail->flags & 0x1000) {
            work->mailTpl = (FileEntry*)fileAsyncf(FILE_TYPE_TPL, NULL, STR_MAIL_TPL, getMarioStDvdRoot());
            if ((u32)work->mailTpl != 0) {
                work->mailTpl = fileAllocf(FILE_TYPE_TPL, STR_MAIL_TPL, getMarioStDvdRoot());
                work->itemState += 1;
            }
        } else {
            work->mailTpl = NULL;
            work->itemState += 1;
        }
        break;
    }

    case 202:
        if ((u32)work->mailLines > 8) {
            if (keyGetDirRep(0) & 0x100) {
                if (work->mailScroll > 0) {
                    work->mailScroll -= 1;
                }
            } else if (keyGetDirRep(0) & 0x200) {
                if (work->mailScroll + 6 < (s32)work->mailLines) {
                    work->mailScroll += 1;
                }
            }
        }
        if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20036);
            if ((u32)work->mailTpl != 0) {
                fileFree(work->mailTpl);
            }
            work->itemState = 200;
        } else if (work->buttonTrg & 0x1000) {
            if ((u32)work->mailTpl != 0) {
                fileFree(work->mailTpl);
            }
            work->field_0x248 = 320.f;
            work->field_0x250 = -240.f;
            return -2;
        }
        work->targetX = -96.0f;
        work->targetY = 120.f;
        break;

    case 300:
        if (work->buttonTrg & 0x200) {
            psndSFXOn((const char*)0x20013);
            work->itemState = 10;
            work->useItemId = 0;
            winMgrClose(work->useTargetWin[0]);
            winMgrClose(work->useTargetWin[1]);
        } else if (work->buttonTrg & 0x100) {
            MarioWork* mario = marioGetPtr();
            s32 ok = 0;

            psndSFXOn((const char*)0x20012);
            if (mario->currMotionId == 0x19 || mario->currMotionId == 0x18) {
                ok = 1;
            }
            if (unk_8009fcbc() || U_marioSlitContinueChk()) {
                ok = 1;
            }
            if (work->useTargetSel > 0) {
                if (marioBgmodeChk()) {
                    ok = 1;
                }
            }
            if (work->useTargetSel > 1) {
                if (!marioBgmodeChk()) {
                    ok = 1;
                }
            }
            if (!ok) {
                winMgrClose(work->useTargetWin[0]);
                winMgrClose(work->useTargetWin[1]);
                return -2;
            }
            if (marioBgmodeChk() == 0) {
                leader = marioGetParty();
            } else {
                leader = marioGetPtr()->prevPartyMemberId[0];
            }
            n = 0;
            for (i = 0; i < 7; i++) {
                if (leader == winPartyDt[i].partnerId) {
                    list[n++] = &winPartyDt[i];
                }
            }
            for (i = 0; i < 7; i++) {
                if (leader != winPartyDt[i].partnerId) {
                    list[n++] = &winPartyDt[i];
                }
            }
            if (work->useTargetSel < 1) {
                pouchSetHP(pouchGetHP() + itemDataTable[work->useItemId].hp_restored);
                pouchSetFP(pouchGetFP() + itemDataTable[work->useItemId].fp_restored);
                pouchSetAP(pouchGetAP() + itemDataTable[work->useItemId].sp_restored);
            } else {
                PartyId pid = list[work->useTargetSel - 1]->partnerId;

                pouchSetPartyHP(pid, pouchGetPartyHP(pid) + itemDataTable[work->useItemId].hp_restored);
                pouchSetFP(pouchGetFP() + itemDataTable[work->useItemId].fp_restored);
                pouchSetAP(pouchGetAP() + itemDataTable[work->useItemId].sp_restored);
            }
            work->useTimer = 0;
            work->itemState = 301;
            break;
        } else if (work->dirRep & 0x1000) {
            old = work->useTargetSel;
            work->useTargetSel = old - 1;
            if (work->useTargetSel < 0) {
                work->useTargetSel = work->partyCount;
            }
            if (old != work->useTargetSel) {
                psndSFXOn((const char*)0x20005);
            }
        } else if (work->dirRep & 0x2000) {
            old = work->useTargetSel;
            work->useTargetSel = old + 1;
            if (work->useTargetSel >= work->partyCount + 1) {
                work->useTargetSel = 0;
            }
            if (old != work->useTargetSel) {
                psndSFXOn((const char*)0x20005);
            }
        } else if (work->buttonTrg & 0x1000) {
            work->useItemId = 0;
            winMgrClose(work->useTargetWin[0]);
            winMgrClose(work->useTargetWin[1]);
            return -2;
        }
        work->targetX = -80.f;
        {
            f32 f4;
            s32 t = work->useTargetSel;

            if (t == 0) {
                f4 = 0.0f;
            } else if (t == 1) {
                f4 = -16.0f;
            } else {
                f4 = -28.0f;
            }
            {
                f32 v = 130.f - 28.0f * (f32)t;

                work->targetY = v + f4;
            }
        }
        break;

    case 301:
        work->useTimer += 1;
        if (work->useTimer > gp->framerate * 2) {
            work->useTimer = 0;
            pouchRemoveItemIndex(work->useItemId, work->useItemSlot);
            work->itemState = 10;
            work->useItemId = 0;
            if (pouchGetHaveItemCnt() == 0) {
                work->itemState = 0;
            } else if (work->itemSel[0] >= pouchGetHaveItemCnt()) {
                work->itemSel[0] = pouchGetHaveItemCnt() - 1;
            }
            winMgrClose(work->useTargetWin[0]);
            winMgrClose(work->useTargetWin[1]);
        }
        break;

    case 1000:
        if (!winSortWait(work)) {
            work->itemState = work->savedState;
        }
        if (work->buttonTrg & 0x1000) {
            return -2;
        }
        break;

    default:
        break;
    }

    return 0;
}

void winItemMain2(WindowWork* work) {
    work->field_0x22C = 190.0f * (f32)work->itemScrollRow[0];
    work->field_0x224 += (work->field_0x22C - work->field_0x224) / 4.0f;
    work->field_0x230 = 190.0f * (f32)work->itemScrollRow[1];
    work->field_0x228 += (work->field_0x230 - work->field_0x228) / 4.0f;

    work->field_0x234 += (work->field_0x238 - work->field_0x234) / 6.0f;
    work->field_0x23C += (work->field_0x240 - work->field_0x23C) / 6.0f;
    work->field_0x244 += (work->field_0x248 - work->field_0x244) / 6.0f;
    work->field_0x24C += (work->field_0x250 - work->field_0x24C) / 6.0f;

    work->field_0x264 = (f32)(work->mailPage * 200);
    work->field_0x260 += (work->field_0x264 - work->field_0x260) / 4.0f;
}


// Draws the item window frame and the two tabs, then the list.
void winItemDisp(CameraId camId, void* param, s32 idx) {
    WindowWork* w = param;
    f32 c;
    f32 b;
    f32 a;
    f32 x = w->recs[idx].b;
    f32 y = w->recs[idx].c;
    GXColor color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    s32 i;

    winBgGX(w, 2, x, y);
    winKirinukiGX(w, 0, x - 150.f, 20.f + (120.f + y), 415.0f,
                  210.f);

    a = 105.0f + y;
    c = (x - 215.0f) - 4.0f;
    b = 4.0f + a;

    for (i = 0; i < 2; i++) {
        if (i == w->itemTab) {
            winTexInit(*(TEXPalette**)w->winTpl->data);
            winTexSet(0xB3, (Vec){x - 215.0f, 20.0f + (a - (f32)(i * 0x28)), 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, dat_8041d46c);
            winTexInit_x2(*(TEXPalette**)w->winTpl->data);
            winTexSet_x2(i + 0x18, 0xB3, (Vec){c, 20.0f + (b - (f32)(i * 0x28)), 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, color);
        } else {
            winTexInit_x2(*(TEXPalette**)w->winTpl->data);
            winTexSet_x2(i + 0x18, 0xB3, (Vec){x - 215.0f, 20.0f + (a - (f32)(i * 0x28)), 0.0f}, (Vec){1.0f, 1.0f, 1.0f},
                         dat_8041d470);
        }
    }

    item_disp(w, x, y);
}

// Draws the item list (two columns, scrolled per tab), the scroll arrows and the sort caption.
void item_disp(WindowWork* work, f32 x, f32 y) {
    GXColor iconColor;
    GXColor dark = dat_8041d478;
    GXColor gray = dat_8041d47c;
    GXColor white;
    f32 base;
    f32 drawY;
    f32 rowY;
    f32 scroll;
    s32 half;
    u8* entry;
    s32 grayFlag;
    s32 item;
    s32 parity;
    s32 i;
    s32 count;
    s32 colX;

    white = dat_8041d474;
    iconColor = dat_8041d474;
    if (work->itemTab == 0) {
        count = pouchGetHaveItemCnt();
    } else {
        count = work->skipCount;
    }

    GXSetScissor(0, 0x6D, 0x260, 0xC0);
    base = 100.0f + y;

    for (i = 0, entry = (u8*)work; i < count; entry += 2, i++) {
        parity = i % 2;
        half = i / 2;

        if (work->itemTab == 0) {
            item = pouchHaveItem(i);
        } else {
            item = *(s16*)(entry + 0x2E8);
        }
        scroll = (&work->field_0x224)[work->itemTab];

        grayFlag = ((work->itemTab != 0) || (itemDataTable[item].usable_locations & 4)) == 0;
        drawY = scroll + (20.0f + ((rowY = base - (f32)(half * 0x26)) - 10.0f));
        if (drawY > 240.0f) {
            continue;
        }
        if (drawY < -240.0f) {
            break;
        }

        if (grayFlag) {
            winIconGrayInit();
        } else {
            winIconInit();
        }
        winIconSetV(*(u16*)&itemDataTable[item].icon_id, (Vec){(x - 125.0f) + (f32)(colX = parity * 0xCC), drawY, 0.0f},
                    work->itemTab == 0 ? (Vec){0.8f, 0.8f, 0.8f} : (Vec){0.6666667f, 0.6666667f, 0.6666667f},
                    iconColor);
        winFontInit();
        winFontSetPitchV((Vec){(x - 105.0f) + (f32)colX, scroll + (20.0f + rowY), 0.0f},
                         (Vec){0.9f, 0.9f, 0.9f}, grayFlag ? gray : dark, 20.0f, msgSearch(itemDataTable[item].name));
    }

    GXSetScissor(0, 0, 0x260, 0x1E0);
    if (work->itemScrollRow[work->itemTab] > 0) {
        f32 arrowX;

        winTexInit(*(TEXPalette**)work->winTpl->data);
        arrowX = 250.0f + x;
        winTexSet(0x17, (Vec){arrowX, 20.0f + (18.0f + (110.0f + y)), 0.0f}, (Vec){-1.0f, -1.0f, -1.0f}, white);
        winIconInit();
        winIconSetV(0x7E, (Vec){arrowX, 20.0f + (113.0f + y), 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, white);
    }
    if (count > work->itemScrollRow[work->itemTab] * 10 + 10) {
        f32 arrowX;

        winTexInit(*(TEXPalette**)work->winTpl->data);
        arrowX = 250.0f + x;
        winTexSet(0x17, (Vec){arrowX, 20.0f + ((y - 90.0f) - 15.0f), 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, white);
        winIconInit();
        winIconSetV(0x80, (Vec){arrowX, 20.0f + (y - 90.0f), 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, white);
    }
    {
        f32 w = 0.7f * (f32)FontGetMessageWidth(msgSearch("msg_menu_sort_narabikae"));
        f32 sortX;

        winFontInit();
        sortX = x + (220.0f - w);
        winFontSetV((Vec){sortX, 20.0f + ((y - 92.0f) - 10.0f), 0.0f}, (Vec){0.7f, 0.7f, 0.7f}, dark,
                    msgSearch("msg_menu_sort_narabikae"));
        winIconInit();
        winIconSetV(0x217, (Vec){sortX - 30.0f, 20.0f + ((y - 100.0f) - 10.0f), 0.0f}, (Vec){0.6f, 0.6f, 0.6f}, white);
    }

    if (work->field_0x234 < 320.0f && work->field_0x23C > -240.0f) {
        winBookGX(work, 0, work->field_0x234, work->field_0x23C);
    }
}

// Draws the use-item target window: Mario's HP/FP rows, then one HP row per partner (the current
// partner first), and the window's background textures.
void itemUseDisp(WinMgrEntry* entry) {
    WindowWork* work = winGetPtr();
    WindowPartyDt* list[10];
    WindowPartyDt* party;
    GXColor color = dat_8041d480;
    GXColor iconColor;
    GXColor white;
    s32 i;
    s32 sel2e0;
    f32 f31;
    const char* name;
    s32 n;
    s32 gray;
    s32 n2;

    white = dat_8041d484;
    iconColor = dat_8041d484;
    if (!winMgrAction(work->useTargetWin[0])) {
        s32 partyId;

        partyId = !marioBgmodeChk() ? marioGetParty() : marioGetPtr()->prevPartyMemberId[0];

        party = winPartyDt;
        n = 0;
        for (i = 0; i < 7; i++) {
            if (party[i].partnerId == partyId) {
                list[n++] = &party[i];
            }
        }
        for (i = 0; i < 7; i++) {
            if (party[i].partnerId != partyId) {
                list[n++] = &party[i];
            }
        }

        for (i = 0; i < work->partyCount + 1; i++) {
            if (i == 0) {
                f31 = 0.0f;
            } else if (i == 1) {
                f31 = -16.0f;
            } else {
                f31 = -28.0f;
            }

            sel2e0 = work->useTimer;
            gray = 0;
            if (sel2e0 != 0) {
                if (i == work->useTargetSel) {
                    color = dat_8041d488;
                } else {
                    color = dat_8041d48c;
                    gray = 1;
                }
            }

            if (i == 0) {
                if (sel2e0 % 20 > 15 && i == work->useTargetSel) {
                    continue;
                }
                if (gray) {
                    winIconGrayInit();
                } else {
                    winIconInit();
                }
                winIconSetV(0x19E, (Vec){(f32)(entry->x + 0x1E), (f32)(entry->y - 0x14 - i * 0x16), 0.0f},
                            (Vec){0.65f, 0.65f, 0.65f}, iconColor);
                winTexInit(*(TEXPalette**)work->winTpl->data);
                GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
                GXSetTevAlphaIn(0, 7, 1, 4, 7);
                winTexSet(0xAF,
                          (Vec){0.9f * (f32)FontGetMessageWidth("\202g\202o") / 2 + (f32)(entry->x + 0xDC),
                                (f32)(entry->y - 0x13 - i * 0x16), 0.0f},
                          (Vec){1.0f, 1.0f, 1.0f}, dat_8041d490);
                winTexSet(0xAF,
                          (Vec){0.9f * (f32)FontGetMessageWidth("\202g\202o") / 2 + (f32)(entry->x + 0xDC),
                                (f32)(entry->y - 0x13 - (i + 1) * 0x16), 0.0f},
                          (Vec){1.0f, 1.0f, 1.0f}, dat_8041d494);
                winFontInit();
                winFontSetV((Vec){(f32)(entry->x + 0x32), (f32)(entry->y - 8 - i * 0x16), 0.0f},
                            (Vec){1.0f, 1.0f, 1.0f}, color, msgSearch("name_mario"));
                winFontSetV((Vec){(f32)(entry->x + 0xDC), (f32)(entry->y - 8 - i * 0x16), 0.0f},
                            (Vec){0.9f, 0.9f, 0.9f}, color, "\202g\202o");
                winFontSetRV((Vec){(f32)(entry->x + 0xF5), (f32)(entry->y - 8 - i * 0x16), 0.0f},
                             (Vec){0.9f, 0.9f, 0.9f}, color, "%d", pouchGetHP());
                winFontSetRV((Vec){(f32)(entry->x + 0x10B), (f32)(entry->y - 8 - i * 0x16), 0.0f},
                             (Vec){0.8f, 0.8f, 0.8f}, color, "\201^");
                winFontSetV((Vec){(f32)(entry->x + 0x136), (f32)(entry->y - 0xA - i * 0x16), 0.0f},
                            (Vec){0.8f, 0.8f, 0.8f}, color, "%d", pouchGetMaxHP());
                if (itemDataTable[work->useItemId].fp_restored != 0) {
                    if (work->useTimer % 20 > 15) {
                        continue;
                    }
                    color = dat_8041d498;
                }
                winFontInit();
                winFontSetV((Vec){(f32)(entry->x + 0xDC), (f32)(entry->y - 8 - (i + 1) * 0x16), 0.0f},
                            (Vec){0.9f, 0.9f, 0.9f}, color, "\202e\202o");
                winFontSetRV((Vec){(f32)(entry->x + 0xF5), (f32)(entry->y - 8 - (i + 1) * 0x16), 0.0f},
                             (Vec){0.9f, 0.9f, 0.9f}, color, "%d", pouchGetFP());
                winFontSetRV((Vec){(f32)(entry->x + 0x10B), (f32)(entry->y - 8 - (i + 1) * 0x16), 0.0f},
                             (Vec){0.8f, 0.8f, 0.8f}, color, "\201^");
                winFontSetV((Vec){(f32)(entry->x + 0x136), (f32)(entry->y - 0xA - (i + 1) * 0x16), 0.0f},
                            (Vec){0.8f, 0.8f, 0.8f}, color, "%d", pouchGetMaxFP());
            } else {
                name = list[i - 1]->partnerId != 4 ? msgSearch(list[i - 1]->nameMsg) : pouchGetYoshiName();
                if (work->useTimer % 20 > 15 && i == work->useTargetSel) {
                    continue;
                }
                winTexInit(*(TEXPalette**)work->winTpl->data);
                GXSetTevColorIn(0, 0xF, 0xF, 0xF, 2);
                GXSetTevAlphaIn(0, 7, 1, 4, 7);
                winTexSet(0xAF,
                          (Vec){0.9f * (f32)FontGetMessageWidth("\202g\202o") / 2 + (f32)(entry->x + 0xDC),
                                f31 + (f32)(entry->y - 8 - i * 0x1C) - 14.0f, 0.0f},
                          (Vec){1.0f, 1.0f, 1.0f}, dat_8041d49c);
                if (gray) {
                    winIconGrayInit();
                } else {
                    winIconInit();
                }
                winIconSetV(list[i - 1]->iconId,
                            (Vec){(f32)(entry->x + 0x1E), f31 + (f32)(entry->y - 0x14 - i * 0x1C), 0.0f},
                            (Vec){0.65f, 0.65f, 0.65f}, iconColor);
                winFontInit();
                winFontSetV((Vec){(f32)(entry->x + 0x32), f31 + (f32)(entry->y - 8 - i * 0x1C), 0.0f},
                            (Vec){(f32)FontGetMessageWidth(name) < 160.0f ? 1.0f : 160.0f / (f32)FontGetMessageWidth(name),
                                  1.0f, 1.0f},
                            color, name);
                winFontSetV((Vec){(f32)(entry->x + 0xDC), f31 + (f32)(entry->y - 8 - i * 0x1C), 0.0f},
                            (Vec){0.9f, 0.9f, 0.9f}, color, "\202g\202o");
                winFontSetRV((Vec){(f32)(entry->x + 0xF5), f31 + (f32)(entry->y - 8 - i * 0x1C), 0.0f},
                             (Vec){0.9f, 0.9f, 0.9f}, color, "%d", pouchGetPtr()->partyData[list[i - 1]->partnerId].currentHP);
                winFontSetRV((Vec){(f32)(entry->x + 0x10B), f31 + (f32)(entry->y - 8 - i * 0x1C), 0.0f},
                             (Vec){0.8f, 0.8f, 0.8f}, color, "\201^");
                winFontSetV((Vec){(f32)(entry->x + 0x136), f31 + (f32)(entry->y - 8 - i * 0x1C) - 2.0f, 0.0f},
                            (Vec){0.8f, 0.8f, 0.8f}, color, "%d", pouchGetPtr()->partyData[list[i - 1]->partnerId].maximumHP);
            }
        }

        if (work->partyCount > 1) {
            winIconInit();
            for (n2 = 2; n2 < 0x1E; n2++) {
                winIconSetV(0x1B7, (Vec){(f32)(entry->x + (entry->width / 31) * n2), (f32)(entry->y - 0x54), 0.0f},
                            (Vec){0.15f, 0.15f, 0.15f}, color);
            }
        }
    }
    winTexInit(*(TEXPalette**)work->winTpl->data);
    winTexSet(0, (Vec){2.0f + work->posX, work->posY - 2.0f, 0.0f}, (Vec){1.0f, 1.0f, 1.0f},
              dat_8041d4a0);
    winTexSet(0, (Vec){work->posX, work->posY, 0.0f}, (Vec){1.0f, 1.0f, 1.0f}, white);
}

void itemUseDisp2(WinMgrEntry* entry) {
    WindowWork* work = winGetPtr();
    s32 id = work->useTargetWin[1];
    const char* msg;
    f32 width;
    GXColor color;

    color = dat_8041d4a4;
    if (!winMgrAction(id)) {
        msg = msgSearch("msg_window_select_5");
        width = (f32)FontGetMessageWidth(msg);
        winFontInit();
        winFontSetV((Vec){(f32)entry->x + ((f32)entry->width - width) / 2, 134.0f, 0.0f},
                    (Vec){1.0f, 1.0f, 1.0f}, color, msg);
    }
}

// The Luigi book page-pointer tables (.data). Each book lists the 12 page strings plus a
// terminating NULL; luigi_book_list (.sdata) points at them in order.
const char* luigi_book_1[13] = {
    luigi_book_1_page1, luigi_book_1_page2, luigi_book_1_page3, luigi_book_1_page4, luigi_book_1_page5, luigi_book_1_page6,
    luigi_book_1_page7, luigi_book_1_page8, luigi_book_1_page9, luigi_book_1_page10, luigi_book_1_page11, luigi_book_1_page12, NULL,
};

const char* luigi_book_2[13] = {
    luigi_book_2_page1, luigi_book_2_page2, luigi_book_2_page3, luigi_book_2_page4, luigi_book_2_page5, luigi_book_2_page6,
    luigi_book_2_page7, luigi_book_2_page8, luigi_book_2_page9, luigi_book_2_page10, luigi_book_2_page11, luigi_book_2_page12, NULL,
};

const char* luigi_book_3[13] = {
    luigi_book_3_page1, luigi_book_3_page2, luigi_book_3_page3, luigi_book_3_page4, luigi_book_3_page5, luigi_book_3_page6,
    luigi_book_3_page7, luigi_book_3_page8, luigi_book_3_page9, luigi_book_3_page10, luigi_book_3_page11, luigi_book_3_page12, NULL,
};

const char* luigi_book_4[13] = {
    luigi_book_4_page1, luigi_book_4_page2, luigi_book_4_page3, luigi_book_4_page4, luigi_book_4_page5, luigi_book_4_page6,
    luigi_book_4_page7, luigi_book_4_page8, luigi_book_4_page9, luigi_book_4_page10, luigi_book_4_page11, luigi_book_4_page12, NULL,
};

const char* luigi_book_5[13] = {
    luigi_book_5_page1, luigi_book_5_page2, luigi_book_5_page3, luigi_book_5_page4, luigi_book_5_page5, luigi_book_5_page6,
    luigi_book_5_page7, luigi_book_5_page8, luigi_book_5_page9, luigi_book_5_page10, luigi_book_5_page11, luigi_book_5_page12, NULL,
};

static WinMgrDesc window_desc[2] = {
    {
        0, 0,
        8, // camId
        -0x46,
        0x96,
        0x15E,
        0x108,
        {0xF0, 0xF0, 0xF0, 0xFF},
        NULL,
        (void*)itemUseDisp,
    },
    {
        2, 0,
        8, // camId
        -0x118,
        0x91,
        0xC8,
        0x32,
        {0xFF, 0x99, 0x00, 0xFF},
        NULL,
        (void*)itemUseDisp2,
    },
};

// The unit's .sdata/.sdata2 objects and the .data tail. They are defined after the code so
// MWCC emits real objects instead of folding their values into the functions that read them.
#pragma explicit_zero_data on
#pragma force_active on

// .data:0x154 | 0x80374D0C | size 0x4
__declspec(section ".data") u32 gap_05_80374D0C_data[1] = { 0 };

// .sdata:0x0 | 0x804120D0 | size 0x14
const char** luigi_book_list[5] = {
    luigi_book_1,
    luigi_book_2,
    luigi_book_3,
    luigi_book_4,
    luigi_book_5,
};

// .sdata:0x14 | 0x804120E4 | size 0x24
u16 menu_skip_list[18] = {
    0x0009, 0x000A, 0x000B, 0x0006, 0x0007, 0x0008, 0x0005, 0x0003, 0x0002,
    0x0004, 0x0032, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078,
};

// .sdata2:0x4 | 0x8041D46C | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d46c = { 0x00, 0x00, 0x00, 0x80 };

// .sdata2:0x8 | 0x8041D470 | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d470 = { 0x80, 0x80, 0x80, 0xFF };

// .sdata2:0xC | 0x8041D474 | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d474 = { 0xFF, 0xFF, 0xFF, 0xFF };

// .sdata2:0x10 | 0x8041D478 | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d478 = { 0x00, 0x00, 0x00, 0xFF };

// .sdata2:0x14 | 0x8041D47C | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d47c = { 0x80, 0x80, 0x80, 0xFF };

// .sdata2:0x18 | 0x8041D480 | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d480 = { 0x00, 0x00, 0x00, 0xFF };

// .sdata2:0x1C | 0x8041D484 | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d484 = { 0xFF, 0xFF, 0xFF, 0xFF };

// .sdata2:0x20 | 0x8041D488 | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d488 = { 0x00, 0x00, 0x00, 0xFF };

// .sdata2:0x24 | 0x8041D48C | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d48c = { 0x80, 0x80, 0x80, 0xFF };

// .sdata2:0x28 | 0x8041D490 | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d490 = { 0xCB, 0x8C, 0xA2, 0xFE };

// .sdata2:0x2C | 0x8041D494 | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d494 = { 0xCB, 0xBE, 0x89, 0xFE };

// .sdata2:0x30 | 0x8041D498 | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d498 = { 0x00, 0x00, 0x00, 0xFF };

// .sdata2:0x34 | 0x8041D49C | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d49c = { 0xCB, 0x8C, 0xA2, 0xF4 };

// .sdata2:0x38 | 0x8041D4A0 | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d4a0 = { 0x00, 0x00, 0x00, 0x80 };

// .sdata2:0x3C | 0x8041D4A4 | size 0x4
__declspec(section ".sdata2") const GXColor dat_8041d4a4 = { 0x00, 0x00, 0x00, 0xFF };

#pragma force_active reset
#pragma explicit_zero_data reset

