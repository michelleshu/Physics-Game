#World Subsystem

##Functional Design Specification
##Jacob Weiss

###Disclaimer

A la Joel Spolsky, I must say that this specification is not complete, but is current to the best of my knowlege.

###Overview

The job of the World subsystem is to describe the world in such a way that the graphics component can draw it as quickly, easily, and efficiently as possible. Some of the functionality of the world component is provided by the [Chipmunk Physics library](http://chipmunk-physics.net/). [Chipmunk Documentation](http://chipmunk-physics.net/release/ChipmunkLatest-Docs/) 

It is also the job of the physics component to identify all of Chipmunk's shortcomings and fill them in when required. 

###Connections to Other Components

####Graphics

The World must be able to supply necessary information to the graphics component such that the graphics can draw the world. Art style lies in between the World and the Graphics subsystems. The World subsystem must know how to render art. The world must be able to associate art with objects. In the beginning, the art can be simple, solid colors. However, custom patterns and art should be included in the final product.

####Core

The World must be able to load and save "state" files. 

####GUI

There is a one-way link between the GUI and the world. The GUI needs to be able to communicate user input that affects the world to the World subsystem. These interactions include all forms of mouse action. The GUI need not know about what it clicks on, only that there was a click. The World then makes the appropriate changes to the world and informs the graphics subsystem of these changes. The graphics subsystem then paints these changes onto the screen.