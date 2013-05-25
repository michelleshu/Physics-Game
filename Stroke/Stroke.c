/*
Stroke.c

Implementation File

Jacob Weiss
*/

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <assert.h>	
#include <math.h>
#include "Stroke.h"

/*
#define THETA_DELTA_MIN (10 * G_PI / 180)
#define RADIUS_DELTA_MIN (.01)
#define MIN_POINTS_FOR_SIDE (0)
#define CHECK_AHEAD_NUM (1)
*/
	
#define RMS_MIN .006
#define CIRCLE_RMS 1
	
#define MAX_CLOSE_DISTANCE .08
	
typedef struct
{
	int id;
	GArray* points;
	ShapeType type;
	double x;
	double y;
	double theta;
	
	double perimeter;
	
	double previous_x;
	double previous_y;
	double previous_theta;
	
	double x_adjust;
	double y_adjust;
	
	double* body_x;
	double* body_y;
	double* body_theta;
	
	gboolean fill;
	double stroke_width;
	
	double red;
	double blue;
	double green;
	
	int creator_id;
	
	int is_dead;
} _Stroke;

/*
I define Point here because it is extremely simple and should only be necessary in
conjunction with the stroke structure. Also, I do not believe that there are any
functions that I want to associate with point, so the fields can be accessed 
directly.
*/
typedef struct
{
	double x;
	double y;
} Point;

//**************************** Private Function Prototypes ***************************

_Stroke* stroke_alloc();

_Stroke* stroke_init(_Stroke* stroke, int id, double initial_x, double initial_y);
_Stroke* stroke_init_full(_Stroke* stroke, int id, double x, double y, ShapeType type, int fill, double width, double r, double g, double b);
static double get_distance_squared(double x1, double y1, double x2, double y2);
static double stroke_distance_from_index_to_line_connecting_two_indices(Stroke* stroke, int point_index, int line_index_a, int line_index_b);

//************************* Public Function Implementations **************************

Stroke* stroke_new(int id, double initial_x, double initial_y)
{
	return (Stroke*)stroke_init(stroke_alloc(), id, initial_x, initial_y);
}

Stroke* stroke_new_full(int id, double x, double y, ShapeType type, int fill, double width, double r, double g, double b)
{
	return (Stroke*)stroke_init_full(stroke_alloc(), id, x, y, type, fill, width, r, g, b);
}

Stroke* stroke_box_new(double x, double y, double width, double height)
{
	return NULL;
}	

double stroke_get_perimeter(Stroke* stroke)
{
	_Stroke* s = (_Stroke*)stroke;
	
	return s->perimeter;
}

int stroke_get_id(Stroke* stroke)
{
	_Stroke* s = (_Stroke*)stroke;
	
	return s->id;
}

void stroke_circle_get_center(Stroke* s, double* cx, double* cy)
{
	assert(stroke_get_shape_type(s) == SHAPE_TYPE_CIRCLE);

	stroke_get_point(s, 0, cx, cy);
}

void stroke_set_shape_type(Stroke* stroke, ShapeType type)
{
	_Stroke* s = (_Stroke*)stroke;

	s->type = type;
}

double stroke_circle_get_radius(Stroke* s)
{
	assert(stroke_get_shape_type(s) == SHAPE_TYPE_CIRCLE);
	
	double cx, cy, cxr, cxy;
	stroke_circle_get_center(s, &cx, &cy);
	stroke_get_point(s, 1, &cxr, &cxy);
	
	return fabs(cxr - cx);
}

void stroke_set_body_coordinates(Stroke* stroke, double* body_x, double* body_y)
{
	_Stroke* s = (_Stroke*)stroke;
	
	s->body_x = body_x;
	s->body_y = body_y;
}

void stroke_set_position(Stroke* stroke, double x, double y)
{
	_Stroke* s = (_Stroke*)stroke;
	
	s->x = x;
	s->y = y;
}

void stroke_set_rotation(Stroke* stroke, double angle)
{
	_Stroke* s = (_Stroke*)stroke;
	
	s->theta = angle;
}

