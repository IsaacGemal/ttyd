#include "nameent.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "driver/dispdrv.h"
#include "driver/fadedrv.h"
#include "driver/windowdrv.h"
#include "driver/msgdrv.h"
#include "win/win_main.h"
#include "manager/fontmgr.h"
void FontDrawColor(GXColor* color);
#include "system.h"
#include <string.h>
#include "math_ext.h"

// The original .rodata pads str_s_w_jp_name_tpl ("%s/w/jp/name.tpl") to 20 bytes with
// three trailing NULs, so the fixed string is written with them and pools to the
// same length.

//.data
const char* hiragana_tbl[] = {
	"\x82\xA0", "\x82\xA2", "\x82\xA4", "\x82\xA6", "\x82\xA8", "\x82\xED",
	"\x81\x40", "\x82\xF0", "\x81\x40", "\x82\xF1", "\x82\x4F", "\x82\x50",
	"\x82\x51", "\x82\x52", "\x82\xA9", "\x82\xAB", "\x82\xAD", "\x82\xAF",
	"\x82\xB1", "\x82\xAA", "\x82\xAC", "\x82\xAE", "\x82\xB0", "\x82\xB2",
	"\x82\x53", "\x82\x54", "\x82\x55", "\x82\x56", "\x82\xB3", "\x82\xB5",
	"\x82\xB7", "\x82\xB9", "\x82\xBB", "\x82\xB4", "\x82\xB6", "\x82\xB8",
	"\x82\xBA", "\x82\xBC", "\x82\x57", "\x82\x58", "\x81\x49", "\x81\x48",
	"\x82\xBD", "\x82\xBF", "\x82\xC2", "\x82\xC4", "\x82\xC6", "\x82\xBE",
	"\x82\xC0", "\x82\xC3", "\x82\xC5", "\x82\xC7", "\x81\x40", "\x94\xB5",
	"\x81\x99", "\x81\xF4", "\x82\xC8", "\x82\xC9", "\x82\xCA", "\x82\xCB",
	"\x82\xCC", "\x82\xCE", "\x82\xD1", "\x82\xD4", "\x82\xD7", "\x82\xDA",
	NULL,       NULL,       NULL,       NULL,       "\x82\xCD", "\x82\xD0",
	"\x82\xD3", "\x82\xD6", "\x82\xD9", "\x82\xCF", "\x82\xD2", "\x82\xD5",
	"\x82\xD8", "\x82\xDB", NULL,       NULL,       NULL,       NULL,
	"\x82\xDC", "\x82\xDD", "\x82\xDE", "\x82\xDF", "\x82\xE0", "\x82\x9F",
	"\x82\xA1", "\x82\xA3", "\x82\xA5", "\x82\xA7", NULL,       NULL,
	NULL,       NULL,       "\x82\xE2", "\x81\x40", "\x82\xE4", "\x81\x40",
	"\x82\xE6", "\x82\xE1", "\x82\xE3", "\x82\xE5", "\x82\xC1", "\x89\x4C",
	NULL,       NULL,       NULL,       NULL,       "\x82\xE7", "\x82\xE8",
	"\x82\xE9", "\x82\xEA", "\x82\xEB", "\x81\x7C", "\x81\x60", "\x81\x45",
	"\x81\x44", "\x81\x40", NULL,       NULL,       NULL,       NULL
};

const char* katakana_tbl[] = {
	"\x83\x41", "\x83\x43", "\x83\x45", "\x83\x47", "\x83\x49", "\x83\x8F",
	"\x81\x40", "\x83\x92", "\x81\x40", "\x83\x93", "\x82\x4F", "\x82\x50",
	"\x82\x51", "\x82\x52", "\x83\x4A", "\x83\x4C", "\x83\x4E", "\x83\x50",
	"\x83\x52", "\x83\x4B", "\x83\x4D", "\x83\x4F", "\x83\x51", "\x83\x53",
	"\x82\x53", "\x82\x54", "\x82\x55", "\x82\x56", "\x83\x54", "\x83\x56",
	"\x83\x58", "\x83\x5A", "\x83\x5C", "\x83\x55", "\x83\x57", "\x83\x59",
	"\x83\x5B", "\x83\x5D", "\x82\x57", "\x82\x58", "\x81\x49", "\x81\x48",
	"\x83\x5E", "\x83\x60", "\x83\x63", "\x83\x65", "\x83\x67", "\x83\x5F",
	"\x83\x61", "\x83\x64", "\x83\x66", "\x83\x68", "\x81\x40", "\x94\xB5",
	"\x81\x99", "\x81\xF4", "\x83\x69", "\x83\x6A", "\x83\x6B", "\x83\x6C",
	"\x83\x6D", "\x83\x6F", "\x83\x72", "\x83\x75", "\x83\x78", "\x83\x7B",
	NULL,       NULL,       NULL,       NULL,       "\x83\x6E", "\x83\x71",
	"\x83\x74", "\x83\x77", "\x83\x7A", "\x83\x70", "\x83\x73", "\x83\x76",
	"\x83\x79", "\x83\x7C", NULL,       NULL,       NULL,       NULL,
	"\x83\x7D", "\x83\x7E", "\x83\x80", "\x83\x81", "\x83\x82", "\x83\x40",
	"\x83\x42", "\x83\x44", "\x83\x46", "\x83\x48", NULL,       NULL,
	NULL,       NULL,       "\x83\x84", "\x81\x40", "\x83\x86", "\x81\x40",
	"\x83\x88", "\x83\x83", "\x83\x85", "\x83\x87", "\x83\x62", "\x83\x94",
	NULL,       NULL,       NULL,       NULL,       "\x83\x89", "\x83\x8A",
	"\x83\x8B", "\x83\x8C", "\x83\x8D", "\x81\x7C", "\x81\x60", "\x81\x45",
	"\x81\x44", "\x81\x40", NULL,       NULL,       NULL,       NULL
};

