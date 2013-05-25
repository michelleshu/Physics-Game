#ClientWorld

####Back To [Overview](./Overview.html)

##Overview

ClientWorld is the client-side analogue of [World](./World.html). It exists simply as a convenient place to store [Strokes](./Stroke.html) before they are requested by [Graphics](./Graphics.html). The ClientWorld structure has only one member, a GHashTable. All of ClientWorld's functions are for manipulating this data structure. When requested to do so by the [ClientPacketHandler](./ClientPacketHandler.html), ClientWorld can add/remove/modify [Strokes](./Stroke.html) in its hash table.