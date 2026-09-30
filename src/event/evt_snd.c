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

USER_FUNC(evt_snd_bgmon) {
    //psndBGMOn((u32)evt->args[0], (const char*)evt->args[1]);
    return EVT_RETURN_DONE;
}



USER_FUNC(evt_snd_sfxon_3d) {
    s32* args = event->args;
    Vec position;
    const char* name;
    s32 index;
    
    name = (const char*)args[0];
    position.x = evtGetFloat(event, args[1]);
    position.y = evtGetFloat(event, args[2]);
    position.z = evtGetFloat(event, args[3]);
    index = args[4];
    if ((index == 0) || ((u32)(index + 0x0EE70000) == 0x4D80U)) {
        //psndSFXOn_3D(name, &position);
    } else {
        //evtSetValue(event, index, psndSFXOn_3D(name, &position));
    }
    return EVT_RETURN_DONE;
}
