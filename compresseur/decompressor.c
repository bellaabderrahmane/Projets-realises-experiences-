#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define UNIFORM_THRESHOLD 2 
#define MAX_LINE_LENGTH 1024


typedef struct QuadTreeNode {
    uint8_t mean_intensity;        
    uint8_t has_been_modified;         
    uint8_t error;                  
    uint8_t uniform;                
    uint8_t  **tab_h;
    struct QuadTreeNode *children[4];
} QuadTreeNode;

QuadTreeNode *node_pool;
int node_pool_index = 0;
int max_nodes = 0;

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

typedef struct {
    QuadTreeNode ***table;
    int levels;
} QuadTreeTable;

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





//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void printBinary(unsigned char byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (byte >> i) & 1);
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int elemnt_index_int_tab_of_valuess = 1; 
int currecnt_bit = 0;                   
int bits_read = 0;                   


uint8_t read_in_table(uint8_t *tab_of_valuess, int number_of_bits_to_read) {
    uint8_t final_value = 0; 
    bits_read = 0; 
   
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

void traverse_and_populate(QuadTreeTable *qt, uint8_t *tab_of_valuess, int n, int last_value_in_tab_of_valuess) {
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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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

    reconstruct_tab_holder(tab_holder, new_tab, half, row, col, index);         
    reconstruct_tab_holder(tab_holder, new_tab, half, row, col + half, index); 
    reconstruct_tab_holder(tab_holder, new_tab, half, row + half, col + half, index);
    reconstruct_tab_holder(tab_holder, new_tab, half, row + half, col, index);  

    for (int i = 0; i < half; i++) {
        for (int j = 0; j < half; j++) {
            tab_holder[row + i][col + half + j] = tab_holder[row + i][col + half + j];

            tab_holder[row + half + i][col + half + j] = tab_holder[row + half + i][col + half + j];

            tab_holder[row + half + i][col + j] = tab_holder[row + half + i][col + j];
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void get_current_datetime(char *datetime_str, size_t max_len) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(datetime_str, max_len, "# %a %b %d %H:%M:%S %Y\n", t);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




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


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



void merge_and_fill_tables(uint8_t **tab_of_m1, uint8_t **tab_of_m2, uint8_t **tab_total, int size_s, int size_big, int index) {
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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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

            merge_and_fill_tables(child1->tab_h, child2->tab_h, parent->tab_h, size_s, size_big, 0);

            merge_and_fill_tables(child4->tab_h, child3->tab_h, parent->tab_h, size_s, size_big, 1);
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


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


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

    fprintf(file, "P5\n%d %d\n255\n", size, size); 
    for (int i = 0; i < size; i++) {
        fwrite(output[i], sizeof(uint8_t), size, file);
        free(output[i]);
    }
    free(output);
    fclose(file);

    printf("Segmentation grid saved as %s\n", filename);
}


int main(int argc, char *argv[]) {
   
    int index =0;
    uint8_t *tab_of_values = NULL;
    long num_characters = count_8bit_characters(fod(argv[1], ".qtc"),&tab_of_values,&index);

    int n =  tab_of_values[0]; 
    printf("Calculated n (tree depth): %d\n", n);
    max_nodes = (1 << (2 * (n + 1))) - 1;
    node_pool = malloc(max_nodes * sizeof(QuadTreeNode));
    QuadTreeTable *qt = create_quadtree_table(n);
    initialise_quad_tree( qt,n );

    traverse_and_populate(qt, tab_of_values, n,index);

    initialize_last_level_tab_holders(qt, n);
    uint8_t **testing = merge_quadtree_tab_h(qt, n);


    uint8_t *tab_final = NULL;
    int tab_size = 0;
    jojo(qt, n, &tab_final, &tab_size);
    int size = pow(2, n);
    uint8_t  **tab_holder = malloc(size * sizeof(uint8_t  *));
    for (int i = 0; i < size; i++) {
        tab_holder[i] = malloc(size * sizeof(uint8_t ));
    }
    int indexxx = 0;
    reconstruct_tab_holder(tab_holder, tab_final, pow(2, n), 0, 0, &indexxx);                        
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

    for (int i = 0; i < size; i++) {
        fwrite(tab_holder[i], sizeof(uint8_t), size, file);
    }
    printf("Data successfully written to test.txt\n");

    create_segmentation_image(qt->table[0][0], n, "segmentation/segmentation_grid.pgm");

    return 0;
}
