#include "manager/fontmgr.h"
#include "manager/dvdmgr.h"
#include "driver/camdrv.h"
#include "driver/msgdrv.h"
#include "driver/icondrv.h"
#include "memory.h"
#include "system.h"
#include "mario/mariost.h"
#include <stdio.h>
#include <string.h>
#include "math_ext.h"

// LOCAL_PROTO(iconDispGxAlpha): fontmgr's original passes the icon id zero-extended (`clrlwi r5, 16`) and loads
// the scale before the integer arguments, so it saw the scale first, as in iconDispGx (same ABI registers).
void iconDispGxAlpha(f32 scale, Vec* position, s16 flags, u16 iconId, u8 alpha);


//.sdata
GXColor fontColTbl[3] = {
	{0x1E, 0x14, 0x14, 0xFF},
	{0xDC, 0xDC, 0xDC, 0xFF},
	{0xB4, 0x78, 0x14, 0xFF}
};
s32 charspace[2] = {4, 2};
const char* fontname[2] = {"papermarioset_JPN", "popjoyB22"};
const Vec vec3_802c48c0 = {1.0f, 1.0f, 1.0f};
const Vec vec3_802c48cc = {0.0f, 0.0f, 0.0f};
const Vec vec3_802c48d8 = {0.0f, 0.0f, 0.0f};

typedef struct {
	f32 x, y, z;
} F32Vec;

//.sbss
Vec fontScale; //same layout as Vec
GXColor fontColor;
u8 fontAlpha;
BOOL fontEdge;
BinaryFontInfo* pfi; //pointer to font info
BinaryFontGlyph* blockGlyph;
GXTexObj fgTexObj;
BinaryFontHeader* pfh; //pointer to font header

extern GXTexObj* wakuTexObj;
extern GXColor unk_80422fd8;

void fontmgrInit(void) {
	; //stubbed in retail
}

static BinaryFontWidth* FontGetWidthPtr(u16 code) {
	// The two block locals' declaration order sets the temp numbering for every inlined caller.
	u32 blockcount = pfh->blockcount;
	BinaryFontWidth* width = (BinaryFontWidth*)(pfh + 1);
	u32 i;

	for (i = 0; i < blockcount; i++) {
		switch (width->magic) {
			case 'WID1':
				if (code >= width->firstcode && code <= width->lastcode) {
					return (BinaryFontWidth*)((u8*)width + (code - width->firstcode) * 2 + 0xc);
				}
				break;
		}
		width = (BinaryFontWidth*)((u32)width + width->size);
	}
	return NULL;
}

#ifdef MUST_MATCH
// MUST_MATCH: inlining the scan with ptr declared before i gives header/i retail registers r8/r6.
static inline void fontmgrScanBlocks(BinaryFontHeader* header) {
	BinaryFontBlock* ptr;
	int i;

	ptr = (BinaryFontBlock*)(header + 1);
	for (i = 0; i < header->blockcount; i++) {
		switch (ptr->magic) {
			case 'INF1':
				pfi = (BinaryFontInfo*)ptr;
				break;
			case 'GLY1':
				blockGlyph = (BinaryFontGlyph*)ptr;
				break;
			case 'MAP1':
			case 'WID1':
			default:
				break;
		}
		ptr = (BinaryFontBlock*)((u32)ptr + ptr->size);
	}
}
#endif

void fontmgrTexSetup(void) {
	DVDEntry* dvd;
#ifdef MUST_MATCH
	u32 rounded;
#endif
	u32 size;
#ifndef MUST_MATCH
	BinaryFontBlock* ptr;
#endif
	char path[128];
#ifndef MUST_MATCH
	int i;
#endif

	if (!pfh) {
		sprintf(path, "%s/f/%s.bfn", getMarioStDvdRoot(), fontname[0]);
		dvd = DVDMgrOpen(path, 2, 0);
		size = DVDMgrGetLength(dvd);
#ifdef MUST_MATCH
		// Keeping the assignment in the argument preserves size in r4 and allocates dvd/rounded to r31/r30.
		pfh = __memAlloc(HEAP_DEFAULT, rounded = OSRoundUp32B(size));
		DVDMgrRead(dvd, pfh, rounded, 0);
#else
		pfh = __memAlloc(HEAP_DEFAULT, OSRoundUp32B(size));
		DVDMgrRead(dvd, pfh, OSRoundUp32B(size), 0);
#endif
		DVDMgrClose(dvd);
#ifdef MUST_MATCH
		fontmgrScanBlocks(pfh);
#else
		ptr = (BinaryFontBlock*)(pfh + 1);
		for (i = 0; i < pfh->blockcount; i++) {
			switch (ptr->magic) {
				case 'INF1':
					pfi = (BinaryFontInfo*)ptr;
					break;
				case 'GLY1':
					blockGlyph = (BinaryFontGlyph*)ptr;
					break;
				case 'MAP1':
				case 'WID1':
				default:
					break;
			}
			ptr = (BinaryFontBlock*)((u32)ptr + ptr->size);
		}
#endif
		GXInitTexObj(
			&fgTexObj,
			&blockGlyph[1],
			blockGlyph->sheetwidth,
			blockGlyph->sheetheight,
			GX_TF_I4,
			GX_CLAMP,
			GX_CLAMP,
			GX_FALSE);
		GXInitTexObjLOD(
			&fgTexObj,
			GX_LINEAR,
			GX_LINEAR,
			0.0f,
			0.0f,
			0.0f,
			GX_FALSE,
			GX_FALSE,
			GX_ANISO_1);

		((u8*)FontGetWidthPtr(JUTFont_CodeToGlyph(0x20)))[1] = 7;
		((u8*)FontGetWidthPtr(JUTFont_CodeToGlyph(0x8140)))[1] = 9;
	}
}

// NONMATCHING (95.06%): storing fontScale/fontAlpha/fontColor before start.a is the order CW schedules to
// retail's bytes; remaining diffs are register allocation and the start/by-value-arg slots are swapped
// (retail start@r1+0x8, arg@r1+0xc; FontDrawStart_alpha has the opposite and matches).
void FontDrawStart(void) {
	GXColor start;

	start = fontColTbl[0];
	fontScale = vec3_802c48c0;
	fontAlpha = 0xFF;
	*(u32*)&fontColor = *(u32*)&fontColTbl[0];
	start.a = (start.a * 0xFF) / 255;
	fontEdge = FALSE;
	JUTFont_DrawStart(start);
}

