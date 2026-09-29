#include "evt/evt_paper.h"
#include "driver/animdrv.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"

USER_FUNC(evt_paper_delete) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);

	animPaperPoseRelease(animPaperPoseGetId((const char*)name, gp->inBattle != 0));
	return EVT_RETURN_DONE;
}
