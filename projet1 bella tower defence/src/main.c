#include "../include/main.h"



int main() {

        float player_health = 150.0;  
        float player_max_health = 2000.0; 
        float player_health_level = 0.0 ;
        int path_size ;
        int mouse_x, mouse_y;
        int tower_num_add = 0 ;
        bool click_processed = false;
        int time_spent = 0;                                                             
        int wave_number = 0 ;
        bool newWaveCreated = false;
        bool launch_wave = false;
        float mana_cost_tower = 0.0;
        bool can_place_tower = false;
        int gem_level_to_be_purchased = 1;
        float mana_cost_per_gem_level = 200.0; 
        int gem_array_length = 0 ;
        GameState gameState = NONE;
        int selectedTowerX = -1;
        int selectedTowerY = -1;
        bool sconds_gem = false ;
        int time_gem = 0 ;
        bool fusener = false ;
        int mouse_trace_x,mouse_trace_y ;


        srand(time(NULL));
        Tower* head_tower = NULL;
        MLV_Mouse_button mouse_button;
        MLV_Event event;
        MLV_Keyboard_button key;
        Cell grid[WIDTH_GRID][HEIGHT_GRID];
        Gem* inventory[MAX_GEM_NUMBER] = {NULL};
        Gem* fusioner[2] = {NULL};
        struct timespec start_time, end_time;
        double frametime, extra_time;
        WaveNode* head = NULL;
        Wave wave ;                                                                    
        

                            
        initialise_grid(grid);  
        Cell **path_array = path_generate(grid,&path_size);                                              
        MLV_create_window("Tower Defense Game", "TDG", WIDTH_WINDOW, HEIGHT_WINDOW); // we create the window with the correponsing size
    


        while (player_health > 0 && true ) {
            MLV_get_mouse_position(&mouse_trace_x, &mouse_trace_y); 
            MLV_clear_window(MLV_COLOR_BLACK);
            clock_gettime(CLOCK_REALTIME, &start_time);
            draw_grid(grid);
            if (!can_place_tower){
                draw_mana_box(mana_cost_tower,MLV_COLOR_WHITE);
            }else{
                draw_mana_box(mana_cost_tower,MLV_rgba(250, 100, 0, 200));
                MLV_draw_filled_circle( mouse_trace_x  , mouse_trace_y , 3*SIZE_CELL, MLV_rgba(0, 250, 0, 150));
            }
            draw_time_box(time_spent, WAVE_INTERVAL, TIME_BOX_X, TIME_BOX_Y, TIME_BOX_WIDTH, TIME_BOX_HEIGHT);
            draw_gem_ui(gem_level_to_be_purchased, mana_cost_per_gem_level);
            draw_gem_inventory(inventory, MAX_GEM_NUMBER);
            draw_gem_fusioner(fusioner);
            if (gameState == SELECT_GEM) {
                draw_inventory_highlights(inventory);
            }            
            event = MLV_get_event(&key, NULL, NULL, NULL, NULL, &mouse_x, &mouse_y, &mouse_button, NULL);
            if (event == MLV_MOUSE_BUTTON && mouse_button == MLV_BUTTON_LEFT) {
                if (MLV_get_mouse_button_state(MLV_BUTTON_LEFT) == MLV_PRESSED) {
                    click_processed = false;  
                    place_gem_on_tower(mouse_x, mouse_y, selectedTowerX, selectedTowerY, &head_tower, inventory, path_array, path_size, &gameState, &click_processed);
                    if (!click_processed) {
                        int gridX = mouse_x / SIZE_CELL;
                        int gridY = mouse_y / SIZE_CELL;
                        if (gridX < WIDTH_GRID && gridY < HEIGHT_GRID ) {
                            Tower* tower = find_tower_at(gridX, gridY, head_tower);
                            if (tower != NULL && tower->crystal == NULL) {
                                selectedTowerX = gridX;
                                selectedTowerY = gridY;
                                gameState = SELECT_GEM;
                                click_processed = true;
                            }
                        }
                        //enable tower placing
                        manage_tower_placement_interaction(mouse_x, mouse_y, grid, &tower_num_add, &player_health, &mana_cost_tower, &can_place_tower, &head_tower);
                        manage_gem_interaction(mouse_x, mouse_y, &gem_level_to_be_purchased, &mana_cost_per_gem_level, &player_health, inventory);                        
                    }
                }  
            }
            if (event == MLV_MOUSE_BUTTON && mouse_button == MLV_BUTTON_RIGHT) {
                if (MLV_get_mouse_button_state(MLV_BUTTON_RIGHT) == MLV_PRESSED) {
                    move_gem_to_fusioner(mouse_x, mouse_y, inventory, fusioner, &click_processed);
                }
            }
            if (mouse_button == MLV_BUTTON_RIGHT) {
                if (MLV_get_mouse_button_state(MLV_BUTTON_RIGHT) == MLV_PRESSED) {
                    move_gem_from_fusioner_to_inventory(mouse_x, mouse_y, inventory, fusioner, &click_processed);
                }
            }
            if (event == MLV_MOUSE_BUTTON && mouse_button == MLV_BUTTON_LEFT) {
                if (MLV_get_mouse_button_state(MLV_BUTTON_LEFT) == MLV_PRESSED) {
                    // Check if the click is within the fuse box
                    if (mouse_x >= FUSE_BOX_X && mouse_x <= FUSE_BOX_X + FUSE_BOX_WIDTH &&
                        mouse_y >= FUSE_BOX_Y && mouse_y <= FUSE_BOX_Y + FUSE_BOX_HEIGHT &&
                        fusioner[0] != NULL && fusioner[1] != NULL && player_health > 100) {
                        if (fusioner[0]->level == fusioner[1]->level) {
                            // Trigger the fusion process
                            fuse_gems(fusioner, inventory, &player_health);
                        } else {
                                // If fusion is not possible due to some condition
                                handle_failed_fusion(fusioner, inventory);
                        }
                    }
                }
            }


            
            if(fusioner[0] != NULL && fusioner[1] != NULL){
                if(fusioner[0]->level == fusioner[1]->level  ){
                    fusener = true ;
                }else if(fusioner[0]->level != fusioner[1]->level){
                    fusener = false ;
                }
                if (fusioner[0] != NULL && fusioner[1] != NULL && fusener  ){
                    MLV_draw_filled_rectangle(FUSE_BOX_X, FUSE_BOX_Y, FUSE_BOX_WIDTH, FUSE_BOX_HEIGHT, MLV_COLOR_WHITE);
                    MLV_draw_text(FUSE_BOX_X + 5, FUSE_BOX_Y + 35, "Fuse:100 M", MLV_COLOR_BLACK);
                }
            }
            
    
            if (event == MLV_KEY && key == MLV_KEYBOARD_SPACE && MLV_get_keyboard_state(key) == MLV_RELEASED) {
                launch_wave = true;
                //need_redraw = true;
            }
            if ( (player_health  - (500 * pow(1.4,player_health_level) ) )>= 1 && event == MLV_KEY && key == MLV_KEYBOARD_UP && MLV_get_keyboard_state(key) == MLV_RELEASED) {
                player_health -= (500 * pow(1.4,player_health_level) ) ;
                player_health_level++;
                player_max_health = (2000 * pow(1.4,player_health_level) ); 
            }
            
            WaveNode* currentWaveNode = head;
            while (currentWaveNode != NULL) {
                update_wave(&head,currentWaveNode->newWave, path_array, path_size, &player_health,player_health_level,frametime,player_max_health);
                currentWaveNode = currentWaveNode->next;
            }
            update_towers_and_projectiles(head_tower, &head, path_array, path_size,&player_health,player_health_level,player_max_health, frametime);
            remove_empty_waves(&head);
            Tower* new_tower = head_tower;
            while (new_tower != NULL) {
                MLV_draw_filled_rectangle((new_tower->x) * SIZE_CELL , (new_tower->y) * SIZE_CELL , SIZE_CELL, SIZE_CELL, MLV_COLOR_BLUE);
                if (new_tower->crystal != NULL) {
                    MLV_draw_filled_circle(new_tower->crystal->x,new_tower->crystal->y, 10, teinte_to_rgb(new_tower->crystal->teinte));
                    MLV_draw_circle(new_tower->crystal->x,new_tower->crystal->y, new_tower->crystal->radius, MLV_COLOR_RED);
                    if (new_tower->crystal != NULL && new_tower->crystal->is_projectile_active) {
                        MLV_draw_filled_circle(new_tower->crystal->projectile_x, new_tower->crystal->projectile_y, 2, MLV_COLOR_WHITE);
                    }
                }
                new_tower = new_tower->next;
            }
            draw_player_health_bar(player_health, player_max_health     );
            draw_wave_box( wave_number);
            MLV_actualise_window();
            clock_gettime(CLOCK_REALTIME, &end_time);
            frametime = end_time.tv_sec - start_time.tv_sec;
            frametime += (end_time.tv_nsec - start_time.tv_nsec) / 1.0E9;
            extra_time = 1.0 / 60 - frametime;
            if (extra_time > 0) {
                MLV_wait_milliseconds((int)(extra_time * 1000));
            }
            time_gem++;
            if ( time_gem == 2*60 ) {
                sconds_gem = true ;
                time_gem = 0;
            }
            if ( (time_spent >= (WAVE_INTERVAL * 60) || launch_wave ) ) {
                if (!path_array[1]->is_occupied){
                    Wave* new_wave = (Wave*)malloc(sizeof(Wave));
                    if (new_wave == NULL) {
                        fprintf(stderr, "Error allocating memory for new wave\n");
                        exit(EXIT_FAILURE);
                    }
                    wave_number++;
                    int wave_successfully_launched = initialize_wave(new_wave, path_array, &head, wave_number);
                    if(wave_successfully_launched == 0){
                        wave_number--;
                    }else if(launch_wave){
                        int time_left = WAVE_INTERVAL - (time_spent / 60); 
                        player_health += ( (time_left * player_max_health) /100)  ;
                    }
                    time_spent = 0;
                    launch_wave = false ;
                }
                
            }
            time_spent++; 
        }
        MLV_wait_mouse(&mouse_x, &mouse_y);                                           // detecting a user's mouse click(closing the window subsequently)
        MLV_free_window();                                                // freeing window from memory after exit
    
        return 0;
    }
