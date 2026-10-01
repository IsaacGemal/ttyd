#pragma once

#include <dolphin/types.h>
s32 mario_plane_cancel(void);
s32 marioGetPlaneStatus(void);
s32 marioPlaneGetFlyingDir(void);
void marioForcePlaneAnime(void);
void mot_plane(void);
void mot_plane_post(void);
s32 chkCollision(f32* out);
