// lecture_button_disp. A lecture helper: see rel/common/evt_lect.h.
#include "rel/common/evt_lect.h"

// The iconId 0x75..0x7D range test is a switch: MWCC lowers the grouped cases to the
// original's `cmpwi 0x7e; bge zero; cmpwi 0x75; bge neg15; b zero` chain. Writing it as
// `if (a < 0x7E && a >= 0x75)` instead makes MWCC invert the second branch (`blt zero`).
static void lecture_button_disp(CameraId cameraId, void* param) {
    s32 i;

    for (i = 0; i < 2; i++) {
        LectureButton* b = &lbtn[i];
#ifdef MUST_MATCH
        // The original addresses the button position off a base of &lbtn[i] + 8 with a +4 displacement
        // (`addi r4, r9, 8` then loads at 4/8/0xC); the natural `b->position` builds no separate base.
        Vec* ppos = (Vec*)((u8*)b + 8);
#endif
        f32 yoff;
        Vec pos;

        if (b->flags & 1) {
            continue;
        }
        switch (b->iconId) {
            case 0x75: case 0x76: case 0x77: case 0x78: case 0x79:
            case 0x7A: case 0x7B: case 0x7C: case 0x7D:
                yoff = -15.0f;
                break;
            default:
                yoff = 0.0f;
                break;
        }
#ifdef MUST_MATCH
        pos = *(Vec*)((u8*)ppos + 4);
#else
        pos = b->position;
#endif
        pos.y += yoff;
        iconDispGxAlpha(&pos, 0x50, b->iconId, b->alpha, 1.0f);
    }
}
