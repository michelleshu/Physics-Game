/*
ClientWorld.c

Jacob Weiss

World describes the current state of the physical world of the game, from the client's perspective.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <gtk/gtk.h>
	
#include "ClientWorld.h"
#include "../Stroke/Stroke.h"
	
typedef struct
{	
	//GArray* strokes;
	GHashTable* strokes;	
} _ClientWorld;

//**************************** Private Function Prototypes ***************************

/*
Allocate a World
*/
static _ClientWorld* client_world_alloc();

/*
Initialize a World
*/
static _ClientWorld* client_world_init(_ClientWorld* world);

//************************* Public Function Implementations **************************

ClientWorld* client_world_get()
{
	static _ClientWorld* world = NULL;
	if (world == NULL)
	{
		world = (_ClientWorld*)client_world_new();
	}
	return (ClientWorld*)world;
}

ClientWorld* client_world_new()
{
	return (ClientWorld*)client_world_init(client_world_alloc());
} 

void client_world_destroy(ClientWorld* world)
{
	// Cast the public struct to the private struct so that we can free its members.
	_ClientWorld* w = (_ClientWorld*)world;
	
	for (int i = 0; i < g_list_length(client_world_get_strokes()); i++)
	{
		// Free the storkes here
	}
	
	// Free the hash table
		
	// Free the members, and then the struct itself
	free(w);
}

GList* client_world_get_strokes()
{
	_ClientWorld* world = (_ClientWorld*)client_world_get();
	
	GList* list = g_hash_table_get_values(world->strokes);
	return list;
}

void client_world_new_stroke(int id, double initial_x, double initial_y, char type, char fill, double width, double r, double g, double b)
{
	_ClientWorld* world = (_ClientWorld*)client_world_get();
	
	Stroke* stroke;
	if (type == SHAPE_TYPE_SKETCH)
	{
		stroke = stroke_new_full(id, initial_x, initial_y, type, fill, width, r, g, b);
	}
	else
	{
		stroke = stroke_new_full(id, 0, 0, type, fill, width, r, g, b);
	}
	
	g_hash_table_insert(world->strokes, GINT_TO_POINTER(id), (gpointer)stroke);
}

void client_world_add_to_stroke(int id, double x, double y)
{
	_ClientWorld* world = (_ClientWorld*)client_world_get();
	
	Stroke* stroke = g_hash_table_lookup(world->strokes, GINT_TO_POINTER(id));
	
	stroke_append_point(stroke, x, y);
}

void client_world_finish_stroke(int id)
{
	// I don't know what to do here, exactly
	// I'm not actually sure whether this is necessary or not
}

void client_world_update_stroke(int id, double x, double y, double theta)
{
	_ClientWorld* world = (_ClientWorld*)client_world_get();
	
	Stroke* stroke = g_hash_table_lookup(world->strokes, GINT_TO_POINTER(id));
	
	if (stroke)
	{
		stroke_set_position(stroke, x, y);
		stroke_set_rotation(stroke, theta);
	}
}

void client_world_delete_stroke(int id)
{
	_ClientWorld* world = (_ClientWorld*)client_world_get();
	
	g_hash_table_remove(world->strokes, GINT_TO_POINTER(id));
}

void client_world_clear()
{
	_ClientWorld* world = (_ClientWorld*)client_world_get();
	
	g_hash_table_remove_all(world->strokes);
}

//************************* Private Function Implementations *************************

static _ClientWorld* client_world_alloc()
{
	// I like dividing the allocation and initialization into two functions.
	_ClientWorld* w = (_ClientWorld*)malloc(sizeof(_ClientWorld));
	
	// Make sure malloc worked.
	assert(w);
	
	// Return the struct.
	return w;
}


static _ClientWorld* client_world_init(_ClientWorld* world)
{
	// Initialize the struct that was passed in.
	//world->strokes = g_array_new(FALSE, FALSE, sizeof(Stroke*));
	world->strokes = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, (GDestroyNotify)stroke_destroy);
	
	// Return the struct
	return world;
}














