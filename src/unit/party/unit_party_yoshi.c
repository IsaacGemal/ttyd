#include "unit/party/unit_party_yoshi.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "battle/battle_disp.h"

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

USER_FUNC(_get_swallow_param) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 val;
    s32 id;
    s32 index;

    val = evtGetValue(event, args[0]);
    id = BattleTransID(event, val);
    index = args[1];
    unit = BattleGetUnitPtr(work, id);
    evtSetValue(event, index, unit->swallowRate);
    return EVT_RETURN_DONE;
}

USER_FUNC(_wait_yoshig_complete) {
    BattleUnit* unit;
    s32 val;

    val = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, val));
    return *(void**)((u8*)unit + 0x310) ? 0 : EVT_RETURN_DONE;
}

// One of the 256 particles drawn by _gundan_yoshi_run_effect: a 0x30-byte entry whose
// display callback argument is the entry's +4 sub-structure. No header declares it.
typedef struct YoshiGundanDispWork {
    f32 field_0x00; // 0x00
    f32 field_0x04; // 0x04
    f32 field_0x08; // 0x08
    f32 field_0x0c; // 0x0C
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    u8  field_0x18[0x0c]; // 0x18
    u8  field_0x24; // 0x24
    u8  field_0x25; // 0x25, index into _yoshig_tpl_no_tbl (0..5)
    u8  field_0x26[2]; // 0x26
    s32 field_0x28; // 0x28
    s32 field_0x2c; // 0x2C
} YoshiGundanDispWork;

// .sdata/.sdata2 constants this unit references by name.
extern u8 _yoshig_tpl_no_tbl[8];
extern const GXColor dat_8041dd38;       // 0xFFFFFFFF
extern const f32 float_0p4_8041dd3c;
extern const f32 float_neg0p4_8041dd40;
extern const f32 float_0_8041dd44;
extern const f32 float_20_8041dd48;

void _btlYoshiDisp(CameraId cameraId, void* param) {
    YoshiGundanDispWork* work = param;
    GXColor color;
    f32 x_scale;

    x_scale = (work->field_0x0c < float_0_8041dd44) ? float_0p4_8041dd3c : float_neg0p4_8041dd40;
    color = dat_8041dd38;
#ifdef MUST_MATCH
    // btlDispTexPlane's real signature is
    // (f32 x, f32 y, f32 z, f32 sx, f32 sy, s32 frame, GXColor* color, s32 unused);
    // include/battle/battle_disp.h declares it with the decomp's (u32 id, GXColor color, ...)
    // order and omits the trailing unused s32. Call through the real signature so the original's
    // argument setup (color pointer in r4, `li r5, 0`) is emitted. The natural decomp-form call
    // is under #else.
    ((void (*)(f32, f32, f32, f32, f32, s32, GXColor*, s32))btlDispTexPlane)(
        work->field_0x00, float_20_8041dd48 + work->field_0x04, work->field_0x08, x_scale,
        float_0p4_8041dd3c, _yoshig_tpl_no_tbl[work->field_0x25], &color, 0);
#else
    btlDispTexPlane(_yoshig_tpl_no_tbl[work->field_0x25], color, work->field_0x00,
                    float_20_8041dd48 + work->field_0x04, work->field_0x08, x_scale,
                    float_0p4_8041dd3c);
#endif
}
