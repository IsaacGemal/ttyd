#pragma once

typedef struct EventEntry EventEntry;

#include <dolphin/types.h>
void N__disp_2D(s32 type, s32 index);
void main_button(s32 index);
s32 main_suki(EventEntry* event, BOOL isFirstCall);

typedef struct MapTevCallback MapTevCallback;
void set_tev(MapTevCallback* param);
extern u32 weapon_words[48];
s32 end_suki(EventEntry* event, BOOL isFirstCall);
s32 wait_star_stone_up_end(EventEntry* event, BOOL isFirstCall);
s32 star_stone_get_attack_point(EventEntry* event);
s32 wait_star_stone_attack_end(EventEntry* event, BOOL isFirstCall);
s32 aud_set_draw(EventEntry* event);
s32 flash_update(EventEntry* event);
s32 exist_map(EventEntry* event);
s32 get_screen_point(EventEntry* event);
s32 eff_crystal(EventEntry* event);
s32 get_img_name(EventEntry* event);
