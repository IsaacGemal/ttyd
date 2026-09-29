#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_status_effect.h"
#include "battle/battle_sub.h"
#include "battle/battle_weapon_power.h"
#include "eff/eff_mahorn2.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "party/party_motion.h"
#include "system.h"
#include <string.h>

#pragma pool_data off

#pragma force_active on
static USER_FUNC(eff_magic_get_posX);
static USER_FUNC(eff_magic);
static USER_FUNC(mahorn_copy_status);
static USER_FUNC(mahorn_shuffle);
static USER_FUNC(mahorn_get_bunsin);
static USER_FUNC(set_hand_dest);
static USER_FUNC(hand_ctrl);
static USER_FUNC(hand_end);
static USER_FUNC(hand_init);
static USER_FUNC(mahorn_get_live_num);
static u32 regist[6];
static u32 parts[95];
static u32 pose_table[34];
static u32 data_table[10];
static u32 data_table_bunsin[6];
static s32 init_event[88];
static s32 hand_ctrl_event[23];
static s32 damage_event[66];
static s32 mahorn_subset_bunsin_all_kill[68];
static s32 mahorn_subset_dead_event[203];
static s32 mahorn_subset_blow_dead_event[78];
static s32 avoid_miss_event[16];
static s32 bunsin_kill_event[37];
static s32 attack_punch_event[429];
static s32 attack_enemy_common_event_sub1[90];
static s32 attack_enemy_common_event_sub2[231];
static s32 attack_enemy_common_event_sub_no_hit[65];
static s32 attack_enemy_common_event_sub_hit[50];
static s32 attack_magic_event[54];
static s32 all_attack_magic_event[41];
static s32 dekadeka_magic_event[49];
static s32 katikati_magic_event[49];
static s32 trans_magic_event[49];
static s32 yokeyoke_magic_event[49];
static s32 recover_magic_event[70];
static s32 bunsin_event[152];
static s32 spawn_event[76];
static s32 wait_event[386];

