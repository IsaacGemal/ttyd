#include "party/party_kuribo.h"
#include "evt/evt_cmd.h"
#include "mario/mario_party.h"
#include "party.h"
#include "pmario_sound.h"

//.sbss
u32 msg_ep; //unknown since it only gets written to once and never used

BOOL christineGetStatus(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (!party || party->currentMemberId != PARTY_MEMBER_GOOMBELLA || !(party->flags & 0x100)) {
        return FALSE;
    }
    return TRUE;
}

// NONMATCHING (7.49%): 1 instruction differs, 415 missing, 1 extra, plus register differences.
USER_FUNC(pre_kurio_use) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (!party) {
        return EVT_RETURN_DONE;
    }

    if (party->flags2 & 0x4000000) {
        return EVT_RETURN_DONE;
    }

    psndSetFlag(0x80);
    party->flags |= 0x04000000;
    party->flags |= 0x02000000;

    if (event->lwData[0]) {
        event->lwData[0] += 8;
        return EVT_RETURN_DONE;
    }

    
}

// .sdata2 constants
extern const f32 float_24_8041b45c;
extern const f32 float_26_8041b460;

void kuribo_init_800C23B4(PartyEntry* party) {
    party->field_33 = 1;
    party->field_32 = 0;
    *(f32*)((u8*)party + 0xF0) = float_24_8041b45c;
    *(f32*)((u8*)party + 0xF4) = float_26_8041b460;
    msg_ep = 0;
}

void kuribo_use_post(PartyEntry* party) {
    party->flags &= ~0x04000000;
    party->flags &= ~0x02000000;
}
