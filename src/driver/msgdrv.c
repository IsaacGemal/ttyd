#include "driver/msgdrv.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/npcdrv.h"
#include "driver/windowdrv.h"
#include "manager/dvdmgr.h"
#include "manager/fontmgr.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "memory.h"
#include "system.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern GlobalWork* gp;
extern GXColor unk_80422fe4;
extern f32 float_0p36_8041a6a0;
extern f32 float_0p6_8041a570;
extern s32 msgWindowOpen_SE_Data[12][2];
extern GXColor dat_8041a4f0;
extern GXColor dat_8041a4f4;
extern GXColor dat_8041a4f8;
extern GXColor dat_8041a4fc;

typedef struct MsgLookupEntry {
	u32 startaddr; //0x0
	u32 endaddr; //0x4
} MsgLookupEntry;

typedef struct MsgIcon {
	const char* name; //0x0
	s32 value; //0x4
} MsgIcon;

//.data
MsgIcon msgIcon[] = {
	{"PAD_A", 0x64},
	{"PAD_B", 0x66},
	{"STICK_LEFT", 0x78},
	{"STICK", 0x79},
	{"AC_ON", 0x95},
	{"BUTTON_L", 0x7E},
	{"BUTTON_R", 0x80},
	{"HM", 0x19F},
	{"PAD_X", 0x68},
	{"PAD_X_ON", 0x69},
	{"PAD_Y", 0x6A},
	{"PAD_Y_ON", 0x6B},
	{"PAD_Z_OFF", 0x82},
	{"PAD_ST_OFF", 0x84},
	{"black_key", 0x130},
	{"ANM_PAD_A", 0x20B},
	{"ANM_STICK_RIGHT", 0x20C},
	{"ANM_PAD_START", 0x20D}
};

//.sbss
u16 active; //TODO: re-type
void* msgSortDvdData; //TODO: re-type
BOOL msgSEflag; //msgSEInit and related functions are unused
MessageWork msgWork;

//.sdata
MessageWork* msgw = &msgWork;
const char* ErrMessage[2] = {
	"<col ff0000ff>\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x82\xAA\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1</col>\n<key>", /*"メッセージがありません"*/
	"<col ff0000ff>No messages.<col>\n<key>"
};
const char* paperPoseTbl[6] = {
	"PM_Y_1B", "PM_Y_2B", "PM_F_1B",
	"PM_F_3B", "PM_C_1B", "PM_C_2B"
};
const char* language_dir[2] = {"msg", "emsg"};





//local prototypes

// NONMATCHING (99.94%): the code itself is byte-identical. The only differences are the three
// computed string offsets (0x208/0x210/0x218 vs 0x13c/0x144/0x14c): this file's .rodata pool is
// 0xCC bytes short because the string literals of the not-yet-decompiled functions (msgAnalize's
// "keyxon".."/paper" block, msgDisp, etc.) aren't emitted yet. The pool follows .text order, so
// msgInit (last function) is hit. Nothing inside msgInit can move them; it will reach 100% once
// those functions contribute their literals.
void msgInit(void) {
	msgw->poseId = animPoseEntry("hoshi_2", 2);
	animPoseSetAnim(msgw->poseId, "HOS_C_1", TRUE);
	animPoseSetMaterialFlagOn(msgw->poseId, 0x1800);
	animPoseSetMaterialFlagOff(msgw->poseId, 0x40);
	animPoseSetGXFunc(msgw->poseId, msgDispKeyWait_render, 0);
	msgw->entries[0].address = NULL;
	msgw->entries[0].lookup = NULL;
	msgw->entries[1].address = NULL;
	msgw->entries[1].lookup = NULL;
	msgLoad("global", 1);
}

// NONMATCHING (86.61%): 6 instructions differ, 12 missing, 8 extra, plus register differences.
void msgLoad(const char* filename, s32 index) {
	MessageEntry* entry;
	DVDEntry* dvd;
	char* msgptr;
	s32 msgcount;
	char filepath[64];
	char dvdpath[128]; //TODO: double check size
	int i;
	u32 r27, r30;
	MsgLookupEntry* lookup;

	entry = &msgw->entries[index];
	sprintf(filepath, "./%s/%s.txt", language_dir[gp->language], filename);
	
	if (entry->address) {
		if (index == 1) {
			__memFree(HEAP_DEFAULT, entry->address);
		}
		entry->address = NULL;
	}
	entry->length = 0;

	if (!entry->address) {
		sprintf(dvdpath, "%s/%s", getMarioStDvdRoot(), filepath);
		dvd = DVDMgrOpen(dvdpath, 2, 0);
		if (dvd) {
			entry->length = DVDMgrGetLength(dvd);
			switch (index) {
				case 0:
					entry->address = _mapAlloc(OSRoundUp32B(entry->length));
					break;
				case 1:
					entry->address = __memAlloc(HEAP_DEFAULT, OSRoundUp32B(entry->length));
					break;
			}
			DVDMgrRead(dvd, entry->address, OSRoundUp32B(entry->length), 0);
			DVDMgrClose(dvd);
		}
	}

	if (entry->address) {
		msgptr = entry->address;
		msgcount = 0;
		while (*msgptr) {
			msgcount++;
			msgptr += strlen(msgptr) + 1;
		}
		entry->count = (u32)(msgcount / 2); //tag, message
	}
	else {
		entry->length = 0;
		entry->count = 0;
	}

	if (entry->lookup) {
		smartFree(entry->lookup);
	}
	if (entry->count) {
		r27 = 0;
		entry->lookup = smartAlloc(entry->count * sizeof(MsgLookupEntry), 0);
		r30 = (u32)entry->address;
		lookup = entry->lookup->address;
		for (i = 0; i < entry->count; i++) {
			lookup->startaddr = r27;
			r27 += strlen((const char*)r30) + 1;
			r30 += strlen((const char*)r30) + 1;
			lookup->endaddr = r27;
			r27 += strlen((const char*)r30) + 1;
			r30 += strlen((const char*)r30) + 1;
			lookup++;
		}
		msgSortDvdData = entry->address;
		qsort(entry->lookup->address, entry->count, sizeof(MsgLookupEntry), msg_compare);
	}
	else {
		entry->lookup = NULL;
	}
}