// clang-format off
extend_data(176,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(177,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_mahorn_custom[49] = {
    0x000000A9, (u32)"btl_un_mahorn_custom", 0x000C0000, 0x01012402,
    0x03460232, 0x270F0030, 0x002C0000, 0x002C0000,
    0x00000000, 0x41B00000, 0x00000000, 0x0000FFF7,
    0x00000000, 0x00000000, 0x00000000, 0x41C00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41C00000,
    0x00000000, 0x00000000, 0x41800000, 0x41E4CCCD,
    0x41200000, 0x00000000, 0x41B00000, 0x00000000,
    0x42400000, 0x42300000, 0x00000002, 0x00140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_MAHO_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000004,
    (u32)&regist, 0x05000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
u32 unit_mahorn_custom_bunsin[49] = {
    0x000000AA, (u32)"btl_un_mahorn_custom_bunsin", 0x000C0000, 0x01010102,
    0x003C0032, 0x270F0030, 0x002C0000, 0x002C0000,
    0x00000000, 0x41B00000, 0x00000000, 0x0000FFF7,
    0x00000000, 0x00000000, 0x00000000, 0x41C00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41C00000,
    0x00000000, 0x00000000, 0x41800000, 0x41E4CCCD,
    0x41200000, 0x00000000, 0x41B00000, 0x00000000,
    0x42400000, 0x42300000, 0x00000002, 0x00140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_MAHO_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00400004,
    (u32)&regist, 0x05000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table_bunsin,
};

// TODO: type
static u32 defence[2] = {
    0x05050505, 0x05000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x001E5064, 0x14646428, 0x644B644B, 0x6450140A,
    0x4B641464, 0x64140000,
};

// TODO: type
static u32 regist_bunsin[6] = {
    0x00000064, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[95] = {
    0x00000001, (u32)"btl_un_mahorn_custom", (u32)"c_maho_w", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41F00000,
    0x00000000, 0x00000000, 0x42200000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table, 0x00000002,
    (u32)"btl_un_mahorn_custom", (u32)"c_maho_w", 0x00000000, 0x00000000,
    0x00000000, 0x40A00000, 0x41F00000, 0x00000000,
    0x00000000, 0x420C0000, 0x00000000, 0x0014001E,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x02010009,
    0x00000000, (u32)&pose_table, 0x00000003, (u32)"btl_un_mahorn_custom",
    (u32)"c_maho_w", 0x00000000, 0x00000000, 0x00000000,
    0x40A00000, 0x41F00000, 0x00000000, 0x00000000,
    0x420C0000, 0x00000000, 0x0014001E, 0x00FF0000,
    (u32)&defence, (u32)&defence_attr, 0x02010009, 0x00000000,
    (u32)&pose_table, 0x00000004, (u32)"btl_un_mahorn_custom", (u32)"c_maho_w",
    0x00000000, 0x00000000, 0x00000000, 0x40A00000,
    0x41F00000, 0x00000000, 0x00000000, 0x420C0000,
    0x00000000, 0x0014001E, 0x00FF0000, (u32)&defence,
    (u32)&defence_attr, 0x02010009, 0x00000000, (u32)&pose_table,
    0x00000005, (u32)"btl_un_mahorn_custom", (u32)"c_maho_w", 0x00000000,
    0x00000000, 0x00000000, 0x40A00000, 0x41F00000,
    0x00000000, 0x00000000, 0x420C0000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x02010009, 0x00000000, (u32)&pose_table,
};

// TODO: type
static u32 weapon_mahorn_attack_magic[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000008, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x00000302,
    0x00000000, 0x000000C7, 0x000007FF, 0x80002000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_mahorn_all_attack_magic[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000008, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x04000302,
    0x00000000, 0x00000007, 0x000007FF, 0x80002000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_mahorn_attack_punch[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000008, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21002000, 0x00000302,
    0x00000000, 0x00000087, 0x00000002, 0x00000004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_mahorn_trans_magic[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01004020, 0x20000000, 0x00000302,
    0x00000000, 0x00000108, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64030000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_mahorn_yokeyoke_magic[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01004020, 0x20000000, 0x00000302,
    0x00000000, 0x00000108, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00640300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_mahorn_dekadeka_magic[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01004020, 0x20000000, 0x00000302,
    0x00000000, 0x00000108, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x03020000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_mahorn_katikati_magic[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01004020, 0x20000000, 0x00000302,
    0x00000000, 0x00000108, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00640303, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_mahorn_recover_magic[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00001500,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 entry_mahorn[12] = {
    (u32)&unit_mahorn_custom_bunsin, 0x01000000, 0x04000004, 0xC47A0000,
    0xC47A0000, 0xC47A0000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// Functions are emitted in reverse source order (deferred inlining); listed here in the
// reverse of their .text order so the object links in the original order.

static USER_FUNC(mahorn_get_live_num) {
    s32 i;
    s32* args = event->args;
    s32 count = 0;
    BattleWork* work = _battleWorkPointer;

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = work->units[i];
        if (unit != NULL && unit->mAlliance == 1 && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
            count++;
        }
    }
    evtSetValue(event, args[0], count);
    return EVT_RETURN_DONE;
}

static USER_FUNC(hand_init) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    f32 x, y, z;
    s32 partNum;
    s32 index;
    s32 id;

    id = evtGetValue(event, *args++);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
    BtlUnit_GetPos(unit, &x, &y, &z);
    index = 0;
    for (partNum = 2; partNum <= 5; partNum++, index++) {
        Vec* dest;
        BtlUnit_SetAnim(BtlUnit_GetPartsPtr(unit, partNum), "MAH_Z_2");
        dest = (Vec*)__memAlloc(HEAP_BATTLE, sizeof(Vec));
        unit->work[index] = (s32)dest;
        dest->x = 0.0f;
        dest->y = 0.0f;
        dest->z = 0.0f;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(hand_end) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 i;
    s32 index;
    s32 id;

    id = evtGetValue(event, *args++);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
    index = 0;
    for (i = 2; i <= 5; i++, index++) {
        void* ptr = (void*)unit->work[index];
        if (ptr != NULL) {
            BattleFree(ptr);
            unit->work[index] = 0;
        }
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(hand_ctrl) {
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s8 turns;
    s8 strength;
    s32 partNum;
    s32 index;
    s32 id;

    id = evtGetValue(event, *args++);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
    if (unit == NULL)
        return EVT_RETURN_BLOCK;
    BtlUnit_GetStatus(unit, STATUS_STOP, &turns, &strength);
    if (turns > 0)
        return EVT_RETURN_DONE;
    BtlUnit_GetStatus(unit, STATUS_FREEZE, &turns, &strength);
    if (turns > 0)
        return EVT_RETURN_DONE;
    BtlUnit_GetStatus(unit, STATUS_SLEEP, &turns, &strength);
    if (turns > 0)
        return EVT_RETURN_DONE;
    index = 0;
    for (partNum = 2; partNum <= 5; partNum++, index++) {
        BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, partNum);
        Vec* dest = (Vec*)unit->work[index];
        f32 px = part->mPositionOffset.x * part->mBaseScale.x;
        f32 py = part->mPositionOffset.y * part->mBaseScale.y;
        f32 pz = part->mPositionOffset.z * part->mBaseScale.z;
        f32 dx = dest->x - px;
        f32 dy = dest->y - py;
        f32 dz = dest->z - pz;
        px = px + dx / 10.0f;
        py = py + dy / 10.0f;
        pz = pz + dz / 10.0f;
        part->mPositionOffset.x = px;
        part->mPositionOffset.y = py;
        part->mPositionOffset.z = pz;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(set_hand_dest) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32* args = event->args;
    s32 unitId;
    s32 partNum;
    s32 index;
    f32 x, y, z;
    Vec* dest;

    unitId = evtGetValue(event, args[0]);
    partNum = evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    unit = BattleGetUnitPtr(wp, BattleTransID(event, unitId));
    if (unit == NULL)
        return EVT_RETURN_BLOCK;
    index = partNum - 2;
    dest = (Vec*)unit->work[index];
    if (BtlUnit_CheckStatus(unit, STATUS_HUGE)) {
        dest->x = 1.25f * -x;
        dest->y = 1.25f * y;
        dest->z = 1.25f * z;
    } else if (BtlUnit_CheckStatus(unit, STATUS_TINY)) {
        dest->x = 0.75f * -x;
        dest->y = 0.75f * y;
        dest->z = 0.75f * z;
    } else {
        dest->x = -x;
        dest->y = y;
        dest->z = z;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(mahorn_get_bunsin) {
    s32 found[4];
    s32 i;
    s32 j;
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    s32 count = 0;

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = wp->units[i];
        if (unit != NULL
            && (unit->currentKind == UNIT_DARK_WIZZERD_CLONE || unit->currentKind == UNIT_ELITE_WIZZERD_CLONE)) {
            found[count++] = i;
        }
    }
    for (j = 0; j < 4; j++) {
        if (j < count)
            evtSetValue(event, *args++, found[j]);
        else
            evtSetValue(event, *args++, -1);
    }
    evtSetValue(event, *args++, count);
    return EVT_RETURN_DONE;
}

static USER_FUNC(mahorn_shuffle) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* units[6];
    s32 count = 0;
    s32 i;

    for (i = 0; i < 64; i++) {
        units[count] = work->units[i];
        if (units[count] != NULL && (units[count]->currentKind == 0x80 ||
                                     units[count]->currentKind == 0x81 ||
                                     units[count]->currentKind == 0xA9 ||
                                     units[count]->currentKind == 0xAA)) {
            count++;
        }
    }
    for (i = 0; i < 10; i++) {
        s32 a = irand(count);
        s32 b = irand(count);
        BattleUnit* unitA = units[a];
        BattleUnit* unitB = units[b];
        f32 tmp;

        tmp = unitA->mPosition.x;
        unitA->mPosition.x = unitB->mPosition.x;
        unitB->mPosition.x = tmp;
        tmp = unitA->mHomePosition.x;
        unitA->mHomePosition.x = unitB->mHomePosition.x;
        unitB->mHomePosition.x = tmp;
        tmp = unitA->mPosition.z;
        unitA->mPosition.z = unitB->mPosition.z;
        unitB->mPosition.z = tmp;
        tmp = unitA->mHomePosition.z;
        unitA->mHomePosition.z = unitB->mHomePosition.z;
        unitB->mHomePosition.z = tmp;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(mahorn_copy_status) {
    s32* args = event->args;
    BattleUnit* unit1;
    BattleUnit* unit2;
    s32 id1;
    s32 id2;
    BattleWork* wp;
    f32 ratio;

    id1 = evtGetValue(event, args[0]);
    id2 = evtGetValue(event, args[1]);
    wp = _battleWorkPointer;
    unit1 = BattleGetUnitPtr(wp, id1);
    unit2 = BattleGetUnitPtr(wp, id2);
    unit2->currentHp = unit1->currentHp;
    memcpy(&unit2->mSleepTurns, &unit1->mSleepTurns, 0x1E);
    unit2->hpGauge.field_0xC = unit2->currentHp;
    unit2->hpGauge.field_0x10 = unit2->currentHp;
    ratio = (f32)unit2->currentHp / (f32)unit2->maxHp;
    unit2->hpGauge.field_0x18 = ratio;
    unit2->hpGauge.field_0x14 = ratio;
    return EVT_RETURN_DONE;
}

static USER_FUNC(eff_magic) {
    s32* args = event->args;
    BattleUnit* unit;
    f32 posX, posY, posZ;
    f32 dstX;
    f32 radius;
    s32 count;
    s32 id;
    double tmp;

    id = BattleTransID(event, evtGetValue(event, args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    dstX = evtGetFloat(event, args[1]);
    evtGetFloat(event, args[2]);
    evtGetFloat(event, args[3]);
    count = evtGetValue(event, args[4]);
    BtlUnit_GetPos(unit, &posX, &posY, &posZ);
    posX += unit->mSizeMultiplier * unit->faceDirection;
    posY += 18.0f * unit->mSizeMultiplier;
    posZ += 5.0f;
    tmp = __fabs((dstX - posX) / (f32)count);
    radius = (f32)tmp;
    unit->work[7] = (s32)effMahorn2Entry(FALSE, posX, posY, posZ, dstX, posY, posZ, unit->mSizeMultiplier, radius);
    return EVT_RETURN_DONE;
}

static USER_FUNC(eff_magic_get_posX) {
    s32* args = event->args;
    BattleUnit* unit;
    EffectEntry* effect;
    effMahorn2Data* data;
    s32 i;

    s32 id = BattleTransID(event, evtGetValue(event, *args++));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    effect = (EffectEntry*)unit->work[7];
    data = (effMahorn2Data*)effect->userdata;
    for (i = 0; i < effect->count; i++, data++) {
        if (data->type == 3) {
            evtSetFloat(event, *args++, data->position.x);
            break;
        }
    }
    if (i == effect->count)
        evtSetFloat(event, *args++, 0.0f);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(9), PTR(weapon_mahorn_attack_punch))
        CALL_CHILD(attack_punch_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(mahorn_get_live_num, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL_CHILD(bunsin_event)
        RETURN
    ELSE
        CALL(mahorn_get_bunsin, LW(1), LW(2), LW(3), LW(4), LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL_CHILD(subsetevt_confuse_flustered)
                GOTO(99)
            END_IF
            SET_VALUE(LW(9), PTR(weapon_mahorn_attack_magic))
            CALL_CHILD(attack_magic_event)
            GOTO(99)
        ELSE
            LABEL(10)
            SET_VALUE(LW(0), 0)
            ADD(LW(0), 30)
            ADD(LW(0), 60)
            ADD(LW(0), 10)
            ADD(LW(0), 10)
            ADD(LW(0), 10)
            ADD(LW(0), 10)
            ADD(LW(0), 10)
            ADD(LW(0), 10)
            SUBTRACT(LW(0), 1)
            CALL(evt_sub_random, LW(0), LW(1))
            SET_VALUE(LW(0), 30)
            IF_INT_LESS(LW(1), LW(0))
                SET_VALUE(LW(9), PTR(weapon_mahorn_attack_punch))
                CALL_CHILD(attack_punch_event)
                GOTO(99)
            END_IF
            ADD(LW(0), 60)
            IF_INT_LESS(LW(1), LW(0))
                SET_VALUE(LW(9), PTR(weapon_mahorn_attack_magic))
                CALL_CHILD(attack_magic_event)
                GOTO(99)
            END_IF
            ADD(LW(0), 10)
            IF_INT_LESS(LW(1), LW(0))
                SET_VALUE(LW(9), PTR(weapon_mahorn_all_attack_magic))
                CALL_CHILD(all_attack_magic_event)
                GOTO(99)
            END_IF
            ADD(LW(0), 10)
            IF_INT_LESS(LW(1), LW(0))
                SET_VALUE(LW(9), PTR(weapon_mahorn_dekadeka_magic))
                CALL_CHILD(dekadeka_magic_event)
                GOTO(99)
            END_IF
            ADD(LW(0), 10)
            IF_INT_LESS(LW(1), LW(0))
                SET_VALUE(LW(9), PTR(weapon_mahorn_katikati_magic))
                CALL_CHILD(katikati_magic_event)
                GOTO(99)
            END_IF
            ADD(LW(0), 10)
            IF_INT_LESS(LW(1), LW(0))
                SET_VALUE(LW(9), PTR(weapon_mahorn_trans_magic))
                CALL_CHILD(trans_magic_event)
                GOTO(99)
            END_IF
            ADD(LW(0), 10)
            IF_INT_LESS(LW(1), LW(0))
                SET_VALUE(LW(9), PTR(weapon_mahorn_yokeyoke_magic))
                CALL_CHILD(yokeyoke_magic_event)
                GOTO(99)
            END_IF
            ADD(LW(0), 10)
            IF_INT_LESS(LW(1), LW(0))
                SET_VALUE(LW(9), PTR(weapon_mahorn_recover_magic))
                CALL_CHILD(recover_magic_event)
                GOTO(99)
            END_IF
            GOTO(10)
        END_IF
    END_IF
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event_bunsin)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// TODO: type
static u32 pose_table[34] = {
    0x00000001, (u32)"MAH_N_1", 0x00000002, (u32)"MAH_Y_1",
    0x00000009, (u32)"MAH_Y_1", 0x00000005, (u32)"MAH_K_1",
    0x00000004, (u32)"MAH_X_1", 0x00000003, (u32)"MAH_X_1",
    0x0000001C, (u32)"MAH_S_1", 0x0000001D, (u32)"MAH_Q_1",
    0x0000001E, (u32)"MAH_Q_1", 0x0000001F, (u32)"MAH_A_1",
    0x00000027, (u32)"MAH_D_1", 0x00000028, (u32)"MAH_W_1",
    0x0000002A, (u32)"MAH_R_1", 0x00000038, (u32)"MAH_X_1",
    0x00000039, (u32)"MAH_X_1", 0x00000041, (u32)"MAH_T_1",
    0x00000045, (u32)"MAH_S_1",
};

// TODO: type
static u32 data_table[10] = {
    0x00000031, (u32)&mahorn_subset_dead_event, 0x0000003A, (u32)&mahorn_subset_blow_dead_event,
    0x0000002E, (u32)&avoid_miss_event, 0x00000030, (u32)btldefaultevt_Dummy,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 data_table_bunsin[6] = {
    0x0000002E, (u32)&avoid_miss_event, 0x00000030, (u32)btldefaultevt_Dummy,
    0x00000000, 0x00000000,
};



static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 20, LW(2))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(hand_init, -2)
    CALL(set_hand_dest, -2, 2, FLOAT(-25.0f), FLOAT(30.0f), FLOAT(-10.0f))
    CALL(set_hand_dest, -2, 3, FLOAT(25.0f), FLOAT(30.0f), FLOAT(10.0f))
    CALL(set_hand_dest, -2, 4, FLOAT(-25.0f), FLOAT(10.0f), FLOAT(-10.0f))
    CALL(set_hand_dest, -2, 5, FLOAT(25.0f), FLOAT(10.0f), FLOAT(10.0f))
    SPAWN_EVENT(hand_ctrl_event)
    CALL(btlevtcmd_SetUnitWork, -2, 4, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 6, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(alpha_down_event)
    IF_INT_EQUAL(LW(0), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_SetAlpha, LW(0), 1, 254)
    CALL(btlevtcmd_AnimeFlagOnOff, LW(0), 1, 1, 1)
    WAIT_FRAMES(1)
    CALL(evt_sub_intpl_init, 0, 255, 0, 30)
    LOOP(30)
        CALL(evt_sub_intpl_get_value_para, LW(1), LW(2))
        CALL(btlevtcmd_SetAlpha, LW(0), 1, LW(1))
        CALL(btlevtcmd_SetAlpha, LW(0), 2, LW(1))
        CALL(btlevtcmd_SetAlpha, LW(0), 3, LW(1))
        CALL(btlevtcmd_SetAlpha, LW(0), 4, LW(1))
        CALL(btlevtcmd_SetAlpha, LW(0), 5, LW(1))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_OnPartsAttribute, LW(0), 1, 33554432)
    CALL(btlevtcmd_OnPartsAttribute, LW(0), 2, 33554432)
    CALL(btlevtcmd_OnPartsAttribute, LW(0), 3, 33554432)
    CALL(btlevtcmd_OnPartsAttribute, LW(0), 4, 33554432)
    CALL(btlevtcmd_OnPartsAttribute, LW(0), 5, 33554432)
    CALL(btlevtcmd_SetAlpha, LW(0), 1, 0)
    CALL(btlevtcmd_AnimeFlagOnOff, LW(0), 1, 1, 1)
    WAIT_FRAMES(1)
    RETURN
EVT_END()

static EVT_BEGIN(alpha_up_event)
    IF_INT_EQUAL(LW(0), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_OffPartsAttribute, LW(0), 1, 33554432)
    CALL(btlevtcmd_OffPartsAttribute, LW(0), 2, 33554432)
    CALL(btlevtcmd_OffPartsAttribute, LW(0), 3, 33554432)
    CALL(btlevtcmd_OffPartsAttribute, LW(0), 4, 33554432)
    CALL(btlevtcmd_OffPartsAttribute, LW(0), 5, 33554432)
    CALL(btlevtcmd_SetAlpha, LW(0), 1, 0)
    CALL(btlevtcmd_AnimeFlagOnOff, LW(0), 1, 1, 1)
    WAIT_FRAMES(1)
    CALL(evt_sub_intpl_init, 0, 0, 254, 30)
    LOOP(30)
        CALL(evt_sub_intpl_get_value_para, LW(1), LW(2))
        CALL(btlevtcmd_SetAlpha, LW(0), 1, LW(1))
        CALL(btlevtcmd_SetAlpha, LW(0), 2, LW(1))
        CALL(btlevtcmd_SetAlpha, LW(0), 3, LW(1))
        CALL(btlevtcmd_SetAlpha, LW(0), 4, LW(1))
        CALL(btlevtcmd_SetAlpha, LW(0), 5, LW(1))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetAlpha, LW(0), 1, 255)
    CALL(btlevtcmd_SetAlpha, LW(0), 2, 255)
    CALL(btlevtcmd_SetAlpha, LW(0), 3, 255)
    CALL(btlevtcmd_SetAlpha, LW(0), 4, 255)
    CALL(btlevtcmd_SetAlpha, LW(0), 5, 255)
    CALL(btlevtcmd_AnimeFlagOnOff, LW(0), 1, 1, 1)
    WAIT_FRAMES(1)
    RETURN
EVT_END()


static EVT_BEGIN(hand_ctrl_event)
    LABEL(0)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    IF_INT_EQUAL(LW(0), -1)
        GOTO(99)
    END_IF
    CALL(hand_ctrl, -2)
    GOTO(0)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(mahorn_get_bunsin, LW(1), LW(2), LW(3), LW(4), LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            CALL(btlevtcmd_SetUnitWork, -2, 5, 1)
        END_IF
    ELSE
        SET_VALUE(LW(0), -2)
        CALL_CHILD(bunsin_kill_event)
        RETURN
    END_IF
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        CALL(btlevtcmd_GetUnitWork, -2, 5, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL_CHILD(mahorn_subset_bunsin_all_kill)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(mahorn_subset_bunsin_all_kill)
    CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(mahorn_get_bunsin, LW(1), LW(2), LW(3), LW(4), LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            CALL(btlevtcmd_WaitAttackEnd)
            CALL(mahorn_get_bunsin, LW(1), LW(2), LW(3), LW(4), LW(0))
            SET_VALUE(LW(0), LW(1))
            SPAWN_EVENT(bunsin_kill_event)
            SET_VALUE(LW(0), LW(2))
            SPAWN_EVENT(bunsin_kill_event)
            SET_VALUE(LW(0), LW(3))
            SPAWN_EVENT(bunsin_kill_event)
            SET_VALUE(LW(0), LW(4))
            SPAWN_EVENT(bunsin_kill_event)
            LOOP(0)
                CALL(mahorn_get_bunsin, LW(1), LW(2), LW(3), LW(4), LW(0))
                IF_INT_EQUAL(LW(0), 0)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(mahorn_subset_dead_event)
    CALL(btlevtcmd_CheckDataOfDataTable, LW(10), LW(0), 50)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_RunDataEventChild, LW(10), 50)
    END_IF
    SET_VALUE(LW(0), 48)
    CALL(btlevtcmd_OnUnitFlag, LW(10), 134217728)
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_ENEMY_DIE1_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LABEL(10)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AddRotate, LW(10), 0, -15, 0)
    SUBTRACT(LW(0), 1)
    IF_INT_GREATER(LW(0), 0)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_SetRotate, LW(10), 0, 0, 0)
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    ADD(LW(2), 10)
    CALL(evt_eff, 0, STRING("kemuri_test"), 4, LW(0), LW(1), LW(2), FLOAT(0.8f), 0, 0, 0, 0, 0, 0, 0)
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
    CALL(btlevtcmd_SetRotateOffset, LW(10), 0, 0, 0)
    SET_VALUE(LW(0), 30)
    LABEL(20)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AddRotate, LW(10), 3, 0, 0)
    SUBTRACT(LW(0), 1)
    IF_INT_GREATER(LW(0), 0)
        GOTO(20)
    END_IF
    CALL(btlevtcmd_GetBackItem, LW(10))
    CALL(btlevtcmd_ClearAllStatus, LW(10))
    CALL(btlevtcmd_OnAttribute, LW(10), 16777216)
    CALL(btlevtcmd_OnAttribute, LW(10), 536870912)
    CALL_CHILD(mahorn_subset_bunsin_all_kill)
    CALL(btlevtcmd_CheckDamageCode, LW(10), 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_KillUnit, LW(10), 0)
    ELSE
        CALL(btlevtcmd_KillUnit, LW(10), 2)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(mahorn_subset_blow_dead_event)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_CLAUD_BREATH3"), 0)
    CALL(btlevtcmd_GetBodyId, -2, LW(0))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(0), 39)
    BEGIN_SIBLING_EVENT
        LABEL(0)
        CALL(btlevtcmd_AddRotate, -2, 0, 20, 0)
        WAIT_FRAMES(1)
        GOTO(0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(2.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStageSize, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    ADD(LW(1), 40)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, -10, 2, 0, -1)
    CALL(btlevtcmd_GetCoin, LW(10), LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, LW(10), LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL_CHILD(mahorn_subset_bunsin_all_kill)
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

static EVT_BEGIN(avoid_miss_event)
    CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(0), -2)
        CALL_CHILD(bunsin_kill_event)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(bunsin_kill_event)
    CALL(btlevtcmd_GetUnitId, LW(0), LW(0))
    IF_INT_EQUAL(LW(0), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, LW(0), 6, LW(1))
    IF_INT_EQUAL(LW(1), 1)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_SetUnitWork, LW(0), 6, 1)
    CALL_CHILD(alpha_down_event)
    LABEL(10)
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(btlevtcmd_KillUnit, LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(attack_punch_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
        CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    END_IF
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
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(set_hand_dest, -2, 2, FLOAT(-16.0f), FLOAT(29.0f), FLOAT(-5.0f))
    CALL(set_hand_dest, -2, 3, FLOAT(16.0f), FLOAT(29.0f), FLOAT(5.0f))
    CALL(set_hand_dest, -2, 4, FLOAT(-16.0f), FLOAT(11.0f), FLOAT(-5.0f))
    CALL(set_hand_dest, -2, 5, FLOAT(16.0f), FLOAT(11.0f), FLOAT(5.0f))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    IF_INT_EQUAL(LW(15), -1)
        ADD(LW(0), 35)
    ELSE
        SUBTRACT(LW(0), 35)
    END_IF
    CALL(btlevtcmd_JumpSetting, -2, 0, 4, FLOAT(0.001f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 20, LW(2), 0, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MAH_S_1"))
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("MAH_Z_2"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("MAH_Z_2"))
    CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("MAH_Z_2"))
    CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("MAH_Z_2"))
    SET_VALUE(LW(10), 2)
    LOOP(4)
        CALL(btlevtcmd_SetPartsBlur, -2, LW(10), 255, 255, 255, 255, 255, 255, 255, 100, 0)
        CALL(btlevtcmd_OnPartsAttribute, -2, LW(10), 67108864)
        ADD(LW(10), 1)
    END_LOOP
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LOOP(25)
        SET_VALUE(LW(10), 2)
        LOOP(4)
            CALL(btlevtcmd_GetPartsPos, -2, LW(10), LW(0), LW(1), LW(2))
            IF_INT_EQUAL(LW(15), -1)
                CALL(evt_sub_random, 120, LW(11))
                ADD(LW(11), -90)
            ELSE
                CALL(evt_sub_random, 120, LW(11))
                MULTIPLY(LW(11), -1)
                ADD(LW(11), 90)
            END_IF
            CALL(evt_sub_random, 60, LW(12))
            ADD(LW(12), -20)
            CALL(set_hand_dest, -2, LW(10), LW(11), LW(12), LW(2))
            ADD(LW(10), 1)
        END_LOOP
        WAIT_FRAMES(2)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MAH_S_1"))
    CALL(set_hand_dest, -2, 2, FLOAT(-16.0f), FLOAT(29.0f), FLOAT(-5.0f))
    CALL(set_hand_dest, -2, 3, FLOAT(16.0f), FLOAT(29.0f), FLOAT(5.0f))
    CALL(set_hand_dest, -2, 4, FLOAT(-16.0f), FLOAT(11.0f), FLOAT(-5.0f))
    CALL(set_hand_dest, -2, 5, FLOAT(16.0f), FLOAT(11.0f), FLOAT(5.0f))
    SET_VALUE(LW(10), 2)
    LOOP(4)
        CALL(btlevtcmd_OffPartsAttribute, -2, LW(10), 67108864)
        ADD(LW(10), 1)
    END_LOOP
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
        LABEL(90)
        WAIT_FRAMES(30)
        GOTO(98)
        LABEL(91)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        WAIT_FRAMES(30)
        GOTO(98)
        LABEL(98)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_ATTACK1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpSetting, -2, 0, 6, FLOAT(0.001f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
        WAIT_FRAMES(45)
        LABEL(99)
        CALL(btlevtcmd_ResetFaceDirection, -2)
        RETURN
EVT_END()

static EVT_BEGIN(start_avoid)
    IF_INT_EQUAL(LW(5), 3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
    END_IF
    IF_INT_EQUAL(LW(5), 6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
    END_IF
    IF_INT_EQUAL(LW(5), 2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
    END_IF
    RETURN
EVT_END()


static EVT_BEGIN(attack_enemy_common_event)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_CHARGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(mahorn_get_bunsin, LW(11), LW(12), LW(13), LW(14), LW(0))
    SET_VALUE(LW(15), LW(11))
    SPAWN_EVENT(attack_enemy_common_event_sub1)
    SET_VALUE(LW(15), LW(12))
    SPAWN_EVENT(attack_enemy_common_event_sub1)
    SET_VALUE(LW(15), LW(13))
    SPAWN_EVENT(attack_enemy_common_event_sub1)
    SET_VALUE(LW(15), LW(14))
    SPAWN_EVENT(attack_enemy_common_event_sub1)
    SET_VALUE(LW(15), -2)
    CALL_CHILD(attack_enemy_common_event_sub1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_MAGIC2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(25)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_MAGIC2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        GOTO(90)
        CASE_EQUAL(2)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
        LABEL(90)
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(60)
            LOOP(0)
                CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
                IF_INT_EQUAL(LW(0), -1)
                    CALL(eff_magic_get_posX, -2, LW(0))
                    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                    IF_INT_LESS_OR_EQUAL(LW(0), LW(6))
                        CALL_CHILD(start_avoid)
                        BREAK_LOOP
                    END_IF
                ELSE
                    CALL(eff_magic_get_posX, -2, LW(0))
                    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
                    IF_INT_GREATER_OR_EQUAL(LW(0), LW(6))
                        CALL_CHILD(start_avoid)
                        BREAK_LOOP
                    END_IF
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(mahorn_get_bunsin, LW(11), LW(12), LW(13), LW(14), LW(0))
        SET_VALUE(LW(15), LW(11))
        SPAWN_EVENT(attack_enemy_common_event_sub_no_hit)
        SET_VALUE(LW(15), LW(12))
        SPAWN_EVENT(attack_enemy_common_event_sub_no_hit)
        SET_VALUE(LW(15), LW(13))
        SPAWN_EVENT(attack_enemy_common_event_sub_no_hit)
        SET_VALUE(LW(15), LW(14))
        SPAWN_EVENT(attack_enemy_common_event_sub_no_hit)
        SET_VALUE(LW(15), -2)
        CALL_CHILD(attack_enemy_common_event_sub_no_hit)
        GOTO(98)
        LABEL(91)
        CALL(mahorn_get_bunsin, LW(11), LW(12), LW(13), LW(14), LW(0))
        SET_VALUE(LW(15), LW(11))
        SPAWN_EVENT(attack_enemy_common_event_sub_hit)
        SET_VALUE(LW(15), LW(12))
        SPAWN_EVENT(attack_enemy_common_event_sub_hit)
        SET_VALUE(LW(15), LW(13))
        SPAWN_EVENT(attack_enemy_common_event_sub_hit)
        SET_VALUE(LW(15), LW(14))
        SPAWN_EVENT(attack_enemy_common_event_sub_hit)
        SET_VALUE(LW(15), -2)
        CALL_CHILD(attack_enemy_common_event_sub_hit)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        GOTO(98)
        LABEL(98)
        CALL(mahorn_get_bunsin, LW(11), LW(12), LW(13), LW(14), LW(0))
        IF_INT_NOT_EQUAL(LW(11), -1)
            CALL(btlevtcmd_StartWaitEvent, LW(11))
        END_IF
        IF_INT_NOT_EQUAL(LW(12), -1)
            CALL(btlevtcmd_StartWaitEvent, LW(12))
        END_IF
        IF_INT_NOT_EQUAL(LW(13), -1)
            CALL(btlevtcmd_StartWaitEvent, LW(13))
        END_IF
        IF_INT_NOT_EQUAL(LW(14), -1)
            CALL(btlevtcmd_StartWaitEvent, LW(14))
        END_IF
        WAIT_FRAMES(30)
        RETURN
EVT_END()

static EVT_BEGIN(attack_enemy_all_common_event)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_CHARGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    SET_VALUE(LW(15), -2)
    CALL_CHILD(attack_enemy_common_event_sub1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_MAGIC2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(25)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_MAGIC2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    SET_VALUE(LW(15), -2)
    CALL_CHILD(attack_enemy_common_event_sub2)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(6), 0)
    LABEL(10)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(evt_eff64, STRING(""), STRING("thunder_n64"), 4, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
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
        LABEL(90)
        GOTO(97)
        LABEL(91)
        IF_INT_EQUAL(LW(6), 0)
            CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
            SET_VALUE(LW(6), 1)
        END_IF
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        GOTO(97)
        LABEL(97)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            GOTO(98)
        END_IF
        GOTO(10)
        LABEL(98)
        WAIT_FRAMES(30)
        RETURN
EVT_END()

static EVT_BEGIN(attack_enemy_common_event_sub1)
    IF_INT_EQUAL(LW(15), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_StopWaitEvent, LW(15))
    CALL(btlevtcmd_GetPos, LW(15), LW(0), LW(1), LW(2))
    ADD(LW(1), 40)
    CALL(evt_eff64, STRING(""), STRING("akari_charge_n64"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 70, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, LW(15), 1, STRING("MAH_A_4"))
    CALL(btlevtcmd_AnimeChangePose, LW(15), 2, STRING("MAH_S_5A"))
    CALL(btlevtcmd_AnimeChangePose, LW(15), 3, STRING("MAH_S_5A"))
    CALL(btlevtcmd_AnimeChangePose, LW(15), 4, STRING("MAH_S_5A"))
    CALL(btlevtcmd_AnimeChangePose, LW(15), 5, STRING("MAH_S_5A"))
    CALL(set_hand_dest, LW(15), 2, FLOAT(-16.0f), FLOAT(29.0f), FLOAT(-5.0f))
    CALL(set_hand_dest, LW(15), 3, FLOAT(16.0f), FLOAT(29.0f), FLOAT(5.0f))
    CALL(set_hand_dest, LW(15), 4, FLOAT(-16.0f), FLOAT(11.0f), FLOAT(-5.0f))
    CALL(set_hand_dest, LW(15), 5, FLOAT(16.0f), FLOAT(11.0f), FLOAT(5.0f))
    WAIT_FRAMES(50)
    RETURN
EVT_END()

static EVT_BEGIN(attack_enemy_common_event_sub2)
    IF_INT_EQUAL(LW(15), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_StopWaitEvent, LW(15))
    CALL(btlevtcmd_AnimeChangePose, LW(15), 1, STRING("MAH_A_1"))
    BEGIN_INLINE_EVENT
        LOOP(2)
            SET_FLOAT_VALUE(LW(0), FLOAT(-16.0f))
            SET_FLOAT_VALUE(LW(1), FLOAT(20.0f))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(2))
            MULTIPLY_FLOAT(LW(1), LW(2))
            CALL(btlevtcmd_AnimeChangePose, LW(15), 2, STRING("MAH_S_2B"))
            ADD(LW(0), LW(1))
            CALL(set_hand_dest, LW(15), 2, LW(0), FLOAT(29.0f), FLOAT(-5.0f))
            WAIT_FRAMES(20)
            CALL(btlevtcmd_AnimeChangePose, LW(15), 2, STRING("MAH_S_2A"))
            SUBTRACT(LW(0), LW(1))
            CALL(set_hand_dest, LW(15), 2, LW(0), FLOAT(29.0f), FLOAT(-5.0f))
            WAIT_FRAMES(20)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(7)
        LOOP(2)
            SET_FLOAT_VALUE(LW(0), FLOAT(16.0f))
            SET_FLOAT_VALUE(LW(1), FLOAT(20.0f))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(2))
            MULTIPLY_FLOAT(LW(1), LW(2))
            CALL(btlevtcmd_AnimeChangePose, LW(15), 5, STRING("MAH_S_2B"))
            ADD(LW(0), LW(1))
            CALL(set_hand_dest, LW(15), 5, LW(0), FLOAT(11.0f), FLOAT(5.0f))
            WAIT_FRAMES(20)
            CALL(btlevtcmd_AnimeChangePose, LW(15), 5, STRING("MAH_S_2A"))
            SUBTRACT(LW(0), LW(1))
            CALL(set_hand_dest, LW(15), 5, LW(0), FLOAT(11.0f), FLOAT(5.0f))
            WAIT_FRAMES(20)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(10)
        LOOP(2)
            SET_FLOAT_VALUE(LW(0), FLOAT(-16.0f))
            SET_FLOAT_VALUE(LW(1), FLOAT(20.0f))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(2))
            MULTIPLY_FLOAT(LW(1), LW(2))
            CALL(btlevtcmd_AnimeChangePose, LW(15), 4, STRING("MAH_S_2B"))
            ADD(LW(0), LW(1))
            CALL(set_hand_dest, LW(15), 4, LW(0), FLOAT(11.0f), FLOAT(-5.0f))
            WAIT_FRAMES(20)
            CALL(btlevtcmd_AnimeChangePose, LW(15), 4, STRING("MAH_S_2A"))
            SUBTRACT(LW(0), LW(1))
            CALL(set_hand_dest, LW(15), 4, LW(0), FLOAT(11.0f), FLOAT(-5.0f))
            WAIT_FRAMES(20)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(15)
        LOOP(2)
            SET_FLOAT_VALUE(LW(0), FLOAT(16.0f))
            SET_FLOAT_VALUE(LW(1), FLOAT(20.0f))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(2))
            MULTIPLY_FLOAT(LW(1), LW(2))
            CALL(btlevtcmd_AnimeChangePose, LW(15), 3, STRING("MAH_S_2B"))
            ADD(LW(0), LW(1))
            CALL(set_hand_dest, LW(15), 3, LW(0), FLOAT(29.0f), FLOAT(5.0f))
            WAIT_FRAMES(20)
            CALL(btlevtcmd_AnimeChangePose, LW(15), 3, STRING("MAH_S_2A"))
            SUBTRACT(LW(0), LW(1))
            CALL(set_hand_dest, LW(15), 3, LW(0), FLOAT(29.0f), FLOAT(5.0f))
            WAIT_FRAMES(20)
        END_LOOP
    END_INLINE_EVENT
    WAIT_FRAMES(55)
    RETURN
EVT_END()


static EVT_BEGIN(attack_enemy_common_event_sub_no_hit)
    IF_INT_EQUAL(LW(15), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_GetPos, LW(15), LW(0), LW(1), LW(2))
    ADD(LW(1), 10)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(10), LW(11), LW(12))
    CALL(btlevtcmd_GetStageSize, LW(10), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetFaceDirection, LW(15), LW(13))
    MULTIPLY(LW(10), LW(13))
    SUBTRACT(LW(11), 5)
    CALL(eff_magic, LW(15), LW(10), LW(11), LW(12), 40)
    CALL_CHILD(attack_enemy_common_event_sub2)
    WAIT_FRAMES(5)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_MAGIC1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_MAGIC3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(40)
    RETURN
EVT_END()

static EVT_BEGIN(attack_enemy_common_event_sub_hit)
    IF_INT_EQUAL(LW(15), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_GetPos, LW(15), LW(0), LW(1), LW(2))
    ADD(LW(1), 10)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(10), LW(11), LW(12))
    CALL(eff_magic, LW(15), LW(10), LW(11), LW(12), 20)
    CALL_CHILD(attack_enemy_common_event_sub2)
    WAIT_FRAMES(5)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_MAGIC1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_MAGIC3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(20)
    RETURN
EVT_END()

static EVT_BEGIN(attack_magic_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
        CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL_CHILD(attack_enemy_common_event)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(confuse_magic_event)
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
    CALL_CHILD(attack_enemy_common_event)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(all_attack_magic_event)
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
    CALL_CHILD(attack_enemy_all_common_event)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(support_friend_common_event)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_CHARGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 40)
    CALL(evt_eff64, STRING(""), STRING("akari_charge_n64"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 70, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MAH_A_4"))
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("MAH_S_5A"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("MAH_S_5A"))
    CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("MAH_S_5A"))
    CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("MAH_S_5A"))
    CALL(set_hand_dest, -2, 2, FLOAT(-16.0f), FLOAT(29.0f), FLOAT(-5.0f))
    CALL(set_hand_dest, -2, 3, FLOAT(16.0f), FLOAT(29.0f), FLOAT(5.0f))
    CALL(set_hand_dest, -2, 4, FLOAT(-16.0f), FLOAT(11.0f), FLOAT(-5.0f))
    CALL(set_hand_dest, -2, 5, FLOAT(16.0f), FLOAT(11.0f), FLOAT(5.0f))
    WAIT_FRAMES(50)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MAH_A_2A"))
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("MAH_S_4A"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("MAH_S_4A"))
    CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("MAH_S_4A"))
    CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("MAH_S_4A"))
    CALL(set_hand_dest, -2, 2, FLOAT(-20.0f), FLOAT(40.0f), FLOAT(-15.0f))
    CALL(set_hand_dest, -2, 3, FLOAT(20.0f), FLOAT(40.0f), FLOAT(15.0f))
    CALL(set_hand_dest, -2, 4, FLOAT(-20.0f), FLOAT(0.0f), FLOAT(-15.0f))
    CALL(set_hand_dest, -2, 5, FLOAT(20.0f), FLOAT(0.0f), FLOAT(15.0f))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_MAGIC4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 15)
    CALL(evt_eff, STRING(""), STRING("mahorn"), 0, LW(0), LW(1), LW(2), FLOAT(1.4f), 2, 30, 0, 0, 0, 0, 0)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_MAGIC4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(1), 15)
    CALL(evt_eff, STRING(""), STRING("mahorn"), 0, LW(0), LW(1), LW(2), FLOAT(1.4f), 2, 30, 0, 0, 0, 0, 0)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("MAH_A_2C"))
    RETURN
EVT_END()

static EVT_BEGIN(dekadeka_magic_event)
    SET_VALUE(LW(3), -2)
    SET_VALUE(LW(4), 1)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL_CHILD(support_friend_common_event)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    WAIT_FRAMES(10)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(30)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(katikati_magic_event)
    SET_VALUE(LW(3), -2)
    SET_VALUE(LW(4), 1)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL_CHILD(support_friend_common_event)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    WAIT_FRAMES(10)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(30)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(trans_magic_event)
    SET_VALUE(LW(3), -2)
    SET_VALUE(LW(4), 1)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL_CHILD(support_friend_common_event)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    WAIT_FRAMES(10)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(30)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(yokeyoke_magic_event)
    SET_VALUE(LW(3), -2)
    SET_VALUE(LW(4), 1)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL_CHILD(support_friend_common_event)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    WAIT_FRAMES(10)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(30)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(recover_magic_event)
    SET_VALUE(LW(3), -2)
    SET_VALUE(LW(4), 1)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL_CHILD(support_friend_common_event)
    CALL(btlevtcmd_RecoverHp, LW(3), LW(4), 10)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(1), 30)
    ADD(LW(2), 10)
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, LW(0), LW(1), LW(2), 10, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(10)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(30)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()


static EVT_BEGIN(bunsin_event)
    SET_VALUE(LW(3), -2)
    SET_VALUE(LW(4), 1)
    CALL_CHILD(support_friend_common_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_OFFSHOOT1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    SET_VALUE(LW(0), -2)
    CALL_CHILD(alpha_down_event)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    LOOP(0)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
        SET_VALUE(LW(4), LW(0))
        IF_INT_GREATER(LW(4), 0)
            SUBTRACT(LW(2), 10)
            CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
            IF_INT_EQUAL(LW(3), -1)
                CALL_CHILD(spawn_event)
            END_IF
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    LOOP(0)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
        IF_INT_LESS_OR_EQUAL(LW(0), 170)
            ADD(LW(2), 10)
            CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
            IF_INT_EQUAL(LW(3), -1)
                CALL_CHILD(spawn_event)
            END_IF
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(mahorn_shuffle)
    CALL(btlevtcmd_StartWaitEvent, -2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_MAHO_OFFSHOOT2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    SET_VALUE(LW(0), -2)
    SPAWN_EVENT(alpha_up_event)
    CALL(mahorn_get_bunsin, LW(1), LW(2), LW(3), LW(4))
    SET_VALUE(LW(0), LW(1))
    SPAWN_EVENT(alpha_up_event)
    SET_VALUE(LW(0), LW(2))
    SPAWN_EVENT(alpha_up_event)
    SET_VALUE(LW(0), LW(3))
    SPAWN_EVENT(alpha_up_event)
    SET_VALUE(LW(0), LW(4))
    SPAWN_EVENT(alpha_up_event)
    WAIT_FRAMES(60)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()


static EVT_BEGIN(spawn_event)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_mahorn), 0)
    CALL(btlevtcmd_SetAlpha, LW(3), 1, 0)
    CALL(btlevtcmd_SetAlpha, LW(3), 2, 0)
    CALL(btlevtcmd_SetAlpha, LW(3), 3, 0)
    CALL(btlevtcmd_SetAlpha, LW(3), 4, 0)
    CALL(btlevtcmd_SetAlpha, LW(3), 5, 0)
    WAIT_FRAMES(2)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetUnitId, -2, LW(13))
    CALL(mahorn_copy_status, LW(13), LW(3))
    CALL(btlevtcmd_OffAttribute, LW(3), 33554432)
    CALL(btlevtcmd_SetUnitWork, LW(3), 4, 1)
    CALL(btlevtcmd_SetEventAttack, LW(3), PTR(attack_event_bunsin))
    CALL(btlevtcmd_SetRegistStatus, LW(3), PTR(regist_bunsin))
    CALL(btlevtcmd_OnPartsAttribute, LW(3), 1, 16384)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(mahorn_get_bunsin, LW(1), LW(2), LW(3), LW(4), LW(0))
        CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
        IF_INT_NOT_EQUAL(LW(1), -1)
            CALL(btlevtcmd_AnimeChangePoseFromTable, LW(1), 1)
        END_IF
        IF_INT_NOT_EQUAL(LW(2), -1)
            CALL(btlevtcmd_AnimeChangePoseFromTable, LW(2), 1)
        END_IF
        IF_INT_NOT_EQUAL(LW(3), -1)
            CALL(btlevtcmd_AnimeChangePoseFromTable, LW(3), 1)
        END_IF
        IF_INT_NOT_EQUAL(LW(4), -1)
            CALL(btlevtcmd_AnimeChangePoseFromTable, LW(4), 1)
        END_IF
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 9, LW(13))
    CALL(btlevtcmd_CheckStatus, -2, 1, LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 2, LW(15))
    OR(LW(15), LW(13))
    OR(LW(15), LW(14))
    IF_INT_NOT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("MAH_Z_2"))
        CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("MAH_Z_2"))
        CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("MAH_Z_2"))
        CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("MAH_Z_2"))
    ELSE
        BEGIN_SIBLING_EVENT
            LABEL(10)
            CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("MAH_S_4A"))
            CALL(evt_sub_random, 15, LW(0))
            ADD(LW(0), 15)
            WAIT_FRAMES(LW(0))
            CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("MAH_S_4B"))
            CALL(evt_sub_random, 15, LW(0))
            ADD(LW(0), 15)
            WAIT_FRAMES(LW(0))
            GOTO(10)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            LABEL(11)
            CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("MAH_S_4A"))
            CALL(evt_sub_random, 15, LW(0))
            ADD(LW(0), 15)
            WAIT_FRAMES(LW(0))
            CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("MAH_S_4B"))
            CALL(evt_sub_random, 15, LW(0))
            ADD(LW(0), 15)
            WAIT_FRAMES(LW(0))
            GOTO(11)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            LABEL(12)
            CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("MAH_S_4A"))
            CALL(evt_sub_random, 15, LW(0))
            ADD(LW(0), 15)
            WAIT_FRAMES(LW(0))
            CALL(btlevtcmd_AnimeChangePose, -2, 4, STRING("MAH_S_4B"))
            CALL(evt_sub_random, 15, LW(0))
            ADD(LW(0), 15)
            WAIT_FRAMES(LW(0))
            GOTO(12)
        END_SIBLING_EVENT
        BEGIN_SIBLING_EVENT
            LABEL(13)
            CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("MAH_S_4A"))
            CALL(evt_sub_random, 15, LW(0))
            ADD(LW(0), 15)
            WAIT_FRAMES(LW(0))
            CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("MAH_S_4B"))
            CALL(evt_sub_random, 15, LW(0))
            ADD(LW(0), 15)
            WAIT_FRAMES(LW(0))
            GOTO(13)
        END_SIBLING_EVENT
    END_IF
    BEGIN_SIBLING_EVENT
        LABEL(0)
        CALL(set_hand_dest, -2, 2, FLOAT(-25.0f), FLOAT(30.0f), FLOAT(-10.0f))
        CALL(evt_sub_random, 15, LW(0))
        ADD(LW(0), 15)
        WAIT_FRAMES(LW(0))
        CALL(set_hand_dest, -2, 2, FLOAT(-15.0f), FLOAT(40.0f), FLOAT(-10.0f))
        CALL(evt_sub_random, 15, LW(0))
        ADD(LW(0), 15)
        WAIT_FRAMES(LW(0))
        GOTO(0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LABEL(1)
        CALL(set_hand_dest, -2, 3, FLOAT(25.0f), FLOAT(30.0f), FLOAT(10.0f))
        CALL(evt_sub_random, 15, LW(0))
        ADD(LW(0), 15)
        WAIT_FRAMES(LW(0))
        CALL(set_hand_dest, -2, 3, FLOAT(15.0f), FLOAT(40.0f), FLOAT(10.0f))
        CALL(evt_sub_random, 15, LW(0))
        ADD(LW(0), 15)
        WAIT_FRAMES(LW(0))
        GOTO(1)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        LABEL(2)
        CALL(set_hand_dest, -2, 4, FLOAT(-25.0f), FLOAT(10.0f), FLOAT(-10.0f))
        CALL(evt_sub_random, 15, LW(0))
        ADD(LW(0), 15)
        WAIT_FRAMES(LW(0))
        CALL(set_hand_dest, -2, 4, FLOAT(-15.0f), FLOAT(0.0f), FLOAT(-10.0f))
        CALL(evt_sub_random, 15, LW(0))
        ADD(LW(0), 15)
        WAIT_FRAMES(LW(0))
        GOTO(2)
    END_SIBLING_EVENT
    LABEL(3)
    CALL(set_hand_dest, -2, 5, FLOAT(25.0f), FLOAT(10.0f), FLOAT(10.0f))
    CALL(evt_sub_random, 15, LW(0))
    ADD(LW(0), 15)
    WAIT_FRAMES(LW(0))
    CALL(set_hand_dest, -2, 5, FLOAT(15.0f), FLOAT(0.0f), FLOAT(10.0f))
    CALL(evt_sub_random, 15, LW(0))
    ADD(LW(0), 15)
    WAIT_FRAMES(LW(0))
    GOTO(3)
    RETURN
EVT_END()

// clang-format on
