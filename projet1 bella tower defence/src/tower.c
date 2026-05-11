#include "../include/tower.h"


/* 
    * Adds a new tower to the game at the specified grid coordinates.
    * The new tower is appended to the end of the linked list of towers.
    * The function initializes the tower's attributes and handles memory allocation.
    *
    * @param x The x-coordinate on the grid where the tower is to be placed.
    * @param y The y-coordinate on the grid where the tower is to be placed.
    * @param number An identifier number for the new tower.
    * @param head_tower Pointer to the pointer to the head of the linked list of towers.
    * @return void
    */
    void add_tower(int x, int y, int number,Tower ** head_tower) {
        // Create a new tower
        Tower* new_tower = (Tower*)malloc(sizeof(Tower));
        if (!new_tower) {
            perror("Failed to allocate memory for new tower");
            exit(EXIT_FAILURE);
        }
        // Initialize the new tower's attributes
        new_tower->x = x;
        new_tower->y = y;
        new_tower->number = number;
        new_tower->crystal = NULL;
        new_tower->next = NULL;
        // Find the last tower in the list
        if (*head_tower == NULL) {
            // If the list is empty, the new tower is now the head
            *head_tower = new_tower;
        } else {
            // If the list is not empty, append the new tower to the end of the list
            Tower* current = *head_tower;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = new_tower;
        }
    }



    void free_towers(Tower ** head_tower) {
        Tower* current = *head_tower;
        while (current != NULL) {
            Tower* next = current->next;
            free(current);  // Free the memory for the current tower
            current = next;
        }
        *head_tower = NULL;  // Reset the head of the list to NULL
    }


    /* 
 * Searches for a tower at a given position in the grid. Returns the tower if found, otherwise returns NULL.
 * The function iterates through the linked list of towers until it finds a tower at the specified coordinates.
 *
 * @param x The x-coordinate on the grid where the tower is expected to be found.
 * @param y The y-coordinate on the grid where the tower is expected to be found.
 * @param head_tower Pointer to the head of the linked list of towers.
 * @return Pointer to the found Tower structure or NULL if no tower is found at the specified coordinates.
 */
Tower* find_tower_at(int x, int y, Tower* head_tower) {
    Tower* current = head_tower;
    while (current != NULL) {
        if (current->x == x && current->y == y) {
            return current; // Tower found at (x, y)
        }
        current = current->next;
    }
    return NULL; // No tower found at (x, y)
}



/* 
 * Updates towers and their projectiles, including launching projectiles, tracking and applying gem effects, and managing health and wave dynamics.
 * The function iterates through all towers, updates projectile positions, and handles collisions and health deductions for hit monsters.
 *
 * @param head_tower Pointer to the head of the linked list of towers.
 * @param head_wave Pointer to the head of the linked list of WaveNodes representing all waves in the game.
 * @param path_array Array of pointers to Cell structures representing the path on the grid.
 * @param path_size The total number of cells in the path.
 * @param player_health Pointer to the player's current health.
 * @param health_reserve_levem The level of health reserve, affecting health regeneration.
 * @param maximum_health The maximum health the player can have.
 * @param frametime The time elapsed since the last update.
 * @return void
 */

