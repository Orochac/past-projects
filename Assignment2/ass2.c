/* Solution to comp10002 Assignment 2, 2019 semester 2.

   Authorship Declaration:

   (1) I certify that the program contained in this submission is completely
   my own individual work, except where explicitly noted by comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students,
   or by non-students as a result of request, solicitation, or payment,
   may not be submitted for assessment in this subject.  I understand that
   submitting for assessment work developed by or in collaboration with
   other students or non-students constitutes Academic Misconduct, and
   may be penalized by mark deductions, or by other penalties determined
   via the University of Melbourne Academic Honesty Policy, as described
   at https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will
   I do so until after the marks are released. I understand that providing
   my work to other students, regardless of my intention or any undertakings
   made to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring
   service, or drawing the attention of others to such services and code
   that may have been made available via such a service, may be regarded
   as Student General Misconduct (interfering with the teaching activities
   of the University and/or inciting others to commit Academic Misconduct).
   I understand that an allegation of Student General Misconduct may arise
   regardless of whether or not I personally make use of such solutions
   or sought benefit from such actions.

   Signed by: Oscar Rochanakij 1082645
   Dated:     19/10/19

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIMENSIONS 0    /* Line location of dimensions*/
#define START 1         /* Line location of start*/
#define GOAL 2          /* Line location of goal*/
#define BLOCK 3         /* Order of blocks in input*/
#define ROUTE 4         /* Order of route in input*/
#define BLOCKSLIST 5    /* Order of blocks for full repait in input*/
#define MAXCELLS 5      /* The max numnber cells to be printed on one line*/
#define ADJACENTCELLS 4 /* The Number of cells adjacent to one cell*/

#define STATUS1   1     /*"Initial cell in the route is wrong!"*/
#define STATUS2   2     /*"Goal cell in the route is wrong!"*/
#define STATUS3   3     /*"There is an illegal move in this route!"*/
#define STATUS4   4     /*"There is a block on this route!"*/
#define STATUS5   5     /*"The route is valid!"*/
#define STATUS6   6     /*"The route cannot be repaired!"*/

#define STAGE0    "==STAGE 0======================================="
#define STAGE1    "==STAGE 1======================================="
#define STAGE2    "==STAGE 2======================================="
#define GRIDEND   "------------------------------------------------"
#define PROGEND   "================================================"
#define PROPROUTE "The proposed route in the grid is:"

/**************************************************************************/

typedef struct node node_t;

typedef struct {
    int row;
    int col;
}point_t;

struct node {
    point_t point;
    int counter;
    node_t *next;
};


typedef struct {
	node_t *head;
	node_t *foot;
} list_t;

typedef struct {
    int rows;
    int columns;
    int status;
    list_t *blocks;
    char **grid;
    point_t start;
    point_t goal;
    list_t *route;
    list_t **blockslist;
    int blockslistlen;
}data_t;

/**************************************************************************/

/* function prototypes */

data_t *get_data();
void print_data(data_t *data);
char** format_grid();
void get_dims(data_t *data, char* line);
void get_loc(data_t *data, int loc, char* line);
point_t text_to_point(char* text);
void add_node(data_t *data,char *line,int type);
int get_num_block(node_t *blocks);
list_t *insert_at_foot(list_t *list, point_t value, int counter);
list_t *insert_at_head(list_t *list, point_t value);
list_t *make_empty_list(void);
void print_route(data_t *data);
void check_status(data_t *data);
void print_status(data_t *data);
list_t *insert_route(list_t *route, char* line);
int compare_point(point_t point1, point_t point2);
point_t get_route_end(data_t *data);
int valid_route(list_t *route, point_t botcorner);
int valid_point(point_t point, point_t botcorner);
int valid_move(point_t curpoint, point_t lastpoint);
point_t *check_block(list_t *route, char **grid);
void fill_grid(data_t *data, node_t *blocks);
point_t *get_sblocks(node_t *blockslist);
int cmppoint(const void *point1, const void *point2);
void print_grid(char **grid, int rows, int columns, list_t *route);
char *my_getline(char *line);
list_t *initialise_repair_queue(char **grid, list_t *route);
void repair_grid(data_t *data);
node_t *get_broken_route(data_t *data);
void add_adjacents(list_t *rqueue, node_t *broute, data_t *data,\
                   point_t point, int counter);
