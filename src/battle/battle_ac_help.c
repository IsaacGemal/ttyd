#include "battle/battle_ac_help.h"
#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "manager/fontmgr.h"

// Pooled constants shared with battle_seq_end.c; the original object references these by name.
extern u32 dat_80420d60;
extern f32 float_0_80420d64;
extern f32 float_20_80420d68;
extern f32 float_neg145_80420d6c;
extern f32 float_10_80420d70;
extern f32 float_neg148_80420d74;
extern f32 float_498_80420d78;

const char str_802FE1D8[28] = "\202\337\202\301\202\271\201[\202\266\202\252\202\335\202\302\202\251\202\350\202\334\202\271\202\361";
void BattleAcHelpSetHelp(const char* msg) {
    _battleWorkPointer->field_18FF0.help = msg;
}
void BattleAcHelpSetDispType(s32 type) {
    _battleWorkPointer->field_18FF0.dispType = type;
    switch (type) {
    case 0:
        BattleAudience_WinSetActive(1);
        break;
    default:
        BattleAudience_WinSetActive(0);
        break;
    }
}
void BattleAcHelpInit(void) {
	//stubbed in retail
}
void BattleAcHelpMain(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleWorkField18FF0* help = &wp->field_18FF0;

    if (help->help != NULL
        && help->dispType != 0
        && !(wp->flags & 2)) {
        dispEntry(CAMERA_2D, float_498_80420d78, 1, _ac_help_disp, NULL);
    }
}

// This function wants -O4,s (its stmw r30 prologue and signed div-by-2 only come out right with it);
// the ledger replaces such pragmas with a file-level -O4,s.
#pragma optimize_for_size on
#ifdef MUST_MATCH
// Build the waku colour in an inline so the caller's local and outgoing copy land in the original slots.
static inline GXColor makeWakuColor(u8 alpha) {
    GXColor c = *(GXColor*)&dat_80420d60;
    c.a = alpha;
    return c;
}
// Resolving the message in an inline (rather than an if in the caller) lets the fallback string
// address coalesce into the message register instead of an extra r0 temporary.
static inline const char* acHelpResolve(const char* m) {
    if (m == NULL) {
        m = str_802FE1D8;
    }
    return m;
}
#endif
void _ac_help_disp(CameraId camera, void* param) {
    BattleWork* wp = _battleWorkPointer;
    const char** helpMsg = &wp->field_18FF0.help;
    s32* dispType = &wp->field_18FF0.dispType;
    u16 lines;
    GXColor color;
    s32 width;
    f32 x;
    const char* msg;
    u8 alpha;

    alpha = 0xFF;
    if (*dispType == 1) {
        alpha = 0x80;
    }
#ifdef MUST_MATCH
    msg = acHelpResolve(msgSearch(*helpMsg));
#else
    msg = msgSearch(*helpMsg);
    if (msg == NULL) {
        msg = str_802FE1D8;
    }
#endif
    width = FontGetMessageWidthLine(msg, &lines);
    lines++;
#ifdef MUST_MATCH
    color = makeWakuColor(alpha);
#else
    *(u32*)&color = dat_80420d60;
    color.a = alpha;
#endif
    x = float_0_80420d64 - (f32)(width / 2);
    windowDispGX_Waku_col(x - float_20_80420d68, float_neg145_80420d6c + (f32)((lines - 1) * 0x1D), (f32)(width + 0x28),
                          (f32)(lines * 0x1D + 1), float_10_80420d70, 0, color);
    FontDrawStart_alpha(alpha);
    FontDrawMessage((s32)x, (s32)float_neg148_80420d74 + (lines - 1) * 0x1D, msg);
}
#pragma optimize_for_size off