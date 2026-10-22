#include "evt_yuutitle.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "driver/dispdrv.h"
#include "driver/msgdrv.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
void FontDrawColor(GXColor color);
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

// The fade factor is a real local, not folded into its use: with copy propagation off it keeps its own
// (low) variable number, so MWCC colours the `field_10 * (1 - t)` product first (f0) and the fade gets f1, as
// in the original. Each lerp is written `to * t + from * (1 - t)`, so fp_contract fuses the `t` product
// into the fmadds as retail does.
void evt_yuutitle_disp_func(CameraId cameraId, void* param) {
    YuuTitleWork* wp = param;
    f32 scale = 2.0f;
    f32 halfWidth;
    f32 alphaF;
    f32 t;
    f32 fade;

    if (wp->alpha < 0xFF) {
        wp->alpha += 6;
        if (wp->alpha > 0xFF) {
            wp->alpha = 0xFF;
        }
        alphaF = (f32)wp->alpha;
        t = alphaF / 255.0f;
        wp->field_08 = wp->field_18 * t + wp->field_10 * (1.0f - t);
        fade = 0.8f * t + 0.2f;
        scale *= fade;
        wp->field_0c = wp->field_1c * t + wp->field_14 * (1.0f - t);
    }
    // MWCC folds `/ 2.0f` into `* 0.5f` but keeps the product's operand order.
    halfWidth = scale * (f32)FontGetMessageWidth(wp->msg) / 2.0f;
    FontDrawStart();
    FontDrawScale(scale);
    FontDrawEdge();
    FontDrawColor((GXColor){ 0xFF, 0xFF, 0xFF, wp->alpha });
    FontDrawString(wp->field_08 - halfWidth, wp->field_0c, wp->msg);
    FontDrawEdgeOff();
}

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
    wp->msg = msgSearch((const char*)evtGetValue(event, args[2]));
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
