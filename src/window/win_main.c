#include "win/win_main.h"
#include "data/item_data.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/seqdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "driver/msgdrv.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "memory.h"
#include "party.h"
#include "system.h"
#include <dolphin/pad.h>
#include <demo/DEMOPad.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

extern ItemData itemDataTable[];
extern int sprintf(char* str, const char* format, ...);
extern int vsprintf(char* str, const char* format, va_list ap);

//.data
EVT_BEGIN(evt_diary_read)
EVT_END()

//.sbss
static WindowWork* wp;
TEXPalette* winTexTpl;

// local prototypes
void winDisp(CameraId camId, void* param);
void winDispKoopa(CameraId camId, void* param);
void winLectureKeyMask(void);

// functions defined in this unit but not declared in any header
extern void winRootDisp(CameraId camId, WindowWork* work);
extern void allPartyForceSlitOff(void);
extern void statusWinForceOff(void);
extern s32 unk_80175e98(void);
extern s32 unk_80175eb0(EventEntry* event, BOOL isFirstCall);
extern s32 unk_80177cf8(void);
extern s32 itemUseFunc2(EventEntry* event, BOOL isFirstCall);
extern s32 winGhostDiaryChk(void);
extern s32 party_color(EventEntry* event, BOOL isFirstCall);
extern s32 famicom_check(EventEntry* event, BOOL isFirstCall);
extern void statusWinForceOpen(void);
extern void* effRecoveryEntry(s32 type, f32 x, f32 y, f32 z);

// mario_pouch.h declares the pouch accessors with s16 parameters, but the
// original win_main.c treated them as s32 (the callees sign-extend their own
// s16 returns). Call through the wider signature so MWCC does not insert
// spurious extsh conversions.
#define POUCH_GETHP() (*(s32(*)(void))pouchGetHP)()
#define POUCH_SETHP(v) (*(void(*)(s32))pouchSetHP)(v)
#define POUCH_GETFP() (*(s32(*)(void))pouchGetFP)()
#define POUCH_SETFP(v) (*(void(*)(s32))pouchSetFP)(v)
#define POUCH_GETAP() (*(s32(*)(void))pouchGetAP)()
#define POUCH_SETAP(v) (*(void(*)(s32))pouchSetAP)(v)
#define POUCH_GETPARTYHP(id) (*(s32(*)(s32))pouchGetPartyHP)(id)
#define POUCH_SETPARTYHP(id, v) (*(void(*)(s32, s32))pouchSetPartyHP)(id, v)

// WindowWork.currentEvent is actually at 0x18; the header wrongly places it
// at 0x3C (0x14 is a s32 counter used by winLecture*). Use a raw offset.
#define WP_CURRENT_EVENT(w) (*(EventEntry**)((u8*)(w) + 0x18))

WindowWork* winGetPtr(void) {
    return wp;
}

void winInit(void) {
    wp = __memAlloc(HEAP_DEFAULT, sizeof(WindowWork));
    memset(wp, 0, sizeof(WindowWork));
}

void winReInit(void) {
    memset(wp, 0, sizeof(WindowWork));
}

BOOL winCheck(void) {
    s32 state;

    if (wp == NULL) {
        return FALSE;
    }

    if (wp->flags & 0x1000) {
        return FALSE;
    }

    state = wp->menuState;
    if (state != 0 && state < 200) {
        return TRUE;
    }
    return FALSE;
}

void winOpenEnable(void) {
    if (wp) {
        wp->flags &= ~1;
    }
}

void winOpenDisable(void) {
    if (wp) {
        wp->flags |= 1;
    }
}

