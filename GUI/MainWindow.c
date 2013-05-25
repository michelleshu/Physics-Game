/*
MainWindow.c

Implementation File for Main Window (TOP LEVEL)

Joyce Chung

*/

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../World/World.h"
#include "../Client/ClientPacketHandler.h"
#include "../Client/Client.h"

#include "MainWindow.h"
#include "AboutDialogWindow.h"
#include "ChatDialogWindow.h"
#include "ColorDialogWindow.h"
#include "LevelDialogWindow.h"
#include "StartupDialogWindow.h"
#include "WinDialogWindow.h"

#define WIDTH 600
#define HEIGHT 600

//*********************** Private Function Prototypes ***********************************

// Main Window Allocation, Initialization, Creation
static MainWindow* main_view_alloc();
static MainWindow* main_view_init(MainWindow* main_window, GameView* game_view);
static void main_window_create_view(MainWindow* main_window, GameView* game_view);

// Time handler to draw main_window and its sub-components
static gboolean redraw_time_handler(MainWindow* main_window);

//***************************** Public Functions ****************************************

// Initialize and Allocate space for main window, and add game_view to main_window
MainWindow* main_window_new()
{
	GameView* game_view = game_view_new();

	// All dialogs under main_window will be created and initialized here, but not shown until called using "run"
	MainWindow* main_window = main_view_init(main_view_alloc(), game_view);	
	
	game_view_set_color_pointer(game_view, &(main_window->color));
	
	// Startup Dialog Must Start here, so that information is sent linearly in order to setup the game before loading things.
	startup_dialog_run(NULL, main_window);
	
	// Level Dialog must also start here (just one time), so that a level is loaded before anything begins.
	level_dialog_run(NULL, main_window);

	return main_window;	
}

// GTK main function is looped, then main_window is destroyed
void main_window_begin(MainWindow* main_window)
{
	// Start the run loop
	gtk_main();
	
	// destroy main_window game_view
	game_view_destroy(main_window->game_view);
	
	// destroy chat- the singleton designed struct
	chat_destroy();
	
	// destroy main_window
	free(main_window);
}

// Gets the game_view for main_window
GameView* main_window_get_game_view(MainWindow* main_window)
{
	return main_window->game_view;
}

//********************************** Private Functions ****************************************

static MainWindow* main_view_alloc()
{
	MainWindow* main_window = (MainWindow*)malloc(sizeof(MainWindow));
	assert(main_window);	
	return main_window;
}

static MainWindow* main_view_init(MainWindow* main_window, GameView* game_view)
{	
	// Initialization functions for MainWindow struct's components
	main_window_create_view(main_window, game_view);

	// Create dialog windows that are children of main_window's window
	about_dialog_create(main_window);
	chat_dialog_create(main_window);
	color_dialog_create(main_window);
	level_dialog_create(main_window);
	startup_dialog_create(main_window);
	win_dialog_create(main_window);

	// Time Handler for Main Window
	g_timeout_add(18, (GSourceFunc)redraw_time_handler, (gpointer)main_window);
		
	main_window->color.red = rand() % 255;
	main_window->color.green = rand() % 255;
	main_window->color.blue = rand() % 255;

	// Show the Main Window
	gtk_widget_show_all(main_window->window);	
	
	return main_window;
}

static gboolean redraw_time_handler(MainWindow* main_window)
{
	gtk_widget_queue_draw(main_window->window);
	
	// check with time handler if anyone has won yet
	win_dialog_run(NULL, main_window);
	
	return TRUE;
}

