#include "battle/battle_actrecord.h"

// dont_inline keeps these helpers out of the Judge functions: retail emits a real `bl` at each call.
#pragma dont_inline on
u8 _check_turn_count_0_end(s32 flag) {
    BattleWork* wp = _battleWorkPointer;
    u8* rec = (u8*)wp->fbatInfo;
    u8* turn = &wp->field_16F1C[0x1F];

    if (*turn <= rec[0x1B] || rec[0x1B] == 0xFF) {
        if (flag == 0) {
            rec[0x19] = 2;
        } else {
            rec[0x19] = 1;
        }
    }
    if (*turn <= rec[0x1A] || rec[0x1A] == 0xFF) {
        if (flag == 0) {
            rec[0x19] = 6;
        } else if (!(rec[0x19] & 2)) {
            rec[0x19] = 1;
        }
    }
    return rec[0x19];
}

u8 _check_turn_count_0_turn(s32 flag) {
    BattleWork* wp = _battleWorkPointer;
    u8* rec = (u8*)wp->fbatInfo;
    u8* turn = &wp->field_16F1C[0x1F];

    if (*turn <= rec[0x1B] || rec[0x1B] == 0xFF) {
        if (flag == 0) {
            rec[0x19] = 2;
        } else {
            rec[0x19] = 1;
        }
    }
    if (*turn <= rec[0x1A] || rec[0x1A] == 0xFF) {
        if (flag == 0) {
            rec[0x19] = 6;
        } else if (!(rec[0x19] & 2)) {
            rec[0x19] = 1;
        }
    }
    return rec[0x19];
}

s32 _check_no_use(s32 arg) {
    BattleWork* wp = _battleWorkPointer;
    u8* rec = (u8*)wp->fbatInfo;

    if (arg >= rec[0x1B]) {
        return 1;
    }
    return (arg >= rec[0x1A]) ? 2 : 6;
}

s32 _check_use(s32 arg) {
    BattleWork* wp = _battleWorkPointer;
    u8* rec = (u8*)wp->fbatInfo;

    if (arg >= rec[0x1A]) {
        return 6;
    }
    return (arg >= rec[0x1B]) + 1;
}
#pragma dont_inline reset


#define WP(i) (f[i])