// Append a value to the message history array at 0x7844, skipping duplicates of the last entry.
static void msgHistory(smartEntry* smart, s32 value) {
	MessageData* data = smart->address;

	if (!(data->field_0x4 & 0x200) && data->field_0x7840 < 0x6e
	    && (data->field_0x7840 == 0
	        || ((s32*)((u8*)data + 0x7844))[data->field_0x7840 - 1] != value)) {
		((s32*)((u8*)data + 0x7844))[data->field_0x7840++] = value;
	}
}

s32 msgMain(smartEntry* smart) {
	MessageData* data = smart->address;
	MsgDataEntry* entry;
	s32 i;
	s32 time;
	u32 mask;
	u32 button;

	msgSEflag = 0;

	if (data->field_0x4 & 2) {
		if ((void (*)(s32, smartEntry*))data->field_0x79FC) {
			((void (*)(s32, smartEntry*))data->field_0x79FC)(0, smart);
		}
		data->field_0x4 &= ~2;
		msgHistory(smart, 0);
	}

	if (data->field_0x24 > data->field_0x28) {
		data->field_0x28 += float_0p36_8041a6a0 * (f32)OSTicksToMilliseconds(gp->renderTime - data->field_0x30);
		if (!(data->field_0x4 & 0x80)) {
			i = (((MessageData*)smart->address)->field_0x4 & 0x100) ? 0x500 : 0x100;
			mask = i | 0x200;
			button = keyGetButtonTrg(0);
			if (button & mask) {
				data->field_0x4 |= 4;
			}
		}
		if (!(data->field_0x4 & 0x10) && (data->field_0x4 & 0x4)) {
			data->field_0x28 += float_0p36_8041a6a0 * (f32)OSTicksToMilliseconds(gp->renderTime - data->field_0x30);
		}
		if (data->field_0x24 < data->field_0x28) {
			data->field_0x28 = data->field_0x24;
		}
		data->field_0x30 = gp->renderTime;
		return 3;
	}

	switch (*(u16*)((u8*)data + 0xa)) {
	case 1:
		return 2;
	case 2:
		*(u16*)((u8*)data + 0xc) = 1;
		(*(u16*)((u8*)data + 0x8))++;
		*(u16*)((u8*)data + 0xa) = 0;
		psndSFXOn((const char*)0x2003D);
		data->field_0x20 += 0x64;
		if ((void (*)(s32, smartEntry*))data->field_0x79FC) {
			((void (*)(s32, smartEntry*))data->field_0x79FC)(3, smart);
		}
		break;
	default:
		time = (s32)OSTicksToMilliseconds(gp->renderTime - data->field_0x30);
		if (!(data->field_0x4 & 0x80)) {
			i = (((MessageData*)smart->address)->field_0x4 & 0x100) ? 0x500 : 0x100;
			mask = i | 0x200;
			button = keyGetButtonTrg(0);
			if (button & mask) {
				data->field_0x4 |= 4;
			}
		}
		if (!(data->field_0x4 & 0x10) && (data->field_0x4 & 0x4)) {
			time *= 0x64;
		}
		data->field_0x20 += time;
		break;
	}

	data->field_0x30 = gp->renderTime;
	entry = &data->entries[data->field_0x10];
	i = data->field_0x10;
	while (i < 0x500) {
		if ((u32)entry->field_0xC > (u32)data->field_0x20) {
			break;
		}
		switch (entry->glyph) {
		case 0xFFFF:
			if ((void (*)(s32, smartEntry*))data->field_0x79FC) {
				((void (*)(s32, smartEntry*))data->field_0x79FC)(1, smart);
			}
			data->field_0x10 = i;
			return 0;
		case 0xFFFE:
			if (*(s16*)((u8*)entry + 0x6) != *(u16*)((u8*)data + 0x8)) {
				break;
			}
			*(u16*)((u8*)data + 0xa) = 1;
			data->field_0x20 = entry->field_0xC;
			data->field_0x4 &= ~4;
			if ((void (*)(s32, smartEntry*))data->field_0x79FC) {
				((void (*)(s32, smartEntry*))data->field_0x79FC)(2, smart);
			}
			break;
		case 0xFFFD:
			data->field_0x24 -= *(s16*)((u8*)entry + 0x8);
			data->field_0x20 = entry->field_0xC;
#ifdef MUST_MATCH
			// The original does this append with byte-offset arithmetic (addi/lwzx/stwx); the
			// array form used in msgHistory comes out as add/displacement here.
			{
				MessageData* d = ((MessageData*)smart->address);
				s32 value = data->field_0x24;
				if (!(d->field_0x4 & 0x200) && d->field_0x7840 < 0x6e
				    && (d->field_0x7840 == 0
				        || *(s32*)((u8*)d + 0x7840 + d->field_0x7840 * 4) != value)) {
					*(s32*)((u8*)d + 0x7844 + d->field_0x7840++ * 4) = value;
				}
			}
#else
			msgHistory(smart, data->field_0x24);
#endif
			break;
		case 0xFFFC:
			data->field_0x10 = i + 1;
			data->field_0x20 = entry->field_0xC;
			data->field_0x4 &= ~4;
			return 1;
		case 0xFFFB:
			if ((void (*)(s32, smartEntry*))data->field_0x79FC) {
				((void (*)(s32, smartEntry*))data->field_0x79FC)(4, smart);
			}
			break;
		case 0xFFFA:
			data->field_0x7A28 = entry->field_0x0;
			break;
		case 0xFFF9:
			data->field_0x7A2C = entry->field_0x0;
			break;
		case 0xFFF5:
			if (entry->field_0x0) {
				data->field_0x4 |= 0x80;
				data->field_0x4 &= ~4;
				data->field_0x20 = entry->field_0xC;
			}
			else {
				data->field_0x4 &= ~0x80;
			}
			break;
		case 0xFFF4:
		case 0xFFF8:
			break;
		case 0xFFF3:
			{
				NpcEntry* npc = (NpcEntry*)entry->field_0x0;
				if (npc == NULL) {
					npc = npcNameToPtr(data->text);
				}
				animPoseSetPaperAnimGroup(npc->poseId, "p_b_st", TRUE);
				animPosePaperPeraOn(npc->poseId);
				((void (*)(s32, const char*, BOOL))animPoseSetPaperAnim)(npc->poseId, paperPoseTbl[entry->field_0xC], TRUE);
			}
			break;
		case 0xFFF2:
			{
				NpcEntry* npc = (NpcEntry*)entry->field_0x0;
				if (*(u16*)((u8*)data + 0xa) != 0) {
					i--;
				}
				else {
					if (npc == NULL) {
						npc = npcNameToPtr(data->text);
					}
					animPoseSetPaperAnimGroup(npc->poseId, NULL, TRUE);
				}
			}
			break;
		default:
			if ((void (*)(s32, smartEntry*))data->field_0x79FC) {
				((void (*)(s32, smartEntry*))data->field_0x79FC)(5, smart);
			}
			if (entry->field_0x0 & 0x10) {
				if (((MessageData*)smart->address)->window->type != 1 && !msgSEflag) {
					psndSFXOnVol(0x20044, ((MessageData*)smart->address)->field_0x7A2C);
					msgSEflag = 1;
				}
			}
			else if (entry->field_0x0 & 0x8) {
				if (((MessageData*)smart->address)->window->type != 3) {
					((MessageData*)smart->address)->field_0x7A50 = 0xf;
				}
				if (((MessageData*)smart->address)->window->type != 1 && !msgSEflag) {
					psndSFXOnVol(0x20044, ((MessageData*)smart->address)->field_0x7A2C);
					msgSEflag = 1;
				}
			}
			break;
		}
		i++;
		entry++;
	}
	data->field_0x10 = i;
	if (*(u16*)((u8*)data + 0xa) != 0) {
		return 2;
	}
	return 3;
}

