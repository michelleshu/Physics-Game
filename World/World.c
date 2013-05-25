/*
World.c

Jacob Weiss

World describes the physical level of the game
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "World.h"
#include "../Chipmunk-Mac/chipmunk.h"
#include "../WorldPhysics/WorldPhysics.h"

#include "../Server/ServerPacketHandler.h"

typedef struct
{
	cpSpace* space;
	
	GHashTable* strokes;
	
	int num_strokes;
	
} _World;

#define DENSITY (7)
#define FRICTION (.9)

//**************************** Private Function Prototypes ***************************

/*
Allocate a World
*/
static _World* world_alloc();

/*
Initialize a World
*/
static _World* world_init(_World* world);

static cpBody* world_vertices_to_body(_World* world, cpVect* vertices, int num_vertices, cpVect* centroid, int user_id);
static double get_concavity(cpVect v0, cpVect v1, cpVect v2);
static int get_winding(const cpVect* vertices, int num_vertices);
static cpVect* triangulate_polygon(const cpVect* vertices_input, int num_vertices);
static cpVect triangle_get_centroid(cpVect a, cpVect b, cpVect c);
static double triangle_get_area(cpVect a, cpVect b, cpVect c);
static cpVect triangles_get_center_of_mass(cpVect* triangles, int num_triangles);
static double triangles_get_mass(cpVect* triangles, int num_triangles);
static double triangles_get_moment(cpVect* triangles, int num_triangles, cpVect axis);
static void fix_triangle_winding(cpVect* triangle);

static void world_detect_shape_overlap(cpBody *body, cpShape *shape, int* overlaps);
static void world_remove_shape(cpBody* body, cpShape* shape, void* data);

// These functions are taken from http://www.blackpawn.com/texts/pointinpoly/default.html
static int same_side(cpVect p1, cpVect p2, cpVect a, cpVect b);
static int point_in_triangle(cpVect p, cpVect a, cpVect b, cpVect c);

//************************* Public Function Implementations **************************

//Create a new world, allocate memory and initialize the struct
World* world_new()
{
	return (World*)world_init(world_alloc());
} 

//Destroy the world.  This also destroys the strokes affiliated with the world as well as the cpSpace associated with the world.
void world_destroy(World* world)
{
	// Cast the public struct to the private struct so that we can free its members.
	_World* w = (_World*)world;
		
	g_hash_table_destroy( w->strokes );
	
	cpSpaceFree(w->space);
	
	// Free the members, and then the struct itself
	free(w);
} 


//Remove data structres within the world so that that the same world instance can be used by the 
void world_clear( World* world )
{
	_World* w = (_World*) world ;
	
	g_hash_table_remove_all(w->strokes );	
}

void world_remove_stroke(World* world, int stroke_id)
{
	_World* w = (_World*)world;
	
	sph_send_delete(stroke_id);
	
	g_hash_table_remove(w->strokes, GINT_TO_POINTER(stroke_id));
}