void winMain(void) {
    MarioWork* mario;

    mario = marioGetPtr();
    if (!(wp->flags & 1) && seqGetSeq() == SEQ_GAME && !seqCheckSeq()) {
        wp->buttonTrg = keyGetButtonTrg(0);
        wp->buttonRep = keyGetButtonRep(0);
        wp->dirTrg = keyGetDirTrg(0);
        wp->dirRep = keyGetDirRep(0);

        if (keyGetButtonTrg(0) & PAD_BUTTON_LEFT) {
            wp->dirTrg |= DEMO_STICK_LEFT;
        }
        if (keyGetButtonTrg(0) & PAD_BUTTON_RIGHT) {
            wp->dirTrg |= DEMO_STICK_RIGHT;
        }
        if (keyGetButtonTrg(0) & PAD_BUTTON_UP) {
            wp->dirTrg |= DEMO_STICK_UP;
        }
        if (keyGetButtonTrg(0) & PAD_BUTTON_DOWN) {
            wp->dirTrg |= DEMO_STICK_DOWN;
        }

        if (keyGetButtonRep(0) & PAD_BUTTON_LEFT) {
            wp->dirRep |= DEMO_STICK_LEFT;
        }
        if (keyGetButtonRep(0) & PAD_BUTTON_RIGHT) {
            wp->dirRep |= DEMO_STICK_RIGHT;
        }
        if (keyGetButtonRep(0) & PAD_BUTTON_UP) {
            wp->dirRep |= DEMO_STICK_UP;
        }
        if (keyGetButtonRep(0) & PAD_BUTTON_DOWN) {
            wp->dirRep |= DEMO_STICK_DOWN;
        }

        winLectureKeyMask();

        switch (wp->menuState) {
            case 0:
                break;

			case 201:
                if (!evtCheckID(wp->currentEvent->eventId)) {
                    wp->menuState = 3;
                }
                break;

            case 300:
                wp->currentEvent = evtEntryType(&evt_diary_read, 0, 0, 0);
                wp->menuState += 1;
                break;

            case 301:
                if (!evtCheckID(wp->currentEvent->eventId)) {
                    wp->menuState = 3;
                }
                break;

            case 400:
                evtYuuWindow();
                wp->menuState += 1;
                break;

            case 401:
                if (evtYuuWindowEndCheck()) {
                    wp->menuState = 3;
                }
                break;
        }

        if (wp->menuState == 2) {
            s8 characterId = marioGetPtr()->characterId;
            if (characterId < 1) {
                if (characterId < 0) {
                    return;
                }
                if (!(wp->flags & 0x1000)) {
                    dispEntry(CAMERA_2D, 250.0f, 0, winDisp, NULL);
                } else {
                    dispEntry(CAMERA_2D, 500.0f, 0, winDisp, NULL);
                }
                return;
            }
            if (characterId < 3) {
                dispEntry(CAMERA_2D, 250.0f, 0, winDispKoopa, NULL);
            }
        }
    }
}

void winDisp(CameraId camId, void* param) {
    winRootDisp(camId, wp);
}

extern s32 evt_lecture_msg[];

// Font drawing helpers defined in manager/fontmgr.c but not declared in a header
void FontDrawColor(GXColor* color);
void FontDrawMessageMtx(Mtx mtx, const char* msg);
void FontDrawStringMtx(Mtx mtx, const char* msg);
void FontDrawStringVecPitch(Vec* pos, const char* text, f32 pitch);
void FontDrawScaleVec(Vec scale);
u16 FontGetMessageWidth(const char* msg);
void FontDrawEdge(void);
void FontDrawEdgeOff(void);

void winFontSetLabel(Vec* pos, Vec* scale, GXColor* color, const char* tag) {
    Mtx mtx1;
    Mtx mtx2;
    GXColor c;

    if (tag != NULL) {
        PSMTXTrans(mtx1, pos->x, pos->y, pos->z);
        PSMTXScale(mtx2, scale->x, scale->y, scale->z);
        PSMTXConcat(mtx1, mtx2, mtx1);
        c = *color;
        FontDrawColor(&c);
        FontDrawMessageMtx(mtx1, msgSearch(tag));
    }
}

void winFontSetPitch(Vec* pos, Vec* scale, GXColor* color, f32 pitch, const char* format, ...) {
    char buf[0x400];
    GXColor c;
    va_list args;
    Vec scaleCopy;

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
    c = *color;
    FontDrawColor(&c);
    scaleCopy = *scale;
    FontDrawScaleVec(scaleCopy);
    FontDrawStringVecPitch(pos, buf, pitch);
}

void winFontSetEdge(Vec* pos, Vec* scale, GXColor* color, const char* format, ...) {
    char buf[0x400];
    Mtx mtx1;
    Mtx mtx2;
    GXColor c;
    va_list args;

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
    PSMTXTrans(mtx1, pos->x, pos->y, pos->z);
    PSMTXScale(mtx2, scale->x, scale->y, scale->z);
    PSMTXConcat(mtx1, mtx2, mtx1);
    c = *color;
    FontDrawColor(&c);
    FontDrawEdge();
    FontDrawStringMtx(mtx1, buf);
    FontDrawEdgeOff();
}