void update_towers_and_projectiles(Tower* head_tower, WaveNode** head_wave, Cell** path_array, int  path_size,float * player_health,int health_reserve_levem,int maximum_health,double frametime) {
    Tower* current_tower = head_tower;
    while (current_tower != NULL) {
        Gem* gem = current_tower->crystal;
        if (gem != NULL){
            gem->time_since_placed += frametime;  // Increase the time since placed
            if (gem->time_since_placed >= 0.2 && !gem->isReadyToShoot) {  // Check if 2 second
                gem->isReadyToShoot = true;  // Gem is now ready to shoot
            }}
        if (gem != NULL && gem->is_projectile_active && gem->projectile_target != NULL) {
            
            Monster* target_monster = gem->projectile_target;
            float dx = target_monster->x - gem->projectile_x;
            float dy = target_monster->y - gem->projectile_y;
            float distance = sqrt(dx * dx + dy * dy);

            // Move projectile towards the target
            
            if (distance > 0 && target_monster != NULL) {
                float speed_factor = 5.0; // Speed of the projectile
                gem->projectile_x += speed_factor * (dx / distance);
                gem->projectile_y += speed_factor * (dy / distance);
            }
            
            float hitThreshold = 5.0; // Adjust this value as needed
            if (distance < hitThreshold) {
                // Projectile has hit the target
                target_monster->hp -= D * pow(2, gem->level) * (1 - cos((gem->teinte - target_monster->teinte) / 2.0)); 
                applyGemEffects(target_monster, gem, path_array,head_wave,path_size,player_health,health_reserve_levem,maximum_health);
                updateMonsterEffects(target_monster, frametime);
                gem->is_projectile_active = false;

                if (target_monster->hp <= 0) {
                    // Deactivate the monster
                    
                    target_monster->isActive = false;
                    // Find the monster in the wave and set its reference to NULL
                    WaveNode* currentWaveNode = *head_wave;
                    while (currentWaveNode != NULL) {
                        Wave* current_wave = currentWaveNode->newWave;
                        for (int j = 0; j < current_wave->numMonsters; j++) {
                            if (current_wave->monsters[j] == target_monster) {
                                *player_health += ((current_wave->wave_initial_health * 10)/100) * pow(1.3,health_reserve_levem);
                                if(*player_health > maximum_health){
                                    *player_health = maximum_health ;
                                }
                                
                                current_wave->monsters[j] = NULL; // Remove the monster from the wave

                                // Set projectile target to NULL if it was targeting the dead monster
                                Tower* tower = head_tower;
                                while (tower != NULL) {
                                    if (tower->crystal != NULL && tower->crystal->projectile_target == target_monster) {
                                        tower->crystal->projectile_target = NULL;
                                        tower->crystal->is_projectile_active = false;
                                        
                                    }
                                    tower = tower->next;
                                }
                                
                                break;
                            }
                        }
                        currentWaveNode = currentWaveNode->next;
                    }
                    if (target_monster != NULL) {
                        //free(target_monster);
                        target_monster = NULL;
                    }

                } 
            }
            
        } else if (gem != NULL && gem->isReadyToShoot && !gem->is_projectile_active) {
            
            // Scan for monsters in range and find the one with the highest HP
            Monster* target_monster = NULL;
            float max_hp = -1;
            
            for (int i = 0; i < gem->num_path_cells_in_range; i++) {
                for (int i = 0; i < gem->num_path_cells_in_range; i++) {
                Cell* current_cell = gem->path_cells_in_range[i];
                float cell_left_edge = current_cell->x * SIZE_CELL;
                float cell_right_edge = (current_cell->x + 1) * SIZE_CELL;
                float cell_top_edge = current_cell->y * SIZE_CELL;
                float cell_bottom_edge = (current_cell->y + 1) * SIZE_CELL;
                
                WaveNode* current_wave_node = *head_wave;
                
                while (current_wave_node != NULL) {
                    Wave* current_wave = current_wave_node->newWave;
                    
                    for (int j = 0; j < current_wave->numMonsters; j++) {
                        Monster* monster = current_wave->monsters[j];

                        //monster->isActive && monster->x >= cell_left_edge ;
                        
                        if (monster != NULL && monster->currentPathIndex != 0 &&  ((monster->isActive && monster->x >= cell_left_edge) && (monster->x < cell_right_edge && monster->y >= cell_top_edge)) && (monster->y < cell_bottom_edge && monster->hp > max_hp)) {
                            target_monster = monster;
                            max_hp = monster->hp;
                            
                        }
                        
                    }
                    
                    current_wave_node = current_wave_node->next;
                }
            }


            }

            if (target_monster != NULL) {
                // Target monster found, launch projectile
                gem->projectile_x = current_tower->x * SIZE_CELL + SIZE_CELL / 2;
                gem->projectile_y = current_tower->y * SIZE_CELL + SIZE_CELL / 2;
                gem->projectile_target = target_monster;
                gem->is_projectile_active = true;
            }
        }

        current_tower = current_tower->next;
    }
}


/* 
 * Applies special effects of a gem to a monster, such as inflicting damage or applying status effects like slow or parasite effects.
 * The function determines the effect based on the type of the gem and applies it to the target monster.
 *
 * @param monster Pointer to the Monster structure to which the gem's effects are to be applied.
 * @param gem Pointer to the Gem structure that is applying its effects.
 * @param path_array Array of pointers to Cell structures representing the path on the grid.
 * @param head_wave Pointer to the head of the linked list of WaveNodes representing all waves in the game.
 * @param path_size The total number of cells in the path.
 * @return void
 */
