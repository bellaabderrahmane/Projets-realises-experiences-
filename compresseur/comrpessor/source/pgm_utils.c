#include "../header/pgm_utils.h"


void read_pgm_binary(const char *filename, uint8_t **tab_of_values, size_t *num_pixels, int **dimensions) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    int ascii_numbers[3] = {0};
    int ascii_count = 0;
    int is_data_section = 0;

    while (fgets(line, MAX_LINE_LENGTH, file)) {

        if (line[0] == 'P' || line[0] == '#') {
            continue;
        }

        // Gère les lignes avec des nombres ASCII
        char *token = strtok(line, " ");
        while (token) {
            if (isdigit(token[0])) {
                if (ascii_count < 3) {
                    ascii_numbers[ascii_count++] = atoi(token);
                }
            }
            token = strtok(NULL, " ");
        }

        if (ascii_count == 3) {
            is_data_section = 1;
            break;
        }
    }

    if (!is_data_section) {
        fprintf(stderr, "Error: Missing binary data section in file.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Calcule le nombre de total de pixel
    size_t total_pixels = (size_t)ascii_numbers[0] * (size_t)ascii_numbers[1];

    // Alloue la mémoire pour les données des pixels
    *tab_of_values = (uint8_t *)malloc(total_pixels * sizeof(uint8_t));
    if (!*tab_of_values) {
        perror("Error allocating memory");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Lis les données binaires
    size_t read_pixels = fread(*tab_of_values, sizeof(uint8_t), total_pixels, file);
    if (read_pixels != total_pixels) {
        fprintf(stderr, "Error: Unexpected end of file while reading pixel data.\n");
        free(*tab_of_values);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    *num_pixels = total_pixels;
    *dimensions =ascii_numbers ;
    
    printf("the paramaters :  ");
    for (int i = 0; i < 3; i++) {
    printf("%d ", ascii_numbers[i]);
    }
    printf("\n");

    fclose(file);
}