// NONMATCHING (60.40%): by-value `JUTFont_DrawStart(GXColor)` call and frame now match retail (0x10);
// remaining diffs are the fontColTbl/vec3x load schedule and register allocation.
void FontDrawStart_alpha(u8 alpha) {
	GXColor start;

	start = fontColTbl[0];
	start.a = (start.a * alpha) / 255;
#ifdef MUST_MATCH
	// MUST_MATCH: the natural `fontColor = fontColTbl[0];` makes CW reserve an extra 0x10 stack slot
	// (frame 0x20 instead of retail's 0x10); copying the word keeps the frame at 0x10.
	*(u32*)&fontColor = *(u32*)&fontColTbl[0];
#else
	fontColor = fontColTbl[0];
#endif
	fontAlpha = alpha;
	fontScale = vec3_802c48c0;
	fontEdge = FALSE;
	JUTFont_DrawStart(start);
}

void FontDrawEdge(void) {
	fontEdge = TRUE;
}

void FontDrawEdgeOff(void) {
	fontEdge = FALSE;
}

void FontDrawRainbowColor(void) {
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
}

void FontDrawRainbowColorOff(void) {
	GXSetNumTevStages(1);
}

void FontDrawNoise(void) {
	Mtx mtx;

	GXSetNumTexGens(2);
	GXSetNumTevStages(2);
	GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_1);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
	GXLoadTexObj(&wakuTexObj[50], GX_TEXMAP2);
	PSMTXScale(mtx, 0.015625f, 0.015625f, 1.0f);
	GXLoadTexMtxImm(mtx, 0x21, GX_MTX2x4);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, 0x21, GX_FALSE, 0x7d);
}

void FontDrawNoiseOff(void) {
	GXSetNumTexGens(1);
	GXSetNumTevStages(1);
}

void FontDrawColorIDX(u32 idx) {
	GXColor color;
	color = fontColTbl[idx];
	FontDrawColor(&color);
}

// NONMATCHING (83.60%): only the first two loads are ordered differently. Retail loads color->a into r5
// before the fontColor word load (lwz r7); ours loads the word first (r5), stores it, and reloads the byte into
// r5. Every other instruction, including the /255 magic sequence and the GXSetTevColor argument setup, matches;
// the GXSetTevColor argument pointer (addi r4) also sits one slot earlier in ours.
void FontDrawColor(GXColor* color) {
	fontColor = *color;
	color->a = (color->a * fontAlpha) / 255;
	GXSetTevColor(GX_TEVREG0, *color);
}

void FontDrawColor_(void) {
	GXColor color;
	color = fontColor;
	FontDrawColor(&color);
}

//local prototypes


GXColor FontGetDrawColor(void) {
	return fontColor;
}

void FontDrawScale(f32 scale) {
	fontScale.x = scale;
	fontScale.y = scale;
	fontScale.z = scale;
}

void FontDrawScaleVec(Vec scale) {
	fontScale = scale;
}

#ifdef MUST_MATCH
// MUST_MATCH: the first colour goes through a by-value inline, which is the only shape that puts that
// call's by-value GXSetTevColor argument copy at r1+0xc and the second call's at r1+0x10; the direct
// `FontDrawColor(&c)` form allocates the two in the opposite order (0x10 then 0xc). The 0.0f passed to
// PSMTXTransApply pools as an anonymous @-label (same value as retail's float_0_8041a300), which objdiff
// scores equal, so the function is 100%.
static inline void FontDrawCode_ColorA(GXColor color) {
	FontDrawColor(&color);
}
#endif

void FontDrawCode(u16 code, f32 x, f32 y) {
	Mtx mtx;
	CameraEntry* cam;

	PSMTXScale(mtx, fontScale.x, fontScale.y, fontScale.z);
	PSMTXTransApply(mtx, mtx, x, y, 0.0f);
	cam = camGetCurPtr();
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	if (fontEdge) {
		GXColor c2;
		GXColor saved = fontColor;
		GXColor c = fontColTbl[0];
#ifdef MUST_MATCH
		FontDrawCode_ColorA(c);
#else
		FontDrawColor(&c);
#endif
		_JUTFont_DrawPos(code, -2, 0);
		_JUTFont_DrawPos(code, 2, 0);
		_JUTFont_DrawPos(code, 0, -2);
		_JUTFont_DrawPos(code, 0, 2);
		c2 = saved;
		FontDrawColor(&c2);
	}
	_JUTFont_DrawPos(code, 0, 0);
}

#ifdef MUST_MATCH
// MUST_MATCH: passing the first colour by value through this inline is the only shape that puts the
// first GXSetTevColor argument copy at r1+0xc and the second at r1+0x10; the direct `FontDrawColor(&c)`
// form allocates `c` at r1+0x10 and `c2` at r1+0xc (opposite of retail).
static inline void FontDrawCodeMtx_ColorA(GXColor color) {
	FontDrawColor(&color);
}
#endif

void FontDrawCodeMtx(Mtx mtx, u16 code) {
	CameraEntry* cam;

	cam = camGetCurPtr();
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	if (fontEdge) {
		GXColor c2;
		GXColor saved = fontColor;
		GXColor c = fontColTbl[0];
#ifdef MUST_MATCH
		FontDrawCodeMtx_ColorA(c);
#else
		FontDrawColor(&c);
#endif
		_JUTFont_DrawPos(code, -2, 0);
		_JUTFont_DrawPos(code, 2, 0);
		_JUTFont_DrawPos(code, 0, -2);
		_JUTFont_DrawPos(code, 0, 2);
		c2 = saved;
		FontDrawColor(&c2);
	}
	_JUTFont_DrawPos(code, 0, 0);
}

// Matched: a local `str` copied from the param `s` (declared between glyph and w) reproduces retail's
// str=r28/glyph=r29/w=r27 split; assigning fx before PSMTXScale and fz before fy after it reproduces
// the float-load schedule; `(s8)*str++` puts the single-byte pointer increment before the sign-extend.
void FontDrawString(const char* s, f32 x, f32 y) {
	CameraEntry* cam;
	Mtx mtx;
	Mtx mtx2;
	Vec pos2;
	Vec pos;
	f32 fx;
	f32 fz;
	f32 fy;
	u16 glyph;
	const char* str;
	u16 w;

	str = s;
	pos = vec3_802c48cc;
	pos.x = x;
	pos.y = y;
	pos2 = pos;
	cam = camGetCurPtr();
	fx = pos2.x;
	PSMTXScale(mtx, fontScale.x, fontScale.y, fontScale.z);
	fz = pos2.z;
	fy = pos2.y;
	while (*(s8*)str) {
		if (_ismbblead(*(s8*)str)) {
			glyph = JUTFont_CodeToGlyph(*(u16*)str);
			str += 2;
		}
		else {
			glyph = JUTFont_CodeToGlyph((s8)*str++);
		}
		w = *(u8*)((u8*)FontGetWidthPtr(glyph) + 1);
		PSMTXTransApply(mtx, mtx2, fx, fy, fz);
		PSMTXConcat(cam->view, mtx2, mtx2);
		GXLoadPosMtxImm(mtx2, GX_PNMTX0);
		if (fontEdge) {
			GXColor c2;
			GXColor saved = fontColor;
			GXColor c = fontColTbl[0];
#ifdef MUST_MATCH
			FontDrawCode_ColorA(c);
#else
			FontDrawColor(&c);
#endif
			_JUTFont_DrawPos(glyph, -2, 0);
			_JUTFont_DrawPos(glyph, 2, 0);
			_JUTFont_DrawPos(glyph, 0, -2);
			_JUTFont_DrawPos(glyph, 0, 2);
			c2 = saved;
			FontDrawColor(&c2);
		}
		_JUTFont_DrawPos(glyph, 0, 0);
		fx += fontScale.x * (f32)(w + charspace[gp->language]);
	}
}

