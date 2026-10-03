#include "eff/eff_splash.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_funemizu.h"
#include "eff/eff_kemutest.h"
#include "eff/n64/eff_water_damage_n64.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include "math_ext.h"
#include <dolphin/mtx.h>

inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

u32 geso_shibuki_vertex_tbl[] = {
	0x94000000, 0x0000AC01, 0x00000000, 0xC2010000, 0x0000D601, 0x00000000, 0xEA010000, 0x00000000,
	0x00000000, 0x90F6014D, 0x0000A4E4, 0x0CDB0000, 0xB8BD13A5, 0x0000CB06, 0x19400000, 0xDF2B1A62,
	0x0000F8FB, 0x12DD0000, 0x91430800, 0x0000A2A7, 0x12390000, 0xB5C21ACD, 0x0000C7AB, 0x214C0000,
	0xDB712353, 0x0000F0C7, 0x1A2E0000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

u32 geso_shibuki_texcoord0_tbl[] = {
	0xD5041884, 0xF3031884, 0xF303477B, 0xD504477B, 0x11001884, 0x1100477B, 0x2EFF1884, 0x2EFF477B,
	0x4CFD1884, 0x4CFD477B, 0x6AFC1884, 0x6AFC477B, 0xD5040200, 0xF3030200, 0x11000200, 0x2EFF0200,
	0x4CFD0200, 0x6AFC0000
};

u8 geso_scale_dt[] = {
	0x60, 0x60, 0x72, 0x88, 0x9F, 0xB6, 0xCC, 0xDF,
	0xEE, 0xF7, 0xF9, 0xF7, 0xF5, 0xF3, 0xF1, 0xEE,
	0xEB, 0xE8, 0xE4, 0xE1, 0xDD, 0xD9, 0xD5, 0xD0,
	0xCC, 0xC7, 0xC2, 0xBD, 0xB8, 0xB3, 0xAE, 0xA8,
	0xA3, 0x9D, 0x97, 0x91, 0x8C, 0x86, 0x80, 0x7A,
	0x74, 0x6D, 0x67, 0x61, 0x5B, 0x55, 0x4F, 0x49,
	0x43, 0x3D, 0x37, 0x31, 0x2B, 0x25, 0x20, 0x1A,
	0x15, 0x0F, 0x0A, 0x05, 0x00, 0x00, 0x00, 0x00
};

s8 geso_trans_dt[] = {
	-40, -12, -40, -12, -39, -9, -39, -6, -39, -3, -39, 0,
	-38, 3, -38, 5, -38, 8, -39, 10, -40, 12, -41, 13,
	-42, 13, -43, 13, -45, 13, -47, 13, -49, 13, -51, 12,
	-52, 12, -54, 12, -56, 12, -57, 12, -58, 12, -59, 12,
	-61, 12, -62, 12, -63, 12, -65, 12, -66, 12, -67, 12,
	-69, 12, -70, 12, -71, 12, -73, 12, -74, 12, -75, 12,
	-76, 12, -78, 12, -79, 12, -80, 12, -81, 12, -83, 12,
	-84, 12, -85, 12, -87, 12, -88, 12, -89, 12, -90, 12,
	-92, 12, -93, 12, -94, 12, -95, 12, -97, 12, -98, 12,
	-99, 12, -100, 12, -102, 12, -103, 12, -104, 12, -106, 12,
	-107, 12, -108, 12, -109, 12, -111, 12, -112, 12, -113, 12,
	-114, 12, -116, 12, -117, 12, -118, 12, -120, 12, 0, 0,
};

u32 geso_shibuki_normal_tbl[] = {
	0x00004000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

GXColor geso_shibuki_color0_tbl[] = {
	{ 0x00, 0x00, 0x00, 0xFF }, { 0xFF, 0xFF, 0xFF, 0xFF },
	{ 0x00, 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }
};

u32 geso_shibuki_dl_0_0[] = { 0x9800060C, 0x00010C0D, 0x00010D06, 0x00000007, 0x00010100, 0x00000301, 0x00000200, 0x00000000 };
u32 geso_shibuki_dl_0_1[] = { 0x9800060D, 0x00010D0E, 0x00010E07, 0x00010108, 0x00010401, 0x00000202, 0x00000500, 0x00000000 };
u32 geso_shibuki_dl_0_2[] = { 0x9800060E, 0x00010E0F, 0x00010F08, 0x00010409, 0x00010602, 0x00000503, 0x00000700, 0x00000000 };
u32 geso_shibuki_dl_0_3[] = { 0x9800060F, 0x00010F10, 0x00011009, 0x0001060A, 0x00010803, 0x00000704, 0x00000900, 0x00000000 };
u32 geso_shibuki_dl_0_4[] = { 0x98000610, 0x00011011, 0x0001110A, 0x0001080B, 0x00010A04, 0x00000905, 0x00000B00, 0x00000000 };

void* geso_shibuki_dl_0_tbl[] = {
	geso_shibuki_dl_0_0, geso_shibuki_dl_0_1, geso_shibuki_dl_0_2, geso_shibuki_dl_0_3, geso_shibuki_dl_0_4
};

u8 geso_shibuki_dl_0_size_tbl[] = { 1, 1, 1, 1, 1, 0, 0, 0 };

// The sin/cos go through these helpers in the original's inlining order (permuter): the radius multiply is
// inside the sin helper, outside the cos one.
static inline f32 splashScaledSin(f32 radius, f32 angle) {
    return radius * (f32)sin(angle);
}

static inline f32 splashCos(f32 angle) {
    return (f32)cos(angle);
}

EffectEntry* effSplashEntry(s32 type, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	SplashData* data;
	SplashData* ptr;
	s32 num;
	s32 radial;
	s32 i;
	f32 spawnAngle;
	f32 sn;
	f32 cs;
	f32 sx;
	f32 sz;
	f32 dx;
	f32 dz;

	entry = effEntry();
	num = 0;
	radial = 0;

	switch (type) {
	case 0:
	case 1:
		num = 2;
		radial = 10;
		break;
	case 2:
		num = 1;
		radial = 10;
		break;
	case 3:
		num = 6;
		radial = 0;
		scale *= 0.5f;
		effWaterDamageN64Entry(2, 0x3C, x, y, z, 1.0f);
		break;
	case 4:
		num = 2;
		radial = 10;
		break;
	}

	entry->type = "splash";
	entry->count = num + radial + 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(SplashData) * entry->count);
	entry->userdata = data;
	entry->callback = effSplashMain;
	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scaleZ = scale;
	data->scaleY = scale;
	data->scaleX = scale;
	data->counter = 0;
	data->field_0x38 = 0;

	ptr = data + 1;
	for (i = 1; i < num + 1; i++, ptr++) {
		ptr->position.z = 0.0f;
		ptr->position.y = 0.0f;
		ptr->position.x = 0.0f;
		ptr->offsetX = 0.0f;
		ptr->offsetY = 0.0f;
		ptr->field_0x28 = 0;
		ptr->scaleZ = 1.0f;
		ptr->scaleY = 1.0f;
		ptr->scaleX = 1.0f;

		switch (type) {
		case 0:
			if (i == 1) {
				ptr->velocity = 5.0f;
				ptr->decel = 0.4f;
				ptr->scaleZ = 0.7f;
				ptr->scaleY = 0.7f;
				ptr->scaleX = 0.7f;
			} else {
				ptr->velocity = 4.0f;
				ptr->decel = 0.4f;
			}
			break;
		case 1:
			if (i == 1) {
				ptr->velocity = 7.0f;
				ptr->decel = 0.4f;
			} else {
				ptr->velocity = 4.0f;
				ptr->decel = 0.4f;
			}
			break;
		case 2:
			ptr->velocity = 2.0f;
			ptr->decel = 0.1f;
			break;
		case 3:
			ptr->velocity = 0.0f;
			ptr->decel = 0.0f;
			switch (i - 1) {
			case 0:
			case 1:
				ptr->position.z = 0.0f;
				break;
			case 2:
			case 3:
				ptr->position.z = 10.0f;
				break;
			case 4:
			case 5:
				ptr->position.z = 20.0f;
				break;
			}
			break;
		case 4:
			if (i == 1) {
				ptr->velocity = 4.0f;
				ptr->decel = 0.48f;
			} else {
				ptr->velocity = 3.6f;
				ptr->decel = 0.8f;
			}
			ptr->scaleZ = 0.01f;
			break;
		}
	}

	for (i = num + 1; i < entry->count; i++, ptr++) {
		f32 angle = (f32)irand(360);
		f32 radius = (f32)irand(5);
		angle = 6.2831855f * angle / 360.0f;
		ptr->offsetX = splashScaledSin(radius, angle);
		ptr->offsetY = radius * splashCos(angle);
		ptr->position.x = 2.0f * ptr->offsetX;
		ptr->position.y = 0.0f;
		ptr->position.z = 2.0f * ptr->offsetY;
		ptr->velocity = 10.0f;
		ptr->decel = 1.0f;
		ptr->field_0x28 = irand(10);
		ptr->scaleZ = 0.5f;
		ptr->scaleY = 0.5f;
		ptr->scaleX = 0.5f;
	}

	if (type == 2) {
		spawnAngle = 6.2831855f * camGetPtr(CAMERA_3D)->viewYaw / 360.0f;
		sn = (f32)sin(spawnAngle);
		cs = splashCos(spawnAngle);
		sx = -30.0f * sn;
		sz = 30.0f * cs;
		dx = scale * sx;
		dz = scale * sz;
		effKemuTestEntry(6, x + dx, y + 5.0f * scale, z + dz, scale);
	}

	return entry;
}

// Instructions are the target's; the remaining objdiff marks are symbol-only and unit-wide.
// The zero-Vec literal below keeps the target's schedule (switching to the named vec3_802fe728
// makes MWCC hoist the first vector load two instructions early), but it resolves to
// .rodata+0x2C instead of vec3_802fe728 at +0x0; extra _half$/_three$ doubles emitted before it
// push vec3 down. double_to_int_mask/double_to_int are shifted the same way, and effSplashDisp's
// -8.0f*0.5f fold puts -4.0f where the target has 8.0f (shifting the 0.5f slot).
void effSplashMain(EffectEntry* effect) {
	SplashData* data = effect->userdata;
	Vec position;
	s32 type;

	position = initializeVec(&data->position);
	type = data->type;

	if (data->field_0x38 == 0) {
		dispEntry(CAMERA_3D, dispCalcZ(&position), 8, effSplashGetCol, effect);
		return;
	}

	{
	s32 i;
	u32 counter;

	switch (type) {
	case 0:
	case 1:
		i = 30;
		break;
	case 2:
	case 4:
		i = 60;
		break;
	case 3:
		i = 71;
		break;
	}

	counter = data->counter;
	data->counter = counter + 1;
	if (data->counter > i) {
		effDelete(effect);
		return;
	}

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->field_0x28 != 0) {
			data->field_0x28--;
			continue;
		}

		data->position.x += data->offsetX;
		data->position.z += data->offsetY;
		data->position.y += data->velocity;
		data->velocity -= data->decel;

		switch (type) {
		case 1:
			if (i == 1 && data->velocity < 0.0f) {
				data->scaleX *= 0.95f;
				data->scaleY *= 0.95f;
				data->scaleZ *= 0.95f;
			}
			break;
		case 3:
			if (i < 7) {
				if (counter < 0x3D) {
					data->scaleY = (f32)geso_scale_dt[counter] / 96.0f;
				}
				if (counter < 0x47) {
					data->position.x = (f32)(geso_trans_dt[counter * 2] + 40) / 8.0f * 10.0f;
					data->position.y = (f32)(geso_trans_dt[counter * 2 + 1] - 12) / 8.0f * 10.0f;
				}

				switch (i - 1) {
				case 0:
				case 1:
					data->position.x *= 0.3f;
					data->position.y *= 0.5f;
					data->scaleY *= 0.6f;
					break;
				case 2:
				case 3:
					data->position.x *= 0.9f;
					break;
				case 4:
				case 5:
					data->position.x *= 1.3f;
					data->position.y *= 0.3f;
					data->scaleY *= 0.6f;
					break;
				}

				switch (i - 1) {
				case 1:
				case 3:
				case 5:
					data->scaleX = -1.0f;
					data->position.x *= -1.0f;
					break;
				}
			}
			break;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 1, effSplashDisp, effect);
	}
}

