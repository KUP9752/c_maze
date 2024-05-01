/**
 * @file maze.c
 * @author (YOUR NAME)
 * @brief Code for the maze game for COMP1921 Assignment 2
 * NOTE - You can remove or edit this file however you like - this is just a provided skeleton code
 * which may be useful to anyone who did not complete assignment 1.
 */

#include <stdio.h>
#include <stdlib.h>

// defines for max and min permitted dimensions
#define MAX_DIM 100
#define MIN_DIM 5

// defines for the required autograder exit codes
#define EXIT_SUCCESS 0
#define EXIT_ARG_ERROR 1
#define EXIT_FILE_ERROR 2
#define EXIT_MAZE_ERROR 3

#define EXIT_MOVEMENT_ERROR 100

#define ERR_READING_FILE 0

#define OBSTACLE '#'

#define START 'S'
#define END 'E'
#define EMPTY ' '

#define UP ('W' | 'w')
#define LEFT ('A' | 'a')
#define DOWN ('S' | 's')
#define RIGHT ('D' | 'd')

typedef struct __Coord
{
    int x;
    int y;
} coord;

typedef struct __Maze
{
    char **map;
    int height;
    int width;
    coord start;
    coord end;
} maze;

/**
 * @brief Initialise a maze object - allocate memory and set attributes
 *
 * @param this pointer to the maze to be initialised
 * @param height height to allocate
 * @param width width to allocate
 * @return int 0 on success, 1 on fail
 */
int create_maze(maze *this, int height, int width)
{

  // Following might not ne necessary I think we validate later

  // // min height and width is 5
  // if (height < 5 || width < 5) return 1;

  // // max height and width is 5
  // if (height > 100 || width > 100) return 1;

  this->height = height;
  this->width = width;

  this->map = calloc(height, sizeof(char *));

  if (this->map == NULL) return EXIT_ARG_ERROR; // initial array failed to allocate

  // otherwise the initial char* is created so we can allocate each column:
  for (int i = 0; i < width; i++) {
    this->map[i] = calloc(height, sizeof(char));
    // likewise checkt he allocation succeeded
    if (this->map[i] == NULL) return EXIT_ARG_ERROR;
  }
  
  return EXIT_SUCCESS;
}

/**
 * @brief Free the memory allocated to the maze struct
 *
 * @param this the pointer to the struct to free
 */
void free_maze(maze *this)
{
  // need to start freeing from the deepest memory address contained 
  // otherwwise they will be unreachable and hang

  // free each column
  for(int i = 0; i < this->width; i++) {
    free(this->map[i]);
  }

  //free the row holding structure
  free(this->map);

  free(this);
  
}

/**
 * @brief Validate and return the width of the mazefile
 *
 * @param file the file pointer to check
 * @return int 0 for error, or a valid width (5-100)
 */
int get_width(FILE *file)
{
  int currentWidth = 0;
  int prevWidth;
  char ch;
  while ((ch = getc(file)) != EOF)  {
    if (ch == '\n') {
      if (currentWidth != prevWidth) return ERR_READING_FILE; // error not not a rectangle
      prevWidth = currentWidth; // TODO: might be a better way, this wastes a variable
    }
    currentWidth++;
  }

  // is width in [5, 10] range
  if (prevWidth < 5 || prevWidth > 100) return ERR_READING_FILE;

  return prevWidth;
}

/**
 * @brief Validate and return the height of the mazefile
 *
 * @param file the file pointer to check
 * @return int 0 for error, or a valid height (5-100)
 */
int get_height(FILE *file)
{
  int lineCount = 0;
  char ch;
  while ((ch = getc(file)) != EOF) {
    if (ch == '\n') lineCount++;
  }

  // is height in [5, 100] range
  if (lineCount < 5 || lineCount > 100) return ERR_READING_FILE;
  return lineCount; 
  // TODO: Check if the example files have a '\n' on the last line which means a - 1 is needed
}

/**
 * @brief read in a maze file into a struct
 *
 * @param this Maze struct to be used
 * @param file Maze file pointer
 * @return int 0 on success, 1 on fail
 */

 // NOT SURE IF I CAN but imma add my player here cuz it makes it a lot simpler to mark start
