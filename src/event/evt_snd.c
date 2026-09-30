#include "evt/evt_snd.h"
#include "pmario_sound.h"
#include "sound.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "mario/mario_pouch.h"
#include "seq/seq_gameover.h"
#include "driver/camdrv.h"
#include "win/win_main.h"

void seq_gameOverInit(SequenceWork* work) {
    winReInit();
    camDispOn(4);
    psndStopAllFadeOut();
    psndSetReverb(0);
    L_psndBGM_stop();
    psndClearFlag(0x80);
    psndClearFlag(0x20);
    psndClearFlag(0x40);
    N_pouchUnEquipBadgeID(0x143);
    marioStSystemLevel(4);
}

void seq_gameOverExit(SequenceWork* work) {
    psndStopAllFadeOut();
    marioStSystemLevel(0);
}

USER_FUNC(N_evt_snd_bgm_unpause) {
    if (psbgm[0].streamId != 0xFFFFFFFF) {
        sound.streams[psbgm[0].streamId].flags &= ~0x8000;
    }
    return EVT_RETURN_DONE;
}
