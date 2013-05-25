/*
GameWindow.c

Implementation File

Jacob Weiss
*/

#include <gtk/gtk.h>
#include <stdlib.h>
#include <assert.h>

#include "GameView.h"
#include "../ClientWorld/ClientWorld.h"
#include "../Client/ClientPacketHandler.h"
#include "../Graphics/Graphics.h"
#include "../Stroke/Stroke.h"
	
#define NO_STROKE (-1)

typedef struct
{
	GtkWidget* drawing_area;
	
	int current_stroke_id;
	
	GdkColor* drawing_color;
	//GArray* strokes; This will be a parallel data structure eventually
	
	ClientWorld* world;
} _GameView;

//*********************** Private Function Prototypes ***********************************

// Interaction Callbacks
static gboolean queue_redraw(GtkWidget* widget);
static gboolean draw(GtkWidget* widget, cairo_t* cr, _GameView* view);
static gboolean mouse_up(GtkWidget* widget, GdkEvent* event, gpointer user_data);
static gboolean mouse_down(GtkWidget* widget, GdkEvent* event, gpointer user_data);
static gboolean mouse_move(GtkWidget *widget, GdkEvent *event, gpointer user_data);

static _GameView* game_view_alloc();
static _GameView* game_view_init(_GameView* view);

//***************************** Public Functions ****************************************

GameView* game_view_new()
{
	return (GameView*)game_view_init(game_view_alloc());
}

GtkWidget* game_view_get_widget(GameView* view)
{
	return ((_GameView*)view)->drawing_area;
}

void game_view_set_color_pointer(GameView* view, GdkColor* color)
{
	_GameView* v = (_GameView*)view;
	
	v->drawing_color = color;
}

void game_view_destroy(GameView* view)
{
	_GameView* v = (_GameView*)view;
	client_world_destroy(v->world);
	free(v);
}

//********************************** Private Functions ***********************************************

static _GameView* game_view_alloc()
{
	_GameView* view = (_GameView*)malloc(sizeof(_GameView));
	assert(view);	
	return view;
}

static _GameView* game_view_init(_GameView* view)
{
	view->current_stroke_id = NO_STROKE;
	
	// Create a drawing area
	GtkWidget* drawing_area = gtk_drawing_area_new();
	
	g_signal_connect(drawing_area, "draw", G_CALLBACK(draw), view);
  
	g_signal_connect(drawing_area, "button_release_event", G_CALLBACK(mouse_up), view);
	g_signal_connect(drawing_area, "motion_notify_event", G_CALLBACK(mouse_move), view);
	g_signal_connect(drawing_area, "button_press_event", G_CALLBACK(mouse_down), view);
	
	gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);
	
	g_timeout_add_full(G_PRIORITY_DEFAULT, 20, (GSourceFunc)queue_redraw, drawing_area, NULL);
	
	view->drawing_area = drawing_area;
	
	view->world = client_world_get();
	
	return view;
}

static gboolean queue_redraw(GtkWidget* widget)
{
	gtk_widget_queue_draw(widget);
	return TRUE;
}

static gboolean draw(GtkWidget* widget, cairo_t* cr, _GameView* view)
{
	graphics_draw(widget, cr, client_world_get_strokes(), *(view->drawing_color));

	return FALSE;
}

static gboolean mouse_up(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
	_GameView* view = (_GameView*)user_data;
	
	if (view->current_stroke_id != NO_STROKE)
	{
		gdouble mouse_x = ((GdkEventButton*)event)->x;
		gdouble mouse_y = ((GdkEventButton*)event)->y;
	
		double width = (double)gtk_widget_get_allocated_width(widget);
		double height = (double)gtk_widget_get_allocated_height(widget);
	
		double x = mouse_x / width;
		double y = mouse_y / height;
	
		cph_send_mouse_up(x, y);	
		
		view->current_stroke_id = NO_STROKE;	
	}
	
	return FALSE;
}

static gboolean mouse_down(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{	
	_GameView* view = (_GameView*)user_data;
		
	GdkColor color = *(view->drawing_color);
	
	gdouble mouse_x = ((GdkEventButton*)event)->x;
	gdouble mouse_y = ((GdkEventButton*)event)->y;
	
	double width = (double)gtk_widget_get_allocated_width(widget);
	double height = (double)gtk_widget_get_allocated_height(widget);
		
	double x = mouse_x / width;
	double y = mouse_y / height;
		
	cph_send_mouse_down(x, y, SHAPE_TYPE_SKETCH, 0, 1, color.red / 255.0, color.green / 255.0, color.blue / 255.0);
		
	view->current_stroke_id = 1;
		
	return FALSE;
}

static gboolean mouse_move(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	_GameView* view = (_GameView*)user_data;

	if (view->current_stroke_id != NO_STROKE)
	{
		gdouble mouse_x = ((GdkEventMotion*)event)->x;
		gdouble mouse_y = ((GdkEventMotion*)event)->y;
	
		guint width = gtk_widget_get_allocated_width(widget);
		guint height = gtk_widget_get_allocated_height(widget);
	
		double x = mouse_x / width;
		double y = mouse_y / height;
		
		cph_send_mouse_move(x, y);
				
		//gtk_widget_queue_draw(widget);
	}
	
	return FALSE;
}
