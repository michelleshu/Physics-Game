#level

####Back To [Overview](./Overview.html)

##Overview
The level subcomponent of the [Server](./Server.html) subsystem is simply where .lvl files are opened and read so that data structures that store information for every shape that is initially present in a given level can be initialized.

An example .lvl file is shown below:

LEVEL 1 5 3 0  
WORLD 40 30  
BOX 10 20   5    8 1 0 0 1  
BOX 10 26   18   5 1 0 1 0  
BOX 10 32.5 27   5 1 1 0 0  
BOX 10 30.5 25.5 1 2 1 0 0  
BOX 10 34.5 25.5 1 2 1 0 0  
POLY 10 10   13 3  0 0  4     0  0  -4 0 1 0  
POLY 10 29   13 3  0 0  0    -4 -4   0 0 1 0  
POLY 10 22.5 17 3  0 0 -13.5  4  0   4 0 1 0  
BALL 0.001 20 0 1 0.9 0.6 0  
GOAL 31 26.5 34 26.5  

The first line of a .lvl file always begins with LEVEL. The numbers following LEVEL represent the following pieces of information:

1. level number
2. number of boxes to be initially present in the level
3. number of polygons to be initially present in the level
4. number of stars to be initially present in the level

The second line of a .lvl file begins with WORLD and the numbers following WORLD are the width and height of the world.

The subsequent lines of a .lvl file begin with an identifying word in all capitals, such as BOX, and then the shape's properties.
There are five types of shapes possible:

1. Boxes
2. Polygons
3. Stars
4. A Ball
5. A Goal

The properties of each of these shapes are described below, and they are ordered in the same order they appear in a .lvl file. Each type of shape has its own data structure that is then filled with the following pieces of information:

####Boxes
* mass
* x-position
* y-position
* width
* height
* r (red)
* g (blue)
* b (green)

####Polygons
* mass
* x-position of the center of the polygon
* y-position of the center of the polygon
* number of vertices
* x and y positions of vertices, relative to the center of the polygon
* r (red)
* g (blue)
* b (green)

####Stars
* mass
* x-position
* y-position
* radius
* r (red)
* g (blue)
* b (green)

####The Ball
* mass
* x-position
* y-position
* radius
* r (red)
* g (blue)
* b (green)

####The Goal
* x\_1-position of the line segment that denotes the bottom of the goal basket
* y\_1-position of the line segment that denotes the bottom of the goal basket
* x\_2-position of the line segment that denotes the bottom of the goal basket
* y\_2-position of the line segment that denotes the bottom of the goal basket