//.bss
static NameEntryWork work;

//.sdata
static NameEntryWork* wp = &work;
static const char* msg_tbl[] = {
	"msg_nameent_1",
	"msg_nameent_2",
	"msg_nameent_3",
	"msg_nameent_3",
	"msg_nameent_4"
};

// The white GXColor the target keeps in .sdata2 and loads once via the sda21 base.
const GXColor nameent_white = { 0xFF, 0xFF, 0xFF, 0xFF };

void nameEntInit(void) {
	memset(wp, 0, sizeof(NameEntryWork));
}

void nameEntReInit(void) {
	wp->phase = 3;
}

BOOL nameEntPrepare(void) {
	return fileAsyncf(FILE_TYPE_TPL, NULL, "%s/w/jp/name.tpl\0\0\0", getMarioStDvdRoot()) != 0;
}

// The integer expression keeps the original's operand order; the form
// `x*24 + x/5*30 - 190` reassociates and emits a different register/add order. The original selects the
// type-2 table entry with a two-case switch, which is why it branches `beq case2; b default`.
void nameEntOn(s32 type) {
	s32 i;

	wp->flags = 0;
	wp->type = type;
	wp->phase = 1;
	wp->step = 0;
	wp->charTable = hiragana_tbl;
	wp->nameLen = 0;
	wp->cursorCol = 0;
	wp->cursorRow = 0;

	wp->x1 = wp->x2 = (f32)(wp->cursorCol * 24 - 190 + wp->cursorCol / 5 * 30);
	wp->y1 = wp->y2 = (f32)(112 - wp->cursorRow * 28);
	wp->charCursorX = wp->charCursorTargetX = -106.0f;
	wp->charCursorY = wp->charCursorTargetY = 135.0f;

	// The original .sdata2 holds the empty string in a 4-byte slot (named float_0);
	// three explicit NULs make the pooled literal four bytes like the original.
	strcpy(wp->name, "\0\0\0");
	for (i = 0; i < 8; i++) {
		strcat(wp->name, "\x81\x40");
	}
	wp->maskAlpha = 0.0f;

	switch (type) {
	case 2:
		hiragana_tbl[9] = "\x81\x40";
		katakana_tbl[9] = "\x81\x40";
		break;
	default:
		hiragana_tbl[9] = "\x82\xF1";
		katakana_tbl[9] = "\x83\x93";
		break;
	}
}

void nameEntOff(void) {
	wp->phase = 3;
}

BOOL nameEntWait(void) {
	wp->flags |= 1;
	return wp->step != 0x65;
}

BOOL nameEntIsCancel(void) {
	return (wp->flags >> 1) & 1;
}

char* nameEntGetName(void) {
	s32 off;
	s32 i;

	if (wp->step != 0x65) {
		return NULL;
	}
	for (i = 7, off = 0xe; i >= 1; i--, off -= 2) {
		if (strncmp(&wp->name[off], "\x81\x40", 2) != 0) {
			break;
		}
		wp->name[off] = 0;
		wp->name[off + 1] = 0;
	}
	return wp->name;
}

void nameEntMain(void) {
	BOOL loaded;

	switch (wp->phase) {
	case 0:
		break;
	case 1:
		if (fileAsyncf(FILE_TYPE_TPL, NULL, "%s/w/jp/name.tpl\0\0\0", getMarioStDvdRoot()) != 0U) {
			wp->tpl = fileAllocf(FILE_TYPE_TPL, "%s/w/jp/name.tpl\0\0\0", getMarioStDvdRoot());
			loaded = TRUE;
		} else {
			loaded = FALSE;
		}
		if (loaded) {
			wp->phase++;
		}
	case 2:
		nameMain();
		break;
	case 3:
		if (wp->tpl != NULL) {
			fileFree(wp->tpl);
		}
		wp->tpl = NULL;
		wp->phase = 0;
		break;
	}
	if (wp->phase > 1) {
		dispEntry(4, 0.0f, 7, nameEntDisp, NULL);
	}
}