void applyGemEffects(Monster *monster, Gem *gem,Cell** path_array,WaveNode** head_wave,int  path_size,float * player_health,int health_reserve_levem,int maximum_health) {
    switch (gem->type) {
        case PURE_RED:  // Pyro
            if(monster->isUnderParasiteEffect){
                monster->hp -= 3* D * pow(2, gem->level) * (1 - cos((gem->teinte - monster->teinte) / 2.0)); //effet combustion
                monster->isUnderParasiteEffect = false ;
            }else{
                monster->hp -= D * pow(2, gem->level) * (1 - cos((gem->teinte - monster->teinte) / 2.0)); 
                applySplashDamage(monster, (D*15)/100, 2,path_array,head_wave,  path_size,player_health,health_reserve_levem,maximum_health);
            }
            // Apply immediate splash damage to nearby monsters
             break;
        case MIXED:  // Pyro
            // Apply immediate splash damage to nearby monsters
            int random_n  = rand() % 100 ;
            if(random_n <  10){
                monster->hp -= 2 * D * pow(2, gem->level) * (1 - cos((gem->teinte - monster->teinte) / 2.0));
            }else{
                monster->hp -= D * pow(2, gem->level) * (1 - cos((gem->teinte - monster->teinte) / 2.0)); //hit him a second tim thus doubling damge
            }
            break;
        
        case PURE_GREEN:  // Dendro
            monster->isUnderParasiteEffect = true;
            monster->parasiteEffectTimer = 10;
            monster->parasiteDamageTimer = 0.2;
            break;
        
        case PURE_BLUE:  // Hydro
            monster->isUnderSlowEffect = true;
            monster->slowEffectTimer = 1;
            // Adjust monster speed
            monster->speed_factor = 1.5;
            break;
        
    }
}

 /* 
 * Scans the path to find cells within the range of a given gem and stores these cells in the gem's data structure.
 * The function calculates the distance from the gem to each cell in the path and includes cells within the gem's radius.
 *
 * @param gem Pointer to the Gem structure whose range is to be scanned.
 * @param path_array Array of pointers to Cell structures representing the path on the grid.
 * @param path_size The total number of cells in the path.
 * @return void
 */

void scan_path_cells_in_range(Gem* gem, Cell** path_array, int path_size) {
    // Temporary list to hold cells in range
    Cell** temp_list = (Cell**)malloc(path_size * sizeof(Cell*));
    int count = 0;

    for (int i = 0; i < path_size; i++) {
        float dx = path_array[i]->x * SIZE_CELL + SIZE_CELL / 2 - (gem->x );
        float dy = path_array[i]->y * SIZE_CELL + SIZE_CELL / 2 - (gem->y );
        float distance = sqrt(dx * dx + dy * dy);
        if (distance <= gem->radius ) {
            temp_list[count++] = path_array[i];
        }
    }

    // Allocate exact space needed for cells in range and copy them from temp_list
    gem->path_cells_in_range = (Cell**)malloc(count * sizeof(Cell*));
    memcpy(gem->path_cells_in_range, temp_list, count * sizeof(Cell*));
    gem->num_path_cells_in_range = count;
    
    // Clean up the temporary list
    free(temp_list);
}


/* 
 * Applies splash damage to monsters around the target monster hit by a gem.
 * The function iterates through all path cells within the splash radius and applies damage to active monsters on those cells.
 *
 * @param hitMonster Pointer to the Monster structure that was directly hit by the gem.
 * @param splashDamage The amount of damage to apply to each monster within the splash radius.
 * @param splashRadius The radius around the hit monster within which other monsters will receive splash damage.
 * @param path_array Array of pointers to Cell structures representing the path on the grid.
 * @param head Pointer to the head of the linked list of WaveNodes representing all waves in the game.
 * @param path_size The total number of cells in the path.
 * @return void
 */
