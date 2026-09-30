#include "cam_shift.h"
#include "cam_road.h"
#include "system.h"
#include "driver/camdrv.h"
#include "mario/mario.h"
#include "mario/mariost.h"

//.bss
static CamShiftWork work;

//.sdata
static CamShiftWork* wp = &work;

void camShiftInit(void) {
    wp->unk_00 = 0.0f;
    wp->unk_04 = 0.0f;
    wp->unk_08 = 0.0f;
    wp->unk_0C = 0.0f;
    wp->unk_10 = 0.0f;
    wp->unk_20 = 300.0f;
    wp->unk_24 = 3.0f;
    wp->unk_28 = 0.002f;
    wp->unk_2C = 0.0f;
    wp->unk_30 = 0.0f;
}

void camShiftMain(CameraEntry* camera, MarioWork* mario, f32* out) {
    f32 rate;
    f32 stick;
    s32 flag;

    flag = 0;
    rate = camRoadGetCurrentShiftXRate();
    if (camera->flags & 0x20) {
        flag = 1;
    }
    if (flag) {
        wp->unk_08 = 1.0f;
        stick = 0.0f;
        wp->unk_04 = 0.0f;
        wp->unk_10 = 1.0f;
    } else {
        stick = (f32)(s8)mario->wStickDir1;
        if (stick > 0.0f) {
            stick = 25.0f;
        }
        if (stick < 0.0f) {
            stick = -25.0f;
        }
        if (mario->position.x == wp->unk_2C && mario->position.z == wp->unk_30) {
            stick = 0.0f;
        }
        wp->unk_2C = mario->position.x;
        wp->unk_30 = mario->position.z;
    }
    if (stick) {
        if (stick < 0.0f) {
            if (wp->unk_0C > 0.0f) {
                wp->unk_0C = getV60FPS(stick, wp->unk_18, gp->renderTime);
            } else {
                wp->unk_0C += getV60FPS(stick, wp->unk_18, gp->renderTime);
            }
            if (wp->unk_0C <= -wp->unk_20) {
                wp->unk_10 = 1.0f;
                if (wp->unk_04 > 0.0f) {
                    wp->unk_08 = 0.0f;
                }
                wp->unk_04 = -rate;
                wp->unk_0C = -wp->unk_20;
            }
        } else {
            if (wp->unk_0C < 0.0f) {
                wp->unk_0C = getV60FPS(stick, wp->unk_18, gp->renderTime);
            } else {
                wp->unk_0C += getV60FPS(stick, wp->unk_18, gp->renderTime);
            }
            if (wp->unk_0C >= wp->unk_20) {
                wp->unk_10 = 1.0f;
                if (wp->unk_04 < 0.0f) {
                    wp->unk_08 = 0.0f;
                }
                wp->unk_04 = rate;
                wp->unk_0C = wp->unk_20;
            }
        }
    }
    if (wp->unk_10) {
        wp->unk_08 += getV60FPS(wp->unk_28, wp->unk_18, gp->renderTime);
        if (wp->unk_08 > 1.0f) {
            wp->unk_08 = 1.0f;
        }
    }
    if (wp->unk_04 - wp->unk_00 == 0.0f) {
        wp->unk_08 = 0.0f;
        wp->unk_10 = 0.0f;
    }
    {
        f32 dist;
        f32 step;

        step = wp->unk_08 * (dist = wp->unk_04 - wp->unk_00);

        if (dist > 0.0f) {
            if (dist < 0.0001f) {
                step = dist;
            }
            if (step > wp->unk_24) {
                step = wp->unk_24;
            }
        } else {
            if (dist > -0.0001f) {
                step = dist;
            }
            if (step < -wp->unk_24) {
                step = -wp->unk_24;
            }
        }
        if (stick || flag) {
            wp->unk_00 += getV60FPS(step, wp->unk_18, gp->renderTime);
            if (wp->unk_04 < 0.0f) {
                if (wp->unk_00 < wp->unk_04) {
                    wp->unk_00 = wp->unk_04;
                }
            } else {
                if (wp->unk_00 > wp->unk_04) {
                    wp->unk_00 = wp->unk_04;
                }
            }
        } else {
            wp->unk_08 = 0.0f;
        }
    }
    *out = wp->unk_00;
}

void camShiftPostMain(void) {
    wp->unk_18 = gp->renderTime;
}

void camShiftReset(void) {
    wp->unk_00 = 0.0f;
    wp->unk_04 = 0.0f;
    wp->unk_08 = 0.0f;
    wp->unk_0C = 0.0f;
    wp->unk_10 = 0.0f;
    wp->unk_20 = 300.0f;
    wp->unk_24 = 3.0f;
    wp->unk_28 = 0.002f;
    wp->unk_2C = 0.0f;
    wp->unk_30 = 0.0f;
}
