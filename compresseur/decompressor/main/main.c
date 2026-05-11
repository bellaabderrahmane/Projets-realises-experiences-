#include "../header/quadtree.h"
#include "../header/file_utils.h"
#include "../header/segmentation.h"
#include <stdio.h>    
#include <stdlib.h>  
#include <math.h>    

int main(int argc, char *argv[]) {
    //-------------------------------------------------------------------------------------lecture et stockage des données du qtc
   
    int index =0;
    uint8_t *tab_of_values = NULL;
    long num_characters = count_8bit_characters(fod(argv[1], ".qtc"),&tab_of_values,&index);
    //-------------------------------------------------------------------------------------création du quad tree
    int n =  tab_of_values[0]; 
    printf("Calculated n (tree depth): %d\n", n);
    max_nodes = (1 << (2 * (n + 1))) - 1;
    node_pool = malloc(max_nodes * sizeof(QuadTreeNode));
    QuadTreeTable *qt = create_quadtree_table(n);
    initialise_quad_tree( qt,n );
    
    //-------------------------------------------------------------------------------------remplissage du quad tree
    traverse_and_populate(qt, tab_of_values, n);
    //print_quadtree_table(qt);
    //-------------------------------------------------------------------------------------Initialisation
    initialize_last_level_tab_holders(qt, n);
    uint8_t **testing = merge_quadtree_tab_h(qt, n);
    //-------------------------------------------------------------------------------------remplissage des feuilles :

    uint8_t *tab_final = NULL;
    int tab_size = 0;
    jojo(qt, n, &tab_final, &tab_size);
    //-------------------------------------------------------------------------------------réorganisation des valeurs
    int size = pow(2, n);
    uint8_t  **tab_holder = malloc(size * sizeof(uint8_t  *));
    for (int i = 0; i < size; i++) {
        tab_holder[i] = malloc(size * sizeof(uint8_t ));
    }
    int indexxx = 0;
    reconstruct_tab_holder(tab_holder, tab_final, pow(2, n), 0, 0, &indexxx);                        
    //-------------------------------------------------------------------------------------ecriture dans le fichier
    FILE *file = fopen(foo("mydecompression/", argv[1], ".pgm")  , "wb"); 
    if (!file) {
        perror("Unable to open file for writing");
        return EXIT_FAILURE;
    }
    fprintf(file, "P5\n");
    char datetime_str[64];
    get_current_datetime(datetime_str, sizeof(datetime_str));
    fwrite(datetime_str, sizeof(char), strlen(datetime_str), file);

    int dimension = (int)pow(2, n);
    fprintf(file, "%d %d\n", dimension, dimension); 
    fprintf(file, "255\n");   

    for (int i = 0; i < size; i++) {
        fwrite(tab_holder[i], sizeof(uint8_t), size, file);
    }
    printf("Data successfully written to test.txt\n");



    
    //-------------------------------------------------------------------------------------ecriture de la grille de segmentation vers pgm
    create_segmentation_image(qt->table[0][0], n, "segmentation/segmentation_grid.pgm");

    return 0;
}