int read_maze(maze *this, coord *player, FILE *file)
{
  char ch;

  // Assume validated/created the maze (this), width and the height

  int i = 0; // row count
  int j = 0; // column count
  while ((ch = getc(file)) != EOF) {
    switch (ch) {
      case START:
        player->x = j;
        player->y = i;
      case END:
      case EMPTY:
      case OBSTACLE:
        this->map[i][j] = ch;
        j++;
        break;
      case '\n': // reset col go to next line
        j = 0;
        i ++;
        break;
      default: //any other ch cases is an error
        return EXIT_ARG_ERROR;
    }
  }

  return EXIT_SUCCESS; // all failures up till now should have been handled
}

/**
 * @brief Prints the maze out - code provided to ensure correct formatting
 *
 * @param this pointer to maze to print
 * @param player the current player location
 */
void print_maze(maze *this, coord *player)
{
    // make sure we have a leading newline..
    printf("\n");
    for (int i = 0; i < this->height; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            // decide whether player is on this spot or not
            if (player->x == j && player->y == i)
            {
                printf("X");
            }
            else
            {
                printf("%c", this->map[i][j]);
            }
        }
        // end each row with a newline.
        printf("\n");
    }
}

/**
 * @brief Validates and performs a movement in a given direction
 *
 * @param this Maze struct
 * @param player The player's current position
 * @param direction The desired direction to move in
 */
void move(maze *this, coord *player, char direction)
{
  // Assuming the coordinates work like:
  /*    x
        --> 
       | #########
     y | #       #
       V #       #
         #       #
         #       #
         #########
  */

  // check if player is within the maze (not sure if necessary)
  if (
    player->x > this->width || player->x < 0
    && player->y > this->height || player->y < 0
  ) return; // player not within the maze
  

  switch (direction) {
    case UP:
      // !! up means y - 1
      // movement if: not at the top or above not obstacle
      if (
        player->y > 0  
        && this->map[player->y - 1][player->x] != OBSTACLE
      ) player->y--; 
      break;
    case LEFT:
      // movement if: not at the very left or right not obstacle
      if (
        player->x > 0
        && this->map[player->y][player->x - 1] != OBSTACLE
      ) player->x--;
      break;
    case DOWN:
      // movement if: not at the bottom or below not obstacle
      if (
        player->y < this->height  
        && this->map[player->y + 1][player->x] != OBSTACLE
      ) player->y++; 
      break;
    case RIGHT:
      // movement if: not at the very right or right not obstacle
      if (
        player->x < this->width 
        && this->map[player->y][player->x + 1] != OBSTACLE
      ) player->x++; 
      break;
    default:
      break;
    // no other cases present, include default for completeness
    // and validation of correct input
  }


}

/**
 * @brief Check whether the player has won and return a pseudo-boolean
 *
 * @param this current maze
 * @param player player position
 * @return int 0 for false, 1 for true
 */
int has_won(maze *this, coord *player)
{
  return 1 ? this->map[player->y][player->x] == END : 0;
}

int main(int argc, char **argv)
{
    // check args
    if (argc != 2) return EXIT_ARG_ERROR; // we need exactly 2 arguments argc also counts the program name

    // !! in the extension argc == 4 is also accepted

    // set up some useful variables (you can rename or remove these if you want)
    coord *player = malloc(sizeof(coord));
    maze *this_maze = malloc(sizeof(maze));
    FILE *f;
    
    // open and validate mazefile
    
    f = fopen(argv[1], "r"); // readonly
    int height;
    int width;

    //validate h and w
    //??  not sure if these should be file errors or arg errors 
    if ((height = get_height(f)) == ERR_READING_FILE) return EXIT_ARG_ERROR;
    if ((width = get_width(f)) == ERR_READING_FILE) return EXIT_ARG_ERROR;

    if (create_maze(this_maze, height, width) == EXIT_ARG_ERROR) return EXIT_FILE_ERROR;

    // read in mazefile to struct
    if (read_maze(this_maze, player, f) == ERR_READING_FILE) return EXIT_MAZE_ERROR;

    fclose(f); //cleanup the file not useful anymore

    // maze game loop
    while (!has_won(this_maze, player)) { //0/1 treated as false/true
      char direction;
      // maybe you dont have to check this but i did why not, scanf can fail
      if (scanf("%c", &direction) != 1) return EXIT_MOVEMENT_ERROR;
      move(this_maze, player, direction);
    }
    // win
    
    // return, free, exit
    free(player);
    free_maze(this_maze);
    free(this_maze);

    return EXIT_SUCCESS
}