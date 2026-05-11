
#include "../include/wave.h"
   

    /*
    this function works as a placeholder , it generates a random color by manipulating red ,green ,values depending on wave number
    the goal of this function is to distinguish between different waves for testing and debugging purposes
    */
    MLV_Color color_generate(int number) {
        number = number * 6000 ;  //multiplied by a big number to ùake noticible changes
        int red = (number & 0xFF0000) >> 16; // Extract red bits 16-23
        int green = (number & 0x00FF00) >> 8; // Extract green bits 8-15
        int blue = (number & 0x0000FF); // Extract blue bits 0-7
        return MLV_rgba(red, green, blue, 255); // 255 for full opacity
    }

    


    /*
    this function creates a node that holds the initialised wave then add it to WaveNode list so it would get updated later on 
    */
    void addWaveNode(WaveNode** head, Wave * newWave) {
        WaveNode* newNode = (WaveNode*)malloc(sizeof(WaveNode));  //we create new WaveNode node
        newNode->newWave = newWave;  //attribute to it our wave
        newNode->next = NULL;
        if (*head == NULL) {
            *head = newNode;  //if list is empty then the node becomes the first wave
        } else {
            WaveNode* last = *head;
            while (last->next != NULL) {
                last = last->next;
            }
            last->next = newNode; //if not , we add our wave at the end of the list
        }
    }


    /*
    this function measures the length of linked list that holds the waves , it main function is for testing and debugging
    */
    int length_list(WaveNode* head) {
        int i = 0 ;
        WaveNode* current = head;
        while (current != NULL) {
            i++ ;
            current = current->next;
        }
        return i ;
    }


    /*
    function used to free the linked list to avoid memory leaks
    */
    void free_waves(WaveNode** head) {
        WaveNode* current = *head;
        while (current != NULL) {
            WaveNode* next = current->next;
            free(current->newWave);
            free(current);
            current = next;
        }
        *head = NULL; // Reset the head to NULL
    }

//--------------------------------------------------------------------------------------------------------------------------------------------------------
    /*
    this function randomly selects a type of waves according to each's chance of occuring
    before 5 waves {%25 for agile    ,%25 for foule    ,%50 for normale}
    after 5 waves {%20 for agile    ,%20 for foule    ,%50 for normale   ,%10 for boss}
    the reason for the difference is that boss waves only occur starting from wave 5 
    */
    waves random_wave_selection(int  wave_number) {
        int random_number;
        waves selected_wave;
    random_number = rand() % 100;            // Random number between 0 and 99
    if (wave_number < 5) {                   // Exclude BOSS wave for the first 5 selections
            if (random_number < 50) {        // 0-49: 50% chance
                selected_wave = NORMAL;
            } else if (random_number < 75) { // 50-69: 20% chance
                selected_wave = FOULE;
            } else { // 70-99: 30% chance
                selected_wave = AGILE;
            }
        } else {                            // Include BOSS wave after 5 selections
            if (random_number < 50) {       // 0-49: 50% chance
                selected_wave = NORMAL;
            } else if (random_number < 70) { // 50-69: 20% chance
                selected_wave = FOULE;
            } else if (random_number < 90) { // 70-89: 20% chance
                selected_wave = AGILE;
        } else {                            // 90-99: 10% chance
                selected_wave = BOSS;
            }
        }
        return selected_wave;
    }


    /*
    this function initialises a wave , it first chooses a random type for it , then attribute to it and its monsters 
    the value accordingly
    if the first cell of path is occupied then the wave won't get initialised
    */
    int initialize_wave(Wave *wave, Cell **path,WaveNode** head,int wave_number) {
        if (path[0]->is_occupied) {   //if first cell of path is occupied then avoid initialising the wave
            return 0;
        } else {
        int monsters_number  ;      
        float health_at_wave  ;  
        float speed = 1.0 ;        
        addWaveNode(head, wave) ;
        waves wave_type = random_wave_selection(wave_number);
        switch(wave_type){   //according to wave type , add the values accordingly
            case NORMAL :
                monsters_number =  12 ;
                health_at_wave = INITIAL_HP_CONSTANT * pow(1.2,wave_number) ;
                break;
            case AGILE : 
                monsters_number =  12 ;
                health_at_wave = INITIAL_HP_CONSTANT * pow(1.2,wave_number) ;
                speed = 2 ;
                break;
            case FOULE :
                monsters_number =  24 ;
                health_at_wave = INITIAL_HP_CONSTANT * pow(1.2,wave_number) ;
                break;
            case BOSS :
                monsters_number =  2 ;
                health_at_wave = 12 * INITIAL_HP_CONSTANT * pow(1.2,wave_number) ;
                break;
            default:
                monsters_number =  12 ;
                health_at_wave = INITIAL_HP_CONSTANT * pow(1.2,wave_number) ;
        }
        wave->numMonsters = monsters_number;  
        wave->monsters = (Monster**)malloc(monsters_number * sizeof(Monster*));   //dynamically allocate monster's table depending on monsters_number
        printf("speed2 %lf\n",speed);
        for (int i = 0; i < monsters_number; i++) {
            wave->monsters[i] = (Monster*)malloc(sizeof(Monster)); // Allocate each monster
            inialise_monster(wave->monsters[i], path, health_at_wave,speed,wave_type); // Initialize each monster
        }
        wave->isActive = true;
        wave->wave_number = wave_number;  
        wave->wave_initial_health  =    health_at_wave ;
        wave->monsters[0]->hp = health_at_wave ;                                 //set initial health for monsters in the wave
        wave->monsters[0]->x = path[0]->x * SIZE_CELL + SIZE_CELL / 2.0;
        wave->monsters[0]->y = path[0]->y * SIZE_CELL + SIZE_CELL / 2.0;
        wave->monsters[0]->currentPathIndex = 0;        
        wave->monsters[0]->isActive = true ;
        wave->monsters[0]->speed_factor = speed;
        path[0]->is_occupied = true ;                                            // if a monster is present in path[0] cell then set to occupied
        path[0]->number_occupation = monsters_number ;                           //set how many monsters are currently in first cell of path
        MLV_draw_filled_circle((int)wave->monsters[0]->x, (int)wave->monsters[0]->y, MONSTER_RADIUS, MLV_COLOR_PURPLE);
        printf("speed %lf\n",speed);
        for (int i = 1; i < monsters_number; i++) {
            inialise_monster(wave->monsters[i],path,health_at_wave,speed,wave_type);            //initialise each individual monster
        }   
        }
        return 1;
    }




