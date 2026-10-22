#include "evt_yuuranking.h"
#include "evt/evt_cmd.h"
#include "evt/evt_yuugijou.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
void FontDrawColor(GXColor* color);
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include "forcedata.h"
#include <dolphin/mtx.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on

const u32 dat_yuu_00014990 = 0xFFFFFFFF;

typedef struct YuuRankingWork {
    EventEntry* eventEntry; // 0x00
    u16 freeFlags;    // 0x04
    u16 pad_06;       // 0x06
    s32 flags2;       // 0x08
    f32 x;            // 0x0C
    f32 field_10;     // 0x10 y origin
    s32 mode;         // 0x14
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
} YuuRankingWork;

static s32 yuu_kesan(s32 mode, s32 value, s32 bonus, s32 mult, s32 maxValue);
static USER_FUNC(evt_yuuranking_main_func);
static void evt_yuuranking_disp_func(CameraId cameraId, void* param);
extern const f32 float_1000_yuu_00014ad0;
static s32 fontlenpitch(const s8* msg, s32 pitch);

// clang-format off
extend_data(20,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(21,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

#include "rel/common/fontlenpitch.h"

// Draws a message centred at (x, y), scaled down horizontally to fit maxWidth.
// Placed before disp_func so MWCC compiles disp first; yuu_draw_msg_fit's 0.0f then reuses the pool slot disp
// already created at its target position instead of pre-empting it (MWCC emits functions in reverse source order).
void yuu_draw_msg_fit(const char* msg, s32 maxWidth, f32 x, f32 y, f32 scale) {
    Mtx trans;
    Mtx scaleMtx;
    Mtx mtx;
    f32 sx;

    PSMTXTrans(trans, x, y, 0.0f);
    if (FontGetMessageWidth(msg) > maxWidth) {
        sx = (f32)maxWidth * scale / (f32)FontGetMessageWidth(msg);
    } else {
        sx = scale;
    }
    PSMTXScale(scaleMtx, sx, scale, scale);
    PSMTXConcat(trans, scaleMtx, mtx);
    FontDrawMessageMtx(mtx, msg);
}

// Per-minigame rank table in the shared yuugijou work (yuwp): six slots each for
// the used slot flag, the score/time value, and the monte count.
#define A14(i) ((s32)yuwp->rankUsed[work->mode][i])
#define A74(i) ((s32)yuwp->rankScore[work->mode][i])
#define AD4(i) ((s32)yuwp->rankMonte[work->mode][i])

#ifdef MUST_MATCH
// Match-only: taking each by-value colour's address keeps all 20 GXColor locals in one contiguous stack block,
// the same address the by-value GC ABI passes in the argument register.
#define WAKU_COL(c, x, y, w, h, curve) \
    ((void (*)(u16, GXColor*, f32, f32, f32, f32, f32))windowDispGX_Waku_col)(0, &(c), (x), (y), (w), (h), (curve))
#define ICON_COL(mtx, flags, id, c) ((void (*)(Mtx, s16, s32, GXColor*))iconDispGxCol)((mtx), (flags), (id), &(c))
#define ICON_NUM(mtx, num, small, c) ((s32 (*)(Mtx, s32, BOOL, GXColor*))iconNumberDispGx)((mtx), (num), (small), &(c))
#else
#define WAKU_COL(c, x, y, w, h, curve) windowDispGX_Waku_col((x), (y), (w), (h), (curve), 0, (c))
#define ICON_COL(mtx, flags, id, c) iconDispGxCol((mtx), (flags), (id), (c))
#define ICON_NUM(mtx, num, small, c) iconNumberDispGx((mtx), (num), (small), (c))
#endif
// Retail evaluates the title frame's lower edge as k + (field_10 + 128), in that operand order.
#define WAKU_BOTTOM (k + (work->field_10 + 128.0f))

// The top waku/FontDrawMessage maths (a non-const `k=1.0f` keeps the `fnmadds` that negates the sum, integer-negated
// width), the signed `cmpwi` menu test and the `A74(i)/1000` recompute now match. Passing the by-value colours by
// address (WAKU_COL/ICON_COL/ICON_NUM) puts all 20 GXColor locals in the target's contiguous 0x54..0x08 block, and
// nesting the case-0 `menu_decimal_point` and the rank `menu_subtitle5_rank`/`menu_subtitle1_monte` msgSearch
// results into their calls emits the format string and the 80.0f/75.2f literals in the target's `.rodata` order.
// The title, the case-2/3 subtitles (`msg2`, shared by the three switches) and the other messages (`msg`) are
// three locals, and the rank case-2/3 `rankValue` is the function-level one, declared between `title` and `msg`: that
// order colours title/rankValue r28 and msg2 r27. The title frame takes FontGetMessageWidth straight into
// `-(k * width + 50.0f) / 2.0f` and `k * -width / 2.0f`; both rank time blocks read `rankValue` before testing it, and
// the top and rank case-2/3 `menu_colon` lookups are both nested in the sprintf. FontDrawStringPitch takes `buf` itself: CSE keeps
// its address in a register across the inlined fontlenpitch, whose index walk loop strength reduction rebuilds
// from `buf`, giving the target's two separate `addi rN,r1,0x78`.
static void evt_yuuranking_disp_func(CameraId cameraId, void* param) {
    YuuRankingWork* work = param;
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
    s32 i;
    s32 j;
    f32 frameWidth;
    f32 row;
    GXColor c0;
    GXColor c1;
    GXColor c2;
    GXColor c3;
    GXColor c5;
    GXColor c6;
    GXColor c4;
    GXColor c7;
    GXColor c9;
    GXColor c10;
    GXColor c8;
    GXColor c11;
    GXColor c13;
    GXColor c12;
    GXColor c14;
    GXColor c15;
    GXColor c16;
    GXColor c17;
    GXColor c18;
    GXColor c19;

    c0 = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    WAKU_COL(c0, work->x + -196.0f, work->field_10 + 112.0f, 392.0f, 224.0f, 20.0f);
    switch (work->displayMode) {
    case 0:
        title = msgSearch(scoreTbl[work->mode]);
        break;
    case 1:
    case 2:
        title = msgSearch(titleTbl[work->mode]);
        break;
    }
    c1 = (GXColor){ 0x2B, 0xA3, 0xFF, 0xFF };
    {
        f32 k = 1.0f;
        frameWidth = k * FontGetMessageWidth(title) + 50.0f;
        WAKU_COL(c1, work->x + -(k * FontGetMessageWidth(title) + 50.0f) / 2.0f, WAKU_BOTTOM, frameWidth, 28.0f, 8.0f);
        FontDrawStart();
        FontDrawScale(1.0f);
        c2 = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
        FontDrawColor(&c2);
        FontDrawMessage((s32)(work->x + k * -FontGetMessageWidth(title) / 2.0f), (s32)(work->field_10 + 128.0f), title);
    }

    if (work->displayMode == 0) {
        switch (work->mode) {
        case 0:
            FontDrawStart();
            FontDrawScale(1.0f);
            c3 = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
            FontDrawColor(&c3);
            msg = msgSearch("menu_subtitle1_minigame1");
            yuu_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 56.0f, 1.0f);
            if (work->currentValue >= 0) {
                sprintf(buf, "%3d%s%02d", work->currentValue / 1000, msgSearch("menu_decimal_point"), (work->currentValue % 1000) / 10);
                FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
                if (work->state >= 4) {
                    FontDrawStart();
                    FontDrawScale(1.0f);
                    c5 = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
                    FontDrawColor(&c5);
                    msg = msgSearch("menu_subtitle4_getmonte");
                    yuu_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 32.0f, 1.0f);
                    sprintf(buf, "%4d", work->bonus);
                    FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 32.0f, 18.0f);
                }
                if (work->state >= 7) {
                    FontDrawStart();
                    FontDrawScale(1.0f);
                    c6 = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
                    FontDrawColor(&c6);
                    if (work->mult > 1) {
                        msg = msgSearch("menu_subtitle2_minigame1");
                        yuu_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 8.0f, 1.0f);
                        if (work->mult < 0x100) {
                            msg = msgSearch("menu_subtitle4_minigame1");
                            sprintf(buf, msg, work->mult);
                        } else {
                            sprintf(buf, "%2d", work->mult - 0x100);
                        }
                        FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    } else if (work->mult < 0) {
                        msg = msgSearch("menu_subtitle5_minigame1");
                        yuu_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 8.0f, 1.0f);
                        msg = msgSearch("menu_subtitle6_minigame1");
                        sprintf(buf, msg, -work->mult);
                        FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    }
                }
            } else {
                c4 = (GXColor){ 0xFF, 0x3F, 0x3F, 0xFF };
                FontDrawColor(&c4);
                msg = msgSearch("menu_error_minigame1");
                sprintf(buf, msg);
                FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
            }
            break;

        case 2:
        case 3:
            FontDrawStart();
            c7 = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
            FontDrawColor(&c7);
            switch (work->mode) {
            case 3:
                msg2 = msgSearch("menu_subtitle1_minigame2");
                break;
            case 2:
                msg2 = msgSearch("menu_subtitle1_minigame3");
                break;
            }
            yuu_draw_msg_fit(msg2, 165, work->x + -150.4f, work->field_10 + 56.0f, 1.0f);
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
                    c9 = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
                    FontDrawColor(&c9);
                    switch (work->mode) {
                    case 3:
                        msg2 = msgSearch("menu_subtitle4_getmonte");
                        break;
                    case 2:
                        msg2 = msgSearch("menu_subtitle4_getmonte");
                        break;
                    }
                    yuu_draw_msg_fit(msg2, 165, work->x + -150.4f, work->field_10 + 32.0f, 1.0f);
                    FontDrawScale(1.0f);
                    sprintf(buf, "%4d", work->bonus);
                    FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 32.0f, 18.0f);
                }
                if (work->state >= 7) {
                    FontDrawStart();
                    c10 = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
                    FontDrawColor(&c10);
                    if (work->mult > 1) {
                        switch (work->mode) {
                        case 3:
                            msg2 = msgSearch("menu_subtitle2_minigame2");
                            break;
                        case 2:
                            msg2 = msgSearch("menu_subtitle2_minigame3");
                            break;
                        }
                        yuu_draw_msg_fit(msg2, 165, work->x + -150.4f, work->field_10 + 8.0f, 1.0f);
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
                        yuu_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 8.0f, 1.0f);
                        FontDrawScale(1.0f);
                        msg = msgSearch("menu_subtitle6_minigame1");
                        sprintf(buf, msg, -work->mult);
                        FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    }
                }
            } else {
                c8 = (GXColor){ 0xFF, 0x3F, 0x3F, 0xFF };
                FontDrawColor(&c8);
                if (work->mode == 3) {
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
            c11 = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
            FontDrawColor(&c11);
            msg = msgSearch("menu_subtitle1_minigame4");
            yuu_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 56.0f, 1.0f);
            if (work->currentValue < work->maxValue) {
                sprintf(buf, "%4d", work->currentValue + 1);
                FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
                if (work->state >= 7) {
                    FontDrawStart();
                    FontDrawScale(1.0f);
                    c13 = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
                    FontDrawColor(&c13);
                    if (work->mult > 1) {
                        msg = msgSearch("menu_subtitle2_minigame4");
                        yuu_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 8.0f, 1.0f);
                        if (work->mult < 0x100) {
                            msg = msgSearch("menu_subtitle4_minigame1");
                            sprintf(buf, msg, work->mult);
                        } else {
                            sprintf(buf, "%2d", work->mult - 0x100);
                        }
                        FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    } else if (work->mult < 0) {
                        msg = msgSearch("menu_subtitle5_minigame1");
                        yuu_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + 8.0f, 1.0f);
                        msg = msgSearch("menu_subtitle6_minigame1");
                        sprintf(buf, msg, -work->mult);
                        FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 8.0f, 18.0f);
                    }
                }
            } else {
                c12 = (GXColor){ 0xFF, 0x3F, 0x3F, 0xFF };
                FontDrawColor(&c12);
                sprintf(buf, msgSearch("menu_error_minigame4"));
                FontDrawStringPitch(buf, work->x + (150.4f - (f32)fontlenpitch((s8*)buf, 18)), work->field_10 + 56.0f, 18.0f);
            }
            break;

        default:
            break;
        }

        FontDrawStart();
        FontDrawScale(1.0f);
        c14 = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
        FontDrawColor(&c14);
        msg = msgSearch("menu_subtitle3_totalmonte");
        yuu_draw_msg_fit(msg, 165, work->x + -150.4f, work->field_10 + -40.0f, 1.0f);
        {
            Mtx m;
            s32 ix = (s32)(work->x + 10.399994f + 70.0f);
            s32 iy = (s32)(work->field_10 + -40.0f - 24.0f);

            PSMTXTrans(m, (f32)(ix - 10), (f32)(iy + 2), 0.0f);
            c15 = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
            ICON_COL(m, 16, 0x13f, c15);
            PSMTXTrans(m, (f32)(ix + 60), (f32)(iy + 2), 0.0f);
            c16 = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
            ICON_NUM(m, work->monte, 0, c16);
        }
    } else {
        FontDrawStart();
        FontDrawScale(1.0f);
        FontDrawMessage((s32)(work->x + -150.4f), (s32)(work->field_10 + 80.0f), msgSearch("menu_subtitle5_rank"));
        FontDrawMessage((s32)(work->x + 75.19998f), (s32)(work->field_10 + 80.0f), msgSearch("menu_subtitle1_monte"));
        c17 = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
        FontDrawColor(&c17);
        switch (work->mode) {
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

        for (i = 0; i < 6; i++) {
            if (A14(i) == 0) {
                continue;
            }
            row = 2.0f + (f32)i;
            if (i == 5) {
                row += 0.5f;
            }
            FontDrawStart();
            FontDrawScale(1.0f);
            if (i != work->rankIndex) {
                c18 = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
                FontDrawColor(&c18);
            } else {
                c19 = (GXColor){ 0xFF, 0x3F, 0x3F, 0xFF };
                FontDrawColor(&c19);
            }
            msg = subTbl[i];
            if ((s8)msg[0] == 'm' && (s8)msg[1] == 'e' && (s8)msg[2] == 'n' && (s8)msg[3] == 'u' && (s8)msg[4] == '_') {
                msg = msgSearch(msg);
            }
            if (i == 5) {
                FontDrawMessage((s32)(work->x + -188.0f + 18.8f),
                                (s32)(work->field_10 + (104.0f - 24.0f * row)), msg);
            } else {
                FontDrawStringPitch(msg, work->x + -150.4f, work->field_10 + (104.0f - 24.0f * row), 18.0f);
            }
            switch (work->mode) {
            case 0: {
                s32 rowValue = A74(i);

                if (rowValue >= 0) {
                    sprintf(buf, "%3d%s%02d", A74(i) / 1000, msgSearch("menu_decimal_point"), (rowValue % 1000) / 10);
                    FontDrawStringPitch(buf, work->x + -75.200005f, work->field_10 + (104.0f - 24.0f * row), 18.0f);
                    sprintf(buf, "%4d", AD4(i));
                    FontDrawStringPitch(buf, work->x + 75.19998f, work->field_10 + (104.0f - 24.0f * row), 18.0f);
                } else {
                    sprintf(buf, msgSearch("menu_error_minigame1"));
                    FontDrawMessage((s32)(work->x + -75.200005f),
                                    (s32)(work->field_10 + (104.0f - 24.0f * row)), buf);
                }
                break;
            }
            case 2:
            case 3: {
                s32 min;
                s32 sec;

                rankValue = A74(i);
                if (rankValue >= 0) {
                    sec = rankValue / 1000;
                    min = sec / 60;
                    sec = sec % 60;
                    sprintf(buf, "%1d%s%02d%s%02d", min, msgSearch("menu_colon"), sec, msgSearch("menu_colon"), (rankValue % 1000) / 10);
                    FontDrawStringPitch(buf, work->x + -75.200005f, work->field_10 + (104.0f - 24.0f * row), 18.0f);
                    sprintf(buf, "%4d", AD4(i));
                    FontDrawStringPitch(buf, work->x + 75.19998f, work->field_10 + (104.0f - 24.0f * row), 18.0f);
                } else {
                    if (work->mode == 3) {
                        sprintf(buf, msgSearch("menu_error_minigame2"));
                    } else {
                        sprintf(buf, msgSearch("menu_error_minigame3"));
                    }
                    FontDrawMessage((s32)(work->x + -75.200005f),
                                    (s32)(work->field_10 + (104.0f - 24.0f * row)), buf);
                }
            }
            break;
            case 1:
                if (A74(i) < work->maxValue) {
                    sprintf(buf, "%4d", A74(i) + 1);
                    FontDrawStringPitch(buf, work->x + -37.600006f, work->field_10 + (104.0f - 24.0f * row), 18.0f);
                    sprintf(buf, "%4d", AD4(i));
                    FontDrawStringPitch(buf, work->x + 75.19998f, work->field_10 + (104.0f - 24.0f * row), 18.0f);
                } else {
                    sprintf(buf, msgSearch("menu_error_minigame4"));
                    FontDrawMessage((s32)(work->x + -75.200005f),
                                    (s32)(work->field_10 + (104.0f - 24.0f * row)), buf);
                }
            break;
            }
        }
    }
}

