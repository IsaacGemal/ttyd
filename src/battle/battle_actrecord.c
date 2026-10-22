#include "battle/battle_actrecord.h"

void BtlActRec_AddCount(void* p) {
    if (*(u8*)p < 100) {
        *(u8*)p += 1;
    }
}

void BtlActRec_AddPoint(void* p, u8 value) {
    *(u8*)p += value;
    if (*(u8*)p > 100) {
        *(u8*)p = 100;
    }
}

#define WP(i) (f[(i) - 0x1C])

void BtlActRec_JudgeTurnRuleKeep(void) {
    BattleWork* wp = _battleWorkPointer;
    u8* f = wp->field_16F1C;
    FieldBattleInfo* info = wp->fbatInfo;
    u8 cond = info->field_0x18;

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

#define WP(i) (f[(i) - 0x1C])

void BtlActRec_JudgeRuleKeep(void) {
    BattleWork* wp = _battleWorkPointer;
    u8* f = wp->field_16F1C;
    FieldBattleInfo* info = wp->fbatInfo;
    u8 cond;
    BattleUnit* mario;
    BattleUnit* party;

    mario = BattleGetMarioPtr(wp);
    party = BattleGetPartyPtr(wp);
    (void)party;
    cond = info->field_0x18;

    switch (cond) {
    case 0x00:
        info->ruleKeepResult = 6;
        break;
    case 0x01:
        info->ruleKeepResult = _check_no_use(WP(0x1C));
        break;
    case 0x02:
        info->ruleKeepResult = _check_use(WP(0x1C));
        break;
    case 0x03:
        info->ruleKeepResult = _check_no_use(WP(0x1D));
        break;
    case 0x04:
        info->ruleKeepResult = _check_use(WP(0x1D));
        break;
    case 0x05:
        info->ruleKeepResult = _check_no_use(WP(0x1E) + WP(0x1F));
        break;
    case 0x06:
        info->ruleKeepResult = _check_use(WP(0x1E) + WP(0x1F));
        break;
    case 0x07:
        info->ruleKeepResult = _check_no_use(WP(0x20));
        break;
    case 0x08:
        info->ruleKeepResult = _check_use(WP(0x20));
        break;
    case 0x09:
        info->ruleKeepResult = _check_no_use(WP(0x21));
        break;
    case 0x0A:
        info->ruleKeepResult = _check_use(WP(0x21));
        break;
    case 0x0B:
        info->ruleKeepResult = _check_no_use(WP(0x20) + WP(0x21));
        break;
    case 0x0C:
        info->ruleKeepResult = _check_use(WP(0x20) + WP(0x21));
        break;
    case 0x0D:
        info->ruleKeepResult = _check_no_use(WP(0x22));
        break;
    case 0x0E:
        info->ruleKeepResult = _check_use(WP(0x22));
        break;
    case 0x0F:
        info->ruleKeepResult = _check_no_use(WP(0x23));
        break;
    case 0x10:
        info->ruleKeepResult = _check_use(WP(0x23));
        break;
    case 0x11:
        info->ruleKeepResult = _check_no_use(WP(0x22) + WP(0x23));
        break;
    case 0x12:
        info->ruleKeepResult = _check_use(WP(0x22) + WP(0x23));
        break;
    case 0x13:
        if (mario != NULL) {
            info->ruleKeepResult = _check_use(mario->currentHp);
        } else {
            info->ruleKeepResult = 1;
        }
        break;
    case 0x14:
        if (mario != NULL) {
            info->ruleKeepResult = _check_no_use(mario->currentHp);
        } else {
            info->ruleKeepResult = 1;
        }
        break;
    case 0x15:
        info->ruleKeepResult = _check_use(WP(0x24));
        break;
    case 0x16:
        info->ruleKeepResult = _check_no_use(WP(0x25) + WP(0x26));
        break;
    case 0x17:
        info->ruleKeepResult = _check_use(WP(0x25) + WP(0x26));
        break;
    case 0x18:
        info->ruleKeepResult = _check_no_use(WP(0x27) + WP(0x28));
        break;
    case 0x19:
        info->ruleKeepResult = _check_use(WP(0x27) + WP(0x28));
        break;
    case 0x1A:
        info->ruleKeepResult = _check_no_use(WP(0x25) + WP(0x26) + WP(0x27) + WP(0x28));
        break;
    case 0x1B:
        info->ruleKeepResult = _check_use(WP(0x25) + WP(0x26) + WP(0x27) + WP(0x28));
        break;
    case 0x1C:
        info->ruleKeepResult = _check_no_use(WP(0x29));
        break;
    case 0x1D:
        info->ruleKeepResult = _check_use(WP(0x29));
        break;
    case 0x1E:
        info->ruleKeepResult = _check_no_use(WP(0x2A));
        break;
    case 0x1F:
        info->ruleKeepResult = _check_use(WP(0x2A));
        break;
    case 0x20:
        info->ruleKeepResult = _check_no_use(WP(0x29) + WP(0x2A));
        break;
    case 0x21:
        info->ruleKeepResult = _check_use(WP(0x29) + WP(0x2A));
        break;
    case 0x22:
        info->ruleKeepResult = _check_no_use(WP(0x2B));
        break;
    case 0x23:
        info->ruleKeepResult = _check_use(WP(0x2B));
        break;
    case 0x24:
        info->ruleKeepResult = _check_no_use(WP(0x2C));
        break;
    case 0x25:
        info->ruleKeepResult = _check_use(WP(0x2C));
        break;
    case 0x26:
        info->ruleKeepResult = _check_no_use(WP(0x2D));
        break;
    case 0x27:
        info->ruleKeepResult = _check_use(WP(0x2D));
        break;
    case 0x28:
        info->ruleKeepResult = _check_no_use(WP(0x2E));
        break;
    case 0x29:
        info->ruleKeepResult = _check_use(WP(0x2E));
        break;
    case 0x2A:
        info->ruleKeepResult = _check_no_use(WP(0x2D) + WP(0x2E));
        break;
    case 0x2B:
        info->ruleKeepResult = _check_use(WP(0x2D) + WP(0x2E));
        break;
    case 0x2C:
        info->ruleKeepResult = _check_no_use(WP(0x2F));
        break;
    case 0x2D:
        info->ruleKeepResult = _check_use(WP(0x2F));
        break;
    case 0x2E:
        info->ruleKeepResult = _check_no_use(WP(0x31));
        break;
    case 0x2F:
        info->ruleKeepResult = _check_use(WP(0x31));
        break;
    case 0x30:
        info->ruleKeepResult = _check_no_use(WP(0x2F) + WP(0x31));
        break;
    case 0x31:
        info->ruleKeepResult = _check_use(WP(0x2F) + WP(0x31));
        break;
    case 0x32:
        info->ruleKeepResult = _check_no_use(WP(0x30));
        break;
    case 0x33:
        info->ruleKeepResult = _check_use(WP(0x30));
        break;
    case 0x34:
        info->ruleKeepResult = _check_no_use(WP(0x32));
        break;
    case 0x35:
        info->ruleKeepResult = _check_use(WP(0x32));
        break;
    case 0x36:
        info->ruleKeepResult = _check_no_use(WP(0x2F) + WP(0x31));
        break;
    case 0x37:
        info->ruleKeepResult = _check_use(WP(0x2F) + WP(0x31));
        break;
    case 0x38:
        info->ruleKeepResult = _check_no_use(WP(0x30) + WP(0x32));
        break;
    case 0x39:
        info->ruleKeepResult = _check_use(WP(0x30) + WP(0x32));
        break;
    case 0x3A:
        if (WP(0x3B) > info->field_0x1A) {
            info->ruleKeepResult = 6;
        } else if (WP(0x3B) > info->field_0x1B) {
            info->ruleKeepResult = 2;
        } else {
            info->ruleKeepResult = 1;
        }
        break;
    case 0x3B:
        if (WP(0x3B) <= info->field_0x1A) {
            info->ruleKeepResult = 6;
        } else if (WP(0x3B) <= info->field_0x1B) {
            info->ruleKeepResult = 2;
        } else {
            info->ruleKeepResult = 1;
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
            info->ruleKeepResult = 6;
        } else {
            info->ruleKeepResult = 1;
        }
        break;
    case 0x43:
        if ((WP(0x1C) + WP(0x1E) + WP(0x1F) + WP(0x25) + WP(0x26) + WP(0x29) + WP(0x33) + WP(0x35) + WP(0x2D) + WP(0x37)) == 0) {
            info->ruleKeepResult = 6;
        } else {
            info->ruleKeepResult = 1;
        }
        break;
    }
}

#undef WP

s32 _check_use(s32 arg) {
    BattleWork* wp = _battleWorkPointer;
    FieldBattleInfo* info = wp->fbatInfo;

    if (arg >= info->field_0x1A) {
        return 6;
    }
    return (arg >= info->field_0x1B) + 1;
}

s32 _check_no_use(s32 arg) {
    BattleWork* wp = _battleWorkPointer;
    FieldBattleInfo* info = wp->fbatInfo;

    if (arg >= info->field_0x1B) {
        return 1;
    }
    return (arg >= info->field_0x1A) ? 2 : 6;
}

u8 _check_turn_count_0_turn(s32 flag) {
    BattleWork* wp = _battleWorkPointer;
    FieldBattleInfo* info = wp->fbatInfo;
    u8* turn = &wp->field_16F1C[0x3B - 0x1C];

    if (*turn <= info->field_0x1B || info->field_0x1B == 0xFF) {
        if (flag == 0) {
            info->ruleKeepResult = 2;
        } else {
            info->ruleKeepResult = 1;
        }
    }
    if (*turn <= info->field_0x1A || info->field_0x1A == 0xFF) {
        if (flag == 0) {
            info->ruleKeepResult = 6;
        } else if (!(info->ruleKeepResult & 2)) {
            info->ruleKeepResult = 1;
        }
    }
    return info->ruleKeepResult;
}

u8 _check_turn_count_0_end(s32 flag) {
    BattleWork* wp = _battleWorkPointer;
    FieldBattleInfo* info = wp->fbatInfo;
    u8* turn = &wp->field_16F1C[0x3B - 0x1C];

    if (*turn <= info->field_0x1B || info->field_0x1B == 0xFF) {
        if (flag == 0) {
            info->ruleKeepResult = 2;
        } else {
            info->ruleKeepResult = 1;
        }
    }
    if (*turn <= info->field_0x1A || info->field_0x1A == 0xFF) {
        if (flag == 0) {
            info->ruleKeepResult = 6;
        } else if (!(info->ruleKeepResult & 2)) {
            info->ruleKeepResult = 1;
        }
    }
    return info->ruleKeepResult;
}
