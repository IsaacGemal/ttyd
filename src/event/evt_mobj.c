#include "mario/mario_motion.h"
#include "math_ext.h"
#include "party.h"
#include "mario/mario_motion.h"
#include "pmario_sound.h"
#include "party.h"
#include "driver/mobjdrv.h"
#include "driver/animdrv.h"
#include "driver/hitdrv.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "driver/dispdrv.h"
#include "driver/itemdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "memory.h"
#include <string.h>
#include "koopa.h"
#include "driver/npcdrv.h"
#include "system.h"
#include "mario/mario_party.h"
#include "evt/evt_mobj.h"
#include "kpa_act.h"
#include "koopa_motion.h"
#include "eff/eff_fire.h"
#include "eff/eff_small_star.h"
#include "eff/eff_mobj_broken.h"
#include "driver/offscreendrv.h"
#include "driver/imgdrv.h"
#include "evt/evt_img.h"
#include <dolphin/mtx.h>

// Prototypes for functions in other, not-yet-decompiled units (koopa / hit / etc.)

// Read-only constants defined in this unit's .rodata (to be defined locally later)
extern f32 float_0_8041a150;
extern f32 float_12p5_8041a178;
extern f32 float_1_8041a160;
extern f32 float_0p5_8041a15c;
extern f32 float_neg1_8041a214;
extern f32 float_10_8041a1a4;
extern f32 float_0p8_8041a1b0;
extern f32 float_0p125_8041a1b4;
extern f32 float_0p99_8041a1b8;
extern const f32 float_1p4_8041a208;
extern const f32 float_1p2_8041a20c;
extern f32 float_5_8041a210;
extern const char str_802C46E4[];
extern Vec vec3_802c3dc8[];
extern Vec vec3_802c3dd4[];
extern Vec vec3_802c40ac[];
extern Vec vec3_802c40b8[];
extern Vec vec3_802c40c4[];
extern Vec vec3_802c4050[];
extern Vec vec3_802c405c[];
extern const char str_802C41D0[];
extern const char str_802C42E8[];
extern const char str_802C42FC[];
extern u32 dat_8041a134;
extern u32 dat_8041a130;
extern f32 float_0p45_8041a1c4;
extern f32 float_0p7_8041a1c8;
extern f32 float_500_8041a1cc;
extern f32 float_120_8041a1d0;
extern f32 float_25_8041a140;
extern f32 float_7p5_8041a1bc;
extern f32 float_40_8041a1ec;
extern f32 float_32767_8041a1e4;
extern s32 kpa_powerup_evt[];

extern const char str_802C44EC[];
extern const char str_8041A12C[4];
extern s32 evt_lock_sound[];
extern f32 float_1024_8041a1a0;
extern const f32 float_neg0p8_8041a220;
extern const f32 float_4_8041a224;

// Prototypes missing from manager/fontmgr.h


// Prototypes missing from driver/mobjdrv.h (to move to the header later)

// Prototypes missing from mario/party/sound headers (to move later)

// ---------------------------------------------------------------------------
// Pole score work (sbss `psw`).  The first bytes are the ASCII score string,
// then three floats holding the on-screen position.
// ---------------------------------------------------------------------------
typedef struct PoleWork {
    char str[4];    // 0x0
    u8 unk4;        // 0x4, string NULL terminator
    u8 unk5[3];     // 0x5
    f32 x;          // 0x8
    f32 y;          // 0xC
    f32 z;          // 0x10
} PoleWork;

PoleWork* psw;

// Anchor for the .data addresses below (kpa_pole and friends live at fixed offsets past it).
#ifdef MUST_MATCH
__declspec(section ".data") static f32 negone_one_374[2] = { -1.0f, 1.0f };
#endif

s32 mobj_koopa_pole_evt(MapObjectEntry* obj) {
#ifdef MUST_MATCH
    u8* base = (u8*)negone_one_374;
#endif
    MarioWork* mario = marioGetPtr();
    NpcWork* wp;
    NpcEntry* entry;
    s32 next = 0;
    s32 score;
    s32 i;

    switch (obj->unk1DC) {
        case 0:
            if (!(obj->flags & 8)) {
                break;
            }
            if (mario->characterId != 2) {
                obj->flags &= ~8;
                break;
            }
            switch (kpaGetLevel()) {
                case 0:
                case 1:
                    if (mario->position.y < float_25_8041a140) {
                        obj->flags &= ~8;
                    } else {
                        next = 0xa;
                    }
                    break;
                case 2:
                case 3:
                    next = 0x32;
                    break;
            }
            if (next == 0) {
                break;
            }
            for (i = 0; i < npcGetWorkPtr()->total; i++) {
                entry = &npcGetWorkPtr()->entries[i];
                if (!(entry->flags & 1)) {
                    continue;
                }
                if (entry->jumpFlags & 4) {
                    continue;
                }
                if (!(entry->flags & 0x08000000)) {
                    continue;
                }
                if (!(entry->flags & 8)) {
                    continue;
                }
                npcGroupDead(entry, 0x20);
            }
            marioKeyOff();
            evtSetValue(NULL, obj->unk1E4, 1);
            *(s16*)mario->unk78 = 0;
            obj->unk1DC = next;
            break;
        case 0xa:
            mario = marioGetPtr();
            score = 0;
            if (kpaGetLevel() == 3) {
                kpaAddScore(0x7d0);
            } else if (kpaGetLevel() >= 2) {
                kpaAddScore(0x3e8);
            } else {
                if (mario->position.y >= float_170_8041a144) {
                    score = 1000;
                } else if (mario->position.y >= float_100_8041a148) {
                    score = 500;
                } else if (mario->position.y >= float_50_8041a14c) {
                    score = 200;
                } else {
                    score = 100;
                }
                kpaAddScore(score);
            }
#ifdef MUST_MATCH
            mobjRunEvent(obj, (s32*)(base + 0x230))->lwData[0] = score;
#else
            mobjRunEvent(obj, kpa_pole)->lwData[0] = score;
#endif
            obj->unk1DC++;
            break;
        case 0xb:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            obj->unk1DC = 0x5a;
            break;
        case 0x32:
            mario = marioGetPtr();
            if (kpaGetLevel() == 3) {
                kpaAddScore(0x7d0);
            } else if (kpaGetLevel() >= 2) {
                kpaAddScore(0x3e8);
            } else {
                if (mario->position.y >= float_170_8041a144) {
                    score = 1000;
                } else if (mario->position.y >= float_100_8041a148) {
                    score = 500;
                } else if (mario->position.y >= float_50_8041a14c) {
                    score = 200;
                } else {
                    score = 100;
                }
                kpaAddScore(score);
            }
#ifdef MUST_MATCH
            mobjRunEvent(obj, (s32*)(base + 0x690));
#else
            mobjRunEvent(obj, kpa_pole_move);
#endif
            obj->unk1DC++;
            break;
        case 0x33:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            obj->unk1DC = 0x5a;
            break;
        case 0x5a:
#ifdef MUST_MATCH
            mobjRunEvent(obj, (s32*)(base + 0x8ec));
#else
            mobjRunEvent(obj, kpa_move_to_toride);
#endif
            obj->unk1DC++;
            break;
        case 0x5b:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            obj->eventId = 0;
            obj->unk1DC++;
            break;
        case 0x5c:
            mobjRunEvent(obj, obj->unk1D8);
            obj->unk1DC++;
            break;
        case 0x5d:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            obj->eventId = 0;
            obj->unk1DC++;
            // fallthrough
        case 0x5e:
            mobjRunEvent(obj, obj->unk1D4);
            obj->unk1DC++;
            break;
#ifdef MUST_MATCH
        case 0x5f:
#endif
        default:
            break;
    }
    return 0;
}

// NONMATCHING (96.88%): MWCC CSEs `obj + 0x1B8` into r25 (r25 is dead there) and reuses it for
// strcpy/strcat/mobjEntry; retail recomputes the addi each time. Everything else matches.
s32 evt_mobj_koopa_pole(EventEntry* event, BOOL isFirstCall) {
    u8* base = (u8*)vec3_802c3dc8;
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[4]);
    s32 unk1D8 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    BOOL inBattle = gp->inBattle != 0;
    MapObjectEntry* obj;
    MapObjectEntry* child;
    Vec pos;
    Vec pos2;

    if (!animGroupBaseAsync((const char*)(base + 0x3B8), inBattle, NULL)) {
        return 0;
    }
    if (!animGroupBaseAsync((const char*)(base + 0x3C8), inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, (const char*)(base + 0x3B8));
    obj = mobjNameToPtr(name);
    obj->flags |= 0x84;
    pos = *(Vec*)(base + 0x3A0);
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->unk1D0 = mobj_koopa_pole_evt;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = (s32*)unk1D8;
    obj->flags |= 0x1c280000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    strcpy((char*)((u8*)obj + 0x1B8), name);
    strcat((char*)((u8*)obj + 0x1B8), "_h");
    mobjEntry((char*)((u8*)obj + 0x1B8), (const char*)(base + 0x3C8));
    child = mobjNameToPtr((char*)((u8*)obj + 0x1B8));
    pos2 = *(Vec*)(base + 0x3AC);
    pos2.x = x;
    pos2.y = 180.0f + y;
    pos2.z = z;
    child->position = pos2;
    child->unk1D0 = NULL;
    child->unk1D4 = NULL;
    child->unk1D8 = NULL;
    child->unk1E4 = 0;
    child->unk1DC = 0;
    strcpy((char*)((u8*)child + 0x1B8), name);
    if (child->hitObj[0].hitObject != NULL) {
        child->hitObj[0].hitObject->flags |= 1;
    }
    return 2;
}

s32 kpa_clear_jump_data(EventEntry* event, BOOL isFirstCall) {
    MarioWork* mario = marioGetPtr();
    mario->airTimer = 0;
    return 2;
}

s32 kpa_disp_pole_init(EventEntry* event, BOOL isFirstCall) {
    f32 zero;

    psw = _mapAlloc(0x14);
    zero = float_0_8041a150;
    psw->str[0] = 0;
    psw->z = zero;
    psw->y = zero;
    psw->x = zero;
    return 2;
}

s32 kpa_get_width(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;

    evtSetValue(event, args[0], (s32)marioGetPtr()->wPlayerCollisionBox.x);
    return 2;
}

static s32 kpa_get_level(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;

    evtSetValue(event, args[0], kpaGetLevel());
    return 2;
}

s32 kpa_get_hata_name(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;

    if (event->thisMapObj != NULL) {
        evtSetValue(event, args[0], (s32)((u8*)event->thisMapObj + 0x1B8));
    } else {
        evtSetValue(event, args[0], 0);
    }
    return 2;
}

s32 kpa_get_pole_name(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;

    if (event->thisMapObj != NULL) {
        evtSetValue(event, args[0], (s32)event->thisMapObj->name);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return 2;
}

s32 kpa_disp_pole_score_main(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    f32 f29;
    f32 f30;
    f32 f31;
    f32 f1;

    f29 = evtGetFloat(event, args[0]);
    f30 = evtGetFloat(event, args[1]);
    f31 = evtGetFloat(event, args[2]);
    f1 = evtGetFloat(event, args[3]);
    if (isFirstCall) {
        psw->x = f29;
        psw->y = f30;
        psw->z = f31;
    }
    if (psw->y < 230.0f) {
        psw->y = psw->y + f1;
    } else {
        psw->y = 230.0f;
    }
    dispEntry(4, 0.0f, 2, kpa_disp_pole_score, NULL);
    return 0;
}

void kpa_disp_pole_score(CameraId camera, void* param) {
    Mtx trans;
    Mtx scale;
    GXColor color;

    PSMTXTrans(trans, psw->x, psw->y, 300.0f);
    PSMTXScale(scale, 0.5f, 0.5f, 0.5f);
    PSMTXConcat(trans, scale, trans);
    FontDrawStart();
    *(u32*)&color = dat_8041a134;
    FontDrawColor(&color);
    FontDrawStringMtx(trans, psw->str);
}

// NONMATCHING (94.37%): register allocation of the 4 extracted digits differs.
s32 kpa_score_to_str(EventEntry* event, BOOL isFirstCall) {
    s32 value = evtGetValue(event, event->args[0]);
    s32 i = 0;
    s32 thousands = value / 1000;
    s32 hundreds;
    s32 tens;
    s32 ones;

    value %= 1000;
    hundreds = value / 100;
    value %= 100;
    tens = value / 10;
    ones = value % 10;

    if (i != 0 || thousands != 0) {
        psw->str[i] = thousands + '0';
        i = 1;
    }
    if (i != 0 || hundreds != 0) {
        psw->str[i++] = hundreds + '0';
    }
    if (i != 0 || tens != 0) {
        psw->str[i++] = tens + '0';
    }
    if (i != 0 || ones != 0) {
        psw->str[i] = ones + '0';
    }
    psw->unk4 = 0;
    return 2;
}

s32 mobj_koopa_stone_blk_evt(MapObjectEntry* obj) {
    return 0;
}

s32 evt_mobj_koopa_stone_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[4]);
    s32 unk1E4 = args[5];
    const char* asyncName = (const char*)evtGetValue(event, args[6]);
    MapObjectEntry* obj;
    Vec pos;

    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    obj->flags |= 4;
    pos = vec3_802c40ac[0];
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->unk1D0 = mobj_koopa_stone_blk_evt;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = 0;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0x63;
    return 2;
}

