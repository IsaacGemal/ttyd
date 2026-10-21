// lecture_button_rtn. A lecture helper: see rel/common/evt_lect.h.
#include "driver/dispdrv.h"
#include "rel/common/evt_lect.h"

#include <string.h>

static USER_FUNC(lecture_button_rtn) {
    s32 i;
    LectureButton* btn;
    s32 done;

    if (isFirstCall) {
        memset(lbtn, 0, sizeof(lbtn));
        for (i = 0; i < 2; i++) {
            lbtn[i].position = (Vec){200.0f, -25.0f, 0.0f};
        }
    }
    done = 0;
    for (i = 0; i < 2; i++) {
        btn = &lbtn[i];
        switch (btn->seq) {
            case 0:
                btn->iconId = 0x68;
                btn->seq = 1;
                break;
            case 2:
                btn->alpha += 7;
                if (btn->alpha >= 0xFF) {
                    btn->alpha = 0xFF;
                    btn->seq = 1;
                }
                break;
            case 3:
                btn->alpha -= 7;
                if (btn->alpha <= 0) {
                    btn->alpha = 0;
                    btn->seq = 1;
                }
                break;
            case 4:
                btn->flags |= 1;
                done++;
                break;
        }
    }
    if (done >= 2) {
        return EVT_RETURN_DONE;
    }
    dispEntry(CAMERA_2D, 0.0f, 1, lecture_button_disp, NULL);
    return EVT_RETURN_BLOCK;
}
