CC = gcc
CFLAGS = -g -Wall --std=c99 -pedantic `pkg-config --cflags gtk+-3.0` `pkg-config --cflags glib-2.0`
LIBS = `pkg-config --libs gtk+-3.0` `pkg-config --libs glib-2.0`
LIBPATH = /usr/local/lib
GUIOBJS = ./GUI/ChatDialogWindow.o ./GUI/ColorDialogWindow.o ./GUI/LevelDialogWindow.o ./GUI/StartupDialogWindow.o ./GUI/WinDialogWindow.o ./GUI/AboutDialogWindow.o
CLIENTOBJS = ./ClientMain.o ./Client/Client.o ./Client/ClientPacketHandler.o ./ClientWorld/ClientWorld.o ./Graphics/Graphics.o ./GUI/GameView.o ./GUI/MainWindow.o
SERVEROBJS = ./ServerMain.o ./Server/Server.o ./Server/ServerPacketHandler.o ./Core/level.o ./WorldPhysics/WorldPhysics.o ./World/World.o
GLOBALOBJS = ./Stroke/Stroke.o ./Network/Packet.o
OBJS = $(GUIOBJS) $(CLIENTOBJS) $(SERVEROBJS) $(GLOBALOBJS)

all:	$(TARGET)
	make server
	make client
	@echo YAAAAAAAAAYYYYY!! HOORAYY

# **************************** GLOBAL ****************************************

./Stroke/Stroke.o: ./Stroke/Stroke.c ./Stroke/Stroke.h
	gcc -c $(CFLAGS) -o ./Stroke/Stroke.o ./Stroke/Stroke.c

./Network/Packet.o: ./Network/Packet.c ./Network/Packet.h
	gcc -c $(CFLAGS) -o ./Network/Packet.o ./Network/Packet.c

# **************************** GUI ****************************************
./GUI/AboutDialogWindow.o: ./GUI/AboutDialogWindow.c ./GUI/AboutDialogWindow.h
	gcc -c $(CFLAGS) -o ./GUI/AboutDialogWindow.o ./GUI/AboutDialogWindow.c

./GUI/ChatDialogWindow.o: ./GUI/ChatDialogWindow.c ./GUI/ChatDialogWindow.h ./Client/ClientPacketHandler.o
	gcc -c $(CFLAGS) -o ./GUI/ChatDialogWindow.o ./GUI/ChatDialogWindow.c

./GUI/ColorDialogWindow.o: ./GUI/ColorDialogWindow.c ./GUI/ColorDialogWindow.h
	gcc -c $(CFLAGS) -o ./GUI/ColorDialogWindow.o ./GUI/ColorDialogWindow.c

./GUI/LevelDialogWindow.o:./GUI/LevelDialogWindow.c ./GUI/LevelDialogWindow.h ./Client/ClientPacketHandler.o ./Client/Client.o
	gcc -c $(CFLAGS) -o ./GUI/LevelDialogWindow.o ./GUI/LevelDialogWindow.c

./GUI/StartupDialogWindow.o: ./GUI/StartupDialogWindow.c ./GUI/StartupDialogWindow.h ./Client/ClientPacketHandler.o ./Client/Client.o
	gcc -c $(CFLAGS) -o ./GUI/StartupDialogWindow.o ./GUI/StartupDialogWindow.c

./GUI/WinDialogWindow.o: ./GUI/WinDialogWindow.c ./GUI/WinDialogWindow.h ./GUI/LevelDialogWindow.o ./Client/ClientPacketHandler.o
	gcc -c $(CFLAGS) -o ./GUI/WinDialogWindow.o ./GUI/WinDialogWindow.c

# **************************** CLIENT ****************************************

./Client/Client.o: ./Client/Client.c ./Client/Client.h ./Client/ClientPacketHandler.o ./Network/Packet.o
	gcc -c $(CFLAGS) -o ./Client/Client.o ./Client/Client.c

./Client/ClientPacketHandler.o: ./Client/ClientPacketHandler.c ./Client/ClientPacketHandler.h ./Client/Client.o ./ClientWorld/ClientWorld.o ./Network/Packet.o ./GUI/ChatDialogWindow.o ./GUI/StartupDialogWindow.o ./GUI/WinDialogWindow.o
	gcc -c $(CFLAGS) -o ./Client/ClientPacketHandler.o ./Client/ClientPacketHandler.c

./Graphics/Graphics.o: ./Graphics/Graphics.c ./Graphics/Graphics.h ./Stroke/Stroke.o
	gcc -c $(CFLAGS) -o ./Graphics/Graphics.o ./Graphics/Graphics.c

