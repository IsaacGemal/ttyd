#include "unit/party/unit_party_yoshi.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"

// A pose-sound table entry. No header declares this type; the layout matches the
// 0x14-byte entries of `posesound_normal_attack` and the fields BtlUnit_ControlPoseSoundMain
// reads (pose name 0x0, volume 0x4, callback 0x8, sound name 0xC, pitch 0x10, flags 0x12).
typedef struct PoseSoundEntry {
    const char* poseName;                                   // 0x0
    f32 volume;                                             // 0x4
    s32 (*callback)(BattleUnit*, struct PoseSoundEntry*, void*); // 0x8
    const char* soundName;                                  // 0xC
    s16 pitch;                                              // 0x10
    u16 flags;                                              // 0x12
} PoseSoundEntry;

s32 _yoshi_slide_move_sound(BattleUnit* unit, PoseSoundEntry* entry) {
    return BtlUnit_snd_se(unit, entry->soundName, 0xF1194D80, (s16)(unit->work[4] * unit->work[5] / 100));
}

USER_FUNC(_wait_yoshig_complete) {
    BattleUnit* unit;
    s32 val;

    val = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, val));
    return *(void**)((u8*)unit + 0x310) ? 0 : EVT_RETURN_DONE;
}
