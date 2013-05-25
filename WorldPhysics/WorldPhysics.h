/*
WorldPhysics.h

Public Header File

Jacob Weiss
Modified by Michelle Shu

World describes the simulation components of the game. It provides information to the 
graphics subsystem on what to draw, and gets information from the GUI on what objects
to create. It also gets information from the Core about level loading and such.
*/

#ifndef WORLD_PHYSICS_H
#define WORLD_PHYSICS_H

#include <gtk/gtk.h>
#include "../Chipmunk-Mac/chipmunk.h"
#include "../Core/level.h"
#include "../World/World.h"
	
typedef enum SHAPE_TYPE {
	RECTANGLE,
	POLYGON,
	STAR,
	BALL,
	SEGMENT,
	BORDER
} SHAPE_TYPE;

typedef struct {
	double red;
	double green;
	double blue;
	SHAPE_TYPE shape_type;
} BodyAttributes;

//**************************** Public Function Prototypes ***************************

/*
This function allocates, initializes, and returns, one World structure.
*/
cpSpace *world_physics_new(Level *level);

/*
This function destroys the specified World structure.
*/
void world_physics_destroy(cpSpace *world);

/* Add collision type to world */
void world_physics_collision_add_type(cpSpace *space, int collision_type, World* world);

/*
Steps the simulation of the world by one time increment
*/
gboolean world_physics_step(cpSpace *world);

/* 
Removes a shape from the world
*/
void world_physics_remove_shapes(cpBody* body, cpShape* shape, cpSpace* space);

/* Load the physics world specified by the level */
void world_physics_load(cpSpace *world, Level *level);

/* Clear contents of physics world */
void world_physics_clear(cpSpace* space);

/* Create new attributes struct to store data about a body */
BodyAttributes *attributes_new(double red, double green, double blue,
		SHAPE_TYPE shape_type);
#endif
