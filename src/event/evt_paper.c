#include "evt/evt_paper.h"
#include "driver/animdrv.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"

USER_FUNC(evt_paper_entry) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	s32 inBattle = gp->inBattle != 0;

	if (!animGroupBaseAsync((const char*)name, inBattle, NULL)) {
		return EVT_RETURN_BLOCK;
	}
	animPaperPoseEntry((const char*)name, inBattle);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_paper_delete) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);

	animPaperPoseRelease(animPaperPoseGetId((const char*)name, gp->inBattle != 0));
	return EVT_RETURN_DONE;
}

