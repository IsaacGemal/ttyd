#include "eff/n64/eff_recover_heart_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math_ext.h>

// .rodata
const Vec vec3_802fb4c8 = { 0.0f, 0.0f, 0.0f };
// The int-to-float magic double is MWCC's own pool entry; declaring it (rather than
// defining it) keeps the pool at the original's .rodata offset (0x10) so objdiff
// resolves the @NNN local to the original's symbol. effRecoverHeartN64Entry passes
// "RecoverHeartN64" as a literal for the same reason: a named char array would be
// emitted before the pool and push it to 0x20.
extern const f64 double_to_int_802fb4d8;

// .sdata
f32 scale_w_dat[7] = { 0.25f, 0.5f, 0.75f, 1.0f, 0.9f, 1.1f, 1.0f };
f32 scale_h_dat[7] = { 0.25f, 0.5f, 0.75f, 1.0f, 1.2f, 0.9f, 1.0f };

// .sdata2
extern const f32 float_0_8041fb10;
extern const f32 float_1_8041fb14;
extern const f32 float_0p05_8041fb1c;
extern const f32 float_6p2832_8041fb20;
extern const f32 float_360_8041fb24;
extern const f32 float_neg30_8041fb28;
extern const f32 float_8_8041fb2c;
extern const f32 float_0p9_8041fb30;
extern const f32 float_1p4_8041fb34;
extern const f32 float_deg2rad_8041fb08;
extern const f32 float_0p03125_8041fb0c;
extern const f32 float_0p0026042_8041fb18;

// uninitialized GXColor (alpha is filled in per particle)
extern u32 unk_80423238;

// .data base the retail effRecoverHeartDisp reaches its vertex arrays from; it is
// defined in eff_rappa_n64.c (the previous unit's last object) and only its address
// is used here.
#ifdef MUST_MATCH
extern u32 lbl_803A4A60[];
#define VTX(off, name) ((effVtxArray*)(dataBase + (off)))
#else
#define VTX(off, name) ((effVtxArray*)(name))
#endif

// .data
__declspec(section ".data") static f32 negone_one_374[2] = { -1.0f, 1.0f };
__declspec(section ".data") f32 lbl_803A4A70[2] = { 1.0f, -1.0f };
__declspec(section ".data") static f32 one_negone_373[2] = { 1.0f, -1.0f };
__declspec(section ".data") f32 lbl_803A4A80[2] = { -1.0f, 1.0f };
__declspec(section ".data") static f32 negone_one_360[2] = { -1.0f, 1.0f };
__declspec(section ".data") f32 lbl_803A4A90[2] = { 1.0f, -1.0f };
__declspec(section ".data") static f32 one_negone_359[2] = { 1.0f, -1.0f };
u32 size32x32_tex32x32_heart_vtx[14] = {
	0xFFF0FFF0, 0x00000000, 0x0000FFFF, 0xFFFF0010, 0xFFF00000, 0x04000000, 0xFFFFFFFF,
	0x00100010, 0x00000400, 0x0400FFFF, 0xFFFFFFF0, 0x00100000, 0x00000400, 0xFFFFFFFF,
};
u32 size16x16_tex32x32_heart_vtx[14] = {
	0xFFF8FFF8, 0x00000000, 0x0000FFFF, 0xFFFF0008, 0xFFF80000, 0x04000000, 0xFFFFFFFF,
	0x00080008, 0x00000400, 0x0400FFFF, 0xFFFFFFF8, 0x00080000, 0x00000400, 0xFFFFFFFF,
};
u32 size24x24_tex32x32_heart_vtx[14] = {
	0xFFF4FFF1, 0x00000000, 0x0000FAFA, 0xFAFF000C, 0xFFF10000, 0x04000000, 0xFAFAFAFF,
	0x000C0009, 0x00000400, 0x0400FAFA, 0xFAFFFFF4, 0x00090000, 0x00000400, 0xFAFAFAFF,
};
u32 size16x24_tex32x32_l_heart_vtx[14] = {
	0xFFF0FFF1, 0x00000000, 0x0000FAFA, 0xFAFF0004, 0xFFF10000, 0x04000000, 0xFAFAFAFF,
	0x00040009, 0x00000400, 0x0400FAFA, 0xFAFFFFF0, 0x00090000, 0x00000400, 0xFAFAFAFF,
};
u32 size16x24_tex32x32_r_heart_vtx[14] = {
	0xFFFCFFF1, 0x00000000, 0x0000FAFA, 0xFAFF0010, 0xFFF10000, 0x04000000, 0xFAFAFAFF,
	0x00100009, 0x00000400, 0x0400FAFA, 0xFAFFFFFC, 0x00090000, 0x00000400, 0xFAFAFAFF,
};
u32 size24x24_tex32x32_flower_vtx[14] = {
	0xFFF4FFF1, 0x00000000, 0x000024BE, 0x14FF000C, 0xFFF10000, 0x04000000, 0x24BE14FF,
	0x000C0009, 0x00000400, 0x040024BE, 0x14FFFFF4, 0x00090000, 0x00000400, 0x24BE14FF,
};
u32 size16x24_tex32x32_l_flower_vtx[14] = {
	0xFFF0FFF1, 0x00000000, 0x000024BE, 0x14FF0004, 0xFFF10000, 0x04000000, 0x24BE14FF,
	0x00040009, 0x00000400, 0x040024BE, 0x14FFFFF0, 0x00090000, 0x00000400, 0x24BE14FF,
};
u32 size16x24_tex32x32_r_flower_vtx[14] = {
	0xFFFCFFF1, 0x00000000, 0x000024BE, 0x14FF0010, 0xFFF10000, 0x04000000, 0x24BE14FF,
	0x00100009, 0x00000400, 0x040024BE, 0x14FFFFFC, 0x00090000, 0x00000400, 0x24BE14FF,
};

