/*
WorldPhysics.c

Michelle Shu

*/

#include <stdlib.h>

#include "WorldPhysics.h"
#include "../Core/level.h"
#include "../Server/ServerPacketHandler.h"
#include <math.h>
	
#define GRAVITY (.5)
#define FRICTION (0.5)
#define TIME_STEP (1.0 / 60.0)
#define BOX_DENSITY (10)	// density of box in kg/m^2
#define DESTRUCT_COLLISION (0)	// Collision type that results in destruction of object
#define NO_COLLISION (1)
#define WIN_COLLISION (2)	// Collision that results in level win (ball is in basket)
	
//**************************** Private Function Prototypes ***************************

static void world_physics_load_box(cpSpace *world, Level* level, double mass, double x, double y,
	double width, double height, double red, double green, double blue);
static void world_physics_load_poly(cpSpace *world, Level* level, double mass, double c_x, double c_y, 
	int num_vertices, double* vertices, double red, double green, double blue);
static void world_physics_load_star(cpSpace *world, Level* level, double mass, double x, double y,
	double radius, int red, int green, int blue);
static void world_physics_load_goal(cpSpace *world, Level* level, double x1, double y1, double x2, double y2);
static cpBody *world_physics_load_ball(cpSpace *world, Level* level, double mass, double x, double y, double radius, double angle, 
	double red, double green, double blue);

static void world_physics_add_destruct_border(cpSpace *world);

static int world_physics_body_collision_callback(cpArbiter *arb, cpSpace* space, World *world);
static int world_physics_border_collision_callback(cpArbiter *arb, cpSpace* space, World *world);
static int world_physics_win_collision_callback(cpArbiter *arb, cpSpace* space, World *world);
 
static void world_physics_add_type_poststep(cpSpace* space, int* collision_type, World *world);
static void world_physics_collision_poststep(cpSpace* space, cpBody *body, World *world);


//************************* Public Function Implementations **************************

/*
Allocates, initializes, and returns a world represented by a cpSpace. Maintains
level struct as user data pointer, to maintain information about the current
configuration of the world.
*/
cpSpace* world_physics_new(Level* level)
{
	// Set gravity for our world
	cpVect gravity = cpv(0, GRAVITY);

	// Create an empty space.
	cpSpace *space = cpSpaceNew();
	cpSpaceSetCollisionSlop(space, .01);
	cpSpaceSetSleepTimeThreshold(space, 1);
	cpSpaceSetCollisionPersistence(space, 10);
	cpSpaceSetGravity(space, gravity);

	world_physics_add_destruct_border(space);

	// Load the world from the level
	world_physics_load(space, level);

	// Save level information to the data pointer of the world.
	space->data = level;
	
	g_timeout_add_full(G_PRIORITY_HIGH, TIME_STEP * 1000, (GSourceFunc)(&world_physics_step), space, NULL);

	return space;
}

/* Remove shape from physics world */
void world_physics_remove_shape(cpBody* body, cpShape* shape, cpSpace* space)
{
	if (cpSpaceContainsShape(space, shape))
	{
		cpSpaceRemoveShape(space, shape);
	}
	cpShapeFree(shape);
}

/* Poststep for same body type collision to actually remove the bodies from cpSpace */
void world_physics_remove_body_poststep(cpSpace* space, cpBody* body, void* data)
{
	cpBodyEachShape(body, (cpBodyShapeIteratorFunc)world_physics_remove_shape, space);
	cpSpaceRemoveBody(space, body);
	cpBodyFree(body);
}

/* Remove bodies from space after callback */
void world_physics_remove_body(cpBody* body, cpSpace* space)
{	
	cpSpaceAddPostStepCallback(space, (cpPostStepFunc)world_physics_remove_body_poststep, body, NULL);
}


void world_physics_clear(cpSpace* space)
{
	cpSpaceEachBody(space, (cpSpaceBodyIteratorFunc)world_physics_remove_body, space);
	cpBodyEachShape(cpSpaceGetStaticBody(space), (cpBodyShapeIteratorFunc)world_physics_remove_shape, space);
}

/*
Destroy the entire cpSpace.
*/
void world_physics_destroy(cpSpace *world)
{
	cpSpaceFree(world);
}

/* Add collision handler for new "type" (user who created it) */
void world_physics_collision_add_type(cpSpace *space, int collision_type, World* world) 
{
	int* col = (int*)malloc(sizeof(int));
	*col = collision_type;
	
	cpSpaceAddPostStepCallback(space, (cpPostStepFunc)world_physics_add_type_poststep, col, world);
}

