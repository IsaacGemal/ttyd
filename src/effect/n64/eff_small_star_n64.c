#include "eff/n64/eff_small_star_n64.h"
#include "driver/effdrv.h"

void effSmallStarN64CamChg(EffectEntry* effect, s32 camId) {
    effSmallStarN64Data* data = effect->userdata;
    s32 i = 0;

    while (i < effect->count) {
        data->camera = camId;
        i++;
        data++;
    }
}
