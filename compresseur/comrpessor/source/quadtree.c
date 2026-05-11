#include "../header/quadtree.h"








QuadTreeNode *node_pool = NULL;
int node_pool_index = 0;
int max_nodes = 0;






QuadTreeTable *create_quadtree_table(int n) {
    QuadTreeTable *qt = malloc(sizeof(QuadTreeTable));
    qt->levels = n + 1;
    qt->table = malloc(qt->levels * sizeof(QuadTreeNode **));

    for (int level = 0; level <= n; level++) {
        int size = 1 << (2 * level); // 2^(level+level)
        qt->table[level] = malloc(size * sizeof(QuadTreeNode *));
    }

    return qt;
}



unsigned long long totalNodes(unsigned char n) {
    return (pow(4, n + 1) - 1) / 3;
}






void compute_parent_values(QuadTreeTable *qt) {
    for (int level = qt->levels - 1; level > 0; level--) {
        int parent_nodes = 1 << (2 * (level - 1)); // Nombre de noeuds parents aux niveau actuel

        for (int i = 0; i < parent_nodes; i++) {
            QuadTreeNode *parent = qt->table[level - 1][i];
            int sum = 0;
            int uniform = 1; 
            int first_mean = parent->children[0]->mean_intensity;

            for (int j = 0; j < 4; j++) {
                QuadTreeNode *child = parent->children[j];
                int child_mean = child->mean_intensity;

                if (!child->uniform || child_mean != first_mean) {
                    uniform = 0; 
                }

                sum += child_mean;
            }

            parent->mean_intensity = sum / 4;
            parent->error = sum % 4;
            parent->uniform = uniform; 
        }
    }
}


int filter_quadtree_dynamic(QuadTreeNode *node, double sigma, double alpha, double beta) {
    if (node->uniform) return 1;  
    if (!node->children[0]) return 1; 

    int all_uniform = 1;
    for (int i = 0; i < 4; i++) {
        all_uniform &= filter_quadtree_dynamic(node->children[i], sigma * (alpha), pow(alpha, beta), beta);
    }

    if (all_uniform && node->variance <= sigma) {
        node->uniform = 1;
        node->error = 0;  
        return 1;
    }



    return 0;
}




void calculate_variances(QuadTreeTable *qt, double *tab_v) {
    int tab_index = 0;
    for (int level = qt->levels - 1; level > 0; level--) {
        int parent_nodes = 1 << (2 * (level - 1)); // Nombre de parents du niveau courant

        for (int i = 0; i < parent_nodes; i++) {
            QuadTreeNode *parent = qt->table[level - 1][i];

            double variance_sum = 0.0f;
            int parent_mean = parent->mean_intensity;

            for (int j = 0; j < 4; j++) {
                QuadTreeNode *child = parent->children[j];
                if (child) {
                    double child_variance = child->variance;
                    int child_mean = child->mean_intensity;

                    // Calcule la contribution de ce fils à la variance
                    variance_sum += (child_variance * child_variance) + 
                                    ((double)(parent_mean - child_mean) * (parent_mean - child_mean));
                }
            }

            // Modifie la variance du noeud parent
            parent->variance = sqrt(variance_sum) / 4.0f;
            tab_v[tab_index++] = parent->variance;
        }
    }
}





/**
 * @brief crée un noeud de quad tree
 */
QuadTreeNode *create_node() {
    if (node_pool_index >= max_nodes) {
        fprintf(stderr, "Error: Node pool exhausted!\n");
        exit(EXIT_FAILURE);
    }
    QuadTreeNode *node = &node_pool[node_pool_index++];
    node->mean_intensity = 0;
    node->error = 0;
    node->uniform = 1; // Default to uniform
    node->variance=-1.0f;
    for (int i = 0; i < 4; i++) {
        node->children[i] = NULL;
    }
    return node;
}





void initialise_quad_tree(QuadTreeTable *qt,int n ){
    qt->table[0][0] = create_node();
    for (int level = 1; level <= n; level++) {
        int parent_nodes = 1 << (2 * (level - 1));
        for (int i = 0; i < parent_nodes; i++) {
            QuadTreeNode *parent = qt->table[level - 1][i];
            for (int j = 0; j < 4; j++) {
                QuadTreeNode *child = create_node();
                parent->children[j] = child;
                qt->table[level][4 * i + j] = child;
            }
        }
    }
    
}



