#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(teresa_wait_breath2);
USER_FUNC(teresa_wait_breath);
USER_FUNC(teresa_appear_breath);
USER_FUNC(teresa_wait_bikkuri2);
USER_FUNC(teresa_wait_bikkuri);
USER_FUNC(teresa_appear_bikkuri);
USER_FUNC(target_id_set);
USER_FUNC(atmic_teresa_id_set);
void teresa_ext_disp(void);
void teresa_ext_main(void);
void teresa_ext_init(void);
EVT_DEF(kaokakusi_event);
EVT_DEF(press_event);
EVT_DEF(surprise_event);
EVT_DEF(bikkuri_event);
EVT_DEF(breath_event_sub);
EVT_DEF(teresa_entry);
