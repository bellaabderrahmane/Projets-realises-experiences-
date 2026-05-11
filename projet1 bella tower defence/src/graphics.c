
#include "../include/graphics.h"

void draw_grid(Cell grid[WIDTH_GRID][HEIGHT_GRID]) {
    int i, j;                                                         //declaring variables for the "for" loop
    for (i = 0; i < WIDTH_GRID; i++) {
        for (j = 0; j < HEIGHT_GRID; j++) {
            MLV_draw_filled_rectangle(
                grid[i][j].x* SIZE_CELL, grid[i][j].y* SIZE_CELL, 
                SIZE_CELL - LENGTH_LINE, SIZE_CELL - LENGTH_LINE,     //deducting  LENGTH_LINE from both width and height to leave a small gap
                grid[i][j].color
            );
        }
    }
    
}




 /* 
    * Draws the player's health bar (or MANA ) on the screen, representing the current health level as a percentage of the maximum health.
    * The health bar changes color based on the health status. A text indicating the current and maximum health is displayed over the health bar.
    *
    * @param current_health The current health level of the player.
    * @param max_health The maximum health level of the player.
    * @return void
    */
    void draw_player_health_bar(float current_health, float max_health) {
        float current_width = PLAYER_HEALTH_BAR_WIDTH * (current_health / max_health);
        
        MLV_draw_filled_rectangle(PLAYER_HEALTH_BAR_X, PLAYER_HEALTH_BAR_Y, PLAYER_HEALTH_BAR_WIDTH, PLAYER_HEALTH_BAR_HEIGHT, MLV_COLOR_RED);

        MLV_draw_filled_rectangle(PLAYER_HEALTH_BAR_X, PLAYER_HEALTH_BAR_Y, (int)current_width, PLAYER_HEALTH_BAR_HEIGHT, MLV_COLOR_GREEN);

        char text[50]; // Make sure this is large enough to hold the entire string
        sprintf(text, "MANA LEVEL: %.0f / %.0f", current_health, max_health); // Convert numbers to string

        // Calculate text size
        int text_width, text_height;
        MLV_get_size_of_text(text, &text_width, &text_height);

        // Calculate the position of the text to center it on the health bar
        int text_x = PLAYER_HEALTH_BAR_X + (PLAYER_HEALTH_BAR_WIDTH - text_width) / 2;
        int text_y = PLAYER_HEALTH_BAR_Y + (PLAYER_HEALTH_BAR_HEIGHT - text_height) / 2;

        // Draw the text
        MLV_draw_text(text_x, text_y, text, MLV_COLOR_WHITE);

    }



    void draw_mana_box(float mana_cost, MLV_Color color) {
        // Draw the box
        MLV_draw_filled_rectangle(MANA_BOX_X-4, MANA_BOX_Y-4, MANA_BOX_WIDTH+8,MANA_BOX_HEIGHT +8, MLV_rgba(0, 255, 255, 150)   );
        MLV_draw_filled_rectangle(MANA_BOX_X, MANA_BOX_Y, MANA_BOX_WIDTH, MANA_BOX_HEIGHT,color   );
        // Prepare and draw the text
        char text[50];
        sprintf(text, " TOWER : %.0f M", mana_cost);
        MLV_draw_text(MANA_BOX_X + 8, MANA_BOX_Y + (MANA_BOX_HEIGHT / 2) - 10, text, MLV_COLOR_BLACK);
    
        MLV_draw_filled_rectangle(840, 54, 358, 30,MLV_COLOR_BROWN   );
        sprintf(text, " SHOP : %.0f M", mana_cost);
        MLV_draw_text(1000, 63 , text, MLV_COLOR_WHITE);
    
    }


    /* 
    * Draws the mana box on the screen, displaying the mana cost for tower placement or other actions.
    * The mana box includes a colored background and text indicating the current mana cost.
    *
    * @param mana_cost The cost of the next tower or action, to be displayed in the mana box.
    * @param color The background color of the mana box.
    * @return void
    */
    void draw_time_box(int time_spent, int wave_interval, int box_x, int box_y, int box_width, int box_height) {
        // Draw the box for displaying time left for the next wave
        MLV_draw_filled_rectangle(box_x, box_y, box_width, box_height,MLV_rgba(255, 215, 0, 255));
        MLV_draw_rectangle(box_x, box_y, box_width, box_height, MLV_COLOR_WHITE);
        // Calculate time left for the next wave
        int time_left = wave_interval - (time_spent / 60); // Convert frames to seconds and calculate remaining time
        char time_text[50];
        sprintf(time_text, "NEXT WAVE IN : %d s", time_left); // Format the text
        MLV_draw_text(box_x + 8, box_y + (box_height / 2) - 7, time_text, MLV_COLOR_BLACK);
    }


    /* 
    * Draws the wave box on the screen, indicating the current wave number in the game.
    * The wave box includes a background and text displaying the current wave number.
    *
    * @param wave_number The current wave number to be displayed in the wave box.
    * @return void
    */
    void draw_wave_box(int wave_number) {
        MLV_draw_filled_rectangle(WAVE_BOX_X, WAVE_BOX_Y, WAVE_BOX_WIDTH, WAVE_BOX_HEIGHT,MLV_rgba(255, 215, 0, 255));
        MLV_draw_rectangle(WAVE_BOX_X, WAVE_BOX_Y, WAVE_BOX_WIDTH, WAVE_BOX_HEIGHT, MLV_COLOR_WHITE);
        char time_text[50];
        sprintf(time_text, "WAVE : %d LV/NUMBER",wave_number); // Format the text
        MLV_draw_text(WAVE_BOX_X + 8, WAVE_BOX_Y + (WAVE_BOX_HEIGHT / 2) - 7, time_text, MLV_COLOR_BLACK);
    }