void traverse_quadtree_in_suffixx(QuadTreeTable *qt, int levels, uint8_t ***tab_of_values, int *value_count,int *num_sizes) {
    *tab_of_values = malloc(max_nodes * sizeof(uint8_t *)); 
    for (int i = 0; i < max_nodes; i++) {
        (*tab_of_values)[i] = malloc(2 * sizeof(uint8_t)); 
    }

    *value_count = 0;

    for (int level = 0; level < levels; level++) {
        int nodes_in_level = 1 << (2 * level); // Nombre de parents du niveau courant

        for (int i = 0; i < nodes_in_level; i++) {
            QuadTreeNode *node = qt->table[level][i];
            QuadTreeNode *parent = NULL;

            if (level > 0) {
                parent = qt->table[level - 1][i / 4];
            }

            if (parent && parent->error == 0 && parent->uniform == 1) {
                continue; 
            }

            

            // Traitement de la racine
            if (level == 0) {
                (*tab_of_values)[*value_count][1] = 8;
                (*tab_of_values)[*value_count][0] = node->mean_intensity;
                (*value_count)++;

                (*tab_of_values)[*value_count][1] = 2;
                (*tab_of_values)[*value_count][0] = node->error;
                (*value_count)++;
               
                if (node->error == 0) {
                    
                    (*tab_of_values)[*value_count][1] = 1;
                    (*tab_of_values)[*value_count][0] = node->uniform;
                    (*value_count)++;
                }
                

                if (node->error == 0 && node->uniform == 1) {
                    return; 
                }
            }
            // Traitement des noeuds internes
            else if (level < levels - 1) {
                

                if (i % 4 != 3 ) { 
                    (*tab_of_values)[*value_count][1] = 8;
                    (*tab_of_values)[*value_count][0] = node->mean_intensity;
                    (*value_count)++;

                    (*tab_of_values)[*value_count][1] = 2;
                    (*tab_of_values)[*value_count][0] = node->error;
                    (*value_count)++;
                    
                    
                }


                if (i % 4 == 3) { 
                    if (node->error == 0) {
                        (*tab_of_values)[*value_count][1] = 2;
                        (*tab_of_values)[*value_count][0] = node->error;
                        (*value_count)++;

                        (*tab_of_values)[*value_count][1] = 1;
                        (*tab_of_values)[*value_count][0] = node->uniform;
                        (*value_count)++;

                        
                    }else if (node->error != 0) {
                        (*tab_of_values)[*value_count][1] = 2;
                        (*tab_of_values)[*value_count][0] = node->error;
                        (*value_count)++;
                        
                    }
                } else if (node->error == 0) {
                    (*tab_of_values)[*value_count][1] = 1;
                    (*tab_of_values)[*value_count][0] = node->uniform;
                    (*value_count)++;
                }

                
            }
            // traitement des feuilles
            else {
                if (i % 4 != 3) { 
                    (*tab_of_values)[*value_count][1] = 8;
                    (*tab_of_values)[*value_count][0] = node->mean_intensity;
                    (*value_count)++;
                }
            }
        }
    }

    *num_sizes = *value_count;
}



void fill_new_tab(int **tab_holder, int **new_tab, int size, int row, int col, int *index) {
    if (size == 2) {
        new_tab[*index][0] = tab_holder[row][col];
        new_tab[*index][1] = tab_holder[row][col + 1];
        new_tab[*index][2] = tab_holder[row + 1][col + 1];
        new_tab[*index][3] = tab_holder[row + 1][col];
        (*index)++;
        return;
    }

    int half = size / 2;
    fill_new_tab(tab_holder, new_tab, half, row, col, index);           // Haut-gauche
    fill_new_tab(tab_holder, new_tab, half, row, col + half, index);   // Haut-droite
    fill_new_tab(tab_holder, new_tab, half, row + half, col + half, index); // Bas-droite
    fill_new_tab(tab_holder, new_tab, half, row + half, col, index);   // Bas-gauche
}



/**
 * @brief compare des doubles
 * 
 * @param const void *a
 * @param const void *b
 */
int compare_doubles(const void *a, const void *b) {
    double da = *(const double *)a;
    double db = *(const double *)b;
    return (da > db) - (da < db); 
}

void calculate_maxvar_and_medvar(double *tab_v, int count, double *maxvar, double *medvar) {

    qsort(tab_v, count, sizeof(double), compare_doubles);

    // maxvar est le dernier élément du tableau trié
    *maxvar = tab_v[count - 1];

    if (count % 2 == 0) {
        *medvar = (tab_v[count / 2 - 1] + tab_v[count / 2]) / 2.0;
    } else {
        *medvar = tab_v[count / 2];
    }
}


void free_quadtree_table(QuadTreeTable *qt) {
    for (int level = 0; level < qt->levels; level++) {
        free(qt->table[level]);
    }
    free(qt->table);
    free(qt);
}