/*
This function creates all of the components that will be in the main, top-level window.
*/
static void main_window_create_view(MainWindow* main_window, GameView* game_view)
{	
	main_window->game_view = game_view;
	
	//************** Window **************//
	main_window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	// Top Level Window Settings
	gtk_window_set_title(GTK_WINDOW(main_window->window), "Screamin* Green");
	gtk_window_set_default_size(GTK_WINDOW(main_window->window), WIDTH, HEIGHT);
	
	// Call gtk_main_quit when window emits destroy signal
	g_signal_connect(G_OBJECT(main_window->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	
	//************** Background - CSS **************//

	GtkCssProvider *provider;
	GdkDisplay *display;
	GdkScreen *screen;
  	
	provider = gtk_css_provider_new ();
	display = gdk_display_get_default ();
	screen = gdk_display_get_default_screen (display);

	gtk_style_context_add_provider_for_screen (screen,
	                                             GTK_STYLE_PROVIDER(provider),
	                                             GTK_STYLE_PROVIDER_PRIORITY_USER);

	gtk_css_provider_load_from_data (GTK_CSS_PROVIDER (provider),
	                                     " GtkWindow {\n"                         
	                                     "   background-image: url('icons/paper.png');\n"   
	                                     "}\n", -1, NULL);
	g_object_unref (provider);

	//************** Grid **************//
	GtkWidget* grid;
	
	grid = gtk_grid_new();
	
	// Grid Settings
	gtk_container_add(GTK_CONTAINER(main_window->window), grid);

	//************** Frame and Drawing Area **************//
	GtkWidget* frame;
	GtkWidget* drawing_area;
	
	frame = gtk_frame_new(NULL);
	drawing_area = game_view_get_widget(game_view);
	
	// Frame Settings
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
	gtk_widget_set_hexpand(frame, TRUE);
	gtk_widget_set_vexpand(frame, TRUE);
	
	gtk_grid_attach(GTK_GRID(grid), frame, 0, 1, 5, 5);
	gtk_container_add(GTK_CONTAINER(frame), drawing_area);
	
	//************** Toolbar **************//
	GtkWidget* toolbar;
	GtkToolItem* tool_item_load;
	GtkToolItem* tool_item_startup;
	GtkToolItem* tool_item_chat;
	GtkToolItem* tool_item_color;
	GtkToolItem* tool_item_logout;
	GtkToolItem* tool_item_quit;
	
	// Create toolbar icons
	GtkToolItem* stock = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
	GtkIconSize stock_size = gtk_tool_item_get_icon_size(stock);
	gint width = 0;
	gint height = 0;
	gtk_icon_size_lookup(stock_size, &width, &height);
	
	// Read-in icon images
	GdkPixbuf* load_pixbuf = gdk_pixbuf_new_from_file_at_size("icons/star_add.png", width, height, NULL);
	GtkWidget* load_image = gtk_image_new_from_pixbuf(load_pixbuf);
	GdkPixbuf* server_pixbuf = gdk_pixbuf_new_from_file_at_size("icons/comment_settings.png", width, height, NULL);
	GtkWidget* server_image = gtk_image_new_from_pixbuf(server_pixbuf);
	GdkPixbuf* chat_pixbuf = gdk_pixbuf_new_from_file_at_size("icons/comment.png", width, height, NULL);
	GtkWidget* chat_image = gtk_image_new_from_pixbuf(chat_pixbuf);
	GdkPixbuf* color_pixbuf = gdk_pixbuf_new_from_file_at_size("icons/favourite_edit.png", width, height, NULL);
	GtkWidget* color_image = gtk_image_new_from_pixbuf(color_pixbuf);
	GdkPixbuf* logout_pixbuf = gdk_pixbuf_new_from_file_at_size("icons/user_deny.png", width, height, NULL);
	GtkWidget* logout_image = gtk_image_new_from_pixbuf(logout_pixbuf);
	GdkPixbuf* quit_pixbuf = gdk_pixbuf_new_from_file_at_size("icons/bullet_deny.png", width, height, NULL);
	GtkWidget* quit_image = gtk_image_new_from_pixbuf(quit_pixbuf);
	
	// Initialize toolbar items and their icons
	toolbar = gtk_toolbar_new();
	tool_item_load = gtk_tool_button_new(NULL, "load");
	tool_item_startup = gtk_tool_button_new(NULL, "setup server and login");
	tool_item_chat = gtk_tool_button_new(NULL, "chat");
	tool_item_color = gtk_tool_button_new(NULL, "crayon color");
	tool_item_logout = gtk_tool_button_new(NULL, "logout");
	tool_item_quit = gtk_tool_button_new(NULL, "exit");
	
	// Set icon images to respective toolbar buttons
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(tool_item_load), load_image);
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(tool_item_startup), server_image);
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(tool_item_chat), chat_image);
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(tool_item_color), color_image);
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(tool_item_logout), logout_image);
	gtk_tool_button_set_icon_widget(GTK_TOOL_BUTTON(tool_item_quit), quit_image);
	
	// Set toolbar settings
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH);
	gtk_container_set_border_width(GTK_CONTAINER(toolbar), 0);
	gtk_widget_set_hexpand(toolbar, TRUE);

	// Insert toolbar items
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_item_load, -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_item_startup, -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_item_chat, -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_item_color, -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_item_logout, -1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_item_quit, -1);

	gtk_grid_attach(GTK_GRID(grid), toolbar, 0, 0, 1, 1);
	
	// signal callback functions
	g_signal_connect(G_OBJECT(tool_item_load), 		"clicked", G_CALLBACK(level_dialog_run),	(gpointer)main_window);
	g_signal_connect(G_OBJECT(tool_item_startup), 	"clicked", G_CALLBACK(startup_dialog_run), 	(gpointer)main_window);
	g_signal_connect(G_OBJECT(tool_item_chat), 		"clicked", G_CALLBACK(chat_dialog_run), 	(gpointer)main_window);
	g_signal_connect(G_OBJECT(tool_item_color), 	"clicked", G_CALLBACK(color_dialog_run),	(gpointer)main_window);
	g_signal_connect(G_OBJECT(tool_item_logout), 	"clicked", G_CALLBACK(logout_dialog_run), 	(gpointer)main_window);
	g_signal_connect(G_OBJECT(tool_item_quit), 		"clicked", G_CALLBACK(gtk_main_quit), 		NULL);
	
}
