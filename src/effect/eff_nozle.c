#include "eff/eff_nozle.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_kemutest.h"
#include "memory.h"

#include <dolphin/mtx.h>
#include <dolphin/gx.h>

typedef struct effNozleEntryData {
	f32 field_0x00; // 0x00
	f32 field_0x04; // 0x04
	f32 field_0x08; // 0x08
	f32 field_0x0C; // 0x0C
	f32 field_0x10; // 0x10
	f32 field_0x14; // 0x14
	f32 field_0x18; // 0x18
	f32 field_0x1C; // 0x1C
	f32 field_0x20; // 0x20
	f32 field_0x24; // 0x24
	f32 field_0x28; // 0x28
	f32 field_0x2C; // 0x2C
	f32 field_0x30; // 0x30
	s32 field_0x34; // 0x34
	s32 field_0x38; // 0x38
	f32 field_0x3C; // 0x3C
	f32 field_0x40; // 0x40
} effNozleEntryData;

typedef struct effNozleData {
	s32 type;                    // 0x00
	Vec position;                // 0x04
	f32 scaleX;                  // 0x10
	f32 scaleY;                  // 0x14
	f32 field_0x18;              // 0x18
	f32 field_0x1C;              // 0x1C
	f32 field_0x20;              // 0x20
	f32 field_0x24;              // 0x24
	f32 scale;                   // 0x28
	s32 lifetime;                // 0x2C
	s32 counter;                 // 0x30
	s32 alpha;                   // 0x34
	f32 field_0x38;              // 0x38
	f32 field_0x3C;              // 0x3C
	f32 field_0x40;              // 0x40
	effNozleEntryData entries[]; // 0x44
} effNozleData;

extern const f32 float_deg2rad_80421230;
extern const f32 float_0_80421234;
extern const f32 float_1_80421238;
extern const f32 float_100_8042123c;
extern const f32 float_neg0p041667_80421240;
extern const f32 float_neg0p083333_80421244;
extern const f32 float_300_80421248;
extern const f32 float_1p3_8042124c;
extern const f32 float_500_80421250;
extern const f32 float_1p2_80421254;
extern const f32 float_0p8_80421258;
extern const f32 float_0p002_8042125c;
extern const f32 float_0p004_80421260;
extern const f32 float_20_8042126c;
extern const u32 unk_80423300;

extern u8 lbl_803AE260[];
// The original reaches this file's tables from its .data section anchor, lbl_803AE260.
#ifdef MUST_MATCH
#define DATA_AT(off, sym) (base + (off))
#else
extern u8 nozle_1_vertex_tbl[];
extern u8 nozle_1_texcoord0_tbl[];
extern u8 nozle_1_texcoord1_tbl[];
extern u8 nozle_3_vertex_tbl[];
extern u8 nozle_3_color0_tbl[];
extern u8 nozle_3_texcoord0_tbl[];
extern u8 nozle_3_texcoord1_tbl[];
extern u8 nozle_3_dl_1_tbl[];
extern u8 nozle_3_dl_0_tbl[];
extern u8 nozle_4_vertex_tbl[];
extern u8 nozle_4_texcoord0_tbl[];
extern u8 nozle_4_texcoord1_tbl[];
#define DATA_AT(off, sym) ((u8*)(sym))
#endif
__declspec(section ".sdata") extern u8* nozle_1_dl_0_tbl[];
__declspec(section ".sdata") extern u8 nozle_1_dl_0_size_tbl[];
__declspec(section ".sdata") extern u8* nozle_1_dl_1_tbl[];
__declspec(section ".sdata") extern u8 nozle_1_dl_1_size_tbl[];
__declspec(section ".sdata") extern u8 nozle_1_normal_tbl[];
__declspec(section ".sdata") extern u8 nozle_1_color0_tbl[];
__declspec(section ".sdata") extern u8 nozle_3_dl_0_size_tbl[];
__declspec(section ".sdata") extern u8 nozle_3_dl_1_size_tbl[];
__declspec(section ".sdata") extern u8 nozle_3_normal_tbl[];
__declspec(section ".sdata") extern u8* nozle_4_dl_0_tbl[];
__declspec(section ".sdata") extern u8 nozle_4_dl_0_size_tbl[];
__declspec(section ".sdata") extern u8 nozle_4_normal_tbl[];
__declspec(section ".sdata") extern u8 nozle_4_color0_tbl[];