/* 
* Draws a health bar for a monster, indicating its current health as a percentage of its maximum health.
* The health bar is displayed above the monster. The bar's width is proportional to the monster's current health.
*
* @param monster Pointer to the Monster structure whose health bar is to be drawn.
* @param max_health The maximum health of the monster, used to determine the health bar's filled percentage.
* @return void
*/
void draw_health_bar(Monster *monster, float max_health) {
    float current_width = HEALTH_BAR_WIDTH * (monster->hp / max_health);
    int bar_x = (int)(monster->x - HEALTH_BAR_WIDTH / 2);
    int bar_y = (int)(monster->y - MONSTER_RADIUS - HEALTH_BAR_HEIGHT );  // 3 pixels above the monster
    MLV_draw_filled_rectangle(bar_x, bar_y, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT, MLV_COLOR_RED);
    MLV_draw_filled_rectangle(bar_x, bar_y, (int)current_width, HEALTH_BAR_HEIGHT, MLV_COLOR_GREEN);
}

/* 
 * Adjusts the speed of a monster based on the distance to the next monster in the path.
 * If the next monster is close, the function slows down the current monster to prevent overlapping.
 * The function iterates through all waves and monsters in the path to find the next active monster.
 *
 * @param monster1 Pointer to the current Monster whose speed may be adjusted.
 * @param path_size The total number of cells in the path.
 * @param head Pointer to the head of the linked list of WaveNodes representing all waves in the game.
 * @return void
 */