ShapeType stroke_get_shape_type(Stroke* stroke)
{
	_Stroke* s = (_Stroke*)stroke;
	return s->type;
}

void stroke_set_body_rotation(Stroke* stroke, double* theta)
{
	_Stroke* s = (_Stroke*)stroke;
	
	s->body_theta = theta;
}

void stroke_destroy(Stroke* stroke)
{
	_Stroke* s = (_Stroke*)stroke;
	s->is_dead = TRUE;
	g_array_free(s->points, FALSE);
	free(s);
}

int stroke_is_dead(Stroke* stroke)
{
	_Stroke* s = (_Stroke*)stroke;
	return s->is_dead;
}

double stroke_append_point(Stroke* stroke, double x, double y)
{
	_Stroke* s = (_Stroke*)stroke;
	
	Point previous;
	Point p = {x - s->x, y - s->y};
	if (s->points->len != 0)
	{
		previous = g_array_index(s->points, Point, s->points->len - 1);	
		s->perimeter += sqrt(get_distance_squared(previous.x, previous.y, p.x, p.y));
	}
	
	g_array_append_val((s->points), p);	
	
	return s->perimeter;
}

int stroke_is_closed(Stroke* stroke)
{
	double x0, y0, xf, yf;
	stroke_get_point(stroke, 0, &x0, &y0);
	stroke_get_point(stroke, stroke_get_length(stroke) - 1, &xf, &yf);
	
	return sqrt(get_distance_squared(x0, y0, xf, yf)) < MAX_CLOSE_DISTANCE;
}

int stroke_get_remaining_ink(Stroke* stroke)
{
	int ink = (int)(((MAX_STROKE_PERIMETER - stroke_get_perimeter(stroke)) / MAX_STROKE_PERIMETER) * 255);
	return (ink < 0) ? 0 : ink;
}

void stroke_get_point(Stroke* stroke, int index, double* x, double* y)
{
	_Stroke* s = (_Stroke*)stroke;
	Point p = g_array_index(s->points, Point, index);
	*x = p.x + s->x_adjust;
	*y = p.y + s->y_adjust;
}

int stroke_get_length(Stroke* stroke)
{
	_Stroke* s = (_Stroke*)stroke;
	assert(s->points);
	return s->points->len;
}

gboolean stroke_get_fill(Stroke* stroke)
{
	_Stroke* s = (_Stroke*)stroke;
	return s->fill;
}

void stroke_get_color(Stroke* stroke, double* red, double* green, double* blue)
{
	_Stroke* s = (_Stroke*)stroke;
	*red = s->red;
	*blue = s->blue;
	*green = s->green;
}
void stroke_set_color(Stroke* stroke, double red, double green, double blue)
{
	_Stroke* s = (_Stroke*)stroke;
	s->red = red;
	s->green = green;
	s->blue = blue;
}

int stroke_get_position(Stroke* stroke, double* x, double* y)
{
	_Stroke* s = (_Stroke*)stroke;
	*x = *(s->body_x);
	*y = *(s->body_y);
	
	int has_changed = (*y != s->previous_x) && (*y != s->previous_y);
	s->previous_x = *x;
	s->previous_y = *y;
	
	return has_changed;
}

int stroke_get_rotation(Stroke* stroke, double* theta)
{
	_Stroke* s = (_Stroke*)stroke;
	*theta = *(s->body_theta);
	
	int has_changed = *theta != s->previous_theta;
	s->previous_theta = *theta;
	
	return has_changed;
}

void stroke_adjust_coordinate_frame(Stroke* stroke, double x_adjust, double y_adjust)
{
	_Stroke* s = (_Stroke*)stroke;
	s->x_adjust = x_adjust;
	s->y_adjust = y_adjust;
}


double stroke_compare_to_segment(Stroke* s, int start_index, int end_index, int* farthest_index)
{		
	double d_max = 0;
	int index_max;
	
	double rms = 0;
	for (int i = start_index; i < end_index; i++)
	{
		double d = stroke_distance_from_index_to_line_connecting_two_indices(s, i, start_index, end_index);
		
		if (d > d_max)
		{
			index_max = i;
			d_max = d;
		}
		
		rms += d * d;
	}
	
	*farthest_index = index_max;
	
	rms /= abs(start_index - end_index);
	return sqrt(rms);
}