//s32 msgMain(void* somearray)
//BOOL msgDisp(unused r3, r4, f32 f1, f32 f2)

void msgDispKeyWait_render(s32 wXluStage) {
	AnimationWork* anim = animGetPtr();

	GXSetZMode(1, 7, 0);
	GXSetBlendMode(0, 4, 5, 0);
	GXSetZCompLoc(0);
	GXSetAlphaCompare(6, 0x80, 1, 0, 0);
	anim->currDispMode = wXluStage;
}

//void msgAnalize(void* sameasmsgmain)

MessageWork* L_msgGetWork(void) {
	return &msgWork;
}

int msg_compare(const void* a, const void* b) {
	s32 result = strcmp((const char*)((u32)msgSortDvdData + *(const u32*)a),
	                    (const char*)((u32)msgSortDvdData + *(const u32*)b));

	if (result > 0) {
		return 1;
	}
	return result < 0 ? -1 : 0;
}

const char* msgSearch(const char* tag) { //1:1
	MessageEntry* entry;
	MsgLookupEntry* lookup;
	smartEntry* smart;
	s32 i, max, j, index;
	s32 result;

	for (i = 0; i < 2; i++) { //check both entries
		entry = &msgw->entries[i];
		smart = entry->lookup;
		if (smart) {
			lookup = smart->address;
			j = 0;
			max = entry->count;
			while (max >= j) {
				index = (j + max) / 2; //tag, message
				if (index >= (s32)entry->count) {
					break;
				}
				result = strcmp(tag, (const char*)((u32)entry->address + lookup[index].startaddr));
				if (!result) {
					return (const char*)((u32)entry->address + lookup[index].endaddr);
				}
				if (result < 0) {
					max = index - 1;
				}
				else {
					j = index + 1;
				}
			}
		}
	}
	return ErrMessage[gp->language];
}

BOOL _ismbblead(u32 a1) {
	u8 check = (u8)a1;
	if ((check >= 0x81 && check <= 0x9F) || (check >= 0xE0 && check <= 0xFC)) {
		return TRUE;
	}
	return FALSE;
}










