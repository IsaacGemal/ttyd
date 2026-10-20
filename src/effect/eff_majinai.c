#include "eff/eff_majinai.h"
#include "driver/effdrv.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/mapdrv.h"
#include "eff/eff_stardust.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern const u32 dat_80422568;
extern const u32 dat_8042256c;
extern const u32 dat_80422570;
extern const f32 float_deg2rad_80422578;
extern const u32 dat_80422574;
extern const f32 float_0_8042257c;
extern const f32 float_10_80422580;
EffectEntry* effMajinaiEntry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    MajinaiWork* work;
    MapWork* map;

    effect = effEntry();
    effect->type = "Majinai";
    effect->count = 1;
    work = __memAlloc(HEAP_EFFECT, effect->count * sizeof(MajinaiWork));
    effect->userdata = work;
    effect->callback = effMajinaiMain;
    effect->flags |= 2;

    work->type = type;
    work->x = x;
    work->y = y;
    work->z = z;
    work->field_0x10 = 1.0f;
    work->alpha = 0xff;
    work->state = 0;
    work->animPose = -1;

    map = mapGetWork();
    *(u32*)&map->entries[0].blend[2] = dat_80422568;

    return effect;
}

#ifdef MUST_MATCH
/* MUST_MATCH: materialising the boolean through an inline makes MWCC colour it r29 (the plain
   `inBattle = gp->inBattle != 0;` gets r28); the inlined body emits the identical instructions. */
static inline s32 effMajinaiGetInBattle(void) {
    return gp->inBattle != 0;
}
#endif

void effMajinaiMain(EffectEntry* effect) {
    s32 inBattle;
    s32 i;
#ifdef MUST_MATCH
    /* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
       dispCalcZ; a single local folds the copy away. */
    Vec position;
#endif
    MajinaiWork* work = effect->userdata;
    Vec disp = { 0.0f, 0.0f, 0.0f };
    MapWork* map;

    disp.x = work->x;
    disp.y = work->y;
    disp.z = work->z;
#ifdef MUST_MATCH
    position = disp;
    inBattle = effMajinaiGetInBattle();
#else
    inBattle = gp->inBattle != 0;
#endif

    if (effect->flags & 4) {
        effect->flags &= ~4;
        if (work->animPose != -1) {
            animPoseRelease(work->animPose);
        }
        mapGetWork()->entries[0].flags &= ~8;
        effDelete(effect);
        return;
    }

    switch (work->state) {
        case 0:
            if (!animGroupBaseAsync("MOBJ_EFF_majinai", inBattle, NULL)) {
                return;
            }
            work->animPose = animPoseEntry("MOBJ_EFF_majinai", inBattle);
            animPoseSetAnim(work->animPose, "A_1", TRUE);
            work->state++;
            break;
        case 1:
            map = mapGetWork();
            {
                GXColor color;
                s32 value = map->entries[0].blend[2].r;

                value -= 5;
                if (value < 0x40) {
                    value = 0x40;
                }
                color = *(GXColor*)&dat_8042256c;
                color.r = value;
                color.g = value;
                color.b = value;
                map->entries[0].blend[2] = color;
            }
            map->entries[0].flags |= 8;

            if (gp->inBattle == 0) {
                MarioWork* mario = marioGetPtr();
                for (i = 0; i < 5; i++) {
                    animPoseSetMaterialFlagOn(mario->paperAnimGroupId[i], 0x800);
                }
            } else {
                BattleUnit* unit = BattleGetMarioPtr(_battleWorkPointer);
                BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
                animPoseSetMaterialFlagOn(part->poseId, 0x800);
            }

            if (animPoseGetLoopTimes(work->animPose) >= 0.93f) {
                f32 scale = work->field_0x10;
                effStardustEntry(0, 0x10, 0x3C, work->x,
                    55.0f * scale + work->y, work->z,
                    45.0f * scale, 45.0f * scale);
                work->state++;
            }
            break;
        case 2:
            if (animPoseGetLoopTimes(work->animPose) >= 1.0f) {
                work->state++;
            }
            break;
        case 3:
            map = mapGetWork();
            {
                GXColor color;
                s32 value = map->entries[0].blend[2].r;

                value += 5;
                if (value > 0xff) {
                    value = 0xff;
                }
                color = *(GXColor*)&dat_80422570;
                color.r = value;
                color.g = value;
                color.b = value;
                map->entries[0].blend[2] = color;
            }
            map->entries[0].flags |= 8;

            if (gp->inBattle == 0) {
                MarioWork* mario = marioGetPtr();
                for (i = 0; i < 5; i++) {
                    animPoseSetMaterialFlagOff(mario->paperAnimGroupId[i], 0x800);
                }
            } else {
                BattleUnit* unit = BattleGetMarioPtr(_battleWorkPointer);
                BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
                animPoseSetMaterialFlagOff(part->poseId, 0x800);
            }

            work->alpha -= 5;
            if (work->alpha < 0) {
                if (work->animPose != -1) {
                    animPoseRelease(work->animPose);
                }
                mapGetWork()->entries[0].flags &= ~8;
                effDelete(effect);
                return;
            }
            break;
    }

#ifdef MUST_MATCH
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMajinaiDisp, effect);
#else
    dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effMajinaiDisp, effect);
#endif
}

static inline void majinaiSetColor(MajinaiWork* work) {
    GXColor color = *(GXColor*)&dat_80422574;
    color.a = work->alpha;
    animPoseSetMaterialEvtColor(work->animPose, color);
}

void effMajinaiDisp(CameraId cameraId, void* param) {
    EffectEntry* effect = param;
    MajinaiWork* work = effect->userdata;
    Mtx trans;
    Mtx scale;
    Mtx rot;

    if (work->animPose != -1) {
        PSMTXTrans(trans, work->x, work->y, work->z);
        PSMTXScale(scale, work->field_0x10, work->field_0x10, work->field_0x10);
        PSMTXRotRad(rot, 'y', float_deg2rad_80422578 * -camGetPtr(cameraId)->viewYaw);
        PSMTXConcat(trans, rot, trans);
        PSMTXConcat(trans, scale, trans);
        animPoseSetMaterialFlagOn(work->animPose, 0x840);
        majinaiSetColor(work);
        animPoseMain(work->animPose);
        animPoseDrawMtx(work->animPose, trans, 1, float_0_8042257c, float_10_80422580);
        animPoseDrawMtx(work->animPose, trans, 2, float_0_8042257c, float_10_80422580);
        animPoseDrawMtx(work->animPose, trans, 3, float_0_8042257c, float_10_80422580);
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const u32 dat_80422568 = 0xFFFFFFFF;
const u32 dat_8042256c = 0x000000FF;
const u32 dat_80422570 = 0x000000FF;
const u32 dat_80422574 = 0xFFFFFF00;
const f32 float_deg2rad_80422578 = 0.017453292f;
const f32 float_0_8042257c = 0.0f;
const f32 float_10_80422580 = 10.0f;