// optimize_for_size: retail builds the JudgeRuleKeep prologue with stmw/lmw and orders the sums the same way.
#pragma optimize_for_size on
void BtlActRec_JudgeRuleKeep(void) {
    BattleWork* wp = _battleWorkPointer;
    u8* f = (u8*)wp + 0x16F00;
    u8* rec = (u8*)wp->fbatInfo;
    u8 cond;
    BattleUnit* mario;
    BattleUnit* party;

    mario = BattleGetMarioPtr(wp);
    party = BattleGetPartyPtr(wp);
    (void)party;
    cond = rec[0x18];

    switch (cond) {
    case 0x00:
        rec[0x19] = 6;
        break;
    case 0x01:
        rec[0x19] = _check_no_use(WP(0x1C));
        break;
    case 0x02:
        rec[0x19] = _check_use(WP(0x1C));
        break;
    case 0x03:
        rec[0x19] = _check_no_use(WP(0x1D));
        break;
    case 0x04:
        rec[0x19] = _check_use(WP(0x1D));
        break;
    case 0x05:
        rec[0x19] = _check_no_use(WP(0x1E) + WP(0x1F));
        break;
    case 0x06:
        rec[0x19] = _check_use(WP(0x1E) + WP(0x1F));
        break;
    case 0x07:
        rec[0x19] = _check_no_use(WP(0x20));
        break;
    case 0x08:
        rec[0x19] = _check_use(WP(0x20));
        break;
    case 0x09:
        rec[0x19] = _check_no_use(WP(0x21));
        break;
    case 0x0A:
        rec[0x19] = _check_use(WP(0x21));
        break;
    case 0x0B:
        rec[0x19] = _check_no_use(WP(0x20) + WP(0x21));
        break;
    case 0x0C:
        rec[0x19] = _check_use(WP(0x20) + WP(0x21));
        break;
    case 0x0D:
        rec[0x19] = _check_no_use(WP(0x22));
        break;
    case 0x0E:
        rec[0x19] = _check_use(WP(0x22));
        break;
    case 0x0F:
        rec[0x19] = _check_no_use(WP(0x23));
        break;
    case 0x10:
        rec[0x19] = _check_use(WP(0x23));
        break;
    case 0x11:
        rec[0x19] = _check_no_use(WP(0x22) + WP(0x23));
        break;
    case 0x12:
        rec[0x19] = _check_use(WP(0x22) + WP(0x23));
        break;
    case 0x13:
        if (mario != NULL) {
            rec[0x19] = _check_use(mario->currentHp);
        } else {
            rec[0x19] = 1;
        }
        break;
    case 0x14:
        if (mario != NULL) {
            rec[0x19] = _check_no_use(mario->currentHp);
        } else {
            rec[0x19] = 1;
        }
        break;
    case 0x15:
        rec[0x19] = _check_use(WP(0x24));
        break;
    case 0x16:
        rec[0x19] = _check_no_use(WP(0x25) + WP(0x26));
        break;
    case 0x17:
        rec[0x19] = _check_use(WP(0x25) + WP(0x26));
        break;
    case 0x18:
        rec[0x19] = _check_no_use(WP(0x27) + WP(0x28));
        break;
    case 0x19:
        rec[0x19] = _check_use(WP(0x27) + WP(0x28));
        break;
    case 0x1A:
        rec[0x19] = _check_no_use(WP(0x25) + WP(0x26) + WP(0x27) + WP(0x28));
        break;
    case 0x1B:
        rec[0x19] = _check_use(WP(0x25) + WP(0x26) + WP(0x27) + WP(0x28));
        break;
    case 0x1C:
        rec[0x19] = _check_no_use(WP(0x29));
        break;
    case 0x1D:
        rec[0x19] = _check_use(WP(0x29));
        break;
    case 0x1E:
        rec[0x19] = _check_no_use(WP(0x2A));
        break;
    case 0x1F:
        rec[0x19] = _check_use(WP(0x2A));
        break;
    case 0x20:
        rec[0x19] = _check_no_use(WP(0x29) + WP(0x2A));
        break;
    case 0x21:
        rec[0x19] = _check_use(WP(0x29) + WP(0x2A));
        break;
    case 0x22:
        rec[0x19] = _check_no_use(WP(0x2B));
        break;
    case 0x23:
        rec[0x19] = _check_use(WP(0x2B));
        break;
    case 0x24:
        rec[0x19] = _check_no_use(WP(0x2C));
        break;
    case 0x25:
        rec[0x19] = _check_use(WP(0x2C));
        break;
    case 0x26:
        rec[0x19] = _check_no_use(WP(0x2D));
        break;
    case 0x27:
        rec[0x19] = _check_use(WP(0x2D));
        break;
    case 0x28:
        rec[0x19] = _check_no_use(WP(0x2E));
        break;
    case 0x29:
        rec[0x19] = _check_use(WP(0x2E));
        break;
    case 0x2A:
        rec[0x19] = _check_no_use(WP(0x2D) + WP(0x2E));
        break;
    case 0x2B:
        rec[0x19] = _check_use(WP(0x2D) + WP(0x2E));
        break;
    case 0x2C:
        rec[0x19] = _check_no_use(WP(0x2F));
        break;
    case 0x2D:
        rec[0x19] = _check_use(WP(0x2F));
        break;
    case 0x2E:
        rec[0x19] = _check_no_use(WP(0x31));
        break;
    case 0x2F:
        rec[0x19] = _check_use(WP(0x31));
        break;
    case 0x30:
        rec[0x19] = _check_no_use(WP(0x2F) + WP(0x31));
        break;
    case 0x31:
        rec[0x19] = _check_use(WP(0x2F) + WP(0x31));
        break;
    case 0x32:
        rec[0x19] = _check_no_use(WP(0x30));
        break;
    case 0x33:
        rec[0x19] = _check_use(WP(0x30));
        break;
    case 0x34:
        rec[0x19] = _check_no_use(WP(0x32));
        break;
    case 0x35:
        rec[0x19] = _check_use(WP(0x32));
        break;
    case 0x36:
        rec[0x19] = _check_no_use(WP(0x2F) + WP(0x31));
        break;
    case 0x37:
        rec[0x19] = _check_use(WP(0x2F) + WP(0x31));
        break;
    case 0x38:
        rec[0x19] = _check_no_use(WP(0x30) + WP(0x32));
        break;
    case 0x39:
        rec[0x19] = _check_use(WP(0x30) + WP(0x32));
        break;
    case 0x3A:
        if (WP(0x3B) > rec[0x1A]) {
            rec[0x19] = 6;
        } else if (WP(0x3B) > rec[0x1B]) {
            rec[0x19] = 2;
        } else {
            rec[0x19] = 1;
        }
        break;
    case 0x3B:
        if (WP(0x3B) <= rec[0x1A]) {
            rec[0x19] = 6;
        } else if (WP(0x3B) <= rec[0x1B]) {
            rec[0x19] = 2;
        } else {
            rec[0x19] = 1;
        }
        break;
    case 0x3C:
    case 0x3D:
    case 0x3E: {
        s32 sum;

        if (cond == 0x3E) {
            sum = WP(0x1C) + WP(0x1D) + WP(0x1E) + WP(0x1F) + WP(0x25) + WP(0x26) + WP(0x29) + WP(0x33) + WP(0x35) + WP(0x37) + WP(0x39) + WP(0x3A) + WP(0x27) + WP(0x28) + WP(0x2A) + WP(0x34) + WP(0x36) + WP(0x38);
        } else if (cond == 0x3C) {
            sum = WP(0x1C) + WP(0x1D) + WP(0x1E) + WP(0x1F) + WP(0x25) + WP(0x26) + WP(0x29) + WP(0x33) + WP(0x35) + WP(0x37);
        } else {
            sum = WP(0x39) + WP(0x3A) + WP(0x27) + WP(0x28) + WP(0x2A) + WP(0x34) + WP(0x36) + WP(0x38);
        }
        _check_turn_count_0_end(sum);
        break;
    }
    case 0x3F:
    case 0x40:
    case 0x41: {
        s32 sum;

        if (cond == 0x41) {
            sum = WP(0x1C) + WP(0x1D) + WP(0x1E) + WP(0x25) + WP(0x39) + WP(0x27);
        } else if (cond == 0x3F) {
            sum = WP(0x1C) + WP(0x1D) + WP(0x1E) + WP(0x25);
        } else {
            sum = WP(0x39) + WP(0x27);
        }
        _check_turn_count_0_end(sum);
        break;
    }
    case 0x42:
        if ((WP(0x1D) + WP(0x1E) + WP(0x1F) + WP(0x25) + WP(0x26) + WP(0x29) + WP(0x33) + WP(0x35) + WP(0x2D) + WP(0x37)) == 0) {
            rec[0x19] = 6;
        } else {
            rec[0x19] = 1;
        }
        break;
    case 0x43:
        if ((WP(0x1C) + WP(0x1E) + WP(0x1F) + WP(0x25) + WP(0x26) + WP(0x29) + WP(0x33) + WP(0x35) + WP(0x2D) + WP(0x37)) == 0) {
            rec[0x19] = 6;
        } else {
            rec[0x19] = 1;
        }
        break;
    }
}

