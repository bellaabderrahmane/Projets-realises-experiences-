#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <math.h> 
#include "quadtree.h"

/**
 * @brief compte le nombre de caractères codés sûr 8 bits 
 * 
 * @param const char *filename le nom du fichier
 * @param uint8_t **tab_of_values le tableau des valeurs
 * @param int *index un index
 * 
 * @return long
 */
long count_8bit_characters(const char *filename, uint8_t **tab_of_values, int *index);

/**
 * @brief récupère la date actuelle
 * 
 * @param char *datetime_str la date
 * @param size_t max_len la longueur maximale de la date
 * 
 * @return void
 */
void get_current_datetime(char *datetime_str, size_t max_len);

/**
 * @brief remplit le tableau à partir du quad tree
 * 
 * @param QuadTreeTable *qt le quad tree
 * @param int n le nombre de niveaux - 1
 * @param uint8_t **tab_final le tableau cible
 * @param int *tab_size la taille du tableau
 * 
 * @return void
 */
void jojo(QuadTreeTable *qt, int n, uint8_t **tab_final, int *tab_size);

/**
 * @brief reconstruit le tableau à partir du tableau des valeurs
 * 
 * @param uint8_t **tab_holder
 * @param uint8_t *new_tab
 * @param int size
 * @param int row
 * @param int col
 * @param int *index
 * 
 * @return void
 */
void reconstruct_tab_holder(uint8_t **tab_holder, uint8_t *new_tab, int size, int row, int col, int *index);

/**
 * @brief concatène les 3 strings
 * 
 * @param const char *str1 une string à concaténer
 * @param const char *str2 une string à concaténer
 * @param const char *str3 une string à concaténer
 * 
 * @return char*
 */
char* foo(const char *str1, const char *str2, const char *str3);

#endif // FILE_UTILS_H