/* Removes a shape from the world */
void world_physics_remove_shapes(cpBody* body, cpShape* shape, cpSpace* space)
{
	cpSpaceRemoveShape(space, shape);
	cpShapeFree(shape);
}

/*
Steps the simulation of the world by one time increment
*/
gboolean world_physics_step(cpSpace *world)
{
	cpSpaceStep(world, TIME_STEP);
	
	/*
	printf("Step\n");
	cpSpaceEachShape(world, print_shapes, NULL);
	*/
	
	return TRUE;
}


//************************* Private Function Implementations **************************

/* Add the destruct border around the space that destroys shapes that exit the space. */
static void world_physics_add_destruct_border(cpSpace *space) 
{	
	// Add a border around the perimeter of the window that will destroy objects that come in contact with it
	cpShape *destruct_border_bottom = cpSegmentShapeNew(space->staticBody, cpv(-1, 2), cpv(2, 2), 0);
	cpShape *destruct_border_left = cpSegmentShapeNew(space->staticBody, cpv(-1, -1), cpv(-1, 2), 0);
	cpShape *destruct_border_right = cpSegmentShapeNew(space->staticBody, cpv(2, -1), cpv(2, 2), 0);
	
	cpSpaceAddShape(space, destruct_border_bottom);
	cpSpaceAddShape(space, destruct_border_left);
	cpSpaceAddShape(space, destruct_border_right);
	
	BodyAttributes *attributes = attributes_new(0, 0, 0, BORDER);
	destruct_border_bottom->data = attributes;
	destruct_border_left->data = attributes;
	destruct_border_right->data = attributes;
	
	/* Set collision type of destruct border */
	cpShapeSetCollisionType(destruct_border_bottom, (cpCollisionType) DESTRUCT_COLLISION);
	cpShapeSetCollisionType(destruct_border_left, (cpCollisionType) DESTRUCT_COLLISION);
	cpShapeSetCollisionType(destruct_border_right, (cpCollisionType) DESTRUCT_COLLISION);
}


/* Handle two types of collisions when new user is added.
 	Object and other object both destruct when they are same collision type.
 	Only the original object self-destructs when it hits a destruct border */
static void world_physics_add_type_poststep(cpSpace *space, int* collision_type, World* world) 
{
	cpSpaceAddCollisionHandler(space, 
							*collision_type, *collision_type, 
							(cpCollisionBeginFunc)world_physics_body_collision_callback, NULL, NULL, NULL, world);
	cpSpaceAddCollisionHandler(space,
							*collision_type, DESTRUCT_COLLISION,
							(cpCollisionBeginFunc)world_physics_border_collision_callback, NULL, NULL, NULL, world);
	free(collision_type);
}

/* Collision callback for when two cpShapes collide */
static int world_physics_body_collision_callback(cpArbiter *arb, cpSpace *space, World* world) 
{
	cpBody *body_a, *body_b;
	cpArbiterGetBodies(arb, &body_a, &body_b);
	
	/* Bodies belong to same user, so call a post step callback to safely remove the bodies and the shapes 
	from the space */
	cpSpaceAddPostStepCallback(space, (cpPostStepFunc)world_physics_collision_poststep, body_a, world);
	cpSpaceAddPostStepCallback(space, (cpPostStepFunc)world_physics_collision_poststep, body_b, world);

	return 0;
}

/* Collision callback for when shape collides with destruct border */
static int world_physics_border_collision_callback(cpArbiter *arb, cpSpace *space, World* world) 
{
	cpShape *shape_a, *shape_b;
	cpArbiterGetShapes(arb, &shape_a, &shape_b);
	
	/* Body collided with border. Destroy it. The object will always be shape_a, because border collision type is 0. */
	cpSpaceAddPostStepCallback(space, (cpPostStepFunc)world_physics_collision_poststep, shape_a->body, world);

	return 0;
}

/* After collision occurs, remove the bodies and strokes associated with collided objects */
static void world_physics_collision_poststep(cpSpace* space, cpBody *body, World* world) 
{	
	cpBodyEachShape(body, (cpBodyShapeIteratorFunc)world_physics_remove_shape, space);
	
	// Remove the stroke
	
	Stroke* stroke = cpBodyGetUserData(body);
	int id = stroke_get_id(stroke);
	world_remove_stroke(world, id);
	
	cpSpaceRemoveBody(space, body);
	cpBodyFree(body);
}

