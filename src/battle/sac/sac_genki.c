#include <stdlib.h>
#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "battle/battle_unit.h"
#include "battle/battle_ac.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/dispdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "system.h"
#include <string.h>
#include <math.h>
#include <dolphin/mtx.h>
#include "eff/eff_recovery.h"
#include "eff/eff_starstone.h"
#include "statuswindow.h"
#include "battle/sac/sac_genki.h"
#include "battle/sac/sac_deka.h"
#include "eff/n64/eff_stamp_n64.h"
#include "eff/eff_stardust.h"
#include "eff/n64/eff_stardust_n64.h"
#include "eff/eff_hit.h"
#include "pmario_sound.h"
static void main_base(void);  // local to this file in the original
static void disp_3D(CameraId cameraId, void* param);  // local to this file in the original

extern char str_8030045C[];
const f32 float_30_80421d80 = 30.0f;
const f32 float_1p25_80421d6c = 1.25f;
extern f64 double_to_int_80300468;
const f32 float_360_80421de8 = 360.0f;
const f32 float_60_80421d50 = 60.0f;
const f32 float_neg1p25_80421e0c = -1.25f;
const f32 float_0_80421d84 = 0.0f;
const char str_8030048C[] = "M_C_2A";
const char str_80300494[] = "M_C_2B";
const char str_80421D3C[] = "M_I_Y";
const char str_80421E10[] = "M_S_1";
const char str_80421E18[] = "M_D_1";
const char str_80421E20[] = "M_X_1";
const f32 float_20_80421d78 = 20.0f;
const f32 float_10_80421d54 = 10.0f;
const s32 dat_803002c8[3] = {0, 4, 2};
const f32 float_1p32_80421ddc = 1.32f;
const f32 float_1p98_80421de0 = 1.98f;
const f32 float_15_80421de4 = 15.0f;
const f32 float_neg1000_80421d88 = -1000.0f;
const f32 float_1_80421d74 = 1.0f;
const f32 float_901_80421e28 = 901.0f;
// First .rodata symbol of this unit; its presence anchors MWCC's literal pool.
const f32 vec3_803002b0[3] = {0.0f, -1000.0f, 0.0f};
extern const Vec vec3_803002bc;
const f32 float_37_80421d8c = 37.0f;
const f32 float_50_80421d90 = 50.0f;
const f32 float_neg1_80421d94 = -1.0f;
const f32 float_2160_80421d98 = 2160.0f;
const f32 float_200_80421d9c = 200.0f;
const f32 float_32767_80421da0 = 32767.0f;
const f32 float_720_80421da4 = 720.0f;
const f32 float_120_80421da8 = 120.0f;
const f32 float_0p2_80421dac = 0.2f;
const f32 float_neg2_80421db0 = -2.0f;
const f32 float_3_80421db4 = 3.0f;
const f32 float_neg3_80421db8 = -3.0f;
const f32 float_0p1_80421dbc = 0.1f;
const f32 float_6p2832_80421dc0 = 6.283184f;
const f32 float_1p5_80421dc4 = 1.5f;
const f32 float_180_80421d4c = 180.0f;
const f32 float_2_80421d70 = 2.0f;
const f32 float_7_80421dc8 = 7.0f;
const f32 float_40_80421dcc = 40.0f;
const f32 float_0p75_80421dd0 = 0.75f;
const f32 float_0p01_80421dd4 = 0.01f;
const f32 float_0p05_80421dd8 = 0.05f;
const f32 float_3p1416_80421d48 = 3.141592f;
const f32 float_28_80421dec = 28.0f;
const f32 float_0p15_80421df0 = 0.15f;
const f32 float_250_80421df4 = 250.0f;
const f32 float_1E07_80421df8 = 10000000.0f;
const f32 float_344_80421dfc = 344.0f;
const f32 float_576_80421e00 = 576.0f;
const f32 float_1296_80421e04 = 1296.0f;
const f32 float_1p125_80421e08 = 1.125f;
extern char str_80300470[];


typedef struct GenkiStar {
    s32 field_0;                // 0x0
    s32 field_4;                // 0x4
    s32 field_8;                // 0x8
    f32 field_C;                // 0xC
    f32 field_10;               // 0x10
    f32 field_14;               // 0x14
    f32 field_18;               // 0x18
    f32 field_1C;               // 0x1C
    f32 field_20;               // 0x20
    f32 field_24;               // 0x24
    f32 field_28;               // 0x28
    f32 field_2C;               // 0x2C
    f32 field_30;               // 0x30
    f32 field_34;               // 0x34
    f32 field_38;               // 0x38
    f32 field_3C;               // 0x3C
    f32 field_40;               // 0x40
    f32 field_44;               // 0x44
    f32 field_48;               // 0x48
    f32 field_4C;               // 0x4C
    f32 field_50;               // 0x50
    f32 field_54;               // 0x54
    f32 field_58;               // 0x58
    f32 field_5C;               // 0x5C
    EffectEntry* field_60;      // 0x60
} GenkiStar;                    // 0x64

typedef struct GenkiObject {
    s32 field_0;                // 0x0
    s32 field_4;                // 0x4
    s32 field_8;                // 0x8
    f32 field_C;                // 0xC
    f32 field_10;               // 0x10
    f32 field_14;               // 0x14
    u8 pad_18[0x2C - 0x18];     // 0x18
} GenkiObject;                  // 0x2C

typedef struct GenkiEffect {
    s32 field_0;                // 0x0
    s32 field_4;                // 0x4
    s32 field_8;                // 0x8
    s32 field_C;                // 0xC
    f32 field_10;               // 0x10
} GenkiEffect;                  // 0x14

