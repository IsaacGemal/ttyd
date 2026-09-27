#include <stdio.h>
#include <string.h>

#include <dolphin/types.h>
#include <dolphin/mtx.h>

#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/hitdrv.h"
#include "driver/imgdrv.h"
#include "driver/mapdrv.h"
#include "driver/offscreendrv.h"
#include "evt/evt_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "party.h"
#include "party/party_cloud.h"
#include "pmario_sound.h"
#include "evt/evt_cloud.h"
#include "evt/evt_img.h"
static USER_FUNC(evt_cloud_main);  // local to this file in the original

// Prototypes not yet in headers.

// .rodata
const char str_802F6B28[] = "P_oof_cloud_1";
const char str_802F6B38[] = "i_name_%1d";
const char str_802F6B44[] = "o_name_%1d";
const char str_802F6B50[] = "P_oof_cloud_2";
const char str_802F6B60[] = "SFX_OFF_CLAUD_WIND2";
const char str_802F6B74[] = "SFX_OFF_CLAUD_WIND3";
f64 double_to_int_mask_802f6b88 = 4503599627370496.0;
f64 double_to_int_802f6b90 = 4503601774854144.0;

// .sdata2
extern const f32 float_90_8041dcd0;
extern const f32 float_270_8041dcd4;
extern const f32 float_1_8041dcd8;
extern const f32 float_0p5_8041dcdc;
const char str_8041DCE0[] = "A_1";
extern const f32 float_0_8041dce4;
const char str_8041DCE8[] = "A_2";

// Work structure for the "poof" cloud effect, not yet extracted into a header.
typedef struct CloudEntry {
	u16 flags;              // 0x0
	u8 pad_02[2];           // 0x2
	s32 mode;               // 0x4
	char animName[0x20];    // 0x8
	char imgName[0x20];     // 0x28
	char offName[0x20];     // 0x48
	char mapGrpName[0x20];  // 0x68
	char hitName[0x20];     // 0x88
	s32 field_0xA8;         // 0xA8
	s32 field_0xAC;         // 0xAC
	EventEntry* field_0xB0; // 0xB0
	void* field_0xB4;       // 0xB4
	s32 field_0xB8;         // 0xB8
	const char** field_0xBC;// 0xBC
	u16 field_0xC0;         // 0xC0
	u16 field_0xC2;         // 0xC2
	f32 field_0xC4;         // 0xC4
} CloudEntry;               // 0xC8

static CloudEntry cloud[16];
CloudEntry* cloud_p;
static s8 cloud_once_flag;

static u32 evt_cloud_main_evt[] = { 0x0001005B, (u32)evt_cloud_main, 0x2, 0x1 };

USER_FUNC(evt_cloud_get_mode) {
	s32* args = event->args;

	if (cloud_p == NULL) {
		evtSetValue(event, args[0], -1);
		return EVT_RETURN_DONE;
	} else {
		evtSetValue(event, args[0], cloud_p->mode);
		return EVT_RETURN_DONE;
	}
}

