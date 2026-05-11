#include "../header/segmentation.h"

/**
 * @brief dessine la grille de segmentation
 * 
 * @param uint8_t **tab_holder
 * @param QuadTreeNode *node
 * @param int x
 * @param int y
 * @param int size
 * @param uint8_t **output
 * @param int image_size
 */
void draw_segmentation_grid(uint8_t **tab_holder, QuadTreeNode *node, int x, int y, int size, uint8_t **output, int image_size) {
    if (node->uniform) {

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (x + i < image_size && y + j < image_size) {
                    output[x + i][y + j] = 255;
                }
            }
        }
    } else {

        int half = size / 2;
        if (half > 0) {
            draw_segmentation_grid(tab_holder, node->children[0], x, y, half, output, image_size);         
            draw_segmentation_grid(tab_holder, node->children[1], x, y + half, half, output, image_size); 
            draw_segmentation_grid(tab_holder, node->children[2], x + half, y + half, half, output, image_size); 
            draw_segmentation_grid(tab_holder, node->children[3], x + half, y, half, output, image_size); 
        }

        for (int i = 0; i < size; i++) {
            if (x + i < image_size) {
                if (y < image_size) output[x + i][y] = 0; 
                if (y + size - 1 < image_size) output[x + i][y + size - 1] = 0;
            }
            if (y + i < image_size) {
                if (x < image_size) output[x][y + i] = 0;
                if (x + size - 1 < image_size) output[x + size - 1][y + i] = 0;
            }
        }
    }
}

void create_segmentation_image(QuadTreeNode *root, int n, const char *filename) {
    int size = pow(2, n);
    uint8_t **output = malloc(size * sizeof(uint8_t *));
    for (int i = 0; i < size; i++) {
        output[i] = calloc(size, sizeof(uint8_t)); 
    }

    draw_segmentation_grid(NULL, root, 0, 0, size, output, size);

    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Unable to open file for writing");
        return;
    }

    fprintf(file, "P5\n%d %d\n255\n", size, size); // PGM header
    for (int i = 0; i < size; i++) {
        fwrite(output[i], sizeof(uint8_t), size, file);
        free(output[i]);
    }
    free(output);
    fclose(file);

    printf("Segmentation grid saved as %s\n", filename);
}