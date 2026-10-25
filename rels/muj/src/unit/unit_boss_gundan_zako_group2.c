#include "unit_boss_gundan_zako_group2.h"
#include "unit_boss_gundan_zako_group3.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_stage_object.h"
#include "driver/extdrv.h"
#include "pmario_sound.h"
#include "system.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "math_ext.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"
#include <stdlib.h>

#pragma pool_data off

#pragma force_active on
extern Vec vec3_muj_0001323c;
extern Vec vec3_muj_00013248;
extern Vec vec3_muj_00013254;
extern Vec vec3_muj_00013260;
extern Vec vec3_muj_0001326c;
extern Vec vec3_muj_00013278;
extern Vec vec3_muj_00013284;

static BattleVulnerableStatus regist;
static BattleDataEntry data_table[3];
static BattleUnitKindPart parts[1];
static s32 init_event[21];
static s32 damage_event[129];
static s32 dead_event[136];
static s32 attack_event[32];
s32 fall_attack_event[93];
static s32 shake_attack_event[281];
static s32 hit_check_event[34];
s32 hit_check_event2[7];
s32 hit_check_event_sub[123];
static s32 wait_event[6];

// clang-format off
extend_data(84,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(85,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boss_gundan_zako_group2 = {
    UNIT_X_NAUTS_CRUMP_FORMATION2, // mUnitType
    "btl_un_gundan_zako_group2",   // name
    6,                             // maxHp
    0,                             // maxFp
    1,                             // mDangerHP
    1,                             // mPerilHP
    0,                             // mLevel
    0,                             // mBonusExp
    0,                             // bonusCoinRolls
    100,                           // bonusCoinRate
    0,                             // baseCoin
    50,                            // escapeRate
    9999,                          // renzokuSoftCap
    400,                           // width
    36,                            // height
    { 0, 55 },                     // statusIconOffset
    { 0.0f, 0.0f, 0.0f },          // centerOffset
    { 0, 0 },                      // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },          // talkTogeBaseOffset
    { 200.0f, 0.0f, -10.0f },      // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },          // burnFlameOffset
    0.0f,                          // field_0x54
    0.0f,                          // field_0x58
    { 200.0f, 0.0f, 0.0f },        // bintaHitOffset
    { 200.0f, 23.4f, 0.0f },       // kissHitOffset
    { 60.0f, -35.0f, 0.0f },       // cutBaseOffset
    280.0f,                        // cutWidth
    36.0f,                         // cutHeight
    0,                             // turnOrder
    0,                             // turnOrderVariance
    -1,                            // swallowRate
    0,                             // swallowAttribute
    0,                             // field_0x8C
    0,                             // itemStealParam
    { 0.0f, 0.0f, 0.0f },          // starPointDispOffset
    "SFX_BOSS_TEAM_3RD_DAMAGED1",  // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",        // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",         // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",    // electricDamageSfxName
    0x00200002,                    // attributeFlags
    &regist,                       // vulnStatus
    1,                             // numParts
    parts,                         // mParts
    init_event,                    // initEvtScript
    data_table,                    // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    0, // sleepRate
    0, // stopRate
    0, // dizzyRate
    0, // poisonRate
    0, // confuseRate
    0, // electricRate
    0, // burnRate
    0, // freezeRate
    0, // hugeRate
    0, // tinyRate
    0, // attackUpRate
    0, // attackDownRate
    0, // defenseUpRate
    0, // defenseDownRate
    0, // allergicRate
    0, // frightRate
    0, // galeForceRate
    0, // fastRate
    0, // slowRate
    0, // dodgyRate
    0, // invisibleRate
    0, // instaKillRate
};

static BattleDispPose pose_table[4] = {
    { 0x1C, "Z_1" },
    { 0x1F, "Z_1" },
    { 0x27, "Z_1" },
    { 0x45, "Z_1" },
};

static BattleDataEntry data_table[3] = {
    { 0x31, dead_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                           // partNum
        "btl_un_gundan_zako_group2", // partName
        "c_zako_n",                  // modelName
        { 0.0f, 0.0f, 0.0f },        // positionOffset
        { 0.0f, 0.0f, 0.0f },        // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },        // hitCursorBaseOffset
        20,                          // shadowSize
        30,                          // field_0x32
        255,                         // baseAlpha
        &defence,                    // defense
        &defence_attr,               // defenseAttr
        0x03600009,                  // attributes
        0,                           // counterAttributes
        pose_table,                  // poseTable
    },
};

BattleWeapon weapon_gundan_zako_group2_attack = {
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
    weaponGetPowerDefault,                    // hpDamageFn
    { 1, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20001000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
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

BattleWeapon weapon_gundan_zako_group2_yusuri_attack = {
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
    weaponGetPowerDefault,                    // hpDamageFn
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x02101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x00000004,                               // targetWeightingFlags
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


s32 down_ceil_20_text_9C8C(EventEntry* event, BOOL isFirstCall) {
    u8* flag = &_battleWorkPointer->field_180C0.stageFlags;
    BattleStageObject* obj = BattleSearchObjectPtr("if_body");
    BattleStageObject* obj2 = BattleSearchObjectPtr("if_wire");
    s32 id;
    f32 value;

    if (isFirstCall) {
        work_20_bss_6C->ceilFrom = vec3_muj_0001323c;
        work_20_bss_6C->ceilTo = vec3_muj_00013248;
        work_20_bss_6C->ceilFrame = 0x78;
        work_20_bss_6C->ceilFrames = 0x78;
        flag[0] &= ~0x20;
        id = psndSFXOn("SFX_BOSS_TEAM_3RD_MOVE_CAILING1");
        work_20_bss_6C->ceilSfxHandle = id;
        work_20_bss_6C->ceilSfxHandle = id;
    }
    value = intplGetValue(4, work_20_bss_6C->ceilFrames - work_20_bss_6C->ceilFrame, work_20_bss_6C->ceilFrames, work_20_bss_6C->ceilFrom.y, work_20_bss_6C->ceilTo.y);
    obj2->mPosition.y = value;
    obj->mPosition.y = value;
    work_20_bss_6C->ceilFrame -= 1;
    if (work_20_bss_6C->ceilFrame < 0) {
        psndSFXOff(work_20_bss_6C->ceilSfxHandle);
        return 2;
    }
    return 0;
}

s32 up_ceil(EventEntry* event, BOOL isFirstCall) {
    u8* flag = &_battleWorkPointer->field_180C0.stageFlags;
    BattleStageObject* obj = BattleSearchObjectPtr("if_body");
    BattleStageObject* obj2 = BattleSearchObjectPtr("if_wire");
    f32 value;

    if (isFirstCall) {
        work_20_bss_6C->ceilFrom = vec3_muj_00013254;
        work_20_bss_6C->ceilTo = vec3_muj_00013260;
        work_20_bss_6C->ceilFrame = 0x78;
        work_20_bss_6C->ceilFrames = 0x78;
        flag[0] |= 0x20;
        work_20_bss_6C->ceilSfxHandle = psndSFXOn("SFX_BOSS_TEAM_3RD_MOVE_CAILING1");
    }
    value = intplGetValue(4, work_20_bss_6C->ceilFrames - work_20_bss_6C->ceilFrame, work_20_bss_6C->ceilFrames, work_20_bss_6C->ceilFrom.y, work_20_bss_6C->ceilTo.y);
    obj2->mPosition.y = value;
    obj->mPosition.y = value;
    work_20_bss_6C->ceilFrame -= 1;
    if (work_20_bss_6C->ceilFrame < 0) {
        psndSFXOff(work_20_bss_6C->ceilSfxHandle);
        return 2;
    }
    return 0;
}

s32 shake_ceil(EventEntry* event, BOOL isFirstCall) {
    BattleStageObject* obj = BattleSearchObjectPtr("if_body");
    BattleStageObject* obj2 = BattleSearchObjectPtr("if_wire");
    f32 start;
    f32 end;
    s32 step;
    f32 value;

    if (isFirstCall) {
        work_20_bss_6C->ceilFrom = vec3_muj_0001326c;
        work_20_bss_6C->ceilTo = vec3_muj_00013278;
        work_20_bss_6C->ceilFrame = 0;
    }
    start = work_20_bss_6C->ceilFrom.y;
    end = work_20_bss_6C->ceilTo.y;
    step = work_20_bss_6C->ceilFrame;

    value = intplGetValue(4, step, 9, start, end);
    obj2->mPosition.y = value;
    obj->mPosition.y = value;
    work_20_bss_6C->ceilFrame += 1;
    if (work_20_bss_6C->ceilFrame > 0x11) {
        f32 last = 130.0f;

        obj2->mPosition.y = last;
        obj->mPosition.y = last;
        return 2;
    }
    return 0;
}

s32 zakogun_ext_init2(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    ExtPose* pose = extGetPosePtr();

    work_20_bss_6C->phase = 1;
    for (i = 0; i < 25; i++, pose++) {
        zakogun_ext_init123_common(pose, i);
    }
    return 2;
}

void zakogun_ext_main2(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit = zakogun_get_ptr();
    ExtPose* pose = extGetPosePtr();
    BattleStageObject* obj;
    s32 i;
    f32 x;
    f32 y;
    f32 z;
    f32 deg2rad = 0.017453292f;
    f32 k = 0.25f;
    Mtx mtx;
    const char* sa;
    const char* sc;
    const char* sa2;

    if (unit == NULL) {
        return;
    }
    obj = BattleSearchObjectPtr("if_body");
#ifdef MUST_MATCH
    // MUST_MATCH: folding each literal through a comma makes MWCC materialise its address directly
    // (no extra `mr`), and a second literal local reproduces the original's duplicate register.
    sa = (0, "SFX_BOSS_TEAM_3RD_ATTACK2");
    sc = (0, "SFX_BOSS_TEAM_3RD_MOVE_CAILING2");
    sa2 = (0, "SFX_BOSS_TEAM_3RD_ATTACK2");
#else
    sa = "SFX_BOSS_TEAM_3RD_ATTACK2";
    sc = "SFX_BOSS_TEAM_3RD_MOVE_CAILING2";
    sa2 = sa;
#endif
    for (i = 0; i < 25; i++, pose++) {
        ZakogunEntry* entry = &zakogun[i];

        pose->texId = entry->poseStart + entry->poseIndex / entry->poseDiv;
        entry->poseIndex++;
        if (entry->poseIndex >= entry->poseCount * entry->poseDiv) {
            entry->poseIndex = 0;
        }
        switch (entry->state) {
        case 0:
            entry->state = 5;
            entry->rotZ = 0.0f;
            entry->rotY = 0.0f;
            entry->rotX = 0.0f;
            zakogun_set_pose(entry, 0x25, 4, 6);
            break;
        case 5:
            entry->pos.x = entry->target.x = (obj->mPosition.x - 250.0f) + 20.0f * (f32)i;
            entry->pos.y = entry->target.y = obj->mPosition.y - 35.0f;
            entry->pos.z = entry->target.z = 0.0f;
            entry->state = 0xA;
            break;
        case 0xA:
            entry->pos.x = entry->target.x = ((obj->mPosition.x + unit->mPosition.x) - 250.0f) + 20.0f * (f32)i;
            entry->pos.y = entry->target.y = obj->mPosition.y - 35.0f;
            if (entry->pos.y < unit->mPosition.y) {
                entry->pos.y = unit->mPosition.y;
            }
            entry->pos.z = entry->target.z = -10.0f;
            break;
        case 0x14: {
            BattleUnit* u = BattleGetUnitPtr(wp, work_20_bss_6C->targetId);

            BtlUnit_GetPos(u, &x, &y, &z);
            entry->from = entry->pos;
            if (entry->pos.x >= x) {
                entry->to.x = entry->pos.x - 30.0f;
                entry->rotY = 0.0f;
            } else {
                entry->to.x = entry->pos.x + 15.0f;
                entry->rotY = 180.0f;
            }
            entry->to.y = entry->from.y - 2.0f;
            entry->state = 0x19;
            entry->timerMax = 8;
            entry->timer = 8;
        }
            // fallthrough
        case 0x19:
            entry->pos.x = intplGetValue(4, entry->timerMax - entry->timer, entry->timerMax, entry->from.x, entry->to.x);
            entry->pos.y = intplGetValue(4, (entry->timerMax - entry->timer) * 2, entry->timerMax, entry->from.y, entry->to.y);
            entry->timer--;
            if (entry->timer < 0) {
                BattleUnit* u = BattleGetUnitPtr(wp, work_20_bss_6C->targetId);

                BtlUnit_GetPos(u, &x, &y, &z);
                if (__fabsf(entry->pos.x - x) <= 20.0) {
                    entry->state = 0x1E;
                } else {
                    entry->state = 0x14;
                }
            }
            break;
        case 0x1E: {
            BattleUnit* u = BattleGetUnitPtr(wp, work_20_bss_6C->targetId);

            BtlUnit_GetPos(u, &x, &y, &z);
            entry->velX = (f32)rand() / 32767.0f - 0.5f;
            entry->velY = 0.0f;
            entry->velZ = 1.5f;
            entry->to.y = 2.0f * (f32)work_20_bss_6C->landedCount + y;
            entry->state = 0x23;
            zakogun_set_pose(entry, 0x2C, 1, 1);
            psndSFXOn(sa);
        }
            // fallthrough
        case 0x23:
            entry->pos.x += entry->velX;
            entry->pos.y += entry->velY;
            entry->pos.z += entry->velZ;
            entry->velY += -0.5f;
            if (entry->pos.y < entry->to.y) {
                entry->pos.y = entry->to.y;
                entry->state = 0x28;
                work_20_bss_6C->landedCount++;
                if (work_20_bss_6C->landedCount == 0x19) {
                    unit->work[0] = 1;
                }
            }
            break;
        case 0x28:
            if (work_20_bss_6C->landedCount == 0x19) {
                entry->state = 0x2D;
                entry->timer = irand(0x3C);
            }
            break;
        case 0x2D:
            entry->timer--;
            if (entry->timer <= 0) {
                entry->velX = -5.0f;
                entry->velY = 3.0f;
                entry->state = 0x30;
                zakogun_set_pose(entry, 0x22, 1, 1);
                entry->rotY = 0.0f;
            }
            break;
        case 0x30:
            entry->pos.x += entry->velX;
            entry->pos.y += entry->velY;
            entry->velY += -0.5f;
            if (entry->pos.y < 0.0f) {
                entry->pos.y = 0.0f;
                zakogun_set_pose(entry, 0x16, 0xA, 1);
                entry->state = 0x31;
            }
            break;
        case 0x31:
            entry->pos.x += entry->velX;
            if (entry->pos.x <= -300.0f) {
                entry->state = 0x32;
                entry->pos.x = 300.0f;
                entry->pos.y = entry->target.y;
                entry->pos.z = entry->target.z;
                zakogun_set_pose(entry, 0x25, 4, 6);
                if (i == 0) {
                    psndSFXOn(sc);
                }
            }
            break;
        case 0x32: {
            BattleUnit* u = BattleGetUnitPtr(wp, work_20_bss_6C->targetId);

            BtlUnit_GetPos(u, &x, &y, &z);
            entry->rotY = 0.0f;
            entry->from = entry->pos;
            entry->to.x = entry->pos.x - 30.0f;
            if (entry->to.x < entry->target.x) {
                entry->to.x = entry->target.x;
            }
            entry->to.y = entry->from.y - 2.0f;
            entry->state = 0x37;
            entry->timerMax = 8;
            entry->timer = 8;
            btl_camera_set_mode(0, 0);
        }
            // fallthrough
        case 0x37:
            entry->pos.x = intplGetValue(4, entry->timerMax - entry->timer, entry->timerMax, entry->from.x, entry->to.x);
            entry->pos.y = intplGetValue(4, (entry->timerMax - entry->timer) * 2, entry->timerMax, entry->from.y, entry->to.y);
            entry->timer--;
            if (entry->timer < 0) {
                if (entry->pos.x <= entry->target.x) {
                    entry->pos.x = entry->target.x;
                    entry->state = 0xA;
                } else {
                    entry->state = 0x32;
                }
            }
            break;
        case 0x3C:
            zakogun_set_pose(entry, 0x29, 3, 6);
            entry->state = 0x41;
            entry->timer = 0x78;
            // fallthrough
        case 0x41:
            entry->pos.x = (obj->mPosition.x - 250.0f) + 20.0f * (f32)i;
            entry->pos.y = obj->mPosition.y - 35.0f;
            entry->pos.z = 0.0f;
            entry->timer--;
            if (entry->timer < 0) {
                entry->state = 0xA;
                zakogun_set_pose(entry, 0x25, 4, 6);
            }
            break;
        case 0x50:
            entry->timer = irand(0xA);
            entry->state = 0x52;
            // fallthrough
        case 0x52:
            entry->timer--;
            if (entry->timer <= 0) {
                entry->state = 0x53;
                entry->velY = 0.0f;
                psndSFXOn(sa2);
            }
            break;
        case 0x53:
            entry->velY += -0.5f;
            entry->pos.y += entry->velY;
            if (entry->pos.y <= 0.0f) {
                entry->pos.y = 0.0f;
                entry->state = 0x54;
                zakogun_set_pose(entry, 0x21, 3, 3);
            }
            break;
        case 0x54:
            entry->from = entry->pos;
            entry->to.x = entry->from.x + (f32)irand(100) - 50.0f;
            entry->to.y = 0.0f;
            entry->to.z = entry->from.z + (f32)irand(20) - 10.0f;
            entry->state = 0x55;
            entry->timerMax = 0x14;
            entry->timer = 0x14;
            entry->velY = (2.0f * (entry->to.y - entry->from.y) - -200.0f) / 40.0f;
            // fallthrough
        case 0x55:
            entry->pos.x = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax, entry->from.x, entry->to.x);
            entry->pos.z = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax, entry->from.z, entry->to.z);
            entry->pos.y += entry->velY;
            entry->velY += -0.5f;
            entry->timer--;
            if (entry->timer < 0) {
                entry->state = 0x56;
            }
            break;
        case 0x56:
            entry->from = entry->pos;
            entry->to.x = entry->from.x + (f32)irand(50) - 25.0f;
            entry->to.y = 0.0f;
            entry->to.z = entry->from.z + (f32)irand(10) - 5.0f;
            entry->state = 0x57;
            entry->timerMax = 0xA;
            entry->timer = 0xA;
            entry->velY = (2.0f * (entry->to.y - entry->from.y) - -50.0f) / 20.0f;
            // fallthrough
        case 0x57:
            entry->pos.x = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax, entry->from.x, entry->to.x);
            entry->pos.z = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax, entry->from.z, entry->to.z);
            entry->pos.y += entry->velY;
            entry->velY += -0.5f;
            entry->timer--;
            if (entry->timer < 0) {
                entry->state = 0x58;
                entry->rotY = 0.0f;
                entry->timer = 0x30;
            }
            break;
        case 0x58:
            entry->rotY -= 15.0f;
            entry->timer--;
            if (entry->timer <= 0) {
                entry->rotY = 0.0f;
                entry->state = 0x59;
            }
            break;
        case 0x59:
            entry->rotX += 3.0f;
            if (entry->rotX >= 90.0f) {
                entry->state = -1;
                entry->pos = vec3_muj_00013284;
            }
            break;
        case 0x5A:
            entry->pos.x = 0.0f;
            entry->pos.y = -1000.0f;
            entry->pos.z = 0.0f;
            entry->state = -1;
            break;
        default:
            break;
        }
        PSMTXIdentity(*pose->matrix);
        PSMTXRotRad(mtx, 'x', deg2rad * entry->rotX);
        PSMTXConcat(*pose->matrix, mtx, *pose->matrix);
        PSMTXRotRad(mtx, 'y', deg2rad * entry->rotY);
        PSMTXConcat(*pose->matrix, mtx, *pose->matrix);
        (*pose->matrix)[0][3] = entry->pos.x;
        (*pose->matrix)[1][3] = entry->pos.y;
        (*pose->matrix)[2][3] = entry->pos.z + k * (f32)i;
    }
}

s32 zakogun_attack2(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    s32 count;

    if (isFirstCall) {
        work_20_bss_6C->landedCount = 0;
        for (i = 0; i < 25; i++) {
            zakogun[i].state = 0x14;
        }
    }
    i = 0;
    count = 0;
    for (; i < 25; i++) {
        if (zakogun[i].state == 0xA) {
            count++;
        }
    }
    return count == 25 ? 2 : 0;
}

s32 zakogun_attack2_yusuri(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    s32 count;

    if (isFirstCall) {
        for (i = 0; i < 25; i++) {
            zakogun[i].state = 0x3C;
        }
    }
    i = 0;
    count = 0;
    for (; i < 25; i++) {
        if (zakogun[i].state == 0xA) {
            count++;
        }
    }
    return count == 25 ? 2 : 0;
}

s32 zakogun_dead2(EventEntry* event, BOOL isFirstCall) {
    s32 i;

    for (i = 0; i < 25; i++) {
        zakogun[i].state = 0x50;
    }
    return 2;
}

s32 zakogun_escape2(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    s32 count;

    if (isFirstCall) {
        for (i = 0; i < 25; i++) {
            zakogun[i].state = 0x5A;
        }
    }
    i = 0;
    count = 0;
    for (; i < 25; i++) {
        if (zakogun[i].state == -1) {
            count++;
        }
    }
    return count == 25 ? 2 : 0;
}

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetDamage, -2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        SET_VALUE(LW(15), 1)
    ELSE
        SET_VALUE(LW(15), 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 27, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 1, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 3, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 5, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 9, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 2, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 4, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 8, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 15, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 20, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 0, LW(14))
    OR(LW(15), LW(14))
    IF_INT_NOT_EQUAL(LW(15), 0)
        CALL(zakogun_damage_pose)
    END_IF
    CALL_CHILD(btldefaultevt_Damage)
    CALL(zakogun_damage_pose_cancel)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 1)
    BEGIN_INLINE_EVENT
        CALL(up_ceil)
    END_INLINE_EVENT
    CALL(zakogun_dead2)
    WAIT_FRAMES(50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_ENEMY_DIE1_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(48)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        LOOP(20)
            CALL(evt_sub_random, 500, LW(3))
            SUBTRACT(LW(3), 250)
            SET_VALUE(LW(0), 0)
            ADD(LW(0), LW(3))
            CALL(evt_sub_random, 30, LW(3))
            SET_VALUE(LW(1), 0)
            ADD(LW(1), LW(3))
            SET_VALUE(LW(2), 75)
            CALL(evt_eff, 0, STRING("kemuri_test"), 4, LW(0), LW(1), LW(2), FLOAT(0.8f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetCoin, -2, LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, -2, LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL(_add_star_point_disp_offset, -2, 0, 0, 0)
    CALL(evt_eff, STRING(""), STRING("star_point"), 0, 0, 0, 0, LW(0), 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_CheckDamageCode, -2, 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_KillUnit, -2, 0)
    ELSE
        CALL(btlevtcmd_KillUnit, -2, 2)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    SET_VALUE(LW(0), 50)
    ADD(LW(0), 50)
    SUBTRACT(LW(0), 1)
    CALL(evt_sub_random, LW(0), LW(1))
    IF_INT_LESS(LW(1), 50)
        CALL_CHILD(fall_attack_event)
        GOTO(99)
    END_IF
    CALL_CHILD(shake_attack_event)
    GOTO(99)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(fall_attack_event)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    SET_VALUE(LW(9), PTR(&weapon_gundan_zako_group2_attack))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(zakogun_set_target_id, LW(3))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, LW(3), -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    BEGIN_SIBLING_EVENT
        CALL_CHILD(hit_check_event)
    END_SIBLING_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_TEAM_3RD_MOVE_CAILING2"), 0)
    CALL(zakogun_attack2)
    DELETE_EVENT(LW(15))
    RETURN
EVT_END()

static EVT_BEGIN(shake_attack_event)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    SET_VALUE(LW(9), PTR(&weapon_gundan_zako_group2_yusuri_attack))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(zakogun_set_target_id, LW(3))
    BEGIN_SIBLING_EVENT
        LOOP(8)
            CALL(shake_ceil)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        SET_FLOAT_VALUE(LW(0), FLOAT(0.5f))
        LOOP(7)
            WAIT_FRAMES(18)
            CALL(evt_btl_camera_shake_h, 0, LW(0), 0, 18, 0)
            CALL(evt_snd_sfxon, STRING("SFX_BOSS_TEAM_3RD_MOVE_CAILING3"), 0)
            ADD_FLOAT(LW(0), FLOAT(0.5f))
        END_LOOP
        WAIT_FRAMES(18)
        CALL(evt_btl_camera_shake_h, 0, LW(0), 0, 100, 0)
    END_SIBLING_EVENT
    CALL(zakogun_attack2_yusuri)
    BEGIN_SIBLING_EVENT
        CALL(evt_eff, STRING(""), STRING("fall"), 2, 0, 0, 20, 0, 250, 0, 1, 60, 0, 0, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(20)
        CALL(evt_eff, STRING(""), STRING("fall"), 2, 100, 0, 20, 0, 250, 0, 1, 60, 0, 0, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(35)
        CALL(evt_eff, STRING(""), STRING("fall"), 2, -35, 0, 20, 0, 250, 0, 1, 60, 0, 0, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(57)
        CALL(evt_eff, STRING(""), STRING("fall"), 2, -100, 0, 20, 0, 250, 0, 1, 60, 0, 0, 0)
        WAIT_FRAMES(20)
        CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
        IF_INT_NOT_EQUAL(LW(3), -1)
            CALL_CHILD(hit_check_event2)
        END_IF
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(82)
        CALL(evt_eff, STRING(""), STRING("fall"), 2, 80, 0, 20, 0, 250, 0, 1, 60, 0, 0, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(90)
        CALL(evt_eff, STRING(""), STRING("fall"), 2, 40, 0, 20, 0, 250, 0, 1, 60, 0, 0, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(105)
        CALL(evt_eff, STRING(""), STRING("fall"), 2, 130, 0, 20, 0, 250, 0, 1, 60, 0, 0, 0)
    END_SIBLING_EVENT
    WAIT_FRAMES(115)
    CALL(evt_eff, STRING(""), STRING("fall"), 2, -145, 0, 20, 0, 250, 0, 1, 60, 0, 0, 0)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        CALL_CHILD(hit_check_event2)
    END_IF
    RETURN
EVT_END()

__declspec(section ".rodata") Vec vec3_muj_0001323c = { 0.0f, 250.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_muj_00013248 = { 0.0f, 130.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_muj_00013254 = { 0.0f, 130.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_muj_00013260 = { 0.0f, 250.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_muj_0001326c = { 0.0f, 130.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_muj_00013278 = { 0.0f, 135.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_muj_00013284 = { 0.0f, -1000.0f, 0.0f };

static EVT_BEGIN(hit_check_event)
    WAIT_FRAMES(10)
    LOOP(4)
        WAIT_FRAMES(20)
        SET_VALUE(LW(15), 0)
        CALL_CHILD(hit_check_event_sub)
    END_LOOP
    LOOP(0)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(LW(15), 1)
    CALL_CHILD(hit_check_event_sub)
    RETURN
EVT_END()

EVT_BEGIN(hit_check_event2)
    SET_VALUE(LW(15), 1)
    CALL_CHILD(hit_check_event_sub)
    RETURN
EVT_END()

EVT_BEGIN(hit_check_event_sub)
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_group2_attack))
        IF_INT_EQUAL(LW(15), 1)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
            IF_INT_EQUAL(LW(0), 1)
                GOTO(92)
            END_IF
        END_IF
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(98)
    LABEL(91)
    LABEL(92)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    IF_INT_EQUAL(LW(15), 1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        CALL(evt_btl_camera_set_mode, 0, 0)
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(&weapon_gundan_zako_group2_attack))
        CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    END_IF
    GOTO(98)
    LABEL(98)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()


// clang-format on
