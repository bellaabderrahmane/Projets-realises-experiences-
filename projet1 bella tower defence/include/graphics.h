#ifndef GRAPHICS_H
#define GRAPHICS_H

    #include <MLV/MLV_all.h>
    #include <stdio.h>
    #include <stdbool.h>
    #include <time.h>
    #include <math.h>




    

    #include "gem.h"



void draw_grid(Cell grid[WIDTH_GRID][HEIGHT_GRID]);
void draw_player_health_bar(float current_health, float max_health) ;
void draw_mana_box(float mana_cost, MLV_Color color) ;
void draw_time_box(int time_spent, int wave_interval, int box_x, int box_y, int box_width, int box_height) ;
void draw_wave_box(int wave_number) ;
void draw_gem_ui(int gem_level_to_be_purchased, float mana_cost_per_gem_level);
void draw_gem_inventory(Gem* inventory[], int max_gems) ;
void draw_gem_fusioner(Gem* fusioner[]);
void draw_inventory_highlights(Gem* inventory[]);



#endif
