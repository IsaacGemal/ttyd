#include "evt_yuuranking.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "manager/fontmgr.h"
#include "driver/msgdrv.h"
#include "memory.h"
#include "system.h"
#include "pmario_sound.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "driver/dispdrv.h"
#include <dolphin/mtx.h>

extern const f32 float_1000_gor_0001cd80;
extern const f32 zero_gor_0001cdd4;
extern const f64 double_to_int_mask_gor_0001d038;
extern const f64 double_to_int_gor_0001d040;

typedef struct YuRankWork {
    u8 pad_00[4];  // 0x0
    u16 field_04;  // 0x4
    u16 pad_06;    // 0x6
    u32 field_08;  // 0x8
    f32 field_0C;  // 0xC
    f32 field_10;  // 0x10
    s32 field_14;  // 0x14
    s32 field_18;  // 0x18
    s32 field_1C;  // 0x1C
    s32 field_20;  // 0x20
    s32 field_24;  // 0x24
    s32 field_28;  // 0x28
    s32 field_2C;  // 0x2C
    s32 field_30;  // 0x30
    s32 field_34;  // 0x34
    s32 field_38;  // 0x38
    s32 field_3C;  // 0x3C
} YuRankWork;

#pragma pool_data off

#pragma force_active on
static USER_FUNC(yuuranking_get_monte);
static s32 yuu_kesan(s32 type, s32 value, s32 bonus, s32 mult, s32 max);
static USER_FUNC(evt_yuuranking_main_func);
static void evt_yuuranking_disp_func(CameraId cameraId, void* param);
static s32 fontlenpitch(s8* str, s32 pitch);

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
// NONMATCHING (99.83%): register allocation and scheduling differ.
USER_FUNC(evt_yuuranking_main_func) {
    static s32 snd_id;
    YuRankWork* wp = (YuRankWork*)event->lwData[0];
    s32 value;
    s32 val;
    s32 bonus;
    s32 mult;
    s32 max;
    s32 type;

    if (wp->field_04 & 1) {
        wp->field_04 |= 2;
        return 2;
    }
    dispEntry(8, float_1000_gor_0001cd80, 0, evt_yuuranking_disp_func, wp);
    if (wp->field_18 == 0) {
        switch (wp->field_30) {
        case 0:
            wp->field_3C = 30;
            wp->field_30++;
            break;
        case 1:
            if (wp->field_3C != 0) {
                wp->field_3C--;
                break;
            }
            wp->field_30++;
            if (wp->field_1C != 0) {
                snd_id = psndSFXOn((const char*)0x8d7);
            }
            break;
        case 2:
            switch (wp->field_14) {
                case 0:
                    wp->field_38 += 5590;
                    if (wp->field_38 > wp->field_1C) {
                        wp->field_38 = wp->field_1C;
                    }
                    break;
                case 3:
                    wp->field_38 += 1090;
                    if (wp->field_38 > wp->field_1C) {
                        wp->field_38 = wp->field_1C;
                    }
                    break;
                case 2:
                    wp->field_38 += 1090;
                    if (wp->field_38 > wp->field_1C) {
                        wp->field_38 = wp->field_1C;
                    }
                    break;
                case 1:
                    wp->field_38 -= 11;
                    if (wp->field_38 < wp->field_1C) {
                        wp->field_38 = wp->field_1C;
                    }
                    break;
            }
            val = wp->field_38;
            max = wp->field_2C;
            type = wp->field_14;
            wp->field_34 = (val < 0) ? 0 : yuu_kesan(type, val, 0, 0, max);
            if (wp->field_38 != wp->field_1C) {
                break;
            }
            if (wp->field_1C != 0) {
                psndSFXOff(snd_id);
            }
            switch (wp->field_14) {
                case 0:
                case 2:
                case 3:
                    if (wp->field_1C > 0) {
                        wp->field_3C = 0x30;
                        wp->field_30++;
                    } else {
                        wp->field_3C = 30;
                        wp->field_30 = 9;
                    }
                    break;
                case 1:
                    if (wp->field_24 > 1 || wp->field_24 < 0) {
                        if (wp->field_1C < wp->field_2C) {
                            wp->field_3C = 0x30;
                            wp->field_30 = 6;
                        } else {
                            wp->field_3C = 30;
                            wp->field_30 = 9;
                        }
                    } else {
                        wp->field_3C = 30;
                        wp->field_30 = 9;
                    }
                    break;
            }
            break;
        case 3:
            if (wp->field_3C != 0) {
                wp->field_3C--;
                break;
            }
            wp->field_3C = 0x30;
            wp->field_30++;
            break;
        case 4:
            if (wp->field_3C != 0) {
                wp->field_3C--;
                break;
            }
            wp->field_30++;
            break;
        case 5:
            switch (wp->field_14) {
                case 0:
                case 2:
                case 3:
                    val = wp->field_38;
                    bonus = wp->field_20;
                    max = wp->field_2C;
                    type = wp->field_14;
                    value = (val < 0) ? 0 : yuu_kesan(type, val, bonus, 0, max);
                    if (wp->field_34 < value) {
                        wp->field_34++;
                        psndSFXOn((const char*)0x8ca);
                    } else if (wp->field_34 > value) {
                        wp->field_34--;
                    } else if (wp->field_1C > 0 && (wp->field_24 > 1 || wp->field_24 < 0)) {
                        wp->field_3C = 0x30;
                        wp->field_30++;
                    } else {
                        wp->field_3C = 30;
                        wp->field_30 = 9;
                    }
                    break;
            }
            break;
        case 6:
            if (wp->field_3C != 0) {
                wp->field_3C--;
                break;
            }
            wp->field_3C = 0x30;
            wp->field_30++;
            break;
        case 7:
            if (wp->field_3C != 0) {
                wp->field_3C--;
                break;
            }
            wp->field_30++;
            break;
        case 8:
            switch (wp->field_14) {
                case 0:
                case 1:
                case 2:
                case 3:
                    val = wp->field_38;
                    bonus = wp->field_20;
                    mult = wp->field_24;
                    max = wp->field_2C;
                    type = wp->field_14;
                    value = (val < 0) ? 0 : yuu_kesan(type, val, bonus, mult, max);
                    if (wp->field_34 < value) {
                        wp->field_34++;
                        psndSFXOn((const char*)0x8ca);
                    } else if (wp->field_34 > value) {
                        wp->field_34--;
                    } else {
                        wp->field_3C = 30;
                        wp->field_30++;
                    }
                    break;
            }
            break;
        case 9:
            wp->field_08 &= ~4;
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
static USER_FUNC(yuuranking_get_monte) {
    YuRankWork* wp = (YuRankWork*)_wp;

    evtSetValue(event, event->args[0], wp->field_34);
    return 2;
}
USER_FUNC(yuuranking_wait_button_10_text_A37C) {
    YuRankWork* wp = (YuRankWork*)_wp;

    if (isFirstCall) {
        wp->field_08 |= 2;
    }
    if (!(wp->field_08 & 4)) {
        if (keyGetButtonTrg(0) & 0x300) {
            wp->field_08 &= ~2;
            psndSFXOn((const char*)0x12);
            return 2;
        }
    }
    return 0;
}
USER_FUNC(evt_yuuranking_free_10_text_A3F8) {
    YuRankWork* wp = (YuRankWork*)_wp;

    if (wp == NULL) {
        return 2;
    }
    if (isFirstCall) {
        wp->field_04 |= 1;
    }
    if (!(wp->field_04 & 2)) {
        return 0;
    }
    _mapFree(_wp);
    _wp = NULL;
    return 2;
}

// TODO: write evt_yuuranking_alloc_10_text_A47C (global, 0xddc bytes) here:
// its asm is in build/G8MJ01/gor/asm/event/evt_yuuranking.s; this is where the original has it, just before the first script that uses it
static s32 yuu_kesan(s32 type, s32 value, s32 bonus, s32 mult, s32 max) {
    s32 total;

    switch (type) {
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
            total = (max - value) * 15 / max;
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

// TODO: write evt_yuuranking_disp_func (static, 0x2580 bytes) here:
// its asm is in build/G8MJ01/gor/asm/event/evt_yuuranking.s; this is where the original has it, just before the first script that uses it
void unk_gor_0000dfc4(const char* msg, s32 width, f32 x, f32 y, f32 z) {
    Mtx trans;
    Mtx scale;
    Mtx out;
    f32 f;

    PSMTXTrans(trans, x, y, zero_gor_0001cdd4);
    if ((u16)FontGetMessageWidth(msg) > width) {
        f = (f32)width * z / (f32)(u16)FontGetMessageWidth(msg);
    } else {
        f = z;
    }
    PSMTXScale(scale, f, z, z);
    PSMTXConcat(trans, scale, out);
    FontDrawMessageMtx(out, msg);
}
static s32 fontlenpitch(s8* text, s32 pitch) {
    if (pitch != 0) {
        s8* str = text;
        s32 count = 0;

        while (*str) {
            if (_ismbblead(*str)) {
                count++;
                str++;
            } else {
                count++;
            }
            str++;
        }
        return count * pitch;
    } else {
        return (u16)FontGetMessageWidth((char*)text);
    }
}

const f32 float_1000_gor_0001cd80 = 1000.0f;
const f32 zero_gor_0001cdd4 = 0.0f;
const f64 double_to_int_mask_gor_0001d038 = 4503599627370496.0;
const f64 double_to_int_gor_0001d040 = 4503601774854144.0;
// clang-format on
