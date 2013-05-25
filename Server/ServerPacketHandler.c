
/*
ServerPacketHandler.c

Eliza Hanson
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "../Core/level.h"
#include "../WorldPhysics/WorldPhysics.h"
#include "../Chipmunk-Mac/chipmunk.h" 
#include "../World/World.h"
#include "ServerPacketHandler.h"
#include "Server.h"
#include "../Stroke/Stroke.h"
	
#define NO_STROKE -1

char* const LEVEL_LIST[] = { "./Core/level1.lvl", "./Core/level2.lvl", "./Core/level3.lvl", "./Core/level4.lvl" };

// ***************** Private Function Headers **************

static void send_update(Stroke* stroke, gpointer data);
static void load_new_world( int level, World* world );
static void sph_send_world_stroke ( Stroke* stroke, void* unused );
static void sph_send_world_to_client(World* w, Connection* c);
static void sph_send_stroke_to_client(Stroke* stroke, Connection* c);
static void sph_send_new_stroke(int id, double x, double y, char type, char fill, double width, double r, double g, double b);
static void sph_send_add_point(int id, double x, double y);
static void sph_send_update(int id, double x, double y, double theta);
static void sph_send_stroke(Stroke* stroke);

// ***************** Public Functions ******************

/*
This function will send the entire world to all clients currently connected to the server.  It is used whenever an entirely new level is loaded.
 */
void sph_send_world_updates(World* w)
{
	GList* list = world_get_stroke_list( w );
	g_list_foreach( list, (GFunc)send_update, NULL );
}

/*
This function tells all clients to delete a specified stroke from representation.  This is used whenever a stroke is transitioned from the array of mouse points to the geometricized shape.
 */
void sph_send_delete(int id)
{
	Packet* packet = packet_delete_new(id);
	server_send_packet(packet);
}

/*
 This function tells the clients that a win has been detected so a level won dialouge will pop up.
 */
void sph_send_win()
{
	Packet* packet = packet_new( PACKET_TYPE_LEVEL_WIN );
	server_send_packet( packet );
}

/*
 * This function makes sure that all packet types coming from the client are handled according to their type.  The switch moves between the different enum cases of the packets which are recieved by poping them off the revieve queue.
 */
