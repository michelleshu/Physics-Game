/*
Client.h

Michelle Shu
Jacob Weiss
*/

#ifndef CLIENT_H
#define CLIENT_H

#include "../Network/Packet.h"
#include <glib.h>

typedef struct Client Client;

//********************** Public Function Prototypes ***************************

/* Get this client's socket file descriptor */
int client_get_file_descriptor();

/* Get the queue of packets to send to the server */
GAsyncQueue* client_get_send_queue();

/* Get the queue of packets received from the server */
GAsyncQueue* client_get_receive_queue();

/*
 Public function to push a packet onto the queue which will take it from the graphcis thread to the client thread and then send it to the server.
 */
void client_send_packet(Packet* packet);

/*
 Public function to say how many packets are in the queuue waiting to be handled by the graphics thread.
 */
int client_num_packets_available();

/*
 * Public function to pop a packet off of the queue coming from the client thread, so it can be handled by the graphics thread.
 */
Packet* client_receive_packet();

/*
Function to start the client thread which is separate from the graphics thread.
*/
int client_start(int port, char* ip_address);

/*
 *  End the client thread gracefully when the primary graphics thread ends.
 */
void client_stop();

/*
 * Check to see if there is actually a connection to a server.
 */
int client_is_connected();


#endif