void distanceToNextMonsterInPath(Monster* monster1, int path_size, WaveNode * head) {
    if(monster1->isUnderSlowEffect){
        return;
    }
    bool found_one_monster = false ;
    WaveNode* currentWaveNode = head;
        while (currentWaveNode != NULL) {        
            for (int i = 0; i < currentWaveNode->newWave->numMonsters; i++) {
            Monster* monster2 = currentWaveNode->newWave->monsters[i];
                if (monster2 != NULL && monster2->isActive && monster2->currentPathIndex == monster1->currentPathIndex + 1) {
                    if(monster2->speed_factor != 1 && monster2->speed_factor == 1.5){
                        monster1->speed_factor = 1.5 ;
                    }else if(monster2->speed_factor == 1){
                            monster1->speed_factor = 1 ;
                    }else if(monster2->speed_factor == 2){
                            monster1->speed_factor = 2 ;
                    }
                    found_one_monster = true ;
                }
            }
            currentWaveNode = currentWaveNode->next;
        }
        if(!found_one_monster) {
            if(monster1->type_of_monster==AGILE){
                            monster1->speed_factor = 2 ;
                        }else{
                            monster1->speed_factor = 1 ;
                        }
            }
}



    /*
    this function is called repreatedly to update a wave and it's monsters positions.
    ,each time a monster enters the last cell in path the player's health (or mana) gets decremmented
    */
    void update_wave(WaveNode ** head ,Wave *wave, Cell **path, int path_size,float * player_hp,float player_health_level,double frametime,float maximum_health) {
        if (!wave->isActive  ) return;
        int old_path_index ;
        for (int i = 0; i < wave->numMonsters; i++) { //update positions for every monster
            if (wave->monsters[i] != NULL && wave->monsters[i]->hp <= 0) {
                free(wave->monsters[i]);  // Free memory for the dead monster
                wave->monsters[i] = NULL;  // Set pointer to NULL
            }
            if (wave->monsters[i] != NULL && wave->monsters[i]->isActive){
            updateMonsterEffects(wave->monsters[i], frametime);
            distanceToNextMonsterInPath(wave->monsters[i], path_size, *head);
            draw_health_bar(wave->monsters[i], wave->wave_initial_health);                                       
            if (wave->monsters[i]->currentPathIndex ==  path_size  && !path[0]->is_occupied){   //if a monster reached the last cell in path and the first cell in path is not occupied
                path[0]->is_occupied = true ; // set the first cell to occupied since we're gonna move there
                path[0]->number_occupation++; //increment the number of monsters in the first cell of path
                wave->monsters[i]->currentPathIndex = 0 ;  //reset the current index to 0
                wave->monsters[i]->x = path[0]->x * SIZE_CELL + SIZE_CELL / 2.0;  //change coordinates to that of path[0]
                wave->monsters[i]->y = path[0]->y * SIZE_CELL + SIZE_CELL / 2.0;  
            }
            else if (wave->monsters[i]->isActive && wave->monsters[i]->currentPathIndex !=  path_size  ) {  //if monster is active and is not in last cell update its position
                old_path_index = wave->monsters[i]->currentPathIndex ; //store old index to use it for next monster's position
                //path[old_path_index]->is_occupied = true ; 
                move_monster(path ,wave->monsters[i],wave->wave_number);
            }

            if (wave->monsters[i]->currentPathIndex >=2 && wave->monsters[i]->currentPathIndex <= path_size - 1){
                if ( abs(wave->monsters[i]->x - (path[old_path_index]->x * SIZE_CELL + SIZE_CELL / 2.0) ) > 1 || abs(wave->monsters[i]->y - (path[old_path_index]->y * SIZE_CELL + SIZE_CELL / 2.0) ) > 1){
                    path[old_path_index]->is_occupied = false ;
                    path[old_path_index]->number_occupation = 0 ;
                    path[wave->monsters[i]->currentPathIndex]->is_occupied = true ;
                }else{
                    path[old_path_index]->is_occupied = true ;
                    path[old_path_index]->number_occupation = wave->wave_number;
                }
            }
            if ( wave->monsters[i]->currentPathIndex != path_size +1 && wave->monsters[i]->currentPathIndex > old_path_index    ) { //if monster changed index then activate the next monster
                if (i + 1 < wave->numMonsters && wave->monsters[i + 1] != NULL) {
                    wave->monsters[i + 1]->isActive = true;
                }
            }
            if (old_path_index == path_size -1 && wave->monsters[i]->currentPathIndex  == path_size){ //if monster entered last cell in path we decrement mana value once
                *player_hp -=    (wave->wave_initial_health * 0.15)* pow(1.3,player_health_level)   ;
                if (*player_hp < 0) *player_hp = 0;
            }
            if (old_path_index == 0 && wave->monsters[i]->currentPathIndex  == 1){ //if monster was in first cell of path then moved to next , we decrease number of monsters in path[0]
                path[0]->number_occupation--;
            }
            if(path[0]->number_occupation == 0){ // if paht[0] has no monsters in it then set its occupied state to false (available)
                path[0]->is_occupied = false ;
            }
        }
        }
    }


    /* 
 * Removes empty waves from the linked list of waves, freeing memory and keeping the wave list current.
 * The function iterates through the wave list and removes any waves where all monsters have been cleared.
 *
 * @param head Pointer to the pointer to the head of the linked list of WaveNodes representing all waves in the game.
 * @return void
 */
void remove_empty_waves(WaveNode** head) {
    WaveNode *current = *head;
    WaveNode *prev = NULL;
    while (current != NULL) {
        bool allMonstersGone = true;
        for (int i = 0; i < current->newWave->numMonsters; i++) {
            if (current->newWave->monsters[i] != NULL) {
                allMonstersGone = false;
                break;
            }
        }
        if (allMonstersGone) {
            // Remove the wave from the linked list
            if (prev == NULL) {
                *head = current->next;
            } else {
                prev->next = current->next;
            }
            WaveNode *temp = current;
            current = current->next;
            free(temp->newWave->monsters); // Free the monsters array
            free(temp->newWave); // Free the wave itself
            free(temp); // Free the node
        } else {
            prev = current;
            current = current->next;
        }
    }
}



