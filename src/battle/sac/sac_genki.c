#include <stdlib.h>
#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "battle/battle_unit.h"
#include "battle/battle_ac.h"
#include "battle/battle_disp.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "driver/dispdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "system.h"
#include <string.h>
// math_ext.h instead of <math.h>: the SDK's math.h inlines would place extra
// float constants ahead of .rodata and shift main_star1's int->float pool double
// off its 0x1B8 offset.
#include "math_ext.h"
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
// This unit's .rodata, in retail order: the vec3_803002b0...vec3_803003e8
// f32[3] table (vec3_803002bc covers 0xc..0x17), then dat_803002c8 and the
// strings. The layout only matters because MWCC folds its anonymous int->float
// pool double into the section base when a function names several of these
// objects, so the double must land at vec3_803002b0+0x1B8 (see main_star1).
const f32 vec3_803002b0[3] = {0.0f, -1000.0f, 0.0f};
const f32 vec3_803002bc[3] = {0.0f, 0.0f, 0.0f};
const s32 dat_803002c8[3] = {0, 4, 2};
const f32 vec3_803002d4[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_803002e0[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_803002ec[3] = {0.0f, 180.0f, 0.0f};
const f32 vec3_803002f8[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_80300304[3] = {0.0f, -1000.0f, 0.0f};
// Match-only: retail defines these five as const. A non-const object defeats
// MWCC's load hoisting in main_star1 (a const object is provably stable, so the
// -O4 scheduler lifts the named loads above the star stores, dropping the
// function to 88.87%); the section attribute keeps the bytes at their retail
// .rodata offsets.
__declspec(section ".rodata") f32 vec3_80300310[3] = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") f32 vec3_8030031c[3] = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") f32 vec3_80300328[3] = {0.0f, 180.0f, 0.0f};
__declspec(section ".rodata") f32 vec3_80300334[3] = {0.0f, 0.0f, 0.0f};
__declspec(section ".rodata") f32 vec3_80300340[3] = {0.0f, -1000.0f, 0.0f};
const f32 vec3_8030034c[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_80300358[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_80300364[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_80300370[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_8030037c[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_80300388[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_80300394[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_803003a0[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_803003ac[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_803003b8[3] = {0.5f, 0.5f, 1.0f};
const f32 vec3_803003c4[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_803003d0[3] = {0.0f, 0.0f, 0.0f};
const f32 vec3_803003dc[3] = {0.5f, 0.5f, 1.0f};
const f32 vec3_803003e8[3] = {0.0f, 0.0f, 0.0f};
const char str_803003F4[] = "SFX_BTL_SAC_START1";
const char str_80300408[] = "SFX_BTL_SAC_START2";
const char str_8030041C[] = "SFX_BTL_SAC_HEART_SHOOT1";
const char str_80300438[] = "SFX_HP_RECOVER_SHINE2";
const char str_80300450[] = "stardust";
const char str_8030045C[] = "M_C_2C";

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
const f32 float_5_80421d58 = 5.0f;
const f32 float_6_80421d5c = 6.0f;
const f32 float_8_80421d60 = 8.0f;
const f32 float_0p5_80421d64 = 0.5f;
const f32 float_90_80421d68 = 90.0f;
const f32 float_85_80421d7c = 85.0f;
const GXColor dat_80421d20 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_80421d24 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_80421d28 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_80421d2c = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_80421d30 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_80421d34 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_80421d38 = {0xFF, 0xFF, 0xFF, 0xFF};
const f32 float_1p32_80421ddc = 1.32f;
const f32 float_1p98_80421de0 = 1.98f;
const f32 float_15_80421de4 = 15.0f;
const f32 float_neg1000_80421d88 = -1000.0f;
const f32 float_1_80421d74 = 1.0f;
const f32 float_901_80421e28 = 901.0f;
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
// `auto_inline off` (not `dont_inline`) is what the original needs: MWCC must not auto-inline
// plain helpers such as get_ptr (every call is a real `bl`, and inlining get_ptr wrecks the
// callers), but `_drawGenkiTex`/`_drawGenkiTex2` are declared `inline` and DO inline at their
// two call sites in disp_3D, exactly as the original does.
#pragma auto_inline off

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

static GenkiWork* get_ptr(void) {
    return (GenkiWork*)((u8*)_battleWorkPointer + 0x1F4C);
}

static void disp_3D_alpha(CameraId camera, void* param) {
}

static inline void _drawGenkiTex(s32 id, GXColor color, Vec trans, Vec rot, Vec scale) {
    btlDispTex4(id, &trans, &scale, &rot, &color);
}
static inline void _drawGenkiTex2(s32 id, Vec scale, Vec rot, Vec trans, GXColor color) {
    btlDispTex4(id, &trans, &scale, &rot, &color);
}

static inline Vec _makeRot2(const f32* vec, f32 z) {
    Vec v = *(const Vec*)&vec[0x138 / 4];
    v.z = z;
    return v;
}
static inline Vec _makeTrans2(const f32* vec, f32 x, f32 y, f32 z) {
    Vec v = *(const Vec*)&vec[0x120 / 4];
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}
static inline Vec _makeRot1(const f32* vec, f32 z) {
    Vec v = *(const Vec*)&vec[0x114 / 4];
    v.z = z;
    return v;
}
static inline Vec _makeTrans1(const f32* vec, f32 x, f32 y, f32 z) {
    Vec v = *(const Vec*)&vec[0xFC / 4];
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

// NONMATCHING (94.95%): the trans `makeVec`-style inline matches the caller trans/scale slots, but
// the inlined wrapper params still land a few Vec slots low, and the magic double is a fresh `@NNN`
// not `lfd 0x1b8(r30)` (unit .rodata pool incomplete, as in main_star0/1/main_weapon). Loop regs match.
static void disp_3D(CameraId cameraId, void* param) {
    GenkiObject* obj;
    const f32* vec = vec3_803002b0;
    GenkiWork* genki = get_ptr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    BattleUnit* party = BattleGetPartyPtr(_battleWorkPointer);
    f32 angle = float_3p1416_80421d48 * genki->field_6C / float_180_80421d4c;
    Mtx mtx;
    Vec pos;
    Vec pos2;
    f32 x, y, z;
    f32 px, py, pz;
    u16 flags;
    s32 i;

    if (genki->field_4 != 0) {
        BtlUnit_GetHomePos(mario, &x, &y, &z);
        y += float_60_80421d50;
        {
            f32* ud = (f32*)genki->field_34->userdata;
            ud[2] = x;
            ud[3] = y - float_10_80421d54;
            ud[4] = z + float_5_80421d58;
        }
        {
            s32 frame;
            if (genki->field_1C >= float_10_80421d54) {
                Vec v = *(const Vec*)&vec[0x9C / 4];
                v.x = x + float_6_80421d5c;
                v.y = y;
                v.z = z + float_8_80421d60;
                pos = v;
            } else {
                Vec v = *(const Vec*)&vec[0xA8 / 4];
                v.x = x;
                v.y = y;
                v.z = z + float_8_80421d60;
                pos = v;
            }
            PSMTXTrans(mtx, pos.x, pos.y, pos.z);
            frame = genki->field_28 % 12;
            if (frame >= 0 && frame <= 5) {
                iconNumberDispGx3D(mtx, (s32)genki->field_1C, 0, *(GXColor*)&dat_80421d20);
            }
        }
        if (genki->field_50 != -1) {
            BtlUnit_GetHomePos(party, &px, &py, &pz);
            py += float_60_80421d50;
            {
                f32* ud = (f32*)genki->field_38->userdata;
                ud[2] = px;
                ud[3] = py - float_10_80421d54;
                ud[4] = pz + float_5_80421d58;
            }
            {
                s32 frame;
                if (genki->field_20 >= float_10_80421d54) {
                    Vec v = *(const Vec*)&vec[0xB4 / 4];
                    v.x = px + float_6_80421d5c;
                    v.y = py;
                    v.z = pz + float_8_80421d60;
                    pos2 = v;
                } else {
                    Vec v = *(const Vec*)&vec[0xC0 / 4];
                    v.x = px;
                    v.y = py;
                    v.z = pz + float_8_80421d60;
                    pos2 = v;
                }
                PSMTXTrans(mtx, pos2.x, pos2.y, pos2.z);
                frame = genki->field_2C % 12;
                if (frame >= 0 && frame <= 5) {
                    iconNumberDispGx3D(mtx, (s32)genki->field_20, 0, *(GXColor*)&dat_80421d24);
                }
            }
            BtlUnit_GetHomePos(mario, &x, &y, &z);
            BtlUnit_GetHomePos(party, &px, &py, &pz);
            x = float_0p5_80421d64 * (x + px);
            y += float_90_80421d68;
            {
                f32* ud = (f32*)genki->field_3C->userdata;
                ud[2] = x;
                ud[3] = y - float_10_80421d54;
                ud[4] = z + float_5_80421d58;
            }
            {
                s32 frame;
                if (genki->field_24 >= float_10_80421d54) {
                    Vec v = *(const Vec*)&vec[0xCC / 4];
                    v.x = x + float_6_80421d5c;
                    v.y = y;
                    v.z = z + float_8_80421d60;
                    pos = v;
                } else {
                    Vec v = *(const Vec*)&vec[0xD8 / 4];
                    v.x = x;
                    v.y = y;
                    v.z = z + float_8_80421d60;
                    pos = v;
                }
                PSMTXTrans(mtx, pos.x, pos.y, pos.z);
                frame = genki->field_30 % 12;
                if (frame >= 0 && frame <= 5) {
                    iconNumberDispGx3D(mtx, (s32)genki->field_24, 0, *(GXColor*)&dat_80421d28);
                }
            }
        } else {
            BtlUnit_GetHomePos(mario, &x, &y, &z);
            y += float_90_80421d68;
            {
                f32* ud = (f32*)genki->field_3C->userdata;
                ud[2] = x;
                ud[3] = y - float_10_80421d54;
                ud[4] = z + float_5_80421d58;
            }
            {
                s32 frame;
                if (genki->field_24 >= float_10_80421d54) {
                    Vec v = *(const Vec*)&vec[0xE4 / 4];
                    v.x = x + float_6_80421d5c;
                    v.y = y;
                    v.z = z + float_8_80421d60;
                    pos = v;
                } else {
                    Vec v = *(const Vec*)&vec[0xF0 / 4];
                    v.x = x;
                    v.y = y;
                    v.z = z + float_8_80421d60;
                    pos = v;
                }
                PSMTXTrans(mtx, pos.x, pos.y, pos.z);
                frame = genki->field_30 % 12;
                if (frame >= 0 && frame <= 5) {
                    iconNumberDispGx3D(mtx, (s32)genki->field_24, 0, *(GXColor*)&dat_80421d2c);
                }
            }
        }
    }

    for (i = 0; i < 10; i++) {
        obj = &genki->objects[i];

        if (obj->field_0 != 2) {
            continue;
        }
        flags = 0;
        {
            f32 scale = float_1p25_80421d6c;
            s32 icon;
            s32 frame;

            switch (obj->field_8) {
            case 0:
            case 1:
                icon = 0x1A3;
                if (obj->field_8 == 1) {
                    scale = float_2_80421d70;
                    frame = obj->field_4 % 12;
                    if (frame >= 0 && frame <= 5) {
                        flags |= 0x20;
                    }
                }
                break;
            case 2:
            case 3:
                switch (party->currentKind - 0xE0) {
                case 0:
                    icon = 0x1A4;
                    break;
                case 1:
                    icon = 0x1A5;
                    break;
                case 3:
                    icon = 0x1AE;
                    break;
                case 2:
                    switch (pouchGetPartyColor(4)) {
                    case 1:
                        icon = 0x1A8;
                        break;
                    case 2:
                        icon = 0x1A9;
                        break;
                    case 3:
                        icon = 0x1AA;
                        break;
                    case 4:
                        icon = 0x1AB;
                        break;
                    case 5:
                        icon = 0x1AC;
                        break;
                    case 6:
                        icon = 0x1AD;
                        break;
                    case 0:
                        icon = 0x1A7;
                        break;
                    }
                    break;
                case 4:
                    icon = 0x1AF;
                    break;
                case 5:
                    icon = 0x1A6;
                    break;
                case 6:
                    icon = 0x1B0;
                    break;
                default:
                    icon = 0;
                    break;
                }
                if (obj->field_8 == 3) {
                    scale = float_2_80421d70;
                    frame = obj->field_4 % 12;
                    if (frame >= 0 && frame <= 5) {
                        flags |= 0x20;
                    }
                }
                break;
            case 4:
            case 5:
                icon = 0x1B1;
                if (obj->field_8 == 5) {
                    scale = float_2_80421d70;
                    frame = obj->field_4 % 12;
                    if (frame >= 0 && frame <= 5) {
                        flags |= 0x20;
                    }
                }
                break;
            case 6:
                icon = 0x143;
                break;
            }
            PSMTXScale(mtx, scale, scale, float_1_80421d74);
            PSMTXTransApply(mtx, mtx, obj->field_C,
                            obj->field_10 - float_20_80421d78 * scale, obj->field_14);
#ifdef MUST_MATCH
            // The original passes `flags` without a sign extension; with the header's s16
            // parameter MWCC emits `extsh`, so call it through a u16 prototype.
            ((void (*)(Mtx, u16, s32, GXColor))iconDispGxCol)(mtx, flags, icon, *(GXColor*)&dat_80421d30);
#else
            iconDispGxCol(mtx, flags, icon, *(GXColor*)&dat_80421d30);
#endif
        }
    }

    if (genki->field_64 >= 2 && genki->field_64 <= 3) {
        Mtx mtx2;

        PSMTXScale(mtx2, float_0p5_80421d64, float_0p5_80421d64, float_0p5_80421d64);
        BtlUnit_GetPos(mario, &x, &y, &z);
        x += float_85_80421d7c * (f32)cos(angle);
        y += float_85_80421d7c * (f32)sin(angle);
        {
            Vec trans;
            Vec rot;

            x += float_10_80421d54;
            y += float_30_80421d80;
            z += float_10_80421d54;
            rot = *(const Vec*)&vec[0x114 / 4];
            rot.z = float_6_80421d5c * (f32)genki->field_68;
            trans = _makeTrans1(vec, x, y, z);
            _drawGenkiTex(0x41, *(GXColor*)&dat_80421d34, trans, rot, *(const Vec*)&vec[0x108 / 4]);
        }
        {
            Vec trans;
            Vec rot;

            rot = *(const Vec*)&vec[0x138 / 4];
            rot.z = float_6_80421d5c * -(f32)genki->field_68;
            trans = _makeTrans2(vec, x, y, z);
            _drawGenkiTex2(0x42, *(const Vec*)&vec[0x12C / 4], rot, trans, *(GXColor*)&dat_80421d38);
        }
    }
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

// The file's incomplete .rodata pool (same limitation as main_star1) means MWCC does not fold the
// int->float magic double into the vec3_803002b0 pool base: it emits a fresh `lis`/`lfd` at a
// separate anonymous `@NNN` pool entry instead of the original's `lfd 0x1b8(r29)`. That `lis`
// clobbers r3 and drags the surrounding `lwz 0x74/0x78(r30)` register choice with it. The
// `str_80300470` load is matched with the vec3_803002b0+0x1C0 base (MUST_MATCH below). Both pool
// offsets only come out once the vec3/string entries from disp_3D are in the unit; the missing
// 0x180 of other functions' .rodata is what puts the double at vec3+0x1b8. The switch jump tables
// are anonymous `@...@` too, but those are symbol-only (`~~`) and resolve when the unit links.
// NONMATCHING (99.27%): only the double's pool base (the fresh `lis @NNN`; 6 instructions).
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
#ifdef MUST_MATCH
            // Match-only: retail reads this Vec through the vec3_803002b0 base register.
            Vec vel = *(const Vec*)&vec[0xC / 4];
#else
            Vec vel = *(const Vec*)vec3_803002bc;
#endif

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
#ifdef MUST_MATCH
        // Match-only: retail reads the sound name through the vec3_803002b0 base (offset 0x1C0).
        BtlUnit_snd_se(mario, (const char*)((const u8*)vec + 0x1C0), 0xF1194D80, 0);
#else
        BtlUnit_snd_se(mario, str_80300470, 0xF1194D80, 0);
#endif
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
                        f32 dx = wx - obj->field_C;
                        f32 dy = wy - obj->field_10;
                        f32 d = dx * dx + dy * dy;

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

// NONMATCHING (91.66%): the int->float magic double is pooled as a local `@712` entry in
// this object's .rodata (offset 0x48, right after vec3_803002b0) instead of at the
// original's .rodata+0x1b8, so MWCC cannot fold it into `lfd fX, 0x1b8(r31)` and emits a
// fresh `lis rN, @712@ha` before every conversion (the file's incomplete .rodata pool is
// file-wide, so this is left as is). That extra lis shifts the schedule around each
// conversion. The whole callee-saved map is also rotated by one register relative to the
// original: here idx=r27, mario=r28, star=r29, vec=r30, genki=r31, where the original has
// idx=r28, mario=r29, star=r30, vec=r31, genki=r27 (a 5-clique, so the allocator's
// tie-break picks the rotation). Logic and constants match the assembly. Putting
// field_58 after field_54/field_3C/field_48 (its original source order) fixed the case-1
// schedule and was worth +1.4%.
// NONMATCHING (91.66%): the pooled int->float magic double is not at vec3_803002b0+0x1B8,
// so its `lis` is emitted and the scheduling plus the rotated callee-saved map differ.
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
        *(Vec*)&star->field_3C = *(const Vec*)&vec[0x24 / 4];
        *(Vec*)&star->field_48 = *(const Vec*)&vec[0x30 / 4];
        star->field_54 = 0.0f;
        star->field_58 = 6.283184f * (f32)idx / 7.0f;
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
                              (60.0f * (60.0f *
                               (0.2f - 0.01f * (f32)idx)))) /
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

// 100%: naming the five vectors (rather than reaching them through a
// vec3_803002b0 pointer) makes MWCC anchor the .rodata section base in r30, so its
// anonymous int->float pool double folds into `lfd fY, 0x1b8(r30)` exactly as in
// the original (the double lives at vec3_803002b0+0x1B8 thanks to the data block
// at the top of this file). Logic, schedule and every constant match the assembly.
void main_star1(void) {
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
        *(Vec*)&star->field_3C = *(const Vec*)&vec3_80300310[0];
        *(Vec*)&star->field_48 = *(const Vec*)&vec3_8030031c[0];
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
            v = *(const Vec*)&vec3_80300328[0];
            v.x = float_200_80421d9c * (f32)rand() / float_32767_80421da0;
            v.z = star->field_20;
            *(Vec*)&star->field_24 = v;
            *(Vec*)&star->field_30 = *(const Vec*)&vec3_80300334[0];
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
            *(Vec*)&star->field_C = *(const Vec*)&vec3_80300340[0];
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