void world_shape_iterate(cpShape *shape, World* world)
{
	cpBody* body = cpShapeGetBody(shape);
	
	BodyAttributes* attributes = shape->data;
	
	double body_x = body->p.x;
	double body_y = body->p.y;
	
	_World* w = (_World*) world;
	
	int id = w->num_strokes++;
	Stroke* s = stroke_new_full(id, body_x, body_y, SHAPE_TYPE_UNKNOWN, TRUE, 1, attributes->red, attributes->green, attributes->blue);	
				
	stroke_set_body_coordinates(s, &(body->p.x), &(body->p.y));
	stroke_set_body_rotation(s, &(body->a));
	
	switch (attributes->shape_type)
	{
		case RECTANGLE:
		{
			for (int i = 0; i < cpPolyShapeGetNumVerts(shape); i++)
			{
				cpVect vertex = cpPolyShapeGetVert(shape, i);
				stroke_append_point(s, vertex.x + body_x, vertex.y + body_y);
			}
			cpVect vertex = cpPolyShapeGetVert(shape, 0);
			stroke_append_point(s, vertex.x + body_x, vertex.y + body_y);
			break;
		}
		case POLYGON:
		{
			for (int i = 0; i < cpPolyShapeGetNumVerts(shape); i++)
			{
				cpVect vertex = cpPolyShapeGetVert(shape, i);
				stroke_append_point(s, vertex.x + body_x, vertex.y + body_y);
			}
			cpVect vertex = cpPolyShapeGetVert(shape, 0);
			stroke_append_point(s, vertex.x + body_x, vertex.y + body_y);
			break;
		}
		case STAR:
		{
			for (int i = 0; i < cpPolyShapeGetNumVerts(shape); i++)
			{
				cpVect vertex = cpPolyShapeGetVert(shape, (i * 2) % cpPolyShapeGetNumVerts(shape));
				stroke_append_point(s, vertex.x + body_x, vertex.y + body_y);
			}
			cpVect vertex = cpPolyShapeGetVert(shape, 0);
			stroke_append_point(s, vertex.x + body_x, vertex.y + body_y);
			break;
		}
		case SEGMENT:
		{
			cpVect a = cpSegmentShapeGetA(shape);
			cpVect b = cpSegmentShapeGetB(shape);
			
			stroke_append_point(s, a.x + body_x, a.y + body_y);
			stroke_append_point(s, b.x + body_x, b.y + body_y);
			
			break;
		}
		case BALL:
		{
			cpVect center = cpCircleShapeGetOffset(shape);
			cpFloat radius = cpCircleShapeGetRadius(shape);
			
			stroke_append_point(s, center.x + body_x, center.y + body_y);
			stroke_append_point(s, center.x + radius + body_x, center.y + radius + body_y);
			
			stroke_set_shape_type(s, SHAPE_TYPE_CIRCLE);
			
			break;			
		}
		case BORDER:
		{
			stroke_destroy(s);
			return;
		}
		default:
		{
			fprintf(stderr, "Loading Unknown Shape\n");
			stroke_destroy(s);
			return;
		}
	}	
	
	cpBodySetUserData(body, (cpDataPointer)s);
	
	g_hash_table_insert( w->strokes, GINT_TO_POINTER(id), (gpointer)s );
}

void world_load_space(World* world, cpSpace* space)
{
	((_World*)world)->space = space;
	cpSpaceEachShape(space, (cpSpaceShapeIteratorFunc)(&world_shape_iterate), world);
}

cpSpace* world_get_space( World* world )
{
	_World* w = (_World*) world;
	return w->space;
}

int world_new_stroke(World* world, double initial_x, double initial_y, ShapeType type, int fill, double width, double r, double g, double b)
{
	_World* w = (_World*)world;
	int id = w->num_strokes++;
	Stroke* s = stroke_new_full(id, initial_x, initial_y, type, fill, width, r, g, b);
	g_hash_table_insert( w->strokes, GINT_TO_POINTER(id), (gpointer) s );
	return id;
}

double world_add_to_stroke(World* world, int id, double x, double y)
{
	_World* w = (_World*)world;
	Stroke* s = g_hash_table_lookup( w->strokes, GINT_TO_POINTER(id) );
	return stroke_append_point(s, x, y);
}

Stroke* world_get_stroke(World* world, int id)
{
	_World* w = (_World*)world;
	return g_hash_table_lookup(w->strokes, GINT_TO_POINTER(id));
}

Stroke* world_finish_stroke(World* world, int id, int user_id)
{
	_World* w = (_World*)world;
	
	// Get the stroke to finish
	Stroke* s = g_hash_table_lookup( w->strokes, GINT_TO_POINTER(id) );
	
	// Analyze the shape of the stroke
	Stroke* analyzed = stroke_analyze_shape(s);
		
	double x, y;
	stroke_get_position(analyzed, &x, &y);
	int num_points = stroke_get_length(analyzed);
	ShapeType type = stroke_get_shape_type(analyzed);
	
	cpVect centroid;
	cpBody* body;
	
	if (type == SHAPE_TYPE_CIRCLE)
	{
		stroke_circle_get_center(analyzed, &centroid.x, &centroid.y);
		double radius = stroke_circle_get_radius(analyzed);
		double mass = G_PI * radius * radius * 1;
		double I = cpMomentForCircle(mass, 0, radius, cpv(0, 0));
		body = cpBodyNew(mass, I);
		cpCircleShape* circle = cpCircleShapeInit(cpCircleShapeAlloc(), body, radius, cpv(0, 0));
		cpShapeSetCollisionType((cpShape*)circle, user_id);
		cpShapeSetFriction((cpShape*)circle, FRICTION);
		cpSpaceAddShape(w->space, (cpShape*)circle);
	}
	else if (type == SHAPE_TYPE_POLYGON)
	{
		if (num_points < 3)
		{
			fprintf(stderr, "Stroke Invalid. < 3 Sides.\n");
			return NULL;
		}
		
		cpVect* vertices = (cpVect*)malloc(num_points * sizeof(cpVect));
	
		for (int i = 0; i < num_points; i++)
		{
			stroke_get_point(analyzed, i, &(vertices[i].x), &(vertices[i].y));
		}
	
		body = world_vertices_to_body(w, vertices, num_points, &centroid, user_id);
	
		if (body == NULL) // The function failed
		{
			g_hash_table_remove(w->strokes, GINT_TO_POINTER(id));
			return NULL;
		}
	}
	
	cpBodySetPos(body, cpv(x + centroid.x, y + centroid.y));
	cpSpaceAddBody(w->space, body);
			
	int body_overlaps = 0;
	cpBodyEachShape(body, (cpBodyShapeIteratorFunc)world_detect_shape_overlap, &body_overlaps);
	if (body_overlaps)
	{
		cpBodyEachShape(body, world_remove_shape, NULL);
		cpSpaceRemoveBody(w->space, body);
		g_hash_table_remove(w->strokes, GINT_TO_POINTER(id));
		return NULL;
	}
	
	g_hash_table_replace(w->strokes, GINT_TO_POINTER(id), (gpointer) analyzed);
	
	cpBodySetUserData(body, (cpDataPointer)analyzed);	
		
	stroke_set_body_coordinates(analyzed, &(body->p.x), &(body->p.y));
	stroke_set_body_rotation(analyzed, &(body->a));
	
	stroke_adjust_coordinate_frame(analyzed, -centroid.x, -centroid.y);
		
	return analyzed;
}

