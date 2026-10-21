#include "evt/evt_fade.h"
#include "driver/fadedrv.h"
#include "driver/camdrv.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"

USER_FUNC(evt_fade_in) {
	s32 duration = evtGetValue(event, event->args[0]);

	if (isFirstCall) {
		fadeEntry(FADE_IN_BLACK, duration, (GXColor){0, 0, 0, 0xFF});
	}
	if (fadeIsFinish() != TRUE) {
		return EVT_RETURN_BLOCK;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_out) {
	s32 duration = evtGetValue(event, event->args[0]);

	if (isFirstCall) {
		fadeEntry(FADE_OUT_BLACK, duration, (GXColor){0, 0, 0, 0xFF});
	}
	if (fadeIsFinish() != TRUE) {
		return EVT_RETURN_BLOCK;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_entry) {
	s32* args = event->args;
	FadeType type;
	s32 duration;
	u8 r, g, b;

	type = evtGetValue(event, *args++);
	duration = evtGetValue(event, *args++);
	r = evtGetValue(event, *args++);
	g = evtGetValue(event, *args++);
	b = evtGetValue(event, *args++);
	fadeEntry(type, duration, (GXColor){r, g, b, 0xFF});
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_end_wait) {
	return fadeIsFinish() != TRUE ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_set_spot_pos) {
	s32* args = event->args;
	s32 x = evtGetValue(event, *args++);
	s32 y = evtGetValue(event, *args++);
	s32 z = evtGetValue(event, *args++);
	CameraEntry* cam;
	f32 proj[GX_PROJECTION_SZ];
	f32 viewport[GX_VIEWPORT_SZ];
	f32 savedProj[GX_PROJECTION_SZ];
	f32 sx, sy, sz;

	cam = camGetPtr(CAMERA_3D);
	GXGetProjectionv(savedProj);
	GXSetProjection(cam->projection, cam->projType);
	GXGetProjectionv(proj);
	GXGetViewportv(viewport);
	GXProject((f32)x, (f32)y, (f32)z, cam->view, proj, viewport, &sx, &sy, &sz);
	fadeSetSpotPos(
	    (f32)(-gp->fbWidth / 2) + sx,
	    (f32)(gp->efbHeight / 2) - sy);
	GXSetProjectionv(savedProj);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_set_anim_ofs_pos) {
	s32* args = event->args;
	f32 x = evtGetFloat(event, *args++);
	f32 y = evtGetFloat(event, *args++);

	fadeSetAnimOfsPos(x, y);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_set_anim_virtual_pos) {
	s32* args = event->args;
	f32 x = evtGetFloat(event, *args++);
	f32 y = evtGetFloat(event, *args++);
	f32 z = evtGetFloat(event, *args++);

	fadeSetAnimVirtualPos(x, y, z);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_set_mapchange_type) {
	s32* args = event->args;
	s32 type = evtGetValue(event, *args++);
	s32 fadeInType = evtGetValue(event, *args++);
	s32 fadeInDuration = evtGetValue(event, *args++);
	s32 fadeOutType = evtGetValue(event, *args++);
	s32 fadeOutDuration = evtGetValue(event, *args++);

	if (type == 0) {
		if (fadeInType != -1) {
			gp->mapFadeInType = fadeInType;
		}
		if (fadeInDuration != -1) {
			gp->mapFadeInDuration = fadeInDuration;
		}
		if (fadeOutType != -1) {
			gp->mapFadeOutType = fadeOutType;
		}
		if (fadeOutDuration != -1) {
			gp->mapFadeOutDuration = fadeOutDuration;
		}
	}
	else {
		if (fadeInType != -1) {
			gp->areaFadeInType = fadeInType;
		}
		if (fadeInDuration != -1) {
			gp->areaFadeInDuration = fadeInDuration;
		}
		if (fadeOutType != -1) {
			gp->areaFadeOutType = fadeOutType;
		}
		if (fadeOutDuration != -1) {
			gp->areaFadeOutDuration = fadeOutDuration;
		}
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_tec_onoff) {
	s32* args = event->args;
	s32 a = evtGetValue(event, *args++);
	s32 b = evtGetValue(event, *args++);

	if (a != 0) {
		if (b != 0) {
			fadeTecSoftOn();
		}
		else {
			fadeTecOn();
		}
	}
	else {
		if (b != 0) {
			fadeTecSoftOff();
		}
		else {
			fadeTecOff();
		}
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_softfocus_onoff) {
	if (evtGetValue(event, event->args[0])) {
		fadeSoftFocusOn();
	}
	else {
		fadeSoftFocusOff();
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_fade_reset) {
	fadeReset(evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}