int adjacent_valid(point_t point, char **grid, int row, int col);
void traverse_queue(list_t *rqueue, node_t *broute, data_t *data);
int is_repeat(node_t *head, point_t point);
void repair_route(list_t *rqueue, node_t *broute, data_t *data,\
 node_t *repair);
list_t *construct_fragment(node_t *repair, node_t *rqueue);
int valid_repair(node_t *rqueue, point_t point, int counter);
void full_repair(data_t *data);
void add_list_to_blockslist(data_t *data, int index);
void single_repair(data_t *data);
void free_list(list_t *list);
/**************************************************************************/

/* main program controls all the action 
 */

int
main(int argc, char *argv[]){
    data_t *data;
    
    /* Stage 0: Get data from standard input then format and 
     * print the data
     */
    data = get_data();
    fill_grid(data, data->blocks->head);
    print_data(data);
    /* Stage 1: Print the grid before and after repair
    */
    single_repair(data);
    /* Stage 2: Print and do a full repair on all other entries of blocks
    */
    if(data->blockslistlen){
        full_repair(data);
    }
    else{
        printf("%s\n", PROGEND);
    } 
    return 0;
}
/**************************************************************************/
/* Stage 1: peform a single repair on the current block placement
*/
void
single_repair(data_t *data){
    printf("%s\n", STAGE1);
    print_grid(data->grid,data->rows,data->columns,data->route);
    /* Only perform repair if the status is blocked route
    */
    if(data->status == STATUS4){
        printf("%s\n", GRIDEND);
        repair_grid(data);
        print_grid(data->grid,data->rows,data->columns,data->route);
        printf("%s\n", GRIDEND);
        if(data->status != STATUS6){
            print_route(data);
            check_status(data);
        }
        print_status(data);
    }
    free_list(data->blocks);
}
/**************************************************************************/
/* Get a line of unspecified length from stdin and return it as a string
*/
char 
*my_getline(char* line){
    /*Start the index at 0 and the length of the line at 1*/
    int index = 0, len = 1;
    char next;
    line = (char*)malloc(sizeof(char));
    while((next = getchar())!='\n'&& next != EOF){
        if(index == len){
            /* Double the length of the input line each realloc
            */
            len *= 2;
            line = (char*)realloc(line, len*sizeof(char));
        }
        line[index] = next;
        index++;
    }
    if(next == EOF){
        return 0;
    }
    else{
        /* Handling if there is not enough space for the null byte*/
        if(index == len){
            /* Double the length of the input line each realloc
            */
            len *= 2;
            line = (char*)realloc(line, len*sizeof(char));
        }
        line[index]='\0';
    }
    return line;
}
/**************************************************************************/
/* Take the input lines from my_getline and process them as to store in
 * the main data struct.
*/
data_t
*get_data(){
    data_t *data;
    data = (data_t*)malloc(sizeof(*data));
    data->blocks = make_empty_list();
    data->route = make_empty_list();
    /* Begin the length at zero for the blocks list*/
    data->blockslistlen =0;
    data->blockslist = (list_t**)malloc(sizeof(list_t*));
    char *line = NULL;
    int counter = DIMENSIONS;
    while((line = my_getline(line))!= 0){
        if(counter == DIMENSIONS){
            get_dims(data, line);
            counter++; 
        }
        else if(counter == START || counter == GOAL){
            get_loc(data, counter, line);
            counter++; 
        }
        else if(counter == BLOCK){
            if(line[0] == '$'){
                counter++;
            }
            else{
                add_node(data, line, counter);
            }

        }
        else if(counter == ROUTE){
            if(line[0] == '$'){
                counter++;
                add_list_to_blockslist(data, counter-BLOCKSLIST);
            }
            else{
                add_node(data, line, counter);
            }
        }
        else if(counter >= BLOCKSLIST){
            if(line[0] == '$'){
                counter++;
                add_list_to_blockslist(data, counter-BLOCKSLIST);
            }
            else{
                add_node(data, line, counter);
            }
        }
    }
    return data;
}
/**************************************************************************/
/* Print the formatted output for stage 0 
*/
void 
print_data(data_t *data){
    int numblock;
    printf("%s\n", STAGE0);
    printf("The grid has %d rows and %d columns.\n",\
   data->rows,data->columns);
    numblock = get_num_block(data->blocks->head);
    printf("The grid has %d block(s).\n", numblock);
    printf("The initial cell in the grid is [%d,%d].\n",\
   data->start.row,data->start.col);
    printf("The goal cell in the grid is [%d,%d].\n",\
   data->goal.row,data->goal.col);
    printf("%s\n", PROPROUTE);
    print_route(data);
    check_status(data);
    print_status(data);
}
/**************************************************************************/
/* Take the input line and store the value of the dimensions in the main
 * data struct
*/
void
get_dims(data_t *data, char* line){
   data->rows = atoi(line);
    int i;
    for(i = 0; i<strlen(line);i++){
        if(line[i] == 'x'){
           data->columns = atoi(line+(i+1));
        }
    }
}
/**************************************************************************/
/* Take the input line and store the value of the start and end locations
 * in the main data struct
*/
void
get_loc(data_t *data, int loc, char* line){
    if(loc == START){
       data->start = text_to_point(line);
    }
    if(loc == GOAL){
       data->goal = text_to_point(line);
    }
}
/**************************************************************************/
/* Converts text into a point_t type given the format of the input [r,c]
 * with r = row and c = col
 */

