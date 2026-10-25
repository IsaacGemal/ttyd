#include <stdlib.h>
#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "battle/battle_unit.h"
#include "battle/battle_ac.h"
#include "battle/battle_disp.h"
#include "battle/battle_disp_tex4.h"
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
#include "battle/battle_event_cmd.h"
#include "battle/battle_camera.h"
#include "battle/sac/sac_common.h"
#include "evt/evt_eff.h"
#include "party/party_motion.h"

// Data objects defined later in this file or in other units, declared before their first use.
extern s32 sac_start_camera_evt[];
extern s32 sac_mario_item_on_evt[];
extern s32 sac_mario_item_on_end_evt[];
extern s32 sac_start_stardust[];
extern s32 status_recover_evt[];
static void disp_2D(CameraId camera, void* param);
static void disp_3D_alpha(CameraId camera, void* param);
static void main_star(void);
static void main_base(void);  // local to this file in the original
static void disp_3D(CameraId cameraId, void* param);  // local to this file in the original
// .rodata: the Vec literals of main_weapon, main_star0 and main_star1 and main_object's
// initialiser open the section; disp_3D's vectors follow it, then the strings.
extern const f32 vec3_8030034c[3];
extern const f32 vec3_80300358[3];
extern const f32 vec3_80300364[3];
extern const f32 vec3_80300370[3];
extern const f32 vec3_8030037c[3];
extern const f32 vec3_80300388[3];
extern const f32 vec3_80300394[3];
extern const f32 vec3_803003a0[3];
extern const f32 vec3_803003ac[3];
extern const f32 vec3_803003b8[3];
extern const f32 vec3_803003c4[3];
extern const f32 vec3_803003d0[3];
extern const f32 vec3_803003dc[3];
extern const f32 vec3_803003e8[3];

// .sdata2: the M_I_Y animation name and the .data-referenced zero are defined after disp_3D,
// whose colour literals' templates open the section. The float constants are file-scope so
// the functions can name them, and MWCC folds each one into its literal pool (the static
// definitions vanish, leaving the pooled words in the original's order).
extern const char str_80421D3C[6];
extern const f32 float_0_80421d44;

typedef struct GenkiStar {
    s32 state;                  // 0x0, main_star0/main_star1's switch; star_appear sets 1
    s32 timer;                  // 0x4, the intplGetValue frame of the current state
    s32 field_0x8;              // 0x8
    Vec pos;                    // 0xC, into the star stone effect's x/y/z
    Vec from;                   // 0x18, intplGetValue start
    Vec to;                     // 0x24, intplGetValue end
    Vec field_0x30;             // 0x30
    Vec scale;                  // 0x3C, x times 1.5 is the star stone's scale
    Vec rot;                    // 0x48, the star stone's tipRotY/rotY/tipRotZ
    f32 radius;                 // 0x54, distance from Mario while circling
    f32 angle;                  // 0x58, cos/sin of it place the star around Mario
    f32 angleVel;               // 0x5C, added to angle each frame
    EffectEntry* eff;           // 0x60, effStarStoneEntry
} GenkiStar;                    // 0x64

typedef struct GenkiObject {
    s32 state;                  // 0x0, main_object's switch; object_entry sets 1, 2 while falling
    s32 timer;                  // 0x4, counts 0..0x78; % 12 blinks the big icons
    s32 kind;                   // 0x8, 0/1 Mario HP, 2/3 partner HP, 4/5 FP (odd: +3), 6 the bad one
    Vec pos;                    // 0xC
    u8 pad_0x18[0x2C - 0x18];   // 0x18
} GenkiObject;                  // 0x2C

typedef struct GenkiEffect {
    s32 field_0x0;              // 0x0
    Vec pos;                    // 0x4
    f32 field_0x10;             // 0x10
} GenkiEffect;                  // 0x14

typedef struct GenkiWeapon {
    s32 state;                  // 0x0, main_weapon's switch; weapon_entry sets 1
    s32 delay;                  // 0x4, frames from weapon_entry to the launch
    Vec pos[4];                 // 0x8, current position then 3 trail positions
    Vec vel;                    // 0x38
    EffectEntry* eff;           // 0x44, effStarStoneEntry at pos[0]
    EffectEntry* trailEff[3];   // 0x48, effStarStoneEntry at pos[1..3]
} GenkiWeapon;                  // 0x54

typedef struct GenkiWork {
    u8 pad_0x0[0x4];            // 0x0
    s32 state;                  // 0x4, main_base's switch; start_game sets 1, wait_game_end waits for 3
    u8 pad_0x8[0xC - 0x8];      // 0x8
    s32 variant;                // 0xC, 0 for marioAttackEvent_Genki0, 1 for Genki1 (main_genki's args[1])
    s32 score[3];               // 0x10
    f32 dispScore[3];           // 0x1C, counts toward score[]; the number disp_3D draws
    s32 scoreBlinkTimer[3];     // 0x28, set to 60 when dispScore changes; % 12 blinks the number
    EffectEntry* recoveryEff[3]; // 0x34, effRecoveryEntry under each score number
    s32 timeLeft;               // 0x40, 900 frames; objects spawn while it is > 0
    s32 marioState;             // 0x44, main_mario's switch
    s32 marioTimer;             // 0x48, frames in marioState 3/4
    s32 spawnTimer;             // 0x4C, frames to the next object_entry
    s32 partyUnitId;            // 0x50, main_genki's args[0]; -1 with no partner
    s32 holdTimer;              // 0x54, counts down while the stick is held; Genki1 fires again at 0
    s32 weaponNum;              // 0x58, 3 or 7: the GenkiWeapon slots in use
    s32* kindTable;             // 0x5C, BattleAlloc'd, shuffled GenkiObject kinds
    s32 spawnCount;             // 0x60, objects spawned; indexes kindTable
    s32 targetState;            // 0x64, main_target's switch
    s32 targetRot;              // 0x68, degrees; the sight's btlDispTex4 rotation
    f32 aimAngle;               // 0x6C, degrees, 0..60: launch angle and sight position
    f32 aimAngleVel;            // 0x70, +-1.25 per frame
    Vec field_0x74;             // 0x74
    GenkiEffect field_0x80[7];  // 0x80
    GenkiWeapon weapon[7];      // 0x10C
    GenkiObject objects[10];    // 0x358
    GenkiStar stars[7];         // 0x510
} GenkiWork;

static GenkiWork* get_ptr(void);

