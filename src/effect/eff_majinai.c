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
    work->scale = 1.0f;
    work->alpha = 0xff;
    work->state = 0;
    work->animPose = -1;

    map = mapGetWork();
    map->entries[0].blend[2] = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };

    return effect;
}

// Materialising the boolean through an inline makes MWCC colour it r29 (the plain
// `inBattle = gp->inBattle != 0;` gets r28); the inlined body emits the identical instructions.
static inline s32 effMajinaiGetInBattle(void) {
    return gp->inBattle != 0;
}

void effMajinaiMain(EffectEntry* effect) {
    s32 inBattle;
    s32 i;
    Vec position;
    MajinaiWork* work = effect->userdata;
    Vec disp = { 0.0f, 0.0f, 0.0f };
    MapWork* map;

    disp.x = work->x;
    disp.y = work->y;
    disp.z = work->z;
    // Retail builds the Vec in a scratch and copies it to a second local before dispCalcZ;
    // a single local folds the copy away.
    position = disp;
    inBattle = effMajinaiGetInBattle();

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
                s32 value = map->entries[0].blend[2].r;

                value -= 5;
                if (value < 0x40) {
                    value = 0x40;
                }
                map->entries[0].blend[2] = (GXColor){ value, value, value, 0xFF };
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
                f32 scale = work->scale;
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
                s32 value = map->entries[0].blend[2].r;

                value += 5;
                if (value > 0xff) {
                    value = 0xff;
                }
                map->entries[0].blend[2] = (GXColor){ value, value, value, 0xFF };
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

    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effMajinaiDisp, effect);
}

void effMajinaiDisp(CameraId cameraId, void* param) {
    EffectEntry* effect = param;
    MajinaiWork* work = effect->userdata;
    Mtx trans;
    Mtx scale;
    Mtx rot;

    if (work->animPose != -1) {
        PSMTXTrans(trans, work->x, work->y, work->z);
        PSMTXScale(scale, work->scale, work->scale, work->scale);
        PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(cameraId)->viewYaw);
        PSMTXConcat(trans, rot, trans);
        PSMTXConcat(trans, scale, trans);
        animPoseSetMaterialFlagOn(work->animPose, 0x840);
        animPoseSetMaterialEvtColor(work->animPose, (GXColor){ 0xFF, 0xFF, 0xFF, work->alpha });
        animPoseMain(work->animPose);
        animPoseDrawMtx(work->animPose, trans, 1, 0.0f, 10.0f);
        animPoseDrawMtx(work->animPose, trans, 2, 0.0f, 10.0f);
        animPoseDrawMtx(work->animPose, trans, 3, 0.0f, 10.0f);
    }
}
