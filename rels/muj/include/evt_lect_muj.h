#pragma once

#include <dolphin/mtx.h>
#include <dolphin/types.h>

#include "evt/evt_cmd.h"

// Corrected signature: the original icondrv.h declares this with s16 iconId,
// but the REL's call site passes a value that must be masked to 16 bits.
void iconDispGxAlpha(Vec* position, s16 flags, u16 iconId, u8 alpha, f32 scale);

EVT_DEF(sanders_party_lecture_init);
EVT_DEF(sanders_party_lecture);
EVT_DEF(all_party_lecture_20_data_3C908);
