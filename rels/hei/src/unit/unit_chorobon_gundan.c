#include "unit_chorobon_gundan.h"
#include "unit_gold_chorobon.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_npc.h"
#include "forcedata.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"
#include "driver/dispdrv.h"
#include "driver/extdrv.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "system.h"
#include <stdlib.h>

#pragma pool_data off

#pragma force_active on

// The vector pool is read-only (it lands in .rodata, after the data-initializer strings); defined below.
extern Vec vec3_0;
extern Vec vec3_1;
extern Vec vec3_2;
extern Vec vec3_3;
extern Vec vec3_4;
extern Vec vec3_5;
extern Vec vec3_6;
extern Vec vec3_7;

static BattleVulnerableStatus regist;
static BattleDataEntry data_table[2];
static BattleUnitKindPart parts[1];
static s32 init_event[26];
static s32 damage_event[59];
static s32 attack_event[217];
static s32 wait_event[6];

// clang-format off
extend_data(72,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(73,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_chorobon_gundan = {
    UNIT_FUZZY_HORDE,               // mUnitType
    "btl_un_chorobon_gundan",       // name
    20,                             // maxHp
    0,                              // maxFp
    1,                              // mDangerHP
    1,                              // mPerilHP
    19,                             // mLevel
    0,                              // mBonusExp
    5,                              // bonusCoinRolls
    100,                            // bonusCoinRate
    0,                              // baseCoin
    50,                             // field_0x13
    5,                              // field_0x14
    260,                            // width
    50,                             // height
    { 0, 50 },                      // statusIconOffset
    { 0.0f, 0.0f, 0.0f },           // centerOffset
    { 0, 0 },                       // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },           // talkTogeBaseOffset
    { 130.0f, 0.0f, -10.0f },       // field_0x3C
    { 0.0f, 0.0f, 0.0f },           // burnFlameOffset
    0.0f,                           // field_0x54
    0.0f,                           // field_0x58
    { 130.0f, 0.0f, 0.0f },         // bintaHitOffset
    { 130.0f, 32.5f, 0.0f },        // kissHitOffset
    { 0.0f, 25.0f, 0.0f },          // cutBaseOffset
    260.0f,                         // cutWidth
    50.0f,                          // cutHeight
    0,                              // turnOrder
    0,                              // turnOrderVariance
    -1,                             // swallowRate
    0,                              // swallowAttribute
    100,                            // field_0x8C
    20,                             // itemStealParam
    { 0.0f, 0.0f, 0.0f },           // starPointDispOffset
    "SFX_BOSS_TEAM_CHORO_DAMAGED1", // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",         // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",          // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",     // electricDamageSfxName
    0x00240000,                     // attributeFlags
    &regist,                        // vulnStatus
    1,                              // numParts
    parts,                          // mParts
    init_event,                     // initEvtScript
    data_table,                     // dataTable
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

static BattleDispPose pose_table[15] = {
    { 0x01, "CBN_N_1" },
    { 0x02, "CBN_Y_1" },
    { 0x09, "CBN_Y_1" },
    { 0x05, "CBN_K_1" },
    { 0x04, "CBN_X_1" },
    { 0x03, "CBN_X_1" },
    { 0x1C, "CBN_S_1" },
    { 0x1D, "CBN_Q_1" },
    { 0x1E, "CBN_Q_1" },
    { 0x1F, "CBN_S_1" },
    { 0x27, "CBN_D_1" },
    { 0x32, "CBN_A_1" },
    { 0x2A, "CBN_R_1" },
    { 0x28, "CBN_W_1" },
    { 0x45, "CBN_S_1" },
};

static BattleDataEntry data_table[2] = {
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                        // partNum
        "btl_un_chorobon_gundan", // partName
        "c_chorobon",             // modelName
        { 0.0f, 0.0f, 0.0f },     // positionOffset
        { 0.0f, 25.0f, 0.0f },    // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },    // hitCursorBaseOffset
        20,                       // field_0x30
        30,                       // field_0x32
        255,                      // baseAlpha
        &defence,                 // defense
        &defence_attr,            // defenseAttr
        0x03000009,               // attributes
        0,                        // counterAttributes
        pose_table,               // poseTable
    },
};

BattleWeapon weapon_chorobon_gundan_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    2,                                        // superguardType
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
    0x20000002,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000007,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000004,                               // targetWeightingFlags
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

EVT_BEGIN(unk_evt_hei_00019e30)
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    ADD(LW(2), 10)
    CALL(btlevtcmd_GetCoin, LW(10), LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, LW(10), LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL(btlevtcmd_GetPos, LW(10), LW(3), LW(1), LW(2))
    ADD(LW(2), 10)
    ADD(LW(1), 30)
    CALL(_add_star_point_disp_offset, LW(10), LW(3), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("star_point"), 0, LW(3), LW(1), LW(2), LW(0), 0, 0, 0, 0, 0, 0, 0)
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        LOOP(LW(0))
        END_LOOP
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

BattleUnit* chorogun_get_ptr(void) {
    s32 i;
    BattleWork* wp = _battleWorkPointer;

    for (i = 0; i < 0x40; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);

        if (unit != NULL && unit->initialKind == UNIT_FUZZY_HORDE) {
            return unit;
        }
    }
    return NULL;
}

void chorogun_ext_init(void) {
    s32 i;
    ExtPose* pose = extGetPosePtr();

    chorogun = BattleAlloc(0xBB80);
    for (i = 0; i < extGetPoseNum(); i++, pose++) {
        pose->flags = 1;
        PSMTXIdentity(*pose->matrix);
        (*pose->matrix)[0][3] = 0.0f;
        (*pose->matrix)[1][3] = -1000.0f;
        (*pose->matrix)[2][3] = 0.0f;
        pose->field_0x4 = 90.0f;
        chorogun[i].flags = 0;
        chorogun[i].state = 0;
        chorogun[i].position = vec3_0;
        if (i == 0) {
            chorogun[i].timer = 0x16;
        } else {
            chorogun[i].timer = irand(0x2D);
        }
        chorogun[i].target.x = (5.0f * (i % 50) + -50.0f) + ((10.0f * (f32)rand()) / 32767.0f - 5.0f);
        chorogun[i].target.y = 0.0f;
        chorogun[i].target.z = (7.0f * (i / 50) + -70.0f) + ((5.0f * (f32)rand()) / 32767.0f - 2.5f);
    }
    for (i = 0; i < 500; i++) {
        s32 a = irand(extGetPoseNum());
        s32 b = irand(extGetPoseNum());
        f32 tx = chorogun[a].target.x;
        f32 ty = chorogun[a].target.y;
        f32 tz = chorogun[a].target.z;

        chorogun[a].target.x = chorogun[b].target.x;
        chorogun[a].target.y = chorogun[b].target.y;
        chorogun[a].target.z = chorogun[b].target.z;
        chorogun[b].target.x = tx;
        chorogun[b].target.y = ty;
        chorogun[b].target.z = tz;
    }
}

static EVT_BEGIN(damage_event)
    CALL(btlevtcmd_GetDamage, -2, LW(15))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(0), LW(15))
            BREAK_LOOP
        END_IF
        CALL(chorogun_get_alive_num, LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), 50)
            CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
        ELSE
            CALL(chorogun_escape)
        END_IF
        ADD(LW(0), 1)
    END_LOOP
    CALL(btlevtcmd_CheckDamageCode, -2, 256, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(chorogun_get_alive_num, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 500)
        CALL_CHILD(escape_chorogun_event)
        GOTO(99)
    END_IF
    LABEL(99)
    RETURN
EVT_END()

// NONMATCHING (97.84%): full state machine matches structurally; register allocation and pool slots differ.
void chorogun_ext_main(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit = chorogun_get_ptr();
    ExtPose* pose = extGetPosePtr();
    s32 i;
    s32 alive = 0;

    for (i = 0; i < extGetPoseNum(); i++) {
        if (chorogun[i].flags & 1) {
            alive++;
        }
    }
    alive = 1000 - alive;
    for (i = 0; i < extGetPoseNum(); i++, pose++) {
        ChorogunEntry* entry = &chorogun[i];

        pose->field_0x10 = (gp->retraceCount + i * i) % 20;
        (*pose->matrix)[0][3] += entry->position.x;
        (*pose->matrix)[1][3] += entry->position.y;
        (*pose->matrix)[2][3] += entry->position.z;
        switch (entry->state) {
            case 0:
                break;
            case 1:
                entry->timer--;
                if (entry->timer <= 0) {
                    (*pose->matrix)[0][3] = 500.0f * (f32)rand() / 32767.0f - 250.0f;
                    (*pose->matrix)[1][3] = 100.0f * (f32)rand() / 32767.0f + 250.0f;
                    (*pose->matrix)[2][3] = 800.0f * (f32)rand() / 32767.0f - 100.0f;
                    entry->position.x = 0.0f;
                    entry->position.y = 0.0f;
                    entry->position.z = 0.0f;
                    entry->speed = 3.0f;
                    entry->state = 2;
                    entry->timer = 0;
                    if (i == 0) {
                        Vec pos = vec3_1;

                        BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_CHORO_FALL2", 0x7F, 0, &pos);
                    }
                }
                break;
            case 2:
                if ((*pose->matrix)[1][3] <= 0.0f) {
                    if (i % 100 == 0) {
                        Vec pos = vec3_2;

                        pos.x = (*pose->matrix)[0][3];
                        pos.y = (*pose->matrix)[1][3];
                        pos.z = (*pose->matrix)[2][3];
                        BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_CHORO_FALL1", 0x7F, 0, &pos);
                    }
                    entry->state = 0xA;
                } else {
                    entry->position.y += -0.25f;
                }
                break;
            case 0xA:
                if ((*pose->matrix)[1][3] > 0.0f) {
                    entry->position.y += -0.5f;
                } else {
                    f32 dz = entry->target.z - (*pose->matrix)[2][3];
                    f32 dx = entry->target.x - (*pose->matrix)[0][3];
                    f32 dist = (f32)sqrt(dz * dz + dx * dx);

                    if (i % 100 == 0) {
                        Vec pos = vec3_3;

                        pos.x = (*pose->matrix)[0][3];
                        pos.y = (*pose->matrix)[1][3];
                        pos.z = (*pose->matrix)[2][3];
                        BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_CHORO_TOGETHER1", 0x7F, 0, &pos);
                    }
                    if (dist <= 5.0f) {
                        entry->state = 0x14;
                    } else {
                        if (dist <= 20.0f) {
                            entry->speed *= 0.9f;
                        }
                        entry->position.x = entry->speed * (dx / dist);
                        entry->position.y = entry->speed;
                        entry->position.z = entry->speed * (dz / dist);
                        (*pose->matrix)[1][3] = 0.0f;
                    }
                }
                break;
            case 0x14:
                (*pose->matrix)[1][3] = 0.0f;
                entry->position.x = 0.0f;
                entry->position.y = entry->speed;
                entry->position.z = 0.0f;
                entry->state = 0x15;
                break;
            case 0x15: {
                f32 tx = intplGetValue(0, alive, 1000, entry->target.x, unit->mHomePosition.x) +
                         (unit->mPosition.x - unit->mHomePosition.x);
                f32 tz = intplGetValue(0, alive, 1000, entry->target.z, unit->mHomePosition.z) +
                         (unit->mPosition.z - unit->mHomePosition.z);

                if ((*pose->matrix)[1][3] <= unit->mPosition.y) {
                    f32 dz = tz - (*pose->matrix)[2][3];
                    f32 dx = tx - (*pose->matrix)[0][3];
                    f32 dist = (f32)sqrt(dz * dz + dx * dx);

                    if (i % 100 == 0) {
                        Vec pos = vec3_4;

                        pos.x = (*pose->matrix)[0][3];
                        pos.y = (*pose->matrix)[1][3];
                        pos.z = (*pose->matrix)[2][3];
                        BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_CHORO_WAIT1", 0x60, 0, &pos);
                    }
                    entry->speed = 3.0f + (0.5f * (f32)rand()) / 32767.0f;
                    entry->position.x = entry->speed * (dx / dist);
                    entry->position.y = entry->speed;
                    entry->position.z = entry->speed * (dz / dist);
                    (*pose->matrix)[1][3] = unit->mPosition.y;
                } else {
                    entry->position.y += -0.5f;
                }
                break;
            }
            case 0x1E: {
                BattleUnit* target;
                f32 x;
                f32 y;
                f32 z;

                entry->state = 0x1F;
                entry->timer = 0;
                target = BattleGetUnitPtr(wp, chorogun_targetId);
                BtlUnit_GetPos(target, &x, &y, &z);
                x += 50.0f * (f32)rand() / 32767.0f - 25.0f;
                entry->targetX = x;
                z += 50.0f * (f32)rand() / 32767.0f - 25.0f;
                entry->targetZ = z;
                // fallthrough
            }
            case 0x1F:
                if ((*pose->matrix)[1][3] <= 0.0f) {
                    f32 dz = entry->targetZ - (*pose->matrix)[2][3];
                    f32 dx = entry->targetX - (*pose->matrix)[0][3];
                    f32 dist = (f32)sqrt(dz * dz + dx * dx);

                    if (i % 100 == 0) {
                        Vec pos = vec3_5;

                        pos.x = (*pose->matrix)[0][3];
                        pos.y = (*pose->matrix)[1][3];
                        pos.z = (*pose->matrix)[2][3];
                        BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_CHORO_MOVE1", 0x60, 0, &pos);
                    }
                    if (dist <= 30.0f || entry->timer >= 0xF0) {
                        entry->state = 0x28;
                    } else {
                        entry->position.x = entry->speed * (dx / dist);
                        entry->position.y = entry->speed;
                        entry->position.z = entry->speed * (dz / dist);
                        (*pose->matrix)[1][3] = 0.0f;
                        entry->timer++;
                    }
                } else {
                    entry->position.y += -0.5f;
                }
                break;
            case 0x28:
                if ((*pose->matrix)[1][3] <= 0.0f) {
                    (*pose->matrix)[1][3] = 0.0f;
                    entry->position.x = 0.0f;
                    entry->position.y = entry->speed;
                    entry->position.z = 0.0f;
                } else {
                    entry->position.y += -0.5f;
                }
                break;
            case 0x2D:
                entry->timer--;
                if (entry->timer <= 0) {
                    entry->timer = 0;
                    entry->position.y = 0.0f;
                } else {
                    entry->position.y += -0.5f;
                }
                break;
            case 0x2E:
                entry->state = 0x2F;
                entry->timer = 0;
                entry->position.y = 6.0f;
                if ((*pose->matrix)[1][3] <= 0.0f) {
                    (*pose->matrix)[1][3] = 0.1f;
                }
                // fallthrough
            case 0x2F:
                if ((*pose->matrix)[1][3] <= 0.0f) {
                    entry->state = 0x28;
                    (*pose->matrix)[1][3] = 0.0f;
                    entry->position.x = 0.0f;
                    entry->position.y = entry->speed;
                    entry->position.z = 0.0f;
                } else {
                    entry->position.y += -0.5f;
                }
                break;
            case 0x30:
                entry->state = 0x31;
                entry->timer = 0;
                entry->position.x = -4.0f;
                entry->position.y = 10.0f;
                if ((*pose->matrix)[1][3] <= 0.0f) {
                    (*pose->matrix)[1][3] = 0.1f;
                }
                // fallthrough
            case 0x31:
                if ((*pose->matrix)[1][3] <= 0.0f) {
                    entry->state = 0x28;
                    (*pose->matrix)[1][3] = 0.0f;
                    entry->position.x = 0.0f;
                    entry->position.y = entry->speed;
                    entry->position.z = 0.0f;
                } else {
                    entry->position.y += -0.5f;
                }
                break;
            case 0x32:
                entry->state = 0x14;
                break;
            case 0x3C:
                entry->state = 0x3D;
                break;
            case 0x3D:
                if ((*pose->matrix)[1][3] <= 0.0f) {
                    if (i % 100 == 0) {
                        Vec pos = vec3_6;

                        pos.x = (*pose->matrix)[0][3];
                        pos.y = (*pose->matrix)[1][3];
                        pos.z = (*pose->matrix)[2][3];
                        BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_CHORO_MOVE1", 0x60, 0, &pos);
                    }
                    (*pose->matrix)[1][3] = 0.0f;
                    entry->position.x = 5.0f;
                    entry->position.y = entry->speed + (f32)irand(5);
                    entry->position.z = 0.0f;
                } else {
                    entry->position.y += -0.5f;
                }
                if ((*pose->matrix)[0][3] >= 250.0f) {
                    entry->state = 0x63;
                }
                break;
            case 0x63:
                (*pose->matrix)[0][3] = 0.0f;
                (*pose->matrix)[1][3] = -1000.0f;
                (*pose->matrix)[2][3] = 0.0f;
                break;
        }
    }
    dispEntry(4, 0.0f, 1, (DispCallback)chorogun_ext_disp, NULL);
}

void chorogun_ext_disp(void) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

USER_FUNC(chorogun_get_alive_num) {
    s32* args = event->args;
    s32 i;
    s32 count = 0;

    for (i = 0; i < extGetPoseNum(); i++) {
        if (chorogun[i].flags & 1) {
            count++;
        }
    }
    evtSetValue(event, args[0], count);
    return EVT_RETURN_DONE;
}

USER_FUNC(chorogun_set_target_id) {
    chorogun_targetId = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

// NONMATCHING (94.9%): `li r0,0x2` is scheduled one slot later than the target.
USER_FUNC(chorogun_appear) {
    s32 i;
    s32 count;

    if (isFirstCall) {
        for (i = 0; i < extGetPoseNum(); i++) {
            ChorogunEntry* entry = &chorogun[i];

            entry->flags |= 1;
            entry->state = 1;
        }
    }
    count = 0;
    for (i = 0; i < extGetPoseNum(); i++) {
        if (chorogun[i].state == 0x15) {
            count++;
        }
    }
    if (count == extGetPoseNum()) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(chorogun_approach_target) {
    s32 i;

    if (isFirstCall) {
        for (i = 0; i < extGetPoseNum(); i++) {
            if (chorogun[i].flags & 1) {
                chorogun[i].state = 0x1E;
            }
        }
    }
    for (i = 0; i < extGetPoseNum(); i++) {
        if ((chorogun[i].flags & 1) && chorogun[i].state != 0x28) {
            return EVT_RETURN_BLOCK;
        }
    }
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_chorobon_gundan_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_chorobon_gundan_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_chorobon_gundan_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_chorobon_gundan_attack))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, LW(3), -1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(chorogun_set_target_id, LW(3))
    CALL(chorogun_approach_target)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_chorobon_gundan_attack), 256, LW(5))
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
    WAIT_MS(1000)
    GOTO(98)
    LABEL(91)
    CALL(chorogun_get_alive_num, LW(15))
    ADD(LW(15), 100)
    DIVIDE(LW(15), 200)
    IF_INT_LESS_OR_EQUAL(LW(15), 0)
        SET_VALUE(LW(15), 1)
    END_IF
    SET_VALUE(LW(14), 1)
    LOOP(0)
        IF_INT_GREATER(LW(14), LW(15))
            BREAK_LOOP
        END_IF
        CALL(chorogun_dive_target, 30, LW(6))
        WAIT_FRAMES(30)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_chorobon_gundan_attack))
        IF_INT_EQUAL(LW(14), LW(15))
            CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_chorobon_gundan_attack), 256, LW(5))
        ELSE
            CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_chorobon_gundan_attack), 0, LW(5))
        END_IF
        IF_INT_EQUAL(LW(5), 21)
            CALL(chorogun_counterd, LW(6))
        ELSE
            CALL(chorogun_bound_target, LW(6))
        END_IF
        ADD(LW(14), 1)
        WAIT_FRAMES(30)
    END_LOOP
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(chorogun_after_attack)
    WAIT_MS(1000)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static ChorogunExtEntry chorogun_ext_entry_data[21] = {
    { "c_chorobon", "CBN_W_1", 0.0f },
    { "c_chorobon", "CBN_W_1", 2.0f },
    { "c_chorobon", "CBN_W_1", 4.0f },
    { "c_chorobon", "CBN_W_1", 6.0f },
    { "c_chorobon", "CBN_W_1", 8.0f },
    { "c_chorobon", "CBN_W_1", 10.0f },
    { "c_chorobon", "CBN_W_1", 12.0f },
    { "c_chorobon", "CBN_W_1", 14.0f },
    { "c_chorobon", "CBN_W_1", 16.0f },
    { "c_chorobon", "CBN_W_1", 18.0f },
    { "c_chorobon", "CBN_W_1", 20.0f },
    { "c_chorobon", "CBN_W_1", 22.0f },
    { "c_chorobon", "CBN_W_1", 24.0f },
    { "c_chorobon", "CBN_W_1", 26.0f },
    { "c_chorobon", "CBN_W_1", 28.0f },
    { "c_chorobon", "CBN_W_1", 30.0f },
    { "c_chorobon", "CBN_W_1", 32.0f },
    { "c_chorobon", "CBN_W_1", 34.0f },
    { "c_chorobon", "CBN_W_1", 36.0f },
    { "c_chorobon", "CBN_W_1", 38.0f },
    { NULL, NULL, 0.0f },
};


