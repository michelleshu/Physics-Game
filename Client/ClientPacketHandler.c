/*
ClientPacketHandler.c

Michelle Shu
Jacob Weiss
*/

#include <stdio.h>
#include <sys/socket.h>
#include <glib.h>
#include <stdlib.h>
	
#include "../Network/Packet.h"
#include "../ClientWorld/ClientWorld.h"
#include "../GUI/ChatDialogWindow.h"
#include "../GUI/StartupDialogWindow.h"
#include "../GUI/WinDialogWindow.h"

#include "ClientPacketHandler.h"
#include "Client.h"

ClientPacketHandler* cph_get()
{
	static ClientPacketHandler* cph = NULL;
	if (cph == NULL)
	{
		cph = (ClientPacketHandler*)malloc(sizeof(ClientPacketHandler));
		cph->ink = 255;
	}
	return cph;
}


/* Send a text message to the server as a message packet */
void cph_send_message(char* message) 
{	
	/* Make a packet out of the message */
	Packet* msg_packet = packet_new(PACKET_TYPE_SEND_MESSAGE);
	packet_add_string(msg_packet, message, SEND_MESSAGE_TEXT);
	
	/* Send the message to the server as a character array. */
	client_send_packet(msg_packet);
	//printf("sent message %s\n", message);
}

/* Send a username to ther server as a name packet */
void cph_send_username(char* username)
{	
	/* Make a packet out of the message */
	Packet* name_packet = packet_new(PACKET_TYPE_SEND_NAME);
	packet_add_string(name_packet, username, SEND_NAME_NAME);
	
	/* Send the message to the server as a character array. */
	client_send_packet(name_packet);	
	//printf("sent username %s\n", username);
}

/*
Send the level number selected by the user.  This will trigger the server to load a new level into the world.
 */
int cph_send_level_number(int level_number)
{	
	Packet* packet = packet_load_level_new( (char) level_number );
	client_send_packet( packet );
	
	return 1;
}

/* Send the x and y position of the mouse down event to server */
void cph_send_mouse_down(double x, double y, char type, char fill, double width, double r, double g, double b) 
{	
	/* Make a packet out of the message */
	Packet* packet = packet_mouse_down_new(x, y, type, fill, width, r, g, b);
		
	/* Send the message to the server as a character array. */
	client_send_packet(packet);
}

/* Send the x and y position of the mouse move event to server */
void cph_send_mouse_move(double x, double y) 
{	
	/* Make a packet out of the message */
	Packet* mouse_move_packet = packet_new(PACKET_TYPE_MOUSE_MOVE);
	packet_add_double(mouse_move_packet, x, MOUSE_MOVE_X);
	packet_add_double(mouse_move_packet, y, MOUSE_MOVE_Y);
	
	/* Send the message to the server as a character array. */
	client_send_packet(mouse_move_packet);
}

/* Send the x and y position of the mouse up event to server */
void cph_send_mouse_up(double x, double y) 
{	
	/* Make a packet out of the message */
	Packet* mouse_up_packet = packet_new(PACKET_TYPE_MOUSE_UP);
	packet_add_double(mouse_up_packet, x, MOUSE_UP_X);
	packet_add_double(mouse_up_packet, y, MOUSE_UP_Y);
	
	/* Send the message to the server as a character array. */
	client_send_packet(mouse_up_packet);
}


/*
Process all packets of information which have been pushed onto the queue from the client
 */
gboolean cph_process_queue(gpointer userdata)
{
	//Pop data from the queue of information coming from the server and handle it.
	//Use .._try_pop() because it is not blocking so if there is no data the program will still continue.
	GAsyncQueue* recv_queue = client_get_receive_queue();
	Packet* packet;
	while((packet = (Packet *)g_async_queue_try_pop(recv_queue)))
	{
		switch (packet_get_type(packet))
		{
			//If it was a username, this should be sent to the chat window.
			case PACKET_TYPE_SEND_NAME:
			{
				chat_username_send(packet_get_string(packet, SEND_NAME_NAME));
				break;
			}
			//Messages should also be sent to chat
			case PACKET_TYPE_SEND_MESSAGE:
			{
				chat_message_send(packet_get_string(packet, SEND_MESSAGE_TEXT));
				break;
			}
			//New strokes contain lots of pertinatnt information about a shape.  Unpack the data and add the stroke to the client world so that the shape can be represented graphically.
			case PACKET_TYPE_NEW_STROKE:
			{
				int id = packet_get_int(packet, NEW_STROKE_ID);
				double x = packet_get_double(packet, NEW_STROKE_X);
				double y = packet_get_double(packet, NEW_STROKE_Y);
				char type = packet_get_byte(packet, NEW_STROKE_SHAPE_TYPE);
				int fill = packet_get_byte(packet, NEW_STROKE_FILL);
				double width = packet_get_double(packet, NEW_STROKE_WIDTH);
				double r = packet_get_double(packet, NEW_STROKE_R);
				double g = packet_get_double(packet, NEW_STROKE_G);
				double b = packet_get_double(packet, NEW_STROKE_B);
								
				client_world_new_stroke(id, x, y, type, fill, width, r, g, b);
				
				break;
			}
			//This adds to the length of a shape already in the client world.
			case PACKET_TYPE_ADD_POINT:
			{
				client_world_add_to_stroke(packet_get_int(packet, ADD_POINT_ID), packet_get_double(packet, ADD_POINT_X),
					packet_get_double(packet, ADD_POINT_Y));
				break;
			}
			//Remove a shape from the client world.
			case PACKET_TYPE_DELETE:
			{
				client_world_delete_stroke(packet_get_int(packet, DELETE_ID));
				break;
			}
			//Update the x, y and theta infomration of a shape so that physical movements will be echoed graphically
			case PACKET_TYPE_UPDATE:
			{
				client_world_update_stroke(packet_get_int(packet, UPDATE_ID), packet_get_double(packet, UPDATE_X), 
					packet_get_double(packet, UPDATE_Y), packet_get_double(packet, UPDATE_ROTATION));
				break;
			}
			//An updated manner of handling chat data from the server.  This sends both the username and the chat information together.
			case PACKET_TYPE_PACKETS:
			{
				Packet* name = packet_get_packet(packet, 0);
				Packet* message = packet_get_packet(packet, 1);
					
				chat_username_send(packet_get_string(name, SEND_NAME_NAME));
				chat_message_send(packet_get_string(message, SEND_MESSAGE_TEXT));
					
				break;
			}
			//Remove all strokes from the world.
			case PACKET_TYPE_CLEAR_ALL:
			{
				client_world_clear();
				break;
			}
			//Inform the client how much longer a stroke can be drawn for.
			case PACKET_TYPE_INK_POT:
			{
				ClientPacketHandler* cph = cph_get();
				char ink = packet_get_byte(packet, INK_POT_INK);
				unsigned int int_ink = (unsigned char)ink;
				cph->ink = int_ink;
				break;
			}
			//Inform the client if the level has been won
			case PACKET_TYPE_LEVEL_WIN:
			{
				win_get_status();
				printf( "Level is won!\n");
				break;
				
			}
			case PACKET_TYPE_UNDEFINED:
			default:
			{
				printf("Received packet of unknown type.\n");
				break;
			}
		}	
		free(packet);
	}
	
	return TRUE;
}