#include "yuu_02.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_movefloor.h"
#include "evt/evt_msg.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_yuugijou.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "cam_shift.h"
#include "driver/dispdrv.h"
#include "driver/hitdrv.h"
#include "driver/icondrv.h"
#include "evt_yuucounter.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mariost.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "motion/mot_roll.h"
#include "party.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>
#include "evt_yuutitle.h"
#include "evt_yuunpc.h"


#pragma pool_data off

#pragma force_active on
extern s32 evt_yuuranking_alloc_31_text_52F8[];
extern s32 evt_yuuranking_free_31_text_5274[];
extern s32 yuuranking_get_monte_31_text_51C0[];
extern s32 yuuranking_wait_button_31_text_51F8[];
static USER_FUNC(re_fall_rtn);
static USER_FUNC(fall_rtn);
static void monte_disp(CameraId cameraId, void* param);
static s32 get_monte_check(Vec* a, Vec* b);

// clang-format off
extend_data(0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(2,
     f64, _three, 3,
     f64, _half, 0.5)

f32 fukkipoint[45] = {
    300.0f, 0.0f, 300.0f, 750.0f,
    0.0f, 300.0f, 1150.0f, 0.0f,
    300.0f, 1500.0f, 0.0f, 300.0f,
    1550.0f, 0.0f, -300.0f, 925.0f,
    200.0f, -300.0f, 250.0f, 200.0f,
    -300.0f, -250.0f, 200.0f, -300.0f,
    -950.0f, 200.0f, -300.0f, -1450.0f,
    0.0f, -300.0f, -1550.0f, 0.0f,
    300.0f, -950.0f, 0.0f, 300.0f,
    -250.0f, 0.0f, 300.0f, 250.0f,
    0.0f, 300.0f, 250.0f, 0.0f,
    300.0f,
};


EVT_BEGIN(unk_evt_yuu_000152bc)
    CALL(unk_yuu_00000f1c)
    RETURN
EVT_END()

static MonteTableEntry montetable[11] = {
    { 1500.0f, 990.0f, -200.0f, -300.0f, 1, 1 },
    { 1500.0f, 990.0f, -300.0f, -400.0f, 1, 2 },
    { 300.0f, 900.0f, -250.0f, -275.0f, 1, 1 },
    { 300.0f, 900.0f, -325.0f, -350.0f, 1, 2 },
    { 150.0f, -150.0f, -260.0f, -350.0f, 1, 2 },
    { -1070.0f, -1330.0f, -225.0f, -375.0f, 1, 2 },
    { -950.0f, -950.0f, 300.0f, 300.0f, 1, 1 },
    { -1250.0f, -1250.0f, 300.0f, 300.0f, 1, 1 },
    { -1350.0f, -1350.0f, 300.0f, 300.0f, 1, 1 },
    { -1450.0f, -1450.0f, 300.0f, 300.0f, 1, 2 },
    { 0.0f, 0.0f, 0.0f, 0.0f, 0, 0 },
};


static EVT_BEGIN(minigame_view_monte)
    CALL(fall_rtn)
    RETURN
EVT_END()

#ifdef MUST_MATCH
// The target keeps this in .rodata and reloads its three words every loop iteration; a plain
// `const` global lets MWCC hoist/CSE the loads out of the loop. The section attribute pins the
// non-const object to .rodata instead (the natural form is the `const` one under #else).
__declspec(section ".rodata") Vec vec3_yuu_00014008 = { 0.0f, 0.0f, 0.0f };
#else
const Vec vec3_yuu_00014008 = { 0.0f, 0.0f, 0.0f };
#endif
u32 gap_05_0000000C_bss;
static void* yuunpc_wp;
MonteEntry* unk_yuu_bss00000004;
static MonteEntry* monte_p;

USER_FUNC(unk_yuu_00001210) {
    camShiftReset();
    return EVT_RETURN_DONE;
}

void fn_31_112C(s32 index) {
    monte_p[index].active = 0;
}

void unk_yuu_000011e0(s32 index) {
    monte_p[index].active = 1;
}

USER_FUNC(unk_yuu_00001160) {
    s32* args = event->args;
    s32 on = evtGetValue(event, args[0]);
    s32 index = evtGetValue(event, args[1]);

    if (monte_p != NULL) {
        if (on != 0) {
            monte_p[index].active = 0;
        } else {
            monte_p[index].active = 1;
        }
    }
    return EVT_RETURN_DONE;
}

void unk_yuu_00001054(CameraId cameraId, void* param) {
    MonteEntry* board = (MonteEntry*)param;
    MarioWork* mario;
    s32 i;

    mario = marioGetPtr();
    if (board != NULL) {
        for (i = 0; i < board->count; i++) {
            f32 dir = mario->position.z >= 0.0f ? 1.0f : -1.0f;
            if (board[i].active == 0) {
                Vec pos = board[i].pos;
                if ((gp->retraceCount & 0x1F) < 9) {
                    pos.x -= 5.0f * dir;
                }
                iconDispGx(-2.0f * dir, &pos, 0, 0x163);
            }
        }
    }
}

#pragma opt_propagation off // keep count*sizeof(MonteEntry) from folding to li 0x44 (count==1)
#ifdef MUST_MATCH
// Match-only shape: the loop counter, the two base pointers and `arr` are separate locals, and `arr`
// is assigned from monte_p inside the loop condition. MWCC colours an inlined helper's locals in
// declaration order and reloads monte_p for the condition, which puts the counter in r9, the
// per-iteration monte_p load in r8 and the row base in r10, as the target has them. The natural
// inline form is the single-`base` loop in the function's #else branch.
static inline void monte_fill(void) {
    s32 i;
    f32* base;
    f32* base2;
    MonteEntry* arr;
    for (i = 0; i < (arr = monte_p)->count; i++) {
        base = (f32*)((u8*)&arr[i] + 8);
        base2 = (f32*)((u8*)&arr[i] + 8);
        *(Vec*)&base[1] = vec3_yuu_00014008;
        base2[4] = 1.0f;
        monte_p[i].active = 1;
    }
}
#endif

// `#pragma opt_propagation off` above is required: with propagation on MWCC folds
// count*sizeof(MonteEntry) (count==1) to li 0x44 and drops the target's li r28,1 /
// mulli r29,r28,0x44 / mr r3,r29 sequence and the r28 save.
USER_FUNC(unk_yuu_00000f1c) {
    s32 already = 0;
    s32 count;
    YuuminigameWork* work = (YuuminigameWork*)event->lwData[0];
    s32 i;

    marioGetPtr();
    if (isFirstCall) {
        if (monte_p != NULL) {
            already = 1;
        }
        count = 1;
        monte_p = (MonteEntry*)_mapAlloc(count * sizeof(MonteEntry));
        memset(monte_p, 0, count * sizeof(MonteEntry));
        monte_p->count = count;
        monte_p->work = work;
#ifdef MUST_MATCH
        monte_fill();
#else
        for (i = 0; i < monte_p->count; i++) {
            f32* base = (f32*)((u8*)&monte_p[i] + 8);
            *(Vec*)&base[1] = vec3_yuu_00014008;
            base[4] = 1.0f;
            monte_p[i].active = 1;
        }
#endif
    }
    for (i = 0; i < monte_p->count; i++) {
    }
    if (already == 0) {
        dispEntry(8, 0.0f, 0, unk_yuu_00001054, monte_p);
    }
    return EVT_RETURN_BLOCK;
}
#pragma opt_propagation reset

static s32 get_monte_check(Vec* a, Vec* b) {
    if (PSVECDistance(a, b) < 20.0f && __fabs(a->y - b->y) < 20.0) {
        return TRUE;
    }
    return FALSE;
}

static void monte_disp(CameraId cameraId, void* param) {
    MonteEntry* board = (MonteEntry*)param;
    s32 i;

    if (board != NULL) {
        for (i = 0; i < board->count; i++) {
            if (board[i].active == 0) {
                iconDispGx(1.0f, &board[i].pos, 0, 0x13F);
            }
        }
    }
}

static inline f32 monte_rand(f32 min, f32 max) {
    return min + (f32)irand((s32)(100.0f * (max - min))) / 100.0f;
}

USER_FUNC(fall_rtn) {
    YuuminigameWork* work = (YuuminigameWork*)event->lwData[0];
    s32 wasAllocated = 0;
    YuuMinigame3Data* data = (YuuMinigame3Data*)work->data;
    s32 total;
    s32 i;
    s32 tableIdx;
    s32 pointIdx;
    s32 placed;
    s32 allocSize;

    if (isFirstCall) {
        if (unk_yuu_bss00000004 != NULL) {
            wasAllocated = 1;
        }
        total = 0;
        i = 0;
        while (montetable[i].type != 0) {
            total += montetable[i].count;
            i++;
        }
        allocSize = total * 0x44;
        unk_yuu_bss00000004 = (MonteEntry*)_mapAlloc(allocSize);
        memset(unk_yuu_bss00000004, 0, allocSize);
        unk_yuu_bss00000004->count = total;
        unk_yuu_bss00000004->work = work;
        data->field_24 = 0;

        tableIdx = 0;
        pointIdx = 0;
        placed = 0;
        for (i = 0; i < unk_yuu_bss00000004->count; i++) {
            s32 k;
            s32 j;
            f32* base = (f32*)((u8*)&unk_yuu_bss00000004[i] + 8);
            f32 min;
            f32 max;
            Vec d;
            Vec cur;
            Vec prev;
            HitCheckQuery query;
            Vec hit;

            base[2] = 210.0f;
            j = 0;
            do {
                j++;
                if (j > 500) {
                    *(Vec*)(base + 1) = (Vec){0.0f, 0.0f, 0.0f};
                    break;
                }
                if (montetable[tableIdx].x0 < montetable[tableIdx].x1) {
                    min = montetable[tableIdx].x0;
                    max = montetable[tableIdx].x1;
                } else {
                    min = montetable[tableIdx].x1;
                    max = montetable[tableIdx].x0;
                }
                base[1] = monte_rand(min, max);
                if (montetable[tableIdx].z0 < montetable[tableIdx].z1) {
                    min = montetable[tableIdx].z0;
                    max = montetable[tableIdx].z1;
                } else {
                    min = montetable[tableIdx].z1;
                    max = montetable[tableIdx].z0;
                }
                max = max - min;
                base[3] = min + (f32)irand((s32)(100.0f * max)) / 100.0f;

                cur = *(Vec*)(base + 1);
                cur.y = 0.0f;
                for (k = 0; k < pointIdx; k++) {
                    u8* prevp = (u8*)base - (k + 1) * 0x38 + 4;
                    prev = *(Vec*)prevp;
                    prev.y = 0.0f;
                    PSVECSubtract(&cur, &prev, &d);
                    if (PSVECMag(&d) < 20.0f) {
                        break;
                    }
                }
                if (k != pointIdx) {
                    continue;
                }

                {
                    HitCheckQuery* q = &query;
                    q->targetPos = *(Vec*)(base + 1);
                    q->targetDistance = 220.0f;
                    q->targetDir = (Vec){0.0f, -1.0f, 0.0f};
                    if (hitCheckVecFilter(q, NULL) == NULL) {
                        continue;
                    }
                    q->targetPos.x -= 9.0f;
                    hit = q->hitPos;
                    if (hitCheckVecFilter(q, NULL) != NULL) {
                        if (q->hitPos.y > hit.y + 1.0f) {
                            continue;
                        }
                    }
                    base[2] = q->hitPos.y;
                }
                break;
            } while (1);
            pointIdx++;
            placed++;
            if (pointIdx >= montetable[tableIdx].count) {
                if (montetable[tableIdx].type == 2) {
                    s32 n;
                    s32 r;
                    for (n = 0; n < placed; n++) {
                        unk_yuu_bss00000004[i - n].active = 1;
                        unk_yuu_bss00000004[i - n].hit = 1;
                    }
                    n = 0;
                    while (n < montetable[tableIdx].count) {
                        do {
                            r = irand(placed);
                        } while (unk_yuu_bss00000004[i - r].active == 0);
                        unk_yuu_bss00000004[i - r].active = 0;
                        unk_yuu_bss00000004[i - r].hit = 0;
                        n++;
                    }
                    placed = 0;
                }
                pointIdx = 0;
                tableIdx++;
            }
            base[4] = 1.0f;
        }
    }

    {
#ifdef MUST_MATCH
        // match-only: the union member makes MWCC materialise the address directly (no `mr`); the plain pointer
        // is the natural form (see yuu_01.c fall_rtn).
        union { const char* p; u32 u; } sfx;
        sfx.p = "SFX_EVT_GAME_MONTE_COUNT1";
#else
        const char* sfx = "SFX_EVT_GAME_MONTE_COUNT1";
#endif
        for (i = 0; i < unk_yuu_bss00000004->count; i++) {
            MonteEntry* p = &unk_yuu_bss00000004[i];
            switch (p->hit) {
            case 0:
                if (get_monte_check(&p->pos, &marioGetPtr()->position) == 1) {
                    data->field_24++;
                    psndSFXOn_3D(
#ifdef MUST_MATCH
                        (const char*)(u32)sfx.p,
#else
                        sfx,
#endif
                        &marioGetPtr()->position);
                    p->active = 1;
                    p->hit = 1;
                }
                break;
            case 1:
                break;
            }
        }
    }
    if (wasAllocated == 0) {
        dispEntry(4, 0.0f, 1, monte_disp, unk_yuu_bss00000004);
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(re_fall_rtn) {
    if (unk_yuu_bss00000004 != NULL) {
        YuuMinigame3Data* data =
            (YuuMinigame3Data*)((YuuminigameWork*)unk_yuu_bss00000004->work)->data;
        if (data->viewEvent != NULL) {
            evtDeleteID(data->viewEvent->eventId);
            data->viewEvent = NULL;
            _mapFree(unk_yuu_bss00000004);
            data->viewEvent = evtEntryType(minigame_view_monte, 0, 0, 0);
            data->viewEvent->lwData[0] = (s32)unk_yuu_bss00000004->work;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(yuuminigame3_set_phase) {
    s32* args = event->args;
    YuuminigameWork* work = (YuuminigameWork*)evtGetValue(event, args[0]);
    YuuMinigame3Data* data = (YuuMinigame3Data*)work->data;

    data->phase = evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(yuuminigame3_roll_fix_on) {
    marioRollFixOn();
    return EVT_RETURN_DONE;
}

USER_FUNC(yuuminigame3_roll_fix_off) {
    return EVT_RETURN_DONE;
}

USER_FUNC(yuuminigame3_opening_on_off) {
    s32* args = event->args;
    YuuMinigame3Data* data =
        (YuuMinigame3Data*)((YuuminigameWork*)evtGetValue(event, args[0]))->data;

    if (evtGetValue(event, args[1]) != 0) {
        data->flags |= 1;
    } else {
        data->flags &= ~1;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(yuuminigame3_opening_chk) {
    s32* args = event->args;
    YuuMinigame3Data* data =
        (YuuMinigame3Data*)((YuuminigameWork*)evtGetValue(event, args[0]))->data;

    if (data->flags & 1) {
        evtSetValue(event, args[1], 1);
    } else {
        evtSetValue(event, args[1], 0);
    }
    return EVT_RETURN_DONE;
}

void yuuminigame_init_3roll(EventEntry* event, BOOL isFirstCall) {
    YuuMinigame3Data* data = ((YuuminigameWork*)event->lwData[0])->data;
    EventEntry* e;

    e = evtEntryType(unk_evt_yuu_000152bc, 0, 0, 0xEF);
    e->lwData[0] = (s32)event->lwData[0];
    data->viewEvent = evtEntryType(minigame_view_monte, 0, 0, 0);
    data->viewEvent->lwData[0] = (s32)event->lwData[0];
    data->flags |= 1;
    marioChgRollMotion();
}

void yuuminigame_main_3roll(EventEntry* event, BOOL isFirstCall) {
    YuuminigameWork* work = (YuuminigameWork*)event->lwData[0];
    YuuMinigame3Data* data = (YuuMinigame3Data*)work->data;
    MarioWork* mario = marioGetPtr();

    switch (data->phase) {
    case 5:
        data->phase = 1;
        // fallthrough
    case 1:
        work->childEvent = evtEntry(minigame03_start, 0, 0);
        work->childEvent->lwData[0] = (s32)event->lwData[0];
        data->phase++;
        break;
    case 2:
        data->position = mario->position;
        data->phase = 3;
        data->field_0C = 0;
        // fallthrough
    case 3:
        if (yuuminigame3_gamerule(event, isFirstCall, work, data)) {
            data->phase++;
        }
        break;
    case 4:
        work->childEvent = evtEntry(minigame03_end, 0, 0);
        work->childEvent->lwData[0] = data->counter;
        if (data->field_0C + 5 <= 0) {
            data->field_0C = 1;
        } else {
            data->field_0C += 0x105;
        }
        work->childEvent->lwData[1] = data->field_0C;
        work->childEvent->lwData[2] = data->field_24;
        data->phase++;
        break;
    default:
        break;
    }
}

void yuuminigame_disp_3roll(CameraId cameraId, void* param) {
    monteCountDisp();
}

USER_FUNC(yuu_roll_reset) {
    marioResetRollSpd();
    clrRollEvtFlag();
    return EVT_RETURN_DONE;
}

s32 yuuminigame3_gamerule(EventEntry* event, BOOL isFirstCall, void* work, YuuMinigame3Data* data) {
    MarioWork* mario = marioGetPtr();
    s32 adj;

    if (!(data->flags & 2) && mario->position.y <= -50.0f) {
        data->flags |= 2;
        data->event = evtEntry(yuu_02_jumpret, 0, 0);
        adj = fukkipoint[data->index * 3 + 2] >= 0.0f ? 5 : -5;
        {
            s32 v = (s32)fukkipoint[data->index * 3 + 0];
            data->event->lwData[0] = v + adj;
        }
        data->event->lwData[1] = (s32)fukkipoint[data->index * 3 + 1] + 200;
        data->event->lwData[2] = (s32)fukkipoint[data->index * 3 + 2];
        data->event->lwData[3] = (s32)fukkipoint[data->index * 3 + 1];
        data->field_0C--;
    }

    if (!(data->flags & 2)) {
        if (mario->position.z * fukkipoint[data->index * 3 + 5] >= 0.0f &&
            mario->position.y >= fukkipoint[data->index * 3 + 4] - 1.0f) {
            if (mario->position.z >= 0.0f) {
                if (data->position.x < fukkipoint[data->index * 3 + 3] &&
                    mario->position.x >= fukkipoint[data->index * 3 + 3]) {
                    goto advance;
                }
            }
            if (mario->position.z < 0.0f) {
                if (data->position.x > fukkipoint[data->index * 3 + 3] &&
                    mario->position.x <= fukkipoint[data->index * 3 + 3]) {
                    goto advance;
                }
            }
        }
        goto check_back;
    advance:
        if (data->index < 0xE) {
            data->index++;
        }
        monte_p->active = 1;
    check_back:
        if (mario->position.z * fukkipoint[data->index * 3 + 2] >= 0.0f) {
            if (mario->position.z < 0.0f) {
                if (data->position.x < fukkipoint[data->index * 3 + 0] &&
                    mario->position.x >= fukkipoint[data->index * 3 + 0]) {
                    goto retreat;
                }
            }
            if (mario->position.z >= 0.0f) {
                if (data->position.x > fukkipoint[data->index * 3 + 0] &&
                    mario->position.x <= fukkipoint[data->index * 3 + 0]) {
                    goto retreat;
                }
            }
        }
        goto finish;
    retreat:
        if (data->index > 0) {
            data->index--;
        }
        monte_p->active = 0;
    finish:
        data->position = mario->position;
        if (yuucounter_get_data() < 0 || data->index >= 0xD) {
            data->counter = yuucounter_get_data();
            data->index = 0;
            marioKeyOff();
            partyKeyOff();
            yuucounter_flag_on(2);
            monte_p->active = 1;
            return 1;
        }
    } else {
        data->position = mario->position;
        if (!evtCheckID(data->event->eventId)) {
            data->flags &= ~2;
        }
    }
    return 0;
}



EVT_BEGIN(yuu_02_jumpret)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam_letter_box_onoff, 1, 0)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_cam_get_pos, 4, LW(7), LW(8), LW(9))
    CALL(evt_cam_get_at, 4, LW(10), LW(11), LW(12))
    CALL(evt_cam3d_evt_set, LW(7), LW(8), LW(9), LW(10), LW(11), LW(12), 0, 0)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(4), LW(5), LW(6))
        IF_INT_LESS_OR_EQUAL(LW(5), -400)
            CALL(evt_mario_cont_onoff, 0)
            CALL(evt_mario_dispflag_onoff, 1, 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_mario_set_pos, LW(0), LW(3), LW(2))
    CALL(yuu_roll_reset)
    CALL(evt_mario_cam_y_on)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_off, 1000, 0)
    WAIT_MS(1000)
    CALL(evt_cam_get_pos, 4, LW(7), LW(8), LW(9))
    CALL(evt_cam_get_at, 4, LW(10), LW(11), LW(12))
    CALL(evt_cam3d_evt_set, LW(7), LW(8), LW(9), LW(10), LW(11), LW(12), 0, 0)
    CALL(evt_mario_cam_y_off)
    CALL(evt_mario_set_pos, LW(0), LW(1), LW(2))
    CALL(evt_mario_cont_onoff, 1)
    CALL(evt_mario_dispflag_onoff, 0, 1)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(3), LW(4), LW(5))
        CALL(evt_mario_set_pos, LW(0), LW(4), LW(2))
        CALL(evt_mario_get_name_hitobj_ride, LW(6))
        IF_STR_NOT_EQUAL(LW(6), 0)
            CALL(evt_eff, STRING(""), STRING("kemuri"), 0, LW(0), LW(4), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_cam3d_evt_off, 500, 0)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_cam_letter_box_disable, 1)
    RETURN
EVT_END()

EVT_BEGIN(yuu_02_kaijo_init)
    CALL(evt_mario_set_pos, 200, 0, 300)
    CALL(evt_mario_set_dir, 90, 0, 0)
    RETURN
EVT_END()

EVT_BEGIN(yuu_02_kaijo_init2)
    CALL(evt_mario_set_pos, 200, 0, 300)
    CALL(evt_mario_set_dir, 90, 0, 0)
    RETURN
EVT_END()


EVT_BEGIN(minigame03_main)
    CALL(evt_mario_key_onoff, 0)
    CALL_CHILD(yuu_02_kaijo_init)
    CALL(evt_yuuminigame_alloc, LW(0), 2)
    CALL(yuuminigame_set_dataadr, 0, LW(0))
    CALL(evt_mario_kill_party, 0)
    CALL(evt_mario_kill_party, 1)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE1"), GW(10))
    CALL(yuuminigame_get_dataadr, 0, LW(0))
    CALL(yuuminigame3_set_phase, LW(0), 1)
    RETURN
EVT_END()

EVT_BEGIN(minigame03_start_cam)
    CALL(evt_cam3d_evt_set, FLOAT(-1600.0f), FLOAT(573.0f), FLOAT(1333.0f), FLOAT(-1600.0f), FLOAT(12.0f), FLOAT(-89.0f), 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_seq_wait, 2)
    CALL(evt_cam3d_evt_set, FLOAT(1600.0f), FLOAT(573.0f), FLOAT(1333.0f), FLOAT(1600.0f), FLOAT(12.0f), FLOAT(-89.0f), 8000, 0)
    WAIT_MS(1000)
    CALL(evt_yuutitle_alloc, 0, 0, STRING("menu_title_minigame3"))
    WAIT_MS(5000)
    CALL(evt_yuutitle_free)
    WAIT_MS(2000)
    RETURN
EVT_END()

EVT_BEGIN(minigame03_start_cam_cancel)
    CALL(evt_seq_wait, 2)
    CALL(evt_yuutitle_free)
    CALL(evt_cam3d_evt_off, 0, 11)
    WAIT_FRAMES(1)
    CALL(evt_mario_wait_move_end)
    CALL_CHILD(yuu_02_kaijo_init2)
    RETURN
EVT_END()




EVT_BEGIN(minigame03_start)
    CALL(yuu_atai_init)
    CALL(yuu_atai_push, LW(0))
    SET_VALUE(LW(0), STRING("S_ugoku"))
    SET_VALUE(LW(1), STRING("ugo1"))
    SET_VALUE(LW(2), 0)
    SET_VALUE(LW(3), -200)
    SET_VALUE(LW(4), 2400)
    SET_VALUE(LW(5), 600)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 1, LW(15))
    SET_VALUE(LW(0), STRING("S_ugoku1"))
    SET_VALUE(LW(1), STRING("ugo2"))
    SET_VALUE(LW(2), 0)
    SET_VALUE(LW(3), 200)
    SET_VALUE(LW(4), 2400)
    SET_VALUE(LW(5), 600)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    SET_VALUE(LW(8), 0)
    SET_VALUE(LW(9), 0)
    SET_VALUE(LW(10), 0)
    SET_VALUE(LW(11), 0)
    SET_VALUE(LW(12), 0)
    SPAWN_EVENT_GET_ID(PTR(moving_floor), LW(15))
    CALL(yuuminigame_set_dataadr, 2, LW(15))
    CALL(yuu_atai_pop, LW(0))
    CALL(yuuminigame3_opening_chk, LW(0), LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        SPAWN_EVENT_GET_ID(PTR(minigame03_start_cam), LW(8))
        LOOP(0)
            IS_EVENT_RUNNING(LW(8), LW(9))
            IF_INT_EQUAL(LW(9), 0)
                CALL(evt_seq_wait, 2)
                BREAK_LOOP
            ELSE
                CALL(evt_key_get_buttontrg, 0, LW(9))
                IF_BITS_SET(LW(9), 768)
                    DELETE_EVENT(LW(8))
                    CALL(evt_seq_wait, 2)
                    CALL_CHILD(minigame03_start_cam_cancel)
                    BREAK_LOOP
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(yuuminigame3_opening_on_off, LW(0), 0)
        CALL_CHILD(yuu_02_kaijo_init2)
        CALL(evt_cam3d_evt_off, 0, 11)
        WAIT_FRAMES(1)
    END_IF
    CALL(evt_mario_mov_pos2, 305, 300, 120)
    CALL(evt_snd_bgmon, 512, STRING("BGM_EVT_MINIGAME1"))
    CALL(eff_call_minigame, 0, 0, 0, 500)
    WAIT_MS(3000)
    CALL(yuuminigame3_roll_fix_on)
    CALL(evt_cam_letter_box_onoff, 0, 0)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_yuucounter_alloc, 0, 70, 1, 180)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()




EVT_BEGIN(minigame03_end)
    CALL(evt_cam_letter_box_onoff, 1, 0)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE2"), 0)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_LESS(LW(1), -50)
                CALL(evt_mario_jump_pos, 300, 0, 300, 600, 0, -1000)
            END_IF
            IF_INT_EQUAL(GF(0), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    CALL(yuuminigame_get_dataadr, 1, LW(15))
    CALL(evt_moving_floor_delete, LW(15))
    CALL(yuuminigame_get_dataadr, 2, LW(15))
    CALL(evt_moving_floor_delete, LW(15))
    CALL(yuuminigame3_roll_fix_off)
    IF_INT_LESS(LW(0), 0)
        CALL(eff_call_minigame, 2, 0, 0, 500)
        WAIT_MS(4000)
    ELSE
        CALL(eff_call_minigame, 1, 0, 0, 500)
        WAIT_MS(4000)
    END_IF
    CALL(evt_yuucounter_free)
    CALL(yuugijou_montemedal_disp_onoff, 1)
    CALL(evt_yuuranking_alloc_31_text_52F8, 0, 0, 2, 0, LW(0), LW(2), LW(1), 180)
    WAIT_FRAMES(1)
    CALL(yuuranking_wait_button_31_text_51F8)
    CALL(yuuranking_get_monte_31_text_51C0, LW(3))
    CALL(yuugijou_add_montemedal, 0, LW(3))
    CALL(yuugijou_montemedal_wait)
    CALL(evt_yuuranking_free_31_text_5274)
    WAIT_MS(200)
    CALL(evt_yuuranking_alloc_31_text_52F8, 0, 0, 2, 1, LW(0), LW(2), LW(1), 180)
    WAIT_FRAMES(1)
    CALL(yuuranking_wait_button_31_text_51F8)
    CALL(evt_yuuranking_free_31_text_5274)
    SET_VALUE(LW(1), 10)
    CALL(yuugijou_get_montemedal, LW(0))
    IF_INT_LESS(LW(0), LW(1))
        SET_VALUE(LW(0), 1)
    ELSE
        CALL(evt_msg_select, 0, STRING("sysyuu_02_00"))
    END_IF
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(GSWF(1229), 1)
        CALL(evt_mario_set_prev_party_pos, 0, 0, -1000, 0)
        CALL(evt_mario_set_prev_party_pos, 1, 0, -1000, 0)
        CALL(evt_bero_mapchange, STRING("gor_03"), STRING("A_dokan_02"))
        CALL(evt_snd_bgmoff, 512)
        RETURN
    END_IF
    MULTIPLY(LW(1), -1)
    CALL(yuugijou_add_montemedal, 0, LW(1))
    CALL(yuugijou_montemedal_wait)
    WAIT_MS(200)
    CALL(yuugijou_montemedal_disp_onoff, 0)
    CALL(evt_snd_bgmoff, 512)
    CALL(evt_snd_sfxoff, GW(10))
    CALL(evt_fade_out, 1000)
    CALL(re_fall_rtn)
    SET_VALUE(GF(0), 0)
    CALL_CHILD(yuu_02_kaijo_init2)
    CALL(unk_yuu_00001210)
    WAIT_MS(500)
    CALL(evt_snd_sfxon, STRING("SFX_EVT_GAME_AUDIENCE1"), GW(10))
    CALL(evt_fade_in, 1000)
    RETURN
EVT_END()

EVT_BEGIN(s_en1_turn)
    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
    SET_FLOAT_VALUE(LW(1), FLOAT(0.75f))
    LOOP(0)
        ADD_FLOAT(LW(0), LW(1))
        IF_INT_GREATER_OR_EQUAL(LW(0), FLOAT(360.0f))
            SUBTRACT_FLOAT(LW(0), FLOAT(360.0f))
        END_IF
        CALL(evt_mapobj_rotate, 1, STRING("S_en1"), FLOAT(0.0f), LW(0), FLOAT(0.0f))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(yuu_02_init_evt)
    CALL(evt_map_playanim, STRING("den1"), 1, 0)
    CALL(evt_map_playanim, STRING("den2"), 1, 0)
    CALL(evt_map_playanim, STRING("kumo"), 1, 0)
    CALL(evt_map_playanim, STRING("wakka"), 1, 0)
    SPAWN_EVENT(s_en1_turn)
    SPAWN_EVENT(minigame03_main)
    CALL_CHILD(evt_yuunpc_minigame3)
    RETURN
EVT_END()

// 4-byte zero the original keeps at the end of .data, after yuu_02_init_evt.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_04_000000E4_data = 0;
#pragma force_active reset
#pragma explicit_zero_data reset











// clang-format on