s32 mobj_float_blk(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0x63:
            if (*(s8*)&obj->unk1A0[0x18] == 0) {
                f32 hitNZ, hitNY, hitNX, distance, hitZ, hitY, hitX;

                distance = float_1_8041a160 + (f32)__fabs(obj->unk5C.y);
                if ((s32)hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                                        obj->position.x, float_0p5_8041a15c + obj->position.y, obj->position.z,
                                        float_0_8041a150, float_neg1_8041a214, float_0_8041a150) != 0) {
                    if (strcmp(animPoseGetCurrentAnim(obj->poseId)->name, "S_1") != 0) {
                        animPoseSetAnim(obj->poseId, "S_1", 1);
                        obj->flags |= 2;
                    }
                } else {
                    if (strcmp(animPoseGetCurrentAnim(obj->poseId)->name, "A_1") != 0) {
                        animPoseSetAnim(obj->poseId, "A_1", 1);
                        obj->flags &= ~2;
                    }
                }
            } else {
                AnimTableEntry* anim = animPoseGetCurrentAnim(mobjNameToPtr((const char*)&obj->unk1A0[0x18])->poseId);

                if ((s8)anim->name[0] == 'A' && (s8)anim->name[2] >= '1' && (s8)anim->name[2] <= '3') {
                    animPoseSetAnim(obj->poseId, "A_1", 0);
                } else {
                    animPoseSetAnim(obj->poseId, "S_1", 1);
                }
            }
            break;
    }
    return 0;
}

// NONMATCHING (80.09%): 8 instructions differ, 17 missing, 14 extra, plus register differences.
s32 evt_mobj_float_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 type = args[4];
    s32 sub = args[5];
    s32 unk1E4 = args[7];
    const char* asyncName;
    MapObjectEntry* obj;
    BOOL inBattle = gp->inBattle != 0;

    evtGetValue(event, args[6]);
    switch (type) {
        case 0:
        default:
            switch (sub) {
                case 0:
                    asyncName = "MOBJ_green_FlowBlock";
                    break;
                case 1:
                    asyncName = "MOBJ_green_big_FlowBlock";
                    break;
                default:
                    asyncName = "MOBJ_green_big02_FlowBlock";
                    break;
            }
            break;
        case 1:
            switch (sub) {
                case 0:
                    asyncName = "MOBJ_purple_FlowBlock";
                    break;
                case 1:
                    asyncName = "MOBJ_purple_big_FlowBlock";
                    break;
                default:
                    asyncName = "MOBJ_purple_big02_FlowBlock";
                    break;
            }
            break;
        case 2:
            switch (sub) {
                case 0:
                    asyncName = "MOBJ_orange_FlowBlock";
                    break;
                case 1:
                    asyncName = "MOBJ_orange_big_FlowBlock";
                    break;
                default:
                    asyncName = "MOBJ_orange_big02_FlowBlock";
                    break;
            }
            break;
        case 3:
            asyncName = "MOBJ_red_big02_FlowBlock";
            break;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_float_blk;
    obj->flags |= 4;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0x63;
    mobjCalcMtx(obj);
    {
        f32 hitNZ, hitNY, hitNX, distance, hitZ, hitY, hitX;

        distance = float_1_8041a160 + (f32)__fabs(obj->unk5C.y);
        if ((s32)hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                                obj->position.x, float_0p5_8041a15c + obj->position.y, obj->position.z,
                                float_0_8041a150, float_neg1_8041a214, float_0_8041a150) != 0) {
            animPoseSetAnim(obj->poseId, "S_1", 1);
            obj->flags |= 2;
        } else {
            animPoseSetAnim(obj->poseId, "A_1", 1);
        }
    }
    return 2;
}

// NONMATCHING (87.88%): 4 missing, 6 extra, plus register differences.
s32 fire_func(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    f32 f31 = evtGetFloat(event, args[0]);
    f32 f30 = evtGetFloat(event, args[1]);
    f32 f29 = evtGetFloat(event, args[2]);
    s32 v = evtGetValue(event, args[3]);
    EffectEntry* eff;
    f32 unk7C;
    f32 unk80;
    f32 n5;
    f32 n6;

    if (isFirstCall) {
        event->userdata[0] =
            (s32)effFireEntry(5, v, f31, f30, f29, float_0p8_8041a1b0 + (f32)irand(10) / float_10_8041a1a4);
    }
    unk7C = event->userdataf[1];
    unk80 = event->userdataf[2];
    eff = (EffectEntry*)event->userdata[0];
    {
        f32 one = float_1_8041a160;
        n5 = (one - unk7C) * float_0p125_8041a1b4 + unk7C;
        n6 = (one - unk80) * float_0p125_8041a1b4 + unk80;
    }
    *(f32*)((u8*)eff->userdata + 0x78) = n5;
    *(f32*)((u8*)eff->userdata + 0x7C) = n6;
    if (n5 >= float_0p99_8041a1b8) {
        *(f32*)((u8*)eff->userdata + 0x78) = float_1_8041a160;
        *(f32*)((u8*)eff->userdata + 0x7C) = float_1_8041a160;
        return 2;
    }
    event->userdataf[1] = n5;
    event->userdataf[2] = n6;
    return 0;
}

s32 mobj_koopa_sango_evt(MapObjectEntry* obj) {
    s32 sp10 = 0;
    EventEntry* event;

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                switch (kpaGetLevel()) {
                    case 0:
                    case 1:
                    case 2:
                        if (obj->unk1E0 == 0x20000000) {
                            sp10 = 1;
                        }
                        break;
                    case 3:
                        sp10 = 1;
                        break;
                }
                if (sp10 == 0) {
                    obj->flags &= ~0x8;
                    break;
                }
                mobjRunEvent(obj, obj->unk1D4);
                evtSetValue(NULL, obj->unk1E4, 1);
                if (obj->unk1E0 == 0x04000000) {
                    npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                }
                if (obj->hitObj[0].hitObject != NULL) {
                    obj->hitObj[0].hitObject->flags |= 1;
                }
                obj->flags |= 2;
                kpaAddScore(kpaMutekiCheck() ? 0x1f4 : 0x190);
                obj->unk1DC = 0xa;
            }
            break;
        case 0xa:
            if (obj->unk1E0 == 0x20000000) {
                psndSFXOn_3D((const char*)0xb67, &obj->position);
            }
            event = evtEntry(evt_fire, 0, 0);
            event->lwData[0] = (s32)(1024.0f * obj->position.x) + 0xF24A7A80;
            event->lwData[1] = (s32)(1024.0f * obj->position.y) + 0xF24A7A80;
            event->lwData[2] = (s32)(1024.0f * (obj->position.z + 10.0f)) + 0xF24A7A80;
            event->lwData[3] = 0x5a;
            obj->unk1DC++;
            break;
        case 0x14:
            event = evtEntry(evt_fire, 0, 0);
            event->lwData[0] = (s32)(1024.0f * (obj->position.x + 10.0f)) + 0xF24A7A80;
            event->lwData[1] = (s32)(1024.0f * (obj->position.y + 30.0f)) + 0xF24A7A80;
            event->lwData[2] = (s32)(1024.0f * (obj->position.z + 10.0f)) + 0xF24A7A80;
            event->lwData[3] = 0x50;
            obj->unk1DC++;
            break;
        case 0x1e:
            event = evtEntry(evt_fire, 0, 0);
            event->lwData[0] = (s32)(1024.0f * (obj->position.x - 10.0f)) + 0xF24A7A80;
            event->lwData[1] = (s32)(1024.0f * (obj->position.y + 60.0f)) + 0xF24A7A80;
            event->lwData[2] = (s32)(1024.0f * (obj->position.z + 10.0f)) + 0xF24A7A80;
            event->lwData[3] = 0x46;
            obj->unk1DC++;
            break;
        case 0x62:
            effMObjBrokenEntry(7, obj->position.x, obj->position.y, obj->position.z + 10.0f);
            obj->unk1DC++;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
        default:
            obj->unk1DC++;
            break;
    }
    return 0;
}

s32 kpa_powerup(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    Vec pos;

    pos.x = evtGetFloat(event, args[0]);
    pos.y = evtGetFloat(event, args[1]);
    pos.z = evtGetFloat(event, args[2]);
    kpaAddScorePos(500, &pos);
    if (kpaGetLevel() == 3) {
        kpaAddScore(1000);
    }
    kpaPowUp();
    return 2;
}

// ---------------------------------------------------------------------------
// Generic MOBJ event commands
// ---------------------------------------------------------------------------

s32 evt_mobj_hit_onoff(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    s32 on;
    s32 i;

    name = (const char*)evtGetValue(event, args[0]);
    on = evtGetValue(event, args[1]);
    obj = mobjNameToPtr(name);

    if (on != 0) {
        for (i = 0; i < 2; i++) {
            if (obj->hitObj[i].name != NULL) {
                hitGrpFlagOff(obj->hitObj[i].name, 1);
            }
        }
    } else {
        for (i = 0; i < 2; i++) {
            if (obj->hitObj[i].name != NULL) {
                hitGrpFlagOn(obj->hitObj[i].name, 1);
            }
        }
    }
    return 2;
}

s32 evt_mobj_hitevt_onoff(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    s32 on;

    name = (const char*)evtGetValue(event, args[0]);
    on = evtGetValue(event, args[1]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }

    if (on != 0) {
        obj->flags &= ~0x10;
    } else {
        obj->flags |= 0x10;
    }
    return 2;
}

s32 evt_mobj_set_camid(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    u8 camId;

    name = (const char*)evtGetValue(event, args[0]);
    camId = (u8)evtGetValue(event, args[1]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    obj->camId = camId;
    return 2;
}

s32 evt_mobj_wait_animation_end(EventEntry* event, BOOL isFirstCall) {
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, event->args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    if (animPoseGetLoopTimes(obj->poseId) > 1.0f) {
        return 2;
    }
    return 0;
}

s32 evt_mobj_set_anim(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    const char* animName;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    animName = (const char*)evtGetValue(event, args[1]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    animPoseSetAnim(obj->poseId, animName, 1);
    return 2;
}

s32 evt_mobj_set_gravity_bound(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 f1;
    f32 f2;

    name = (const char*)evtGetValue(event, args[0]);
    f1 = evtGetFloat(event, args[1]);
    f2 = evtGetFloat(event, args[2]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    obj->unk68 = f1;
    obj->unk6C = f2;
    return 2;
}

s32 evt_mobj_exec_cancel(EventEntry* event, BOOL isFirstCall) {
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, event->args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    obj->flags |= 0x1000;
    return 2;
}

s32 evt_mobj_set_z_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    Vec pos;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    pos = obj->position;
    pos.z = evtGetFloat(event, args[1]);
    obj->position = pos;
    obj->flags |= 4;
    return 2;
}

s32 evt_mobj_set_y_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    Vec pos;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    pos = obj->position;
    pos.y = evtGetFloat(event, args[1]);
    obj->position = pos;
    obj->flags |= 4;
    return 2;
}

s32 evt_mobj_set_x_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    Vec pos;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    pos = obj->position;
    pos.x = evtGetFloat(event, args[1]);
    obj->position = pos;
    obj->flags |= 4;
    return 2;
}

s32 evt_mobj_set_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 x;
    f32 y;
    f32 z;
    Vec pos;

    name = (const char*)evtGetValue(event, args[0]);
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    pos = vec3_802c3dd4[0];
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->flags |= 4;
    return 2;
}

s32 evt_mobj_get_z_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    evtSetFloat(event, args[1], obj->position.z);
    return 2;
}

s32 evt_mobj_get_y_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    evtSetFloat(event, args[1], obj->position.y);
    return 2;
}

s32 evt_mobj_get_x_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    evtSetFloat(event, args[1], obj->position.x);
    return 2;
}

s32 evt_mobj_get_position(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") != 0) {
        obj = mobjNameToPtr(name);
    } else {
        obj = event->thisMapObj;
    }
    evtSetFloat(event, args[1], obj->position.x);
    evtSetFloat(event, args[2], obj->position.y);
    evtSetFloat(event, args[3], obj->position.z);
    return 2;
}

s32 evt_mobj_set_scale(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 x;
    f32 y;
    f32 z;
    Vec scale;

    name = (const char*)evtGetValue(event, args[0]);
    if (strcmp(name, "me") == 0) {
        obj = event->thisMapObj;
    } else {
        obj = mobjNameToPtr(name);
    }
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    scale = vec3_802c3dc8[0];
    scale.x = x;
    scale.y = y;
    scale.z = z;
    obj->scale = scale;
    obj->flags |= 4;
    return 2;
}

s32 evt_mobj_get_kindname(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;

    name = (const char*)evtGetValue(event, args[0]);
    obj = mobjNameToPtr(name);
    evtSetValue(event, args[1], (s32)obj->animName);
    return 2;
}

s32 evt_mobj_flag_onoff(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 on;
    const char* name;
    s32 flag;
    MapObjectEntry* obj;

    on = evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    flag = args[2];
    obj = mobjNameToPtr(name);

    if (on != 0) {
        obj->flags |= flag;
    } else {
        obj->flags &= ~flag;
    }
    return 2;
}

s32 evt_mobj_check(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;

    name = (const char*)evtGetValue(event, args[0]);
    evtSetValue(event, args[1], (s32)mobjNameToPtrNoAssert(name));
    return 2;
}

s32 evt_mobj_delete(EventEntry* event, BOOL isFirstCall) {
    mobjDelete((const char*)evtGetValue(event, event->args[0]));
    return 2;
}

