#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/types.h>
s32 evtRunCaseEntry(s32 a1, s32 a2, s32 a3, s32 a4, s32* a5, s32* a6);

USER_FUNC(evt_del_case_evt);
USER_FUNC(evt_exit_case_evt);
USER_FUNC(evt_run_case_evt);
USER_FUNC(evt_set_case_wrk);
USER_FUNC(evt_run_case_evt_bero);