#ifdef MUST_MATCH
// MUST_MATCH: passing the first colour by value through this inline is the only shape that puts the
// first GXSetTevColor argument copy at r1+0xc and the second at r1+0x10; the direct `FontDrawColor(&c)`
// form allocates `c` at r1+0x10 and `c2` at r1+0xc (opposite of retail).
static inline void FontDrawStringPitch_ColorA(GXColor color) {
	FontDrawColor(&color);
}
#endif

// Matched: a local `str` copied from the param `s` (between glyph and w) gives retail's w=r27/str=r28
// split; assigning fx before PSMTXScale and fz before fy after it fixes the float-load schedule; the
// sign-extended loop test `*(s8*)str` and `(s8)*str++` reproduce the extsb/increment order.
void FontDrawStringPitch(const char* s, f32 x, f32 y, f32 pitch) {
	CameraEntry* cam;
	Mtx mtx;
	Mtx mtx2;
	Vec pos2;
	Vec pos;
	f32 fx;
	f32 fz;
	f32 fy;
	f32 tx;
	u16 glyph;
	const char* str;
	u16 w;

	str = s;
	pos = vec3_802c48d8;
	pos.x = x;
	pos.y = y;
	pos2 = pos;
	cam = camGetCurPtr();
	fx = pos2.x;
	PSMTXScale(mtx, fontScale.x, fontScale.y, fontScale.z);
	fz = pos2.z;
	fy = pos2.y;
	while (*(s8*)str) {
		if (_ismbblead(*(s8*)str)) {
			glyph = JUTFont_CodeToGlyph(*(u16*)str);
			str += 2;
		}
		else {
			glyph = JUTFont_CodeToGlyph((s8)*str++);
		}
		w = *(u8*)((u8*)FontGetWidthPtr(glyph) + 1);
		tx = (pitch < 0.0f) ? fx : fx + fontScale.x * (0.5f * (pitch - (f32)w));
		PSMTXTransApply(mtx, mtx2, tx, fy, fz);
		PSMTXConcat(cam->view, mtx2, mtx2);
		GXLoadPosMtxImm(mtx2, GX_PNMTX0);
		if (fontEdge) {
			GXColor c2;
			GXColor saved = fontColor;
			GXColor c = fontColTbl[0];
#ifdef MUST_MATCH
			FontDrawStringPitch_ColorA(c);
#else
			FontDrawColor(&c);
#endif
			_JUTFont_DrawPos(glyph, -2, 0);
			_JUTFont_DrawPos(glyph, 2, 0);
			_JUTFont_DrawPos(glyph, 0, -2);
			_JUTFont_DrawPos(glyph, 0, 2);
			c2 = saved;
			FontDrawColor(&c2);
		}
		_JUTFont_DrawPos(glyph, 0, 0);
		if (pitch < 0.0f) {
			fx += fontScale.x * (f32)(w + charspace[gp->language]);
		}
		else {
			fx += pitch * fontScale.x;
		}
	}
}

#ifdef MUST_MATCH
// MUST_MATCH: passing the first colour by value through this inline is the only shape that puts the
// first GXSetTevColor argument copy at r1+0xc and the second at r1+0x10; the direct `FontDrawColor(&c)`
// form allocates `c` at r1+0x10 and `c2` at r1+0xc (opposite of retail).
static inline void FontDrawStringVecPitch_ColorA(GXColor color) {
	FontDrawColor(&color);
}
#endif

// Matched: sign-extended loop test `*(s8*)text` and `(s8)*text++` fix the extsb/increment order.
void FontDrawStringVecPitch(Vec* pos, const char* text, f32 pitch) {
	CameraEntry* cam;
	Mtx mtx2;
	Mtx mtx;
	f32 x;
	f32 f1;
	u16 w;
	u16 glyph;

	cam = camGetCurPtr();
	x = pos->x;
	PSMTXScale(mtx, fontScale.x, fontScale.y, fontScale.z);
	while (*(s8*)text) {
		if (_ismbblead(*(s8*)text)) {
			glyph = JUTFont_CodeToGlyph(*(u16*)text);
			text += 2;
		}
		else {
			glyph = JUTFont_CodeToGlyph((s8)*text++);
		}
		w = *(u8*)((u8*)FontGetWidthPtr(glyph) + 1);
		f1 = (pitch < 0.0f) ? x : x + fontScale.x * (0.5f * (pitch - (f32)w));
		PSMTXTransApply(mtx, mtx2, f1, pos->y, pos->z);
		PSMTXConcat(cam->view, mtx2, mtx2);
		GXLoadPosMtxImm(mtx2, GX_PNMTX0);
		if (fontEdge) {
			GXColor c2;
			GXColor saved = fontColor;
			GXColor c = fontColTbl[0];
#ifdef MUST_MATCH
			FontDrawStringVecPitch_ColorA(c);
#else
			FontDrawColor(&c);
#endif
			_JUTFont_DrawPos(glyph, -2, 0);
			_JUTFont_DrawPos(glyph, 2, 0);
			_JUTFont_DrawPos(glyph, 0, -2);
			_JUTFont_DrawPos(glyph, 0, 2);
			c2 = saved;
			FontDrawColor(&c2);
		}
		_JUTFont_DrawPos(glyph, 0, 0);
		if (pitch < 0.0f) {
			x += fontScale.x * (f32)(w + charspace[gp->language]);
		}
		else {
			x += pitch * fontScale.x;
		}
	}
}

