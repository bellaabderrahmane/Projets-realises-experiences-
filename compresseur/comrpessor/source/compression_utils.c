



#include "../header/compression_utils.h"



void write_bits_to_file(uint8_t **test, int num_sizess, const char *filename,int n) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file");
        return;
    }
    fprintf(file, "Q1\n");
    char datetime_str[64];
    get_current_datetime(datetime_str, sizeof(datetime_str));
    fwrite(datetime_str, sizeof(char), strlen(datetime_str), file);
    fprintf(file, "# compression rate %f%%\n", 0.93*100);
    uint8_t size = n;
    fwrite(&size, 1, 1, file);
    int elemnt_index_int_tab_of_valuess = 0; // index de l'élément courant
    int currecnt_bit = 0;                    // Position du bit courant dans l'octet
    uint8_t buffer = 0;                      // Buffer pour garder les 8 bits à écrire
    int bits_in_buffer = 0;                  // Nombre de bit dans le buffer

    for (int i = 0; i < num_sizess; i++) {
        uint8_t value = test[i][0];         // valeurs à encoder
        int bits_to_read = test[i][1];      // nombre de bite à lire dans la valeur

        while (bits_to_read > 0) {

            uint8_t bit = (value >> (bits_to_read - 1)) & 1;

            // ajoute le bit au buffer
            buffer = (buffer << 1) | bit;
            bits_in_buffer++;
            bits_to_read--;

            // ecris dans le fichier lorsque le buffer est rempli
            if (bits_in_buffer == 8) {
                fwrite(&buffer, 1, 1, file);
                buffer = 0;
                bits_in_buffer = 0;
            }
        }
    }

    // Padding avec des 0 s'il reste des bits
    if (bits_in_buffer > 0) {
        buffer <<= (8 - bits_in_buffer);
        fwrite(&buffer, 1, 1, file);
    }

    fclose(file);
}