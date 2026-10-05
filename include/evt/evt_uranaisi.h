#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(uranaisi_data_alloc);
USER_FUNC(uranaisi_data_free);
USER_FUNC(uranaisi_next_check);
USER_FUNC(uranaisi_next_makelabel);
USER_FUNC(uranaisi_ryokin);
USER_FUNC(uranaisi_starpiece_check);
USER_FUNC(uranaisi_starpiece_makelabel);
USER_FUNC(uranaisi_starpiece_setreadflag);
USER_FUNC(uranaisi_supercoin_check);
USER_FUNC(uranaisi_supercoin_makelabel);
USER_FUNC(uranaisi_supercoin_setreadflag);
USER_FUNC(uranaisi_table_clear);
s32 uranaisi_ryokin_rtn(s32 idx);
s32 uranaisi_data_make_supercoin(const char* prefix, s32 type);
s32 uranaisi_data_make_starpiece(const char* prefix, s32 type);
s32 uranaisi_data_make_next(const char* prefix, s32 type);