void winFontSetR(Vec* pos, Vec* scale, GXColor* color, const char* format, ...) {
    char buf[0x400];
    Mtx mtx1;
    Mtx mtx2;
    GXColor c;
    s32 diff;
    va_list args;

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
    diff = FontGetMessageWidth("\202X\202X\202X") - FontGetMessageWidth(buf);
    PSMTXTrans(mtx1, pos->x + scale->x * (f32)diff, pos->y, pos->z);
    PSMTXScale(mtx2, scale->x, scale->y, scale->z);
    PSMTXConcat(mtx1, mtx2, mtx1);
    c = *color;
    FontDrawColor(&c);
    FontDrawStringMtx(mtx1, buf);
}

void winFontSet(Vec* pos, Vec* scale, GXColor* color, const char* format, ...) {
    char buf[0x400];
    Mtx mtx1;
    Mtx mtx2;
    GXColor c;
    va_list args;

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
    PSMTXTrans(mtx1, pos->x, pos->y, pos->z);
    PSMTXScale(mtx2, scale->x, scale->y, scale->z);
    PSMTXConcat(mtx1, mtx2, mtx1);
    c = *color;
    FontDrawColor(&c);
    FontDrawMessageMtx(mtx1, buf);
}

s32 winLectureCheck(void) {
    if ((wp->flags & 0x1000) && evtCheckID(WP_CURRENT_EVENT(wp)->eventId)) {
        return 1;
    }
    wp->flags &= ~0x1000;
    return 0;
}

void winLectureOn(void) {
    wp->flags |= 0x1000;
    wp->flags |= 0x800;
    *(s32*)wp->field_0x14 = 0;
    WP_CURRENT_EVENT(wp) = evtEntry(evt_lecture_msg, 0, 0);
}

s32 unk_80175e98(void) {
    wp->flags &= ~0x800;
    return 2;
}

s32 unk_80175eb0(EventEntry* event, BOOL isFirstCall) {
    s32 value = evtGetValue(event, event->args[0]);

    if (isFirstCall) {
        wp->flags &= ~0x800;
    }
    if (*(s32*)wp->field_0x14 < value) {
        return 0;
    }
    wp->flags |= 0x800;
    return 2;
}

s32 unk_80177cf8(void) {
    allPartyForceSlitOff();
    return 2;
}

USER_FUNC(itemUseFunc2) {
    statusWinForceOff();
    return EVT_RETURN_DONE;
}

void winFontInit(void) {
    FontDrawStart();
}

s32 winGhostDiaryChk(void) {
    if (wp->menuState == 301) {
        return 1;
    }
    return seqGetSeq() == SEQ_GAME_OVER;
}

s32 cam_r(void) {
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec v;
    f32 s;

    cam->rollAngle += 3.0f;
    s = sin((f32)gp->retraceCount / 10.0f);
    PSVECSubtract(&cam->cameraAt, &cam->cameraPos, &v);
    PSVECScale(&v, &v, 0.2f * s);
    cam->viewOffset.x = v.x;
    cam->viewOffset.y = v.y;
    cam->viewOffset.z = v.z;
    return 0;
}

USER_FUNC(party_color) {
    s32* args = event->args;
    s32 color = pouchGetPartyColor(PARTNER_YOSHI);

    evtSetValue(event, args[0], color);
    return EVT_RETURN_DONE;
}

