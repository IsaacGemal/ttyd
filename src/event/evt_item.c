/* "evt_item" - Item-related Event Script Functions
 * Status: In progress.
 */
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "driver/itemdrv.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "system.h"

// `#pragma opt_propagation off` required: with it on MWCC rematerialises the polynomial
// coefficients into every branch, as in evt_sub_get_coin.
#pragma opt_propagation off
// NONMATCHING (76.90%): radian lands in f0 vs f5 and the coefficients/accumulator use
// different FPRs; event->userdataf temporaries are not stack-spilled (frame 0x50 vs 0x60).
USER_FUNC(evt_item_get_important_item) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	MarioWork* mario = marioGetPtr();
	f32 mx = mario->position.x;
	f32 my = mario->position.y + *(f32*)((u8*)mario + 0x1B8);
	f32 mz = mario->position.z;
	f32 f0, f1, f2, f3, f4, f5;

	if (isFirstCall) {
		f32 d = distABf(mx, mz, item->position.x, item->position.z) / 0.5f;
		f4 = my - item->position.y;
		f0 = f4 / d;
		f2 = 0.1f * d;
		event->userdataf[0] = d;
		event->userdataf[1] = f2 * 0.5f + f0;
		event->userdataf[2] = 0.5f;
		event->field_84 = 0;
	}

	f5 = (6.2831855f * angleABf(item->position.x, item->position.z, mx, mz)) / 360.0f;

	f4 = 0.00761f;
	f3 = 0.16605f;
	f2 = 1.0f;
	f1 = 3.1415927f;
	if (f5 <= f1) {
		f1 = 1.5707964f;
		if (f5 <= f1) {
			f0 = f5 * f5;
			f4 = f4 * f0 - f3;
			f4 = f4 * f0 + f2;
			f4 = f4 * f5;
		} else {
			f0 = f5 - f1;
			f1 = f1 - f0;
			f0 = f1 * f1;
			f4 = f4 * f0 - f3;
			f4 = f4 * f0 + f2;
			f4 = f4 * f1;
		}
	} else {
		f0 = 4.712389f;
		if (f5 < f0) {
			f1 = f5 - f1;
			f0 = f1 * f1;
			f4 = f4 * f0 - f3;
			f4 = f4 * f0 + f2;
			f4 = f4 * f1;
			f4 = -f4;
		} else {
			f0 = f5 - f0;
			f1 = 1.5707964f;
			f1 = f1 - f0;
			f0 = f1 * f1;
			f4 = f4 * f0 - f3;
			f4 = f4 * f0 + f2;
			f4 = f4 * f1;
			f4 = -f4;
		}
	}
	item->position.x += event->userdataf[2] * f4;

	f4 = 0.03705f;
	f3 = 0.4967f;
	f2 = 1.0f;
	f1 = 3.1415927f;
	if (f5 <= f1) {
		f1 = 1.5707964f;
		if (f5 <= f1) {
			f0 = f5 * f5;
			f4 = f4 * f0 - f3;
			f4 = f4 * f0 + f2;
		} else {
			f0 = f5 - f1;
			f0 = f1 - f0;
			f0 = f0 * f0;
			f4 = f4 * f0 - f3;
			f4 = f4 * f0 + f2;
			f4 = -f4;
		}
	} else {
		f0 = 4.712389f;
		if (f5 < f0) {
			f0 = f5 - f1;
			f0 = f0 * f0;
			f4 = f4 * f0 - f3;
			f4 = f4 * f0 + f2;
			f4 = -f4;
		} else {
			f0 = f5 - f0;
			f1 = 1.5707964f;
			f0 = f1 - f0;
			f0 = f0 * f0;
			f4 = f4 * f0 - f3;
			f4 = f4 * f0 + f2;
		}
	}
	item->position.z -= event->userdataf[2] * f4;
	item->position.y += event->userdataf[1];

	f1 = event->userdataf[0] - 1.0f;
	f2 = event->userdataf[1] - 0.1f;
	if (f1 >= 0.0f) {
		event->userdataf[0] = f1;
		event->userdataf[1] = f2;
		return EVT_RETURN_BLOCK;
	}
	item->position.x = mx;
	item->position.y = my;
	item->position.z = mz;
	return EVT_RETURN_DONE;
}

