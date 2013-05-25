
/*
Stroke.h

Public Header File

Jacob Weiss

The Stroke structure represents an array of points that are connected to one another. 
This structure serves a dual purpose. First, the GUI sends strokes to the world to 
be created. And second, the World sends strokes to the Graphics system to be drawn.

In effect, the Stroke should be the only data structure that ever traverses the network
in a client/server split.
*/

#ifndef STROKE_H
#define STROKE_H

#define MAX_STROKE_PERIMETER (.8)

//****************************** Type Definitions ********************************

typedef struct Stroke Stroke;

typedef enum
{
	// Set specifically to 0 so that stroke_is_shape can be implemented simply
	SHAPE_TYPE_UNKNOWN = 0, 
	SHAPE_TYPE_CIRCLE,
	SHAPE_TYPE_POLYGON,
	SHAPE_TYPE_SKETCH
} ShapeType;

//**************************** Public Function Prototypes ***************************

/*
This function allocates, initializes, and returns, one stroke structure.

Note that the stroke initializes with a single point in the array already at (0,0).
Every other point in the stroke is relative to (0,0).

Each stroke should have some unique id attached to it. This is so strokes that
represent the same information can be matched accross the network. This function
sets the id of the stroke. The id should be set before the stroke is sent
*/
Stroke* stroke_new(int id, double initial_x, double initial_y);
Stroke* stroke_new_full(int id, double x, double y, ShapeType type, int fill, double width, double r, double g, double b);

/*
Creates a new stroke in the shape of a box
*/
Stroke* stroke_box_new(double x, double y, double width, double height);

/*
Set the coordinate pointers to something other than the internal x and y values;
*/
void stroke_set_body_coordinates(Stroke* stroke, double* body_x, double* body_y);

/*
Set the rotation pointer
*/
void stroke_set_body_rotation(Stroke* stroke, double* theta);
int stroke_get_rotation(Stroke* stroke, double* theta);

double stroke_get_perimeter(Stroke* s);

int stroke_get_remaining_ink(Stroke* stroke);

int stroke_is_closed(Stroke* stroke);

/*
Gets the unique id for the stroke.
*/
int stroke_get_id(Stroke* s);

void stroke_circle_get_center(Stroke* s, double* cx, double* cy);
double stroke_circle_get_radius(Stroke* s);

/*
Take a stroke and turn it into data that can be sent over the network.
*/
const char* stroke_serialize(const Stroke* s);

/*
Take data from the network and turn it into a stroke.
*/
Stroke* stroke_new_from_data(const char* data);

int* merge_arrays(int* array_1, int array_1_length, int* array_2, int array_2_length);

/*
Update this stroke with the information contained in data.
*/
void stroke_update_from_data(const char* data);

/*
This function destroys the specified stroke structure.
*/
void stroke_destroy(Stroke* s);

int stroke_is_dead(Stroke* s);

/*
Print out the internal array of points. This seems like a useful function to have.
*/
void stroke_print_points(const Stroke* s);

/*
Appends a point to the stroke structure.
Assumes that a line connects previous point to this one.
Returns the new perimeter
*/
double stroke_append_point(Stroke* s, double x, double y);

/*
Replaces the specified point with the provided point.
*/
//void stroke_replace_point(Stroke* s, Point p, int index);

/*
Prepends the specified point to the stroke.
I don't think this should be necessary.
*/
//void stroke_prepend_point(Stroke* s, Point p);

/*
Inserts the specified point into the stroke after index.
*/
//void stroke_insert_point(Stroke* s, Point p, int index);

/*
Gets the point at index.
*/
void stroke_get_point(Stroke* s, int index, double* x, double* y);

/*
Gets the length of the stroke.
*/
int stroke_get_length(Stroke* s);

/*
Get the shape type
*/
ShapeType stroke_get_shape_type(Stroke* stroke);

void stroke_set_shape_type(Stroke* stroke, ShapeType type);

void stroke_adjust_coordinate_frame(Stroke* stroke, double x_adjust, double y_adjust);

/*
Sets whether the stroke should be filled in, or an outline.
*/
void stroke_set_fill(int should_fill);

/*
Returns whether or not the stroke should be filled.
*/
int stroke_should_fill(const Stroke* s);

/*
Set the rotation angle for the Stroke.
*/
void stroke_set_rotation(Stroke* s, double angle);

/*
Set the position of the Stroke.
*/
void stroke_set_position(Stroke* s, double x, double y);

/*
Get the position of the first point of the stroke.
*/
int stroke_get_position(Stroke* s, double* x, double* y);

/*
Gets the shape type of the Stroke.
*/
ShapeType stroke_get_shape(const Stroke* s);

gboolean stroke_get_fill(Stroke* stroke);

/*
Tests if the shape of this stroke has been defined yet.
(All this does is cast stroke_get_shape to an int and return it)
*/
int stroke_is_shape(const Stroke* s);

/*
Sets the color of the stroke. For now, we just use
RGB values. Each RGB component should be 0≤RGB≤1.
*/
void stroke_set_color(Stroke* s, double red, double green, double blue);

/*
Gets the color of the stroke.
*/
void stroke_get_color(Stroke* s, double* red, double* green, double* blue);

/*
Analyzes the stroke to determine what kind of shape
it represents. A new stroke is returned that represents
the analyzed stroke. This is going to be one very fun
function to write! The stroke returned should be much
simpler than the one submitted. For example, the input
stroke may have a couple hundred points. If it is 
decided that this stroke is a rectangle, then the resulting
shape should only have 4 points.
*/
Stroke* stroke_analyze_shape(Stroke* s);

/*
Modifies the from stroke in such a way that if this
function is called "steps" number of times, the from
stroke and the to stroke will be identical. I have
no idea how this would work, but it seems cool.

If we can get this to work, one cool thing would be 
to have a stroke, analyze it for it's shape, and then
use this function to morph the raw user input into the
analyzed shape.
*/
void stroke_morph(Stroke* from, const Stroke* to, int steps);

#endif


















