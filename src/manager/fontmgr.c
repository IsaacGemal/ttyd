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


//.sdata
GXColor fontColTbl[3] = {
	{0x1E, 0x14, 0x14, 0xFF},
	{0xDC, 0xDC, 0xDC, 0xFF},
	{0xB4, 0x78, 0x14, 0xFF}
};
s32 charspace[2] = {4, 2};
const char* fontname[2] = {"papermarioset_JPN", "popjoyB22"};
const GXColor dat_8041a2f0 = {0x00, 0xAA, 0xFF, 0xFF};
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

//local prototypes


GXColor FontGetDrawColor(void) {
	return fontColor;
}

void FontDrawEdge(void) {
	fontEdge = TRUE;
}

void FontDrawEdgeOff(void) {
	fontEdge = FALSE;
}

void FontDrawScale(f32 scale) {
	fontScale.x = scale;
	fontScale.y = scale;
	fontScale.z = scale;
}

void FontDrawScaleVec(Vec scale) {
	fontScale = scale;
}

void FontDrawRainbowColorOff(void) {
	GXSetNumTevStages(1);
}

void FontDrawNoiseOff(void) {
	GXSetNumTexGens(1);
	GXSetNumTevStages(1);
}

void FontDrawRainbowColor(void) {
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
}

extern GXTexObj* wakuTexObj;

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

// NONMATCHING (83.60%)
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

void FontDrawColorIDX(u32 idx) {
	GXColor color;
	color = fontColTbl[idx];
	FontDrawColor(&color);
}

void fontmgrInit(void) {
	; //stubbed in retail
}