#pragma opt_propagation reset

#pragma opt_propagation off // needed: with propagation on the step value is coalesced into r27 instead of dividing into r4
USER_FUNC(evt_item_move_3d_position) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	f32 targetX = (f32)evtGetValue(event, args[1]);
	f32 targetY = (f32)evtGetValue(event, args[2]);
	f32 targetZ = (f32)evtGetValue(event, args[3]);
	s32 steps = evtGetValue(event, args[4]);
	s32 mode = evtGetValue(event, args[5]);
	ItemEntry* item = itemNameToPtr((char*)name);
#ifdef MUST_MATCH
	// MUST_MATCH: the u32 temporary and assigning curStep inside the first call keep the division
	// result in r4 and its callee-saved copy in r27; the natural s32 form homes it in r27 throughout.
	u32 rawStep;
#endif
	s32 curStep;

	if (isFirstCall) {
		if (item == NULL) {
			return EVT_RETURN_DONE;
		}
		event->userdata[0] = (s32)gp->renderTime;
		event->userdataf[1] = item->position.x;
		event->userdataf[2] = item->position.y;
		*(f32*)&event->field_84 = item->position.z;
	}
#ifdef MUST_MATCH
	rawStep = OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)event->userdata[0]);
	if ((s32)rawStep < steps) {
		item->position.x = intplGetValue(mode, curStep = rawStep, steps, event->userdataf[1], targetX);
#else
	curStep = (s32)OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)event->userdata[0]);
	if (curStep < steps) {
		item->position.x = intplGetValue(mode, curStep, steps, event->userdataf[1], targetX);
#endif
		item->position.y = intplGetValue(mode, curStep, steps, event->userdataf[2], targetY);
		item->position.z = intplGetValue(mode, curStep, steps, *(f32*)&event->field_84, targetZ);
		return EVT_RETURN_BLOCK;
	}
	item->position.x = targetX;
	item->position.y = targetY;
	item->position.z = targetZ;
	return EVT_RETURN_DONE;
}
#pragma opt_propagation reset

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

USER_FUNC(evt_item_set_bound_next_dir) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	s32 dir = evtGetValue(event, args[1]);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	*(s32*)((u8*)item + 0x90) = dir;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_item_set_bound_next_speed) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	f32 speed = evtGetFloat(event, args[1]);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	*(f32*)((u8*)item + 0x94) = speed;
	return EVT_RETURN_DONE;
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

USER_FUNC(evt_item_set_bound_limit) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	s32 limit = evtGetValue(event, args[1]);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	*(s32*)((u8*)item + 0x88) = limit;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_item_set_bound_rate) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	f32 rate = evtGetFloat(event, args[1]);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	*(f32*)((u8*)item + 0x80) = rate;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_item_set_gravity) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	f32 gravity = evtGetFloat(event, args[1]);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	*(f32*)((u8*)item + 0x84) = gravity;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_item_set_jump_power) {
	s32* args = event->args;
	ItemEntry* item = itemNameToPtr((char*)evtGetValue(event, args[0]));
	f32 power = evtGetFloat(event, args[1]);

	if (item == NULL) {
		return EVT_RETURN_DONE;
	}
	*(f32*)((u8*)item + 0x58) = power;
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
	*(s32*)((u8*)item + 0x54) = dir;
	*(f32*)((u8*)item + 0x50) = speed;
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

USER_FUNC(evt_item_delete) {
	s32* args = event->args;

	itemDelete((char*)evtGetValue(event, args[0]));
	return EVT_RETURN_DONE;
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
	itemEntry((char*)name, type, (u16)a3, evtId, a5, (f32)a6, (f32)a7, (f32)a8);
	return EVT_RETURN_DONE;
}
