/*
Server.c

Jacob Weiss
Eliza Hanson

Based on simpleserver.c by Devin Balkcom based on CS 23 example by Andrew Campbell
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/time.h>

#include <assert.h>
#include <netdb.h>
#include <pthread.h>
#include <gtk/gtk.h>

#include "Server.h"
	
#define SERVER_PORT 4545
#define LISTEN_QUEUE_MAX 8  // maximum number of pending client connections
#define MAX_CONNECTIONS 10  // maximum number of open connections
#define MAXLINE 4096        // buffer size for reads and writes

#define NO_CONNECTION (-1)

//Private server structure.
typedef struct
{
	int listen_file_descriptor;
	short listen_port;
	
	GHashTable* connections;
	//Connection** connections;
	//int num_connections;

	GAsyncQueue* send_queue;
	GAsyncQueue* recv_queue;
		
	fd_set descriptors_with_data;
	
	pthread_t server_thread;
	
	int stop_signal;
} _Server;


//**************************** Private Function Prototypes ***************************

static _Server* server_alloc();
static _Server* server_init(_Server* s);
static Wrapper* wrapper_new(Packet* packet, Connection* connection);
static void server_send_wrapper(Wrapper* wrapper);
static Connection* connection_new();
static void connection_destroy(Connection* c);
static void server_create_socket();
static void server_bind(int port);
static void server_listen(double seconds);
static void server_process_new_connections();
static void server_receive_packets();
static void server_send_packets();
static Server* server_get();

static void* server_begin_thread(void* data);

//************************* Public Function Implementations **************************

/*
Public function to add a packet to the send queue sending it to all connections.
 */
void server_send_packet(Packet* packet)
{
	server_send_packet_to(packet, NULL);
}

/*
Public function to send a packet to the send queue makeing sure it is sent only to a specific connection.
 */
void server_send_packet_to(Packet* packet, Connection* connection)
{	
	Wrapper* wrapper = wrapper_new(packet, connection);
	
	server_send_wrapper(wrapper);
}

/*
Tells how many packets are still in the queue waiting to be recieved by the physics world.
 */
int server_num_packets_available()
{
	_Server* server = (_Server*)server_get();
	
	return g_async_queue_length(server->recv_queue);
}


/*
Pop a packet off the recieved queue going away from the server.
 */
Wrapper* server_receive_packet()
{
	_Server* server = (_Server*)server_get();
	
	return g_async_queue_try_pop(server->recv_queue);
}

/*
Set up and start the server thread.
 */
void server_start()
{
	_Server* server = (_Server*)server_get();
	
	server_create_socket();
	server_bind(SERVER_PORT);
		
	pthread_create(&(server->server_thread), NULL, server_begin_thread, NULL);
}

/*
Check whether the server is has any active connections.
 */
int server_is_connected()
{
	_Server* server = (_Server*)server_get();
	
	return (server->listen_file_descriptor != NO_CONNECTION);
}

/*
Gracefully disconnect the server from any client.
 */
void server_disconnect()
{
	_Server* server = (_Server*)server_get();
	
	//Run through the hash table of connections removing them all.
	g_hash_table_remove_all(server->connections);
	
	//Close all connections.
	if (server_is_connected())
	{
		close(server->listen_file_descriptor);
		server->listen_file_descriptor = NO_CONNECTION;
	}
	//server->num_connections = 0;
}

/*
Destroy the server.
 */
void server_destroy()
{
	_Server* server = (_Server*)server_get();
	
	server->stop_signal = 1;
	
	while (server_is_connected());
		
	g_hash_table_destroy(server->connections);
	g_async_queue_unref(server->send_queue);
	g_async_queue_unref(server->recv_queue);
}

//************************* Private Function Implementations *************************

//Take a wrapper packet and push it onto the server send_queue so it can be sent to clients.
static void server_send_wrapper(Wrapper* wrapper)
{
	_Server* server = (_Server*)server_get();
	
	g_async_queue_push(server->send_queue, (gpointer)wrapper);
}

//Thhis is the primary server thread function. The server thread should remain in here as long as the thread is active.
static void* server_begin_thread(void* data)
{
	printf("Server thread started.\n");
	
	_Server* server = (_Server*)server_get();
	
	while (!(server->stop_signal))
	{
		server_listen(.01);
		server_process_new_connections();
		server_receive_packets();
		server_send_packets();
	}
	
	server_disconnect();
	
	printf("Server thread terminated cleanly.\n");
	
    return NULL;
}