const Vec vec3_802fe6b8 = {0.0f, 0.0f, 0.0f};

const char str_80421264[] = "Nozle";

// 44 scale entries (22 pairs) indexed by counter/lifetime.
static u8 scale_dt[] = {
	0x00, 0x00, 0x0B, 0x0D, 0x16, 0x1B, 0x22, 0x29, 0x2E, 0x37, 0x39,
	0x45, 0x44, 0x52, 0x4F, 0x5E, 0x57, 0x69, 0x5E, 0x71, 0x64, 0x78,
	0x66, 0x7B, 0x67, 0x7C, 0x65, 0x7B, 0x62, 0x78, 0x5F, 0x74, 0x5A,
	0x6F, 0x56, 0x6B, 0x53, 0x67, 0x51, 0x65, 0x50, 0x64, 0x00, 0x00,
};

EffectEntry* effNozleEntry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 rotX, f32 rotY) {
	EffectEntry* effect;
	effNozleData* data;
	effNozleEntryData* entry;
	s32 i;
	s32 dir;
	s32 count;

	if (type == 1) {
		return effKemuTestEntry(3, x, y, z, float_1_80421238);
	}

	effect = effEntry();
	count = 1;
	switch (type) {
	case 0:
	case 1:
	case 2:
		count = 1;
		break;
	case 3:
		count = 4;
		break;
	}
	effect->type = str_80421264;
	effect->count = count;
	data = __memAlloc(HEAP_EFFECT, count * sizeof(effNozleEntryData));
	effect->userdata = data;
	effect->callback = effNozleMain;
	effect->flags |= 2;

	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scaleX = (type == 3) ? float_1_80421238 : float_0_80421234;
	data->scaleY = (type == 3) ? float_1_80421238 : float_0_80421234;
	data->scale = float_1_80421238;
	data->field_0x1C = float_0_80421234;
	data->field_0x18 = float_0_80421234;
	data->field_0x24 = float_0_80421234;
	data->field_0x20 = float_0_80421234;
	if (duration <= 0) {
		data->lifetime = 1000;
	} else {
		data->lifetime = duration;
	}
	data->counter = 0;
	data->alpha = 0xFF;
	data->field_0x3C = rotX;
	data->field_0x40 = rotY;

	entry = data->entries;
	for (i = 1; i < effect->count; i++) {
		dir = (i & 1) ? -1 : 1;
		entry->field_0x04 = (f32)(dir * 400);
		entry->field_0x08 = float_0_80421234;
		entry->field_0x0C = float_20_8042126c * (f32)i;
		entry->field_0x10 = (f32)((i & 1) ? 1 : -1);
		entry->field_0x14 = float_1_80421238;
		entry->field_0x1C = float_0_80421234;
		entry->field_0x18 = float_0_80421234;
		entry->field_0x24 = float_0_80421234;
		entry->field_0x20 = float_0_80421234;
		entry->field_0x34 = 0xFF;
		entry->field_0x38 = 0;
		entry++;
	}
	return effect;
}