// math.h comes after msgInit: included at the top it changes how msgInit addresses its strings.
#include <math.h>
// NONMATCHING (93.84%): 247 instructions differ, nearly all register allocation and
// instruction scheduling; see REPORT.md. The logic is complete.
BOOL msgDisp(smartEntry* smart, u8 alpha, f32 x, f32 y) {
	MessageData* data = smart->address;
	BOOL result = FALSE;
	MsgDataEntry* entry = data->entries;
	s32 i;

	FontDrawStart_alpha(alpha);

	for (i = 0; i < data->field_0x10; i++, entry++) {
		switch (entry->glyph) {
		case 0xFFFF:
			return FALSE;
		case 0xFFFE:
			if (*(u16*)((u8*)data + 0x8) == *(s16*)((u8*)entry + 0x6)) {
				result = TRUE;
			}
			break;
		case 0xFFF7: {
			GXColor color = *(GXColor*)((u8*)entry + 0x0);
			FontDrawColor(&color);
			break;
		}
		case 0xFFF6:
			if (*(u32*)((u8*)entry + 0x0)) {
				FontDrawNoise();
			}
			else {
				FontDrawNoiseOff();
			}
			break;
		case 0xFFF4: {
			GXColor color = FontGetDrawColor();
			Mtx mtx;
			f32 rotY;

			PSMTXIdentity(mtx);
			rotY = *(f32*)((u8*)entry + 0x14);
			if (rotY >= 90.0f && rotY <= 270.0f) {
				PSMTXScaleApply(mtx, mtx, 1.0f, 1.0f, -1.0f);
			}
			PSMTXTransApply(mtx, mtx,
				x + (f32)*(s16*)((u8*)entry + 0x6),
				y + (f32)data->field_0x28 + (f32)*(s16*)((u8*)entry + 0x8) - 40.0f,
				0.0f);
			animPoseMain(*(s32*)((u8*)entry + 0x0));
			animPoseDrawMtx(*(s32*)((u8*)entry + 0x0), mtx, 2,
				*(f32*)((u8*)entry + 0x14), *(f32*)((u8*)entry + 0x10));
			FontDrawStart_alpha(alpha);
			FontDrawColor(&color);
			break;
		}
		case 0xFFF5:
		case 0xFFF9:
		case 0xFFFA:
		case 0xFFFB:
		case 0xFFFC:
		case 0xFFFD:
			break;
		case 0xFFF8:
		default: {
			f32 px = x + (f32)*(s16*)((u8*)entry + 0x6);
			f32 py = y + (f32)data->field_0x28 + (f32)*(s16*)((u8*)entry + 0x8);
			BOOL tev = FALSE;
			u8 fade = 0xFF;

			if (data->field_0x4 & 0x40) {
				s32 cur = data->field_0x28;
				s32 idx = *(s32*)((u8*)data + 0x783C);
				s32 e8 = *(s16*)((u8*)entry + 0x8);
				s32* base = (s32*)((u8*)data + idx * 4);

				if (cur == *(s32*)((u8*)base + 0x7844)) {
					if (e8 + *(s32*)((u8*)base + 0x7848) <= 0) {
						break;
					}
					tev = TRUE;
				}
				else if (cur < *(s32*)((u8*)base + 0x7844)) {
					if (e8 + *(s32*)((u8*)base + 0x7844) > 0) {
						tev = TRUE;
					}
					else {
						s32 t = *(s32*)((u8*)base + 0x7844) - cur;
						if (t > 100) {
							t = 100;
						}
						fade = (u32)((100 - t) * 255 / 100);
						if (idx != data->field_0x7840 - 1) {
							if (e8 + *(s32*)((u8*)base + 0x7848) > 0) {
								tev = TRUE;
							}
							else {
								break;
							}
						}
						else {
							GXColor color = FontGetDrawColor();
							color.a = (color.a * fade) / 255;
							GXSetTevColor(GX_TEVREG0, color);
						}
					}
				}
				else {
					if (e8 + *(s32*)((u8*)base + 0x7848) > 0) {
						tev = TRUE;
					}
					else {
						s32 t = cur - *(s32*)((u8*)base + 0x7844);
						if (t > 100) {
							t = 100;
						}
						fade = (u32)(t * 255 / 100);
						if (idx != data->field_0x7840 - 2) {
							if (e8 + *(s32*)((u8*)base + 0x784C) > 0) {
								tev = TRUE;
							}
							else {
								break;
							}
						}
						else {
							GXColor color = FontGetDrawColor();
							color.a = (color.a * fade) / 255;
							GXSetTevColor(GX_TEVREG0, color);
						}
					}
				}
			}

			if (entry->glyph == 0xFFF8) {
				GXColor color = FontGetDrawColor();
				Vec pos;
				pos.x = 20.0f + (x + (f32)*(s16*)((u8*)entry + 0x6));
				pos.y = (f32)*(s16*)((u8*)entry + 0xA) + (((f32)*(s16*)((u8*)entry + 0x8) + (y + (f32)data->field_0x28)) - 40.0f);
				pos.z = 0.0f;
				iconDispGxAlpha(&pos, 0x10, *(u32*)((u8*)entry + 0x0) & 0xFFFF,
					(u8)((fade * alpha) / 255), *(f32*)((u8*)entry + 0x10));
				FontDrawStart_alpha(alpha);
				FontDrawColor(&color);
				break;
			}

			if ((f32)(*(s16*)((u8*)entry + 0x8) + data->field_0x28) >= 32.0f * *(f32*)((u8*)entry + 0x10) - 10.0f) {
				break;
			}

			if (tev) {
				GXColor color = *(GXColor*)&dat_8041a4f0;
				switch (data->window->type) {
				case 3:
				case 8:
				case 9:
					color = *(GXColor*)&dat_8041a4f4;
					break;
				}
				color.a = fade;
				GXSetTevColor(GX_TEVREG0, color);
				FontDrawScale(*(f32*)((u8*)entry + 0x10));
				FontDrawCode(entry->glyph, px, py);
				FontDrawColor_();
				break;
			}

			if (*(u32*)((u8*)entry + 0x0) & 1) {
				u16 half = kanjiGetWidth(entry->glyph) >> 1;
				f32 fx = px;
				f32 fy = py;
				Mtx mtx;

				if (*(u32*)((u8*)entry + 0x0) & 2) {
					fx = px + (f32)(irand(10000) % 3);
					fy = py + (f32)(irand(10000) % 3);
				}
				PSMTXTrans(mtx, (f32)(-half), 12.0f, 0.0f);
				PSMTXScaleApply(mtx, mtx, *(f32*)((u8*)entry + 0x14), *(f32*)((u8*)entry + 0x14), *(f32*)((u8*)entry + 0x14));
				PSMTXTransApply(mtx, mtx, (f32)half, -12.0f, 0.0f);
				PSMTXScaleApply(mtx, mtx, *(f32*)((u8*)entry + 0x10), *(f32*)((u8*)entry + 0x10), *(f32*)((u8*)entry + 0x10));
				PSMTXTransApply(mtx, mtx, fx, fy, 0.0f);
				FontDrawCodeMtx(mtx, entry->glyph);
				*(f32*)((u8*)entry + 0x14) = 0.5f * (*(f32*)((u8*)entry + 0x14) - 1.0f) + 1.0f;
			}
			else if (*(u32*)((u8*)entry + 0x0) & 2) {
				f32 fx = px + (f32)(irand(10000) % 3);
				f32 fy = py + (f32)(irand(10000) % 3);
				FontDrawScale(*(f32*)((u8*)entry + 0x10));
				FontDrawCode(entry->glyph, fx, fy);
			}
			else if (*(u32*)((u8*)entry + 0x0) & 4) {
				f32 t = (f32)OSTicksToMilliseconds(gp->renderTime - data->field_0x18);
				f32 angle = 0.01f * t - 0.02f * px;
				f32 ca = (f32)cos(angle);
				f32 sa = (f32)sin(angle);
				FontDrawScale(*(f32*)((u8*)entry + 0x10));
				FontDrawCode(entry->glyph, 3.2f * ca + px, py - 3.2f * sa);
			}
			else if (*(u32*)((u8*)entry + 0x0) & (0x100 << data->field_0x7A43)) {
				f32 t = (f32)OSTicksToMilliseconds(gp->renderTime - data->field_0x18);
				f32 angle = 0.01f * t - 0.02f * px;
				f32 ca = (f32)cos(angle);
				f32 fx = 3.2f * ca + px;
				f32 sa = (f32)sin(angle);
				f32 fy = py - 3.2f * sa;

				FontDrawScale(*(f32*)((u8*)entry + 0x10));
				{
					GXColor color = *(GXColor*)&dat_8041a4f8;
					FontDrawColor(&color);
				}
				FontDrawCode(entry->glyph, fx, fy);
				FontDrawEdge();
				FontDrawRainbowColor();
				{
					GXColor color2 = *(GXColor*)&dat_8041a4fc;
					FontDrawColor(&color2);
				}
				FontDrawCode(entry->glyph, fx - 4.0f, fy + 4.0f);
				FontDrawEdgeOff();
				FontDrawRainbowColorOff();
				FontDrawColorIDX(0);
			}
			else {
				FontDrawScale(*(f32*)((u8*)entry + 0x10));
				FontDrawCode(entry->glyph, px, py);
			}
			break;
		}
		}
	}
	return result;
}


