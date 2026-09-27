#include "evt/evt_npc.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include "driver/npcdrv.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "system.h"
#include <string.h>
#include "koopa.h"
#include "driver/mobjdrv.h"
#include "eff/n64/eff_magic1_n64.h"
#include "eff/eff_fire.h"
#include "math_ext.h"

// From driver/mobjdrv.c (no prototype in header)

// From driver/npcdrv.c (not yet decompiled / no prototype in header)
// From mario/... (not yet decompiled)
// From driver/effdrv.c (not yet decompiled)

USER_FUNC(gesso_ground_check) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");

    f32 hitX, hitY, hitZ;
    f32 distance;
    f32 hitNX, hitNY, hitNZ;

    distance = 5.0f;
    if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, npc->position.x,
                       npc->position.y + 1.0f, npc->position.z, 0.0f, -1.0f, 0.0f)) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(set_bottle_flag_init) {
    NpcEntry* npc = evtNpcNameToPtr(event, "slave_0");
    npc->flags |= 0x28020020;
    *(u16*)npc->field_31E |= 0x1F;
    return EVT_RETURN_DONE;
}

// NONMATCHING (99.94%): only the per-TU string-pool base offset differs; instruction sequence identical.
USER_FUNC(dokanCheck) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    char buf[32];
    MapObjectEntry* mobj;

    if (strcmp(npc->description,
               "\x8c\x52\x92\x63\x83\x55\x83\x52\x82\xaa\x82\xc5\x82\xe9\x93\x79\x8a\xc7\x82\x50") == 0) {
        strcpy(buf, "mobj9");
    }
    if (strcmp(npc->description,
               "\x8c\x52\x92\x63\x83\x55\x83\x52\x82\xaa\x82\xc5\x82\xe9\x93\x79\x8a\xc7\x82\x51") == 0) {
        strcpy(buf, "mobj14");
    }
    mobj = mobjNameToPtrNoAssert(buf);
    if (mobj != NULL) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

static void piders_draw_yarn_sub(CameraId cameraId, void* param);

USER_FUNC(limitSpd) {
    s32* args = event->args;
    f32 speed = evtGetFloat(event, args[0]);
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    npc->field_1D0 = speed;
    return EVT_RETURN_DONE;
}

