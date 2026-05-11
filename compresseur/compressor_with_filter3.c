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
    uint8_t error;                  
    uint8_t uniform;               
    double variance;
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
    node->error = 0;
    node->uniform = 1;
    node->variance=-1.0f;
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



void print_quadtree_table(QuadTreeTable *qt) {
    for (int level = 0; level < qt->levels; level++) {
        int nodes_in_level = 1 << (2 * level);
        printf("Level %d: %d nodes\n", level, nodes_in_level);
        for (int i = 0; i < nodes_in_level; i++) {
            QuadTreeNode *node = qt->table[level][i];
            printf("Node %d at level %d has mean_intensity: %d, error: %d, uniform: %d variance(exist/99) %2f \n",
                   i, level, node->mean_intensity, node->error, node->uniform,(node->variance != -1.0f) ? node->variance : -1.0f );
        }
    }
}





//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


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
    fill_new_tab(tab_holder, new_tab, half, row, col, index);          
    fill_new_tab(tab_holder, new_tab, half, row, col + half, index);  
    fill_new_tab(tab_holder, new_tab, half, row + half, col + half, index); 
    fill_new_tab(tab_holder, new_tab, half, row + half, col, index);  
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void compute_parent_values(QuadTreeTable *qt) {
    for (int level = qt->levels - 1; level > 0; level--) {
        int parent_nodes = 1 << (2 * (level - 1)); 
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










void store_binary_value(uint8_t e, int bits, char ***tab, int *num_sizes) {

    *tab = (char **)realloc(*tab, (*num_sizes + 1) * sizeof(char *));
    if (*tab == NULL) {
        perror("Memory reallocation failed");
        exit(1); 
    }

    (*tab)[*num_sizes] = (char *)malloc((bits + 1) * sizeof(char)); 
    if ((*tab)[*num_sizes] == NULL) {
        perror("Memory allocation failed");
        exit(1); 
    }

    for (int i = bits - 1; i >= 0; i--) {
        (*tab)[*num_sizes][bits - 1 - i] = (e >> i) & 1 ? '1' : '0';
    }
    (*tab)[*num_sizes][bits] = '\0'; 

    (*num_sizes)++;
}

void print_table(char **tab, int num_sizes) {
    printf("Table contents:  ");
    for (int i = 0; i < 10 ; i++) {
        printf("%s , ", tab[i]); 
    }
    printf("\n");
}

void free_table(char **tab, int num_sizes) {
   
    for (int i = 0; i < num_sizes; i++) {
        free(tab[i]);
    }
   
    free(tab);
}





typedef struct Value {
    uint8_t mean_intensity; 
    uint8_t error;          
    uint8_t uniform;       
} Value;






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

    size_t total_pixels = (size_t)ascii_numbers[0] * (size_t)ascii_numbers[1];

    *tab_of_values = (uint8_t *)malloc(total_pixels * sizeof(uint8_t));
    if (!*tab_of_values) {
        perror("Error allocating memory");
        fclose(file);
        exit(EXIT_FAILURE);
    }

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





















void get_current_datetime(char *datetime_str, size_t max_len) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(datetime_str, max_len, "# %a %b %d %H:%M:%S %Y\n", t);
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










void traverse_quadtree_in_suffixx(QuadTreeTable *qt, int levels, uint8_t ***tab_of_values, int *value_count,int *num_sizes) {
    *tab_of_values = malloc(max_nodes * sizeof(uint8_t *)); 
    for (int i = 0; i < max_nodes; i++) {
        (*tab_of_values)[i] = malloc(2 * sizeof(uint8_t));  
    }

    *value_count = 0;

    for (int level = 0; level < levels; level++) {
        int nodes_in_level = 1 << (2 * level);

        for (int i = 0; i < nodes_in_level; i++) {
            QuadTreeNode *node = qt->table[level][i];
            QuadTreeNode *parent = NULL;

            if (level > 0) {
                parent = qt->table[level - 1][i / 4];
            }

            if (parent && parent->error == 0 && parent->uniform == 1) {
                continue; 
            }

            Value temp = {0};

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

            else {
                if (i % 4 != 3) { 
                    temp.mean_intensity = node->mean_intensity;
                    (*tab_of_values)[*value_count][1] = 8;
                    (*tab_of_values)[*value_count][0] = node->mean_intensity;
                    (*value_count)++;
                }
            }
        }
    }

    *num_sizes = *value_count;
}









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
    int elemnt_index_int_tab_of_valuess = 0;
    int currecnt_bit = 0;                    
    uint8_t buffer = 0;                     
    int bits_in_buffer = 0;                 
    for (int i = 0; i < num_sizess; i++) {
        uint8_t value = test[i][0];         
        int bits_to_read = test[i][1];      

        while (bits_to_read > 0) {
            uint8_t bit = (value >> (bits_to_read - 1)) & 1;

            buffer = (buffer << 1) | bit;
            bits_in_buffer++;

            bits_to_read--;

            if (bits_in_buffer == 8) {
                fwrite(&buffer, 1, 1, file);
                buffer = 0; 
                bits_in_buffer = 0;
            }
        }
    }

    if (bits_in_buffer > 0) {
        buffer <<= (8 - bits_in_buffer);
        fwrite(&buffer, 1, 1, file);
    }

    fclose(file);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



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








//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void calculate_variances(QuadTreeTable *qt, double *tab_v) {
    int tab_index = 0;
    for (int level = qt->levels - 1; level > 0; level--) {
        int parent_nodes = 1 << (2 * (level - 1));

        for (int i = 0; i < parent_nodes; i++) {
            QuadTreeNode *parent = qt->table[level - 1][i];

            double variance_sum = 0.0f;
            int parent_mean = parent->mean_intensity;

            for (int j = 0; j < 4; j++) {
                QuadTreeNode *child = parent->children[j];
                if (child) {
                    double child_variance = child->variance;
                    int child_mean = child->mean_intensity;

                    variance_sum += (child_variance * child_variance) + 
                                    ((double)(parent_mean - child_mean) * (parent_mean - child_mean));
                }
            }

            parent->variance = sqrt(variance_sum) / 4.0f;
            tab_v[tab_index++] = parent->variance;
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


int compare_doubles(const void *a, const void *b) {
    double da = *(const double *)a;
    double db = *(const double *)b;
    return (da > db) - (da < db); 
}

void calculate_maxvar_and_medvar(double *tab_v, int count, double *maxvar, double *medvar) {

    qsort(tab_v, count, sizeof(double), compare_doubles);

    *maxvar = tab_v[count - 1];

    if (count % 2 == 0) {
     
        *medvar = (tab_v[count / 2 - 1] + tab_v[count / 2]) / 2.0;
    } else {
        
        *medvar = tab_v[count / 2];
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


unsigned long long totalNodes(unsigned char n) {
    return (pow(4, n + 1) - 1) / 3;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


int main(int argc, char *argv[]) {

    double alpha = 1.0; 
    double beta = 0.9;  

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



    uint8_t *tab_of_valuess = NULL;
    size_t num_pixels = 0;
    int * dimeensions = malloc(3 *sizeof(int *)); ;
    read_pgm_binary(foo("",argv[1] ,".pgm" ), &tab_of_valuess, &num_pixels,&dimeensions);
    printf("first byte is : %d  \n", tab_of_valuess[1]);
  
    int size = dimeensions[0];
    printf("size is  %d : \n", size);
    int n = log(size) / log(2);
    printf("Calculated n (tree depth): %d\n", n);

    max_nodes = (1 << (2 * (n + 1))) - 1;
    node_pool = malloc(max_nodes * sizeof(QuadTreeNode));
    QuadTreeTable *qt = create_quadtree_table(n);
    initialise_quad_tree(qt,n );
    //print_quadtree_table(qt);
    printf("created tree \n");

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
 
    int new_tab_rows = (size / 2) * (size / 2);
    int **new_tab = malloc(new_tab_rows * sizeof(int *));
    for (int i = 0; i < new_tab_rows; i++) {
        new_tab[i] = malloc(4 * sizeof(int));
    }
    int index = 0;
    printf("tryinf to fill \n");
    fill_new_tab(tab_holder, new_tab, size, 0, 0, &index);
    printf("succeded  to fill \n");

    index = 0;
    for (int i = 0; i < new_tab_rows; i++) {
        for (int j = 0; j < 4; j++) {
            qt->table[n][index]->variance = 0.0 ;
            qt->table[n][index++]->mean_intensity = new_tab[i][j];
            
        }
    }
    printf("filled new tab  \n");
 
    compute_parent_values(qt);
    printf("filled parent nodes \n");

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

    uint8_t **test;
    int value_countt;
    int num_sizess = 0;  

    traverse_quadtree_in_suffixx(qt, qt->levels, &test, &value_countt,&num_sizess);

    write_bits_to_file(test, num_sizess,  foo("mycompressions/",argv[1] , ".qtc"),n);
    return 0;
}