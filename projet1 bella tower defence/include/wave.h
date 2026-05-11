#ifndef WAVE_H
#define WAVE_H

#include "monster.h"







/*
this structure holds all monsters that belong to  acertain wave ,it also has a field for number of monsters
has isActive field to indicate status , and wavenumber,plus initial health of monsters
*/
typedef struct {
    Monster** monsters;
    int numMonsters;
    bool isActive; 
    int wave_number ;
    float wave_initial_health ;
} Wave;


/*
this linked holds all waves in current game
*/
typedef struct WaveNode {
    Wave * newWave;
    struct WaveNode* next;
} WaveNode;


MLV_Color color_generate(int number);

void addWaveNode(WaveNode** head, Wave * newWave) ;
int length_list(WaveNode* head) ;
void free_waves(WaveNode** head);
waves random_wave_selection(int  wave_number);
void draw_health_bar(Monster *monster, float max_health);

void distanceToNextMonsterInPath(Monster* monster1, int path_size, WaveNode * head);

int initialize_wave(Wave *wave, Cell **path,WaveNode** head,int wave_number);
void update_wave(WaveNode ** head ,Wave *wave, Cell **path, int path_size,float * player_hp,float player_health_level,double frametime,float maximum_health);
void remove_empty_waves(WaveNode** head);


#endif