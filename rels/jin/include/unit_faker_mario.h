#pragma once

#include "evt/evt_cmd.h"

// Saved party state (0x30 bytes) stashed at BattleUnit + 0x310 by
// _gullible_party_info_init / _save_param and read back by _load_param.
typedef struct BattleUnitStatus {
    s8 data[0x1E];
} BattleUnitStatus;

typedef struct BattleUnitParam {
    f32 mSizeMultiplier;      // 0x0
    BattleUnitStatus mStatus; // 0x4
    u8 pad_0x22[2];           // 0x22
    s32 mStatusFlags;         // 0x24
    s16 currentHp;            // 0x28
    s16 maxHp;                // 0x2A
    s16 currentFp;            // 0x2C
    s16 maxFp;                // 0x2E
} BattleUnitParam;

USER_FUNC(_save_param);
USER_FUNC(_load_param);
USER_FUNC(_gullible_party_info_init);
EVT_DEF(fmario_entry_event);
EVT_DEF(dead_event_fmario_down);
EVT_DEF(phase_event_fmario);
EVT_DEF(party_change_event);
