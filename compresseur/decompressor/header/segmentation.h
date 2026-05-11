#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <stdint.h>
#include <math.h> 
#include "quadtree.h"




/**
 * @brief fonction créant et sauvegardant une image à partir d'un quad tree
 * 
 * @param QuadTreeNode *root le noeud de la racine du quad tree
 * @param int n le nombre de niveaux - 1
 * @param const char *filename le nom du fichier
 * 
 * @return void
 */
void create_segmentation_image(QuadTreeNode *root, int n, const char *filename);

#endif // SEGMENTATION_H