typedef struct GenkiWeapon {
    s32 field_0;                // 0x0
    s32 field_4;                // 0x4
    Vec pos[4];                 // 0x8, current position then 3 trail positions
    Vec vel;                    // 0x38
    EffectEntry* field_44;      // 0x44
    EffectEntry* field_48[3];   // 0x48
} GenkiWeapon;                  // 0x54

typedef struct GenkiWork {
    u8 pad_0[0x4];              // 0x0
    s32 field_4;                // 0x4
    u8 pad_8[0xC - 0x8];        // 0x8
    s32 field_C;                // 0xC
    s32 score[3];               // 0x10
    f32 field_1C;               // 0x1C
    f32 field_20;               // 0x20
    f32 field_24;               // 0x24
    s32 field_28;               // 0x28
    s32 field_2C;               // 0x2C
    s32 field_30;               // 0x30
    EffectEntry* field_34;      // 0x34
    EffectEntry* field_38;      // 0x38
    EffectEntry* field_3C;      // 0x3C
    s32 field_40;               // 0x40
    s32 field_44;               // 0x44
    s32 field_48;               // 0x48
    s32 field_4C;               // 0x4C
    s32 field_50;               // 0x50
    s32 field_54;               // 0x54
    s32 field_58;               // 0x58
    s32* field_5C;              // 0x5C
    s32 field_60;               // 0x60
    s32 field_64;               // 0x64
    s32 field_68;               // 0x68
    f32 field_6C;               // 0x6C
    f32 field_70;               // 0x70
    s32 field_74;               // 0x74
    s32 field_78;               // 0x78
    s32 field_7C;               // 0x7C
    GenkiEffect field_80[7];    // 0x80
    GenkiWeapon weapon[7];      // 0x10C
    GenkiObject objects[10];    // 0x358
    GenkiStar stars[7];         // 0x510
} GenkiWork;

// The whole file was compiled with -O4,s (optimize for size); without this every constant-trip
// loop is unrolled and nothing matches. configure.py should carry extra_cflags=["-O4,s"] for it.
#pragma optimize_for_size on
// The original file never inlines its own helpers: every call is a real `bl`.
#pragma dont_inline on

static GenkiWork* get_ptr(void);

USER_FUNC(recover_cmd_disable) {
    u32* p = (u32*)((u8*)_battleWorkPointer + 0x19038);

    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    p[4] = 0;
    return 2;
}

USER_FUNC(star_stone_appear) {
    GenkiWork* genki = get_ptr();

    genki->stars[0].field_0 = 1;
    genki->stars[0].field_8 = 1;
    return 2;
}

USER_FUNC(star_appear) {
    GenkiWork* genki = get_ptr();
    s32 i;

    for (i = 0; i < 7; i++) {
        GenkiStar* star = &genki->stars[i];

        star->field_0 = 1;
        star->field_8 = 0;
    }
    return 2;
}

USER_FUNC(status_on) {
    statusWinOpen();
    return 2;
}

USER_FUNC(get_score) {
    GenkiWork* genki = get_ptr();
    s32* args = event->args;

    evtSetValue(event, args[0], genki->score[0]);
    evtSetValue(event, args[1], genki->score[1]);
    evtSetValue(event, args[2], genki->score[2]);
    return 2;
}

USER_FUNC(wait_game_end) {
    GenkiWork* genki = get_ptr();

    return (genki->field_4 == 3) ? 2 : 0;
}

USER_FUNC(start_game) {
    GenkiWork* genki = get_ptr();

    genki->field_4 = 1;
    return 2;
}

s32 object_get_num(void) {
    GenkiWork* genki = get_ptr();
    s32 i = 0;
    s32 count = 0;

    for (; i < 10; i++) {
        if (genki->objects[i].field_0 == 2) {
            count++;
        }
    }
    return count;
}

void object_entry(void) {
    GenkiWork* genki = get_ptr();
    s32 i;

    for (i = 0; i < 10; i++) {
        GenkiObject* obj = &genki->objects[i];

        if (obj->field_0 == 0) {
            obj->field_0 = 1;
            break;
        }
    }
}

// The trailing block scope puts mario/parts in r31 and genki in r29, matching the original;
// at function scope MWCC cycles those two callee-saved registers.
void weapon_entry(void) {
    GenkiWork* genki = get_ptr();
    s32 offsets[10];
    s32 found = 0;
    s32 i;
    s32 idx;
    s32 count;

    {
        BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
        BattleUnitPart* parts = BtlUnit_GetPartsPtr(mario, BtlUnit_GetBodyPartsId(mario));

        count = genki->field_58;
        for (i = 0; i < count; i++) {
            if (genki->weapon[i].field_0 == 0) {
                offsets[found++] = i;
            }
        }
        if (found >= 1) {
            idx = offsets[irand(found)];
            {
                GenkiWeapon* wep = &genki->weapon[idx];

                wep->field_0 = 1;
                wep->field_4 = 7;
            }
            BtlUnit_SetAnim(parts, str_8030045C);
        }
    }
}

#pragma dont_inline on
static GenkiWork* get_ptr(void) {
    return (GenkiWork*)((u8*)_battleWorkPointer + 0x1F4C);
}
#pragma dont_inline reset

static void disp_3D_alpha(CameraId camera, void* param) {
}

static void disp_2D(CameraId camera, void* param) {
}

