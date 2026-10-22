#include "evt_yuuranking.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "manager/fontmgr.h"
void FontDrawColor(GXColor color);
#include "driver/msgdrv.h"
#include "memory.h"
#include "system.h"
#include "pmario_sound.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/windowdrv.h"
#include "evt/evt_yuugijou.h"
#include <dolphin/mtx.h>
#include <string.h>

extern const f32 float_1000_gor_0001cd80;

typedef struct YuRankWork {
    EventEntry* eventEntry; // 0x00
    u16 freeFlags;    // 0x04
    u16 pad_06;       // 0x06
    u32 flags2;       // 0x08
    f32 x;            // 0x0C
    f32 field_10;     // 0x10 y origin
    s32 field_14;     // 0x14 minigame mode
    s32 displayMode;  // 0x18
    s32 result;       // 0x1C
    s32 bonus;        // 0x20
    s32 mult;         // 0x24
    s32 rankIndex;    // 0x28
    s32 maxValue;     // 0x2C
    s32 state;        // 0x30
    s32 monte;        // 0x34
    s32 currentValue; // 0x38
    s32 timer;        // 0x3C
} YuRankWork;

#pragma pool_data off

#pragma force_active on

const u32 dat_gor_0001cc40 = 0xFFFFFFFF;

static USER_FUNC(yuuranking_get_monte);
static s32 yuu_kesan(s32 mode, s32 value, s32 bonus, s32 mult, s32 maxValue);
static inline s32 yuu_kesan_chk(s32 mode, s32 value, s32 bonus, s32 mult, s32 maxValue) {
    if (value < 0) {
        return 0;
    }
    return yuu_kesan(mode, value, bonus, mult, maxValue);
}

static USER_FUNC(evt_yuuranking_main_func);
static void evt_yuuranking_disp_func(CameraId cameraId, void* param);
static s32 fontlenpitch(const s8* msg, s32 pitch);

