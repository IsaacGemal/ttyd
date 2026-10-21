#pragma once

#include <dolphin/mtx.h>

// Shared by the lecture units (rels/*/src/event/evt_lect_*.c). The helpers in rel/common/evt_lect/ (and x_flag.h,
// set_dir.h, bgm_start_wait.h next to it) compile to the same code in every lecture unit that has them. A unit
// includes each run of them where the run sits in its function order, after defining the file-scope data they use:
// `OSTime wwstart`, `s32 x_flag_data[2]`, `LectureButton lbtn[2]`, and, where it has those helpers, `EffectEntry* eff`
// and `Vec daiv`.
//
// MWCC gives every included file's functions their own .text section and links the unit's own section ahead of them,
// so a unit can take its helpers from these headers only if its own functions all come after them (before them in
// gon and mri, which are built without deferred inlining). gon, hei, mri, tik and usu take every helper from here.
// dou (its lect_status_on and lect_status_off swap bodies) and eki, gor, muj, tou and win (lecture_button_rtn,
// lecture_button_set_pos and in some effitemget_busaiku copy named .rodata Vecs of their own) keep their own copies
// and share only this header.

// One of the two buttons lecture_button_rtn draws through lecture_button_disp.
typedef struct LectureButton {
    u8 flags;          // 0x0
    u8 seq;            // 0x1
    u8 pad_0x2[0xA];   // 0x2
    Vec position;      // 0xC
    u8 pad_0x18[0x28]; // 0x18
    s16 iconId;        // 0x40
    s16 alpha;         // 0x42
} LectureButton;

// The lecture units pass the icon id converted as u16 (`clrlwi`), where icondrv_gx.h's definition signature takes an
// s32 (see the iconDispGxAlpha entry in tools/local_prototype_exceptions.json).
void iconDispGxAlpha(Vec* position, s16 flags, u16 iconId, u8 alpha, f32 scale);