#undef WP
#pragma optimize_for_size off

// optimize_for_size: retail builds the JudgeTurnRuleKeep prologue with stmw/lmw and orders the sums the same way.
#pragma optimize_for_size on
#define WP(i) (f[i])

void BtlActRec_JudgeTurnRuleKeep(void) {
    BattleWork* wp = _battleWorkPointer;
    u8* rec = (u8*)wp->fbatInfo;
    u8* f = (u8*)wp + 0x16F00;
    u8 cond = rec[0x18];

    switch (cond) {
    case 0x3C:
    case 0x3D:
    case 0x3E: {
        s32 sum;

        if (cond == 0x3E) {
            sum = WP(0x1C) + WP(0x1D) + WP(0x1E) + WP(0x1F) + WP(0x25) + WP(0x26) + WP(0x29) + WP(0x33) + WP(0x35) + WP(0x37) + WP(0x39) + WP(0x3A) + WP(0x27) + WP(0x28) + WP(0x2A) + WP(0x34) + WP(0x36) + WP(0x38);
        } else if (cond == 0x3C) {
            sum = WP(0x1C) + WP(0x1D) + WP(0x1E) + WP(0x1F) + WP(0x25) + WP(0x26) + WP(0x29) + WP(0x33) + WP(0x35) + WP(0x37);
        } else {
            sum = WP(0x39) + WP(0x3A) + WP(0x27) + WP(0x28) + WP(0x2A) + WP(0x34) + WP(0x36) + WP(0x38);
        }
        _check_turn_count_0_turn(sum);
        break;
    }
    case 0x3F:
    case 0x40:
    case 0x41: {
        s32 sum;

        if (cond == 0x41) {
            sum = WP(0x1C) + WP(0x1D) + WP(0x1E) + WP(0x25) + WP(0x39) + WP(0x27);
        } else if (cond == 0x3F) {
            sum = WP(0x1C) + WP(0x1D) + WP(0x1E) + WP(0x25);
        } else {
            sum = WP(0x39) + WP(0x27);
        }
        _check_turn_count_0_turn(sum);
        break;
    }
    }
}

#undef WP
#pragma optimize_for_size off

void BtlActRec_AddPoint(void* p, u8 value) {
    *(u8*)p += value;
    if (*(u8*)p > 100) {
        *(u8*)p = 100;
    }
}

void BtlActRec_AddCount(void* p) {
    if (*(u8*)p < 100) {
        *(u8*)p += 1;
    }
}
