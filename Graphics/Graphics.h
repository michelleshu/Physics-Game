/*
graphics.h

This file is the primary control of the graphics system.  All calls to draw individual shapes, structs and images originate from information passed in here.  It takes a cairo_t pointer from the callback as well as information about the stroke superstructure. (Note: The exact format of this super structure still needs to be discussed.)

Eliza Hanson
Jacob Weiss
2/14/13

*/

//DONT FORGET TO DELETE OLD INFORMATION AT THE BOTTOM.


#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <gtk/gtk.h>
	
/*
This will be linked to the GUI as the draw callback so it has the 
capability of drawing all images which should be occuring in the world.  
It is passed a reference to the surface being drawn as well as the 
super-structure containing all strokes so that it has references to 
every component of the world which is going to be drawn.  This function 
separates out the background images from the structures which need 
to be drawn in.  These are handled by different mechanisms.
*/
gboolean graphics_draw(GtkWidget* drawing_area, cairo_t* cr, GList* strokes, GdkColor color);

#endif