// The target's `offset`/`woffset` are MWCC's strength-reduced induction variables for
// `&genki->stars[i]`/`&genki->weapon[i]`; no explicit offset locals.
USER_FUNC(end_genki) {
    GenkiWeapon* weapon;
    s32 i;
    GenkiWork* genki = get_ptr();
    GenkiStar* star;
    s32 j;

    for (i = 0; i < 7; i++) {
        star = &genki->stars[i];
        if (star->field_60 != NULL) {
            effDelete(*(EffectEntry**)((u8*)star->field_60->userdata + 0x34));
            effDelete(star->field_60);
        }
    }
    for (i = 0; i < 7; i++) {
        weapon = &genki->weapon[i];
        if (weapon->field_44 != NULL) {
            effDelete(*(EffectEntry**)((u8*)weapon->field_44->userdata + 0x34));
            effDelete(weapon->field_44);
            for (j = 0; j < 3; j++) {
                effDelete(*(EffectEntry**)((u8*)weapon->field_48[j]->userdata + 0x34));
                effDelete(weapon->field_48[j]);
            }
        }
    }
    effDelete(genki->field_34);
    effDelete(genki->field_38);
    effDelete(genki->field_3C);
    BattleFree(genki->field_5C);
    return 2;
}

static void main_star(void) {
    GenkiWork* genki = get_ptr();
    s32 i;

    if (genki->field_C == 0) {
        for (i = 0; i < 7; i++) {
            main_star0(i);
        }
    } else {
        main_star1();
    }
}

void main_target(void) {
    GenkiWork* genki = get_ptr();

    switch (genki->field_64) {
    case 0:
        break;
    case 1:
        genki->field_68 = 0;
        genki->field_6C = float_30_80421d80;
        genki->field_70 = float_1p25_80421d6c;
        genki->field_64 = 2;
    case 2:
        genki->field_68++;
        if (genki->field_68 >= 0x168) {
            genki->field_68 = (s32)((f32)genki->field_68 - float_360_80421de8);
        }
        genki->field_6C += genki->field_70;
        if (genki->field_6C >= float_60_80421d50) {
            genki->field_6C = float_60_80421d50;
            genki->field_70 = float_neg1p25_80421e0c;
            genki->field_64 = 3;
        }
        break;
    case 3:
        genki->field_68++;
        if (genki->field_68 >= 0x168) {
            genki->field_68 = (s32)((f32)genki->field_68 - float_360_80421de8);
        }
        genki->field_6C += genki->field_70;
        if (genki->field_6C <= float_0_80421d84) {
            genki->field_6C = float_0_80421d84;
            genki->field_70 = float_1p25_80421d6c;
            genki->field_64 = 2;
        }
        break;
    case 4:
        break;
    }
}

void main_mario(void) {
    GenkiWork* genki = get_ptr();
    s32 stickX = (s8)keyGetStickX(0);
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    BattleUnitPart* parts = BtlUnit_GetPartsPtr(mario, BtlUnit_GetBodyPartsId(mario));

    if (genki->field_44 >= 1 && genki->field_44 <= 4 && genki->field_40 <= 0 && object_get_num() == 0) {
        genki->field_44 = 5;
        genki->field_64 = 4;
        BtlUnit_SetAnim(parts, str_80421E10);
    }
    switch (genki->field_44) {
    case 0:
        break;
    case 1:
        if (stickX < -0x28) {
            genki->field_44 = 2;
            genki->field_48 = 0;
            genki->field_54 = 10;
            BtlUnit_SetAnim(parts, str_8030048C);
        }
        break;
    case 2:
        if (stickX > -0x14) {
            genki->field_44 = 1;
            weapon_entry();
        } else {
            genki->field_54--;
            if (genki->field_54 == 2) {
                BtlUnit_SetAnim(parts, str_80300494);
            }
            if (genki->field_54 <= 0 && genki->field_C == 1) {
                genki->field_54 = 10;
                weapon_entry();
            }
        }
        break;
    case 3:
        genki->field_44 = 4;
        genki->field_48 = 0;
        BtlUnit_SetAnim(parts, str_80421E18);
        {
            f32 x, y, z;
            EffectEntry* eff;
            BtlUnit_GetPos(mario, &x, &y, &z);
            eff = effStampN64Entry(2, x, float_20_80421d78 + y, float_10_80421d54 + z);
            ((u8*)eff->userdata)[0x38] = 0;
            ((u8*)eff->userdata)[0x39] = 0xC8;
            ((u8*)eff->userdata)[0x3A] = 0;
            ((u8*)eff->userdata)[0x3B] = 0;
            ((u8*)eff->userdata)[0x3C] = 0xA;
            ((u8*)eff->userdata)[0x3D] = 0;
        }
    case 4:
        genki->field_48++;
        if (genki->field_48 == 0x1E) {
            BtlUnit_SetAnim(parts, str_80421E20);
        }
        if (genki->field_48 == 0xB4) {
            BtlUnit_SetAnim(parts, str_80421D3C);
            genki->field_44 = 1;
            genki->field_48 = 0;
        }
        break;
    case 5:
        break;
    }
}

