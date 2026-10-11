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

#ifdef MUST_MATCH
// .sbss2: the default .sbss2 is only used for compiler-generated pool templates.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80422fe0;
__declspec(section ".sdata") GXColor unk_80422fe4;
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
		data->field_0x28 += 0.36f * (f32)OSTicksToMilliseconds(gp->renderTime - data->field_0x30);
		if (!(data->field_0x4 & 0x80)) {
			i = (((MessageData*)smart->address)->field_0x4 & 0x100) ? 0x500 : 0x100;
			mask = i | 0x200;
			button = keyGetButtonTrg(0);
			if (button & mask) {
				data->field_0x4 |= 4;
			}
		}
		if (!(data->field_0x4 & 0x10) && (data->field_0x4 & 0x4)) {
			data->field_0x28 += 0.36f * (f32)OSTicksToMilliseconds(gp->renderTime - data->field_0x30);
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










// math_ext.h declares sin/cos as out-of-line MSL entry points. Do NOT include the SDK <math.h>
// here: its `extern inline` sqrtf/sqrt emit four 0.5/3.0 localstatics into .rodata (0x20 bytes)
// that the original unit does not have, shifting every later string (and msgInit's offsets) by
// 0x20. It also has to come after msgInit, as it did before.
#include "math_ext.h"
extern const f32 float_0_8041a514;
extern const f32 float_30_8041a518;
extern const f32 float_40_8041a51c;
extern const f32 float_25_8041a520;
extern const f32 float_2_8041a524;
extern const f32 float_0p3_8041a528;
#pragma pool_data on // the original addresses its .rodata through one base register
#ifdef MUST_MATCH
// MUST_MATCH: retail passes these colours by value (MWCC copies the GXColor to a stack temp and passes
// its address to FontDrawColor). The header declares a pointer, so the natural call is FontDrawColor(&c)
// and it does not copy.
static inline void msgDrawColor(GXColor color) {
	FontDrawColor(&color);
}
#endif
// Matching. The `idx`/`base` arithmetic is written inline at each array access (a `base` pointer local let the
// allocator coalesce it the wrong way), and the `ca`/`sa` of the two cos/sin branches are one shared pair
// hoisted to the default case so the allocator keeps them in the retail registers; `angle` stays local to
// each branch. The by-value font colour comes from the msgDrawColor inline above.
BOOL msgDisp(smartEntry* smart, u8 alpha, f32 x, f32 y) {
	MessageData* data = smart->address;
	MsgDataEntry* entry = data->entries;
	s32 i;
	BOOL result = FALSE;

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
			GXColor color = *(GXColor*)&entry->field_0x0;
#ifdef MUST_MATCH
			msgDrawColor(color);
#else
			FontDrawColor(&color);
#endif
			break;
		}
		case 0xFFF6:
			if (entry->field_0x0) {
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
				y + (f32)data->field_0x28 + (f32)*(s16*)((u8*)entry + 0x8) - float_40_8041a51c,
				float_0_8041a514);
			animPoseMain(entry->field_0x0);
			animPoseDrawMtx(entry->field_0x0, mtx, 2,
				*(f32*)((u8*)entry + 0x14), *(f32*)((u8*)entry + 0x10));
			FontDrawStart_alpha(alpha);
#ifdef MUST_MATCH
			msgDrawColor(color);
#else
			FontDrawColor(&color);
#endif
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
			// The two rotating-code branches below share this cos/sin pair; one shared variable pair is
			// what puts them in the retail registers (each branch keeps its own `angle`).
			f32 ca;
			f32 sa;

			if (data->field_0x4 & 0x40) {
				s32 cur = data->field_0x28;
				s32 idx = *(s32*)((u8*)data + 0x783C);
				s32 e8 = *(s16*)((u8*)entry + 0x8);
				// Window queue: cursor at data+0x783C, count at data+0x7840, three parallel arrays at
				// data+0x7844/0x7848/0x784C, all indexed by the cursor.
				GXColor color;

				if (cur == ((s32*)((u8*)data + idx * 4))[0x7844 / 4]) {
					if (e8 + ((s32*)((u8*)data + idx * 4))[0x7848 / 4] <= 0) {
						break;
					}
					tev = TRUE;
				}
				else if (cur < ((s32*)((u8*)data + idx * 4))[0x7844 / 4]) {
					if (e8 + ((s32*)((u8*)data + idx * 4))[0x7844 / 4] > 0) {
						tev = TRUE;
					}
					else {
						s32 t = ((s32*)((u8*)data + idx * 4))[0x7844 / 4] - cur;
						if (t > 100) {
							t = 100;
						}
						fade = (u32)((100 - t) * 255 / 100);
						if (idx != data->field_0x7840 - 1) {
							if (e8 + ((s32*)((u8*)data + idx * 4))[0x7848 / 4] > 0) {
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
					if (e8 + ((s32*)((u8*)data + idx * 4))[0x7848 / 4] > 0) {
						tev = TRUE;
					}
					else {
						s32 t = cur - ((s32*)((u8*)data + idx * 4))[0x7844 / 4];
						if (t > 100) {
							t = 100;
						}
						fade = (u32)(t * 255 / 100);
						if (idx != data->field_0x7840 - 2) {
							if (e8 + ((s32*)((u8*)data + idx * 4))[0x784C / 4] > 0) {
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
				pos.x = 20.f + (x + (f32)*(s16*)((u8*)entry + 0x6));
				pos.y = (f32)*(s16*)((u8*)entry + 0xA) + (((f32)*(s16*)((u8*)entry + 0x8) + (y + (f32)data->field_0x28)) - float_40_8041a51c);
				pos.z = float_0_8041a514;
				// MUST_MATCH: retail converts iconId as u16 (clrlwi, no extsh); cast the callee at the call site.
#ifdef MUST_MATCH
				((void (*)(Vec*, s16, u16, u8, f32))iconDispGxAlpha)(&pos, 0x10, (u16)(entry->field_0x0),
					(u8)((fade * alpha) / 255), *(f32*)entry->field_0x10);
#else
				iconDispGxAlpha(&pos, 0x10, entry->field_0x0 & 0xFFFF,
					(u8)((fade * alpha) / 255), *(f32*)entry->field_0x10);
#endif
				FontDrawStart_alpha(alpha);
#ifdef MUST_MATCH
				msgDrawColor(color);
#else
				FontDrawColor(&color);
#endif
				break;
			}

			if ((f32)(*(s16*)((u8*)entry + 0x8) + data->field_0x28) >= 32.0f * *(f32*)((u8*)entry + 0x10) - 10.f) {
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

			if (entry->field_0x0 & 1) {
				u16 half = kanjiGetWidth(entry->glyph) >> 1;
				f32 fx = px;
				f32 fy = py;
				Mtx mtx;

				if (entry->field_0x0 & 2) {
					fx = px + (f32)(irand(10000) % 3);
					fy = py + (f32)(irand(10000) % 3);
				}
				PSMTXTrans(mtx, (f32)(-half), 12.0f, float_0_8041a514);
				PSMTXScaleApply(mtx, mtx, *(f32*)((u8*)entry + 0x14), *(f32*)((u8*)entry + 0x14), *(f32*)((u8*)entry + 0x14));
				PSMTXTransApply(mtx, mtx, (f32)half, -12.0f, float_0_8041a514);
				PSMTXScaleApply(mtx, mtx, *(f32*)((u8*)entry + 0x10), *(f32*)((u8*)entry + 0x10), *(f32*)((u8*)entry + 0x10));
				PSMTXTransApply(mtx, mtx, fx, fy, float_0_8041a514);
				FontDrawCodeMtx(mtx, entry->glyph);
				*(f32*)((u8*)entry + 0x14) = 0.5f * ((*(f32*)((u8*)entry + 0x14)) - 1.0f) + 1.0f;
			}
			else if (entry->field_0x0 & 2) {
				f32 fx = px + (f32)(irand(10000) % 3);
				f32 fy = py + (f32)(irand(10000) % 3);
				FontDrawScale(*(f32*)((u8*)entry + 0x10));
				FontDrawCode(entry->glyph, fx, fy);
			}
			else if (entry->field_0x0 & 4) {
				f32 t = (f32)OSTicksToMilliseconds(gp->renderTime - data->field_0x18);
				f32 angle = 0.01f * t - 0.02f * px;
				ca = (f32)cos(angle);
				sa = (f32)sin(angle);
				FontDrawScale(*(f32*)((u8*)entry + 0x10));
				FontDrawCode(entry->glyph, 3.2f * ca + px, py - 3.2f * sa);
			}
			else if (entry->field_0x0 & (0x100 << data->field_0x7A43)) {
				f32 t = (f32)OSTicksToMilliseconds(gp->renderTime - data->field_0x18);
				f32 angle = 0.01f * t - 0.02f * px;
				f32 fx;
				f32 fy;
				ca = (f32)cos(angle);
				fx = 3.2f * ca + px;
				sa = (f32)sin(angle);
				fy = py - 3.2f * sa;

				FontDrawScale(*(f32*)((u8*)entry + 0x10));
				{
					GXColor color = *(GXColor*)&dat_8041a4f8;
#ifdef MUST_MATCH
					msgDrawColor(color);
#else
					FontDrawColor(&color);
#endif
				}
				FontDrawCode(entry->glyph, fx, fy);
				FontDrawEdge();
				FontDrawRainbowColor();
				{
					GXColor color2 = *(GXColor*)&dat_8041a4fc;
#ifdef MUST_MATCH
					msgDrawColor(color2);
#else
					FontDrawColor(&color2);
#endif
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

// opt_dead_assignments off: keeps the original's dead cursor stores in place, which pins MWCC's
// register allocation for the spilled s16 cursor group and the R-ruby float temps to match the
// shipped binary (this is how MWCC 2.6 behaves under the original's higher register pressure).
#pragma opt_dead_assignments off
// The R-ruby loops index through the `rp` pointer (not the tagName array), so MWCC's frontend leaves the
// offset counters alone and the backend's strength reduction keeps them next to the pointer it derives,
// as in the target; the icon name goes through msgIconStr2ID (auto-inlined).
// What differs: the second colouring pass. The target keys the loop-2 counter and an extra R-ruby temp so the
// tagName base lands in r0 in both preheaders and the long-lived GPRs (data, window, entry, .rodata base) sit
// higher; ours colours them differently, and loop 2's preheader schedules `add ptr, tagName, skip` after the
// ey/flags hoists instead of before them.
// NONMATCHING (98.16%): GPR colouring of the second allocation pass; see above.
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
	MessageData* data;
	WindowEntry* window;
	MsgDataEntry* entry;
	s16 ex;
	s16 ey;
	s16 r25;
	s32 r24;
	u32 flags;
	f32 maxScale;
	char* pa;
	char* pn;
	char* q;
	const char* after;
	s32 mode;
	u32 n;
	u32 check;
	s8 tch;
	char tchc;
	BOOL checkkanji;
	u16 glyph, width;
	NpcEntry* npc;
	s32 off;
	s32 i;
	u32 skip;
	s32 len;
	s32 half;
	s32 total;
	const char* rp;
	f32 base;
	s32 mid;
	s32 j;
	s16 var[3];

	data = smart->address;
	window = data->window;
	flags = 0;
	entry = &data->entries[data->entryId];
	maxScale = float_0_8041a514;
	scale = 1.0f;
	if (window->type < 0) {
		window->type = 0;
	}
	var[2] = 0;
	r24 = data->field_0x7A30;
	var[1] = data->field_0x7A38;
	ex = data->field_0x7A34;
	var[0] = data->field_0x7A3A;
	ey = data->field_0x7A36;
	var54 = data->field_0x7A3C;

	switch (window->type) {
	case 3:
	case 8:
		switch (window->type) {
		case 3:
			entry->field_0x0 = dat_8041a500;
			break;
		case 8:
			entry->field_0x0 = dat_8041a504;
			break;
		}
		entry->glyph = 0xFFF7;
		entry->field_0xC = r24;
		entry++;
		data->entryId++;
		break;

	default:
		break;
	}

loop:
	check = (u8)*msg;
	{
		u32 c = (u8)check;
		if ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC)) {
			checkkanji = 1;
		}
		else {
			checkkanji = 0;
		}
	}
	if (checkkanji) {
		glyph = kanjiSearch(*(const u16*)msg);
		width = kanjiGetWidth((u16)glyph);
		{
		s32 exinc = (s32)(scale * (f32)(width + 4));
		entry->glyph = glyph;
		*(s16*)(entry->field_0x6) = ex;
		*(s16*)(entry->field_0x6 + 2) = (s16)((s32)ey - (s32)(10.f * scale));
		*(f32*)(entry->field_0x10) = scale;
		entry->field_0x0 = flags;
		if (*(const u16*)msg == 0x8145) {
			entry->field_0x0 |= 0x10;
		}
		else if (*(const u16*)msg != 0x8140 && *(const u16*)msg != 0x8151) {
			entry->field_0x0 |= 8;
		}
		if (flags & 1) {
			*(f32*)(entry->field_0x10 + 4) = dyn;
		}
		if (maxScale < scale) {
			maxScale = scale;
		}
		entry->field_0xC = r24;
		ex = (s16)((s32)ex + exinc);
		}
		r24 += var54;
		msg += 2;
		entry++;
		data->entryId++;
		goto loop;
	}

	switch ((s8)check) {
	case 0:
		entry->glyph = 0xFFFF;
		entry->field_0xC = r24;
		data->field_0x7A30 = r24;
		data->field_0x7A34 = ex;
		data->field_0x7A36 = ey;
		data->field_0x7A38 = var[1];
		data->field_0x7A3A = var[0];
		goto done;

	case '\n':
		entry->glyph = 0xFFFB;
		ex = 0;
		entry->field_0xC = r24;
		if (float_0_8041a514 == maxScale) {
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
		maxScale = float_0_8041a514;
		if (data->field_0x4 & 0x20) {
			flags = (flags & ~0xFF00) | (0x100 << (s8)data->field_0x7A40);
			data->field_0x7A40++;
		}
		goto loop;

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
				goto parsed;
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
			*(s16*)(entry->field_0x6) = var[1];
			entry->field_0xC = r24;
			r24 += 100;
			entry++;
			data->entryId++;
			var[1]++;
		}
		else if (strcmp(tagName, "keyxon") == 0) {
			data->field_0x4 |= 0x100;
		}
		else if (strcmp(tagName, "p") == 0 || strcmp(tagName, "scrl_auto") == 0) {
			if (var[0] != 0) {
				entry->glyph = 0xFFFD;
				*(s16*)(entry->field_0x6 + 2) = (s16)(var[0] - 3);
				ey -= 3;
				entry->field_0xC = r24;
				r24 += 50;
				entry++;
				data->entryId++;
				var[2] = (s16)(ey - 3);
				var[0] = 0;
			}
		}
		else if (strcmp(tagName, "o") == 0 || strcmp(tagName, "once_stop") == 0) {
			entry->glyph = 0xFFFC;
			entry->field_0xC = r24;
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
			var54 = data->field_0x7A3C;
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
			sscanf(tagArg, "%x", &entry->field_0x0);
			entry->glyph = 0xFFF7;
			entry->field_0xC = r24;
			entry++;
			data->entryId++;
		}
		else if (strcmp(tagName, "/col") == 0) {
			entry->field_0x0 = 0xFF;
			entry->glyph = 0xFFF7;
			entry->field_0xC = r24;
			entry++;
			data->entryId++;
		}
		else if (strcmp(tagName, "noise") == 0) {
			entry->field_0x0 = 1;
			entry->glyph = 0xFFF6;
			entry->field_0xC = r24;
			entry++;
			data->entryId++;
		}
		else if (strcmp(tagName, "/noise") == 0) {
			entry->field_0x0 = 0;
			entry->glyph = 0xFFF6;
			entry++;
			data->entryId++;
		}
		else if (strcmp(tagName, "dkey") == 0) {
			entry->field_0x0 = 1;
			entry->glyph = 0xFFF5;
			entry->field_0xC = r24;
			entry++;
			data->entryId++;
		}
		else if (strcmp(tagName, "/dkey") == 0) {
			entry->field_0x0 = 0;
			entry->glyph = 0xFFF5;
			entry->field_0xC = r24;
			entry++;
			data->entryId++;
		}
		else if (strcmp(tagName, "plate") == 0) {
			window->type = 2;
			var54 = 0;
			data->field_0x7A3C = 0;
		}
		else if (strcmp(tagName, "system") == 0) {
			window->type = 3;
			entry->field_0x0 = dat_8041a508;
			entry->glyph = 0xFFF7;
			entry->field_0xC = r24;
			entry++;
			data->entryId++;
		}
		else if (strcmp(tagName, "tec") == 0) {
			window->type = 8;
			entry->field_0x0 = dat_8041a50c;
			entry->glyph = 0xFFF7;
			entry->field_0xC = r24;
			entry++;
			data->entryId++;
		}
		else if (strcmp(tagName, "majo") == 0) {
			window->type = 9;
		}
		else if (strcmp(tagName, "diary") == 0) {
			window->type = 7;
		}
		else if (strcmp(tagName, "housou") == 0) {
			window->type = 10;
		}
		else if (strcmp(tagName, "boss") == 0) {
			window->type = 6;
		}
		else if (strcmp(tagName, "kanban") == 0) {
			window->type = 4;
			var54 = 0;
			data->field_0x7A3C = 0;
		}
		else if (strcmp(tagName, "small") == 0) {
			window->type = 1;
			data->field_0x4 |= 0x10;
		}
		else if (strcmp(tagName, "clear") == 0) {
			window->type = 11;
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
			window->type = 5;
			data->field_0x4 |= 0x20;
			sscanf(tagArg, "%hd%hd%hd%hd%hd", &sel2, &sel1, &data->field_0x7A44, &data->field_0x7A46);
			data->field_0x7A41 = (s8)sel2;
			data->field_0x7A42 = (s8)sel1;
			data->field_0x7A40 = 1;
			flags |= 0x100;
			var54 = 0;
		}
		else if (strcmp(tagName, "se") == 0) {
			sscanf(tagArg, "%d", &entry->field_0x0);
			entry->glyph = 0xFFFA;
			entry++;
			data->entryId++;
		}
		else if (strcmp(tagName, "vol") == 0) {
			sscanf(tagArg, "%d", &entry->field_0x0);
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

				entry->field_0x0 = (u32)iconId;
				iconGetWidthHight(&w, &h, (u16)iconId);
				entry->glyph = 0xFFF8;
				ex += 40;
				*(s16*)(entry->field_0x6) = (s16)((f32)icon3 + (0.5f * (f32)w * iconScale + (f32)oldEx));
				*(s16*)(entry->field_0x6 + 2) = (s16)((f32)ey - (f32)h * iconScale);
				*(s16*)(entry->field_0x6 + 4) = (s16)icon4;
				*(f32*)(entry->field_0x10) = iconScale;
				entry->field_0xC = r24;
				r24 += var54;
				entry++;
				data->entryId++;
			}
			else {
				glyph = kanjiSearch(0x94B5);
				width = kanjiGetWidth((u16)glyph);
				w = (u16)(s32)((f32)(width + 4) * scale);
				entry->glyph = glyph;
				*(s16*)(entry->field_0x6) = ex;
				*(s16*)(entry->field_0x6 + 2) = (s16)((s32)ey - (s32)(10.f * scale));
				*(f32*)(entry->field_0x10) = scale;
				entry->field_0x0 = flags;
				entry->field_0x0 |= 8;
				if (flags & 1) {
					*(f32*)(entry->field_0x10 + 4) = dyn;
				}
				if (maxScale < scale) {
					maxScale = scale;
				}
				entry->field_0xC = r24;
				ex = (s16)(ex + w);
				r24 += var54;
				entry++;
				data->entryId++;
			}
		}
		else if (strcmp(tagName, "anim") == 0) {
			sscanf(tagArg, "%s%s%d%d%f%d", buf3, buf2, &animA, &animB, &animF, &animC);
			entry->field_0x0 = animPoseEntry(buf3, 2);
			animPoseSetAnim(entry->field_0x0, buf2, 1);
			animPoseSetMaterialFlagOn(entry->field_0x0, 0x1800);
			animPoseSetMaterialFlagOff(entry->field_0x0, 0x40);
			animPoseSetGXFunc(msgw->poseId, msgDispKeyWait_render, 0);
			entry->glyph = 0xFFF4;
			*(s16*)(entry->field_0x6) = (s16)animA;
			*(s16*)(entry->field_0x6 + 2) = (s16)((s32)var[2] - animB);
			*(f32*)(entry->field_0x10) = animF;
			*(f32*)(entry->field_0x10 + 4) = (animC == 0) ? float_0_8041a514 : 180.f;
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
			entry->field_0x0 = (u32)npc;
			entry->glyph = 0xFFF3;
			entry->field_0xC = paperId;
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
			entry->field_0x0 = (u32)npc;
			entry->glyph = 0xFFF2;
			entry++;
			data->entryId++;
		}
		else if (strcmp(tagName, "r") == 0) {
			if (data->window->type != 1) {
				r25 = ex;
			}
		}
		else if (*(s8*)tagName == 'R' || (*(s8*)tagName == '/' && *(s8*)tagName == '/r')) {
			skip = 1;
			if (*(s8*)tagName != 'R') {
				skip = 2;
			}
			if (data->window->type == 1) {
				goto loop;
			}
			len = strlen(tagName + skip);
			half = len / 2;
			total = 0;
			rp = tagName;
			off = skip;
			for (i = 0; i < half; i++) {
				u16 g = kanjiSearch(*(const u16*)(rp + off));
				u16 kwidth = kanjiGetWidth((u16)g);

				f32 fw = (f32)(kwidth + 4) * scale;

				total += (s32)(0.55f * fw);
				off += 2;
			}
			base = 4.0f * scale;
			mid = (s32)((f32)(u16)total - 0.55f * base);
			{
				f32 t = (f32)((s16)r25 + (s32)ex) - base;
				r25 = (t - (f32)(u16)mid) / 2.0f;
			}

			rp = tagName;
			for (j = 0; j < half; j++) {
				u16 g = kanjiSearch(*(const u16*)(rp + skip));
				u16 kwidth = kanjiGetWidth((u16)g);
				f32 fw = (f32)(kwidth + 4) * scale;
				s32 rinc = (s32)(0.55f * fw);

				entry->glyph = g;
				*(s16*)(entry->field_0x6) = (s16)r25;
				*(s16*)(entry->field_0x6 + 2) = (s32)ey;
				*(f32*)(entry->field_0x10) = 0.55f * scale;
				entry->field_0x0 = flags;
				if (flags & 1) {
					*(f32*)(entry->field_0x10 + 4) = dyn;
				}
				entry->field_0xC = 0;
				r25 += rinc;
				skip += 2;
				entry++;
				data->entryId++;
			}
			goto loop;
		}
		goto aftertag;

	default:
		goto defaultchar;
	}

aftertag:
	if ((s8)*after == '\n') {
		msg++;
	}
	goto loop;

defaultchar:
	if ((u32)gp->language == 0 && (s8)check >= '0' && (s8)check <= '9') {
		glyph = kanjiSearch((u16)(check + 0x10000 - 0x7DE1));
	}
	else {
		glyph = hankakuSearch(check);
	}
	width = kanjiGetWidth((u16)glyph);
	{
	s32 exinc = (s32)(scale * (f32)(width + 2));
	entry->glyph = glyph;
	*(s16*)(entry->field_0x6) = ex;
	*(s16*)(entry->field_0x6 + 2) = (s16)((s32)ey - (s32)(10.f * scale));
	*(f32*)(entry->field_0x10) = scale;
	entry->field_0x0 = flags;
	if ((s8)*msg != ' ') {
		entry->field_0x0 |= 8;
	}
	if (flags & 1) {
		*(f32*)(entry->field_0x10 + 4) = dyn;
	}
	if (maxScale < scale) {
		maxScale = scale;
	}
	entry->field_0xC = r24;
	ex = (s16)((s32)ex + exinc);
	}
	r24 += var54;
	msg++;
	entry++;
	data->entryId++;
	goto loop;

done:
	return;
}
#pragma opt_dead_assignments reset

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


// The third parameter is really s16 (the caller in evt_msg.c sign-extends it with `extsh`), but
// windowEntry is declared u16 in windowdrv.h and defined that way in windowdrv.c, so the plain
// call zero-extends b. Calling through the callee's real width reproduces the original's `mr r3, r5`
// without touching the shared header/definition; the natural call is kept under #else.
s32 msgWindow_Entry(const char* msg, s32 a, s32 b) {
	// Declaration order steers MWCC's callee-saved allocation to the original (msg=r28, id=r27,
	// buf=r30); the natural order lands msg in r27/buf in r27.
	char* buf;
	WindowEntry* window;
	MessageData* data;
	smartEntry* smart;
	s32 id;

#ifdef MUST_MATCH
	id = ((s32 (*)(s32))windowEntry)(b);
#else
	id = windowEntry(b);
#endif
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
	data = window->userdata->address;
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
		window->x = (s16)((-((MessageData*)window->userdata->address)->field_0x7A44) / 2);
		window->y = ((MessageData*)window->userdata->address)->field_0x7A46;
		window->height = ((MessageData*)window->userdata->address)->field_0x7A44;
		window->width = (s16)(((MessageData*)window->userdata->address)->field_0x7A40 * 0x24 + 0x28);
		((MessageData*)window->userdata->address)->field_0x7A43 = ((MessageData*)window->userdata->address)->field_0x7A41;
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
	data->field_0x30 = gp->renderTime;
	msgAnalize(smart, msg);
	*(u16*)window->field_0x0 = 1;
	window->flags |= 2;
}

void msgWindow_Continue(s32 id) {
	WindowEntry* window = windowGetPointer(id);
	MessageData* data;

	*(u16*)window->field_0x0 = 1;
	window->flags |= 2;
	data = window->userdata->address;
	data->field_0x30 = gp->renderTime;
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
		data->field_0x28 = (s32)((f32)((MessageData*)data)->field_0x28 - 0.6f * (f32)OSTicksToMilliseconds(gp->renderTime - *(OSTime*)(window->field_0x1C + 4)));
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
		data->field_0x28 = (s32)((f32)((MessageData*)data)->field_0x28 + 0.6f * (f32)OSTicksToMilliseconds(gp->renderTime - *(OSTime*)(window->field_0x1C + 4)));
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

	dispEntry(8, 400.f - (f32)*(s16*)&window->field_0x4, 0, msgWindow_Disp, window);
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
	dispEntry(8, 400.f - (f32)*(s16*)&window->field_0x4, 0, msgWindow_Disp, window);
}


void msgWindow_Disp(CameraId camId, void* param) {
	WindowEntry* window = param;
	CameraEntry* camera = camGetCurPtr();
	BOOL drawn = FALSE;
	Vec pos;

	GXSetFog(GX_FOG_NONE, float_0_8041a514, float_0_8041a514, float_0_8041a514, float_0_8041a514, unk_80422fe0);

	switch (window->type) {
	case 0:
	case 1:
	case 6:
	case 8:
	case 9: {
		f32 half = 0.5f;

		if (window->field_0x2C != 0) {
			f32 px, py, h, w;

			if (window->field_0x2C & 0x10) {
				pos = window->anchor;
			} else {
				getScreenPoint(&window->anchor, &pos);
			}
			switch (window->type) {
			case 0:
			case 6:
			case 8:
			case 9:
				px = intplGetValue(0, window->alpha, 0xFF, pos.x, window->x + window->height * half);
				py = intplGetValue(0, window->alpha, 0xFF, pos.y, window->y - window->width * half);
				h = intplGetValue(0, window->alpha, 0xFF, 10.f, window->height);
				w = intplGetValue(0, window->alpha, 0xFF, 10.f, window->width);
				windowDispGX_Message(window->type, window->field_0x2C, (u8)window->alpha, px - h * half, py + w * half, h, w, pos.x, pos.y);
				if (window->alpha == 0xFF) {
					GXSetScissor(camera->scissor[0], (u32)(240.f - (window->y - 36.0f)), camera->scissor[2], 100);
					drawn = msgDisp(window->userdata, (u8)window->alpha, 62.0f + window->x, window->y - 36.0f);
				}
				break;
			case 1: {
				s32 top;

				window->x = pos.x - window->height * half;
				if (window->field_0x2C & 8) {
					window->y = pos.y - 80.f;
				} else {
					window->y = 60.f + pos.y;
				}
				px = intplGetValue(0, window->alpha, 0xFF, pos.x, window->x + window->height * half);
				py = intplGetValue(0, window->alpha, 0xFF, pos.y, window->y - window->width * half);
				h = intplGetValue(0, window->alpha, 0xFF, 10.f, window->height);
				w = intplGetValue(0, window->alpha, 0xFF, 10.f, window->width);
				windowDispGX_Message(window->type, window->field_0x2C, (u8)window->alpha, px - h * half, py + w * half, h, w, pos.x, pos.y);
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
			windowDispGX_Message(window->type, 0, (u8)window->alpha, window->x, window->y, window->height, window->width, float_0_8041a514, float_0_8041a514);
			GXSetScissor(camera->scissor[0], (u32)(240.f - (window->y - 36.0f) - 8.0f), camera->scissor[2], 0x7E);
			drawn = msgDisp(window->userdata, (u8)window->alpha, 62.0f + window->x, window->y - 36.0f);
		}
		break;
	}
	case 2:
	case 4:
	case 10:
		windowDispGX_Kanban(window->type, (u8)window->alpha, window->x, window->y, window->height, window->width);
		GXSetScissor(camera->scissor[0], (u32)(240.f - (window->y - 36.0f) - float_2_8041a524), camera->scissor[2], 0x68);
		drawn = msgDisp(window->userdata, (u8)window->alpha, 62.0f + window->x, window->y - 36.0f);
		break;
	case 7:
		windowDispGX_Kanban(window->type, (u8)window->alpha, window->x, window->y, window->height, window->width);
		GXSetScissor(camera->scissor[0], (u32)(240.f - (window->y - 36.0f) - 8.0f), camera->scissor[2], 0x7E);
		drawn = msgDisp(window->userdata, (u8)window->alpha, 62.0f + window->x, window->y - 36.0f);
		break;
	case 3:
		windowDispGX_System(window->type, (u8)window->alpha, window->x, window->y, window->height, window->width);
		drawn = (u32)(window->width - 50.f);
		GXSetScissor(camera->scissor[0], (u32)(240.f - (window->y - 36.0f) - 8.0f), camera->scissor[2], drawn);
		drawn = msgDisp(window->userdata, (u8)window->alpha, 62.0f + window->x, window->y - 36.0f);
		break;
	case 11:
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
		animPoseDraw(msgw->poseId, window->x + window->height - 52.0f, 31.0f + (window->y - window->width), float_0_8041a514, 180.f, 5.0f, 2);
	}
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
	dispEntry(8, 400.f - (f32)*(s16*)&window->field_0x4, 0, selectWindow_Disp, window);
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
#ifdef MUST_MATCH
	GXColor color;
#else
	GXColor color = dat_8041a510;
#endif
	Vec pos;

	GXSetFog(0, float_0_8041a514, float_0_8041a514, float_0_8041a514, float_0_8041a514, unk_80422fe4);

#ifdef MUST_MATCH
	color = selectWindowColor(window->alpha);
#else
	color.a = window->alpha;
#endif
	windowDispGX_Waku_col(0, color, window->x, window->y, window->height, window->width, float_30_8041a518);

	msgDisp(window->userdata, (u8)window->alpha, float_40_8041a51c + window->x, window->y - float_25_8041a520);

	pos.x = float_2_8041a524 + window->x;
	data->field_0x7A48 += float_0p3_8041a528 * (window->y - (f32)((s8)data->field_0x7A43 * 0x1F + 0x3A) - data->field_0x7A48);
	pos.y = data->field_0x7A48;
	pos.z = float_0_8041a514;

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
const f32 float_0_8041a514 = 0.0f;
#pragma explicit_zero_data reset
const f32 float_30_8041a518 = 30.f;
const f32 float_40_8041a51c = 40.f;
const f32 float_25_8041a520 = 25.0f;
const f32 float_2_8041a524 = 2.0f;
const f32 float_0p3_8041a528 = 0.3f;

