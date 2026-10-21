#include "driver/msgdrv.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "driver/npcdrv.h"
#include "driver/windowdrv.h"
#include "manager/dvdmgr.h"
#include "manager/fontmgr.h"
void FontDrawColor(GXColor color);
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "memory.h"
#include "system.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern GlobalWork* gp;

#ifdef MUST_MATCH
// .sbss2: the default .sbss2 is only used for compiler-generated pool templates. Declared in reverse, as
// MWCC emits these in reverse source order.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80422fe4;
__declspec(section ".sdata") GXColor unk_80422fe0;
#pragma section ".sdata" ".sbss"
#else
GXColor unk_80422fe0;
GXColor unk_80422fe4;
#endif
extern GXColor dat_8041a4f0;
extern GXColor dat_8041a4f4;
extern GXColor dat_8041a4f8;
extern GXColor dat_8041a4fc;
extern GXColor dat_8041a510;

const char R_no_messages_JP[] = "<col ff0000ff>\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x82\xAA\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1</col>\n<key>"; /*"メッセージがありません"*/
const char R_no_messages_US[] = "<col ff0000ff>No messages.<col>\n<key>";

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

//.data
s32 msgWindowOpen_SE_Data[12][2] = {
	{0x16, 0x17},
	{0x1A, 0x1B},
	{0x20, 0x21},
	{0x1C, 0x1D},
	{0x20, 0x21},
	{0x1C, 0x1D},
	{0x18, 0x19},
	{0x22, 0x23},
	{0x24, 0x25},
	{0x26, 0x27},
	{0x1E, 0x1F},
	{-1, -1}
};

//.sbss (declared in reverse: MWCC emits these in reverse source order, and the original's order is
// active, gap_08, msgSortDvdData, msgSEflag, msgWork)
MessageWork msgWork;
BOOL msgSEflag; //msgSEInit and related functions are unused
void* msgSortDvdData; //TODO: re-type
u16 gap_08_80418812_sbss;
u16 active; //TODO: re-type

//.sdata
MessageWork* msgw = &msgWork;
const char* ErrMessage[2] = {
	R_no_messages_JP,
	R_no_messages_US
};
const char* paperPoseTbl[6] = {
	"PM_Y_1B", "PM_Y_2B", "PM_F_1B",
	"PM_F_3B", "PM_C_1B", "PM_C_2B"
};
const char* language_dir[2] = {"msg", "emsg"};

// 4-byte zero word that ends the unit's .sdata.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_804103F4_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset





//local prototypes

// Matching. Three things have to hold for its `hoshi_2`/`HOS_C_1`/`global` offsets
// (0x208/0x210/0x218 from the R_no_messages_JP base) to come out right, all of them outside the
// body: (1) `#include "math_ext.h"` instead of the SDK <math.h>, whose `extern inline`
// sqrtf/sqrt otherwise dump four 0.5/3.0 localstatics (0x20 bytes) into .rodata; (2) msgWindow_Entry
// (whose `(f32)` cast of an unsigned value is what makes MWCC pool the second i2f magic
// 0x4330000000000000) generated before msgAnalize, whose strcmp strings are keyxon..; (3) no
// duplicate i2f magic. (2) is why msgWindow_Entry sits after msgAnalize in this source: function
// bodies are emitted in reverse source order, so putting it later makes MWCC generate it first and
// places the magic at 0x120, right after 0x4330000080000000. Moving it back costs msgInit 0.06%.
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
#ifdef MUST_MATCH
	// the original's string object is padded to the next word: the extra NUL spells the pad out
	msgLoad("global\0", 1);
#else
	msgLoad("global", 1);
#endif
}

void msgLoad(const char* filename, s32 index) {
	// This declaration order is load-bearing: MWCC colours the callee-saved registers r27-r31 in
	// declaration order, and only this order gives the original's assignment (entry=r31, r30/address
	// =r30, msgptr=r29, lookup=r29, dvd=r28, msgcount=r28, i=r28, r27/offset=r27; the two loop temps
	// must also be split into their own declarations, not `u32 r27, r30;`). Any other order moves
	// dvd/offset/lookup and loses the match.
	char dvdpath[128]; //TODO: double check size
	u32 r30;
	MessageEntry* entry;
	MsgLookupEntry* lookup;
	int i;
	s32 msgcount;
	DVDEntry* dvd;
	char filepath[64];
	char* msgptr;
	u32 r27;

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

	if (entry->address == NULL) {
		entry->length = 0;
		entry->count = 0;
	}
	else {
		msgptr = entry->address;
		msgcount = 0;
		while (*(s8*)msgptr) {
			msgptr += strlen(msgptr) + 1;
			msgcount++;
		}
		entry->count = (u32)(msgcount / 2); //tag, message
	}

	if (entry->lookup) {
		smartFree(entry->lookup);
	}
	if ((s32)entry->count == 0) {
		entry->lookup = NULL;
	}
	else {
		r27 = 0;
		entry->lookup = smartAlloc(entry->count * sizeof(MsgLookupEntry), 0);
		r30 = (u32)entry->address;
		lookup = entry->lookup->address;
		i = 0;
		while (i < (s32)entry->count) {
			lookup->startaddr = r27;
			r27 += strlen((const char*)r30) + 1;
			r30 += strlen((const char*)r30) + 1;
			lookup->endaddr = r27;
			r27 += strlen((const char*)r30) + 1;
			r30 += strlen((const char*)r30) + 1;
			i++;
			lookup++;
		}
		msgSortDvdData = entry->address;
		qsort(entry->lookup->address, entry->count, sizeof(MsgLookupEntry), msg_compare);
	}
}

