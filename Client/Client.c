/*
Client.c

Michelle Shu
Jacob Weiss
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <glib.h>
#include "Client.h"
#include <sys/time.h>
#include "ClientPacketHandler.h"

#define SERVER_PORT 4545

#define NO_CONNECTION (-1)


//Private version of the client structure.
typedef struct 
{
	int fd;
	short server_port;
	GAsyncQueue* send_queue;	// Queue of packets from server to be used by world
	GAsyncQueue* recv_queue;	// Queue of incoming messages to be displayed by GUI
	
	pthread_t client_thread; // The thread that the client is running on
	int stop_signal; // Set to 1 if the thread should stop
} _Client;


//********************** Private Function Prototypes ***************************

Client* client_get();

static void client_destroy();
static _Client* client_alloc();
static _Client* client_init(_Client* client);
static void client_create_socket();
static int client_connect(int port, char* server_ip);
static int client_listen(double seconds);
static void* client_begin_thread(void* data);
static void client_disconnect();

//******************** Public Function Implementations ***************************

/*
Public function to push a packet onto the queue which will take it from the graphcis thread to the client thread and then send it to the server.
 */
void client_send_packet(Packet* packet)
{
	_Client* client = (_Client*)client_get();
	
	if (client_is_connected())
	{
		g_async_queue_push(client->send_queue, (gpointer)packet);
	}
}

/*
Public function to say how many packets are in the queuue waiting to be handled by the graphics thread.
 */
int client_num_packets_available()
{
	_Client* client = (_Client*)client_get();
	
	return g_async_queue_length(client->recv_queue);
}


/*
Public function to pop a packet off of the queue coming from the client thread, so it can be handled by the graphics thread.
 */
Packet* client_receive_packet()
{
	_Client* client = (_Client*)client_get();
	
	return g_async_queue_try_pop(client->recv_queue);
}


/*
Function to start the client thread which is separate from the graphcis thread.
 */
int client_start(int port, char* ip_address)
{	
	_Client* client = (_Client*)client_get();
	
	//Create the socket which will send and receive information from the server
	client_create_socket();
	
	//If the socket was created, create the thread to handle client communication.
	int status = client_connect(port, ip_address);
	if (status)
	{	
		pthread_create(&(client->client_thread), NULL, client_begin_thread, NULL);
	}
	
	return status;
}

/*
 End the client thread gracefully when the primary graphics thread ends.
 */
void client_stop()
{
	_Client* client = (_Client*)client_get();
	
	client->stop_signal = 1;
	
	while (client_is_connected());
	
	return;
}

/*
Check to see if there is actually a connection to a server.
 */
int client_is_connected()
{
	_Client* client = (_Client*)client_get();
	
	return (client->fd != NO_CONNECTION);
}



//******************* Private Function Implementations **************************


//This is the function in which the client thread will run for the duration of the program.
static void* client_begin_thread(void* data)
{
	printf("Client thread started.\n");
	
	_Client* client = (_Client*)client_get();
	
	//Continue as long as the client has not received a signal to stop from the main thread.
	while (!(client->stop_signal))
	{
		//Check to see if the client has recieved any packets of information.  If so, handle them
		if (client_listen(.01))
		{
			//Get the packet of information and push it onto the queue to send it to the graphics thread if there was data there.  If there was not, the server terminated and exit the while loop.
			Packet* packet = packet_new_from_file_descriptor(client_get_file_descriptor());
			if (packet != NULL)
			{
				//printf("Recieved Packet\n");
				//packet_print(packet);
				g_async_queue_push(client->recv_queue, (gpointer)packet);
			}
			else
			{
				printf("The server closed the connection.\n");
				break;
			}
		}
		
		//Check to see if there are any packets to send to the server coming from the graphics thread.
		Packet* packet;
		/* Send packets in send_queue to server */
		while ((packet = g_async_queue_try_pop(client->send_queue)))
		{
			//printf("Sending Packet\n");
			//packet_print(packet);
			send(client->fd, (char*)packet, packet_get_length(packet), 0);
			
			free(packet);
		}
		/* Send packets in recv_queue to GUI or ClientWorld */
	}
	
	//If the look has ended, that means the server or client disconnected.  End the connection gracefully.
	client_disconnect();
	
	printf("Client thread terminated cleanly.\n");
	
	return NULL;
}

/*
Using Singleton design pattern to maintain client properties in a place easily
accessible to other functions.
*/
Client* client_get() 
{
	static _Client* client = NULL;
	if (client == NULL)
	{
		client = client_init(client_alloc());
		client_create_socket();
		//printf("Client created.\n");
	}
	return (Client*)client;
}

/*
 Disconnect from the server but do not eliminate the client thread.
 */