point_t 
text_to_point(char* text){
    point_t point;
    point.row = atoi(text+1);
    int i;
    for(i = 0; i<strlen(text);i++){
        if(text[i] == ','){
            point.col = atoi(text+(i+1));
            break;
        }
    }
    return point;
}
/**************************************************************************/
/* Take a line from the input and given the parsed type of node either add
 * to the block list, the route list or the current index of the array of
 * blocks list
 */

void
add_node(data_t *data, char *line, int type){
    point_t point;
    if(type == BLOCK){
        point = text_to_point(line);
        data->blocks = insert_at_foot(data->blocks, point, 0);
    }
    else if(type == ROUTE){
       data->route = insert_route(data->route, line);
    }
    else if(type >= BLOCKSLIST){
        point = text_to_point(line);
        data->blockslist[type-BLOCKSLIST] = \
        insert_at_foot(data->blockslist[type-BLOCKSLIST], point, 0);
    }
}
/**************************************************************************/
/* Add another list to the array of blocks list by reallocating the array
 * to one that is one list_t pointer larger
 */
void add_list_to_blockslist(data_t *data, int index){
    data->blockslistlen++;
    data->blockslist = (list_t**)realloc(data->blockslist,\
    data->blockslistlen*sizeof(list_t*));
    data->blockslist[index] = make_empty_list();
            
}
/**************************************************************************/
/* Create and empty list with a null pointer at the head and tail
 * From the lecture 7 slides.
 */
list_t
*make_empty_list(void) {
	list_t *list;
	list = (list_t*)malloc(sizeof(*list));
    if(list == NULL){
        return NULL;
    }
	if(list==NULL){
        return NULL;
    }
	list->head = list->foot = NULL;
	return list;
}
/**************************************************************************/
/* Free up the locations in memory that the parsed list has taken up
 * From the lecture 7 slides 
 */