void effSplashGetCol(enum CameraId camId, void* param) {
	EffectEntry* effect = param;
	SplashData* data = effect->userdata;
	CameraEntry* cam;
	f32 projection[7];
	f32 viewport[6];
	f32 screenX;
	f32 screenY;
	f32 screenZ;
	u32 color;
	u32 palette;

	cam = camGetPtr(camId);
	GXSetProjection(cam->projection, cam->projType);
	GXGetProjectionv(projection);
	GXGetViewportv(viewport);
	GXProject(data->position.x, data->position.y, data->position.z, cam->view, projection, viewport, &screenX, &screenY, &screenZ);
	if (screenX > 0.0f && screenX < 608.0f && screenY > 0.0f && screenY < 480.0f) {
		sysWaitDrawSync();
		GXPeekARGB((u16)screenX, (u16)screenY, &color);
		data->color.r = ((u8*)&color)[1];
		data->color.g = ((u8*)&color)[2];
		data->color.b = ((u8*)&color)[3];
		data->color.a = 0xFF;
	}
	data->field_0x38 = 1;
	palette = unk_80224298();
	data->color.r = ((u8*)&palette)[0];
	data->color.g = ((u8*)&palette)[1];
	data->color.b = ((u8*)&palette)[2];
}


// The display-list loop must live in an inlined helper: with it inline in effSplashDisp MWCC
// colours its counter r20 instead of the target's r25 (which the j loop above already used);
// the rest of the function is byte-identical (same fix as effKumokumoCallDL in eff_kumokumo_n64.c).
static inline void effSplashCallDL(void** dl, const u8* size) {
	u32 k;

	for (k = 0; k < 5; k++) {
		GXCallDisplayList(dl[k], size[k] << 5);
	}
}