// NONMATCHING (3.56%): 2 instructions differ, 422 missing, plus register differences.
void msgWindow_Disp(CameraId camId, void* param) {
	WindowEntry* window = param; //cast to correct type
	CameraEntry* camera = camGetCurPtr();
	s32 type;

	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0, 0, 0, 0});
	type = window->type;

	switch (type) {
	case 0:
	case 1:
	case 6:
	case 8:
	case 9:
		break;
	}

}

void msgWindow_Continue(s32 id) {
	WindowEntry* window = windowGetPointer(id);
	MessageData* data;

	*(u16*)window->field_0x0 = 1;
	window->flags |= 2;
	data = window->userdata->address;
	data->field_0x30 = gp->renderTime;
}

void msgWindow_Add(const char* msg, s32 id) {
	WindowEntry* window = windowGetPointer(id);
	smartEntry* smart = window->userdata;
	MessageData* data;

	data = smart->address;
	data->field_0x30 = gp->renderTime;
	msgAnalize(smart, msg);
	*(u16*)window->field_0x0 = 1;
	window->flags |= 2;
}

void msgWindow_Repeat(s32 id) {
	WindowEntry* window = windowGetPointer(id);
	MessageData* data;

	*(u16*)window->field_0x0 = 1;
	window->flags |= 2;
	data = window->userdata->address;
	data->field_0x30 = gp->renderTime;
	data->field_0x18 = gp->renderTime;
	data->field_0x20 = 0;
	data->field_0x24 = 0;
	data->field_0x28 = 0;
	data->field_0x10 = 0;
}

void msgWindow_Delete(WindowEntry* window) {
	MessageData* data;
	s32 i;

	if (window->userdata) {
		// The original emits an empty mtctr/bdnz loop over data->entryId here.
		data = window->userdata->address;
		for (i = data->entryId; i > 0; i--) {
		}
		smartFree(window->userdata);
		window->userdata = NULL;
	}
}

void msgWindow_ForceClose(const char* tag) {
	s32 i = 0;
	WindowEntry* window;
	MessageData* data;

	for (i = 0; i < 7; i++) {
		window = windowGetPointer(i);
		if (window->flags && *(u16*)window->field_0x0 && window->userdata) {
			data = window->userdata->address;
			if (data->tag && strcmp(data->tag, tag) == 0) {
				*(u16*)window->field_0x0 = 7;
				data = window->userdata->address;
				if (data->field_0x4 & 8) {
					BtlUnit_ChangeStayAnim(data->field_0x7A24);
				}
				else {
					npcSetStayPose(data->text);
				}
			}
		}
	}
}

// NONMATCHING (93.29%): full body written. The original's 3rd parameter reaches windowEntry with no
// clrlwi (u16), but the pre-existing prototype declares it s32; register allocation also differs.
s32 msgWindow_Entry(const char* msg, s32 a, s32 b) {
	s32 id;
	WindowEntry* window;
	MessageData* data;
	smartEntry* smart;
	char* buf;

	id = windowEntry(b);
	window = windowGetPointer(id);
	window->main = msgWindow_Main;
	window->delete = msgWindow_Delete;
	*(u16*)window->field_0x0 = 5;
	window->type = -1;
	window->flags |= 2;
	buf = __memAlloc(HEAP_DEFAULT, strlen(msg));
	strcpy(buf, msg);
	smart = smartAlloc(0x7A58, 1);
	data = smart->address;
	memset(data, 0, 0x7A58);
	data->window = window;
	data->field_0x4 = 3;
	data->field_0x79FC = a;
	data->field_0x30 = gp->renderTime;
	data->field_0x18 = gp->renderTime;
	data->field_0x7A28 = 0;
	data->field_0x7A2C = 0x7f;
	data->field_0x7A30 = 0;
	data->field_0x7A34 = 0;
	data->field_0x7A36 = 0;
	data->field_0x7A38 = 0;
	data->field_0x7A3A = 0;
	data->field_0x7A3C = 0x19;
	data->field_0x7840 = 0;
	msgAnalize(smart, buf);
	if (window->type == 1) {
		data->field_0x4 |= 0x200;
	}
	__memFree(HEAP_DEFAULT, buf);
	window->userdata = smart;
	data->field_0x7A50 = 0;
	data->field_0x7A54 = 0;
	switch (window->type) {
	case 11:
		window->main = msgWindow_Clear_Main;
		/* fallthrough */
	case 0:
	case 2:
	case 3:
	case 4:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		if (!(window->flags & 4)) {
			window->x = -280.0f;
			window->y = 225.0f;
			window->height = 560.0f;
			window->width = 176.0f;
		}
		break;
	case 5:
		window->main = selectWindow_Main;
		data = window->userdata->address;
		window->x = (-data->field_0x7A44) / 2;
		window->y = data->field_0x7A46;
		window->height = data->field_0x7A44;
		window->width = data->field_0x7A40 * 0x24 + 0x28;
		data->field_0x7A43 = data->field_0x7A41;
		data->field_0x7A48 = window->y - (f32)(data->field_0x7A43 * 0x1F + 0x3A);
		break;
	case 1:
		window->field_0x4 = 10;
		window->height = 0.7f * (f32)GXGetTexObjWidth(getWakuTexObj(3));
		window->width = 0.7f * (f32)GXGetTexObjHeight(getWakuTexObj(3));
		break;
	}
	return id;
}

