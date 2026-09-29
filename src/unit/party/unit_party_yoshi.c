#include "unit/party/unit_party_yoshi.h"
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