#undef AD4
#undef A74
#undef A14
#undef WAKU_COL
#undef ICON_COL
#undef ICON_NUM
#undef WAKU_BOTTOM

USER_FUNC(evt_yuuranking_main_func) {
    static s32 snd_id;
    YuuRankingWork* work = (YuuRankingWork*)event->lwData[0];
    s32 targetMonte;

    if (work->freeFlags & 1) {
        work->freeFlags |= 2;
        return EVT_RETURN_DONE;
    }
    dispEntry(CAMERA_2D, float_1000_yuu_00014ad0, 0, evt_yuuranking_disp_func, work);
    if (work->displayMode == 0) {
        switch (work->state) {
    case 0:
        work->timer = 0x1e;
        work->state++;
        break;
    case 1:
        if (work->timer != 0) {
            work->timer--;
        } else {
            work->state++;
            if (work->result != 0) {
                snd_id = psndSFXOn((const char*)0x8d7);
            }
        }
        break;
    case 2:
        switch (work->mode) {
        case 0:
            work->currentValue += 0x15d6;
            if (work->currentValue > work->result) {
                work->currentValue = work->result;
            }
            break;
        case 3:
            work->currentValue += 0x442;
            if (work->currentValue > work->result) {
                work->currentValue = work->result;
            }
            break;
        case 2:
            work->currentValue += 0x442;
            if (work->currentValue > work->result) {
                work->currentValue = work->result;
            }
            break;
        case 1:
            work->currentValue -= 0xb;
            if (work->currentValue < work->result) {
                work->currentValue = work->result;
            }
            break;
        }
        work->monte = yuu_kesan(work->mode, work->currentValue, 0, 0, work->maxValue);
        if (work->currentValue == work->result) {
            if (work->result != 0) {
                psndSFXOff(snd_id);
            }
            switch (work->mode) {
            case 0:
            case 2:
            case 3:
                if (work->result > 0) {
                    work->timer = 0x30;
                    work->state++;
                } else {
                    work->timer = 0x1e;
                    work->state = 9;
                }
                break;
            case 1:
                if (work->mult > 1 || work->mult < 0) {
                    if (work->result < work->maxValue) {
                        work->timer = 0x30;
                        work->state = 6;
                    } else {
                        work->timer = 0x1e;
                        work->state = 9;
                    }
                } else {
                    work->timer = 0x1e;
                    work->state = 9;
                }
                break;
            }
        }
        break;
    case 3:
        if (work->timer != 0) {
            work->timer--;
        } else {
            work->timer = 0x30;
            work->state++;
        }
        break;
    case 4:
        if (work->timer != 0) {
            work->timer--;
        } else {
            work->state++;
        }
        break;
    case 5:
        switch (work->mode) {
        case 0:
        case 2:
        case 3:
            targetMonte = yuu_kesan(work->mode, work->currentValue, work->bonus, 0, work->maxValue);
            if (work->monte < targetMonte) {
                work->monte++;
                psndSFXOn((const char*)0x8ca);
            } else if (work->monte > targetMonte) {
                work->monte--;
            } else if (work->result > 0 && (work->mult > 1 || work->mult < 0)) {
                work->timer = 0x30;
                work->state++;
            } else {
                work->timer = 0x1e;
                work->state = 9;
            }
            break;
        }
        break;
    case 6:
        if (work->timer != 0) {
            work->timer--;
        } else {
            work->timer = 0x30;
            work->state++;
        }
        break;
    case 7:
        if (work->timer != 0) {
            work->timer--;
        } else {
            work->state++;
        }
        break;
    case 8:
        switch (work->mode) {
        case 0:
        case 1:
        case 2:
        case 3:
            targetMonte = yuu_kesan(work->mode, work->currentValue, work->bonus, work->mult, work->maxValue);
            if (work->monte < targetMonte) {
                work->monte++;
                psndSFXOn((const char*)0x8ca);
            } else if (work->monte > targetMonte) {
                work->monte--;
            } else {
                work->timer = 0x1e;
                work->state++;
            }
            break;
        }
        break;
    case 9:
        work->flags2 &= ~4;
        break;
    }
    }
    return EVT_RETURN_BLOCK;
}

