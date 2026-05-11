#ifndef COMPRESSION_UTILS_H
#define COMPRESSION_UTILS_H

#include "compression_utils.h" 
#include <stdio.h>            
#include <stdlib.h>            
#include <stdint.h>         
#include <string.h>           
#include <time.h>   
#include <math.h>

#include "utils.h"             

/**
 * @brief write_bits_to_file convertit le quadtree compressé en fichier qtc
 * 
 * @param uint8_t **data valeurs à encoder
 * @param int num_sizes taille des valeurs
 * @param const char *filename nom du fichier
 * @param int n taille du fichier
 *
 * @return void
 */
void write_bits_to_file(uint8_t **data, int num_sizes, const char *filename, int n);

#endif // COMPRESSION_UTILS_H