void
free_list(list_t *list) {
	node_t *curr, *prev;
	if(list==NULL){
        return;
    }
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(list);
}
/**************************************************************************/
/* Insert a point at the head of a list 
 * From the lecture 7 slides modified with points 
 */
list_t
*insert_at_head(list_t *list, point_t value) {
	node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	if(new == NULL){
        return list;
    }
	new->point = value;
	new->next = list->head;
    new->counter = 0;
	list->head = new;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->foot = new;
	}
	return list;
}
/**************************************************************************/
/* Insert a point at the tail of a list 
 * From the lecture 7 slides modified with points 
 */
list_t
*insert_at_foot(list_t *list, point_t value, int counter) {
	node_t *new;
	new = (node_t*)malloc(sizeof(*new));
    if(new == NULL){
        return list;
    }
	new->point = value;
	new->next = NULL;
    new->counter = counter;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = new;
        list->foot = new;
	} 
    else {
		list->foot->next = new;
		list->foot = new;
	}
	return list;
}
/**************************************************************************/
/* A function that takes in a node from a linked list and counts the 
 * amount of remaining nodes within the list
 */
int 
get_num_block(node_t *blocks){
    int count = 0;
    node_t *temp = blocks;
    if(temp == NULL){
        return 0;
    }
    while(temp != NULL){
        count++;
        temp = temp->next;
    }
    return count;
}
/**************************************************************************/
/* From the main data struct this function prints each point on the current
 * route in order at a the rate of 5 cells a line
 */
void 
print_route(data_t *data){
    int counter = 0;
    node_t *temp;
    temp =data->route->head;
    while(1){
        while(counter < MAXCELLS){
            if(temp != NULL){
                if(temp->next !=NULL){
                    printf("[%d,%d]->", temp->point.row, temp->point.col);
                }
                else{
                    /* The case for the last point in the route*/
                    printf("[%d,%d].", temp->point.row, temp->point.col);
                }
                temp = temp->next;
                counter++;
            }
            else{
                printf("\n");
                return;
            }
        }
        printf("\n");
        counter = 0;
    }
}

/**************************************************************************/
/* Takes a line and a pointer to the route list and inserts the point held
 * on the line at the foot of the route linked list
 */
list_t *insert_route(list_t *route, char* line){
    int position = 0;
    point_t point;
    while (position<strlen(line)){
        if(line[position]=='['){
            point = text_to_point(line+position);
            route = insert_at_foot(route, point, 0);
        }
        position++;
    }
    return route;
}
/**************************************************************************/
/* Compares two points and returns 1 if they are the same and 0 if not
 * 
 */
int compare_point(point_t point1, point_t point2){
    if(point1.col == point2.col && point1.row == point2.row){
        return 1;
    }
    return 0;
}
/**************************************************************************/
/* Takes the main data struct and stores the status of the route.
 */
void 
check_status(data_t *data){
    point_t routestart =data->route->head->point,\
    routeend, botcorner, *test=NULL;
    botcorner.row =data->rows;
    botcorner.col =data->columns;
    routeend = get_route_end(data);
    if(!compare_point(data->start, routestart)){
        data->status = STATUS1;
    }
    else if(!compare_point(data->goal, routeend)){
        data->status = STATUS2;
    }
    else if(!valid_route(data->route, botcorner)){
        data->status = STATUS3;
    }
    else if((test=check_block(data->route,data->grid))!=NULL){
        data->status = STATUS4;
    }
    else{
        data->status = STATUS5;
    }
}
/**************************************************************************/
/* Takes the main data struct and prints the status of the route.
 */