s32 main_genki(EventEntry* event, BOOL isFirstCall) {
    GenkiWork* genki = get_ptr();
    s32 i;
    s32 j;
    s32* args;

    if (isFirstCall) {
        args = event->args;
        memset(genki, 0, 0x7CC);
        genki->partyUnitId = evtGetValue(event, args[0]);
        genki->variant = evtGetValue(event, args[1]);
        genki->timeLeft = 0x384;
        genki->score[2] = 0;
        genki->score[1] = 0;
        genki->score[0] = 0;
        genki->dispScore[2] = 0.0f;
        genki->dispScore[1] = 0.0f;
        genki->dispScore[0] = 0.0f;
        if (genki->variant == 0) {
            genki->weaponNum = 3;
        } else {
            genki->weaponNum = 7;
        }
        if (genki->variant == 0) {
            genki->kindTable = BattleAlloc(0x64);
        } else {
            genki->kindTable = BattleAlloc(0xC8);
        }
        genki->recoveryEff[0] = effRecoveryEntry(2, 0, 0.0f, -1000.0f, 0.0f);
        genki->recoveryEff[1] = effRecoveryEntry(2, 0, 0.0f, -1000.0f, 0.0f);
        genki->recoveryEff[2] = effRecoveryEntry(3, 0, 0.0f, -1000.0f, 0.0f);
        for (i = 0; i < 7; i++) {
            genki->weapon[i].eff = effStarStoneEntry(i, 0.0f, -1000.0f, 0.0f, 1.0f);
            for (j = 0; j < 3; j++) {
                genki->weapon[i].trailEff[j] = effStarStoneEntry(i + 7, 0.0f, -1000.0f, 0.0f, 1.0f);
            }
        }
    }
    main_base();
    main_mario();
    main_target();
    for (i = 0; i < genki->weaponNum; i++) {
        main_weapon(i);
    }
    for (i = 0; i < 10; i++) {
        main_object(i);
    }
    main_star();
    dispEntry(8, 901.0f, 1, disp_2D, NULL);
    dispEntry(4, 0.0f, 1, disp_3D, NULL);
    dispEntry(4, 0.0f, 2, disp_3D_alpha, NULL);
    return 0;
}

static void main_base(void) {
    GenkiWork* genki = get_ptr();
    s32 i;
    s32 a;
    s32 b;
    s32 t;
    s32 k;

    if (genki->dispScore[0] > (f32)genki->score[0]) {
        genki->dispScore[0] -= 1.0f;
        genki->scoreBlinkTimer[0] = 0x3C;
    }
    if (genki->dispScore[0] < (f32)genki->score[0]) {
        genki->dispScore[0] += 1.0f;
        genki->scoreBlinkTimer[0] = 0x3C;
    }
    if (genki->dispScore[1] > (f32)genki->score[1]) {
        genki->dispScore[1] -= 1.0f;
        genki->scoreBlinkTimer[1] = 0x3C;
    }
    if (genki->dispScore[1] < (f32)genki->score[1]) {
        genki->dispScore[1] += 1.0f;
        genki->scoreBlinkTimer[1] = 0x3C;
    }
    if (genki->dispScore[2] > (f32)genki->score[2]) {
        genki->dispScore[2] -= 1.0f;
        genki->scoreBlinkTimer[2] = 0x3C;
    }
    if (genki->dispScore[2] < (f32)genki->score[2]) {
        genki->dispScore[2] += 1.0f;
        genki->scoreBlinkTimer[2] = 0x3C;
    }
    if (genki->scoreBlinkTimer[0] > 0) {
        genki->scoreBlinkTimer[0]--;
    }
    if (genki->scoreBlinkTimer[1] > 0) {
        genki->scoreBlinkTimer[1]--;
    }
    if (genki->scoreBlinkTimer[2] > 0) {
        genki->scoreBlinkTimer[2]--;
    }

    switch (genki->state) {
    case 0:
        break;
    case 1:
        genki->state = 2;
        genki->marioState = 1;
        genki->targetState = 1;
        genki->spawnCount = 0;
        if (genki->variant == 0) {
            k = 0;
            for (i = 0; i < 7; i++) {
                genki->kindTable[k++] = 0;
            }
            for (i = 0; i < 7; i++) {
                genki->kindTable[k++] = 2;
            }
            for (i = 0; i < 8; i++) {
                genki->kindTable[k++] = 4;
            }
            for (i = 0; i < 3; i++) {
                genki->kindTable[k++] = 6;
            }
            for (i = 0; i < 200; i++) {
                a = irand(25);
                b = irand(25);
                t = genki->kindTable[a];
                genki->kindTable[a] = genki->kindTable[b];
                genki->kindTable[b] = t;
            }
        } else {
            k = 0;
            for (i = 0; i < 12; i++) {
                genki->kindTable[k++] = 0;
            }
            for (i = 0; i < 4; i++) {
                genki->kindTable[k++] = 1;
            }
            for (i = 0; i < 12; i++) {
                genki->kindTable[k++] = 2;
            }
            for (i = 0; i < 4; i++) {
                genki->kindTable[k++] = 3;
            }
            for (i = 0; i < 12; i++) {
                genki->kindTable[k++] = 4;
            }
            for (i = 0; i < 4; i++) {
                genki->kindTable[k++] = 5;
            }
            for (i = 0; i < 2; i++) {
                genki->kindTable[k++] = 6;
            }
            for (i = 0; i < 200; i++) {
                a = irand(50);
                b = irand(50);
                t = genki->kindTable[a];
                genki->kindTable[a] = genki->kindTable[b];
                genki->kindTable[b] = t;
            }
        }
        /* fallthrough */
    case 2:
        genki->spawnTimer--;
        if (genki->spawnTimer <= 0) {
            if (genki->variant == 0) {
                genki->spawnTimer = 0x25;
            } else {
                genki->spawnTimer = 0x12;
            }
            if (genki->timeLeft > 0) {
                object_entry();
            }
        }
        if (genki->timeLeft <= 0) {
            s32 ocount;
            s32 wcount = 0;
            for (i = 0; i < genki->weaponNum; i++) {
                if (genki->weapon[i].state == 0) {
                    wcount++;
                } else {
                    break;
                }
            }
            ocount = 0;
            for (i = 0; i < 10; i++) {
                if (genki->objects[i].state == 0) {
                    ocount++;
                } else {
                    break;
                }
            }
            if (wcount == genki->weaponNum && ocount == 10) {
                genki->state = 3;
            }
        } else {
            genki->timeLeft--;
        }
        break;
    case 3:
        break;
    }
}

