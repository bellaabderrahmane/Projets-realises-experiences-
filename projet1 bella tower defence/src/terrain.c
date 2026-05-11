#include "../include/terrain.h"

    /*
    the function initialises the grid  by iterating through the grid's table of tables then sets thhree values to each case accordingly
    */
    void initialise_grid(Cell grid[WIDTH_GRID][HEIGHT_GRID]){
        for (int i = 0; i < WIDTH_GRID; i++) {
            for (int j = 0; j < HEIGHT_GRID; j++) {
                grid[i][j].x = i ;
                grid[i][j].y = j ;
                grid[i][j].color = MLV_COLOR_GRAY;                      //gray color represents the default case ,other colors represents differnt objects
                grid[i][j].type_c = EMPTY ;                             // setting case to empty by default
                grid[i][j].is_occupied = false ;
            }
        }
    }

    Cell** path_generate( Cell grid[WIDTH_GRID][HEIGHT_GRID],int * path_size){
        /*
        we start by declaring all varibales needed for the algorithm :
        pathCompleted : while it not true the algorithm continues
        start_cell : we declare the starting cell that we start adding cells from 
        path : we declare the head of linked list that reprsents the path
        pathLength : declare the variable that holds the path's length
        turns : de"clare of the variable that holds the number of turns the path made
        extent : declaration of the variable that holds the extent of the path in the current direction
        restartPath : while it's still false we restart the path  
        segmentLength : we declare the variable that holds segmentLength 
        */
        bool pathCompleted = false;
        Cell *start_cell ;
        PathNode *path = NULL;
        int pathLength;
        int turns;
        int extent;
        bool restartPath;
        int segmentLength ;
        Direction currentDirection; 
        while (!pathCompleted) {                                                    //  while path is not completed we continue reinitialising the path 
            start_cell = random_starting_cell(grid);                                //  we choose a randomly select a grid's cell  
            path = NULL;                                                            //  we initialise path node with NULL value
            add_to_path(&path, start_cell);                                         //  we add the starting cell at the head of path linked list
            turns = 0;                                                              //  we initialise the turns variable with 0 value 
            pathLength = 0;                                                         //  initialisation of pathLength
            restartPath = false;                                                    //  we initialise restartPath with false value
            currentDirection = select_initial_direction(*start_cell, grid,&extent); //  we set currentDirection with the randomly picked direction
            while (!restartPath) {
                if (extent <= 2) {                                   // if the extent that's calculated is inferior 
                    if ((pathLength-turns) >= 75 && turns >= 7) {            // if the pathlength is superior or equal 75 and turns supeior or equal to 7
                        pathCompleted = true;                        // the path is then set to "true"
                        break;
                    }
                    else{
                        restartPath = true;                          // the restartPath is then set to "true"
                        break;
                    }   
                }
                segmentLength = calculate_path_length(extent);                                                    // we calculate the the number of cells to add to the path 
                start_cell = add_segment_to_path(&path,*start_cell, currentDirection ,grid, segmentLength);       // we add the cells to the path 
                pathLength += segmentLength ;                                                                      // we increment pathLength by  number of cells we added 
                currentDirection = select_one_of_directions( *start_cell , grid , currentDirection , &extent ) ;  // we then select a random direction and affect the value of its extent to the variable extent  
                turns++;  //we increment the number of turns taken by 1
            }
            if (restartPath) {
            free_path(path);                                             // we free the path in case we need to restart over
            initialise_grid(grid);                                       // Reset the grid state
            }
        }
        start_cell->color = MLV_COLOR_BLUE ;  //we attribute the color blue to the last case to signal
        Cell **path_array = copy_path_to_array(path, pathLength);        //we affect the array that holds the cells of the path  
        free_path(path);                  // we free the linked list 
        *path_size = pathLength - turns ;
        return path_array ;
    }



    /*
    this function creates a node that can be later added to path linked list
    */
    PathNode* create_node(Cell *cell) {
        PathNode *new_node = (PathNode*) malloc(sizeof(PathNode));  //dynamically allocating space in memory for the node
        if (new_node == NULL) {//checking if node is not NULL
            fprintf(stderr, "Error allocating memory\n"); //handling the error
            exit(EXIT_FAILURE); //exiting on error with code EXIT_FAILURE
        }

        new_node->cell = cell;  //allocating the corresponding cell to the node
        new_node->next = NULL;  //attributing NULL value to next sevtion (means it's the currently last node i nlinked list)
        return new_node;  //return the resulting node
    }