static BinaryFontWidth* FontGetWidthPtr(u16 code) {
	BinaryFontWidth* width = (BinaryFontWidth*)(pfh + 1);
	u32 blockcount = pfh->blockcount;
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

// NONMATCHING (98.42%): head block-scan register swap (pfh in r8/r6, i in r6/r8), OSRoundUp32B scheduling of __memAlloc args, and sdata2/rodata label names.
void fontmgrTexSetup(void) {
	DVDEntry* dvd;
	u32 size;
	BinaryFontBlock* ptr;
	char path[128];
	int i;

	if (!pfh) {
		sprintf(path, "%s/f/%s.bfn", getMarioStDvdRoot(), fontname[0]);
		dvd = DVDMgrOpen(path, 2, 0);
		size = OSRoundUp32B(DVDMgrGetLength(dvd));
		pfh = __memAlloc(HEAP_DEFAULT, size);
		DVDMgrRead(dvd, pfh, size, 0);
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

// NONMATCHING (98.66%): only the OSRoundUp32B() evaluation before __memAlloc differs (orig keeps len in r4
// across `li r3,0`; ours folds it into r0 before the clear) plus sdata2/rodata label names.
void JUTFontSetup(u16 idx) {
	u32 size;
	DVDEntry* dvd;
	BinaryFontBlock* ptr;
	char path[128];
	int i;

	if (!pfh) {
		sprintf(path, "%s/f/%s.bfn", getMarioStDvdRoot(), fontname[idx]);
		dvd = DVDMgrOpen(path, 2, 0);
		size = OSRoundUp32B(DVDMgrGetLength(dvd));
		pfh = __memAlloc(HEAP_DEFAULT, size);
		DVDMgrRead(dvd, pfh, size, 0);
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

extern GXColor unk_80422fd8;

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
	PSMTXScale(mtx, 0.0019531f, 0.00097656f, 1.0f);
	GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXLoadPosMtxImm(camera->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, unk_80422fd8);
}

void JUTFont_Free(void) {
	if (pfh) {
		__memFree(HEAP_DEFAULT, pfh);
		pfh = NULL;
	}
}

u16 FontGetMessageWidth(const char* message) {
	return FontGetMessageWidthLine(message, NULL);
}

// NONMATCHING (99.38%): `(s8)*message++` in the default case matched. Remaining diffs are register
// allocation only: retail reuses the maxwidth/curwidth source register (mr r29,r28, subf with r29)
// where ours uses the masked r0/r5 copy; the two anonymous @-labels are symbol-only.
u16 FontGetMessageWidthLine(const char* message, u16* lines) {
	u16 glyph;
	u16 maxwidth = 0;
	s32 curwidth = 0;
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
					curwidth = (s32)((f32)width * scale + offset + (f32)(u16)curwidth);
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
		curwidth = (u16)(curwidth + (((BinaryFontWidthEntry*)FontGetWidthPtr(glyph))->width + charspace[gp->language]));
	}
	if ((u16)maxwidth < (u16)curwidth) {
		maxwidth = curwidth;
	}
	if ((u16)maxwidth != 0) {
		maxwidth = (u16)(maxwidth - charspace[gp->language]);
	}
	if (lines) {
		*lines = linecount;
	}
	return maxwidth;
}



u16 hankakuSearch(s8 code) {
	return JUTFont_CodeToGlyph(code);
}

// The header declares `u32 code`; the original passes r3 straight to JUTFont_CodeToGlyph(u32) without a
// zero-extension (only hankakuSearch sign-extends its s8).
u16 kanjiSearch(u32 code) {
	return JUTFont_CodeToGlyph(code);
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

u16 kanjiGetWidth(u16 code) {
	BinaryFontWidth* width = FontGetWidthPtr(code);
	return *(u8*)((u8*)width + 1);
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

// NONMATCHING (95.76%): `x`/`y` after camGetCurPtr fixed f31/f30 and the mtx register; the first edge pair
// now uses `(s16)x`/`(s16)y` inline and the second caches `s32 sx`/`sy` (CenterMtx shape). Remaining: frame
// 0x1a0 vs 0x1b0 and retail CSEs the first pair's `(s16)y` where ours does not.
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
			s32 sx;
			s32 sy;
#ifdef MUST_MATCH
			FontDrawCode_ColorA(c);
#else
			FontDrawColor(&c);
#endif
			_JUTFont_DrawPos(glyph, (s16)x - 2, (s16)y);
			_JUTFont_DrawPos(glyph, (s16)x + 2, (s16)y);
			sx = (s16)x;
			sy = (s16)y;
			_JUTFont_DrawPos(glyph, sx, sy - 2);
			_JUTFont_DrawPos(glyph, sx, sy + 2);
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
static inline void FontDrawStringShake_ColorA(GXColor color) {
	FontDrawColor(&color);
}
#endif

// NONMATCHING (99.51%): only register allocation differs: retail keeps fontScale in r26 and the glyph
// width in r27 (and pfh in r7, the block pointer in r6 in the inlined FontGetWidthPtr); ours is swapped.
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
		w = *(u8*)((u8*)FontGetWidthPtr(glyph) + 1);
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

// NONMATCHING (98.65%): rodata format offsets (another function's string is emitted first), iconDispGxAlpha's
// s16 iconId (header, see sac_suki.c), the inlined FontDrawStart_alpha/FontDrawColor_ schedule and an 8-byte
// stack pad before ipos still differ.
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
					GXColor c = {0xE0, 0x00, 0x00, 0xFF};
					FontDrawColor(&c);
				}
				else if (strcmp(arg1, "/red") == 0) {
					GXColor c = fontColTbl[0];
					FontDrawColor(&c);
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
					ipos.y = (f32)y - (f32)ih * scale + (f32)a2;
					ipos.z = 0.0f;
					PSMTXMultVec(mtx, &ipos, &ipos);
					iconDispGxAlpha(&ipos, 0x10, (u16)id, fontAlpha, scale);
					FontDrawStart_alpha(fontAlpha);
					FontDrawColor_();
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
				msg++;
				glyph = JUTFont_CodeToGlyph(*(s8*)(msg - 1));
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
			GXColor c2;
			GXColor saved = fontColor;
			GXColor c = fontColTbl[0];
			FontDrawColor(&c);
			_JUTFont_DrawPos(glyph, -2, 0);
			_JUTFont_DrawPos(glyph, 2, 0);
			_JUTFont_DrawPos(glyph, 0, -2);
			_JUTFont_DrawPos(glyph, 0, 2);
			c2 = saved;
			FontDrawColor(&c2);
		}
		_JUTFont_DrawPos(glyph, 0, 0);
		x += *(u8*)((u8*)FontGetWidthPtr(glyph) + 1) + charspace[gp->language];
	}
}


#ifdef MUST_MATCH
// MUST_MATCH: the inlined FontDrawColor has a by-value parameter temp (retail r1+0xc..0x20); passing the
// colour by value through this inline is the shape that reproduces it. The natural call is under each #else.
static inline void FontDrawMessage_ColorA(GXColor color) {
	FontDrawColor(&color);
}
#endif

// NONMATCHING (98.75%): FontDrawColor's by-value temps and the edge block's colours still land in different
// stack slots than retail, and the unit-wide sda21/rodata pool labels differ.

void FontDrawMessage(s32 x0, s32 y, const char* msg) {
	s32 x = x0;
	s32 yy = y;
	u16 glyph;
#ifdef MUST_MATCH
	// MUST_MATCH: hoisting the edge's saved colour to function scope makes the compiler put it after the
	// icon locals instead of on top of them, which aligns the icon locals with retail (natural form is the
	// block-scope `GXColor saved` under the #else in the edge block).
	GXColor edgeSaved;
#endif
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
					GXColor c = {0xE0, 0x00, 0x00, 0xFF};
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
#ifdef MUST_MATCH
					// MUST_MATCH: the original reads the icon id zero-extended (iconDispGxAlpha's
					// `clrlwi r5, 16`), but include/driver/icondrv.h declares iconId s16, so a plain
					// call emits `extsh`. Cast through the signature the original used; still a direct
					// `bl iconDispGxAlpha` (same workaround as battle_menu_disp / ac_lst_rc_keep).
					((void (*)(Vec*, s16, u16, u8, f32))iconDispGxAlpha)(&ipos, 0x10, (u16)id, fontAlpha, scale);
#else
					iconDispGxAlpha(&ipos, 0x10, (u16)id, fontAlpha, scale);
#endif
					{
#ifdef MUST_MATCH
					// MUST_MATCH: hand-expanded FontDrawStart_alpha; this statement order is the one whose
					// register schedule matches retail's (the plain call inlines with a different order).
						GXColor start;
						u8 a = fontAlpha;
						start = fontColTbl[0];
						fontEdge = FALSE;
						fontColor = fontColTbl[0];
						fontAlpha = a;
						fontScale = vec3_802c48c0;
						start.a = (start.a * a) / 255;
						JUTFont_DrawStart(start);
#else
						FontDrawStart_alpha(fontAlpha);
#endif
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
			GXColor c2;
			GXColor c = fontColTbl[0];
			s16 sx;
			s16 sy;
			#ifdef MUST_MATCH
			edgeSaved = fontColor;
			FontDrawMessage_ColorA(c);
			#else
			GXColor saved = fontColor;
			FontDrawColor(&c);
			#endif
			sx = x;
			sy = yy;
			_JUTFont_DrawPos(glyph, sx - 2, sy);
			_JUTFont_DrawPos(glyph, sx + 2, sy);
			_JUTFont_DrawPos(glyph, (s16)x, (s16)(yy - 2));
			_JUTFont_DrawPos(glyph, (s16)x, (s16)(yy + 2));
			#ifdef MUST_MATCH
			c2 = edgeSaved;
			FontDrawMessage_ColorA(c2);
			#else
			c2 = saved;
			FontDrawColor(&c2);
			#endif
		}
		_JUTFont_DrawPos(glyph, (s16)x, (s16)yy);
		x += *(u8*)((u8*)FontGetWidthPtr(glyph) + 1) + charspace[gp->language];
	}
}

// immediate-mode GX quad reconstructed; loading blockGlyph after FontGetWidthPtr, declaring hueoff/hueL/hueR
// as u8 and computing hueR at its second use matched retail's schedule. width is a u8 (retail lbz's it).
// yoffs is computed as a separate multiply then add (retail's fmuls+fadds are not contracted). The right-edge
// x is (s16)(x + fwidthS) (kept in an s16 local, and the hue recomputes it cast through u16) and the
// bottom-edge y is (s16)((y - 1) - fheightS). All four vertices write HSV2RGB's result after the position pair.
// NONMATCHING (56.70%): retail keeps a taller frame (0x140 vs 0x130) and two more callee-saved GPRs; the
// preamble's int->float conversion schedule and register colours still differ.
void _JUTFont_DrawPos(u16 code, s16 x, s16 y) {
	u8* info = (u8*)FontGetWidthPtr(code);
	BinaryFontGlyph* blk = blockGlyph;
	u8 width = info[1];
	s32 n = code - blk->firstcode;
	s32 row = n / blk->sheetrows;
	s32 col = row / blk->sheetcolumns;
	f32 xoffs;
	f32 yoffs;
	f32 fwidth;
	f32 fheight;
	f32 fwidthS;
	f32 fheightS;
		s16 xr;
	s16 yb;
	u8 hueoff;
	u8 hueL;
	u8 hueR;
	GXColor hsv;
	u32 rgb;
	u8 n2;
	u8 n1;

	yoffs = (f32)(row - col * blk->sheetcolumns) * blk->cellheight;
	xoffs = (f32)(n - row * blk->sheetrows) * blk->cellwidth + info[0];
	fheight = blk->cellheight;
	yoffs = 1.0f + yoffs;
	fheightS = fheight * 1.0f;
	fwidth = width;
	fwidthS = fwidth * 1.0f;

	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	n1 = irand(64);
	n2 = irand(64);
	hueoff = (u8)gp->retraceCount * 2;
	hueL = (u8)(x + (u8)(x - hueoff));
	xr = (s16)(x + fwidthS);
	yb = (s16)(y - 1 - fheightS);

	hsv = dat_8041a2f0;
	GXPosition3s16(x, y - 1, 0);
	hsv.r = hueL;

	rgb = HSV2RGB(&hsv);
	GXColor4u8(((u8*)&rgb)[0], ((u8*)&rgb)[1], ((u8*)&rgb)[2], ((u8*)&rgb)[3]);
	GXTexCoord2u16((u16)xoffs, (u16)yoffs);
	GXTexCoord2u8(n1, n2);

	hueR = (u8)((u16)(s16)(x + fwidthS) + (u8)(x - hueoff));
	GXPosition3s16(xr, y - 1, 0);
	hsv.r = hueR;

	rgb = HSV2RGB(&hsv);
	GXColor4u8(((u8*)&rgb)[0], ((u8*)&rgb)[1], ((u8*)&rgb)[2], ((u8*)&rgb)[3]);
	GXTexCoord2u16((u16)(xoffs + fwidth), (u16)yoffs);
	GXTexCoord2u8(n1 + (u8)width, n2);

	hsv.r = hueR;
	GXPosition3s16(xr, yb, 0);

	rgb = HSV2RGB(&hsv);
	GXColor4u8(((u8*)&rgb)[0], ((u8*)&rgb)[1], ((u8*)&rgb)[2], ((u8*)&rgb)[3]);
	GXTexCoord2u16((u16)(xoffs + fwidth), (u16)(yoffs + fheight));
	GXTexCoord2u8(n1 + (u8)width, n2 + (u8)fheight);

	GXPosition3s16(x, yb, 0);
	hsv.r = hueL;

	rgb = HSV2RGB(&hsv);
	GXColor4u8(((u8*)&rgb)[0], ((u8*)&rgb)[1], ((u8*)&rgb)[2], ((u8*)&rgb)[3]);
	GXTexCoord2u16((u16)xoffs, (u16)(yoffs + fheight));
	GXTexCoord2u8(n1, n2 + (u8)fheight);
}
