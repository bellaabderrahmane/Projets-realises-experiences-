#ifndef TERRAIN_H
#define TERRAIN_H



#include <MLV/MLV_all.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>



#define WIDTH_WINDOW  1200       // defined window's size in pixels
    #define HEIGHT_WINDOW 690       // defined window's height in pixels
    #define WIDTH_GRID    28        // this global variable represents  the actual grid's width ( measuring one case as a single unit)
    #define HEIGHT_GRID   22        // this one represents the grid's height (same as above )
    #define SIZE_CELL     30        // here we define the size of the cell in pixels (considering an individual case as a square)
    #define LENGTH_LINE   1         // this line allows to leave a small gap between each case giving the illusion of an actual grid
    #define EXIT_FAILURE  1         // we define exit code for error handling and signaling
    #define MONSTER_RADIUS 10       // define the radius for the circle that represents the monster
    #define WAVE_INTERVAL 35        // 35 seconds for each wave
    #define INITIAL_HP_CONSTANT 100 // Adjust as needed for game balance

    #define HEALTH_BAR_WIDTH (SIZE_CELL * 0.8)  // define the monster's health bar width
    #define HEALTH_BAR_HEIGHT 5                 // define the monster's health bar height

    #define PLAYER_HEALTH_BAR_WIDTH 840      // Width of the player's health bar  (or MANA bar)
    #define PLAYER_HEALTH_BAR_HEIGHT 27      // Height of the player's health bar
    #define PLAYER_HEALTH_BAR_X 0            //  X Position of  player's health bar  
    #define PLAYER_HEALTH_BAR_Y 660          //  Y Position of  player's health bar 

    #define MANA_BOX_WIDTH 177               // the width of tower constructing costs box
    #define MANA_BOX_HEIGHT 43               // the height of tower constructing costs box
    #define MANA_BOX_X 845                   // X position of tower constructing costs box
    #define MANA_BOX_Y 4                     // Y position of tower constructing costs box

    #define TIME_BOX_WIDTH 180                                              // the width of timer box (for next wave)
    #define TIME_BOX_HEIGHT 27                                              // the height of timer box
    #define TIME_BOX_X (PLAYER_HEALTH_BAR_X + PLAYER_HEALTH_BAR_WIDTH + 1)  // Position X of timer box
    #define TIME_BOX_Y PLAYER_HEALTH_BAR_Y                                  // Position Y of timer box

    #define WAVE_BOX_WIDTH 180      // the width  of wave box
    #define WAVE_BOX_HEIGHT 27      // the height of wave box
    #define WAVE_BOX_X 1018         // Position X of wave box
    #define WAVE_BOX_Y 660          // Position Y of wave box

    #define MAX_GEM_NUMBER 6        //determine inventory slot number

    #define minus_box_x 1178        //define X position for minus sign box (used to decrease gem's level for purchase)
    #define minus_box_y 26          //define Y position for minus sign box 
    #define minus_box_width 20      //define width for minus sign box
    #define minus_box_height 25     //define height for minus sign box

    #define info_box_x 1027         //define the coordinates for ( gem purchase box)
    #define info_box_y 4
    #define info_box_width 150
    #define info_box_height 51

    #define plus_box_x 1178         //define the coordinates for plus sign box
    #define plus_box_y 0
    #define plus_box_width 20
    #define plus_box_height 25


    #define GEM_INVENTORY_START_X 900    //define coordinates for inventory zone
    #define GEM_INVENTORY_START_Y 120
    #define GEM_INVENTORY_END_X 1150
    #define GEM_INVENTORY_END_Y 170
    #define GEM_BOX_WIDTH  (GEM_INVENTORY_END_X - GEM_INVENTORY_START_X) / (MAX_GEM_NUMBER)
    #define GEM_BOX_HEIGHT 50 

    #define FUSE_BOX_X GEM_INVENTORY_START_X + 90  //define coordinates for fuse box used to fuse gems
    #define FUSE_BOX_Y GEM_INVENTORY_END_Y +40   
    #define FUSE_BOX_WIDTH GEM_BOX_WIDTH
    #define FUSE_BOX_HEIGHT GEM_BOX_HEIGHT

#define WIDTH_WINDOW  1200       // defined window's size in pixels
#define HEIGHT_WINDOW 690       // defined window's height in pixels
#define WIDTH_GRID    28        // this global variable represents  the actual grid's width ( measuring one case as a single unit)
#define HEIGHT_GRID   22        // this one represents the grid's height (same as above )
#define SIZE_CELL     30        // here we define the size of the cell in pixels (considering an individual case as a square)
#define LENGTH_LINE   1         // this line allows to leave a small gap between each case giving the illusion of an actual grid
#define EXIT_FAILURE  1         // we define exit code for error handling and signaling


/*
this type defines both types of cases (empty ones and ones used to represent the path)
*/

typedef enum {
    PATH ,
    EMPTY,
    FULL
}Type_case;

/*
this structure represents an individual case , it has three variables : 
1) x : represents the horizontal position in the grid and allows quick access to the case (it works as an index )
2) y : represents the vertical position of the case in the grid (works same as x)
3) color : this variable stores the color of the case ( it's set to gray as default)
*/

typedef struct {
    int x, y;
    MLV_Color color;
    Type_case type_c ;
    bool is_occupied ;
    int number_occupation ;  //
} Cell;

/*
this enum object called DIrection defines the 4 cardinal directions (eaxh represents an integer value {0,1,2,3})
*/
typedef enum { NORTH, SOUTH, EAST, WEST } Direction;

/*
this strucutre is used to store the cells that represents a path in a linked list which we can use
to iterate through and store the cells eventually in an array
the strucutre has : 
1- "cell" section that stores a pointer of a cell
2- "next" section that points to the next node in the list*/
typedef struct PathNode {
    Cell *cell;
    struct PathNode *next;
} PathNode;







void initialise_grid(Cell grid[WIDTH_GRID][HEIGHT_GRID]);
Cell** path_generate(Cell grid[WIDTH_GRID][HEIGHT_GRID], int* path_size);
PathNode* create_node(Cell *cell) ;
 int manhattan_distance(int x1, int y1, int x2, int y2) ;
 Cell* random_starting_cell(Cell grid[WIDTH_GRID][HEIGHT_GRID]);
 int etendu_north(Cell start_cell, Cell grid[WIDTH_GRID][HEIGHT_GRID]);
 int etendu_south(Cell start_cell, Cell grid[WIDTH_GRID][HEIGHT_GRID]) ;
 int etendu_west(Cell start_cell, Cell grid[WIDTH_GRID][HEIGHT_GRID]);
 int etendu_east(Cell start_cell, Cell grid[WIDTH_GRID][HEIGHT_GRID]) ;
  Direction select_initial_direction(Cell start_cell , Cell grid[WIDTH_GRID][HEIGHT_GRID],int *etendu) ;
 Direction select_one_of_directions( Cell start_cell , Cell grid[WIDTH_GRID][HEIGHT_GRID] , Direction direction , int *etendu );
 int calculate_path_length(int extent) ;
 void add_to_path(PathNode **path, Cell *cell);
void free_path(PathNode *path);
Cell* add_segment_to_path(PathNode ** path ,Cell start_cell , Direction direction, Cell grid[WIDTH_GRID][HEIGHT_GRID] ,int length_segment);
Cell** copy_path_to_array(PathNode *path, int path_length) ;



#endif