gboolean sph_process_queue( World* world )
{

	// WE ARE TRYING TO KEEP THE QUEUES WITHIN THE SERVER SO USE THE PUBLIC FUNCTIONS TO PUSH AND POP
		
	//Get the wrapper which from the queue this holds both the packet and the connection from which the data came.
	//Because there might not always be information, to be sent, use ..._try_pop() and handle the tries which do not work.
	Wrapper* wrapper;
	while (( wrapper = server_receive_packet()))  //SHOULD THIS REALLY BE A WHILE LOOP? IM THINKING IT MIGHT BE BETTER AS AN IF
	{
		
		//Unpackage the wrapper
		Connection* c = wrapper->c;
		Packet* packet = wrapper->packet;
		
		PacketType type = packet_get_type ( packet );
		
		//printf( "Handle new packet of type %d\n", (int) type );
		
		//Use the switch case to control how you handle each type of packet.
		switch ( type ) 
		{
			//In this case we need to receive the message from the queue, get the username from the connection and put it into a SEND_NAME packet then put both the username and the message into a packet packet before pushing the whole thing onto the send_queue
			case PACKET_TYPE_SEND_MESSAGE:
			{
				//We already have the message packet in packet.
				
				//Get the user_name and create a SEND_NAME packet
				Packet* name_packet = packet_new( PACKET_TYPE_SEND_NAME );
				packet_add_string( name_packet, c->user_name, SEND_NAME_NAME );
				
				//Create packet packet.
				Packet* packet_packet = packet_new( PACKET_TYPE_PACKETS );
				//Add the user_name packet to the packet packet
				packet_add_packet( &packet_packet, name_packet );
				
				//Add the message packet to the packet packet
				packet_add_packet( &packet_packet, packet );
								
				//Now that the packet packet is constructed, push it on to the send queue
				server_send_packet( packet_packet );			
				
				//Free the memory in the name packet because it is no longer needed.
				free( name_packet );
				
				break;
			}
			//Get the username stored in the packet and add it to the connection.  The string needs to be copied out of the memory because the packet memory will be freed.
			case PACKET_TYPE_SEND_NAME:
			{
				//NOTE HERE::: IM NOT SURE IF THIS IS SUFFICIENT TO GET RID OF OLD DATA.  PAY ATTENTION.
				strcpy( c->user_name, packet_get_string( packet, SEND_NAME_NAME ) );
				
				break;
			}
			//In this case there is a new stroke so get the initial points from the packet and create a new stroke.  The ID for the stroke should be stored in the client so that a particular client continues adding to the same stroke later on.
			case PACKET_TYPE_MOUSE_DOWN:
			{
				//Get all of the infomation out of the packet create a new stroke structure within the world to represent it.
				double init_x = packet_get_double(packet, MOUSE_DOWN_X);
				double init_y = packet_get_double(packet, MOUSE_DOWN_Y);
				ShapeType type = (ShapeType)packet_get_byte(packet, MOUSE_DOWN_SHAPE_TYPE);
				int fill = (int)packet_get_byte(packet, MOUSE_DOWN_FILL);
				double width = packet_get_double(packet, MOUSE_DOWN_WIDTH);
				double r = packet_get_double(packet, MOUSE_DOWN_R);
				double g = packet_get_double(packet, MOUSE_DOWN_G);
				double b = packet_get_double(packet, MOUSE_DOWN_B);
				
				//Because the client will continue to add points to the stroke, return the id number of the stroke in the hash table.  This will temporarily be associated with the connection structure.
				int id = world_new_stroke(world, init_x, init_y, type, fill, width, r, g, b);
					
				//Clients also draw the stroke as it is being created so send the stroke information back out over the network.
				sph_send_new_stroke(id, init_x, init_y, (char)type, (char)fill, width, r, g, b);
													
				c->current_id = id;
				break;
			}
			//This where the mouse has moved so the clients drawing is being made more complex.  This needs to add to the shape, detect length and inform all clients of the changes as well.
			case PACKET_TYPE_MOUSE_MOVE:
			{
				//Get the information from the packet
				double move_x = packet_get_double( packet, MOUSE_MOVE_X );
				double move_y = packet_get_double( packet, MOUSE_MOVE_Y );
				
				//Detect the length of the perimeter of the stroke already created.
				Stroke* s = world_get_stroke(world, c->current_id);
				int ink = stroke_get_remaining_ink(s);
					
				Packet* ink_pot = packet_new(PACKET_TYPE_INK_POT);
				packet_add_byte(ink_pot, ink, INK_POT_INK);
				server_send_packet_to(ink_pot, c);
				
				// If the perimeter is above the permissable length, do not allow the physics structure or the client representations to be updated.
				if (ink != 0)
				{
					sph_send_add_point(c->current_id, move_x, move_y);
				
					world_add_to_stroke( world, c->current_id, move_x, move_y );
				}								
				break;
			}
			//The client has finished createing the stroke, transform the stroke from user input to a condensed shape.  This shape should replace the previous ones in the hash tables.
			case PACKET_TYPE_MOUSE_UP:
			{
				int id = c->current_id;
				
				//Delete the client representations of the user input
				sph_send_delete(id);
					
				Stroke* s = world_get_stroke(world, c->current_id);
				
				//Only create the full stroke if conditions of length and the shape being closed are true.
				// Short circuit on purpose here. Stroke_is_closed will fail if length == 0.
				if (stroke_get_length(s) > 0 && stroke_is_closed(s))
				{					
					Stroke* to_send = world_finish_stroke(world, id, c->file_descriptor);
					
					if (to_send)
					{
						sph_send_stroke(to_send);
					}
				}
				else
				{
					world_remove_stroke(world, id);
				}
				
				//Reset the temporary information associated with creating a stroke.
				c->current_id = NO_STROKE;
				Packet* ink_pot = packet_new(PACKET_TYPE_INK_POT);
					
				packet_add_byte(ink_pot, (char)255, INK_POT_INK);
				server_send_packet_to(ink_pot, c);
				
				//printf( "Finished creating a new stroke from connection %d\n", c->file_descriptor);
				
				break;
			}
			//If there is a new connection, send only that connection the entire world.
			case PACKET_TYPE_NEW_CONNECTION:
			{
				// Send all the current strokes over to the client when it connects
				//But only if a world is currently defined.
				sph_send_world_to_client(world, c);
				world_collision_add_type(world, c->file_descriptor);
				break;
			} 
			//If a new level has been selected, load it.
			case PACKET_TYPE_LOAD_LEVEL:
			{
				char level_id = packet_get_byte( packet, LOAD_LEVEL_LEVEL );
				//printf( "Need to load level: %c\n" , level_id );
				//CALL A FUNCITON TO DEAL WITH THE LOAD LEVEL STUFF.
				load_new_world( (int) level_id, world );
				break;
			}
			case PACKET_TYPE_UNDEFINED:
			{
				//We still havent decided what to do if we get PACKET_TYPE_UNDEFINED.  We should either fail or just let it go without doing anything to it.
				break;
			}
			default:
			{
				printf("Yarrrgghh!\n");
			}
		}
	
	free ( wrapper );
	//THIS MAY CAUSE PROBLEMS....
	free ( packet );
	}
		
	

	
	return TRUE;
}





// ********************************** Private Function ************************