GList* world_get_stroke_list ( World* world )
{
	_World* w = (_World*) world;
	GList* list = g_hash_table_get_values(w->strokes);
	return list;
}

//************************* Private Function Implementations *************************

static _World* world_alloc()
{
	// I like dividing the allocation and initialization into two functions.
	_World* w = (_World*)malloc(sizeof(_World));
	
	// Make sure malloc worked.
	assert(w);
	
	// Return the struct.
	return w;
}

static _World* world_init(_World* world)
{
	// Initialize the struct that was passed in.
	world->strokes = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, (GDestroyNotify)stroke_destroy);
	
	world->num_strokes = 0;
	// Return the struct
	return world;
	
}

static void world_shapes_overlap(cpShape *shape, cpContactPointSet *points, void** data)
{
	long test_body = (long)cpShapeGetBody(shape);
	long this_body = (long)data[0];
	if (test_body != this_body);
	{
		data[1] = (cpBody*)((long)(data[1]) + 1);
	}
}

static void world_detect_shape_overlap(cpBody *body, cpShape *shape, int* overlaps)
{
	void** array = (void**)malloc(2 * sizeof(void*));
	array[0] = body;
	array[1] = 0;
	cpSpaceShapeQuery(cpBodyGetSpace(body), shape, (cpSpaceShapeQueryFunc)world_shapes_overlap, array);
	*overlaps = (int)(long)(array[1]);
	free(array);
}

static void world_remove_shape(cpBody* body, cpShape* shape, void* data)
{
	cpSpaceRemoveShape(cpBodyGetSpace(body), shape);
}

void world_collision_add_type(World* world, int user_id)
{
	_World* w = (_World*) world;
	
	world_physics_collision_add_type(w->space, user_id, world);
}