// Append a value to the message history array at 0x7844, skipping duplicates of the last entry.
static void msgHistory(smartEntry* smart, s32 value) {
	MessageData* data = smart->address;

	if (!(data->flags & 0x200) && data->historyCount < 0x6e
	    && (data->historyCount == 0
	        || data->history[data->historyCount - 1] != value)) {
		data->history[data->historyCount++] = value;
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

	if (data->flags & 2) {
		if (data->callback) {
			data->callback(0, smart);
		}
		data->flags &= ~2;
		msgHistory(smart, 0);
	}

	if (data->scrollTarget > data->scrollY) {
		data->scrollY += 0.36f * (f32)OSTicksToMilliseconds(gp->renderTime - data->lastFrameTime);
		if (!(data->flags & 0x80)) {
			i = (((MessageData*)smart->address)->flags & 0x100) ? 0x500 : 0x100;
			mask = i | 0x200;
			button = keyGetButtonTrg(0);
			if (button & mask) {
				data->flags |= 4;
			}
		}
		if (!(data->flags & 0x10) && (data->flags & 0x4)) {
			data->scrollY += 0.36f * (f32)OSTicksToMilliseconds(gp->renderTime - data->lastFrameTime);
		}
		if (data->scrollTarget < data->scrollY) {
			data->scrollY = data->scrollTarget;
		}
		data->lastFrameTime = gp->renderTime;
		return 3;
	}

	switch (data->choiceState) {
	case 1:
		return 2;
	case 2:
		data->choiceMade = 1;
		data->choiceIndex++;
		data->choiceState = 0;
		psndSFXOn((const char*)0x2003D);
		data->textTime += 0x64;
		if (data->callback) {
			data->callback(3, smart);
		}
		break;
	default:
		time = (s32)OSTicksToMilliseconds(gp->renderTime - data->lastFrameTime);
		if (!(data->flags & 0x80)) {
			i = (((MessageData*)smart->address)->flags & 0x100) ? 0x500 : 0x100;
			mask = i | 0x200;
			button = keyGetButtonTrg(0);
			if (button & mask) {
				data->flags |= 4;
			}
		}
		if (!(data->flags & 0x10) && (data->flags & 0x4)) {
			time *= 0x64;
		}
		data->textTime += time;
		break;
	}

	data->lastFrameTime = gp->renderTime;
	entry = &data->entries[data->entryPos];
	i = data->entryPos;
	while (i < 0x500) {
		if ((u32)entry->startTime > (u32)data->textTime) {
			break;
		}
		switch (entry->glyph) {
		case 0xFFFF:
			if (data->callback) {
				data->callback(1, smart);
			}
			data->entryPos = i;
			return 0;
		case 0xFFFE:
			if (entry->x != data->choiceIndex) {
				break;
			}
			data->choiceState = 1;
			data->textTime = entry->startTime;
			data->flags &= ~4;
			if (data->callback) {
				data->callback(2, smart);
			}
			break;
		case 0xFFFD:
			data->scrollTarget -= entry->y;
			data->textTime = entry->startTime;
			msgHistory(smart, data->scrollTarget);
			break;
		case 0xFFFC:
			data->entryPos = i + 1;
			data->textTime = entry->startTime;
			data->flags &= ~4;
			return 1;
		case 0xFFFB:
			if (data->callback) {
				data->callback(4, smart);
			}
			break;
		case 0xFFFA:
			data->field_0x7A28 = entry->arg;
			break;
		case 0xFFF9:
			data->sfxVolume = entry->arg;
			break;
		case 0xFFF5:
			if (entry->arg) {
				data->flags |= 0x80;
				data->flags &= ~4;
				data->textTime = entry->startTime;
			}
			else {
				data->flags &= ~0x80;
			}
			break;
		case 0xFFF4:
		case 0xFFF8:
			break;
		case 0xFFF3:
			{
				NpcEntry* npc = (NpcEntry*)entry->arg;
				if (npc == NULL) {
					npc = npcNameToPtr(data->text);
				}
				animPoseSetPaperAnimGroup(npc->poseId, "p_b_st", TRUE);
				animPosePaperPeraOn(npc->poseId);
				animPoseSetPaperAnim(npc->poseId, paperPoseTbl[entry->startTime], TRUE);
			}
			break;
		case 0xFFF2:
			{
				NpcEntry* npc = (NpcEntry*)entry->arg;
				if (data->choiceState != 0) {
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
			if (data->callback) {
				data->callback(5, smart);
			}
			if (entry->arg & 0x10) {
				if (((MessageData*)smart->address)->window->type != WINDOW_TYPE_SMALL && !msgSEflag) {
					psndSFXOnVol(0x20044, ((MessageData*)smart->address)->sfxVolume);
					msgSEflag = 1;
				}
			}
			else if (entry->arg & 0x8) {
				if (((MessageData*)smart->address)->window->type != WINDOW_TYPE_SYSTEM) {
					((MessageData*)smart->address)->talkAnimTimer = 0xf;
				}
				if (((MessageData*)smart->address)->window->type != WINDOW_TYPE_SMALL && !msgSEflag) {
					psndSFXOnVol(0x20044, ((MessageData*)smart->address)->sfxVolume);
					msgSEflag = 1;
				}
			}
			break;
		}
		i++;
		entry++;
	}
	data->entryPos = i;
	if (data->choiceState != 0) {
		return 2;
	}
	return 3;
}










// math_ext.h declares sin/cos as out-of-line MSL entry points. Do NOT include the SDK <math.h>
// here: its `extern inline` sqrtf/sqrt emit four 0.5/3.0 localstatics into .rodata (0x20 bytes)
// that the original unit does not have, shifting every later string (and msgInit's offsets) by
// 0x20. It also has to come after msgInit, as it did before.
#include "math_ext.h"
#pragma pool_data on // the original addresses its .rodata through one base register
// Matching. The `idx`/`base` arithmetic is written inline at each array access (a `base` pointer local let the
// allocator coalesce it the wrong way), and the `ca`/`sa` of the two cos/sin branches are one shared pair
// hoisted to the default case so the allocator keeps them in the retail registers; `angle` stays local to
// each branch.
BOOL msgDisp(smartEntry* smart, u8 alpha, f32 x, f32 y) {
	MessageData* data = smart->address;
	MsgDataEntry* entry = data->entries;
	s32 i;
	BOOL result = FALSE;

	FontDrawStart_alpha(alpha);

	for (i = 0; i < data->entryPos; i++, entry++) {
		switch (entry->glyph) {
		case 0xFFFF:
			return FALSE;
		case 0xFFFE:
			if (data->choiceIndex == entry->x) {
				result = TRUE;
			}
			break;
		case 0xFFF7: {
			GXColor color = *(GXColor*)&entry->arg;
			FontDrawColor(color);
			break;
		}
		case 0xFFF6:
			if (entry->arg) {
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
			rotY = entry->field_0x14;
			if (rotY >= 90.0f && rotY <= 270.0f) {
				PSMTXScaleApply(mtx, mtx, 1.0f, 1.0f, -1.0f);
			}
			PSMTXTransApply(mtx, mtx,
				x + (f32)entry->x,
				y + (f32)data->scrollY + (f32)entry->y - 40.f,
				0.0f);
			animPoseMain(entry->arg);
			animPoseDrawMtx(entry->arg, mtx, 2,
				entry->field_0x14, entry->scale);
			FontDrawStart_alpha(alpha);
			FontDrawColor(color);
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
			f32 px = x + (f32)entry->x;
			f32 py = y + (f32)data->scrollY + (f32)entry->y;
			BOOL tev = FALSE;
			u8 fade = 0xFF;
			// The two rotating-code branches below share this cos/sin pair; one shared variable pair is
			// what puts them in the retail registers (each branch keeps its own `angle`).
			f32 ca;
			f32 sa;

			if (data->flags & 0x40) {
				s32 cur = data->scrollY;
				s32 idx = data->historyCursor;
				s32 e8 = entry->y;
				// Window queue: historyCursor indexes history[], historyCount is its length; history[idx],
				// history[idx + 1] and history[idx + 2] are the entries the fade checks.
				GXColor color;

				if (cur == data->history[idx]) {
					if (e8 + data->history[idx + 1] <= 0) {
						break;
					}
					tev = TRUE;
				}
				else if (cur < data->history[idx]) {
					if (e8 + data->history[idx] > 0) {
						tev = TRUE;
					}
					else {
						s32 t = data->history[idx] - cur;
						if (t > 100) {
							t = 100;
						}
						fade = (u32)((100 - t) * 255 / 100);
						if (idx != data->historyCount - 1) {
							if (e8 + data->history[idx + 1] > 0) {
								tev = TRUE;
							}
							else {
								break;
							}
						}
						else {
							color = FontGetDrawColor();
							color.a = (color.a * fade) / 255;
							GXSetTevColor(GX_TEVREG0, color);
						}
					}
				}
				else {
					if (e8 + data->history[idx + 1] > 0) {
						tev = TRUE;
					}
					else {
						s32 t = cur - data->history[idx];
						if (t > 100) {
							t = 100;
						}
						fade = (u32)(t * 255 / 100);
						if (idx != data->historyCount - 2) {
							if (e8 + data->history[idx + 2] > 0) {
								tev = TRUE;
							}
							else {
								break;
							}
						}
						else {
							color = FontGetDrawColor();
							color.a = (color.a * fade) / 255;
							GXSetTevColor(GX_TEVREG0, color);
						}
					}
				}
			}

			if (entry->glyph == 0xFFF8) {
				GXColor color = FontGetDrawColor();
				Vec pos;
				pos.x = 20.f + (x + (f32)entry->x);
				pos.y = (f32)entry->iconOffsetY + (((f32)entry->y + (y + (f32)data->scrollY)) - 40.f);
				pos.z = 0.0f;
				iconDispGxAlpha(&pos, 0x10, entry->arg & 0xFFFF,
					(u8)((fade * alpha) / 255), entry->scale);
				FontDrawStart_alpha(alpha);
				FontDrawColor(color);
				break;
			}

			if ((f32)(entry->y + data->scrollY) >= 32.0f * entry->scale - 10.f) {
				break;
			}

			if (tev) {
				GXColor color = *(GXColor*)&dat_8041a4f0;
				switch (data->window->type) {
				case WINDOW_TYPE_SYSTEM:
				case WINDOW_TYPE_TEC:
				case WINDOW_TYPE_MAJO:
					color = *(GXColor*)&dat_8041a4f4;
					break;
				}
				color.a = fade;
				GXSetTevColor(GX_TEVREG0, color);
				FontDrawScale(entry->scale);
				FontDrawCode(entry->glyph, px, py);
				FontDrawColor_();
				break;
			}

			if (entry->arg & 1) {
				u16 half = kanjiGetWidth(entry->glyph) >> 1;
				f32 fx = px;
				f32 fy = py;
				Mtx mtx;

				if (entry->arg & 2) {
					fx = px + (f32)(irand(10000) % 3);
					fy = py + (f32)(irand(10000) % 3);
				}
				PSMTXTrans(mtx, (f32)(-half), 12.0f, 0.0f);
				PSMTXScaleApply(mtx, mtx, entry->field_0x14, entry->field_0x14, entry->field_0x14);
				PSMTXTransApply(mtx, mtx, (f32)half, -12.0f, 0.0f);
				PSMTXScaleApply(mtx, mtx, entry->scale, entry->scale, entry->scale);
				PSMTXTransApply(mtx, mtx, fx, fy, 0.0f);
				FontDrawCodeMtx(mtx, entry->glyph);
				entry->field_0x14 = 0.5f * ((entry->field_0x14) - 1.0f) + 1.0f;
			}
			else if (entry->arg & 2) {
				f32 fx = px + (f32)(irand(10000) % 3);
				f32 fy = py + (f32)(irand(10000) % 3);
				FontDrawScale(entry->scale);
				FontDrawCode(entry->glyph, fx, fy);
			}
			else if (entry->arg & 4) {
				f32 t = (f32)OSTicksToMilliseconds(gp->renderTime - data->effectStart);
				f32 angle = 0.01f * t - 0.02f * px;
				ca = (f32)cos(angle);
				sa = (f32)sin(angle);
				FontDrawScale(entry->scale);
				FontDrawCode(entry->glyph, 3.2f * ca + px, py - 3.2f * sa);
			}
			else if (entry->arg & (0x100 << data->selectCursor)) {
				f32 t = (f32)OSTicksToMilliseconds(gp->renderTime - data->effectStart);
				f32 angle = 0.01f * t - 0.02f * px;
				f32 fx;
				f32 fy;
				ca = (f32)cos(angle);
				fx = 3.2f * ca + px;
				sa = (f32)sin(angle);
				fy = py - 3.2f * sa;

				FontDrawScale(entry->scale);
				{
					GXColor color = *(GXColor*)&dat_8041a4f8;
					FontDrawColor(color);
				}
				FontDrawCode(entry->glyph, fx, fy);
				FontDrawEdge();
				FontDrawRainbowColor();
				{
					GXColor color2 = *(GXColor*)&dat_8041a4fc;
					FontDrawColor(color2);
				}
				FontDrawCode(entry->glyph, fx - 4.0f, fy + 4.0f);
				FontDrawEdgeOff();
				FontDrawRainbowColorOff();
				FontDrawColorIDX(0);
			}
			else {
				FontDrawScale(entry->scale);
				FontDrawCode(entry->glyph, px, py);
			}
			break;
		}
		}
	}
	return result;
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

static inline u16 msgGetKanji(const char* str, s32 off) {
	return kanjiSearch(*(const u16*)(str + off));
}

// The R-ruby loops read each kanji with msgGetKanji(tagName, skip + i * 2). The u32 counters keep MWCC's
// frontend from strength-reducing `i * 2`, so the backend builds the offset and pointer induction variables
// the target has; `(s32)i < half` is the signed compare. Every width is `scale * (f32)(kanjiGetWidth(g) + n)`:
// the call inside the conversion is evaluated before `scale` is loaded, which gives the target's fmuls operand
// order. opt_lifetimes off keeps loop 1's `total`, the post-loop total and the loop-2 glyph in one register.
#pragma opt_lifetimes off
void msgAnalize(smartEntry* smart, const char* msg) {
	char tagName[128];
	char tagArg[128];
	char buf4[32];
	char buf3[32];
	char buf2[32];
	char buf1[32];
	char buf0[32];
	s32 var54;
	f32 scale;
	f32 dyn;
	s32 posX;
	s32 posY;
	s32 wait;
	s32 iconId;
	s32 icon3;
	s32 icon4;
	s32 icon5;
	f32 iconScale;
	s32 animA;
	s32 animB;
	s32 animC;
	f32 animF;
	s32 paperId;
	s16 wposX;
	s16 wposY;
	s16 wposH;
	s16 wposW;
	s16 sel2;
	s16 sel1;
	u16 w;
	u16 h;
	f32 maxScale;
	f32 base;
	u32 skip;
	char* pa;
	NpcEntry* npc;
	s32 total;
	u32 j;
	u16 glyph;
	s32 len;
	char* pn;
	u32 i;
	const char* after;
	char* q;
	MessageData* data;
	WindowEntry* window;
	MsgDataEntry* entry;
	s16 ex;
	s16 ey;
	s16 r25;
	s32 r24;
	u32 flags;
	s32 half;
	u32 check;
	u32 n;
	s32 mode;
	s8 tch;
	char tchc;
	s16 var[3];

	data = smart->address;
	window = data->window;
	flags = 0;
	entry = &data->entries[data->entryId];
	maxScale = 0.0f;
	scale = 1.0f;
	if (window->type < 0) {
		window->type = WINDOW_TYPE_NORMAL;
	}
	var[2] = 0;
	r24 = data->timeCursor;
	var[1] = data->savedX;
	ex = data->penX;
	var[0] = data->savedY;
	ey = data->penY;
	var54 = data->charDelay;

	switch (window->type) {
	case WINDOW_TYPE_SYSTEM:
	case WINDOW_TYPE_TEC:
		switch (window->type) {
		case WINDOW_TYPE_SYSTEM:
			entry->arg = dat_8041a500;
			break;
		case WINDOW_TYPE_TEC:
			entry->arg = dat_8041a504;
			break;
		}
		entry->glyph = 0xFFF7;
		entry->startTime = r24;
		entry++;
		data->entryId++;
		break;

	default:
		break;
	}

	for (;;) {
		check = (u8)*msg;
		if (_ismbblead(check)) {
			glyph = kanjiSearch(*(const u16*)msg);
			{
			s32 exinc = (s32)(scale * (f32)(kanjiGetWidth((u16)glyph) + 4));
			entry->glyph = glyph;
			entry->x = ex;
			entry->y = (s16)((s32)ey - (s32)(10.f * scale));
			entry->scale = scale;
			entry->arg = flags;
			if (*(const u16*)msg == 0x8145) {
				entry->arg |= 0x10;
			}
			else if (*(const u16*)msg != 0x8140 && *(const u16*)msg != 0x8151) {
				entry->arg |= 8;
			}
			if (flags & 1) {
				entry->field_0x14 = dyn;
			}
			if (maxScale < scale) {
				maxScale = scale;
			}
			entry->startTime = r24;
			ex = (s16)((s32)ex + exinc);
			}
			r24 += var54;
			msg += 2;
			entry++;
			data->entryId++;
			continue;
		}

		switch ((s8)check) {
		case 0:
			entry->glyph = 0xFFFF;
			entry->startTime = r24;
			data->timeCursor = r24;
			data->penX = ex;
			data->penY = ey;
			data->savedX = var[1];
			data->savedY = var[0];
			return;

		case '\n':
			entry->glyph = 0xFFFB;
			ex = 0;
			entry->startTime = r24;
			if (0.0f == maxScale) {
				maxScale = 1.0f;
			}
			{
				f32 dy = 32.0f * maxScale;

				ey = (s16)((f32)ey - dy);
				var[0] = (s16)((f32)var[0] - dy);
			}
			r24 += var54;
			msg++;
			entry++;
			data->entryId++;
			maxScale = 0.0f;
			if (data->flags & 0x20) {
				flags &= ~0xFF00;
				flags |= 0x100 << (s8)data->selectCount;
				data->selectCount++;
			}
			continue;

		case '<':
			q = (char*)msg + 1;
			pn = tagName;
			pa = tagArg;
			mode = 0;
			for (n = 0x80; n != 0; n--) {
				tchc = *q;
				tch = tchc;
				switch (tch) {
				case '>':
					if (mode == 0) {
						*pn = 0;
					}
					else {
						*pa = 0;
					}
					after = q + 1;
					goto parsed; // out of the switch and the loop, past the unterminated-tag store
				case ' ':
					if (mode == 0) {
						*pn = 0;
						mode = 1;
					}
					else {
						*pa++ = tchc;
					}
					break;
				default:
					if (mode == 0) {
						*pn++ = tchc;
					}
					else {
						*pa++ = tchc;
					}
					break;
				}
				q++;
			}
			after = NULL;
		parsed:
			msg = after;

			if (strcmp(tagName, "k") == 0 || strcmp(tagName, "key") == 0) {
				entry->glyph = 0xFFFE;
				entry->x = var[1];
				entry->startTime = r24;
				r24 += 100;
				entry++;
				data->entryId++;
				var[1]++;
			}
			else if (strcmp(tagName, "keyxon") == 0) {
				data->flags |= 0x100;
			}
			else if (strcmp(tagName, "p") == 0 || strcmp(tagName, "scrl_auto") == 0) {
				if (var[0] != 0) {
					entry->glyph = 0xFFFD;
					entry->y = (s16)(var[0] - 3);
					ey -= 3;
					entry->startTime = r24;
					r24 += 50;
					entry++;
					data->entryId++;
					var[2] = (s16)(ey - 3);
					var[0] = 0;
				}
			}
			else if (strcmp(tagName, "o") == 0 || strcmp(tagName, "once_stop") == 0) {
				entry->glyph = 0xFFFC;
				entry->startTime = r24;
				r24 += 50;
				entry++;
				data->entryId++;
			}
			else if (strcmp(tagName, "pos") == 0) {
				sscanf(tagArg, "%d%d", &posX, &posY);
				{
					s32 y = posY;

					ex = (s16)posX;
					if (y != 999) {
						ey = (s16)(var[2] - y);
					}
					var[0] = (s16)(-y);
				}
			}
			else if (strcmp(tagName, "scale") == 0) {
				sscanf(tagArg, "%f", &scale);
			}
			else if (strcmp(tagName, "/scale") == 0) {
				scale = 1.0f;
			}
			else if (strcmp(tagName, "wait") == 0) {
				sscanf(tagArg, "%d", &wait);
				r24 += wait;
			}
			else if (strcmp(tagName, "speed") == 0) {
				sscanf(tagArg, "%d", &var54);
			}
			else if (strcmp(tagName, "/speed") == 0) {
				var54 = data->charDelay;
			}
			else if (strcmp(tagName, "dynamic") == 0) {
				sscanf(tagArg, "%f", &dyn);
				flags |= 1;
			}
			else if (strcmp(tagName, "/dynamic") == 0) {
				flags &= ~1;
			}
			else if (strcmp(tagName, "shake") == 0) {
				flags |= 2;
			}
			else if (strcmp(tagName, "/shake") == 0) {
				flags &= ~2;
			}
			else if (strcmp(tagName, "wave") == 0) {
				flags |= 4;
			}
			else if (strcmp(tagName, "/wave") == 0) {
				flags &= ~4;
			}
			else if (strcmp(tagName, "col") == 0) {
				sscanf(tagArg, "%x", &entry->arg);
				entry->glyph = 0xFFF7;
				entry->startTime = r24;
				entry++;
				data->entryId++;
			}
			else if (strcmp(tagName, "/col") == 0) {
				entry->arg = 0xFF;
				entry->glyph = 0xFFF7;
				entry->startTime = r24;
				entry++;
				data->entryId++;
			}
			else if (strcmp(tagName, "noise") == 0) {
				entry->arg = 1;
				entry->glyph = 0xFFF6;
				entry->startTime = r24;
				entry++;
				data->entryId++;
			}
			else if (strcmp(tagName, "/noise") == 0) {
				entry->arg = 0;
				entry->glyph = 0xFFF6;
				entry++;
				data->entryId++;
			}
			else if (strcmp(tagName, "dkey") == 0) {
				entry->arg = 1;
				entry->glyph = 0xFFF5;
				entry->startTime = r24;
				entry++;
				data->entryId++;
			}
			else if (strcmp(tagName, "/dkey") == 0) {
				entry->arg = 0;
				entry->glyph = 0xFFF5;
				entry->startTime = r24;
				entry++;
				data->entryId++;
			}
			else if (strcmp(tagName, "plate") == 0) {
				window->type = WINDOW_TYPE_PLATE;
				var54 = 0;
				data->charDelay = 0;
			}
			else if (strcmp(tagName, "system") == 0) {
				window->type = WINDOW_TYPE_SYSTEM;
				entry->arg = dat_8041a508;
				entry->glyph = 0xFFF7;
				entry->startTime = r24;
				entry++;
				data->entryId++;
			}
			else if (strcmp(tagName, "tec") == 0) {
				window->type = WINDOW_TYPE_TEC;
				entry->arg = dat_8041a50c;
				entry->glyph = 0xFFF7;
				entry->startTime = r24;
				entry++;
				data->entryId++;
			}
			else if (strcmp(tagName, "majo") == 0) {
				window->type = WINDOW_TYPE_MAJO;
			}
			else if (strcmp(tagName, "diary") == 0) {
				window->type = WINDOW_TYPE_DIARY;
			}
			else if (strcmp(tagName, "housou") == 0) {
				window->type = WINDOW_TYPE_HOUSOU;
			}
			else if (strcmp(tagName, "boss") == 0) {
				window->type = WINDOW_TYPE_BOSS;
			}
			else if (strcmp(tagName, "kanban") == 0) {
				window->type = WINDOW_TYPE_KANBAN;
				var54 = 0;
				data->charDelay = 0;
			}
			else if (strcmp(tagName, "small") == 0) {
				window->type = WINDOW_TYPE_SMALL;
				data->flags |= 0x10;
			}
			else if (strcmp(tagName, "clear") == 0) {
				window->type = WINDOW_TYPE_CLEAR;
			}
			else if (strcmp(tagName, "wpos") == 0) {
				sscanf(tagArg, "%hd%hd%hd%hd", &wposX, &wposY, &wposH, &wposW);
				window->flags |= 4;
				window->x = (f32)wposX;
				window->y = (f32)wposY;
				window->height = (f32)wposH;
				window->width = (f32)wposW;
			}
			else if (strcmp(tagName, "select") == 0) {
				window->type = WINDOW_TYPE_SELECT;
				data->flags |= 0x20;
				sscanf(tagArg, "%hd%hd%hd%hd%hd", &sel2, &sel1, &data->selectWidth, &data->selectY);
				data->selectInit = (s8)sel2;
				data->field_0x7A42 = (s8)sel1;
				data->selectCount = 1;
				flags |= 0x100;
				var54 = 0;
			}
			else if (strcmp(tagName, "se") == 0) {
				sscanf(tagArg, "%d", &entry->arg);
				entry->glyph = 0xFFFA;
				entry++;
				data->entryId++;
			}
			else if (strcmp(tagName, "vol") == 0) {
				sscanf(tagArg, "%d", &entry->arg);
				entry->glyph = 0xFFF9;
				entry++;
				data->entryId++;
			}
			else if (strcmp(tagName, "nowinse") == 0) {
				window->flags |= 8;
			}
			else if (strcmp(tagName, "icon") == 0) {
				s32 isHM = 0;

				if ((s8)tagArg[0] >= '0' && (s8)tagArg[0] <= '9') {
					sscanf(tagArg, "%d%f%d%d%d", &iconId, &iconScale, &icon3, &icon4, &icon5);
				}
				else {
					sscanf(tagArg, "%s%f%d%d%d", buf4, &iconScale, &icon3, &icon4, &icon5);
					if (strcmp(buf4, "HM") == 0) {
						isHM = 1;
					}
					iconId = msgIconStr2ID(buf4);
					if (iconId < 0) {
						iconId = 0;
					}
				}
				if (isHM == 0) {
					s16 oldEx = ex;

					entry->arg = (u32)iconId;
					iconGetWidthHight(&w, &h, (u16)iconId);
					entry->glyph = 0xFFF8;
					ex += 40;
					entry->x = (s16)((f32)icon3 + (0.5f * (f32)w * iconScale + (f32)oldEx));
					entry->y = (s16)((f32)ey - (f32)h * iconScale);
					entry->iconOffsetY = (s16)icon4;
					entry->scale = iconScale;
					entry->startTime = r24;
					r24 += var54;
					entry++;
					data->entryId++;
				}
				else {
					glyph = kanjiSearch(0x94B5);
					w = (u16)(s32)(scale * (f32)(kanjiGetWidth((u16)glyph) + 4));
					entry->glyph = glyph;
					entry->x = ex;
					entry->y = (s16)((s32)ey - (s32)(10.f * scale));
					entry->scale = scale;
					entry->arg = flags;
					entry->arg |= 8;
					if (flags & 1) {
						entry->field_0x14 = dyn;
					}
					if (maxScale < scale) {
						maxScale = scale;
					}
					entry->startTime = r24;
					ex = (s16)(ex + w);
					r24 += var54;
					entry++;
					data->entryId++;
				}
			}
			else if (strcmp(tagName, "anim") == 0) {
				sscanf(tagArg, "%s%s%d%d%f%d", buf3, buf2, &animA, &animB, &animF, &animC);
				entry->arg = animPoseEntry(buf3, 2);
				animPoseSetAnim(entry->arg, buf2, 1);
				animPoseSetMaterialFlagOn(entry->arg, 0x1800);
				animPoseSetMaterialFlagOff(entry->arg, 0x40);
				animPoseSetGXFunc(msgw->poseId, msgDispKeyWait_render, 0);
				entry->glyph = 0xFFF4;
				entry->x = (s16)animA;
				entry->y = (s16)((s32)var[2] - animB);
				entry->scale = animF;
				entry->field_0x14 = (animC == 0) ? 0.0f : 180.f;
				r24 += var54;
				entry++;
				data->entryId++;
			}
			else if (strcmp(tagName, "paper") == 0) {
				sscanf(tagArg, "%s%d", buf1, &paperId);
				if (strcmp(buf1, "me") != 0) {
					npc = npcNameToPtr(buf1);
				}
				else {
					npc = NULL;
				}
				entry->arg = (u32)npc;
				entry->glyph = 0xFFF3;
				entry->startTime = paperId;
				entry++;
				data->entryId++;
			}
			else if (strcmp(tagName, "/paper") == 0) {
				sscanf(tagArg, "%s", buf0);
				if (strcmp(buf0, "me") != 0) {
					npc = npcNameToPtr(buf0);
				}
				else {
					npc = NULL;
				}
				entry->arg = (u32)npc;
				entry->glyph = 0xFFF2;
				entry++;
				data->entryId++;
			}
			else if (strcmp(tagName, "r") == 0) {
				if (data->window->type != WINDOW_TYPE_SMALL) {
					r25 = ex;
				}
			}
			else if (*(s8*)tagName == 'R' || (*(s8*)tagName == '/' && *(s8*)tagName == '/r')) {
				skip = 1;
				if (*(s8*)tagName != 'R') {
					skip = 2;
				}
				if (data->window->type == WINDOW_TYPE_SMALL) {
					continue;
				}
				len = strlen(tagName + skip);
				half = len / 2;
				total = 0;
				for (i = 0; (s32)i < half; i++) {
					u16 g = msgGetKanji(tagName, skip + i * 2);
					f32 fw = scale * (f32)(kanjiGetWidth((u16)g) + 4);

					total += (s32)(0.55f * fw);
				}
				base = 4.0f * scale;
				total = (s32)((f32)(u16)total - 0.55f * base);
				r25 = ((f32)(r25 + ex) - base - (f32)(u16)total) / 2.0f;

				for (j = 0; (s32)j < half; j++) {
					u16 g = msgGetKanji(tagName, skip + j * 2);
					f32 fw = scale * (f32)(kanjiGetWidth((u16)g) + 4);
					s32 rinc = (s32)(0.55f * fw);

					entry->glyph = g;
					entry->x = (s16)r25;
					entry->y = (s32)ey;
					entry->scale = 0.55f * scale;
					entry->arg = flags;
					if (flags & 1) {
						entry->field_0x14 = dyn;
					}
					entry->startTime = 0;
					r25 += rinc;
					entry++;
					data->entryId++;
				}
				continue;
			}
			if ((s8)*after == '\n') {
				msg++;
			}
			continue;
		}

		if ((u32)gp->language == 0 && (s8)check >= '0' && (s8)check <= '9') {
			glyph = kanjiSearch((u16)(check + 0x10000 - 0x7DE1));
		}
		else {
			glyph = hankakuSearch(check);
		}
		{
		s32 exinc = (s32)(scale * (f32)(kanjiGetWidth((u16)glyph) + 2));
		entry->glyph = glyph;
		entry->x = ex;
		entry->y = (s16)((s32)ey - (s32)(10.f * scale));
		entry->scale = scale;
		entry->arg = flags;
		if ((s8)*msg != ' ') {
			entry->arg |= 8;
		}
		if (flags & 1) {
			entry->field_0x14 = dyn;
		}
		if (maxScale < scale) {
			maxScale = scale;
		}
		entry->startTime = r24;
		ex = (s16)((s32)ex + exinc);
		}
		r24 += var54;
		msg++;
		entry++;
		data->entryId++;
	}
}
#pragma opt_lifetimes reset

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

s32 msgIconStr2ID(const char* str) {
	u32 i;

	for (i = 0; i < 18; i++) {
		if (!strcmp(msgIcon[i].name, str)) {
			return msgIcon[i].value;
		}
	}
	return -1;
}


// The caller in evt_msg.c sign-extends `b` (`extsh`) before the call; here it goes to windowEntry unchanged.
s32 msgWindow_Entry(const char* msg, s32 a, s32 b) {
	// Declaration order steers MWCC's callee-saved allocation to the original (msg=r28, id=r27,
	// buf=r30); the natural order lands msg in r27/buf in r27.
	char* buf;
	WindowEntry* window;
	MessageData* data;
	smartEntry* smart;
	s32 id;

	id = windowEntry(b);
	window = windowGetPointer(id);
	window->main = msgWindow_Main;
	window->delete = msgWindow_Delete;
	window->state = 5;
	window->type = -1;
	window->flags |= 2;
	buf = __memAlloc(HEAP_DEFAULT, strlen(msg));
	strcpy(buf, msg);
	smart = smartAlloc(0x7A58, 1);
	data = smart->address;
	memset(data, 0, 0x7A58);
	data->window = window;
	data->flags = 3;
	data->callback = (void (*)(s32, smartEntry*))a;
	data->lastFrameTime = gp->renderTime;
	data->effectStart = gp->renderTime;
	data->field_0x7A28 = 0;
	data->sfxVolume = 0x7f;
	data->timeCursor = 0;
	data->penX = 0;
	data->penY = 0;
	data->savedX = 0;
	data->savedY = 0;
	data->charDelay = 0x19;
	data->historyCount = 0;
	msgAnalize(smart, buf);
	if (window->type == WINDOW_TYPE_SMALL) {
		data->flags |= 0x200;
	}
	__memFree(HEAP_DEFAULT, buf);
	window->userdata = smart;
	data = window->userdata->address;
	data->talkAnimTimer = 0;
	data->talkAnimOn = 0;
	switch (window->type) {
	case WINDOW_TYPE_CLEAR:
		window->main = msgWindow_Clear_Main;
		/* fallthrough */
	case WINDOW_TYPE_NORMAL:
	case WINDOW_TYPE_PLATE:
	case WINDOW_TYPE_SYSTEM:
	case WINDOW_TYPE_KANBAN:
	case WINDOW_TYPE_BOSS:
	case WINDOW_TYPE_DIARY:
	case WINDOW_TYPE_TEC:
	case WINDOW_TYPE_MAJO:
	case WINDOW_TYPE_HOUSOU:
		if (!(window->flags & 4)) {
			window->x = -280.0f;
			window->y = 225.0f;
			window->height = 560.0f;
			window->width = 176.0f;
		}
		break;
	case WINDOW_TYPE_SELECT:
		window->main = selectWindow_Main;
		window->x = (s16)((-((MessageData*)window->userdata->address)->selectWidth) / 2);
		window->y = ((MessageData*)window->userdata->address)->selectY;
		window->height = ((MessageData*)window->userdata->address)->selectWidth;
		window->width = (s16)(((MessageData*)window->userdata->address)->selectCount * 0x24 + 0x28);
		((MessageData*)window->userdata->address)->selectCursor = ((MessageData*)window->userdata->address)->selectInit;
		data->selectTopY = window->y - (f32)(data->selectCursor * 0x1F + 0x3A);
		break;
	case WINDOW_TYPE_SMALL:
		window->priority = 10;
		window->height = 0.7f * (f32)GXGetTexObjWidth(getWakuTexObj(3));
		window->width = 0.7f * (f32)GXGetTexObjHeight(getWakuTexObj(3));
		break;
	}
	return id;
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

void msgWindow_Add(const char* msg, s32 id) {
	WindowEntry* window = windowGetPointer(id);
	smartEntry* smart = window->userdata;
	MessageData* data;

	data = smart->address;
	data->lastFrameTime = gp->renderTime;
	msgAnalize(smart, msg);
	window->state = 1;
	window->flags |= 2;
}

void msgWindow_Continue(s32 id) {
	WindowEntry* window = windowGetPointer(id);
	MessageData* data;

	window->state = 1;
	window->flags |= 2;
	data = window->userdata->address;
	data->lastFrameTime = gp->renderTime;
}

void msgWindow_Repeat(s32 id) {
	WindowEntry* window = windowGetPointer(id);
	MessageData* data;

	window->state = 1;
	window->flags |= 2;
	data = window->userdata->address;
	data->lastFrameTime = gp->renderTime;
	data->effectStart = gp->renderTime;
	data->textTime = 0;
	data->scrollTarget = 0;
	data->scrollY = 0;
	data->entryPos = 0;
}

void msgWindow_ForceClose(const char* tag) {
	s32 i = 0;
	WindowEntry* window;
	MessageData* data;

	for (i = 0; i < 7; i++) {
		window = windowGetPointer(i);
		if (window->flags && window->state && window->userdata) {
			data = window->userdata->address;
			if (data->tag && strcmp(data->tag, tag) == 0) {
				window->state = 7;
				data = window->userdata->address;
				if (data->flags & 8) {
					BtlUnit_ChangeStayAnim(data->battleUnitId);
				}
				else {
					npcSetStayPose(data->text);
				}
			}
		}
	}
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
	s32 value;

	if (data->talkAnimTimer != 0) {
		if (data->talkAnimOn == 0) {
			if (data->flags & 8) {
				BtlUnit_ChangeTalkAnim(data->battleUnitId);
			}
			else {
				npcSetTalkPose(data->text);
			}
			data->talkAnimOn = 1;
		}
		data->talkAnimTimer--;
		if (data->talkAnimTimer == 0) {
			if (((MessageData*)smart->address)->flags & 8) {
				BtlUnit_ChangeStayAnim(((MessageData*)smart->address)->battleUnitId);
			}
			else {
				npcSetStayPose(((MessageData*)smart->address)->text);
			}
			data->talkAnimOn = 0;
		}
	}

	switch (window->state) {
	case 5:
		if (!(window->flags & 8)) {
			s32 se = msgWindowOpen_SE_Data[window->type][0];
			if (se >= 0) {
				psndSFXOn((const char*)(se | 0x20000));
			}
		}
		window->stateTime = gp->renderTime;
		window->state = 6;
		/* fallthrough */
	case 6:
		window->flags |= 2;
		window->alpha = (s16)(OSTicksToMilliseconds(gp->renderTime - window->stateTime) * 255 / 200);
		if (window->alpha > 0xff) {
			window->alpha = 0xff;
			window->state = 1;
			data->effectStart = gp->renderTime;
			data->lastFrameTime = gp->renderTime;
		}
		break;
	case 7:
		if (!(window->flags & 8)) {
			s32 se = msgWindowOpen_SE_Data[window->type][1];
			if (se >= 0) {
				psndSFXOn((const char*)(se | 0x20000));
			}
		}
		window->stateTime = gp->renderTime;
		window->state = 8;
		/* fallthrough */
	case 8:
		if (window->alpha == 0) {
			windowDelete(window);
			return;
		}
		window->alpha = 0xff - OSTicksToMilliseconds(gp->renderTime - window->stateTime) * 255 / 200;
		if (window->alpha <= 0) {
			window->alpha = 0;
		}
		break;
	case 1:
		switch (msgMain(window->userdata)) {
		case 0:
			window->state = 7;
			data = window->userdata->address;
			if (data->flags & 8) {
				BtlUnit_ChangeStayAnim(data->battleUnitId);
			}
			else {
				npcSetStayPose(data->text);
			}
			break;
		case 1:
			window->state = 2;
			window->flags &= ~2;
			data = window->userdata->address;
			if (data->flags & 8) {
				BtlUnit_ChangeStayAnim(data->battleUnitId);
			}
			else {
				npcSetStayPose(data->text);
			}
			break;
		case 2:
			window->state = 3;
			break;
		case 3:
			break;
		}
		break;
	case 3: {
		u32 mask;
		u32 button;

		mask = (((MessageData*)window->userdata->address)->flags & 0x100) ? 0x500 : 0x100;
		button = keyGetButtonTrg(0);
		if (button & (mask | 0x200)) {
			window->state = 1;
			data->choiceState = 2;
			if (keyGetButtonTrg(0) & 0x200) {
				if (!(data->flags & 0x80)) {
					data->flags |= 4;
				}
			}
		}
		if (keyGetButtonTrg(0) & 0x10) {
			u32 ok;
			data->historyCursor = data->historyCount - 1;
			{
				MessageData* d = window->userdata->address;
				d->historyCursor--;
				if (d->historyCursor < 0) {
					d->historyCursor = (ok = 0);
				}
				else {
					ok = 1;
				}
			}
			if (ok) {
				window->state = 9;
				window->stateTime = gp->renderTime;
				psndSFXOn((const char*)0x20041);
				data->flags |= 0x40;
			}
		}
		break;
	}
	// The (MessageData*) cast here is a no-op that stops the compiler CSE-ing scrollY across the
	// OSTicksToMilliseconds call; the original reloads it.
	case 9: {
		value = data->history[data->historyCursor];
		if (data->scrollY < value) {
			break;
		}
		data->scrollY = (s32)((f32)((MessageData*)data)->scrollY - 0.6f * (f32)OSTicksToMilliseconds(gp->renderTime - window->stateTime));
		window->stateTime = gp->renderTime;
		if (data->scrollY > value) {
			break;
		}
		data->scrollY = value;
		window->state = 0xb;
		break;
	}
	// Same no-op cast as case 9, on the scrollY update below.
	case 10: {
		value = data->history[data->historyCursor];
		if (data->scrollY > value) {
			break;
		}
		data->scrollY = (s32)((f32)((MessageData*)data)->scrollY + 0.6f * (f32)OSTicksToMilliseconds(gp->renderTime - window->stateTime));
		window->stateTime = gp->renderTime;
		if (data->scrollY < value) {
			break;
		}
		data->scrollY = value;
		if (data->historyCursor == data->historyCount - 1) {
			window->state = 3;
			data->flags &= ~0x40;
		}
		else {
			window->state = 0xb;
		}
		break;
	}
	case 11: {
		u32 mask;
		u32 button;

		mask = (((MessageData*)window->userdata->address)->flags & 0x100) ? 0x500 : 0x100;
		button = keyGetButtonTrg(0);
		if (button & mask) {
			window->state = 0xa;
			window->stateTime = gp->renderTime;
			{
				MessageData* d = window->userdata->address;
				d->historyCursor++;
				if (d->historyCursor > d->historyCount - 1) {
					d->historyCursor = d->historyCount - 1;
				}
			}
			psndSFXOn((const char*)0x20041);
		}
		if (keyGetButtonTrg(0) & 0x200) {
			window->state = 0xa;
			window->stateTime = gp->renderTime;
			data->historyCursor = data->historyCount - 1;
			psndSFXOn((const char*)0x20041);
		}
		if (keyGetButtonTrg(0) & 0x10) {
			u32 ok;
			window->state = 9;
			window->stateTime = gp->renderTime;
			{
				MessageData* d = window->userdata->address;
				d->historyCursor--;
				if (d->historyCursor < 0) {
					d->historyCursor = (ok = 0);
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

	dispEntry(8, 400.f - (f32)window->priority, 0, msgWindow_Disp, window);
}

void msgWindow_Clear_Main(WindowEntry* window) {
	MessageData* data = window->userdata->address;

	switch (window->state) {
	case 5:
		window->state = 1;
		window->stateTime = gp->renderTime;
		data->effectStart = gp->renderTime;
		data->lastFrameTime = gp->renderTime;
		/* fallthrough */
	case 1:
	case 6:
		window->flags |= 2;
		if (window->alpha < 0xff) {
			window->alpha = (s16)(OSTicksToMilliseconds(gp->renderTime - window->stateTime) * 255 / 200);
			if (window->alpha > 0xff) {
				window->alpha = 0xff;
			}
		}
		if (window->state != 2) {
			s32 ret = msgMain(window->userdata);
			switch (ret) {
			case 0:
				window->state = 7;
				window->stateTime = gp->renderTime;
				data = window->userdata->address;
				if (data->flags & 8) {
					BtlUnit_ChangeStayAnim(data->battleUnitId);
				}
				else {
					npcSetStayPose(data->text);
				}
				break;
			case 1:
				window->state = 2;
				window->flags &= ~2;
				data = window->userdata->address;
				if (data->flags & 8) {
					BtlUnit_ChangeStayAnim(data->battleUnitId);
				}
				else {
					npcSetStayPose(data->text);
				}
				break;
			case 2:
				window->state = 3;
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
		window->alpha = 0xff - OSTicksToMilliseconds(gp->renderTime - window->stateTime) * 255 / 200;
		if (window->alpha <= 0) {
			window->alpha = 0;
		}
		break;
	}
	dispEntry(8, 400.f - (f32)window->priority, 0, msgWindow_Disp, window);
}


void msgWindow_Disp(CameraId camId, void* param) {
	WindowEntry* window = param;
	CameraEntry* camera = camGetCurPtr();
	BOOL drawn = FALSE;
	Vec pos;

	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, unk_80422fe0);

	switch (window->type) {
	case WINDOW_TYPE_NORMAL:
	case WINDOW_TYPE_SMALL:
	case WINDOW_TYPE_BOSS:
	case WINDOW_TYPE_TEC:
	case WINDOW_TYPE_MAJO: {
		f32 half = 0.5f;

		if (window->dispFlags != 0) {
			f32 px, py, h, w;

			if (window->dispFlags & 0x10) {
				pos = window->anchor;
			} else {
				getScreenPoint(&window->anchor, &pos);
			}
			switch (window->type) {
			case WINDOW_TYPE_NORMAL:
			case WINDOW_TYPE_BOSS:
			case WINDOW_TYPE_TEC:
			case WINDOW_TYPE_MAJO:
				px = intplGetValue(0, window->alpha, 0xFF, pos.x, window->x + window->height * half);
				py = intplGetValue(0, window->alpha, 0xFF, pos.y, window->y - window->width * half);
				h = intplGetValue(0, window->alpha, 0xFF, 10.f, window->height);
				w = intplGetValue(0, window->alpha, 0xFF, 10.f, window->width);
				windowDispGX_Message(window->type, window->dispFlags, (u8)window->alpha, px - h * half, py + w * half, h, w, pos.x, pos.y);
				if (window->alpha == 0xFF) {
					GXSetScissor(camera->scissor[0], (u32)(240.f - (window->y - 36.0f)), camera->scissor[2], 100);
					drawn = msgDisp(window->userdata, (u8)window->alpha, 62.0f + window->x, window->y - 36.0f);
				}
				break;
			case WINDOW_TYPE_SMALL: {
				s32 top;

				window->x = pos.x - window->height * half;
				if (window->dispFlags & 8) {
					window->y = pos.y - 80.f;
				} else {
					window->y = 60.f + pos.y;
				}
				px = intplGetValue(0, window->alpha, 0xFF, pos.x, window->x + window->height * half);
				py = intplGetValue(0, window->alpha, 0xFF, pos.y, window->y - window->width * half);
				h = intplGetValue(0, window->alpha, 0xFF, 10.f, window->height);
				w = intplGetValue(0, window->alpha, 0xFF, 10.f, window->width);
				windowDispGX_Message(window->type, window->dispFlags, (u8)window->alpha, px - h * half, py + w * half, h, w, pos.x, pos.y);
				if (window->alpha == 0xFF) {
					top = 0xF0 - ((s32)window->y - 10);
					if (top < 0) {
						top = 0;
					}
					if (top > 0x1E0) {
						top = 0x1E0;
					}
					GXSetScissor(camera->scissor[0], top, camera->scissor[2], 0x7E);
					msgDisp(window->userdata, (u8)window->alpha, 20.f + window->x, window->y - 7.0f);
				}
				break;
			}
			}
		} else {
			windowDispGX_Message(window->type, 0, (u8)window->alpha, window->x, window->y, window->height, window->width, 0.0f, 0.0f);
			GXSetScissor(camera->scissor[0], (u32)(240.f - (window->y - 36.0f) - 8.0f), camera->scissor[2], 0x7E);
			drawn = msgDisp(window->userdata, (u8)window->alpha, 62.0f + window->x, window->y - 36.0f);
		}
		break;
	}
	case WINDOW_TYPE_PLATE:
	case WINDOW_TYPE_KANBAN:
	case WINDOW_TYPE_HOUSOU:
		windowDispGX_Kanban(window->type, (u8)window->alpha, window->x, window->y, window->height, window->width);
		GXSetScissor(camera->scissor[0], (u32)(240.f - (window->y - 36.0f) - 2.0f), camera->scissor[2], 0x68);
		drawn = msgDisp(window->userdata, (u8)window->alpha, 62.0f + window->x, window->y - 36.0f);
		break;
	case WINDOW_TYPE_DIARY:
		windowDispGX_Kanban(window->type, (u8)window->alpha, window->x, window->y, window->height, window->width);
		GXSetScissor(camera->scissor[0], (u32)(240.f - (window->y - 36.0f) - 8.0f), camera->scissor[2], 0x7E);
		drawn = msgDisp(window->userdata, (u8)window->alpha, 62.0f + window->x, window->y - 36.0f);
		break;
	case WINDOW_TYPE_SYSTEM:
		windowDispGX_System(window->type, (u8)window->alpha, window->x, window->y, window->height, window->width);
		drawn = (u32)(window->width - 50.f);
		GXSetScissor(camera->scissor[0], (u32)(240.f - (window->y - 36.0f) - 8.0f), camera->scissor[2], drawn);
		drawn = msgDisp(window->userdata, (u8)window->alpha, 62.0f + window->x, window->y - 36.0f);
		break;
	case WINDOW_TYPE_CLEAR:
		drawn = (u32)(window->width - 50.f);
		GXSetScissor(camera->scissor[0], (u32)(240.f - (window->y - 36.0f) - 8.0f), camera->scissor[2], drawn);
		drawn = msgDisp(window->userdata, (u8)window->alpha, 62.0f + window->x, window->y - 36.0f);
		break;
	}

	GXSetScissor(camera->scissor[0], camera->scissor[1], camera->scissor[2], camera->scissor[3]);
	if (drawn) {
		if (active == 0) {
			active = 1;
		}
		animPoseMain(msgw->poseId);
		animPoseDraw(msgw->poseId, window->x + window->height - 52.0f, 31.0f + (window->y - window->width), 0.0f, 180.f, 5.0f, 2);
	}
}


void selectWindow_Main(WindowEntry* window) {
	MessageData* data = window->userdata->address;
	s8 old;

	msgMain(window->userdata);
	switch (window->state) {
	case 5:
		window->alpha += 0x19;
		if (window->alpha > 0xff) {
			window->alpha = 0xff;
			window->state = 1;
		}
		break;
	case 1:
		old = data->selectCursor;
		if ((keyGetButtonTrg(0) & 8) || (keyGetDirTrg(0) & 0x1000)) {
			data->selectCursor--;
		}
		else if ((keyGetButtonRep(0) & 8) || (keyGetDirRep(0) & 0x1000)) {
			if (data->selectCursor != 0) {
				data->selectCursor--;
			}
		}
		if ((keyGetButtonTrg(0) & 4) || (keyGetDirTrg(0) & 0x2000)) {
			data->selectCursor++;
		}
		else if ((keyGetButtonRep(0) & 4) || (keyGetDirRep(0) & 0x2000)) {
			if ((s8)data->selectCursor < (s8)data->selectCount - 1) {
				data->selectCursor++;
			}
		}
		if (data->selectCursor < 0) {
			data->selectCursor = data->selectCount - 1;
		}
		if (data->selectCursor >= data->selectCount) {
			data->selectCursor = 0;
		}
		if (old != data->selectCursor) {
			psndSFXOn((const char*)0x20005);
		}
		if (keyGetButtonTrg(0) & 0x200) {
			switch (data->field_0x7A42) {
			case -1:
				break;
			case -2:
				data->selectCursor = -1;
				window->state = 7;
				psndSFXOn((const char*)0x20013);
				break;
			default:
				data->selectCursor = data->field_0x7A42;
				window->state = 7;
				psndSFXOn((const char*)0x20013);
				break;
			}
		}
		else if (keyGetButtonTrg(0) & 0x100) {
			window->state = 7;
			psndSFXOn((const char*)0x20012);
		}
		break;
	case 7:
		if (window->alpha == 0) {
			window->state = 4;
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
	dispEntry(8, 400.f - (f32)window->priority, 0, selectWindow_Disp, window);
}

// Builds the waku colour in one expression so MWCC lands it in the same stack slot as the
// original (a plain local followed by `color.a = ...` puts it one slot higher).
static inline GXColor selectWindowColor(u8 a) {
	GXColor c = dat_8041a510;
	c.a = a;
	return c;
}

void selectWindow_Disp(CameraId camId, void* param) {
	WindowEntry* window = param; //cast to correct type
	MessageData* data = window->userdata->address;
	GXColor color;
	Vec pos;

	GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, unk_80422fe4);

	color = selectWindowColor(window->alpha);
	windowDispGX_Waku_col(window->x, window->y, window->height, window->width, 30.f, 0, color);

	msgDisp(window->userdata, (u8)window->alpha, 40.f + window->x, window->y - 25.0f);

	pos.x = 2.0f + window->x;
	data->selectTopY += 0.3f * (window->y - (f32)((s8)data->selectCursor * 0x1F + 0x3A) - data->selectTopY);
	pos.y = data->selectTopY;
	pos.z = 0.0f;

	iconDispGxAlpha(&pos, 0x14, 0x1F0, (u8)window->alpha, 1.0f);
}

// .sdata2 colour words read by name in the message renderer.
__declspec(section ".sdata2") GXColor dat_8041a4f0 = { 0x50, 0x50, 0x50, 0xFF };
__declspec(section ".sdata2") GXColor dat_8041a4f4 = { 0x6E, 0x6E, 0x6E, 0xFF };
__declspec(section ".sdata2") GXColor dat_8041a4f8 = { 0x00, 0x00, 0x00, 0x50 };
__declspec(section ".sdata2") GXColor dat_8041a4fc = { 0xFF, 0xFF, 0xFF, 0xFF };
__declspec(section ".sdata2") u32 dat_8041a500 = 0xDCC8DCFF;
__declspec(section ".sdata2") u32 dat_8041a504 = 0x00D914FF;
__declspec(section ".sdata2") u32 dat_8041a508 = 0xDCC8DCFF;
__declspec(section ".sdata2") u32 dat_8041a50c = 0x00D914FF;
__declspec(section ".sdata2") GXColor dat_8041a510 = { 0xFF, 0xFF, 0xFF, 0x00 };

// Named .sdata2 constants kept by tools/pool_plan.py, defined after the code in the original's order.
#pragma explicit_zero_data on
#pragma explicit_zero_data reset

