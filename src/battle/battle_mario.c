#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"
#include "driver/animdrv.h"
#include "driver/effdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "pmario_sound.h"
#include "system.h"
#include "battle/battle_audience.h"
#include "eff/eff_majinai.h"
#include "eff/eff_hibashira.h"
#include "eff/eff_energy.h"
#include "eff/eff_fire.h"
#include "eff/n64/eff_whirlwind_n64.h"
#include "eff/n64/eff_kameki_tornade_n64.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_mario.h"
#include "battle/battle_message.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_audience.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/sac/sac_deka.h"
#include "battle/sac/sac_scissor.h"
#include "battle/sac/sac_zubastar.h"
static s32 _get_mario_hammer_lv(EventEntry* event, BOOL isFirstCall);  // local to this file in the original

// Prototypes for helpers that have no header yet. See REPORT.md.

extern s32 btlevtcmd_ACHelpSet[];
extern s32 btlevtcmd_AnimeChangePaperAnime[];
extern s32 btlevtcmd_AnimeChangePaperGroup[];
extern s32 btlevtcmd_GetWeaponActionLv[];
extern s32 evt_audience_acrobat_notry[];
extern s32 evt_majinai_effect_se_btl[];
extern s32 marioAttackEvent_BakuGame[];
extern s32 marioAttackEvent_Deka[];
extern s32 marioAttackEvent_Genki0[];
extern s32 marioAttackEvent_Genki1[];
extern s32 marioAttackEvent_Muki[];
extern s32 marioAttackEvent_Scissor[];
extern s32 marioAttackEvent_Suki[];
extern s32 marioAttackEvent_ZubaStar[];
static s32 marioAttackEvent_NormalJump[889];
static s32 marioAttackEvent_KururinJump[1210];
s32 marioAttackEvent_JyabaraJump[1675];
s32 marioAttackEvent_TatsumakiJump[1668];
s32 marioAttackEvent_RenzokuJump[840];
s32 marioAttackEvent_TugiTugiJump[793];
static s32 marioAttackEvent_KaitenHammer[1060];
s32 marioAttackEvent_FirstAttackKaitenHammer[19];
s32 marioAttackEvent_JishinHammer[834];
s32 marioAttackEvent_UltraJishinHammer[665];
s32 marioAttackEvent_HammerNageru[446];
s32 marioAttackEvent_HammerNageru_object[240];
s32 marioAttackEvent_FireNaguri[702];
s32 marioAttackEvent_FireNaguri_object[374];
s32 marioAttackEvent_MajinaiPowerUpCheck[35];
s32 marioAttackEvent_MajinaiJump[43];
s32 marioAttackEvent_GatsunJump[7];
s32 marioAttackEvent_DokanJump[7];
s32 marioAttackEvent_MiniminiFumi[4];
s32 marioAttackEvent_NemuraseFumi[4];
s32 marioAttackEvent_FunyafunyaJump[4];
static s32 marioAttackEvent_NormalHammer[7];
s32 marioAttackEvent_GatsunHammer[7];
s32 marioAttackEvent_IceNaguri[7];
s32 marioAttackEvent_TsuranukiHammer[7];
s32 marioAttackEvent_NormalCharge[7];
s32 marioAttackEvent_NormalChargeP[7];
s32 marioAttackEvent_SuperCharge[7];
s32 marioAttackEvent_SuperChargeP[7];

// clang-format off

static USER_FUNC(_get_mario_hammer_lv) {
    s32* args = event->args;
    s32 value = args[0];

    evtSetValue(event, value, pouchGetPtr()->hammerLevel);
    return EVT_RETURN_DONE;
}


USER_FUNC(_whirlwind_effect) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    EffectEntry* effect;
    s32 value0, value1;
    Vec pos;

    value0 = evtGetValue(event, args[0]);
    value1 = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value0));
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    effect = effWhirlwindN64Entry((u32)unit->faceDirection >> 31, 0, value1, unit->mSizeMultiplier);
    ((EffWhirlwindData*)effect->userdata)->x = pos.x;
    ((EffWhirlwindData*)effect->userdata)->y = pos.y;
    ((EffWhirlwindData*)effect->userdata)->z = pos.z;
    return EVT_RETURN_DONE;
}


USER_FUNC(_jump_star_effect) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 value0, value1;
    s32 step;
    Vec pos;
    f32 a, b;

    value0 = evtGetValue(event, args[0]);
    value1 = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value0));
    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    step = event->userdata[0] / 4;
    if (event->userdata[0] % 4 == 0) {
        if (unit->faceDirection >= 0) {
            a = 10.0f;
            b = -10.0f;
        } else {
            a = -10.0f;
            b = 10.0f;
        }
        BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
        pos.x += b * unit->mSizeMultiplier;
        pos.y += (f32)(step * 8 / 10);
        {
            f32 scale = 0.1f * (f32)step + 0.8f;
            if (scale > 2.0f) {
                scale = 2.0f;
            }
            effEnergyEntry(value1, 50, pos.x, pos.y, pos.x, scale, a, 0.05f);
        }
    }
    if (event->userdata[0] <= 0x18) {
        event->userdata[0]++;
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}


USER_FUNC(_bgset_iron_frame_check) {
    s32* args = event->args;
    u8 flags = _battleWorkPointer->field_180C0.stageFlags;
    s32 value = args[0];

    if (flags & 0x20) {
        evtSetValue(event, value, 0);
    } else {
        evtSetValue(event, value, 1);
    }
    return EVT_RETURN_DONE;
}


USER_FUNC(_wait_jyabara_hit_iron_frame) {
    s32* args = event->args;
    BattleUnit* unit;
    Vec pos;
    s32 value;

    value = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, value));
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    if (pos.y + (f32)BtlUnit_GetHeight(unit) < 130.0f) {
        return EVT_RETURN_BLOCK;
    }
    BtlUnit_SetPos(unit, pos.x, (f32)(130 - BtlUnit_GetHeight(unit)), pos.z);
    return EVT_RETURN_DONE;
}


USER_FUNC(_tatsumaki_effect) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    EffectEntry* effect;
    s32 value;

    value = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value));
    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    switch (event->userdata[0]) {
    case 0: {
        Vec pos;

        event->userdata[0]++;
        event->userdata[1] = 0;
        event->userdata[2] = (s32)unit->mPosition.x;
        event->field_84 = (s32)unit->mPosition.y - 40;
        event->userdataEx[0] = (s32)unit->mPosition.z;
        event->userdataEx[1] = (s32)effKamekiTornadeN64Entry(0, 0x5A,
            (f32)event->userdata[2], (f32)(s32)event->field_84, (f32)event->userdataEx[0],
            3.0f);
        event->field_0x90[0] = (s32)effKamekiTornadeN64Entry(0, 0x5A,
            (f32)event->userdata[2], (f32)(s32)event->field_84, (f32)event->userdataEx[0],
            3.0f);
        event->field_0x90[1] = psndSFXOn("SFX_BTL_JUMP_TATUMAKI2");
        pos.x = (f32)event->userdata[2];
        pos.y = (f32)(s32)event->field_84;
        pos.z = (f32)event->userdataEx[0];
        psndSFX_pos(event->field_0x90[1], &pos);
    }
    case 1: {
        f32 amp;
        f32 c;
        f32 s;
        f32 angle;

        event->userdata[1]++;
        amp = intplGetValue(5, event->userdata[1], 90, 0.0f, 80.0f);
        angle = (f32)(event->userdata[1] * 4);
        c = amp * cosfd(angle);
        s = amp * sinfd(angle);
        ((effKamekiTornadeData*)((EffectEntry*)event->userdataEx[1])->userdata)->x =
            (f32)event->userdata[2] + c;
        ((effKamekiTornadeData*)((EffectEntry*)event->userdataEx[1])->userdata)->y =
            (f32)(s32)event->field_84;
        ((effKamekiTornadeData*)((EffectEntry*)event->userdataEx[1])->userdata)->z =
            (f32)event->userdataEx[0] + s;
        ((effKamekiTornadeData*)((EffectEntry*)event->field_0x90[0])->userdata)->x =
            (f32)event->userdata[2] - c;
        ((effKamekiTornadeData*)((EffectEntry*)event->field_0x90[0])->userdata)->y =
            (f32)(s32)event->field_84;
        ((effKamekiTornadeData*)((EffectEntry*)event->field_0x90[0])->userdata)->z =
            (f32)event->userdataEx[0] - s;
        if (event->userdata[1] >= 90) {
            psndSFXOff(event->field_0x90[1]);
            return EVT_RETURN_DONE;
        }
        break;
    }
    default:
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}


USER_FUNC(_record_renzoku_count) {
    s32* args = event->args;
    s32 count;
    PouchData* pouch;
    u8* record;

    count = evtGetValue(event, args[0]);
    record = (u8*)_battleWorkPointer + 0x16F24;
    pouch = pouchGetPtr();
    if (count > record[0]) {
        if (count > pouch->mPowerBounceRecord) {
            pouch->mPowerBounceRecord = count;
            BattleAudience_Case_JumpNewRecord();
        }
        if (count > 100) {
            count = 100;
        }
        record[0] = count;
    }
    return EVT_RETURN_DONE;
}


USER_FUNC(mario_get_renzoku_count_max) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 val0, val1;
    s32 dest;
    s32 max;
    BattleWork* wp;

    val0 = evtGetValue(event, args[0]);
    val1 = evtGetValue(event, args[1]);
    dest = args[2];
    wp = _battleWorkPointer;
    unit = BattleGetUnitPtr(wp, BattleTransID(event, val0));
    if (unit == NULL) {
        evtSetValue(event, dest, 0);
        return EVT_RETURN_DONE;
    }
    max = unit->mKindParams->field_0x14;
    if (val1 < max) {
        evtSetValue(event, dest, max);
        return EVT_RETURN_DONE;
    }
    if (val1 >= max * 2) {
        evtSetValue(event, dest, val1);
        return EVT_RETURN_DONE;
    }
    if (irand(100) < 33) {
        evtSetValue(event, dest, val1 + 1);
    } else {
        evtSetValue(event, dest, val1);
    }
    return EVT_RETURN_DONE;
}


// Read event->userdata[0] directly in each intplGetValue call (not via a shared
// local): a local makes MWCC hoist one `lwz` above the faceDirection test, while
// reading it in place lets the compiler schedule it as the last argument load.
USER_FUNC(_hammer_star_effect) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 value0, value1;
    Vec pos;
    f32 angle;
    f32 dir;

    value0 = evtGetValue(event, args[0]);
    value1 = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value0));
    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    if (unit->faceDirection >= 0) {
        angle = intplGetValue(1, event->userdata[0], 12, 210.0f, 420.0f);
        dir = -10.0f;
    } else {
        angle = intplGetValue(1, event->userdata[0], 12, 150.0f, -60.0f);
        dir = 10.0f;
    }
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    pos.y += unit->mSizeMultiplier * (0.85f * (f32)BtlUnit_GetHeight(unit));
    pos.x += unit->mSizeMultiplier * (26.0f * -sinfd(angle));
    pos.y += unit->mSizeMultiplier * (26.0f * cosfd(angle));
    effEnergyEntry(value1, 30, pos.x, pos.y, pos.x, 0.08f * event->userdata[0] + 0.1f, dir, 0.05f);
    if (event->userdata[0] < 12) {
        event->userdata[0]++;
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}


USER_FUNC(_kaiten_hammer_acrobat_rotate) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 value0, value1;
    s32 count;
    s32 anim;
    f32 x, y, z;

    value0 = evtGetValue(event, args[0]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value0));
    value1 = evtGetValue(event, args[1]);
    part = BtlUnit_GetPartsPtr(unit, value1);
    count = evtGetValue(event, args[2]);
    anim = evtGetValue(event, args[3]);
    if (isFirstCall) {
        BtlUnit_GetRotate(unit, &x, &y, &z);
        event->userdata[0] = count;
        if (unit->faceDirection >= 0) {
            event->userdata[1] = (s32)(360.0f + (360.0f - reviseAngle(y)));
        } else {
            event->userdata[1] = (s32)(360.0f + reviseAngle(y));
        }
        event->userdata[2] = (unit->faceDirection * event->userdata[1]) / count;
        event->field_84 = 0;
    }
    BtlUnit_AddRotate(unit, 0.0f, (f32)event->userdata[2], 0.0f);
    event->userdata[0]--;
    if ((s32)event->field_84 == 0) {
        event->userdata[1] -= event->userdata[2];
        if (event->userdata[1] <= 0x10E) {
            event->field_84 = 1;
            BtlUnit_SetAnim(part, (const char*)anim);
        }
    }
    if (event->userdata[0] > 0) {
        return EVT_RETURN_BLOCK;
    }
    BtlUnit_SetRotate(unit, 0.0f, 0.0f, 0.0f);
    return EVT_RETURN_DONE;
}


extern const f32 float_1_8041c0c4;

USER_FUNC(_mario_fire_ball_controll) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    EffectEntry* effect;
    effFireData* data;
    s32 value0, value1;
    BattleUnitPart* part;

    value0 = evtGetValue(event, args[0]);
    value1 = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, value0));
    part = BtlUnit_GetPartsPtr(unit, value1);
    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[1] = (s32)effFireEntry(5, 1000, 0.0f, 0.0f, 0.0f, float_1_8041c0c4);
    }
    effect = (EffectEntry*)event->userdata[1];
    if (part->work[0] == 0) {
        effDelete(effect);
        return EVT_RETURN_DONE;
    }
    data = effect->userdata;
    BtlUnit_GetPartsPos(part, &data->position.x, &data->position.y, &data->position.z);
    return EVT_RETURN_BLOCK;
}


USER_FUNC(_fire_wave) {
    s32* args = event->args;
    f32 x = (f32)evtGetValue(event, args[0]);
    f32 y = (f32)evtGetValue(event, args[1]);
    f32 z = (f32)evtGetValue(event, args[2]);
    f32 scale = 0.6f * (f32)evtGetValue(event, args[3]);

    if (isFirstCall) {
        event->userdata[0] = (s32)effHibashiraEntry(0, 1, 0x5A, x, y, z, x, y, z, scale);
    }
    return EVT_RETURN_DONE;
}


USER_FUNC(_majinai_powerup_check) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 var = args[0];

    if (pouchGetPtr()->mMerleeNextCurseType != 1) {
        evtSetValue(event, var, 0);
    } else {
        evtSetValue(event, var, 1);
        wp->field_18FE1 = 1;
        BattleMajinaiDone();
        BattleMajinaiEndCheck();
    }
    return EVT_RETURN_DONE;
}