void applySplashDamage(Monster *hitMonster, float splashDamage, int splashRadius,Cell** path_array,WaveNode** head,int  path_size,float * player_health,int health_reserve_levem,int maximum_health) {
    // Iterate through all the path cells within the splash radius
    for (int i = 0; i < path_size; i++) {
        float dx = path_array[i]->x * SIZE_CELL + SIZE_CELL / 2 - hitMonster->x;
        float dy = path_array[i]->y * SIZE_CELL + SIZE_CELL / 2 - hitMonster->y;
        float distance = sqrt(dx * dx + dy * dy) / SIZE_CELL;  // Normalize by cell size to get distance in terms of cells

        // Check if the path cell is within the splash radius
        if (distance <= splashRadius) {
            // Check for monsters on the current path cell
            WaveNode* currentWaveNode = *head;
            while (currentWaveNode != NULL) {
                Wave* current_wave = currentWaveNode->newWave;
                for (int j = 1; j < current_wave->numMonsters; j++) {
                    Monster* monster = current_wave->monsters[j];
                    if (monster != NULL && monster->isActive && monster != hitMonster) {
                        float target_dx = monster->x - hitMonster->x;
                        float target_dy = monster->y - hitMonster->y;
                        float target_distance = sqrt(target_dx * target_dx + target_dy * target_dy) / SIZE_CELL;

                        // Apply splash damage if the monster is within the splash radius
                        if (target_distance <= splashRadius && monster->currentPathIndex != 0) {
                            monster->hp -= splashDamage;
                            if(monster->hp <= 0){
                                int health_to_add ;
                                if(monster->type_of_monster == BOSS){
                                    health_to_add = 12 * INITIAL_HP_CONSTANT * pow(1.2,current_wave->wave_number) ;
                                }else{
                                    health_to_add = 12 * INITIAL_HP_CONSTANT * pow(1.2,current_wave->wave_number) ;
                                }
                                 *player_health += ((health_to_add * 10)/100) * pow(1.3,health_reserve_levem);
                                if(*player_health > maximum_health){*player_health = maximum_health;}
                                monster->hp = 0;

                            }
                            // You can add additional effects or checks here if needed
                        }
                    }
                }
                currentWaveNode = currentWaveNode->next;
            }
        }
    }
}

/* 
 * Places a selected gem from the inventory onto a tower, updating the gem's position, radius, and other properties.
 * The function finds the tower at the selected coordinates, assigns the gem to the tower, and updates inventory and game state.
 *
 * @param mouse_x The x-coordinate of the mouse click.
 * @param mouse_y The y-coordinate of the mouse click.
 * @param selectedTowerX The x-coordinate of the selected tower.
 * @param selectedTowerY The y-coordinate of the selected tower.
 * @param head_tower Pointer to the head of the linked list of towers.
 * @param inventory Array of pointers to Gem structures representing the player's inventory.
 * @param path_array Array of pointers to Cell structures representing the path on the grid.
 * @param path_size The total number of cells in the path.
 * @param gameState Reference to the current game state.
 * @param click_processed Reference to a boolean indicating if the click has been processed.
 * @return void
 */
void place_gem_on_tower(int mouse_x, int mouse_y, int selectedTowerX, int selectedTowerY, Tower** head_tower, Gem* inventory[], Cell** path_array, int path_size, GameState *gameState, bool *click_processed) {
    if (*gameState == SELECT_GEM) {
        for (int i = 0; i < MAX_GEM_NUMBER; i++) {
            int box_x = GEM_INVENTORY_START_X + i * GEM_BOX_WIDTH;
            int box_y = GEM_INVENTORY_START_Y;
            if (mouse_x >= box_x && mouse_x <= box_x + GEM_BOX_WIDTH &&
                mouse_y >= box_y && mouse_y <= box_y + GEM_BOX_HEIGHT && inventory[i] != NULL) {
                Tower* tower = find_tower_at(selectedTowerX, selectedTowerY, *head_tower);
                if (tower != NULL) {
                    tower->crystal = inventory[i];
                    tower->crystal->time_since_placed = 0;
                    tower->crystal->x = tower->x * SIZE_CELL + SIZE_CELL / 2;
                    tower->crystal->y = tower->y * SIZE_CELL + SIZE_CELL / 2;
                    tower->crystal->radius = tower->crystal->level == 1 ? 3 * SIZE_CELL : (3 + tower->crystal->level) * SIZE_CELL;
                    tower->crystal->path_cells_in_range = NULL;
                    tower->crystal->projectile_x = 0.0;
                    tower->crystal->projectile_y = 0.0;
                    tower->crystal->projectile_target = NULL;
                    tower->crystal->is_projectile_active = false;
                    tower->crystal->isReadyToShoot = false;
                    scan_path_cells_in_range(tower->crystal, path_array, path_size);
                    printf("number of cells :  %d \n", tower->crystal->num_path_cells_in_range);
                    inventory[i] = NULL; // Remove from inventory
                    *gameState = NONE; // Close inventory and options
                }
                *click_processed = true;
                break;
            }
        }
        if (!*click_processed && *gameState == SELECT_GEM) {
            *gameState = NONE;
            *click_processed = true;
        }
    }
}



