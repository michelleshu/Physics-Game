#Server

####Back To [Overview](./Overview.html)

##Overview

The server is the connection between the central physics world and each individual client who is drawing. The server receives information updates from the [client](./Client.c) informing the server of user updates and interactions with the program. Additionally, the server receives information from the physics world to add delete and update the locations of strokes within the physics world so that they can be accurately drawn on the client's screen. The server also has the ability to send information to all clients or just a specified client depending on input.

##Data Structures

###Private Singleton Server

To avoid passing the server as a parameter in all functions, there is a private server structure which can be accesssed using a singleton. This structure maintains a reference to the listening port, a hash table keeping track of active connections to clients, the set of file descriptors which are ready to be read from, and two references to the Asynchronous Queues which streamline the connection between the server itself and the separately threaded Chipmunk world.

###Asynchronous Queues

There are two asynchronous queues the send\_queue and the recv\_queue. These facilitate a safe connection between the two server side threads. Unlike many structures provided by [Glib](https://developer.gnome.org/glib/stable/glib-Asynchronous-Queues.html), they are thread safe, making them ideal for the one way transport of information. Information is pushed onto the send\_queue by the Chipmunk thread. It waits there for the server to send it to the client. Whenever the server receives a packet from a client, it pushes the received packet (in a wrapper, see below) onto the recv\_queue. They sit there until the sph\_process\_queue function is called and they are distributed to their proper locations. Most of them go to the [World](./World.html)

###Connections

This public structure is utilized by both the server and the [ServerPacketHandler](./ServerPacketHandler.html) to access the a particular client and associate information. Specifically for the server a connection keeps track of the file descriptor and associated socket informaiton. For the ServerPacketHandler, a connection keeps track of client specific information such as username in the chat function as well as current stroke being drawn so that a connection adds information to the correct stroke.

###Wrapper

This structure has references to a single connection and an address of a packet of informaiton. It is used so that data as well as the location it cam from can be passed together through the asynchronous arrays.

##Flow of Information

The server is a critical juncture in the flow of information between the [physics thread](./ServerMain.html) on the server side and all [clients](./Client.html) over the network. Coming in, informaiton from a client connection is immediately constructed into a [Packet](./Packet.html). It is then pushed onto the recv\_queue in a wrapper containing the connection the informaiton originally came from. 

In the other direction, a wrapper is popped off of the send\_queue with the packet of information to be sent as well as the the connection to send it to (if any) if a connection is specified, the server will only send the packet to a specified client. If the connection pointer is NULL, the informaiton is sent to all active clients.


##Public Functions

There are two categories of public functions, the ones that access the queues and the functions that maintain the server thread and structure.

The functions that maintain the queues push an pop wrappers in their specified manners. Popped wrappers come from the send\_queue because those are leaving the server. Pushed wrappers are put onto the recv\_queue to be sent out to specified server subsystems. Here are the functions:

* void server\_send\_packet( Packet* )
* void server\_send/_packet/_to( Packet*, Connection* )
* int server\_num\_packets\_available()
* Wrapper* server\_recieve\_packet()

The second set of public functions are for server maintenance. These functions create the server thread, and server connections as well as destroy and disconnect the server. They are listed here:

* void server\_start()
* int server\_is\_connected()
* void server\_disconnect()
* void server\_destroy()


##Server Design

The server subsystem is largely self sufficient because it is designed to run entirely within its own thread. The private functions initialize structures, and handle the thread. server\_begin\_thread() is the private method which is the main body of the thread. In its while loop, the thread checks server\_listen(), which factors the select function, server\_process\_new\_connections(), which checks if the listening file descriptor has a new connection and initializes it if it does, server\_recieve\_packets(), which recieves packets from clients and pushes them onto the receive queue, and finally server\_send\_packets(), which sends packets on the send\_queue out to the active connections.