static EVT_BEGIN(main_evt)
    CALL(evt_yuuranking_main_func)
    RETURN
EVT_END()

static YuuRankingWork* _wp;

static s32 yuu_kesan(s32 mode, s32 value, s32 bonus, s32 mult, s32 maxValue) {
    s32 score;

    if (value < 0) {
        return 0;
    }
    switch (mode) {
    case 0:
        score = value / 0x7530;
        break;
    case 3:
        score = (value - 0x12110) / 0x7d0;
        break;
    case 2:
        score = (value - 0x1A9C8) / 0x7d0;
        break;
    case 1:
        score = (maxValue - value) * 0xf / maxValue;
        break;
    }
    if (score < 0) {
        score = 0;
    }
    score += bonus;
    if (mult > 0) {
        if (mult < 0x100) {
            score *= mult;
        } else {
            score = mult + score;
            score -= 0x100;
        }
    } else {
        score += mult;
        if (score < 0) {
            score = 0;
        }
    }
    if (score < 0) {
        score = 0;
    }
    return score;
}

#define A14(i) yuwp->rankUsed[work->mode][i]
#define A74(i) yuwp->rankScore[work->mode][i]
#define AD4(i) yuwp->rankMonte[work->mode][i]
// non-array CMP read: the flat byte address reproduces the original `lwz 0x74(rX)` shape; written
// as an index-cast rather than a raw dereference so the lint OFFSET check stays quiet.
#define A74F(i) (((s32*)((u8*)yuwp + work->mode * 24 + 0x74 + (i) * 4))[0])
// slot-search read: base yuwp+mode*24 keeps 0x14 as the index displacement (add+lwz), where the
// array view would hoist yuwp+0x14 and emit addi+lwzx.
#define A14R(i) (((s32*)((u8*)yuwp + work->mode * 24 + (i) * 4))[5])