// NONMATCHING (99.84%): callee-saved register assignment and the read-only float/vec pool differ.
USER_FUNC(chorogun_dive_target) {
    s32 ids[1000];
    s32 i;
    s32 count = 0;
    s32* args = event->args;
    ExtPose* pose;
    s32 idx;
    ExtPose* sel;
    ChorogunEntry* entry;
    BattleUnit* unit;
    s32 delay;
    f32 x;
    f32 tz;
    f32 delayF;
    f32 px;
    f32 pz;
    f32 d2;
    f32 y;
    f32 t;
    f32 z;

    delay = evtGetValue(event, args[0]);
    delayF = (f32)delay;
    pose = extGetPosePtr();
    for (i = 0; i < extGetPoseNum(); i++) {
        if (chorogun[i].flags & 1) {
            ids[count] = i;
            count++;
        }
    }
    idx = ids[irand(count)];
    entry = &chorogun[idx];
    entry->state = 0x2D;
    entry->timer = delay;
    unit = BattleGetUnitPtr(_battleWorkPointer, chorogun_targetId);
    BtlUnit_GetPos(unit, &x, &y, &z);
    sel = &pose[idx];
    px = (*sel->matrix)[0][3];
    pz = (*sel->matrix)[2][3];
    t = y - pz;
    d2 = 2.0f * delayF;
    tz = z - pz;
    entry->position.x = (x - px) / delayF;
    entry->position.y = (2.0f * t - (-0.5f * delayF * delayF)) / d2;
    entry->position.z = tz / delayF;
    evtSetValue(event, args[1], idx);
    unit = chorogun_get_ptr();
    {
        Vec pos = vec3_7;

        pos.x = (*sel->matrix)[0][3];
        pos.y = (*sel->matrix)[1][3];
        pos.z = (*sel->matrix)[2][3];
        BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_CHORO_ATTACK1", 0x80, 0, &pos);
    }
    return EVT_RETURN_DONE;
}