// clang-format off
extend_data(52,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(53,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

#include "rel/common/fontlenpitch.h"

void gor_draw_msg_fit(const char* msg, s32 width, f32 x, f32 y, f32 z) {
    Mtx trans;
    Mtx scale;
    Mtx out;
    f32 scaleX;

    PSMTXTrans(trans, x, y, 0.0f);
    if (FontGetMessageWidth(msg) > width) {
        scaleX = (f32)width * z / (f32)FontGetMessageWidth(msg);
    } else {
        scaleX = z;
    }
    PSMTXScale(scale, scaleX, z, z);
    PSMTXConcat(trans, scale, out);
    FontDrawMessageMtx(out, msg);
}

#define A14(i) ((s32)yuwp->rankUsed[work->field_14][i])
#define A74(i) ((s32)yuwp->rankScore[work->field_14][i])
#define AD4(i) ((s32)yuwp->rankMonte[work->field_14][i])



// History: block order, the nested mode switches, the callee-saved set (stmw r24-r31) and the
// call sequence all match. Adopting the yuu source's `k = 1.0f` forms (wakuWidth = k*w + 50.0f,
// x = work->x + -(k*w + 50.0f)*0.5f, y = work->field_10 + 128.0f + k, FontDrawMessage x = k*(-w)*0.5f) reproduces
// the target's fnmadds/fmadds and lifted this function 96.67 -> 97.03 (the earlier note that they lowered the
// score was measured against a misaligned prefix). The rank-list `msg[0]=='m'...` chain is `cmpwi` in the
// target; `(s8)` casts give cmpwi and 97.03 -> 97.08. Reloading A74(i) in the rank
// case-0 sprintf instead of reusing the `rowValue` temp lifted 97.38 -> 97.58. Declaring the case-2/3 minute/second
// temps `min` before `sec` (instead of reusing `row`/`j`) lifted 97.58 -> 97.80, and nesting the top-level
// case-2/3 first `msgSearch("menu_colon")` in the sprintf (keeping the same two calls) lifted it to 97.82.
// Writing the minute/second remainder as `sec = sec % 60` instead of `sec - min * 60` makes MWCC emit the
// target's `mullw` (reusing the 0x3c already in r0) in both case-2/3 blocks and lifted 97.82 -> 97.87.
// Retail evaluates the title frame's lower edge as k + (field_10 + 128), in that operand order.
#define WAKU_BOTTOM (k + (work->field_10 + 128.0f))

// The top waku/FontDrawMessage maths (a non-const `k=1.0f` keeps the `fnmadds` that negates the sum, integer-negated
// width), the signed `cmpwi` menu test and the `A74(i)/1000` recompute now match. Nesting the case-0
// `menu_decimal_point` and the rank `menu_subtitle5_rank`/`menu_subtitle1_monte` msgSearch results into their
// calls emits the format string and the 80.0f/75.2f literals in the target's `.rodata` order.
// The title, the case-2/3 subtitles (`msg2`, shared by the three switches) and the other messages (`msg`) are
// three locals, and the rank case-2/3 `rankValue` is the function-level one, declared between `title` and `msg`: that
// order colours title/rankValue r28 and msg2 r27. The title frame takes FontGetMessageWidth straight into
// `-(k * width + 50.0f) / 2.0f` and `k * -width / 2.0f`; both rank time blocks read `rankValue` before testing it, and
// the top and rank case-2/3 `menu_colon` lookups are both nested in the sprintf. FontDrawStringPitch takes `buf` itself: CSE keeps
// its address in a register across the inlined fontlenpitch, whose index walk loop strength reduction rebuilds
// from `buf`, giving the target's two separate `addi rN,r1,0x78`.
static void evt_yuuranking_disp_func(CameraId cameraId, void* param) {
    YuRankWork* work = param;
    const char* titleTbl[4] = {
        "menu_title_minigame1", "menu_title_minigame4", "menu_title_minigame3", "menu_title_minigame2",
    };
    const char* scoreTbl[4] = {
        "menu_title_score", "menu_title_score", "menu_title_score", "menu_title_score",
    };
    const char* subTbl[6] = {
        "\x81\x40\x82\x50", "\x81\x40\x82\x51", "\x81\x40\x82\x52",
        "\x81\x40\x82\x53", "\x81\x40\x82\x54", "menu_subtitle2_rankgai",
    };
    char buf[32];
    const char* title;
    s32 rankValue;
    const char* msg;
    const char* msg2;
    s32 row;
    s32 j;
    f32 rowY;

    windowDispGX_Waku_col(work->x + -196.0f, work->field_10 + 112.0f, 392.0f, 224.0f, 20.0f, 0,
                          (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    switch (work->displayMode) {
    case 0:
        title = msgSearch(scoreTbl[work->field_14]);
        break;
    case 1:
    case 2:
        title = msgSearch(titleTbl[work->field_14]);
        break;
    }
    {
        f32 k = 1.0f;
        windowDispGX_Waku_col(work->x + -(k * FontGetMessageWidth(title) + 50.0f) / 2.0f, WAKU_BOTTOM,
                              k * FontGetMessageWidth(title) + 50.0f, 28.0f, 8.0f, 0, (GXColor){ 0x2B, 0xA3, 0xFF, 0xFF });
        FontDrawStart();
        FontDrawScale(1.0f);
        FontDrawColor((GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
        FontDrawMessage((s32)(work->x + k * -FontGetMessageWidth(title) / 2.0f), (s32)(work->field_10 + 128.0f), title);
    }

    if (work->displayMode == 0) {
        switch (work->field_14) {
        case 0:
            FontDrawStart();
            FontDrawScale(1.0f);
            FontDrawColor((GXColor){ 0x00, 0x00, 0x00, 0xFF });
            msg = msgSearch("menu_subtitle1_minigame1");
            gor_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 56.0f, 1.0f);
            if (work->currentValue >= 0) {
                sprintf(buf, "%3d%s%02d", work->currentValue / 1000, msgSearch("menu_decimal_point"), (work->currentValue % 1000) / 10);
                FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
                if (work->state >= 4) {
                    FontDrawStart();
                    FontDrawScale(1.0f);
                    FontDrawColor((GXColor){ 0x00, 0x00, 0x00, 0xFF });
                    msg = msgSearch("menu_subtitle4_getmonte");
                    gor_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 32.0f, 1.0f);
                    sprintf(buf, "%4d", work->bonus);
                    FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 32.0f, 18.0f);
                }
                if (work->state >= 7) {
                    FontDrawStart();
                    FontDrawScale(1.0f);
                    FontDrawColor((GXColor){ 0x00, 0x00, 0x00, 0xFF });
                    if (work->mult > 1) {
                        msg = msgSearch("menu_subtitle2_minigame1");
                        gor_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 8.0f, 1.0f);
                        if (work->mult < 0x100) {
                            msg = msgSearch("menu_subtitle4_minigame1");
                            sprintf(buf, msg, work->mult);
                        } else {
                            sprintf(buf, "%2d", work->mult - 0x100);
                        }
                        FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    } else if (work->mult < 0) {
                        msg = msgSearch("menu_subtitle5_minigame1");
                        gor_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 8.0f, 1.0f);
                        msg = msgSearch("menu_subtitle6_minigame1");
                        sprintf(buf, msg, -work->mult);
                        FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    }
                }
            } else {
                FontDrawColor((GXColor){ 0xFF, 0x3F, 0x3F, 0xFF });
                msg = msgSearch("menu_error_minigame1");
                sprintf(buf, msg);
                FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
            }
            break;

        case 2:
        case 3:
            FontDrawStart();
            FontDrawColor((GXColor){ 0x00, 0x00, 0x00, 0xFF });
            switch (work->field_14) {
            case 3:
                msg2 = msgSearch("menu_subtitle1_minigame2");
                break;
            case 2:
                msg2 = msgSearch("menu_subtitle1_minigame3");
                break;
            }
            gor_draw_msg_fit(msg2, 165, work->x + -150.4f, work->field_10 + 56.0f, 1.0f);
            FontDrawScale(1.0f);
            if (work->currentValue >= 0) {
                s32 timeValue = work->currentValue;
                s32 min;
                s32 sec;

                sec = timeValue / 1000;
                min = sec / 60;
                sec = sec % 60;
                sprintf(buf, "%1d%s%02d%s%02d", min, msgSearch("menu_colon"), sec, msgSearch("menu_colon"), (timeValue % 1000) / 10);
                FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
                if (work->state >= 4) {
                    FontDrawStart();
                    FontDrawColor((GXColor){ 0x00, 0x00, 0x00, 0xFF });
                    switch (work->field_14) {
                    case 3:
                        msg2 = msgSearch("menu_subtitle4_getmonte");
                        break;
                    case 2:
                        msg2 = msgSearch("menu_subtitle4_getmonte");
                        break;
                    }
                    gor_draw_msg_fit(msg2, 165, work->x + -150.4f, work->field_10 + 32.0f, 1.0f);
                    FontDrawScale(1.0f);
                    sprintf(buf, "%4d", work->bonus);
                    FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 32.0f, 18.0f);
                }
                if (work->state >= 7) {
                    FontDrawStart();
                    FontDrawColor((GXColor){ 0x00, 0x00, 0x00, 0xFF });
                    if (work->mult > 1) {
                        switch (work->field_14) {
                        case 3:
                            msg2 = msgSearch("menu_subtitle2_minigame2");
                            break;
                        case 2:
                            msg2 = msgSearch("menu_subtitle2_minigame3");
                            break;
                        }
                        gor_draw_msg_fit(msg2, 165, work->x + -150.4f, work->field_10 + 8.0f, 1.0f);
                        FontDrawScale(1.0f);
                        if (work->mult < 0x100) {
                            msg = msgSearch("menu_subtitle4_minigame1");
                            sprintf(buf, msg, work->mult);
                        } else {
                            sprintf(buf, "%2d", work->mult - 0x100);
                        }
                        FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    } else if (work->mult < 0) {
                        msg = msgSearch("menu_subtitle5_minigame1");
                        gor_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 8.0f, 1.0f);
                        FontDrawScale(1.0f);
                        msg = msgSearch("menu_subtitle6_minigame1");
                        sprintf(buf, msg, -work->mult);
                        FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    }
                }
            } else {
                FontDrawColor((GXColor){ 0xFF, 0x3F, 0x3F, 0xFF });
                if (work->field_14 == 3) {
                    sprintf(buf, msgSearch("menu_error_minigame2"));
                } else {
                    sprintf(buf, msgSearch("menu_error_minigame3"));
                }
                FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
            }
            break;

        case 1:
            FontDrawStart();
            FontDrawScale(1.0f);
            FontDrawColor((GXColor){ 0x00, 0x00, 0x00, 0xFF });
            msg = msgSearch("menu_subtitle1_minigame4");
            gor_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 56.0f, 1.0f);
            if (work->currentValue < work->maxValue) {
                sprintf(buf, "%4d", work->currentValue + 1);
                FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
                if (work->state >= 7) {
                    FontDrawStart();
                    FontDrawScale(1.0f);
                    FontDrawColor((GXColor){ 0x00, 0x00, 0x00, 0xFF });
                    if (work->mult > 1) {
                        msg = msgSearch("menu_subtitle2_minigame4");
                        gor_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 8.0f, 1.0f);
                        if (work->mult < 0x100) {
                            msg = msgSearch("menu_subtitle4_minigame1");
                            sprintf(buf, msg, work->mult);
                        } else {
                            sprintf(buf, "%2d", work->mult - 0x100);
                        }
                        FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    } else if (work->mult < 0) {
                        msg = msgSearch("menu_subtitle5_minigame1");
                        gor_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 8.0f, 1.0f);
                        msg = msgSearch("menu_subtitle6_minigame1");
                        sprintf(buf, msg, -work->mult);
                        FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    }
                }
            } else {
                FontDrawColor((GXColor){ 0xFF, 0x3F, 0x3F, 0xFF });
                sprintf(buf, msgSearch("menu_error_minigame4"));
                FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
            }
            break;

        default:
            break;
        }

        FontDrawStart();
        FontDrawScale(1.0f);
        FontDrawColor((GXColor){ 0x00, 0x00, 0x00, 0xFF });
        msg = msgSearch("menu_subtitle3_totalmonte");
        gor_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + -40.0f, 1.0f);
        {
            Mtx m;
            s32 ix = (s32)(work->x + 10.399994f + 70.0f);
            s32 iy = (s32)(work->field_10 + -40.0f - 24.0f);

            PSMTXTrans(m, (f32)(ix - 10), (f32)(iy + 2), 0.0f);
            iconDispGxCol(m, 16, 0x13f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
            PSMTXTrans(m, (f32)(ix + 60), (f32)(iy + 2), 0.0f);
            iconNumberDispGx(m, work->monte, 0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
        }
    } else {
        FontDrawStart();
        FontDrawScale(1.0f);
        FontDrawMessage((s32)(work->x + -150.4f), (s32)(work->field_10 + 80.0f), msgSearch("menu_subtitle5_rank"));
        FontDrawMessage((s32)(work->x + 75.19998f), (s32)(work->field_10 + 80.0f), msgSearch("menu_subtitle1_monte"));
        FontDrawColor((GXColor){ 0x00, 0x00, 0x00, 0xFF });
        switch (work->field_14) {
        case 0:
            msg = msgSearch("menu_subtitle1_minigame1");
            FontDrawMessage((s32)(work->x + -75.200005f), (s32)(work->field_10 + 80.0f), msg);
            break;
        case 3:
            msg = msgSearch("menu_subtitle1_minigame2");
            FontDrawMessage((s32)(work->x + -75.200005f), (s32)(work->field_10 + 80.0f), msg);
            break;
        case 2:
            msg = msgSearch("menu_subtitle1_minigame3");
            FontDrawMessage((s32)(work->x + -75.200005f), (s32)(work->field_10 + 80.0f), msg);
            break;
        case 1:
            msg = msgSearch("menu_subtitle1_minigame4");
            FontDrawMessage((s32)(work->x + -75.200005f), (s32)(work->field_10 + 80.0f), msg);
            break;
        }

        for (row = 0; row < 6; row++) {
            if (A14(row) == 0) {
                continue;
            }
            rowY = 2.0f + (f32)row;
            if (row == 5) {
                rowY += 0.5f;
            }
            FontDrawStart();
            FontDrawScale(1.0f);
            if (row != work->rankIndex) {
                FontDrawColor((GXColor){ 0x00, 0x00, 0x00, 0xFF });
            } else {
                FontDrawColor((GXColor){ 0xFF, 0x3F, 0x3F, 0xFF });
            }
            msg = subTbl[row];
            if ((s8)msg[0] == 'm' && (s8)msg[1] == 'e' && (s8)msg[2] == 'n' && (s8)msg[3] == 'u' && (s8)msg[4] == '_') {
                msg = msgSearch(msg);
            }
            if (row == 5) {
                FontDrawMessage((s32)(work->x + -188.0f + 18.8f),
                                (s32)(work->field_10 + (104.0f - 24.0f * rowY)), msg);
            } else {
                FontDrawStringPitch(msg, work->x + -150.4f, work->field_10 + (104.0f - 24.0f * rowY), 18.0f);
            }
            switch (work->field_14) {
            case 0: {
                s32 rowValue = A74(row);

                if (rowValue >= 0) {
                    sprintf(buf, "%3d%s%02d", A74(row) / 1000, msgSearch("menu_decimal_point"), (rowValue % 1000) / 10);
                    FontDrawStringPitch(buf, work->x + -75.200005f, work->field_10 + (104.0f - 24.0f * rowY), 18.0f);
                    sprintf(buf, "%4d", AD4(row));
                    FontDrawStringPitch(buf, work->x + 75.19998f, work->field_10 + (104.0f - 24.0f * rowY), 18.0f);
                } else {
                    sprintf(buf, msgSearch("menu_error_minigame1"));
                    FontDrawMessage((s32)(work->x + -75.200005f),
                                    (s32)(work->field_10 + (104.0f - 24.0f * rowY)), buf);
                }
                break;
            }
            case 2:
            case 3: {
                s32 min;
                s32 sec;

                rankValue = A74(row);
                if (rankValue >= 0) {
                    sec = rankValue / 1000;
                    min = sec / 60;
                    sec = sec % 60;
                    sprintf(buf, "%1d%s%02d%s%02d", min, msgSearch("menu_colon"), sec, msgSearch("menu_colon"), (rankValue % 1000) / 10);
                    FontDrawStringPitch(buf, work->x + -75.200005f, work->field_10 + (104.0f - 24.0f * rowY), 18.0f);
                    sprintf(buf, "%4d", AD4(row));
                    FontDrawStringPitch(buf, work->x + 75.19998f, work->field_10 + (104.0f - 24.0f * rowY), 18.0f);
                } else {
                    if (work->field_14 == 3) {
                        sprintf(buf, msgSearch("menu_error_minigame2"));
                    } else {
                        sprintf(buf, msgSearch("menu_error_minigame3"));
                    }
                    FontDrawMessage((s32)(work->x + -75.200005f),
                                    (s32)(work->field_10 + (104.0f - 24.0f * rowY)), buf);
                }
            }
            break;
            case 1:
                if (A74(row) < work->maxValue) {
                    sprintf(buf, "%4d", A74(row) + 1);
                    FontDrawStringPitch(buf, work->x + -37.600006f, work->field_10 + (104.0f - 24.0f * rowY), 18.0f);
                    sprintf(buf, "%4d", AD4(row));
                    FontDrawStringPitch(buf, work->x + 75.19998f, work->field_10 + (104.0f - 24.0f * rowY), 18.0f);
                } else {
                    sprintf(buf, msgSearch("menu_error_minigame4"));
                    FontDrawMessage((s32)(work->x + -75.200005f),
                                    (s32)(work->field_10 + (104.0f - 24.0f * rowY)), buf);
                }
            break;
            }
        }
    }
}
#undef AD4
#undef A74
#undef A14