s32 evt_mobj_itembox(EventEntry* event, BOOL isFirstCall) {
    u8* base = (u8*)vec3_802c3dc8;
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 type = evtGetValue(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1D8 = evtGetValue(event, args[6]);
    s32 unk1E4 = args[7];
    BOOL inBattle = gp->inBattle != 0;
    const char* modelName;
    MapObjectEntry* obj;

    switch (type) {
        case 0:
            modelName = (const char*)(base + 0x6A0);
            break;
        case 1:
            modelName = (const char*)(base + 0x6B4);
            break;
        case 2:
            modelName = (const char*)(base + 0x6C8);
            break;
        case 3:
            modelName = (const char*)(base + 0x6E0);
            break;
        case 4:
        default:
            modelName = (const char*)(base + 0x6F8);
            break;
    }
    if (!animGroupBaseAsync(modelName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, modelName);
    obj = mobjNameToPtr(name);
    mobjHitEntry(obj, 2);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_itembox;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = (s32*)unk1D8;
    obj->flags |= 0x64;
    obj->flags |= 0x40000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    mobjCalcMtx(obj);
    if (evtGetValue(event, unk1E4) != 0) {
        AnimationPoseData* animData = animPoseGetAnimBaseDataPtr(obj->poseId);
        Vec maxTmp;
        Vec minTmp;
        Vec bboxMax;
        Vec bboxMin;

        minTmp = *(Vec*)(base + 0x210);
        minTmp.x = animData->bboxMin.x;
        minTmp.y = animData->bboxMin.y;
        minTmp.z = animData->bboxMin.z;
        bboxMin = minTmp;
        maxTmp = *(Vec*)(base + 0x21C);
        maxTmp.x = animData->bboxMax.x;
        maxTmp.y = animData->bboxMax.y;
        maxTmp.z = animData->bboxMax.z;
        bboxMax = maxTmp;
        bboxMin.z = bboxMin.z - *(f64*)(base + 0x710) * __fabsf(bboxMax.y - bboxMin.y);
        PSVECAdd(&bboxMax, &bboxMin, &obj->translation);
        PSVECScale(&obj->translation, &obj->translation, float_0p5_8041a15c);
        PSVECScale(&obj->translation, &obj->translation, float_10_8041a1a4);
        obj->translation.y = float_0_8041a150;
        obj->scale2.y = obj->scale2.y * float_1p4_8041a208;
        obj->scale2.z = obj->scale2.z * float_1p2_8041a20c;
        mobjCalcMtx2(obj);
        obj->unk1DC = 0x63;
    }
    {
        f32 hitX;
        f32 hitY;
        f32 hitZ;
        f32 distance = float_5_8041a210 + (f32)__fabs(obj->unk5C.y);
        f32 hitNX;
        f32 hitNY;
        f32 hitNZ;

        if (hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                           obj->position.x, float_1_8041a160 + obj->position.y, obj->position.z,
                           float_0_8041a150, float_neg1_8041a214, float_0_8041a150) != NULL) {
            obj->flags |= 2;
            obj->flags &= ~0x40;
        }
    }
    return 2;
}

// NONMATCHING (97.14%): retail schedules the `unk1E4 = args[5]` load two instructions later (after the
// string address is materialised); otherwise byte-identical.
s32 evt_mobj_signboard(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 f1;
    f32 f2;
    f32 f3;
    s32 unk1D4;
    s32 unk1E4;
    const char* asyncName;

    name = (const char*)evtGetValue(event, args[0]);
    f1 = evtGetFloat(event, args[1]);
    f2 = evtGetFloat(event, args[2]);
    f3 = evtGetFloat(event, args[3]);
    unk1D4 = evtGetValue(event, args[4]);
    asyncName = "MOBJ_Signboard";
    unk1E4 = args[5];
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f1, f2, f3);
    obj->unk1D0 = mobj_signboard;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 0x24;
    obj->flags |= 0x40000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    return 2;
}

s32 mobj_signboard(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                marioKeyOff();
                mobjRunEvent(obj, obj->unk1D4);
                obj->unk1DC++;
            }
            break;
        case 1:
            if (evtCheckID(obj->eventId) != 1) {
                marioKeyOn();
                obj->eventId = 0;
                obj->flags &= ~8;
                obj->unk1DC = 0;
            }
            break;
    }
    return 0;
}