// The file's incomplete .rodata pool (same limitation as main_star1) makes MWCC emit the
// int->float magic double and the two switch jump tables as anonymous `@...@` entries instead of
// double_to_int_80300468 / jumptable_803B3C40 / jumptable_803B3C24. All other differences are
// register allocation and instruction scheduling.
// NONMATCHING (94.81%): the anonymous pool entries listed above.
void main_weapon(s32 idx) {
    const f32* vec = vec3_803002b0;
    GenkiWork* genki = get_ptr();
    GenkiWeapon* weapon = &genki->weapon[idx];
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 angle = float_3p1416_80421d48 * (float_2_80421d70 * genki->field_6C) / float_360_80421de8;
    s32 audienceNum = BattleAudience_GetAudienceNum();
    s32 i;

    for (i = 2; i > 0; i--) {
        weapon->pos[i + 1] = weapon->pos[i];
    }
    weapon->pos[1] = weapon->pos[0];

    switch (weapon->field_0) {
    case 0:
        weapon->pos[0] = *(const Vec*)&vec[0];
        break;
    case 1:
        weapon->field_4--;
        if (weapon->field_4 >= 0) {
            break;
        }
        weapon->field_0 = 2;
        BtlUnit_GetPos(mario, &weapon->pos[0].x, &weapon->pos[0].y, &weapon->pos[0].z);
        weapon->pos[0].x += float_20_80421d78 * mario->mSizeMultiplier;
        weapon->pos[0].y += float_28_80421dec * mario->mSizeMultiplier;
        {
            Vec vel = vec3_803002bc;

            vel.x = float_10_80421d54 * (f32)cos(angle);
            vel.y = float_10_80421d54 * (f32)sin(angle);
            weapon->vel = vel;
        }
        for (i = 0; i < 7; i++) {
            GenkiEffect* eff = &genki->field_80[i];

            if (eff->field_0 <= 0) {
                eff->field_0 = 0x1E;
                eff->field_4 = genki->field_74;
                eff->field_8 = genki->field_78;
                eff->field_C = genki->field_7C;
                eff->field_10 = float_2_80421d70 * (f32)genki->field_68;
                break;
            }
        }
        BtlUnit_snd_se(mario, str_80300470, 0xF1194D80, 0);
        /* fallthrough */
    case 2:
        weapon->pos[0].x += weapon->vel.x;
        weapon->pos[0].y += weapon->vel.y;
        weapon->vel.y -= float_0p15_80421df0;
        {
            f32 x, y, z;

            BtlUnit_GetPos(mario, &x, &y, &z);
            if (weapon->pos[0].x >= float_250_80421df4 || weapon->pos[0].y >= float_180_80421d4c ||
                weapon->pos[0].y <= y) {
                weapon->field_0 = 3;
                break;
            }
            {
                GenkiObject* obj;
                f32 best = float_1E07_80421df8;
                s32 found = -1;
                f32 wx = weapon->pos[0].x;
                f32 wy = weapon->pos[0].y;

                for (i = 0; i < 10; i++) {
                    obj = &genki->objects[i];

                    if (obj->field_0 == 2) {
                        f32 dy = wy - obj->field_10;
                        f32 dx = wx - obj->field_C;
                        f32 d = dy * dy + dx * dx;

                        if (d < best) {
                            best = d;
                            found = i;
                        }
                    }
                }
                if (found == -1) {
                    break;
                }
                obj = &genki->objects[found];
                {
                    f32 threshold;

                    switch (obj->field_8) {
                    case 6:
                        threshold = float_344_80421dfc;
                        break;
                    case 0:
                    case 2:
                    case 4:
                        threshold = float_576_80421e00;
                        break;
                    default:
                        threshold = float_1296_80421e04;
                        break;
                    }
                    if (best <= threshold && obj->field_10 <= float_200_80421d9c) {
                        weapon->field_0 = 3;
                        obj->field_0 = 3;
                effStardustN64Entry(5, obj->field_C, obj->field_10, obj->field_14,
                                    float_10_80421d54);
                effHitEntry(0, 5, obj->field_C, obj->field_10, obj->field_14, float_1_80421d74);
                switch (obj->field_8) {
                case 0:
                    psndSFXOn_3D((const char*)0xA64, &weapon->pos[0]);
                    genki->score[0] += 1;
                    if (genki->score[0] >= 99) {
                        genki->score[0] = 99;
                    }
                    break;
                case 1:
                    psndSFXOn_3D((const char*)0xA66, &weapon->pos[0]);
                    genki->score[0] += 3;
                    if (genki->score[0] >= 99) {
                        genki->score[0] = 99;
                    }
                    break;
                case 2:
                    psndSFXOn_3D((const char*)0xA64, &weapon->pos[0]);
                    genki->score[1] += 1;
                    if (genki->score[1] >= 99) {
                        genki->score[1] = 99;
                    }
                    break;
                case 3:
                    psndSFXOn_3D((const char*)0xA66, &weapon->pos[0]);
                    genki->score[1] += 3;
                    if (genki->score[1] >= 99) {
                        genki->score[1] = 99;
                    }
                    break;
                case 4:
                    psndSFXOn_3D((const char*)0xA65, &weapon->pos[0]);
                    genki->score[2] += 1;
                    if (genki->score[2] >= 99) {
                        genki->score[2] = 99;
                    }
                    break;
                case 5:
                    psndSFXOn_3D((const char*)0xA67, &weapon->pos[0]);
                    genki->score[2] += 3;
                    if (genki->score[2] >= 99) {
                        genki->score[2] = 99;
                    }
                    break;
                case 6:
                    psndSFXOn_3D((const char*)0xA68, &weapon->pos[0]);
                    genki->field_44 = 3;
                    break;
                }
                switch (obj->field_8) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    if (audienceNum >= 1 && audienceNum <= 0x31) {
                        BattleAudienceSoundCallKind(1);
                    }
                    if (audienceNum >= 0x32 && audienceNum <= 0x63) {
                        BattleAudienceSoundCallKind(1);
                        BattleAudienceSoundHandBeat();
                    }
                    if (audienceNum >= 0x64 && audienceNum <= 0x95) {
                        BattleAudienceSoundCallKind(2);
                    }
                    if (audienceNum >= 0x96) {
                        BattleAudienceSoundCallKind(2);
                        BattleAudienceSoundHandBeat();
                    }
                    break;
                case 6:
                    if (audienceNum >= 1 && audienceNum <= 0x31) {
                        BattleAudienceSoundBooingKind(1);
                    }
                    if (audienceNum >= 0x32 && audienceNum <= 0x63) {
                        BattleAudienceSoundBooingKind(1);
                    }
                    if (audienceNum >= 0x64 && audienceNum <= 0x95) {
                        BattleAudienceSoundBooingKind(2);
                    }
                    if (audienceNum >= 0x96) {
                        BattleAudienceSoundBooingKind(3);
                    }
                    break;
                }
                }
            }
        }
        }
        break;
    case 3:
        weapon->field_0 = 0;
        break;
    }

    {
        f32 scale = float_1p125_80421e08;

        if (weapon->field_44 != NULL) {
            effStarStoneData* data = (effStarStoneData*)weapon->field_44->userdata;

            data->field_0x8 = weapon->pos[0].x;
            data->field_0xC = weapon->pos[0].y;
            data->field_0x10 = weapon->pos[0].z;
            data->field_0x14 = scale;
            data->field_0x5F = 0xFF;
        }
        for (i = 0; i < 3; i++) {
            effStarStoneData* data = (effStarStoneData*)weapon->field_48[i]->userdata;

            data->field_0x8 = weapon->pos[i + 1].x;
            data->field_0xC = weapon->pos[i + 1].y;
            data->field_0x10 = weapon->pos[i + 1].z;
            data->field_0x14 = scale;
            data->field_0x5F = 0xFF - (i + 1) * 0x32;
        }
    }
}