USER_FUNC(evt_yuuranking_main_func) {
    static s32 snd_id;
    YuRankWork* wp = (YuRankWork*)event->lwData[0];
    s32 targetMonte;

    if (wp->freeFlags & 1) {
        wp->freeFlags |= 2;
        return 2;
    }
    dispEntry(8, float_1000_gor_0001cd80, 0, evt_yuuranking_disp_func, wp);
    if (wp->displayMode == 0) {
        switch (wp->state) {
        case 0:
            wp->timer = 30;
            wp->state++;
            break;
        case 1:
            if (wp->timer != 0) {
                wp->timer--;
                break;
            }
            wp->state++;
            if (wp->result != 0) {
                snd_id = psndSFXOn((const char*)0x8d7);
            }
            break;
        case 2:
            switch (wp->field_14) {
                case 0:
                    wp->currentValue += 5590;
                    if (wp->currentValue > wp->result) {
                        wp->currentValue = wp->result;
                    }
                    break;
                case 3:
                    wp->currentValue += 1090;
                    if (wp->currentValue > wp->result) {
                        wp->currentValue = wp->result;
                    }
                    break;
                case 2:
                    wp->currentValue += 1090;
                    if (wp->currentValue > wp->result) {
                        wp->currentValue = wp->result;
                    }
                    break;
                case 1:
                    wp->currentValue -= 11;
                    if (wp->currentValue < wp->result) {
                        wp->currentValue = wp->result;
                    }
                    break;
            }
            wp->monte = yuu_kesan_chk(wp->field_14, wp->currentValue, 0, 0, wp->maxValue);
            if (wp->currentValue != wp->result) {
                break;
            }
            if (wp->result != 0) {
                psndSFXOff(snd_id);
            }
            switch (wp->field_14) {
                case 0:
                case 2:
                case 3:
                    if (wp->result > 0) {
                        wp->timer = 0x30;
                        wp->state++;
                    } else {
                        wp->timer = 30;
                        wp->state = 9;
                    }
                    break;
                case 1:
                    if (wp->mult > 1 || wp->mult < 0) {
                        if (wp->result < wp->maxValue) {
                            wp->timer = 0x30;
                            wp->state = 6;
                        } else {
                            wp->timer = 30;
                            wp->state = 9;
                        }
                    } else {
                        wp->timer = 30;
                        wp->state = 9;
                    }
                    break;
            }
            break;
        case 3:
            if (wp->timer != 0) {
                wp->timer--;
                break;
            }
            wp->timer = 0x30;
            wp->state++;
            break;
        case 4:
            if (wp->timer != 0) {
                wp->timer--;
                break;
            }
            wp->state++;
            break;
        case 5:
            switch (wp->field_14) {
                case 0:
                case 2:
                case 3:
                    targetMonte = yuu_kesan_chk(wp->field_14, wp->currentValue, wp->bonus, 0, wp->maxValue);
                    if (wp->monte < targetMonte) {
                        wp->monte++;
                        psndSFXOn((const char*)0x8ca);
                    } else if (wp->monte > targetMonte) {
                        wp->monte--;
                    } else if (wp->result > 0 && (wp->mult > 1 || wp->mult < 0)) {
                        wp->timer = 0x30;
                        wp->state++;
                    } else {
                        wp->timer = 30;
                        wp->state = 9;
                    }
                    break;
            }
            break;
        case 6:
            if (wp->timer != 0) {
                wp->timer--;
                break;
            }
            wp->timer = 0x30;
            wp->state++;
            break;
        case 7:
            if (wp->timer != 0) {
                wp->timer--;
                break;
            }
            wp->state++;
            break;
        case 8:
            switch (wp->field_14) {
                case 0:
                case 1:
                case 2:
                case 3:
                    targetMonte = yuu_kesan_chk(wp->field_14, wp->currentValue, wp->bonus, wp->mult, wp->maxValue);
                    if (wp->monte < targetMonte) {
                        wp->monte++;
                        psndSFXOn((const char*)0x8ca);
                    } else if (wp->monte > targetMonte) {
                        wp->monte--;
                    } else {
                        wp->timer = 30;
                        wp->state++;
                    }
                    break;
            }
            break;
        case 9:
            wp->flags2 &= ~4;
            break;
    }
    }
    return EVT_RETURN_BLOCK;
}