static void client_disconnect()
{
	_Client* client = (_Client*)client_get();
	
	if (client_is_connected())
	{
		close(client->fd);
		client->fd = NO_CONNECTION;
	}
}

/*
Free the space taken up by the client information struct
*/
static void client_destroy()
{
	_Client* client = (_Client*)client_get();
	
	client->stop_signal = 1;
	
	while (client_is_connected());
	
	g_async_queue_unref(client->send_queue);
	g_async_queue_unref(client->recv_queue);
	free(client);
}

/*
Allocate space for a new client struct
*/
static _Client* client_alloc()
{
	_Client* client = (_Client*)malloc(sizeof(_Client));
	assert(client);
	return client;
}

/*
Initialize a client with provided socket file descriptor
*/
static _Client* client_init(_Client* client)
{
	client->fd = NO_CONNECTION;
	client->server_port = SERVER_PORT;
	client->send_queue = g_async_queue_new();	// Initialize the send queue
	client->recv_queue = g_async_queue_new();	// Initialize the recieve queue
	client->stop_signal = 0;
	return client;
}

/* Get this client's socket file descriptor. */
int client_get_file_descriptor() {
	_Client* client = (_Client*)client_get();
	if (!client_is_connected()) 
	{
		perror("Error: Connection to server has not yet been established.");
		exit(EXIT_FAILURE);
	}
	return client->fd;
}

/* Get this client's send queue. */
GAsyncQueue* client_get_send_queue() {
	_Client* client = (_Client*)client_get();
	if (!client_is_connected()) 
	{
		perror("Error: Connection to server has not yet been established.");
		exit(EXIT_FAILURE);
	}
	return client->send_queue;
}

/* Get this client's receive queue. */
GAsyncQueue* client_get_receive_queue() {
	_Client* client = (_Client*)client_get();
	/* Check that client has been connected
	If client has already been connected, it will have a fd other than -1 we are passing in 
	*/
	if (!client_is_connected()) {
		perror("Error: Connection to server has not yet been established.");
		exit(EXIT_FAILURE);
	}
	return client->recv_queue;
}

/*
Initialize the client's socket
*/
static void client_create_socket()
{
	int sockfd;
	/* Create a socket for client and check that socket was created 
	successfully. */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	    perror("Error: Could not create socket");
	    exit(EXIT_FAILURE);
	}
	((_Client*)client_get())->fd = sockfd;
}

/*
Connect the client to server port port, providing the IP address of the server.
*/
static int client_connect(int port, char* server_ip)
{
	_Client* client = (_Client*)client_get();
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_ip);
    server_address.sin_port =  htons(port);	/* Convert to proper network byte order */

    /* Connect the client to the socket. Error if connection failed.*/
    if (connect(client->fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) 
	{
        perror("Error: Could not connect");
        return 0;
    }
	else
	{
		printf("Connected to %s:%d.\n", server_ip, port);
	}

	return 1;
}

// Test tp see if the client has revieved any data using select
static int client_listen(double seconds)
{		
	struct timeval timeout;
	timeout.tv_sec = (int)seconds;
	timeout.tv_usec = 1000000 * (seconds - (int)seconds);
			
	fd_set set;
	FD_ZERO(&set);
	int fd = client_get_file_descriptor();
	FD_SET(fd, &set);
		
	//printf( "Creating file descriptor set.\n");
	select(fd + 1, &set, NULL, NULL, &timeout);
		
	return FD_ISSET(fd, &set);
}




//THIS IS FOR ELIZA'S TESTING TO SEND MESSAGE PACKETS BACK AND FORTH TO THE SERVER.
/*
int main (int argc, char** argv )
{
	client_create_socket();
	client_connect(4545, "127.0.0.1");
	_Client* client = (_Client*)client_get();
	
	Packet* packet = packet_new( PACKET_TYPE_SEND_NAME );
	packet_add_string( packet, "Eliza", SEND_NAME_NAME );
	send(client->fd, (char*)packet, packet_get_length(packet), 0);
	
	if (client_listen(.01))
	{
		printf( "NOT GOOD.\n");
	}
	
	Packet* packet2 = packet_new( PACKET_TYPE_SEND_MESSAGE );
	packet_add_string( packet2, "Hello World!", SEND_MESSAGE_TEXT );
	send(client->fd, (char*)packet2, packet_get_length(packet2), 0);
	
	if (client_listen(.01))
	{
		 Packet* name = packet_new_from_file_descriptor(client->fd);
		 
		 printf( "username: %s\n", packet_get_string( name, SEND_NAME_NAME ));
	}
	if (client_listen(.01))
	{
		Packet* message = packet_new_from_file_descriptor(client->fd);
		
		printf( "message: %s\n", packet_get_string( message, SEND_NAME_NAME ));
	}
	
	
	return 0;
}


*/