// Matched: the s16 first-pair locals give the two separate conversion pairs (x=r27/y=r28 then y=r27/x=r28);
// the MUST_MATCH `sy + 0` promotion makes MWCC emit the first pair's y extsh that retail has, and the s32
// second-pair locals (ty declared before tx) get its fctiwz order and register assignment right.
void FontDrawStringMtx(Mtx mtx, const char* msg) {
	CameraEntry* cam;
	Mtx m;
	f32 x;
	f32 y;
	u16 glyph;

	cam = camGetCurPtr();
	x = 0.0f;
	y = 0.0f;
	PSMTXConcat(cam->view, mtx, m);
	GXLoadPosMtxImm(m, GX_PNMTX0);
	while (*(s8*)msg) {
		glyph = 0xFFFF;
		if (_ismbblead(*(s8*)msg)) {
			glyph = JUTFont_CodeToGlyph(*(u16*)msg);
			msg += 2;
		}
		else {
			switch (*(s8*)msg) {
			case '\n':
				x = 0.0f;
				y -= 28.0f;
				msg++;
				break;
			case '<': {
				char arg1[128];
				char arg2[128];
				msg = msgGetCommand((char*)msg + 1, arg1, arg2);
#ifdef MUST_MATCH
				if ((s8)arg1[0] == 'R' || ((s8)arg1[0] == '/' && (u32)(s8)arg1[0] == 0x2f72)) {
#else
				if (arg1[0] == 'R') {
#endif
					break;
				}
				if (*(s8*)msg == '\n') {
					msg++;
				}
				break;
			}
			default:
				glyph = JUTFont_CodeToGlyph((s8)*msg++);
				break;
			}
		}
		if (glyph == 0xFFFF) {
			continue;
		}
		if (fontEdge) {
			GXColor c2;
			GXColor saved = fontColor;
			GXColor c = fontColTbl[0];
			s16 sx;
			s16 sy;
			s32 ty;
			s32 tx;
#ifdef MUST_MATCH
			FontDrawCode_ColorA(c);
#else
			FontDrawColor(&c);
#endif
			sx = (s16)x;
			sy = (s16)y;
#ifdef MUST_MATCH
			// MUST_MATCH: `sy + 0` promotes the s16 to int, which makes MWCC emit the extsh on the
			// first pair's y that retail has. Writing sy as s32 (the natural form) makes MWCC CSE the
			// second pair's conversions and shrink the frame; `+ 0` is a no-op.
			_JUTFont_DrawPos(glyph, sx - 2, sy + 0);
			_JUTFont_DrawPos(glyph, sx + 2, sy + 0);
#else
			_JUTFont_DrawPos(glyph, sx - 2, sy);
			_JUTFont_DrawPos(glyph, sx + 2, sy);
#endif
			tx = (s16)x;
			ty = (s16)y;
			_JUTFont_DrawPos(glyph, tx, ty - 2);
			_JUTFont_DrawPos(glyph, tx, ty + 2);
			c2 = saved;
			FontDrawColor(&c2);
		}
		_JUTFont_DrawPos(glyph, (s16)x, (s16)y);
		x += (f32)(charspace[gp->language] + *(u8*)((u8*)FontGetWidthPtr(glyph) + 1));
	}
}

#ifdef MUST_MATCH
// MUST_MATCH: passing the first colour by value through this inline is the only shape that puts the
// first GXSetTevColor argument copy at r1+0xc and the second at r1+0x10; the direct `FontDrawColor(&c)`
// form allocates `c` at r1+0x10 and `c2` at r1+0xc (opposite of retail).
static inline void FontDrawStringCenterMtx_ColorA(GXColor color) {
	FontDrawColor(&color);
}
#endif

// The edge block's first pair uses `(s16)offsetX` directly; the second pair caches it in an `s32 sx` (a
// `s16 sx` omits retail's `extsh`) so the sign-extended value is reused across the two calls.
void FontDrawStringCenterMtx(Mtx mtx, const char* msg) {
	CameraEntry* cam;
	Mtx m;
	f32 offsetX;
	u16 glyph;

	cam = camGetCurPtr();
	offsetX = -(f32)FontGetMessageWidthLine(msg, NULL) * 0.5f;
	PSMTXConcat(cam->view, mtx, m);
	GXLoadPosMtxImm(m, GX_PNMTX0);
	while (*(s8*)msg) {
		if (_ismbblead(*(s8*)msg)) {
			glyph = JUTFont_CodeToGlyph(*(u16*)msg);
			msg += 2;
		}
		else {
			glyph = JUTFont_CodeToGlyph((s8)*msg++);
		}
		if (fontEdge) {
			GXColor c2;
			GXColor saved = fontColor;
			GXColor c = fontColTbl[0];
			s32 sx;
#ifdef MUST_MATCH
			FontDrawStringCenterMtx_ColorA(c);
#else
			FontDrawColor(&c);
#endif
			_JUTFont_DrawPos(glyph, (s16)offsetX - 2, 0);
			_JUTFont_DrawPos(glyph, (s16)offsetX + 2, 0);
			sx = (s16)offsetX;
			_JUTFont_DrawPos(glyph, sx, -2);
			_JUTFont_DrawPos(glyph, sx, 2);
			c2 = saved;
			FontDrawColor(&c2);
		}
		_JUTFont_DrawPos(glyph, (s16)offsetX, 0);
		offsetX += (f32)(charspace[gp->language] + *(u8*)((u8*)FontGetWidthPtr(glyph) + 1));
	}
}

#ifdef MUST_MATCH
// MUST_MATCH: passing the first colour by value through this inline is the only shape that puts the
// first GXSetTevColor argument copy at r1+0xc and the second at r1+0x10; the direct `FontDrawColor(&c)`
// form allocates `c` at r1+0x10 and `c2` at r1+0xc (opposite of retail).
static inline void FontDrawStringShake_ColorA(GXColor color) {
	FontDrawColor(&color);
}
#endif

#ifdef MUST_MATCH
// MUST_MATCH: reading the width byte through this inline numbers its result after the inlined
// FontGetWidthPtr temps, which gives the glyph width retail's r27; the plain direct read is under #else.
static inline u16 FontDrawStringShake_Width(u16 glyph) {
	return ((u8*)FontGetWidthPtr(glyph))[1];
}
#endif

void FontDrawStringShake(const char* str, f32 x, f32 y) {
	CameraEntry* cam;
	Mtx mtx2;
	Mtx mtx;
	f32 offX;
	f32 offY;
	u16 w;
	u16 glyph;

	cam = camGetCurPtr();
	PSMTXScale(mtx, fontScale.x, fontScale.y, fontScale.z);
	while (*(s8*)str) {
		if (_ismbblead(*(s8*)str)) {
			glyph = JUTFont_CodeToGlyph(*(u16*)str);
			str += 2;
		}
		else {
			glyph = JUTFont_CodeToGlyph((s8)*str++);
		}
		offX = fontScale.x * (f32)(irand(10000) % 3) + x;
		offY = fontScale.y * (f32)(irand(10000) % 3) + y;
#ifdef MUST_MATCH
		w = FontDrawStringShake_Width(glyph);
#else
		w = ((u8*)FontGetWidthPtr(glyph))[1];
#endif
		PSMTXTransApply(mtx, mtx2, offX, offY, 0.0f);
		PSMTXConcat(cam->view, mtx2, mtx2);
		GXLoadPosMtxImm(mtx2, GX_PNMTX0);
		if (fontEdge) {
			GXColor c2;
			GXColor saved = fontColor;
			GXColor c = fontColTbl[0];
#ifdef MUST_MATCH
			FontDrawStringShake_ColorA(c);
#else
			FontDrawColor(&c);
#endif
			_JUTFont_DrawPos(glyph, -2, 0);
			_JUTFont_DrawPos(glyph, 2, 0);
			_JUTFont_DrawPos(glyph, 0, -2);
			_JUTFont_DrawPos(glyph, 0, 2);
			c2 = saved;
			FontDrawColor(&c2);
		}
		_JUTFont_DrawPos(glyph, 0, 0);
		x += fontScale.x * (f32)(w + charspace[gp->language]);
	}
}

#ifdef MUST_MATCH
// MUST_MATCH: the inlined FontDrawColor has a by-value parameter temp (retail r1+0xc..0x20); passing the
// colour by value through this inline is the shape that reproduces it. The natural call is under each #else.
static inline void FontDrawMessage_ColorA(GXColor color) {
	FontDrawColor(&color);
}

// MUST_MATCH: routing the edge block through this inline makes the compiler allocate saved/c2 in the
// inline pass, i.e. after the inlined FontDrawStart_alpha `start`, which is retail's slot order.
static inline void FontDrawMessage_Edge(u16 glyph, s32 x, s32 yy) {
	GXColor c2;
	GXColor saved = fontColor;
	s16 sx;
	s16 sy;
	FontDrawColorIDX(0);
	sx = x;
	sy = yy;
	_JUTFont_DrawPos(glyph, sx - 2, sy);
	_JUTFont_DrawPos(glyph, sx + 2, sy);
	{
		s16 sx2 = x;
		s16 sy2 = yy;
		_JUTFont_DrawPos(glyph, sx2, sy2 - 2);
		_JUTFont_DrawPos(glyph, sx2, sy2 + 2);
	}
	c2 = saved;
	FontDrawColor(&c2);
}
#endif

// FontDrawMessage_Edge's second pair (the vertical offsets) narrows x and yy into fresh s16 locals in its own block:
// the original sign-extends x again there and reuses the narrowed y.
// NONMATCHING (99.94%): the instructions match; the colour branches' GXSetTevColor argument copies are 0x10 low of
// the edge saved/c2 and the icon colour temps (retail has them above), and the two .rodata int->float doubles are
// swapped (the unit's pool order comes from _JUTFont_DrawPos, the first function emitted).
void FontDrawMessage(s32 x0, s32 y, const char* msg) {
	s32 x = x0;
	s32 yy = y;
	u16 glyph;
	char arg1[128];
	char arg2[128];

	while (*(s8*)msg) {
		glyph = 0xFFFF;
		if (_ismbblead(*(s8*)msg)) {
			glyph = JUTFont_CodeToGlyph(*(u16*)msg);
			msg += 2;
		}
		else {
			switch ((s8)*msg) {
			case '\n':
				x = x0;
				yy -= 28;
				msg++;
				break;
			case '<':
				msg = msgGetCommand((char*)msg + 1, arg1, arg2);
				if (strcmp(arg1, "red") == 0) {
					GXColor c;
					c = (GXColor){0xE0, 0x00, 0x00, 0xFF};
					#ifdef MUST_MATCH
					FontDrawMessage_ColorA(c);
					#else
					FontDrawColor(&c);
					#endif
				}
				else if (strcmp(arg1, "/red") == 0) {
					GXColor c = fontColTbl[0];
					#ifdef MUST_MATCH
					FontDrawMessage_ColorA(c);
					#else
					FontDrawColor(&c);
					#endif
				}
				else if (strcmp(arg1, "col") == 0) {
					GXColor c;
					sscanf(arg2, "%x", &c);
					#ifdef MUST_MATCH
					FontDrawMessage_ColorA(c);
					#else
					FontDrawColor(&c);
					#endif
				}
				else if (strcmp(arg1, "/col") == 0) {
					GXColor c = fontColTbl[0];
					#ifdef MUST_MATCH
					FontDrawMessage_ColorA(c);
					#else
					FontDrawColor(&c);
					#endif
				}
				else if (strcmp(arg1, "icon") == 0) {
					s32 id;
					s32 a1;
					s32 a2;
					s32 offset;
					f32 scale;
					char name[32];
					u16 iw;
					u16 ih;
					Vec ipos;

					camGetCurPtr();
					if ((s8)arg2[0] >= '0' && (s8)arg2[0] <= '9') {
						sscanf(arg2, "%d%f%d%d%d", &id, &scale, &a1, &a2, &offset);
					}
					else {
						sscanf(arg2, "%s%f%d%d%d", name, &scale, &a1, &a2, &offset);
						id = msgIconStr2ID(name);
						if (id < 0) {
							id = 0;
						}
					}
					iconGetWidthHight(&iw, &ih, (u16)id);
					ipos.x = (f32)x + (0.5f * (f32)iw) * scale + (f32)a1;
					ipos.y = (f32)yy - (f32)ih * scale + (f32)a2;
					ipos.z = 0.0f;
					iconDispGxAlpha(scale, &ipos, 0x10, id, fontAlpha);
					{
						FontDrawStart_alpha(fontAlpha);
					}
					{
						GXColor c = fontColor;
						#ifdef MUST_MATCH
						FontDrawMessage_ColorA(c);
						#else
						FontDrawColor(&c);
						#endif
					}
					x = (s32)((f32)x + ((f32)iw * scale + (f32)offset));
				}
#ifdef MUST_MATCH // retail emits `cmpwi r0,0x2f / bne / cmplwi r0,0x2f72`; the second test is always
                 // false (r0 is a sign-extended byte), so this disjunct is a no-op. Natural form is below.
				else if ((s8)arg1[0] == 'R' || ((s8)arg1[0] == '/' && (u32)(s8)arg1[0] == 0x2f72)) {
#else
				else if ((s8)arg1[0] == 'R') {
#endif
					break;
				}
				if ((s8)*msg == '\n') {
					msg++;
				}
				break;
			default:
				msg++;
				glyph = JUTFont_CodeToGlyph((s8)*(msg - 1));
				break;
			}
		}
		if (glyph == 0xFFFF) {
			continue;
		}
		if (fontEdge) {
#ifdef MUST_MATCH
			FontDrawMessage_Edge(glyph, x, yy);
#else
			GXColor c2;
			GXColor saved = fontColor;
			s16 sx;
			s16 sy;
			FontDrawColorIDX(0);
			sx = x;
			sy = yy;
			_JUTFont_DrawPos(glyph, sx - 2, sy);
			_JUTFont_DrawPos(glyph, sx + 2, sy);
			_JUTFont_DrawPos(glyph, (s16)x, (s16)(yy - 2));
			_JUTFont_DrawPos(glyph, (s16)x, (s16)(yy + 2));
			c2 = saved;
			FontDrawColor(&c2);
#endif
		}
		_JUTFont_DrawPos(glyph, (s16)x, (s16)yy);
		x += *(u8*)((u8*)FontGetWidthPtr(glyph) + 1) + charspace[gp->language];
	}
}

#ifdef MUST_MATCH
// MUST_MATCH: routing the edge block through this inline makes the compiler allocate saved/c2 in the
// inline pass, i.e. after the inlined FontDrawStart_alpha `start`, which is retail's slot order.
static inline void FontDrawMessageMtx_Edge(u16 glyph) {
	GXColor saved = fontColor;
	GXColor c2;
	FontDrawColorIDX(0);
	_JUTFont_DrawPos(glyph, -2, 0);
	_JUTFont_DrawPos(glyph, 2, 0);
	_JUTFont_DrawPos(glyph, 0, -2);
	_JUTFont_DrawPos(glyph, 0, 2);
	c2 = saved;
	FontDrawColor(&c2);
}
#endif

// Icon scalars hoisted to function scope (MUST_MATCH); the edge block is routed through FontDrawMessageMtx_Edge so
// its saved/c2 land after the inlined FontDrawStart_alpha `start`. The unit's int->float pool order comes from
// _JUTFont_DrawPos, the first function emitted, where retail converts blk->cellheight (unsigned) before any signed
// value: starting it with `fheight = (f32)blk->cellheight;` makes this function 100% but drops _JUTFont_DrawPos to ~61.
// NONMATCHING (99.99%): only the two .rodata int->float doubles are swapped (0x48/0x50); see above.
void FontDrawMessageMtx(Mtx mtx, const char* msg) {
	CameraEntry* cam;
	Mtx m;
	Mtx trans;
	u16 glyph;
	s32 x;
	s32 y;
#ifdef MUST_MATCH
	// Retail materialises the newline reset value with an fctiwz of 0.0f (r28), not a `li`.
	f32 x0 = 0.0f;
#else
	s32 x0 = 0;
#endif
	char arg1[128];
	char arg2[128];
#ifdef MUST_MATCH
	// MUST_MATCH: retail allocates these icon scalars before the colour-branch locals; declaring them at
	// function scope puts them in the function's scalar block, above the branch colours (natural form is
	// the block-scope declarations in the icon branch).
	s32 id;
	s32 a1;
	s32 a2;
	s32 offset;
	f32 scale;
#endif

	cam = camGetCurPtr();
	x = 0;
	y = 0;
	while (*(s8*)msg) {
		glyph = 0xFFFF;
		if (_ismbblead(*(s8*)msg)) {
			glyph = JUTFont_CodeToGlyph(*(u16*)msg);
			msg += 2;
		}
		else {
			switch (*(s8*)msg) {
			case '\n':
				x = x0;
				y -= 28;
				msg++;
				break;
			case '<':
				msg = msgGetCommand((char*)msg + 1, arg1, arg2);
				if (strcmp(arg1, "red") == 0) {
					GXColor c;
					c = (GXColor){0xE0, 0x00, 0x00, 0xFF};
					FontDrawColor(&c);
				}
				else if (strcmp(arg1, "/red") == 0) {
					GXColor c = fontColTbl[0];
					FontDrawColor(&c);
				}
				else if (strcmp(arg1, "icon") == 0) {
#ifndef MUST_MATCH
					s32 id;
					s32 a1;
					s32 a2;
					s32 offset;
					f32 scale;
#endif
					char name[32];
					u16 iw;
					u16 ih;
					Vec ipos;

					camGetCurPtr();
					if ((s8)arg2[0] >= '0' && (s8)arg2[0] <= '9') {
						sscanf(arg2, "%d%f%d%d%d", &id, &scale, &a1, &a2, &offset);
					}
					else {
						sscanf(arg2, "%s%f%d%d%d", name, &scale, &a1, &a2, &offset);
						id = msgIconStr2ID(name);
						if (id < 0) {
							id = 0;
						}
					}
					iconGetWidthHight(&iw, &ih, (u16)id);
					ipos.x = (f32)x + (0.5f * (f32)iw) * scale + (f32)a1;
					ipos.y = (f32)y - (f32)ih * scale + (f32)a2;
					ipos.z = 0.0f;
					PSMTXMultVec(mtx, &ipos, &ipos);
					iconDispGxAlpha(scale, &ipos, 0x10, id, fontAlpha);
					FontDrawStart_alpha(fontAlpha);
#ifdef MUST_MATCH
					// MUST_MATCH: the explicit by-value GXColor temp is the shape that puts the post-icon colour at
					// retail's 0x44 stack slot (natural form is the FontDrawColor_() call under the #else).
					{
						GXColor c = fontColor;
						FontDrawColor(&c);
					}
#else
					FontDrawColor_();
#endif
					x = (s32)((f32)x + ((f32)iw * scale + (f32)offset));
				}
				// Retail's `/r` test is dead: the compiler compares the sign-extended
				// first byte against the two-char constant 0x2f72 ('/r'). Reproduce it.
				else if ((s8)arg1[0] == 'R' || (s8)arg1[0] == '/' && (u32)(s8)arg1[0] == 0x2f72) {
					break;
				}
				if (*(s8*)msg == '\n') {
					msg++;
				}
				break;
			default:
				glyph = JUTFont_CodeToGlyph((s8)*msg++);
				break;
			}
		}
		if (glyph == 0xFFFF) {
			continue;
		}
		PSMTXTrans(trans, (f32)x, (f32)y, 0.0f);
		PSMTXConcat(cam->view, mtx, m);
		PSMTXConcat(m, trans, m);
		GXLoadPosMtxImm(m, GX_PNMTX0);
		if (fontEdge) {
#ifdef MUST_MATCH
			FontDrawMessageMtx_Edge(glyph);
#else
			GXColor saved = fontColor;
			GXColor c2;
			FontDrawColorIDX(0);
			_JUTFont_DrawPos(glyph, -2, 0);
			_JUTFont_DrawPos(glyph, 2, 0);
			_JUTFont_DrawPos(glyph, 0, -2);
			_JUTFont_DrawPos(glyph, 0, 2);
			c2 = saved;
			FontDrawColor(&c2);
#endif
		}
		_JUTFont_DrawPos(glyph, 0, 0);
		x += *(u8*)((u8*)FontGetWidthPtr(glyph) + 1) + charspace[gp->language];
	}
}

// msgAnalize passes its u32 character straight through (no extsb at the call), so the parameter is u32 and
// the callee does the `(s8)`.
u16 hankakuSearch(u32 code) {
	return JUTFont_CodeToGlyph((s8)code);
}

// The header declares `u32 code`; the original passes r3 straight to JUTFont_CodeToGlyph(u32) without a
// zero-extension (only hankakuSearch sign-extends, through its `(s8)` cast).
u16 kanjiSearch(u32 code) {
	return JUTFont_CodeToGlyph(code);
}

u16 kanjiGetWidth(u16 code) {
	BinaryFontWidth* width = FontGetWidthPtr(code);
	return *(u8*)((u8*)width + 1);
}

// maxwidth and curwidth are both u16; the original truncates curwidth with a compound assignment
// (no zero-extension before the add) and its final subtraction reads maxwidth in a 32-bit context.
u16 FontGetMessageWidthLine(const char* message, u16* lines) {
	u16 glyph;
	u16 maxwidth = 0;
	u16 curwidth = 0;
	s32 linecount = 0;

	while (*(s8*)message != 0) {
		glyph = 0xFFFF;
		if (_ismbblead(*(s8*)message)) {
			glyph = JUTFont_CodeToGlyph(*(u16*)message);
			message += 2;
		}
		else {
			switch (*(s8*)message) {
			case '\n':
				if ((u16)maxwidth < (u16)curwidth) {
					maxwidth = curwidth;
				}
				curwidth = 0;
				linecount++;
				message++;
				break;
			case '<': {
				char arg1[128];
				char arg2[128];
				char name[32];
				s32 id;
				s32 a1;
				s32 a2;
				s32 offset;
				f32 scale;
				u16 width;
				u16 height;

				message = msgGetCommand((char*)message + 1, arg1, arg2);
				if (strcmp(arg1, "icon") == 0) {
					camGetCurPtr();
					if ((s8)arg2[0] >= '0' && (s8)arg2[0] <= '9') {
						sscanf(arg2, "%d%f%d%d%d", &id, &scale, &a1, &a2, &offset);
					}
					else {
						sscanf(arg2, "%s%f%d%d%d", name, &scale, &a1, &a2, &offset);
						id = msgIconStr2ID(name);
						if (id < 0) {
							id = 0;
						}
					}
					iconGetWidthHight(&width, &height, (u16)id);
					curwidth = (f32)width * scale + offset + (f32)curwidth;
				}
#ifdef MUST_MATCH // retail's second test is dead (compares a sign-extended byte against 0x2f72)
				else if ((s8)arg1[0] == 'R' || ((s8)arg1[0] == '/' && (u32)(s8)arg1[0] == 0x2f72)) {
#else
				else if ((s8)arg1[0] == 'R') {
#endif
					break;
				}
				if ((s8)*message == '\n') {
					message++;
				}
				break;
			}
			default:
				glyph = JUTFont_CodeToGlyph((s8)*message++);
				break;
			}
		}
		if (glyph == 0xFFFF) {
			continue;
		}
		curwidth += ((BinaryFontWidthEntry*)FontGetWidthPtr(glyph))->width + charspace[gp->language];
	}
	if ((u16)maxwidth < (u16)curwidth) {
		maxwidth = curwidth;
	}
	if ((u16)maxwidth != 0) {
		maxwidth = (s32)maxwidth - charspace[gp->language];
	}
	if (lines) {
		*lines = linecount;
	}
	return maxwidth;
}

u16 FontGetMessageWidth(const char* message) {
	return FontGetMessageWidthLine(message, NULL);
}

u32 HSV2RGB(GXColor* hsv) {
	f32 h = hsv->r * 360.0f / 256.0f;
	f32 s = hsv->g / 255.0f;
	f32 v = hsv->b / 255.0f;
	f32 f = h / 60.0f;
	s32 i = floor(f);
	f32 m, n, r, g, b;
	GXColor out;

	f = f - (f32)i;
	if (!(i & 1)) {
		f = 1.0f - f;
	}
	m = v * (1.0f - s);
	n = v * (1.0f - s * f);
	switch (i) {
		case 0: r = v; g = n; b = m; break;
		case 1: r = n; g = v; b = m; break;
		case 2: r = m; g = v; b = n; break;
		case 3: r = m; g = n; b = v; break;
		case 4: r = n; g = m; b = v; break;
		case 5: r = v; g = m; b = n; break;
	}
	out.a = hsv->a;
	out.r = 255.0f * r;
	out.g = 255.0f * g;
	out.b = 255.0f * b;
	return *(u32*)&out;
}

void JUTFontSetup(u16 idx) {
	u32 size;
	DVDEntry* dvd;
	BinaryFontBlock* ptr;
	char path[128];
	int i;

	if (!pfh) {
		sprintf(path, "%s/f/%s.bfn", getMarioStDvdRoot(), fontname[idx]);
		dvd = DVDMgrOpen(path, 2, 0);
		size = DVDMgrGetLength(dvd);
		pfh = __memAlloc(HEAP_DEFAULT, OSRoundUp32B(size));
		DVDMgrRead(dvd, pfh, OSRoundUp32B(size), 0);
		DVDMgrClose(dvd);
		ptr = (BinaryFontBlock*)(pfh + 1);
		for (i = 0; i < pfh->blockcount; i++) {
			switch (ptr->magic) {
				case 'INF1':
					pfi = (BinaryFontInfo*)ptr;
					break;
				case 'GLY1':
					blockGlyph = (BinaryFontGlyph*)ptr;
					break;
				case 'MAP1':
				case 'WID1':
				default:
					break;
			}
			ptr = (BinaryFontBlock*)((u32)ptr + ptr->size);
		}
		GXInitTexObj(
			&fgTexObj,
			&blockGlyph[1],
			blockGlyph->sheetwidth,
			blockGlyph->sheetheight,
			GX_TF_I4,
			GX_CLAMP,
			GX_CLAMP,
			GX_FALSE);
		GXInitTexObjLOD(
			&fgTexObj,
			GX_LINEAR,
			GX_LINEAR,
			0.0f,
			0.0f,
			0.0f,
			GX_FALSE,
			GX_FALSE,
			GX_ANISO_1);
		((u8*)FontGetWidthPtr(JUTFont_CodeToGlyph(0x20)))[1] = 7;
		((u8*)FontGetWidthPtr(JUTFont_CodeToGlyph(0x8140)))[1] = 9;
	}
}

void JUTFont_Free(void) {
	if (pfh) {
		__memFree(HEAP_DEFAULT, pfh);
		pfh = NULL;
	}
}

u16 JUTFont_CodeToGlyph(u32 code) {
	BinaryFontMap* map;
	u32 blockcount;
	u16 glyph, basecode;
	u8 sjis[2];
	u32 lead, index;
	s32 max, min;
	u32 i;

	map = (BinaryFontMap*)&pfh[1];
	glyph = pfi->replace;
	blockcount = pfh->blockcount;
	for (i = 0; i < blockcount; i++) {
		if (map->magic == 'MAP1') { //search for a correct mapping
			basecode = map->firstchar;
			if (((u16)code) >= basecode && ((u16)code) <= map->lastchar) {
				switch (map->mapping) {
					case LINEAR_CHARACTER_MAP:
						glyph = (u16)(code - basecode);
						break;

					case SJIS_CHARACTER_MAP:
						*(u16*)sjis = (u16)code;
						lead = sjis[0];
						index = sjis[1];
						if (sjis[1] >= 0x80) {
							index -= 65;
						}
						else {
							index -= 64;
						}

						if (!map->count) {
							glyph = (u16)((((lead - 136) * 188) + index) + 702);
						}
						else {
							glyph = (u16)(((lead - 136) * 188) + index - 94 + map->table[0]);
						}
					break;

					case TABLE_CHARACTER_MAP:
						glyph = map->table[(u16)code - basecode];
						break;

					case MAP_CHARACTER_MAP:
						max = map->count;
						min = 0;

						while (max >= min) {
							s32 index = (min + max) / 2;
							if ((u16)code == map->table[index * 2]) {
								glyph = map->table[2 * index + 1];
								break;
							}
							if ((u16)code < map->table[2 * index]) {
								max = index - 1;
							}
							else {
								min = index + 1;
							}
						}
					break;
				}
				break;
			}
		}
		map = (BinaryFontMap*)((u32)map + map->size);
	}
	return glyph;
}

void JUTFont_DrawStart(GXColor color) {
	CameraEntry* camera;
	Mtx mtx;

	camera = camGetCurPtr();
	__GXSetIndirectMask(0);
	GXSetCullMode(GX_CULL_NONE);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
	GXSetZMode(GX_TRUE, GX_ALWAYS, GX_FALSE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U16, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_U8, 0);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumChans(1);
	GXSetNumTexGens(1);
	GXSetTevColor(GX_TEVREG0, color);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_ONE, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXLoadTexObj(&fgTexObj, GX_TEXMAP0);
	PSMTXScale(mtx, 1.0f / 512.0f, 1.0f / 1024.0f, 1.0f);
	GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXLoadPosMtxImm(camera->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, unk_80422fd8);
}

// NONMATCHING (91.70%): frame (0x140), stack slots, pool order and every callee-saved register match retail.
// Passing the hue colour by value through fontHSV2RGB gives retail's four per-call copies; `scale` is a local so
// the `fmuls` by 1.0f survive; xr/ty0 are ints narrowed later into xr2/ty02, which gives retail's raw first use
// and masked/sign-extended later uses; hueL/hueR are named ints so they take retail's registers.
// Still different: retail loads the 1.0f constant and does fheight's fsubs later in the prologue, has a
// `mr r0, r31` before the ty1 conversion, and orders a few loads/stores in the last two vertices differently.
static inline u32 fontHSV2RGB(GXColor hsv) {
	return HSV2RGB(&hsv);
}

void _JUTFont_DrawPos(u16 code, s16 x, s16 y) {
	u16 ty1;
	s16 xr2;
	s32 hueR;
	u8 hueoff;
	s16 ys;
	s32 xr;
	u8* info = (u8*)FontGetWidthPtr(code);
	s32 hueL;
	u16 tx0;
	u8 nx1;
	u8 ny1;
	s32 yy;
	u8 n1;
	u8 n2;
	u16 tx1;
	u16 ty02;
	s16 yb;
	s32 xs;
	s32 ty0;
	BinaryFontGlyph* blk = blockGlyph;
	u8 width = info[1];
	s32 xx = (code - blk->firstcode) % blk->sheetrows;
	f32 fheight;
	f32 xoffs;
	f32 fwidthS;
	f32 fheightS;
	f32 fwidth;
	f32 yoffs;
	f32 scale = 1.0f;
	GXColor hsv;
	GXColor rgb;

	yy = (code - blk->firstcode) / blk->sheetrows;
	yy %= blk->sheetcolumns;
	fheight = blk->cellheight;
	xoffs = (f32)xx * blk->cellwidth + info[0];
	fheightS = fheight * scale;
	fwidth = info[1];
	fwidthS = width * scale;

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	n1 = irand(64);
	n2 = irand(64);
	xs = x;
	xr = (s32)(xs + fwidthS);
	ys = y - 1;
	hueoff = (u8)(xs - (u8)gp->retraceCount * 2);
	yoffs = yy * fheight;
	ty0 = (s32)(1.0f + yoffs);
	hueL = xs + hueoff;

	hsv = (GXColor){0x00, 0xAA, 0xFF, 0xFF};
	GXPosition3s16(x, ys, 0);
	hsv.r = hueL;

	*(u32*)&rgb = fontHSV2RGB(hsv);
	xr2 = xr;
	GXColor4u8(rgb.r, rgb.g, rgb.b, rgb.a);
	tx0 = (s32)xoffs;
	GXTexCoord2u16(tx0, ty0);
	ty02 = ty0;
	GXTexCoord2u8(n1, n2);

	GXPosition3s16(xr, ys, 0);
	hueR = xr2 + hueoff;
	hsv.r = hueR;

	*(u32*)&rgb = fontHSV2RGB(hsv);
	GXColor4u8(rgb.r, rgb.g, rgb.b, rgb.a);
	tx1 = (s32)(xoffs + fwidth);
	nx1 = (s32)(n1 + fwidthS);
	yb = (s32)(ys - fheightS);
	GXTexCoord2u16(tx1, ty02);
	GXTexCoord2u8(nx1, n2);

	GXPosition3s16(xr2, yb, 0);
	hsv.r = hueR;

	*(u32*)&rgb = fontHSV2RGB(hsv);
	GXColor4u8(rgb.r, rgb.g, rgb.b, rgb.a);
	ty1 = (s32)(ty02 + fheight);
	ny1 = (s32)(n2 + fheightS);
	GXTexCoord2u16(tx1, ty1);
	GXTexCoord2u8(nx1, ny1);

	GXPosition3s16(xs, yb, 0);
	hsv.r = hueL;

	*(u32*)&rgb = fontHSV2RGB(hsv);
	GXColor4u8(rgb.r, rgb.g, rgb.b, rgb.a);
	GXTexCoord2u16(tx0, ty1);
	GXTexCoord2u8(n1, ny1);
}
