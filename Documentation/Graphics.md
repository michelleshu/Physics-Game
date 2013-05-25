#Graphics

####Back To [Overview](./Overview.html)

##Overview

The graphics subsystem takes the hash table of client strokes from [client world](./ClientWorld.html) and graphically represents them using the [cairo](http://www.cairographics.org/) drawing sysystem. 

##Drawing Shapes

Because shapes are represented in a hash table in client world, they must be specially iterated over in GList form using a GFunc type method.  Within the method to draw a specific stroke, strokes are drawn point by point as they are represented within the stroke structure.  (Circles have their own method to be drawn, aren't they special.)  Cairo rotation and transformation function guarentee that the shape has the correct location and rotaion because all shapes are zero centered.  Finally, fill is determined by the shape type because all completed shapes are filled in, but sketches are only filled if they are almost complete.

##Ink Pot

This is a graphical representation of how much ink is left for the user's current or next stroke.  It is always drawn in the bottom left corner of the screen and it's color matches that of the user's strokes.