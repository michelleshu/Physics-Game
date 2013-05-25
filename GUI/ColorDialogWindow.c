/*
ColorSelectWindow.c

Implementation File for Color Select Dialog Window

Joyce Chung

*/

#include <gtk/gtk.h>
#include "MainWindow.h"
#include "ColorDialogWindow.h"

#define WIDTH 50
#define HEIGHT 50
#define WINDOWWIDTH 250
#define WINDOWHEIGHT 500

//********************************** Private Function Prototypes **********************************

static void color_button_click1(GtkWidget* widget, MainWindow* main_window);
static void color_button_click2(GtkWidget* widget, MainWindow* main_window);
static void color_button_click3(GtkWidget* widget, MainWindow* main_window);
static void color_button_click4(GtkWidget* widget, MainWindow* main_window);
static void color_button_click5(GtkWidget* widget, MainWindow* main_window);
static void exit_button_click(GtkWidget* widget, gpointer data);

//********************************** Public Functions **********************************

/*
Create Color Select Dialog Window
*/
void color_dialog_create(MainWindow* main_window)
{	
	GtkWidget* color_dialog_area;
	GtkWidget* grid;
	GtkWidget* button_1;
	GtkWidget* button_2;
	GtkWidget* button_3;
	GtkWidget* button_4;
	GtkWidget* button_5;
	GtkWidget* button_cancel;
	
	// Initialize Level Dialog Items in Main Window Struct
	main_window->color_dialog = gtk_dialog_new();
	gtk_window_set_modal(GTK_WINDOW(main_window->color_dialog), FALSE);
	
	// Initialize Other Level Dialog Items
	color_dialog_area = gtk_dialog_get_content_area(GTK_DIALOG(main_window->color_dialog));	
	grid = gtk_grid_new();
	button_1 = gtk_button_new();
	button_2 = gtk_button_new();
	button_3 = gtk_button_new();
	button_4 = gtk_button_new();
	button_5 = gtk_button_new();
	button_cancel = gtk_button_new();

	// Button Settings
	gtk_button_set_relief(GTK_BUTTON(button_1), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(button_1), 0);
	gtk_button_set_relief(GTK_BUTTON(button_2), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(button_2), 0);
	gtk_button_set_relief(GTK_BUTTON(button_3), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(button_3), 0);
	gtk_button_set_relief(GTK_BUTTON(button_4), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(button_4), 0);
	gtk_button_set_relief(GTK_BUTTON(button_5), GTK_RELIEF_NONE);
	gtk_container_set_border_width(GTK_CONTAINER(button_5), 0);
	gtk_button_set_relief(GTK_BUTTON(button_cancel), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(button_cancel), 0);

	// Color Dialog Window Settings
	gtk_widget_set_size_request(color_dialog_area, WINDOWWIDTH, WINDOWHEIGHT);
	gtk_container_add(GTK_CONTAINER(color_dialog_area), grid);
	
	// Color Dialog Button Images
	GdkPixbuf* pixbuf_0 = gdk_pixbuf_new_from_file_at_size("icons/Color_bg.png", 250, 250, NULL);
	GtkWidget* image_0 = gtk_image_new_from_pixbuf(pixbuf_0);

	GdkPixbuf* pixbuf_1 = gdk_pixbuf_new_from_file_at_size("icons/Red.png", WIDTH, HEIGHT, NULL);
	GtkWidget* image_1 = gtk_image_new_from_pixbuf(pixbuf_1);
	gtk_button_set_image(GTK_BUTTON(button_1), image_1);
	
	GdkPixbuf* pixbuf_2 = gdk_pixbuf_new_from_file_at_size("icons/Yellow.png", WIDTH, HEIGHT, NULL);
	GtkWidget* image_2 = gtk_image_new_from_pixbuf(pixbuf_2);
	gtk_button_set_image(GTK_BUTTON(button_2), image_2);
	
	GdkPixbuf* pixbuf_3 = gdk_pixbuf_new_from_file_at_size("icons/Green.png", WIDTH, HEIGHT, NULL);
	GtkWidget* image_3 = gtk_image_new_from_pixbuf(pixbuf_3);
	gtk_button_set_image(GTK_BUTTON(button_3), image_3);
	
	GdkPixbuf* pixbuf_4 = gdk_pixbuf_new_from_file_at_size("icons/Junglegreen.png", WIDTH, HEIGHT, NULL);
	GtkWidget* image_4 = gtk_image_new_from_pixbuf(pixbuf_4);
	gtk_button_set_image(GTK_BUTTON(button_4), image_4);
	
	GdkPixbuf* pixbuf_5 = gdk_pixbuf_new_from_file_at_size("icons/Teal.png", WIDTH, HEIGHT, NULL);
	GtkWidget* image_5 = gtk_image_new_from_pixbuf(pixbuf_5);
	gtk_button_set_image(GTK_BUTTON(button_5), image_5);
	
	GdkPixbuf* pixbuf_cancel = gdk_pixbuf_new_from_file_at_size("icons/bullet_deny.png", WIDTH, HEIGHT, NULL);
	GtkWidget* image_cancel = gtk_image_new_from_pixbuf(pixbuf_cancel);
	gtk_button_set_image(GTK_BUTTON(button_cancel), image_cancel);
	
	// Attach buttons to grid
	gtk_grid_attach(GTK_GRID(grid), image_0, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), button_1, 0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), button_2, 0, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), button_3, 0, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), button_4, 0, 5, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), button_5, 0, 6, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), button_cancel, 0, 7, 1, 1);
	
	// callback functions for each color button
	g_signal_connect(G_OBJECT(button_1), "clicked", G_CALLBACK(color_button_click1), (gpointer)main_window);
	g_signal_connect(G_OBJECT(button_2), "clicked", G_CALLBACK(color_button_click2), (gpointer)main_window);
	g_signal_connect(G_OBJECT(button_3), "clicked", G_CALLBACK(color_button_click3), (gpointer)main_window);
	g_signal_connect(G_OBJECT(button_4), "clicked", G_CALLBACK(color_button_click4), (gpointer)main_window);
	g_signal_connect(G_OBJECT(button_5), "clicked", G_CALLBACK(color_button_click5), (gpointer)main_window);
	g_signal_connect(G_OBJECT(button_cancel), "clicked", G_CALLBACK(exit_button_click), (gpointer)main_window->color_dialog);
	g_signal_connect(G_OBJECT(main_window->color_dialog), "delete_event", G_CALLBACK (gtk_widget_hide), NULL);
}