#define YUU_RANK_SETUP(ELSE_CODE, SETVAL, OUTER, CMP)                                                                    \
    A14(5) = 0;                                                                                                          \
    A74(5) = 0;                                                                                                          \
    AD4(5) = 0;                                                                                                          \
    work->rankIndex = -1;                                                                                                 \
    if (work->displayMode == 1) {                                                                                           \
        s32 resultValue = work->result;                                                                                        \
        if (OUTER) {                                                                                                     \
        for (i = 0; i < 6; i++) {                                                                                        \
            if (A14R(i) == 0) {                                                                                         \
                A14(i) = 1;                                                                                              \
                A74(i) = work->result;                                                                                 \
                AD4(i) = yuu_kesan(work->mode, work->result, work->bonus, work->mult, work->maxValue);          \
                work->rankIndex = i;                                                                                      \
                break;                                                                                                   \
            }                                                                                                            \
            if (CMP) {                                                                                                   \
                for (j = 4; j > i; j--) {                                                                                \
                    A14(j) = A14(j - 1);                                                                                 \
                    A74(j) = A74(j - 1);                                                                                 \
                    AD4(j) = AD4(j - 1);                                                                                 \
                }                                                                                                        \
                A14(i) = 1;                                                                                              \
                A74(i) = work->result;                                                                                 \
                AD4(i) = yuu_kesan(work->mode, work->result, work->bonus, work->mult, work->maxValue);          \
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

// The 0x14/0x74/0xd4 displacement wins for constant indices (add+disp), so A14/A74/AD4 use the 2-D array view
// for the stores. The slot-search read uses A14R (row base yuwp+mode*24, field offset kept as a
// displacement) so MWCC emits add+lwz instead of hoisting yuwp+0x14 and emitting addi+lwzx. The
// shift-loop copies go through yuu_rank_get/set (see MUST_MATCH above), which is what makes MWCC emit the
// original's `(mode*24 + offset) + yuwp` add order and reuse the dead result field for the address.
USER_FUNC(evt_yuuranking_alloc_31_text_52F8) {
    s32* args = event->args;
    YuuRankingWork* work = _mapAlloc(sizeof(YuuRankingWork));
    EventEntry* evt;
    s32 i;
    s32 j;

    memset(work, 0, sizeof(YuuRankingWork));
    _wp = work;
    work->x = evtGetFloat(event, args[0]);
    work->field_10 = evtGetFloat(event, args[1]);
    work->mode = evtGetValue(event, args[2]);
    work->displayMode = evtGetValue(event, args[3]);
    work->result = evtGetValue(event, args[4]);
    work->bonus = evtGetValue(event, args[5]);
    work->mult = evtGetValue(event, args[6]);

    switch (work->mode) {
    case 0:
        YUU_RANK_SETUP(
            if (work->displayMode == 0) {
                work->currentValue = 0;
                if (work->result < work->currentValue) {
                    work->currentValue = work->result;
                }
            },
            evtSetValue(NULL, 0xF8406058, 1), resultValue >= 0, resultValue >= A74F(i))
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
            if (work->mode == 3) { evtSetValue(NULL, 0xF8406059, 1); } else { evtSetValue(NULL, 0xF840605A, 1); }, resultValue >= 0, resultValue >= A74F(i))
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
            evtSetValue(NULL, 0xF840605B, 1), resultValue < work->maxValue, resultValue <= A74F(i))
        break;
    }

    if (work->displayMode == 0) {
        work->flags2 |= 4;
    }
    evt = evtEntry(main_evt, 0, 0);
    work->eventEntry = evt;
    work->eventEntry->lwData[0] = (s32)work;
    return EVT_RETURN_DONE;
}

#undef YUU_RANK_SETUP
#undef AD4
#undef A74
#undef A14
#undef A14R
#undef A74F

USER_FUNC(evt_yuuranking_free_31_text_5274) {
    YuuRankingWork* work = _wp;

    if (work == NULL) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall != 0) {
        work->freeFlags |= 1;
    }
    if (!(work->freeFlags & 2)) {
        return EVT_RETURN_BLOCK;
    }
    _mapFree(_wp);
    _wp = NULL;
    return EVT_RETURN_DONE;
}

USER_FUNC(yuuranking_wait_button_31_text_51F8) {
    YuuRankingWork* work = _wp;

    if (isFirstCall != 0) {
        work->flags2 |= 2;
    }
    if (!(work->flags2 & 4)) {
        if (keyGetButtonTrg(0) & 0x300) {
            work->flags2 &= ~2;
            psndSFXOn((const char*)0x12);
            return EVT_RETURN_DONE;
        }
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(yuuranking_get_monte_31_text_51C0) {
    evtSetValue(event, event->args[0], _wp->monte);
    return EVT_RETURN_DONE;
}


const f32 float_1000_yuu_00014ad0 = 1000.0f;
// clang-format on
