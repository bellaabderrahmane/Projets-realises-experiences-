#ifndef GEM_H
#define GEM_H


#include "tower.h"




void initialise_gem(Gem * gem);
void manage_gem_interaction(int mouse_x, int mouse_y, int* gem_level_to_be_purchased, float* mana_cost_per_gem_level, float* player_health, Gem* inventory[]);
void move_gem_to_fusioner(int mouse_x, int mouse_y, Gem* inventory[], Gem* fusioner[], bool* click_processed) ;
void move_gem_from_fusioner_to_inventory(int mouse_x, int mouse_y, Gem* inventory[], Gem* fusioner[], bool* click_processed);
void fuse_gems(Gem* fusioner[], Gem* inventory[], float* player_health);
void handle_failed_fusion(Gem* fusioner[], Gem* inventory[]);



#endif