void main_mario(void) {
    GenkiWork* genki = get_ptr();
    s32 stickX = keyGetStickX(0);
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    BattleUnitPart* parts = BtlUnit_GetPartsPtr(mario, BtlUnit_GetBodyPartsId(mario));

    if (genki->marioState >= 1 && genki->marioState <= 4 && genki->timeLeft <= 0 && object_get_num() == 0) {
        genki->marioState = 5;
        genki->targetState = 4;
        BtlUnit_SetAnim(parts, "M_S_1");
    }
    switch (genki->marioState) {
    case 0:
        break;
    case 1:
        if (stickX < -0x28) {
            genki->marioState = 2;
            genki->marioTimer = 0;
            genki->holdTimer = 10;
            BtlUnit_SetAnim(parts, "M_C_2A");
        }
        break;
    case 2:
        if (stickX > -0x14) {
            genki->marioState = 1;
            weapon_entry();
        } else {
            genki->holdTimer--;
            if (genki->holdTimer == 2) {
                BtlUnit_SetAnim(parts, "M_C_2B");
            }
            if (genki->holdTimer <= 0 && genki->variant == 1) {
                genki->holdTimer = 10;
                weapon_entry();
            }
        }
        break;
    case 3:
        genki->marioState = 4;
        genki->marioTimer = 0;
        BtlUnit_SetAnim(parts, "M_D_1");
        {
            f32 x, y, z;
            EffectEntry* eff;
            BtlUnit_GetPos(mario, &x, &y, &z);
            eff = effStampN64Entry(2, x, 20.0f + y, 10.0f + z);
            ((effStampData*)eff->userdata)->tevReg0R = 0;
            ((effStampData*)eff->userdata)->tevReg0G = 0xC8;
            ((effStampData*)eff->userdata)->tevReg0B = 0;
            ((effStampData*)eff->userdata)->tevReg1R = 0;
            ((effStampData*)eff->userdata)->tevReg1G = 0xA;
            ((effStampData*)eff->userdata)->tevReg1B = 0;
        }
    case 4:
        genki->marioTimer++;
        if (genki->marioTimer == 0x1E) {
            BtlUnit_SetAnim(parts, "M_X_1");
        }
        if (genki->marioTimer == 0xB4) {
            BtlUnit_SetAnim(parts, str_80421D3C);
            genki->marioState = 1;
            genki->marioTimer = 0;
        }
        break;
    case 5:
        break;
    }
}

void main_target(void) {
    GenkiWork* genki = get_ptr();

    switch (genki->targetState) {
    case 0:
        break;
    case 1:
        genki->targetRot = 0;
        genki->aimAngle = 30.0f;
        genki->aimAngleVel = 1.25f;
        genki->targetState = 2;
    case 2:
        genki->targetRot++;
        if (genki->targetRot >= 0x168) {
            genki->targetRot = (s32)((f32)genki->targetRot - 360.0f);
        }
        genki->aimAngle += genki->aimAngleVel;
        if (genki->aimAngle >= 60.0f) {
            genki->aimAngle = 60.0f;
            genki->aimAngleVel = -1.25f;
            genki->targetState = 3;
        }
        break;
    case 3:
        genki->targetRot++;
        if (genki->targetRot >= 0x168) {
            genki->targetRot = (s32)((f32)genki->targetRot - 360.0f);
        }
        genki->aimAngle += genki->aimAngleVel;
        if (genki->aimAngle <= 0.0f) {
            genki->aimAngle = 0.0f;
            genki->aimAngleVel = 1.25f;
            genki->targetState = 2;
        }
        break;
    case 4:
        break;
    }
}