USER_FUNC(getSpdRun) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    evtSetFloat(event, args[0], npc->territoryHoming.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(getSpdWalk) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    evtSetFloat(event, args[0], npc->territoryLoiter.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(camGetPos) {
    s32* args = event->args;
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    evtSetFloat(event, args[0], cam->cameraPos.x);
    return EVT_RETURN_DONE;
}

USER_FUNC(kamec_fire_magic_delete) {
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    EffectEntry* eff = effNameToPtr(npc->description);
    if (eff != NULL) {
        *(s32*)((u8*)eff->userdata + 0x30) = 1;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(piders_draw_yawn) {
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    dispEntry(CAMERA_3D, 0.0f, 1, piders_draw_yarn_sub, npc);
    return EVT_RETURN_DONE;
}

// NONMATCHING (96.58%): Vec temp stack slots shifted by 4 bytes and base2/pos2 copies partly merged; GX calls identical.
static void piders_draw_yarn_sub(CameraId cameraId, void* param) {
    NpcEntry* npc = param;
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    GXColor color = { 0xFF, 0xFF, 0xFF, 0xFF };
    Vec base = { 0.0f, 0.0f, 0.0f };
    Vec pos = { 0.0f, 0.0f, 0.0f };
    Vec base2;
    Vec pos2;

    base.x = npc->territoryBase.x;
    base.y = npc->territoryBase.y + 30.0f;
    base.z = npc->territoryBase.z;
    pos.x = npc->position.x;
    pos.y = npc->position.y + 30.0f;
    pos.z = npc->position.z;

    base2 = base;
    pos2 = pos;

    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
    GXSetChanMatColor(GX_COLOR0A0, color);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXLoadPosMtxImm(cam->view, 0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(base2.x - 1.0f, base2.y, base2.z - 5.0f);
    GXPosition3f32(base2.x + 1.0f, base2.y, base2.z - 5.0f);
    GXPosition3f32(pos2.x + 1.0f, pos2.y, pos2.z - 5.0f);
    GXPosition3f32(pos2.x - 1.0f, pos2.y, pos2.z - 5.0f);
    GXEnd();
}

USER_FUNC(piders_get_height) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    evtSetFloat(event, args[0], npc->territoryLoiter.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(_wait_blow_end) {
    s32 id = evtGetValue(event, *event->args);
    NpcEntry* npc = evtNpcNameToPtr(event, (const char*)id);
    if (npcCheckBlow(npc)) {
        npc->rotation.y += 18.0f;
        return EVT_RETURN_BLOCK;
    }
    npc->rotation.y = 0.0f;
    return EVT_RETURN_DONE;
}

USER_FUNC(npc_check_wall_stop) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    if (npc->field_2FC != 0) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(killer_make_name) {
    s32* args = event->args;
    static char name[32];
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    s32 value = evtGetValue(event, args[0]);
    s32 len;

    strcpy(name, npc->description);
    len = strlen(name);
    name[len] = value + 0x31;
    name[len + 1] = '\0';
    evtSetValue(event, args[1], (s32)name);
    return EVT_RETURN_DONE;
}

// NONMATCHING (99.09%): offset temp allocated to f1 instead of f0 (f0/f1 constant swap); rest identical.
USER_FUNC(_2d_get_dead_jump_offset) {
    s32* args = event->args;
    NpcEntry* npc;
    MarioWork* mario;
    s32 arg1;
    s32 name;
    f32 offset;

    name = evtGetValue(event, args[0]);
    npc = evtNpcNameToPtr(event, (const char*)name);
    arg1 = args[1];
    mario = marioGetPtr();
    if (mario->position.x == npc->position.x) {
        offset = 0.0f;
    } else if (mario->position.x > npc->position.x) {
        offset = -1.0f;
    } else {
        offset = 1.0f;
    }
    evtSetValue(event, arg1, (s32)(20.0f * offset));
    return EVT_RETURN_DONE;
}

USER_FUNC(upperCheck) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    f32 hitX, hitY, hitZ;
    f32 distance;
    f32 hitNX, hitNY, hitNZ;
    HitEntry* hitA;
    HitEntry* hitB;

    distance = 5.0f;
    hitA = hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, npc->position.x - 15.0f,
                          npc->position.y + 30.0f, npc->position.z, 0.0f, 2.0f, 0.0f);
    hitB = hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, npc->position.x + 15.0f,
                          npc->position.y + 30.0f, npc->position.z, 0.0f, 2.0f, 0.0f);
    if (hitA != NULL || hitB != NULL) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(cliffCheck) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    f32 hitX, hitY, hitZ;
    f32 distance;
    f32 hitNX, hitNY, hitNZ;
    f32 xoff;
    HitEntry* hitA;
    HitEntry* hitB;

    if (npcTransRytoFaceDir(npc) == 90.0f) {
        xoff = 1.0f;
    } else {
        xoff = -1.0f;
    }
    distance = 5.0f;
    hitA = hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, npc->position.x,
                          npc->position.y + 1.0f, npc->position.z, 0.0f, -1.0f, 0.0f);
    distance = 5.0f;
    hitB = hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, npc->position.x,
                          npc->position.y + 1.0f, npc->position.z, xoff, -1.0f, 0.0f);
    if (hitA != NULL && hitB != NULL) {
        evtSetValue(event, args[0], 0);
    } else if (hitA != NULL && hitB == NULL) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 2);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(pressCheck) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    f32 hitX, hitY, hitZ;
    f32 distance;
    f32 hitNX, hitNY, hitNZ;

    if (kpaGetLevel() == 0) {
        evtSetValue(event, args[0], 0);
        return EVT_RETURN_DONE;
    }

    distance = 5.0f;
    if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, npc->position.x,
                       npc->position.y + 1.0f, npc->position.z, 0.0f, -1.0f, 0.0f)) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(kamec_fire_magic) {
    s32* args = event->args;
    s32 a0 = evtGetValue(event, args[0]);
    s32 a1 = evtGetValue(event, args[1]);
    s32 a2 = evtGetValue(event, args[2]);
    s32 a3 = evtGetValue(event, args[3]);
    s32 a4 = evtGetValue(event, args[4]);
    s32 a5 = evtGetValue(event, args[5]);
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    EffectEntry* eff;

    eff = effMagic1N64Entry(0, 0x3C, (f32)a0, (f32)a1, (f32)a2, (f32)a3, (f32)a4, (f32)a5);
    effSetName(eff, npc->description);
    evtSetValue(event, args[6], (s32)eff);
    return EVT_RETURN_DONE;
}

