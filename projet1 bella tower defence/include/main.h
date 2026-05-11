#ifndef MAIN_H
#define MAIN_H

#include "graphics.h"
//#include "gem.h"

int initialize_wave(Wave *wave, Cell **path,WaveNode** head,int wave_number);
void update_wave(WaveNode ** head ,Wave *wave, Cell **path, int path_size,float * player_hp,float player_health_level,double frametime,float maximum_health);
void remove_empty_waves(WaveNode** head);


int main();

#endif