s32 evt_mobj_entry(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    const char* animName;

    name = (const char*)evtGetValue(event, args[0]);
    animName = (const char*)evtGetValue(event, args[1]);
    if (!animGroupBaseAsync(animName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, animName);
    return 2;
}

s32 evt_mobj_lock_unlock(EventEntry* event, BOOL isFirstCall) {
    const char* name = (const char*)evtGetValue(event, event->args[0]);
    MapObjectEntry* obj = mobjNameToPtr(name);

    if (strcmp(obj->animName, "MOBJ_Lock") != 0) {
        return 2;
    }
    if (obj->unk1D0 == NULL) {
        obj->unk1D0 = mobj_lock;
    }
    obj->flags |= 8;
    marioKeyOff();
    partyKeyOff();
    marioStSystemLevel(1);
    obj->unk1DC = 3;
    return 2;
}

s32 mobj_switch_red(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                mobjRunEvent(obj, obj->unk1D4);
                if (obj->unk1E0 == 0x200000 || obj->unk1E0 == 0x1000000) {
                    marioChgMot(5);
                }
                psndSFXOn_3D((const char*)0x1d2, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->flags &= ~8;
            }
            break;
        case 1:
            if (obj->flags & 8) {
                psndSFXOn_3D((const char*)0x1d2, &obj->position);
                if (obj->unk1E0 == 0x200000) {
                    marioChgMot(5);
                }
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->flags &= ~8;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_1", 1);
            obj->flags &= ~8;
            obj->unk1DC = 0;
            break;
    }
    return 0;
}

s32 mobj_switch_blue(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                mobjRunEvent(obj, obj->unk1D4);
                if (obj->unk1E0 == 0x200000 || obj->unk1E0 == 0x1000000) {
                    marioChgMot(5);
                }
                psndSFXOn_3D((const char*)0x1d2, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->unk1DC++;
            }
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_1", 1);
            effSmallStarCamChg(effSmallStarEntry(0, 4, obj->position.x,
                                                 obj->position.y + 12.5f, obj->position.z,
                                                 0.0f, -1.0f, 0.0f),
                               obj->camId);
            evtSetValue(NULL, obj->unk1E4, 1);
            obj->unk1DC = 0x63;
            /* fall through */
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

// NONMATCHING (95.54%): one extra callee-saved register (register-allocation mismatch)
s32 evt_mobj_floatswitch_red(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 x;
    f32 y;
    f32 z;
    s32 unk1D4;
    s32 unk1E4;
    const char* asyncName;

    evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    unk1D4 = evtGetValue(event, args[5]);
    unk1E4 = args[6];
    asyncName = "MOBJ_RedSwitchBlock";
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->unk1D0 = mobj_switch_red;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x400000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    mobjCalcMtx(obj);
    return 2;
}

s32 evt_mobj_switch_red(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    const char* asyncName;
    MapObjectEntry* obj;
    BOOL inBattle = gp->inBattle != 0;

    /* MUST_MATCH: the empty-through case 1 forces MWCC's switch decision tree; default alone folds to `beq`. */
#ifdef MUST_MATCH
    switch (type) {
        case 0:
            asyncName = "MOBJ_RedSwitch";
            break;
        case 1:
        default:
            asyncName = "MOBJ_WhiteSwitch";
            break;
    }
#else
    asyncName = "MOBJ_WhiteSwitch";
    if (type == 0) {
        asyncName = "MOBJ_RedSwitch";
    }
#endif
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->unk1D0 = mobj_switch_red;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 0x44;
    obj->flags |= 0xa10000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    mobjCalcMtx(obj);
    return 2;
}

// NONMATCHING (95.54%): one extra callee-saved register (register-allocation mismatch)
s32 evt_mobj_tornadoswitch_blue(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 x;
    f32 y;
    f32 z;
    s32 unk1D4;
    s32 unk1E4;
    const char* asyncName;

    evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    unk1D4 = evtGetValue(event, args[5]);
    unk1E4 = args[6];
    asyncName = "MOBJ_BlueTornadoSwitch";
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->unk1D0 = mobj_switch_blue;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x1000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    mobjCalcMtx(obj);
    return 2;
}

// NONMATCHING (95.54%): one extra callee-saved register (register-allocation mismatch)
s32 evt_mobj_floatswitch_blue(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 x;
    f32 y;
    f32 z;
    s32 unk1D4;
    s32 unk1E4;
    const char* asyncName;

    evtGetValue(event, args[0]);
    name = (const char*)evtGetValue(event, args[1]);
    x = evtGetFloat(event, args[2]);
    y = evtGetFloat(event, args[3]);
    z = evtGetFloat(event, args[4]);
    unk1D4 = evtGetValue(event, args[5]);
    unk1E4 = args[6];
    asyncName = "MOBJ_BlueSwitchBlock";
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->unk1D0 = mobj_switch_blue;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x400000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    mobjCalcMtx(obj);
    return 2;
}

// NONMATCHING (17.52%): the reference materialises the x/y/z arrays on the stack; MWCC scalar-
// replaces our arrays into f26-f31 (extra stfd/psq_st saves) and shifts every callee-saved
// register and the names[]/obj[] stack slots. Body/loop logic is otherwise identical.
s32 evt_mobj_timerswitch(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name0 = (const char*)evtGetValue(event, args[1]);
    f32 x0 = evtGetFloat(event, args[2]);
    f32 y0 = evtGetFloat(event, args[3]);
    f32 z0 = evtGetFloat(event, args[4]);
    const char* name1 = (const char*)evtGetValue(event, args[5]);
    f32 x1 = evtGetFloat(event, args[6]);
    f32 y1 = evtGetFloat(event, args[7]);
    f32 z1 = evtGetFloat(event, args[8]);
    s32 unk1B4 = evtGetValue(event, args[9]);
    s32 unk1D4 = evtGetValue(event, args[10]);
    s32 unk1E4 = args[11];
    const char* asyncName;
    MapObjectEntry* obj[2];
    const char* names[2];
    f32 xs[2];
    f32 ys[2];
    f32 zs[2];
    s32 i;
    BOOL inBattle = gp->inBattle != 0;

    {
        const char* jumptable[2] = {"MOBJ_BlueTimerSwitch", "MOBJ_RedTimerSwitch"};
        asyncName = jumptable[type];
    }
    if (type == 0 && evtGetValue(event, args[11]) != 0) {
        return 2;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    names[0] = name0;
    names[1] = name1;
    xs[0] = x0;
    xs[1] = x1;
    ys[0] = y0;
    ys[1] = y1;
    zs[0] = z0;
    zs[1] = z1;
    for (i = 0; i < 2; i++) {
        mobjEntry(names[i], asyncName);
        obj[i] = mobjNameToPtr(names[i]);
        strcpy((char*)((u8*)obj[i] + 0x1B8), (i == 0) ? name1 : name0);
        mobjSetPosition(names[i], xs[i], ys[i], zs[i]);
        obj[i]->unk1D0 = mobj_timerswitch;
        obj[i]->unk1D4 = (s32*)unk1D4;
        obj[i]->flags |= 0x44;
        obj[i]->flags |= 0xa10000;
        obj[i]->unk1E4 = unk1E4;
        obj[i]->unk1DC = 0;
        *(s32*)((u8*)obj[i] + 0x1B4) = unk1B4;
        if (evtGetValue(event, unk1E4) != 0) {
            obj[i]->unk1DC = 0x63;
        }
        mobjCalcMtx(obj[i]);
    }
    return 2;
}

extern u8 evt_cam_shake[];
static u32 evt_shake[] = {
    0x0005005B, (u32)evt_cam_shake, 0x00000004, 0xF24A7A80,
    0xF24A7A8A, 0x000000C8, 0x00000002, 0x00000001,
};

s32 mobj_timerswitch(MapObjectEntry* obj) {
    MapObjectEntry* child = mobjNameToPtr((char*)((u8*)obj + 0x1B8));

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                if (obj->unk1E0 == 0x200000) {
                    marioChgMot(5);
                }
                if (child->unk1DC != 2) {
                    psndSFXOn_3D((const char*)0x1C1, &obj->position);
                } else {
                    psndSFXOn_3D((const char*)0x1C2, &obj->position);
                }
                evtEntryType(evt_shake, 0, 0, 0);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->unk1DC++;
                obj->flags &= ~8;
            }
            break;
        case 1:
            if (obj->flags & 8) {
                animPoseSetAnim(obj->poseId, "S_1", 1);
                animPoseSetAnim(child->poseId, "S_1", 1);
                obj->unk1DC = 0;
                break;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 0.3f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "A_2", 1);
            animPoseSetAnim(child->poseId, "A_2", 1);
            obj->unk1DC++;
            *(s32*)((u8*)obj + 0x1B0) = *(s32*)((u8*)obj + 0x1B4);
            obj->flags &= ~8;
            break;
        case 2:
            if (obj->flags & 8) {
                animPoseSetAnim(obj->poseId, "S_1", 1);
                animPoseSetAnim(child->poseId, "S_1", 1);
                obj->unk1DC = 0;
                break;
            }
            (*(s32*)((u8*)obj + 0x1B0))--;
            if (*(s32*)((u8*)obj + 0x1B0) <= 0) {
                obj->flags &= ~8;
                animPoseSetAnim(obj->poseId, "S_1", 1);
                animPoseSetAnim(child->poseId, "S_1", 1);
                obj->unk1DC = 0;
                break;
            }
            if (child->unk1DC == 2) {
                if (marioKeyOffChk() != 0 || marioCtrlOffChk() != 0) {
                    obj->unk1DC = 0;
                    break;
                }
                mobjRunEvent(obj, obj->unk1D4);
                if (strcmp(obj->animName, "MOBJ_RedTimerSwitch") == 0) {
                    obj->flags &= ~8;
                    child->flags &= ~8;
                    animPoseSetAnim(obj->poseId, "S_1", 1);
                    animPoseSetAnim(child->poseId, "S_1", 1);
                    obj->unk1DC = 0;
                    child->unk1DC = 0;
                    break;
                }
                effSmallStarEntry(0, 8, obj->position.x, obj->position.y + 12.5f, obj->position.z,
                                  0.0f, -1.0f, 0.0f);
                effSmallStarEntry(0, 8, child->position.x, child->position.y + 12.5f,
                                  child->position.z, 0.0f, -1.0f, 0.0f);
                evtSetValue(NULL, obj->unk1E4, 1);
                evtSetValue(NULL, child->unk1E4, 1);
                mobjDelete(obj->name);
                mobjDelete(child->name);
            }
            break;
    }
    return 0;
}

s32 mobj_koopa_brickblk(MapObjectEntry* obj) {
    BOOL flag = FALSE;
    u8* base = (u8*)vec3_802c3dc8;

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                switch (kpaGetLevel()) {
                    case 0:
                        if (obj->unk1E0 == 0x20000000) {
                            flag = TRUE;
                        }
                        break;
                    case 1:
                        if (obj->unk1E0 == 0x20000000 || obj->unk1E0 == 0x4000000) {
                            flag = TRUE;
                        }
                        break;
                    case 2:
                        if (obj->unk1E0 == 0x20000000 || obj->unk1E0 == 0x10000000 ||
                            obj->unk1E0 == 0x4000000) {
                            flag = TRUE;
                        }
                        break;
                    case 3:
                        flag = TRUE;
                        break;
                }
                if (!flag) {
                    if (obj->unk1E0 == 0x4000000) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                        obj->unk1DC = 0x14;
                    } else {
                        obj->flags &= ~8;
                    }
                } else {
                    mobjRunEvent(obj, obj->unk1D4);
                    evtSetValue(NULL, obj->unk1E4, 1);
                    if (obj->unk1E0 == 0x4000000) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        obj->hitObj[0].hitObject->flags |= 1;
                    }
                    obj->flags |= 2;
                    kpaAddScore(kpaMutekiCheck() ? 500 : 400);
                    obj->unk1DC = 0xa;
                }
            }
            break;
        case 0xa:
            psndSFXOn_3D((const char*)0x1B8, &obj->position);
            if (strcmp(obj->animName, (const char*)(base + 0x488)) == 0) {
                effMObjBrokenEntry(0, obj->position.x, obj->position.y + 12.5f,
                                   obj->position.z + 12.5f);
            } else if (strcmp(obj->animName, (const char*)(base + 0x49C)) == 0) {
                effMObjBrokenEntry(1, obj->position.x, obj->position.y + 12.5f,
                                   obj->position.z + 12.5f);
            } else if (strcmp(obj->animName, (const char*)(base + 0x4AC)) == 0) {
                effMObjBrokenEntry(3, obj->position.x, obj->position.y + 25.0f,
                                   obj->position.z + 25.0f);
            } else if (strcmp(obj->animName, (const char*)(base + 0x4C0)) == 0) {
                effMObjBrokenEntry(4, obj->position.x, obj->position.y + 25.0f,
                                   obj->position.z + 25.0f);
            }
            obj->unk1DC = 0x63;
            break;
        case 0x14:
            psndSFXOn_3D((const char*)0x1B7, &obj->position);
            animPoseSetAnim(obj->poseId, "A_1", 1);
            obj->unk1DC = 0x15;
            break;
        case 0x15:
            if (!(obj->flags & 8)) {
                obj->unk1DC = 0;
                break;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_1", 1);
            obj->unk1DC = 0;
            obj->flags &= ~8;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

// NONMATCHING (98.83%): 2 instructions differ, 1 extra, plus register differences.
s32 evt_mobj_koopa_fireber_dodai(EventEntry* event, BOOL isFirstCall) {
    u8* base = (u8*)vec3_802c3dc8;
    s32* args = event->args;
    s32 sign = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    s32 count = evtGetValue(event, args[5]);
    f32 f27 = evtGetFloat(event, args[6]);
    s32 unk1D4 = evtGetValue(event, args[7]);
    s32 unk1E4 = args[8];
    BOOL inBattle = gp->inBattle != 0;
    MapObjectEntry* obj;
    Vec pos;
    s32 i;
    char buf[32];

    if (!animGroupBaseAsync((const char*)(base + 0x3D8), inBattle, NULL)) {
        return 0;
    }
    if (!animGroupBaseAsync((const char*)(base + 0x3E8), inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, (const char*)(base + 0x3D8));
    obj = mobjNameToPtr(name);
    obj->flags |= 4;
    pos = *(Vec*)(base + 0x308);
    pos.x = x;
    pos.y = y;
    pos.z = z;
    obj->position = pos;
    obj->unk1D0 = mobj_koopa_fireber_dodai_evt;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = NULL;
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    if (f27 == 0.0f) {
        f27 = 1.0f;
    }
    *(s32*)((u8*)obj + 0x1A4) = count;
    *(f32*)((u8*)obj + 0x1AC) = (sign != 0) ? __fabsf(f27) : -__fabsf(f27);
    *(s32*)((u8*)obj + 0x1B4) = 0;
    y = 25.0f + y;
    z = 175.0f + z;
    for (i = 0; i < count; i++) {
        NpcEntry* npc;

        sprintf(buf, "%s%2x", name, i);
        npcEntry(buf, (const char*)(base + 0x3E8));
        npc = npcNameToPtr(buf);
        npc->flags |= 0x08000000;
        *(u16*)((u8*)npc + 0x31E) |= 0xF;
        npc->position.x = x;
        npc->position.y = 25.0f * (f32)i + y;
        npc->position.z = z;
        animPoseSetAnim(npc->poseId, (const char*)(base + 0x3F4), 1);
        npc->flags |= 0x40000;
    }
    return 2;
}

// NONMATCHING (73.98%): 21 instructions differ, 41 missing, 25 extra, plus register differences.
void kururing_capture(CameraId cameraId, void* param) {
    MapObjectEntry* obj = param;
    u8* base = (u8*)vec3_802c3dc8;
    CameraEntry save;
    CameraEntry* cam;
    AnimationPoseData* animData;
    Vec bboxMax;
    Vec bboxMin;
    u16 texWidth;
    u16 texHeight;
    f32 t;

    memcpy(&save, camGetPtr(cameraId), 0x260);
    animData = animPoseGetAnimBaseDataPtr(obj->poseId);
    bboxMax = animData->bboxMax;
    bboxMin = animData->bboxMin;
    texWidth = (s32)((bboxMax.x - bboxMin.x) * float_10_8041a1a4);
    texHeight = (s32)((bboxMax.z - bboxMin.z) * float_10_8041a1a4);
    cam = camGetPtr(cameraId);
    texWidth = (texWidth + 1) & ~1;
    texHeight = (texHeight + 1) & ~1;
    cam->cameraUp = *(Vec*)(base + 0x27C);
    cam->cameraAt = obj->position;
    cam->cameraPos = obj->position;
    t = (f32)tan(*(f64*)(base + 0x510));
    cam->cameraPos.y += 0.5f * (f32)texHeight / t;
    C_MTXPerspective(cam->projection, float_25_8041a140, (f32)(texWidth / texHeight),
                     float_1_8041a160, float_32767_8041a1e4);
    cam->projType = GX_PERSPECTIVE;
    C_MTXLookAt(cam->view, &cam->cameraPos, &cam->cameraUp, &cam->cameraAt);
    GXSetProjection(cam->projection, cam->projType);
    GXSetViewport(float_0_8041a150, float_0_8041a150, (f32)texWidth, (f32)texHeight, float_0_8041a150,
                  float_1_8041a160);
    GXSetScissor(0, 0, texWidth, texHeight);
    test_kururing_mapdisp(cameraId);
    sysWaitDrawSync();
    obj->unk208 = smartAlloc(GXGetTexBufferSize(texWidth / 2, texHeight / 2, 6, 0, 0), 3);
    GXSetTexCopySrc(0, 0, texWidth, texHeight);
    GXSetTexCopyDst(texWidth / 2, texHeight / 2, 6, 1);
    GXSetZMode(1, 3, 1);
    GXCopyTex(((smartEntry*)obj->unk208)->address, 1);
    GXInvalidateTexAll();
    GXPixModeSync();
    GXTexModeSync();
    GXInitTexObj(&obj->paperTexObj, ((smartEntry*)obj->unk208)->address, texWidth / 2,
                 texHeight / 2, 6, 0, 0, 0);
    GXInitTexObjLOD(&obj->paperTexObj, 0, 0, float_0_8041a150, float_0_8041a150, float_0_8041a150, 0,
                    0, 0);
    PSMTXIdentity(obj->paperTexMtx);
    memcpy(camGetPtr(cameraId), &save, 0x260);
}

s32 evt_mobj_brick(EventEntry* event, BOOL isFirstCall) {
    u8* base = (u8*)vec3_802c3dc8;
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    f32 x = evtGetFloat(event, args[1]);
    f32 y = evtGetFloat(event, args[2]);
    f32 z = evtGetFloat(event, args[3]);
    s32 unk1A0 = evtGetValue(event, args[4]);
    s32 type = args[5];
    s32 unk1D4 = evtGetValue(event, args[6]);
    s32 evtId = args[7];
    BOOL inBattle = gp->inBattle != 0;
    const char* animName;
    MapObjectEntry* obj;

    switch (type) {
        case 0:
            animName = (const char*)(base + 0x488);
            break;
        case 1:
            animName = (const char*)(base + 0x54C);
            break;
        case 2:
            animName = (const char*)(base + 0x564);
            break;
        case 3:
            animName = (const char*)(base + 0x57C);
            break;
        case 0xa:
            animName = (const char*)(base + 0x45C);
            break;
        case 0xb:
            animName = (const char*)(base + 0x45C);
            break;
        case 0xc:
            animName = (const char*)(base + 0x590);
            break;
        case 0xd:
            animName = (const char*)(base + 0x5A8);
            break;
    }
    if (!animGroupBaseAsync(animName, inBattle, NULL)) {
        return 0;
    }
    if (!animGroupBaseAsync((const char*)(base + 0x5C0), inBattle, NULL)) {
        return 0;
    }
    mobjEntry((const char*)name, animName);
    obj = mobjNameToPtr((const char*)name);
    mobjSetPosition((const char*)name, x, y, z);
    if (type >= 0xa) {
        obj->flags |= 0x800;
    }
    switch (type) {
        case 0:
        case 0xa:
            obj->unk1D0 = mobj_brickblk;
            break;
        case 1:
        case 0xb:
            obj->unk1D0 = mobj_badgeblk;
            break;
        case 2:
        case 0xc:
            obj->unk1D0 = mobj_badgeblk;
            break;
        case 3:
        case 0xd:
            obj->unk1D0 = mobj_10countblk;
            break;
    }
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0xC10000;
    obj->unk1E4 = evtId;
    obj->unk1DC = 0;
    *(s32*)((u8*)obj + 0x1A0) = unk1A0;
    mobjCalcMtx(obj);
    if (type < 0xa) {
        animPoseSetAnim(obj->poseId, "S_1", 1);
    } else if (type == 0xd) {
        animPoseSetAnim(obj->poseId, "S_3", 1);
    } else {
        animPoseSetAnim(obj->poseId, "S_1", 1);
    }
    if (evtGetValue(event, evtId) != 0) {
        const char* newName = NULL;

        switch (type) {
            case 0:
            case 0xa:
                animPoseSetAnim(obj->poseId, "S_1", 1);
                obj->unk1DC = 0x63;
                break;
            case 1:
            case 0xb:
                animPoseSetAnim(obj->poseId, "S_2", 1);
                obj->unk1DC = 0x5a;
                newName = (const char*)(base + 0x450);
                break;
            case 2:
            case 0xc:
                animPoseSetAnim(obj->poseId, "S_2", 1);
                obj->unk1DC = 0x5a;
                newName = (const char*)(base + 0x5C8);
                break;
            case 3:
            case 0xd:
                animPoseSetAnim(obj->poseId, "S_2", 1);
                obj->unk1DC = 0x5a;
                newName = (const char*)(base + 0x450);
                break;
        }
        if (newName != NULL) {
            strcpy(obj->animName, newName);
        }
        obj->flags &= ~0x800;
    }
    return 2;
}

// NONMATCHING (94.95%): the `char x[16] = "o_"` initializer templates pool at
// different .rodata offsets (ours) than retail's base+0x238/base+0x248, so the
// 16-byte copy loads reference a separate `lis/addi` base.
s32 mobj_brickblk(MapObjectEntry* obj) {
    u8* base = (u8*)vec3_802c3dc8;
    char offName[16] = "o_";
    char imgName[16] = "i_";
    BOOL inBattle = gp->inBattle != 0;
    ImageEntry* img;

    strcat(offName, obj->name);
    strcat(imgName, obj->name);
    switch (obj->unk1DC) {
        case 0:
            animGroupBaseAsync((const char*)(base + 0x5C0), inBattle, NULL);
            if (obj->flags & 8) {
                mobjRunEvent(obj, obj->unk1D4);
                evtSetValue(NULL, obj->unk1E4, 1);
                obj->unk1DC = 0xa;
            }
            break;
        case 0xa: {
            s32 captureArgs1[10];
            EventEntry tmpEvent1;
            s32 captureArgs2[10];
            EventEntry tmpEvent2;

            animPaperPoseEntry((const char*)(base + 0x5C0), inBattle);
            offscreenEntry(offName);
            imgEntry(imgName, inBattle);
            *(s32*)((u8*)obj + 0x19C) = offscreenNameToId(offName);
            tmpEvent1.args = captureArgs1;
            captureArgs1[0] = (s32)imgName;
            captureArgs1[1] = 0;
            captureArgs1[2] = (s32)offName;
            captureArgs1[3] = 1;
            captureArgs1[4] = 0;
            captureArgs1[5] = 0;
            captureArgs1[6] = 0;
            captureArgs1[7] = 0;
            evt_img_alloc_capture(&tmpEvent1, 1);
            tmpEvent2.args = captureArgs2;
            captureArgs2[0] = (s32)imgName;
            captureArgs2[1] = 1;
            captureArgs2[2] = (s32)offName;
            captureArgs2[3] = 1;
            captureArgs2[4] = 0;
            captureArgs2[5] = 0;
            captureArgs2[6] = 0;
            captureArgs2[7] = 0;
            evt_img_alloc_capture(&tmpEvent2, 1);
            img = imgNameToPtr(imgName, inBattle);
            img->poseId = animPaperPoseGetId((const char*)(base + 0x5C0), inBattle);
            img->animName = "A_1";
            *(OSTime*)((u8*)img + 0x110) = animTimeGetTime(inBattle);
            imgSetShadow(img, 2);
            psndSFXOn_3D((const char*)0x1B8, &obj->position);
            obj->unk1DC = 0xb;
            break;
        }
        case 0xb:
            img = imgNameToPtr(imgName, inBattle);
            if (img->field_0x118 >= 1.0f) {
                obj->unk1DC = 0xc;
            }
            if (img->flags & 0x10000000) {
                obj->unk1DC = 0xc;
            }
            obj->position = *(Vec*)(base + 0x258);
            obj->flags |= 4;
            obj->flags |= 8;
            break;
        case 0xc:
            img = imgNameToPtr(imgName, inBattle);
            imgFreeCapture(img, 0);
            imgFreeCapture(img, 1);
            imgSetShadow(img, 0);
            imgRelease(img, inBattle);
            offscreenDelete(offName);
            *(s32*)((u8*)obj + 0x19C) = -1;
            obj->unk1DC = 0x63;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

// NONMATCHING (99.35%): the itemEntry argument setup schedules the evtId load before the fadds and the x
// load after it (target: x load, fadds, z load, evtId load); 2 replaced instructions, otherwise identical.
s32 mobj_badgeblk(MapObjectEntry* obj) {
    u8* base = (u8*)vec3_802c3dc8;

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                if (strcmp(obj->animName, (const char*)(base + 0x614)) == 0 ||
                    strcmp(obj->animName, (const char*)(base + 0x564)) == 0 ||
                    strcmp(obj->animName, (const char*)(base + 0x590)) == 0) {
                    psndSFXOn_3D((const char*)0x1BB, &obj->position);
                } else {
                    psndSFXOn_3D((const char*)0x1BA, &obj->position);
                }
                if (obj->flags & 0x800) {
                    obj->flags &= ~0x800;
                }
                animPoseSetAnim(obj->poseId, "A_1", 1);
                if (obj->unk1D4 != NULL) {
                    mobjRunEvent(obj, obj->unk1D4);
                } else if (*(s32*)((u8*)obj + 0x1A0) != 0x79) {
                    ItemEntry* item;
#ifdef MUST_MATCH
                    // Two-step sum loads the 0.04 constant into f2 (the target's register) instead of f1.
                    f32 y = float_40_8041a1ec;

                    y += obj->position.y;
#else
                    f32 y = float_40_8041a1ec + obj->position.y;
#endif
                    itemEntry(obj->name, (ItemType)(*(s32*)((u8*)obj + 0x1A0)), 4, obj->unk1E4, 0,
                              obj->position.x, y, obj->position.z);
                    item = itemNameToPtr(obj->name);
                    itemFlagOn(item, 0x10);
                    *(f32*)((u8*)item + 0x80) = float_0p7_8041a1c8;
                    *(f32*)((u8*)item + 0x84) = 1.0f;
                    *(s32*)((u8*)item + 0x88) = 0;
                    *(f32*)((u8*)item + 0x58) = 360.0f;
                    psndSFXOn_3D((const char*)0x1BC, &obj->position);
                } else {
                    pouchAddCoin(1);
                    itemEntry(NULL, (ItemType)0x79, 0xf, -1, 0, obj->position.x, obj->position.y,
                              obj->position.z);
                    evtSetValue(NULL, obj->unk1E4, 1);
                    psndSFXOn_3D((const char*)0x5B, &obj->position);
                }
                obj->unk1DC++;
            }
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_2", 1);
            obj->flags &= ~8;
            obj->unk1DC = 0x5a;
            if (strcmp(obj->animName, (const char*)(base + 0x474)) == 0 ||
                strcmp(obj->animName, (const char*)(base + 0x54C)) == 0 ||
                strcmp(obj->animName, (const char*)(base + 0x45C)) == 0) {
                strcpy(obj->animName, (const char*)(base + 0x450));
            } else {
                strcpy(obj->animName, (const char*)(base + 0x5C8));
            }
            break;
        case 0x5a:
            if (obj->flags & 8) {
                obj->flags &= ~8;
                animPoseSetAnim(obj->poseId, "A_2", 1);
                psndSFXOn_3D((const char*)0x1B7, &obj->position);
                obj->unk1DC++;
            }
            break;
        case 0x5b:
            if (!(obj->flags & 8)) {
                obj->unk1DC = 0x5a;
                break;
            }
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_2", 1);
            obj->unk1DC = 0x5a;
            obj->flags &= ~8;
            break;
    }
    return 0;
}

s32 evt_mobj_switch_blue(EventEntry* event, BOOL isFirstCall) {
#ifdef MUST_MATCH
    u8* base = (u8*)vec3_802c3dc8;
#endif
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 x = evtGetFloat(event, args[2]);
    f32 y = evtGetFloat(event, args[3]);
    f32 z = evtGetFloat(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    const char* asyncName;
    MapObjectEntry* obj;
    BOOL inBattle = gp->inBattle != 0;

    switch (type) {
        case 0:
#ifdef MUST_MATCH
            asyncName = (const char*)(base + 0x9d8);
#else
            asyncName = "MOBJ_BlueSwitch";
#endif
            break;
        case 1:
#ifdef MUST_MATCH
            asyncName = (const char*)(base + 0x9e8);
#else
            asyncName = "MOBJ_BlackSwitch";
#endif
            break;
        case 2:
        default:
#ifdef MUST_MATCH
            asyncName = (const char*)(base + 0x9fc);
#else
            asyncName = "HOBJ_BlueSwitch";
#endif
            break;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, x, y, z);
    obj->unk1D0 = mobj_switch_blue;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 0x44;
    obj->flags |= 0xa10000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (type == 2) {
        obj->flags |= 2;
    }
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    mobjCalcMtx(obj);
    return 2;
}

// NONMATCHING (93.34%): register allocation (r28/r30 swapped) around the local jumptable
s32 evt_mobj_jumpstand_blue(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 f31 = evtGetFloat(event, args[2]);
    f32 f30 = evtGetFloat(event, args[3]);
    f32 f29 = evtGetFloat(event, args[4]);
    f32 f28 = evtGetFloat(event, args[5]);
    s32 unk1D4 = evtGetValue(event, args[6]);
    s32 unk1E4 = args[7];
    const char* asyncName;
    MapObjectEntry* obj;

    {
        const char* jumptable[2] = {"MOBJ_BlueJumpStand", "MOBJ_BlueJumpStand2"};
        asyncName = jumptable[type];
    }
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f30, f29, f28);
    obj->unk1D0 = mobj_jumpstand_blue;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 0x44;
    obj->flags |= 0x200000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    *(f32*)((u8*)obj + 0x1AC) = f31;
    mobjCalcMtx(obj);
    return 2;
}

s32 mobj_jumpstand_blue(MapObjectEntry* obj) {
    MarioWork* mario = marioGetPtr();

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                gp->systemLevelFlags |= 0x10;
                mobjRunEvent(obj, obj->unk1D4);
                if (obj->unk1E0 == 0x200000) {
                    marioChgJumpStandMotion(*(f32*)((u8*)obj + 0x1AC));
                }
                if (mario->characterId != 2) {
                    if (strcmp(obj->animName, "MOBJ_BlueJumpStand") == 0) {
                        psndSFXOn_3D((const char*)0x1eb, &obj->position);
                    } else {
                        psndSFXOn_3D((const char*)0x1ec, &obj->position);
                    }
                    switch (irand(4)) {
                        case 0:
                            psndSFXOn_3D((const char*)0xab, &obj->position);
                            break;
                        case 1:
                            psndSFXOn_3D((const char*)0xac, &obj->position);
                            break;
                        case 2:
                            psndSFXOn_3D((const char*)0xad, &obj->position);
                            break;
                        case 3:
                            psndSFXOn_3D((const char*)0xae, &obj->position);
                            break;
                        case 4:
                            psndSFXOn_3D((const char*)0xaf, &obj->position);
                            break;
                    }
                } else if (strcmp(obj->animName, "MOBJ_BlueJumpStand") == 0) {
                    psndSFXOn((const char*)0x1eb);
                } else {
                    psndSFXOn((const char*)0x1ec);
                }
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->flags &= ~8;
                obj->unk1DC++;
            }
            break;
        case 1:
            if (obj->flags & 8) {
                animPoseSetAnim(obj->poseId, "S_1", 1);
                obj->unk1DC = 0;
            } else if (evtCheckID(obj->eventId) != 1) {
                gp->systemLevelFlags &= ~0x10;
                if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                    break;
                }
                animPoseSetAnim(obj->poseId, "S_1", 1);
                obj->unk1DC = 0;
            }
            break;
    }
    return 0;
}

