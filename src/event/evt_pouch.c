/* "evt_pouch" - Pouch-related Event Script Functions
 * Status: In progress.
 */
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "mario/mario_party.h"


// Local prototypes not yet in a shared header.
extern void partyLeft(PartyId partnerId);
extern s32 pouchRemoveItemIndex(s32 id, s32 index);

USER_FUNC(evt_pouch_get_yoshiname) {
	s32* args = event->args;

	evtSetValue(event, args[0], (s32)pouchGetYoshiName());
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_set_yoshiname) {
	pouchSetYoshiName((char*)evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_open_mail) {
	pouchOpenMail(evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_receive_mail) {
	pouchReceiveMail(evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_majinai_set) {
	pouchMajinaiInit(evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_get_equipbadgecnt) {
	s32* args = event->args;

	evtSetValue(event, args[0], pouchGetEquipBadgeCnt());
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_get_havebadgecnt) {
	s32* args = event->args;

	evtSetValue(event, args[0], pouchGetHaveBadgeCnt());
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_get_haveitemcnt) {
	s32* args = event->args;

	evtSetValue(event, args[0], pouchGetHaveItemCnt());
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_all_party_recovery) {
	PouchData* pouch = pouchGetPtr();
	s32 i;

	for (i = 0; i < 8; i++) {
		pouch->partyData[i].currentHP = pouch->partyData[i].maximumHP;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_mario_recovery) {
	pouchSetHP(pouchGetMaxHP());
	pouchSetFP(pouchGetMaxFP());
	pouchSetAP(pouchGetMaxAP());
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_party_left) {
	partyLeft((PartyId)evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_party_join) {
	partyJoin((PartyId)evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_set_ap) {
	pouchSetAP(evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_get_max_ap) {
	s32* args = event->args;

	evtSetValue(event, args[0], pouchGetMaxAP());
	return EVT_RETURN_DONE;
}

USER_FUNC(L_evt_pouch_set_hp) {
	pouchSetHP(evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(L_evt_pouch_get_hp) {
	s32* args = event->args;

	evtSetValue(event, args[0], pouchGetHP());
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_check_item) {
	s32* args = event->args;

	evtSetValue(event, args[1], pouchCheckItem(evtGetValue(event, args[0])));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_remove_keepitem) {
	s32* args = event->args;
	s32 value0 = evtGetValue(event, args[0]);
	s32 value1 = evtGetValue(event, args[1]);

	if (pouchRemoveKeepItem(value0, value1)) {
		evtSetValue(event, args[2], 0);
	} else {
		evtSetValue(event, args[2], -1);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_add_keepitem) {
	s32* args = event->args;

	if (pouchAddKeepItem(evtGetValue(event, args[0]))) {
		evtSetValue(event, args[1], 0);
	} else {
		evtSetValue(event, args[1], -1);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(N_evt_pouch_remove_item_index) {
	s32* args = event->args;
	s32 value0 = evtGetValue(event, args[0]);
	s32 value1 = evtGetValue(event, args[1]);

	if (pouchRemoveItemIndex(value0, value1) == 1) {
		pouchReviseMarioParam();
		pouchRevisePartyParam();
		evtSetValue(event, args[2], 0);
	} else {
		evtSetValue(event, args[2], -1);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_remove_item) {
	s32* args = event->args;

	if (pouchRemoveItem(evtGetValue(event, args[0])) == 1) {
		pouchReviseMarioParam();
		pouchRevisePartyParam();
		evtSetValue(event, args[1], 0);
	} else {
		evtSetValue(event, args[1], -1);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_add_item) {
	s32* args = event->args;

	if (pouchGetItem(evtGetValue(event, args[0])) == 1) {
		evtSetValue(event, args[1], 0);
	} else {
		evtSetValue(event, args[1], -1);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_get_starstone) {
	pouchGetStarStone(evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_add_starpiece) {
	pouchAddStarPiece(evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_get_starpiece) {
	s32* args = event->args;

	evtSetValue(event, args[0], pouchGetStarPiece());
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_set_supercoin) {
	pouchSetSuperCoin(evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_get_supercoin) {
	s32* args = event->args;

	evtSetValue(event, args[0], pouchGetSuperCoin());
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_set_coin) {
	pouchSetCoin(evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_add_coin) {
	pouchAddCoin(evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_pouch_get_coin) {
	s32* args = event->args;

	evtSetValue(event, args[0], pouchGetCoin());
	return EVT_RETURN_DONE;
}
