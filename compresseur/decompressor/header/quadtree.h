#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define UNIFORM_THRESHOLD 2

typedef struct QuadTreeNode {
    uint8_t mean_intensity;
    uint8_t has_been_modified;
    uint8_t error;
    uint8_t uniform;
    uint8_t **tab_h;
    struct QuadTreeNode *children[4];
} QuadTreeNode;

typedef struct {
    QuadTreeNode ***table;
    int levels;
} QuadTreeTable;

// Variable Globales
extern QuadTreeNode *node_pool;
extern int node_pool_index;
extern int max_nodes;

/**
 * @brief crée le tableau contenant le quad tree
 * 
 * @param int n le nombre de niveaux - 1
 * 
 * @return QuadTreeTable
 */
QuadTreeTable *create_quadtree_table(int n);

/**
 * @brief Libère le tableau contenant le quad tree
 * 
 * @param QuadTreeTable *qt le quad tree
 * 
 * @return void
 */
void free_quadtree_table(QuadTreeTable *qt);

/**
 * @brief Crée la structure du quad tree dans le tableau
 * 
 * @param QuadTreeTable *qt le quad tree
 * @param int n le nombre de niveaux - 1
 * 
 * @return void
 */
void initialise_quad_tree(QuadTreeTable *qt, int n);

/**
 * @brief Propage les couleurs des parents uniformes aux fils
 * 
 * @param QuadTreeNode *node le noeud courant
 * @param int level le niveau courant
 * @param int max_level le niveau maximal
 * 
 * @return void
 */
void propagate_uniform_values(QuadTreeNode *node, int level, int max_level);

/**
 * @brief remplis la table des valeurs à partir du quad tree
 * 
 * @param QuadTreeTable *qt le quad tree
 * @param uint8_t *tab_of_valuess le tableau des valeurs
 * @param int n le nombre de niveaux - 1
 * 
 * @return void
 */
void traverse_and_populate(QuadTreeTable *qt, uint8_t *tab_of_valuess, int n );

/**
 * @brief affiche le quad tree sous forme de table
 * 
 * @param QuadTreeTable *qt le quad tree
 * 
 * @return void
 */
void print_quadtree_table(QuadTreeTable *qt);

/**
 * @brief gère l'initialisation des feuilles
 * 
 * @param QuadTreeTable *qt le quad tree
 * @param int n le nombre de niveaux - 1
 * 
 * @return void
 */
void initialize_last_level_tab_holders(QuadTreeTable *qt, int n);

/**
 * @brief fusionne les tab holder du quad tree
 * 
 * @param QuadTreeTable *qt le quad tree
 * @param int n le nombre de niveaux - 1
 * 
 * @return void
 */
uint8_t **merge_quadtree_tab_h(QuadTreeTable *qt, int n);

/**
 * @brief concatène deux strings
 * 
 * @param const char *str1 une string à concaténer
 * @param const char *str2 une string à concaténer
 * 
 * @return void
 */
char* fod(const char *str1, const char *str2);

#endif // QUADTREE_H
