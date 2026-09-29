#include "main.h"

#include "mario/mariost.h"
#include "sdk/DEMOInit.h"

extern GlobalWork* gp;

const char R_build_date[] = "Jun 17 2004";
const char R_build_time[12] = "17:20:01";

int main(void) {
    OSTick base_tick;

    marioStInit();
    marioStMain();
    while (1) {
        base_tick = OSGetTick();
        DEMOBeforeRender();
        marioStDisp();
        marioStMain();
        gp->mDeltaGame = OSGetTick() - base_tick;
        DEMODoneRender();
        gp->mDeltaRender = OSGetTick() - base_tick;
    }

    return 0;
}
