#include "eff/n64/eff_snowman_n64.h"
#include "eff/n64/eff_snow_dust_n64.h"
#include "eff/n64/eff_vapor_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <stdlib.h>
#include <dolphin/mtx.h>

// .rodata constants (named after their original addresses in the asm).

// .sdata/.data animation tables (bytes in the original's order).
s8 jump_hscale_data[20] = {
	0x64, 0x5A, 0x52, 0x4C, 0x48, 0x46, 0x4A, 0x59, 0x67, 0x74,
	0x76, 0x78, 0x78, 0x78, 0x78, 0x76, 0x72, 0x6E, 0x00, 0x00,
};
u8 land_hscale_data[28] = {
	0x64, 0x5A, 0x52, 0x4C, 0x48, 0x46, 0x4A, 0x59, 0x64, 0x6C, 0x6E, 0x6C, 0x66, 0x60,
	0x5F, 0x60, 0x63, 0x65, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x00,
};
u8 jump2_hscale_data[76] = {
	0x64, 0x5A, 0x52, 0x4C, 0x48, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46,
	0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46,
	0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46,
	0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x4A, 0x59,
	0x67, 0x74, 0x76, 0x78, 0x78, 0x78, 0x78, 0x76, 0x72, 0x6E, 0x00, 0x00,
};
s8 jump2_ry_data[76] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x0A, 0x10, 0x16, 0x1C, 0x21, 0x26, 0x28, 0x29, 0x29, 0x28,
	0x26, 0x21, 0x1C, 0x16, 0x10, 0x0A, 0x05, 0x01, 0x01, 0x05, 0x0A, 0x10, 0x16, 0x1C, 0x21, 0x26,
	0x28, 0x29, 0x29, 0x28, 0x26, 0x21, 0x1C, 0x16, 0x10, 0x0A, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
EffectEntry* effSnowmanN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effSnowmanN64Data* data;

	effect = effEntry();
	effect->type = "SnowmanN64";
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effSnowmanN64Data));
	effect->userdata = data;
	effect->callback = effSnowmanMain;
	effect->flags |= 2;
	data->type = type;
	data->frame = 0;
	if (lifetime <= 0) {
		data->lifetime = 1000;
	}
	else {
		data->lifetime = lifetime;
	}
	data->alpha = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->offX = 0.0f;
	data->offY = 200.0f;
	data->offZ = 0.0f;
	data->velX = 0.0f;
	data->velY = 0.0f;
	data->velZ = 0.0f;
	data->animIdx = 0;
	data->state = 2;
	data->scale = scale;
	data->bounceCount = 0;
	data->field_0x18 = 0xc8;
	data->field_0x1C = 0xdc;
	data->field_0x20 = 0xf0;
	data->velY = -8.0f;
	data->rotY = 0.0f;
	data->gravity = -1.0f;
	return effect;
}

void effSnowmanMain(EffectEntry* effect) {
	effSnowmanN64Data* data;
	Vec position2;
	Vec position;
	// The original keeps frame and velX in registers; reading the fields directly
	// is equivalent behavior but allocates different registers, so the cached forms are match-only.
#ifdef MUST_MATCH
	s32 frame;
#endif
	f32 speed;
#ifdef MUST_MATCH
	f32 velX;
#endif

	data = effect->userdata;
	position = (Vec){ 0.0f, 0.0f, 0.0f };
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->frame++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
#ifdef MUST_MATCH
	frame = data->frame;
#endif
	if (data->lifetime < 16) {
		data->alpha = data->lifetime << 4;
	}
#ifdef MUST_MATCH
	if (frame < 16) {
		data->alpha = frame * 16 + 15;
	}
#else
	if (data->frame < 16) {
		data->alpha = data->frame * 16 + 15;
	}
#endif
#ifdef MUST_MATCH
	velX = data->velX;
	if (velX) {
#else
	if (data->velX) {
#endif
		speed = data->velY;
		if (speed < 0.0f) {
			speed = -speed;
		}
#ifdef MUST_MATCH
		if (velX < 0.0f) {
#else
		if (data->velX < 0.0f) {
#endif
			data->rotY -= speed;
		}
		else {
			data->rotY += speed;
		}
	}

	switch (data->state) {
	case 0:
		data->hScale = (f32)land_hscale_data[data->animIdx++] * 0.01f;
		data->vScale = 2.0f - data->hScale;
		if (data->animIdx >= 27) {
			data->state = 2;
		}
		break;
	case 1:
		data->hScale = (f32)(s8)jump_hscale_data[data->animIdx++] * 0.01f;
		data->vScale = 2.0f - data->hScale;
		if (data->animIdx >= 18) {
			data->state = 2;
		}
		if (data->animIdx >= 8) {
			data->offX += data->velX;
			data->offY += data->velY;
			data->offZ += data->velZ;
			data->velY += -1.0f;
		}
		break;
	case 3:
		data->hScale = (f32)jump2_hscale_data[data->animIdx++] * 0.01f;
		data->vScale = 2.0f - data->hScale;
		data->rotY = (f32)(s8)jump2_ry_data[data->animIdx++];
		if (data->animIdx >= 74) {
			data->state = 2;
		}
		if (data->animIdx >= 64) {
			data->offX += data->velX;
			data->offY += data->velY;
			data->offZ += data->velZ;
			data->velY += -1.0f;
		}
		break;
	case 2:
		data->hScale = (f32)(s8)jump_hscale_data[17] * 0.01f;
		data->vScale = 2.0f - data->hScale;
		data->offX += data->velX;
		data->offY += data->velY;
		data->offZ += data->velZ;
		data->velY += data->gravity;
		if (data->offY < 0.0f) {
			switch (data->bounceCount) {
			case 0:
				data->state = 0;
				break;
			case 1:
				data->velX = 2.0f;
				data->velZ = 2.0f;
				data->velY = 10.0f;
				data->state = 1;
				break;
			case 2:
				data->velX = 2.0f;
				data->velZ = -2.0f;
				data->velY = 10.0f;
				data->state = 1;
				break;
			case 3:
				data->velX = -2.0f;
				data->velZ = -2.0f;
				data->velY = 10.0f;
				data->state = 1;
				break;
			case 4:
				data->velX = 10.0f;
				data->velZ = 0.0f;
				data->state = 1;
				data->velY = 14.0f;
				data->gravity = -1.0f;
				break;
			default:
				data->velX = 0.0f;
				data->velY = 0.0f;
				data->velZ = 0.0f;
				data->offY = 0.0f;
				data->state = 2;
				break;
			}
			data->offY = 0.0f;
			data->animIdx = 0;
			data->bounceCount++;
		}
		break;
	}
#ifdef MUST_MATCH
	if ((frame & 7) == 1) {
#else
	if ((data->frame & 7) == 1) {
#endif
		effVaporN64Entry(2, 0x28, data->x + data->offX + (f32)(rand() % 60) - 30.0f,
		                 data->y + data->offY + (f32)(rand() % 100) + 50.0f,
		                 data->z + data->offZ, 4.0f);
		effSnowDustN64Entry(1, data->x + data->offX, 10.0f + (data->y + data->offY),
		                    60.0f + (data->z + data->offZ), 10, 30, 60.0f, 100.0f, 2.0f);
	}
	dispEntry(4, dispCalcZ(&position2), 2, effSnowmanDisp, effect);
}

void effSnowmanDisp(CameraId cameraId, void* param) {
}