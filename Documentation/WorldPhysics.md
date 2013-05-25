#WorldPhysics

####Back To [Overview](./Overview.html)

##Overview
WorldPhysics is the physical representation of a given world and all the bodies it contains. This is where the physical forces of gravity, inertia and friction on bodies are calculated and where shape collisions are detected. The state of the WorldPhysics environment is communicated to [World](./World.html), where physics objects are translated into strokes that can then be displayed in the graphical view of the world. WorldPhysics runs on the server side of this application, not on the client side.

##Chipmunk
The major data structures used to represent the world in WorldPhysics are from the [Chipmunk Physics](http://www.chipmunk-physics.net/) library.  The world itself is nothing more than a cpSpace containing cpShapes attached to cpBodies. Chipmunk takes care of simulating the motion and behavior of the bodies over time.

##Loading Level Environments
WorldPhysics instantiates cpBodies and cpShapes from pre-made level configurations ([Level](./level.html) structures). It uses the body/shape properties provided (x, y, width, height, radius, color, etc) to create the appropriate cpBodies and cpShapes and add them to the cpWorld. For the purposes of our game, none of the shapes that are part of the except for the ball need to move, so all shapes except the ball are attached to the static body of the cpSpace.

###Dimensions  
For uniformity, we normalize the dimensions of the cpSpace to be 1 x 1 by dividing by the level's specified width and height where necessary.

###Body Attributes
When a shape is added to WorldPhysics from the Level struct, all information associated with that shape that will be used down the line in [World](./World.html) is stored as the user data of that shape in a struct called BodyAttributes. BodyAttributes holds the type of the shape and its color, which will eventually be accessed and used to convert the shape into a [Stroke](./Stroke.html) and then to draw it.

##Addition of Bodies/Shapes and Physics Simulation
Bodies and shapes are added to physics world from within [World](./World.html), which contains the cpSpace controlled by WorldPhysics in the World struct. WorldPhysics simulates the physical changes that occur in one timestep with the function *world\_physics\_step*, which steps the cpSpace.

##Collision Detection
WorldPhysics uses collision callbacks (a functionality provided by Chipmunk) to respond to two shapes colliding. Specifically, there are 3 types of callbacks of interest:

1. When two shapes drawn by the same user collide, all bodies and shapes involved in the collision are destroyed. (They are removed from the physics world and this information is communicated to the graphical world to remove them there also.)
2. When a shape falls far out of bounds of the game screen (out of view of the user), it will collide with a destruct border, which destroys the body to which the shape is attached and all other shapes on that body.
3. When the ball collides with the goal, the game is won. This is communicated to the [Server](./Server.html) via a Level Win [Packet](./Packet.html).

##Functions
WorldPhysics contains private helper functions that load the objects that are part of the level. All these objects are loaded when the public function *world\_physics\_load* is called. Objects that are added by the user(s) are added to the cpSpace by [World](./World.html).

Most of the public functions of WorldPhysics provide means for the [Server](./Server.html) to control the game state. There are functions available to create, load, clear and destroy physics worlds as users select new levels to play.

The function *world\_physics\_collision\_add\_type*, provides the capability for a user to "register" with the physics world. WorldPhysics assigns a unique collision type id to each user who joins the game to enable detection of collisions involving shapes drawn by the same user.