//Send the changes in location of a specified stroke.  This will only send a packet if the has been a change x, y or theta to minimize information sent over the network.
static void send_update(Stroke* stroke, gpointer data )
{	
	int id = stroke_get_id(stroke);
	double x, y, theta;
	int pos = stroke_get_position(stroke, &x, &y);
	int rot = stroke_get_rotation(stroke, &theta);
	if ( pos || rot )
	{
		sph_send_update(id, x, y, theta);
	}
}

//Load a new level of the world, destroying the current level.  This is based on input which came from the client.
static void load_new_world( int level, World* world )
{	
	// Clear the strokes from the world
	world_clear(world);
		
	//Clear all the information client side too...
	Packet* packet = packet_new( PACKET_TYPE_CLEAR_ALL );
	server_send_packet(packet);
	
	// Clear the physics world of all bodies and shapes
	world_physics_clear(world_get_space(world));
	
	// Load the level
	printf("%d\n", level - 1);
	char* level_name = LEVEL_LIST[level - 1];
	Level* l = level_new( level_name );  //TEMPORARY FIX....
	
	// Load the level into the cpSpace
	world_physics_load(world_get_space(world), l);
	
	// Load the cpSpace into the World
	world_load_space(world, world_get_space(world));

	// Send the new world to all the clients
	GList* list = world_get_stroke_list( world );
	g_list_foreach(list, (GFunc)sph_send_world_stroke, NULL);
}

//A wrapper function to send strokes to all clients using a GFunc iterator.
static void sph_send_world_stroke ( Stroke* stroke, void* unused )
{
	sph_send_stroke(stroke);
}

//Send the entire world to a specified client.  This is used when there is a new connection.
static void sph_send_world_to_client(World* w, Connection* c)
{
	GList* list = world_get_stroke_list(w);
	g_list_foreach(list, (GFunc)sph_send_stroke_to_client, (gpointer)c);
}

//Send a stroke only to a specified client.  This is used as a GFunc iterator by sph_send_world_to_client()
static void sph_send_stroke_to_client(Stroke* stroke, Connection* c)
{
	//Get the information from the stroke.
	int id = stroke_get_id(stroke);
	//printf( "ID NUMBER: %d\n", id );
	double xi, yi, theta;
	stroke_get_position(stroke, &xi, &yi);
	stroke_get_rotation(stroke, &theta);
	
	double r, g, b;
	stroke_get_color(stroke, &r, &g, &b);
	char type = stroke_get_shape_type(stroke);
	char fill = stroke_get_fill(stroke);
	
	//Send the initial point.
	server_send_packet_to(packet_new_stroke_new(id, xi, yi, type, fill, 2, r, g, b), c);
	//Send all successive points.
	for (int j = 0; j < stroke_get_length(stroke); j++)
	{
		double x, y;
		stroke_get_point(stroke, j, &x, &y);
		server_send_packet_to(packet_add_point_new(id, x, y), c);
	}
	//Send relative location (x, y and theta)
	server_send_packet_to(packet_update_new(id, xi, yi, theta), c);
}

//Send an entirely new stroke to all clients. 
static void sph_send_new_stroke(int id, double x, double y, char type, char fill, double width, double r, double g, double b)
{
	Packet* packet = packet_new_stroke_new(id, x, y, type, fill, width, r, g, b);
	server_send_packet(packet);
	//printf("New stroke packet created to be sent.  ID number: %d\n", id );
}

//Add a point to a stroke on the client side 
static void sph_send_add_point(int id, double x, double y)
{
	Packet* packet = packet_add_point_new(id, x, y);
	server_send_packet(packet);
}

//Update the client locations of a stroke.
static void sph_send_update(int id, double x, double y, double theta)
{
	Packet* packet = packet_update_new(id, x, y, theta);
	server_send_packet(packet);
}

//Send all components specified stroke to all clients.
static void sph_send_stroke(Stroke* stroke)
{
	//Get the intial information of the stroke.
	int id = stroke_get_id(stroke);
	double xi, yi, theta;
	stroke_get_position(stroke, &xi, &yi);
	stroke_get_rotation(stroke, &theta);
	
	double r, g, b;
	stroke_get_color(stroke, &r, &g, &b);
	char type = stroke_get_shape_type(stroke);
	char fill = stroke_get_fill(stroke);
	
	//Send the initial point and all affiliated information.
	server_send_packet(packet_new_stroke_new(id, xi, yi, type, fill, 2, r, g, b));
	
	//Run through all other points in the stroke and send packets to add those points to the client representation.
	for (int j = 0; j < stroke_get_length(stroke); j++)
	{
		double x, y;
		stroke_get_point(stroke, j, &x, &y);
		server_send_packet(packet_add_point_new(id, x, y));
	}
	
	//When all the points are added, include the relative location and rotation of the shape.
	server_send_packet(packet_update_new(id, xi, yi, theta));
}
