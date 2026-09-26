#include <string.h>

#include <dolphin/types.h>

#include "driver/casedrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"

// Not yet in a header, and not decompiled yet (casedrv.s).
CaseEntry* caseIdToPtr(s32 caseId);

USER_FUNC(evt_set_case_wrk) {
	s32* args = event->args;
	s32 caseId = evtGetValue(event, args[0]);
	s32 index = evtGetValue(event, args[1]);
	s32 value = evtGetValue(event, args[2]);
	CaseEntry* entry = caseIdToPtr(caseId);

	entry->lwData[index] = value;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_del_case_evt) {
	s32* args = event->args;
	s32 flag = evtGetValue(event, args[0]);
	s32 caseId = evtGetValue(event, args[1]);

	if (flag == 0) {
		CaseEntry* entry = caseIdToPtr(caseId);
		if (evtCheckID(*(s32*)entry->field_0x94)) {
			evtDeleteID(*(s32*)entry->field_0x94);
		}
	}
	caseDelete(caseId);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_exit_case_evt) {
	caseDelete(event->caseId);
	return EVT_RETURN_DONE;
}

s32 evtRunCaseEntry(s32 activeConditionId, s32 flag, s32 hitObjName, s32 swFlag, s32* evtCode, s32* lwData) {
	CaseSetup setup;
	CaseSetup* p = &setup;

	if (flag != 0) {
		activeConditionId |= 0x8000;
	}
	p->activeConditionId = activeConditionId;
	p->hitObjName = (const char*)hitObjName;
	p->swFlag = swFlag;
	p->activeFunc = 0;
	p->evtCode = evtCode;
	p->priority = 0;
	if (lwData != NULL) {
		memcpy(p->lwData, lwData, 0x40);
	} else {
		memset(p->lwData, 0, 0x40);
	}
	return caseEntry(p);
}

USER_FUNC(evt_run_case_evt_bero) {
	s32* args = event->args;
	s32 activeConditionId = evtGetValue(event, args[0]);
	s32 flag = evtGetValue(event, args[1]);
	s32 hitObjName = evtGetValue(event, args[2]);
	s32 swFlag = evtGetNumber(event, args[3]);
	s32 evtCode = evtGetValue(event, args[4]);
	s32 conditionId = activeConditionId;
	CaseSetup setup;
	CaseSetup* p = &setup;
	s32 ret;

	if (flag != 0) {
		conditionId |= 0x8000;
	}
	p->activeConditionId = conditionId;
	p->hitObjName = (const char*)hitObjName;
	p->swFlag = swFlag;
	p->activeFunc = 0;
	p->evtCode = (void*)evtCode;
	p->priority = 0x14;
	memcpy(p->lwData, event->lwData, 0x40);

	ret = caseEntry(p);
	if (args[5] != 0) {
		evtSetValue(event, args[5], ret);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_run_case_evt) {
	s32* args = event->args;
	s32 activeConditionId = evtGetValue(event, args[0]);
	s32 flag = evtGetValue(event, args[1]);
	s32 hitObjName = evtGetValue(event, args[2]);
	s32 swFlag = evtGetNumber(event, args[3]);
	s32 evtCode = evtGetValue(event, args[4]);
	s32 conditionId = activeConditionId;
	CaseSetup setup;
	CaseSetup* p = &setup;
	s32 ret;

	if (flag != 0) {
		conditionId |= 0x8000;
	}
	p->activeConditionId = conditionId;
	p->hitObjName = (const char*)hitObjName;
	p->swFlag = swFlag;
	p->activeFunc = 0;
	p->evtCode = (void*)evtCode;
	p->priority = 0;
	memcpy(p->lwData, event->lwData, 0x40);

	ret = caseEntry(p);
	if (args[5] != 0) {
		evtSetValue(event, args[5], ret);
	}
	return EVT_RETURN_DONE;
}
