
/*

ServerPacketHandler.h

Eliza Hanson

*/

#ifndef SERVERPACKETHANDLER_H
#define SERVERPACKETHANDLER_H

#include <glib.h>
#include "../Network/Packet.h"
#include "Server.h"
#include "../World/World.h"

/*
 * This function makes sure that all packet types coming from the client are handled according to their type.  The switch moves between the different enum cases of the packets which are recieved by poping them off the revieve queue.
 */
gboolean sph_process_queue( World* world );

/*
 This function will send the entire world to all clients currently connected to the server.  It is used whenever an entirely new level is loaded.
 */
void sph_send_world_updates(World* w);

/*
 This function tells all clients to delete a specified stroke from representation.  This is used whenever a stroke is transitioned from the array of mouse points to the geometricized shape.
 */
void sph_send_delete(int id);

/*
 This function tells the clients that a win has been detected so a level won dialouge will pop up.
 */
void sph_send_win();


#endif