void 
print_status(data_t *data){
    if(data->status == STATUS1){
        printf("Initial cell in the route is wrong!\n");
    }
    else if(data->status == STATUS2){
        printf("Goal cell in the route is wrong!\n");
    }
    else if(data->status == STATUS3){
        printf("There is an illegal move in this route!\n");
    }
    else if(data->status == STATUS4){
        printf("There is a block on this route!\n");
    }
    else if(data->status == STATUS5){
        printf("The route is valid!\n");
    }
    else if(data->status == STATUS6){
        printf("The route cannot be repaired!\n");
    }
}
/**************************************************************************/
/* Takes the main data struct and returns the final point of the route.
 * 
 */
point_t get_route_end(data_t *data){
    point_t routeend;
    node_t *temp =data->route->head;
    while(temp != NULL){
        routeend = temp->point;
        temp = temp->next;
    }
    return routeend;
}
/**************************************************************************/
/* Checks the validity of a route and returns 1 if valid or 0 if invalid
 */
int valid_route(list_t *route, point_t botcorner){
    node_t *temp = route->head;
    point_t lastpoint,curpoint;
    lastpoint = curpoint = temp->point;
    if(!valid_point(curpoint, botcorner)){
        return 0;
    }
    temp = temp->next;
    while(temp != NULL){
        curpoint = temp->point;
        /* Check that the move is both adjacent and within the grid*/
        if(!valid_point(curpoint, botcorner)||\
           !valid_move(curpoint, lastpoint)){
            return 0;
        }
        lastpoint = curpoint;
        temp = temp->next;
        
    }
    return 1;
}
/**************************************************************************/
/* Checks the validity of a point by testing if it is within the bounds of
 * of the grid, and returns 1 if valid and 0 in not.
 */
int valid_point(point_t point, point_t botcorner){
    if(point.row < botcorner.row && point.row >= 0){
        if(point.col < botcorner.col && point.col >= 0){
            return 1;
        }
    }
    return 0;
}
/**************************************************************************/
/* Checks the validity of a move by testing if it is within +1 or -1 of the
 * last move in either row or col but not both and, not diagonal.
 * returns 1 if valid 0 if not.
 */
int valid_move(point_t curpoint, point_t lastpoint){
    if(curpoint.col + 1 == lastpoint.col ||\
       curpoint.col - 1 == lastpoint.col){
        if(curpoint.row == lastpoint.row){
            return 1;
        }
    }
    if(curpoint.row + 1 == lastpoint.row ||\
       curpoint.row - 1 == lastpoint.row){
        if(curpoint.col == lastpoint.col){
            return 1;
        }
    }
    return 0;
}
/**************************************************************************/
/* Checks if the route is blocked, returns the last non blocked point
 * if blocked or 0 if clear.
 */
point_t 
*check_block(list_t *route, char **grid){
    node_t *temp = route->head, *last = route->head;
    while(temp != NULL){
        if(grid[temp->point.row][temp->point.col] == '#'){
            return &last->point;
        }
        last = temp;
        temp = temp->next;
    }
    return NULL;
}
/**************************************************************************/
/* Fill the grid with ' 's and add '#'s where there is a block. Also add
 * 'I' to the initial cell and 'G' to the goal cell.
 */
void
fill_grid(data_t *data, node_t *blocks){
    char **grid;
    int i, j, k = 0;
    point_t *sortedblocks, point;
    sortedblocks = get_sblocks(blocks);
    grid = (char**)malloc(data->rows*sizeof(char*));
    if(grid == NULL){
        return;
    }
    for(i = 0; i<data->rows; i++){
        grid[i] = (char*)malloc(data->columns*sizeof(char));
        if(grid == NULL){
            return;
        }
    }
    /* Iterate throught the grid adding the required values*/
    for(i = 0; i<data->rows; i++){
        for(j = 0; j<data->columns; j++){
            point.row = i; point.col = j;
            if(compare_point(data->start, point)){
                grid[i][j] = 'I';
            }
            else if(compare_point(data->goal, point)){
                grid[i][j] = 'G';
            }
            else if(compare_point(sortedblocks[k], point)){
                grid[i][j] = '#';
                k++;
            }
            else{
                grid[i][j] = ' ';
            }
        }
    }
   data->grid = grid;

}
/**************************************************************************/
/* Sort the linked list of blocks as to be increasing by row then column.
 * And return as an array of points.
 */