// NONMATCHING (91.16%): register allocation around the local jumptable
s32 evt_mobj_jumpstand_red(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 f31 = evtGetFloat(event, args[2]);
    f32 f30 = evtGetFloat(event, args[3]);
    f32 f29 = evtGetFloat(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    const char* asyncName;
    MapObjectEntry* obj;

    {
        const char* jumptable[2] = {"MOBJ_RedJumpStand", "MOBJ_RedJumpStand2"};
        asyncName = jumptable[type];
    }
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_jumpstand_red;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 0x44;
    obj->flags |= 0x200000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    mobjCalcMtx(obj);
    return 2;
}

s32 mobj_jumpstand_red(MapObjectEntry* obj) {
    MarioWork* mario = marioGetPtr();
    s32 partyId = marioGetPartyId();

    partyGetPtr(partyId);
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                EventEntry* evt;
                gp->systemLevelFlags |= 0x10;
                evt = mobjRunEvent(obj, obj->unk1D4);
                if (evt != NULL) {
                    s32 flag;
                    if (*(f32*)((u8*)mario + 0xD8) < mario->position.y) {
                        flag = 0;
                    } else if (*(f32*)((u8*)mario + 0xD8) <
                               obj->position.y + mario->wPlayerCollisionBox.y) {
                        flag = 0;
                    } else {
                        flag = 1;
                    }
                    evt->lwData[0] = flag;
                }
                if (mario->characterId != 2) {
                    if (strcmp(obj->animName, "MOBJ_RedJumpStand") == 0) {
                        psndSFXOn_3D((const char*)0x1eb, &obj->position);
                    } else {
                        psndSFXOn_3D((const char*)0x1ec, &obj->position);
                    }
                    switch (irand(4)) {
                        case 0:
                            psndSFXOn_3D((const char*)0xab, &obj->position);
                            break;
                        case 1:
                            psndSFXOn_3D((const char*)0xac, &obj->position);
                            break;
                        case 2:
                            psndSFXOn_3D((const char*)0xad, &obj->position);
                            break;
                        case 3:
                            psndSFXOn_3D((const char*)0xae, &obj->position);
                            break;
                        case 4:
                            psndSFXOn_3D((const char*)0xaf, &obj->position);
                            break;
                    }
                } else if (strcmp(obj->animName, "MOBJ_RedJumpStand") == 0) {
                    psndSFXOn((const char*)0x1eb);
                } else {
                    psndSFXOn((const char*)0x1ec);
                }
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->flags &= ~8;
                obj->unk1DC++;
            }
            break;
        case 1:
            if (obj->flags & 8) {
                if (evtCheckID(obj->eventId) == 0) {
                    animPoseSetAnim(obj->poseId, "S_1", 1);
                    obj->unk1DC = 0;
                    break;
                }
            }
            if (evtCheckID(obj->eventId) != 1) {
                gp->systemLevelFlags &= ~0x10;
                if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                    break;
                }
                animPoseSetAnim(obj->poseId, "S_1", 1);
                obj->unk1DC = 0;
            }
            break;
    }
    return 0;
}

// NONMATCHING (96.42%): one extra callee-saved register
s32 evt_mobj_rideswitch_lightblue(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name;
    MapObjectEntry* obj;
    f32 f31;
    f32 f30;
    f32 f29;
    s32 unk1D4;
    s32 unk1D8;
    s32 unk1E4;
    const char* asyncName;

    name = (const char*)evtGetValue(event, args[0]);
    f31 = evtGetFloat(event, args[1]);
    f30 = evtGetFloat(event, args[2]);
    f29 = evtGetFloat(event, args[3]);
    unk1D4 = evtGetValue(event, args[4]);
    unk1D8 = evtGetValue(event, args[5]);
    unk1E4 = args[6];
    asyncName = str_802C46E4;
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    obj->scale2.x *= 0.8f;
    obj->scale2.z *= 0.8f;
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_rideswitch_lightblue;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = (s32*)unk1D8;
    obj->flags |= 4;
    obj->flags |= 0x200000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    mobjCalcMtx2(obj);
    return 2;
}

