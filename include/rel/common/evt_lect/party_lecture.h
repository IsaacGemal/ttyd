// lect_itemget_kesan, lect_npc_balloon_on_off, efffukidashichange_cam, lect_party_ido_sub and lect_party_ido_sum: the
// helpers of the units with a partner lecture (*_party_lecture). See rel/common/evt_lect.h.
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "driver/npcdrv.h"
#include "eff/eff_fukidashi.h"
#include "eff/eff_mario_balloon.h"
#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "party.h"

#include <dolphin/os.h>

static USER_FUNC(lect_itemget_kesan) {
    s32* args = event->args;
    PartyEntry* party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    f32 dist = evtGetFloat(event, args[1]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec pos;
    Vec dir;

    if (party == NULL) {
        evtSetFloat(event, args[2], 0.0f);
        evtSetFloat(event, args[3], 0.0f);
        evtSetFloat(event, args[4], 0.0f);
        return EVT_RETURN_DONE;
    }
    pos = party->position;
    pos.y += party->height / 2.0f;
    PSVECSubtract(&pos, &cam->cameraPos, &dir);
    PSVECNormalize(&dir, &dir);
    PSVECScale(&dir, &dir, dist);
    PSVECAdd(&cam->cameraPos, &dir, &dir);
    evtSetFloat(event, args[2], dir.x);
    evtSetFloat(event, args[3], dir.y);
    evtSetFloat(event, args[4], dir.z);
    return EVT_RETURN_DONE;
}

static USER_FUNC(lect_npc_balloon_on_off) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    NpcEntry* npc = evtNpcNameToPtr(event, (const char*)name);

    if (evtGetValue(event, args[1])) {
        EffMarioBalloonData* data;

        eff = effMarioBalloonEntry(1, npc);
        data = eff->userdata;
        data->cameraId = 7;
    } else {
        effDelete(eff);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(efffukidashichange_cam) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 camId = evtGetValue(event, args[1]);
    EffectEntry* effect = effNameToPtr((const char*)name);
    effFukidashiData* data = effect->userdata;

    data->camId = camId;
    return EVT_RETURN_DONE;
}

static f32 lect_party_ido_sub(f32 elapsed, f32 duration, f32 start, f32 end) {
    f32 t = duration - elapsed;

    return (t * start + elapsed * end) / duration;
}

static USER_FUNC(lect_party_ido_sum) {
    s32* args = event->args;
    PartyEntry* party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    s32 duration = evtGetValue(event, args[1]);
    s32 elapsed;

    if (isFirstCall) {
        wwstart = event->lifetime;
        evtSetFloat(event, LW(3), party->position.x);
        evtSetFloat(event, LW(4), party->position.y);
        evtSetFloat(event, LW(5), party->position.z);
        return EVT_RETURN_BLOCK;
    }
    elapsed = (s32)((u64)((s32)event->lifetime - (s32)wwstart) / (OS_TIMER_CLOCK / 1000));
    if (elapsed > duration) {
        elapsed = duration;
    }
    party->position.x = lect_party_ido_sub(elapsed, duration, evtGetFloat(event, LW(3)), evtGetFloat(event, LW(0)));
    party->position.y = lect_party_ido_sub(elapsed, duration, evtGetFloat(event, LW(4)), evtGetFloat(event, LW(1)));
    party->position.z = lect_party_ido_sub(elapsed, duration, evtGetFloat(event, LW(5)), evtGetFloat(event, LW(2)));
    if (elapsed >= duration) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}