// The function names its vec3 objects directly and the SFX sound name is a pooled string literal, so
// MWCC anchors the .rodata base in r29 and folds the int->float pool double into `lfd 0x1b8(r29)` and
// the string into `addi r4, r29, 0x1c0`, as retail does. The effect's position is one Vec copy.
void main_weapon(s32 idx) {
    GenkiWork* genki = get_ptr();
    GenkiWeapon* weapon = &genki->weapon[idx];
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 angle = 3.141592f * (2.0f * genki->aimAngle) / 360.0f;
    s32 audienceNum = BattleAudience_GetAudienceNum();
    s32 i;

    for (i = 2; i > 0; i--) {
        weapon->pos[i + 1] = weapon->pos[i];
    }
    weapon->pos[1] = weapon->pos[0];

    switch (weapon->state) {
    case 0:
        weapon->pos[0] = (Vec){0.0f, -1000.0f, 0.0f};
        break;
    case 1:
        weapon->delay--;
        if (weapon->delay >= 0) {
            break;
        }
        weapon->state = 2;
        BtlUnit_GetPos(mario, &weapon->pos[0].x, &weapon->pos[0].y, &weapon->pos[0].z);
        weapon->pos[0].x += 20.0f * mario->mSizeMultiplier;
        weapon->pos[0].y += 28.0f * mario->mSizeMultiplier;
        weapon->vel = (Vec){10.0f * (f32)cos(angle), 10.0f * (f32)sin(angle), 0.0f};
        for (i = 0; i < 7; i++) {
            GenkiEffect* eff = &genki->field_0x80[i];

            if (eff->field_0x0 <= 0) {
                eff->field_0x0 = 0x1E;
                eff->pos = genki->field_0x74;
                eff->field_0x10 = 2.0f * (f32)genki->targetRot;
                break;
            }
        }
        BtlUnit_snd_se(mario, "SFX_BTL_SAC_HEART_SHOOT2", 0xF1194D80, 0);
        /* fallthrough */
    case 2:
        weapon->pos[0].x += weapon->vel.x;
        weapon->pos[0].y += weapon->vel.y;
        weapon->vel.y -= 0.15f;
        {
            f32 x, y, z;

            BtlUnit_GetPos(mario, &x, &y, &z);
            if (weapon->pos[0].x >= 250.0f || weapon->pos[0].y >= 180.0f ||
                weapon->pos[0].y <= y) {
                weapon->state = 3;
                break;
            }
            {
                GenkiObject* obj;
                f32 best = 10000000.0f;
                s32 found = -1;
                f32 wx = weapon->pos[0].x;
                f32 wy = weapon->pos[0].y;

                for (i = 0; i < 10; i++) {
                    obj = &genki->objects[i];

                    if (obj->state == 2) {
                        f32 dx = wx - obj->pos.x;
                        f32 dy = wy - obj->pos.y;
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

                    switch (obj->kind) {
                    case 6:
                        threshold = 344.0f;
                        break;
                    case 0:
                    case 2:
                    case 4:
                        threshold = 576.0f;
                        break;
                    default:
                        threshold = 1296.0f;
                        break;
                    }
                    if (best <= threshold && obj->pos.y <= 200.0f) {
                        weapon->state = 3;
                        obj->state = 3;
                effStardustN64Entry(5, obj->pos.x, obj->pos.y, obj->pos.z,
                                    10.0f);
                effHitEntry(0, 5, obj->pos.x, obj->pos.y, obj->pos.z, 1.0f);
                switch (obj->kind) {
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
                    genki->marioState = 3;
                    break;
                }
                switch (obj->kind) {
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
        weapon->state = 0;
        break;
    }

    {
        f32 scale = 1.125f;

        if (weapon->eff != NULL) {
            effStarStoneData* data = weapon->eff->userdata;

            data->x = weapon->pos[0].x;
            data->y = weapon->pos[0].y;
            data->z = weapon->pos[0].z;
            data->scale = scale;
            data->tevReg0A = 0xFF;
        }
        for (i = 0; i < 3; i++) {
            effStarStoneData* data = weapon->trailEff[i]->userdata;

            data->x = weapon->pos[i + 1].x;
            data->y = weapon->pos[i + 1].y;
            data->z = weapon->pos[i + 1].z;
            data->scale = scale;
            data->tevReg0A = 0xFF - (i + 1) * 0x32;
        }
    }
}

// Matches. The int->float loads read the anonymous pool double (at .rodata+0x1B8, like
// retail's double_to_int_80300468) with an absolute `lis`, and the 30.0f literal as
// `@NNN` instead of retail's `float_30_80421d80`; objdiff does not score those symbol names. The
// `30.0f` literal, rather than a named constant (same value), is what makes MWCC emit
// the original's interleaved `li`/`lfs` argument setup. Keeping `fifteen` in a variable fixed the
// compare's f1/f2 assignment; the if/else that builds `limit` fixed the case-1 integer register
// allocation.
void main_object(s32 idx) {
    GenkiWork* genki = get_ptr();
    GenkiObject* obj = &genki->objects[idx];
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    s32 local[3] = {0, 4, 2};
    GenkiStar* star;
    f32 x, y, z;

    switch (obj->state) {
    case 0:
        break;
    case 1:
        obj->state = 2;
        obj->timer = 0;
        {
            s32 limit;
            if (genki->variant == 0) {
                limit = 0x19;
            } else {
                limit = 0x32;
            }
            if (genki->spawnCount < limit) {
                obj->kind = genki->kindTable[genki->spawnCount];
            } else {
                s32 r = irand(genki->variant == 0 ? 7 : 0xE);
                if (r == 0) {
                    obj->kind = 6;
                } else {
                    if (genki->partyUnitId != -1) {
                        obj->kind = local[irand(3)];
                    } else {
                        obj->kind = local[irand(2)];
                    }
                    if (irand(5) == 0 && genki->variant == 1) {
                        switch (obj->kind) {
                        case 0:
                            obj->kind = 1;
                            break;
                        case 2:
                            obj->kind = 3;
                            break;
                        case 4:
                            obj->kind = 5;
                            break;
                        }
                    }
                }
            }
        }
        if (genki->partyUnitId == -1 && obj->kind == 2) {
            obj->kind = 0;
        }
        if (genki->partyUnitId == -1 && obj->kind == 3) {
            obj->kind = 1;
        }
        genki->spawnCount++;
        if (genki->variant == 0) {
            star = &genki->stars[irand(7)];
        } else {
            star = &genki->stars[0];
        }
        {
            BtlUnit_GetPos(mario, &x, &y, &z);
            obj->pos.x = star->pos.x;
            obj->pos.y = star->pos.y;
            obj->pos.z = z + (f32)idx;
        }
    case 2:
        if (genki->variant == 0) {
            obj->pos.y -= 1.32f;
        } else {
            obj->pos.y -= 1.98f;
        }
        {
            BtlUnit_GetPos(mario, &x, &y, &z);
            {
                f32 fifteen = 15.0f;
                if (obj->pos.y <= fifteen + y) {
                    f32 thirty = 30.0f;
                    obj->state = 3;
                    effStardustEntry(5, 0xA, 0x3C, obj->pos.x, fifteen + y, obj->pos.z,
                                     thirty, thirty);
                }
            }
        }
        obj->timer++;
        if (obj->timer > 0x78) {
            obj->timer = 0;
        }
        break;
    case 3:
        obj->state = 0;
        break;
    }
}

static void main_star(void) {
    GenkiWork* genki = get_ptr();
    s32 i;

    if (genki->variant == 0) {
        for (i = 0; i < 7; i++) {
            main_star0(i);
        }
    } else {
        main_star1();
    }
}

// 100%: the Vec literals make MWCC anchor the .rodata section base in r31, fold the
// anonymous int->float pool double into `lfd fX, 0x1b8(r31)`, and pick the original's
// callee-saved map (idx=r28, mario=r29, star=r30, vec=r31, genki=r27).
// Putting the angle store after radius/scale/rot (its original source order) fixed
// the case-1 schedule. Logic, schedule and constants match the assembly.
void main_star0(s32 idx) {
    GenkiWork* genki = get_ptr();
    GenkiStar* star = &genki->stars[idx];
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 x, y, z;
    f32 f;

    switch (star->state) {
    case 0:
        break;
    case 1:
        star->state = 2;
        star->timer = 0;
        star->eff = effStarStoneEntry(idx, 0.0f, -1000.0f, 0.0f, 1.0f);
        BtlUnit_GetPos(mario, &star->pos.x, &star->pos.y, &star->pos.z);
        star->pos.y = star->pos.y +
            (mario->mSizeMultiplier * (f32)mario->mHeight + 37.0f);
        star->from = star->pos;
        star->to = star->pos;
        star->to.y += 30.0f;
        star->scale = (Vec){0.0f, 0.0f, 0.0f};
        star->rot = (Vec){0.0f, 0.0f, 0.0f};
        star->radius = 0.0f;
        star->angle = 6.283184f * (f32)idx / 7.0f;
        star->angleVel = 0.1f;
        /* fallthrough */
    case 2:
        star->timer++;
        if (star->timer <= 0x64) {
            star->pos.y = intplGetValue(0, star->timer, 0x64, star->from.y, star->to.y);
            BtlUnit_GetPos(mario, &x, &y, &z);
            star->radius = intplGetValue(0, star->timer, 0x64, 0.0f,
                                           40.0f);
            star->angle += star->angleVel;
            star->pos.x = x + star->radius * (f32)cos(star->angle);
            star->pos.z = z + star->radius * (f32)sin(star->angle);
            f = intplGetValue(0, star->timer, 0x64, 0.0f, 0.75f);
            star->scale.z = f;
            star->scale.y = f;
            star->scale.x = f;
        } else {
            star->pos.y = star->to.y;
            BtlUnit_GetPos(mario, &x, &y, &z);
            star->angle += star->angleVel;
            star->pos.x = x + star->radius * (f32)cos(star->angle);
            star->pos.z = z + star->radius * (f32)sin(star->angle);
            star->scale.z = 0.75f;
            star->scale.y = 0.75f;
            star->scale.x = 0.75f;
        }
        star->rot.y = intplGetValue(4, star->timer, 0x78, 0.0f,
                                       2160.0f);
        if (star->timer >= 0x78) {
            star->state = 3;
            star->timer = 0;
            star->from = star->pos;
            star->to = (Vec){200.0f * (f32)rand() / 32767.0f, 180.0f, star->from.z};
            star->field_0x30 = (Vec){0.0f, 0.0f, 0.0f};
            star->field_0x30.y = (2.0f * (star->to.y - star->from.y) -
                              (60.0f * (60.0f *
                               (0.2f - 0.01f * (f32)idx)))) /
                             120.0f;
        }
        break;
    case 3:
        star->timer++;
        star->pos.x = intplGetValue(4, star->timer, 0x3C, star->from.x, star->to.x);
        star->pos.y += star->field_0x30.y;
        star->field_0x30.y += 0.2f - 0.01f * (f32)idx;
        star->rot.y = 0.0f;
        star->scale.z = 0.75f;
        star->scale.y = 0.75f;
        star->scale.x = 0.75f;
        if (star->timer >= 0x3C) {
            star->state = 4;
            star->timer = 0;
            if (irand(2) == 0) {
                star->field_0x30.x = -2.0f - 0.05f * (f32)idx;
            }
            star->field_0x30.x = 2.0f + 0.05f * (f32)idx;
            star->field_0x30.y = 0.0f;
        }
        break;
    case 4:
        star->pos.x += star->field_0x30.x;
        if (star->pos.x < 0.0f) {
            star->field_0x30.x = 3.0f + 0.05f * (f32)idx;
        }
        if (star->pos.x > 200.0f) {
            star->field_0x30.x = -3.0f - 0.05f * (f32)idx;
        }
        star->field_0x30.y += 0.1f + 0.01f * (f32)idx;
        if (star->field_0x30.y >= 6.283184f) {
            star->field_0x30.y -= 6.283184f;
        }
        star->pos.y = 10.0f * (f32)sin(star->field_0x30.y) + 180.0f;
        if (genki->timeLeft <= 0) {
            star->state = 5;
            star->timer = 0;
            star->pos = (Vec){0.0f, -1000.0f, 0.0f};
        }
        break;
    case 5:
        break;
    }

    if (star->eff != NULL) {
        effStarStoneData* p = star->eff->userdata;
        p->x = star->pos.x;
        p->y = star->pos.y;
        p->z = star->pos.z;
        p->tipRotY = star->rot.x;
        p->rotY = star->rot.y;
        p->tipRotZ = star->rot.z;
        p->scale = 1.5f * star->scale.x;
    }
}

// 100%: the Vec literals make MWCC anchor the .rodata section base in r30, so its
// anonymous int->float pool double (at .rodata+0x1B8) folds into `lfd fY, 0x1b8(r30)`
// exactly as in the original. Logic, schedule and every constant match the assembly.
void main_star1(void) {
    GenkiWork* genki = get_ptr();
    GenkiStar* star = &genki->stars[0];
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 f;

    switch (star->state) {
    case 0:
        break;
    case 1:
        star->state = 2;
        star->timer = 0;
        star->eff = effStarStoneEntry(4, 0.0f, -1000.0f, 0.0f, 1.0f);
        BtlUnit_GetPos(mario, &star->pos.x, &star->pos.y, &star->pos.z);
        star->pos.y = star->pos.y +
            (mario->mSizeMultiplier * (f32)mario->mHeight + 37.0f);
        star->from = star->pos;
        star->to = star->pos;
        star->to.y += 50.0f;
        star->to.z += -1.0f;
        star->scale = (Vec){0.0f, 0.0f, 0.0f};
        star->rot = (Vec){0.0f, 0.0f, 0.0f};
        /* fallthrough */
    case 2:
        star->timer++;
        if (star->timer <= 0x64) {
            star->pos.x = intplGetValue(0, star->timer, 0x64, star->from.x, star->to.x);
            star->pos.y = intplGetValue(0, star->timer, 0x64, star->from.y, star->to.y);
            star->pos.z = intplGetValue(0, star->timer, 0x64, star->from.z, star->to.z);
            f = intplGetValue(0, star->timer, 0x64, 0.0f, 2.0f);
            star->scale.z = f;
            star->scale.y = f;
            star->scale.x = f;
        } else {
            star->pos.x = star->to.x;
            star->pos.y = star->to.y;
            star->pos.z = star->to.z;
            star->scale.z = 2.0f;
            star->scale.y = 2.0f;
            star->scale.x = 2.0f;
        }
        star->rot.y = intplGetValue(4, star->timer, 0x78, 0.0f,
                                       2160.0f);
        if (star->timer >= 0x78) {
            star->state = 3;
            star->timer = 0;
            star->from = star->pos;
            star->to = (Vec){200.0f * (f32)rand() / 32767.0f, 180.0f, star->from.z};
            star->field_0x30 = (Vec){0.0f, 0.0f, 0.0f};
            star->field_0x30.y = (2.0f * (star->to.y - star->from.y) -
                              720.0f) / 120.0f;
        }
        break;
    case 3:
        star->timer++;
        star->pos.x = intplGetValue(4, star->timer, 0x3C, star->from.x, star->to.x);
        star->pos.y += star->field_0x30.y;
        star->field_0x30.y += 0.2f;
        star->rot.y = 0.0f;
        star->scale.z = 2.0f;
        star->scale.y = 2.0f;
        star->scale.x = 2.0f;
        if (star->timer >= 0x3C) {
            star->state = 4;
            star->timer = 0;
            if (irand(2) == 0) {
                star->field_0x30.x = -2.0f;
            }
            star->field_0x30.x = 2.0f;
            star->field_0x30.y = 0.0f;
        }
        break;
    case 4:
        star->pos.x += star->field_0x30.x;
        if (star->pos.x < 0.0f) {
            star->field_0x30.x = 3.0f;
        }
        if (star->pos.x > 200.0f) {
            star->field_0x30.x = -3.0f;
        }
        star->field_0x30.y += 0.1f;
        if (star->field_0x30.y >= 6.283184f) {
            star->field_0x30.y -= 6.283184f;
        }
        star->pos.y = 10.0f * (f32)sin(star->field_0x30.y) + 180.0f;
        if (genki->timeLeft <= 0) {
            star->state = 5;
            star->timer = 0;
            star->pos = (Vec){0.0f, -1000.0f, 0.0f};
        }
        break;
    case 5:
        break;
    }

    if (star->eff != NULL) {
        effStarStoneData* p = star->eff->userdata;
        p->x = star->pos.x;
        p->y = star->pos.y;
        p->z = star->pos.z;
        p->tipRotY = star->rot.x;
        p->rotY = star->rot.y;
        p->tipRotZ = star->rot.z;
        p->scale = 1.5f * star->scale.x;
    }
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
        if (star->eff != NULL) {
            effDelete(((effStarStoneData*)star->eff->userdata)->dustEffect);
            effDelete(star->eff);
        }
    }
    for (i = 0; i < 7; i++) {
        weapon = &genki->weapon[i];
        if (weapon->eff != NULL) {
            effDelete(((effStarStoneData*)weapon->eff->userdata)->dustEffect);
            effDelete(weapon->eff);
            for (j = 0; j < 3; j++) {
                effDelete(((effStarStoneData*)weapon->trailEff[j]->userdata)->dustEffect);
                effDelete(weapon->trailEff[j]);
            }
        }
    }
    effDelete(genki->recoveryEff[0]);
    effDelete(genki->recoveryEff[1]);
    effDelete(genki->recoveryEff[2]);
    BattleFree(genki->kindTable);
    return 2;
}

static void disp_2D(CameraId camera, void* param) {
}

// Reading each vector by name (`*(const Vec*)vec3_8030034c`) rather than through a base
// pointer makes MWCC anchor the whole .rodata section in r30 and copies the
// trans/scale/rot/tmp locals into the original's slots. The two final quads call btlDispTex4
// directly. The int->float loads read the compiler's anonymous pool double, which lands
// at .rodata+0x1B8 just like retail's double_to_int_80300468. Under MUST_MATCH the five icon colours are named
// locals passed by address, so MWCC allocates them 0x20..0x10 and the final colours 0xc/0x8.
static void disp_3D(CameraId cameraId, void* param) {
    GenkiObject* obj;
    GenkiWork* genki = get_ptr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    BattleUnit* party = BattleGetPartyPtr(_battleWorkPointer);
    f32 angle = 3.141592f * genki->aimAngle / 180.0f;
    Mtx mtx;
    Vec pos;
    Vec pos2;
    f32 x, y, z;
    f32 px, py, pz;
    u16 flags;
    s32 i;
#ifdef MUST_MATCH
    // Match-only: the original allocates the five icon colours 0x20..0x10 in declaration
    // order, then the two final colours at 0xc/0x8. That needs each colour to be a named
    // local whose address is passed, instead of the anonymous by-value temp MWCC makes for
    // the natural call. The #else forms below use the natural by-value calls.
    GXColor iconC1;
    GXColor iconC2;
    GXColor iconC3;
    GXColor iconC4;
    GXColor iconC5;
#endif

    if (genki->state != 0) {
        BtlUnit_GetHomePos(mario, &x, &y, &z);
        y += 60.0f;
        {
            RecoveryData* ud = genki->recoveryEff[0]->userdata;
            ud->x = x;
            ud->y = y - 10.0f;
            ud->z = z + 5.0f;
        }
        {
            s32 frame;
            if (genki->dispScore[0] >= 10.0f) {
                Vec v = *(const Vec*)vec3_8030034c;
                v.x = x + 6.0f;
                v.y = y;
                v.z = z + 8.0f;
                pos = v;
            } else {
                Vec v = *(const Vec*)vec3_80300358;
                v.x = x;
                v.y = y;
                v.z = z + 8.0f;
                pos = v;
            }
            PSMTXTrans(mtx, pos.x, pos.y, pos.z);
            frame = genki->scoreBlinkTimer[0] % 12;
            if (frame >= 0 && frame <= 5) {
#ifdef MUST_MATCH
                iconC1 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
                ((void (*)(Mtx, s32, BOOL, GXColor*))iconNumberDispGx3D)(mtx, (s32)genki->dispScore[0], 0, &iconC1);
#else
                iconNumberDispGx3D(mtx, (s32)genki->dispScore[0], 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
#endif

            }
        }
        if (genki->partyUnitId != -1) {
            BtlUnit_GetHomePos(party, &px, &py, &pz);
            py += 60.0f;
            {
                RecoveryData* ud = genki->recoveryEff[1]->userdata;
                ud->x = px;
                ud->y = py - 10.0f;
                ud->z = pz + 5.0f;
            }
            {
                s32 frame;
                if (genki->dispScore[1] >= 10.0f) {
                    Vec v = *(const Vec*)vec3_80300364;
                    v.x = px + 6.0f;
                    v.y = py;
                    v.z = pz + 8.0f;
                    pos2 = v;
                } else {
                    Vec v = *(const Vec*)vec3_80300370;
                    v.x = px;
                    v.y = py;
                    v.z = pz + 8.0f;
                    pos2 = v;
                }
                PSMTXTrans(mtx, pos2.x, pos2.y, pos2.z);
                frame = genki->scoreBlinkTimer[1] % 12;
                if (frame >= 0 && frame <= 5) {
#ifdef MUST_MATCH
                    iconC2 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
                    ((void (*)(Mtx, s32, BOOL, GXColor*))iconNumberDispGx3D)(mtx, (s32)genki->dispScore[1], 0, &iconC2);
#else
                    iconNumberDispGx3D(mtx, (s32)genki->dispScore[1], 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
#endif

                }
            }
            BtlUnit_GetHomePos(mario, &x, &y, &z);
            BtlUnit_GetHomePos(party, &px, &py, &pz);
            x = 0.5f * (x + px);
            y += 90.0f;
            {
                RecoveryData* ud = genki->recoveryEff[2]->userdata;
                ud->x = x;
                ud->y = y - 10.0f;
                ud->z = z + 5.0f;
            }
            {
                s32 frame;
                if (genki->dispScore[2] >= 10.0f) {
                    Vec v = *(const Vec*)vec3_8030037c;
                    v.x = x + 6.0f;
                    v.y = y;
                    v.z = z + 8.0f;
                    pos = v;
                } else {
                    Vec v = *(const Vec*)vec3_80300388;
                    v.x = x;
                    v.y = y;
                    v.z = z + 8.0f;
                    pos = v;
                }
                PSMTXTrans(mtx, pos.x, pos.y, pos.z);
                frame = genki->scoreBlinkTimer[2] % 12;
                if (frame >= 0 && frame <= 5) {
#ifdef MUST_MATCH
                    iconC3 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
                    ((void (*)(Mtx, s32, BOOL, GXColor*))iconNumberDispGx3D)(mtx, (s32)genki->dispScore[2], 0, &iconC3);
#else
                    iconNumberDispGx3D(mtx, (s32)genki->dispScore[2], 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
#endif

                }
            }
        } else {
            BtlUnit_GetHomePos(mario, &x, &y, &z);
            y += 90.0f;
            {
                RecoveryData* ud = genki->recoveryEff[2]->userdata;
                ud->x = x;
                ud->y = y - 10.0f;
                ud->z = z + 5.0f;
            }
            {
                s32 frame;
                if (genki->dispScore[2] >= 10.0f) {
                    Vec v = *(const Vec*)vec3_80300394;
                    v.x = x + 6.0f;
                    v.y = y;
                    v.z = z + 8.0f;
                    pos = v;
                } else {
                    Vec v = *(const Vec*)vec3_803003a0;
                    v.x = x;
                    v.y = y;
                    v.z = z + 8.0f;
                    pos = v;
                }
                PSMTXTrans(mtx, pos.x, pos.y, pos.z);
                frame = genki->scoreBlinkTimer[2] % 12;
                if (frame >= 0 && frame <= 5) {
#ifdef MUST_MATCH
                    iconC4 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
                    ((void (*)(Mtx, s32, BOOL, GXColor*))iconNumberDispGx3D)(mtx, (s32)genki->dispScore[2], 0, &iconC4);
#else
                    iconNumberDispGx3D(mtx, (s32)genki->dispScore[2], 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
#endif

                }
            }
        }
    }

    for (i = 0; i < 10; i++) {
        obj = &genki->objects[i];

        if (obj->state != 2) {
            continue;
        }
        flags = 0;
        {
            f32 scale = 1.25f;
            s32 icon;
            s32 frame;

            switch (obj->kind) {
            case 0:
            case 1:
                icon = 0x1A3;
                if (obj->kind == 1) {
                    scale = 2.0f;
                    frame = obj->timer % 12;
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
                if (obj->kind == 3) {
                    scale = 2.0f;
                    frame = obj->timer % 12;
                    if (frame >= 0 && frame <= 5) {
                        flags |= 0x20;
                    }
                }
                break;
            case 4:
            case 5:
                icon = 0x1B1;
                if (obj->kind == 5) {
                    scale = 2.0f;
                    frame = obj->timer % 12;
                    if (frame >= 0 && frame <= 5) {
                        flags |= 0x20;
                    }
                }
                break;
            case 6:
                icon = 0x143;
                break;
            }
            PSMTXScale(mtx, scale, scale, 1.0f);
            PSMTXTransApply(mtx, mtx, obj->pos.x,
                            obj->pos.y - 20.0f * scale, obj->pos.z);
#ifdef MUST_MATCH
            // The original passes `flags` without a sign extension; with the header's s16
            // parameter MWCC emits `extsh`, so call it through a u16 prototype. The named
            // `iconC5` address (not a by-value temp) gives it the original's 0x10 slot.
            iconC5 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
            ((void (*)(Mtx, u16, s32, GXColor*))iconDispGxCol)(mtx, flags, icon, &iconC5);
#else
            iconDispGxCol(mtx, flags, icon, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
#endif
        }
    }

    if (genki->targetState >= 2 && genki->targetState <= 3) {
        Mtx mtx2;

        PSMTXScale(mtx2, 0.5f, 0.5f, 0.5f);
        BtlUnit_GetPos(mario, &x, &y, &z);
        x += 85.0f * (f32)cos(angle);
        y += 85.0f * (f32)sin(angle);
        {
            Vec trans;
            Vec pos;
            Vec scale;
            Vec rot;
            Vec tmp;
            GXColor color;

            x += 10.0f;
            y += 30.0f;
            z += 10.0f;
            color = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
            tmp = *(const Vec*)vec3_803003c4;
            tmp.z = 6.0f * (f32)genki->targetRot;
            rot = tmp;
            scale = *(const Vec*)vec3_803003b8;
            pos = *(const Vec*)vec3_803003ac;
            pos.x = x;
            pos.y = y;
            pos.z = z;
            trans = pos;
            btlDispTex4(0x41, &trans, &scale, &rot, &color);
        }
        {
            Vec trans;
            Vec pos;
            Vec scale;
            Vec rot;
            Vec tmp;
            GXColor color;

            color = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
            tmp = *(const Vec*)vec3_803003e8;
            tmp.z = 6.0f * -(f32)genki->targetRot;
            rot = tmp;
            scale = *(const Vec*)vec3_803003dc;
            pos = *(const Vec*)vec3_803003d0;
            pos.x = x;
            pos.y = y;
            pos.z = z;
            trans = pos;
            btlDispTex4(0x42, &trans, &scale, &rot, &color);
        }
    }
}

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
const char str_80421D3C[] = "M_I_Y";
const f32 float_0_80421d44 = 0.0f;

static void disp_3D_alpha(CameraId camera, void* param) {
}

static GenkiWork* get_ptr(void) {
    return (GenkiWork*)&_battleWorkPointer->actionCommands.acWork[0];
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

        count = genki->weaponNum;
        for (i = 0; i < count; i++) {
            if (genki->weapon[i].state == 0) {
                offsets[found++] = i;
            }
        }
        if (found >= 1) {
            idx = offsets[irand(found)];
            {
                GenkiWeapon* wep = &genki->weapon[idx];

                wep->state = 1;
                wep->delay = 7;
            }
            BtlUnit_SetAnim(parts, "M_C_2C");
        }
    }
}

void object_entry(void) {
    GenkiWork* genki = get_ptr();
    s32 i;

    for (i = 0; i < 10; i++) {
        GenkiObject* obj = &genki->objects[i];

        if (obj->state == 0) {
            obj->state = 1;
            break;
        }
    }
}

s32 object_get_num(void) {
    GenkiWork* genki = get_ptr();
    s32 i = 0;
    s32 count = 0;

    for (; i < 10; i++) {
        if (genki->objects[i].state == 2) {
            count++;
        }
    }
    return count;
}

USER_FUNC(start_game) {
    GenkiWork* genki = get_ptr();

    genki->state = 1;
    return 2;
}

USER_FUNC(wait_game_end) {
    GenkiWork* genki = get_ptr();

    return (genki->state == 3) ? 2 : 0;
}

USER_FUNC(get_score) {
    GenkiWork* genki = get_ptr();
    s32* args = event->args;

    evtSetValue(event, args[0], genki->score[0]);
    evtSetValue(event, args[1], genki->score[1]);
    evtSetValue(event, args[2], genki->score[2]);
    return 2;
}

USER_FUNC(status_on) {
    statusWinOpen();
    return 2;
}

USER_FUNC(star_appear) {
    GenkiWork* genki = get_ptr();
    s32 i;

    for (i = 0; i < 7; i++) {
        GenkiStar* star = &genki->stars[i];

        star->state = 1;
        star->field_0x8 = 0;
    }
    return 2;
}

USER_FUNC(star_stone_appear) {
    GenkiWork* genki = get_ptr();

    genki->stars[0].state = 1;
    genki->stars[0].field_0x8 = 1;
    return 2;
}

USER_FUNC(recover_cmd_disable) {
    s32* p = _battleWorkPointer->cmdDisable;

    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    p[4] = 0;
    return 2;
}




EVT_BEGIN(genki_evt_common)
    CALL(btlevtcmd_GetUnitId, -4, LW(13))
    CALL(btlevtcmd_GetBodyId, LW(13), LW(14))
    CALL(btlevtcmd_CheckStatus, LW(13), 27, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(13), -1)
    END_IF
    CALL(btlevtcmd_reset_move_color_lv_all)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    BEGIN_SIBLING_EVENT
        CALL(main_genki, LW(13), LW(15))
    END_SIBLING_EVENT
    CALL_CHILD(sac_start_camera_evt)
    WAIT_MS(500)
    CALL(sac_enemy_slide_go)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_START1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL_CHILD(sac_mario_item_on_evt)
    WAIT_MS(500)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_START2"), EVTDAT_POINTER_MAX, 0, LW(11))
    IF_INT_EQUAL(LW(15), 0)
        CALL(star_appear)
    ELSE
        CALL(star_stone_appear)
    END_IF
    BEGIN_SIBLING_EVENT
        CALL_CHILD(sac_start_stardust)
    END_SIBLING_EVENT
    WAIT_FRAMES(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_SAC_HEART_SHOOT1"), EVTDAT_POINTER_MAX, 0, LW(10))
    CALL(evt_snd_sfxoff, LW(11))
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(40)
    CALL_CHILD(sac_mario_item_on_end_evt)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, -75, LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, PTR((void*)str_80421D3C))
    WAIT_FRAMES(40)
    CALL(evt_snd_sfxoff, LW(10))
    CALL(sac_ac_help_on)
    CALL(start_game)
    CALL(wait_game_end)
    CALL(sac_ac_help_off)
    CALL(get_score, LW(10), LW(11), LW(12))
    WAIT_FRAMES(70)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(status_on)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoomSpeedLv, 0, 2)
    SET_VALUE(LW(15), LW(10))
    ADD(LW(15), LW(11))
    ADD(LW(15), LW(12))
    IF_INT_GREATER_OR_EQUAL(LW(15), 1)
        CALL(sac_wao)
    END_IF
    IF_INT_NOT_EQUAL(LW(13), -1)
        CALL(evt_snd_sfxon, STRING("SFX_HP_RECOVER_SHINE2"), 0)
        SET_VALUE(LW(9), -2)
        CALL_CHILD(status_recover_evt)
        CALL(btlevtcmd_RecoverHp, -2, 1, LW(10))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(evt_eff, PTR((void*)&float_0_80421d44), STRING("stardust"), 2, LW(0), LW(1), LW(2), 50, 50, 50, 100, 0, 0, 0, 0)
        SET_VALUE(LW(9), LW(13))
        CALL_CHILD(status_recover_evt)
        CALL(btlevtcmd_StartWaitEvent, LW(13))
        CALL(btlevtcmd_RecoverHp, LW(13), LW(14), LW(11))
        CALL(btlevtcmd_GetPos, LW(13), LW(0), LW(1), LW(2))
        CALL(evt_eff, PTR((void*)&float_0_80421d44), STRING("stardust"), 2, LW(0), LW(1), LW(2), 50, 50, 50, 100, 0, 0, 0, 0)
        CALL(btlevtcmd_RecoverFp, -2, 1, LW(12))
    ELSE
        CALL(evt_snd_sfxon, STRING("SFX_HP_RECOVER_SHINE2"), 0)
        SET_VALUE(LW(9), -2)
        CALL_CHILD(status_recover_evt)
        CALL(btlevtcmd_RecoverHp, -2, 1, LW(10))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(evt_eff, PTR((void*)&float_0_80421d44), STRING("stardust"), 2, LW(0), LW(1), LW(2), 50, 50, 50, 100, 0, 0, 0, 0)
        CALL(btlevtcmd_RecoverFp, -2, 1, LW(12))
    END_IF
    WAIT_MS(1000)
    CALL(sac_enemy_slide_return)
    WAIT_MS(1000)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(1000)
    CALL(end_genki)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(status_recover_evt)
    CALL(btlevtcmd_CheckStatus, LW(9), 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 1, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 2, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 3, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 3, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 4, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 4, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 5, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 5, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 8, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 8, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 9, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 9, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 13, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 13, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 11, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 11, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 15, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 15, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, LW(9), 20, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_OnOffStatus, LW(9), 20, 0, 0, 0)
    END_IF
    CALL(recover_cmd_disable)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_Genki0)
    SET_VALUE(LW(15), 0)
    CALL_CHILD(genki_evt_common)
    RETURN
EVT_END()

EVT_BEGIN(marioAttackEvent_Genki1)
    SET_VALUE(LW(15), 1)
    CALL_CHILD(genki_evt_common)
    RETURN
EVT_END()

