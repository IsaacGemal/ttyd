#include "eff/eff_mahojin.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/mapdrv.h"
#include "dolphin/gx/GXTev.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"

// .data
static const char* name_tbl[21] = {
    "A_2_1", "S_2_1", "S_3_1",
    "A_2_2", "S_2_2", "S_3_2",
    "A_2_3", "S_2_3", "S_3_3",
    "A_2_4", "S_2_4", "S_3_4",
    "A_2_5", "S_2_5", "S_3_5",
    "A_2_6", "S_2_6", "S_3_6",
    "A_2_7", "S_2_7", "S_3_7",
};

Vec pos_tbl[7] = {
    { 0.0f, 0.0f, -14.3210001f },
    { 10.7070999f, 0.0f, -9.16479969f },
    { 13.8430004f, 0.0f, 2.79049993f },
    { 6.36530018f, 0.0f, 12.7509003f },
    { -6.08519983f, 0.0f, 12.9425001f },
    { -13.8746996f, 0.0f, 3.09509993f },
    { -11.1931f, 0.0f, -9.04349995f },
};

// .sbss
s32 mahojin_rate;

static void callback(TevCallbackParam* param);

// TEV colour callback: blends the colour toward purple (E8,00,E8) by mahojin_rate/255; alpha passes through
static void callback(TevCallbackParam* param) {
    s32 stage = param->field_C;
    s32 field_10 = param->field_10;
    s32 field_14 = param->field_14;
    s32 field_18 = param->field_18;
    GXSetTevColor(GX_TEVREG0, (GXColor){ 0xE8, 0x00, 0xE8, mahojin_rate });
    GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(stage, GX_CC_CPREV, GX_CC_C0, GX_CC_A0, GX_CC_ZERO);
    GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);

    param->field_C = stage + 1;
    param->field_14 = field_14;
    param->field_18 = field_18;
    param->field_10 = field_10;
}


EffectEntry* effMahojinEntry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    MahojinWork* data;
    s32 i;
    MahojinWork* entry;
    s32 count;

    effect = effEntry();
    // data[0]/data[1] are the circle's two base layers, the rest are min(type, 7) rim pieces
    count = ((type < 8) ? type : 7) + 2;
    effect->type = "Mahojin";
    effect->count = count;
    data = __memAlloc(HEAP_EFFECT, count * sizeof(MahojinWork));    effect->userdata = data;
    effect->callback = effMahojinMain;
    effect->flags |= 2;

    data[0].type = type;
    data[0].position.x = x;
    data[0].position.y = y;
    data[0].position.z = z;
    data[0].timer = 1000;
    data[0].field_0x14 = 0;
    data[0].animId[0] = -1;
    data[0].animId[1] = -1;
    data[0].alpha = 0xFF;
    data[0].state = 0;
    data[0].draw_arg = NULL;
    data[0].draw_func = NULL;

    data[1].type = type;
    data[1].position.x = x;
    data[1].position.y = y;
    data[1].position.z = z;
    data[1].timer = 1000;
    data[1].field_0x14 = 0;
    data[1].animId[0] = -1;
    data[1].animId[1] = -1;
    data[1].alpha = 0xFF;
    data[1].state = 0;
    data[1].draw_arg = NULL;
    data[1].draw_func = NULL;

    for (i = 2, entry = &data[2]; i < effect->count; i++, entry++) {
        entry->position.x = x;
        entry->position.y = y;
        entry->position.z = z;
        entry->animId[0] = -1;
        entry->animId[1] = -1;
        entry->alpha = 0;
        entry->state = 0;
        entry->draw_arg = NULL;
        entry->draw_func = NULL;
    }

    return effect;
}