//Singleton function to ensure all private functions have access to the server structure and its associated information.
static Server* server_get()
{
	static _Server* server = NULL;
	if (server == NULL)
	{
		server = server_init(server_alloc());
	}
	return (Server*)server;
}

//Allocate memory for the server struct
static _Server* server_alloc()
{
	_Server* s = (_Server*)malloc(sizeof(_Server));
	assert(s);
	return s;
}

//Initialize the server struct.  
static _Server* server_init(_Server* s)
{	
	//s->num_connections = 0;
	//s->connections = NULL;
	
	//There is no connetion to listen to initially.
	s->listen_file_descriptor = NO_CONNECTION;
	s->stop_signal = 0;
	//s->connections = (Connection**)malloc(MAX_CONNECTIONS * sizeof(Connection*));
	
	//Create a hash table to keep track of connections.
	s->connections = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, (GDestroyNotify)connection_destroy);
	
	//Create the queues to move data between the server and physics threads.
	s->send_queue = g_async_queue_new ();
	s->recv_queue = g_async_queue_new ();
	return s;
}

//Initialize a connection struct 
static Connection* connection_new()
{
	Connection* c = (Connection*)malloc(sizeof(Connection));
	assert(c);
	c->address_length = sizeof(c->address);
	
	//The user_name should be set to anonymous before the client sends a user_name
	c->user_name = (char*)malloc(30*sizeof(char));
	strcpy( c->user_name, "Anonomous" );
	
	//Because there is no stroke currently being draw set this to -1
	c->current_id = -1;
	c->file_descriptor = NO_CONNECTION;
	
	return c;
}

//Destroy a connection freeing the associate memory.
static void connection_destroy(Connection* c)
{		
	close(c->file_descriptor);
	
	free(c->user_name);
	free(c);
}

//Create a new wrapper packet to and store the packet and connection references inside so they can be accessed by the other thread on the side of the queues
static Wrapper* wrapper_new(Packet* packet, Connection* connection)
{
	Wrapper* wrapper = (Wrapper*) malloc( sizeof ( wrapper ));
	assert( wrapper );
	wrapper->c = connection;
	wrapper->packet = packet;
	
	return wrapper;
}

//Actually set up the socket information within the server struct
static void server_create_socket()
{
	_Server* server = ((_Server*)server_get());
	server->listen_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	int value = 1;
	setsockopt(server->listen_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));
}

//Bind the server port and to a listener and save it in the server struct so it will continue to be accessible.
static void server_bind(int port)
{
	//Bind the socket to an address
	((_Server*)server_get())->listen_port = port;
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(port);
	
	bind(((_Server*)server_get())->listen_file_descriptor, (struct sockaddr*)(&server_address), sizeof(server_address));
	
	//Make the socket a passive listening socket.
	listen(((_Server*)server_get())->listen_file_descriptor, LISTEN_QUEUE_MAX);
}

//This function handles the select for the server.  It zeros the previous file_descriptor_with_data set, re-adds all file_descriptor's to it and then runs select on the entire collection looking if any file_descriptors have data
static void server_listen(double seconds)
{
	_Server* server = (_Server*)server_get();
	
	struct timeval timeout;
	timeout.tv_sec = (int)seconds;
	timeout.tv_usec = 1000000 * (seconds - (int)seconds);
	
	//This is initially the largest file descriptor in the set
	int max_descriptor = server->listen_file_descriptor;
	
	//Zero out the set and add the lister to it.
	FD_ZERO(&(server->descriptors_with_data));
	FD_SET(server->listen_file_descriptor, &(server->descriptors_with_data));
	
	//Iterate through connection hash table and for each one that exists add it to the file descriptor set 
	GList* connections = g_hash_table_get_values(server->connections);
	GList* connection = g_list_nth(connections, 0);
	while (connection != NULL)
	{
		Connection* c = g_list_nth_data(connection, 0);
		
		int fd = c->file_descriptor;
		
		if (fd > max_descriptor)
		{
			max_descriptor = fd;
		}
		FD_SET(fd, &(server->descriptors_with_data));
		
		connection = g_list_next(connection);
	}
		
	//printf( "Creating file descriptor set.\n");
	select(max_descriptor + 1, &(server->descriptors_with_data), NULL, NULL, &timeout);
}