./ClientWorld/ClientWorld.o: ./ClientWorld/ClientWorld.c ./Stroke/Stroke.o
	gcc -c $(CFLAGS) -o ./ClientWorld/ClientWorld.o ./ClientWorld/ClientWorld.c

./GUI/GameView.o: ./GUI/GameView.c ./GUI/GameView.h ./Graphics/Graphics.o ./ClientWorld/ClientWorld.o ./Client/ClientPacketHandler.o ./Stroke/Stroke.o
	gcc -c $(CFLAGS) -o ./GUI/GameView.o ./GUI/GameView.c

./GUI/MainWindow.o: ./GUI/MainWindow.c ./GUI/MainWindow.h ./GUI/GameView.o ./Client/ClientPacketHandler.o ./Client/Client.o ./GUI/AboutDialogWindow.o ./GUI/ChatDialogWindow.o ./GUI/ColorDialogWindow.o ./GUI/LevelDialogWindow.o ./GUI/StartupDialogWindow.o ./GUI/WinDialogWindow.o
	gcc -c $(CFLAGS) -o ./GUI/MainWindow.o ./GUI/MainWindow.c 

./ClientMain.o: ./ClientMain.c ./GUI/MainWindow.o ./Client/Client.o	
	gcc -c $(CFLAGS) -o ./ClientMain.o ./ClientMain.c 

# **************************** SERVER ****************************************

./Server/Server.o: ./Server/Server.c ./Server/Server.h ./Network/Packet.o ./Server/ServerPacketHandler.o
	gcc -c $(CFLAGS) -o ./Server/Server.o ./Server/Server.c

./Server/ServerPacketHandler.o: ./Server/ServerPacketHandler.c ./Server/ServerPacketHandler.h ./Server/Server.o ./Network/Packet.o ./World/World.o ./Stroke/Stroke.o
	gcc -c $(CFLAGS) -o ./Server/ServerPacketHandler.o ./Server/ServerPacketHandler.c

./Core/level.o: ./Core/level.c ./Core/level.h
	gcc -c $(CFLAGS) -o ./Core/level.o ./Core/level.c

./WorldPhysics/WorldPhysics.o: ./WorldPhysics/WorldPhysics.c ./WorldPhysics/WorldPhysics.h ./Core/level.o
	gcc -c $(CFLAGS) -o ./WorldPhysics/WorldPhysics.o ./WorldPhysics/WorldPhysics.c

./World/World.o: ./World/World.c ./World/World.h ./Stroke/Stroke.o ./WorldPhysics/WorldPhysics.o ./Server/ServerPacketHandler.o
	gcc -c $(CFLAGS) -o ./World/World.o ./World/World.c

./ServerMain.o: ./ServerMain.c ./Core/level.o ./WorldPhysics/WorldPhysics.o ./World/World.o ./Server/Server.o ./Server/ServerPacketHandler.o
	gcc -c $(CFLAGS) -o ./ServerMain.o ./ServerMain.c 

# ********************************************************************

server: $(SERVEROBJS) $(GLOBALOBJS)
	gcc -o ScreaminServer $(CFLAGS) $(LIBS) -L$(LIBPATH) $(SERVEROBJS) $(GLOBALOBJS) ./Chipmunk-Mac/libChipmunk.a -lM
	@echo Server Compile Succeeded!!

client: $(CLIENTOBJS) $(GLOBALOBJS) $(GUIOBJS)
	gcc -g -o ScreaminClient $(CFLAGS) $(LIBS) -L$(LIBPATH) $(CLIENTOBJS) $(GLOBALOBJS) $(GUIOBJS) -lM
	@echo Client Compile Succeeded!!
	
Eserver: $(SERVEROBJS) $(GLOBALOBJS)
	gcc -o ScreaminServer $(CFLAGS) $(LIBS) -L$(LIBPATH) $(SERVEROBJS) $(GLOBALOBJS) ../Chipmunk-6.1.3/src/libchipmunk.a -lm 
	@echo Server Compile Succeeded!!
	
Eclient: $(CLIENTOBJS) $(GLOBALOBJS) $(GUIOBJS)
	gcc -o ScreaminClient $(CFLAGS) $(LIBS) -L$(LIBPATH) $(CLIENTOBJS) $(GLOBALOBJS) $(GUIOBJS) -lm
	@echo Client Compile Succeeded!!

eliza:	$(OBJS)
	gcc -o $(TARGET) $(CFLAGS) $(OBJS) $(LIBS) -L$(LIBPATH) ../Chipmunk-6.1.3/src/libchipmunk.a -lm
	
clean:
	rm -f $(TARGET) $(OBJS)
	