BattleWeapon marioDefaultWeapon_Jump = {
    "btl_wn_mario_normal_jump",               // name
    159,                                      // icon
    ITEM_NULL,                                // item
    "msg_normal_jump",                        // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromMarioJumpLv,            // hpDamageFn
    { 1, 1, 2, 2, 3, 3, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_jump",                            // acHelpMsg
    0x00035807,                               // special
    0x0000000C,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_NormalJump,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 2, 2, 1, 1, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_KururinJump = {
    "btl_wn_mario_kururin_jump",              // name
    161,                                      // icon
    ITEM_NULL,                                // item
    "msg_kururin_jump",                       // description
    100,                                      // baseAccuracy
    2,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromMarioJumpLv,            // hpDamageFn
    { 1, 1, 2, 2, 3, 3, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_k_jump",                          // acHelpMsg
    0x00025807,                               // special
    0x0000000C,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_KururinJump,             // mAttackEvtScript
    { 0, 0, 0, 100, 0, 2, 5, 1, 1, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_KururinJump2 = {
    "btl_wn_mario_kururin_jump",                // name
    161,                                        // icon
    ITEM_NULL,                                  // item
    "msg_kururin_jump",                         // description
    100,                                        // baseAccuracy
    0,                                          // baseFpCost
    0,                                          // baseSpCost
    0,                                          // superguardType
    1.0f,                                       // field_0x14
    1,                                          // mStylishCommandMultiplier
    5,                                          // field_0x19
    100,                                        // mBingoSlotChance
    50,                                         // field_0x1B
    weaponGetPowerFromMarioJumpLv,              // hpDamageFn
    { 1, 2, 2, 4, 3, 6, 0, 0 },                 // hpDamageWork
    NULL,                                       // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
    0x01001260,                                 // targetClassFlags
    0x20001004,                                 // targetPropertyFlags
    0,                                          // element
    0,                                          // damagePattern
    3,                                          // actionLevel
    2,                                          // field_0x6F
    "msg_ac_k_jump",                            // acHelpMsg
    0x00025807,                                 // special
    0x0000000C,                                 // counterResistanceFlags
    0x80002004,                                 // targetWeightingFlags
    0,                                          // sleepChance
    0,                                          // sleepTime
    0,                                          // stopChance
    0,                                          // stopTime
    0,                                          // dizzyChance
    0,                                          // dizzyTime
    0,                                          // poisonChance
    0,                                          // poisonTime
    0,                                          // poisonStrength
    0,                                          // confuseChance
    0,                                          // confuseTime
    0,                                          // electricChance
    0,                                          // electricTime
    0,                                          // dodgyChance
    0,                                          // dodgyTime
    0,                                          // burnChance
    0,                                          // burnTime
    0,                                          // freezeChance
    0,                                          // freezeTime
    0,                                          // sizeChangeChance
    0,                                          // sizeChangeTime
    0,                                          // sizeChangeStrength
    0,                                          // atkChangeChance
    0,                                          // atkChangeTime
    0,                                          // atkChangeStrength
    0,                                          // defChangeChance
    0,                                          // defChangeTime
    0,                                          // defChangeStrength
    0,                                          // allergicChance
    0,                                          // allergicTime
    0,                                          // ohkoChance
    0,                                          // chargeStrength
    0,                                          // fastChance
    0,                                          // fastTime
    0,                                          // slowChance
    0,                                          // slowTime
    0,                                          // frightChance
    0,                                          // galeForceChance
    0,                                          // paybackTime
    0,                                          // holdFastTime
    0,                                          // invisibleChance
    0,                                          // invisibleTime
    0,                                          // hpRegenTime
    0,                                          // hpRegenStrength
    0,                                          // fpRegenTime
    0,                                          // fpRegenStrength
    marioAttackEvent_KururinJump,               // mAttackEvtScript
    { 0, 5, 5, 100, 0, 10, 10, 5, 5, 5, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_JyabaraJump = {
    "btl_wn_mario_jyabara_jump",                // name
    160,                                        // icon
    ITEM_NULL,                                  // item
    "msg_jyabara_jump",                         // description
    100,                                        // baseAccuracy
    4,                                          // baseFpCost
    0,                                          // baseSpCost
    0,                                          // superguardType
    1.0f,                                       // field_0x14
    2,                                          // mStylishCommandMultiplier
    5,                                          // field_0x19
    100,                                        // mBingoSlotChance
    50,                                         // field_0x1B
    weaponGetPowerFromMarioJumpLv,              // hpDamageFn
    { 1, 2, 2, 4, 3, 6, 0, 0 },                 // hpDamageWork
    NULL,                                       // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
    0x01001260,                                 // targetClassFlags
    0x20001004,                                 // targetPropertyFlags
    0,                                          // element
    0,                                          // damagePattern
    3,                                          // actionLevel
    2,                                          // field_0x6F
    "msg_ac_j_jump",                            // acHelpMsg
    0x00025807,                                 // special
    0x0000000C,                                 // counterResistanceFlags
    0x80002004,                                 // targetWeightingFlags
    0,                                          // sleepChance
    0,                                          // sleepTime
    0,                                          // stopChance
    0,                                          // stopTime
    0,                                          // dizzyChance
    0,                                          // dizzyTime
    0,                                          // poisonChance
    0,                                          // poisonTime
    0,                                          // poisonStrength
    0,                                          // confuseChance
    0,                                          // confuseTime
    0,                                          // electricChance
    0,                                          // electricTime
    0,                                          // dodgyChance
    0,                                          // dodgyTime
    0,                                          // burnChance
    0,                                          // burnTime
    0,                                          // freezeChance
    0,                                          // freezeTime
    0,                                          // sizeChangeChance
    0,                                          // sizeChangeTime
    0,                                          // sizeChangeStrength
    0,                                          // atkChangeChance
    0,                                          // atkChangeTime
    0,                                          // atkChangeStrength
    0,                                          // defChangeChance
    0,                                          // defChangeTime
    0,                                          // defChangeStrength
    0,                                          // allergicChance
    0,                                          // allergicTime
    0,                                          // ohkoChance
    0,                                          // chargeStrength
    0,                                          // fastChance
    0,                                          // fastTime
    0,                                          // slowChance
    0,                                          // slowTime
    0,                                          // frightChance
    0,                                          // galeForceChance
    0,                                          // paybackTime
    0,                                          // holdFastTime
    0,                                          // invisibleChance
    0,                                          // invisibleTime
    0,                                          // hpRegenTime
    0,                                          // hpRegenStrength
    0,                                          // fpRegenTime
    0,                                          // fpRegenStrength
    marioAttackEvent_JyabaraJump,               // mAttackEvtScript
    { 0, 5, 5, 100, 0, 10, 10, 5, 5, 5, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_JyabaraJumpFailed = {
    "btl_wn_mario_jyabara_jump",                 // name
    160,                                         // icon
    ITEM_NULL,                                   // item
    "msg_jyabara_jump",                          // description
    100,                                         // baseAccuracy
    4,                                           // baseFpCost
    0,                                           // baseSpCost
    0,                                           // superguardType
    1.0f,                                        // field_0x14
    2,                                           // mStylishCommandMultiplier
    5,                                           // field_0x19
    1,                                           // mBingoSlotChance
    50,                                          // field_0x1B
    NULL,                                        // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                  // hpDamageWork
    NULL,                                        // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                  // fpDamageWork
    0x01001260,                                  // targetClassFlags
    0x20001004,                                  // targetPropertyFlags
    0,                                           // element
    0,                                           // damagePattern
    3,                                           // actionLevel
    2,                                           // field_0x6F
    NULL,                                        // acHelpMsg
    0x00025807,                                  // special
    0x0000000C,                                  // counterResistanceFlags
    0x80002004,                                  // targetWeightingFlags
    0,                                           // sleepChance
    0,                                           // sleepTime
    0,                                           // stopChance
    0,                                           // stopTime
    0,                                           // dizzyChance
    0,                                           // dizzyTime
    0,                                           // poisonChance
    0,                                           // poisonTime
    0,                                           // poisonStrength
    0,                                           // confuseChance
    0,                                           // confuseTime
    0,                                           // electricChance
    0,                                           // electricTime
    0,                                           // dodgyChance
    0,                                           // dodgyTime
    0,                                           // burnChance
    0,                                           // burnTime
    0,                                           // freezeChance
    0,                                           // freezeTime
    0,                                           // sizeChangeChance
    0,                                           // sizeChangeTime
    0,                                           // sizeChangeStrength
    0,                                           // atkChangeChance
    0,                                           // atkChangeTime
    0,                                           // atkChangeStrength
    0,                                           // defChangeChance
    0,                                           // defChangeTime
    0,                                           // defChangeStrength
    0,                                           // allergicChance
    0,                                           // allergicTime
    0,                                           // ohkoChance
    0,                                           // chargeStrength
    0,                                           // fastChance
    0,                                           // fastTime
    0,                                           // slowChance
    0,                                           // slowTime
    0,                                           // frightChance
    0,                                           // galeForceChance
    0,                                           // paybackTime
    0,                                           // holdFastTime
    0,                                           // invisibleChance
    0,                                           // invisibleTime
    0,                                           // hpRegenTime
    0,                                           // hpRegenStrength
    0,                                           // fpRegenTime
    0,                                           // fpRegenStrength
    marioAttackEvent_JyabaraJump,                // mAttackEvtScript
    { 0, 50, 50, 100, 0, 0, 0, 50, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioDefaultWeapon_FirstAttackKaitenHammer = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerFromMarioHammerLv,          // hpDamageFn
    { 1, 2, 2, 4, 3, 6, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    14,                                       // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020887,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_FirstAttackKaitenHammer, // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioDefaultWeapon_FirstAttackUltraHammer = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerFromMarioHammerLv,          // hpDamageFn
    { 1, 2, 2, 4, 3, 6, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    16,                                       // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020887,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_FirstAttackKaitenHammer, // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioDefaultWeapon_Hammer = {
    "btl_wn_mario_normal_hammer",             // name
    156,                                      // icon
    ITEM_NULL,                                // item
    "msg_normal_hammer",                      // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromMarioHammerLv,          // hpDamageFn
    { 1, 2, 2, 4, 3, 6, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_hammer",                          // acHelpMsg
    0x00030887,                               // special
    0x000007FB,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_NormalHammer,            // mAttackEvtScript
    { 0, 3, 3, 100, 3, 3, 2, 3, 3, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_KaitenHammer = {
    "btl_wn_mario_kaiten_hammer",             // name
    157,                                      // icon
    ITEM_NULL,                                // item
    "msg_kaiten_hammer",                      // description
    100,                                      // baseAccuracy
    2,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromMarioHammerLv,          // hpDamageFn
    { 1, 2, 2, 4, 3, 6, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    14,                                       // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_kaiten_hammer",                   // acHelpMsg
    0x00020887,                               // special
    0x000007FB,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_KaitenHammer,            // mAttackEvtScript
    { 0, 0, 0, 100, 0, 3, 5, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_KaitenHammerInvolved = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    0.0f,                                     // field_0x14
    0,                                        // mStylishCommandMultiplier
    0,                                        // field_0x19
    0,                                        // mBingoSlotChance
    0,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 1, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02100060,                               // targetClassFlags
    0x20102000,                               // targetPropertyFlags
    0,                                        // element
    15,                                       // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020040,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_UltraHammer = {
    "btl_wn_mario_ultra_hammer",              // name
    158,                                      // icon
    ITEM_NULL,                                // item
    "msg_ultra_hammer",                       // description
    100,                                      // baseAccuracy
    4,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    2,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromMarioHammerLv,          // hpDamageFn
    { 1, 2, 2, 4, 3, 6, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    16,                                       // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_ultra_hammer",                    // acHelpMsg
    0x00020807,                               // special
    0x000007FB,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_KaitenHammer,            // mAttackEvtScript
    { 0, 0, 0, 100, 0, 3, 5, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_UltraHammerInvolved = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    0.0f,                                     // field_0x14
    0,                                        // mStylishCommandMultiplier
    0,                                        // field_0x19
    0,                                        // mBingoSlotChance
    0,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 3, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02100060,                               // targetClassFlags
    0x20102000,                               // targetPropertyFlags
    0,                                        // element
    17,                                       // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020040,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_UltraHammerFinish = {
    "btl_wn_mario_ultra_hammer",              // name
    158,                                      // icon
    ITEM_NULL,                                // item
    "msg_ultra_hammer",                       // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    2,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01100060,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    19,                                       // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x0002088F,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_KaitenHammer,            // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 5, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_TugitugiJump = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_MULTIBOUNCE,                         // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    2,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerOverlapJump1,               // hpDamageFn
    { 0, 0, 1, 1, 2, 2, 241, 1 },             // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_jump",                            // acHelpMsg
    0x00025807,                               // special
    0x0000000C,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_TugiTugiJump,            // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 2, 2, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_GatsunJump = {
    NULL,                                           // name
    0,                                              // icon
    ITEM_POWER_JUMP,                                // item
    NULL,                                           // description
    100,                                            // baseAccuracy
    2,                                              // baseFpCost
    0,                                              // baseSpCost
    0,                                              // superguardType
    1.0f,                                           // field_0x14
    1,                                              // mStylishCommandMultiplier
    5,                                              // field_0x19
    100,                                            // mBingoSlotChance
    50,                                             // field_0x1B
    weaponGetPowerOverlapJump1,                     // hpDamageFn
    { 1, 2, 2, 4, 3, 6, 240, 2 },                   // hpDamageWork
    NULL,                                           // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                     // fpDamageWork
    0x01001260,                                     // targetClassFlags
    0x20001004,                                     // targetPropertyFlags
    0,                                              // element
    0,                                              // damagePattern
    3,                                              // actionLevel
    2,                                              // field_0x6F
    "msg_ac_jump",                                  // acHelpMsg
    0x00025807,                                     // special
    0x0000000C,                                     // counterResistanceFlags
    0x80002004,                                     // targetWeightingFlags
    0,                                              // sleepChance
    0,                                              // sleepTime
    0,                                              // stopChance
    0,                                              // stopTime
    0,                                              // dizzyChance
    0,                                              // dizzyTime
    0,                                              // poisonChance
    0,                                              // poisonTime
    0,                                              // poisonStrength
    0,                                              // confuseChance
    0,                                              // confuseTime
    0,                                              // electricChance
    0,                                              // electricTime
    0,                                              // dodgyChance
    0,                                              // dodgyTime
    0,                                              // burnChance
    0,                                              // burnTime
    0,                                              // freezeChance
    0,                                              // freezeTime
    0,                                              // sizeChangeChance
    0,                                              // sizeChangeTime
    0,                                              // sizeChangeStrength
    0,                                              // atkChangeChance
    0,                                              // atkChangeTime
    0,                                              // atkChangeStrength
    0,                                              // defChangeChance
    0,                                              // defChangeTime
    0,                                              // defChangeStrength
    0,                                              // allergicChance
    0,                                              // allergicTime
    0,                                              // ohkoChance
    0,                                              // chargeStrength
    0,                                              // fastChance
    0,                                              // fastTime
    0,                                              // slowChance
    0,                                              // slowTime
    0,                                              // frightChance
    0,                                              // galeForceChance
    0,                                              // paybackTime
    0,                                              // holdFastTime
    0,                                              // invisibleChance
    0,                                              // invisibleTime
    0,                                              // hpRegenTime
    0,                                              // hpRegenStrength
    0,                                              // fpRegenTime
    0,                                              // fpRegenStrength
    marioAttackEvent_GatsunJump,                    // mAttackEvtScript
    { 0, 25, 25, 100, 25, 25, 25, 5, 25, 5, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_DokanJump = {
    NULL,                                        // name
    0,                                           // icon
    ITEM_MEGA_JUMP,                              // item
    NULL,                                        // description
    100,                                         // baseAccuracy
    6,                                           // baseFpCost
    0,                                           // baseSpCost
    0,                                           // superguardType
    1.0f,                                        // field_0x14
    1,                                           // mStylishCommandMultiplier
    5,                                           // field_0x19
    100,                                         // mBingoSlotChance
    50,                                          // field_0x1B
    weaponGetPowerOverlapJump1,                  // hpDamageFn
    { 1, 2, 2, 4, 3, 6, 332, 4 },                // hpDamageWork
    NULL,                                        // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                  // fpDamageWork
    0x01001260,                                  // targetClassFlags
    0x20001004,                                  // targetPropertyFlags
    0,                                           // element
    0,                                           // damagePattern
    3,                                           // actionLevel
    2,                                           // field_0x6F
    "msg_ac_jump",                               // acHelpMsg
    0x00025807,                                  // special
    0x0000000C,                                  // counterResistanceFlags
    0x80002004,                                  // targetWeightingFlags
    0,                                           // sleepChance
    0,                                           // sleepTime
    0,                                           // stopChance
    0,                                           // stopTime
    0,                                           // dizzyChance
    0,                                           // dizzyTime
    0,                                           // poisonChance
    0,                                           // poisonTime
    0,                                           // poisonStrength
    0,                                           // confuseChance
    0,                                           // confuseTime
    0,                                           // electricChance
    0,                                           // electricTime
    0,                                           // dodgyChance
    0,                                           // dodgyTime
    0,                                           // burnChance
    0,                                           // burnTime
    0,                                           // freezeChance
    0,                                           // freezeTime
    0,                                           // sizeChangeChance
    0,                                           // sizeChangeTime
    0,                                           // sizeChangeStrength
    0,                                           // atkChangeChance
    0,                                           // atkChangeTime
    0,                                           // atkChangeStrength
    0,                                           // defChangeChance
    0,                                           // defChangeTime
    0,                                           // defChangeStrength
    0,                                           // allergicChance
    0,                                           // allergicTime
    0,                                           // ohkoChance
    0,                                           // chargeStrength
    0,                                           // fastChance
    0,                                           // fastTime
    0,                                           // slowChance
    0,                                           // slowTime
    0,                                           // frightChance
    0,                                           // galeForceChance
    0,                                           // paybackTime
    0,                                           // holdFastTime
    0,                                           // invisibleChance
    0,                                           // invisibleTime
    0,                                           // hpRegenTime
    0,                                           // hpRegenStrength
    0,                                           // fpRegenTime
    0,                                           // fpRegenStrength
    marioAttackEvent_DokanJump,                  // mAttackEvtScript
    { 0, 25, 25, 100, 25, 5, 5, 5, 5, 5, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_RenzokuJump = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_POWER_BOUNCE,                        // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    3,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerOverlapJump1,               // hpDamageFn
    { 0, 0, 1, 1, 2, 2, 242, 1 },             // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_jump",                            // acHelpMsg
    0x00025817,                               // special
    0x0000000C,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_RenzokuJump,             // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 2, 2, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_TatsumakiJump = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_TORNADO_JUMP,                        // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    3,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    3,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerOverlapJump1,               // hpDamageFn
    { 0, 1, 1, 3, 2, 5, 243, 1 },             // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_tatsumaki_jump",                  // acHelpMsg
    0x00025807,                               // special
    0x0000000C,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_TatsumakiJump,           // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_TatsumakiJumpInvolved = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    0.0f,                                     // field_0x14
    0,                                        // mStylishCommandMultiplier
    0,                                        // field_0x19
    0,                                        // mBingoSlotChance
    0,                                        // field_0x1B
    weaponGetPowerTatsumaki,                  // hpDamageFn
    { 0, 2, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000010,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020040,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_MiniminiFumi = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_SHRINK_STOMP,                        // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    2,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromMarioJumpLv,            // hpDamageFn
    { 1, 1, 2, 2, 3, 3, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_jump",                            // acHelpMsg
    0x00025807,                               // special
    0x0000000C,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    100,                                      // sizeChangeChance
    3,                                        // sizeChangeTime
    -2,                                       // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_MiniminiFumi,            // mAttackEvtScript
    { 0, 0, 0, 100, 0, 3, 5, 1, 1, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_NemuraseFumi = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_SLEEPY_STOMP,                        // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    2,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromMarioJumpLv,            // hpDamageFn
    { 1, 1, 2, 2, 3, 3, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_jump",                            // acHelpMsg
    0x00025807,                               // special
    0x0000000C,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    100,                                      // sleepChance
    5,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_NemuraseFumi,            // mAttackEvtScript
    { 0, 0, 0, 100, 0, 3, 5, 1, 1, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_FunyafunyaJump = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_SOFT_STOMP,                          // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    2,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromMarioJumpLv,            // hpDamageFn
    { 1, 1, 2, 2, 3, 3, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_jump",                            // acHelpMsg
    0x00025807,                               // special
    0x0000000C,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    100,                                      // defChangeChance
    3,                                        // defChangeTime
    -3,                                       // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_FunyafunyaJump,          // mAttackEvtScript
    { 0, 0, 0, 100, 0, 3, 5, 1, 1, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_GatsunHammer = {
    NULL,                                           // name
    0,                                              // icon
    ITEM_POWER_SMASH,                               // item
    NULL,                                           // description
    100,                                            // baseAccuracy
    2,                                              // baseFpCost
    0,                                              // baseSpCost
    0,                                              // superguardType
    1.0f,                                           // field_0x14
    1,                                              // mStylishCommandMultiplier
    5,                                              // field_0x19
    100,                                            // mBingoSlotChance
    50,                                             // field_0x1B
    weaponGetPowerOverlapHammer1,                   // hpDamageFn
    { 1, 2, 2, 4, 3, 6, 247, 2 },                   // hpDamageWork
    NULL,                                           // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                     // fpDamageWork
    0x01001260,                                     // targetClassFlags
    0x21002000,                                     // targetPropertyFlags
    0,                                              // element
    0,                                              // damagePattern
    3,                                              // actionLevel
    2,                                              // field_0x6F
    "msg_ac_hammer",                                // acHelpMsg
    0x00020887,                                     // special
    0x000007FB,                                     // counterResistanceFlags
    0x80002004,                                     // targetWeightingFlags
    0,                                              // sleepChance
    0,                                              // sleepTime
    0,                                              // stopChance
    0,                                              // stopTime
    0,                                              // dizzyChance
    0,                                              // dizzyTime
    0,                                              // poisonChance
    0,                                              // poisonTime
    0,                                              // poisonStrength
    0,                                              // confuseChance
    0,                                              // confuseTime
    0,                                              // electricChance
    0,                                              // electricTime
    0,                                              // dodgyChance
    0,                                              // dodgyTime
    0,                                              // burnChance
    0,                                              // burnTime
    0,                                              // freezeChance
    0,                                              // freezeTime
    0,                                              // sizeChangeChance
    0,                                              // sizeChangeTime
    0,                                              // sizeChangeStrength
    0,                                              // atkChangeChance
    0,                                              // atkChangeTime
    0,                                              // atkChangeStrength
    0,                                              // defChangeChance
    0,                                              // defChangeTime
    0,                                              // defChangeStrength
    0,                                              // allergicChance
    0,                                              // allergicTime
    0,                                              // ohkoChance
    0,                                              // chargeStrength
    0,                                              // fastChance
    0,                                              // fastTime
    0,                                              // slowChance
    0,                                              // slowTime
    0,                                              // frightChance
    0,                                              // galeForceChance
    0,                                              // paybackTime
    0,                                              // holdFastTime
    0,                                              // invisibleChance
    0,                                              // invisibleTime
    0,                                              // hpRegenTime
    0,                                              // hpRegenStrength
    0,                                              // fpRegenTime
    0,                                              // fpRegenStrength
    marioAttackEvent_GatsunHammer,                  // mAttackEvtScript
    { 0, 25, 25, 100, 25, 25, 25, 5, 25, 5, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_DokanHammer = {
    NULL,                                        // name
    0,                                           // icon
    ITEM_MEGA_SMASH,                             // item
    NULL,                                        // description
    100,                                         // baseAccuracy
    6,                                           // baseFpCost
    0,                                           // baseSpCost
    0,                                           // superguardType
    1.0f,                                        // field_0x14
    1,                                           // mStylishCommandMultiplier
    5,                                           // field_0x19
    100,                                         // mBingoSlotChance
    50,                                          // field_0x1B
    weaponGetPowerOverlapHammer1,                // hpDamageFn
    { 1, 2, 2, 4, 3, 6, 333, 4 },                // hpDamageWork
    NULL,                                        // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                  // fpDamageWork
    0x01001260,                                  // targetClassFlags
    0x21002000,                                  // targetPropertyFlags
    0,                                           // element
    0,                                           // damagePattern
    3,                                           // actionLevel
    2,                                           // field_0x6F
    "msg_ac_hammer",                             // acHelpMsg
    0x00020887,                                  // special
    0x000007FB,                                  // counterResistanceFlags
    0x80002004,                                  // targetWeightingFlags
    0,                                           // sleepChance
    0,                                           // sleepTime
    0,                                           // stopChance
    0,                                           // stopTime
    0,                                           // dizzyChance
    0,                                           // dizzyTime
    0,                                           // poisonChance
    0,                                           // poisonTime
    0,                                           // poisonStrength
    0,                                           // confuseChance
    0,                                           // confuseTime
    0,                                           // electricChance
    0,                                           // electricTime
    0,                                           // dodgyChance
    0,                                           // dodgyTime
    0,                                           // burnChance
    0,                                           // burnTime
    0,                                           // freezeChance
    0,                                           // freezeTime
    0,                                           // sizeChangeChance
    0,                                           // sizeChangeTime
    0,                                           // sizeChangeStrength
    0,                                           // atkChangeChance
    0,                                           // atkChangeTime
    0,                                           // atkChangeStrength
    0,                                           // defChangeChance
    0,                                           // defChangeTime
    0,                                           // defChangeStrength
    0,                                           // allergicChance
    0,                                           // allergicTime
    0,                                           // ohkoChance
    0,                                           // chargeStrength
    0,                                           // fastChance
    0,                                           // fastTime
    0,                                           // slowChance
    0,                                           // slowTime
    0,                                           // frightChance
    0,                                           // galeForceChance
    0,                                           // paybackTime
    0,                                           // holdFastTime
    0,                                           // invisibleChance
    0,                                           // invisibleTime
    0,                                           // hpRegenTime
    0,                                           // hpRegenStrength
    0,                                           // fpRegenTime
    0,                                           // fpRegenStrength
    marioAttackEvent_GatsunHammer,               // mAttackEvtScript
    { 0, 25, 25, 100, 25, 5, 5, 5, 5, 5, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_JishinHammer = {
    NULL,                                                    // name
    0,                                                       // icon
    ITEM_QUAKE_HAMMER,                                       // item
    NULL,                                                    // description
    100,                                                     // baseAccuracy
    3,                                                       // baseFpCost
    0,                                                       // baseSpCost
    0,                                                       // superguardType
    1.0f,                                                    // field_0x14
    2,                                                       // mStylishCommandMultiplier
    5,                                                       // field_0x19
    100,                                                     // mBingoSlotChance
    50,                                                      // field_0x1B
    weaponGetPowerOverlapHammer1,                            // hpDamageFn
    { 4294967295, 0, 4294967295, 0, 4294967295, 0, 248, 2 }, // hpDamageWork
    NULL,                                                    // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                              // fpDamageWork
    0x02101260,                                              // targetClassFlags
    0x20000008,                                              // targetPropertyFlags
    0,                                                       // element
    0,                                                       // damagePattern
    3,                                                       // actionLevel
    2,                                                       // field_0x6F
    "msg_ac_hammer",                                         // acHelpMsg
    0x000238C7,                                              // special
    0x000007FF,                                              // counterResistanceFlags
    0x80002004,                                              // targetWeightingFlags
    0,                                                       // sleepChance
    0,                                                       // sleepTime
    0,                                                       // stopChance
    0,                                                       // stopTime
    0,                                                       // dizzyChance
    0,                                                       // dizzyTime
    0,                                                       // poisonChance
    0,                                                       // poisonTime
    0,                                                       // poisonStrength
    0,                                                       // confuseChance
    0,                                                       // confuseTime
    0,                                                       // electricChance
    0,                                                       // electricTime
    0,                                                       // dodgyChance
    0,                                                       // dodgyTime
    0,                                                       // burnChance
    0,                                                       // burnTime
    0,                                                       // freezeChance
    0,                                                       // freezeTime
    0,                                                       // sizeChangeChance
    0,                                                       // sizeChangeTime
    0,                                                       // sizeChangeStrength
    0,                                                       // atkChangeChance
    0,                                                       // atkChangeTime
    0,                                                       // atkChangeStrength
    0,                                                       // defChangeChance
    0,                                                       // defChangeTime
    0,                                                       // defChangeStrength
    0,                                                       // allergicChance
    0,                                                       // allergicTime
    0,                                                       // ohkoChance
    0,                                                       // chargeStrength
    0,                                                       // fastChance
    0,                                                       // fastTime
    0,                                                       // slowChance
    0,                                                       // slowTime
    0,                                                       // frightChance
    0,                                                       // galeForceChance
    0,                                                       // paybackTime
    0,                                                       // holdFastTime
    0,                                                       // invisibleChance
    0,                                                       // invisibleTime
    0,                                                       // hpRegenTime
    0,                                                       // hpRegenStrength
    0,                                                       // fpRegenTime
    0,                                                       // fpRegenStrength
    marioAttackEvent_JishinHammer,                           // mAttackEvtScript
    { 0, 50, 50, 100, 50, 25, 25, 5, 25, 5, 0, 0 },          // field_B4
};

BattleWeapon badgeWeapon_UltraJishinHammer = {
    NULL,                                        // name
    0,                                           // icon
    ITEM_MEGA_QUAKE,                             // item
    "msg_ultra_attack",                          // description
    100,                                         // baseAccuracy
    7,                                           // baseFpCost
    0,                                           // baseSpCost
    0,                                           // superguardType
    1.0f,                                        // field_0x14
    1,                                           // mStylishCommandMultiplier
    5,                                           // field_0x19
    100,                                         // mBingoSlotChance
    50,                                          // field_0x1B
    weaponGetPowerOverlapHammer1,                // hpDamageFn
    { 3, 6, 3, 6, 3, 6, 334, 4 },                // hpDamageWork
    NULL,                                        // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                  // fpDamageWork
    0x02101260,                                  // targetClassFlags
    0x20000008,                                  // targetPropertyFlags
    0,                                           // element
    0,                                           // damagePattern
    3,                                           // actionLevel
    2,                                           // field_0x6F
    "msg_ac_ultra_jishin_hammer",                // acHelpMsg
    0x000238C7,                                  // special
    0x000007FF,                                  // counterResistanceFlags
    0x80002004,                                  // targetWeightingFlags
    0,                                           // sleepChance
    0,                                           // sleepTime
    0,                                           // stopChance
    0,                                           // stopTime
    0,                                           // dizzyChance
    0,                                           // dizzyTime
    0,                                           // poisonChance
    0,                                           // poisonTime
    0,                                           // poisonStrength
    0,                                           // confuseChance
    0,                                           // confuseTime
    0,                                           // electricChance
    0,                                           // electricTime
    0,                                           // dodgyChance
    0,                                           // dodgyTime
    0,                                           // burnChance
    0,                                           // burnTime
    0,                                           // freezeChance
    0,                                           // freezeTime
    0,                                           // sizeChangeChance
    0,                                           // sizeChangeTime
    0,                                           // sizeChangeStrength
    0,                                           // atkChangeChance
    0,                                           // atkChangeTime
    0,                                           // atkChangeStrength
    0,                                           // defChangeChance
    0,                                           // defChangeTime
    0,                                           // defChangeStrength
    0,                                           // allergicChance
    0,                                           // allergicTime
    0,                                           // ohkoChance
    0,                                           // chargeStrength
    0,                                           // fastChance
    0,                                           // fastTime
    0,                                           // slowChance
    0,                                           // slowTime
    0,                                           // frightChance
    0,                                           // galeForceChance
    0,                                           // paybackTime
    0,                                           // holdFastTime
    0,                                           // invisibleChance
    0,                                           // invisibleTime
    0,                                           // hpRegenTime
    0,                                           // hpRegenStrength
    0,                                           // fpRegenTime
    0,                                           // fpRegenStrength
    marioAttackEvent_UltraJishinHammer,          // mAttackEvtScript
    { 0, 50, 50, 100, 50, 5, 5, 5, 5, 5, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_HammerNageru = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_HAMMER_THROW,                        // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    2,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerOverlapHammer1,             // hpDamageFn
    { 0, 1, 1, 3, 2, 5, 249, 1 },             // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_hammer_nageru",                   // acHelpMsg
    0x00020887,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_HammerNageru,            // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_TsuranukiNaguri = {
    NULL,                                       // name
    0,                                          // icon
    ITEM_PIERCING_BLOW,                         // item
    NULL,                                       // description
    100,                                        // baseAccuracy
    2,                                          // baseFpCost
    0,                                          // baseSpCost
    0,                                          // superguardType
    1.0f,                                       // field_0x14
    1,                                          // mStylishCommandMultiplier
    5,                                          // field_0x19
    100,                                        // mBingoSlotChance
    50,                                         // field_0x1B
    weaponGetPowerFromMarioHammerLv,            // hpDamageFn
    { 1, 2, 2, 4, 3, 6, 0, 0 },                 // hpDamageWork
    NULL,                                       // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
    0x01001260,                                 // targetClassFlags
    0x21002000,                                 // targetPropertyFlags
    0,                                          // element
    0,                                          // damagePattern
    3,                                          // actionLevel
    2,                                          // field_0x6F
    "msg_ac_hammer",                            // acHelpMsg
    0x000208C7,                                 // special
    0x000007FB,                                 // counterResistanceFlags
    0x80002004,                                 // targetWeightingFlags
    0,                                          // sleepChance
    0,                                          // sleepTime
    0,                                          // stopChance
    0,                                          // stopTime
    0,                                          // dizzyChance
    0,                                          // dizzyTime
    0,                                          // poisonChance
    0,                                          // poisonTime
    0,                                          // poisonStrength
    0,                                          // confuseChance
    0,                                          // confuseTime
    0,                                          // electricChance
    0,                                          // electricTime
    0,                                          // dodgyChance
    0,                                          // dodgyTime
    0,                                          // burnChance
    0,                                          // burnTime
    0,                                          // freezeChance
    0,                                          // freezeTime
    0,                                          // sizeChangeChance
    0,                                          // sizeChangeTime
    0,                                          // sizeChangeStrength
    0,                                          // atkChangeChance
    0,                                          // atkChangeTime
    0,                                          // atkChangeStrength
    0,                                          // defChangeChance
    0,                                          // defChangeTime
    0,                                          // defChangeStrength
    0,                                          // allergicChance
    0,                                          // allergicTime
    0,                                          // ohkoChance
    0,                                          // chargeStrength
    0,                                          // fastChance
    0,                                          // fastTime
    0,                                          // slowChance
    0,                                          // slowTime
    0,                                          // frightChance
    0,                                          // galeForceChance
    0,                                          // paybackTime
    0,                                          // holdFastTime
    0,                                          // invisibleChance
    0,                                          // invisibleTime
    0,                                          // hpRegenTime
    0,                                          // hpRegenStrength
    0,                                          // fpRegenTime
    0,                                          // fpRegenStrength
    marioAttackEvent_TsuranukiHammer,           // mAttackEvtScript
    { 0, 25, 25, 100, 0, 5, 5, 5, 5, 5, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_ConfuseHammer = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_HEAD_RATTLE,                         // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    2,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromMarioHammerLv,          // hpDamageFn
    { 1, 2, 2, 4, 3, 6, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_hammer",                          // acHelpMsg
    0x00020887,                               // special
    0x000007FB,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    100,                                      // confuseChance
    3,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_NormalHammer,            // mAttackEvtScript
    { 0, 5, 5, 100, 0, 5, 5, 5, 5, 5, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_FireNaguri = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_FIRE_DRIVE,                          // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    5,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    2,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerOverlapHammer1,             // hpDamageFn
    { 4294967295, 3, 0, 3, 1, 3, 252, 2 },    // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02001260,                               // targetClassFlags
    0x20002000,                               // targetPropertyFlags
    1,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_fire_naguri",                     // acHelpMsg
    0x00020867,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    100,                                      // burnChance
    3,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_FireNaguri,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 5, 5, 5, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_FireNaguriFailed = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_FIRE_DRIVE,                          // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    5,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    2,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    1,                                        // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromMarioHammerLv,          // hpDamageFn
    { 1, 5, 2, 5, 3, 5, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02100160,                               // targetClassFlags
    0x10002000,                               // targetPropertyFlags
    1,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020007,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    100,                                      // burnChance
    3,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_FireNaguri,              // mAttackEvtScript
    { 0, 0, 0, 100, 0, 5, 5, 5, 5, 5, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_IceNaguri = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_ICE_SMASH,                           // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    3,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPowerFromMarioHammerLv,          // hpDamageFn
    { 1, 2, 2, 4, 3, 6, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    2,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_ac_hammer",                          // acHelpMsg
    0x00020807,                               // special
    0x000007FB,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    100,                                      // freezeChance
    2,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_IceNaguri,               // mAttackEvtScript
    { 0, 5, 5, 100, 0, 5, 5, 5, 5, 5, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_Charge = {
    "btl_wn_mario_normal_charge",             // name
    159,                                      // icon
    ITEM_CHARGE,                              // item
    "btl_hlp_cmd_operation_charge",           // description
    100,                                      // baseAccuracy
    1,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01004020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020108,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    2,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_NormalCharge,            // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_ChargeP = {
    "btl_wn_mario_normal_charge",             // name
    159,                                      // icon
    ITEM_CHARGE_P,                            // item
    "btl_hlp_cmd_operation_charge",           // description
    100,                                      // baseAccuracy
    1,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01004020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020108,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    2,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_NormalChargeP,           // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_SuperCharge = {
    "btl_wn_mario_super_charge",              // name
    159,                                      // icon
    ITEM_SUPER_CHARGE,                        // item
    "btl_hlp_cmd_operation_super_charge",     // description
    100,                                      // baseAccuracy
    2,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01004020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020108,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    2,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_SuperCharge,             // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_SuperChargeP = {
    "btl_wn_mario_super_charge",              // name
    159,                                      // icon
    ITEM_SUPER_CHARGE_P,                      // item
    "btl_hlp_cmd_operation_super_charge",     // description
    100,                                      // baseAccuracy
    2,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01004020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020108,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    2,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_SuperChargeP,            // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_2KaiItem = {
    "btl_wn_2kai_item",                       // name
    0,                                        // icon
    ITEM_DOUBLE_DIP,                          // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    4,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    0.0f,                                     // field_0x14
    0,                                        // mStylishCommandMultiplier
    0,                                        // field_0x19
    0,                                        // mBingoSlotChance
    0,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01004020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020108,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_3KaiItem = {
    "btl_wn_3kai_item",                       // name
    0,                                        // icon
    ITEM_TRIPLE_DIP,                          // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    8,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    0.0f,                                     // field_0x14
    0,                                        // mStylishCommandMultiplier
    0,                                        // field_0x19
    0,                                        // mBingoSlotChance
    0,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01004020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020108,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_DefenceCommand = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_UNUSED_DEFEND_BADGE,                 // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    0.0f,                                     // field_0x14
    0,                                        // mStylishCommandMultiplier
    0,                                        // field_0x19
    0,                                        // mBingoSlotChance
    0,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01004020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020008,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    100,                                      // defChangeChance
    1,                                        // defChangeTime
    1,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon badgeWeapon_DefenceCommandP = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_UNUSED_DEFEND_BADGE_P,               // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    0.0f,                                     // field_0x14
    0,                                        // mStylishCommandMultiplier
    0,                                        // field_0x19
    0,                                        // mBingoSlotChance
    0,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01004020,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020008,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    100,                                      // defChangeChance
    1,                                        // defChangeTime
    1,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_BakuGame = {
    "btl_wn_sac_bakugame",                    // name
    408,                                      // icon
    ITEM_NULL,                                // item
    "msg_bakugame",                           // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    2,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02100060,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_sac_3",                              // acHelpMsg
    0x00020088,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    100,                                      // stopChance
    1,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_BakuGame,                // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_Scissor = {
    "btl_wn_sac_scissor",                     // name
    412,                                      // icon
    ITEM_NULL,                                // item
    "msg_scissor",                            // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    4,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPower_Scissor,                   // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_sac_5",                              // acHelpMsg
    0x000200C8,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_Scissor,                 // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_ZubaStar = {
    "btl_wn_sac_zubastar",                    // name
    411,                                      // icon
    ITEM_NULL,                                // item
    "msg_zubastar",                           // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    6,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPower_ZubaStar,                  // hpDamageFn
    { 0, 3, 6, 9, 12, 15, 0, 0 },             // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_sac_8",                              // acHelpMsg
    0x000200C8,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_ZubaStar,                // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_Genki0 = {
    "btl_wn_sac_genki0",                      // name
    305,                                      // icon
    ITEM_NULL,                                // item
    "msg_genkigenki",                         // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    1,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02100070,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_sac_1",                              // acHelpMsg
    0x00020008,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_Genki0,                  // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_Genki1 = {
    "btl_wn_sac_genki1",                      // name
    413,                                      // icon
    ITEM_NULL,                                // item
    "msg_super_genki",                        // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    5,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02100070,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_sac_6",                              // acHelpMsg
    0x00020008,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_Genki1,                  // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_Deka = {
    "btl_wn_sac_dekkaku_dokkan",              // name
    407,                                      // icon
    ITEM_NULL,                                // item
    "msg_dekkaku_dokkan",                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    2,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    weaponGetPower_Deka,                      // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_sac_2",                              // acHelpMsg
    0x000200C8,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_Deka,                    // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_Muki = {
    "btl_wn_sac_mukimuki_body",               // name
    410,                                      // icon
    ITEM_NULL,                                // item
    "msg_mukimuki_body",                      // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    3,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02100070,                               // targetClassFlags
    0x10000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_sac_4",                              // acHelpMsg
    0x00020008,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_Muki,                    // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon marioWeapon_Suki = {
    "btl_wn_sac_sukkari_sukkiri",             // name
    409,                                      // icon
    ITEM_NULL,                                // item
    "msg_sukkari_sukkiri",                    // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    2,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    5,                                        // field_0x19
    100,                                      // mBingoSlotChance
    50,                                       // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    "msg_sac_7",                              // acHelpMsg
    0x00020008,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    100,                                      // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    marioAttackEvent_Suki,                    // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

BattleWeapon* superActionTable[18] = {
    &marioWeapon_Genki0, &marioWeapon_Deka, &marioWeapon_BakuGame, &marioWeapon_Muki, &marioWeapon_Scissor,
    &marioWeapon_Genki1, &marioWeapon_Suki, &marioWeapon_ZubaStar, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL,
};

static EVT_BEGIN(marioAttackEvent_NormalJump)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            GOTO(99)
        END_IF
        SET_VALUE(LW(12), PTR(&marioDefaultWeapon_Jump))
        CALL(btlevtcmd_ACHelpSet, PTR(&marioDefaultWeapon_Jump))
        CALL(evt_btl_camera_set_mode, 0, 10)
        CALL(evt_btl_camera_set_homing_unitparts, 0, LW(3), LW(4), -2, 1)
        CALL(evt_btl_camera_set_zoom, 0, 250)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    ELSE
        CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
        CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
        CALL(btlevtcmd_WaitGuardMove)
        CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
        CALL(btlevtcmd_RunDataEventChild, -2, 7)
        CALL_CHILD(marioAttackEvent_MajinaiPowerUpCheck)
        CALL(evt_btl_camera_set_mode, 0, 11)
        CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(evt_btl_camera_set_zoom, 0, 250)
    END_IF
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetWeaponActionLv, LW(12), LW(0))
        CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
        CALL(btlevtcmd_SetupAC, -2, 1, 1, 0)
    END_IF
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
        CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
        WAIT_MS(1000)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(6))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(6))
        CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), 0, LW(7))
        IF_INT_GREATER(LW(7), 22)
            SUBTRACT(LW(7), 22)
        ELSE
            SUBTRACT(LW(7), 22)
            MULTIPLY(LW(7), -1)
        END_IF
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(LW(7))
            CALL(btlevtcmd_GetWeaponActionLv, LW(12), LW(5))
            CALL(btlevtcmd_AcSetDifficulty, -2, LW(5))
            CALL(btlevtcmd_SetupAC, -2, 1, 1, 0)
        END_SIBLING_EVENT
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    ELSE
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(6))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(6))
        BEGIN_SIBLING_EVENT
            SWITCH(LW(12))
                CASE_EQUAL(PTR(&badgeWeapon_MiniminiFumi))
                SET_VALUE(LW(13), 4)
                CASE_EQUAL(PTR(&badgeWeapon_NemuraseFumi))
                SET_VALUE(LW(13), 3)
                CASE_EQUAL(PTR(&badgeWeapon_FunyafunyaJump))
                SET_VALUE(LW(13), 1)
            END_SWITCH
            SWITCH(LW(12))
                CASE_OR_EQUAL(PTR(&badgeWeapon_MiniminiFumi))
                CASE_OR_EQUAL(PTR(&badgeWeapon_NemuraseFumi))
                CASE_OR_EQUAL(PTR(&badgeWeapon_FunyafunyaJump))
                CALL(_jump_star_effect, -2, LW(13))
                CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_MARIO_HAMMER_SHINE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                CASE_END
            END_SWITCH
        END_SIBLING_EVENT
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_StartAC, 1)
    SET_VALUE(LW(10), 0)
    LABEL(10)
    IF_INT_GREATER_OR_EQUAL(LW(10), 1)
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 1048832, LW(5))
    ELSE
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    END_IF
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(4)
        CASE_OR_EQUAL(6)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
        WAIT_MS(1000)
    END_SWITCH
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    IF_INT_GREATER_OR_EQUAL(LW(10), 1)
        SET_VALUE(LW(6), 36)
        CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(6))
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(3)
            SET_VALUE(LW(0), LW(6))
            DIVIDE(LW(0), 2)
            SUBTRACT(LW(0), 3)
            SET_VALUE(LW(1), LW(0))
            SET_VALUE(LW(2), LW(0))
            SUBTRACT(LW(1), 3)
            ADD(LW(2), 3)
            SET_VALUE(LW(0), LW(6))
            SUBTRACT(LW(0), 4)
            CALL(btlevtcmd_ACRStart, -2, LW(1), LW(2), LW(0), 0)
            CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
            SWITCH(LW(6))
                CASE_GREATER_OR_EQUAL(2)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_4"))
                CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, -60, -25, 0)
                CALL(btlevtcmd_snd_voice, -2, 1)
                CASE_DEFAULT
                CALL(evt_audience_acrobat_notry)
            END_SWITCH
        END_SIBLING_EVENT
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(6), 3, -1)
    ELSE
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        SET_VALUE(LW(6), 36)
        CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(6))
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(6), 0, -1)
    END_IF
    IF_INT_NOT_EQUAL(LW(5), 1)
        IF_INT_EQUAL(LW(5), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(5), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(5), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_StopAC)
        CALL(btlevtcmd_JumpContinue, -2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_LAND_DAMAGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_D_2"))
        WAIT_FRAMES(40)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1D"))
    CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
    CALL(btlevtcmd_ac_timing_get_success_frame, LW(0))
    DIVIDE(LW(0), 2)
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_ac_timing_flag_onoff, 0, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
        IF_INT_EQUAL(LW(10), 0)
            CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
        END_IF
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_StopAC)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(btlevtcmd_WeaponAftereffect, LW(12))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 27)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_snd_voice, -2, 2)
    CALL(btlevtcmd_GetResultCountAC, LW(6))
    IF_INT_LESS_OR_EQUAL(LW(6), 1)
        BEGIN_INLINE_EVENT
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
            CALL(evt_btl_camera_set_zoom, 0, 200)
            WAIT_FRAMES(10)
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
            CALL(evt_btl_camera_set_zoom, 0, 250)
        END_INLINE_EVENT
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131072, LW(5))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
        WAIT_FRAMES(2)
        ADD(LW(10), 1)
        GOTO(10)
    ELSE
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131328, LW(5))
        WAIT_FRAMES(2)
    END_IF
    LABEL(90)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StopAC)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(marioAttackEvent_KururinJump)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.4f))
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(12), PTR(&marioWeapon_KururinJump))
        CALL(btlevtcmd_ACHelpSet, PTR(&marioWeapon_KururinJump))
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            GOTO(99)
        END_IF
        CALL(evt_btl_camera_set_mode, 0, 10)
        CALL(evt_btl_camera_set_homing_unitparts, 0, LW(3), LW(4), -2, 1)
        CALL(evt_btl_camera_set_zoom, 0, 250)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
        CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
        WAIT_FRAMES(30)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(6))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(6))
        CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), 0, LW(7))
        IF_INT_GREATER(LW(7), 22)
            SUBTRACT(LW(7), 22)
        ELSE
            SUBTRACT(LW(7), 22)
            MULTIPLY(LW(7), -1)
        END_IF
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(LW(7))
            CALL(btlevtcmd_GetWeaponActionLv, LW(12), LW(5))
            CALL(btlevtcmd_AcSetDifficulty, -2, LW(5))
            CALL(btlevtcmd_SetupAC, -2, 1, 1, 0)
        END_SIBLING_EVENT
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    ELSE
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
        CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
        CALL(btlevtcmd_WaitGuardMove)
        CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
        CALL(btlevtcmd_RunDataEventChild, -2, 7)
        CALL_CHILD(marioAttackEvent_MajinaiPowerUpCheck)
        CALL(evt_btl_camera_set_mode, 0, 11)
        CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(evt_btl_camera_set_zoom, 0, 250)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_GetWeaponActionLv, LW(12), LW(0))
        CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
        CALL(btlevtcmd_SetupAC, -2, 1, 1, 0)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(6))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(6))
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_StartAC, 1)
    SET_VALUE(LW(10), 0)
    LABEL(10)
    IF_INT_GREATER_OR_EQUAL(LW(10), 1)
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 1048832, LW(5))
    ELSE
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    END_IF
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(4)
        CASE_OR_EQUAL(6)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
        WAIT_MS(1000)
    END_SWITCH
    IF_INT_LESS_OR_EQUAL(LW(10), 0)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
        SET_VALUE(LW(6), 36)
        CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(6))
        BEGIN_SIBLING_EVENT
            SET_VALUE(LW(0), LW(6))
            SET_VALUE(LW(1), LW(6))
            DIVIDE(LW(0), 2)
            SUBTRACT(LW(1), 1)
            CALL(btlevtcmd_ACRStart, -2, 5, LW(0), LW(1), 0)
            CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
            SWITCH(LW(6))
                CASE_GREATER_OR_EQUAL(2)
                CALL(btlevtcmd_snd_voice, -2, 1)
                CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
                CALL(btlevtcmd_SetRotateOffset, -2, 0, 14, 0)
                LOOP(15)
                    CALL(btlevtcmd_AddRotate, -2, 0, 0, -24)
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                CASE_DEFAULT
                CALL(evt_audience_acrobat_notry)
            END_SWITCH
        END_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(6), 0, -1)
        IF_INT_NOT_EQUAL(LW(5), 1)
            IF_INT_EQUAL(LW(5), 3)
                CALL(btlevtcmd_StartAvoid, LW(3), 38)
            END_IF
            IF_INT_EQUAL(LW(5), 6)
                CALL(btlevtcmd_StartAvoid, LW(3), 39)
            END_IF
            IF_INT_EQUAL(LW(5), 2)
                CALL(btlevtcmd_StartAvoid, LW(3), 40)
            END_IF
            CALL(btlevtcmd_StopAC)
            CALL(btlevtcmd_JumpContinue, -2)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_LAND_DAMAGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_D_2"))
            WAIT_FRAMES(40)
            GOTO(95)
        END_IF
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1D"))
        CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
        CALL(btlevtcmd_ac_timing_get_success_frame, LW(0))
        DIVIDE(LW(0), 2)
        WAIT_FRAMES(LW(0))
        CALL(btlevtcmd_ac_timing_flag_onoff, 0, 1)
        CALL(btlevtcmd_ResultAC)
        CALL(btlevtcmd_GetResultAC, LW(6))
        IF_BITS_CLEAR(LW(6), 2)
            CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
            CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(btlevtcmd_StopAC)
            CALL(evt_audience_ap_recovery)
            CALL(btlevtcmd_InviteApInfoReport)
            CALL(btlevtcmd_WeaponAftereffect, LW(12))
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
            CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 27)
            CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
            CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
            CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
            CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
            CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
            CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
            GOTO(95)
        END_IF
        BEGIN_INLINE_EVENT
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
            CALL(evt_btl_camera_set_zoom, 0, 200)
            WAIT_FRAMES(10)
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
            CALL(evt_btl_camera_set_zoom, 0, 250)
        END_INLINE_EVENT
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131072, LW(5))
        ADD(LW(10), 1)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    END_IF
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        SET_VALUE(LW(6), 36)
        CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(6))
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(6), 0, -1)
    END_SIBLING_EVENT
    WAIT_FRAMES(15)
    DELETE_EVENT(LW(14))
    GOTO(55)
    LABEL(52)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_N_7"))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
    CALL(btlevtcmd_FallPosition, -2, LW(0), LW(1), LW(2), 14)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_LAND_DAMAGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_D_2"))
    WAIT_FRAMES(28)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_U_3"))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_StopAC)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    GOTO(95)
    LABEL(55)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(15))
    MULTIPLY(LW(15), -30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_1A"))
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 12, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    WAIT_FRAMES(15)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_JUMP_KURURIN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_POWER1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LOOP(12)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(15))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
    WAIT_FRAMES(15)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 4, 0, 0, 0, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_1B"))
    WAIT_FRAMES(2)
    CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
    CALL(btlevtcmd_ac_timing_get_success_frame, LW(0))
    DIVIDE(LW(0), 2)
    ADD(LW(0), 1)
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_ac_timing_flag_onoff, 0, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_SET(LW(6), 2)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&marioWeapon_KururinJump2), 131328, LW(5))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 1, LW(6))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_AudienceDeclareACResult, PTR(&marioWeapon_KururinJump2), LW(6))
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_StopAC)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(btlevtcmd_WeaponAftereffect, LW(12))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&marioWeapon_KururinJump2), 256, LW(5))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_AudienceDeclareACResult, PTR(&marioWeapon_KururinJump2), LW(6))
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_StopAC)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(btlevtcmd_WeaponAftereffect, LW(12))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 27)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    END_IF
    GOTO(95)
    LABEL(90)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StopAC)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_JyabaraJump)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL_CHILD(marioAttackEvent_MajinaiPowerUpCheck)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetWeaponActionLv, LW(12), LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_SetupAC, -2, 1, 1, 0)
    WAIT_FRAMES(21)
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_S_1"))
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AnimeChangePaperGroup, -2, 1, STRING("p_jyabara"), 1)
    CALL(btlevtcmd_AnimeChangePaperAnime, -2, 1, STRING("PM_J_1E"))
    WAIT_FRAMES(16)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_POWER2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_JUMP_JABARA1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePaperGroup, -2, 1, STRING("p_jyabara"), 0)
    CALL(btlevtcmd_AnimeChangePaperAnime, -2, 1, STRING("PM_J_1A"))
    WAIT_FRAMES(8)
    CALL(btlevtcmd_AnimeChangePaperAnime, -2, 1, STRING("PM_J_1B"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_JUMP_JABARA2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePaperAnime, -2, 1, STRING("PM_J_2A"))
    WAIT_FRAMES(8)
    CALL(btlevtcmd_AnimeChangePaperAnime, -2, 1, STRING("PM_J_2B"))
    WAIT_FRAMES(10)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_JUMP_JABARA3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePaperAnime, -2, 1, STRING("PM_J_3A"))
    WAIT_FRAMES(10)
    CALL(btlevtcmd_AnimeChangePaperAnime, -2, 1, STRING("PM_J_3B"))
    WAIT_FRAMES(20)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_JUMP_JABARA4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_AC3_5"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(_bgset_iron_frame_check, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
        CALL(btlevtcmd_AnimeChangePaperAnime, -2, 1, STRING("PM_J_1C"))
        BEGIN_INLINE_EVENT_GET_ID(LW(14))
            LOOP(50)
                CALL(btlevtcmd_AddRotate, -2, 0, 48, 0)
                WAIT_FRAMES(1)
            END_LOOP
            CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        END_INLINE_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
        CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_BTL_JUMP_CONTINUE1"), 0, 0, -1, -1, 0, 0)
        BEGIN_INLINE_EVENT_GET_ID(LW(15))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 90, -2)
        END_INLINE_EVENT
        CALL(_wait_jyabara_hit_iron_frame, -2)
        DELETE_EVENT(LW(14))
        DELETE_EVENT(LW(15))
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_WeaponAftereffect, PTR(&marioWeapon_JyabaraJumpFailed))
        CALL(btlevtcmd_StopAC)
        CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
        BEGIN_SIBLING_EVENT
            SET_FLOAT_VALUE(LW(0), FLOAT(1.0f))
            LOOP(5)
                WAIT_FRAMES(1)
                SUBTRACT_FLOAT(LW(0), FLOAT(0.2f))
                CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), LW(0), FLOAT(1.0f))
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_DivePosition, -2, LW(0), 130, LW(2), 5, 0, 0, 0, -1)
        WAIT_FRAMES(16)
        CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
        CALL(btlevtcmd_AnimeChangePaperAnime, -2, 1, STRING("PM_J_1H"))
        CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
        CALL(btlevtcmd_TransStageFloorPosition, LW(5), LW(6), LW(7))
        BEGIN_SIBLING_EVENT
            SET_VALUE(LW(5), LW(6))
            CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(9), EVTDAT_POINTER_MAX)
            SUBTRACT(LW(9), LW(5))
            DIVIDE(LW(9), 4)
            CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(6), EVTDAT_POINTER_MAX)
            SUBTRACT(LW(6), LW(9))
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(7), LW(1))
            CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(7), LW(1))
            ADD(LW(7), LW(9))
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(8), LW(1))
            CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(8), LW(1))
            CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(0), EVTDAT_POINTER_MAX)
            CALL(evt_sub_intpl_init, 0, LW(0), LW(6), 32)
            LOOP(32)
                CALL(evt_sub_intpl_get_value)
                CALL(btlevtcmd_SetPos, -2, EVTDAT_POINTER_MAX, LW(0), EVTDAT_POINTER_MAX)
                WAIT_FRAMES(1)
            END_LOOP
            CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(0), EVTDAT_POINTER_MAX)
            CALL(evt_sub_intpl_init, 0, LW(0), LW(7), 64)
            LOOP(64)
                CALL(evt_sub_intpl_get_value)
                CALL(btlevtcmd_SetPos, -2, EVTDAT_POINTER_MAX, LW(0), EVTDAT_POINTER_MAX)
                WAIT_FRAMES(1)
            END_LOOP
            CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(0), EVTDAT_POINTER_MAX)
            CALL(evt_sub_intpl_init, 0, LW(0), LW(8), 32)
            LOOP(32)
                CALL(evt_sub_intpl_get_value)
                CALL(btlevtcmd_SetPos, -2, EVTDAT_POINTER_MAX, LW(0), EVTDAT_POINTER_MAX)
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_GetPos, -2, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetFaceDirection, -2, LW(7))
            MULTIPLY(LW(7), -30)
            SET_VALUE(LW(1), LW(0))
            ADD(LW(1), LW(7))
            CALL(evt_sub_intpl_init, 11, LW(0), LW(1), 26)
            LOOP(26)
                CALL(evt_sub_intpl_get_value)
                CALL(btlevtcmd_SetPos, -2, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
                WAIT_FRAMES(1)
            END_LOOP
            WAIT_FRAMES(6)
            CALL(btlevtcmd_GetPos, -2, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetFaceDirection, -2, LW(7))
            MULTIPLY(LW(7), 60)
            SET_VALUE(LW(1), LW(0))
            ADD(LW(1), LW(7))
            CALL(evt_sub_intpl_init, 11, LW(0), LW(1), 58)
            LOOP(58)
                CALL(evt_sub_intpl_get_value)
                CALL(btlevtcmd_SetPos, -2, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
                WAIT_FRAMES(1)
            END_LOOP
            WAIT_FRAMES(6)
            CALL(btlevtcmd_GetPos, -2, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetFaceDirection, -2, LW(7))
            MULTIPLY(LW(7), -30)
            SET_VALUE(LW(1), LW(0))
            ADD(LW(1), LW(7))
            CALL(btlevtcmd_GetHomePos, -2, LW(1), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
            CALL(evt_sub_intpl_init, 11, LW(0), LW(1), 32)
            LOOP(32)
                CALL(evt_sub_intpl_get_value)
                CALL(btlevtcmd_SetPos, -2, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        WAIT_FRAMES(130)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_I_S"))
        CALL(btlevtcmd_AnimeChangePaperAnime, -2, 1, STRING("PM_J_1E"))
        BEGIN_SIBLING_EVENT_GET_ID(LW(15))
            CALL(evt_sub_intpl_init_float, 5, FLOAT(0.0f), FLOAT(1.2f), 10)
            LOOP(10)
                CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
            CALL(evt_sub_intpl_init_float, 7, FLOAT(1.2f), FLOAT(1.0f), 30)
            LOOP(30)
                CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        WAIT_FRAMES(20)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_AnimeChangePaperAnime, -2, 1, STRING("PM_J_1C"))
    BEGIN_SIBLING_EVENT
        LOOP(50)
            CALL(btlevtcmd_AddRotate, -2, 0, 48, 0)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_1A"))
        CALL(btlevtcmd_SetPartsBlur, -2, 1, 255, 255, 255, 80, 255, 255, 255, 0, 0)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 67108864)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_BTL_JUMP_CONTINUE1"), 0, 0, -1, -1, 0, 0)
    CALL(evt_sub_random, 20, LW(5))
    SUBTRACT(LW(5), 10)
    MULTIPLY(LW(5), 6)
    ADD(LW(5), 180)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.15f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), LW(5), -2)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    IF_INT_NOT_EQUAL(LW(5), 1)
        IF_INT_EQUAL(LW(5), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(5), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(5), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_StopAC)
        CALL(btlevtcmd_JumpContinue, -2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_B_2"))
        CALL(btlevtcmd_AnimeChangePaperGroup, -2, 1, 0, 0)
        CALL(btlevtcmd_SetPartsBlur, -2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        BEGIN_INLINE_EVENT
            WAIT_FRAMES(10)
            CALL(btlevtcmd_OffPartsAttribute, -2, 1, 67108864)
        END_INLINE_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 35)
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 12, 0)
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
            MULTIPLY(LW(0), -24)
            LOOP(20)
                CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(0))
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 20)
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
            MULTIPLY(LW(0), -20)
            LOOP(12)
                CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(0))
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_Z_11"))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 10)
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 20, 0, 4, 0, -1)
        WAIT_FRAMES(30)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_U_3"))
        WAIT_FRAMES(12)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1D"))
    WAIT_FRAMES(2)
    CALL(btlevtcmd_AnimeChangePaperGroup, -2, 1, 0, 0)
    CALL(btlevtcmd_SetPartsBlur, -2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(10)
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 67108864)
    END_INLINE_EVENT
    CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
    CALL(btlevtcmd_ac_timing_get_success_frame, LW(0))
    DIVIDE(LW(0), 2)
    ADD(LW(0), 2)
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_ac_timing_flag_onoff, 0, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
        CALL(btlevtcmd_GetDamage, LW(3), LW(0))
        CALL(evt_btl_camera_shake_h, 0, LW(0), 0, 10, 13)
        CALL(btlevtcmd_WeaponAftereffect, LW(12))
        CALL(btlevtcmd_StopAC)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 27)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
        GOTO(95)
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131072, LW(5))
    END_IF
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
    CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        SET_VALUE(LW(6), 36)
        CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(6))
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(6), 0, -1)
    END_SIBLING_EVENT
    WAIT_FRAMES(15)
    DELETE_EVENT(LW(14))
    CALL(btlevtcmd_GetFaceDirection, -2, LW(15))
    MULTIPLY(LW(15), -30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_1A"))
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 12, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    WAIT_FRAMES(15)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_POWER1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_JUMP_JABARA5"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LOOP(12)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(15))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
    WAIT_FRAMES(15)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 4, 0, 0, 0, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_1B"))
    WAIT_FRAMES(2)
    CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
    CALL(btlevtcmd_ac_timing_get_success_frame, LW(0))
    DIVIDE(LW(0), 2)
    ADD(LW(0), 1)
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_ac_timing_flag_onoff, 0, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_SET(LW(6), 2)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131328, LW(5))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 1, LW(6))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
        CALL(btlevtcmd_GetDamage, LW(3), LW(0))
        CALL(evt_btl_camera_shake_h, 0, LW(0), 0, 10, 13)
        CALL(btlevtcmd_WeaponAftereffect, LW(12))
        CALL(btlevtcmd_StopAC)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 27)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
        GOTO(95)
    END_IF
    LABEL(90)
    CALL(btlevtcmd_GetDamage, LW(3), LW(0))
    CALL(evt_btl_camera_shake_h, 0, LW(0), 0, 10, 13)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_StopAC)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(1)
        CALL(btlevtcmd_ACRStart, -2, 11, 15, 29, 0)
        CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
        SWITCH(LW(6))
            CASE_GREATER_OR_EQUAL(2)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_4"))
            CALL(btlevtcmd_snd_voice, -2, 1)
            CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
            CASE_DEFAULT
            CALL(evt_audience_acrobat_notry)
        END_SWITCH
    END_SIBLING_EVENT
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_GatsuDokaJump)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL_CHILD(marioAttackEvent_MajinaiPowerUpCheck)
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_SetupAC, -2, 1, 1, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_I_Y"))
    WAIT_FRAMES(10)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(6)
        CASE_OR_EQUAL(4)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
        WAIT_MS(1000)
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    SET_VALUE(LW(6), 36)
    CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(6))
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(0), LW(6))
        SET_VALUE(LW(1), LW(6))
        DIVIDE(LW(0), 2)
        SUBTRACT(LW(1), 1)
        CALL(btlevtcmd_ACRStart, -2, 5, LW(0), LW(1), 0)
        CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
        SWITCH(LW(6))
            CASE_GREATER_OR_EQUAL(2)
            CALL(btlevtcmd_snd_voice, -2, 1)
            CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
            CALL(btlevtcmd_SetRotateOffset, -2, 0, 14, 0)
            LOOP(15)
                CALL(btlevtcmd_AddRotate, -2, 0, 0, -24)
                WAIT_FRAMES(1)
            END_LOOP
            CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
            CASE_DEFAULT
            CALL(evt_audience_acrobat_notry)
        END_SWITCH
    END_SIBLING_EVENT
    CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(6), 0, -1)
    IF_INT_NOT_EQUAL(LW(5), 1)
        IF_INT_EQUAL(LW(5), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(5), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(5), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_StopAC)
        CALL(btlevtcmd_JumpContinue, -2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_LAND_DAMAGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_D_2"))
        WAIT_FRAMES(40)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1D"))
    CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
    CALL(btlevtcmd_ac_timing_get_success_frame, LW(0))
    DIVIDE(LW(0), 2)
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_ac_timing_flag_onoff, 0, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
        CALL(btlevtcmd_GetDamage, LW(3), LW(0))
        CALL(evt_btl_camera_shake_h, 0, LW(0), 0, 10, 13)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_WeaponAftereffect, LW(0))
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_StopAC)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 27)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
    CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
    WAIT_FRAMES(2)
    LABEL(90)
    CALL(btlevtcmd_GetDamage, LW(3), LW(0))
    CALL(evt_btl_camera_shake_h, 0, LW(0), 0, 10, 13)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(0))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StopAC)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_TatsumakiJump)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL_CHILD(marioAttackEvent_MajinaiPowerUpCheck)
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_SetupAC, -2, 1, 1, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), 0, LW(7))
    SUBTRACT(LW(7), 21)
    IF_INT_LESS(LW(7), 0)
        MULTIPLY(LW(7), -1)
        WAIT_FRAMES(LW(7))
    END_IF
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_I_Y"))
    WAIT_FRAMES(10)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(6)
        CASE_OR_EQUAL(4)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
        WAIT_MS(1000)
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    SET_VALUE(LW(6), 36)
    CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(6))
    CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(6), 0, -1)
    IF_INT_NOT_EQUAL(LW(5), 1)
        IF_INT_EQUAL(LW(5), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(5), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(5), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_StopAC)
        CALL(btlevtcmd_JumpContinue, -2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_LAND_DAMAGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_D_2"))
        WAIT_FRAMES(40)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1D"))
    CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
    CALL(btlevtcmd_ac_timing_get_success_frame, LW(0))
    DIVIDE(LW(0), 2)
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_ac_timing_flag_onoff, 0, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
        GOTO(90)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
        CALL(evt_btl_camera_set_zoom, 0, 200)
        WAIT_FRAMES(10)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(evt_btl_camera_set_zoom, 0, 250)
    END_INLINE_EVENT
    CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
    CALL(btlevtcmd_GetFriendBelong, LW(3), LW(15))
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    END_IF
    WAIT_FRAMES(2)
    CALL(btlevtcmd_StopAC)
    BEGIN_INLINE_EVENT_GET_ID(LW(13))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.15f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), 80, 3, -1)
    END_INLINE_EVENT
    BEGIN_SIBLING_EVENT
        LOOP(24)
            CALL(btlevtcmd_AddRotate, -2, 0, 15, 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_FRAMES(27)
    DELETE_EVENT(LW(13))
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        CALL(btlevtcmd_ftof, 600, LW(0))
        CASE_EQUAL(-2)
        CALL(btlevtcmd_ftof, 480, LW(0))
        CASE_EQUAL(-1)
        CALL(btlevtcmd_ftof, 420, LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_ftof, 360, LW(0))
        CASE_EQUAL(1)
        CALL(btlevtcmd_ftof, 300, LW(0))
        CASE_EQUAL(2)
        CALL(btlevtcmd_ftof, 200, LW(0))
        CASE_DEFAULT
        CALL(btlevtcmd_ftof, 100, LW(0))
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, LW(0), 1, 3, -4, 3, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 0)
    CALL(btlevtcmd_SetupAC, -2, 10, 1, 0)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_SetUnitWork, -3, 5, 0)
    CALL(btlevtcmd_SetUnitWork, -3, 6, 0)
    CALL(btlevtcmd_SetUnitWork, -3, 7, 0)
    BEGIN_INLINE_EVENT_GET_ID(LW(13))
        CALL(btlevtcmd_SetUnitWork, -2, 2, 1)
        CALL(btlevtcmd_SetUnitWork, -2, 3, 30)
        CALL(btlevtcmd_SetUnitWork, -2, 4, 0)
        SET_VALUE(LW(0), 15)
        LABEL(60)
        CALL(btlevtcmd_GetUnitWork, -2, 2, LW(1))
        IF_INT_EQUAL(LW(1), 0)
            GOTO(61)
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, 3, LW(10))
        ADD(LW(0), 1)
        IF_INT_GREATER(LW(0), LW(10))
            SET_VALUE(LW(0), LW(10))
        END_IF
        CALL(btlevtcmd_SetUnitWork, -2, 4, LW(0))
        CALL(btlevtcmd_AddRotate, -2, 0, LW(0), 0)
        WAIT_FRAMES(1)
        GOTO(60)
        LABEL(61)
    END_INLINE_EVENT
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        SET_VALUE(LW(0), 0)
        LOOP(1000)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AcGetOutputParam, 0, LW(1))
            IF_INT_NOT_EQUAL(LW(1), LW(0))
                SET_VALUE(LW(0), LW(1))
                SWITCH(LW(0))
                    CASE_EQUAL(0)
                    CASE_EQUAL(1)
                    CALL(btlevtcmd_SetUnitWork, -3, 5, 1023)
                    CALL(btlevtcmd_GetUnitWork, -3, 6, LW(10))
                    SUBTRACT(LW(10), 1023)
                    MULTIPLY(LW(10), -1)
                    DIVIDE(LW(10), 10)
                    CALL(btlevtcmd_SetUnitWork, -3, 7, LW(10))
                    CASE_EQUAL(2)
                    CALL(btlevtcmd_SetUnitWork, -2, 3, 45)
                    CALL(btlevtcmd_SetUnitWork, -3, 5, 2046)
                    CALL(btlevtcmd_GetUnitWork, -3, 6, LW(10))
                    SUBTRACT(LW(10), 2046)
                    MULTIPLY(LW(10), -1)
                    DIVIDE(LW(10), 10)
                    CALL(btlevtcmd_SetUnitWork, -3, 7, LW(10))
                    CASE_EQUAL(3)
                    CALL(btlevtcmd_SetUnitWork, -3, 5, 3069)
                    CALL(btlevtcmd_GetUnitWork, -3, 6, LW(10))
                    SUBTRACT(LW(10), 3069)
                    MULTIPLY(LW(10), -1)
                    DIVIDE(LW(10), 10)
                    CALL(btlevtcmd_SetUnitWork, -3, 7, LW(10))
                    CASE_DEFAULT
                    CALL(btlevtcmd_SetUnitWork, -2, 3, 60)
                END_SWITCH
            END_IF
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_BTL_JUMP_TATUMAKI1"), LW(11))
    BEGIN_SIBLING_EVENT_GET_ID(LW(12))
        SET_VALUE(LW(5), 5)
        LOOP(LW(5))
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(evt_snd_sfx_pos, LW(11), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_GetUnitWork, -3, 5, LW(1))
            CALL(btlevtcmd_GetUnitWork, -3, 6, LW(0))
            IF_INT_NOT_EQUAL(LW(0), LW(1))
                CALL(btlevtcmd_GetUnitWork, -3, 7, LW(2))
                ADD(LW(0), LW(2))
                IF_INT_GREATER_OR_EQUAL(LW(2), 0)
                    IF_INT_GREATER_OR_EQUAL(LW(0), LW(1))
                        SET_VALUE(LW(0), LW(1))
                    END_IF
                ELSE
                    IF_INT_LESS_OR_EQUAL(LW(0), LW(1))
                        SET_VALUE(LW(0), LW(1))
                    END_IF
                END_IF
                CALL(btlevtcmd_SetUnitWork, -3, 6, LW(0))
                CALL(evt_snd_sfx_pit, LW(11), LW(0))
            END_IF
            WAIT_FRAMES(1)
            SET_VALUE(LW(5), 5)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_ResultAC)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        DELETE_EVENT(LW(14))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_SetUnitWork, -3, 5, -2728)
            CALL(btlevtcmd_GetUnitWork, -3, 6, LW(10))
            SUBTRACT(LW(10), -2728)
            MULTIPLY(LW(10), -1)
            DIVIDE(LW(10), 40)
            CALL(btlevtcmd_SetUnitWork, -3, 7, LW(10))
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
            LOOP(40)
                CALL(btlevtcmd_GetUnitWork, -2, 3, LW(0))
                IF_INT_GREATER(LW(0), 10)
                    SUBTRACT(LW(0), 1)
                    CALL(btlevtcmd_SetUnitWork, -2, 3, LW(0))
                END_IF
                CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, LW(0), EVTDAT_POINTER_MAX)
                IF_INT_LESS(LW(1), 90)
                    IF_INT_GREATER_OR_EQUAL(LW(0), 90)
                        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_N_7"))
                        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_MARIO_FALL_CRUSH1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                    END_IF
                END_IF
                SET_VALUE(LW(1), LW(0))
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        WAIT_FRAMES(40)
        DELETE_EVENT(LW(13))
        DELETE_EVENT(LW(12))
        CALL(evt_snd_sfxoff, LW(11))
        BEGIN_SIBLING_EVENT_GET_ID(LW(13))
            LOOP(60)
                CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, LW(0), EVTDAT_POINTER_MAX)
                CALL(btlevtcmd_GetUnitWork, -2, 3, LW(1))
                ADD(LW(0), LW(1))
                IF_INT_GREATER_OR_EQUAL(LW(0), 360)
                    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                    BREAK_LOOP
                END_IF
                CALL(btlevtcmd_SetRotate, -2, 0, LW(0), 0)
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_WaitEventEnd, LW(13))
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        WAIT_FRAMES(60)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpSetting, -2, 40, FLOAT(0.0f), FLOAT(0.1f))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(12)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_LAND_DAMAGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_D_2"))
        END_SIBLING_EVENT
        CALL(btlevtcmd_FallPosition, -2, LW(0), LW(1), LW(2), 16)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
        WAIT_FRAMES(32)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_U_3"))
        WAIT_FRAMES(12)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        GOTO(95)
    ELSE
        DELETE_EVENT(LW(14))
        CALL(btlevtcmd_SetUnitWork, -2, 3, 45)
        LABEL(62)
        CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
        IF_INT_LESS(LW(0), 44)
            WAIT_FRAMES(1)
            GOTO(62)
        END_IF
        BEGIN_SIBLING_EVENT
            SET_VALUE(LW(6), LW(3))
            CALL(_tatsumaki_effect, -2)
            SET_VALUE(LW(0), LW(15))
            CALL(btlevtcmd_SamplingEnemy, -5, LW(0), PTR(&badgeWeapon_TatsumakiJumpInvolved))
            CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
            LABEL(50)
            IF_INT_EQUAL(LW(3), -1)
                GOTO(59)
            END_IF
            IF_INT_EQUAL(LW(3), LW(6))
                GOTO(58)
            END_IF
            CALL(btlevtcmd_PreCheckDamage, -5, LW(3), LW(4), PTR(&badgeWeapon_TatsumakiJumpInvolved), 256, LW(5))
            SWITCH(LW(5))
                CASE_EQUAL(2)
                CALL(btlevtcmd_StartAvoid, LW(3), 40)
                CASE_EQUAL(3)
                CALL(btlevtcmd_StartAvoid, LW(3), 38)
                CASE_EQUAL(6)
                CALL(btlevtcmd_StartAvoid, LW(3), 39)
                CASE_EQUAL(4)
                CASE_EQUAL(1)
                CALL(btlevtcmd_GetResultAC, LW(0))
                IF_BITS_SET(LW(0), 2)
                    CALL(btlevtcmd_CheckDamage, -5, LW(3), LW(4), PTR(&badgeWeapon_TatsumakiJumpInvolved), 131328, LW(5))
                    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
                    CALL(btlevtcmd_GetResultPrizeLv, LW(3), 1, LW(7))
                    CALL(btlevtcmd_ACSuccessEffect, LW(7), LW(0), LW(1), LW(2))
                ELSE
                    CALL(btlevtcmd_CheckDamage, -5, LW(3), LW(4), PTR(&badgeWeapon_TatsumakiJumpInvolved), 256, LW(5))
                END_IF
                CASE_DEFAULT
            END_SWITCH
            LABEL(58)
            CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
            GOTO(50)
            LABEL(59)
            CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
            CALL(btlevtcmd_GetResultPrizeLv, LW(3), 1, LW(6))
            CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
        END_SIBLING_EVENT
        WAIT_FRAMES(60)
        BEGIN_INLINE_EVENT_GET_ID(LW(13))
            LOOP(30)
                CALL(btlevtcmd_GetUnitWork, -2, 3, LW(0))
                SUBTRACT(LW(0), 1)
                CALL(btlevtcmd_SetUnitWork, -2, 3, LW(0))
                WAIT_FRAMES(1)
            END_IF
            CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
            CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
            LOOP(6)
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AddRotate, -2, 0, 45, 0)
            END_LOOP
            LOOP(6)
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AddRotate, -2, 0, 15, 0)
            END_LOOP
            CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        END_INLINE_EVENT
        CALL(btlevtcmd_WaitEventEnd, LW(13))
        DELETE_EVENT(LW(12))
        CALL(evt_snd_sfxoff, LW(11))
        WAIT_FRAMES(30)
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_ACRStart, -2, 7, 21, 21, 0)
            CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
            IF_INT_GREATER_OR_EQUAL(LW(6), 2)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_B_1"))
            END_IF
        END_SIBLING_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.15f))
        CALL(btlevtcmd_FallPosition, -2, LW(0), LW(1), LW(2), 16)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1D"))
        WAIT_FRAMES(6)
        CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
        IF_INT_GREATER_OR_EQUAL(LW(6), 2)
            GOTO(80)
        END_IF
    END_IF
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    GOTO(95)
    LABEL(80)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), 0, LW(7))
    SET_VALUE(LW(8), LW(7))
    ADD(LW(8), 12)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(1)
        CALL(btlevtcmd_ACRStart, -2, LW(7), LW(8), LW(8), 0)
    END_SIBLING_EVENT
    ADD(LW(8), 1)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(15))
    MULTIPLY(LW(15), 30)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 14, 0)
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(10), 0)
        LOOP(LW(8))
            CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(15))
            IF_INT_EQUAL(LW(10), 0)
                CALL(btlevtcmd_snd_voice, -2, 1)
            END_IF
            ADD(LW(10), 1)
            IF_INT_GREATER_OR_EQUAL(LW(10), 12)
                SET_VALUE(LW(10), 0)
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), LW(8), -1, 0)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
    MULTIPLY(LW(5), 36)
    BEGIN_INLINE_EVENT_GET_ID(LW(14))
        CALL(btlevtcmd_GetRotate, -2, LW(0), LW(1), LW(2))
        MODULO(LW(2), 360)
        DIVIDE(LW(2), 10)
        ADD(LW(2), LW(5))
        LOOP(10)
        END_LOOP
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    SWITCH(LW(6))
        CASE_GREATER_OR_EQUAL(2)
        CALL(btlevtcmd_WaitEventEnd, LW(14))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_3B"))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 50)
        CALL(evt_eff, STRING("_BTLEF"), STRING("confetti"), 3, LW(0), LW(1), LW(2), 90, 0, 0, 0, 0, 0, 0, 0)
        CALL(btlevtcmd_snd_voice, -2, 1)
        CALL(evt_snd_sfxon, STRING("SFX_BTL_ACROBAT_FINISH1"), 0)
        WAIT_FRAMES(90)
        CASE_LESS_OR_EQUAL(-1)
        DELETE_EVENT(LW(14))
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 0, 0, 0, 0)
        CALL(evt_snd_sfxon, STRING("SFX_BTL_ACROBAT_MISS1"), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_I_S"))
        WAIT_MS(1500)
        CASE_DEFAULT
        CALL(btlevtcmd_WaitEventEnd, LW(14))
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(evt_audience_acrobat_notry)
    END_SWITCH
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    GOTO(99)
    LABEL(90)
    CALL(btlevtcmd_StopAC)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(0))
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 27)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_RenzokuJump)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL_CHILD(marioAttackEvent_MajinaiPowerUpCheck)
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_SetupAC, -2, 1, 1, 0)
    CALL(btlevtcmd_ac_timing_flag_onoff, 1, 2)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_StartAC, 1)
    SET_VALUE(LW(10), 0)
    LABEL(10)
    IF_INT_GREATER_OR_EQUAL(LW(10), 1)
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 1048832, LW(5))
    ELSE
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    END_IF
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(6)
        CASE_OR_EQUAL(4)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
        WAIT_MS(1000)
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    IF_INT_GREATER_OR_EQUAL(LW(10), 1)
        SET_VALUE(LW(8), 36)
        CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(8))
        SWITCH(LW(10))
            CASE_EQUAL(1)
            SET_VALUE(LW(6), STRING("SFX_BTL_JUMP_CONTINUE2"))
            CASE_EQUAL(2)
            SET_VALUE(LW(6), STRING("SFX_BTL_JUMP_CONTINUE3"))
            CASE_EQUAL(3)
            SET_VALUE(LW(6), STRING("SFX_BTL_JUMP_CONTINUE4"))
            CASE_EQUAL(4)
            SET_VALUE(LW(6), STRING("SFX_BTL_JUMP_CONTINUE5"))
            CASE_EQUAL(5)
            SET_VALUE(LW(6), STRING("SFX_BTL_JUMP_CONTINUE6"))
            CASE_EQUAL(6)
            SET_VALUE(LW(6), STRING("SFX_BTL_JUMP_CONTINUE7"))
            CASE_EQUAL(7)
            SET_VALUE(LW(6), STRING("SFX_BTL_JUMP_CONTINUE8"))
            CASE_EQUAL(8)
            SET_VALUE(LW(6), STRING("SFX_BTL_JUMP_CONTINUE9"))
            CASE_DEFAULT
            SET_VALUE(LW(6), STRING("SFX_BTL_JUMP_CONTINUE10"))
        END_SWITCH
        CALL(btlevtcmd_SetMoveSoundDataWork, -2, LW(6), 0, 0, -1, -1, 0, 0)
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(3)
            CALL(btlevtcmd_ACRStart, -2, 11, 15, 30, 0)
            CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
            SWITCH(LW(6))
                CASE_GREATER_OR_EQUAL(2)
                CALL(evt_sub_random, 2, LW(0))
                SWITCH(LW(0))
                    CASE_EQUAL(0)
                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_4"))
                    CASE_EQUAL(1)
                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_5"))
                    CASE_DEFAULT
                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_6"))
                END_SWITCH
                CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
                CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, -60, -30, 0)
                CALL(btlevtcmd_snd_voice, -2, 1)
                CASE_DEFAULT
                CALL(evt_audience_acrobat_notry)
            END_SWITCH
        END_SIBLING_EVENT
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(8), 3, -2)
    ELSE
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_BTL_JUMP_CONTINUE1"), 0, 0, -1, -1, 0, 0)
        SET_VALUE(LW(8), 36)
        CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(8))
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(8), 0, -2)
    END_IF
    IF_INT_NOT_EQUAL(LW(5), 1)
        IF_INT_EQUAL(LW(5), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(5), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(5), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_StopAC)
        CALL(btlevtcmd_JumpContinue, -2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_LAND_DAMAGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_D_2"))
        WAIT_FRAMES(40)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1D"))
    CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
    CALL(btlevtcmd_ac_timing_get_success_frame, LW(0))
    DIVIDE(LW(0), 2)
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_ac_timing_flag_onoff, 0, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        IF_INT_EQUAL(LW(10), 0)
            CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
        ELSE
            SET_VALUE(LW(6), LW(10))
            SUBTRACT(LW(6), 1)
            CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(6), LW(6))
            CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
        END_IF
        CALL(btlevtcmd_StopAC)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_WeaponAftereffect, LW(0))
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 27)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
        GOTO(95)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
        CALL(evt_btl_camera_set_zoom, 0, 200)
        WAIT_FRAMES(10)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(evt_btl_camera_set_zoom, 0, 250)
    END_INLINE_EVENT
    CALL(btlevtcmd_snd_voice, -2, 2)
    CALL(btlevtcmd_GetResultCountAC, LW(6))
    CALL(_record_renzoku_count, LW(6))
    CALL(btlevtcmd_GetResultCountAC, LW(6))
    CALL(mario_get_renzoku_count_max, LW(3), LW(6), LW(7))
    IF_INT_LESS(LW(6), LW(7))
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131072, LW(5))
    ELSE
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
    END_IF
    ADD(LW(10), 1)
    SET_VALUE(LW(1), GSW(32))
    SET_VALUE(LW(2), GSW(35))
    MULTIPLY(LW(2), 256)
    ADD(LW(1), LW(2))
    IF_INT_GREATER(LW(10), LW(1))
        SET_VALUE(LW(0), LW(10))
        IF_INT_GREATER(LW(0), 9999)
            SET_VALUE(LW(0), 9999)
        END_IF
        SET_VALUE(LW(1), LW(0))
        MODULO(LW(1), 256)
        SET_VALUE(LW(2), LW(0))
        DIVIDE(LW(2), 256)
        SET_VALUE(GSW(32), LW(1))
        SET_VALUE(GSW(35), LW(2))
    END_IF
    SET_VALUE(LW(0), LW(10))
    SUBTRACT(LW(0), 1)
    CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(0), LW(6))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
    CALL(btlevtcmd_GetResultCountAC, LW(6))
    IF_INT_LESS(LW(6), LW(7))
        WAIT_FRAMES(2)
        CALL(btlevtcmd_StartAC, 0)
        GOTO(10)
    END_IF
    LABEL(90)
    CALL(btlevtcmd_StopAC)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(0))
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_TugiTugiJump)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL_CHILD(marioAttackEvent_MajinaiPowerUpCheck)
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_SetupAC, -2, 1, 1, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_StartAC, 1)
    SET_VALUE(LW(10), 0)
    LABEL(10)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(6)
        CASE_OR_EQUAL(4)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
        WAIT_MS(1000)
    END_SWITCH
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_BTL_JUMP_TUGITUGI1"), 0, 0, -1, -1, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    SWITCH(LW(10))
        CASE_EQUAL(0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        SET_VALUE(LW(8), 36)
        CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(8))
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(8), 0, -2)
        CASE_EQUAL(1)
        SET_VALUE(LW(8), 36)
        CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(8))
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(3)
            CALL(btlevtcmd_ACRStart, -2, 13, 17, 30, 0)
            CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
            SWITCH(LW(6))
                CASE_GREATER_OR_EQUAL(2)
                CALL(evt_sub_random, 2, LW(0))
                SWITCH(LW(0))
                    CASE_EQUAL(0)
                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_4"))
                    CASE_EQUAL(1)
                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_5"))
                    CASE_DEFAULT
                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_6"))
                END_SWITCH
                CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
                CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, -60, -30, 0)
                CASE_DEFAULT
                CALL(evt_audience_acrobat_notry)
            END_SWITCH
        END_SIBLING_EVENT
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(8), 3, -2)
        CASE_DEFAULT
        SET_VALUE(LW(8), 36)
        CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(8))
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(3)
            CALL(btlevtcmd_ACRStart, -2, 13, 17, 30, 0)
            CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
            SWITCH(LW(6))
                CASE_GREATER_OR_EQUAL(2)
                CALL(evt_sub_random, 2, LW(0))
                SWITCH(LW(0))
                    CASE_EQUAL(0)
                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_4"))
                    CASE_EQUAL(1)
                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_5"))
                    CASE_DEFAULT
                    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_6"))
                END_SWITCH
                CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
                CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, -60, -30, 0)
                CASE_LESS_OR_EQUAL(-1)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_N_7"))
                CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
                CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 0, 0, 0, 0)
                CASE_DEFAULT
                CALL(evt_audience_acrobat_notry)
            END_SWITCH
        END_SIBLING_EVENT
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(8), 4, -2)
    END_SWITCH
    IF_INT_NOT_EQUAL(LW(5), 1)
        IF_INT_EQUAL(LW(5), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(5), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(5), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_StopAC)
        CALL(btlevtcmd_JumpContinue, -2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_LAND_DAMAGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_D_2"))
        WAIT_FRAMES(40)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1D"))
    CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
    CALL(btlevtcmd_ac_timing_get_success_frame, LW(0))
    DIVIDE(LW(0), 2)
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_ac_timing_flag_onoff, 0, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        IF_INT_EQUAL(LW(10), 0)
            CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
        ELSE
            SET_VALUE(LW(6), LW(10))
            SUBTRACT(LW(6), 1)
            CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(6), LW(6))
            CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
        END_IF
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(btlevtcmd_StopAC)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_WeaponAftereffect, LW(0))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 27)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        CALL(btlevtcmd_snd_voice, -2, 2)
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(10), LW(6))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    END_IF
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        BEGIN_INLINE_EVENT
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
            CALL(evt_btl_camera_set_zoomSpeedLv, 0, 0)
            CALL(evt_btl_camera_set_zoom, 0, 200)
            WAIT_FRAMES(10)
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
            CALL(evt_btl_camera_set_zoom, 0, 250)
        END_INLINE_EVENT
        WAIT_FRAMES(2)
        ADD(LW(10), 1)
        BEGIN_INLINE_EVENT
            WAIT_FRAMES(10)
            CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
            CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        END_INLINE_EVENT
        GOTO(10)
    END_IF
    CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(10), LW(6))
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
    LABEL(90)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_StopAC)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(0))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_J_1B"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_NormalHammer_Core)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(12), PTR(&marioDefaultWeapon_Hammer))
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            GOTO(99)
        END_IF
        CALL(evt_btl_camera_set_mode, 0, 7)
        CALL(evt_btl_camera_set_homing_unit, 0, LW(3), -1)
        CALL(evt_btl_camera_set_zoom, 0, 300)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
        CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
        WAIT_FRAMES(30)
    ELSE
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
        CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
        CALL(btlevtcmd_WaitGuardMove)
        CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
        CALL(btlevtcmd_RunDataEventChild, -2, 7)
        CALL_CHILD(marioAttackEvent_MajinaiPowerUpCheck)
        CALL(evt_btl_camera_set_mode, 0, 11)
        CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(evt_btl_camera_set_zoom, 0, 300)
    END_IF
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetWeaponActionLv, LW(12), LW(0))
        CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
        SWITCH(LW(12))
            CASE_EQUAL(PTR(&marioDefaultWeapon_Hammer))
            CALL(btlevtcmd_AcSetParamAll, 4, 25, 0, 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
            CASE_EQUAL(PTR(&badgeWeapon_ConfuseHammer))
            CALL(btlevtcmd_AcSetParamAll, 4, 30, 0, 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
            CASE_EQUAL(PTR(&badgeWeapon_TsuranukiNaguri))
            CALL(btlevtcmd_AcSetParamAll, 4, 30, 0, 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
            CASE_DEFAULT
            CALL(btlevtcmd_AcSetParamAll, 4, 40, 0, 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        END_SWITCH
        CALL(btlevtcmd_AcSetFlag, 0)
        CALL(btlevtcmd_SetupAC, -2, 3, 1, 0)
    END_IF
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
        CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
        WAIT_MS(1000)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 35)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
        ADD(LW(2), 5)
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
        CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), 0, LW(7))
        IF_INT_GREATER(LW(7), 22)
            SUBTRACT(LW(7), 22)
        ELSE
            SUBTRACT(LW(7), 22)
            MULTIPLY(LW(7), -1)
        END_IF
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(LW(7))
            CALL(btlevtcmd_GetWeaponActionLv, LW(12), LW(0))
            CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
            SWITCH(LW(12))
                CASE_EQUAL(PTR(&marioDefaultWeapon_Hammer))
                CALL(btlevtcmd_AcSetParamAll, 4, 25, 0, 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
                CASE_EQUAL(PTR(&badgeWeapon_ConfuseHammer))
                CALL(btlevtcmd_AcSetParamAll, 4, 30, 0, 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
                CASE_EQUAL(PTR(&badgeWeapon_TsuranukiNaguri))
                CALL(btlevtcmd_AcSetParamAll, 4, 30, 0, 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
                CASE_DEFAULT
                CALL(btlevtcmd_AcSetParamAll, 4, 40, 0, 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
            END_SWITCH
            CALL(btlevtcmd_AcSetFlag, 0)
            CALL(btlevtcmd_SetupAC, -2, 3, 1, 0)
        END_SIBLING_EVENT
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    ELSE
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 35)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        ADD(LW(2), 5)
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    END_IF
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(6))
    IF_INT_EQUAL(LW(6), 5)
        CALL(btlevtcmd_StopAC)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
        RETURN
    END_IF
    IF_INT_EQUAL(LW(12), PTR(&badgeWeapon_TsuranukiNaguri))
        CALL(_get_mario_hammer_lv, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(2)
            SET_VALUE(LW(0), STRING("M_H_5A"))
            SET_VALUE(LW(1), STRING("M_H_5B"))
            SET_VALUE(LW(2), STRING("M_H_14"))
            CASE_EQUAL(3)
            SET_VALUE(LW(0), STRING("M_H_9A"))
            SET_VALUE(LW(1), STRING("M_H_9B"))
            SET_VALUE(LW(2), STRING("M_H_15"))
            CASE_DEFAULT
            SET_VALUE(LW(0), STRING("M_H_1A"))
            SET_VALUE(LW(1), STRING("M_H_1B"))
            SET_VALUE(LW(2), STRING("M_H_13"))
        END_SWITCH
    ELSE
        CALL(_get_mario_hammer_lv, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(2)
            SET_VALUE(LW(0), STRING("M_H_5A"))
            SET_VALUE(LW(1), STRING("M_H_5B"))
            SET_VALUE(LW(2), STRING("M_H_5C"))
            CASE_EQUAL(3)
            SET_VALUE(LW(0), STRING("M_H_9A"))
            SET_VALUE(LW(1), STRING("M_H_9B"))
            SET_VALUE(LW(2), STRING("M_H_9C"))
            CASE_DEFAULT
            SET_VALUE(LW(0), STRING("M_H_1A"))
            SET_VALUE(LW(1), STRING("M_H_1B"))
            SET_VALUE(LW(2), STRING("M_H_1C"))
        END_SWITCH
    END_IF
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_POWER2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(0))
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_EQUAL(1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_DEFAULT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SWITCH
    BEGIN_SIBLING_EVENT
        SWITCH(LW(12))
            CASE_EQUAL(PTR(&badgeWeapon_ConfuseHammer))
            SET_VALUE(LW(13), 0)
            CASE_EQUAL(PTR(&badgeWeapon_TsuranukiNaguri))
            SET_VALUE(LW(13), 2)
        END_SWITCH
        SWITCH(LW(12))
            CASE_OR_EQUAL(PTR(&badgeWeapon_ConfuseHammer))
            CASE_OR_EQUAL(PTR(&badgeWeapon_TsuranukiNaguri))
            CALL(btlevtcmd_ftomsec, 9, LW(0))
            WAIT_MS(LW(0))
            CALL(_hammer_star_effect, -2, LW(13))
            CASE_END
        END_SWITCH
        SWITCH(LW(12))
            CASE_OR_EQUAL(PTR(&badgeWeapon_ConfuseHammer))
            CASE_OR_EQUAL(PTR(&badgeWeapon_TsuranukiNaguri))
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_MARIO_HAMMER_SHINE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CASE_END
        END_SWITCH
    END_SIBLING_EVENT
    CALL(btlevtcmd_ftomsec, 30, LW(0))
    WAIT_MS(LW(0))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(1))
    CALL(btlevtcmd_StartAC, 0)
    CALL(btlevtcmd_ResultAC)
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING2"), 0)
        CASE_EQUAL(1)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING1"), 0)
        CASE_DEFAULT
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING3"), 0)
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
    WAIT_FRAMES(7)
    IF_INT_EQUAL(LW(12), PTR(&badgeWeapon_TsuranukiNaguri))
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_TURANUKI1"), 0)
    ELSE
        CALL(_get_mario_hammer_lv, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(2)
            CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_HIT_GROUND3"), 0)
            CASE_EQUAL(1)
            CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_HIT_GROUND2"), 0)
            CASE_DEFAULT
            CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_HIT_GROUND1"), 0)
        END_SWITCH
    END_IF
    IF_INT_NOT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        GOTO(90)
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131328, LW(5))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
    LABEL(90)
    CALL(btlevtcmd_StopAC)
    CALL(btlevtcmd_GetDamage, LW(3), LW(0))
    CALL(evt_btl_camera_shake_h, 0, LW(0), 0, 10, 13)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_ACRStart, -2, 0, 15, 60, 0)
    CALL(btlevtcmd_ACRGetResult, LW(15), LW(13))
    IF_INT_EQUAL(LW(15), 1)
        GOTO(91)
    END_IF
    IF_INT_LESS_OR_EQUAL(LW(15), -1)
        GOTO(91)
    END_IF
    GOTO(92)
    LABEL(91)
    CALL(evt_audience_acrobat_notry)
    SET_VALUE(LW(0), 60)
    SUBTRACT(LW(0), LW(13))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        WAIT_FRAMES(LW(0))
    END_IF
    IF_INT_EQUAL(LW(12), PTR(&badgeWeapon_TsuranukiNaguri))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_H_16"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_S_1"))
    END_IF
    WAIT_FRAMES(10)
    GOTO(95)
    LABEL(92)
    IF_INT_NOT_EQUAL(LW(15), 0)
        CALL(evt_btl_camera_set_mode, 0, 0)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_snd_voice, -2, 1)
            CALL(btlevtcmd_SetRotateOffset, -2, 0, 20, 0)
            LOOP(30)
                CALL(btlevtcmd_AddRotate, -2, 0, 0, 12)
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(8)
            CALL(btlevtcmd_ACRStart, -2, 12, 25, 25, 0)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(4)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_Z_1"))
        END_SIBLING_EVENT
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_3A"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 35)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
        WAIT_FRAMES(4)
        CALL(btlevtcmd_ACRGetResult, LW(0), LW(1))
        IF_INT_EQUAL(LW(14), 0)
            IF_INT_EQUAL(LW(0), 2)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_3B"))
                CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                ADD(LW(1), 50)
                CALL(evt_eff, STRING("_BTLEF"), STRING("confetti"), 3, LW(0), LW(1), LW(2), 90, 0, 0, 0, 0, 0, 0, 0)
                CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
                WAIT_FRAMES(90)
                GOTO(95)
            ELSE
                CALL(evt_audience_acrobat_notry)
                GOTO(95)
            END_IF
        ELSE
            SWITCH(LW(0))
                CASE_EQUAL(2)
                CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
                CASE_EQUAL(1)
                CALL(evt_audience_acrobat_notry)
                GOTO(95)
            END_SWITCH
        END_IF
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_snd_voice, -2, 1)
            CALL(btlevtcmd_SetRotateOffset, -2, 0, 20, 0)
            LOOP(40)
                CALL(btlevtcmd_AddRotate, -2, 0, 0, 18)
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(18)
            CALL(btlevtcmd_ACRStart, -2, 12, 25, 25, 0)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(18)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_Z_1"))
        END_SIBLING_EVENT
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_3A"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 40, -1)
        WAIT_FRAMES(4)
        CALL(btlevtcmd_ACRGetResult, LW(0), LW(1))
        IF_INT_NOT_EQUAL(LW(0), 2)
            GOTO(95)
        END_IF
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        CALL(btlevtcmd_snd_voice, -2, 1)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 20, 0)
        BEGIN_SIBLING_EVENT
            LOOP(60)
                CALL(btlevtcmd_AddRotate, -2, 0, 12, 12)
                WAIT_FRAMES(1)
            END_LOOP
            CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(35)
            CALL(btlevtcmd_ACRStart, -2, 15, 28, 28, 0)
        END_SIBLING_EVENT
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_C_3"))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.2f))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 50)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 60, -1)
        WAIT_FRAMES(4)
        CALL(btlevtcmd_ACRGetResult, LW(0), LW(1))
        IF_INT_NOT_EQUAL(LW(0), 2)
            GOTO(95)
        END_IF
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_3B"))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 50)
        CALL(evt_eff, STRING("_BTLEF"), STRING("confetti"), 3, LW(0), LW(1), LW(2), 90, 0, 0, 0, 0, 0, 0, 0)
        CALL(btlevtcmd_snd_voice, -2, 1)
        CALL(evt_snd_sfxon, STRING("SFX_BTL_ACROBAT_FINISH1"), 0)
        WAIT_FRAMES(90)
    END_IF
    LABEL(95)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(marioAttackEvent_KaitenHammer)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            GOTO(99)
        END_IF
        CALL(evt_btl_camera_set_mode, 0, 7)
        CALL(evt_btl_camera_set_homing_unit, 0, LW(3), -1)
        CALL(evt_btl_camera_set_zoom, 0, 300)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
        CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
        WAIT_FRAMES(30)
    ELSE
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
        CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            GOTO(99)
        END_IF
        CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
        CALL(btlevtcmd_WaitGuardMove)
        CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
        CALL(btlevtcmd_RunDataEventChild, -2, 7)
        CALL_CHILD(marioAttackEvent_MajinaiPowerUpCheck)
        CALL(evt_btl_camera_set_mode, 0, 11)
        CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(evt_btl_camera_set_zoom, 0, 300)
    END_IF
    CALL(btlevtcmd_GetWeaponActionLv, LW(12), LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    SWITCH(LW(12))
        CASE_OR_EQUAL(PTR(&marioWeapon_UltraHammer))
        CASE_OR_EQUAL(PTR(&marioDefaultWeapon_FirstAttackUltraHammer))
        CALL(btlevtcmd_AcSetParamAll, 10, 8, 0, 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        CASE_END
        CASE_DEFAULT
        CALL(btlevtcmd_AcSetParamAll, 7, 15, 0, 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    END_SWITCH
    CALL(btlevtcmd_AcSetFlag, 1)
    CALL(btlevtcmd_SetupAC, -2, 3, 1, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 30)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    ADD(LW(2), 5)
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), 0, LW(7))
    SUBTRACT(LW(7), 21)
    IF_INT_LESS(LW(7), 0)
        MULTIPLY(LW(7), -1)
        WAIT_FRAMES(LW(7))
    END_IF
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_zoom, 0, 350)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(7))
    IF_INT_EQUAL(LW(7), 5)
        CALL(btlevtcmd_StopAC)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
        RETURN
    END_IF
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        SET_VALUE(LW(0), STRING("M_H_6A"))
        SET_VALUE(LW(1), STRING("M_H_6B"))
        CASE_EQUAL(3)
        SET_VALUE(LW(0), STRING("M_H_10A"))
        SET_VALUE(LW(1), STRING("M_H_10B"))
        CASE_DEFAULT
        SET_VALUE(LW(0), STRING("M_H_2A"))
        SET_VALUE(LW(1), STRING("M_H_2B"))
    END_SWITCH
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_POWER2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePaperGroup, -2, 1, STRING("p_kaiten_h"), 0)
    CALL(btlevtcmd_AnimeChangePaperAnime, -2, 1, STRING("P_H_1A"))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(0))
    CALL(_paper_light_off, -2, 1)
    SWITCH(LW(12))
        CASE_OR_EQUAL(PTR(&marioWeapon_KaitenHammer))
        CASE_OR_EQUAL(PTR(&marioDefaultWeapon_FirstAttackKaitenHammer))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_KAITEN1"), EVTDAT_POINTER_MAX, 0, LW(15))
        CASE_END
        CASE_OR_EQUAL(PTR(&marioWeapon_UltraHammer))
        CASE_OR_EQUAL(PTR(&marioDefaultWeapon_FirstAttackUltraHammer))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_ULTRA1"), EVTDAT_POINTER_MAX, 0, LW(15))
        CASE_END
        CASE_DEFAULT
    END_SWITCH
    WAIT_FRAMES(28)
    CALL(btlevtcmd_AnimeChangePaperAnime, -2, 1, STRING("P_H_1B"))
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    SWITCH(LW(12))
        CASE_OR_EQUAL(PTR(&marioWeapon_KaitenHammer))
        CASE_OR_EQUAL(PTR(&marioDefaultWeapon_FirstAttackKaitenHammer))
        CALL(evt_snd_sfxoff, LW(15))
        CASE_OR_EQUAL(PTR(&marioWeapon_UltraHammer))
        CASE_OR_EQUAL(PTR(&marioDefaultWeapon_FirstAttackUltraHammer))
        CALL(evt_snd_sfxoff, LW(15))
        CASE_END
        CASE_END
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(1))
    CALL(btlevtcmd_AnimeChangePaperAnime, -2, 1, STRING("P_H_1C"))
    WAIT_FRAMES(10)
    CALL(btlevtcmd_AnimeChangePaperGroup, -2, 1, 0, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    IF_INT_NOT_EQUAL(LW(7), 1)
        IF_INT_EQUAL(LW(7), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(7), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(7), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        GOTO(90)
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
        GOTO(90)
    END_IF
    SWITCH(LW(12))
        CASE_OR_EQUAL(PTR(&marioWeapon_UltraHammer))
        CASE_OR_EQUAL(PTR(&marioDefaultWeapon_FirstAttackUltraHammer))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131328, LW(5))
        CASE_END
        CASE_DEFAULT
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131328, LW(5))
    END_SWITCH
    CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
    LABEL(90)
    CALL(btlevtcmd_StopAC)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    SWITCH(LW(12))
        CASE_OR_EQUAL(PTR(&marioWeapon_UltraHammer))
        CASE_OR_EQUAL(PTR(&marioDefaultWeapon_FirstAttackUltraHammer))
        SET_VALUE(LW(0), 1)
        CASE_END
        CASE_DEFAULT
        SET_VALUE(LW(0), 0)
    END_SWITCH
    IF_INT_EQUAL(LW(0), 1)
        BEGIN_INLINE_EVENT
            CALL(_whirlwind_effect, -2, 60)
        END_INLINE_EVENT
        CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
        MULTIPLY(LW(0), 36)
        BEGIN_SIBLING_EVENT_GET_ID(LW(9))
            LOOP(6)
                SWITCH(LW(12))
                    CASE_OR_EQUAL(PTR(&marioWeapon_UltraHammer))
                    CASE_OR_EQUAL(PTR(&marioDefaultWeapon_FirstAttackUltraHammer))
                    CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_ULTRA2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                    CASE_END
                END_SWITCH
                LOOP(10)
                    CALL(btlevtcmd_AddRotate, -2, 0, LW(0), 0)
                    WAIT_FRAMES(1)
                END_LOOP
            END_LOOP
            CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        END_SIBLING_EVENT
        IF_INT_NOT_EQUAL(LW(7), 1)
            WAIT_FRAMES(50)
            DELETE_EVENT(LW(9))
            BEGIN_SIBLING_EVENT
                DIVIDE(LW(0), 3)
                LOOP(30)
                    CALL(btlevtcmd_AddRotate, -2, 0, LW(0), 0)
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
            END_SIBLING_EVENT
            WAIT_FRAMES(8)
            CALL(btlevtcmd_AnimeChangePaperGroup, -2, 1, 0, 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_I_S"))
            WAIT_FRAMES(24)
            WAIT_MS(1500)
        ELSE
            CALL(btlevtcmd_SetPartsBlur, -2, 1, 255, 255, 255, 80, 255, 255, 255, 0, 0)
            CALL(btlevtcmd_OnPartsAttribute, -2, 1, 67108864)
            BEGIN_INLINE_EVENT
                WAIT_FRAMES(50)
                CALL(btlevtcmd_SetPartsBlur, -2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0)
                WAIT_FRAMES(10)
                CALL(btlevtcmd_OffPartsAttribute, -2, 1, 67108864)
            END_INLINE_EVENT
            CALL(btlevtcmd_ACRStart, -2, 40, 63, 63, 0)
            CALL(btlevtcmd_ACRGetResult, LW(6), EVTDAT_POINTER_MAX)
            SWITCH(LW(6))
                CASE_GREATER_OR_EQUAL(2)
                DELETE_EVENT(LW(9))
                CALL(btlevtcmd_AnimeChangePaperGroup, -2, 1, 0, 0)
                CALL(_kaiten_hammer_acrobat_rotate, -2, 1, 30, STRING("M_V_2"))
                CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
                WAIT_FRAMES(30)
                CASE_DEFAULT
                CALL(btlevtcmd_WaitEventEnd, LW(9))
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                CALL(btlevtcmd_AnimeChangePaperGroup, -2, 1, 0, 0)
                CALL(evt_audience_acrobat_notry)
            END_SWITCH
        END_IF
    ELSE
        IF_INT_NOT_EQUAL(LW(7), 1)
            CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
            MULTIPLY(LW(0), 36)
            BEGIN_SIBLING_EVENT_GET_ID(LW(9))
                LOOP(6)
                    SWITCH(LW(12))
                        CASE_OR_EQUAL(PTR(&marioWeapon_KaitenHammer))
                        CASE_OR_EQUAL(PTR(&marioDefaultWeapon_FirstAttackKaitenHammer))
                        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_MARIO_HAMMER_KAITEN2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                        CASE_END
                    END_SWITCH
                    LOOP(10)
                        CALL(btlevtcmd_AddRotate, -2, 0, LW(0), 0)
                        WAIT_FRAMES(1)
                    END_LOOP
                END_LOOP
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
            END_SIBLING_EVENT
            WAIT_FRAMES(50)
            DELETE_EVENT(LW(9))
            BEGIN_SIBLING_EVENT
                DIVIDE(LW(0), 3)
                LOOP(30)
                    CALL(btlevtcmd_AddRotate, -2, 0, LW(0), 0)
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
            END_SIBLING_EVENT
            WAIT_FRAMES(8)
            CALL(btlevtcmd_AnimeChangePaperGroup, -2, 1, 0, 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_I_S"))
            WAIT_FRAMES(24)
            WAIT_MS(1500)
        ELSE
            BEGIN_INLINE_EVENT
                CALL(_whirlwind_effect, -2, 60)
            END_INLINE_EVENT
            BEGIN_SIBLING_EVENT_GET_ID(LW(9))
                CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
                MULTIPLY(LW(0), 36)
                LOOP(6)
                    SWITCH(LW(12))
                        CASE_OR_EQUAL(PTR(&marioWeapon_KaitenHammer))
                        CASE_OR_EQUAL(PTR(&marioDefaultWeapon_FirstAttackKaitenHammer))
                        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_MARIO_HAMMER_KAITEN2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
                        CASE_END
                    END_SWITCH
                    LOOP(10)
                        CALL(btlevtcmd_AddRotate, -2, 0, LW(0), 0)
                        WAIT_FRAMES(1)
                    END_LOOP
                END_LOOP
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
            END_SIBLING_EVENT
            CALL(btlevtcmd_ACRStart, -2, 40, 63, 63, 0)
            CALL(btlevtcmd_ACRGetResult, LW(6), EVTDAT_POINTER_MAX)
            SWITCH(LW(6))
                CASE_GREATER_OR_EQUAL(2)
                DELETE_EVENT(LW(9))
                CALL(btlevtcmd_AnimeChangePaperGroup, -2, 1, 0, 0)
                CALL(_kaiten_hammer_acrobat_rotate, -2, 1, 30, STRING("M_V_2"))
                CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
                WAIT_FRAMES(30)
                CASE_DEFAULT
                CALL(btlevtcmd_WaitEventEnd, LW(9))
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                CALL(btlevtcmd_AnimeChangePaperGroup, -2, 1, 0, 0)
                CALL(evt_audience_acrobat_notry)
            END_SWITCH
        END_IF
    END_IF
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    SWITCH(LW(12))
        CASE_OR_EQUAL(PTR(&marioWeapon_UltraHammer))
        CASE_OR_EQUAL(PTR(&marioDefaultWeapon_FirstAttackUltraHammer))
        CASE_END
        CASE_DEFAULT
        GOTO(99)
    END_SWITCH
    CALL(btlevtcmd_GetResultAC, LW(6))
    IF_BITS_CLEAR(LW(6), 2)
        GOTO(99)
    END_IF
    IF_INT_NOT_EQUAL(LW(7), 1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckToken, LW(3), 32, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    LABEL(96)
    CALL(btlevtcmd_CheckToken, LW(3), 64, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        WAIT_FRAMES(1)
        GOTO(96)
    END_IF
    CALL(btlevtcmd_OffToken, LW(3), 64)
    LABEL(98)
    CALL(btlevtcmd_CheckToken, LW(3), 128, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckToken, -2, 256, LW(0))
        IF_INT_EQUAL(LW(0), 0)
        END_IF
        WAIT_FRAMES(1)
        GOTO(98)
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&marioWeapon_UltraHammerFinish), 131328, LW(5))
    LABEL(99)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_FirstAttackKaitenHammer)
    CALL(_get_mario_hammer_lv, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 3)
        SET_VALUE(LW(12), PTR(&marioDefaultWeapon_FirstAttackKaitenHammer))
    ELSE
        SET_VALUE(LW(12), PTR(&marioDefaultWeapon_FirstAttackUltraHammer))
    END_IF
    CALL_CHILD(marioAttackEvent_KaitenHammer)
    RETURN
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_JishinHammer)
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL_CHILD(marioAttackEvent_MajinaiPowerUpCheck)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcSetParamAll, 4, 25, 0, 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 0)
    CALL(btlevtcmd_SetupAC, -2, 3, 1, 0)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -2)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_MovePosition, -2, -60, LW(1), LW(2), 0, -1, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 4)
    CALL(evt_btl_camera_set_zoom, 0, 450)
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        SET_VALUE(LW(0), STRING("M_H_5A"))
        SET_VALUE(LW(1), STRING("M_H_5B"))
        SET_VALUE(LW(2), STRING("M_H_14"))
        CASE_EQUAL(3)
        SET_VALUE(LW(0), STRING("M_H_9A"))
        SET_VALUE(LW(1), STRING("M_H_9B"))
        SET_VALUE(LW(2), STRING("M_H_15"))
        CASE_DEFAULT
        SET_VALUE(LW(0), STRING("M_H_1A"))
        SET_VALUE(LW(1), STRING("M_H_1B"))
        SET_VALUE(LW(2), STRING("M_H_13"))
    END_SWITCH
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_POWER2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(0))
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_EQUAL(1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_DEFAULT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SWITCH
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(1))
    CALL(btlevtcmd_StartAC, 1)
    WAIT_FRAMES(75)
    BEGIN_SIBLING_EVENT_GET_ID(LW(13))
        SET_VALUE(LW(5), 5)
        LOOP(LW(5))
            CALL(btlevtcmd_AcGetOutputParam, 1, LW(6))
            IF_INT_GREATER_OR_EQUAL(LW(6), 3)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
            SET_VALUE(LW(5), 5)
        END_LOOP
        WAIT_FRAMES(25)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        SET_VALUE(LW(5), 5)
        LOOP(LW(5))
            CALL(btlevtcmd_AcGetOutputParam, 1, LW(6))
            IF_INT_GREATER_OR_EQUAL(LW(6), 3)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
            SET_VALUE(LW(5), 5)
        END_LOOP
        WAIT_FRAMES(5)
        CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
        MULTIPLY(LW(5), 45)
        CALL(evt_sub_intpl_init, 11, 0, LW(5), 10)
        LOOP(10)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_FRAMES(8)
        CALL(evt_sub_intpl_init, 12, LW(5), 0, 15)
        LOOP(15)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        SET_VALUE(LW(5), 5)
        LOOP(LW(5))
            CALL(btlevtcmd_AcGetOutputParam, 1, LW(6))
            IF_INT_GREATER_OR_EQUAL(LW(6), 3)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
            SET_VALUE(LW(5), 5)
        END_LOOP
        CALL(btlevtcmd_GetPos, -2, LW(6), LW(7), LW(8))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
        CALL(btlevtcmd_JumpPosition, -2, LW(6), LW(7), LW(8), 40, -1)
    END_SIBLING_EVENT
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(8))
    CALL(btlevtcmd_AcGetOutputParam, 1, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 3)
        GOTO(10)
    END_IF
    DELETE_EVENT(LW(13))
    DELETE_EVENT(LW(14))
    DELETE_EVENT(LW(15))
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING2"), 0)
        SET_VALUE(LW(2), STRING("M_H_14"))
        CASE_EQUAL(1)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING1"), 0)
        SET_VALUE(LW(2), STRING("M_H_13"))
        CASE_DEFAULT
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING3"), 0)
        SET_VALUE(LW(2), STRING("M_H_15"))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
    CALL(evt_snd_sfxon, STRING("SFX_MARIO_ATTACK_ULTRAQUAKE1"), 0)
    WAIT_FRAMES(10)
    GOTO(40)
    LABEL(10)
    CALL(btlevtcmd_WaitEventEnd, LW(13))
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING2"), 0)
        CASE_EQUAL(1)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING1"), 0)
        CASE_DEFAULT
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING3"), 0)
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
    CALL(evt_snd_sfxon, STRING("SFX_MARIO_ATTACK_ULTRAQUAKE1"), 0)
    CALL(btlevtcmd_WaitEventEnd, LW(14))
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    LABEL(40)
    CALL(evt_btl_camera_set_mode, 0, 1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
    CALL(evt_btl_camera_set_zoom, 0, 0)
    CALL(evt_btl_camera_off_posoffset_manual, 0)
    CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_ULTRAQUAKE1"), 0)
    CALL(evt_btl_camera_shake_h, 0, 8, 0, 42, 13)
    CALL(evt_audience_reflesh_natural_all, 100)
    CALL(btlevtcmd_GetResultAC, LW(7))
    IF_BITS_SET(LW(7), 2)
        CALL(btlevtcmd_GetResultPrizeLv, -5, 0, LW(6))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), 0, 50, 0)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
    ELSE
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
    END_IF
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_StopAC)
        GOTO(90)
    END_IF
    GOTO(51)
    LABEL(50)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_StopAC)
        GOTO(90)
    END_IF
    LABEL(51)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(6))
    IF_INT_EQUAL(LW(6), 1)
        CALL(btlevtcmd_DamageDirect, LW(3), LW(4), 0, 0, 2, 0)
        BEGIN_INLINE_EVENT
            WAIT_FRAMES(42)
            IF_BITS_SET(LW(7), 2)
                CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
            ELSE
                CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
            END_IF
        END_INLINE_EVENT
    ELSE
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
    END_IF
    GOTO(50)
    LABEL(90)
    WAIT_FRAMES(12)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(0))
    WAIT_FRAMES(60)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_H_16"))
        WAIT_FRAMES(18)
    END_SIBLING_EVENT
    CALL(btlevtcmd_ACRStart, -2, 0, 17, 17, 0)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    SWITCH(LW(6))
        CASE_GREATER_OR_EQUAL(2)
        CASE_DEFAULT
        CALL(evt_audience_acrobat_notry)
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        GOTO(95)
    END_SWITCH
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_voice, -2, 1)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 20, 0)
        LOOP(60)
            CALL(btlevtcmd_AddRotate, -2, 0, 0, 12)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(8)
        CALL(btlevtcmd_ACRStart, -2, 42, 55, 55, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(4)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_Z_1"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_3A"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.2f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 60, -1)
    WAIT_FRAMES(4)
    CALL(btlevtcmd_ACRGetResult, LW(0), LW(1))
    IF_INT_EQUAL(LW(0), 2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_3B"))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 50)
        CALL(evt_eff, STRING("_BTLEF"), STRING("confetti"), 3, LW(0), LW(1), LW(2), 90, 0, 0, 0, 0, 0, 0, 0)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        WAIT_MS(1500)
    END_IF
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    LABEL(95)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_UltraJishinHammer)
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL_CHILD(marioAttackEvent_MajinaiPowerUpCheck)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcSetParamAll, 4, 16, 0, 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 0)
    CALL(btlevtcmd_SetupAC, -2, 3, 1, 0)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -2)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_MovePosition, -2, -60, LW(1), LW(2), 0, -1, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 4)
    CALL(evt_btl_camera_set_zoom, 0, 450)
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        SET_VALUE(LW(0), STRING("M_H_5A"))
        SET_VALUE(LW(1), STRING("M_H_5B"))
        SET_VALUE(LW(2), STRING("M_H_13"))
        CASE_EQUAL(3)
        SET_VALUE(LW(0), STRING("M_H_9A"))
        SET_VALUE(LW(1), STRING("M_H_9B"))
        SET_VALUE(LW(2), STRING("M_H_14"))
        CASE_DEFAULT
        SET_VALUE(LW(0), STRING("M_H_1A"))
        SET_VALUE(LW(1), STRING("M_H_1B"))
        SET_VALUE(LW(2), STRING("M_H_15"))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(0))
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_EQUAL(1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_DEFAULT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SWITCH
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(1))
    CALL(btlevtcmd_StartAC, 1)
    WAIT_FRAMES(32)
    BEGIN_SIBLING_EVENT_GET_ID(LW(13))
        SET_VALUE(LW(5), 5)
        LOOP(LW(5))
            CALL(btlevtcmd_AcGetOutputParam, 1, LW(6))
            IF_INT_GREATER_OR_EQUAL(LW(6), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
            SET_VALUE(LW(5), 5)
        END_LOOP
        WAIT_FRAMES(44)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        SET_VALUE(LW(5), 5)
        LOOP(LW(5))
            CALL(btlevtcmd_AcGetOutputParam, 1, LW(6))
            IF_INT_GREATER_OR_EQUAL(LW(6), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
            SET_VALUE(LW(5), 5)
        END_LOOP
        WAIT_FRAMES(10)
        CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
        MULTIPLY(LW(5), 45)
        CALL(evt_sub_intpl_init, 11, 0, LW(5), 10)
        LOOP(15)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_FRAMES(14)
        CALL(evt_sub_intpl_init, 12, LW(5), 0, 15)
        LOOP(15)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        SET_VALUE(LW(5), 5)
        LOOP(LW(5))
            CALL(btlevtcmd_AcGetOutputParam, 1, LW(6))
            IF_INT_GREATER_OR_EQUAL(LW(6), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
            SET_VALUE(LW(5), 5)
        END_LOOP
        CALL(btlevtcmd_GetPos, -2, LW(6), LW(7), LW(8))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.24f))
        CALL(btlevtcmd_JumpPosition, -2, LW(6), LW(7), LW(8), 55, -1)
    END_SIBLING_EVENT
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(8))
    CALL(btlevtcmd_AcGetOutputParam, 1, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 2)
        GOTO(10)
    END_IF
    DELETE_EVENT(LW(13))
    DELETE_EVENT(LW(14))
    DELETE_EVENT(LW(15))
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING2"), 0)
        SET_VALUE(LW(2), STRING("M_H_13"))
        CASE_EQUAL(1)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING1"), 0)
        SET_VALUE(LW(2), STRING("M_H_15"))
        CASE_DEFAULT
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING3"), 0)
        SET_VALUE(LW(2), STRING("M_H_14"))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
    CALL(evt_snd_sfxon, STRING("SFX_MARIO_ATTACK_ULTRAQUAKE1"), 0)
    WAIT_FRAMES(10)
    GOTO(40)
    LABEL(10)
    CALL(btlevtcmd_WaitEventEnd, LW(13))
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING2"), 0)
        CASE_EQUAL(1)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING1"), 0)
        CASE_DEFAULT
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING3"), 0)
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
    CALL(evt_snd_sfxon, STRING("SFX_MARIO_ATTACK_ULTRAQUAKE1"), 0)
    CALL(btlevtcmd_WaitEventEnd, LW(14))
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    LABEL(40)
    CALL(evt_btl_camera_set_mode, 0, 1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
    CALL(evt_btl_camera_set_zoom, 0, 0)
    CALL(evt_btl_camera_off_posoffset_manual, 0)
    CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_ULTRAQUAKE1"), 0)
    CALL(evt_btl_camera_shake_h, 0, 8, 0, 42, 13)
    CALL(evt_audience_reflesh_natural_all, 100)
    CALL(btlevtcmd_GetResultAC, LW(7))
    IF_BITS_SET(LW(7), 2)
        CALL(btlevtcmd_GetResultPrizeLv, -5, 0, LW(6))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), 0, 50, 0)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
    ELSE
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
    END_IF
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_StopAC)
        GOTO(90)
    END_IF
    GOTO(51)
    LABEL(50)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_StopAC)
        GOTO(90)
    END_IF
    LABEL(51)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(6))
    IF_INT_EQUAL(LW(6), 1)
        CALL(btlevtcmd_DamageDirect, LW(3), LW(4), 0, 0, 2, 0)
        BEGIN_INLINE_EVENT
            WAIT_FRAMES(42)
            IF_BITS_SET(LW(7), 2)
                CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
            ELSE
                CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
            END_IF
        END_INLINE_EVENT
    ELSE
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
    END_IF
    GOTO(50)
    LABEL(90)
    WAIT_FRAMES(12)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    WAIT_FRAMES(30)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(0))
    WAIT_FRAMES(60)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_H_16"))
    WAIT_FRAMES(10)
    LABEL(95)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_HammerRotate)
    SET_VALUE(LW(0), -25)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, 3, 0, 12, 0)
    LABEL(0)
    CALL(btlevtcmd_AddPartsRotate, -2, 3, 0, 0, LW(0))
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_HammerNageru)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL_CHILD(marioAttackEvent_MajinaiPowerUpCheck)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcSetParamAll, 5, 40, 0, 0, 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 0)
    CALL(btlevtcmd_SetupAC, -2, 3, 1, 0)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -2)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(6))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 4)
    CALL(evt_btl_camera_set_zoom, 0, 450)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_POWER2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        SET_VALUE(LW(0), STRING("M_H_5A"))
        SET_VALUE(LW(1), STRING("M_H_5B"))
        SET_VALUE(LW(2), STRING("M_H_7"))
        CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("M_H_8"))
        CASE_EQUAL(3)
        SET_VALUE(LW(0), STRING("M_H_9A"))
        SET_VALUE(LW(1), STRING("M_H_9B"))
        SET_VALUE(LW(2), STRING("M_H_11"))
        CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("M_H_12"))
        CASE_DEFAULT
        SET_VALUE(LW(0), STRING("M_H_1A"))
        SET_VALUE(LW(1), STRING("M_H_1B"))
        SET_VALUE(LW(2), STRING("M_H_3"))
        CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("M_H_4"))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(0))
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_EQUAL(1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_DEFAULT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SWITCH
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(1))
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_off_posoffset_manual, 0)
    CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_NAGERU1"), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
    WAIT_FRAMES(3)
    SPAWN_EVENT_GET_ID(PTR(marioAttackEvent_HammerNageru_object), LW(15))
    CALL(btlevtcmd_ACRStart, -2, 0, 15, 15, 0)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    SWITCH(LW(6))
        CASE_GREATER_OR_EQUAL(2)
        CASE_DEFAULT
        CALL(evt_audience_acrobat_notry)
        WAIT_MS(1000)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_S_1"))
        WAIT_MS(667)
        GOTO(94)
    END_SWITCH
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_voice, -2, 1)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 20, 0)
        LOOP(30)
            CALL(btlevtcmd_AddRotate, -2, 0, 0, -24)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(8)
        CALL(btlevtcmd_ACRStart, -2, 12, 25, 25, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(4)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_Z_1"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_3A"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.2f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    WAIT_FRAMES(4)
    CALL(btlevtcmd_ACRGetResult, LW(0), LW(1))
    IF_INT_EQUAL(LW(0), 2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_A_3B"))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 50)
        CALL(evt_eff, STRING("_BTLEF"), STRING("confetti"), 3, LW(0), LW(1), LW(2), 90, 0, 0, 0, 0, 0, 0, 0)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        WAIT_MS(1500)
    END_IF
    LABEL(94)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 5)
    CALL(evt_btl_camera_off_posoffset_manual, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_HammerNageru_object)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 25)
    CALL(btlevtcmd_SetPartsPos, -2, 3, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
    SET_VALUE(LW(0), -25)
    CALL(btlevtcmd_SetPartsRotate, -2, 3, 0, 0, 0)
    SPAWN_EVENT_GET_ID(PTR(marioAttackEvent_HammerRotate), LW(13))
    CALL(btlevtcmd_JumpPartsSetting, -2, 3, 0, FLOAT(6.0f), FLOAT(0.35f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsPosition, -2, 3, LW(0), LW(1), LW(2), 0, -1)
    IF_INT_NOT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_JumpPartsContinue, -2, 3)
        CALL(btlevtcmd_WeaponAftereffect, LW(12))
        CALL(btlevtcmd_GetPartsPos, -2, 3, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), -30)
        CALL(btlevtcmd_JumpPartsPosition, -2, 3, LW(0), LW(1), LW(2), 20, -1)
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), -15)
        CALL(btlevtcmd_JumpPartsPosition, -2, 3, LW(0), LW(1), LW(2), 12, -1)
        DELETE_EVENT(LW(13))
        WAIT_FRAMES(30)
        CALL(btlevtcmd_OnPartsAttribute, -2, 3, 16777216)
        GOTO(90)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_btl_camera_set_mode, 0, 1)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 4)
        CALL(evt_btl_camera_set_zoom, 0, 0)
        CALL(evt_btl_camera_off_posoffset_manual, 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
    ELSE
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
    END_IF
    CALL(btlevtcmd_GetDamage, LW(3), LW(0))
    CALL(evt_btl_camera_shake_h, 0, LW(0), 0, 10, 13)
    DELETE_EVENT(LW(13))
    CALL(btlevtcmd_OnPartsAttribute, -2, 3, 16777216)
    WAIT_FRAMES(20)
    LABEL(90)
    CALL(btlevtcmd_StopAC)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_FireNaguri)
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL_CHILD(marioAttackEvent_MajinaiPowerUpCheck)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcSetParamAll, 0, 83, 63, 83, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 16)
    CALL(btlevtcmd_SetupAC, -2, 11, 1, 0)
    WAIT_FRAMES(2)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -2)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(evt_btl_camera_set_posoffset, 0, 40, 0, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 25)
    CALL(btlevtcmd_GetStageSize, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetPartsPos, -2, 3, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("M_H_8"))
    CALL(btlevtcmd_SetPartsWork, -2, 3, 0, 1)
    BEGIN_SIBLING_EVENT
        CALL(_mario_fire_ball_controll, -2, 3)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    ADD(LW(1), 140)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_DivePartsPosition, -2, 3, LW(0), LW(1), LW(2), 60, 0, 5, 0, -1)
    END_SIBLING_EVENT
    LABEL(10)
    CALL(btlevtcmd_PadCheckNow, 0, 262144, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        WAIT_FRAMES(1)
        GOTO(10)
    END_IF
    IS_EVENT_RUNNING(LW(15), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        DELETE_EVENT(LW(15))
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_MARIO_POWER2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        SET_VALUE(LW(0), STRING("M_H_18A"))
        SET_VALUE(LW(1), STRING("M_H_18B"))
        SET_VALUE(LW(2), STRING("M_H_18C"))
        CASE_EQUAL(3)
        SET_VALUE(LW(0), STRING("M_H_19A"))
        SET_VALUE(LW(1), STRING("M_H_19B"))
        SET_VALUE(LW(2), STRING("M_H_19C"))
        CASE_DEFAULT
        SET_VALUE(LW(0), STRING("M_H_17A"))
        SET_VALUE(LW(1), STRING("M_H_17B"))
        SET_VALUE(LW(2), STRING("M_H_17C"))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(0))
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_EQUAL(1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_DEFAULT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_MARIO_HAMMER_HOLD3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SWITCH
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_MARIO_HAMMER_FIRE1"), EVTDAT_POINTER_MAX, 0, LW(15))
    BEGIN_INLINE_EVENT_GET_ID(LW(14))
        CALL(btlevtcmd_GetPartsPos, -2, 3, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpPartsSetting, -2, 3, 0, FLOAT(4.0f), FLOAT(0.2f))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_MARIO_HAMMER_FIRE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_FallPartsPosition, -2, 3, LW(0), LW(1), LW(2), 75)
    END_INLINE_EVENT
    CALL(btlevtcmd_StartAC, 1)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(1))
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_CLEAR(LW(0), 2)
        CALL(btlevtcmd_AcGetOutputParam, 0, LW(0))
        BEGIN_SIBLING_EVENT
            CALL(_get_mario_hammer_lv, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(2)
                CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING2"), 0)
                CASE_EQUAL(1)
                CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING1"), 0)
                CASE_DEFAULT
                CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING3"), 0)
            END_SWITCH
            CALL(evt_snd_sfxoff, LW(15))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_MARIO_HAMMER_FIRE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
            WAIT_FRAMES(7)
            WAIT_FRAMES(60)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_S_1"))
        END_SIBLING_EVENT
        CALL(btlevtcmd_WaitEventEnd, LW(14))
        CALL(btlevtcmd_SetPartsWork, -2, 3, 0, 0)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        CALL(btlevtcmd_GetPartsPos, -2, 3, LW(0), LW(1), LW(2))
        CALL(evt_eff64, STRING(""), STRING("fire_spark_n64"), 0, LW(0), LW(1), LW(2), 30, 40, FLOAT(1.0f), 12, 100, FLOAT(1.0f), FLOAT(1.0f), 0)
        ADD(LW(1), 5)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 11, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        CALL(evt_btl_camera_set_mode, 0, 1)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
        CALL(evt_btl_camera_set_zoom, 0, 0)
        CALL(evt_btl_camera_off_posoffset_manual, 0)
        CALL(btlevtcmd_GetUnitId, -2, LW(6))
        CALL(btlevtcmd_GetEnemyBelong, LW(6), LW(0))
        CALL(btlevtcmd_SamplingEnemy, -5, LW(0), PTR(&badgeWeapon_FireNaguriFailed))
        CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
        LABEL(15)
        IF_INT_EQUAL(LW(3), -1)
            GOTO(17)
        END_IF
        IF_INT_EQUAL(LW(3), LW(6))
            GOTO(16)
        END_IF
        LABEL(16)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
        GOTO(15)
        LABEL(17)
        CALL(btlevtcmd_GetUnitId, -2, LW(6))
        CALL(btlevtcmd_GetEnemyBelong, LW(6), LW(0))
        CALL(btlevtcmd_SamplingEnemy, -5, LW(0), PTR(&badgeWeapon_FireNaguriFailed))
        CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
        LABEL(20)
        IF_INT_EQUAL(LW(3), -1)
            GOTO(22)
        END_IF
        IF_INT_NOT_EQUAL(LW(3), LW(6))
            GOTO(21)
        END_IF
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        LABEL(21)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
        GOTO(20)
        LABEL(22)
        WAIT_FRAMES(30)
        GOTO(99)
    END_IF
    CALL(_get_mario_hammer_lv, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING2"), 0)
        CASE_EQUAL(1)
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING1"), 0)
        CASE_DEFAULT
        CALL(evt_snd_sfxon, STRING("SFX_MARIO_HAMMER_SWING3"), 0)
    END_SWITCH
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_MARIO_HAMMER_FIRE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(2))
    WAIT_FRAMES(7)
    DELETE_EVENT(LW(14))
    CALL(evt_btl_camera_set_mode, 0, 1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 0)
    CALL(evt_btl_camera_off_posoffset_manual, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_MARIO_HAMMER_FIRE4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    SPAWN_EVENT_GET_ID(PTR(marioAttackEvent_FireNaguri_object), LW(15))
    CALL(btlevtcmd_ACRStart, -2, 0, 15, 15, 0)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    SWITCH(LW(6))
        CASE_GREATER_OR_EQUAL(2)
        LOOP(20)
            CALL(btlevtcmd_AddRotate, -2, 0, 36, 0)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_snd_voice, -2, 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_V_2"))
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        WAIT_MS(500)
        CASE_DEFAULT
        CALL(evt_audience_acrobat_notry)
        WAIT_FRAMES(60)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_S_1"))
    END_SWITCH
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("M_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_FireNaguri_object)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_StopAC)
        CALL(btlevtcmd_GetPartsPos, -2, 3, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetStageSize, LW(5), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        MULTIPLY(LW(5), 2)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(5))
        CALL(btlevtcmd_JumpPartsSetting, -2, 3, 0, FLOAT(6.0f), FLOAT(0.3f))
        CALL(btlevtcmd_FallPartsPosition, -2, 3, LW(0), LW(1), LW(2), 0)
        CALL(btlevtcmd_SetPartsWork, -2, 3, 0, 0)
        RETURN
    END_IF
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 20)
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsSetting, -2, 3, 0, FLOAT(4.0f), FLOAT(0.5f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 3, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_SetPartsWork, -2, 3, 0, 0)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(0))
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        CALL(btlevtcmd_GetResultPrizeLv, -5, 0, LW(6))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), 0, 50, 0)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), LW(6))
    ELSE
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareACResult, LW(0), -1)
    END_IF
    CALL(btlevtcmd_GetPartsPos, -2, 3, LW(7), LW(8), LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, -1, -1, 2, LW(12))
    MULTIPLY(LW(12), 16)
    SET_VALUE(LW(13), 5)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_StopAC)
        GOTO(90)
    END_IF
    GOTO(51)
    LABEL(50)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_StopAC)
        GOTO(90)
    END_IF
    LABEL(51)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(11), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, -1, -1, 2, LW(14))
    IF_INT_GREATER_OR_EQUAL(LW(14), 0)
        IF_INT_LESS_OR_EQUAL(LW(7), LW(11))
            WAIT_FRAMES(6)
            ADD(LW(7), LW(12))
            BEGIN_INLINE_EVENT
                CALL(_fire_wave, LW(7), LW(8), LW(9), LW(13))
            END_INLINE_EVENT
            GOTO(51)
        END_IF
    ELSE
        IF_INT_GREATER_OR_EQUAL(LW(7), LW(11))
            WAIT_FRAMES(6)
            ADD(LW(7), LW(12))
            BEGIN_INLINE_EVENT
                CALL(_fire_wave, LW(7), LW(8), LW(9), LW(13))
            END_INLINE_EVENT
            GOTO(51)
        END_IF
    END_IF
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(6))
    IF_INT_EQUAL(LW(6), 1)
        CALL(btlevtcmd_GetResultAC, LW(0))
        IF_BITS_SET(LW(0), 2)
            CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
        ELSE
            CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        END_IF
        IF_INT_GREATER_OR_EQUAL(LW(13), 2)
            SUBTRACT(LW(13), 1)
        END_IF
    ELSE
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
    END_IF
    GOTO(50)
    LABEL(90)
    LABEL(92)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, -1, -1, 2, LW(10))
    CALL(btlevtcmd_GetStageSize, LW(11), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    MULTIPLY(LW(11), LW(10))
    IF_INT_GREATER_OR_EQUAL(LW(11), 0)
        IF_INT_LESS_OR_EQUAL(LW(7), LW(11))
            WAIT_FRAMES(6)
            ADD(LW(7), LW(12))
            BEGIN_INLINE_EVENT
                CALL(_fire_wave, LW(7), LW(8), LW(9), LW(13))
            END_INLINE_EVENT
            GOTO(92)
        END_IF
    ELSE
        IF_INT_GREATER_OR_EQUAL(LW(7), LW(11))
            WAIT_FRAMES(6)
            ADD(LW(7), LW(12))
            BEGIN_INLINE_EVENT
                CALL(_fire_wave, LW(7), LW(8), LW(9), LW(13))
            END_INLINE_EVENT
            GOTO(92)
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(_NormalCharge_core)
    CALL(btlevtcmd_GetUnitId, -2, LW(3))
    CALL(btlevtcmd_GetBodyId, -2, LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    WAIT_FRAMES(45)
    LOOP(30)
        CALL(btlevtcmd_AddRotate, -2, 0, 24, 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(evt_snd_sfxon, STRING("SFX_MARIO_CHARGE1"), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, -2, LW(5))
    DIVIDE(LW(5), 2)
    ADD(LW(1), LW(5))
    ADD(LW(2), 10)
    CALL(evt_eff64, STRING("noname"), STRING("crystal_n64"), 7, LW(0), LW(1), LW(2), FLOAT(1.5f), 60, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(_SuperCharge_core)
    CALL(btlevtcmd_GetUnitId, -2, LW(3))
    CALL(btlevtcmd_GetBodyId, -2, LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    WAIT_FRAMES(45)
    LOOP(30)
        CALL(btlevtcmd_AddRotate, -2, 0, 24, 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(evt_snd_sfxon, STRING("SFX_MARIO_SUPER_CHARGE1"), 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, -2, LW(5))
    DIVIDE(LW(5), 2)
    ADD(LW(1), LW(5))
    ADD(LW(2), 10)
    CALL(evt_eff64, STRING("noname"), STRING("crystal_n64"), 7, LW(0), LW(1), LW(2), FLOAT(1.5f), 60, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()


USER_FUNC(_battle_majinai_effect) {
    EffectEntry* effect;
    Vec pos;

    BattleGetMarioPtr(_battleWorkPointer);
    if (isFirstCall) {
        pos = (Vec){ 0.0f, 10.0f, 0.0f };
        event->userdata[0] = (s32)effMajinaiEntry(0, pos.x, pos.y, pos.z);
        effect = (EffectEntry*)event->userdata[0];
        effSetName(effect, "mjef_coinup");
    }
    if (event->userdata[0] != 0 && effNameToPtr("mjef_coinup") != NULL) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_paper_light_off) {
    s32* args = event->args;
    BattleUnitPart* part;
    s32 id;
    s32 v0, v1;

    v0 = evtGetValue(event, args[0]);
    id = BattleTransID(event, v0);
    v1 = evtGetValue(event, args[1]);
    part = BattleGetUnitPartsPtr(id, v1);
    animPoseSetMaterialLightFlagOff(part->poseId, 1);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(marioAttackEvent_MajinaiPowerUpCheck)
    CALL(_majinai_powerup_check, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 0)
    SPAWN_EVENT(evt_majinai_effect_se_btl)
    CALL(_battle_majinai_effect, -3)
    CALL_CHILD(marioAttackEvent_MajinaiJump)
    CALL(btlevtcmd_AnnounceMessage, 0, 0, 0, STRING("btl_msg_mj_power_up"), 60)
    WAIT_FRAMES(60)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_MajinaiJump)
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetBodyId, -2, LW(6))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(6), 44)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(6), 35)
    RETURN
EVT_END()

EVT_BEGIN(_MajinaiPoseReset)
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_MajinaiDefenceUp)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 0)
    SPAWN_EVENT(evt_majinai_effect_se_btl)
    CALL(_battle_majinai_effect, -3)
    CALL_CHILD(marioAttackEvent_MajinaiJump)
    CALL(btlevtcmd_AnnounceMessage, 0, 0, 0, STRING("btl_msg_mj_defence_up"), 60)
    WAIT_FRAMES(60)
    CALL_CHILD(_MajinaiPoseReset)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_MajinaiExpUp)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 0)
    SPAWN_EVENT(evt_majinai_effect_se_btl)
    CALL(_battle_majinai_effect, -3)
    CALL_CHILD(marioAttackEvent_MajinaiJump)
    CALL(btlevtcmd_AnnounceMessage, 0, 0, 0, STRING("btl_msg_mj_exp_up"), 60)
    WAIT_FRAMES(60)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_GatsunJump)
    SET_VALUE(LW(12), PTR(&badgeWeapon_GatsunJump))
    CALL_CHILD(marioAttackEvent_GatsuDokaJump)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_DokanJump)
    SET_VALUE(LW(12), PTR(&badgeWeapon_DokanJump))
    CALL_CHILD(marioAttackEvent_GatsuDokaJump)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_MiniminiFumi)
    CALL_CHILD(marioAttackEvent_NormalJump)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_NemuraseFumi)
    CALL_CHILD(marioAttackEvent_NormalJump)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_FunyafunyaJump)
    CALL_CHILD(marioAttackEvent_NormalJump)
    RETURN