/*
    this function calculates manhattan's distance using the formula given in the context
    */
    int manhattan_distance(int x1, int y1, int x2, int y2) {
        return abs(x1 - x2) + abs(y1 - y2);
    }

    /*
    this function randomely selects a cell within the grid with condition that it keep a distance of 3 cells away from any of grid's borders
    */
    Cell* random_starting_cell(Cell grid[WIDTH_GRID][HEIGHT_GRID]) {
        int x, y;                                                         //declaring the vairables used to store random values
        x = rand() % (WIDTH_GRID - 5) + 2;                                // no need to use manahttan distance ,simply we choose a random number between 3 and 25 (28 is out of index since we start from 0 )
        y = rand() % (HEIGHT_GRID- 5) + 2;                                // set the color to red to indicate the random cell visually
        grid[x][y].color = MLV_COLOR_RED;
        grid[x][y].type_c = PATH;                                         // this is to distinguish between an empty case and one that represents a path
        return &grid[x][y] ;                                              // we return the cell so we can initialise the path from it
    }



    /*
    this function calculates the etxtent from a starting cell and heading north leaving 3 manhattan distance between the path cells and 
    other path cells  (or border) ,it returns the extent.
    the extent represents all cells in one direction that verify the conditions , meaning starting cell is also included
    */
    int etendu_north(Cell start_cell, Cell grid[WIDTH_GRID][HEIGHT_GRID]) {
        int entendu = 0;                                        //we start by declaring the variable extent to hold the extent value in said direction
        int current_x = start_cell.x;                           //we copy the coordinates of starting cell so we can use it in our algorithm
        int current_y = start_cell.y ;
        while (current_y  > 1) {                                //this condition ensures we dont get in  distance of more than 3 blocks away from northborder
            entendu++;                                          //increment etendu at the start of while loop , since we start from 0 this includes the original starting cell and makes sure we verify the conditions before reentering
            if (current_x > 2 && grid[current_x - 1][current_y -1].type_c != EMPTY) { //we check the northwest cell
                break;
            }
            if (current_x < WIDTH_GRID - 3 && grid[current_x + 1][current_y - 1].type_c != EMPTY) {  //we check the northeast cell
                break;
            }
            if (  grid[current_x][current_y - 2].type_c != EMPTY) { //we check the north cell that's 1 block away (or 2 manahttan distance away)
                break;
            }
            if (current_x > 2 && grid[current_x - 1][current_y -2].type_c != EMPTY) { //we check the northwest cell (that's 3 manhattan distance away)
                break;
            }
            if (current_x < WIDTH_GRID - 3 && grid[current_x + 1][current_y - 2].type_c != EMPTY) { //we check the northeast cell (that's 3 manhattan distance away)
                break;
            }
            if (  grid[current_x][current_y - 3].type_c != EMPTY) {  //we check the north cell that's 2 block away (or 3 manahttan distance away)
                break;
            }
            if (current_x > 2 && grid[current_x - 2][current_y -1].type_c != EMPTY) { //we check the northwest cell (that's 3 manhattan distance away)
                break;
            }
            if (current_x < WIDTH_GRID - 3 && grid[current_x + 2][current_y - 1].type_c != EMPTY) { //we check the northweast cell (that's 3 manhattan distance away)
                break;
            }
            current_y--;   //if none of these conditions is verified , we move one block north
        }
        return entendu ;
    }


    /*
    this function calculates the etxtent from a starting cell and heading south leaving 3 manhattan distance between the path cells and 
    other path cells  (or border) ,it returns the extent
    the extent represents all cells in a direction that verifies the conditions , meaning starting cell is also included
    */
    int etendu_south(Cell start_cell, Cell grid[WIDTH_GRID][HEIGHT_GRID]) {
        int entendu = 0;                                        //we start by declaring the variable extent to hold the extent value in said direction
        int current_x = start_cell.x;                           //we copy the coordinates of starting cell so we can use it in our algorithm
        int current_y = start_cell.y ;
        while (current_y  < HEIGHT_GRID - 2) {                  //this condition ensures we dont get in  distance of more than 3 blocks away from south border
            entendu++;                                           //increment etendu at the start of while loop , since we start from 0 this includes the original starting cell and makes sure we verify the conditions before reentering
            if (current_x > 2 && grid[current_x - 1][current_y +1].type_c != EMPTY) { //we check the southwest cell
                break;
            }
            if (current_x < WIDTH_GRID - 3 && grid[current_x + 1][current_y + 1].type_c != EMPTY) { //we check the southheast cell
                break;
            }
            if (  grid[current_x][current_y + 2].type_c != EMPTY) { //we check the south cell that's 1 block away (or 2 manahttan distance away)
                break;
            }
            if (current_x > 2 && grid[current_x - 1][current_y +2].type_c != EMPTY) { //we check the southwest cell (that's 3 manhattan distance away)
                break;
            }
            if (current_x < WIDTH_GRID - 3 && grid[current_x + 1][current_y + 2].type_c != EMPTY) {  //we check the south cell that's 2 block away (or 3 manahttan distance away)
                break;
            }
            if (  grid[current_x][current_y + 3].type_c != EMPTY) { //we check the south cell that's 2 block away (or 3 manahttan distance away)
                break;
            }
            if (current_x > 2 && grid[current_x - 2][current_y +1].type_c != EMPTY) { //we check the southwest cell (that's 3 manhattan distance away)
                break;
            }
            if (current_x < WIDTH_GRID - 3 && grid[current_x + 2][current_y + 1].type_c != EMPTY) { //we check the southweast cell (that's 3 manhattan distance away)
                break;
            }
            current_y++;    //if none of these conditions is verified , we move one block south
        }
        return entendu ;
    }



    /*
    this function calculates the etxtent from a starting cell and heading west leaving 3 manhattan distance between the path cells and 
    other path cells  (or border) ,it returns the extent
    the extent represents all cells in a  direction that verifies the conditions , meaning starting cell is also included
    */
    int etendu_west(Cell start_cell, Cell grid[WIDTH_GRID][HEIGHT_GRID]) {
        int entendu = 0;                                                    //we start by declaring the variable extent to hold the extent value in said direction
        int current_x = start_cell.x;                                                 //we copy the coordinates of starting cell so we can use it in our algorithm
        int current_y = start_cell.y ;
        while (current_x  > 1) {                                                //this condition ensures we dont get in  distance of more than 3 blocks away from west border
            entendu++;                                                            //increment etendu at the start of while loop , since we start from 0 this includes the original starting cell and makes sure we verify the conditions before reentering
            if ( grid[current_x - 1][current_y - 1].type_c != EMPTY) {  //we check the northwest cell
                break;
            }
            if (grid[current_x - 1][current_y + 1].type_c != EMPTY) {  //we check the southwest cell
                break;
            }
            if (  grid[current_x - 2][current_y ].type_c != EMPTY) { //we check the swest cell that's 1 block away (or 2 manahttan distance away)
                break;
            }
            if ( grid[current_x - 2][current_y - 1].type_c != EMPTY) {//we check the northwest cell (that's 3 manhattan distance away)
                break;
            }
            if (grid[current_x - 2][current_y + 1].type_c != EMPTY) { //we check the southwest cell that's 2 block away (or 3 manahttan distance away)
                break;
            }
            if (  grid[current_x - 3][current_y ].type_c != EMPTY) {  //we check the west cell that's 2 block away (or 3 manahttan distance away)
                break;
            }
            if ( grid[current_x - 1][current_y - 2].type_c != EMPTY) { //we check the northwest cell (that's 3 manhattan distance away)
                break;
            }
            if (grid[current_x - 1][current_y + 2].type_c != EMPTY) { //we check the southwest cell (that's 3 manhattan distance away)
                break;
            }


            current_x--;              //if none of these conditions is verified , we move one block west
        }
        return entendu ;
    }


    /*
    this function calculates the etxtent from a starting cell and heading east leaving 3 manhattan distance between the path cells and 
    other path cells  (or border) ,it returns the extent
    the extent represents all cells in a  direction that verifies the conditions , meaning starting cell is also included
    */
    int etendu_east(Cell start_cell, Cell grid[WIDTH_GRID][HEIGHT_GRID]) {
        int entendu = 0;                                                //we start by declaring the variable extent to hold the extent value in said direction
        int current_x = start_cell.x;                           //we copy the coordinates of starting cell so we can use it in our algorithm
        int current_y = start_cell.y ;
        while (current_x  < WIDTH_GRID -2) {                    //this condition ensures we dont get in  distance of more than 3 blocks away from east border
            entendu++;                                                     //increment etendu at the start of while loop , since we start from 0 this includes the original starting cell and makes sure we verify the conditions before reentering
            if ( grid[current_x + 1][current_y - 1].type_c != EMPTY) { //we check the northeast cell
                break;
            }
            if (grid[current_x + 1][current_y + 1].type_c != EMPTY) {  //we check the southeast cell
                break;
            }
            if (  grid[current_x + 2][current_y ].type_c != EMPTY) { //we check the east cell that's 1 block away (or 2 manahttan distance away)
                break;
            }
            if ( grid[current_x + 2][current_y - 1].type_c != EMPTY) { //we check the northeast cell (that's 3 manhattan distance away)
                break;
            }
            if (grid[current_x + 2][current_y + 1].type_c != EMPTY) { //we check the southeast cell that's 2 block away (or 3 manahttan distance away)
                break;
            }
            if (  grid[current_x + 3][current_y ].type_c != EMPTY) { //we check the east cell that's 2 block away (or 3 manahttan distance away)
                break;
            }
            if ( grid[current_x + 1][current_y - 2].type_c != EMPTY) { //we check the northeast cell (that's 3 manhattan distance away)
                break;
            }
            if (grid[current_x + 1][current_y + 2].type_c != EMPTY) {  //we check the southeast cell (that's 3 manhattan distance away)
                break;
            }
            current_x++;                            //if none of these conditions is verified , we move one block east
        }
        return entendu ;
    }





    /*
    this function selects an initial direction from one of the 4 cardinal directions by calculating the extent of each direction
    then randomely selescting one of them in proportion to its extent.
    the function then returns the direction and affects its extent to the etendu variable
    */
    Direction select_initial_direction(Cell start_cell , Cell grid[WIDTH_GRID][HEIGHT_GRID],int *etendu) {
        int north_extent = etendu_north(start_cell,grid) ;   //here we calculate thenorth extent by calling etendu_north function
        int south_extent = etendu_south(start_cell,grid) ;
        int east_extent  = etendu_east(start_cell,grid)  ;
        int west_extent  = etendu_west(start_cell,grid);
        int total_extent = north_extent + south_extent + east_extent + west_extent;  //we add the sum of all extents to use in random picking
        int random_pick = rand() % total_extent;              //we choose a random number between 0 and total_extent
        if (random_pick < north_extent) {         //we check if the random_pick is inferior to north_extent
            *etendu = north_extent;                   //  we affect the value of its extent to the value of etendu variable
            return NORTH;                            //we return the direction
        } else if (random_pick < north_extent + south_extent) {  /*we check if the random_pick is inferior to the sum of north extent plus south extent,meaning here 
                                                                random_pick is superior to north extent but inerior to south extent*/
            *etendu = south_extent;
            return SOUTH;
        } else if (random_pick < north_extent + south_extent + east_extent) { //same logic to the rest
            *etendu = east_extent;
            return EAST;
        } else {
            *etendu = west_extent;
            return WEST;
        }
    }


    /*
    this function is imilar to select_initial_direction but instead of having to choose between 4 directions , it only chooses between 
    tw<o according to the current direction while turning 90 degrees,for example : 
    if cuurent direction is north 
        randomly choose between east or west */
    Direction select_one_of_directions( Cell start_cell , Cell grid[WIDTH_GRID][HEIGHT_GRID] , Direction direction , int *etendu ) {
        int total_extent ;              //we decalre the vaiable that hosts the sum of both directions extents
        int random_pick ;
        if (direction == NORTH || direction == SOUTH){          //check if direction is south or north ,if so we randomly pick between east and west
            int east_extent  = etendu_east(start_cell,grid)  ;  //we calculate the extent of east direction
            int west_extent  = etendu_west(start_cell,grid);
            total_extent =  east_extent + west_extent;          //we add their sum together (their extents)
            random_pick = rand() % total_extent;                //we randomely pick a number in range of 0 to total_extent

            if (random_pick < east_extent) {   //we check if the randomly picked number is inferior to east extent
                *etendu = east_extent;         //we affect the east extent to entendu vairable
                return EAST;                   // we return the direction
            } else {
                *etendu = west_extent;
                return WEST;
            }
            
        }else if( direction == EAST || direction == WEST ){  //check if direction is west or east ,if so we randomly pick between north and south
            int north_extent = etendu_north(start_cell,grid) ; //same logic as above
            int south_extent = etendu_south(start_cell,grid) ;
            total_extent = north_extent + south_extent ;
            random_pick = rand() % total_extent;

            if (random_pick < north_extent) {
                *etendu = north_extent;
                return NORTH;
            } else  {
                *etendu = south_extent;    
                return SOUTH;
            } 

        }


    }

    /*
    this function calculates the number of cells to add in a path by randomley picking a number between 1(3/4 chance) and 0 (1/4 chance)
    then it add the sum of these randomely picked numbers and returns it if it's superior to 3 , if not it returns 3 ensuring
    that the path has at least 3 cells in any given direction */
    int calculate_path_length(int extent) {
        int sum = 0;                        //declaration of sum variable
        for (int i = 0; i < extent; i++) {      /*randomely picking an umber between 0 and 1 ,"extent" amount of times,until we reach "extent" not included 
                                                ,this ensures we start from 0 and finish at extent -1 : we excude starting cell from the number of cells to add
                                                */
            if (rand() < 0.75 * RAND_MAX) {/*here's a condition that basically generates a number between 0 and rand_max , if the genrated
                                            number falls below 0.75 * RAND_MAX or in other terms 75%of its value then we increment sum by 1 , if not we do nothing
                                            since adding 0 doesn't change sum's value.
                                            */
                sum += 1;
            }
        }
        return (sum > 3) ? sum : 3;   // we return sum if it's superior to 3 , if not we return 3 directly
    }


    
    /*
    this function takes a node an adds it to path linked list ,it takes "path" node which is at the head of the linked list then iterates through
    it until it reaches the last one which it adds the node after it.
    */
    void add_to_path(PathNode **path, Cell *cell) {
        PathNode *new_node = create_node(cell); //we create a new node that holds the cell
        if (*path == NULL) {
            *path = new_node; //if the linked list is empty we inialise it 
        } else {
            PathNode *current = *path;  //if the list is not empty we set our cuurent node to the head of the list
            while (current->next != NULL) {   //we check is we still ahven't encountered  the last node in the list
                current = current->next;
            }
            current->next = new_node; //if yes we set "next" section to the new_node
        }
    }



    /*
    this function frees the memory space that holds the linked list by iterating through it and freeing each individual node ,
    it useful to avoid memory leaks
    */
    void free_path(PathNode *path) {
        PathNode *current = path;
        while (current != NULL) { //we free each node until we reach the last one
            PathNode *temp = current;
            current = current->next;
            free(temp);
        }
    }



    /*
    this function adds cells to the path in a given direction 
    */
    Cell* add_segment_to_path(PathNode ** path ,Cell start_cell , Direction direction, Cell grid[WIDTH_GRID][HEIGHT_GRID] ,int length_segment){
        int x = start_cell.x;  //we get the coordinates of the starting cell
        int y = start_cell.y; 
        for (int i = 0; i < length_segment -1 ; i++) {   //we kep looping until we reach length_segment -1
            switch (direction) {   //we check which direction to add the cells to
                case NORTH:
                    y--;          //if it's north then we decrement the value of its y variable( we add a cell to the north)
                    break;
                case SOUTH:
                    y++;
                    break;
                case EAST:
                    x++;
                    break;
                case WEST:
                    x--;
                    break;
            }
            grid[x][y].type_c = PATH;   //we change the state of chosen cell to represent a PATH cell
            grid[x][y].color = MLV_COLOR_YELLOW;   //we change its color to yellow for visual purposes
            add_to_path(path, &grid[x][y]); //once we update the y oand x positions accordingly , we add the corresponding cell of grid to the path linked list
        }
        return &grid[x][y] ; //we return the last cell we added(so we can start from it next time when we need to add cells in another direction)
    }

    /*
    this function copies the cells from path linked list to an array , the reason is it's much more efficient mezmory wise and easier to 
    access cells when needed .
    */
    Cell** copy_path_to_array(PathNode *path, int path_length) {
        Cell **pathArray = (Cell **)malloc(sizeof(Cell *) * path_length);  //we daclare a table that hosts pointers of the cells in path,this approach allows us to store pointersd tto the original cells in grid
        if (pathArray == NULL) {
            fprintf(stderr, "Error allocating memory for path array\n");//handling error in case memory allocation goes wrong
            exit(EXIT_FAILURE);
        }
        PathNode *current = path;
        for (int i = 0; i < path_length; i++) { //we iterate through path linked list 
            if (current == NULL) {
                break;
            }
            pathArray[i] = current->cell; //we affect the cell to patharray at i index
            current = current->next;
        }
        return pathArray;  //return the array
    }