point_t 
*get_sblocks(node_t *blockslist){
    int i, numblocks = get_num_block(blockslist);
    point_t *blocks;
    node_t *temp =blockslist;
    blocks = (point_t*)malloc(numblocks*sizeof(point_t));
    if(blocks == NULL){
        return NULL;
    }
    /*Loop throught the list of blocks adding to the array*/
    for(i = 0; i< numblocks; i++){
        blocks[i] = temp->point;
        temp = temp->next;
    }
    /*Then sort the array*/
    qsort(blocks, numblocks, sizeof(point_t), cmppoint);
    return blocks;
}
/**************************************************************************/
/* Comparator function for qsort with points, returning p1 > p2 if row is 
 * greater then at a lower rank if column is greater.
 */
int cmppoint(const void *p1, const void *p2){
    int p1r = ((point_t *)p1)->row;
    int p2r = ((point_t *)p2)->row;
    int p1c = ((point_t *)p1)->col;
    int p2c = ((point_t *)p2)->col;
    if(p1r!=p2r){
        return(p1r-p2r);
    }
    if(p1c!=p2c){
        return(p1c-p2c);
    }
    return 0;
}
/**************************************************************************/
/* Takes the pointer and dimensions of the grid and prints the grid
 */
void print_grid(char **grid, int rows, int columns, list_t *route){
    int i, j;
    point_t point;
    node_t *temp = route->head;
    while(temp != NULL){
        if(grid[temp->point.row][temp->point.col] != '#'&&\
            grid[temp->point.row][temp->point.col] != 'I'&&\
            grid[temp->point.row][temp->point.col] != 'G'){
            grid[temp->point.row][temp->point.col] = '*';
        }
        temp = temp->next;
    }
    /*for formating*/
    printf(" ");
    for(j = 0; j < columns;j++){
        printf("%d", j);
    }
    printf("\n");
    /*Iterate through the grid printing each value*/
    for(i = 0; i < rows;i++){
        /*Start the iteration at -1 to account for printing the 
         * numbers on the side
        */
        for(j = -1; j < columns; j++){
            point.row = i; point.col = j;
            if(j == -1){
                printf("%d", i);
            }
            else{
                if(grid[i][j]=='#'||grid[i][j]=='I'||grid[i][j]=='G'\
                || grid[i][j]=='*'){
                    printf("%c", grid[i][j]);
                    /* Clear the grid of the route after printing*/
                    if(grid[i][j]=='*'){
                        grid[i][j]=' ';
                    }
                }
                else{
                    printf(" ");
                }
            }
        }
        printf("\n");
    }
    
}
/**************************************************************************/
/* Repair the grid of any blockages altering the main data struct
*/
void
repair_grid(data_t *data){
    list_t *repairqueue;
    node_t *brokenroute;
    repairqueue = initialise_repair_queue(data->grid,data->route);
    brokenroute = get_broken_route(data);
    traverse_queue(repairqueue, brokenroute, data);
    if(data->status != STATUS6){
        check_status(data);
    }
}
/**************************************************************************/
/* Create a repair queue with the point before the blockage as the head
*/
list_t 
*initialise_repair_queue(char **grid, list_t *route){
    list_t *queue = make_empty_list();
    point_t segmentstart, *temp = check_block(route, grid);
    segmentstart.row = temp->row;segmentstart.col = temp->col;
    queue = insert_at_foot(queue, segmentstart, 0);
    return queue;
}
/**************************************************************************/
/* Get the route after the block to identify the repair targets
*/
node_t
*get_broken_route(data_t *data){
    point_t bpoint, *temppoint = check_block(data->route,data->grid);
    node_t *temp =data->route->head, *last = temp;
    bpoint.row = temppoint->row; bpoint.col = temppoint->col;
    /* Iterate through the current route until a block is encountered*/
    while(temp != NULL){
        if(compare_point(temp->point, bpoint)){
            last = temp;
        }
        temp = temp->next;
    }
    return last;
}
/**************************************************************************/
/* Traverse the queue adding while moving along and repair the broken route
*/
void
traverse_queue(list_t *rqueue, node_t *broute, data_t *data){
    node_t *temp = rqueue->head, *fixednode =broute;
    add_adjacents(rqueue, broute, data, temp->point, temp->counter);
    temp = temp->next;
    while(temp != NULL){
        /* If a point is found that connects to the rest of the route
           begin the repair.
        */
        if(is_repeat(broute, temp->point)){
            while(fixednode != NULL){
                if(compare_point(fixednode->point, temp->point)){
                    broute = fixednode;
                }
                fixednode = fixednode->next;
            }
            repair_route(rqueue, broute, data, temp);
            return;
        }
        /* Add all possible adjacent values for each point passd in the 
         * queue
         */
        add_adjacents(rqueue, broute, data, temp->point, temp->counter);
        temp = temp->next;
    }
    data->status = STATUS6;
}
/**************************************************************************/
/* Insert adjacent cells into the repair queue with a counter value of 
* the amount of moves required to get there. Check for duplicate cells
* already in the repair queue and that the cell is valid.
*/
void 
add_adjacents(list_t *rqueue, node_t *broute, data_t *data,\
                   point_t point, int counter){
    point_t temp;
    temp.row = point.row; temp.col = point.col;
    /* The cell above so subtract 1 from row*/
    temp.row -= 1;
    if(adjacent_valid(temp,data->grid,data->rows,data->columns)){
        if(!is_repeat(rqueue->head, temp)){
            rqueue = insert_at_foot(rqueue, temp, counter+1);
        }
    }
    /* The cell bellow so add 2 to row*/
    temp.row += 2;
    if(adjacent_valid(temp,data->grid,data->rows,data->columns)){
        if(!is_repeat(rqueue->head, temp)){
            rqueue = insert_at_foot(rqueue, temp, counter+1);
        }
    }
    /* The cell to the left so subtract 1 from row and 1 from col*/
    temp.row -= 1; temp.col -= 1;
    if(adjacent_valid(temp,data->grid,data->rows,data->columns)){
        if(!is_repeat(rqueue->head, temp)){
            rqueue = insert_at_foot(rqueue, temp, counter+1);
        }
    }
    /* The cell to the right so add 2 to col*/
    temp.col += 2;
    if(adjacent_valid(temp,data->grid,data->rows,data->columns)){
        if(!is_repeat(rqueue->head, temp)){
            rqueue = insert_at_foot(rqueue, temp, counter+1);
        }
    }
}
/**************************************************************************/
/* Check that the adjacent cell is valid by testing that it is within the 
 * grid and that it is not blocked.
*/
int
adjacent_valid(point_t point, char **grid, int row, int col){
    point_t botcorner;
    botcorner.row = row;
    botcorner.col = col;
    if(valid_point(point, botcorner)){
        if(grid[point.row][point.col]!='#'){
            return 1;
        }
    }
    return 0;
}
/**************************************************************************/
/* Given the head of a linked list and a point check if the point is in the
* the list. Return a 1 if the point is in the list and a 0 if not.
*/
int
is_repeat(node_t *head, point_t point){
    node_t *temp = head;
    while(temp != NULL){
        if(compare_point(point, temp->point)){
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}
/**************************************************************************/
/* Given the repair queue and the point where the route can be repaired
*  repair the route.
*/
void
repair_route(list_t *rqueue, node_t *broute, data_t *data, node_t *repair){
    point_t s, *temppoint = check_block(data->route, data->grid);
    node_t *temp = data->route->head;
    list_t *fragment = construct_fragment(repair, rqueue->head);
    s.row = temppoint->row; s.col = temppoint->col;    
    while(temp != NULL){
        if(compare_point(s, temp->point)){
            data->route->foot = temp;
            break;
        }
        temp = temp->next;
    }
    /* Make the point on the route where the path is blocked point to the
    * begining of the fragment
    */
    data->route->foot->next = fragment->head->next;
    data->route->foot = fragment->foot;
    /* Then make the point at the end of the fragment point to the point 
     * where it joins with the broken route
    */
    data->route->foot->next = broute->next;
    temp = broute;
    while(temp != NULL){
        temp = temp->next;
    }
    data->route->foot = temp;    
}
/**************************************************************************/
/* Take the repair queue and the point where the repair ends and work
 * backwards to identify fragment of repaired route 
*/
list_t
*construct_fragment(node_t *repair, node_t *rqueue){
    list_t *fragment = make_empty_list();
    point_t temp, point;
    int counter = repair->counter;
    fragment = insert_at_head(fragment, repair->point);
    /*Begin at the end of the fragment and work backwards adding values 
    from the repair queue if they are adjacent to the last point and
    the counter is one less */
    while(1){
        point = fragment->head->point;
        if(compare_point(point,rqueue->point)){
            break;
        }
        temp.row = point.row; temp.col = point.col;
        /* The cell above so subtract 1 from row*/
        temp.row -= 1;
        if(valid_repair(rqueue, temp, counter)){
            fragment = insert_at_head(fragment, temp);
            counter--;
            continue;
        }
         /* The cell bellow so add 2 to row*/
        temp.row += 2;
        if(valid_repair(rqueue, temp, counter)){
            fragment = insert_at_head(fragment, temp);  
            counter--;
            continue;      
            }
        /* The cell to the left so subtract 1 from row and 1 from col*/
        temp.row -= 1; temp.col -= 1;
        if(valid_repair(rqueue, temp, counter)){
            fragment = insert_at_head(fragment, temp);
            counter--;
            continue;        
        }
        /* The cell to the right so add 2 to col*/
        temp.col += 2;
        if(valid_repair(rqueue, temp, counter)){
            fragment = insert_at_head(fragment, temp); 
            counter--;
            continue;       
        }
        if(!counter){
            break;
        }
    }
    return fragment;
}
/**************************************************************************/
/* Check that a repair is valid if the counter is one less than the current
*  and the same point/adjacent from the previous.
*/
int
valid_repair(node_t *rqueue, point_t point, int counter){
    node_t *temp = rqueue;
    while(temp != NULL){
        if(compare_point(point, temp->point)){
            if(temp->counter == counter-1){
                return 1;
            }
        }
        temp = temp->next;
    }
    return 0;
}
/**************************************************************************/
/* Stage 2: Loop throught the stored lists of blocks printing a full repair
 * if possible */
void 
full_repair(data_t *data){
    int i = 0;
    node_t *temp;
    printf("%s\n", STAGE2);
    while(i<data->blockslistlen){
        /*Fill and print grid and route pre repairs*/
        temp = data->blockslist[i]->head;
        fill_grid(data, temp);
        print_grid(data->grid,data->rows,data->columns,data->route);
        check_status(data);
        /* Ignore repair if route is already valid*/
        if(data->status != STATUS5){
            printf("%s\n", GRIDEND);
            /* Continue to repair the route until success or failure*/
            while(data->status == STATUS4){
                repair_grid(data);
            }
            print_grid(data->grid,data->rows,data->columns,data->route);
            printf("%s\n", GRIDEND);
            if(data->status != STATUS6){
                print_route(data);
                check_status(data);
            }
            print_status(data);
        }
        free_list(data->blockslist[i]);
        printf("%s\n", PROGEND);
        i++;
    }
    free_list(data->route);
}
/**************************************************************************/
// Program end; algorithms are fun