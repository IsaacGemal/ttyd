/* "evt_item" - Item-related Event Script Functions
 * Status: In progress.
 */
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "driver/itemdrv.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "system.h"

#include "evt/evt_item.h"

#ifdef MUST_MATCH
// MUST_MATCH: retail loads the .sdata2 0.5 by name here (the literal form pools differently and scores lower).
extern const f32 float_0p5_8041af58;
#define EI_HALF float_0p5_8041af58
#else
#define EI_HALF 0.5f
#endif
static inline f32 item_sin_approx(f32 angle) {
	f32 v = 0.00761f;
	f32 b = 0.16605f;
	f32 one = 1.0f;
	f32 t, u;

	if (angle <= 3.1415927f) {
		if (angle <= 1.5707964f) {
			u = angle * angle;
			v = v * u - b;
			v = v * u + one;
			v = v * angle;
		} else {
			t = 1.5707964f - (angle - 1.5707964f);
			u = t * t;
			v = v * u - b;
			v = v * u + one;
			v = v * t;
		}
	} else if (angle < 4.712389f) {
		t = angle - 3.1415927f;
		u = t * t;
		v = v * u - b;
		v = v * u + one;
		v = v * t;
		v = -v;
	} else {
		t = 1.5707964f - (angle - 4.712389f);
		u = t * t;
		v = v * u - b;
		v = v * u + one;
		v = v * t;
		v = -v;
	}
	return v;
}

static inline f32 item_cos_approx(f32 angle) {
	f32 v = 0.03705f;
	f32 b = 0.4967f;
	f32 one = 1.0f;
	f32 u;

	if (angle <= 3.1415927f) {
		if (angle <= 1.5707964f) {
			u = angle * angle;
			v = v * u - b;
			v = v * u + one;
		} else {
			u = 1.5707964f - (angle - 1.5707964f);
			u = u * u;
			v = v * u - b;
			v = v * u + one;
			v = -v;
		}
	} else if (angle < 4.712389f) {
		u = angle - 3.1415927f;
		u = u * u;
		v = v * u - b;
		v = v * u + one;
		v = -v;
	} else {
		u = 1.5707964f - (angle - 4.712389f);
		u = u * u;
		v = v * u - b;
		v = v * u + one;
	}
	return v;
}

// itemEntry's third parameter is u16 (retail stores it with `sth` and masks it with `clrlwi`).
USER_FUNC(evt_item_entry) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	s32 type = evtGetValue(event, args[1]);
	s32 a6 = evtGetValue(event, args[2]);
	s32 a7 = evtGetValue(event, args[3]);
	s32 a8 = evtGetValue(event, args[4]);
	s32 a3 = evtGetValue(event, args[5]);
	s32 evtId = args[6];
	s32 a5 = evtGetValue(event, args[7]);
	itemEntry((char*)name, type, (u16)a3, (f32)a6, (f32)a7, (f32)a8, evtId, a5);
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_delete) {
	s32* args = event->args;

	itemDelete((char*)evtGetValue(event, args[0]));
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_delete_check) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));

	if (item != NULL) {
		evtSetValue(event, args[1], 0);
	}
	else {
		evtSetValue(event, args[1], 1);
	}
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_flag_on) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	u16 flag = args[1];

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	itemFlagOn(item, flag);
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_flag_off) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	u16 flag = args[1];

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	itemFlagOff(item, flag);
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_status_on) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	s32 mask = args[1];

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	itemStatusOn(item, mask);
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_set_move_dir_speed) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	ItemEntry* item = itemNameToPtr((char*)name);
	s32 dir = evtGetValue(event, args[1]);
	f32 speed = evtGetFloat(event, args[2]);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	item->moveDir = dir;
	item->moveSpeed = speed;
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_set_jump_power) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	f32 power = evtGetFloat(event, args[1]);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	item->jumpPower = power;
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_set_gravity) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	f32 gravity = evtGetFloat(event, args[1]);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	item->gravity = gravity;
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_set_bound_rate) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	f32 rate = evtGetFloat(event, args[1]);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	item->boundRate = rate;
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_set_bound_limit) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	s32 limit = evtGetValue(event, args[1]);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	item->boundLimit = limit;
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_get_item) {
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, event->args[0]));

	if (isFirstCall) {
		itemForceGet(item);
		return EVT_RETURN_BLOCK;
	}
	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	if (!(item->flags & 1)) {
		return EVT_RETURN_DONE;
	}
	if ((u32)itemStatus(item, 2) != 0) {
		return EVT_RETURN_DONE;
	}
	return itemStatus(item, 0x10000) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}
USER_FUNC(evt_item_get_item_end_wait) {
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, event->args[0]));

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	if (!(item->flags & 1)) {
		return EVT_RETURN_DONE;
	}
	if ((u32)itemStatus(item, 2) != 0) {
		return EVT_RETURN_DONE;
	}
	return itemStatus(item, 0x10000) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}
