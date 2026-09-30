#include <dolphin/pad.h>
#include <demo/DEMOPad.h>
#include "sdk/DEMOPad.h"

static u32 PadChanMask[4] = {
    PAD_CHAN0_BIT,
    PAD_CHAN1_BIT,
    PAD_CHAN2_BIT,
    PAD_CHAN3_BIT,
};

u32 DemoNumValidPads;

static PADStatus Pad[4];
DEMOPadStatus DemoPad[4];

void DEMOPadCopy(PADStatus* pad, DEMOPadStatus* dmpad) {
    u16 dirs;

    if (pad->err != PAD_ERR_TRANSFER) {
        dirs = 0;
        if (pad->stickX < -0x30) {
            dirs |= DEMO_STICK_LEFT;
        }
        if (pad->stickX > 0x30) {
            dirs |= DEMO_STICK_RIGHT;
        }
        if (pad->stickY < -0x30) {
            dirs |= DEMO_STICK_DOWN;
        }
        if (pad->stickY > 0x30) {
            dirs |= DEMO_STICK_UP;
        }
        if (pad->substickX < -0x30) {
            dirs |= DEMO_SUBSTICK_LEFT;
        }
        if (pad->substickX > 0x30) {
            dirs |= DEMO_SUBSTICK_RIGHT;
        }
        if (pad->substickY < -0x30) {
            dirs |= DEMO_SUBSTICK_DOWN;
        }
        if (pad->substickY > 0x30) {
            dirs |= DEMO_SUBSTICK_UP;
        }

        dmpad->dirsNew = (dirs & (dmpad->dirs ^ dirs));
        dmpad->dirsReleased = (dmpad->dirs & (dmpad->dirs ^ dirs));
        dmpad->dirs = dirs;
        dmpad->buttonDown = (pad->button & (dmpad->pst.button ^ pad->button));
        dmpad->buttonUp = (dmpad->pst.button & (dmpad->pst.button ^ pad->button));
        dmpad->stickDeltaX = (pad->stickX - dmpad->pst.stickX);
        dmpad->stickDeltaY = (pad->stickY - dmpad->pst.stickY);
        dmpad->substickDeltaX = (pad->substickX - dmpad->pst.substickX);
        dmpad->substickDeltaY = (pad->substickY - dmpad->pst.substickY);
        dmpad->pst = *pad;
    } else {
        dmpad->dirsNew = dmpad->dirsReleased = 0;
        dmpad->buttonDown = dmpad->buttonUp = 0;
        dmpad->stickDeltaX = dmpad->stickDeltaY = 0;
        dmpad->substickDeltaX = dmpad->substickDeltaY = 0;
    }
}

void DEMOPadRead(void) {
    s32 i;
    u32 ResetReq = 0;

    PADRead(&Pad[0]);
    PADClamp(&Pad[0]);

    DemoNumValidPads = 0;

    for (i = 0; i < 4; i++) {
        if (Pad[i].err == PAD_ERR_NONE || Pad[i].err == PAD_ERR_TRANSFER) {
            DemoNumValidPads++;
        } else if (Pad[i].err == PAD_ERR_NO_CONTROLLER) {
            ResetReq |= PadChanMask[i];
        }

        DEMOPadCopy(&Pad[i], &DemoPad[i]);
    }

    if (ResetReq != 0) {
        PADReset(ResetReq);
    }
}

void DEMOPadInit(void) {
    s32 i;

    PADInit();

    for (i = 0; i < 4; i++) {
        DemoPad[i].pst.button = 0;
        DemoPad[i].pst.stickX = 0;
        DemoPad[i].pst.stickY = 0;
        DemoPad[i].pst.substickX = 0;
        DemoPad[i].pst.substickY = 0;
        DemoPad[i].pst.triggerLeft = 0;
        DemoPad[i].pst.triggerRight = 0;
        DemoPad[i].pst.analogA = 0;
        DemoPad[i].pst.analogB = 0;
        DemoPad[i].pst.err = 0;
        DemoPad[i].buttonDown = 0;
        DemoPad[i].buttonUp = 0;
        DemoPad[i].dirs = 0;
        DemoPad[i].dirsNew = 0;
        DemoPad[i].dirsReleased = 0;
        DemoPad[i].stickDeltaX = 0;
        DemoPad[i].stickDeltaY = 0;
        DemoPad[i].substickDeltaX = 0;
        DemoPad[i].substickDeltaY = 0;
    }
}