EVT_END()

static EVT_BEGIN(marioAttackEvent_NormalHammer)
    SET_VALUE(LW(14), 0)
    CALL_CHILD(marioAttackEvent_NormalHammer_Core)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_GatsunHammer)
    SET_VALUE(LW(14), 1)
    CALL_CHILD(marioAttackEvent_NormalHammer_Core)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_IceNaguri)
    SET_VALUE(LW(14), 0)
    CALL_CHILD(marioAttackEvent_NormalHammer_Core)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_TsuranukiHammer)
    SET_VALUE(LW(14), 1)
    CALL_CHILD(marioAttackEvent_NormalHammer_Core)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_NormalCharge)
    SET_VALUE(LW(12), PTR(&badgeWeapon_Charge))
    CALL_CHILD(_NormalCharge_core)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_NormalChargeP)
    SET_VALUE(LW(12), PTR(&badgeWeapon_ChargeP))
    CALL_CHILD(_NormalCharge_core)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_SuperCharge)
    SET_VALUE(LW(12), PTR(&badgeWeapon_SuperCharge))
    CALL_CHILD(_SuperCharge_core)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_SuperChargeP)
    SET_VALUE(LW(12), PTR(&badgeWeapon_SuperChargeP))
    CALL_CHILD(_SuperCharge_core)
    RETURN
EVT_END()
// clang-format on

const f32 float_1_8041c0c4 = 1.0f;