// Matches. The file's incomplete .rodata pool (see main_weapon) leaves the int->float magic double
// as an anonymous `@693@` and the 30.0f literal as `@959@` instead of the named
// `double_to_int_80300468` / `float_30_80421d80`; objdiff does not score those symbol names. The
// `30.0f` literal, rather than the named `float_30_80421d80` (same value), is what makes MWCC emit
// the original's interleaved `li`/`lfs` argument setup. Keeping `fifteen` in a variable fixed the
// compare's f1/f2 assignment; the if/else that builds `limit` fixed the case-1 integer register
// allocation.
void main_object(s32 idx) {
    GenkiWork* genki = get_ptr();
    GenkiObject* obj = (GenkiObject*)((u8*)genki + 0x358 + idx * 0x2C);
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    s32 local[3];
    GenkiStar* star;
    f32 x, y, z;

    local[0] = dat_803002c8[0];
    local[1] = dat_803002c8[1];
    local[2] = dat_803002c8[2];

    switch (obj->field_0) {
    case 0:
        break;
    case 1:
        obj->field_0 = 2;
        obj->field_4 = 0;
        {
            s32 limit;
            if (genki->field_C == 0) {
                limit = 0x19;
            } else {
                limit = 0x32;
            }
            if (genki->field_60 < limit) {
                obj->field_8 = genki->field_5C[genki->field_60];
            } else {
                s32 r = irand(genki->field_C == 0 ? 7 : 0xE);
                if (r == 0) {
                    obj->field_8 = 6;
                } else {
                    if (genki->field_50 != -1) {
                        obj->field_8 = local[irand(3)];
                    } else {
                        obj->field_8 = local[irand(2)];
                    }
                    if (irand(5) == 0 && genki->field_C == 1) {
                        switch (obj->field_8) {
                        case 0:
                            obj->field_8 = 1;
                            break;
                        case 2:
                            obj->field_8 = 3;
                            break;
                        case 4:
                            obj->field_8 = 5;
                            break;
                        }
                    }
                }
            }
        }
        if (genki->field_50 == -1 && obj->field_8 == 2) {
            obj->field_8 = 0;
        }
        if (genki->field_50 == -1 && obj->field_8 == 3) {
            obj->field_8 = 1;
        }
        genki->field_60++;
        if (genki->field_C == 0) {
            star = &genki->stars[irand(7)];
        } else {
            star = &genki->stars[0];
        }
        {
            BtlUnit_GetPos(mario, &x, &y, &z);
            obj->field_C = star->field_C;
            obj->field_10 = star->field_10;
            obj->field_14 = z + (f32)idx;
        }
    case 2:
        if (genki->field_C == 0) {
            obj->field_10 -= float_1p32_80421ddc;
        } else {
            obj->field_10 -= float_1p98_80421de0;
        }
        {
            BtlUnit_GetPos(mario, &x, &y, &z);
            {
                f32 fifteen = float_15_80421de4;
                if (obj->field_10 <= fifteen + y) {
                    f32 thirty = 30.0f;
                    obj->field_0 = 3;
                    effStardustEntry(5, 0xA, 0x3C, obj->field_C, fifteen + y, obj->field_14,
                                     thirty, thirty);
                }
            }
        }
        obj->field_4++;
        if (obj->field_4 > 0x78) {
            obj->field_4 = 0;
        }
        break;
    case 3:
        obj->field_0 = 0;
        break;
    }
}

