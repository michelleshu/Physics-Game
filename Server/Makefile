
CC = gcc
CFLAGS = -g -Wall -std=c99 -pedantic `pkg-config --cflags gtk+-3.0` -DDEBUG
LIBS = `pkg-config --libs gtk+-3.0` 
LIBPATH = /usr/local/lib


# ******* make the server.... ***********

server: Server.c
	$(CC) $(CFLAGS) $(LIBS) -L$(LIBPATH) Server.c Packet.c -o server

client: Client.c ../Network/Packet.c
	$(CC) $(CFLAGS) $(LIBS) -L$(LIBPATH) Client.c Packet.c -o client