void selectWindow_Main(WindowEntry* window) {
	MessageData* data = window->userdata->address;
	s8 old;

	msgMain(window->userdata);
	switch (*(u16*)window->field_0x0) {
	case 5:
		window->alpha += 0x19;
		if (window->alpha > 0xff) {
			window->alpha = 0xff;
			*(u16*)window->field_0x0 = 1;
		}
		break;
	case 1:
		old = data->field_0x7A43;
		if ((keyGetButtonTrg(0) & 8) || (keyGetDirTrg(0) & 0x1000)) {
			data->field_0x7A43--;
		}
		else if ((keyGetButtonRep(0) & 8) || (keyGetDirRep(0) & 0x1000)) {
			if (data->field_0x7A43 != 0) {
				data->field_0x7A43--;
			}
		}
		if ((keyGetButtonTrg(0) & 4) || (keyGetDirTrg(0) & 0x2000)) {
			data->field_0x7A43++;
		}
		else if ((keyGetButtonRep(0) & 4) || (keyGetDirRep(0) & 0x2000)) {
			if ((s8)data->field_0x7A43 < (s8)data->field_0x7A40 - 1) {
				data->field_0x7A43++;
			}
		}
		if (data->field_0x7A43 < 0) {
			data->field_0x7A43 = data->field_0x7A40 - 1;
		}
		if (data->field_0x7A43 >= data->field_0x7A40) {
			data->field_0x7A43 = 0;
		}
		if (old != data->field_0x7A43) {
			psndSFXOn((const char*)0x20005);
		}
		if (keyGetButtonTrg(0) & 0x200) {
			switch (data->field_0x7A42) {
			case -1:
				break;
			case -2:
				data->field_0x7A43 = -1;
				*(u16*)window->field_0x0 = 7;
				psndSFXOn((const char*)0x20013);
				break;
			default:
				data->field_0x7A43 = data->field_0x7A42;
				*(u16*)window->field_0x0 = 7;
				psndSFXOn((const char*)0x20013);
				break;
			}
		}
		else if (keyGetButtonTrg(0) & 0x100) {
			*(u16*)window->field_0x0 = 7;
			psndSFXOn((const char*)0x20012);
		}
		break;
	case 7:
		if (window->alpha == 0) {
			*(u16*)window->field_0x0 = 4;
			window->flags &= ~2;
			return;
		}
		window->alpha -= 0x19;
		if (window->alpha <= 0) {
			window->alpha = 0;
		}
		break;
	case 4:
		return;
	}
	dispEntry(8, 400.0f - (f32)*(s16*)&window->field_0x4, 0, selectWindow_Disp, window);
}