/* 
    * Draws the user interface for gem purchase and upgrade, including buttons to adjust gem level and an information box displaying the current gem level and its cost.
    * The UI includes a minus button to decrease gem level, an info box to display current gem level and cost, and a plus button to increase gem level.
    *
    * @param gem_level_to_be_purchased The current level of the gem that can be purchased or upgraded.
    * @param mana_cost_per_gem_level The mana cost for purchasing or upgrading the gem to the specified level.
    * @return void
    */
    void draw_gem_ui(int gem_level_to_be_purchased, float mana_cost_per_gem_level) {
        // Draw minus box
        MLV_draw_filled_rectangle(minus_box_x, minus_box_y, minus_box_width, minus_box_height, MLV_COLOR_WHITE);
        MLV_draw_text(minus_box_x + 8, minus_box_y + 5, "-", MLV_COLOR_RED);
        // Draw info box (gem level and cost)
        char info_text[100];
        sprintf(info_text, "GEM LV %d: %.0f", gem_level_to_be_purchased, mana_cost_per_gem_level );
        MLV_draw_filled_rectangle(info_box_x, 0, info_box_width, info_box_height, MLV_rgba(255, 7, 58, 255));
        MLV_draw_filled_rectangle(info_box_x+4, info_box_y+1, info_box_width - 8, info_box_height-10, MLV_COLOR_WHITE);
        MLV_draw_text(info_box_x + 10, info_box_y + 10, info_text, MLV_COLOR_BLACK);
        // Draw plus box    
        MLV_draw_filled_rectangle(plus_box_x, plus_box_y, plus_box_width, plus_box_height, MLV_COLOR_WHITE);
        MLV_draw_text(plus_box_x + 5, plus_box_y + 5, "+", MLV_COLOR_RED);
    }


    /* 
    * Draws the inventory interface on the screen, representing each gem in the inventory as a box.
    * Each box displays the gem type, color, and level. The gems are visually represented within the boxes.
    *
    * @param inventory Array of pointers to Gem structures representing the player's inventory.
    * @param max_gems The maximum number of gems that can be held in the inventory.
    * @return void
    */
    void draw_gem_inventory(Gem* inventory[], int max_gems) {
        for (int i = 0; i < max_gems; i++) {
            int box_x = GEM_INVENTORY_START_X + i * GEM_BOX_WIDTH;
            int box_y = GEM_INVENTORY_START_Y;
            MLV_draw_filled_rectangle(box_x, box_y, GEM_BOX_WIDTH, GEM_BOX_HEIGHT, MLV_COLOR_WHITE);
            MLV_draw_rectangle(box_x, box_y, GEM_BOX_WIDTH, GEM_BOX_HEIGHT, MLV_COLOR_RED);
            if (inventory[i] != NULL) {
                // Draw gem as a small rectangle in the center of the box
                int rect_width = GEM_BOX_WIDTH / 4;  // Adjust the size as needed
                int rect_height = GEM_BOX_HEIGHT / 4; // Adjust the size as needed
                int rect_x = box_x + (GEM_BOX_WIDTH - rect_width) / 2;
                int rect_y = box_y + (GEM_BOX_HEIGHT - rect_height) / 2;
                //printf("teinte : %d \n",inventory[i]->teinte);
                MLV_draw_filled_rectangle(rect_x, rect_y, rect_width, rect_height, teinte_to_rgb(inventory[i]->teinte));

                // Draw the gem level below the rectangle
                char level_text[10];
                sprintf(level_text, "Lv %d", inventory[i]->level);
                MLV_draw_text(rect_x, rect_y + rect_height + 5, level_text, MLV_COLOR_BLACK);
            }
        }
    }


    /* 
    * Draws the fusion interface on the screen, representing the slots where gems can be placed for fusion.
    * The interface shows the selected gems for fusion and their levels.
    *
    * @param fusioner Array of pointers to Gem structures representing the selected gems for fusion.
    * @return void
    */
    void draw_gem_fusioner(Gem* fusioner[]) {
        for (int i = 0; i < 2; i++) {
            int box_x = GEM_INVENTORY_START_X + i * GEM_BOX_WIDTH;
            int box_y = GEM_INVENTORY_END_Y +  GEM_BOX_WIDTH ;
            MLV_draw_rectangle(box_x, box_y, GEM_BOX_WIDTH, GEM_BOX_HEIGHT, MLV_COLOR_WHITE);
            if (fusioner[i] != NULL) {
                // Draw gem as a small rectangle in the center of the box
                int rect_width = GEM_BOX_WIDTH / 4;  // Adjust the size as needed
                int rect_height = GEM_BOX_HEIGHT / 4; // Adjust the size as needed
                int rect_x = box_x + (GEM_BOX_WIDTH - rect_width) / 2;
                int rect_y = box_y + (GEM_BOX_HEIGHT - rect_height) / 2;
                //printf("teinte : %d \n",inventory[i]->teinte);
                MLV_draw_filled_rectangle(rect_x, rect_y, rect_width, rect_height, teinte_to_rgb(fusioner[i]->teinte));

                // Draw the gem level below the rectangle
                char level_text[10];
                sprintf(level_text, "Lv %d", fusioner[i]->level);
                MLV_draw_text(rect_x, rect_y + rect_height + 5, level_text, MLV_COLOR_WHITE);
            }
        }
    }


/* 
 * Highlights the inventory slots where gems are placed, providing a visual indication of occupied slots.
 *
 * @param inventory Array of pointers to Gem structures representing the player's inventory.
 * @return void
 */
void draw_inventory_highlights(Gem* inventory[]) {
    int box_x ;
    int box_y ;
    for (int i = 0; i < MAX_GEM_NUMBER; i++) {
        if (inventory[i] != NULL) {
            int box_x = GEM_INVENTORY_START_X + i * GEM_BOX_WIDTH;
            int box_y = GEM_INVENTORY_START_Y;
            MLV_draw_filled_rectangle(box_x, box_y, GEM_BOX_WIDTH, GEM_BOX_HEIGHT, (250, 0, 0, 200));
        }
    }
}