// Matches: the original schedules each effRecoveryEntry's argument setup the way mwcc does for
// known constants (x and the count are hoisted above the previous call's result store). It only
// does that when it can treat the values as literals, so the MUST_MATCH branch passes the two
// values inline (identical to float_0_80421d84 / float_neg1000_80421d88); the natural extern
// form is under #else.
s32 main_genki(EventEntry* event, BOOL isFirstCall) {
    GenkiWork* genki = get_ptr();
    s32 i;
    s32 j;
    s32* args;

    if (isFirstCall) {
        args = event->args;
        memset(genki, 0, 0x7CC);
        genki->field_50 = evtGetValue(event, args[0]);
        genki->field_C = evtGetValue(event, args[1]);
        {
            f32 zero = float_0_80421d84;

            genki->field_40 = 0x384;
            genki->score[2] = 0;
            genki->score[1] = 0;
            genki->score[0] = 0;
            genki->field_24 = zero;
            genki->field_20 = zero;
            genki->field_1C = zero;
        }
        if (genki->field_C == 0) {
            genki->field_58 = 3;
        } else {
            genki->field_58 = 7;
        }
        if (genki->field_C == 0) {
            genki->field_5C = (s32*)BattleAlloc(0x64);
        } else {
            genki->field_5C = (s32*)BattleAlloc(0xC8);
        }
#ifdef MUST_MATCH
        genki->field_34 = effRecoveryEntry(2, 0, 0.0f, -1000.0f, 0.0f);
        genki->field_38 = effRecoveryEntry(2, 0, 0.0f, -1000.0f, 0.0f);
        genki->field_3C = effRecoveryEntry(3, 0, 0.0f, -1000.0f, 0.0f);
#else
        genki->field_34 = effRecoveryEntry(2, 0, float_0_80421d84, float_neg1000_80421d88, float_0_80421d84);
        genki->field_38 = effRecoveryEntry(2, 0, float_0_80421d84, float_neg1000_80421d88, float_0_80421d84);
        genki->field_3C = effRecoveryEntry(3, 0, float_0_80421d84, float_neg1000_80421d88, float_0_80421d84);
#endif
        for (i = 0; i < 7; i++) {
            genki->weapon[i].field_44 = effStarStoneEntry(i, float_0_80421d84, float_neg1000_80421d88,
                                                          float_0_80421d84, float_1_80421d74);
            for (j = 0; j < 3; j++) {
                genki->weapon[i].field_48[j] = effStarStoneEntry(i + 7, float_0_80421d84,
                                                                 float_neg1000_80421d88,
                                                                 float_0_80421d84, float_1_80421d74);
            }
        }
    }
    main_base();
    main_mario();
    main_target();
    for (i = 0; i < genki->field_58; i++) {
        main_weapon(i);
    }
    for (i = 0; i < 10; i++) {
        main_object(i);
    }
    main_star();
    dispEntry(8, float_901_80421e28, 1, disp_2D, NULL);
    dispEntry(4, float_0_80421d84, 1, disp_3D, NULL);
    dispEntry(4, float_0_80421d84, 2, disp_3D_alpha, NULL);
    return 0;
}

static void main_base(void) {
    GenkiWork* genki = get_ptr();
    s32 i;
    s32 a;
    s32 b;
    s32 t;
    s32 k;

    if (genki->field_1C > (f32)genki->score[0]) {
        genki->field_1C -= float_1_80421d74;
        genki->field_28 = 0x3C;
    }
    if (genki->field_1C < (f32)genki->score[0]) {
        genki->field_1C += float_1_80421d74;
        genki->field_28 = 0x3C;
    }
    if (genki->field_20 > (f32)genki->score[1]) {
        genki->field_20 -= float_1_80421d74;
        genki->field_2C = 0x3C;
    }
    if (genki->field_20 < (f32)genki->score[1]) {
        genki->field_20 += float_1_80421d74;
        genki->field_2C = 0x3C;
    }
    if (genki->field_24 > (f32)genki->score[2]) {
        genki->field_24 -= float_1_80421d74;
        genki->field_30 = 0x3C;
    }
    if (genki->field_24 < (f32)genki->score[2]) {
        genki->field_24 += float_1_80421d74;
        genki->field_30 = 0x3C;
    }
    if (genki->field_28 > 0) {
        genki->field_28--;
    }
    if (genki->field_2C > 0) {
        genki->field_2C--;
    }
    if (genki->field_30 > 0) {
        genki->field_30--;
    }

    switch (genki->field_4) {
    case 0:
        break;
    case 1:
        genki->field_4 = 2;
        genki->field_44 = 1;
        genki->field_64 = 1;
        genki->field_60 = 0;
        if (genki->field_C == 0) {
            k = 0;
            for (i = 0; i < 7; i++) {
                genki->field_5C[k++] = 0;
            }
            for (i = 0; i < 7; i++) {
                genki->field_5C[k++] = 2;
            }
            for (i = 0; i < 8; i++) {
                genki->field_5C[k++] = 4;
            }
            for (i = 0; i < 3; i++) {
                genki->field_5C[k++] = 6;
            }
            for (i = 0; i < 200; i++) {
                a = irand(25);
                b = irand(25);
                t = genki->field_5C[a];
                genki->field_5C[a] = genki->field_5C[b];
                genki->field_5C[b] = t;
            }
        } else {
            k = 0;
            for (i = 0; i < 12; i++) {
                genki->field_5C[k++] = 0;
            }
            for (i = 0; i < 4; i++) {
                genki->field_5C[k++] = 1;
            }
            for (i = 0; i < 12; i++) {
                genki->field_5C[k++] = 2;
            }
            for (i = 0; i < 4; i++) {
                genki->field_5C[k++] = 3;
            }
            for (i = 0; i < 12; i++) {
                genki->field_5C[k++] = 4;
            }
            for (i = 0; i < 4; i++) {
                genki->field_5C[k++] = 5;
            }
            for (i = 0; i < 2; i++) {
                genki->field_5C[k++] = 6;
            }
            for (i = 0; i < 200; i++) {
                a = irand(50);
                b = irand(50);
                t = genki->field_5C[a];
                genki->field_5C[a] = genki->field_5C[b];
                genki->field_5C[b] = t;
            }
        }
        /* fallthrough */
    case 2:
        genki->field_4C--;
        if (genki->field_4C <= 0) {
            if (genki->field_C == 0) {
                genki->field_4C = 0x25;
            } else {
                genki->field_4C = 0x12;
            }
            if (genki->field_40 > 0) {
                object_entry();
            }
        }
        if (genki->field_40 <= 0) {
            s32 ocount;
            s32 wcount = 0;
            for (i = 0; i < genki->field_58; i++) {
                if (genki->weapon[i].field_0 == 0) {
                    wcount++;
                } else {
                    break;
                }
            }
            ocount = 0;
            for (i = 0; i < 10; i++) {
                if (genki->objects[i].field_0 == 0) {
                    ocount++;
                } else {
                    break;
                }
            }
            if (wcount == genki->field_58 && ocount == 10) {
                genki->field_4 = 3;
            }
        } else {
            genki->field_40--;
        }
        break;
    case 3:
        break;
    }
}

