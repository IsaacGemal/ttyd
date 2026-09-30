#include "cam_shift.h"
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

void camShiftPostMain(void) {
    wp->unk_18 = gp->renderTime;
}
