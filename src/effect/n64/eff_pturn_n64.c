#include "eff/n64/eff_pturn_n64.h"

void effPturnDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effPturnN64Data* data = effect->userdata;

    if (data->field_0x4 == 0) {
        return;
    }
    pturn_main_dispp(camId, param);
    if (data->field_0x0 == 1) {
        return;
    }
    pturn_num_dispp(camId, param);
}
