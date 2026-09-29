#include "eff/eff_torch.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"

void effTorchMain(EffectEntry* effect) {
	effTorchData* data = effect->userdata;

	dispEntry(CAMERA_3D, dispCalcZ(&data->position), 2, effTorchDisp, effect);
}
