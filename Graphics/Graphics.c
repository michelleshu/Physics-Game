/*
Graphics.c

Implementation File

Jacob Weiss
*/

#include "Graphics.h"
#include "../Stroke/Stroke.h"
#include <stdlib.h>
#include <math.h>
#include "../Client/ClientPacketHandler.h"
		
typedef struct
{
	GtkWidget* drawing_area;
	cairo_t* cr;
} Context;
	
//**************************** Private Function Prototypes ***************************

void draw_stroke(Stroke* s, Context* context);

//************************* Public Function Implementations **************************

void draw_ink_bar(GtkWidget* drawing_area, cairo_t* cr, GdkColor color)
{
	//Get the amount of ink remaining in the bar
	int width = gtk_widget_get_allocated_width(drawing_area);
	int height = gtk_widget_get_allocated_height(drawing_area);
	
	int border = 10;
	int amount = cph_get()->ink;
	
	//Draw the solid rectangle representing the remainin ink in the stroke
	cairo_move_to(cr, border, height - border);
	cairo_line_to(cr, border + width / 3 * amount / 255.0, height - border);
	cairo_line_to(cr, border + width / 3 * amount / 255.0, height - border - 20);
	cairo_line_to(cr, border, height - border - 20);
	cairo_line_to(cr, border, height - border);
	
	cairo_set_source_rgb(cr, color.red / 255.0, color.green / 255.0, color.blue / 255.0);
	cairo_fill(cr);
	
	cairo_set_source_rgb(cr, 0, 0, 0);
	
	//Draw an unfilled box representing the ink which has already been used.
	cairo_move_to(cr, border, height - border);
	cairo_line_to(cr, border + width / 3, height - border);
	cairo_line_to(cr, border + width / 3, height - border - 20);
	cairo_line_to(cr, border, height - border - 20);
	cairo_line_to(cr, border, height - border);
	
	cairo_stroke(cr);	
}

gboolean graphics_draw(GtkWidget* drawing_area, cairo_t* cr, GList* strokes, GdkColor color)
{
	//Paint the background white. This is a function we may want to change in the future as we decide to load background pictures etc.
	//cairo_set_source_rgba(cr, 0, 0, 0, 0);
	//cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
	//cairo_paint(cr);
	
	Context context = {drawing_area, cr};
	
	//Go through the hash table of strokes using the iterator and draw each stroke.
	g_list_foreach(strokes, (GFunc)draw_stroke, (gpointer)(&context));

	//Draw the ink bar representing the remaining ink in the stroke currently being drawn
	draw_ink_bar(drawing_area, cr, color);

	return FALSE;
}

//************************* Private Function Implementations *************************
	
//Draw an individual stroke.  This has the form of a GFunc so it can be used within the GList iterator
void draw_stroke(Stroke* s, Context* context)
{
	// Fixes a weird race condition where a stroke that has been removed from
	// the hash table will still be attempted to get drawn.
	
	//Get both the widget and the cairo drawing area
	GtkWidget* drawing_area = context->drawing_area;
	cairo_t* cr = context->cr;
	
	//Get the size of the drawing area so that shapes will continue to be drawn to scale even if the size of the drawing window changes.
	double width = (double)gtk_widget_get_allocated_width(drawing_area);
	double height = (double)gtk_widget_get_allocated_height(drawing_area);
	
	//Set the shape color.
	double red;
	double blue;
	double green;
	stroke_get_color(s, &red, &blue, &green);
	cairo_set_source_rgb(cr, red, blue, green);
	int length = stroke_get_length(s);
	
	//If the stroke is empty dont actually draw anything.
	if (length == 0)
	{
		return;
	}
		
	//Get the offset of the center of mass from (0, 0) and the angle of rotation to translate the shape from the physical number representation to information which is appopriately ready by cairo.
	double dx;
	double dy;
	stroke_get_position(s, &dx, &dy);
	double theta;
	stroke_get_rotation(s, &theta);
				
	double x;
	double y;
			
	//Save the shift that previously existed because you need to get back to those coordinates once the path being drawn has been completed.
	cairo_matrix_t saved_transform;
	cairo_get_matrix(cr, &saved_transform);

	//Apply unique rotation and transformation of the shape.
	cairo_scale(cr, width, height);
	cairo_translate(cr, dx, dy);
	cairo_rotate(cr, theta);		
	
	//Circles must be handled separately because they are drawn as degrees around a radius.  
	if (stroke_get_shape_type(s) == SHAPE_TYPE_CIRCLE)
	{
		double cx, cy, r;
		stroke_circle_get_center(s, &cx, &cy);
		r = stroke_circle_get_radius(s);
		
		cairo_arc(cr, cx, cy, r, 0, 2 * G_PI);
	}
	else
	{
		//Only draw strokes and move x and y if there are actually points within the stroke.
		if (length > 0)
		{
			stroke_get_point(s, 0, &x, &y);
			cairo_move_to(cr, x, y);
		}
		
		//Loop through all remaining points and append them to the path.
		for (int j = 1; j < length; j++)
		{
			stroke_get_point(s, j, &x, &y);
			cairo_line_to(cr, x, y);
		}
			
		//Only polygons are closed in this graphics, so limit the paths which are closed.
		if (stroke_get_shape_type(s) == SHAPE_TYPE_POLYGON)
		{
			cairo_close_path(cr);
		}
	}
	
	cairo_set_matrix(cr, &saved_transform);
	
	// Fill in the stroke if the beginning and ending points are close enough together that the shape will be accepted.
	if (stroke_get_fill(s) || (stroke_get_shape_type(s) == SHAPE_TYPE_SKETCH && stroke_is_closed(s)))
	{
		cairo_fill_preserve(cr);
		cairo_set_source_rgb(cr, red / 2, green / 2, blue / 2);
		cairo_stroke(cr);
	}
	//If the stroke is a scetch and it is not closed enough to be accepeted, do not fill it in, only show the outline
	else if (stroke_get_shape_type(s) == SHAPE_TYPE_SKETCH)
	{
		cairo_set_source_rgb(cr, red / 2, green / 2, blue / 2);
		cairo_stroke(cr);
	}
	//Otherwise complete the stroke.
	else
	{
		cairo_stroke(cr);
	}	
}

















