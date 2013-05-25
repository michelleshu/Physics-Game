#Client

####Back To [Overview](./Overview.html)

##Overview
Each player that joins the game runs their own client. The client receives user input (in the forms of chat messages, drawings, level selection, etc.) to be sent to the [Server](./Server.html), which echoes the information back to all clients. Thus, the client also receives information from the server and passes that information along to the appropriate recipients. All lines of communication go through the [ClientPacketHandler](./ClientPacketHandler.html). The Client itself is only responsible for connecting to the server via a socket and maintaining queues of [Packets](./Packet.html) going to and coming from the server.

##Singleton Design Pattern
To avoid using a global variable to store the client's socket file descriptor, we use a singleton design pattern for the client. Client is a structure containing all necessary details about a single client: the file descriptor, server port, send and receive queues, and the client thread. Only one client struct can be constructed by each user. After it is created, the client is connected to the server with a specific configuration and any function can retrieve information about the connection without modifying it. This Singleton structure is achieved by using a static Client variable initialized to null in *client\_get*. A new client is initialized only if the client is null at the time of call.

##Send and Receive Queues
The client maintains two queues:

1. Send queue: Contains packets received from GUI to be sent to the Server.
2. Receive queue: Contains packets received from Server to be displayed in the client's graphics window.

Please reference [ClientPacketHandler](./ClientPacketHandler.html) for details on how these packets are transferred to their appropriate destinations.

The threadsafe GAsynchronousQueue from GLib 2.0 is the data structure used to represent the send and receive queues of the client. [Packets](./Packet.html) are delivered to the client by getting pushed onto the client's receive or send queue (depending on which direction they are going) and delivered by being popped off. The client does not distinguish between different types of packets. The client does provide public functions for sending packets (called by [Server](./Server.html)) and receiving packets (called by [ClientPacketHandler](./ClientPacketHandler.html)).

##Initializing and Destroying Clients
The client is not explicitly initialized in *ClientMain.c* (which runs the main program on the client's side). Instead, it is created on the fly at the first instance at which it is needed. This happens to be when the user logs in and provides the server's port and IP address. At this point, the [LoginDialogWindow](./GUI.html) tells [ClientPacketHandler](./ClientPacketHandler.html) to send information to the client. Since the client does not exist yet, when *client\_get* is called in this process, the client is created. During initialization, a socket is created for the client and the socket file descriptor stored in the Client struct.

The client can be cleanly stopped when the public function *client\_stop* is called (when the graphics thread terminates).

##Threading
Each client has its own thread to handle client communication. The thread is created when the client is initialized and stored as a variable in the client struct.