// NONMATCHING (94.25%): the type-3 branch reloads data into r4 in the target where we reuse r3,
// and several block-local register assignments differ.
void effNozleMain(EffectEntry* effect) {
	effNozleData* data = effect->userdata;
	s32 i;
	s32 type;
	effNozleEntryData* entry;
	Vec pos;
	Vec out;

	out = vec3_802fe6b8;
	out.x = data->position.x;
	out.y = data->position.y;
	out.z = data->position.z;
	pos = out;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 0x20;
	}

	type = data->type;
	switch (type) {
	case 0:
	case 2:
		if ((u32)data->counter < 0x15) {
			data->scaleX = (f32)scale_dt[data->counter * 2] / float_100_8042123c;
			data->scaleY = (f32)scale_dt[data->counter * 2 + 1] / float_100_8042123c;
		}
		if ((u32)data->lifetime < 0x15) {
			data->scaleX = (f32)scale_dt[data->lifetime * 2] / float_100_8042123c;
			data->scaleY = (f32)scale_dt[data->lifetime * 2 + 1] / float_100_8042123c;
		}
		data->field_0x1C += float_neg0p041667_80421240;
		data->field_0x24 += float_neg0p083333_80421244;
		break;
	case 3:
		entry = data->entries;
		for (i = 1; i < effect->count; i++) {
			entry->field_0x04 += -entry->field_0x04 / float_300_80421248;
			if (entry->field_0x38 == 0) {
				entry->field_0x14 += (float_1p3_8042124c - entry->field_0x14) / float_500_80421250;
				if (entry->field_0x14 > float_1p2_80421254) {
					entry->field_0x38 = 1;
				}
			} else {
				entry->field_0x14 += (float_0p8_80421258 - entry->field_0x14) / float_300_80421248;
			}
			entry->field_0x1C += float_0p002_8042125c;
			entry->field_0x20 += float_0p002_8042125c;
			entry->field_0x24 += float_0p004_80421260;
			entry++;
		}
		break;
	}

	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	if (data->lifetime < 0x10) {
		data->alpha = data->lifetime << 4;
	}
	data->counter++;
	if (data->lifetime < 0) {
		effDelete(effect);
	} else if (type == 3) {
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 8, effNozleDisp, effect);
	} else {
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effNozleDisp, effect);
	}
}