USER_FUNC(famicom_check) {
    s32* args = event->args;

    if (wp->flags & 0x2000) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

void winDispKoopa(CameraId camId, void* param) {
    winTexTpl = **(TEXPalette***)(*(u8**)((u8*)wp + 0x28) + 0xA0);
    GXSetCullMode(GX_CULL_NONE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTexGens(1u);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    winTexSet(0xB7, (Vec){ 0.0f, 50.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
}

USER_FUNC(itemUseFunc) {
    static s32 cnt;
    PartyEntry* party;
    MarioWork* mario;
    ItemData* item;

    item = &itemDataTable[event->lwData[10]];
    mario = marioGetPtr();
    party = partyGetPtr(0);
    if (isFirstCall) {
        cnt = 0;
    }

    switch (cnt) {
        case 0:
            statusWinForceOpen();
            if (event->lwData[11] == 0) {
                POUCH_SETHP(POUCH_GETHP() + (u8)item->hp_restored);
                POUCH_SETFP(POUCH_GETFP() + (u8)item->fp_restored);
                POUCH_SETAP(POUCH_GETAP() + (u8)item->sp_restored);
            } else {
                POUCH_SETPARTYHP(event->lwData[11], POUCH_GETPARTYHP(event->lwData[11]) + (u8)item->hp_restored);
                POUCH_SETFP(POUCH_GETFP() + (u8)item->fp_restored);
                POUCH_SETAP(POUCH_GETAP() + (u8)item->sp_restored);
            }
            cnt++;
            break;

        case 1:
            if (event->lwData[11] != 0) {
                cnt = 0xC8;
                break;
            }
            if ((u8)item->hp_restored == 0) {
                cnt = 0x64;
                break;
            }
            effRecoveryEntry(0, mario->position.x, mario->position.y + mario->wPlayerCollisionBox.y, mario->position.z);
            cnt++;
            break;

        case 0x1F:
            cnt++;
            break;

        case 0x3C:
            cnt = 0x64;
            break;

        case 0x64:
            if ((u8)item->fp_restored == 0) {
                cnt = 0x3E7;
                break;
            }
            effRecoveryEntry(1, mario->position.x, mario->position.y + mario->wPlayerCollisionBox.y, mario->position.z);
            /* fallthrough */
        case 0x82:
            cnt = 0x3E7;
            break;

        case 0xC8: {
            f32* target;
            if ((u8)item->hp_restored == 0) {
                cnt = 0x12C;
                break;
            }
            target = (f32*)(*(u8**)((u8*)effRecoveryEntry(0, party->position.x, party->position.y + *(f32*)((u8*)party + 0xF0), party->position.z) + 0xC) + 0x40);
            *target = (-(*(f32*)((u8*)party + 0xF0))) / 2.0f;
            cnt++;
            break;
        }

        case 0xE6:
            cnt++;
            break;

        case 0x104:
            cnt = 0x12C;
            break;

        case 0x12C:
            if ((u8)item->fp_restored == 0) {
                cnt = 0x3E7;
                break;
            }
            effRecoveryEntry(1, party->position.x, party->position.y + *(f32*)((u8*)party + 0xF0), party->position.z);
            cnt++;
            break;

        case 0x14A:
            cnt = 0x3E7;
            break;

        case 0x3E7:
        case 0x3E8:
            return 2;

        default:
            cnt++;
            break;
    }
    return 0;
}

void winIconInit(void) {
    winTexTpl = NULL;
    GXSetCullMode(GX_CULL_NONE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTexGens(1u);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
}

void winIconSet(s32 iconId, Vec* translate, Vec* scale, GXColor* color) {
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    f32 height;

    if (iconId == 0x115) {
        iconId = 0x114;
    }
    (*(void(*)(GXTexObj*, u16))iconGetTexObj)(&texObj, iconId);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);

    if (color->a != 0xFF) {
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_TRUE);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    } else {
        GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_FALSE);
        GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    }

    GXSetTevColor(GX_TEVREG0, *color);
    PSMTXTrans(mtx1, translate->x, translate->y, translate->z);
    height = scale->y * (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx2, scale->x * (f32)GXGetTexObjWidth(&texObj), height, scale->z);
    PSMTXConcat(mtx1, mtx2, mtx3);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx3, mtx3);
    GXLoadPosMtxImm(mtx3, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(-0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXEnd();
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
}

void winIconGrayInit(void) {
    winTexTpl = NULL;
    GXSetCullMode(GX_CULL_NONE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C, GX_FALSE, 0x7D);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetTevColor(GX_TEVREG1, (GXColor){ 0xFF, 0x55, 0x55, 0x55 });
    GXSetNumTevStages(4);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C1, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP1);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C1, GX_CC_TEXC, GX_CC_CPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP2);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C1, GX_CC_TEXC, GX_CC_CPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP3);
    GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
}

