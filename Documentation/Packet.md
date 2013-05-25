#Packet

####Back To [Overview](./Overview.html)

##Overview

The Packet encompasses the sending and receiving of data over the network. Packets can contain the following types: int, double, byte, string, and Packet. 

##Structure

Under the hood, a Packet is just an array of char. The only character that is inherently special is the first one, which contains the PACKET\_TYPE. PacketType is an enumerated value defined in Packet.h. Except for the PACKET\_TYPE\_SEND\_MESSAGE and PACKET\_TYPE\_SEND\_NAME types, a length is not incuded in the packet. This is because every packet of a certain type is the same size. The sizes of the packets are defined in PACKET\_SIZES in Packet.h. Each entry in PACKET\_SIZES corresponds to an enumerated PacketType. This way, retrieving the size for a given type of packet is trivial. 

The PacketIndex enumerated type contains the indeces for the various elements of each packet type. This type had to be carefully created such that the values didn't overlap. It relies on the fact that ints and floats are 4 bytes, doubles are 8 bytes, and shorts are 2 bytes. These conditions are checked for each time a Packet is constructed. So far, this has not been a problem. In theory, the packet could be generalized such that these conditions are not necessary. However, it was not required for us to make this change. 

##Packet Types/Packet Indices

* PACKET\_TYPE\_SEND\_MESSAGE : Send a chat message
	* SEND\_MESSAGE\_LENGTH : byte
	* SEND\_MESSAGE\_TEXT : string
* PACKET\_TYPE\_SEND\_NAME : Send a chat user name
	* SEND\_NAME\_LENGTH : byte
	* SEND\_NAME\_NAME : string
* PACKET\_TYPE\_MOUSE\_DOWN : Send a mouse down event
	* MOUSE\_DOWN\_X : double
	* MOUSE\_DOWN\_Y : double
	* MOUSE\_DOWN\_SHAPE\_TYPE : byte
	* MOUSE\_DOWN\_FILL : byte
	* MOUSE\_DOWN\_WIDTH : double
	* MOUSE\_DOWN\_R : double
	* MOUSE\_DOWN\_G : double
	* MOUSE\_DOWN\_B : double
* PACKET\_TYPE\_MOUSE\_MOVE : Send a mouse move event
	* MOUSE\_MOVE\_X : double
	* MOUSE\_MOVE\_Y : double
* PACKET\_TYPE\_MOUSE\_UP : Send a mouse up event
	* MOUSE\_UP\_X : double
	* MOUSE\_UP\_Y : double
* PACKET\_TYPE\_NEW\_STROKE : Create a stroke with the given information
	* NEW\_STROKE\_ID : int
	* NEW\_STROKE\_X : double
	* NEW\_STROKE\_Y : double
	* NEW\_STROKE\_SHAPE\_TYPE : byte
	* NEW\_STROKE\_FILL : byte
	* NEW\_STROKE\_WIDTH : double
	* NEW\_STROKE\_R : double
	* NEW\_STROKE\_G : double
	* NEW\_STROKE\_B : double
* PACKET\_TYPE\_ADD\_POINT : Add a point to the given stroke
	* ADD\_POINT\_ID : int
	* ADD\_POINT\_X : double
	* ADD\_POINT\_Y : double
* PACKET\_TYPE\_DELETE : Delete a stroke
	* DELETE\_ID : int
* PACKET\_TYPE\_UPDATE : Update the position and rotation of a stroke
	* UPDATE\_ID : int
	* UPDATE\_X : double
	* UPDATE\_Y : double
	* UPDATE\_ROTATION : double
* PACKET\_TYPE\_CLEAR\_ALL : Clear the entire screen
* PACKET\_TYPE\_PACKETS : A packet packet
* PACKET\_TYPE\_NEW\_CONNECTION : Put on the receive queue to signify a new connection
* PACKET\_TYPE\_LOAD\_LEVEL : Load the given level
	* LOAD_LEVEL_LEVEL : byte
* PACKET\_TYPE\_INK\_POT : Set the ink pot to the given level
	* INK\_POT\_INK : byte
* PACKET\_TYPE\_LEVEL\_WIN : A level has been won

##Storage

As was previously stated, a Packet is just an array of chars. However, this array of chars is not a string. Therefor, an integer takes 4 bytes, a double takes 8, etc. The PacketIndex enumerated value displays this implicitly. To set or get a value to a packet, you must call the function that corresponds to the type that you wish to set or get. These functions take a Packet, the value, and then a PacketIndex enum. The value is then inserted, byte by byte, into the packet at the given index. Any value may be inserted into any index of any PacketType. No type checking happens. Therefor, the developer must be careful to use only the specifieds indexes with their corresponding values and PacketTypes. 

##Ensuring Cross-Platform Compatability

In order to ensure cross-platform compatability over the network, byte order must be preserved. Functions such as htons are provided, but Packet does not use them. Lets say that a Packet has been constructed and packet\_add\_int has been called with an integer and an index. The value is inserted inserted at the index given by iterating over the bytes of the integer. Bytes are accessed using the right shift operator. The value is anded with 0xFF and then the resulting 8 bits (after being cast to a char) are put into the Packet at index. The value is then left shifted by 8 and anded with 0xFF again. That value is then cast to a char and added to index + 1. This process is continued until all 4 bytes (in the case of an int) are placed into the Packet. To access values from a Packet, this process is simply reversed. To store a double, the double is first stored into a union that contains a double and a long. The value is then extracted as a long. This way, the bit order is preserved. On the receiving end, this process is also reversed. Because binary right and left shift operations do not change between systems, this method of encoding binary data in Packets is compatible on all platforms.

##Sending Packets

Because a Packet is just an array of chars, send() can be called directly on a Packet. The packet length is retrieved using packet\_get\_length.

##Receiving Packets

The Packet provides functionality for receiving packets. The magic happens in the packet\_new\_from\_file\_descriptor function. When select returns on the [server](./Server.html) and new data has been detected, the [server](./Server.html) calls this function and passes in the file descriptor that contains the new data. As it happens, this is the only place in our code where the recv function is called. First, only 1 byte is requested. Because of the structure of a Packet, this byte will always be the PacketType. Because all Packets of a given PacketType are the same length, receiving the rest of the packet is trivial. In the case of PACKET\_TYPE\_SEND\_MESSAGE and PACKET\_TYPE\_SEND\_NAME, a second byte must be received after the PacketType. This is because the second byte contains the length of the string that the Packet contains. Once this length is obtained, the rest of the Packet can be received.

##Packet Packets

Yo dawg. I heard you like Packets, so I put a Packet in your Packet so you can Packet while you Packet.

At times, we found that it would be useful to be able to group packets together. Hence, the Packet Packet was born. However, using Packet Packets is quite a bit slower than using normal packets. This is due to the fact that the length must be calculated. Insertion is, in many ways, similar to a linked list. You must iterate through the packet in order to find the length so that more data can be added. Several of the function that work on Packet Packets are recursive, including packet\_new\_from\_file\_descriptor. In theory, you could add Packet Packets to Packet Packets. However, this operation is untested. Attempting to add a Packet Packet to a Packet Packet currently throws an assertion.





