
#include "../header/quadtree.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


QuadTreeNode *node_pool = NULL;
int node_pool_index = 0;
int max_nodes = 0;

QuadTreeTable *create_quadtree_table(int n) {
    QuadTreeTable *qt = malloc(sizeof(QuadTreeTable));
    qt->levels = n + 1;
    qt->table = malloc(qt->levels * sizeof(QuadTreeNode **));

    for (int level = 0; level <= n; level++) {
        int size = 1 << (2 * level); 
        qt->table[level] = malloc(size * sizeof(QuadTreeNode *));
    }

    return qt;
}


void free_quadtree_table(QuadTreeTable *qt) {
    for (int level = 0; level < qt->levels; level++) {
        free(qt->table[level]);
    }
    free(qt->table);
    free(qt);
}

QuadTreeNode *create_node() {
    if (node_pool_index >= max_nodes) {
        fprintf(stderr, "Error: Node pool exhausted!\n");
        exit(EXIT_FAILURE);
    }
    QuadTreeNode *node = &node_pool[node_pool_index++];
    node->mean_intensity = 0;
    node->has_been_modified = 0;
    node->error = 0;
    node->uniform = 0; 
    node->tab_h = NULL;
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



int elemnt_index_int_tab_of_valuess = 1;
int currecnt_bit = 0;                
int bits_read = 0;                    

/**
 * @brief lis les valeurs octet par octet
 * 
 * @param uint8_t *tab_of_valuess le tableau des valeurs
 * @param int number_of_bits_to_read le nombre de bit à lire
 */
uint8_t read_in_table(uint8_t *tab_of_valuess, int number_of_bits_to_read) {
    uint8_t final_value = 0; 
    bits_read = 0;
    // Continue à lire jusqu'à avoir lû le nombre de bits requis
    while (bits_read < number_of_bits_to_read) {
        uint8_t current_byte = tab_of_valuess[elemnt_index_int_tab_of_valuess];
        uint8_t current_bit_value = (current_byte >> (7 - currecnt_bit)) & 1;
        final_value = (final_value << 1) | current_bit_value;
        currecnt_bit++;
        bits_read++;
        if (currecnt_bit == 8) {
            elemnt_index_int_tab_of_valuess++;
            currecnt_bit = 0;
        }
    }
     return final_value;
}

void propagate_uniform_values(QuadTreeNode *node, int level, int max_level) {
    for (int i = 0; i < 4; i++) {
        QuadTreeNode *child = node->children[i];
        if (child) {
            child->mean_intensity = node->mean_intensity;
            child->has_been_modified = 1 ;
            if (level < max_level) {
                propagate_uniform_values(child, level + 1, max_level);
            }
        }
    }
}


void merge_and_fill_tables(uint8_t **tab_of_m1, uint8_t **tab_of_m2, uint8_t **tab_total, int size_s, int index) {
    if (!tab_of_m1 || !tab_of_m2 || !tab_total) {
        fprintf(stderr, "Error: Invalid input tables.\n");
        return;
    }

    uint8_t **tab_of_m3 = malloc(size_s * sizeof(uint8_t *));
    if (!tab_of_m3) {
        fprintf(stderr, "Error: Memory allocation for tab_of_m3 failed.\n");
        return;
    }

    for (int i = 0; i < size_s; i++) {
        tab_of_m3[i] = malloc(2 * size_s * sizeof(uint8_t)); 
        if (!tab_of_m3[i]) {
            fprintf(stderr, "Error: Memory allocation for tab_of_m3[%d] failed.\n", i);
            for (int j = 0; j < i; j++) {
                free(tab_of_m3[j]);
            }
            free(tab_of_m3);
            return;
        }

        for (int j = 0; j < size_s; j++) {
            tab_of_m3[i][j] = tab_of_m1[i][j];
        }

        for (int j = 0; j < size_s; j++) {
            tab_of_m3[i][size_s + j] = tab_of_m2[i][j];
        }
    }

    int start_index = (index == 0) ? 0 : size_s; 
    for (int i = 0; i < size_s; i++) {
        tab_total[start_index + i] = tab_of_m3[i];
    }

}


uint8_t **merge_quadtree_tab_h(QuadTreeTable *qt, int n) {
    if (!qt || n < 1) {
        fprintf(stderr, "Invalid QuadTreeTable or depth.\n");
        return NULL;
    }

    int current_size = 1; 

    for (int level = n; level > 0; level--) {
        int num_parents = 1 << (2 * (level - 1)); 
        int size_s = current_size;               
        int size_big = 2 * size_s;              

        for (int i = 0; i < num_parents; i++) {
            QuadTreeNode *parent = qt->table[level - 1][i];
            if (!parent) {
                fprintf(stderr, "Warning: Parent node %d at level %d is NULL.\n", i, level - 1);
                continue;
            }
            parent->tab_h = malloc(2 * sizeof(uint8_t *)); 
            if (!parent->tab_h) {
                fprintf(stderr, "Error: Memory allocation for parent tab_h failed for node %d at level %d.\n", i, level - 1);
                continue;
            }
            for (int j = 0; j < 2; j++) {
                parent->tab_h[j] = malloc(size_big * sizeof(uint8_t)); 
                if (!parent->tab_h[j]) {
                    fprintf(stderr, "Error: Memory allocation for parent tab_h[%d] failed for node %d at level %d.\n", j, i, level - 1);
                    for (int k = 0; k < j; k++) {
                        free(parent->tab_h[k]);
                    }
                    free(parent->tab_h);
                    parent->tab_h = NULL;
                    continue;
                }
            }

            QuadTreeNode *child1 = parent->children[0];
            QuadTreeNode *child2 = parent->children[1];
            QuadTreeNode *child3 = parent->children[2];
            QuadTreeNode *child4 = parent->children[3];

            if (!child1 || !child2 || !child3 || !child4) {
                fprintf(stderr, "Warning: Missing children for parent node %d at level %d.\n", i, level - 1);
                continue;
            }

            merge_and_fill_tables(child1->tab_h, child2->tab_h, parent->tab_h, size_s, 0);

            merge_and_fill_tables(child4->tab_h, child3->tab_h, parent->tab_h, size_s, 1);
        }

        current_size = size_big;
    }
    QuadTreeNode *root = qt->table[0][0];
    if (!root) {
        fprintf(stderr, "Error: Root node is NULL.\n");
        return NULL;
    }
    return root->tab_h;
}



void initialize_last_level_tab_holders(QuadTreeTable *qt, int n) {
    if (!qt || n < 0) {
        fprintf(stderr, "Invalid QuadTreeTable or depth.\n");
        return;
    }

    int last_level_nodes = pow(4,n); 

    for (int i = 0; i < last_level_nodes; i++) {
        QuadTreeNode *node = qt->table[n][i]; 
        if (!node) {
            fprintf(stderr, "Warning: Node %d at level %d is NULL.\n", i, n);
            continue;
        }

        node->tab_h = malloc(sizeof(uint8_t *)); 
        if (!node->tab_h) {
            fprintf(stderr, "Error: Memory allocation for tab_holder failed for node %d.\n", i);
            continue;
        }

        node->tab_h[0] = malloc(sizeof(uint8_t)); 
        if (!node->tab_h[0]) {
            fprintf(stderr, "Error: Memory allocation for tab_holder[0] failed for node %d.\n", i);
            free(node->tab_h);
            node->tab_h = NULL;
            continue;
        }

        node->tab_h[0][0] = node->mean_intensity;
    }
}






void traverse_and_populate(QuadTreeTable *qt, uint8_t *tab_of_valuess, int n) {
    for (int level = 0; level <= n; level++) {
        int nodes_in_level = 1 << (2 * level);
        for (int i = 0; i < nodes_in_level; i++) {
            QuadTreeNode *node = qt->table[level][i];
            if (level == 0) {
                node->mean_intensity = read_in_table(tab_of_valuess, 8);
                node->has_been_modified = 1;
                node->error = read_in_table(tab_of_valuess, 2);
                if (node->error == 0) {
                    node->uniform = read_in_table(tab_of_valuess, 1);
                } 

                if (node->uniform == 1) {
                    propagate_uniform_values(node, level, n);
                    return; 
                }
                continue;
            }

            if (node->has_been_modified == 1 ) {
                continue;
            }
            if (level == n) {
                if ((i % 4 == 3) && level > 0) {
                    QuadTreeNode *parent = qt->table[level - 1][i / 4];
                    QuadTreeNode *sibling1 = qt->table[level][i - 3];
                    QuadTreeNode *sibling2 = qt->table[level][i - 2];
                    QuadTreeNode *sibling3 = qt->table[level][i - 1];
                    node->mean_intensity = (4 * parent->mean_intensity + parent->error) -(sibling1->mean_intensity + sibling2->mean_intensity +sibling3->mean_intensity); 
                    node->has_been_modified = 1 ; 

                }else{

                     node->mean_intensity = read_in_table(tab_of_valuess, 8);
                     node->has_been_modified = 1 ;
                }
                continue;
            }


            if ((i % 4 == 3) && level > 0) {
                QuadTreeNode *parent = qt->table[level - 1][i / 4];
                QuadTreeNode *sibling1 = qt->table[level][i - 3];
                QuadTreeNode *sibling2 = qt->table[level][i - 2];
                QuadTreeNode *sibling3 = qt->table[level][i - 1];
                node->mean_intensity = (4 * parent->mean_intensity + parent->error) -(sibling1->mean_intensity + sibling2->mean_intensity +sibling3->mean_intensity);
                node->has_been_modified = 1 ;
                node->error = read_in_table(tab_of_valuess, 2);
                if (node->error == 0) {
                    node->uniform = read_in_table(tab_of_valuess, 1);
                } 

                if (node->uniform == 1) {
                    propagate_uniform_values(node, level, n);
                }
                continue;
            }

            node->mean_intensity = read_in_table(tab_of_valuess, 8);
            node->has_been_modified = 1 ;
            node->error = read_in_table(tab_of_valuess, 2);
            if (node->error == 0) {
                node->uniform = read_in_table(tab_of_valuess, 1);
            } 

            if (node->uniform == 1) {
                propagate_uniform_values(node, level, n);
                continue; 
            }
        }
    }
}



void print_quadtree_table(QuadTreeTable *qt) {
    for (int level = 0; level < qt->levels; level++) {
        int nodes_in_level = 1 << (2 * level);
        printf("Level %d: %d nodes\n", level, nodes_in_level);
        for (int i = 0; i < nodes_in_level; i++) {
            QuadTreeNode *node = qt->table[level][i];
            printf("Node %d at level %d has mean_intensity: %d, error: %d, uniform: %d , has_been_updated : %d",
                   i, level, node->mean_intensity, node->error, node->uniform,node->has_been_modified);
            if (node->tab_h != NULL ){printf(" table exists: %d ",node->tab_h[0][0]);}
            printf("\n");
        }
    }
}


char* fod(const char *str1, const char *str2) {

    size_t len1 = str1 ? strlen(str1) : 0;
    size_t len2 = str2 ? strlen(str2) : 0;
    size_t total_len = len1 + len2 + 1; 

    char *result = (char *)malloc(total_len);
    if (!result) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    if (str1) strcpy(result, str1);  
    if (str2) strcat(result, str2);  

    return result;
}