// The callee-saved register assignment is shifted (star/vec/genki land in r29/r30/r31
// here, r30/r31/r27 in the original) and the int->float magic double is an anonymous
// local loaded with its own `lis` instead of the original's vec3+0x1b8 offset; the
// intplGetValue argument setup is ordered differently.
// NONMATCHING (87.67%): register assignment and the pooled magic double/schedule.
// Logic and constants match the assembly.
void main_star0(s32 idx) {
    const f32* vec = vec3_803002b0;
    GenkiWork* genki = get_ptr();
    GenkiStar* star = &genki->stars[idx];
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 x, y, z;
    f32 f;

    switch (star->field_0) {
    case 0:
        break;
    case 1:
        star->field_0 = 2;
        star->field_4 = 0;
        star->field_60 = effStarStoneEntry(idx, 0.0f, -1000.0f, 0.0f, 1.0f);
        BtlUnit_GetPos(mario, &star->field_C, &star->field_10, &star->field_14);
        star->field_10 = star->field_10 +
            (mario->mSizeMultiplier * (f32)mario->mHeight + 37.0f);
        *(Vec*)&star->field_18 = *(Vec*)&star->field_C;
        *(Vec*)&star->field_24 = *(Vec*)&star->field_C;
        star->field_28 += 30.0f;
        star->field_58 = 6.283184f * (f32)idx / 7.0f;
        *(Vec*)&star->field_3C = *(const Vec*)&vec[0x24 / 4];
        *(Vec*)&star->field_48 = *(const Vec*)&vec[0x30 / 4];
        star->field_54 = 0.0f;
        star->field_5C = 0.1f;
        /* fallthrough */
    case 2:
        star->field_4++;
        if (star->field_4 <= 0x64) {
            star->field_10 = intplGetValue(0, star->field_4, 0x64, star->field_1C, star->field_28);
            BtlUnit_GetPos(mario, &x, &y, &z);
            star->field_54 = intplGetValue(0, star->field_4, 0x64, 0.0f,
                                           40.0f);
            star->field_58 += star->field_5C;
            star->field_C = x + star->field_54 * (f32)cos(star->field_58);
            star->field_14 = z + star->field_54 * (f32)sin(star->field_58);
            f = intplGetValue(0, star->field_4, 0x64, 0.0f, 0.75f);
            star->field_44 = f;
            star->field_40 = f;
            star->field_3C = f;
        } else {
            star->field_10 = star->field_28;
            BtlUnit_GetPos(mario, &x, &y, &z);
            star->field_58 += star->field_5C;
            star->field_C = x + star->field_54 * (f32)cos(star->field_58);
            star->field_14 = z + star->field_54 * (f32)sin(star->field_58);
            star->field_44 = 0.75f;
            star->field_40 = 0.75f;
            star->field_3C = 0.75f;
        }
        star->field_4C = intplGetValue(4, star->field_4, 0x78, 0.0f,
                                       2160.0f);
        if (star->field_4 >= 0x78) {
            Vec v;

            star->field_0 = 3;
            star->field_4 = 0;
            *(Vec*)&star->field_18 = *(Vec*)&star->field_C;
            v = *(const Vec*)&vec[0x3C / 4];
            v.x = 200.0f * (f32)rand() / 32767.0f;
            v.z = star->field_20;
            *(Vec*)&star->field_24 = v;
            *(Vec*)&star->field_30 = *(const Vec*)&vec[0x48 / 4];
            star->field_34 = (2.0f * (star->field_28 - star->field_1C) -
                              (60.0f * 60.0f *
                               (0.2f - 0.01f * (f32)idx))) /
                             120.0f;
        }
        break;
    case 3:
        star->field_4++;
        star->field_C = intplGetValue(4, star->field_4, 0x3C, star->field_18, star->field_24);
        star->field_10 += star->field_34;
        star->field_34 += 0.2f - 0.01f * (f32)idx;
        star->field_4C = 0.0f;
        star->field_44 = 0.75f;
        star->field_40 = 0.75f;
        star->field_3C = 0.75f;
        if (star->field_4 >= 0x3C) {
            star->field_0 = 4;
            star->field_4 = 0;
            if (irand(2) == 0) {
                star->field_30 = -2.0f - 0.05f * (f32)idx;
            }
            star->field_30 = 2.0f + 0.05f * (f32)idx;
            star->field_34 = 0.0f;
        }
        break;
    case 4:
        star->field_C += star->field_30;
        if (star->field_C < 0.0f) {
            star->field_30 = 3.0f + 0.05f * (f32)idx;
        }
        if (star->field_C > 200.0f) {
            star->field_30 = -3.0f - 0.05f * (f32)idx;
        }
        star->field_34 += 0.1f + 0.01f * (f32)idx;
        if (star->field_34 >= 6.283184f) {
            star->field_34 -= 6.283184f;
        }
        star->field_10 = 10.0f * (f32)sin(star->field_34) + 180.0f;
        if (genki->field_40 <= 0) {
            star->field_0 = 5;
            star->field_4 = 0;
            *(Vec*)&star->field_C = *(const Vec*)&vec[0x54 / 4];
        }
        break;
    case 5:
        break;
    }

    if (star->field_60 != NULL) {
        f32* p = (f32*)star->field_60->userdata;
        p[0x8 / 4] = star->field_C;
        p[0xC / 4] = star->field_10;
        p[0x10 / 4] = star->field_14;
        p[0x18 / 4] = star->field_48;
        p[0x1C / 4] = star->field_4C;
        p[0x20 / 4] = star->field_50;
        p[0x14 / 4] = 1.5f * star->field_3C;
    }
}