// Two source shapes are load-bearing: the four `(target-cur)/4.0f` blends must be a division, not `*0.25f`
// (both fold to the same `float_0p25` load, but the division makes MWCC emit the target's `fmadds fD, diff, k,
// cur`; the multiply commutes to `fD, k, diff, cur`), and the `idx = idx - 1` clamp must be an explicit
// subtraction, not `idx--` (the decrement form pushes the tail's i2f conversions into the wrong stack slots).
s32 nameMain(void) {
	f32 dist;
	s32 count;
	s32 ofs;
	s32 half;

	dist = distABf(wp->x2, wp->y2, wp->x1, wp->y1);

	switch (wp->step) {
	case 0:
		if (wp->flags & 1) {
			if (gp->unk8 == 0) {
				if (keyGetButtonTrg(0) & 0x100) {
					s32 f10 = wp->cursorCol;

					if (f10 >= 10 && wp->cursorRow >= 4) {
						switch (wp->cursorRow) {
						case 4:
							wp->charTable = hiragana_tbl;
							psndSFXOn((const char*)0x2000C);
							break;
						case 5:
							wp->charTable = katakana_tbl;
							psndSFXOn((const char*)0x2000C);
							break;
						case 6:
							if (wp->nameLen > 0) {
								wp->nameLen--;
								psndSFXOn((const char*)0x2000B);
							}
							break;
						case 7:
							half = strlen(wp->name) >> 1;
							for (count = 0; count < half; count++) {
								if (strncmp(&wp->name[count * 2], "\x81\x40", 2) != 0) {
									break;
								}
							}
							if (count < half || wp->type != 0) {
								wp->dialogCursor = 1;
								wp->step = 100;
								psndSFXOn((const char*)0x20012);
								if (wp->type == 1 && count >= half) {
									strcpy(wp->name, msgSearch("name_party3"));
									wp->nameLen = strlen(wp->name) >> 1;
								}
							} else {
								psndSFXOn((const char*)0x20014);
							}
							break;
						}
					} else {
						if (wp->nameLen < 8) {
							((u16*)wp->name)[wp->nameLen] =
							    *(u16*)wp->charTable[wp->cursorRow * 14 + wp->cursorCol];
							wp->nameLen++;
							if (wp->nameLen >= 8) {
								wp->nameLen = 7;
								wp->cursorCol = 0xA;
								wp->cursorRow = 7;
							}
							psndSFXOn((const char*)0xA);
						} else if (wp->nameLen == 8) {
							wp->nameLen = 7;
							((u16*)wp->name)[wp->nameLen] =
							    *(u16*)wp->charTable[wp->cursorRow * 14 + wp->cursorCol];
							wp->cursorCol = 0xA;
							wp->cursorRow = 7;
							psndSFXOn((const char*)0xA);
						}
					}
				} else if (keyGetButtonTrg(0) & 0x1000) {
					s32 count2;
					s32 half2 = strlen(wp->name) >> 1;
					s32 ofs2;

					// target increments the byte offset before the counter; a plain
					// `count2++, ofs2 += 2` emits them the other way around.
					for (count2 = 0, ofs2 = 0; count2 < half2; ofs2 += 2, count2++) {
						if (strncmp(&wp->name[ofs2], "\x81\x40", 2) != 0) {
							break;
						}
					}
					if (count2 < half2 || wp->type != 0) {
						wp->dialogCursor = 1;
						wp->step = 100;
						psndSFXOn((const char*)0x12);
						if (wp->type == 1 && count2 >= half2) {
							strcpy(wp->name, msgSearch("name_party3"));
							wp->nameLen = strlen(wp->name) >> 1;
						}
					} else {
						psndSFXOn((const char*)0x20014);
					}
				} else if (keyGetButtonTrg(0) & 0x200) {
					if (wp->nameLen == 0 && wp->type == 0 &&
					    strncmp(wp->name, "\x81\x40", 2) == 0) {
						psndSFXOn((const char*)0x13);
						wp->step = 0x65;
						wp->flags |= 2;
					} else {
						((u16*)wp->name)[wp->nameLen] = *(u16*)"\x81\x40";
						wp->nameLen--;
						if (wp->nameLen < 0) {
							wp->nameLen = 0;
						}
						psndSFXOn((const char*)0xB);
					}
				} else if (keyGetDirRep(0) & 0x4000) {
					if (dist < 10.0f) {
						wp->cursorCol--;
						if (wp->cursorRow > 3) {
							if (wp->cursorCol < 0) {
								wp->cursorCol = 10;
								if (wp->cursorRow > 7) {
									wp->cursorRow = 7;
								}
							}
						} else if (wp->cursorCol < 0) {
							wp->cursorCol = 13;
						}
						psndSFXOn((const char*)5);
					}
				} else if (keyGetDirRep(0) & 0x8000) {
					if (dist < 10.0f) {
						wp->cursorCol++;
						if (wp->cursorRow > 3) {
							if (wp->cursorCol >= 10 && wp->cursorRow > 7) {
								wp->cursorRow = 7;
							}
							if (wp->cursorCol >= 11) {
								wp->cursorCol = 0;
							}
						} else if (wp->cursorCol >= 14) {
							wp->cursorCol = 0;
						}
						psndSFXOn((const char*)5);
					}
				} else if (keyGetDirRep(0) & 0x1000) {
					if (dist < 10.0f) {
						wp->cursorRow--;
						if (wp->cursorRow < 0) {
							if (wp->cursorCol > 9) {
								wp->cursorRow = 7;
								wp->cursorCol = 10;
							} else {
								wp->cursorRow = 8;
							}
						}
						psndSFXOn((const char*)5);
					}
				} else if (keyGetDirRep(0) & 0x2000) {
					if (dist < 10.0f) {
						wp->cursorRow++;
						if (wp->cursorCol > 9) {
							if (wp->cursorRow >= 4) {
								wp->cursorCol = 10;
							}
							if (wp->cursorRow >= 8) {
								wp->cursorRow = 0;
							}
						} else if (wp->cursorRow >= 9) {
							wp->cursorRow = 0;
						}
						psndSFXOn((const char*)5);
					}
				}
			}
		}
		{
			s32 f10 = wp->cursorCol;

			if (f10 > 9 && wp->cursorRow > 3) {
				wp->x2 = 100.0f;
				wp->y2 = (f32)(-((wp->cursorRow - 4) * 0x22) - 0x14);
				wp->x2 -= 6.0f;
				wp->y2 += 6.0f;
			} else {
				wp->x2 = (f32)(f10 * 24 - 190 + (f10 / 5) * 30);
				wp->y2 = (f32)(112 - wp->cursorRow * 28);
				wp->x2 -= 16.0f;
				wp->y2 -= 12.0f;
			}
		}
		wp->maskAlpha -= 0.05f;
		if (wp->maskAlpha <= 0.0f) {
			wp->maskAlpha = 0.0f;
		}
		break;
	case 100:
		if (keyGetButtonTrg(0) & 0x100) {
			if (wp->dialogCursor == 0) {
				wp->step++;
				psndSFXOn((const char*)0x12);
			} else {
				wp->step = 0;
				psndSFXOn((const char*)0x13);
			}
		} else if (keyGetButtonTrg(0) & 0x200) {
			wp->step = 0;
			psndSFXOn((const char*)0x13);
		} else if (keyGetDirRep(0) & 0x3000) {
			wp->dialogCursor = 1 - wp->dialogCursor;
			psndSFXOn((const char*)5);
		}
		wp->x2 = -60.0f;
		wp->y2 = -30.0f - 40.0f * (f32)wp->dialogCursor;
		wp->maskAlpha += 0.05f;
		if (wp->maskAlpha >= 1.0f) {
			wp->maskAlpha = 1.0f;
		}
		break;
	}

	wp->x1 += (wp->x2 - wp->x1) / 4.0f;
	wp->y1 += (wp->y2 - wp->y1) / 4.0f;
	{
		f32 k125 = 0.125f;

		wp->x1 += __fabs(2.0f * (f32)sin((f32)gp->retraceCount * k125));
	}

	{
		s32 idx = wp->nameLen;

		if (idx >= 8) {
			idx = idx - 1;
		}
		wp->charCursorTargetX = (f32)(idx * 32 - 106);
		wp->charCursorTargetY = 135.0f;
		wp->charCursorX += (wp->charCursorTargetX - wp->charCursorX) / 4.0f;
		wp->charCursorY += (wp->charCursorTargetY - wp->charCursorY) / 4.0f;
		{
			f32 k125 = 0.125f;

			wp->charCursorY += __fabs(2.0f * (f32)sin((f32)gp->retraceCount * k125));
		}
	}

	return 0;
}

