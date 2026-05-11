#ifndef MONSTER_H
#define MONSTER_H
#include <stdbool.h>
#include <math.h>
#include "terrain.h"


#define MONSTER_RADIUS 10       // define the radius for the circle that represents the monster
#define D 6  //the constant d that's included in gem damage formula (or base damage in short)
/*
this enum represents all possible types for a wave 
*/
typedef enum {
    NORMAL,
    FOULE,
    AGILE,
    BOSS
} waves;


/*
this structure defines monster's attributes , it has : 
hp : it's current health
x : the x cooridnates in gird
y : the y cooridnates in gird
currentPathIndex : determines it's position in path
isActive : determines if a monster is active (can move or not)
*/
typedef struct {
    float hp;
    float x;
    float y;
    int teinte;
    int currentPathIndex; 
    waves type_of_monster ;
    bool isActive;  
    float speed_factor ;  
    bool isUnderParasiteEffect;   // Flag for parasite effect (Dendro)
    float parasiteEffectTimer;    // Timer for parasite effect duration
    float parasiteDamageTimer;    // Timer for applying parasite damage
    bool isUnderSlowEffect;       // Flag for slow effect (Hydro)
    float slowEffectTimer;        // Timer for slow effect duration   
} Monster;

MLV_Color teinte_to_rgb(int teinte);
void move_monster(Cell **path_array ,Monster *monster,int wave_number);
void inialise_monster(Monster * monster,Cell **path_array,int health ,float speed,waves type_mons);
void updateMonsterEffects(Monster *monster, float deltaTime);
#endif
