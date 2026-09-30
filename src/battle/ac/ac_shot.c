#include "battle/ac/ac_shot.h"
#include "battle/battle.h"
#include "battle/battle_disp.h"
#include "battle/battle_sub.h"
#include <string.h>

// .sdata2 constants used by _ac_disp_init; the original emits them as named symbols.
extern const f32 float_neg200_8041e4d0;
extern const f32 float_0_8041e4b8;
extern u32 dat_8041e4a8;

// .sdata2 constants used by _disp_target_mark.
extern const f32 float_60_8041e4c0;
extern const GXColor dat_8041e4ac;

// _disp_target_mark's action-command work area at BattleWork+0x1F4C. No header declares it,
// so the fields are named from the original's uses. Offsets here are relative to 0x1F4C.
typedef struct TargetMarkWork {
    u8 field_0x0[0x24]; // 0x00
    f32 rotZ;           // 0x24 (BattleWork+0x1F70), marker angle
    u8 field_0x28[4];   // 0x28 (BattleWork+0x1F74), unused by this function
    f32 field_0x2C;     // 0x2C (BattleWork+0x1F78), passed as the f4 argument
    f32 x;              // 0x30 (BattleWork+0x1F7C)
    f32 y;              // 0x34 (BattleWork+0x1F80)
    f32 z;              // 0x38 (BattleWork+0x1F84)
} TargetMarkWork;

s32 battleAcResult_Shot(BattleWork* work) {
    return work->actionCommands.result;
}

void battleAcDelete_Shot(BattleWork* work) {
    work->actionCommands.mAcState = 0x3EC;
}

static void _ac_disp_init(BattleWork* work) {
    BattleACDispParams* disp = &work->actionCommands.dispParams;

    memset(disp, 0, 0x2C);
    disp->iconX = float_neg200_8041e4d0;
    disp->iconY = float_0_8041e4b8;
    disp->counter = 0x14;
    disp->field_0xC = dat_8041e4a8;
    disp->field_0x10[0] = 0;
    *(s8*)&disp->field_0x10[1] = -0x10;
    *(s8*)&disp->field_0x10[2] = -0x10;
    disp->field_0x10[3] = 0;
    disp->value = float_0_8041e4b8;
}

void _disp_target_mark(CameraId cameraId, void* param) {
    BattleWork* work = param;
    TargetMarkWork* tm = (TargetMarkWork*)((u8*)work + 0x1F4C);
    Vec pos;
    Vec screen;
    GXColor color;

    pos.x = float_60_8041e4c0 * sinfd(tm->rotZ) + tm->x;
    pos.y = float_60_8041e4c0 * cosfd(tm->rotZ) + tm->y;
    pos.z = tm->z;
    btlGetScreenPoint(&pos, &screen);
    color = dat_8041e4ac;
    _draw_target_mark(&color, screen.x, screen.y, screen.z, tm->field_0x2C);
}

const f32 float_neg200_8041e4d0 = -200.0f;
const f32 float_0_8041e4b8 = 0.0f;
u32 dat_8041e4a8 = 0xFFFFFFFF;
const f32 float_60_8041e4c0 = 60.0f;
const GXColor dat_8041e4ac = {0xFF, 0xFF, 0xFF, 0xFF};
