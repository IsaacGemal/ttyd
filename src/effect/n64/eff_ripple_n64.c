#include "eff/n64/eff_ripple_n64.h"
#include "driver/effdrv.h"

void effRippleN64SetRxRz(EffectEntry* effect, f32 rx, f32 rz) {
	effRippleN64Data* data = effect->userdata;

	data->field_0x2C = rx;
	data->field_0x30 = rz;
}