// The translate/scale Vecs are written as compound literals, not named objects: MWCC then reloads them
// from the .rodata base (r31+0x58/...) inside each loop exactly as retail does, whereas a named object is
// hoisted into a callee-saved register. The anonymous Vecs pool at the target's .rodata offsets, so nameBG
// keeps its doubles in place. The 0x29 loop's varying-x translate is the exception: it goes through
// nameEntMkVec (same shape as winBadgeMkVec), which is what keeps that loop's copy in the target's slot.
static inline Vec nameEntMkVec(Vec base, f32 x) {
	base.x = x;
	return base;
}

// nameWinGX is declared (void* tex, s32 arg1, f32 x, f32 y, f32 z, f32 w) in include/nameent.h, so MWCC
// evaluates the palette pointer first and schedules its final data[0] load one slot before the last lfs.
// Retail evaluates the four floats first (its declaration had the pointer last). The MUST_MATCH cast
// below calls through that order; the natural pointer-first call is under #else.
void nameEntDisp(CameraId cameraId, void* param) {
	CameraEntry saved;
	f32 projv[7];
	f32 viewport[6];
	char buf[16];
	s32 i;
	s32 j;
	s32 row;
	const char* msg;
	const char* okMsg;
	const char* yesMsg;
	const char* noMsg;
	const char* name;
	f32 width;
	GXColor white;
	GXColor white2;
	GXColor white3;

	white3 = nameent_white;
	white2 = nameent_white;
	white = nameent_white;

	saved = *camGetCurPtr();
	GXGetProjectionv(projv);
	GXGetViewportv(viewport);
	*camGetCurPtr() = *camGetPtr(CAMERA_2D);
	GXSetProjection(camGetPtr(CAMERA_2D)->projection, camGetPtr(CAMERA_2D)->projType);
	nameBG();
	winTexInit((TEXPalette*)wp->tpl->data[0]);
	winTexSet(0x2A, (Vec){ -130.0f, 160.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white2);
	for (i = 0; i < 8; i++) {
		winTexSet(0x29, nameEntMkVec((Vec){ 0.0f, 160.0f, 0.0f }, (f32)(-106 + i * 32)), (Vec){ 1.0f, 1.0f, 1.0f }, white);
	}
	winTexSet(0x2B, (Vec){ 142.0f, 160.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white2);
	nameKirinukiGX(-240.0f, 120.0f, 480.0f, 270.0f);
	FontDrawStart();
	for (row = 0; row < 9; row++) {
		for (j = 0; j < 14; j++) {
			if (wp->charTable[row * 14 + j] != NULL) {
				const char* s = wp->charTable[row * 14 + j];
				f32 x = (f32)(-190 + j * 24 + j / 5 * 30 + (24 - (s32)FontGetMessageWidth(s)) / 2);
				f32 y = (f32)(112 - row * 28);

				FontDrawString(s, x, y);
			}
		}
	}
	for (i = 0; i < 4; i++) {
		if (wp->cursorCol >= 10 && wp->cursorRow - 4 == i) {
			winTexInit((TEXPalette*)wp->tpl->data[0]);
			winTexSet(0x33, (Vec){ 160.0f, (f32)(-20 - i * 34), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0, 0, 0, 0x40 });
			winTexInit_x2((TEXPalette*)wp->tpl->data[0]);
			winTexSet_x2(i + 0x25, 0x33, (Vec){ 154.0f, (f32)(-14 - i * 34), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white);
		} else {
			winTexInit_x2((TEXPalette*)wp->tpl->data[0]);
			winTexSet_x2(i + 0x25, 0x33, (Vec){ 160.0f, (f32)(-20 - i * 34), 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white);
		}
	}
	winTexInit((TEXPalette*)wp->tpl->data[0]);
	winTexSet(0x24, (Vec){ wp->charCursorX + 2.0f, wp->charCursorY - 2.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0, 0, 0, 0x80 });
	winTexSet(0x24, (Vec){ wp->charCursorX, wp->charCursorY, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white2);
	FontDrawStart();
	for (i = 0; i < 8; i++) {
		memset(buf, 0, 16);
		strncpy(buf, &wp->name[i * 2], 2);
		if (strlen(buf) != 0) {
			{ f32 t = 28.0f - (f32)FontGetMessageWidth(buf); f32 h = 0.5f; FontDrawString(buf, t * h + (f32)(-120 + i * 32), 171.0f); }
		}
	}
	msg = msg_tbl[wp->type];
	width = (f32)FontGetMessageWidth(msgSearch(msg));
	windowDispGX_Waku_col(0, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF }, -250.0f, -160.0f, 500.0f, 40.0f, 20.0f);
	FontDrawStart();
	{ f32 h = 0.5f; FontDrawString(msgSearch(msg), (240.0f - width) * h + -120.0f, -168.0f); }
	nameMaskGX(wp->maskAlpha);
	if (wp->step == 0x64) {
		okMsg = msgSearch("msg_nameent_ok");
		yesMsg = msgSearch("msg_nameent_yes");
		noMsg = msgSearch("msg_nameent_no");
		if (wp->type == 4) {
			okMsg = msgSearch("msg_nameent_ok_01");
		}
#ifdef MUST_MATCH
		// MUST_MATCH: floats before pointer, so the data[0] load sinks past the last lfs (see note above).
		((void (*)(f32, f32, f32, f32, void*, s32))nameWinGX)(-180.0f, 130.0f, 360.0f, 100.0f, (TEXPalette*)wp->tpl->data[0], 1);
		((void (*)(f32, f32, f32, f32, void*, s32))nameWinGX)(-80.0f, 10.0f, 160.0f, 120.0f, (TEXPalette*)wp->tpl->data[0], 1);
#else
		nameWinGX((TEXPalette*)wp->tpl->data[0], 1, -180.0f, 130.0f, 360.0f, 100.0f);
		nameWinGX((TEXPalette*)wp->tpl->data[0], 1, -80.0f, 10.0f, 160.0f, 120.0f);
#endif
		FontDrawStart();
		FontDrawColor(&(GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
		FontDrawEdge();
		{ f32 wf = -(f32)FontGetMessageWidth(okMsg); f32 h = 0.5f; FontDrawString(okMsg, wf * h, 110.0f); }
		name = wp->name;
		{ f32 wf = -(f32)FontGetMessageWidth(name); f32 h = 0.5f; FontDrawString(name, wf * h, 70.0f); }
		{ f32 wf = -(f32)FontGetMessageWidth(noMsg); f32 h = 0.5f; FontDrawString(yesMsg, wf * h, -20.0f); }
		{ f32 wf = -(f32)FontGetMessageWidth(noMsg); f32 h = 0.5f; FontDrawString(noMsg, wf * h, -60.0f); }
		FontDrawEdgeOff();
	}
	winTexInit((TEXPalette*)wp->tpl->data[0]);
	winTexSet(0, (Vec){ wp->x1 + 2.0f, wp->y1 - 2.0f, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, (GXColor){ 0, 0, 0, 0x80 });
	winTexSet(0, (Vec){ wp->x1, wp->y1, 0.0f }, (Vec){ 1.0f, 1.0f, 1.0f }, white2);
	*camGetCurPtr() = saved;
	GXSetProjectionv(projv);
	GXSetViewport(viewport[0], viewport[1], viewport[2], viewport[3], viewport[4], viewport[5]);
}

// The scale is the original's (f32)__fabs(<double literal>) / GXGetTexObj*; the separate f64
// temporaries per quad keep the pool in instruction order. Depends on the file using math_ext.h
// instead of <math.h>: math.h's extern inline sqrtf/sqrt put _half/_three at the start of
// .rodata and stop the compiler anchoring the pool to str_msg_nameent_1.
// .rodata pool note: the double_480/32/544 displacements are set by the unit's .text/.rodata order,
// not by this function. The 20 file-scope vec3_* objects above must stay declared between msg_tbl and
// the functions (they occupy .rodata 0x40..0x12F), and the functions must stay in reverse .s order
// (nameEntMain after nameBG), or the literals here move and this function drops below 100%.
void nameBG(void) {
	GXTexObj texObj1;
	GXTexObj texObj2;
	GXTexObj texObj3;
	Mtx mtx1;
	Mtx mtx2;
	Mtx mtx3;
	f32 xscale;
	f32 yscale;
	f64 yt;
	f64 xt;
	f64 yt2;
	f64 xt2;
	f64 yt3;
	f64 xt3;

	GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
	GXSetZCompLoc(GX_FALSE);
	GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
	GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
	GXSetNumChans(0);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
	GXSetCurrentMtx(0);

	TEXGetGXTexObjFromPalette((TEXPalette*)wp->tpl->data[0], &texObj1, 0x22);
	GXInitTexObjLOD(&texObj1, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
	GXLoadTexObj(&texObj1, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	yt = __fabs(480.0);
	xt = __fabs(32.0);
	yscale = (f32)yt / GXGetTexObjHeight(&texObj1);
	xscale = (f32)xt / GXGetTexObjWidth(&texObj1);
	PSMTXScale(mtx1, xscale, yscale, 1.0f);
	GXLoadTexMtxImm(mtx1, GX_TEXMTX0, GX_MTX2x4);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(-304.0f, 240.0f, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(-304.0f, -240.0f, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
	GXPosition3f32(-272.0f, -240.0f, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(-272.0f, 240.0f, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);

	TEXGetGXTexObjFromPalette((TEXPalette*)wp->tpl->data[0], &texObj2, 0x21);
	GXInitTexObjLOD(&texObj2, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
	GXLoadTexObj(&texObj2, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	yt2 = __fabs(480.0);
	xt2 = __fabs(544.0);
	yscale = (f32)yt2 / GXGetTexObjHeight(&texObj2);
	xscale = (f32)xt2 / GXGetTexObjWidth(&texObj2);
	PSMTXScale(mtx2, xscale, yscale, 1.0f);
	GXLoadTexMtxImm(mtx2, GX_TEXMTX0, GX_MTX2x4);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(-272.0f, 240.0f, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(-272.0f, -240.0f, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
	GXPosition3f32(272.0f, -240.0f, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(272.0f, 240.0f, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);

	TEXGetGXTexObjFromPalette((TEXPalette*)wp->tpl->data[0], &texObj3, 0x23);
	GXInitTexObjLOD(&texObj3, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
	GXLoadTexObj(&texObj3, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	yt3 = __fabs(480.0);
	xt3 = __fabs(32.0);
	yscale = (f32)yt3 / GXGetTexObjHeight(&texObj3);
	xscale = (f32)xt3 / GXGetTexObjWidth(&texObj3);
	PSMTXScale(mtx3, xscale, yscale, 1.0f);
	GXLoadTexMtxImm(mtx3, GX_TEXMTX0, GX_MTX2x4);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(272.0f, 240.0f, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(272.0f, -240.0f, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
	GXPosition3f32(304.0f, -240.0f, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(304.0f, 240.0f, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
}

#define NAME_KIRINUKI_SCALE(id_, W, H) \
	{ \
		GXTexObj texObj; \
		Mtx mtx; \
		f32 xs; \
		f64 hh; \
		f64 ww; \
		TEXGetGXTexObjFromPalette((TEXPalette*)wp->tpl->data[0], &texObj, (id_)); \
		GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1); \
		GXLoadTexObj(&texObj, GX_TEXMAP0); \
		GXSetNumTexGens(1); \
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY); \
		hh = __fabs(H); \
		ww = __fabs(W); \
		yscale = (f32)hh / (f32)GXGetTexObjHeight(&texObj); \
		xs = (f32)ww / (f32)GXGetTexObjWidth(&texObj); \
		PSMTXScale(mtx, xs, yscale, 1.0f); \
		GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4); \
	}

#define NAME_KIRINUKI_VTX(px, py, s, t) \
	GXPosition3f32((px), (py), 0.0f); \
	GXTexCoord2f32((s), (t))

// Eight quads: the 8x8 corners and the stretched edges and middle. Each edge coordinate is computed right after
// the GXBegin of the first quad that uses it and reused by the later quads.
void nameKirinukiGX(f32 x, f32 y, f32 z, f32 w) {
	f32 size = 8.0f;
	f32 yscale;
	f32 midX;
	f32 midW;
	f32 midR;
	f32 rightX;
	f32 rightR;
	f32 midH;
	f32 botY;
	f32 botB;

	GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
	GXSetZCompLoc(GX_FALSE);
	GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
	GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
	GXSetNumChans(0);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
	GXSetCurrentMtx(0);

	NAME_KIRINUKI_SCALE(0x2f, 8.0f, 8.0f);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	NAME_KIRINUKI_VTX(x, y, 0.0f, 0.0f);
	NAME_KIRINUKI_VTX(x, y - size, 0.0f, 1.0f);
	NAME_KIRINUKI_VTX(x + size, y - size, 1.0f, 1.0f);
	NAME_KIRINUKI_VTX(x + size, y, 1.0f, 0.0f);

	NAME_KIRINUKI_SCALE(0x2d, midW = z - size - size, 8.0f);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	midX = size + x;
	midR = midX + midW;
	NAME_KIRINUKI_VTX(midX, y, 0.0f, 0.0f);
	NAME_KIRINUKI_VTX(midX, y - size, 0.0f, 1.0f);
	NAME_KIRINUKI_VTX(midR, y - size, 1.0f, 1.0f);
	NAME_KIRINUKI_VTX(midR, y, 1.0f, 0.0f);

	NAME_KIRINUKI_SCALE(0x31, 8.0f, 8.0f);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	rightX = x + z - size;
	rightR = rightX + size;
	NAME_KIRINUKI_VTX(rightX, y, 0.0f, 0.0f);
	NAME_KIRINUKI_VTX(rightX, y - size, 0.0f, 1.0f);
	NAME_KIRINUKI_VTX(rightR, y - size, 1.0f, 1.0f);
	NAME_KIRINUKI_VTX(rightR, y, 1.0f, 0.0f);

	NAME_KIRINUKI_SCALE(0x2e, 8.0f, midH = w - size - size);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	NAME_KIRINUKI_VTX(x, y - size, 0.0f, 0.0f);
	NAME_KIRINUKI_VTX(x, y - size - midH, 0.0f, 1.0f);
	NAME_KIRINUKI_VTX(x + size, y - size - midH, 1.0f, 1.0f);
	NAME_KIRINUKI_VTX(x + size, y - size, 1.0f, 0.0f);

	NAME_KIRINUKI_SCALE(0x2c, z - size, midH);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	NAME_KIRINUKI_VTX(midX, y - size, 0.0f, 0.0f);
	NAME_KIRINUKI_VTX(midX, y - size - midH, 0.0f, 1.0f);
	NAME_KIRINUKI_VTX(midX + (z - size), y - size - midH, 1.0f, 1.0f);
	NAME_KIRINUKI_VTX(midX + (z - size), y - size, 1.0f, 0.0f);

	NAME_KIRINUKI_SCALE(0x30, 8.0f, 8.0f);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	botY = y - (w - size);
	botB = botY - size;
	NAME_KIRINUKI_VTX(x, botY, 0.0f, 0.0f);
	NAME_KIRINUKI_VTX(x, botB, 0.0f, 1.0f);
	NAME_KIRINUKI_VTX(x + size, botB, 1.0f, 1.0f);
	NAME_KIRINUKI_VTX(x + size, botY, 1.0f, 0.0f);

	NAME_KIRINUKI_SCALE(0x2c, midW, 8.0f);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	NAME_KIRINUKI_VTX(midX, botY, 0.0f, 0.0f);
	NAME_KIRINUKI_VTX(midX, botB, 0.0f, 1.0f);
	NAME_KIRINUKI_VTX(midR, botB, 1.0f, 1.0f);
	NAME_KIRINUKI_VTX(midR, botY, 1.0f, 0.0f);

	NAME_KIRINUKI_SCALE(0x32, 8.0f, 8.0f);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	NAME_KIRINUKI_VTX(rightX, botY, 0.0f, 0.0f);
	NAME_KIRINUKI_VTX(rightX, botB, 0.0f, 1.0f);
	NAME_KIRINUKI_VTX(rightR, botB, 1.0f, 1.0f);
	NAME_KIRINUKI_VTX(rightR, botY, 1.0f, 0.0f);
}

void nameMaskGX(f32 alpha) {
	GXColor color;
	GXTexObj texObj;
	Mtx mtxMain;
	Mtx mtxScale;
	Mtx mtxNeg;
	f32 scale;

	scale = 1000.0f * alpha * alpha * alpha * alpha * alpha;
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
	GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	color = (GXColor){ 0, 0, 0, 0x80 };
	GXSetTevColor(GX_TEVREG0, color);
	TEXGetGXTexObjFromPalette(fadeGetTpl(), &texObj, 1);
	GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumTexGens(1);
	PSMTXTrans(mtxMain, 0.5f, 0.5f, 0.0f);
	PSMTXScale(mtxScale, scale, scale, 0.0f);
	PSMTXTrans(mtxNeg, -0.5f, -0.5f, 0.0f);
	PSMTXConcat(mtxMain, mtxScale, mtxMain);
	PSMTXConcat(mtxMain, mtxNeg, mtxMain);
	GXLoadTexMtxImm(mtxMain, GX_TEXMTX0, GX_MTX2x4);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
	GXSetCurrentMtx(0);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3s16(-304, 240, 0);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3s16(304, 240, 0);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3s16(304, -240, 0);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3s16(-304, -240, 0);
	GXTexCoord2f32(0.0f, 1.0f);
}
static inline void nameWinQuad(f32 x, f32 y, f32 w, f32 h) {
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(x, y, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(x, y - h, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
	GXPosition3f32(x + w, y - h, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(x + w, y, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
}

// The second coord's scale. Separate f64 temps (hh2/ww2) reproduce the target's
// callee-saved FP assignment; a shared hh/ww pair reassigns the register the first
// coord just freed, so f23/f24 come out swapped.
#define NAME_WIN_SCALE2(H, W) \
		hh2 = __fabs(H); \
		ww2 = __fabs(W); \
		yscale = (f32)hh2 / (f32)GXGetTexObjHeight(&texObj); \
		xs = (f32)ww2 / (f32)GXGetTexObjWidth(&texObj);

#define NAME_WIN_QUAD(tex, id0, id1, id2, vx, vy, W, H) \
	{ \
		GXTexObj texObj; \
		Mtx mtx; \
		f32 xs; \
		f64 hh; \
		f64 ww; \
		f64 hh2; \
		f64 ww2; \
		GXSetNumTexGens(2); \
		TEXGetGXTexObjFromPalette((TEXPalette*)(tex), &texObj, (id0)); \
		GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1); \
		GXLoadTexObj(&texObj, GX_TEXMAP0); \
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY); \
		hh = __fabs(H); \
		ww = __fabs(W); \
		yscale = (f32)hh / (f32)GXGetTexObjHeight(&texObj); \
		xs = (f32)ww / (f32)GXGetTexObjWidth(&texObj); \
		PSMTXScale(mtx, xs, yscale, 1.0f); \
		GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4); \
		TEXGetGXTexObjFromPalette((TEXPalette*)(tex), &texObj, (id1)); \
		GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1); \
		GXLoadTexObj(&texObj, GX_TEXMAP1); \
		TEXGetGXTexObjFromPalette((TEXPalette*)(tex), &texObj, (id2)); \
		GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1); \
		GXLoadTexObj(&texObj, GX_TEXMAP2); \
		GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY); \
		NAME_WIN_SCALE2(H, W) \
		PSMTXScale(mtx, xs, yscale, 1.0f); \
		GXLoadTexMtxImm(mtx, GX_TEXMTX1, GX_MTX2x4); \
		nameWinQuad(vx, vy, W, H); \
	}

void nameWinGX(void* tex, s32 arg1, f32 x, f32 y, f32 z, f32 w) {
	GXColor color;
	f32 yscale; // shared across the four quads like nameKirinukiGX's

	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
	GXSetZMode(GX_DISABLE, GX_ALWAYS, GX_DISABLE);
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
	GXSetNumChans(0);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(3);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
	GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXA, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	color = (GXColor){ 0, 0, 0, 0x80 };
	GXSetTevColor(GX_TEVREG0, color);
	GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_CPREV, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_APREV, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevKAlphaSel(GX_TEVSTAGE2, GX_TEV_KASEL_1);
	GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, 0);
	GXSetCurrentMtx(0);

	NAME_WIN_QUAD(tex, arg1 + 4, arg1 + 5, arg1, x, y, z - 16.0f, w - 16.0f);
	NAME_WIN_QUAD(tex, arg1 + 4, arg1 + 7, arg1 + 2, x + z - 16.0f, y, 16.0f, w - 16.0f);
	// bottom row: the top edge is written as bottom+16 so it is recomputed from the
	// y-w the compiler already has; the form `y - (w - 16.0f)` gets CSE'd with the
	// upper rows and throws off the callee-saved FP assignment.
	NAME_WIN_QUAD(tex, arg1 + 4, arg1 + 6, arg1 + 1, x, y - w + 16.0f, z - 16.0f, 16.0f);
	NAME_WIN_QUAD(tex, arg1 + 4, arg1 + 8, arg1 + 3, x + z - 16.0f, y - w + 16.0f, 16.0f, 16.0f);
}