// The int->float magic double is pooled as an anonymous local and loaded through a
// fresh `lis` instead of the original's `lfd 0x1b8(r30)`; the intplGetValue argument
// setup orders the field_4 load before the two float loads where the original loads it
// last. Logic and constants match the assembly.
// NONMATCHING (95.37%): the pooled magic double and intplGetValue arg ordering.
void main_star1(void) {
    const f32* vec = vec3_803002b0;
    GenkiWork* genki = get_ptr();
    GenkiStar* star = &genki->stars[0];
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 f;

    switch (star->field_0) {
    case 0:
        break;
    case 1:
        star->field_0 = 2;
        star->field_4 = 0;
        star->field_60 = effStarStoneEntry(4, float_0_80421d84, float_neg1000_80421d88, float_0_80421d84, float_1_80421d74);
        BtlUnit_GetPos(mario, &star->field_C, &star->field_10, &star->field_14);
        star->field_10 = star->field_10 +
            (mario->mSizeMultiplier * (f32)mario->mHeight + float_37_80421d8c);
        *(Vec*)&star->field_18 = *(Vec*)&star->field_C;
        *(Vec*)&star->field_24 = *(Vec*)&star->field_C;
        star->field_28 += float_50_80421d90;
        star->field_2C += float_neg1_80421d94;
        *(Vec*)&star->field_3C = *(const Vec*)&vec[0x60 / 4];
        *(Vec*)&star->field_48 = *(const Vec*)&vec[0x6C / 4];
        /* fallthrough */
    case 2:
        star->field_4++;
        if (star->field_4 <= 0x64) {
            star->field_C = intplGetValue(0, star->field_4, 0x64, star->field_18, star->field_24);
            star->field_10 = intplGetValue(0, star->field_4, 0x64, star->field_1C, star->field_28);
            star->field_14 = intplGetValue(0, star->field_4, 0x64, star->field_20, star->field_2C);
            f = intplGetValue(0, star->field_4, 0x64, float_0_80421d84, float_2_80421d70);
            star->field_44 = f;
            star->field_40 = f;
            star->field_3C = f;
        } else {
            star->field_C = star->field_24;
            star->field_10 = star->field_28;
            star->field_14 = star->field_2C;
            star->field_44 = float_2_80421d70;
            star->field_40 = float_2_80421d70;
            star->field_3C = float_2_80421d70;
        }
        star->field_4C = intplGetValue(4, star->field_4, 0x78, float_0_80421d84,
                                       float_2160_80421d98);
        if (star->field_4 >= 0x78) {
            Vec v;

            star->field_0 = 3;
            star->field_4 = 0;
            *(Vec*)&star->field_18 = *(Vec*)&star->field_C;
            v = *(const Vec*)&vec[0x78 / 4];
            v.x = float_200_80421d9c * (f32)rand() / float_32767_80421da0;
            v.z = star->field_20;
            *(Vec*)&star->field_24 = v;
            *(Vec*)&star->field_30 = *(const Vec*)&vec[0x84 / 4];
            star->field_34 = (float_2_80421d70 * (star->field_28 - star->field_1C) -
                              float_720_80421da4) / float_120_80421da8;
        }
        break;
    case 3:
        star->field_4++;
        star->field_C = intplGetValue(4, star->field_4, 0x3C, star->field_18, star->field_24);
        star->field_10 += star->field_34;
        star->field_34 += float_0p2_80421dac;
        star->field_4C = float_0_80421d84;
        star->field_44 = float_2_80421d70;
        star->field_40 = float_2_80421d70;
        star->field_3C = float_2_80421d70;
        if (star->field_4 >= 0x3C) {
            star->field_0 = 4;
            star->field_4 = 0;
            if (irand(2) == 0) {
                star->field_30 = float_neg2_80421db0;
            }
            star->field_30 = float_2_80421d70;
            star->field_34 = float_0_80421d84;
        }
        break;
    case 4:
        star->field_C += star->field_30;
        if (star->field_C < float_0_80421d84) {
            star->field_30 = float_3_80421db4;
        }
        if (star->field_C > float_200_80421d9c) {
            star->field_30 = float_neg3_80421db8;
        }
        star->field_34 += float_0p1_80421dbc;
        if (star->field_34 >= float_6p2832_80421dc0) {
            star->field_34 -= float_6p2832_80421dc0;
        }
        star->field_10 = float_10_80421d54 * (f32)sin(star->field_34) + float_180_80421d4c;
        if (genki->field_40 <= 0) {
            star->field_0 = 5;
            star->field_4 = 0;
            *(Vec*)&star->field_C = *(const Vec*)&vec[0x90 / 4];
        }
        break;
    case 5:
        break;
    }

    if (star->field_60 != NULL) {
        f32* p = (f32*)star->field_60->userdata;
        p[0x8 / 4] = star->field_C;
        p[0xC / 4] = star->field_10;
        p[0x10 / 4] = star->field_14;
        p[0x18 / 4] = star->field_48;
        p[0x1C / 4] = star->field_4C;
        p[0x20 / 4] = star->field_50;
        p[0x14 / 4] = float_1p5_80421dc4 * star->field_3C;
    }
}

