/* "evt_pouch" - Pouch-related Event Script Functions
 * Status: In progress.
 */
// mario_pouch.h declares these accessors with s16 parameters/returns, but the
// original evt_pouch.c used them through s32, so MWCC must not insert spurious
// extsh conversions. Hide the header prototypes and redeclare them wider.
#define pouchGetMaxHP     pouchGetMaxHP_decl
#define pouchSetHP        pouchSetHP_decl
#define pouchGetMaxFP     pouchGetMaxFP_decl
#define pouchSetFP        pouchSetFP_decl
#define pouchGetMaxAP     pouchGetMaxAP_decl
#define pouchSetAP        pouchSetAP_decl
#define pouchGetHP        pouchGetHP_decl
#define pouchGetStarPiece pouchGetStarPiece_decl
#define pouchAddStarPiece pouchAddStarPiece_decl
#define pouchSetSuperCoin pouchSetSuperCoin_decl
#define pouchGetSuperCoin pouchGetSuperCoin_decl
#define pouchSetCoin      pouchSetCoin_decl
#define pouchAddCoin      pouchAddCoin_decl
#define pouchGetCoin      pouchGetCoin_decl
#define pouchAddKeepItem  pouchAddKeepItem_decl

#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "mario/mario_party.h"

#undef pouchGetMaxHP
#undef pouchSetHP
#undef pouchGetMaxFP
#undef pouchSetFP
#undef pouchGetMaxAP
#undef pouchSetAP
#undef pouchGetHP
#undef pouchGetStarPiece
#undef pouchAddStarPiece
#undef pouchSetSuperCoin
#undef pouchGetSuperCoin
#undef pouchSetCoin
#undef pouchAddCoin
#undef pouchGetCoin
#undef pouchAddKeepItem

extern s32 pouchGetMaxHP(void);
extern void pouchSetHP(s32 points);
extern s32 pouchGetMaxFP(void);
extern void pouchSetFP(s32 points);
extern s32 pouchGetMaxAP(void);
extern void pouchSetAP(s32 points);
extern s32 pouchGetHP(void);
extern s32 pouchGetStarPiece(void);
extern void pouchAddStarPiece(s32 count);
extern s32 pouchSetSuperCoin(s32 count);
extern s32 pouchGetSuperCoin(void);
extern s32 pouchSetCoin(s32 coins);
extern s32 pouchAddCoin(s32 coins);
extern s32 pouchGetCoin(void);
extern s32 pouchAddKeepItem(s32 id);

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