static EVT_BEGIN(main_evt)
    CALL(evt_yuuranking_main_func)
    RETURN
EVT_END()

// TODO: type
static YuRankWork* _wp;

// TODO: snd_id$716 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// static u32 snd_id$716[1] = {
//     0x00000000,
// };
static s32 yuu_kesan(s32 mode, s32 value, s32 bonus, s32 mult, s32 maxValue) {
    s32 total;

    switch (mode) {
        case 0:
            total = value / 30000;
            break;
        case 3:
            total = (value - 74000) / 2000;
            break;
        case 2:
            total = (value - 109000) / 2000;
            break;
        case 1:
            total = (maxValue - value) * 15 / maxValue;
            break;
    }
    if (total < 0) {
        total = 0;
    }
    total += bonus;
    if (mult > 0) {
        if (mult < 256) {
            total *= mult;
        } else {
            total = mult + total;
            total -= 256;
        }
    } else {
        total += mult;
        if (total < 0) {
            total = 0;
        }
    }
    if (total < 0) {
        total = 0;
    }
    return total;
}

// A dead copy of the checked entry above (yuu_kesan_chk): the original object carries it between
// evt_yuuranking_alloc and yuu_kesan, unreferenced (nothing in gor branches to or points at it), as
// `cmpwi r4,0 / bge yuu_kesan / li r3,0 / blr`, a frameless tail branch into yuu_kesan that the original compiler
// emitted on its own. GC/2.6 never emits a frameless tail branch from C (every `return f(...)` gets a frame), and no
// available compiler version or pragma produces it (swept: GC/1.0-1.3.2r, 2.0-2.7, 3.0a3-3.0a5, linkage keywords,
// force_active, the inline and opt_* pragmas). So under MUST_MATCH its body is an asm block: a narrow exception Isaac
// approved on 2026-10-04, listed in tools/asm_exceptions.json (docs/CONTEXT.md, "Inline asm in game code: the
// exception policy"). The block sits in a C function, which MWCC defers like the others, so it lands at this slot
// (after alloc in .text); an `asm` function is generated as soon as it is parsed and would come first. The
// leaf C function adds only the `blr`. The natural build keeps the C form.
#ifdef MUST_MATCH
// clang-format off
static s32 yuu_kesan_chk_copy(s32 mode, s32 value, s32 bonus, s32 mult, s32 maxValue) {
    asm {
        cmpwi r4, 0
        bge yuu_kesan
        li r3, 0
    }
}
// clang-format on
#else
static s32 yuu_kesan_chk_copy(s32 mode, s32 value, s32 bonus, s32 mult, s32 maxValue) {
    if (value < 0) {
        return 0;
    }
    return yuu_kesan(mode, value, bonus, mult, maxValue);
}
#endif

