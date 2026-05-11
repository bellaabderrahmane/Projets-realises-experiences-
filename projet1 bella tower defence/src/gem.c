  
#include "../include/gem.h"






void initialise_gem(Gem * gem){
        GemType selected_type;
        int random_value = rand() % 3; // Random value between 0 and 2
        switch (random_value) {
            case 0:
                selected_type = PURE_RED;
                break;
            case 1:
                selected_type = PURE_GREEN;
                break;
            case 2:
                selected_type = PURE_BLUE;
                break;
        }

        // Assign teinte based on the selected type
        int teinte;
        if (selected_type == PURE_RED) {
            // Random teinte between 0-30 or 330-359
            if (rand() % 2 == 0) {
                teinte = rand() % 31; // 0-30
            } else {
                teinte = 330 + rand() % 30; // 330-359
            }
        } else if (selected_type == PURE_GREEN) {
            // Random teinte between 90-150
            teinte = 90 + rand() % 61; // 90-150
        } else if (selected_type == PURE_BLUE) {
            // Random teinte between 210-270
            teinte = 210 + rand() % 61; // 210-270
        }
        gem->type =  selected_type ;
        gem->teinte = teinte;

}







void manage_gem_interaction(int mouse_x, int mouse_y, int* gem_level_to_be_purchased, float* mana_cost_per_gem_level, float* player_health, Gem* inventory[]) {
    // Decrease gem level and mana cost if minus box is clicked
    if (mouse_x >= minus_box_x && mouse_x <= minus_box_x + minus_box_width && 
        mouse_y >= minus_box_y && mouse_y <= minus_box_y + minus_box_height) {
        if (*gem_level_to_be_purchased > 1 && *mana_cost_per_gem_level > 0) {
            (*gem_level_to_be_purchased)--;
            *mana_cost_per_gem_level -= 200;                                
        }
    }
    // Increase gem level and mana cost if plus box is clicked
    else if (mouse_x >= plus_box_x && mouse_x <= plus_box_x + plus_box_width && 
             mouse_y >= plus_box_y && mouse_y <= plus_box_y + plus_box_height) {
        (*gem_level_to_be_purchased)++;
        *mana_cost_per_gem_level += 200;
    }
    // Buy gem if info box is clicked and there's enough health (mana)
    else if (mouse_x >= info_box_x && mouse_x <= info_box_x + info_box_width && 
             mouse_y >= info_box_y && mouse_y <= info_box_y + info_box_height) {
        for (int i = 0; i < MAX_GEM_NUMBER; i++) {
            if (inventory[i] == NULL && *player_health > *mana_cost_per_gem_level) {
                inventory[i] = (Gem*)malloc(sizeof(Gem));
                inventory[i]->level = *gem_level_to_be_purchased;
                inventory[i]->radius = (inventory[i]->level == 1) ? 3 * 30 : (3 + inventory[i]->level) * 30;
                inventory[i]->damage = 20;
                initialise_gem(inventory[i]);
                *player_health -= *mana_cost_per_gem_level;
                break;
            }
        }
    }
}




void move_gem_to_fusioner(int mouse_x, int mouse_y, Gem* inventory[], Gem* fusioner[], bool* click_processed) {
    if ((mouse_x >= GEM_INVENTORY_START_X &&  mouse_x <= GEM_INVENTORY_END_X) && 
        (mouse_y >= GEM_INVENTORY_START_Y && mouse_y <= GEM_INVENTORY_END_Y)) {
        for (int i = 0; i < MAX_GEM_NUMBER; i++) {
            int box_x = GEM_INVENTORY_START_X + i * GEM_BOX_WIDTH;
            int box_y = GEM_INVENTORY_START_Y;
            if (mouse_x >= box_x && mouse_x <= box_x + GEM_BOX_WIDTH &&
                mouse_y >= box_y && mouse_y <= box_y + GEM_BOX_HEIGHT && inventory[i] != NULL) {
                for (int j = 0; j < 2; j++) {
                    if (fusioner[j] == NULL) {
                        fusioner[j] = inventory[i];
                        inventory[i] = NULL; // Remove from inventory
                        *click_processed = true; // Mark as processed
                        break; // Break after moving gem to fusioner
                    }
                }
                break; // Break after processing the inventory slot
            }
        }
    }
}