void effMahojinMain(EffectEntry* effect) {
    s32 type;
    s32 inBattle;
    MahojinWork* data = effect->userdata;
    MahojinWork* entry;
    Vec disp;
    f32 loop;
    s32 i;

    disp = initializeVec(&data[0].position);
    loop = 0.0f;
    inBattle = gp->inBattle != 0;
    type = data[0].type;

    if (data[0].animId[0] == -1) {
        if (!animGroupBaseAsync("MOBJ_EFF_mahojin_omote", inBattle, NULL)) {
            return;
        }
        if (!animGroupBaseAsync("MOBJ_EFF_mahojin_ura", inBattle, NULL)) {
            return;
        }
        data[0].animId[0] = animPoseEntry("MOBJ_EFF_mahojin_omote", inBattle);
        data[0].animId[1] = animPoseEntry("MOBJ_EFF_mahojin_ura", inBattle);
        animPoseSetAnim(data[0].animId[0], "A_1", TRUE);
        animPoseSetAnim(data[0].animId[1], "A_1", TRUE);
        data[1].animId[0] = animPoseEntry("MOBJ_EFF_mahojin_omote", inBattle);
        data[1].animId[1] = animPoseEntry("MOBJ_EFF_mahojin_ura", inBattle);
        animPoseSetAnim(data[1].animId[0], "A_4", TRUE);
        animPoseSetAnim(data[1].animId[1], "A_4", TRUE);
        data = effect->userdata;
    }

    // soft-delete flag set by effSoftDelete: jump straight to the fade-out
    if (effect->flags & 4) {
        effect->flags &= ~4;
        data[0].timer = 16;
    }

    // 1000 means "no timeout"; alpha fades to 0 over the last 16 frames
    if (data[0].timer < 1000) {
        data[0].timer--;
    }
    if (data[0].timer < 16 && data[0].alpha > data[0].timer * 16) {
        data[0].alpha = data[0].timer * 16;
    }

    if (data[0].timer < 0) {
        entry = effect->userdata;
        for (i = 0; i < effect->count; i++, entry++) {
            if (entry->animId[0] != -1) {
                animPoseRelease(entry->animId[0]);
            }
            if (entry->animId[1] != -1) {
                animPoseRelease(entry->animId[1]);
            }
        }
        effDelete(effect);
        return;
    }

    data = effect->userdata;
    switch (data[0].state) {
    case 0:
        loop = animPoseGetLoopTimes(data[0].animId[0]);
        if (loop > 1.0f) {
            animPoseSetAnim(data[0].animId[0], "S_1", TRUE);
            animPoseSetAnim(data[0].animId[1], "S_1", TRUE);
            animPoseSetAnim(data[1].animId[0], "S_4", TRUE);
            animPoseSetAnim(data[1].animId[1], "S_4", TRUE);
            data[0].state++;
        }
        break;
    case 1:
        if (type == 7) {
            if (animPoseGetLoopTimes(data[effect->count - 1].animId[0]) > 1.0f) {
                mapSetTevCallback(10, callback);
                for (i = 0, entry = data; i < effect->count; entry++, i++) {
                    animPoseSetMaterialFlagOn(entry->animId[0], 0xA000000);
                    animPoseSetMaterialFlagOn(entry->animId[1], 0xA000000);
                }
                mahojin_rate = 0;
                data[0].state++;
            }
        }
        break;
    case 2:
        mahojin_rate += 2;
        if (mahojin_rate > 0xFF) {
            mahojin_rate = 0xFF;
            data[0].state++;
        }
        break;
    }

    entry = (MahojinWork*)effect->userdata;
    entry += 2;
    for (i = 2; i < effect->count; i++, entry++) {
        switch (entry->state) {
        case 0:
            // the first type-1 pieces fade in on S_3_n (S_2_n for types 7/8); the rest play A_2_n then S_2_n at full alpha
            if (i - 1 < type) {
                if (loop > 0.6f) {
                    entry->animId[0] = animPoseEntry("MOBJ_EFF_mahojin_omote", inBattle);
                    entry->animId[1] = animPoseEntry("MOBJ_EFF_mahojin_ura", inBattle);
                    if (type != 7 && type != 8) {
                        animPoseSetAnim(entry->animId[0], name_tbl[(i - 2) * 3 + 2], TRUE);
                        animPoseSetAnim(entry->animId[1], name_tbl[(i - 2) * 3 + 2], TRUE);
                    } else {
                        animPoseSetAnim(entry->animId[0], name_tbl[(i - 2) * 3 + 1], TRUE);
                        animPoseSetAnim(entry->animId[1], name_tbl[(i - 2) * 3 + 1], TRUE);
                    }
                    entry->alpha = 0;
                    entry->state = 1;
                }
            } else {
                if (loop > 0.8f) {
                    entry->animId[0] = animPoseEntry("MOBJ_EFF_mahojin_omote", inBattle);
                    entry->animId[1] = animPoseEntry("MOBJ_EFF_mahojin_ura", inBattle);
                    animPoseSetAnim(entry->animId[0], name_tbl[(i - 2) * 3 + 0], TRUE);
                    animPoseSetAnim(entry->animId[1], name_tbl[(i - 2) * 3 + 0], TRUE);
                    entry->alpha = 0xFF;
                    entry->state = 10;
                }
            }
            break;
        case 1:
            entry->alpha += 10;
            if (entry->alpha > 0xFF) {
                entry->alpha = 0xFF;
                entry->state = 2;
            }
            break;
        case 2:
            break;
        case 10:
            if (animPoseGetLoopTimes(entry->animId[0]) > 1.0f) {
                animPoseSetAnim(entry->animId[0], name_tbl[(i - 2) * 3 + 1], TRUE);
                animPoseSetAnim(entry->animId[1], name_tbl[(i - 2) * 3 + 1], TRUE);
                entry->state = 11;
            }
            break;
        case 11:
            break;
        }
    }

    dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effMahojinDisp, effect);
}