//This function will run in its entirety if the listening port is set in the connect.  In this case the server will process and add a new connection if it does not already have too many connections.
static void server_process_new_connections()
{
	_Server* server = (_Server*)server_get();
	
	if (FD_ISSET(server->listen_file_descriptor, &(server->descriptors_with_data)))
	{
		//Create a new connection struct and add the file descriptor to it.
		Connection* connection = connection_new();
		connection->file_descriptor = accept(server->listen_file_descriptor, (struct sockaddr*)(&(connection->address)), &(connection->address_length));
		
		//The struct should then be inserted in the hash table with the file_descriptor as its key.
		g_hash_table_insert(server->connections, GINT_TO_POINTER(connection->file_descriptor), (gpointer)connection);
		//server->connections[server->num_connections++] = connection;
		printf("Accepted Connection %d\n", connection->file_descriptor);
		
		//Push a new connection packet so that the new world selectively gets all of the strokes in the world.
		Packet* nc_packet = packet_new(PACKET_TYPE_NEW_CONNECTION);
		Wrapper* wrapper = wrapper_new(nc_packet, connection);
		g_async_queue_push(server->recv_queue, (gpointer*)wrapper);
	}
}


//This function loops through all the connections in the hash table and gets data from them if the file_descriptor is set in the file_descriptor set.
static void server_receive_packets( )
{	
	_Server* server = (_Server*)server_get();
	
	//Get a list of connections in the hashtable.
	GList* connections = g_hash_table_get_values(server->connections);	
	GList* connection = g_list_nth(connections, 0);
	
	//Loop through the connections in the hash table
	while (connection != NULL)
	{
		Connection* c = g_list_nth_data(connection, 0);
		
		//If the file descriptor is set (has data) process it.
		if (FD_ISSET(c->file_descriptor, &(server->descriptors_with_data)))
		{	
			//Get a packet of information from the file descriptor.
			Packet* packet = packet_new_from_file_descriptor(c->file_descriptor);

			//If there was nothing that means that the connection was terminated.  Remove all information from that gracefully.
			if (packet == NULL)
			{
				printf("Connection to %d terminated.\n", c->file_descriptor);
				g_hash_table_remove(server->connections, GINT_TO_POINTER(c->file_descriptor));
			}
			//Otherwise there was information included.  Create a wrapper to include the connection and then push the wrapper onto the recv_queue so that it can be sent to the physics world queue.
			else 
			{
				//printf("Recieved Packet\n");
				//packet_print(packet);
				
				Wrapper* wrapper = wrapper_new(packet, c);
				
				g_async_queue_push(server->recv_queue, (gpointer*)wrapper);
			}
		}
		
		connection = g_list_next(connection);
	}	
}

//Send all packets that have been sent to the server from the physics thread.
static void server_send_packets( )
{
	_Server* server = (_Server*)server_get();
	
	//Pop try to pop from the queue.  This is not blocking and will just reutrn null if there is nothing to send.
	Wrapper* wrapper;
	while ((wrapper = g_async_queue_try_pop(server->send_queue)))
	{						
		int len = packet_get_length(wrapper->packet);
		
		//printf("Sending Packet\n");
		//packet_print(wrapper->packet);
		
		//If there was no connection specifically included, send the packet to all of the connections.
		if (wrapper->c == NULL)
		{
			GList* connections = g_hash_table_get_values(server->connections);
			GList* connection = g_list_nth(connections, 0);
			while (connection != NULL)
			{
				Connection* c = g_list_nth_data(connection, 0);
				send(c->file_descriptor, (char*)(wrapper->packet), len, 0);
				connection = g_list_next(connection);
			}
		}
		//If there was a connection designated, only send the packet to that one.
		else
		{
			Connection* c = wrapper->c;
			send(c->file_descriptor, (char*)(wrapper->packet), len, 0);
		}
		
		free(wrapper->packet);
		free(wrapper);
	}
}



/*
int main ( int argc, char** argv )
{

	server_create_socket();
	server_bind(SERVER_PORT);
	
	printf( "Waiting for a connection\n");
	while (1)
	{
		server_listen(.01);
		server_process_new_connections();
		server_receive_packets();
		packets_from_server();
		server_send_packets();
	}
	return 0;
	
}


*/








