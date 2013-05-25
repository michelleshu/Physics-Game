// This level.h file holds all of the function prototypes and structure definitions for
// allocating space for and initializing Level, Box, and Star.
// The structure type definitions are 

#ifndef LEVEL_H
#define LEVEL_H

#include <stdio.h>

//*************************** Type Definitions ********************************

/*
The Box structure holds all of the information about the box and its properties
*/

typedef struct box Box;
struct box {
	double mass;
	double x;
	double y;
	double width;
	double height;
	double red;
	double green;
	double blue;
};

typedef struct poly Poly;
struct poly {
	double mass;
	double c_x;
	double c_y;
	int num_vertices;
	double* vertices;
	double red;
	double green;
	double blue;
};

/*
The Star structure holds all of the information about the star and its properties
*/
typedef struct star Star;
struct star {
	double mass;
	double x;
	double y;
	double radius;
	double red;
	double green;
	double blue;
};

/*
The Ball structure holds the information about the ball and its properties
*/
typedef struct ball Ball;
struct ball {
	double mass;
	double x;
	double y;
	double radius;
	double red;
	double green;
	double blue;
};

/*
Goal is a line segment that denotes the bottom of the goal basket
*/
typedef struct goal Goal;
struct goal {
	double x1;
	double y1;
	double x2;
	double y2;
};

/*
The Level structure contains all of the information needed to load a world.
There is the level number, the initial number of boxes in each level, and
the number of stars in each level. Then there are two arrays that store pointers
to all of the boxes and stars that are initially present in each level.
*/
typedef struct level Level;
struct level {
	int level_number;
	int box_number;
	int poly_number;
	int star_number;
	double world_width;
	double world_height;
	Box* boxes;
	Poly* polygons;
	Star* stars;
	Ball ball;
	Goal goal;
};

//************************* Public Function Prototypes *************************

/*
Initialize structures
*/
Level* level_initialize(Level* l, char* file_name);

/*
Allocate space for the structure and load information from file
*/
Level* level_new (char* file_name);
Level* box_new (Level* l, FILE* file_pointer);
Level* poly_new (Level *l, FILE* file_pointer);
Level* star_new (Level* l, FILE* file_pointer);
Level* ball_new (Level *l, FILE* file_pointer);
Level* goal_new (Level *l, FILE* file_pointer);

/*
Free all memory allocated to level
*/
void level_destroy (Level* l);

/*
Return the appropriate filename for level_new to use
*/
char* level_file_name (const char* level_number);
	
/*
Open file, and make sure it isnt NULL
*/
FILE* level_file_open (char* file_name);

#endif