#define A14(i) yuwp->rankUsed[work->field_14][i]
#define A74(i) yuwp->rankScore[work->field_14][i]
#define AD4(i) yuwp->rankMonte[work->field_14][i]
#define A74F(i) (((s32*)((u8*)yuwp + work->field_14 * 24 + 0x74 + (i) * 4))[0])
// slot-search read: row base yuwp+mode*24 keeps 0x14 as the index displacement (add+lwz), not addi+lwzx.
#define A14R(i) (((s32*)((u8*)yuwp + work->field_14 * 24 + (i) * 4))[5])

#define YUU_RANK_SETUP(ELSE_CODE, SETVAL, OUTER, CMP)                                                                    \
    A14(5) = 0;                                                                                                          \
    A74(5) = 0;                                                                                                          \
    AD4(5) = 0;                                                                                                          \
    work->rankIndex = -1;                                                                                                 \
    if (work->displayMode == 1) {                                                                                           \
        if (OUTER) {                                                                                                     \
        for (i = 0; i < 6; i++) {                                                                                        \
            if (A14R(i) == 0) {                                                                                          \
                A14(i) = 1;                                                                                              \
                A74(i) = work->result;                                                                                 \
                AD4(i) = yuu_kesan_chk(work->field_14, work->result, work->bonus, work->mult, work->maxValue);  \
                work->rankIndex = i;                                                                                      \
                break;                                                                                                   \
            }                                                                                                            \
            if (CMP) {                                                                                                   \
                for (j = 4; j > i; j--) {                                                                                \
                    A14(j) = A14(j - 1);                                                                                   \
                    A74(j) = A74(j - 1);                                                                                   \
                    AD4(j) = AD4(j - 1);                                                                                   \
                }                                                                                                        \
                A14(i) = 1;                                                                                              \
                A74(i) = work->result;                                                                                 \
                AD4(i) = yuu_kesan_chk(work->field_14, work->result, work->bonus, work->mult, work->maxValue);  \
                work->rankIndex = i;                                                                                      \
                if (i == 0) {                                                                                            \
                    SETVAL;                                                                                              \
                }                                                                                                        \
                break;                                                                                                   \
            }                                                                                                            \
        }                                                                                                                \
        } else {                                                                                                         \
            A14(5) = 1;                                                                                                  \
            A74(5) = work->result;                                                                                     \
            AD4(5) = 0;                                                                                                  \
            work->rankIndex = 5;                                                                                          \
        }                                                                                                                \
    } else {                                                                                                             \
        ELSE_CODE                                                                                                        \
    }

