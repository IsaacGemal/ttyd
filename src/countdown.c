#include "countdown.h"

#include "mario/mariost.h"
#include "pmario_sound.h"
#include <string.h>

extern GlobalWork* gp;

//.sbss
static CountdownWork* wp;

void countDownInit(void) {
    wp = &gp->countdown;
    memset(&gp->countdown, 0, sizeof(gp->countdown));
}

// NONMATCHING (1.47%): empty placeholder, not decompiled yet.
void countDownSaveReStart(void) {
}

void countDownSetRestartTime(s32 time) {
    // offset 0x1C is unnamed in CountdownWork (inside field_0x2 padding)
    *(s32*)((u8*)wp + 0x1C) = time;
}

// NONMATCHING (0.72%): empty placeholder, not decompiled yet.
void countDownMain(void) {
}

s32 countDownGetStatus(void) {
    CountdownWork* work = wp;
    return !(work->flags & 1) ? -1 : (work->flags >> 1) & 1;
}

void countDownEnd(void) {
    if ((wp->flags & 1) != 0) {
        wp->unk30 = 2;
    }
}

void countDownStop(void) {
    wp->flags |= 8;
    // offset 0x38 is unnamed in CountdownWork (inside field_0x34 padding)
    psndSFXOff(*(s32*)(wp->field_0x34 + 4));
}

void countDownFlagOn(u16 flag) {
    wp->flags |= flag;
}

void countDownFlagOff(u16 flag) {
    wp->flags &= ~flag;
}
