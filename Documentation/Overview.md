#Screamin* Green Documentation

##Contributors
####Jacob Weiss, Eliza Hanson, Joyce Chung, Michelle Shu

##Overview

###Strengths
1. Rapid, efficient network communication (i.e. no lag): Due to our simplified representation of shapes (see [Stroke](./Stroke.html)), drawings are loaded to the graphical display in an efficient manner, so we experience no lag. Also, the Packet structure enables small amounts of information to travel quickly to their proper destinations over the network.
2. Stable: As far as we know, the program is reliable and will not crash, even with many users drawing shapes simultaneously over the network.
3. Information Flow and High Level Design: The flow between subsections is organized and streamlined. Everything has its place and there are few (if any) unnecessary components bogging down the program.

###Weaknesses
1. Some files (such as [Packet](./Packet.html)) lack commenting and may not be as readable to developers.
2. Memory leaks may still exist. We didn't thoroughly examine the Valgrind reports for our server and client code, because we couldn't manage to suppress errors that were purely a result of GTK or exiting the server abruptly with Ctrl-C.
3. Some minor aspects of [ServerMain](./ServerMain.html) are not entirely compatible with Linux as they stand. We use functions for timekeeping and signalling that work in Mac but not Linux.

###Bugs

The code found in the RELEASE folder of our svn repository is fully functional and bug free, in the sense that it will not crash. However, there are several imperfections that we have found.

1. In the [Client](./Client.html), cph\_process\_queue is being called in the client thread. This caused a race condition when a [stroke](./Stroke.html) gets deleted from the world in the middle of the graphics callback. This bug was fixed by adding a property to the stroke called is\_dead. When the stroke got destroyed, is\_dead would be set to TRUE. Graphics would then know not to draw that stroke. In RELEASEv2, cph\_process\_queue was moved into a g\_timeout where it gets called from the main thread. This was the intended behavior of the code.
2. If a stroke is drawn that intersects itself, there is a chance that triangulation will not fail and the stroke will be considered valid. In this case, the outcome of the physics engine is undefined. Sometimes it will collide with other objects, and sometimes it will not. To fix this, we would simply need to implement a check to make sure that strokes do not intersect themselves.
3. When the user exits the GUI, the thread that runs the [client](./Client.html) is not explicitly closed. A function for this is provided (client\_stop) and adding it to the close procedure would be trivial, but we forgot to.
 
<table>
<tr>
<td style="vertical-align: top;">
##Index	
* [Game](./Game.html) 
* [Server](./Server.html)
	* [ServerMain](./ServerMain.html) 
	* [ServerPacketHandler](./ServerPacketHandler.html)
	* [level](./level.html) 
	* [World](./World.html)
	* [WorldPhysics](./WorldPhysics.html) 
* [Client](./Client.html)
	* [ClientMain](./ClientMain.html)
	* [ClientPacketHandler](./ClientPacketHandler.html)
	* [Graphics](./Graphics.html)
	* [ClientWorld](./ClientWorld.html)
	* [GUI](./GUI.html)
* Global
	* [Packet](./Packet.html)
	* [Stroke](./Stroke.html)
</td>
<td>
![Figure 1](./Flow\ Chart.png)
</td>
</tr>
</table>

