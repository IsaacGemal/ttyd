#include "evt_yuutitle.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "driver/dispdrv.h"
#include "driver/msgdrv.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "memory.h"

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

const GXColor dat_yuu_00014d98 = { 0xFF, 0xFF, 0xFF, 0x00 };

// `#pragma opt_common_subs off` is required for the register allocation: with CSE on, the
// two `(1.0f - t)` uses are merged and MWCC puts `1.0f`/`wp->field_18` in different FP
// registers, which changes the whole block.  Same reason as evt_badgeshop.c/evt_sub.c.
#pragma opt_common_subs off
// Only MWCC's register allocation/scheduling differs.  The `wp->field_08` lerp and the `0.8f * t + 0.2f`
// fade both compute in registers that are one apart from the target: the original keeps the fade factor in f1
// and the `field_10 * (1-t)` product in f0, while this build does the opposite (`fmadds f0, f1, f5, f0` /
// `fmuls f1, f3, f2`), which also swaps the dependent `fmuls f31` / `fmadds f0, f4, f5, ...`.  Writing the
// half-width factor as a division rather than `* 0.5f` does reproduce the original's operand order
// (`fmuls f30, f1, f0`, since MWCC keeps `x / 2.0f` as `x * 0.5` instead of forcing the constant into FRA),
// which is where the extra 0.10% comes from.  The tail also hoists `lwz r3, wp->msg` one instruction early
// (original: lfs 0x8, lfs 0xc, fsubs, lwz r3; ours: lfs 0x8, lwz r3, fsubs, lfs 0xc).  Tried:
// every statement/temporary order, explicit/inline fade and lerp helpers, `opt_lifetimes`/`opt_propagation`/
// `optimize_for_size`/`defer_codegen` pragmas, and the `/ 2.0f` shape on the blend; none changes the
// factor/product allocation or the tail.  Same class as shadowdrv.c:cylinder.
// NONMATCHING (95.75%): fade/first-lerp register allocation and FontDrawString tail scheduling differ.
void evt_yuutitle_disp_func(CameraId cameraId, void* param) {
    YuuTitleWork* wp = param;
    f32 scale = 2.0f;
    f32 halfWidth;
    f32 temp;
    f32 alphaF;
    GXColor c;
    GXColor color;
    f32 t;

    if (wp->alpha < 0xFF) {
        wp->alpha += 6;
        if (wp->alpha > 0xFF) {
            wp->alpha = 0xFF;
        }
        alphaF = (f32)wp->alpha;
        t = alphaF / 255.0f;
        temp = wp->field_10 * (1.0f - t);
        wp->field_08 = temp + wp->field_18 * t;
        scale *= 0.8f * t + 0.2f;
        temp = wp->field_14 * (1.0f - t);
        wp->field_0c = temp + wp->field_1c * t;
    }
#ifdef MUST_MATCH
    // MWCC folds `/ 2.0f` into `* 0.5f` but keeps the product's operand order.
    halfWidth = scale * (f32)FontGetMessageWidth(wp->msg) / 2.0f;
#else
    halfWidth = scale * (f32)FontGetMessageWidth(wp->msg) * 0.5f;
#endif
    FontDrawStart();
    FontDrawScale(scale);
    FontDrawEdge();
    color = dat_yuu_00014d98;
    color.a = wp->alpha;
    c = color;
    FontDrawColor(&c);
    FontDrawString(wp->msg, wp->field_08 - halfWidth, wp->field_0c);
    FontDrawEdgeOff();
}
#pragma opt_common_subs on
USER_FUNC(evt_yuutitle_main_func) {
    YuuTitleWork* wp = (YuuTitleWork*)event->lwData[0];

    if (wp->flags & 1) {
        wp->flags |= 2;
        return EVT_RETURN_DONE;
    }
    dispEntry(CAMERA_2D, 1000.0f, 0, evt_yuutitle_disp_func, wp);
    return EVT_RETURN_BLOCK;
}

static EVT_BEGIN(main_evt)
    CALL(evt_yuutitle_main_func)
    RETURN
EVT_END()

static YuuTitleWork* _wp;
USER_FUNC(evt_yuutitle_alloc) {
    s32* args = event->args;
    YuuTitleWork* wp = _mapAlloc(0x28);
    f32 temp;

    memset(wp, 0, 0x28);
    _wp = wp;
    wp->field_18 = evtGetFloat(event, args[0]);
    wp->field_1c = evtGetFloat(event, args[1]);
    wp->msg = (const char*)msgSearch((const char*)evtGetValue(event, args[2]));
    temp = 200.0f + wp->field_18;
    wp->field_08 = temp;
    wp->field_10 = temp;
    temp = 200.0f + wp->field_1c;
    wp->field_0c = temp;
    wp->field_14 = temp;
    wp->alpha = 0;
    wp->entry = evtEntry(main_evt, 0, 0);
    wp->entry->lwData[0] = (s32)wp;
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_yuutitle_free) {
    YuuTitleWork* wp = _wp;

    if (wp == NULL) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        wp->flags |= 1;
    }
    if (!(wp->flags & 2)) {
        return EVT_RETURN_BLOCK;
    }
    _mapFree(_wp);
    _wp = NULL;
    return EVT_RETURN_DONE;
}
// clang-format on
