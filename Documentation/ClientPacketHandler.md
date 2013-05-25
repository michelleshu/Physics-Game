#ClientPacketHandler

####Back To [Overview](./Overview.html)

##Overview
ClientPacketHandler facilitates the movement of different types of [Packets](./Packet.html) going to and from the [Client](./Client.html) by pushing to and popping from the client's send and receive queues.

##GUI to Client Communication
The GUI can send the following types of information to the ClientPacketHandler to be sent through the [Client](./Client.html) to the [Server](./Server.html):

1. Chat message from chat dialog window
2. Username from login dialog window
3. Level from level selection window
4. Mouse movements from interaction with drawing area: mouse down, mouse move, mouse up

The ClientPacketHandler provides separate public functions to transmit each of these types of information. It passes the information along by first packaging it in its appropriate [Packet](./Packet.html) type and then pushing the packet onto the Client's send queue.

##Client to GUI Communication
ClientPacketHandler receives the following types of information from the [Server](./Server.html) to be passed to the [GUI](./GUI.html):

1. Send chat username (of user who entered chat message) and chat message together to chat dialog window
2. Send level win signal to open level win dialog window

The packets are unpacked and the information within them is sent by calling functions defined within the [GUI](./GUI.html).

Every once in a while (i.e. every time the client thread goes through its while loop), ClientPacketHandler's *cph\_process\_queue* is called by the [Client](./Client.html). This function will process every packet currently lined up to be sent to the [GUI](./GUI.html) and [ClientWorld](./ClientWorld.html) from the Client's receive queue.

##Client to ClientWorld Communication
Other packets in the Client's receive queue (received from Server) that are related to drawing are passed to the [ClientWorld](./ClientWorld.html). These packets are of the following types:

1. New stroke: Initialize a new stroke object in the ClientWorld
2. Add a point to current stroke
3. Delete a stroke
4. Update the position, rotation of a stroke to reflect its movement in the physical world
5. Clear all strokes from the world

The information contained in these packets is unpacked and then transferred to [ClientWorld](./ClientWorld.html) by calling ClientWorld functions.

##Additional Function: Ink Left
An additional function of ClientPacketHandler is to retain data about how much "ink" the user has left in producing the current stroke. The ClientPacketHandler struct follows the same Singleton design pattern described in [Client](./Client.html) to store the variable ink.

Ink is a number between 0 and 255 that describes the amount of ink left at any given time. The singleton ClientPacketHandler allows [Graphics](./Graphics.html) to access the ink value whenever needed to update the graphical display of ink remaining without modifying the value. The value of ink is tracked by the [Stroke](./Stroke.html) and is periodically updated in ClientPacketHandler as the stroke is drawn.