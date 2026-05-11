#ifndef PGM_UTILS_H
#define PGM_UTILS_H


#include "pgm_utils.h"
#include <stdio.h>    
#include <stdlib.h>   
#include <stdint.h>    
#include <string.h>    
#include <ctype.h> 
#include <math.h>

#define MAX_LINE_LENGTH 1024   

/**
 * @brief La fonction read_pgm_binary remplit le tableau des valeurs à partir d'un fichier pgm
 * 
 * @param const char *filename le nom du fichier
 * @param uint8_t **tab_of_values le tableau de valeur cible
 * @param size_t *num_pixels Le nombre de pixel du pgm
 * @param int **dimensions les dimensions du pgm
 * 
 * @return void
 */
void read_pgm_binary(const char *filename, uint8_t **tab_of_values, size_t *num_pixels, int **dimensions);

#endif // PGM_UTILS_H