void winTexInit(TEXPalette* texture) {
    winTexTpl = texture;
    GXSetCullMode(GX_CULL_NONE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTexGens(1u);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
}

void winTexSet(u32 textureId, Vec translate, Vec scale, GXColor color) {
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    f32 height;

    if (color.a != 0xFF) {
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_TRUE);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    } else {
        GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_FALSE);
        GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    }

    TEXGetGXTexObjFromPalette(winTexTpl, &texObj, textureId);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetTevColor(GX_TEVREG0, color);
    PSMTXTrans(mtx1, translate.x, translate.y, translate.z);
    height = scale.y * (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx2, scale.x * (f32)GXGetTexObjWidth(&texObj), height, scale.z);
    PSMTXConcat(mtx1, mtx2, mtx3);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx3, mtx3);
    GXLoadPosMtxImm(mtx3, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(-0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXEnd();
}

void winTexSet_x2(u32 textureId, u32 textureId2, Vec translate, Vec scale, GXColor color) {
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    f32 height;

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    TEXGetGXTexObjFromPalette(winTexTpl, &texObj, textureId);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    TEXGetGXTexObjFromPalette(winTexTpl, &texObj, textureId2);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP1);
    GXSetTevColor(GX_TEVREG0, color);
    PSMTXTrans(mtx1, translate.x, translate.y, translate.z);
    height = scale.y * (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx2, scale.x * (f32)GXGetTexObjWidth(&texObj), height, scale.z);
    PSMTXConcat(mtx1, mtx2, mtx3);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx3, mtx3);
    GXLoadPosMtxImm(mtx3, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(-0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXEnd();
}

void winTexInit_x2(TEXPalette* texture) {
    winTexTpl = texture;
    GXSetCullMode(GX_CULL_NONE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTexGens(1u);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetNumTexGens(2u);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
    GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
    GXSetNumTevStages(2);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
}

void winTexSetRot(u32 textureId, Vec translate, Vec scale, f32 rot, GXColor color) {
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    Mtx mtx4;
    f32 height;

    if (color.a != 0xFF) {
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_TRUE);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    } else {
        GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_FALSE);
        GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    }

    TEXGetGXTexObjFromPalette(winTexTpl, &texObj, textureId);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetTevColor(GX_TEVREG0, color);
    PSMTXTrans(mtx1, translate.x, translate.y, translate.z);
    PSMTXRotRad(mtx3, 'z', MTXDegToRad(rot));
    height = scale.y * (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx2, scale.x * (f32)GXGetTexObjWidth(&texObj), height, scale.z);
    PSMTXConcat(mtx1, mtx3, mtx4);
    PSMTXConcat(mtx4, mtx2, mtx4);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx4, mtx4);
    GXLoadPosMtxImm(mtx4, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(-0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXEnd();
}

void unk_80177008(u32 textureId, Vec translate, Vec scale, GXColor color) {
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    f32 height;

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    TEXGetGXTexObjFromPalette(winTexTpl, &texObj, textureId);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetTevColor(GX_TEVREG0, color);
    PSMTXTrans(mtx1, translate.x, translate.y, translate.z);
    height = scale.y * (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx2, scale.x * (f32)GXGetTexObjWidth(&texObj), height, scale.z);
    PSMTXConcat(mtx1, mtx2, mtx3);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx3, mtx3);
    GXLoadPosMtxImm(mtx3, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(-0.5f, 0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXEnd();
}

void winLectureKeyMask(void) {
    if (!(wp->flags & 0x1000)) {
        return;
    }
    if (wp->flags & 0x800) {
        wp->buttonTrg = 0;
        wp->buttonRep = 0;
        wp->dirTrg = 0;
        wp->dirRep = 0;
        return;
    }
    switch (*(s32*)wp->field_0x14) {
        case 0:
        case 1:
            wp->buttonTrg = 0;
            wp->buttonRep = 0;
            wp->dirTrg = 0;
            wp->dirRep = 0;
            if (keyGetButtonTrg(0) & PAD_BUTTON_A) {
                (*(s32*)wp->field_0x14)++;
            }
            break;

        case 2:
        case 3:
        case 4:
            wp->buttonTrg = 0;
            wp->buttonRep = 0;
            wp->dirTrg &= DEMO_STICK_RIGHT;
            wp->dirRep &= DEMO_STICK_RIGHT;
            if ((s32)wp->dirTrg != 0 || (s32)wp->dirRep != 0) {
                (*(s32*)wp->field_0x14)++;
            }
            break;

        case 5:
        case 6:
        case 7:
        case 8:
            wp->buttonTrg &= PAD_BUTTON_A;
            wp->buttonRep &= PAD_BUTTON_A;
            wp->dirTrg = 0;
            wp->dirRep = 0;
            if ((s32)wp->buttonTrg != 0 || (s32)wp->buttonRep != 0) {
                (*(s32*)wp->field_0x14)++;
            }
            break;

        case 9:
            wp->buttonTrg &= PAD_BUTTON_START;
            wp->buttonRep &= PAD_BUTTON_START;
            wp->dirTrg = 0;
            wp->dirRep = 0;
            if ((s32)wp->buttonTrg != 0 || (s32)wp->buttonRep != 0) {
                (*(s32*)wp->field_0x14)++;
            }
            break;
    }
}
