#pragma once

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// Not in battle_disp.h: btlDispTex4 takes its vectors and colour by value, which passes each one as the address of a
// caller-made copy. These callers make that copy themselves and pass its address, so they see this pointer form.
void btlDispTex4(s32 id, Vec* trans, Vec* scale, Vec* rot, GXColor* color);
