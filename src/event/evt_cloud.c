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

// .sdata2

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
	s32 breathLimit;        // 0xA8
	s32 breathCount;        // 0xAC
	EventEntry* event;      // 0xB0
	void* endScript;        // 0xB4
	s32 waitTimer;          // 0xB8
	const char** mapGrpNames; // 0xBC
	u16 capWidth;           // 0xC0
	u16 capHeight;          // 0xC2
	f32 lastLoopTimes;      // 0xC4
} CloudEntry;               // 0xC8

static CloudEntry cloud[16];
static s8 cloud_once_flag;
CloudEntry* cloud_p;

static u32 evt_cloud_main_evt[] = { 0x0001005B, (u32)evt_cloud_main, 0x2, 0x1 };

// The screen-space math needs the literal 0.5f and separate product temp to keep MWCC's FPR allocation
// matching the original (t in f6, the screen size in f4, the 0.5f in f3); the named 0.5f
// or folding the product into one expression rotates the three registers.
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
	f32 w;
	f32 half;
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
		cp->breathCount = 0;
		break;

	case 1: {
		f32 angle = mario->dispReqDir;

		if (angle > 90.f && angle <= 270.f) {
			strcpy(cp->animName, "P_oof_cloud_2");
		} else {
			strcpy(cp->animName, "P_oof_cloud_1");
		}
		if (animGroupBaseAsync(cp->animName, inBattle, NULL)) {
			cloud_once_flag = 1;
			animPaperPoseEntry(cp->animName, inBattle);
			offscreenEntry(cp->offName);
			if (!(cp->flags & 2)) {
				mapGrpSetOffScreen(cp->mapGrpName, cp->offName);
			} else {
				const char** p = cp->mapGrpNames;
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
		if (cp->capWidth != 0 && cp->capHeight != 0) {
			mapObjGetPos(cp->mapGrpName, &pos);
			PSMTXMultVec(camGetPtr(4)->view, &pos, &pos);
			PSMTX44MultVec(camGetPtr(4)->projection, &pos, &pos);
			half = 0.5f;
			t = 1.0f + pos.x;
			w = t * (f32)gp->fbWidth;
			pos.x = w * half;
			t = 1.0f - pos.y;
			w = t * (f32)gp->efbHeight;
			pos.y = w * half;
			width = cp->capWidth;
			height = cp->capHeight;
			left = (s32)(pos.x - (f32)(cp->capWidth / 2));
			top = (s32)(pos.y - (f32)(cp->capHeight / 2));
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
		img->startTime = animTimeGetTime(inBattle);
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
		img->animName = "A_1";
		img->startTime = animTimeGetTime(inBattle);
		mapGrpFlagOn(cp->mapGrpName, 1);
		cp->waitTimer = 0x1E;
		cp->mode++;
		cp->lastLoopTimes = 0.0f;
		break;

	case 4:
		img = imgNameToPtr(cp->imgName, inBattle);
		if (0.0f == cp->lastLoopTimes || 1.0f + cp->lastLoopTimes < img->loopTimes) {
			psndSFXOn_3D("SFX_OFF_CLAUD_WIND2", &mario->position);
			cp->lastLoopTimes = img->loopTimes;
		}
		if (mario->currMotionId != 0x1B) {
			cp->mode = 0xA;
			break;
		}
		hit = hitNameToPtr(cp->hitName);
		if (!(h0 == hit || h1 == hit || h2 == hit || h3 == hit)) {
			if (cp->waitTimer == 0) {
				cp->mode = 0xA;
			} else {
				cp->waitTimer--;
				cp->breathCount += 2;
			}
		} else {
			cp->waitTimer = 0x1E;
			cp->breathCount += 2;
			if (cp->breathCount > cp->breathLimit) {
				cp->mode = 0x14;
			}
		}
		break;

	case 0xA:
		mapGrpFlagOff(cp->mapGrpName, 1);
		cp->mode = 0x63;
		break;

	case 0x14:
		psndSFXOn_3D("SFX_OFF_CLAUD_WIND3", &mario->position);
		if (cp->endScript != NULL) {
			evtEntry(cp->endScript, 0, 0);
		}
		img = imgNameToPtr(cp->imgName, inBattle);
		img->animName = "A_2";
		img->startTime = animTimeGetTime(inBattle);
		hitObjFlagOn(cp->hitName, 1);
		cp->waitTimer = 0x3C;
		cp->mode++;
		break;

	case 0x15:
		if (cp->waitTimer != 0) {
			cp->waitTimer--;
		} else {
			cp->waitTimer = 0x1E;
			cp->breathCount += 2;
			cp->mode = 0x63;
		}
		break;

	case 0x63:
		img = imgNameToPtr(cp->imgName, inBattle);
		if (!(cp->flags & 2)) {
			mapGrpClearOffScreen(cp->mapGrpName);
		} else {
			const char** p = cp->mapGrpNames;
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

USER_FUNC(evt_cloud_init) {
	s32 i;

	for (i = 0; i < 16; i++) {
		cloud[i].flags = 0;
		sprintf(cloud[i].imgName, "i_name_%1d", i);
		sprintf(cloud[i].offName, "o_name_%1d", i);
	}
	cloud_p = NULL;
	cloud_once_flag = 0;
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
	strncpy(cp->animName, "P_oof_cloud_1", 0x20);
	if (a == 0) {
		strncpy(cp->mapGrpName, (const char*)b, 0x20);
	} else {
		strncpy(cp->mapGrpName, *(const char**)b, 0x20);
	}
	strncpy(cp->hitName, (const char*)c, 0x20);
	cp->endScript = (void*)d;
	cp->breathLimit = 0xB4;
	cp->breathCount = 0;
	cp->mode = 0;
	cp->waitTimer = 0;
	cp->capWidth = 0;
	cp->capHeight = 0;
	cp->event = evtEntry(evt_cloud_main_evt, 0, 0);
	cp->event->lwData[0] = (s32)cp;
	cloud_p = cp;
	if (a != 0) {
		cp->mapGrpNames = (const char**)b;
		cp->flags |= 2;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_cloud_set_cap_size) {
	s32* args = event->args;
	u16 width = evtGetValue(event, args[0]);
	s32 height = evtGetValue(event, args[1]);

	cloud_p->capWidth = width;
	cloud_p->capHeight = height;
	return EVT_RETURN_DONE;
}

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

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