USER_FUNC(evt_cloud_set_cap_size) {
	s32* args = event->args;
	u16 width = evtGetValue(event, args[0]);
	s32 height = evtGetValue(event, args[1]);

	cloud_p->field_0xC0 = width;
	cloud_p->field_0xC2 = height;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_cloud_ent) {
	s32* args = event->args;
	s32 a = evtGetValue(event, args[0]);
	s32 b = evtGetValue(event, args[1]);
	s32 c = evtGetValue(event, args[2]);
	s32 d = evtGetValue(event, args[3]);
	CloudEntry* cp;
	s32 i;

	for (i = 0; i < 16; i++) {
		if (!(cloud[i].flags & 1)) {
			break;
		}
	}
	if (i >= 16) {
		return EVT_RETURN_DONE;
	}

	cp = &cloud[i];
	cp->flags |= 1;
	strncpy(cp->animName, str_802F6B28, 0x20);
	if (a == 0) {
		strncpy(cp->mapGrpName, (const char*)b, 0x20);
	} else {
		strncpy(cp->mapGrpName, *(const char**)b, 0x20);
	}
	strncpy(cp->hitName, (const char*)c, 0x20);
	cp->field_0xB4 = (void*)d;
	cp->field_0xA8 = 0xB4;
	cp->field_0xAC = 0;
	cp->mode = 0;
	cp->field_0xB8 = 0;
	cp->field_0xC0 = 0;
	cp->field_0xC2 = 0;
	cp->field_0xB0 = evtEntry(evt_cloud_main_evt, 0, 0);
	cp->field_0xB0->lwData[0] = (s32)cp;
	cloud_p = cp;
	if (a != 0) {
		cp->field_0xBC = (const char**)b;
		cp->flags |= 2;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_cloud_init) {
	s32 i;

	for (i = 0; i < 16; i++) {
		cloud[i].flags = 0;
		sprintf(cloud[i].imgName, str_802F6B38, i);
		sprintf(cloud[i].offName, str_802F6B44, i);
	}
	cloud_p = NULL;
	cloud_once_flag = 0;
	return EVT_RETURN_DONE;
}

// NONMATCHING (99.91%): everything matches except the FP register numbering in
// the mode-2 screen-space math; original uses f6=sum, f4=fbWidth, f3=0.5 while
// MWCC assigns that set as the rotation f4, f3, f6.
static USER_FUNC(evt_cloud_main) {
	CloudEntry* cp = (CloudEntry*)event->lwData[0];
	s32 inBattle = (gp->inBattle != 0);
	MarioWork* mario = marioGetPtr();
	PartyEntry* party = partyGetPtr(0);
	ImageEntry* img;
	HitEntry* hit;
	Vec pos;
	s32 captureArgs[10];
	EventEntry tmpEvent;
	s32 left;
	s32 top;
	s32 width;
	s32 height;
	f32 t;
	HitEntry* h0;
	HitEntry* h1;
	HitEntry* h2;
	HitEntry* h3;

	if (isFirstCall) {
		return 0;
	}
	if (party == NULL) {
		return 0;
	}

	h0 = (HitEntry*)cloudGetHitBreathout(0);
	h1 = (HitEntry*)cloudGetHitBreathout(1);
	h2 = (HitEntry*)cloudGetHitBreathout(2);
	h3 = (HitEntry*)cloudGetHitBreathout(3);

	switch (cp->mode) {
	case 0:
		if (cloud_once_flag == 1) {
			break;
		}
		hit = hitNameToPtr(cp->hitName);
		if (h0 == hit || h1 == hit || h2 == hit || h3 == hit) {
			cp->mode++;
		}
		cp->field_0xAC = 0;
		break;

	case 1: {
		f32 angle = *(f32*)((u8*)mario + 0x1AC);

		if (angle > float_90_8041dcd0 && angle <= float_270_8041dcd4) {
			strcpy(cp->animName, str_802F6B50);
		} else {
			strcpy(cp->animName, str_802F6B28);
		}
		if (animGroupBaseAsync(cp->animName, inBattle, NULL)) {
			cloud_once_flag = 1;
			animPaperPoseEntry(cp->animName, inBattle);
			offscreenEntry(cp->offName);
			if (!(cp->flags & 2)) {
				mapGrpSetOffScreen(cp->mapGrpName, cp->offName);
			} else {
				const char** p = cp->field_0xBC;
				while (*p != NULL) {
					mapGrpSetOffScreen(*p, cp->offName);
					p++;
				}
			}
			cp->mode++;
		}
		break;
	}

	case 2:
		if (cp->field_0xC0 != 0 && cp->field_0xC2 != 0) {
			mapObjGetPos(cp->mapGrpName, &pos);
			PSMTXMultVec(camGetPtr(4)->view, &pos, &pos);
			PSMTX44MultVec(camGetPtr(4)->projection, &pos, &pos);
			t = float_1_8041dcd8 + pos.x;
			pos.x = (f32)gp->fbWidth * t * float_0p5_8041dcdc;
			t = float_1_8041dcd8 - pos.y;
			pos.y = t * (f32)gp->efbHeight * float_0p5_8041dcdc;
			width = cp->field_0xC0;
			height = cp->field_0xC2;
			left = (s32)(pos.x - (f32)(cp->field_0xC0 / 2));
			top = (s32)(pos.y - (f32)(cp->field_0xC2 / 2));
			if (left < 0) {
				left = 0;
			}
			if (top < 0) {
				top = 0;
			}
			if (width > gp->fbWidth - left) {
				width = gp->fbWidth - left;
			}
			if (height > gp->efbHeight - top) {
				height = gp->efbHeight - top;
			}
		} else {
			height = 0;
			width = 0;
			top = 0;
			left = 0;
		}
		imgEntry(cp->imgName, inBattle);
		img = imgNameToPtr(cp->imgName, inBattle);
		img->flags |= 2;
		img->poseId = animPaperPoseGetId(cp->animName, inBattle);
		*(OSTime*)((u8*)img + 0x110) = animTimeGetTime(inBattle);
		tmpEvent.args = captureArgs;
		captureArgs[0] = (s32)cp->imgName;
		captureArgs[1] = 0;
		captureArgs[2] = (s32)cp->offName;
		captureArgs[3] = 1;
		captureArgs[4] = left;
		captureArgs[5] = top;
		captureArgs[6] = width;
		captureArgs[7] = height;
		evt_img_alloc_capture(&tmpEvent, 1);
		imgClearVirtualPoint(img);
		imgSetShadow(img, 2);
		cp->mode++;
		break;

	case 3:
		img = imgNameToPtr(cp->imgName, inBattle);
		img->flags &= ~2;
		img->field_0x108 = (s32)str_8041DCE0;
		*(OSTime*)((u8*)img + 0x110) = animTimeGetTime(inBattle);
		mapGrpFlagOn(cp->mapGrpName, 1);
		cp->field_0xB8 = 0x1E;
		cp->mode++;
		cp->field_0xC4 = float_0_8041dce4;
		break;

	case 4:
		img = imgNameToPtr(cp->imgName, inBattle);
		if (float_0_8041dce4 == cp->field_0xC4 || float_1_8041dcd8 + cp->field_0xC4 < img->field_0x118) {
			psndSFXOn_3D(str_802F6B60, &mario->position);
			cp->field_0xC4 = img->field_0x118;
		}
		if (mario->currMotionId != 0x1B) {
			cp->mode = 0xA;
			break;
		}
		hit = hitNameToPtr(cp->hitName);
		if (!(h0 == hit || h1 == hit || h2 == hit || h3 == hit)) {
			if (cp->field_0xB8 == 0) {
				cp->mode = 0xA;
			} else {
				cp->field_0xB8--;
				cp->field_0xAC += 2;
			}
		} else {
			cp->field_0xB8 = 0x1E;
			cp->field_0xAC += 2;
			if (cp->field_0xAC > cp->field_0xA8) {
				cp->mode = 0x14;
			}
		}
		break;

	case 0xA:
		mapGrpFlagOff(cp->mapGrpName, 1);
		cp->mode = 0x63;
		break;

	case 0x14:
		psndSFXOn_3D(str_802F6B74, &mario->position);
		if (cp->field_0xB4 != NULL) {
			evtEntry(cp->field_0xB4, 0, 0);
		}
		img = imgNameToPtr(cp->imgName, inBattle);
		img->field_0x108 = (s32)str_8041DCE8;
		*(OSTime*)((u8*)img + 0x110) = animTimeGetTime(inBattle);
		hitObjFlagOn(cp->hitName, 1);
		cp->field_0xB8 = 0x3C;
		cp->mode++;
		break;

	case 0x15:
		if (cp->field_0xB8 != 0) {
			cp->field_0xB8--;
		} else {
			cp->field_0xB8 = 0x1E;
			cp->field_0xAC += 2;
			cp->mode = 0x63;
		}
		break;

	case 0x63:
		img = imgNameToPtr(cp->imgName, inBattle);
		if (!(cp->flags & 2)) {
			mapGrpClearOffScreen(cp->mapGrpName);
		} else {
			const char** p = cp->field_0xBC;
			while (*p != NULL) {
				mapGrpClearOffScreen(*p);
				p++;
			}
		}
		animPaperPoseRelease(img->poseId);
		imgFreeCapture(img, 0);
		imgSetShadow(img, 0);
		imgRelease(img, inBattle);
		offscreenDelete(cp->offName);
		cloud_once_flag = 0;
		cp->mode = 0;
		break;

	default:
		break;
	}

	return 0;
}
