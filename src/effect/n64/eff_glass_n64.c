#include "eff/n64/eff_glass_n64.h"
#include "driver/dispdrv.h"

void effGlassMain(EffectEntry* effect) {
	effGlassData* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
	   local before dispCalcZ; a single local folds the copy away (same idiom as
	   effFlameMain). */
	Vec position;
#endif
	Vec disp = { 0 };

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 16;
	}

	if (data->life < 1000) {
		data->life--;
	}
	data->counter++;

	if (data->life < 0) {
		effDelete(effect);
		return;
	}

	if (data->life < 16) {
		data->alpha = data->life * 16;
	}

	data->angle += data->angleVel;
	if (data->angle < float_0_8041f094) {
		data->angle += float_1_8041f098;
	}
	if (data->angle > float_1_8041f098) {
		data->angle = data->angle - float_1_8041f098;
	}

	data->velY += data->gravity;
	data->position.x += data->velX;
	data->position.y += data->velY;
	data->position.z += data->velZ;
	data->rot += data->rotVel;

#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effGlassDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effGlassDisp, effect);
#endif
}