// The slot-search/insert bookkeeping, per-mode constants, call sequence and the shift-loop addressing all match.
// The two `>= 0` case-0/2/3 OUTER tests are written `0 <= work->result` (reversed operands) to pin result
// into r8 like the target.
USER_FUNC(evt_yuuranking_alloc_10_text_A47C) {
    s32* args = event->args;
    YuRankWork* work = _mapAlloc(sizeof(YuRankWork));
    EventEntry* evt;
    s32 i;
    s32 j;

    memset(work, 0, sizeof(YuRankWork));
    _wp = work;
    work->x = evtGetFloat(event, args[0]);
    work->field_10 = evtGetFloat(event, args[1]);
    work->field_14 = evtGetValue(event, args[2]);
    work->displayMode = evtGetValue(event, args[3]);
    work->result = evtGetValue(event, args[4]);
    work->bonus = evtGetValue(event, args[5]);
    work->mult = evtGetValue(event, args[6]);

    switch (work->field_14) {
    case 0:
        YUU_RANK_SETUP(
            if (work->displayMode == 0) {
                work->currentValue = 0;
                if (work->result < work->currentValue) {
                    work->currentValue = work->result;
                }
            },
            evtSetValue(NULL, 0xF8406058, 1), 0 <= work->result, work->result >= A74F(i))
        break;
    case 2:
    case 3:
        YUU_RANK_SETUP(
            if (work->displayMode == 0) {
                work->currentValue = 0;
                if (work->result < work->currentValue) {
                    work->currentValue = work->result;
                }
            },
            if (work->field_14 == 3) { evtSetValue(NULL, 0xF8406059, 1); } else { evtSetValue(NULL, 0xF840605A, 1); }, 0 <= work->result, work->result >= A74F(i))
        break;
    case 1:
        work->maxValue = evtGetValue(event, args[7]);
        YUU_RANK_SETUP(
            if (work->displayMode == 2) {
                work->maxValue = 0x2710;
            }
            if (work->displayMode == 0) {
                work->currentValue = work->maxValue - 1;
                if (work->result > work->currentValue) {
                    work->currentValue = work->result;
                }
            },
            evtSetValue(NULL, 0xF840605B, 1), work->result < work->maxValue, work->result <= A74F(i))
        break;
    }

    if (work->displayMode == 0) {
        work->flags2 |= 4;
    }
    evt = evtEntry(main_evt, 0, 0);
    work->eventEntry = evt;
    work->eventEntry->lwData[0] = (s32)work;
    return 2;
}

#undef YUU_RANK_SETUP
#undef AD4
#undef A74
#undef A14
#undef A14R
#undef A74F
USER_FUNC(evt_yuuranking_free_10_text_A3F8) {
    YuRankWork* wp = _wp;

    if (wp == NULL) {
        return 2;
    }
    if (isFirstCall) {
        wp->freeFlags |= 1;
    }
    if (!(wp->freeFlags & 2)) {
        return 0;
    }
    _mapFree(_wp);
    _wp = NULL;
    return 2;
}

USER_FUNC(yuuranking_wait_button_10_text_A37C) {
    YuRankWork* wp = _wp;

    if (isFirstCall) {
        wp->flags2 |= 2;
    }
    if (!(wp->flags2 & 4)) {
        if (keyGetButtonTrg(0) & 0x300) {
            wp->flags2 &= ~2;
            psndSFXOn((const char*)0x12);
            return 2;
        }
    }
    return 0;
}

static USER_FUNC(yuuranking_get_monte) {
    YuRankWork* wp = _wp;

    evtSetValue(event, event->args[0], wp->monte);
    return 2;
}

const f32 float_1000_gor_0001cd80 = 1000.0f;
// clang-format on
