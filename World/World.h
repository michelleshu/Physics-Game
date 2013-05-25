/*
World.h

Public Header File

Jacob Weiss

World describes the simulation components of the game. It provides information to the 
graphics subsystem on what to draw, and gets information from the GUI on what objects
to create. It also gets information from the Core about level loading and such.
*/

#ifndef WORLD_H
#define WORLD_H

#include <glib.h>
#include "../Stroke/Stroke.h"
#include "../Chipmunk-Mac/chipmunk.h" // This is a local include statement!!!
	
typedef struct World World;

//**************************** Public Function Prototypes ***************************

/*
This function allocates, initializes, and returns, one World structure.
*/
World* world_new();

/*
This function destroys the specified World structure.
*/
void world_destroy(World* world);

/*
This function clears all the information within the world so that a new one can be loaded into the structure.
*/

void world_clear( World* world );


/*
Calls the server packet handler to update all the client strokes
*/
void world_send_updates(World* world);

/*
Load a cp space into the world
*/
void world_load_space(World* world, cpSpace* space);

cpSpace* world_get_space( World* world );


void world_collision_add_type(World* world, int user_id);

void world_remove_stroke(World* world, int stroke_id);
Stroke* world_get_stroke(World* world, int id);

/*
Call to request that a new stroke be created. 
The returned integer is the unique ID associated with the newly created stroke.
*/
int world_new_stroke(World* world, double initial_x, double initial_y, ShapeType type, int fill, double width, double r, double g, double b);

/*
Call to add a point to the stroke associated with ID.
*/
double world_add_to_stroke(World* world, int id, double x, double y);

/*
Call to signal the end of a stroke.
*/
Stroke* world_finish_stroke(World* world, int id, int user_id);

/*
This is a temporary function

When networking is implemented, parallel data stuctures must be kept.
*/
GList* world_get_stroke_list ( World* world );

/*
Loads a world from a structure
*/
void world_load(void* structure);


/*
Saves the world to a structure
*/
void* world_save();

#endif