#include "cam_shift.h"
#include "mario/mariost.h"

//.bss
static CamShiftWork work;

//.sdata
static CamShiftWork* wp = &work;

void camShiftPostMain(void) {
    wp->unk_18 = gp->renderTime;
}