// Last object of this unit's .data; effReleaseDisp reaches its own .data through it.
__declspec(section ".data") f32 lbl_803A4C60[2] = { -1.0f, 1.0f };

typedef struct RecoverHeartData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 vx;         // 0x10
	f32 vy;         // 0x14
	f32 vz;         // 0x18
	f32 angle;      // 0x1C
	f32 field_0x20; // 0x20
	f32 scale;      // 0x24
	s32 field_0x28; // 0x28
	s32 field_0x2C; // 0x2C
	s32 field_0x30; // 0x30
	s32 field_0x34; // 0x34
	f32 scaleW;     // 0x38
	f32 scaleH;     // 0x3C
	s32 field_0x40; // 0x40
	s32 field_0x44; // 0x44
} RecoverHeartData; // 0x48

// NONMATCHING (85.66%): instructions differ; see check.py
EffectEntry* effRecoverHeartN64Entry(s32 type, s32 time, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	RecoverHeartData* data;

	effect = effEntry();
	effect->type = "RecoverHeartN64";
	effect->count = 2;
	data = __memAlloc(HEAP_EFFECT, 0x90);
	effect->userdata = data;
	effect->callback = effRecoverHeartMain;
	data[0].type = type;
	data[0].x = x;
	data[0].y = y;
	data[0].z = z;
	data[0].field_0x2C = 0x2E;
	data[0].field_0x30 = 0;
	if (time < 0) {
		data[0].field_0x44 = 1;
		time = -time;
	} else {
		data[0].field_0x44 = 0;
	}
	data[0].field_0x40 = time;
	{
		s32 i;

		for (i = 1; i < 2; i++) {
			data[1].x = float_0_8041fb10;
			data[1].y = float_0_8041fb10;
			data[1].z = float_0_8041fb10;
			data[1].scale = float_0_8041fb10;
			data[1].vx = float_0_8041fb10;
			data[1].vz = float_0_8041fb10;
			data[1].field_0x20 = float_0_8041fb10;
			data[1].angle = float_0_8041fb10;
			if (i == 1) {
				data[1].x = float_0_8041fb10;
				data[1].field_0x34 = 0;
			} else {
				data[1].x = (f32)(((i & 1) * 2 - 1) * (i / 2) * 4);
				data[1].field_0x34 = 11;
			}
			data[1].vy = float_1p4_8041fb34;
			data[1].scaleW = float_0_8041fb10;
			data[1].scaleH = float_0_8041fb10;
		}
	}
	data[1].field_0x28 = 0xFF;
	data[1].field_0x2C = 0;
	return effect;
}

// Local copy of system.c's inline helper so it inlines here (as in eff_guruguru.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = vec3_802fb4c8;
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effRecoverHeartMain(EffectEntry* effect) {
	s32 phi;
	s32 scaleBase;
	s32 life;
	RecoverHeartData* data;
	s32 i;
	s32 field2C;
	f32 fade;
	f32 angle;
	Vec position;

	data = effect->userdata;
	position = initializeVec((Vec*)&data->x);
	data->field_0x2C--;
	data->field_0x30++;
	life = data->field_0x2C;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	phi = life * 12 + 38;
	scaleBase = life * 25;
	fade = float_0p05_8041fb1c * (f32)life;
	data++;
	for (i = 1; i < effect->count; phi += 38, i++, data++) {
		field2C = data->field_0x2C;
		if (data->field_0x34 > 0) {
			data->field_0x34--;
			if (data->field_0x34 > 0) {
				continue;
			}
		}
		if (field2C >= 7) {
			data->scaleH = float_1_8041fb14;
			data->scaleW = float_1_8041fb14;
		} else {
			data->scaleW = scale_w_dat[field2C];
			data->scaleH = scale_h_dat[field2C];
		}
		data->x += data->vx;
		data->y += data->vy;
		data->z += data->vz;
		angle = float_6p2832_8041fb20 * (f32)phi / float_360_8041fb24;
		data->angle = float_neg30_8041fb28 * (f32)sin(angle);
		data->scale = float_8_8041fb2c * (f32)cos(angle);
		if (life < 20) {
			data->scale *= fade;
			data->angle *= fade;
		}
		if (life < 10) {
			data->field_0x28 = scaleBase;
			data->vy *= float_0p9_8041fb30;
		}
		data->field_0x2C++;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effRecoverHeartDisp, effect);
}

#ifdef MUST_MATCH
// The original builds the GXColor in the inline's own frame slot and passes the
// pointer to GXSetTevColor; doing the build and the call in one inline puts the
// local and the outgoing copy in the original's stack slots.
static inline void recoverHeartSetColor(RecoverHeartData* d) {
	GXColor color = *(GXColor*)&unk_80423238;
	color.a = d->field_0x28;
	GXSetTevColor(GX_TEVREG0, color);
}
#endif

// Keep this the last function in the file: MWCC reads global_optimizer when the unit is finalized, so a
// trailing `global_optimizer reset` drops the setting for this function too (95.5%).
#pragma global_optimizer off // needed so MWCC keeps the loop-invariant tile-offset arithmetic inside the loop and emits the two independent life/10 and life%10 divisions (without it: 96.9%)
// The vertex arrays are reached as offsets from the previous unit's last .data object
// (lbl_803A4A60); the int->float magic is the compiler's own pool entry. See the notes
// on these at the top of the file.
void effRecoverHeartDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	s32 unknown;
	s32 remainder;
	s32 quotient;
	RecoverHeartData* data;
	s32 i;
	s32 life;
	s32 type;
	s32 flag;
	CameraEntry* cam;
	s32 stripe;
	u32 texId;
	GXTexObj texObj;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx work;
	Mtx mtx;
	Mtx finalMtx;
#ifdef MUST_MATCH
	u8* dataBase = (u8*)lbl_803A4A60;
