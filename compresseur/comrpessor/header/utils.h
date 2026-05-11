#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include "utils.h"   
#include <stdio.h>   
#include <stdlib.h> 
#include <string.h> 
#include <time.h>  
#include <math.h>


/**
 * @brief concatène les 3 strings
 * 
 * @param const char *str1 une string à concaténer
 * @param const char *str2 une string à concaténer
 * @param const char *str3 une string à concaténer
 * 
 * @return char
 */
char *foo(const char *str1, const char *str2, const char *str3);

/**
 * @brief récupère la date actuelle
 * 
 * @param char *datetime_str la date
 * @param size_t max_len la longueur maximale de la date
 * 
 * @return void
 */
void get_current_datetime(char *datetime_str, size_t max_len);

#endif // UTILS_H