/* Responds to level win event */
static int world_physics_win_collision_callback(cpArbiter *arb, cpSpace *space, World* world)
{	
	printf("Level win collision callback called.\n");
	sph_send_win();
	
	return 0;
}

/*
Loads a world from a level structure
*/
void world_physics_load(cpSpace *world, Level *level)
{
	printf("Loading Level %d\n", level->level_number);
	
	// Initialize boxes in world.
	for (int i = 0; i < level->box_number; i++) 
	{
		Box b = (level->boxes)[i];
		world_physics_load_box(world, level, b.mass, b.x, b.y, b.width, b.height,
				b.red, b.green, b.blue);
		//printf("Box has loaded\n");
	}
	
	// Initialize polygons in world.
	for (int i = 0; i < level->poly_number; i++) 
	{
		Poly p = (level->polygons)[i];
		world_physics_load_poly(world, level, p.mass, p.c_x, p.c_y, p.num_vertices, p.vertices, p.red, 
			p.green, p.blue);
		//printf("Polygon has loaded.\n");
	}

	// Initialize stars in world.
	for (int i = 0; i < level->star_number; i++) 
	{
		Star star = (level->stars)[i];
		//printf("Star mass: %lf\nStar position: (%lf, %lf)\n", star.mass, star.x, star.y);
		world_physics_load_star(world, level, star.mass, star.x, star.y, star.radius,
				star.red, star.green, star.blue);
		//printf("Star has loaded\n");
	}
	
	// Initialize the ball in world.
	Ball ball = level->ball;
	world_physics_load_ball(world, level, ball.mass, ball.x, ball.y, ball.radius, 0, ball.red, ball.green, ball.blue);
	
	// Initialize the segment denoting the goal.
	Goal goal = level->goal;
	world_physics_load_goal(world, level, goal.x1, goal.y1, goal.x2, goal.y2);
	//printf("Goal Loaded at (%lf, %lf), (%lf, %lf)\n", goal.x1, goal.y1, goal.x2, goal.y2);
	
	// Handle collision for level win.
	cpSpaceAddCollisionHandler(world,
							(cpCollisionType) WIN_COLLISION, (cpCollisionType) WIN_COLLISION,
							(cpCollisionBeginFunc)world_physics_win_collision_callback, NULL, NULL, NULL, NULL);
}


/************ Functions for loading individual shapes into the space from the level file **************************/

/*
Load a ball into world from graphics.
*/
static cpBody *world_physics_load_ball(cpSpace *world, Level* level, double mass, double x, double y, double radius, double angle, 
	double red, double green, double blue)
{	
	cpFloat moment = cpMomentForCircle(mass, 0, radius / level->world_width, cpvzero);
	
	cpBody *ballBody = cpSpaceAddBody(world, cpBodyNew(mass, moment));
	cpBodySetPos(ballBody, cpv(x / level->world_width, y / level->world_height));
	
	cpCircleShape* ballShape = cpCircleShapeInit(cpCircleShapeAlloc(), ballBody, radius / level->world_width, cpvzero);
	// Set the collision type of the ball to detect level win
	cpShapeSetCollisionType((cpShape*)ballShape, (cpCollisionType) WIN_COLLISION);
	
	cpSpaceAddShape(world, (cpShape*)ballShape);
	
	cpShapeSetFriction((cpShape*)ballShape, FRICTION / 2);
	
	BodyAttributes *attributes = attributes_new(red, green, blue, BALL);
	((cpShape*)ballShape)->data = attributes;
	
	return ballBody;
}

/*
Load box into the world from the existing level. 
*/
static void world_physics_load_box(cpSpace *world, Level* level, double mass, double x, double y,
	double width, double height, double red, double green, double blue)
{
	cpVect boxVerts[] = 
	{ 
		cpv((-width / 2) / level->world_width, (-height / 2) / level->world_height), 
		cpv((-width / 2) / level->world_width, (height / 2) / level->world_height), 
		cpv((width / 2) / level->world_width, (height / 2) / level->world_height), 
		cpv((width / 2) / level->world_width, -(height / 2) / level->world_height) 
	};

	cpVect offset = cpv(x / level->world_width, y / level->world_height);

	// Add the box shape to the static body.
	cpShape *boxShape = cpSpaceAddShape(world, cpPolyShapeNew(world->staticBody, 4, boxVerts, offset));
	cpShapeSetCollisionType((cpShape*)boxShape, (cpCollisionType) NO_COLLISION);
	
	cpShapeSetFriction(boxShape, FRICTION);

	// Save the color and shape of the box to the data pointer of the body.
	BodyAttributes *attributes = attributes_new(red, green, blue, RECTANGLE);
	boxShape->data = attributes;

	//printf("world_load_box: Box loaded.\n");
}