static cpBody* world_vertices_to_body(_World* world, cpVect* vertices, int num_vertices, cpVect* centroid, int user_id)
{
	cpBody* polygon_body;
	if (!cpPolyValidate(vertices, num_vertices))
	{
		cpVect* triangles = triangulate_polygon(vertices, num_vertices);
		int num_triangles = num_vertices - 2;
		
		if (triangles == NULL)
		{
			return NULL;
		}
		
		cpVect center_of_mass = triangles_get_center_of_mass(triangles, num_triangles);
		//printf("center of mass = (%f, %f)\n", center_of_mass.x, center_of_mass.y);
		*centroid = center_of_mass; // return the center of mass
		
		double mass = triangles_get_mass(triangles, num_triangles);
		double moment = triangles_get_moment(triangles, num_triangles, center_of_mass);
		
		//printf("New Shape: mass=%lf, moment=%lf\n", mass, moment);
		
		polygon_body = cpBodyNew(mass, moment);
				
		for (int i = 0; i < num_triangles * 3; i += 3)
		{
			cpVect* triangle = triangles + i;
			
			fix_triangle_winding(triangle);
			
			triangle[0].x -= center_of_mass.x;
			triangle[0].y -= center_of_mass.y;
			triangle[1].x -= center_of_mass.x;
			triangle[1].y -= center_of_mass.y;
			triangle[2].x -= center_of_mass.x;
			triangle[2].y -= center_of_mass.y;
			
			cpPolyShape* triangle_shape = cpPolyShapeInit(cpPolyShapeAlloc(), polygon_body, 3, triangle, cpv(0, 0));
			
			cpShapeSetCollisionType((cpShape*)triangle_shape, user_id);
			
			cpShapeSetFriction((cpShape*)triangle_shape, FRICTION);
			BodyAttributes *attributes = attributes_new(255, 0, 0, RECTANGLE);
			((cpShape*)triangle_shape)->data = attributes;
			
			cpSpaceAddShape(world->space, (cpShape*)triangle_shape);
		}
	}
	else
	{
		cpVect center_of_mass = cpCentroidForPoly(num_vertices, vertices);
		*centroid = center_of_mass;
		
		double area = cpAreaForPoly(num_vertices, vertices);
		double mass = area * DENSITY;
	
		polygon_body = cpBodyNew(mass, cpMomentForPoly(mass, num_vertices, vertices, cpv(0, 0)));
	
		cpRecenterPoly(num_vertices, vertices);
			
		cpPolyShape* polygon = cpPolyShapeInit(cpPolyShapeAlloc(), polygon_body, num_vertices, vertices, cpv(0, 0));
	
		cpShapeSetCollisionType((cpShape*)polygon, user_id);
	
		cpShapeSetFriction((cpShape*)polygon, FRICTION);

		BodyAttributes *attributes = attributes_new(255, 0, 0, RECTANGLE);
		((cpShape*)polygon)->data = attributes;

		cpSpaceAddShape(((_World*)world)->space, (cpShape*)polygon);
	}
	
	return polygon_body;
}

static void fix_triangle_winding(cpVect* triangle)
{
	if (get_winding(triangle, 3) == -1)
	{
		cpVect temp = triangle[1];
		triangle[1] = triangle[2];
		triangle[2] = temp;
	}
}


static double get_concavity(cpVect v0, cpVect v1, cpVect v2)
{
	cpVect dv0 = cpvsub(v1, v0);
	cpVect dv1 = cpvsub(v2, v1);
	
	return cpvcross(dv0, dv1);
}

static int get_winding(const cpVect* vertices, int num_vertices)
{
	double min_x = vertices[0].x;
	int min_index = 0;
	for (int i = 0; i < num_vertices; i++)
	{
		if (vertices[i].x < min_x)
		{
			min_x = vertices[i].x;
			min_index = i;
		}
	}
	
	// Do extra add so that v0 never goes negative.
	cpVect v0 = vertices[(min_index + num_vertices - 1) % num_vertices];
	cpVect v1 = vertices[min_index];
	cpVect v2 = vertices[(min_index + 1) % num_vertices];
	
	double c = get_concavity(v0, v1, v2);
	
	return -(int)(c/fabs(c));
}

static int same_side(cpVect p1, cpVect p2, cpVect a, cpVect b)
{
    double cp1 = cpvcross(cpvsub(b, a), cpvsub(p1, a));
    double cp2 = cpvcross(cpvsub(b, a), cpvsub(p2, a));
	
	return cp1 * cp2 >= 0;
}

static int point_in_triangle(cpVect p, cpVect a, cpVect b, cpVect c)
{
	return same_side(p, a, b, c) && same_side(p, b, a, c) && same_side(p, c, a, b);
}