void process_tower_placement(int mouse_x, int mouse_y, Tower** head_tower, Cell grid[][HEIGHT_GRID], int* tower_num_add, float* player_health, float* mana_cost_tower, bool* can_place_tower, GameState* gameState, bool* click_processed) {
    int gridX = mouse_x / SIZE_CELL;
    int gridY = mouse_y / SIZE_CELL;
    
    if (gridX < WIDTH_GRID && gridY < HEIGHT_GRID) {
        Tower* tower = find_tower_at(gridX, gridY, *head_tower);
        if (tower != NULL && tower->crystal == NULL) {
            *gameState = SELECT_GEM;
            *click_processed = true;
        }
    }

    // Enable tower placing
    if (mouse_x >= MANA_BOX_X && mouse_x <= MANA_BOX_X + MANA_BOX_WIDTH &&
        mouse_y >= MANA_BOX_Y && mouse_y <= MANA_BOX_Y + MANA_BOX_HEIGHT) {
        *can_place_tower = true; // Enable tower placement
    } else if (*can_place_tower && grid[mouse_x / SIZE_CELL][mouse_y / SIZE_CELL].type_c == EMPTY && *player_health > *mana_cost_tower + 1) {
        // Add tower
        (*tower_num_add)++;
        *player_health -= *mana_cost_tower;
        if (*tower_num_add > 2) {
            *mana_cost_tower = (*mana_cost_tower == 0) ? 100 : (*mana_cost_tower * 2);
        }
        grid[mouse_x / SIZE_CELL][mouse_y / SIZE_CELL].number_occupation++;
        grid[mouse_x / SIZE_CELL][mouse_y / SIZE_CELL].type_c = FULL;
        add_tower(mouse_x / SIZE_CELL, mouse_y / SIZE_CELL, *tower_num_add, head_tower);
        *can_place_tower = false;
    } else {
        *can_place_tower = false;
    }
}
    


void manage_tower_placement_interaction(int mouse_x, int mouse_y, Cell grid[][HEIGHT_GRID], int* tower_num_add, float* player_health, float* mana_cost_tower, bool* can_place_tower, Tower** head_tower) {
    // Check if the click is within the mana box to enable tower placement
    if (mouse_x >= MANA_BOX_X && mouse_x <= MANA_BOX_X + MANA_BOX_WIDTH &&
        mouse_y >= MANA_BOX_Y && mouse_y <= MANA_BOX_Y + MANA_BOX_HEIGHT) {
        *can_place_tower = true;
    }
    // Check if tower placement is enabled and the cell is empty and player has enough health to place a tower
    else if (*can_place_tower && grid[mouse_x / SIZE_CELL][mouse_y / SIZE_CELL].type_c == EMPTY && *player_health > *mana_cost_tower + 1) {
        (*tower_num_add)++;
        *player_health -= *mana_cost_tower;
        if (*tower_num_add > 2) {
            *mana_cost_tower = (*mana_cost_tower == 0) ? 100 : (*mana_cost_tower * 2);
        }
        grid[mouse_x / SIZE_CELL][mouse_y / SIZE_CELL].number_occupation++;
        grid[mouse_x / SIZE_CELL][mouse_y / SIZE_CELL].type_c = FULL;
        add_tower(mouse_x / SIZE_CELL, mouse_y / SIZE_CELL, *tower_num_add, head_tower);
        *can_place_tower = false;
    }
    else {
        *can_place_tower = false;
    }
}