// NONMATCHING (84.10%): case 2's bbox float computation is scheduled with different FP registers
// (and the .sdata2/rodata constant symbols are anonymous @NNN here).
s32 mobj_rideswitch_lightblue(MapObjectEntry* obj) {
    Vec* vec = vec3_802c3dc8;

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                mobjRunEvent(obj, obj->unk1D4);
                psndSFXOn_3D((const char*)0x1d3, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                obj->unk1DC = 1;
            }
            break;
        case 1: {
            f32 x = obj->scale2.x;
            f32 y = obj->scale2.y - 0.9f;
            f32 z = obj->scale2.z;
            Vec v;

            if (y <= 0.2f) {
                obj->unk1DC = 2;
                obj->flags &= ~8;
                y = 0.2f;
            }
            v = *(Vec*)((u8*)vec + 0x7C);
            v.x = x;
            v.y = y;
            v.z = z;
            obj->scale2 = v;
            mobjCalcMtx2(obj);
            break;
        }
        case 2: {
            AnimationPoseData* data = animPoseGetAnimBaseDataPtr(obj->poseId);
            Vec v = *(Vec*)((u8*)vec + 0x88);
            f32 mag;
            f32 dist;

            v.x = 0.8f * (10.0f * data->bboxMax.x);
            v.z = 0.8f * (10.0f * data->bboxMax.z);
            v.y = 10.0f * data->bboxMax.y;
            mag = PSVECMag(&v);
            dist = PSVECDistance(&obj->position, &marioGetPtr()->position);
            if (dist > mag) {
                if (!(obj->flags & 8)) {
                    obj->unk1DC = 3;
                    animPoseSetAnim(obj->poseId, "A_2", 1);
                    psndSFXOn_3D((const char*)0x1d4, &obj->position);
                    mobjRunEvent(obj, obj->unk1D8);
                } else {
                    obj->flags &= ~8;
                }
            }
            break;
        }
        case 3: {
            f32 x = obj->scale2.x;
            f32 y = obj->scale2.y + 0.9f;
            f32 z = obj->scale2.z;
            Vec v;

            if (y >= 15.0f) {
                obj->unk1DC = 0;
                y = 15.0f;
            }
            v = *(Vec*)((u8*)vec + 0x94);
            v.x = x;
            v.y = y;
            v.z = z;
            obj->scale2 = v;
            mobjCalcMtx2(obj);
            obj->flags &= ~8;
            break;
        }
        case 0x63:
            break;
    }
    return 0;
}

s32 evt_mobj_koopa_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    MapObjectEntry* obj;

    if (!animGroupBaseAsync("MOBJ_Block", gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, "MOBJ_Block");
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_koopa_blk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    return 2;
}

s32 mobj_koopa_blk(MapObjectEntry* obj) {
    s32 flag = 0;

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                // MUST_MATCH: the empty case 2 makes MWCC emit the retail switch decision tree
                // (cmpwi/beq/bge/b) instead of folding the two default edges together.
#ifdef MUST_MATCH
                switch (kpaGetLevel()) {
                    case 2:
                        break;
                    case 3:
                        flag = 1;
                        break;
                }
#else
                if (kpaGetLevel() == 3) {
                    flag = 1;
                }
#endif
                if (flag == 0) {
                    obj->flags &= ~8;
                } else {
                    mobjRunEvent(obj, obj->unk1D4);
                    evtSetValue(NULL, obj->unk1E4, 1);
                    if (obj->unk1E0 == 0x4000000) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        *(u16*)obj->hitObj[0].hitObject |= 1;
                    }
                    obj->flags |= 2;
                    obj->unk1DC = 0xa;
                }
            }
            break;
        case 0xa:
            psndSFXOn_3D((const char*)0x1b8, &obj->position);
            effMObjBrokenEntry(2, obj->position.x, float_12p5_8041a178 + obj->position.y,
                                 float_12p5_8041a178 + obj->position.z);
            obj->unk1DC = 0x63;
            /* fall through */
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

s32 evt_mobj_powerupblk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f30 = evtGetFloat(event, args[1]);
    f32 f29 = evtGetFloat(event, args[2]);
    f32 f28 = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[4]);
    s32 unk1E4 = args[5];
    f32 f31 = evtGetFloat(event, args[6]);
    const char* asyncName = "MOBJ_PowerUpBlock";
    MapObjectEntry* obj;

    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f30, f29, f28);
    obj->unk1D0 = mobj_powerupblk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0xc10000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    obj->rotation.y = f31;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x5a;
        animPoseSetAnim(obj->poseId, "A_2", 1);
    }
    return 2;
}

extern s32 mobj_save_blk_sysevt[];

s32 evt_mobj_save_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[4]);
    s32 unk1E4 = args[5];
    const char* asyncName = "MOBJ_SaveBlock";
    MapObjectEntry* obj;
    BOOL inBattle = gp->inBattle != 0;

    if (gp->flags & 0x20) {
        return 2;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_save_blk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = mobj_save_blk_sysevt;
    obj->flags |= 0x84;
    obj->flags |= 0x400000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    return 2;
}

s32 mobj_save_blk(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                marioKeyOff();
                marioStSystemLevel(1);
                psndSFXOn_3D((const char*)0x1AF, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                camGetPtr(8)->flags |= 0x200;
                obj->unk1DC++;
            }
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < float_1_8041a160) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_1", 1);
            if (obj->unk1D4 != NULL) {
                mobjRunEvent(obj, obj->unk1D4);
                obj->unk1DC++;
            } else {
                obj->unk1DC = 3;
            }
            break;
        case 2:
            if (evtCheckID(obj->eventId) != 1) {
                obj->eventId = 0;
                obj->unk1DC++;
            }
            break;
        case 3:
            mobjRunEvent(obj, obj->unk1D8);
            obj->unk1DC++;
            break;
        case 4:
            if (evtCheckID(obj->eventId) != 1) {
                marioStSystemLevel(0);
                camGetPtr(8)->flags &= ~0x200;
                obj->flags &= ~8;
                marioKeyOn();
                obj->unk1DC = 0;
            }
            break;
    }
    return 0;
}

extern Vec vec3_802c40a0[];

s32 evt_mobj_koopa_ojama_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f29 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f31 = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[4]);
    s32 unk1E4 = args[5];
    const char* asyncName = (const char*)evtGetValue(event, args[6]);
    MapObjectEntry* obj;
    Vec pos;

    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    obj->flags |= 4;
    pos = vec3_802c40a0[0];
    pos.x = f29;
    pos.y = f30;
    pos.z = f31;
    obj->position = pos;
    obj->unk1D0 = mobj_koopa_ojama_blk_evt;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = 0;
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    return 2;
}

// NONMATCHING (97.78%): gp materialised in r4/string in r5 (retail uses r5/r4) and args[6] loaded
// one instruction earlier than retail; otherwise identical.
s32 evt_mobj_koopa_badgeblk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 unk1A0 = evtGetValue(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    const char* asyncName = "MOBJ_HatenaBlock";
    MapObjectEntry* obj;

    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_koopa_badgeblk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    *(s32*)obj->unk1A0 = unk1A0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x5a;
        animPoseSetAnim(obj->poseId, "S_2", 1);
    }
    return 2;
}

// NONMATCHING (88.34%): register allocation for args[6]/args[7] is swapped (retail r28=args[7],
// r29=args[6]) and the two `?:` name selections are scheduled differently.
s32 evt_mobj_badgeblk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 unk1A0 = evtGetValue(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    s32 var = args[7];
    const char* asyncName;
    const char* animName;
    MapObjectEntry* obj;

    asyncName = (var == 0) ? "MOBJ_HatenaBlock" : "MOBJ_BadgeBlock";
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_badgeblk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0xc10000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    *(s32*)obj->unk1A0 = unk1A0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x5a;
        animPoseSetAnim(obj->poseId, "S_2", 1);
        animName = (var == 0) ? "MOBJ_Block" : "MOBJ_PinkBlock";
        strcpy(obj->animName, animName);
    }
    return 2;
}

extern Vec vec3_802c3ef4[];
extern const char str_802C41DC[];
extern const char str_802C41F0[];