static cpVect* triangulate_polygon(const cpVect* vertices_input, int num_vertices)
{
	cpVect* triangles = (cpVect*)malloc((num_vertices - 2) * 3 * sizeof(cpVect));
		
	GArray* vertices = g_array_new(FALSE, FALSE, sizeof(cpVect));
	g_array_append_vals(vertices, vertices_input, num_vertices);
		
	int triangle_count = 0;
	int i_0 = 0;
	int i_1 = 0;
	int i_2 = 0;
	
	gboolean triangle_was_found = TRUE;
	int winding = get_winding(vertices_input, num_vertices);
	
	while (triangle_count < num_vertices - 2)
	{		
		if (i_0 == 0)
		{
			if (!triangle_was_found)
			{
				fprintf(stderr, "Triangulation Failed. Invalid Polygon.\n");
				return NULL;
			}
			else
			{
				triangle_was_found = FALSE;
			}
		}

		i_1 = (i_0 + 1) % (num_vertices - triangle_count);
		i_2 = (i_1 + 1) % (num_vertices - triangle_count);
		
		cpVect vert_0 = g_array_index(vertices, cpVect, i_0);
		cpVect vert_1 = g_array_index(vertices, cpVect, i_1);
		cpVect vert_2 = g_array_index(vertices, cpVect, i_2);
 		
		double concavity = get_concavity(vert_0, vert_1, vert_2);
		//printf("%d %d %d concavity %f winding %d\n", i_0, i_1, i_2, concavity, winding);
		if (concavity * winding < 0)
		{
			gboolean found_triangle = TRUE;
			int p = (i_2 + 1) % (num_vertices - triangle_count);
			while (p != i_0)
			{
				if (point_in_triangle(g_array_index(vertices, cpVect, p), vert_0, vert_1, vert_2))
				{
					//printf("Triangle num = %d, Points = %d\n", triangle_count, num_vertices);
					i_0 = i_1;
					found_triangle = FALSE;
					break;
				}
				p = (p + 1) % (num_vertices - triangle_count);
			}
			if (found_triangle)
			{
				//printf("Found triangle: {(%f, %f), (%f, %f), (%f, %f)}\n", vert_0.x * 100, vert_0.y * 100, vert_1.x * 100, vert_1.y * 100, vert_2.x * 100, vert_2.y * 100);
				triangles[(triangle_count * 3)] = vert_0;
				triangles[(triangle_count * 3) + 1] = vert_1;
				triangles[(triangle_count * 3) + 2] = vert_2;
			
				g_array_remove_index(vertices, i_1);
			
				triangle_count++; 
				triangle_was_found = TRUE;
			}
		}
		else
		{
			i_0 = i_1;
		}
	}
	
	g_array_free(vertices, TRUE);
	
	return triangles;
}

static cpVect triangle_get_centroid(cpVect a, cpVect b, cpVect c)
{
	cpVect centroid;
	centroid.x = (a.x + b.x + c.x) / 3;
	centroid.y = (a.y + b.y + c.y) / 3;
	return centroid;
}

static double triangle_get_area(cpVect a, cpVect b, cpVect c)
{
	return fabs(cpvcross(cpvsub(b, a), cpvsub(b, c)) / 2);
}

static cpVect triangles_get_center_of_mass(cpVect* triangles, int num_triangles)
{
	double total_mass = 0;
	double x_weighted_mass = 0;
	double y_weighted_mass = 0;
	
	for (int i = 0; i < num_triangles * 3; i += 3)
	{		
		cpVect* triangle = triangles + i;
				
		cpVect centroid = triangle_get_centroid(triangle[0], triangle[1], triangle[2]);
		double area = triangle_get_area(triangle[0], triangle[1], triangle[2]);
		
		double density = DENSITY; // Assume constant density for now
		
		total_mass += density * area; // for now, assume density is 1
		x_weighted_mass += density * area * centroid.x;
		y_weighted_mass += density * area * centroid.y;
	}
	
	return cpv(x_weighted_mass / total_mass, y_weighted_mass / total_mass);
}

static double triangles_get_mass(cpVect* triangles, int num_triangles)
{
	double total_mass = 0;
	
	for (int i = 0; i < num_triangles * 3; i += 3)
	{
		cpVect* triangle = triangles + i;
		
		double area = triangle_get_area(triangle[0], triangle[1], triangle[2]);
		
		double density = DENSITY; // Assume constant density for now
		
		total_mass += density * area; // for now, assume density is 1
	}
	
	return total_mass;
}

static double triangles_get_moment(cpVect* triangles, int num_triangles, cpVect axis)
{
	double moment = 0;
	
	for (int i = 0; i < num_triangles * 3; i += 3)
	{
		cpVect* triangle = triangles + i;
		
		double d = DENSITY; // Assume constant density
		
		double a = triangle_get_area(triangle[0], triangle[1], triangle[2]);
		double b = cpvlength(cpvsub(triangle[0], triangle[1]));
		double h = 2.0 * a / b;
		double m = d * a;
		
		cpVect centroid = triangle_get_centroid(triangle[0], triangle[1], triangle[2]);
		
		double radius_sq = cpvlengthsq(cpvsub(centroid, axis));	
		
		// http://www.efunda.com/math/areas/triangle.cfm
		moment += m * ((h * b) * (b * b + a * b + a * a + h * h)) / 12 + m * radius_sq;
	}
	
	return moment;
}