USER_FUNC(evt_item_change_mode) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	s32 mode = evtGetValue(event, args[1]);
	ItemEntry* item = itemNameToPtr((char*)name);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	itemModeChange(item, (u16)mode);
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_set_bound_next_speed) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	f32 speed = evtGetFloat(event, args[1]);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	item->boundNextSpeed = speed;
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_set_bound_next_dir) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	s32 dir = evtGetValue(event, args[1]);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	item->boundNextDir = dir;
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_set_position) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	f32 x = evtGetFloat(event, args[1]);
	f32 y = evtGetFloat(event, args[2]);
	f32 z = evtGetFloat(event, args[3]);
	ItemEntry* item = itemNameToPtr((char*)name);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	item->position.x = x;
	item->position.y = y;
	item->position.z = z;
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_get_position) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	s32 outX = args[1];
	s32 outY = args[2];
	s32 outZ = args[3];
	ItemEntry* item = itemNameToPtr((char*)name);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	evtSetFloat(event, outX, item->position.x);
	evtSetFloat(event, outY, item->position.y);
	evtSetFloat(event, outZ, item->position.z);
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_set_scale) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	f32 scale = evtGetFloat(event, args[1]);
	ItemEntry* item = itemNameToPtr((char*)name);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	item->scale = scale;
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_item_set_alpha) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	u8 alpha = evtGetValue(event, args[1]);
	ItemEntry* item = itemNameToPtr((char*)name);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	item->alpha = alpha;
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_item_move_3d_position) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	f32 targetX = (f32)evtGetValue(event, args[1]);
	f32 targetY = (f32)evtGetValue(event, args[2]);
	f32 targetZ = (f32)evtGetValue(event, args[3]);
	s32 steps = evtGetValue(event, args[4]);
	s32 mode = evtGetValue(event, args[5]);
	ItemEntry* item = itemNameToPtr((char*)name);
	int curStep; // int, not s32, as evt_npc.c's `int elapsed`: s32 allocates differently

	if (isFirstCall) {
		if (item == NULL) {
			return EVT_RETURN_DONE;
		}
		event->userdata[0] = (s32)gp->renderTime;
		event->userdataf[1] = item->position.x;
		event->userdataf[2] = item->position.y;
		*(f32*)&event->field_84 = item->position.z;
	}
	curStep = (s32)OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)event->userdata[0]);
	if (curStep < steps) {
		item->position.x = intplGetValue(mode, curStep, steps, event->userdataf[1], targetX);
		item->position.y = intplGetValue(mode, curStep, steps, event->userdataf[2], targetY);
		item->position.z = intplGetValue(mode, curStep, steps, *(f32*)&event->field_84, targetZ);
		return EVT_RETURN_BLOCK;
	}
	item->position.x = targetX;
	item->position.y = targetY;
	item->position.z = targetZ;
	return EVT_RETURN_DONE;
}

#pragma opt_propagation off // needed by evt_item_get_important_item: keeps the sin/cos approximation constants in registers
#pragma opt_lifetimes off // needed: without it evt_item_get_important_item 96.10%
USER_FUNC(evt_item_get_important_item) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	MarioWork* mario = marioGetPtr();
	f32 mx = mario->position.x;
	f32 my = mario->position.y + mario->wPlayerCollisionBox.y;
	f32 mz = mario->position.z;
	f32 f2, f4, f5;
	f32 t;
	f32 scale;
	f32 d;

	scale = EI_HALF;
	if (isFirstCall) {
		d = distABf(mx, mz, item->position.x, item->position.z) / scale;
		f4 = my - item->position.y;
		f2 = 0.1f * d;
		t = f2 * EI_HALF + f4 / d;
		event->userdata[0] = *(s32*)&d;
		event->userdata[1] = *(s32*)&t;
		event->userdata[2] = *(s32*)&scale;
		event->field_84 = 0;
	}

	d = event->userdataf[0];
	t = event->userdataf[1];
	scale = event->userdataf[2];

	f2 = 6.2831855f * angleABf(item->position.x, item->position.z, mx, mz);
	f5 = f2 / 360.0f;
	item->position.x = scale * item_sin_approx(f5) + item->position.x;
	item->position.z = item->position.z - scale * item_cos_approx(f5);
	item->position.y = item->position.y + t;

	t = t - 0.1f;
	d = d - 1.0f;
	if (d < 0.0f) {
		item->position.x = mx;
		item->position.y = my;
		item->position.z = mz;
		return EVT_RETURN_DONE;
	}
	event->userdata[0] = *(s32*)&d;
	event->userdata[1] = *(s32*)&t;
	return EVT_RETURN_BLOCK;
}
#pragma opt_lifetimes reset
#pragma opt_propagation reset

#ifdef MUST_MATCH
// The .sdata2 pool's first word. Named, so it lands first in .sdata2 where retail has it; scale uses it too, so the
// pool holds no second 0.5. Defined after the code so its uses stay loads.
const f32 float_0p5_8041af58 = 0.5f;
#endif
