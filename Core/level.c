/*
level.c

Joyce Chung
Michelle Shu
*/


// This level.c file contains all of the functions needed to allocate space for 
// structures and substructures under Level, and intializes those structures with
// information loaded from .lvl files.

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <assert.h>
#include "level.h"

#define STRINGSIZE 8

//************************* Private Function Prototypes *************************

/*
Allocate space for structures
*/
static Level* level_allocate();
static Box* box_allocate();
static Poly* poly_allocate();
static Star* star_allocate();
static Ball* ball_allocate();
static Goal* goal_allocate();

//************************* Private Function Implementation *************************

/*
Allocate space for structure Level
*/
static Level* level_allocate()
{
	// allocate memory, make sure malloc works, then return struct
	Level* l = (Level *) malloc(sizeof(Level));
	assert(l);
	return l;
}

/*
Allocate space for structure Box
*/
static Box* box_allocate(void)
{
	// allocate memory, make sure malloc works, then return struct
	Box* b = (Box *) malloc(sizeof(Box));
	assert(b);
	return b;
}

/*
Allocate space for structure Poly
*/
static Poly* poly_allocate()
{
	Poly* p = (Poly *) malloc(sizeof(Poly));
	assert(p);
	return p;
}

/*
Allocate space for structure Star
*/
static Star* star_allocate()
{
	// allocate memory, make sure malloc works, then return struct
	Star* s = (Star *) malloc(sizeof(Star));
	assert(s);
	return s;
}

/*
Allocate space for structure Ball
*/
static Ball* ball_allocate()
{
	// allocate memory, make sure malloc works, then return struct
	Ball* b = (Ball *) malloc(sizeof(Ball));
	assert(b);
	return b;
}

/*
Allocate space for structure Goal
*/
static Goal* goal_allocate()
{
	Goal* g = (Goal *) malloc(sizeof(Goal));
	assert(g);
	return g;
}

//************************* Public Function Implementations *************************

/*
Allocate memory for and initialize boxes by scanning information from level.lvl
*/
Level* box_new (Level* l, FILE* file_pointer)
{
	int i = 0;
    char box_string[STRINGSIZE];
    
	for (i = 0; i < (l->box_number); i++) {
		l->boxes[i] = *box_allocate();
        fscanf(file_pointer, "%s", box_string);
		fscanf(file_pointer, "%lf", &(&l->boxes[i])->mass);
		fscanf(file_pointer, "%lf", &(&l->boxes[i])->x);
		fscanf(file_pointer, "%lf", &(&l->boxes[i])->y);
        fscanf(file_pointer, "%lf", &(&l->boxes[i])->width);
        fscanf(file_pointer, "%lf", &(&l->boxes[i])->height);
        fscanf(file_pointer, "%lf", &(&l->boxes[i])->red);
		fscanf(file_pointer, "%lf", &(&l->boxes[i])->green);
		fscanf(file_pointer, "%lf", &(&l->boxes[i])->blue);
	}

	return l;
}

/*
Allocate memory for polygons by scanning information from level.lvl
*/
Level* poly_new(Level* l, FILE* file_pointer)
{
	int i = 0;
	char poly_string[STRINGSIZE];
	for (i = 0; i < (l->poly_number); i++) 
	{
		l->polygons[i] = *poly_allocate();
        fscanf(file_pointer, "%s", poly_string);
		fscanf(file_pointer, "%lf", &(&l->polygons[i])->mass);
		fscanf(file_pointer, "%lf", &(&l->polygons[i])->c_x);	// x pos of center
		fscanf(file_pointer, "%lf", &(&l->polygons[i])->c_y);	// y pos of center
		fscanf(file_pointer, "%d", &(&l->polygons[i])->num_vertices);
		
		// Add the x and y positions of the vertices relative to the center of the polygon
		(&l->polygons[i])->vertices = malloc(2 * (&l->polygons[i])->num_vertices * sizeof(double));
		int j = 0;
		for (j = 0; j < 2 * (&l->polygons[i])->num_vertices; j += 2)
		{
			fscanf(file_pointer, "%lf", &(&l->polygons[i])->vertices[j]);
			fscanf(file_pointer, "%lf", &(&l->polygons[i])->vertices[j + 1]);
		}
		
        fscanf(file_pointer, "%lf", &(&l->polygons[i])->red);
		fscanf(file_pointer, "%lf", &(&l->polygons[i])->green);
		fscanf(file_pointer, "%lf", &(&l->polygons[i])->blue);
	}

	return l;	
}

/*
Allocate memory for and initialize stars by scanning information from level.lvl
*/
Level* star_new (Level* l, FILE* file_pointer)
{
	int i = 0;
    char star_string[STRINGSIZE];
    
	for (i = 0; i < l->star_number; i++) {
		l->stars[i] = *star_allocate();
        fscanf(file_pointer, "%s", star_string);
        fscanf(file_pointer, "%lf", &(&l->stars[i])->mass);
        fscanf(file_pointer, "%lf", &(&l->stars[i])->x);
        fscanf(file_pointer, "%lf", &(&l->stars[i])->y);
		fscanf(file_pointer, "%lf", &(&l->stars[i])->radius);
		fscanf(file_pointer, "%lf", &(&l->stars[i])->red);
		fscanf(file_pointer, "%lf", &(&l->stars[i])->blue);
		fscanf(file_pointer, "%lf", &(&l->stars[i])->green);
	}
	
	return l;
}

