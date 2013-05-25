#ifndef CLIENTPACKETHANDLER_H
#define CLIENTPACKETHANDLER_H

#include <glib.h>

typedef struct 
{
	int ink;
} ClientPacketHandler;

//********************** Public Function Prototypes ***************************

ClientPacketHandler* cph_get();

/* Send a text message to the server as a message packet */
void cph_send_message(char* message);

/* Send a username to ther server as a name packet */
void cph_send_username(char* username);

/*
 Send the level number selected by the user.  This will trigger the server to load a new level into the world.
 */
int cph_send_level_number(int level_number);

/* Send the x and y position of the mouse down event to server */
void cph_send_mouse_down(double x, double y, char type, char fill, double width, double r, double g, double b);

/* Send the x and y position of the mouse move event to server */
void cph_send_mouse_move(double x, double y);

/* Send the x and y position of the mouse up event to server */
void cph_send_mouse_up(double x, double y);

/*
 Process all packets of information which have been pushed onto the queue from the client
 */
gboolean cph_process_queue(gpointer userdata);

#endif