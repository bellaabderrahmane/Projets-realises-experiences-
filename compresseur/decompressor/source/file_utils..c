#include "../header/file_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


char* foo(const char *str1, const char *str2, const char *str3) {
    size_t len1 = str1 ? strlen(str1) : 0;
    size_t len2 = str2 ? strlen(str2) : 0;
    size_t len3 = str3 ? strlen(str3) : 0;
    size_t total_len = len1 + len2 + len3 + 1; 

    char *result = (char *)malloc(total_len);
    if (!result) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    if (str1) strcpy(result, str1);
    if (str2) strcat(result, str2);
    if (str3) strcat(result, str3);

    return result;
}


void reconstruct_tab_holder(uint8_t **tab_holder, uint8_t *new_tab, int size, int row, int col, int *index) {
    if (size == 2) {
        tab_holder[row][col] = new_tab[*index];
        tab_holder[row][col + 1] = new_tab[*index + 1];
        tab_holder[row + 1][col + 1] = new_tab[*index + 2];
        tab_holder[row + 1][col] = new_tab[*index + 3];
        (*index) += 4; 
        return;
    }

    int half = size / 2;

    reconstruct_tab_holder(tab_holder, new_tab, half, row, col, index);           // Haut-gauche
    reconstruct_tab_holder(tab_holder, new_tab, half, row, col + half, index);   // Haut-droit
    reconstruct_tab_holder(tab_holder, new_tab, half, row + half, col + half, index); // Bas-droit
    reconstruct_tab_holder(tab_holder, new_tab, half, row + half, col, index);   // Bas-gauche

    for (int i = 0; i < half; i++) {
        for (int j = 0; j < half; j++) {
            tab_holder[row + i][col + half + j] = tab_holder[row + i][col + half + j];

            tab_holder[row + half + i][col + half + j] = tab_holder[row + half + i][col + half + j];

            tab_holder[row + half + i][col + j] = tab_holder[row + half + i][col + j];
        }
    }
}



/**
 * @brief calcule le nombres de noeud du niveau
 * 
 * @param int K le niveau
 */
int calculate_nodes_at_level(int K) {
    return pow(4, K);
}

void jojo(QuadTreeTable *qt, int n, uint8_t **tab_final, int *tab_size) {
    if (!qt || !tab_final || !tab_size) {
        fprintf(stderr, "Error: Invalid QuadTree or output parameters.\n");
        return;
    }
    int last_level_nodes = calculate_nodes_at_level(n); 
    *tab_final = (uint8_t *)malloc(last_level_nodes * sizeof(uint8_t));
    if (!*tab_final) {
        fprintf(stderr, "Error: Memory allocation for tab_final failed.\n");
        return;
    }
    for (int i = 0; i < last_level_nodes; i++) {
        QuadTreeNode *node = qt->table[n][i];
        if (!node) {
            fprintf(stderr, "Warning: Node %d at level %d is NULL.\n", i, n);
            (*tab_final)[i] = 0; 
            continue;
        }
        (*tab_final)[i] = node->mean_intensity;
    }
    *tab_size = last_level_nodes;
}

/**
 * @brief Calcule le nombre total de noeuds
 * 
 * @param unsigned char n le nombre de niveau - 1
 */
unsigned long long totalNodes(unsigned char n) {
    return (pow(4, n + 1) - 1) / 3;
}

long count_8bit_characters(const char *filename,uint8_t **tab_of_values, int *index) {
    FILE *file = fopen(filename, "rb"); 
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    long count = 0;
    int table_place = 0;
    uint8_t ch;
    int check_magic_code = 0;
    int check_comments = 0 ;
    int read_first_byte = 0 ;
    // Lis chaque octet et compte le nombre de caractères
    while (fread(&ch, sizeof(uint8_t), 1, file) == 1) {
        if (ch ==0x51 && !check_magic_code ) {
            count++;
             while (fread(&ch, 1, 1, file) == 1) {
                count++;
                if (ch == 0x0A) {
                    check_magic_code = 1 ;
                    break;
                }
             }
        }
        else if (ch == 0x23 && !check_comments) {
            count++;
             while (fread(&ch, 1, 1, file) == 1) {
                count++;
                if (ch == 0x0A) {
                    break;
                }
             }
        }else if (ch != 0x23 && !check_comments) {
            check_comments = 1 ;
            if(1){
                if(!read_first_byte){
                    *tab_of_values = (uint8_t *)malloc((totalNodes(ch) * 3) * sizeof(uint8_t));
                    read_first_byte = 1;
                }
                (*tab_of_values)[table_place] = ch;
                table_place++;
                }
        }

        else if(1){
            
            if(!read_first_byte){
                *tab_of_values = (uint8_t *)malloc((totalNodes(ch) * 3) * sizeof(uint8_t));
                read_first_byte = 1;
            }
            (*tab_of_values)[table_place] = ch;
            table_place++;
            }

        count++; 

        
    }
    fclose(file);
    *index = table_place ;
    return count;
}










void get_current_datetime(char *datetime_str, size_t max_len) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(datetime_str, max_len, "# %a %b %d %H:%M:%S %Y\n", t);
}

























