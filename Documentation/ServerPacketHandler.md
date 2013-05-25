#ServerPacketHandler

####Back To [Overview](./Overview.html)

##Overview

ServerPacketHandler (SPH) is the interface between the [server thread](./Server.html) and the chipmunck/physics [thread](ServerMain.html).  Its most important function is as the translator between the [stroke](./Stroke.html) representation of the the [world](./World.html) and the [packet] type information which is sent over the network.

##Data Transfer

ServerPacketHandler effectively translates information from the network for the world and vice versa.  On the side facing the server, SPH sends in and out Packets and Wrappers so that packets of information can be sent and received in conjunction with the connection the data is coming from or going to.

On the side of ServerPacketHandler facing the world, this subsystem represents information in strokes.  SPH creates new strokes and adds to their length until they have been completed by the client who created them.  It can also receive all pertinent information about strokes to convert it into packet form to be sent to clients.

##SPH Process Queue

The primary method of the ServerPacketHandler is:

gboolean sph_process_queue( World*)

This method is the sole recipients of information off the [server's](./Server.html) recv_queue.  That means there is a major inflow of a broad variety of information specifically to this method and the system must be capable of handling all of it correctly.  Thankfully [packets](./Packet.html), the sole information type sent over the network, are all enumerated, with each information type under a different description.  Thus a switch is used to separate different information types so they may be correctly deconstructed.  Here are the primary switch cases:

###SEND_MESSAGE and SEND_NAME

These two cases are for the chat feature of the game.  A username is associated with each connection so it is set here.  Then, when messages come it, the username is extracted from the connection and placed into a packet.  Both the username and message are placed in a [PACKET PACKET](./Packet.html) (isn't that fun to say?) and sent directly back to the [server](./Server.html).

###MOUSE_DOWN

This packet type adds a new [stroke](./Stroke.html) to the physics world.  As a result it contains a lot of initial information about a stroke like color and fill.  This is all unpacked from the packet, and then a new stroke is added to the world.  The hash table id number is returned so it can be sent to clients and also so that the connection this information is coming from can continue to add points to the stroke.  The last thing accomplished is that a new stroke is sent to all clients so that the active drawing can be displayed on the client screens.

###MOUSE_MOVE

This packet adds to the length of the stroke currently being drawn.  It also keeps track of total length and if the stroke becomes too long, the current point which wants to be added, cannot.  If the new point is within a permissable length, the point will be added to the physics stroke and also sent off to the clients so they can see the current stroke information.

###MOUSE_UP

This packet type indicates a user is finished drawing a stroke.  If the stroke has been correctly closed and has the correct length,  the world will convert the stroke from shaky mouse inputs to a smooth shape.  (This deletes the client representation of the user created stroke directly.  If the stroke was correctly created, send the entire stroke structure to the clients.  This enum also cleans up temporary iformation associated with drawing a single stroke, like the ink pot and the current stroke information in the connection.

###NEW_CONNECTION

This packet type indicates that a new connection has been added to the client and the entirety of the currently active world should be send to that client (and only that client).

###LOAD_LEVEL

A packet type of this enum indicates that a user wishes to entirely destroy the current world and load a new level.  Accomplished primarily within a private method, this clears the physics thread and clients of the current shapes.  The method then loads a new level, creates a new physics space, and loads the space into the world.  Then each stroke of the newly loaded world is sent out to all connections.


##Packets Triggered by the World

Some packets of information sent out to the clients are not triggered by user input but rather directly by the world.  These include functions like world updates to reposition shapes which are moving within the physics world on client screens.  Other direct calls are to delete the stroke represented at a specified location in the hash table, and to tell clients that a win has been detected.
















