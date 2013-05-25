/*
Server.h

Jacob Weiss
*/

#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>



#include "../Network/Packet.h"
#include "ServerPacketHandler.h"

//********************* Public Structures **********************************

//Public server structure 
typedef struct Server Server;

//Connections.  This is public because ServerPacketHandler needs to update information within this structure like user_name and current_id
typedef struct
{
	int file_descriptor;	
	struct sockaddr_in address;
	socklen_t address_length;
	
	char* user_name;
	
	int current_id;
	
} Connection;

// A wrapper structure to contain packet and connection information so they can continue to be associated together within the queue moving between the server thread and the main physics thread.
typedef struct
{
	Connection* c;
	Packet* packet;
} Wrapper;



//********************** Public Function Prototypes ***************************

void server_send_packet(Packet* packet);
void server_send_packet_to(Packet* packet, Connection* connection);
int server_num_packets_available();
Wrapper* server_receive_packet();
void server_destroy();

void server_start();

#endif