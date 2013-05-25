#World

####Back To [Overview](./Overview.html)

##Overview

The World provides some of the most important functions to the [game](./Game.html). At a very high level, the World's only job is to connect the cpSpace to the [Stroke](./Stroke.html). To do this, the World must do two very important things.

1. Load a cpSpace
2. Manipulate Strokes

##Structure

The World structure only contains three members: a GArray of [Strokes](./Stroke.html), a cpSpace, and a counter variable (called num\_strokes). The first two members will be discussed in the following sections. The counter variable, however, is not exactly a counter. Every time a new [Stroke](./Stroke.html) is added to the World, the [Stroke](./Stroke.html) must be assigned a unique id. num\_strokes provides this id. The name num\_strokes is, however, slightly misleading, as it never decreases. A better name would have been num\_strokes\_existed or even next\_stroke\_id.

##Loading a cpSpace

Although [WorldPhysics](./WorldPhysics.html) is in charge of loading the cpSpace from the [level](./level.html), World has to be able to take this cpSpace and represent turn its cpBodies into [Strokes](./Stroke.html). This process begins in the world\_load\_space function, which in turn calls the world\_shape\_iterate function. In this function, cpShapes are processed and turned into [Strokes](./Stroke.html), which are then linked to the cpBody (see [Stroke](./Stroke.html)) and added to the World's hash table.  

##Stroke Manipulation

The World provides functions that add, remove, modify, and get [Strokes](./Stroke.html). These functions are all fairly trivial and mostly exist to link the [ServerPacketHandler](./ServerPacketHandler.html) to the World.

##Triangulation

When a [Stroke](./Stroke.html) is finished, several functions get called. First, the [Stroke](./Stroke.html) is analyzed (see [Stroke](./Stroke.html)). If the [Stroke](./Stroke.html) is a circle, then a cpBody and cpShape are constructed and added to the cpSpace. If the [Stroke](./Stroke.html) is a polygon and it fails cpPolyValidate, then it must be triangulated.

Of the three algorithms that I wrote (polygon approximation, circle detection, and triangulation), the triangulation algorithm is by far the slowest and least elegant. However, it works, and for our purposes, it runs fast enough. At a high level, the algorithm employs [ear clipping](http://en.wikipedia.org/wiki/Polygon_triangulation#Ear_clipping_method). In order to work effectively, though, once an ear is identified, all other points must be checked to ensure that no points exist inside the ear that is to be clipped. In the case that the ear has a point inside it, the ear is skipped. If the entire polygon is traversed without finding a valid ear, then the triangulation fails. 

According to the two-ear theorem, every simple polygon has at least two ears. Therefore, ever time the polygon's vertices are traversed, at least two ears are found and removed. The amount of work required is therefore approximately: N + N-2 + N-4 + ... + N-N = (N^2)/2 + (N^2)/4 = 3/4 * N^2 = O(N^2). However, when an ear is found, another order N operation must be performed to check if it overlaps any other ears. This must, in some way, negatively affect the runtime. I would guess that the runtime is somewhere between O(N^2) and O(N^3) or worse.

If an ear is not found after traversing the entire polygon, then the polygon is necessarily invalid and the triangulation fails. Sometimes, invalid polygons don't fail triangulation. In this case, the behavior is undefined. One way to fix this would be to check the polygon for self-intersections. If there are any self-intersections, then the polygon is necessarily invalid and should be discarded.

