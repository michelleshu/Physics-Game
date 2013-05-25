/*
Packet.h

Universal structure for arranging ints, doubles, and bytes to send over
the network. The only assumption this makes is that an int is always 4
bytes, a byte is one byte, and a double is 8 bytes.

Note: This does not use htonl or any of those families of functions

Jacob Weiss
*/

#ifndef PACKET_H
#define PACKET_H

typedef struct Packet Packet;

typedef enum
{
	PACKET_TYPE_SEND_MESSAGE = 0,
	PACKET_TYPE_SEND_NAME = 1,
	PACKET_TYPE_MOUSE_DOWN = 2,
	PACKET_TYPE_MOUSE_MOVE = 3,
	PACKET_TYPE_MOUSE_UP = 4,
	PACKET_TYPE_NEW_STROKE = 5,
	PACKET_TYPE_ADD_POINT = 6,
	PACKET_TYPE_DELETE = 7,
	PACKET_TYPE_UPDATE = 8,
	PACKET_TYPE_CLEAR_ALL = 9,
	PACKET_TYPE_PACKETS = 10,
	PACKET_TYPE_NEW_CONNECTION = 11,
	PACKET_TYPE_LOAD_LEVEL = 12,
	PACKET_TYPE_INK_POT = 13,
	PACKET_TYPE_LEVEL_WIN = 14,
	PACKET_TYPE_UNDEFINED // Leave this one on the end
} PacketType;

static const int PACKET_SIZES[] = 
{
	255 + 1,// Send Message
	255 + 1,// Send Name
	51 + 1, // Mouse Down
	16 + 1,	// Mouse Move
	16 + 1, // Mouse Up
	55 + 1, // New Stroke
	20 + 1, // Add Point
	4  + 1,  // Delete
	28 + 1, // Update
	1, // Clear All
	2, // Packets
	1, // New Connection
	2, // Load Level
	2, // Ink Pot
	1, // Level Win
	1 // Undefined
};

typedef enum
{
	SEND_MESSAGE_LENGTH = 0 + 1, // byte
	SEND_MESSAGE_TEXT = 1 + 1, // string
	
	SEND_NAME_LENGTH = 0 + 1, // byte
	SEND_NAME_NAME = 1 + 1, // string
	
	MOUSE_DOWN_X = 0 + 1, // double
	MOUSE_DOWN_Y = 8 + 1, // double
	MOUSE_DOWN_SHAPE_TYPE = 16 + 1, // byte
	MOUSE_DOWN_FILL = 17 + 1, // byte
	MOUSE_DOWN_WIDTH = 18 + 1, // double
	MOUSE_DOWN_R = 26 + 1, // double
	MOUSE_DOWN_G = 34 + 1, // double
	MOUSE_DOWN_B = 42 + 1, // double
	
	MOUSE_MOVE_X = 0 + 1, // double
	MOUSE_MOVE_Y = 8 + 1, // double
	
	MOUSE_UP_X = 0 + 1, // double
	MOUSE_UP_Y = 8 + 1, // double
	
	NEW_STROKE_ID = 0 + 1, // int
	NEW_STROKE_X = 4 + 1, // double
	NEW_STROKE_Y = 12 + 1, // double
	NEW_STROKE_SHAPE_TYPE = 20 + 1, // byte
	NEW_STROKE_FILL = 21 + 1, // byte
	NEW_STROKE_WIDTH = 22 + 1, // double
	NEW_STROKE_R = 30 + 1, // double
	NEW_STROKE_G = 38 + 1, // double
	NEW_STROKE_B = 46 + 1, // double
		
	ADD_POINT_ID = 0 + 1, // int
	ADD_POINT_X = 4 + 1, // double
	ADD_POINT_Y = 12 + 1, // double
		
	DELETE_ID = 0 + 1, // int
		
	UPDATE_ID = 0 + 1, // int
	UPDATE_X = 4 + 1, // double
	UPDATE_Y = 12 + 1, // double
	UPDATE_ROTATION = 20 + 1, // double
		
	LOAD_LEVEL_LEVEL = 0 + 1,
	
	INK_POT_INK = 0 + 1
} PacketIndex;

Packet* packet_new(PacketType type);
Packet* packet_new_from_file_descriptor(int file_descriptor);

// Convenience constructors
Packet* packet_send_message_new(char* message);
Packet* packet_send_name_new(char* name);
Packet* packet_mouse_down_new(double x, double y, char type, char fill, double width, double r, double g, double b);
Packet* packet_mouse_move_new(double x, double y);
Packet* packet_mouse_up_new(double x, double y);
Packet* packet_new_stroke_new(int id, double x, double y, char type, char fill, double width, double r, double g, double b);
Packet* packet_add_point_new(int id, double x, double y);
Packet* packet_delete_new(int id);
Packet* packet_update_new(int id, double x, double y, double theta);
Packet* packet_packets_new();
Packet* packet_new_connection_new();
Packet* packet_clear_all_new();
Packet* packet_load_level_new(char level);

void packet_print(Packet* packet);

PacketType packet_get_type(Packet* packet);
int packet_get_length(Packet* packet);

void packet_add_int(Packet* packet, int data, PacketIndex index);
int packet_get_int(Packet* packet, PacketIndex index);

void packet_add_double(Packet* packet, double data, PacketIndex index);
double packet_get_double(Packet* packet, PacketIndex index);

void packet_add_byte(Packet* packet, char data, PacketIndex index);
char packet_get_byte(Packet* packet, PacketIndex index);

void packet_add_string(Packet* packet, char* data, PacketIndex index);
char* packet_get_string(Packet* packet, PacketIndex index);

void packet_add_packet(Packet** packet_packet, Packet* packet);
Packet* packet_get_packet(Packet* packet_packet, int index);

// Generally don't use these functions, but you could if you wanted
void packet_add_value(Packet* packet, long data, int num_bytes, int index);
long packet_get_value(Packet* packet, int num_bytes, int index);

#endif