// The target keeps `j` live in a register and uses indexed `stwx`/`stfsx` stores in the dist[]/order[]
// initialiser. Writing `order[j++] = 0/1` (index expression, constant value) instead of `order[j] = j; j++`
// stops MWCC constant-folding the index, which reproduces the init exactly.
// The fill loop uses its own counter `n` while `j` carries on as the byte offset: indexing both arrays with
// `j` (or reusing the outer `i`) makes MWCC strength-reduce `order` to a pointer instead of the target's
// base+offset `stwx`. The separate `n` survives the dispCalcZ call in a callee-saved register, and the
// declaration order (`i, n, j, k`, then `alpha`, `flags`, `mapWork`) is what the allocator wants.
void effMahojinDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    MahojinWork* data = effect->userdata;
    Mtx mtx;
    f32 dist[16];
    s32 order[16];
    s32 i, n, j, k;
    s32 alpha;
    u16 flags;
    MapWork* mapWork;

    camGetPtr(camId);
    alpha = data[0].alpha;
    mapWork = mapGetWork();
    flags = mapWork->entries[0].flags;

    for (i = 0; i < 2; i++) {
        j = 0;
        if (i == 0) {
            dist[j] = -9999.0f;
            order[j++] = 0;
            dist[j] = -10000.0f;
            order[j++] = 1;
        } else {
            dist[j] = -10000.0f;
            order[j++] = 0;
            dist[j] = 10000.0f;
            order[j++] = 1;
        }

        for (n = 2; n < effect->count; n++) {
            dist[j] = dispCalcZ(&pos_tbl[n - 2]);
            order[j++] = n;
        }

        for (j = 0; j < effect->count - 1; j++) {
            for (k = j + 1; k < effect->count; k++) {
                if (dist[j] > dist[k]) {
                    f32 d = dist[j];
                    s32 o = order[j];
                    dist[j] = dist[k];
                    order[j] = order[k];
                    dist[k] = d;
                    order[k] = o;
                }
            }
        }

        mapWork->entries[0].flags &= ~1;
        PSMTXTrans(mtx, data[0].position.x, data[0].position.y, data[0].position.z);

        for (j = 0; j < effect->count; j++) {
            MahojinWork* entry = &data[order[j]];

            if (i == 1 && entry->draw_arg != NULL) {
                entry->draw_func(camId, entry->draw_arg);
            }

            if (entry->animId[i] != -1) {
                animPoseMain(entry->animId[i]);
                animPoseSetMaterialFlagOn(entry->animId[i], 0x40);
                animPoseSetMaterialEvtColor(entry->animId[i],
                    (GXColor){ 0xFF, 0xFF, 0xFF, entry->alpha * alpha / 255 });
                animPoseDrawMtx(entry->animId[i], mtx, 3, 0.0f, 10.0f);
            }
        }

        mapWork->entries[0].flags = flags;
    }
}


inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

// 24 zero bytes that end the unit's .data: an all-zero object needs explicit_zero_data to stay in .data and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_803AE248_data[6] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset
