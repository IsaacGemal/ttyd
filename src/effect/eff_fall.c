#include "eff/eff_fall.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include "eff/n64/eff_kemuri1_n64.h"
#include "eff/n64/eff_water_damage_n64.h"
#include "manager/evtmgr.h"
#include "evt/evt_cam.h"
#include <dolphin/os/OSCache.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h" // external MSL math ABI; <math.h> pulls in the inline sqrtf/sqrt statics

#pragma pool_data on // the original addresses its .rodata through one base register

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_803ADD20[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };

// Float-pair anchors at .data+0x8..0x40. The original names the local ones `negone_one$374` etc.;
// no code references them, so `force_active` keeps them. The following shibuki tables are reached
// from effFallDispWater through lbl_803ADD20 as the common base register.
#pragma force_active on
__declspec(section ".data") static u32 negone_one_374[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_803ADD30[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_373[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") u32 lbl_803ADD40[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") static u32 negone_one_360[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_803ADD50[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_359[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") u32 shibuki_vertex_tbl[64] = {
	0xC94403B2, 0xF600E345, 0x03B2F600, 0xFD4703B2, 0xF6001747, 0x03B2F600,
	0x314903B2, 0xF600BC47, 0x1CF9F600, 0xE03E29DD, 0xF600FD47, 0x2FEFF600,
	0x1A4E29DD, 0xF6003E46, 0x1CF9F600, 0xA5C72D0F, 0xF600D2C7, 0x48A2F600,
	0xFD474FCF, 0xF6002A46, 0x48A2F600, 0x54C62D0F, 0xF600C647, 0x0F30F600,
	0xE2590F52, 0xF600FD47, 0x0F52F600, 0x18330F52, 0xF60033CE, 0x0F52F600,
	0xB4A4226F, 0xF600D9AE, 0x38DCF600, 0xFD473F71, 0xF600227F, 0x39A5F600,
	0x45E8226F, 0xF600CF07, 0x0000F240, 0x01060000, 0xF2403306, 0x0000F240,
	0xCF0706A6, 0xF2400106, 0x0FA6F240, 0x330606A6, 0xF240CF07, 0x134CF240,
	0x01061F4C, 0xF2403306, 0x134CF240, 0x2925174C, 0xF24028C7, 0x0AA6F240,
	0x28E40000, 0xF240D8CB, 0x174CF240, 0xD8DB0AA6, 0xF240D8EA, 0x0000F240,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".data") u32 shibuki_texcoord0_tbl[40] = {
	0x00002EF2, 0x100031CD, 0x10003999, 0x00003999, 0x200032DF, 0x20003999,
	0x300031CD, 0x30003999, 0x40002EF2, 0x40003999, 0x00000666, 0x10000666,
	0x10001385, 0x00001750, 0x20000666, 0x2000138B, 0x30000666, 0x300012DE,
	0x40000666, 0x40001750, 0x00002000, 0x10002000, 0x20002000, 0x30002000,
	0x40002000, 0xF36C2000, 0xC0002000, 0xC0004000, 0xF3584000, 0x32E22000,
	0x40004000, 0x33084000, 0xF3800000, 0xC0000000, 0x335B0000, 0x40000000,
	0x00000000, 0x00004000, 0x00000000, 0x00000000,
};
__declspec(section ".data") u32 shibuki_dl_0_0[16] = {
	0x98000A0A, 0x00000A0B, 0x00000B14, 0x00000D15, 0x00020C05, 0x00001406,
	0x0003150F, 0x00000010, 0x00010100, 0x00000301, 0x00000200, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".data") u32 shibuki_dl_0_1[16] = {
	0x98000A0B, 0x00000B0C, 0x00000E15, 0x00020C16, 0x00020F06, 0x00031507,
	0x00031610, 0x00010111, 0x00010401, 0x00000202, 0x00000500, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".data") u32 shibuki_dl_0_2[16] = {
	0x98000A0C, 0x00000E0D, 0x00001016, 0x00020F17, 0x00021107, 0x00031608,
	0x00031711, 0x00010412, 0x00010602, 0x00000503, 0x00000700, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".data") u32 shibuki_dl_0_3[16] = {
	0x98000A0D, 0x0000100E, 0x00001217, 0x00021118, 0x00001308, 0x00031709,
	0x00001812, 0x00010613, 0x00000803, 0x00000704, 0x00000900, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
#pragma force_active reset

// effFallMain constants and effect-local rodata (addresses from the target symbol table).
// .sdata tables and the shake event script. `shibuki_normal_tbl`/`shibuki_color0_tbl` are the
// GXSetArray sources; `evt_shake` is the local event script (the original names it local).
__declspec(section ".sdata") u32 shibuki_normal_tbl[8] = {
	0x00004000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000,
};
__declspec(section ".sdata") u32 shibuki_color0_tbl[8] = {
	0xFFFFFF00, 0xFFFFFF7F, 0xFFFFFF19, 0xFFFFFF4C, 0xB6CCCB33, 0xB6CCCB4C,
	0xB6CCCB99, 0xB6CCCB00,
};
__declspec(section ".sdata") u32 shibuki_dl_1_0[8] = {
	0x9800061F, 0x00072425, 0x0004201C, 0x00071426, 0x00041919, 0x00072527,
	0x00061C00, 0x00000000,
};
__declspec(section ".sdata") u32 shibuki_dl_1_1[8] = {
	0x98000620, 0x00042422, 0x0004221D, 0x00051423, 0x00041D1A, 0x00062524,
	0x00061F00, 0x00000000,
};
__declspec(section ".sdata") u32 shibuki_dl_1_2[8] = {
	0x98000622, 0x00042221, 0x00072323, 0x00041D1E, 0x00071824, 0x00061F1B,
	0x00071E00, 0x00000000,
};
__declspec(section ".sdata") u32 shibuki_dl_1_3[8] = {
	0x98000625, 0x00042020, 0x00042126, 0x0004191D, 0x00051A27, 0x00061C1A,
	0x00061B00, 0x00000000,
};
__declspec(section ".sdata") static EVT_BEGIN(evt_shake)
    CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.01f), 300)
    RETURN
EVT_END()


// Full element-0 init, particle loop, and the type 0-8 switch bodies are reconstructed. The ternary is the
// `irand(100) > 50 ? 0 : 180` form (writing `<= 50 ? 180 : 0` compiles to a different select idiom), the vec
// copy stores the zero vector before the x/y/z overwrite, and the count/type `switch` (with the redundant
// `case 0`) reproduces the original's two dead `b`s. The sound name is a local set before the loop; the
// `(s32)c` range is converted in the loop and hoisted by the compiler, which ranks it above `sfx` as the
// original's colouring needs.
EffectEntry* effFallEntry(s32 type, f32 x, f32 y, s32 num, f32 z, s32 timerMax, f32 radius, f32 height, f32 angle) {
	const char* sfx;
	EffectEntry* entry;
	FallWork* elem;
	FallWork* data;
	Vec pos;
	s32 count;
	s32 i;
	f32 rad;
	f32 dist;

	entry = effEntry();
	count = num + 1;
	switch (type) {
	case 0:
		count = num + 1;
		break;
	case 1: case 2: case 3: case 4:
	case 5: case 6: case 7: case 8:
		num = 1;
		count = 2;
		break;
	}
	entry->type = "Fall";
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, sizeof(FallWork) * entry->count);
	entry->userdata = data;
	entry->callback = effFallMain;
	sfx = "SFX_BTL_STAGE_FALL_WATER1";
	entry->flags |= 2;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->radius = radius;
	data->height = height;
	data->angle = angle;
	data->num = num;
	data->timer = 0;
	data->texFrame = 0;
	data->timerMax = timerMax;
	data->poseId = -1;
	data->poseId2 = -1;

	elem = &data[1];
	for (i = 1; i < entry->count; i++, elem++) {
		rad = (f32)irand(360) * 3.1415927f / 180.f;
		dist = (f32)irand((s32)radius);
		elem->x = dist * (f32)sin(rad);
		elem->y = 300.f + (f32)irand(entry->count);
		elem->z = dist * (f32)cos(rad);
		elem->timer = 0;
		elem->texFrame = 0;
		elem->alpha = 0xFF;
		elem->spin = 0.0f;
		elem->angle = (f32)((irand(100) > 50) ? 0 : 180);
		elem->state = 0;
		elem->poseId = -1;
		elem->poseId2 = -1;
		switch (type) {
		case 0:
		case 1:
			elem->velY = 0.0f;
			elem->gravity = 0.2f;
			if (i == 1) {
				elem->x = 0.0f;
				elem->z = 0.0f;
				elem->timer = 0;
			} else {
				elem->timer = irand(entry->count) + 0x3c;
			}
			elem->scale = (type == 0) ? 0.5f : 1.2f;
			break;
		case 2:
			elem->velY = 0.0f;
			elem->gravity = 1.6f;
			elem->x = 0.0f;
			elem->z = 0.0f;
			elem->timer = 0;
			elem->wave = 0.0f;
			elem->scale = 0.5f;
			break;
		case 3:
			elem->velY = 0.0f;
			elem->gravity = 0.1f;
			elem->x = 0.0f;
			elem->y = 200.f;
			elem->z = 0.0f;
			elem->unk50 = 0.0f;
			elem->unk54 = 0.0f;
			elem->unk58 = 0.0f;
			elem->scale = 1.0f;
			pos = (Vec){0.0f, 0.0f, 0.0f};
			pos.x = x;
			pos.y = y;
			pos.z = z;
#ifdef MUST_MATCH
			// MUST_MATCH: the s32 round trip pins the address materialisation into sfx's own callee-saved
			// register (`addi r29, r5, ...`); a plain use leaves an `addi r6` / `mr r29, r6` copy (eff_nice's
			// acrobatMain has the same idiom).
			elem->sfxId = psndSFXOn_3D((const char*)(s32)sfx, &pos);
#else
			elem->sfxId = psndSFXOn_3D(sfx, &pos);
#endif
			break;
		case 4:
		case 5:
			elem->velY = 0.0f;
			elem->gravity = 0.6f;
			elem->x = 0.0f;
			elem->z = 0.0f;
			elem->timer = 0;
			elem->scale = 1.0f;
			break;
		case 6:
			elem->x = 0.0f;
			elem->y = 200.f;
			elem->z = 0.0f;
			elem->unk50 = -elem->x / 30.f;
			elem->unk54 = -elem->y / 30.f;
			elem->unk58 = -elem->z / 30.f;
			elem->scale = 1.3f;
			elem->spin = 30.f;
			elem->roll = 0.0f;
			break;
		case 7:
			elem->x = 400.f;
			elem->y = 200.f;
			elem->z = 0.0f;
			elem->scale = 0.5f;
			break;
		case 8:
			elem->velY = 0.0f;
			elem->gravity = 0.25f;
			elem->x = 0.0f;
			elem->y = 400.f;
			elem->z = 0.0f;
			elem->scale = 1.0f;
			break;
		}
	}
	return entry;
}

// Full reconstruction of the element-0 init, the per-element state machines for types 0-8, the type 2
// procedural vertex build, the type 7 sin/cos approximations, and the display registration.

// Builds the effect's position vector through a temporary so the zero-vector copy and the x/y/z overwrite
// schedule (and spill) as the original does.
static inline Vec effFallMakePos(f32 x, f32 y, f32 z) {
	Vec temp = {0.0f, 0.0f, 0.0f};
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

// Polynomial sine/cosine approximations in the shape eff_damage_star and evt_sub use; the type-7 meteor steers
// with them. Coefficients are assigned (not initialized) so they pool before the range constants.
static inline f32 effFallSinf(f32 t) {
	f32 v;
	f32 b;
	f32 one;
	f32 t2;

	v = 0.00761f;
	b = 0.16605f;
	one = 1.0f;

	if (t <= 3.1415927f) {
		if (t <= 1.5707964f) {
			t2 = t * t;
			v = v * t2 - b;
			v = v * t2 + one;
			v = v * t;
		} else {
			t = 1.5707964f - (t - 1.5707964f);
			t2 = t * t;
			v = v * t2 - b;
			v = v * t2 + one;
			v = v * t;
		}
	} else if (t < 4.712389f) {
		t = t - 3.1415927f;
		t2 = t * t;
		v = v * t2 - b;
		v = v * t2 + one;
		v = v * t;
		v = -v;
	} else {
		t = 1.5707964f - (t - 4.712389f);
		t2 = t * t;
		v = v * t2 - b;
		v = v * t2 + one;
		v = v * t;
		v = -v;
	}
	return v;
}

static inline f32 effFallCosf(f32 t) {
	f32 v;
	f32 b;
	f32 one;
	f32 t2;

	v = 0.03705f;
	b = 0.4967f;
	one = 1.0f;

	if (t <= 3.1415927f) {
		if (t <= 1.5707964f) {
			t2 = t * t;
			v = v * t2 - b;
			v = v * t2 + one;
		} else {
			t = 1.5707964f - (t - 1.5707964f);
			t2 = t * t;
			v = v * t2 - b;
			v = v * t2 + one;
			v = -v;
		}
	} else if (t < 4.712389f) {
		t = t - 3.1415927f;
		t2 = t * t;
		v = v * t2 - b;
		v = v * t2 + one;
		v = -v;
	} else {
		t = 1.5707964f - (t - 4.712389f);
		t2 = t * t;
		v = v * t2 - b;
		v = v * t2 + one;
	}
	return v;
}

#pragma opt_lifetimes off // one register per variable: the sin/cos accumulators stay in their coefficient's register
#pragma opt_propagation off // keeps the sin/cos coefficients as variables loaded before the loop
// The sound names are computed once in the loop preheader, BUG1 and FORK1 spilled to the stack: MWCC's frontend
// hoists an integer round trip of a literal out of the loop as an invariant expression (as in battle_stage_object's
// BSO_SFX), while a plain literal is rematerialised at each call. The release loop has its own `rel`/`j`. The
// type-2 vertex loops step the vertex pointer (`for (...; i++, v += 5)`), which MWCC unrolls into the original's
// mtctr/bdnz loops.
#ifdef MUST_MATCH
#define FALL_SFX(name) ((char*)(u32)(name))
#else
#define FALL_SFX(name) (name)
#endif
void effFallMain(EffectEntry* effect) {
	FallWork* elem = effect->userdata;
	s32 i;
	s32 type;
	s32 timerMax;
	s32 flag = 0;
	s32 done;
	s32 inBattle;
	Vec pos;
	FallWork* rel;
	s32 j;
	f32 y;
	f32 f30;
	f32 landY;
	f32 radius;
	f32 floor;
	f32 rad;
	f32 cs;
	f32 angle;

	pos = effFallMakePos(elem->x, elem->y, elem->z);

	type = elem->type;
	radius = elem->radius;
	y = elem->y;
	floor = y + elem->height;
	angle = elem->angle;
	inBattle = gp->inBattle != 0;
	timerMax = elem->timerMax;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		for (j = 0, rel = effect->userdata; j < effect->count; j++, rel++) {
			if (rel->poseId != -1) {
				animPoseRelease(rel->poseId);
			}
			if (rel->poseId2 != -1) {
				animPoseRelease(rel->poseId2);
			}
			psndSFXOff(rel->sfxId);
		}
		effDelete(effect);
		return;
	}

	f30 = 6.2831855f * angle;
	landY = 30.f + y;

	done = 0;
	for (i = 1, elem++; i < effect->count; i++, elem++) {
		switch (type) {
		case 0:
		case 1:
			switch (elem->state) {
			case 0:
				if (elem->timer != 0) {
					elem->timer--;
				}
				if (elem->timer == 0) {
					elem->state++;
					if (type == 1) {
						psndSFXOn_3D(FALL_SFX("SFX_BTL_STAGE_FALL_BUG1"), &pos);
					} else if (i == 1) {
						psndSFXOn_3D(FALL_SFX("SFX_BTL_STAGE_FALL_BUG2"), &pos);
					}
				}
				break;
			case 1:
				if (type == 0 && i > 1 && flag == 0 && gp->retraceCount % 6 == 0) {
					psndSFXOn_3D(FALL_SFX("SFX_BTL_STAGE_FALL_BUG2"), &pos);
					flag = 1;
				}
				elem->y += elem->velY;
				elem->velY -= elem->gravity;
				if (distABf(elem->x, elem->z, 0.0f, 0.0f) < radius / 2.0f) {
					if (elem->y < floor) {
						elem->y = floor;
						elem->velY *= -0.3f;
						elem->state++;
					}
				} else {
					if (elem->y < y) {
						elem->y = y;
						elem->velY *= -0.3f;
						elem->state++;
					}
				}
				break;
			case 2:
				elem->y += elem->velY;
				elem->velY -= elem->gravity;
				rad = 6.2831855f * angleABf(0.0f, 0.0f, elem->x, elem->z) / 360.f;
				if (i == 1) {
					rad = f30 / 360.f;
				}
				elem->x += 0.5f * (f32)sin(rad);
				cs = cos(rad);
				elem->z += 0.5f * -cs;
				if (elem->y < y) {
					elem->y = y;
					elem->timer = 0;
					elem->state++;
				}
				break;
			case 3:
				elem->timer++;
				if (elem->timer > 30) {
					elem->state++;
				}
				break;
			case 4:
				elem->alpha -= 10;
				if (elem->alpha < 0) {
					elem->alpha = 0;
					elem->state++;
				}
				break;
			case 5:
				done++;
				break;
			}
			if ((gp->retraceCount >> 2) & 1) {
				elem->texFrame++;
			}
			if (elem->texFrame >= 3) {
				elem->texFrame = 0;
			}
			elem->spin = 35.0f * (f32)sin((f32)gp->retraceCount / 2.0f + (f32)i);
			break;
		case 2:
			switch (elem->state) {
			case 0:
				elem->y += elem->velY;
				elem->velY -= elem->gravity;
				if (elem->y < y) {
					elem->y = y;
					elem->timer = 0;
					elem->state++;
					psndSFXOn_3D(FALL_SFX("SFX_BTL_STAGE_DAMAGE_FORK1"), &pos);
				}
				break;
			case 1: {
				f32 wave = (f32)sin(62.831856f * (f32)elem->timer / 60.f);
				elem->wave = (60.f - (f32)elem->timer) * wave / 60.f;
				elem->timer++;
				if (elem->timer > 60) {
					elem->state++;
				}
				break;
			}
			case 2:
				elem->timer++;
				if (elem->timer > 30) {
					elem->state++;
				}
				break;
			case 3:
				elem->alpha -= 10;
				if (elem->alpha < 0) {
					elem->alpha = 0;
					elem->state++;
				}
				break;
			case 4:
				done++;
				break;
			}
			{
				s32 j;
				f32* v;
				f32* w;
				elem->vertexBuf = smartAlloc(0x280, 3);
				v = elem->vertexBuf->address;
				for (j = 0; j < 0x20; j++, v += 5) {
					v[0] = (j & 1) ? 16.0f : -16.0f;
					v[1] = 128.0f - 8.533334f * (f32)(j / 2);
					v[2] = 0.0f;
					v[3] = (j & 1) ? 2.0f : 0.0f;
					v[4] = 0.06666667f * (f32)(j / 2);
				}
				w = elem->vertexBuf->address;
				for (i = 0; i < 0x20; i++, w += 5) {
					w[0] += elem->wave * (w[1] * w[1] / 600.f);
				}
				DCFlushRange(elem->vertexBuf->address, 0x280);
			}
			break;
		case 3:
			switch (elem->state) {
			case 0:
				elem->y += elem->velY;
				elem->velY -= elem->gravity;
				if (elem->y < y) {
					elem->y = y;
					elem->timer = 0;
					elem->state++;
				}
				break;
			case 1:
				if (elem->timer % 10 == 0) {
					effWaterDamageN64Entry(4, 20, pos.x, pos.y, pos.z, 1.0f);
				}
				elem->timer++;
				if (elem->timer > timerMax) {
					elem->state++;
				}
				break;
			case 2:
				elem->alpha *= 0.9f;
				elem->scale *= 0.9f;
				if (elem->scale < 0.01f) {
					psndSFXOff(elem->sfxId);
					effDelete(effect);
					return;
				}
				break;
			}
			elem->unk54 -= 0.2f; // pan value for the splash texture in effFallDispWater
			break;
		case 4:
		case 5:
			switch (elem->state) {
			case 0:
				if (type == 5) {
					if (!animGroupBaseAsync("EFF_Baketu", inBattle, NULL)) {
						break;
					}
					if (elem->poseId == -1) {
						elem->poseId = animPoseEntry("EFF_Baketu", inBattle);
						animPoseSetAnim(elem->poseId, "Z_1", 1);
					}
				} else {
					if (!animGroupBaseAsync("EFF_Tarai", inBattle, NULL)) {
						break;
					}
					if (elem->poseId == -1) {
						elem->poseId = animPoseEntry("EFF_Tarai", inBattle);
						animPoseSetAnim(elem->poseId, "Z_1", 1);
					}
				}
				if (elem->timer != 0) {
					elem->timer--;
				}
				if (elem->timer == 0) {
					elem->state++;
				}
				break;
			case 1:
				elem->y += elem->velY;
				elem->velY -= elem->gravity;
				if (distABf(elem->x, elem->z, 0.0f, 0.0f) < radius / 2.0f) {
					if (elem->y < floor) {
						elem->y = floor;
						elem->velY *= -0.3f;
						elem->state++;
					}
				} else {
					if (elem->y < y) {
						elem->y = y;
						elem->velY *= -0.3f;
						elem->state++;
					}
				}
				break;
			case 2:
				elem->y += elem->velY;
				elem->velY -= elem->gravity;
				rad = 6.2831855f * angleABf(0.0f, 0.0f, elem->x, elem->z) / 360.f;
				if (i == 1) {
					rad = f30 / 360.f;
				}
				elem->x += 2.0f * (f32)sin(rad);
				cs = cos(rad);
				elem->z += 2.0f * -cs;
				if (elem->y < y) {
					elem->y = y;
					elem->velY *= -0.5f;
					elem->y = y;
					elem->timer = 0;
					if (elem->velY < 1.0f) {
						elem->state++;
					}
				}
				break;
			case 3:
				elem->timer++;
				if (elem->timer > 30) {
					elem->state++;
				}
				break;
			case 4:
				elem->alpha -= 10;
				if (elem->alpha < 0) {
					elem->alpha = 0;
					elem->state++;
					animPoseRelease(elem->poseId);
					elem->poseId = -1;
				}
				break;
			case 5:
				done++;
				break;
			}
			switch (type) {
			case 4:
				if (elem->state > 1) {
					elem->spin += (360.f - elem->spin) / 10.f;
				}
				break;
			case 5:
				if (elem->state > 1) {
					elem->spin += (430.f - elem->spin) / 10.f;
				}
				break;
			}
			if (elem->poseId != -1) {
				animPoseMain(elem->poseId);
			}
			break;
		case 6:
			switch (elem->state) {
			case 0:
				if (!animGroupBaseAsync("EFF_koopa", inBattle, NULL)) {
					break;
				}
				if (elem->poseId == -1) {
					elem->poseId = animPoseEntry("EFF_koopa", inBattle);
					animPoseSetAnim(elem->poseId, "Z_1", 1);
					psndSFXOn_3D(FALL_SFX("SFX_BTL_STAGE_FALL_KOOPA1"), &pos);
				}
				elem->x += elem->unk50;
				elem->y += elem->unk54;
				elem->z += elem->unk58;
				elem->timer++;
				if (elem->timer > 30) {
					elem->timer = 0;
					elem->state++;
					effKemuri1N64Entry(4, pos.x + elem->x, pos.y + elem->y, pos.z + elem->z, 0.0f);
					evtEntry(evt_shake, 0, 0);
				}
				break;
			case 1:
				elem->spin = intplGetValue(0xb, elem->timer, 0xb4, 30.f, -27.0f);
				if (elem->spin < 0.0f) {
					elem->roll = elem->spin;
					elem->spin = 0.0f;
				}
				elem->timer++;
				if (elem->timer > 0xb4) {
					elem->timer = 0;
					elem->state++;
				}
				if (elem->timer == 0x64) {
					psndSFXOn_3D(FALL_SFX("SFX_BTL_STAGE_FALL_KOOPA1"), &pos);
				}
				break;
			case 2:
				elem->spin = intplGetValue(0xb, elem->timer, 0xc8, -27.0f, 80.f);
				if (elem->spin < 0.0f) {
					elem->roll = elem->spin;
					elem->spin = 0.0f;
				}
				elem->timer++;
				if (elem->timer > 0xc8) {
					animPoseRelease(elem->poseId);
					elem->poseId = -1;
					done++;
				}
				if (elem->timer == 0x46) {
					psndSFXOn_3D(FALL_SFX("SFX_BTL_STAGE_FALL_KOOPA1"), &pos);
				}
				break;
			}
			if (elem->poseId != -1) {
				animPoseMain(elem->poseId);
			}
			break;
		case 7:
			switch (elem->state) {
			case 0:
				if (!animGroupBaseAsync("EFF_inseki", inBattle, NULL)) {
					break;
				}
				psndSFXOn_3D(FALL_SFX("SFX_BTL_STAGE_FALL_METEOR1"), &pos);
				if (elem->poseId == -1) {
					elem->poseId = animPoseEntry("EFF_inseki", inBattle);
					animPoseSetAnim(elem->poseId, "Z_1", 1);
				}
				if (elem->poseId2 == -1) {
					elem->poseId2 = animPoseEntry("EFF_inseki", inBattle);
					animPoseSetAnim(elem->poseId2, "Z_2", 1);
				}
				/* fallthrough */
			case 2:
			case 4:
			case 6: {
				Vec tbl[4] = {
					{ 140.0f, 40.0f, -10.0f },
					{ 0.0f, 40.0f, -40.0f },
					{ -140.0f, 40.0f, -10.0f },
					{ -400.0f, 40.0f, -40.0f },
				};
				elem->unk50 = tbl[elem->state / 2].x;
				elem->unk54 = tbl[elem->state / 2].y;
				elem->unk58 = tbl[elem->state / 2].z;
				elem->gravity = 0.4f;
				elem->timer = (s32)(distABf(elem->unk50, elem->unk58, elem->x, elem->z) / 5.0f);
				elem->velY = (elem->unk54 - elem->y) / (f32)elem->timer +
				              elem->gravity * (f32)elem->timer / 2.0f;
				elem->angle = angleABf(elem->x, elem->z, elem->unk50, elem->unk58);
				elem->spin = 0.0f;
				elem->state++;
				break;
			}
			case 1:
			case 3:
			case 5:
			case 7: {
				elem->y += elem->velY;
				elem->velY -= elem->gravity;
				elem->x += 5.0f * effFallSinf(6.2831855f * elem->angle / 360.f);
				elem->z += 5.0f * -effFallCosf(elem->angle * 6.2831855f / 360.f);
				if (--elem->timer < 0) {
					elem->x = elem->unk50;
					elem->y = elem->unk54;
					elem->z = elem->unk58;
					elem->state++;
					if (elem->state == 2) {
						elem->sfxId = psndSFXOn_3D(FALL_SFX("SFX_BTL_STAGE_DAMAGE_METEOR2"), &pos);
					}
					if (elem->state != 8) {
						effKemuri1N64Entry(4, pos.x + elem->x, pos.y + elem->y - 40.f,
						                   pos.z + elem->z, 0.0f);
						evtEntry(evt_shake, 0, 0);
					}
				}
				elem->spin += (360.f - elem->spin) / 30.f;
				break;
			}
			case 8:
				psndSFXOff(elem->sfxId);
				animPoseRelease(elem->poseId);
				animPoseRelease(elem->poseId2);
				elem->poseId = -1;
				elem->poseId2 = -1;
				elem->state++;
				break;
			case 9:
				done++;
				break;
			}
			if (elem->poseId != -1) {
				animPoseMain(elem->poseId);
			}
			if (elem->poseId2 != -1) {
				animPoseMain(elem->poseId2);
			}
			break;
		case 8:
			switch (elem->state) {
			case 0:
				if (!animGroupBaseAsync("EFF_light", inBattle, NULL)) {
					break;
				}
				elem->poseId = animPoseEntry("EFF_light", inBattle);
				animPoseSetAnim(elem->poseId, "Z_1", 1);
				elem->state++;
				break;
			case 1:
			case 2:
				elem->y += elem->velY;
				elem->velY -= elem->gravity;
				if (elem->y < y) {
					elem->y = y;
					elem->timer = 0;
					elem->state++;
				}
				if (elem->state == 1 && elem->y < landY) {
					effKemuri1N64Entry(4, pos.x + elem->x, pos.y + y, pos.z + elem->z, 0.0f);
					evtEntry(evt_shake, 0, 0);
					animPoseSetAnim(elem->poseId, "A_1", 1);
					elem->state++;
					psndSFXOn_3D(FALL_SFX("SFX_BTL_STAGE_DAMAGE_LIGHT1"), &pos);
				}
				break;
			case 3:
				if (animPoseGetLoopTimes(elem->poseId) < 1.0f) {
					break;
				}
				elem->timer++;
				if (elem->timer > elem->timerMax) {
					elem->state++;
				}
				break;
			case 4:
				elem->alpha -= 10;
				if (elem->alpha < 0) {
					elem->alpha = 0;
					elem->state++;
				}
				break;
			case 5:
				animPoseRelease(elem->poseId);
				elem->poseId = -1;
				elem->state++;
				break;
			case 6:
				done++;
				break;
			}
			if (elem->poseId != -1) {
				animPoseMain(elem->poseId);
			}
			break;
		}
	}

	if (done >= effect->count - 1) {
		effDelete(effect);
		return;
	}

	switch (type) {
	case 3:
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effFallDispWater, effect);
		break;
	case 4:
	case 5:
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 0, effFallDisp, effect);
		break;
	case 7:
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effFallDispInseki, effect);
		break;
	default:
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effFallDisp, effect);
		break;
	}
}
#pragma opt_propagation reset
#pragma opt_lifetimes reset

// GX display for the default/koopa/light types: a per-type textured quad, the procedural type-2 strip,
// the type 4/5 animation halo, the type 6 koopa body and the type 8 light pose.
// The viewYaw product, the case-0/1 half-angle products and the two GXChanMat colours go through
// explicit locals: `rad`, `w` and `neg29`/`pos29` put the compiler's temporaries in the
// target's FPRs, and the colour helpers put them in its GPRs/stack slots.
void effFallDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	FallWork* elem = effect->userdata;
	CameraEntry* cam;
	CameraEntry* camView;
	GXTexObj texObj;
	Mtx mtxPos, mtxScale, mtxRot, mtxView;
	s32 i;
	s32 type;
	f32 f31;
	f32 f30;
	f32 f29;
	f32 f28;
	f32 rad;

	camView = camGetPtr(cameraId);
	type = elem->type;
	f30 = elem->angle;
	PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
	cam = camGetPtr(cameraId);
	rad = -cam->viewYaw;
	rad = 0.017453292f * rad;
	PSMTXRotRad(mtxRot, 'y', rad);
	PSMTXConcat(mtxPos, mtxRot, mtxView);

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevOp(0, 0);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xd, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);

	switch (type) {
	case 0:
	case 1:
		effGetTexObj(0x32, &texObj);
		GXLoadTexObj(&texObj, 0);
		break;
	case 2:
		effGetTexObj(0x33, &texObj);
		GXLoadTexObj(&texObj, 0);
		break;
	}

	f31 = 6.2831855f * f30;
	for (i = 1, elem = &elem[1]; i < effect->count; i++, elem++) {
		switch (type) {
		case 0:
		case 1: {
			f32 half = 0.5f;
			f32 ofs;
			f29 = 64.0f;
			f28 = 42.666668f;
			f29 *= 0.5f;
			f28 *= 0.5f;
			PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
			PSMTXRotRad(mtxRot, 'y', 0.017453292f * elem->angle);
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
			ofs = f28 * half;
			PSMTXTrans(mtxRot, 0.0f, ofs * elem->scale, 0.0f);
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
			PSMTXRotRad(mtxRot, 'z', 0.017453292f * elem->spin);
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
			ofs = -f28 * half;
			PSMTXTrans(mtxRot, 0.0f, ofs * elem->scale, 0.0f);
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
			PSMTXScale(mtxScale, elem->scale, elem->scale, elem->scale);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXConcat(mtxView, mtxPos, mtxPos);
			PSMTXConcat(camView->view, mtxPos, mtxPos);
			GXLoadPosMtxImm(mtxPos, 0);
			GXSetCurrentMtx(0);
			GXSetChanMatColor(4, (GXColor){ 0xFF, 0xFF, 0xFF, elem->alpha });
			PSMTXScale(mtxScale, 1.0f, 0.33333334f, 1.0f);
			PSMTXTrans(mtxPos, 0.0f, (f32)elem->texFrame, 0.0f);
			PSMTXConcat(mtxScale, mtxPos, mtxPos);
			GXLoadTexMtxImm(mtxPos, 0x1e, 1);
			GXBegin(0x80, 0, 4);
			GXPosition3f32(-f29 * half, f28, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(f29 * half, f28, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(f29 * half, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(-f29 * half, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
			break;
		}
		case 2: {
			s32 j;
			f32* v;
			PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
			PSMTXScale(mtxScale, elem->scale, elem->scale, elem->scale);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXConcat(mtxView, mtxPos, mtxPos);
			PSMTXConcat(camView->view, mtxPos, mtxPos);
			GXLoadPosMtxImm(mtxPos, 0);
			GXSetCurrentMtx(0);
			GXSetChanMatColor(4, (GXColor){ 0xFF, 0xFF, 0xFF, elem->alpha });
			PSMTXIdentity(mtxPos);
			GXLoadTexMtxImm(mtxPos, 0x1e, 1);
			for (j = 0; j < 0x1e; j += 2) {
				v = (f32*)elem->vertexBuf->address + j * 5;
				GXBegin(0x80, 0, 4);
				GXPosition3f32(v[0], v[1], v[2]);
				GXTexCoord2f32(v[3], v[4]);
				GXPosition3f32(v[10], v[11], v[12]);
				GXTexCoord2f32(v[13], v[14]);
				GXPosition3f32(v[15], v[16], v[17]);
				GXTexCoord2f32(v[18], v[19]);
				GXPosition3f32(v[5], v[6], v[7]);
				GXTexCoord2f32(v[8], v[9]);
			}
			break;
		}
		case 3:
		case 7:
			break;
		case 4:
		case 5: {
			Vec axis;
			Vec tmpAxis;
			f28 = f31 / 360.f;
			tmpAxis = (Vec){0.0f, 0.0f, 0.0f};
			tmpAxis.x = (f32)cos(f28);
			tmpAxis.z = (f32)sin(f28);
			axis = tmpAxis;
			PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
			PSMTXScale(mtxScale, elem->scale, elem->scale, elem->scale);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXTrans(mtxScale, 0.0f, (f32)(type == 4 ? 8 : 16), 0.0f);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXRotAxisRad(mtxRot, &axis, 0.017453292f * -elem->spin);
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
			PSMTXTrans(mtxScale, 0.0f, (f32)-(type == 4 ? 8 : 16), 0.0f);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXConcat(mtxView, mtxPos, mtxPos);
			if (elem->poseId != -1) {
				animPoseDrawMtx(elem->poseId, mtxPos, 1, 0.0f, 10.f);
				animPoseDrawMtx(elem->poseId, mtxPos, 2, 0.0f, 10.f);
				animPoseDrawMtx(elem->poseId, mtxPos, 3, 0.0f, 10.f);
			}
			break;
		}
		case 6: {
			GXColor color;
			if (elem->poseId != -1) {
				PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
				if (f30 < 180.f) {
					PSMTXScale(mtxScale, elem->scale, elem->scale, elem->scale);
				} else {
					PSMTXScale(mtxScale, -elem->scale, elem->scale, elem->scale);
				}
				PSMTXConcat(mtxPos, mtxScale, mtxPos);
				if (elem->spin) {
					PSMTXRotRad(mtxRot, 'z', 0.017453292f * -elem->spin);
					PSMTXConcat(mtxPos, mtxRot, mtxPos);
				} else {
					PSMTXTrans(mtxScale, -200.f, 0.0f, 0.0f);
					PSMTXConcat(mtxPos, mtxScale, mtxPos);
					PSMTXRotRad(mtxRot, 'z', 0.017453292f * -elem->roll);
					PSMTXConcat(mtxPos, mtxRot, mtxPos);
					PSMTXTrans(mtxScale, 200.f, 0.0f, 0.0f);
					PSMTXConcat(mtxPos, mtxScale, mtxPos);
				}
				PSMTXConcat(mtxView, mtxPos, mtxPos);
				PSMTXTrans(mtxScale, -82.655f, 88.8f, 0.0f);
				PSMTXConcat(mtxPos, mtxScale, mtxScale);
				PSMTXScale(mtxRot, 1.03f, 1.03f, 1.03f);
				PSMTXConcat(mtxScale, mtxRot, mtxRot);
				PSMTXTrans(mtxScale, 82.655f, -88.8f, 0.0f);
				PSMTXConcat(mtxRot, mtxScale, mtxScale);
				animPoseSetMaterialFlagOn(elem->poseId, 0x40);
				color = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
				animPoseSetMaterialEvtColor(elem->poseId, color);
				animPoseDrawMtx(elem->poseId, mtxScale, 1, 0.0f, 10.f);
				animPoseDrawMtx(elem->poseId, mtxScale, 2, 0.0f, 10.f);
				animPoseDrawMtx(elem->poseId, mtxScale, 3, 0.0f, 10.f);
				animPoseSetMaterialFlagOn(elem->poseId, 0x40);
				color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
				animPoseSetMaterialEvtColor(elem->poseId, color);
				animPoseDrawMtx(elem->poseId, mtxPos, 1, 0.0f, 10.f);
				animPoseDrawMtx(elem->poseId, mtxPos, 2, 0.0f, 10.f);
				animPoseDrawMtx(elem->poseId, mtxPos, 3, 0.0f, 10.f);
			}
			break;
		}
		case 8: {
			if (elem->poseId != -1) {
				PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
				PSMTXScale(mtxScale, elem->scale, elem->scale, elem->scale);
				PSMTXConcat(mtxPos, mtxScale, mtxPos);
				PSMTXConcat(mtxView, mtxPos, mtxPos);
				animPoseSetMaterialFlagOn(elem->poseId, 0x40);
				animPoseSetMaterialEvtColor(elem->poseId, (GXColor){ 0xFF, 0xFF, 0xFF, elem->alpha });
				animPoseDrawMtx(elem->poseId, mtxPos, 1, 0.0f, 10.f);
				animPoseDrawMtx(elem->poseId, mtxPos, 2, 0.0f, 10.f);
				animPoseDrawMtx(elem->poseId, mtxPos, 3, 0.0f, 10.f);
			}
			break;
		}
		}
	}
}

void effFallDispInseki(CameraId cameraId, void* param) {
	f32 rad;
	EffectEntry* effect = param;
	FallWork* work = effect->userdata;
	CameraEntry* cam;
	Mtx mtxA, mtxD, mtxB, mtxC;
	Vec axis;
	GXColor color;
	s32 i;

	camGetPtr(cameraId);
	PSMTXTrans(mtxA, work->x, work->y, work->z);
	cam = camGetPtr(cameraId);
	rad = -cam->viewYaw;
	rad = 0.017453292f * rad;
	PSMTXRotRad(mtxB, 'y', rad);
	PSMTXConcat(mtxA, mtxB, mtxC);

	if (work[1].poseId != -1 && work[1].poseId2 != -1) {
		Vec tmpAxis;
		tmpAxis = (Vec){0.0f, 0.0f, 0.0f};
		tmpAxis.x = (f32)cos(6.2831855f * work[1].angle / 360.f);
		tmpAxis.z = (f32)sin(6.2831855f * work[1].angle / 360.f);
		axis = tmpAxis;

		for (i = 0; i < 2; i++) {
			if (i == 0) {
				PSMTXTrans(mtxA, work[1].x, work[1].y, work[1].z);
				PSMTXRotAxisRad(mtxB, &axis, 0.017453292f * -work[1].spin);
				PSMTXScale(mtxD, 2.05f * work[1].scale, 2.05f * work[1].scale,
				           2.05f * work[1].scale);
				PSMTXConcat(mtxC, mtxA, mtxA);
				PSMTXConcat(mtxA, mtxB, mtxA);
				PSMTXConcat(mtxA, mtxD, mtxA);
				animPoseSetMaterialFlagOn(work[1].poseId, 0x40);
				color = (GXColor){ 0x00, 0x00, 0x00, 0xFF };
				animPoseSetMaterialEvtColor(work[1].poseId, color);
			} else {
				PSMTXTrans(mtxA, work[1].x, work[1].y, work[1].z);
				PSMTXRotAxisRad(mtxB, &axis, 0.017453292f * -work[1].spin);
				PSMTXScale(mtxD, 2.0f * work[1].scale, 2.0f * work[1].scale,
				           2.0f * work[1].scale);
				PSMTXConcat(mtxC, mtxA, mtxA);
				PSMTXConcat(mtxA, mtxB, mtxA);
				PSMTXConcat(mtxA, mtxD, mtxA);
				animPoseSetMaterialFlagOn(work[1].poseId, 0x40);
				color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
				animPoseSetMaterialEvtColor(work[1].poseId, color);
			}
			animPoseDrawMtx(work[1].poseId, mtxA, 1, 0.0f, 10.f);
			animPoseDrawMtx(work[1].poseId, mtxA, 2, 0.0f, 10.f);
			animPoseDrawMtx(work[1].poseId, mtxA, 3, 0.0f, 10.f);
		}

		PSMTXTrans(mtxA, work[1].x, work[1].y, work[1].z);
		PSMTXScale(mtxD, 2.1f * work[1].scale, 2.1f * work[1].scale,
		           2.1f * work[1].scale);
		PSMTXConcat(mtxC, mtxA, mtxA);
		PSMTXConcat(mtxA, mtxD, mtxA);
		animPoseDrawMtx(work[1].poseId2, mtxA, 1, 0.0f, 10.f);
		animPoseDrawMtx(work[1].poseId2, mtxA, 2, 0.0f, 10.f);
		animPoseDrawMtx(work[1].poseId2, mtxA, 3, 0.0f, 10.f);
	}
}

// Full GX state, quad display list and both shibuki splash passes. The quad uses float literals and the
// splash pointer is re-read from effect->userdata. `rad`/`nf31`/`half` are
// the spellings that give the register-allocation clusters. `half` is deliberately non-const: a
// literal folds and the compiler commutes `x * 0.5f` to `0.5f * x`, breaking the quad's operand order.
void effFallDispWater(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	FallWork* work = effect->userdata;
	CameraEntry* cam;
	CameraEntry* camView;
	GXTexObj texObj;
	Mtx mtxPos, mtxScale, mtxRot, mtxView;
	u8* base;
	u16 height;
	f32 f31;
	f32 f30;
	f32 rad;
	f32 nf31;
	f32 half;
	base = lbl_803ADD20;
	camView = camGetPtr(cameraId);
	PSMTXTrans(mtxPos, work->x, work->y, work->z);
	cam = camGetPtr(cameraId);
	rad = -cam->viewYaw;
	rad = 0.017453292f * rad;
	PSMTXRotRad(mtxRot, 'y', rad);
	PSMTXConcat(mtxPos, mtxRot, mtxView);
	PSMTXConcat(camView->view, mtxView, mtxView);

	GXSetNumTevStages(2);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevOp(0, 0);
	GXSetTevOrder(1, 0xff, 0xff, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xf, 2, 0, 0xf);
	GXSetTevAlphaIn(1, 7, 1, 0, 7);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetCullMode(2);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xb, 1);
	GXSetVtxDesc(0xd, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
	GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);
	effGetTexObj(0x35, &texObj);
	GXLoadTexObj(&texObj, 0);

	f31 = 64.0f;
	f30 = 704.0f;
	f31 *= 0.5f;
	f30 *= 0.5f;
	PSMTXTrans(mtxPos, work[1].unk50, work[1].unk54, work[1].unk58);
	height = GXGetTexObjHeight(&texObj);
	PSMTXScale(mtxScale, 1.0f, f30 / (f32)height, 1.0f);
	PSMTXConcat(mtxPos, mtxScale, mtxPos);
	GXLoadTexMtxImm(mtxPos, 0x1e, 1);
	PSMTXTrans(mtxPos, work[1].x, work[1].y, work[1].z);
	PSMTXScale(mtxScale, work[1].scale, 1.0f, 1.0f);
	PSMTXConcat(mtxPos, mtxScale, mtxPos);
	PSMTXConcat(mtxView, mtxPos, mtxPos);
	GXLoadPosMtxImm(mtxPos, 0);
	GXSetTevColor(1, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
	nf31 = -f31;
	half = 0.5f;
	GXBegin(0x80, 0, 4);
	GXPosition3f32(nf31 * half, f30, 0.0f);
	GXColor4u8(0xff, 0xff, 0xff, 0xff);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(f31 * half, f30, 0.0f);
	GXColor4u8(0xff, 0xff, 0xff, 0xff);
	GXTexCoord2f32(2.0f, 0.0f);
	GXPosition3f32(f31 * half, 0.0f, 0.0f);
	GXColor4u8(0xff, 0xff, 0xff, 0x00);
	GXTexCoord2f32(2.0f, 1.0f);
	GXPosition3f32(0.5f * -f31, 0.0f, 0.0f);
	GXColor4u8(0xff, 0xff, 0xff, 0x00);
	GXTexCoord2f32(0.0f, 1.0f);

	work = effect->userdata;
	if (work[1].state > 0) {
		PSMTXTrans(mtxPos, work[1].x, work[1].y, work[1].z);
		PSMTXScale(mtxScale, 0.56f, 0.4f, 0.8f);
		PSMTXConcat(mtxView, mtxPos, mtxPos);
		PSMTXConcat(mtxPos, mtxScale, mtxPos);
		GXLoadPosMtxImm(mtxPos, 0);
		{
			f32 k = 0.125f;
			PSMTXTrans(mtxPos, work[1].unk54 * k, 0.0f, 0.0f);
		}
		GXLoadTexMtxImm(mtxPos, 0x1e, 1);
		GXSetTevColor(1, (GXColor){ 0xFF, 0xFF, 0xFF, work[1].alpha });
		effGetTexObj(0x37, &texObj);
		GXLoadTexObj(&texObj, 0);
		GXClearVtxDesc();
		GXSetVtxDesc(9, 2);
		GXSetVtxAttrFmt(0, 9, 1, 3, 8);
		GXSetArray(9, base + 0x40, 6);
		GXSetVtxDesc(0xa, 2);
		GXSetVtxAttrFmt(0, 0xa, 0, 1, 6);
		GXSetArray(0xa, shibuki_normal_tbl, 3);
		GXSetVtxDesc(0xb, 2);
		GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
		GXSetArray(0xb, shibuki_color0_tbl, 4);
		GXSetVtxDesc(0xd, 2);
		GXSetVtxAttrFmt(0, 0xd, 1, 3, 0xe);
		GXSetArray(0xd, base + 0x140, 4);
		GXCallDisplayList(shibuki_dl_1_0, 0x20);
		GXCallDisplayList(shibuki_dl_1_1, 0x20);
		GXCallDisplayList(shibuki_dl_1_2, 0x20);
		GXCallDisplayList(shibuki_dl_1_3, 0x20);
		PSMTXTrans(mtxPos, work[1].x, work[1].y, work[1].z);
		{
			f32 k = 0.8f;
			PSMTXScale(mtxScale, 0.7f * (work[1].scale * k), 0.5f * (work[1].scale * k), work[1].scale * k);
		}
		PSMTXConcat(mtxView, mtxPos, mtxPos);
		PSMTXConcat(mtxPos, mtxScale, mtxPos);
		GXLoadPosMtxImm(mtxPos, 0);
		{
			f32 k = 0.25f;
			PSMTXTrans(mtxPos, 0.0f, -work[1].unk54 * k, 0.0f);
		}
		GXLoadTexMtxImm(mtxPos, 0x1e, 1);
		GXSetTevColor(1, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
		effGetTexObj(0x36, &texObj);
		GXLoadTexObj(&texObj, 0);
		GXClearVtxDesc();
		GXSetVtxDesc(9, 2);
		GXSetVtxAttrFmt(0, 9, 1, 3, 8);
		GXSetArray(9, base + 0x40, 6);
		GXSetVtxDesc(0xa, 2);
		GXSetVtxAttrFmt(0, 0xa, 0, 1, 6);
		GXSetArray(0xa, shibuki_normal_tbl, 3);
		GXSetVtxDesc(0xb, 2);
		GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
		GXSetArray(0xb, shibuki_color0_tbl, 4);
		GXSetVtxDesc(0xd, 2);
		GXSetVtxAttrFmt(0, 0xd, 1, 3, 0xe);
		GXSetArray(0xd, base + 0x140, 4);
		GXCallDisplayList(base + 0x1e0, 0x40);
		GXCallDisplayList(base + 0x220, 0x40);
		GXCallDisplayList(base + 0x260, 0x40);
		GXCallDisplayList(base + 0x2a0, 0x40);
	}
}

s32 effFallCheckHit(EffectEntry* effect) {
	FallWork* work = effect->userdata;

	switch (work->type) {
	case 0:
	case 1:
	case 4:
	case 5:
		if (work[1].state >= 2) {
			return 1;
		}
		break;
	case 2:
	case 3:
		if (work[1].state >= 1) {
			return 1;
		}
		break;
	case 6:
		if (work[1].state < 2) {
			if (work[1].state != 1) {
				break;
			}
			if (work[1].timer <= 90) {
				break;
			}
		}
		return 1;
	case 7:
		if (work[1].state >= 4) {
			return 1;
		}
		break;
	case 8:
		if (work[1].state >= 3) {
			return 1;
		}
		break;
	}
	return 0;
}