// NONMATCHING (99.11%): data lands in r28 instead of the target's r27, shifting the loop-table
// pointers and counters by one register; the instructions themselves match.
void effNozleDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effNozleData* data = effect->userdata;
	CameraEntry* camera;
	effNozleEntryData* entry;
	u8* base;
	GXTexObj tex;
	Mtx transMtx;
	Mtx rotMtx;
	Mtx scaleMtx;
	Mtx mtx;
	GXColor color;
	s32 alpha;
	s32 i;
	u32 j;

	base = lbl_803AE260;
	camera = camGetPtr(camId);
	alpha = data->alpha;
	GXSetBlendMode(1, 4, 5, 0);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetZMode(1, 3, 0);

	PSMTXTrans(transMtx, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_80421230 * data->field_0x40);
	PSMTXConcat(transMtx, rotMtx, transMtx);
	PSMTXRotRad(rotMtx, 'x', float_deg2rad_80421230 * data->field_0x3C);
	PSMTXConcat(transMtx, rotMtx, transMtx);
	PSMTXScale(scaleMtx, data->scale * data->scaleX, data->scale * data->scaleY, data->scale);
	PSMTXConcat(transMtx, scaleMtx, transMtx);
	PSMTXConcat(camera->view, transMtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	GXSetTexCoordGen2(1, 1, 5, 0x21, 0, 0x7D);

	PSMTXTrans(transMtx, data->field_0x18, -data->field_0x1C, float_0_80421234);
	GXLoadTexMtxImm(transMtx, 0x1E, 1);
	PSMTXTrans(transMtx, data->field_0x20, -data->field_0x24, float_0_80421234);
	GXLoadTexMtxImm(transMtx, 0x21, 1);

	effGetTexObj(0x3C, &tex);
	GXLoadTexObj(&tex, 0);
	switch (data->type) {
	case 0:
	case 3:
		effGetTexObj(0x3D, &tex);
		GXLoadTexObj(&tex, 1);
		break;
	case 2:
		effGetTexObj(0x3E, &tex);
		GXLoadTexObj(&tex, 1);
		break;
	}

	GXSetNumTevStages(3);
	GXSetTevOrder(0, 0, 1, 0xFF);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
	GXSetTevAlphaIn(0, 7, 7, 7, 4);
	GXSetTevOrder(1, 1, 0, 0xFF);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xF, 0, 9, 0xF);
	GXSetTevAlphaIn(1, 7, 0, 4, 7);
	GXSetTevOrder(2, 0xFF, 0xFF, 4);
	GXSetTevColorOp(2, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 0xF, 0, 0xA, 0xF);
	GXSetTevAlphaIn(2, 7, 0, 5, 7);
	GXSetCullMode(0);

	switch (data->type) {
	case 0:
		GXClearVtxDesc();
		GXSetVtxDesc(9, 2);
		GXSetVtxAttrFmt(0, 9, 1, 3, 7);
		GXSetArray(9, DATA_AT(0x40, nozle_1_vertex_tbl), 6);
		GXSetVtxDesc(0xA, 2);
		GXSetVtxAttrFmt(0, 0xA, 0, 1, 6);
		GXSetArray(0xA, nozle_1_normal_tbl, 3);
		GXSetVtxDesc(0xB, 2);
		GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
		GXSetArray(0xB, nozle_1_color0_tbl, 4);
		GXSetVtxDesc(0xD, 2);
		GXSetVtxAttrFmt(0, 0xD, 1, 3, 0xE);
		GXSetArray(0xD, DATA_AT(0x200, nozle_1_texcoord0_tbl), 4);
		GXSetVtxDesc(0xE, 2);
		GXSetVtxAttrFmt(0, 0xE, 1, 3, 0xE);
		GXSetArray(0xE, DATA_AT(0x2E0, nozle_1_texcoord1_tbl), 4);
		for (j = 0; j < 10; j++) {
			GXCallDisplayList(nozle_1_dl_0_tbl[j], nozle_1_dl_0_size_tbl[j] << 5);
		}
		GXClearVtxDesc();
		GXSetVtxDesc(9, 2);
		GXSetVtxAttrFmt(0, 9, 1, 3, 7);
		GXSetArray(9, DATA_AT(0x40, nozle_1_vertex_tbl), 6);
		GXSetVtxDesc(0xA, 2);
		GXSetVtxAttrFmt(0, 0xA, 0, 1, 6);
		GXSetArray(0xA, nozle_1_normal_tbl, 3);
		GXSetVtxDesc(0xB, 2);
		GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
		GXSetArray(0xB, nozle_1_color0_tbl, 4);
		GXSetVtxDesc(0xD, 2);
		GXSetVtxAttrFmt(0, 0xD, 1, 3, 0xE);
		GXSetArray(0xD, DATA_AT(0x200, nozle_1_texcoord0_tbl), 4);
		GXSetVtxDesc(0xE, 2);
		GXSetVtxAttrFmt(0, 0xE, 1, 3, 0xE);
		GXSetArray(0xE, DATA_AT(0x2E0, nozle_1_texcoord1_tbl), 4);
		for (j = 0; j < 10; j++) {
			GXCallDisplayList(nozle_1_dl_1_tbl[j], nozle_1_dl_1_size_tbl[j] << 5);
		}
		break;
	case 2:
		GXClearVtxDesc();
		GXSetVtxDesc(9, 3);
		GXSetVtxAttrFmt(0, 9, 1, 3, 7);
		GXSetArray(9, DATA_AT(0x640, nozle_3_vertex_tbl), 6);
		GXSetVtxDesc(0xA, 3);
		GXSetVtxAttrFmt(0, 0xA, 0, 1, 6);
		GXSetArray(0xA, nozle_3_normal_tbl, 3);
		GXSetVtxDesc(0xB, 3);
		GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
		GXSetArray(0xB, DATA_AT(0xBC0, nozle_3_color0_tbl), 4);
		GXSetVtxDesc(0xD, 3);
		GXSetVtxAttrFmt(0, 0xD, 1, 3, 0xE);
		GXSetArray(0xD, DATA_AT(0x8C0, nozle_3_texcoord0_tbl), 4);
		GXSetVtxDesc(0xE, 3);
		GXSetVtxAttrFmt(0, 0xE, 1, 3, 0xE);
		GXSetArray(0xE, DATA_AT(0xA40, nozle_3_texcoord1_tbl), 4);
		for (j = 0; j < 0x11; j++) {
			GXCallDisplayList(((u8**)(DATA_AT(0x1820, nozle_3_dl_1_tbl)))[j], nozle_3_dl_1_size_tbl[j] << 5);
		}
		GXClearVtxDesc();
		GXSetVtxDesc(9, 3);
		GXSetVtxAttrFmt(0, 9, 1, 3, 7);
		GXSetArray(9, DATA_AT(0x640, nozle_3_vertex_tbl), 6);
		GXSetVtxDesc(0xA, 3);
		GXSetVtxAttrFmt(0, 0xA, 0, 1, 6);
		GXSetArray(0xA, nozle_3_normal_tbl, 3);
		GXSetVtxDesc(0xB, 3);
		GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
		GXSetArray(0xB, DATA_AT(0xBC0, nozle_3_color0_tbl), 4);
		GXSetVtxDesc(0xD, 3);
		GXSetVtxAttrFmt(0, 0xD, 1, 3, 0xE);
		GXSetArray(0xD, DATA_AT(0x8C0, nozle_3_texcoord0_tbl), 4);
		GXSetVtxDesc(0xE, 3);
		GXSetVtxAttrFmt(0, 0xE, 1, 3, 0xE);
		GXSetArray(0xE, DATA_AT(0xA40, nozle_3_texcoord1_tbl), 4);
		for (j = 0; j < 0x11; j++) {
			GXCallDisplayList(((u8**)(DATA_AT(0x1200, nozle_3_dl_0_tbl)))[j], nozle_3_dl_0_size_tbl[j] << 5);
		}
		break;
	case 3:
		GXSetTevAlphaIn(0, 7, 1, 4, 7);
		color = *(GXColor*)&unk_80423300;
		color.a = alpha;
		GXSetTevColor(1, color);
		entry = data->entries;
		for (i = 1; i < effect->count; i++) {
			PSMTXTrans(transMtx, entry->field_0x18, -entry->field_0x1C, float_0_80421234);
			GXLoadTexMtxImm(transMtx, 0x1E, 1);
			PSMTXTrans(transMtx, entry->field_0x20, -entry->field_0x24, float_0_80421234);
			GXLoadTexMtxImm(transMtx, 0x21, 1);
			PSMTXTrans(transMtx, entry->field_0x04, entry->field_0x08, entry->field_0x0C);
			PSMTXScale(scaleMtx, entry->field_0x10, entry->field_0x14, float_1_80421238);
			PSMTXConcat(transMtx, scaleMtx, scaleMtx);
			PSMTXConcat(mtx, scaleMtx, scaleMtx);
			GXLoadPosMtxImm(scaleMtx, 0);

			GXClearVtxDesc();
			GXSetVtxDesc(9, 2);
			GXSetVtxAttrFmt(0, 9, 1, 3, 6);
			GXSetArray(9, DATA_AT(0x1880, nozle_4_vertex_tbl), 6);
			GXSetVtxDesc(0xA, 2);
			GXSetVtxAttrFmt(0, 0xA, 0, 1, 6);
			GXSetArray(0xA, nozle_4_normal_tbl, 3);
			GXSetVtxDesc(0xB, 2);
			GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
			GXSetArray(0xB, nozle_4_color0_tbl, 4);
			GXSetVtxDesc(0xD, 2);
			GXSetVtxAttrFmt(0, 0xD, 1, 3, 0xD);
			GXSetArray(0xD, DATA_AT(0x1940, nozle_4_texcoord0_tbl), 4);
			GXSetVtxDesc(0xE, 2);
			GXSetVtxAttrFmt(0, 0xE, 1, 3, 0xD);
			GXSetArray(0xE, DATA_AT(0x19C0, nozle_4_texcoord1_tbl), 4);
			for (j = 0; j < 8; j++) {
				GXCallDisplayList(nozle_4_dl_0_tbl[j], nozle_4_dl_0_size_tbl[j] << 5);
			}
			entry++;
		}
		break;
	}
}
