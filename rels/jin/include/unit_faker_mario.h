#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(_save_param);
USER_FUNC(_load_param);
USER_FUNC(_gullible_party_info_init);
EVT_DEF(fmario_entry_event);
EVT_DEF(dead_event_fmario_down);
EVT_DEF(phase_event_fmario);
EVT_DEF(party_change_event);
