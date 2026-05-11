#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>


typedef struct QuadTreeNode {
    uint8_t mean_intensity; // L'intensité du pixel ou du grooupe de pixel uniforme
    uint8_t error;          // Erreur pour la compression sans perte
    uint8_t uniform;        // Flag d'uniformité (0 -> non uniforme, 1 -> uniforme)
    double variance;        // Variance acceptée
    struct QuadTreeNode *children[4]; // Pointeurs vers les fils
} QuadTreeNode;

typedef struct {
    QuadTreeNode ***table;
    int levels;
} QuadTreeTable;

extern QuadTreeNode *node_pool;
extern int node_pool_index;
extern int max_nodes;

// Fonctions publiques

/**
 * @brief crée le tableau contenant le quad tree
 * 
 * @param int n le nombre de niveaux - 1
 * 
 * @return QuadTreeTable
 */
QuadTreeTable *create_quadtree_table(int n);

/**
 * @brief Crée la structure du quad tree dans le tableau
 * 
 * @param QuadTreeTable *qt le quad tree
 * @param int n le nombre de niveaux - 1
 */
void initialise_quad_tree(QuadTreeTable *qt, int n);

/**
 * @brief Vérifie l'uniformité des noeuds parents
 * 
 * @param QuadTreeTable *qt le quad tree
 */
void compute_parent_values(QuadTreeTable *qt);

/**
 * @brief Parcours l'arbre pour calcluler la variance
 * 
 * @param QuadTreeTable *qt le quad tree
 * @param double *tab_v tableau des variances
 */
void calculate_variances(QuadTreeTable *qt, double *tab_v);

/**
 * @brief filtre le quad tree
 * 
 * @param QuadTreeNode *node le noeud courant
 * @param double sigma
 * @param double alpha
 * @param double beta
 */
int filter_quadtree_dynamic(QuadTreeNode *node, double sigma, double alpha, double beta);

/***
 * @brief Remplis le quad tree des valeurs adéquates
 * 
 * @param QuadTreeTable *qt le quad tree
 * @param int levels les niveaix de l'arbre
 * @param uint8_t ***tab_of_values les valeurs à donner au quad tree
 * @param int *value_count la taille des valeurs
 * @param int *num_sizes la tailles des valeurs
 */
void traverse_quadtree_in_suffixx(QuadTreeTable *qt, int levels, uint8_t ***tab_of_values, int *value_count, int *num_sizes);

/**
 * @brief Remplis un nouveau tableau de manière récursive
 * 
 * @param int **tab_holder le tableau contenant les valeurs
 * @param int **new_tab le nouveau tableau
 * @param int size la taille du tableau
 * @param int row le numéro de la ligne
 * @param int col le numéro de colonne
 * @param int *index un index
 */
void fill_new_tab(int **tab_holder, int **new_tab, int size, int row, int col, int *index);

/**
 * @brief Calcule la vairiance moyenne et la variance maximale
 * 
 * @param double *tab_v le tableau des valeurs
 * @param int count un compteur
 * @param double *maxvar la variance maximale
 * @param double *medvar la variance mediane
 */
void calculate_maxvar_and_medvar(double *tab_v, int count, double *maxvar, double *medvar);

/**
 * @brief Retourne le nombre total de noeuds
 * 
 * @param unsigned char n le nombre de niveaux - 1
 */
unsigned long long totalNodes(unsigned char n);

/**
 * @brief Libère le tableau contenant le quad tree
 * 
 * @param QuadTreeTable *qt le quad tree
 */
void free_quadtree_table(QuadTreeTable *qt);

#endif // QUADTREE_H