// NONMATCHING (82.82%): zero-Vec aggregate init optimized away (direct field copies), rodata pool base offsets differ.
USER_FUNC(zakoEntryFall) {
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    char names[3][16];
    s32 i;

    if (strcmp(npc->description,
               "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x92\x6e\x93\x5f\x82\x50") == 0) {
        strcpy(names[0], "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x82\x50");
        strcpy(names[1], "\x82\xc8\x82\xb5");
        strcpy(names[2], "\x82\xc8\x82\xb5");
    }
    if (strcmp(npc->description,
               "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x92\x6e\x93\x5f\x82\x51\x81\x51\x82\x50") == 0) {
        strcpy(names[0], "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x82\x50");
        strcpy(names[1], "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x82\x51");
        strcpy(names[2], "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x82\x52");
    }
    if (strcmp(npc->description,
               "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x92\x6e\x93\x5f\x82\x51\x81\x51\x82\x51") == 0) {
        strcpy(names[0], "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x82\x53");
        strcpy(names[1], "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x82\x54");
        strcpy(names[2], "\x8c\x52\x92\x63\x83\x55\x83\x52\x8d\x7e\x89\xba\x82\x55");
    }

    for (i = 0; i < 3; i++) {
        NpcEntry* target = npcNameToPtr_NoAssert(names[i]);

        if (target != NULL && target->position.y <= -75.0f) {
            Vec pos = { 0.0f, 0.0f, 0.0f };

            target->flags |= 0x20410;
            target->flags &= ~0x20;
            target->jumpFlags |= 0x1002;
            target->jumpFlags &= 0xEF8FFFFB;

            pos.x = npc->position.x;
            pos.y = npc->position.y;
            pos.z = npc->position.z;
            target->position = pos;
            target->rotation.x = 0.0f;
            target->rotation.y = 0.0f;
            target->rotation.z = 0.0f;
            target->unitWork[0] = (void*)2;

            if (target->moveEvent != NULL) {
                if (target->regularEvtId != 0) {
                    evtDeleteID(target->regularEvtId);
                }
                {
                    EventEntry* e = evtEntry(target->moveEvent, 0, 0x20);
                    e->wNpcEventType = 1;
                    e->ownerNPC = target;
                    target->regularEvtId = e->eventId;
                }
            }
            return EVT_RETURN_DONE;
        }
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (85.70%): frame 0x80 vs 0x90 and zero-Vec/rodata pool offsets; no instruction-logic difference.
USER_FUNC(zakoEntryDokan) {
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    char mobName[16];
    char names[3][16];
    const char* hit;
    s32 i;

    if (strcmp(npc->description,
               "\x8c\x52\x92\x63\x83\x55\x83\x52\x82\xaa\x82\xc5\x82\xe9\x93\x79\x8a\xc7\x82\x50") == 0) {
        strcpy(names[0], "\x8c\x52\x92\x63\x83\x55\x83\x52\x93\x79\x8a\xc7\x82\x50");
        strcpy(names[1], "\x8c\x52\x92\x63\x83\x55\x83\x52\x93\x79\x8a\xc7\x82\x51");
        strcpy(names[2], "\x8c\x52\x92\x63\x83\x55\x83\x52\x93\x79\x8a\xc7\x82\x52");
        strcpy(mobName, "MOBJ_mobj9");
    }
    if (strcmp(npc->description,
               "\x8c\x52\x92\x63\x83\x55\x83\x52\x82\xaa\x82\xc5\x82\xe9\x93\x79\x8a\xc7\x82\x51") == 0) {
        strcpy(names[0], "\x8c\x52\x92\x63\x83\x55\x83\x52\x93\x79\x8a\xc7\x82\x53");
        strcpy(names[1], "\x8c\x52\x92\x63\x83\x55\x83\x52\x93\x79\x8a\xc7\x82\x54");
        strcpy(names[2], "\x8c\x52\x92\x63\x83\x55\x83\x52\x93\x79\x8a\xc7\x82\x55");
        strcpy(mobName, "MOBJ_mobj14");
    }

    hit = hitGetName(marioGetPtr()->hitobjStandOn);
    if (hit != NULL && strcmp(hit, mobName) == 0) {
        return EVT_RETURN_DONE;
    }
    hit = hitGetName(marioGetPtr()->hitobjJumpFrom);
    if (hit != NULL && strcmp(hit, mobName) == 0) {
        return EVT_RETURN_DONE;
    }
    hit = hitGetName(*(void**)((u8*)marioGetPtr() + 0x1E0));
    if (hit != NULL && strcmp(hit, mobName) == 0) {
        return EVT_RETURN_DONE;
    }

    for (i = 0; i < 3; i++) {
        NpcEntry* target = npcNameToPtr(names[i]);

        if (target->position.y <= -75.0f) {
            Vec pos = { 0.0f, 0.0f, 0.0f };

            target->flags |= 0x20410;
            target->flags &= ~0x20;
            target->jumpFlags |= 0x1002;
            target->jumpFlags &= 0xEFAFFFFB;

            pos.x = npc->position.x;
            pos.y = npc->position.y - 35.0f;
            pos.z = npc->position.z;
            target->position = pos;
            target->rotation.x = 0.0f;
            target->rotation.y = 0.0f;
            target->rotation.z = 0.0f;
            target->unitWork[0] = (void*)1;

            if (target->moveEvent != NULL) {
                if (target->regularEvtId != 0) {
                    evtDeleteID(target->regularEvtId);
                }
                {
                    EventEntry* e = evtEntry(target->moveEvent, 0, 0x20);
                    e->wNpcEventType = 1;
                    e->ownerNPC = target;
                    target->regularEvtId = e->eventId;
                }
            }
            return EVT_RETURN_DONE;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(unk_800ece0c) {
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    s32* args = event->args;
    f32 f31 = evtGetFloat(event, args[0]);
    f32 f30 = evtGetFloat(event, args[1]);
    f32 f29 = evtGetFloat(event, args[2]);
    s32 v = evtGetValue(event, args[3]);
    EffectEntry* eff;

    if (isFirstCall) {
        eff = effFireEntry(5, 1000, f31, f30, f29, 1.0f);
        npc->unitWork[0] = eff;
        npc->unitWork[1] = (void*)v;
    }
    eff = npc->unitWork[0];
    npc->unitWork[1] = (void*)((s32)npc->unitWork[1] - 1);
    if ((s32)npc->unitWork[1] <= 0x1E) {
        f32 t = intplGetValue(1, 0x1E - (s32)npc->unitWork[1], 0x1E, 1.0f, 0.0f);
        *(f32*)((u8*)eff->userdata + 0x78) = t;
        *(f32*)((u8*)eff->userdata + 0x7C) = t;
        *(u8*)((u8*)eff->userdata + 0x10) =
            (u8)(s32)intplGetValue(0, 0x1E - (s32)npc->unitWork[1], 0x1E, 255.0f, 0.0f);
    }
    if ((s32)npc->unitWork[1] <= 0) {
        effDelete(eff);
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

// NONMATCHING (70.15%): float temp register allocation shifted (scale lands in f30 not f31) and stack Vec slots differ; logic correct.
USER_FUNC(chain_main) {
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    NpcEntry* ref;
    NpcEntry* cur;
    f32 f31;
    f32 f30;
    f32 f29;
    f32 f28;
    f32 f27;
    f32 f26;
    f32 f25;
    f32 f24;
    f32 f23;
    f32 dist;
    f32 t;
    s32 i;

    {
        NpcEntry* n = npc->slaves[3];
        f32 dy = npc->position.y - n->position.y;
        f32 dx = npc->position.x - n->position.x;
        f32 dz = npc->position.z - n->position.z;

        f31 = (f32)sqrt(dy * dy + dx * dx + dz * dz) * 0.25f;
    }
    if (f31 < 10.0f) {
        f31 = 10.0f;
    }
    f30 = 300.0f / (f31 * f31);

    {
        Vec pos = { 0.0f, 0.0f, 0.0f };
        NpcEntry* s = npc->slaves[0];

        f29 = s->position.y - f30;
        f28 = s->position.x;
        f27 = s->position.z;
        f26 = npc->position.x - f28;
        f24 = npc->position.z - f27;
        f25 = npc->position.y - f29;
        dist = (f32)sqrt(f25 * f25 + f26 * f26 + f24 * f24);
        t = 1.0f - f31 / dist;
        f29 = f25 * t + f29;
        f26 = f26 * t + f28;
        f24 = f24 * t + f27;
        if (f29 < npc->territoryBase.y) {
            f29 = npc->territoryBase.y;
        }
        pos.x = f26;
        pos.y = f29;
        pos.z = f24;
        s->position = pos;
    }

    for (i = 0; i < 2; i++) {
        Vec pos = { 0.0f, 0.0f, 0.0f };

        ref = npc->slaves[i];
        cur = npc->slaves[i + 1];
        f23 = cur->position.y - f30;
        f27 = cur->position.x;
        f29 = cur->position.z;
        f25 = ref->position.y - f23;
        f26 = ref->position.x - f27;
        f24 = ref->position.z - f29;
        dist = (f32)sqrt(f25 * f25 + f26 * f26 + f24 * f24);
        t = 1.0f - f31 / dist;
        f23 = f25 * t + f23;
        f26 = f26 * t + f27;
        f24 = f24 * t + f29;
        if (f23 < npc->territoryBase.y) {
            f23 = npc->territoryBase.y;
        }
        pos.x = f26;
        pos.y = f23;
        pos.z = f24;
        cur->position = pos;
    }

    for (i = 2; i >= 0; i--) {
        Vec pos = { 0.0f, 0.0f, 0.0f };

        cur = npc->slaves[i];
        ref = npc->slaves[i + 1];
        f23 = cur->position.y - f30;
        f27 = cur->position.x;
        f29 = cur->position.z;
        f25 = ref->position.y - f23;
        f26 = ref->position.x - f27;
        f24 = ref->position.z - f29;
        dist = (f32)sqrt(f25 * f25 + f26 * f26 + f24 * f24);
        t = 1.0f - f31 / dist;
        f23 = f25 * t + f23;
        f26 = f26 * t + f27;
        f24 = f24 * t + f29;
        if (f23 < npc->territoryBase.y) {
            f23 = npc->territoryBase.y;
        }
        pos.x = f26;
        pos.y = f23;
        pos.z = f24;
        cur->position = pos;
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (87.94%): original keeps a dead magnitude compare before angleABf and has different frame/reg allocation; logic correct.
USER_FUNC(_2d_dead_jump) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 f31;
    f32 f30;
    f32 f29;
    f32 f28;
    f32 f27;
    f32 f26;
    f32 f25;
    f32 f24;
    NpcEntry* npc;
    Vec diff;
    f32 dy;

    f31 = evtGetFloat(event, args[1]);
    f24 = evtGetFloat(event, args[2]);
    f25 = evtGetFloat(event, args[3]);
    f30 = (f32)evtGetValue(event, args[4]) / 1000.0f;
    f26 = evtGetFloat(event, args[5]);
    f29 = evtGetFloat(event, args[6]);
    f28 = (f32)evtGetValue(event, args[7]);
    npc = evtNpcNameToPtr(event, (const char*)name);
    f27 = f29;

    npc->jumpFlags |= 0x10;
    npc->jumpFlags |= 0x1000;

    if (isFirstCall) {
        f32 g;
        f32 h;
        f32 half;
        f32 f4;

        npc->flags |= 0x20000;
        npc->flags &= 0xBFFFFFEF;
        npc->field_188 = 0;
        npc->field_178 = 0;
        npc->field_108 = 0;
        npc->jumpTargetPos.x = f31;
        npc->jumpTargetPos.y = f24;
        npc->jumpTargetPos.z = f25;
        npc->jumpStartPos = npc->position;
        PSVECSubtract(&npc->jumpTargetPos, &npc->jumpStartPos, &diff);
        npc->moveDir = angleABf(0.0f, 0.0f, diff.x, diff.z);
        if (f30 > 0.0f) {
            npc->moveSpeed = (f32)(fabs(f31 - npc->position.x) / f30);
        } else {
            npc->moveSpeed = f26;
        }
        npc->moveTime = 0.0f;
        npc->moveOffset.y = 0.0f;
        npc->jumpVelY = 0.0f;
        npc->jumpYScale = 1.0f;
        dy = npc->jumpTargetPos.y - npc->jumpStartPos.y;
        if (f29 == 0.0f) {
            if (dy >= 0.0f) {
                return EVT_RETURN_YIELD;
            }
            return EVT_RETURN_BLOCK;
        }
        if (dy > 0.0f) {
            f27 = f27 + dy;
        }
        npc->jumpVelY = 0.0f;
        if (f30 == 0.0f) {
            npc->jumpVelY = f29;
        } else {
            g = npc->jumpGravity;
            h = -980.0f;
            half = 0.5f;
            f4 = half * f30;
            npc->jumpVelY = -(f30 * ((h * g) * f30)) / f30;
            {
                f32 y = f4 * ((h * g) * f4);
                y = npc->jumpVelY * f4 + y;
                if (y != 0.0f) {
                    npc->jumpYScale = f27 / y;
                }
            }
        }
        return EVT_RETURN_BLOCK;
    } else {
        f32 s;
        f32 c;

        s = (f32)sin(6.2831855f * npc->moveDir / 360.0f);
        npc->moveOffset.x = npc->moveDt * npc->moveSpeed * s + npc->moveOffset.x;
        c = (f32)cos(6.2831855f * npc->moveDir / 360.0f);
        npc->moveOffset.z = npc->moveDt * npc->moveSpeed * -c + npc->moveOffset.z;
        if (npc->position.y >= f28) {
            return EVT_RETURN_BLOCK;
        }
        npc->moveSpeed = 0.0f;
        npc->jumpVelY = 0.0f;
        npc->jumpYScale = 1.0f;
        npc->jumpGravity = 1.0f;
        return EVT_RETURN_YIELD;
    }
}

USER_FUNC(mahoon_get_groupname) {
    s32* args = event->args;
    NpcEntry* npc = evtNpcNameToPtr(event, "me");
    evtSetValue(event, args[0], (s32)npc->tribe->modelName);
    return EVT_RETURN_DONE;
}