// The angle is `6.2831855f * t / 8.0f`: MWCC strength-reduces the division by 8 to a multiply by
// 0.125f and keeps 6.2832f*t in the scratch f0, which is exactly the target's two fmuls.
void effSplashDisp(enum CameraId camId, void* param) {
	EffectEntry* effect = param;
	SplashData* data = effect->userdata;
	f32 colA;
	Mtx mtxA;
	f32 eight;
	f32 halfq;
	f32 radiusA;
	f32 radiusB;
	f32 colB;
	f32 fourq;
	GXTexObj tex;
	f32 offsetB;
	Mtx mtxB;
	f32 mulR;
	f32 mulG;
	f32 mulB;
	s32 i;
	s32 j;
	CameraEntry* cam;
	Mtx mtxC;
	s32 type;
	Mtx mtxD;
	s32 mode;
	GXColor color;
	f32 mulT;

	cam = camGetPtr(camId);
	type = data->type;
	color = data->color;

	mode = 0;
	switch (type) {
	case 0:
	case 1:
	case 4:
		mode = 2;
		break;
	case 2:
		mode = 1;
		break;
	case 3:
		mode = 6;
		break;
	}

	PSMTXTrans(mtxA, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(mtxC, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXScale(mtxB, data->scaleX, data->scaleY, data->scaleZ);
	PSMTXConcat(mtxA, mtxC, mtxD);
	PSMTXConcat(mtxD, mtxB, mtxD);
	PSMTXConcat(cam->view, mtxD, mtxD);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 30, 0, 125);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	data++;
	for (i = 1; i < mode + 1; i++, data++) {
		if (data->field_0x28 != 0) {
			continue;
		}

		PSMTXTrans(mtxA, data->position.x, data->position.y, data->position.z);
		PSMTXScale(mtxB, data->scaleX, data->scaleY, data->scaleZ);
		PSMTXConcat(mtxA, mtxB, mtxA);
		PSMTXConcat(mtxD, mtxA, mtxA);
		GXLoadPosMtxImm(mtxA, 0);
		GXSetCurrentMtx(0);

		switch (type) {
		case 0:
		case 1:
		case 2:
		case 4:
			effGetTexObj(0x41, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			break;
		case 3:
			effGetTexObj(0x43, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			break;
		}

		switch (type) {
		case 0:
		case 1:
		case 4:
			PSMTXTrans(mtxA, (f32)gp->retraceCount / 20.0f, 0.0f, 0.0f);
			break;
		case 2:
			PSMTXTrans(mtxA, (f32)gp->retraceCount / 40.0f, 0.0f, 0.0f);
			break;
		case 3:
			PSMTXTrans(mtxA, (f32)gp->retraceCount / 10.0f, 0.0f, 0.0f);
			break;
		}
		GXLoadTexMtxImm(mtxA, GX_TEXMTX0, GX_MTX2x4);

		switch (type) {
		case 0:
		case 2:
			radiusA = 23.0f;
			radiusB = 15.0f;
			offsetB = -37.0f;
			break;
		case 1:
			if (i == 1) {
				radiusA = 1.8f;
				radiusB = 6.0f;
				offsetB = -85.0f;
			} else {
				radiusB = 11.6f;
				offsetB = -21.0f;
				radiusA = radiusB;
			}
			break;
		case 4:
			if (i == 1) {
				radiusB = 10.0f;
				offsetB = -20.0f;
				radiusA = radiusB;
			} else {
				radiusA = 15.0f;
				radiusB = 12.0f;
				offsetB = -10.0f;
			}
			break;
		}

		switch (type) {
		case 0:
		case 1:
		case 4:
			mulR = 1.0f;
			mulG = 1.0f;
			mulB = 1.0f;
			colA = (f32)color.r / 255.0f;
			colB = (f32)color.g / 255.0f;
			mulT = (f32)color.b / 255.0f;
			break;
		case 2:
			mulR = 1.0f;
			mulG = 0.869f;
			mulB = 0.0f;
			mulT = 0.0f;
			colA = 1.0f;
			colB = 0.314f;
			break;
		}

		switch (type) {
		case 0:
		case 1:
		case 2:
		case 4:
			GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, 18);
			for (j = 0; j < 9; j++) {
				f32 angle, c, s, t;
				t = (f32)j;
#ifdef MUST_MATCH
				// MWCC strength-reduces / 8.0f to a * 0.125f and leaves 6.2832f*t in the scratch f0,
				// which is the target's two fmuls; the natural form is under #else.
				angle = 6.2831855f * t / 8.0f;
#else
				angle = 6.2831855f * t * 0.125f;
#endif
				c = (f32)cos(angle);
				s = (f32)sin(angle);
				GXPosition3f32(radiusA * s, 0.0f, radiusA * c);
				GXColor4u8((u8)(255.0f * mulR), (u8)(255.0f * mulG), (u8)(255.0f * mulB), 0xFF);
				GXTexCoord2f32((f32)(j & 1), 0.1f);
				c = (f32)cos(angle);
				s = (f32)sin(angle);
				GXPosition3f32(radiusB * s, offsetB, radiusB * c);
				GXColor4u8((u8)(255.0f * colA), (u8)(255.0f * colB), (u8)(255.0f * mulT), 0xFF);
				GXTexCoord2f32((f32)(j & 1), 2.1f);
			}
			break;
		case 3:
			geso_shibuki_color0_tbl[0] = color;
			GXClearVtxDesc();
			GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 8);
			GXSetArray(GX_VA_POS, geso_shibuki_vertex_tbl, 6);
			GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
			GXSetArray(GX_VA_NRM, geso_shibuki_normal_tbl, 3);
			GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
			GXSetArray(GX_VA_CLR0, geso_shibuki_color0_tbl, 4);
			GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xE);
			GXSetArray(GX_VA_TEX0, geso_shibuki_texcoord0_tbl, 4);
			effSplashCallDL(geso_shibuki_dl_0_tbl, geso_shibuki_dl_0_size_tbl);
			break;
		}
	}

	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	for (i = mode + 1; i < effect->count; i++, data++) {
		if (data->field_0x28 != 0) {
			continue;
		}

		PSMTXTrans(mtxA, data->position.x, data->position.y, data->position.z);
		PSMTXRotRad(mtxC, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
		PSMTXScale(mtxB, data->scaleX, data->scaleY, data->scaleZ);
		PSMTXConcat(mtxA, mtxC, mtxA);
		PSMTXConcat(mtxA, mtxB, mtxA);
		PSMTXConcat(mtxD, mtxA, mtxA);
		GXLoadPosMtxImm(mtxA, 0);
		GXSetCurrentMtx(0);
		effGetTexObj(0x40, &tex);
		GXLoadTexObj(&tex, GX_TEXMAP0);
		PSMTXIdentity(mtxA);
		GXLoadTexMtxImm(mtxA, GX_TEXMTX0, GX_MTX2x4);

		switch (type) {
		case 2:
			mulR = 1.0f;
			mulG = 0.869f;
			mulB = 0.0f;
			break;
		default:
			mulR = (1.0f + (f32)color.r / 255.0f) / 2.0f;
			mulG = (1.0f + (f32)color.g / 255.0f) / 2.0f;
			mulB = (1.0f + (f32)color.b / 255.0f) / 2.0f;
			break;
		}

		eight = 8.0f;
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		halfq = -eight / 2.0f;
		fourq = 4.0f;
		GXPosition3f32(halfq, fourq, 0.0f);
		GXColor4u8((u8)(255.0f * mulR), (u8)(255.0f * mulG), (u8)(255.0f * mulB), 0xFF);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(fourq, fourq, 0.0f);
		GXColor4u8((u8)(255.0f * mulR), (u8)(255.0f * mulG), (u8)(255.0f * mulB), 0xFF);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(fourq, halfq, 0.0f);
		GXColor4u8((u8)(255.0f * mulR), (u8)(255.0f * mulG), (u8)(255.0f * mulB), 0xFF);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(halfq, halfq, 0.0f);
		GXColor4u8((u8)(255.0f * mulR), (u8)(255.0f * mulG), (u8)(255.0f * mulB), 0xFF);
		GXTexCoord2f32(0.0f, 1.0f);
	}
}

// 4-byte zero word that ends the unit's .sdata: all-zero objects need explicit_zero_data to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_804161FC_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