/*
Allocate memory for and initialize the ball by scanning information from level.lvl
*/
Level* ball_new (Level* l, FILE* file_pointer)
{
    char ball_string[STRINGSIZE];

	l->ball = *ball_allocate();
	Ball *ball = &(l->ball);
    fscanf(file_pointer, "%s", ball_string);
	fscanf(file_pointer, "%lf", &(ball->mass));
    fscanf(file_pointer, "%lf", &(ball->x));
    fscanf(file_pointer, "%lf", &(ball->y));
	fscanf(file_pointer, "%lf", &(ball->radius));
	fscanf(file_pointer, "%lf", &(ball->red));
	fscanf(file_pointer, "%lf", &(ball->green));
	fscanf(file_pointer, "%lf", &(ball->blue));
	
	//printf( "Ball loaded with mass: %lf\n", ball->mass);
	
	return l;
}

/*
Allocate memory for and initialize the ball by scanning information from level.lvl
*/
Level* goal_new (Level* l, FILE* file_pointer)
{
    char goal_string[STRINGSIZE];
	
	l->goal = *goal_allocate();	
	Goal *goal = &(l->goal);
    fscanf(file_pointer, "%s", goal_string);
	fscanf(file_pointer, "%lf", &(goal->x1));
    fscanf(file_pointer, "%lf", &(goal->y1));
    fscanf(file_pointer, "%lf", &(goal->x2));
	fscanf(file_pointer, "%lf", &(goal->y2));
	
	//printf("Goal is loaded at (%5.2lf, %5.2lf), (%5.2lf, %5.2lf)", goal->x1, goal->y1, goal->x2, goal->y2);
	
	return l;
}

/*
 Load the level file at this filename into structure level
 */
Level* level_initialize(Level* l, char* file_name)
{
	FILE* file_pointer;
    char level_string[STRINGSIZE];
	char world_string[STRINGSIZE];
	
	// check that file_name's extension is a .lvl
	file_pointer = level_file_open(file_name);
    
	// load information from file into level struct
    fscanf(file_pointer, "%s", level_string);
	fscanf(file_pointer, "%i", &l->level_number);
	fscanf(file_pointer, "%i", &l->box_number);
	fscanf(file_pointer, "%i", &l->poly_number);
    fscanf(file_pointer, "%i", &l->star_number);
    fscanf(file_pointer, "%s", world_string);
    fscanf(file_pointer, "%lf", &l->world_width);
    fscanf(file_pointer, "%lf", &l->world_height);
	l->boxes = (Box *) malloc(l->box_number * sizeof(Box));
	l->polygons = (Poly *)malloc(l->poly_number * sizeof(Poly));
	l->stars = (Star *) malloc(l->star_number * sizeof(Star));

	// debugging messages
	//fprintf(stderr, "%s %i %i %i\n", level_string, l->level_number, l->box_number, l->star_number);
	//fprintf(stderr, "%s %lf %lf\n", world_string, l->world_width, l->world_height);

	// create box and star structs and initialize
	l = box_new(l, file_pointer);
	l = poly_new(l, file_pointer);
	l = star_new(l, file_pointer);
	l = ball_new(l, file_pointer);
	l = goal_new(l, file_pointer);
	//printf("Goal loaded.\n");
    
	// close file
    fclose(file_pointer);
	return l;
}

/*
Allocate memory for and initialize the Level structure
*/
Level* level_new (char* file_name)
{
	return (Level *) level_initialize(level_allocate(), file_name);
}

/*
Return a filename depending on the level selected, to be used by level_new
*/
char* level_file_name (const char* level_number)
{
	int i = atoi(level_number);
	char* file_name;
	
	switch (i) {
		case 1:
			file_name = "level.lvl";
			break;
		case 2:
			file_name = "level.lvl";
			break;
		case 3:
			file_name = "level.lvl";
			break;
		case 4:
			file_name = "/Core/level.lvl";
			break;
		case 5:
			file_name = "/Core/level.lvl";
			break;
		case 6:
			file_name = "/Core/level.lvl";
			break;
		case 7:
			file_name = "/Core/level.lvl";
			break;
		case 8:
			file_name = "/Core/level.lvl";
			break;
		case 9:
			file_name = "/Core/level.lvl";
			break;
		case 10:
			file_name = "/Core/level.lvl";
			break;
		default:
			file_name = "/Core/level.lvl";
			break;
	}
	return file_name;
}

/*
Open the .lvl file, and perform all checks on the file
*/
FILE* level_file_open (char* file_name)
{
	FILE* file_pointer;
	
	// check that the file is the right format
    char* temp;
    temp = strrchr(file_name, '.');
    
        if(!strcmp(temp, ".lvl")) {
			file_pointer = fopen(file_name, "r");
			
			// check that the file is not empty
			if (file_pointer == NULL) {
				fprintf(stderr, "Error opening file.\n");
			}
		} else {
            fprintf(stderr, "wrong type of file.\n");
            return (NULL);
        }
    
    return (file_pointer);
}

/*
Free allocated memory for the Level structure
*/
void level_destroy (Level* l)
{
	
	// freeing the array of pointers to the boxes
	free(l->boxes);
	
	// To free the polygons, we must first go through and free all their vertex arrays.
	int i = 0;
	for (i = 0; i < l->poly_number; i++)
	{
		free((&l->polygons[i])->vertices);
	}
	
	// free the array of pointers to the polygons
	free(l->polygons);
	
	// freeing the array of pointers to the stars
	free(l->stars);
    
	// freeing the overall level structure
	free(l);
}