void move_gem_from_fusioner_to_inventory(int mouse_x, int mouse_y, Gem* inventory[], Gem* fusioner[], bool* click_processed) {
    // Check if the click is within the fusioner area
    if ((mouse_x >= GEM_INVENTORY_START_X && mouse_x <= GEM_INVENTORY_START_X + 2 * GEM_BOX_WIDTH) &&
        (mouse_y >= GEM_INVENTORY_START_Y + GEM_BOX_HEIGHT + 20 && mouse_y <= GEM_INVENTORY_START_Y + GEM_BOX_HEIGHT * 3)) {
        
        // Iterate over the fusioner slots
        for (int j = 0; j < 2; j++) {
            int box_x = GEM_INVENTORY_START_X + j * GEM_BOX_WIDTH;
            int box_y = GEM_INVENTORY_START_Y + GEM_BOX_HEIGHT + 20;
            
            // Check if the gem in the fusioner was clicked
            if (mouse_x >= box_x && mouse_x <= box_x + GEM_BOX_WIDTH &&
                mouse_y >= box_y && mouse_y <= box_y + GEM_BOX_HEIGHT && fusioner[j] != NULL) {
                
                // Find the first empty slot in the inventory
                for (int i = 0; i < MAX_GEM_NUMBER; i++) {
                    if (inventory[i] == NULL) {
                        // Move the gem from the fusioner to the inventory
                        inventory[i] = fusioner[j];
                        fusioner[j] = NULL; // Remove from fusioner
                        *click_processed = true; // Mark as processed
                        break; // Break after moving gem to inventory
                    }
                }
                
                break; // Break after processing the fusioner slot
            }
        }
    }
}





void fuse_gems(Gem* fusioner[], Gem* inventory[], float* player_health) {
    // Deduct mana cost for fusion
    *player_health -= 100;

    // Allocate memory for the new gem
    Gem* new_gem = (Gem*)malloc(sizeof(Gem));
    if (new_gem == NULL) {
        fprintf(stderr, "Error allocating memory for new gem\n");
        exit(EXIT_FAILURE);
    }

    // Set properties of the new gem
    new_gem->level = fusioner[0]->level + 1;
    new_gem->teinte = (fusioner[0]->teinte + fusioner[1]->teinte) / 2;  // Midpoint of teintes
    if (fusioner[0]->type == fusioner[1]->type &&
        (fusioner[0]->type == PURE_RED || fusioner[0]->type == PURE_GREEN || fusioner[0]->type == PURE_BLUE)) {
        new_gem->type = fusioner[0]->type;
    } else {
        new_gem->type = MIXED;
    }

    // Place the new gem in the first available slot in the inventory
    bool gemPlaced = false;
    for (int i = 0; i < MAX_GEM_NUMBER; i++) {
        if (inventory[i] == NULL) {
            inventory[i] = new_gem;
            gemPlaced = true;
            break;
        }
    }

    // If the gem couldn't be placed, free its memory
    if (!gemPlaced) {
        free(new_gem);
    }

    // Free the gems used for fusion and reset the fusioner slots
    free(fusioner[0]);
    free(fusioner[1]);
    fusioner[0] = NULL;
    fusioner[1] = NULL;
}




void handle_failed_fusion(Gem* fusioner[], Gem* inventory[]) {
    int emptySlots[2] = {-1, -1};  // To keep track of empty slots in the inventory
    int emptySlotCount = 0;

    // Check for empty slots in the inventory
    for (int i = 0; i < MAX_GEM_NUMBER && emptySlotCount < 2; i++) {
        if (inventory[i] == NULL) {
            emptySlots[emptySlotCount++] = i;
        }
    }

    // If there are two empty slots, return both gems to the inventory
    if (emptySlotCount == 2) {
        inventory[emptySlots[0]] = fusioner[0];
        inventory[emptySlots[1]] = fusioner[1];
    } else if (emptySlotCount == 1) {
        // Only one empty slot, return the first gem and destroy the second
        inventory[emptySlots[0]] = fusioner[0];
        free(fusioner[1]);  // Destroy the second gem
    } else {
        // No empty slots, destroy both gems
        free(fusioner[0]);
        free(fusioner[1]);
    }

    // Clear the fusioner slots
    fusioner[0] = NULL;
    fusioner[1] = NULL;
}