// 100%: the original keeps window->userdata in r30 and the MessageData pointer in r29; this compiler
// otherwise swaps them, so `value` is hoisted to function scope to steer the allocator (below is the
// natural per-case scope). The button temp likewise fixes the two commutative `and.` operand orders.
// The switch jump table is emitted as an anonymous @829 instead of jumptable_8030DBA0 (its .data offset
// differs because msgWindowOpen_SE_Data is extern in this unit rather than defined here); objdiff does
// not score that data-symbol name (winMsgMain matches with the same difference).
void msgWindow_Main(WindowEntry* window) {
	smartEntry* smart = window->userdata;
	MessageData* data = smart->address;
#ifdef MUST_MATCH
	s32 value;
#endif

	if (data->field_0x7A50 != 0) {
		if (data->field_0x7A54 == 0) {
			if (data->field_0x4 & 8) {
				BtlUnit_ChangeTalkAnim(data->field_0x7A24);
			}
			else {
				npcSetTalkPose(data->text);
			}
			data->field_0x7A54 = 1;
		}
		data->field_0x7A50--;
		if (data->field_0x7A50 == 0) {
			if (((MessageData*)smart->address)->field_0x4 & 8) {
				BtlUnit_ChangeStayAnim(((MessageData*)smart->address)->field_0x7A24);
			}
			else {
				npcSetStayPose(((MessageData*)smart->address)->text);
			}
			data->field_0x7A54 = 0;
		}
	}

	switch (*(u16*)window->field_0x0) {
	case 5:
		if (!(window->flags & 8)) {
			s32 se = msgWindowOpen_SE_Data[window->type][0];
			if (se >= 0) {
				psndSFXOn((const char*)(se | 0x20000));
			}
		}
		*(OSTime*)(window->field_0x1C + 4) = gp->renderTime;
		*(u16*)window->field_0x0 = 6;
		/* fallthrough */
	case 6:
		window->flags |= 2;
		window->alpha = (s16)(OSTicksToMilliseconds(gp->renderTime - *(OSTime*)(window->field_0x1C + 4)) * 255 / 200);
		if (window->alpha > 0xff) {
			window->alpha = 0xff;
			*(u16*)window->field_0x0 = 1;
			data->field_0x18 = gp->renderTime;
			data->field_0x30 = gp->renderTime;
		}
		break;
	case 7:
		if (!(window->flags & 8)) {
			s32 se = msgWindowOpen_SE_Data[window->type][1];
			if (se >= 0) {
				psndSFXOn((const char*)(se | 0x20000));
			}
		}
		*(OSTime*)(window->field_0x1C + 4) = gp->renderTime;
		*(u16*)window->field_0x0 = 8;
		/* fallthrough */
	case 8:
		if (window->alpha == 0) {
			windowDelete(window);
			return;
		}
		window->alpha = 0xff - OSTicksToMilliseconds(gp->renderTime - *(OSTime*)(window->field_0x1C + 4)) * 255 / 200;
		if (window->alpha <= 0) {
			window->alpha = 0;
		}
		break;
	case 1:
		switch (msgMain(window->userdata)) {
		case 0:
			*(u16*)window->field_0x0 = 7;
			data = window->userdata->address;
			if (data->field_0x4 & 8) {
				BtlUnit_ChangeStayAnim(data->field_0x7A24);
			}
			else {
				npcSetStayPose(data->text);
			}
			break;
		case 1:
			*(u16*)window->field_0x0 = 2;
			window->flags &= ~2;
			data = window->userdata->address;
			if (data->field_0x4 & 8) {
				BtlUnit_ChangeStayAnim(data->field_0x7A24);
			}
			else {
				npcSetStayPose(data->text);
			}
			break;
		case 2:
			*(u16*)window->field_0x0 = 3;
			break;
		case 3:
			break;
		}
		break;
	case 3: {
#ifdef MUST_MATCH
		u32 mask;
		u32 button;

		mask = (((MessageData*)window->userdata->address)->field_0x4 & 0x100) ? 0x500 : 0x100;
		button = keyGetButtonTrg(0);
		if (button & (mask | 0x200)) {
#else
		u32 mask = (((MessageData*)window->userdata->address)->field_0x4 & 0x100) ? 0x500 : 0x100;
		if (keyGetButtonTrg(0) & (mask | 0x200)) {
#endif
			*(u16*)window->field_0x0 = 1;
			*(u16*)((u8*)data + 0xa) = 2;
			if (keyGetButtonTrg(0) & 0x200) {
				if (!(data->field_0x4 & 0x80)) {
					data->field_0x4 |= 4;
				}
			}
		}
		if (keyGetButtonTrg(0) & 0x10) {
			u32 ok;
			*(s32*)((u8*)data + 0x783C) = data->field_0x7840 - 1;
			{
				MessageData* d = window->userdata->address;
				(*(s32*)((u8*)d + 0x783C))--;
				if (*(s32*)((u8*)d + 0x783C) < 0) {
					*(s32*)((u8*)d + 0x783C) = (ok = 0);
				}
				else {
					ok = 1;
				}
			}
			if (ok) {
				*(u16*)window->field_0x0 = 9;
				*(OSTime*)(window->field_0x1C + 4) = gp->renderTime;
				psndSFXOn((const char*)0x20041);
				data->field_0x4 |= 0x40;
			}
		}
		break;
	}
	// The (MessageData*) cast here is a no-op that stops the compiler CSE-ing field_0x28 across the
	// OSTicksToMilliseconds call; the original reloads it.
	case 9: {
#ifdef MUST_MATCH
		value = ((s32*)data->pad_0x7844)[*(s32*)data->pad_0x783C];
#else
		s32 value = ((s32*)data->pad_0x7844)[*(s32*)data->pad_0x783C];
#endif
		if (data->field_0x28 < value) {
			break;
		}
		data->field_0x28 = (s32)((f32)((MessageData*)data)->field_0x28 - float_0p6_8041a570 * (f32)OSTicksToMilliseconds(gp->renderTime - *(OSTime*)(window->field_0x1C + 4)));
		*(OSTime*)(window->field_0x1C + 4) = gp->renderTime;
		if (data->field_0x28 > value) {
			break;
		}
		data->field_0x28 = value;
		*(u16*)window->field_0x0 = 0xb;
		break;
	}
	// Same no-op cast as case 9, on the field_0x28 update below.
	case 10: {
#ifdef MUST_MATCH
		value = ((s32*)data->pad_0x7844)[*(s32*)data->pad_0x783C];
#else
		s32 value = ((s32*)data->pad_0x7844)[*(s32*)data->pad_0x783C];
#endif
		if (data->field_0x28 > value) {
			break;
		}
		data->field_0x28 = (s32)((f32)((MessageData*)data)->field_0x28 + float_0p6_8041a570 * (f32)OSTicksToMilliseconds(gp->renderTime - *(OSTime*)(window->field_0x1C + 4)));
		*(OSTime*)(window->field_0x1C + 4) = gp->renderTime;
		if (data->field_0x28 < value) {
			break;
		}
		data->field_0x28 = value;
		if (*(s32*)((u8*)data + 0x783C) == data->field_0x7840 - 1) {
			*(u16*)window->field_0x0 = 3;
			data->field_0x4 &= ~0x40;
		}
		else {
			*(u16*)window->field_0x0 = 0xb;
		}
		break;
	}
	case 11: {
#ifdef MUST_MATCH
		u32 mask;
		u32 button;

		mask = (((MessageData*)window->userdata->address)->field_0x4 & 0x100) ? 0x500 : 0x100;
		button = keyGetButtonTrg(0);
		if (button & mask) {
#else
		u32 mask = (((MessageData*)window->userdata->address)->field_0x4 & 0x100) ? 0x500 : 0x100;
		if (keyGetButtonTrg(0) & mask) {
#endif
			*(u16*)window->field_0x0 = 0xa;
			*(OSTime*)(window->field_0x1C + 4) = gp->renderTime;
			{
				MessageData* d = window->userdata->address;
				(*(s32*)((u8*)d + 0x783C))++;
				if (*(s32*)((u8*)d + 0x783C) > d->field_0x7840 - 1) {
					*(s32*)((u8*)d + 0x783C) = d->field_0x7840 - 1;
				}
			}
			psndSFXOn((const char*)0x20041);
		}
		if (keyGetButtonTrg(0) & 0x200) {
			*(u16*)window->field_0x0 = 0xa;
			*(OSTime*)(window->field_0x1C + 4) = gp->renderTime;
			*(s32*)((u8*)data + 0x783C) = data->field_0x7840 - 1;
			psndSFXOn((const char*)0x20041);
		}
		if (keyGetButtonTrg(0) & 0x10) {
			u32 ok;
			*(u16*)window->field_0x0 = 9;
			*(OSTime*)(window->field_0x1C + 4) = gp->renderTime;
			{
				MessageData* d = window->userdata->address;
				(*(s32*)((u8*)d + 0x783C))--;
				if (*(s32*)((u8*)d + 0x783C) < 0) {
					*(s32*)((u8*)d + 0x783C) = (ok = 0);
				}
				else {
					ok = 1;
				}
			}
			if (ok) {
				psndSFXOn((const char*)0x20041);
			}
		}
		break;
	}
	}

	dispEntry(8, float_400_8041a530 - (f32)*(s16*)&window->field_0x4, 0, msgWindow_Disp, window);
}

void msgWindow_Clear_Main(WindowEntry* window) {
	MessageData* data = window->userdata->address;

	switch (*(u16*)window->field_0x0) {
	case 5:
		*(u16*)window->field_0x0 = 1;
		*(OSTime*)(window->field_0x1C + 4) = gp->renderTime;
		data->field_0x18 = gp->renderTime;
		data->field_0x30 = gp->renderTime;
		/* fallthrough */
	case 1:
	case 6:
		window->flags |= 2;
		if (window->alpha < 0xff) {
			window->alpha = (s16)(OSTicksToMilliseconds(gp->renderTime - *(OSTime*)(window->field_0x1C + 4)) * 255 / 200);
			if (window->alpha > 0xff) {
				window->alpha = 0xff;
			}
		}
		if (*(u16*)window->field_0x0 != 2) {
			s32 ret = msgMain(window->userdata);
			switch (ret) {
			case 0:
				*(u16*)window->field_0x0 = 7;
				*(OSTime*)(window->field_0x1C + 4) = gp->renderTime;
				data = window->userdata->address;
				if (data->field_0x4 & 8) {
					BtlUnit_ChangeStayAnim(data->field_0x7A24);
				}
				else {
					npcSetStayPose(data->text);
				}
				break;
			case 1:
				*(u16*)window->field_0x0 = 2;
				window->flags &= ~2;
				data = window->userdata->address;
				if (data->field_0x4 & 8) {
					BtlUnit_ChangeStayAnim(data->field_0x7A24);
				}
				else {
					npcSetStayPose(data->text);
				}
				break;
			case 2:
				*(u16*)window->field_0x0 = 3;
				break;
			case 3:
				break;
			}
		}
		break;
	case 7:
		if (window->alpha == 0) {
			windowDelete(window);
			return;
		}
		window->alpha = 0xff - OSTicksToMilliseconds(gp->renderTime - *(OSTime*)(window->field_0x1C + 4)) * 255 / 200;
		if (window->alpha <= 0) {
			window->alpha = 0;
		}
		break;
	}
	dispEntry(8, 400.0f - (f32)*(s16*)&window->field_0x4, 0, msgWindow_Disp, window);
}

char* msgGetCommand(char* str, char* arg1, char* arg2) {
	s32 i;
	char c;
	s32 state = 0;

	for (i = 0; i < 0x80; i++) {
		c = *str;
		switch ((s8)c) {
		case '>':
			if (state == 0) {
				arg1[0] = 0;
			}
			else {
				arg2[0] = 0;
			}
			return str + 1;
		case ' ':
			if (state == 0) {
				arg1[0] = 0;
				state = 1;
			}
			else {
				*arg2++ = c;
			}
			break;
		default:
			if (state == 0) {
				*arg1++ = c;
			}
			else {
				*arg2++ = c;
			}
			break;
		}
		str++;
	}
	return NULL;
}

// NONMATCHING (4.75%): 25 instructions differ, 1271 missing, plus register differences.
void msgAnalize(smartEntry* smart, const char* msg) {
	MessageData* data;
	MsgDataEntry* entry;
	WindowEntry* window;
	s32 smth_7A30;
	char check;
	BOOL checkkanji;
	u16 glyph, width;
	u32 r23;

	data = smart->address;
	window = data->window;
	r23 = 0;
	//r1+0x50 = 1.0f;
	entry = &data->entries[data->entryId];
	if (window->type < 0) {
		window->type = 0;
	}
	smth_7A30 = data->field_0x7A30;
	//s16 r1+0x234 = 0;
	//s16 r1+0x232 = 0;
	//s16 r1+0x230 = 0;
	//s32 r1+0x54 = 0;
	switch (window->type) {
	case 3:
	case 8:
		switch (window->type) {
		case 3:
			entry->field_0x0 = 0xDCC8DCFF; //(GXColor){0xDC, 0xC8, 0xDC, 0xFF};
			break;
		case 8:
			entry->field_0x0 = 0x00D914FF;//(GXColor){0x00, 0xD9, 0x14, 0xFF};
			break;
		}
		entry->glyph = 0xFFF7;
		entry->field_0xC = smth_7A30;
		entry++; //next entry
		data->entryId++; //next entry
		break;

	default:
		break;
	}

	check = *msg;
	if ((check < 0x81 && check <= 0x9F) || (check < 0xE0 && check > 0xFC)) {
		checkkanji = TRUE;
	}
	else {
		checkkanji = FALSE;
	}
	if (checkkanji) {
		glyph = kanjiSearch(*(u16*)msg);
		width = kanjiGetWidth(glyph);
		entry->glyph = glyph;


		entry->field_0x0 = r23;
	}
}


// NONMATCHING (95.15%): 1 extra instruction; the compiler materialises the array address into r0 before
// saving r31 instead of using addi r31 directly.
s32 msgIconStr2ID(const char* str) {
	MsgIcon* icon;
	u32 i;

	icon = msgIcon;
	for (i = 0; i < 18; i++, icon++) {
		if (!strcmp(icon->name, str)) {
			return msgIcon[i].value;
		}
	}
	return -1;
}

// NONMATCHING (91.60%): the GXSetFog/waku setup allocates its stack temps differently.
void selectWindow_Disp(CameraId camId, void* param) {
	WindowEntry* window = param; //cast to correct type
	MessageData* data = window->userdata->address;
	GXColor color = { 0xFF, 0xFF, 0xFF, 0 };
	Vec pos;

	GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, unk_80422fe4);

	color.a = window->alpha;
	windowDispGX_Waku_col(0, color, window->x, window->y, window->height, window->width, 30.0f);

	msgDisp(window->userdata, (u8)window->alpha, 40.0f + window->x, window->y - 25.0f);

	pos.x = 2.0f + window->x;
	data->field_0x7A48 += 0.3f * (window->y - (f32)((s8)data->field_0x7A43 * 0x1F + 0x3A) - data->field_0x7A48);
	pos.y = data->field_0x7A48;
	pos.z = 0.0f;

	iconDispGxAlpha(&pos, 0x14, 0x1F0, (u8)window->alpha, 1.0f);
}