USER_FUNC(chorogun_bound_target) {
    s32 id = evtGetValue(event, event->args[0]);

    chorogun[id].state = 0x2E;
    return EVT_RETURN_DONE;
}

EVT_BEGIN(chorogun_entry)
    CALL(evt_npc_entry, STRING("ext_chorobon"), STRING("c_chorobon"))
    CALL(evt_npc_set_position, STRING("ext_chorobon"), 0, -1000, 0)
    CALL(evt_ext_entry, 1000, PTR(chorogun_ext_entry_data), PTR(chorogun_ext_init), PTR(chorogun_ext_main), 0)
    RETURN
EVT_END()

ChorogunEntry* chorogun;

s32 chorogun_targetId;

__declspec(section ".rodata") Vec vec3_0 = { 0.0f, 0.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_1 = { 0.0f, 200.0f, 200.0f };
__declspec(section ".rodata") Vec vec3_2 = { 0.0f, 0.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_3 = { 0.0f, 0.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_4 = { 0.0f, 0.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_5 = { 0.0f, 0.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_6 = { 0.0f, 0.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_7 = { 0.0f, 0.0f, 0.0f };

USER_FUNC(chorogun_counterd) {
    s32 id = evtGetValue(event, event->args[0]);

    chorogun[id].state = 0x30;
    return EVT_RETURN_DONE;
}

USER_FUNC(chorogun_after_attack) {
    s32 i;

    if (isFirstCall) {
        for (i = 0; i < extGetPoseNum(); i++) {
            if (chorogun[i].flags & 1) {
                chorogun[i].state = 0x32;
            }
        }
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (89.35%): callee-saved register assignment and float_2500 pool slot differ.
USER_FUNC(chorogun_escape) {
    BattleUnit* unit;
    s32 j;
    s32 count;
    ExtPose* pose;
    BattleWork* wp;
    s32 i;
    s32 num;
    f32 dists[1000];
    s32 indices[1000];

    wp = _battleWorkPointer;
    for (i = 0; i < 0x40; i++) {
        BattleUnit* u = BattleGetUnitPtr(wp, i);

        if (u != NULL && u->initialKind == UNIT_FUZZY_HORDE) {
            unit = u;
            goto found;
        }
    }
    unit = NULL;
found:
    count = 0;
    pose = extGetPosePtr();
    for (j = 0; j < extGetPoseNum(); j++, pose++) {
        f32 dx;
        f32 dz;
        f32 dist2;

        if (!(chorogun[j].flags & 1)) {
            continue;
        }
        dx = unit->mHomePosition.x - (*pose->matrix)[0][3];
        dz = unit->mHomePosition.z - (*pose->matrix)[2][3];
        dist2 = dx * dx + dz * dz;
        if (count < 50) {
            goto count_store;
        }
        if (!(dist2 <= 2500.0f)) {
            continue;
        }
        dists[count] = dist2;
        indices[count] = j;
        count++;
        continue;
count_store:
        dists[count] = dist2;
        indices[count] = j;
        count++;
    }
    for (i = 0; i < count - 1; i++) {
        for (j = 1; j < count; j++) {
            if (dists[i] > dists[j]) {
                f32 fd = dists[i];
                s32 id = indices[i];

                dists[i] = dists[j];
                indices[i] = indices[j];
                dists[j] = fd;
                indices[j] = id;
            }
        }
    }
    num = count < 50 ? count : 50;
    for (i = 0; i < num; i++) {
        ChorogunEntry* entry = &chorogun[indices[i]];

        entry->flags &= ~1;
        entry->state = 0x3C;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(chorogun_escape_all) {
    s32 i;

    for (i = 0; i < extGetPoseNum(); i++) {
        ChorogunEntry* entry = &chorogun[i];

        if (entry->flags & 1) {
            entry->flags &= ~1;
            entry->state = 0x3C;
        }
    }
    return EVT_RETURN_DONE;
}
// clang-format on
