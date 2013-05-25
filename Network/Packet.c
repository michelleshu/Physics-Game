/*
Packet.c

Jacob Weiss
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
	
#include <sys/socket.h>

#include "Packet.h"

typedef struct
{
	char type;
	char data;
} _Packet;

typedef union
{
	double Double;
	long Long;
} DtoL;

//**************************** Private Function Prototypes ***************************

static int packet_index_is_valid(_Packet* p, int size, PacketIndex index);
static int packet_packet_get_length(_Packet* p, int* num_packets);

//************************* Public Function Implementations **************************

Packet* packet_new(PacketType type)
{
	assert(sizeof(int) == 4);
	assert(sizeof(double) == 8);
	assert(sizeof(float) == 4);
	assert(sizeof(short) == 2);
	
	_Packet* packet;
	
	packet = (_Packet*)calloc(PACKET_SIZES[type], sizeof(char));
	
	packet->type = (char)type;
	
	if (packet->type == PACKET_TYPE_PACKETS)
	{
		packet->data = PACKET_TYPE_UNDEFINED;
	}
	
	return (Packet*)packet;
}

Packet* packet_send_message_new(char* message)
{
	Packet* packet = packet_new(PACKET_TYPE_SEND_MESSAGE);
	packet_add_string(packet, message, SEND_MESSAGE_TEXT);
	return packet;
}
Packet* packet_send_name_new(char* name)
{
	Packet* packet = packet_new(PACKET_TYPE_SEND_NAME);
	packet_add_string(packet, name, SEND_NAME_NAME);
	return packet;
}
Packet* packet_mouse_down_new(double x, double y, char type, char fill, double width, double r, double g, double b)
{
	Packet* packet = packet_new(PACKET_TYPE_MOUSE_DOWN);
	packet_add_double(packet, x, MOUSE_DOWN_X);
	packet_add_double(packet, y, MOUSE_DOWN_Y);
	packet_add_byte(packet, type, MOUSE_DOWN_SHAPE_TYPE);
	packet_add_byte(packet, fill, MOUSE_DOWN_FILL);
	packet_add_double(packet, width, MOUSE_DOWN_WIDTH);
	packet_add_double(packet, r, MOUSE_DOWN_R);
	packet_add_double(packet, g, MOUSE_DOWN_G);
	packet_add_double(packet, b, MOUSE_DOWN_B);
	return packet;
}
Packet* packet_mouse_move_new(double x, double y)
{
	Packet* packet = packet_new(PACKET_TYPE_MOUSE_MOVE);
	packet_add_double(packet, x, MOUSE_MOVE_X);
	packet_add_double(packet, y, MOUSE_MOVE_Y);
	return packet;
}
Packet* packet_mouse_up_new(double x, double y)
{
	Packet* packet = packet_new(PACKET_TYPE_MOUSE_UP);
	packet_add_double(packet, x, MOUSE_UP_X);
	packet_add_double(packet, y, MOUSE_UP_Y);
	return packet;
}
Packet* packet_new_stroke_new(int id, double x, double y, char type, char fill, double width, double r, double g, double b)
{
	Packet* packet = packet_new(PACKET_TYPE_NEW_STROKE);
	packet_add_int(packet, id, NEW_STROKE_ID);
	packet_add_double(packet, x, NEW_STROKE_X);
	packet_add_double(packet, y, NEW_STROKE_Y);
	packet_add_byte(packet, type, NEW_STROKE_SHAPE_TYPE);
	packet_add_byte(packet, fill, NEW_STROKE_FILL);
	packet_add_double(packet, width, NEW_STROKE_WIDTH);
	packet_add_double(packet, r, NEW_STROKE_R);
	packet_add_double(packet, g, NEW_STROKE_G);
	packet_add_double(packet, b, NEW_STROKE_B);
	
	return packet;
}
Packet* packet_add_point_new(int id, double x, double y)
{
	Packet* packet = packet_new(PACKET_TYPE_ADD_POINT);
	packet_add_int(packet, id, ADD_POINT_ID);
	packet_add_double(packet, x, ADD_POINT_X);
	packet_add_double(packet, y, ADD_POINT_Y);
	return packet;
	
}
Packet* packet_delete_new(int id)
{
	Packet* packet = packet_new(PACKET_TYPE_DELETE);
	packet_add_int(packet, id, DELETE_ID);
	return packet;
}
Packet* packet_update_new(int id, double x, double y, double theta)
{
	Packet* packet = packet_new(PACKET_TYPE_UPDATE);
	packet_add_int(packet, id, UPDATE_ID);
	packet_add_double(packet, x, UPDATE_X);
	packet_add_double(packet, y, UPDATE_Y);
	packet_add_double(packet, theta, UPDATE_ROTATION);
	return packet;
}
Packet* packet_packets_new()
{
	Packet* packet = packet_new(PACKET_TYPE_PACKETS);
	return packet;
}
Packet* packet_new_connection_new()
{
	Packet* packet = packet_new(PACKET_TYPE_NEW_CONNECTION);
	return packet;
}
Packet* packet_clear_all_new()
{
	Packet* packet = packet_new(PACKET_TYPE_CLEAR_ALL);
	return packet;
}
Packet* packet_load_level_new(char level)
{
	Packet* packet = packet_new(PACKET_TYPE_LOAD_LEVEL);
	packet_add_byte(packet, level, LOAD_LEVEL_LEVEL);
	return packet;
}

void packet_add_packet(Packet** packet_packet_pointer, Packet* packet)
{
	Packet* packet_packet = *packet_packet_pointer;
	
	assert(packet_get_type(packet_packet) == PACKET_TYPE_PACKETS);
	assert(packet_get_type(packet) != PACKET_TYPE_PACKETS);
	
	int num_packets;
	int length = packet_packet_get_length((_Packet*)packet_packet, &num_packets);// - 1;
	int length_to_add = packet_get_length(packet);
		
 	int total_length = length + length_to_add;// + 1;
		
	*packet_packet_pointer = (Packet*)realloc((char*)packet_packet, total_length);
		
	char* character_packet = (char*)(*packet_packet_pointer);
	
	character_packet[total_length - 1] = PACKET_TYPE_UNDEFINED;
		
	memcpy(character_packet + length - 1, packet, length_to_add);
}

Packet* packet_get_packet(Packet* packet_packet, int index)
{
	char* character_packet = (char*)packet_packet;
	
	int packet_index = 1;
	
	for (int i = 0; i < index; i++)
	{
		Packet* next_packet = (Packet*)(character_packet + packet_index);
		if (packet_get_type(next_packet) == PACKET_TYPE_UNDEFINED)
		{
			break;
		}
		packet_index += packet_get_length(next_packet);
	}
		
	return (Packet*)(character_packet + packet_index);
}

Packet* packet_new_from_file_descriptor(int file_descriptor)
{
	PacketType packet_type = 0;
	
	// Read the first value for packet type information
	int num_recieved = recv(file_descriptor, &packet_type, 1, 0);
	
	if (num_recieved == 0)
	{
		return NULL;
	}
	if (packet_type == PACKET_TYPE_UNDEFINED)
	{
		printf("packet type undefined");
		return NULL;
	}	
		
	assert(packet_type >= 0 && packet_type < PACKET_TYPE_UNDEFINED);
						
	int packet_size = PACKET_SIZES[(int)packet_type];
	
	char* packet = (char*)packet_new(packet_type);
		
	switch (packet_type)
	{
		case PACKET_TYPE_SEND_MESSAGE:
		case PACKET_TYPE_SEND_NAME:
		{
			char length;
			num_recieved = recv(file_descriptor, &length, 1, 0);	
			
			if (num_recieved == 0)
			{
				return NULL;
			}
			
			assert(num_recieved == 1);
		
			num_recieved = recv(file_descriptor, packet + 2, length, 0);
			
			if (num_recieved == 0)
			{
				return NULL;
			}
			
			assert(num_recieved == length);		
		
			((char*)packet)[1] = (char)length;	
			
			break;
		}
		case PACKET_TYPE_PACKETS:
		{
			Packet* internal_packet;
			while(1)
			{
				internal_packet = packet_new_from_file_descriptor(file_descriptor);
				if (internal_packet == NULL)
				{
					break;
				}
				else
				{
					packet_add_packet((Packet**)(&packet), internal_packet);
				}
			}
			break;
		}
		case PACKET_TYPE_UNDEFINED:
		{
			return NULL;
			break;
		}
		default:
		{
			int num_recieved = 0;
			while (num_recieved < packet_size - 1)
			{
				int n = recv(file_descriptor, packet + 1 + num_recieved, packet_size - 1 - num_recieved, 0);
				if (n == 0)
				{
					return NULL;
				}
				num_recieved += n;
			}
			assert(num_recieved == packet_size - 1);		
			break;
		}
	}
			
	return (Packet*)packet;
}

void packet_add_value(Packet* packet, long data, int num_bytes, int index)
{	
	_Packet* p = (_Packet*)packet;	
	char* char_p = (char*)packet;
	
	assert(packet_index_is_valid(p, num_bytes, index));
	
	for (int i = 0; i < num_bytes; i++)
	{
		int byte = ((data >> (i * 8)) & 0xFF);
		char_p[i + index] = byte;
	}
}

long packet_get_value(Packet* packet, int num_bytes, int index)
{
	_Packet* p = (_Packet*)packet;
	char* char_p = (char*)packet;
	
	assert(packet_index_is_valid(p, num_bytes, index));
	
	long data = 0;
	for (int i = 0; i < num_bytes; i++)
	{
		long byte = ((long)(char_p[index + i] & 0xFF)) << (i * 8);
		data |= byte;
	}
	
	return data;
}

void packet_add_short(Packet* packet, short data, PacketIndex index)
{
	packet_add_value(packet, data, sizeof(short), index);
}

short packet_get_short(Packet* packet, PacketIndex index)
{
	return (short)packet_get_value(packet, sizeof(short), index);
}

void packet_add_int(Packet* packet, int data, PacketIndex index)
{	
	packet_add_value(packet, data, sizeof(int), index);
}

int packet_get_int(Packet* packet, PacketIndex index)
{	
	return (int)packet_get_value(packet, sizeof(int), index);
}

void packet_add_double(Packet* packet, double d_data, PacketIndex index)
{
	// Convince C that the bits of d_data are actually a long
	DtoL converter;
	converter.Double = d_data;
	
	packet_add_value(packet, converter.Long, sizeof(long), index);
}

double packet_get_double(Packet* packet, PacketIndex index)
{	
	// Convince C that the bits of l_data are actually a double
	DtoL converter;
	converter.Long = packet_get_value(packet, sizeof(double), index);
	return converter.Double;	
}

void packet_add_byte(Packet* packet, char data, PacketIndex index)
{
	packet_add_value(packet, data, sizeof(char), index);
}

char packet_get_byte(Packet* packet, PacketIndex index)
{
	return (char)packet_get_value(packet, sizeof(char), index);
}

void packet_add_string(Packet* packet, char* data, PacketIndex index)
{
	_Packet* p = (_Packet*)packet;
	char* char_p = (char*)packet;
	
	if (p->type != PACKET_TYPE_SEND_MESSAGE && p->type != PACKET_TYPE_SEND_NAME)
	{
		fprintf(stderr, "Packet Not Meant To Hold Strings.\n");
		return;
	}
	
	int i;
	for (i = index; i < PACKET_SIZES[(int)(p->type)]; i++)
	{
		char c = data[i - index];
		char_p[i] = c;
		
		if (c == 0)
		{
			break;
		}
	}
	
	((char*)packet)[1] = (char)(i - 1);
}

char* packet_get_string(Packet* packet, PacketIndex index)
{
	_Packet* p = (_Packet*)packet;
	char* char_p = (char*)packet;
	
	if (p->type != PACKET_TYPE_SEND_MESSAGE && p->type != PACKET_TYPE_SEND_NAME)
	{
		fprintf(stderr, "Packet Not Meant To Hold Strings.\n");
		return NULL;
	}
	
	return char_p + index;
}

PacketType packet_get_type(Packet* packet)
{
	_Packet* p = (_Packet*)packet;
	
	return (PacketType)(p->type);
}

int packet_get_length(Packet* packet)
{	
	_Packet* p = (_Packet*)packet;

	switch (packet_get_type(packet))
	{
		case PACKET_TYPE_SEND_MESSAGE:
		{
			return packet_get_byte(packet, SEND_MESSAGE_LENGTH) + 2;
			break;
		}
		case PACKET_TYPE_SEND_NAME:
		{
			return packet_get_byte(packet, SEND_NAME_LENGTH) + 2;
			break;
		}
		case PACKET_TYPE_PACKETS:
		{
			int num_packets;
			return packet_packet_get_length(p, &num_packets);
			break;
		}
		default:
		{
			return PACKET_SIZES[(int)(p->type)];
			break;
		}
	}
}

void packet_print(Packet* packet)
{
	switch (packet_get_type(packet))
	{
		case PACKET_TYPE_SEND_MESSAGE:
		{
			printf("Send Message: Length=%d, Message=%s\n", 
				packet_get_length(packet), 
				packet_get_string(packet, SEND_MESSAGE_TEXT));
			break;
		}
		case PACKET_TYPE_SEND_NAME:
		{
			printf("Send Name: Length=%d, Name=%s\n", 
				packet_get_length(packet), 
				packet_get_string(packet, SEND_NAME_NAME));
			break;
		}
		case PACKET_TYPE_MOUSE_DOWN:
		{
			printf("Mouse Down: Length=%d, X=%lf, Y=%lf\n", 
				packet_get_length(packet), 
				packet_get_double(packet, MOUSE_DOWN_X), 
				packet_get_double(packet, MOUSE_DOWN_Y));
			break;
		}
		case PACKET_TYPE_MOUSE_MOVE:
		{
			printf("Mouse Move: Length=%d, X=%lf, Y=%lf\n", 
				packet_get_length(packet), 
				packet_get_double(packet, MOUSE_MOVE_X), 
				packet_get_double(packet, MOUSE_MOVE_Y));
			break;
		}
		case PACKET_TYPE_MOUSE_UP:
		{
			printf("Mouse Up: Length=%d, X=%lf, Y=%lf\n", 
				packet_get_length(packet), 
				packet_get_double(packet, MOUSE_UP_X), 
				packet_get_double(packet, MOUSE_UP_Y));
			break;
		}
		case PACKET_TYPE_NEW_STROKE:
		{
			printf("New Stroke: Length=%d, ID=%d, X=%lf, Y=%lf, TYPE=%d, FILL=%d, WIDTH=%lf, R=%lf, G=%lf, B=%lf\n", 
				packet_get_length(packet), 
				packet_get_int(packet, NEW_STROKE_ID),
				packet_get_double(packet, NEW_STROKE_X), 
				packet_get_double(packet, NEW_STROKE_Y),
				packet_get_byte(packet, NEW_STROKE_SHAPE_TYPE),
				packet_get_byte(packet, NEW_STROKE_FILL),
				packet_get_double(packet, NEW_STROKE_WIDTH),
				packet_get_double(packet, NEW_STROKE_R),
				packet_get_double(packet, NEW_STROKE_G),
				packet_get_double(packet, NEW_STROKE_B));
			break;
		}
		case PACKET_TYPE_ADD_POINT:
		{
			printf("Add Point: Length=%d, ID=%d, X=%lf, Y=%lf\n", 
				packet_get_length(packet), 
				packet_get_int(packet, ADD_POINT_ID),
				packet_get_double(packet, ADD_POINT_X), 
				packet_get_double(packet, ADD_POINT_Y));
			break;
		}
		case PACKET_TYPE_DELETE:
		{
			printf("New Stroke: Length=%d, ID=%d\n", 
				packet_get_length(packet), 
				packet_get_int(packet, DELETE_ID));
			break;
		}
		case PACKET_TYPE_UPDATE:
		{
			printf("Update: Length=%d, ID=%d, X=%lf, Y=%lf, T=%lf\n", 
				packet_get_length(packet), 
				packet_get_int(packet, UPDATE_ID),
				packet_get_double(packet, UPDATE_X), 
				packet_get_double(packet, UPDATE_Y),
				packet_get_double(packet, UPDATE_ROTATION));
			break;
		}
		case PACKET_TYPE_PACKETS:
		{
			printf("Packets: Length=%d\n", packet_get_length(packet));
			int i = 0;
			Packet* p = NULL;
			do
			{
				printf("\t");
				p = packet_get_packet(packet, i);
				packet_print(p);
				i++;
			} while (packet_get_type(p) != PACKET_TYPE_UNDEFINED);
			break;
		}
		case PACKET_TYPE_NEW_CONNECTION:
		{
			printf("New Connection: Length=1\n");
			break;
		}
		case PACKET_TYPE_INK_POT:
		{
			printf("Ink: %d\n", packet_get_byte(packet, INK_POT_INK));
			break;
		}
		case PACKET_TYPE_UNDEFINED:
		default:
		{
			printf("Packet Undefined\n");
			break;
		}
	}
}

//************************* Private Function Implementations *************************

static int packet_packet_get_length(_Packet* p, int* num_packets)
{
	assert(packet_get_type((Packet*)p) == PACKET_TYPE_PACKETS);
	
	int total_length = 1;
	char* character_packet = (char*)p;
	int counter = 0;
	
	while (1)
	{
		Packet* next_packet = (Packet*)(character_packet + total_length);
		if (packet_get_type(next_packet) == PACKET_TYPE_UNDEFINED)
		{
			break;
		}
		counter++;
		total_length += packet_get_length(next_packet);	
	}
	
	*num_packets = counter;
	return total_length + 1;	
}

static int packet_index_is_valid(_Packet* p, int size, PacketIndex index)
{	
	return (index + size <= PACKET_SIZES[(int)(p->type)]);
}

/*int main(int argc, char* argv[])
{
	Packet* pp = packet_new(PACKET_TYPE_PACKETS);
	
	Packet* np = packet_new(PACKET_TYPE_SEND_NAME);
	packet_add_string(np, "Jacob Weiss", SEND_MESSAGE_TEXT);
	
	Packet* mp = packet_new(PACKET_TYPE_SEND_MESSAGE);
	packet_add_string(mp, "Hello, World!", SEND_MESSAGE_TEXT);
	
	Packet* up = packet_new(PACKET_TYPE_UPDATE);
	packet_add_int(up, 42, UPDATE_ID);
	packet_add_double(up, 123.456, UPDATE_X);
	packet_add_double(up, 111.222, UPDATE_Y);
	packet_add_double(up, 3.14, UPDATE_ROTATION);

	//packet_add_packet(&pp, np);
	packet_add_packet(&pp, mp);
	//packet_add_packet(&pp, up);
	//packet_add_packet(&pp, mp);
	packet_print(pp);
	
	return EXIT_SUCCESS;
}
*/













