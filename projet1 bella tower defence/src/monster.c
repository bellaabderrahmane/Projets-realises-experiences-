#include "../include/monster.h"

 void move_monster(Cell **path_array ,Monster *monster,int wave_number){
        float targetX = path_array[monster->currentPathIndex + 1]->x * SIZE_CELL + SIZE_CELL / 2.0;  //calculate the x coordinate of next cell's center
        float targetY = path_array[monster->currentPathIndex + 1]->y * SIZE_CELL + SIZE_CELL / 2.0;
        float dx = targetX - monster->x;  //we calculate the difference between the current monster's x position and targetx
        float dy = targetY - monster->y;
        float distance = sqrt(dx * dx + dy * dy); //we calculate euclidien distance between the monster and the center of the next cell
        if (distance > 0) { 
            dx /= distance;   //we covert dx to unit vector (to point to the direction of the targetx)
            dy /= distance;
        }
        if (abs(monster->x - targetX) < 1 && abs(monster->y - targetY) < 1) {   //checks if monster has reached center of next cell
            monster->x = targetX;
            monster->y = targetY;            //update the monster's position
            monster->currentPathIndex += 1;  //increment its path index to the new cell
        } else {
            // Continue moving towards the next cell
            if(monster->isUnderSlowEffect || monster->speed_factor ==1.5){
                monster->x += (dx / monster->speed_factor);
                monster->y += (dy / monster->speed_factor);
            }else{
                monster->x += (dx * monster->speed_factor);
                monster->y += (dy * monster->speed_factor);
                printf("speed: %lf\n",monster->speed_factor);
            }
            
        }
        MLV_draw_filled_circle((int)monster->x, (int)monster->y, MONSTER_RADIUS,teinte_to_rgb(monster->teinte)); //draw a circle represent a monster with a unique color to indicate wave
    }


    /*
    this function initialises a monster  and sets initial values for health , position and active status
    it also sets the first cell of path to occupied (used later for avoiding collisions)
    */
    void inialise_monster(Monster * monster,Cell **path_array,int health ,float speed,waves type_mons){
        monster->hp = health ;  //set monster's health to initial value according to wave levem
        monster->x = path_array[0]->x * SIZE_CELL + SIZE_CELL / 2.0; //set its position to the center of the first cell in path
        monster->y = path_array[0]->y * SIZE_CELL + SIZE_CELL / 2.0;
        monster->currentPathIndex = 0;  //set its index to path index of current cell
        monster->isActive = false ;  //set its active status to off (can't move )
        monster->teinte = rand() % 360;
        monster->speed_factor = speed ;
        monster->isUnderParasiteEffect = false;
        monster->parasiteEffectTimer = 0;
        monster->parasiteDamageTimer = 0;
        monster->isUnderSlowEffect = false;
        monster->type_of_monster = type_mons;
        monster->slowEffectTimer = 0;
        monster->speed_factor = speed;
        MLV_draw_filled_circle((int)monster->x, (int)monster->y, MONSTER_RADIUS, teinte_to_rgb(monster->teinte));
    }

/*
    * Converts a hue value into an RGB color.
    * The function takes a hue value (teinte) as an input and converts it into an MLV_Color type.
    * The hue value should be between 0 and 359, representing the color's angle on the color wheel.
    * The saturation and value (brightness) are fixed at 1.0, making the color fully saturated and bright.
    * It uses the HSV to RGB conversion formula to compute the red, green, and blue components.
    * After calculating these components, it converts them into the range of 0 to 255 (typical for RGB colors).
    * Finally, it returns the color using the MLV_rgba function, which constructs a color from red, green, blue, and alpha (transparency) components.
    * The alpha component is set to 255, making the color fully opaque.
    *
    * @param teinte The hue of the color, an integer between 0 and 359.
    * @return An MLV_Color object representing the color in the RGB color space with full opacity.
    */
    MLV_Color teinte_to_rgb(int teinte) {
        float hue = teinte / 360.0; 
        float saturation = 1.0;
        float value = 1.0;
        float c = value * saturation;
        float x = c * (1 - fabs(fmod(hue * 6, 2) - 1));
        float m = value - c;
        float r, g, b;
        if (hue >= 0 && hue < 1.0/6) {
            r = c, g = x, b = 0;
        } else if (hue >= 1.0/6 && hue < 1.0/3) {
            r = x, g = c, b = 0;
        } else if (hue >= 1.0/3 && hue < 0.5) {
            r = 0, g = c, b = x;
        } else if (hue >= 0.5 && hue < 2.0/3) {
            r = 0, g = x, b = c;
        } else if (hue >= 2.0/3 && hue < 5.0/6) {
            r = x, g = 0, b = c;
        } else {
            r = c, g = 0, b = x;
        }
        int red = (int)((r + m) * 255);
        int green = (int)((g + m) * 255);
        int blue = (int)((b + m) * 255);
        return MLV_rgba(red, green, blue, 255);
    }

/* 
 * Updates the status effects (like parasite or slow effects) on a monster based on the elapsed time.
 * The function reduces the timers for each effect and applies damage or changes in speed accordingly.
 * When a timer reaches zero, the corresponding effect is removed from the monster.
 *
 * @param monster Pointer to the Monster structure whose status effects are to be updated.
 * @param deltaTime The time elapsed since the last update, used to decrement effect timers.
 * @return void
 */
void updateMonsterEffects(Monster *monster, float deltaTime) {
    if (monster->isUnderParasiteEffect) {
        printf("here \n");
        monster->parasiteEffectTimer -= deltaTime;
        monster->parasiteDamageTimer -= deltaTime;
        if (monster->parasiteDamageTimer <= 0) {
            monster->hp -= 500;
            monster->parasiteDamageTimer = 0.2;
            if(monster->hp <= 0){
                monster->hp = 0 ;
            }
        }
        if (monster->parasiteEffectTimer <= 0) {
            monster->isUnderParasiteEffect = false;
        }
    }
    
    if (monster->isUnderSlowEffect) {
        monster->speed_factor = 1.5;
        monster->slowEffectTimer -= deltaTime;
        if (monster->slowEffectTimer <= 0) {
            monster->isUnderSlowEffect = false;
            // Reset speed
            if(monster->type_of_monster == AGILE){
                monster->speed_factor = 2;
            }else{
                monster->speed_factor = 2;
            }
            
        }
    }
}