int* merge_arrays(int* array_1, int array_1_length, int* array_2, int array_2_length)
{
	int* merged = (int*)malloc((array_1_length + array_2_length) * sizeof(int));
	
	int index_merged = 0;
	int index_1 = 0;
	int index_2 = 0;
	
	while (index_1 < array_1_length && index_2 < array_2_length)
	{
		int item_1 = array_1[index_1];
		int item_2 = array_2[index_2];
		
		if (item_1 <= item_2)
		{
			merged[index_merged++] = item_1;
			index_1++;
		}
		else
		{
			merged[index_merged++] = item_2;
			index_2++;
		}
	}
	while (index_1 < array_1_length)
	{
		merged[index_merged++] = array_1[index_1++];
	}
	while (index_2 < array_2_length)
	{
		merged[index_merged++] = array_2[index_2++];
	}
		
	free(array_1);
	free(array_2);
		
	return merged;
}


int* stroke_analyze_shape_in_range(Stroke* stroke, int start_index, int end_index, int* num_corners_found)
{	
	int corner;
	double corner_distance = stroke_compare_to_segment(stroke, start_index, end_index, &corner);
		
	if (corner_distance > RMS_MIN) // Possibly only check distance to single point, not rms
	{
		int num_corners_found_1;
		int* corners_1 = stroke_analyze_shape_in_range(stroke, start_index, corner, &num_corners_found_1);
		
		int num_corners_found_2;
		int* corners_2 = stroke_analyze_shape_in_range(stroke, corner, end_index, &num_corners_found_2);
		
		*num_corners_found = num_corners_found_1 + num_corners_found_2;
		return merge_arrays(corners_1, num_corners_found_1, corners_2, num_corners_found_2);
	}
	else
	{
		*num_corners_found = 1;
		int* c = (int*)malloc(sizeof(int));
		c[0] = start_index;
		return c;
	}
}

void stroke_range_bounding_box(Stroke* s, int start_index, int end_index, double* min_x, double* min_y, double* max_x, double* max_y)
{		
	stroke_get_point(s, start_index, max_x, max_y);
	stroke_get_point(s, start_index, min_x, min_y);
	
	int num_points = stroke_get_length(s);
	
	for (int i = start_index; TRUE; i++)
	{
		if (i == end_index)
		{
			break;
		}
		else
		{
			i %= num_points;
		}
		
		double x;
		double y;
		
		stroke_get_point(s, i, &x, &y);
		
		if (x < *min_x)
		{
			*min_x = x;
		}
		if (x > *max_x)
		{
			*max_x = x;
		}
		if (y < *min_y)
		{
			*min_y = y;
		}
		if (y > *max_y)
		{
			*max_y = y;
		}
	}
}

int stroke_analyze_shape_for_circle(Stroke* stroke, double* center_x, double* center_y, double* radius)
{
	double min_x, min_y, max_x, max_y;
	
	stroke_range_bounding_box(stroke, 0, stroke_get_length(stroke), &min_x, &min_y, &max_x, &max_y);
		
	double cx = (min_x + max_x) / 2;
	double cy = (min_y + max_y) / 2;
	double ideal_r = ((max_x - min_x) + (max_y - min_y)) / 4;
	
	double rms = 0;
	
	for (int i = 0; i < stroke_get_length(stroke); i++)
	{
		double x, y;
		stroke_get_point(stroke, i, &x, &y);
		double real_r = sqrt(get_distance_squared(x, y, cx, cy));
		
		rms += (real_r - ideal_r) * (real_r - ideal_r);
	}
	
	rms = sqrt(rms) / ideal_r;
	
	*center_x = cx;
	*center_y = cy;
	*radius = ideal_r;
	
	return rms < CIRCLE_RMS;
}