/*
This function allows a player to change the color of their crayon
at any point in the game. If a game is not loaded yet, changing the 
color will only change the color of the "hello _____" message displayed
on the window.
*/
void color_dialog_run(GtkToolItem* tool_item, MainWindow* main_window)
{
	gtk_widget_show_all((GtkWidget *)main_window->color_dialog);
	gtk_window_move(GTK_WINDOW((GtkWidget *)main_window->color_dialog), 600, 100);		
}

//********************************** Privates Functions **********************************

static void color_button_click1(GtkWidget* widget, MainWindow* main_window)
{
	main_window->color.red = 220;
	main_window->color.green = 0;
	main_window->color.blue = 20;
}

static void color_button_click2(GtkWidget* widget, MainWindow* main_window)
{
	main_window->color.red = 250;
	main_window->color.green = 200;
	main_window->color.blue = 0;
}

static void color_button_click3(GtkWidget* widget, MainWindow* main_window)
{
	main_window->color.red = 150;
	main_window->color.green = 200;
	main_window->color.blue = 25;
}

static void color_button_click4(GtkWidget* widget, MainWindow* main_window)
{
	main_window->color.red = 20;
	main_window->color.green = 150;
	main_window->color.blue = 120;
}

static void color_button_click5(GtkWidget* widget, MainWindow* main_window)
{
	main_window->color.red = 100;
	main_window->color.green = 180;
	main_window->color.blue = 250;
}

/*
This function hides the chat dialog when the user clicks cancel
*/
static void exit_button_click(GtkWidget* widget, gpointer data)
{
	gtk_widget_hide((GtkWidget *)data);
}