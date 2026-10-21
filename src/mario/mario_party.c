// the best Mario Party, partners with actual abilities in Paper Mario,
// something Nintendo's forgotten about
#include "mario/mario_party.h"
#include "mario/mario.h"
#include "party.h"
#include "party/party_yoshi.h"
#include "driver/npcdrv.h"

s32 marioUseParty(void) {
    MarioWork* wp = marioGetPtr();
    PartyEntry* party;
    s8* timer = &wp->partyUseTimer;

    if (N_marioChkUseParty()) {
        *timer = 0;
        wp->trigFlags &= ~MARIO_TRIG_FLAG_USE_PARTNER;
        return 0;
    }
    if ((wp->trigFlags & MARIO_TRIG_FLAG_USE_PARTNER) && marioKeyOffChk()) {
        *timer = 0;
        wp->trigFlags &= ~MARIO_TRIG_FLAG_USE_PARTNER;
        return 0;
    }
    party = partyGetPtr(marioGetPartyId());
    if (party != NULL && party->field_34 == 3) {
        return 0;
    }
    if (wp->currPartySlotId[0] < 0) {
        wp->trigFlags &= ~MARIO_TRIG_FLAG_USE_PARTNER;
        return 0;
    }
    if (wp->flags & MARIO_FLAG_IS_8_BIT) {
        return 0;
    }
    if (N_fbatPreventMarioEventChk()) {
        *timer = 0;
        wp->trigFlags &= ~MARIO_TRIG_FLAG_USE_PARTNER;
        return 0;
    }
    if ((wp->flags & (MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING)) || wp->airTimer != 0) {
        *timer = 0;
        wp->trigFlags &= ~MARIO_TRIG_FLAG_USE_PARTNER;
        return 0;
    }
    if (wp->currMotionId != MARIO_MOTION_STAY && wp->currMotionId != MARIO_MOTION_WALK
        && wp->currMotionId != MARIO_MOTION_DASH) {
        wp->trigFlags &= ~MARIO_TRIG_FLAG_USE_PARTNER;
        return 0;
    }
    if ((wp->dispFlags & (1 << 24)) || party->field_37 == 1) {
        return 0;
    }
    party = partyGetPtr(marioGetPartyId());
    if (party->flags & PARTY_FLAG_IS_BEING_USED) {
        return 0;
    }
    party = partyGetPtr(marioGetPartyId());
    if (party == NULL) {
        return 0;
    }
    if (party->currentMemberId == PARTY_MEMBER_GOOMBELLA && !(party->flags & PARTY_FLAG_IS_GROUNDED)) {
        return 0;
    }
#ifdef MUST_MATCH
    // The explicit (s8) makes MWCC sign-extend into r0 for the comparison as the original does.
    if ((s8)party->currentMemberId == PARTY_MEMBER_YOSHI && yoshiGetStatus() != 0) {
#else
    if (party->currentMemberId == PARTY_MEMBER_YOSHI && yoshiGetStatus() != 0) {
#endif
        return 0;
    }
    if (!(wp->trigFlags & MARIO_TRIG_FLAG_USE_PARTNER) && (wp->buttonNew & 0x400)) {
        wp->trigFlags |= MARIO_TRIG_FLAG_USE_PARTNER;
        *timer = 5;
    }
    return 0;
}

s32 unk_8013be14(void) {
    MarioWork* wp = marioGetPtr();
    s8* timer = &wp->partyUseTimer;

    if (wp->trigFlags & MARIO_TRIG_FLAG_USE_PARTNER) {
        if (marioKeyOffChk() != 0) {
            *timer = 0;
            wp->trigFlags &= ~MARIO_TRIG_FLAG_USE_PARTNER;
            return 0;
        }
        if (--*timer <= 0) {
            *timer = 0;
            wp->trigFlags &= ~MARIO_TRIG_FLAG_USE_PARTNER;
            return partyDoWork();
        }
    }
    return 0;
}

// Kills the party member in one of Mario's two party slots; marioPartyKill is this for both slots.
static inline void marioPartyKillSlot(s32 slot) {
    MarioWork* mario = marioGetPtr();

    if (mario->currPartySlotId[slot] >= 0) {
        partyKill(mario->currPartySlotId[slot]);
        mario->currPartySlotId[slot] = -1;
    }
}

s32 marioPartyEntry(s32 memberId) {
    MarioWork* wp;
    s32 id;

    wp = marioGetPtr();
    if (memberId < 8) {
        if (!partyChkJoin(memberId)) {
            return -1;
        }
        if (wp->currPartySlotId[0] >= 0) {
            marioPartyKillSlot(0);
        }
        id = partyEntry(memberId);
        if (id >= 0) {
            wp->currPartySlotId[0] = id;
            partyHello(partyGetPtr(id));
        }
    } else {
        if (wp->currPartySlotId[1] >= 0) {
            marioPartyKillSlot(1);
        }
        id = partyEntry2(memberId);
        if (id >= 0) {
            wp->currPartySlotId[1] = id;
            partyHello(partyGetPtr(id));
        }
    }
    return id;
}

s32 marioPartyHello(s32 memberId) {
    MarioWork* wp;
    s32 id;

    id = -1;
    wp = marioGetPtr();
    if (memberId < 8) {
        if (!partyChkJoin(memberId)) {
            return -1;
        }
        if (wp->currPartySlotId[0] < 0) {
            id = partyEntryHello(memberId);
            if (id >= 0) {
                wp->currPartySlotId[0] = id;
            }
        }
    } else {
        id = partyEntry2Hello(memberId);
        if (id >= 0) {
            wp->currPartySlotId[1] = id;
        }
    }
    return id;
}

s32 marioPartyGoodbye(void) {
    s32 goodbye;
    s8 memberId;

    goodbye = 0;
    memberId = marioGetPtr()->currPartySlotId[0];
    if (memberId >= 0) {
        partyGoodbye(memberId);
        goodbye = 1;
    }
    return goodbye;
}

void marioPartyKill(void) {
    marioPartyKillSlot(0);
    marioPartyKillSlot(1);
}

s32 marioGetParty(void) {
    s32 id;

    id = marioGetPartyId();
    if (id >= 0) {
        return partyGetPtr(id)->currentMemberId;
    } else {
        return 0;
    }
}

s32 marioGetPartyId(void) {
    MarioWork* wp = marioGetPtr();
    PartyEntry* party;
    s32 partyId;

    partyId = wp->currPartySlotId[0];
    if (partyId >= 0) {
        party = partyGetPtr(partyId);
        if ((party != NULL) && !(party->flags & PARTY_FLAG_IS_PARTNER)) {
            return partyId;
        }
    }
    partyId = wp->currPartySlotId[1];
    if (partyId >= 0) {
        party = partyGetPtr(partyId);
        if ((party != NULL) && !(party->flags & PARTY_FLAG_IS_PARTNER)) {
            return partyId;
        }
    }
    return -1;
}

s32 marioGetExtraPartyId(void) {
    MarioWork* wp = marioGetPtr();
    PartyEntry* party;
    s32 partyId;

    partyId = wp->currPartySlotId[0];
    if (partyId >= 0) {
        party = partyGetPtr(partyId);
        if ((party != NULL) && (party->flags & PARTY_FLAG_IS_PARTNER)) {
            return partyId;
        }
    }

    partyId = wp->currPartySlotId[1];
    if (partyId >= 0) {
        party = partyGetPtr(partyId);
        if ((party != NULL) && (party->flags & PARTY_FLAG_IS_PARTNER)) {
            return partyId;
        }
    }
    return -1;
}

void partyJoin(PartyId partnerId) {
    if (partnerId < 8) {
        pouchGetPtr()->partyData[partnerId].flags |= 1; // obtained
    }
}

void partyLeft(PartyId partnerId) {
    if (partnerId < 8) {
        pouchGetPtr()->partyData[partnerId].flags &= ~1;
    }
}

s32 partyChkJoin(s32 memberId) {
    s32 joined;
    s32 t;

    joined = 0;
    if (memberId < 8) {
        t = pouchGetPtr()->partyData[memberId].flags & 1;
        joined = t;
    }
    return joined;
}

s32 partyGetHp(s32 partyId) {
    s32 hp = 0;

    if (partyId < 8) {
        hp = pouchGetPtr()->partyData[partyId].currentHP;
    }
    return hp;
}

s32 partyGetTechLv(s32 partyId) {
    s32 level = 0;

    if (partyId < 8) {
        level = pouchGetPtr()->partyData[partyId].techLevel;
    }
    return level;
}