#endif

	cam = camGetPtr(camId);
	data = effect->userdata;
	flag = data->field_0x44;

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', float_deg2rad_8041fb08 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, work);

	type = data->type;
	life = data->field_0x40;
	data++;

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetCullMode(0);

	unknown = 0x160 - life * 32;
	remainder = life % 10;
	quotient = life / 10;

	for (i = 1; i < effect->count; i++, data++) {
		if (data->field_0x34 > 0) {
			continue;
		}

#ifdef MUST_MATCH
		recoverHeartSetColor(data);
#else
		{
			GXColor color = *(GXColor*)&unk_80423238;
			color.a = data->field_0x28;
			GXSetTevColor(GX_TEVREG0, color);
		}
#endif

		if (data->field_0x28 == 0xff) {
			GXSetNumTevStages(1);
			GXSetTevOrder(0, 0, 0, 4);
			GXSetTevColorOp(0, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
			GXSetTevColorIn(0, 0xf, 8, 0xa, 0xf);
			GXSetTevAlphaIn(0, 7, 7, 7, 4);
		} else {
			GXSetNumTevStages(1);
			GXSetTevOrder(0, 0, 0, 4);
			GXSetTevColorOp(0, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
			GXSetTevColorIn(0, 0xf, 8, 0xa, 0xf);
			GXSetTevAlphaIn(0, 7, 4, 1, 7);
		}

		if ((type == 0 || type == 2) != 0) {
			texId = 0x51;
			if (flag == 0) {
				texId = 0x50;
			}
		} else {
			texId = 0x52;
		}
		effGetTexObjN64(texId, &texObj);
		GXLoadTexObj(&texObj, 0);

		PSMTXScale(scale, float_0p03125_8041fb0c, float_0p03125_8041fb0c, float_0_8041fb10);
		GXLoadTexMtxImm(scale, 0x1e, 1);
		PSMTXTrans(trans, data->x + data->scale, data->y, data->z);
		PSMTXScale(scale, data->scaleW, data->scaleH, float_1_8041fb14);
		PSMTXConcat(trans, scale, trans);
		PSMTXConcat(work, trans, mtx);
		PSMTXRotRad(rot, 'z', float_deg2rad_8041fb08 * data->angle);
		PSMTXConcat(mtx, rot, finalMtx);

		if (i == 1 && type != 2) {
			effSetVtxDescN64(VTX(0x40, size32x32_tex32x32_heart_vtx));
			PSMTXConcat(cam->view, finalMtx, trans);
			GXLoadPosMtxImm(trans, 0);
			GXSetCurrentMtx(0);
			GXBegin(0x90, 0, 6);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);

			GXSetNumTevStages(1);
			GXSetTevOrder(0, 0, 0, 4);
			GXSetTevColorOp(0, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
			GXSetTevColorIn(0, 0xf, 8, 0xa, 0xf);
			GXSetTevAlphaIn(0, 7, 4, 1, 7);

			if (life < 10 && flag == 0) {
				effGetTexObjN64(0x53, &texObj);
				GXLoadTexObj(&texObj, 0);
				PSMTXScale(scale, float_0p03125_8041fb0c, float_0p0026042_8041fb18, float_0_8041fb10);
				PSMTXTrans(trans, float_0_8041fb10, (f32)unknown, float_0_8041fb10);
				PSMTXConcat(scale, trans, scale);
				GXLoadTexMtxImm(scale, 0x1e, 1);
				if (type == 0) {
					effSetVtxDescN64(VTX(0xb0, size24x24_tex32x32_heart_vtx));
				} else {
					effSetVtxDescN64(VTX(0x158, size24x24_tex32x32_flower_vtx));
				}
				PSMTXConcat(cam->view, mtx, trans);
				GXLoadPosMtxImm(trans, 0);
				GXSetCurrentMtx(0);
				GXBegin(0x90, 0, 6);
				tri2(0, 1, 2, 0, 0, 2, 3, 0);
			} else {
				stripe = flag != 0 ? 10 : quotient;
				effGetTexObjN64(0x53, &texObj);
				GXLoadTexObj(&texObj, 0);
				PSMTXScale(scale, float_0p03125_8041fb0c, float_0p0026042_8041fb18, float_0_8041fb10);
				PSMTXTrans(trans, float_0_8041fb10, (f32)(0x160 - remainder * 32), float_0_8041fb10);
				PSMTXConcat(scale, trans, scale);
				GXLoadTexMtxImm(scale, 0x1e, 1);
				if (type == 0) {
					effSetVtxDescN64(VTX(0x120, size16x24_tex32x32_r_heart_vtx));
				} else {
					effSetVtxDescN64(VTX(0x1c8, size16x24_tex32x32_r_flower_vtx));
				}
				PSMTXConcat(cam->view, mtx, trans);
				GXLoadPosMtxImm(trans, 0);
				GXSetCurrentMtx(0);
				GXBegin(0x90, 0, 6);
				tri2(0, 1, 2, 0, 0, 2, 3, 0);

				PSMTXScale(scale, float_0p03125_8041fb0c, float_0p0026042_8041fb18, float_0_8041fb10);
				PSMTXTrans(trans, float_0_8041fb10, (f32)(0x160 - stripe * 32), float_0_8041fb10);
				PSMTXConcat(scale, trans, scale);
				GXLoadTexMtxImm(scale, 0x1e, 1);
				if (type == 0) {
					effSetVtxDescN64(VTX(0xe8, size16x24_tex32x32_l_heart_vtx));
				} else {
					effSetVtxDescN64(VTX(0x190, size16x24_tex32x32_l_flower_vtx));
				}
				GXBegin(0x90, 0, 6);
				tri2(0, 1, 2, 0, 0, 2, 3, 0);
			}
		} else {
			PSMTXConcat(cam->view, finalMtx, trans);
			GXLoadPosMtxImm(trans, 0);
			GXSetCurrentMtx(0);
			effSetVtxDescN64(VTX(0x78, size16x16_tex32x32_heart_vtx));
			GXBegin(0x90, 0, 6);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);
		}
	}
}
