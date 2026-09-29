#include "eff/n64/eff_hokori_n64.h"
#include "driver/dispdrv.h"

// The body, both loop specialisations and the two position stack slots match. The two
// remaining differences are the int->float 2^52 magic, which the compiler pools as an
// anonymous local while the target names it double_to_int_802fa7b8, and a scheduler swap
// at the top: the userdata load lands after the vec3_802fa7a8 loads.
// NONMATCHING (98.16%): see above.
void effHokoriMain(EffectEntry* effect) {
	effHokoriData* data;
	Vec position2;
	Vec position;
	s32 type;
	s32 i;

	data = effect->userdata;
	position = vec3_802fa7a8;
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;
	data->life--;
	if (data->life < 0) {
		effDelete(effect);
	}
	else {
		type = data->type;
		if (data->life < 0x18) {
			data->field_0x18 = (s32)((f32)data->field_0x18 * float_0p9_8041f118);
		}
		i = 1;
		data++;
		if (type <= 1) {
			for (; i < effect->count; i++, data++) {
				data->field_0x10 += data->field_0x14;
				data->y += data->field_0x10;
				data->field_0x24 += float_0p005_8041f11c;
				data->field_0x1C += data->field_0x24;
				data->field_0x20 += data->field_0x28;
			}
		}
		else {
			for (; i < effect->count; i++, data++) {
				data->field_0x10 += data->field_0x14;
				data->y += data->field_0x10;
				data->field_0x1C += data->field_0x24;
				data->field_0x20 += data->field_0x28;
			}
		}
		dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effHokoriDisp, effect);
	}
}
