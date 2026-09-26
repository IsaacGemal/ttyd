#include "manager/fontmgr.h"
#include "manager/dvdmgr.h"
#include "driver/camdrv.h"
#include "memory.h"
#include "system.h"

extern int sprintf(char* str, const char* format, ...);

//.sdata
GXColor fontColTbl[3] = {
	{0x1E, 0x14, 0x14, 0xFF},
	{0xDC, 0xDC, 0xDC, 0xFF},
	{0xB4, 0x78, 0x14, 0xFF}
};
s32 charspace[2] = {4, 2};
const char* fontname[2] = {"papermarioset_JPN", "popjoyB22"};
const Vec vec3_802c48c0 = {1.0f, 1.0f, 1.0f};

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

void JUTFont_DrawStart(GXColor color);
void _JUTFont_DrawPos(u16 code, s16 x, s16 y);
void FontDrawColor(GXColor* color);
u16 JUTFont_CodeToGlyph(u32 code);

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

// NONMATCHING (99.95%): only stack slots for the two inlined GXSetTevColor args are swapped (orig 0xc/0x10, ours 0x10/0xc).
extern double floor(double x);

// NONMATCHING (99.57%): correct algorithm; only float-register-allocation/constant-label diffs remain.
u32 HSV2RGB(GXColor* hsv) {
	f32 h = hsv->r * 360.0f * 0.00390625f;
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
		FontDrawColor(&c);
		_JUTFont_DrawPos(code, -2, 0);
		_JUTFont_DrawPos(code, 2, 0);
		_JUTFont_DrawPos(code, 0, -2);
		_JUTFont_DrawPos(code, 0, 2);
		c2 = saved;
		FontDrawColor(&c2);
	}
	_JUTFont_DrawPos(code, 0, 0);
}

// NONMATCHING (99.96%): same two swapped GXSetTevColor arg stack slots as FontDrawCodeMtx (plus a local float-pool label).
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
		FontDrawColor(&c);
		_JUTFont_DrawPos(code, -2, 0);
		_JUTFont_DrawPos(code, 2, 0);
		_JUTFont_DrawPos(code, 0, -2);
		_JUTFont_DrawPos(code, 0, 2);
		c2 = saved;
		FontDrawColor(&c2);
	}
	_JUTFont_DrawPos(code, 0, 0);
}

// NONMATCHING (83.60%): retail loads `color->a` before the whole-word `*color`; ours loads the word first
// and hoists the GXSetTevColor argument-copy address. Same multiset, different schedule.
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

// NONMATCHING (52.34%): by-value `JUTFont_DrawStart(GXColor)` call is correct, but CW reserves a 0x20 frame (start at 0x10) vs retail's 0x10 frame (start at 0xc) and schedules the fontColTbl/vec3 loads differently.
void FontDrawStart_alpha(u8 alpha) {
	GXColor start;

	start = fontColTbl[0];
	fontAlpha = alpha;
	fontScale = vec3_802c48c0;
	fontEdge = FALSE;
	fontColor = fontColTbl[0];
	start.a = (start.a * alpha) / 255;
	JUTFont_DrawStart(start);
}

// NONMATCHING (64.58%): `FontDrawStart_alpha(0xFF);` inlined; same frame/scheduling diffs as FontDrawStart_alpha.
//does some fucky stuff on the alpha channel that doesn't change anything
void FontDrawStart(void) {
	FontDrawStart_alpha(0xFF);
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


u16 JUTFont_CodeToGlyph(u32 code);

u16 hankakuSearch(s8 code) {
	return JUTFont_CodeToGlyph(code);
}

// NONMATCHING (87.50%): the header types `code` as u16, forcing a `clrlwi r3,r3,16` before the
// JUTFont_CodeToGlyph(u32) call; retail has no extension, so the real parameter is u32/int.
u16 kanjiSearch(u16 code) {
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
