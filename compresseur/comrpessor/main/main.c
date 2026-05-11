#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "../header/quadtree.h"
#include "../header/pgm_utils.h"
#include "../header/compression_utils.h"
#include "../header/utils.h"

int main(int argc, char *argv[]) {

    double alpha = 1.0; // Valeur d'alpha par défaut
    double beta = 0.9;   // Valeur de bêta par défaut

    // Analyse les arguments de la ligne de commande
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            alpha = atof(argv[++i]);
        }
        if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            beta = atof(argv[++i]);
        }
    }

    printf("Alpha value set to: %.2f\n", alpha);
    printf("Beta value set to: %.2f\n", beta);



    //-------------------------------------------------------------------------------------lire le fichier 
    uint8_t *tab_of_valuess = NULL;
    size_t num_pixels = 0;
    int * dimeensions = malloc(3 *sizeof(int *)); ;
    read_pgm_binary(foo("",argv[1] ,".pgm" ), &tab_of_valuess, &num_pixels,&dimeensions);
    printf("first byte is : %d  \n", tab_of_valuess[1]);
    //-------------------------------------------------------------------------------------calcule la taille et n
    int size = dimeensions[0];
    printf("size is  %d : \n", size);
    int n = log(size) / log(2);
    printf("Calculated n (tree depth): %d\n", n);
    //-------------------------------------------------------------------------------------création du quad tree
    max_nodes = (1 << (2 * (n + 1))) - 1;
    node_pool = malloc(max_nodes * sizeof(QuadTreeNode));
    QuadTreeTable *qt = create_quadtree_table(n);
    initialise_quad_tree(qt,n );
    //print_quadtree_table(qt);
    printf("created tree \n");
    //-------------------------------------------------------------------------------------récupération des pixels
    int **tab_holder = malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        tab_holder[i] = malloc(size * sizeof(int));
    }
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            tab_holder[i][j] = tab_of_valuess[i * size + j];
        }
    }
    printf("organized the pixels \n");
    //-------------------------------------------------------------------------------------changement d'ordre des pixels
    int new_tab_rows = (size / 2) * (size / 2);
    int **new_tab = malloc(new_tab_rows * sizeof(int *));
    for (int i = 0; i < new_tab_rows; i++) {
        new_tab[i] = malloc(4 * sizeof(int));
    }
    int index = 0;
    printf("tryinf to fill \n");
    fill_new_tab(tab_holder, new_tab, size, 0, 0, &index);
    printf("succeded  to fill \n");
    //-------------------------------------------------------------------------------------rempissage des feuilles
    index = 0;
    for (int i = 0; i < new_tab_rows; i++) {
        for (int j = 0; j < 4; j++) {
            qt->table[n][index]->variance = 0.0 ;
            qt->table[n][index++]->mean_intensity = new_tab[i][j];
            
        }
    }
    printf("filled new tab  \n");
    //-------------------------------------------------------------------------------------remplissage des parents
    compute_parent_values(qt);
    printf("filled parent nodes \n");
    //-------------------------------------------------------------------------------------filtrage
    double *tab_v = malloc((totalNodes(n) - pow(4,n)) * sizeof(double));
    if (!tab_v) {
        perror("Memory allocation for tab_v failed");
        exit(EXIT_FAILURE);
    }
    calculate_variances(qt,tab_v);

    double maxvar = 0.0;
    double medvar = 0.0;
    calculate_maxvar_and_medvar(tab_v, (totalNodes(n) - pow(4,n)), &maxvar, &medvar);

    printf("\n");
    printf("\n");
    printf("Max variance (maxvar): %.6f\n", maxvar);
    printf("Median variance (medvar): %.6f\n", medvar);
    printf("\n"); 
    double sigma = medvar  / maxvar;
    printf("Calculated sigma (σ): %.6f\n", sigma);

    filter_quadtree_dynamic(qt->table[0][0], sigma, alpha, beta);

    printf("done filtering\n");


    //-------------------------------------------------------------------------------------preparation au stockage des données
    uint8_t **test;
    int value_countt;
    int num_sizess = 0;  
    //-------------------------------------------------------------------------------------lecture des données
    traverse_quadtree_in_suffixx(qt, qt->levels, &test, &value_countt,&num_sizess);
    //-------------------------------------------------------------------------------------écriture des données
    write_bits_to_file(test, num_sizess,  foo("mycompressions/",argv[1] , ".qtc"),n);
    return 0;
}