Stroke* stroke_analyze_shape(Stroke* stroke)
{	
	//stroke_find_convex_corners_in_range(stroke, 0, stroke_get_length(stroke), NUMBER_OF_TEST_POINTS, &num_corners);
	
	int id = ((_Stroke*) stroke)->id;
	Stroke* return_stroke = stroke_new(id, 0, 0);
	
	_Stroke* a = (_Stroke*)return_stroke;
	_Stroke* b = (_Stroke*)stroke;
	
	double center_x, center_y, radius;
	if (stroke_analyze_shape_for_circle(stroke, &center_x, &center_y, &radius))
	{
		printf("Building circle with radius: %f\n", radius);
		
		stroke_append_point(return_stroke, center_x, center_y);
		stroke_append_point(return_stroke, center_x + radius, center_y + radius);
		
		a->type = SHAPE_TYPE_CIRCLE;
	}
	else
	{
		int num_corners;
		int* corners = stroke_analyze_shape_in_range(stroke, 0, stroke_get_length(stroke), &num_corners);
		
		printf("Building %d-cornered polygon...\n", num_corners);
		printf("\t{ ");
		for (int i = 0; i < num_corners; i++)
		{
			int corner = corners[i];
			printf("%d ", corner);
			double x;
			double y;
			stroke_get_point(stroke, corner, &x, &y);
			stroke_append_point(return_stroke, x, y);
		}
		printf("}\n");
		
		a->type = SHAPE_TYPE_POLYGON;
	}
						
	a->x = b->x;
	a->y = b->y;
	a->fill = TRUE;//b->fill;
	a->stroke_width = b->stroke_width;
	a->red = b->red;
	a->green = b->green;
	a->blue = b->blue;	
				
	return return_stroke;
}

//************************* Private Function Implementations *************************

_Stroke* stroke_alloc()
{
	_Stroke* s = (_Stroke*)malloc(sizeof(_Stroke));
	assert(s);
	return s;
}

_Stroke* stroke_init_full(_Stroke* stroke, int id, double x, double y, ShapeType type, int fill, double width, double r, double g, double b)
{
	stroke_init(stroke, id, x, y);
	stroke->type = type;
	stroke->fill = fill;
	stroke->stroke_width = width;
	stroke->red = r;
	stroke->green = g;
	stroke->blue = b;
	
	return stroke;
}

_Stroke* stroke_init(_Stroke* stroke, int id, double initial_x, double initial_y)
{
	stroke->id = id;
	stroke->type = SHAPE_TYPE_UNKNOWN;
	stroke->points = g_array_new(FALSE, FALSE, 2 * sizeof(double));
	stroke->x = initial_x;
	stroke->y = initial_y;
	stroke->previous_x = -1;
	stroke->previous_y = -1;
	stroke->body_x = &(stroke->x);
	stroke->body_y = &(stroke->y);
	stroke->theta = 0;
	stroke->body_theta = &(stroke->theta);
	stroke->fill = FALSE;
	stroke->stroke_width = 5;
	
	stroke->perimeter = 0;
	
	stroke->x_adjust = 0;
	stroke->y_adjust = 0;
	
	stroke->red = 0;
	stroke->blue = 0;
	stroke->green = 1;
	
	stroke->is_dead = FALSE;
	
	return stroke;
}

static double get_distance_squared(double x1, double y1, double x2, double y2)
{
	return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

static double stroke_distance_from_index_to_line_connecting_two_indices(Stroke* stroke, int point_index, int line_index_a, int line_index_b)
{
	double ax;
	double ay;
	stroke_get_point(stroke, line_index_a, &ax, &ay);
	ax++;
	ay++;	
	
	double bx;
	double by;
	stroke_get_point(stroke, line_index_b, &bx, &by);
	bx++;
	by++;
	
	double px;
	double py;
	stroke_get_point(stroke, point_index, &px, &py);
	px++;
	py++;
		
	double dx = bx - ax;
	double dy = by - ay;
	
	if (dx == 0)
	{
		return fabs(px - ax);
	}
	if (dy == 0)
	{
		return fabs(py - ay);
	}
		
	double m = dy/dx;
	
	double a = -m;
	double b = 1;
	double c = m * ax - ay;
	
	return fabs(a * px + b * py + c) / sqrt(a * a + b * b);
}