s32 evt_mobj_lv_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f29 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f31 = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[4]);
    s32 unk1E4 = args[5];
    const char* asyncName = (const char*)evtGetValue(event, args[6]);
    MapObjectEntry* obj;
    Vec pos;
    BOOL inBattle = gp->inBattle != 0;

    if (evtGetValue(event, unk1E4) != 0) {
        return 2;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    if (!animGroupBaseAsync("P_box_y", inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    obj->flags |= 0x44;
    pos = vec3_802c3ef4[0];
    pos.x = f29;
    pos.y = f30;
    pos.z = f31;
    obj->position = pos;
    obj->unk1D0 = mobj_lv_blk_evt;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = 0;
    obj->flags |= 0x10000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    return 2;
}

s32 mobj_lock(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                marioKeyOff();
                partyKeyOff();
                marioStSystemLevel(1);
                mobjRunEvent(obj, obj->unk1D4);
                obj->unk1DC++;
            }
            break;
        case 1:
            if (evtCheckID(obj->eventId) == 1) {
                break;
            }
            if (obj->flags & 0x1000) {
                obj->flags &= ~0x1000;
                obj->flags &= ~0x8;
                marioKeyOn();
                partyKeyOn();
                marioStSystemLevel(0);
                obj->eventId = 0;
                obj->unk1DC = 0;
            } else {
                obj->eventId = 0;
                obj->unk1DC++;
            }
            break;
        case 2: {
            s32 item;

            if (*(s32*)obj->unk1A0 & 0x80000000) {
                s32 i;
                s32 check;

                item = 0;
                i = 0;
                while (1) {
                    check = ((s32*)*(s32*)obj->unk1A0)[i];
                    if (check == -1) {
                        break;
                    }
                    check = pouchCheckItem(check);
                    if (check != 0) {
                        item = check;
                        break;
                    }
                    i++;
                }
            } else {
                s32 check = pouchCheckItem(*(s32*)obj->unk1A0);

                item = (check != 0) ? *(s32*)obj->unk1A0 : 0;
            }
            if (item != 0) {
                pouchRemoveItem(item);
                obj->unk1DC++;
            } else {
                *(s32*)&obj->unk1A0[8] = msgWindow_Entry(msgSearch(str_802C44EC), 0, 0);
                obj->unk1DC = 0xa;
            }
            break;
        }
        case 3: {
            EventEntry* event = evtEntryType(evt_lock_sound, 0, 0, 0);
            f32 scale = float_1024_8041a1a0;

            event->lwData[0] = (s32)(scale * obj->position.x) + 0xF24A7A80;
            event->lwData[1] = (s32)(scale * obj->position.y) + 0xF24A7A80;
            event->lwData[2] = (s32)(scale * obj->position.z) + 0xF24A7A80;
            animPoseSetAnim(obj->poseId, str_8041A12C, 1);
            obj->unk1DC++;
            break;
        }
        case 4:
            if (animPoseGetLoopTimes(obj->poseId) < float_1_8041a160) {
                break;
            }
            obj->unk5C.y = 0.0f;
            obj->unk68 = 0.5f;
            obj->unk6C = 0.0f;
            evtSetValue(NULL, obj->unk1E4, 1);
            mobjRunEvent(obj, obj->unk1D8);
            obj->unk1DC++;
            break;
        case 5: {
            f32 hitX, hitY, hitZ, distance, hitNX, hitNY, hitNZ;

            distance = float_1_8041a160 + (f32)__fabs(obj->unk5C.y);
            if (hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                               obj->position.x, float_1_8041a160 + obj->position.y, obj->position.z,
                               float_0_8041a150, obj->unk5C.y, float_0_8041a150) != NULL) {
                obj->position.y = hitY;
                obj->unk5C.y *= float_neg0p8_8041a220;
                obj->unk6C += 1.0f;
                if (obj->unk6C < float_4_8041a224) {
                    psndSFXOn_3D((const char*)0x1e6, &obj->position);
                }
            } else {
                obj->position.y += obj->unk5C.y;
                obj->unk5C.y -= obj->unk68;
            }
            if (obj->unk6C < float_4_8041a224) {
                break;
            }
            if (evtCheckID(obj->eventId) != 1) {
                obj->unk1DC++;
                *(s32*)&obj->unk1A0[0x10] = 0x14;
            }
            break;
        }
        case 6:
            (*(s32*)&obj->unk1A0[0x10])--;
            if (*(s32*)&obj->unk1A0[0x10] & 2) {
                obj->flags |= 0x4000;
            } else {
                obj->flags &= ~0x4000;
            }
            if (*(s32*)&obj->unk1A0[0x10] == 0) {
                obj->unk1DC = 0x63;
            }
            break;
        case 10:
            if (windowCheckID(*(s32*)&obj->unk1A0[8]) == 0) {
                obj->flags &= ~0x8;
                marioKeyOn();
                partyKeyOn();
                marioStSystemLevel(0);
                obj->unk1DC = 0;
            }
            break;
        case 0x63:
            marioKeyOn();
            partyKeyOn();
            marioStSystemLevel(0);
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

s32 evt_mobj_lock(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 unk1A0 = evtGetValue(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    f32 f28 = evtGetFloat(event, args[4]);
    f32 f31 = evtGetFloat(event, args[5]);
    s32 unk1D4 = evtGetValue(event, args[6]);
    s32 unk1D8 = evtGetValue(event, args[7]);
    s32 unk1E4 = args[8];
    BOOL inBattle = gp->inBattle != 0;
    MapObjectEntry* obj;
    const char* asyncName = "MOBJ_Lock";

    if (evtGetValue(event, unk1E4) != 0) {
        return 2;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjHitEntry(obj, 2);
    *(s32*)obj->unk1A0 = unk1A0;
    mobjSetPosition(name, f30, f29, f28);
    obj->rotation.y = f31;
    obj->unk1D0 = mobj_lock;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = (s32*)unk1D8;
    obj->flags |= 0x26;
    obj->flags |= 0x40000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "Z_1", 1);
    return 2;
}

// NONMATCHING (87.23%): correct registers, but MWCC evaluates the asyncName `?:` before loading gp
// (retail interleaves the gp/args[6] loads between the two string materialisations).
s32 evt_mobj_recovery_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 type = evtGetValue(event, args[1]);
    f32 f31 = evtGetFloat(event, args[2]);
    f32 f30 = evtGetFloat(event, args[3]);
    f32 f29 = evtGetFloat(event, args[4]);
    s32 unk1E4;
    const char* asyncName;
    MapObjectEntry* obj;

    evtGetValue(event, args[5]);
    unk1E4 = args[6];
    asyncName = (type == 0) ? "MOBJ_RecoveryBlock" : "MOBJ_CoinTakenRecoveryBlock";
    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    *(s32*)((u8*)obj + 0x1A4) = type;
    mobjSetPosition(name, f31, f30, f29);
    obj->flags |= 4;
    obj->unk1D0 = mobj_recovery_blk;
    obj->unk1D4 = 0;
    obj->unk1D8 = 0;
    obj->flags |= 4;
    obj->flags |= 0x400000;
    obj->unk1E4 = unk1E4;
    if (type == 0) {
        obj->unk1DC = 0;
    } else {
        obj->unk1DC = -1;
    }
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    return 2;
}

// NONMATCHING (97.90%): 1 missing, 1 extra, plus register differences.
s32 evt_mobj_koopa_hidden_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 unk1A0 = evtGetValue(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[6]);
    s32 unk1E4 = args[7];
    const char* asyncName = "MOBJ_HiddenHatenaBlock";
    MapObjectEntry* obj;

    if (!animGroupBaseAsync(asyncName, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->flags |= 0x800;
    mobjHitEntry(obj, 1);
    obj->unk1D0 = mobj_koopa_badgeblk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    *(s32*)obj->unk1A0 = unk1A0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x5a;
        animPoseSetAnim(obj->poseId, "S_2", 1);
    }
    return 2;
}

// The original's `psndSFXOn_3D` argument is the literal 0x401E8; `animPoseGetMaterialEvtColor`
// returns a GXColor* in the header but retail treats r3 as the colour value itself, so the
// value is reinterpreted rather than dereferenced.
s32 mobj_breaking_floor(MapObjectEntry* obj) {
    GXColor color2;
    GXColor color;

    marioGetPtr();
    color = *(GXColor*)&dat_8041a130;
    color2 = *(GXColor*)&dat_8041a130;
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                psndSFXOn_3D((const char*)0x401E8, &obj->position);
                animPoseSetAnim(obj->poseId, "A_1", 1);
                if (obj->unk1D4 != NULL) {
                    mobjRunEvent(obj, obj->unk1D4);
                }
                if (obj->hitObj[0].hitObject != NULL) {
                    *(u16*)obj->hitObj[0].hitObject |= 1;
                }
                obj->flags |= 2;
                evtSetValue(NULL, obj->unk1E4, 1);
                obj->unk1DC = 1;
                animPoseSetMaterialFlagOn(obj->poseId, 0x40);
                animPoseSetMaterialEvtColor(obj->poseId, color);
                *(s32*)((u8*)obj + 0x1B0) = 0x40;
            }
            break;
        case 1: {

            // MUST_MATCH: the header declares animPoseGetMaterialEvtColor as returning GXColor*, but
            // retail treats the returned r3 as the colour value (it stores r3 and reads its high byte).
#ifdef MUST_MATCH
            *(u32*)&color2 = (u32)animPoseGetMaterialEvtColor(obj->poseId);
#else
            color2 = *animPoseGetMaterialEvtColor(obj->poseId);
#endif
            if (color2.a >= 4) {
                color2.a -= 4;
            } else {
                color2.a = 0;
                obj->unk1DC = 0x63;
            }
            animPoseSetMaterialEvtColor(obj->poseId, color2);
            *(s32*)((u8*)obj + 0x1B0) -= 1;
            if (*(s32*)((u8*)obj + 0x1B0) <= 0) {
                obj->unk1DC = 0x63;
            }
            break;
        }
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

s32 evt_mobj_koopa_sango(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f29 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f31 = evtGetFloat(event, args[3]);
    s32 unk1D4 = evtGetValue(event, args[4]);
    s32 unk1E4 = args[5];
    MapObjectEntry* obj;
    Vec pos;

    if (!animGroupBaseAsync(str_802C41D0, gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, str_802C41D0);
    obj = mobjNameToPtr(name);
    obj->flags |= 0x84;
    pos = vec3_802c40c4[0];
    pos.x = f29;
    pos.y = f30;
    pos.z = f31;
    obj->position = pos;
    obj->unk1D0 = mobj_koopa_sango_evt;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = 0;
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    return 2;
}

s32 evt_mobj_breaking_floor(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 type = evtGetValue(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    BOOL inBattle = gp->inBattle != 0;
    char animName[0x80];
    MapObjectEntry* obj;

    switch (type) {
        case 0:
            strcpy(animName, str_802C42E8);
            break;
        case 1:
            strcpy(animName, str_802C42FC);
            break;
        default:
            strcpy(animName, str_802C42E8);
            break;
    }
    if (!animGroupBaseAsync(animName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, animName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_breaking_floor;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x1000000;
    obj->unk1E4 = unk1E4;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    } else {
        obj->unk1DC = 0;
    }
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    return 2;
}

s32 evt_mobj_koopa_brick(EventEntry* event, BOOL isFirstCall) {
#ifdef MUST_MATCH
    u8* base = (u8*)vec3_802c3dc8;
#endif
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    s32 unk1A0 = evtGetValue(event, args[4]);
    s32 type = args[5];
    s32 unk1D4 = evtGetValue(event, args[6]);
    s32 unk1E4 = args[7];
    BOOL inBattle = gp->inBattle != 0;
    const char* asyncName;
    MapObjectEntry* obj;

    switch (type) {
        case 0:
#ifdef MUST_MATCH
            asyncName = (const char*)(base + 0x488);
#else
            asyncName = "MOBJ_FlowBrickBlock";
#endif
            break;
        case 1:
#ifdef MUST_MATCH
            asyncName = (const char*)(base + 0x49C);
#else
            asyncName = "MOBJ_block_ky";
#endif
            break;
        case 2:
#ifdef MUST_MATCH
            asyncName = (const char*)(base + 0x4AC);
#else
            asyncName = "MOBJ_bigblock_01";
#endif
            break;
        case 3:
        default:
#ifdef MUST_MATCH
            asyncName = (const char*)(base + 0x4C0);
#else
            asyncName = "MOBJ_bigblock_02";
#endif
            break;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_koopa_brickblk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    *(s32*)obj->unk1A0 = unk1A0;
    mobjCalcMtx(obj);
#ifdef MUST_MATCH
    animPoseSetAnim(obj->poseId, str_8041A12C, 1);
#else
    animPoseSetAnim(obj->poseId, "S_1", 1);
#endif
    if (evtGetValue(event, unk1E4) != 0) {
#ifdef MUST_MATCH
        animPoseSetAnim(obj->poseId, str_8041A12C, 1);
#else
        animPoseSetAnim(obj->poseId, "S_1", 1);
#endif
        obj->unk1DC = 0x63;
    }
    return 2;
}

// NONMATCHING (97.01%): missing unit .rodata base, so the asyncName base register and arg registers shift.
s32 evt_mobj_koopa_dokan(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 type = evtGetValue(event, args[0]);
    const char* name = (const char*)evtGetValue(event, args[1]);
    f32 f29 = evtGetFloat(event, args[2]);
    f32 f30 = evtGetFloat(event, args[3]);
    f32 f31 = evtGetFloat(event, args[4]);
    s32 unk1D4 = evtGetValue(event, args[5]);
    s32 unk1E4 = args[6];
    BOOL inBattle = gp->inBattle != 0;
    const char* asyncName;
    MapObjectEntry* obj;
    Vec pos;

    switch (type) {
        case 0:
            asyncName = "MOBJ_GreenPipe_ks";
            break;
        case 1:
            asyncName = "MOBJ_GreenPipe_kl";
            break;
        case 2:
            asyncName = "MOBJ_GreenPipe_ky";
            break;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    obj->flags |= 6;
#ifdef MUST_MATCH
    // The original addresses vec3_802c40b8 from the vec3_802c3dc8 base.
    pos = *(Vec*)((u8*)vec3_802c3dc8 + 0x2F0);
#else
    pos = vec3_802c40b8[0];
#endif
    pos.x = f29;
    pos.y = f30;
    pos.z = f31;
    obj->position = pos;
    obj->unk1D0 = mobj_koopa_dokan_evt;
    obj->unk1D4 = (s32*)unk1D4;
    obj->unk1D8 = 0;
    mobjCalcMtx(obj);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    obj->flags |= 0x3c000000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    if (evtGetValue(event, unk1E4) != 0) {
        obj->unk1DC = 0x63;
    }
    return 2;
}

// NONMATCHING (77.23%): missing unit .rodata base shifts the callee-saved register allocation.
s32 evt_mobj_kururing_floor(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f29 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f31 = evtGetFloat(event, args[3]);
    s32 r30 = evtGetValue(event, args[4]);
    s32 unk1E4 = args[6];
    MapObjectEntry* obj;
    Vec pos;
    Vec scale;

    evtGetValue(event, args[5]);
    if (!animGroupBaseAsync("MOBJ_KururinFloor", gp->inBattle != 0, NULL)) {
        return 0;
    }
    mobjEntry(name, "MOBJ_KururinFloor");
    obj = mobjNameToPtr(name);
    pos = vec3_802c4050[0];
    pos.x = f29;
    pos.y = f30 + 0.1f;
    pos.z = f31;
    obj->position = pos;
    obj->unk1D0 = mobj_kururing_floor;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    obj->flags |= 0x1010000;
    obj->flags |= 2;
    mobjCalcMtx(obj);
    scale = obj->scale2;
    scale.x = vec3_802c405c[0].x;
    scale.z = vec3_802c405c[0].z;
    obj->scale2 = scale;
    mobjCalcMtx2(obj);
    mapGrpFlagOn((const char*)r30, 0x400000);
    animPoseSetAnim(obj->poseId, "S_1", 1);
    animPoseWorldPositionEvalOn(obj->poseId);
    return 2;
}

// NONMATCHING (93.84%): item-field store scheduling differs and `kpa_powerup_evt` is `.sdata` but the extern is
// emitted lis/addi rather than SDA21.
s32 mobj_koopa_ojama_blk_evt(MapObjectEntry* obj) {
    s32 flag = 0;

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                switch (kpaGetLevel()) {
                    case 0:
                    case 1:
                        if (obj->unk1E0 == 0x20000000 || obj->unk1E0 == 0x4000000) {
                            flag = 1;
                        }
                        break;
                    case 2:
                        if (obj->unk1E0 == 0x20000000 || obj->unk1E0 == 0x10000000 ||
                            obj->unk1E0 == 0x4000000) {
                            flag = 1;
                        }
                        break;
                    case 3:
                        flag = 1;
                        break;
                }
                if (flag == 0) {
                    obj->flags &= ~8;
                } else {
                    mobjRunEvent(obj, obj->unk1D4);
                    evtSetValue(NULL, obj->unk1E4, 1);
                    if (obj->unk1E0 == 0x4000000) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        *(u16*)obj->hitObj[0].hitObject |= 1;
                    }
                    obj->flags |= 2;
                    kpaAddScore(kpaMutekiCheck() ? 500 : 400);
                    obj->unk1DC = 0xa;
                }
            }
            break;
        case 0xa: {
            ItemEntry* item;

            psndSFXOn_3D((const char*)0x1C4, &obj->position);
            effMObjBrokenEntry(5, obj->position.x, float_25_8041a140 + obj->position.y,
                               float_25_8041a140 + obj->position.z);
            if ((item = itemEntry(obj->name, (ItemType)0x59, 4, -1, (s32)kpa_powerup_evt,
                                  obj->position.x, float_25_8041a140 + obj->position.y,
                                  obj->position.z)) != NULL) {
                itemStatusOn(item, 0x10);
                itemFlagOn(item, 0x14);
                *(s32*)((u8*)item + 0x7C) = 0x2710;
                *(s32*)((u8*)item + 0x78) = 0;
                *(f32*)((u8*)item + 0x80) = float_0p45_8041a1c4;
                *(f32*)((u8*)item + 0x84) = float_0p7_8041a1c8;
                *(f32*)((u8*)item + 0x58) = float_500_8041a1cc;
                *(f32*)((u8*)item + 0x50) = float_120_8041a1d0;
                *(s32*)((u8*)item + 0x54) = 0x5a;
                *(s32*)((u8*)item + 0x88) = 0;
                psndSFXOn_3D((const char*)0x836, &obj->position);
            }
            obj->unk1DC = 0x63;
            break;
        }
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

s32 mobj_powerupblk(MapObjectEntry* obj) {
#ifdef MUST_MATCH
    // Match only: the five strings are addressed as `base + offset` to reach the original's .rodata layout.
    u8* base = (u8*)vec3_802c3dc8;
#define PB_DOU02 ((const char*)(base + 0x5EC))
#define PB_DOU05 ((const char*)(base + 0x5F4))
#define PB_DOU10 ((const char*)(base + 0x5FC))
#define PB_BLK1 ((const char*)(base + 0x604))
#define PB_BLK2 ((const char*)(base + 0x60C))
#else
#define PB_DOU02 "dou_02"
#define PB_DOU05 "dou_05"
#define PB_DOU10 "dou_10"
#define PB_BLK1 "pwrblk1"
#define PB_BLK2 "pwrblk2"
#endif

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                if (marioKeyOffChk() != 0 || marioCtrlOffChk() != 0) {
                    obj->flags &= ~8;
                } else {
                    marioKeyOff();
                    if (strcmp(gp->currentMapName, PB_DOU02) == 0) {
                        marioCtrlOff();
                    } else if (strcmp(gp->currentMapName, PB_DOU05) == 0 ||
                               strcmp(gp->currentMapName, PB_DOU10) == 0) {
                        if (obj->unk1E0 == 0x800000) {
                            marioCtrlOff();
                        }
                    }
                    gp->systemLevelFlags |= 3;
                    psndSFXOn_3D((const char*)0x201C0, &obj->position);
                    animPoseSetAnim(obj->poseId, "A_1", 1);
                    itemEntry(PB_BLK1, (ItemType)0x57, 0xf, -1, 0, obj->position.x,
                              obj->position.y, obj->position.z);
                    obj->unk1DC++;
                }
            }
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "A_2", 1);
            obj->flags &= ~8;
            *(s32*)((u8*)obj + 0x1A4) = 1;
            obj->unk1DC++;
            break;
        case 2:
            if (*(s32*)((u8*)obj + 0x1A4) != 0) {
                (*(s32*)((u8*)obj + 0x1A4))--;
            } else {
                itemForceGet(itemEntry(PB_BLK2, (ItemType)0x57, 0x11, -1, 0,
                                       obj->position.x, float_40_8041a1ec + obj->position.y,
                                       obj->position.z));
                evtSetValue(NULL, obj->unk1E4, 1);
                marioKeyOn();
                if (strcmp(gp->currentMapName, PB_DOU02) == 0) {
                    marioCtrlOn();
                } else if (strcmp(gp->currentMapName, PB_DOU05) == 0 ||
                           strcmp(gp->currentMapName, PB_DOU10) == 0) {
                    if (obj->unk1E0 == 0x800000) {
                        marioCtrlOn();
                    }
                }
                obj->unk1DC = 0x5a;
            }
            break;
        case 0x5a:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}
#undef PB_DOU02
#undef PB_DOU05
#undef PB_DOU10
#undef PB_BLK1
#undef PB_BLK2

s32 mobj_10countblk(MapObjectEntry* obj) {
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                if (obj->flags & 0x800) {
                    obj->flags &= ~0x800;
                    strcpy(obj->animName, "MOBJ_Block");
                }
                animPoseSetAnim(obj->poseId, "A_1", 1);
                psndSFXOn_3D((const char*)0x1BE, &obj->position);
                evtSetValue(NULL, obj->unk1E4, 1);
                if (obj->unk1D4 != NULL) {
                    mobjRunEvent(obj, obj->unk1D4);
                } else {
                    pouchAddCoin(1);
                    itemEntry(NULL, (ItemType)0x79, 0xf, -1, 0, obj->position.x, obj->position.y,
                              obj->position.z);
                    *(s32*)((u8*)obj + 0x1B0) = 0xC8;
                }
                obj->unk1DC++;
                obj->flags &= ~8;
            }
            break;
        case 1:
            if (*(s32*)((u8*)obj + 0x1B0) > 0) {
                (*(s32*)((u8*)obj + 0x1B0))--;
            }
            if (*(s32*)((u8*)obj + 0x1B0) <= 0) {
                obj->unk1DC = 0x5a;
                animPoseSetAnim(obj->poseId, "S_2", 1);
                strcpy(obj->animName, "MOBJ_Block");
            } else if (obj->flags & 8) {
                psndSFXOn_3D((const char*)0x1BF, &obj->position);
                pouchAddCoin(1);
                itemEntry(NULL, (ItemType)0x79, 0xf, -1, 0, obj->position.x, obj->position.y,
                          obj->position.z);
                obj->flags &= ~8;
                animPoseSetAnim(obj->poseId, "A_1", 1);
            }
            break;
        case 0x5a:
            if (obj->flags & 8) {
                psndSFXOn_3D((const char*)0x1B7, &obj->position);
                animPoseSetAnim(obj->poseId, "A_3", 1);
                obj->flags &= ~8;
                obj->unk1DC++;
            }
            break;
        case 0x5b:
            if (!(obj->flags & 8)) {
                obj->unk1DC = 0x5a;
            } else {
                if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                    break;
                }
                animPoseSetAnim(obj->poseId, "S_2", 1);
                obj->unk1DC = 0x5a;
                obj->flags &= ~8;
            }
            break;
    }
    return 0;
}

