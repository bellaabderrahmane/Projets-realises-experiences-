#ifndef TOWER_H
#define TOWER_H

#include "wave.h"


    

    

/*
this enum strucure's used to define game state (purchasing gems in particular)
*/
typedef enum {
    NONE,
    SELECT_GEM,
} GameState;


/*this enum structure is used to define a cell type (with its effect)*/
typedef enum {
    PURE_RED,    // Pyro
    PURE_GREEN,  // Dendro
    PURE_BLUE,   // Hydro
    MIXED,        // Mixed gem without special effects but higher damage potential

} GemType;


/*this structure's used to define a gem wiith its proporties*/
typedef struct Gem {
    int x, y;            // Coordinates of the crystal on the grid
    float damage;        // Base damage of the crystal
    Cell * path_cells_in_vicinity ;
    GemType type;        // Type of the gem (PURE_RED, PURE_GREEN, PURE_BLUE, MIXED)
    int teinte;          // Color of the crystal, an angle between 0 and 359
    int level;           // Level of the crystal, influencing the damage calculation
    float radius ;
    int effet_special ;
    Cell** path_cells_in_range;  // Dynamic array of pointers to path cells in range
    int num_path_cells_in_range; // Number of path cells in range
    float projectile_x, projectile_y; // Coordinates of the projectile
    Monster * projectile_target;
    bool is_projectile_active;
    bool isReadyToShoot;
    double time_since_placed;
} Gem;

/*this strucure's used to define a tower*/
typedef struct Tower {
    int x, y;            // Coordinates of the tower on the grid
    int number;          // The ordinal number of the tower (1st tower, 2nd tower, etc.) ,basically he's postion in linked list
    Gem* crystal;        // Pointer to a Gem object (starts as NULL)
    struct Tower* next;  // Pointer to the next tower in the linked list
} Tower;


void scan_path_cells_in_range(Gem* gem, Cell** path_array, int path_size);
void applySplashDamage(Monster *hitMonster, float splashDamage, int splashRadius,Cell** path_array,WaveNode** head,int  path_size,float * player_health,int health_reserve_levem,int maximum_health) ;
void applyGemEffects(Monster *monster, Gem *gem,Cell** path_array,WaveNode** head_wave,int  path_size,float * player_health,int health_reserve_levem,int maximum_health);
void add_tower(int x, int y, int number,Tower ** head_tower);
void free_towers(Tower ** head_tower) ;
Tower* find_tower_at(int x, int y, Tower* head_tower);
void update_towers_and_projectiles(Tower* head_tower, WaveNode** head_wave, Cell** path_array, int  path_size,float * player_health,int health_reserve_levem,int maximum_health,double frametime);
void place_gem_on_tower(int mouse_x, int mouse_y, int selectedTowerX, int selectedTowerY, Tower** head_tower, Gem* inventory[], Cell** path_array, int path_size, GameState *gameState, bool *click_processed);
void process_tower_placement(int mouse_x, int mouse_y, Tower** head_tower, Cell grid[][HEIGHT_GRID], int* tower_num_add, float* player_health, float* mana_cost_tower, bool* can_place_tower, GameState* gameState, bool* click_processed);
void manage_tower_placement_interaction(int mouse_x, int mouse_y, Cell grid[][HEIGHT_GRID], int* tower_num_add, float* player_health, float* mana_cost_tower, bool* can_place_tower, Tower** head_tower);

#endif