/*
Load polygon into the world from the existing level.
*/
static void world_physics_load_poly(cpSpace *world, Level* level, double mass, double c_x, double c_y, 
	int num_vertices, double* vertices, double red, double green, double blue)
{
	cpVect polyVerts[num_vertices];
	int i;
	for (i = 0; i < num_vertices; i++)
	{
		polyVerts[i] = cpv((vertices[2 * i] / level->world_width), (vertices[2 * i + 1] / level->world_height));
	}

	cpVect offset = cpv(c_x / level->world_width, c_y / level->world_height);

	// Add the polygon shape to the static body.
	cpShape *polyShape = cpSpaceAddShape(world, cpPolyShapeNew(world->staticBody, num_vertices, polyVerts, offset));
	cpShapeSetCollisionType((cpShape*)polyShape, NO_COLLISION);
	
	cpShapeSetFriction(polyShape, FRICTION);

	// Save the color and shape of the box to the data pointer of the body.
	BodyAttributes *attributes = attributes_new(red, green, blue, POLYGON);
	polyShape->data = attributes;

	//printf("world_load_poly: Polygon loaded.\n");
}

/*
Load a star into the world from the existing level. The star will be attached to
the staticBody of the world.
*/
static void world_physics_load_star(cpSpace *world, Level* level, double mass, double x, double y,
	double radius, int red, int green, int blue)
{
	// Get the vertices of the star (represent as a pentagon)
	cpVect starVerts[] = 
	{ 
		cpv(0, radius / level->world_height), 
		cpv(cos(G_PI / 180 * 18) * radius / level->world_width, sin(G_PI / 180 * 18) * radius / level->world_height),
		cpv(cos(G_PI / 180 * 306) * radius / level->world_width, sin(G_PI / 180 * 306) * radius / level->world_height),
		cpv(cos(G_PI / 180 * 234) * radius / level->world_width, sin(G_PI / 180 * 234) * radius / level->world_height),
		cpv(cos(G_PI / 180 * 162) * radius / level->world_width, sin(G_PI / 180 * 162) * radius / level->world_height)
	};

	cpVect offset = cpv(x / level->world_width, y / level->world_height);

	// Add a pentagon shape to represent the star
	cpShape *starShape = cpSpaceAddShape(world, cpPolyShapeNew(world->staticBody, 5, starVerts, offset));
	cpShapeSetCollisionType((cpShape*)starShape, NO_COLLISION);
	
	cpShapeSetFriction(starShape, FRICTION);

	// Save the color and shape of the star to the data pointer of the body.
	BodyAttributes *attributes = attributes_new(red, green, blue, STAR);
	starShape->data = attributes;

	//printf("world_load_star: Star loaded.\n");
}

/* Loads the goal */
static void world_physics_load_goal(cpSpace *world, Level* level, double x1, double y1, double x2, double y2)
{	
	// Load a segment at the goal that will act as a sensor
	cpShape *goal = cpSegmentShapeNew(world->staticBody, cpv(x1 / level->world_width, y1 / level->world_height), 
		cpv(x2 / level->world_width, y2 / level->world_height), 0.01);
	cpSpaceAddShape(world, goal);
	
	BodyAttributes *attributes = attributes_new(1, 1, 1, SEGMENT);
	goal->data = attributes;
		
	cpShapeSetSensor(goal, TRUE); // Set up this shape as a sensor
	cpShapeSetCollisionType(goal, (cpCollisionType) WIN_COLLISION);

	//printf("world_load_goal: Goal loaded.\n");
}

BodyAttributes *attributes_new(double red, double green, double blue,
		SHAPE_TYPE shape_type) {
	BodyAttributes *attributes = (BodyAttributes *) malloc(sizeof(BodyAttributes));
	attributes->red = red;
	attributes->green = green;
	attributes->blue = blue;
	attributes->shape_type = shape_type;
	return attributes;
}