// NONMATCHING (93.76%): `kpa_powerup_evt` is `.sdata` but extern emits lis/addi, plus item-field f32 load/store
// scheduling.
s32 mobj_koopa_badgeblk(MapObjectEntry* obj) {
    BOOL flag = FALSE;

    marioGetPtr();
    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                if (obj->unk1E0 != 0x4000000) {
                    obj->flags &= ~8;
                } else {
                    if (obj->flags & 0x800) {
                        mobjHitEntry(obj, 0);
                    }
                    animPoseSetAnim(obj->poseId, "A_1", 1);
                    if (obj->unk1D4 != NULL) {
                        mobjRunEvent(obj, obj->unk1D4);
                    } else {
                        if (*(s32*)((u8*)obj + 0x1A0) == 0x59) {
                            ItemEntry* item;
                            itemEntry(obj->name, (ItemType)(*(s32*)((u8*)obj + 0x1A0)), 4, -1,
                                      (s32)kpa_powerup_evt, obj->position.x,
                                      float_12p5_8041a178 + obj->position.y, obj->position.z);
                            item = itemNameToPtr(obj->name);
                            itemStatusOn(item, 0x10);
                            itemFlagOn(item, 0x14);
                            *(s32*)((u8*)item + 0x7C) = 0x2710;
                            *(s32*)((u8*)item + 0x78) = 0;
                            *(f32*)((u8*)item + 0x80) = float_0p45_8041a1c4;
                            *(f32*)((u8*)item + 0x84) = float_0p7_8041a1c8;
                            *(s32*)((u8*)item + 0x88) = 0;
                            *(f32*)((u8*)item + 0x58) = float_500_8041a1cc;
                            *(f32*)((u8*)item + 0x50) = float_120_8041a1d0;
                            *(s32*)((u8*)item + 0x54) = 0x5a;
                            psndSFXOn_3D((const char*)0x836, &obj->position);
                        } else {
                            kpaAddCoin(1);
                            itemEntry(NULL, (ItemType)0x79, 0xf, -1, 0, obj->position.x, obj->position.y,
                                      obj->position.z);
                            evtSetValue(NULL, obj->unk1E4, 1);
                            psndSFXOn_3D((const char*)0x5B, &obj->position);
                        }
                    }
                    if (obj->unk1E0 == 0x4000000) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    obj->unk1DC++;
                }
            }
            break;
        case 1:
            if (animPoseGetLoopTimes(obj->poseId) < 1.0f) {
                break;
            }
            animPoseSetAnim(obj->poseId, "S_2", 1);
            obj->flags &= ~8;
            obj->unk1DC = 0x5a;
            strcpy(obj->animName, "MOBJ_Block");
            break;
        case 0x5a:
            if (obj->flags & 8) {
                switch (kpaGetLevel()) {
                    case 3:
                        flag = TRUE;
                        break;
                }
                if (!flag) {
                    obj->flags &= ~8;
                } else {
                    if (obj->unk1E0 == 0x4000000) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        obj->hitObj[0].hitObject->flags |= 1;
                    }
                    obj->flags |= 2;
                    obj->unk1DC++;
                }
            }
            break;
        case 0x5b:
            psndSFXOn_3D((const char*)0x1B8, &obj->position);
            effMObjBrokenEntry(2, obj->position.x, float_12p5_8041a178 + obj->position.y,
                               float_12p5_8041a178 + obj->position.z);
            obj->unk1DC = 0x63;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

s32 mobj_koopa_dokan_evt(MapObjectEntry* obj) {
    BOOL flag = FALSE;

    switch (obj->unk1DC) {
        case 0:
            if (obj->flags & 8) {
                switch (kpaGetLevel()) {
                    case 0:
                        break;
                    case 1:
                    case 2:
                        if (obj->unk1E0 == 0x20000000) {
                            flag = TRUE;
                        }
                        break;
                    case 3:
                        flag = TRUE;
                        break;
                }
                if (!flag) {
                    obj->flags &= ~8;
                } else {
                    mobjRunEvent(obj, obj->unk1D4);
                    evtSetValue(NULL, obj->unk1E4, 1);
                    if (obj->unk1E0 == 0x4000000) {
                        npcKoopaModeMobjBoundDeadCheck(obj->hitObj[0].hitObject);
                    }
                    if (obj->hitObj[0].hitObject != NULL) {
                        obj->hitObj[0].hitObject->flags |= 1;
                    }
                    obj->flags |= 2;
                    kpaAddScore(kpaMutekiCheck() ? 500 : 400);
                    obj->unk1DC = 0xa;
                }
            }
            break;
        case 0xa:
            if (strcmp(obj->animName, str_802C41DC) == 0) {
                effMObjBrokenEntry(6, obj->position.x, float_25_8041a140 + obj->position.y,
                                   float_25_8041a140 + obj->position.z);
            } else if (strcmp(obj->animName, str_802C41F0) == 0) {
                effMObjBrokenEntry(6, obj->position.x,
                                   float_7p5_8041a1bc + (float_25_8041a140 + obj->position.y),
                                   float_25_8041a140 + obj->position.z);
            } else {
                effMObjBrokenEntry(6, obj->position.x - float_25_8041a140,
                                   float_25_8041a140 + obj->position.y,
                                   float_25_8041a140 + obj->position.z);
            }
            psndSFXOn_3D((const char*)0x834, &obj->position);
            obj->unk1DC = 0x63;
            break;
        case 0x63:
            mobjDelete(obj->name);
            break;
    }
    return 0;
}

// NONMATCHING (96.77%): unit .rodata is not defined locally, so each pooled anim-name gets its own
// lis/addi instead of the retail vec3_802c3dc8 base register (costs one callee-saved register).
s32 evt_mobj_switch_float_blk(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    f32 f31 = evtGetFloat(event, args[1]);
    f32 f30 = evtGetFloat(event, args[2]);
    f32 f29 = evtGetFloat(event, args[3]);
    const char* childName = (const char*)evtGetValue(event, args[4]);
    s32 type = args[5];
    s32 unk1D4 = evtGetValue(event, args[6]);
    s32 unk1E4 = args[7];
    BOOL inBattle = gp->inBattle != 0;
    const char* asyncName;
    MapObjectEntry* obj;

    switch (type) {
        case 0:
            asyncName = "MOBJ_green_FlowBlockSwitch";
            break;
        case 1:
            asyncName = "MOBJ_purple_FlowBlockSwitch";
            break;
        case 2:
            asyncName = "MOBJ_orange_FlowBlockSwitch";
            break;
        case 3:
            asyncName = "MOBJ_red_FlowBlockSwitch";
            break;
        default:
            asyncName = "MOBJ_green_FlowBlockSwitch";
            break;
    }
    if (!animGroupBaseAsync(asyncName, inBattle, NULL)) {
        return 0;
    }
    mobjEntry(name, asyncName);
    obj = mobjNameToPtr(name);
    mobjSetPosition(name, f31, f30, f29);
    obj->unk1D0 = mobj_switch_float_blk;
    obj->unk1D4 = (s32*)unk1D4;
    obj->flags |= 4;
    obj->flags |= 0xe10000;
    obj->unk1E4 = unk1E4;
    obj->unk1DC = 0;
    *(s32*)((u8*)obj + 0x1B0) = 0;
    mobjCalcMtx(obj);
    strcpy((char*)((u8*)obj + 0x1B8), childName);
    if (*(char*)((u8*)obj + 0x1B8)) {
        f32 hitNZ, hitNY, hitNX, distance, hitZ, hitY, hitX;
        MapObjectEntry* obj2 = mobjNameToPtr((char*)((u8*)obj + 0x1B8));
        HitEntry* hit;
        strcpy((char*)((u8*)obj2 + 0x1B8), obj->name);
        distance = float_1_8041a160 + (f32)__fabs(*(f32*)((u8*)obj2 + 0x60));
        hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                             *(f32*)((u8*)obj2 + 0x38), float_0p5_8041a15c + *(f32*)((u8*)obj2 + 0x3c),
                             *(f32*)((u8*)obj2 + 0x40), float_0_8041a150, float_neg1_8041a214,
                             float_0_8041a150);
        if (hit != NULL) {
            animPoseSetAnim(obj->poseId, "S_1", 1);
        } else {
            animPoseSetAnim(obj->poseId, "A_3", 1);
        }
    }
    return 2;
}
