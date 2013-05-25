#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../Client/Client.h"
#include "../Client/ClientPacketHandler.h"

#include "MainWindow.h"
#include "LevelDialogWindow.h"

#define WIDTH 100
#define HEIGHT 100
#define WINDOWWIDTH 600
#define WINDOWHEIGHT 400

//********************************** Private Function Prototypes **********************************

// Level Dialog Callback Functions
static void 	level_load_button1_click			(GtkWidget* widget, MainWindow* main_window);
static void 	level_load_button2_click			(GtkWidget* widget, MainWindow* main_window);
static void 	level_load_button3_click			(GtkWidget* widget, MainWindow* main_window);
static void 	level_load_button4_click			(GtkWidget* widget, MainWindow* main_window);
static void 	level_load_cancel_button_click	(GtkWidget* widget, MainWindow* main_window);

//********************************** Public Functions  **********************************

/*
This function allows a player to choose a level to load and play.
Currently, we are displaying the levels in a dialog box, with buttons
as selectors. We will have to split out "New Level" and "Load Level"
to distinguish between starting a new level and loading up a saved level.
For now, there will only be new levels.
*/
void level_dialog_create(MainWindow* main_window)
{
	GtkWidget* level_dialog_area;
	GtkWidget* vbox;
	GtkWidget* hbox;
	GtkWidget* button_1;
	GtkWidget* button_2;
	GtkWidget* button_3;
	GtkWidget* button_4;
	GtkWidget* button_cancel;
	
	// Initialize Level Dialog Items in Main Window Struct
	main_window->level_dialog = gtk_dialog_new();
	
	// Initialize Other Level Dialog Items
	level_dialog_area = gtk_dialog_get_content_area(GTK_DIALOG(main_window->level_dialog));
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	button_1 = gtk_button_new();
	button_2 = gtk_button_new();
	button_3 = gtk_button_new();
	button_4 = gtk_button_new();
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

	gtk_button_set_relief(GTK_BUTTON(button_cancel), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(button_cancel), 0);
	
	// Load Level Window Settings
	gtk_window_set_modal(GTK_WINDOW(main_window->level_dialog), TRUE);
	gtk_widget_set_size_request(level_dialog_area, WINDOWWIDTH, WINDOWHEIGHT);
	
	// Load Level Button Images
	GdkPixbuf* pixbuf_label = gdk_pixbuf_new_from_file_at_size("icons/select_a_level_label.png", 500, 300, NULL);
	GtkWidget* image_label = gtk_image_new_from_pixbuf(pixbuf_label);
	
	GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file_at_size("icons/star_add.png", 50, 50, NULL);
	GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf);
	
	GdkPixbuf* pixbuf_1 = gdk_pixbuf_new_from_file_at_size("icons/yellow_stars/yellow_star1.png", WIDTH, HEIGHT, NULL);
	GtkWidget* image_1 = gtk_image_new_from_pixbuf(pixbuf_1);
	gtk_button_set_image(GTK_BUTTON(button_1), image_1);
	
	GdkPixbuf* pixbuf_2 = gdk_pixbuf_new_from_file_at_size("icons/yellow_stars/yellow_star2.png", WIDTH, HEIGHT, NULL);
	GtkWidget* image_2 = gtk_image_new_from_pixbuf(pixbuf_2);
	gtk_button_set_image(GTK_BUTTON(button_2), image_2);
	
	GdkPixbuf* pixbuf_3 = gdk_pixbuf_new_from_file_at_size("icons/yellow_stars/yellow_star3.png", WIDTH, HEIGHT, NULL);
	GtkWidget* image_3 = gtk_image_new_from_pixbuf(pixbuf_3);
	gtk_button_set_image(GTK_BUTTON(button_3), image_3);
	
	GdkPixbuf* pixbuf_4 = gdk_pixbuf_new_from_file_at_size("icons/yellow_stars/yellow_star4.png", WIDTH, HEIGHT, NULL);
	GtkWidget* image_4 = gtk_image_new_from_pixbuf(pixbuf_4);
	gtk_button_set_image(GTK_BUTTON(button_4), image_4);
	
	GdkPixbuf* pixbuf_cancel = gdk_pixbuf_new_from_file_at_size("icons/bullet_deny.png", 50, 50, NULL);
	GtkWidget* image_cancel = gtk_image_new_from_pixbuf(pixbuf_cancel);
	gtk_button_set_image(GTK_BUTTON(button_cancel), image_cancel);
	
	// Pack buttons to vbox
	gtk_box_pack_start(GTK_BOX(vbox), image_label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), image, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), button_1, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), button_2, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), button_3, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), button_4, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), button_cancel, FALSE, FALSE, 5);
	gtk_container_add(GTK_CONTAINER(level_dialog_area), vbox);
	
	// callback functions for each button
	g_signal_connect(G_OBJECT(button_1), "clicked", G_CALLBACK(level_load_button1_click), (gpointer)main_window);
	g_signal_connect(G_OBJECT(button_2), "clicked", G_CALLBACK(level_load_button2_click), (gpointer)main_window);
	g_signal_connect(G_OBJECT(button_3), "clicked", G_CALLBACK(level_load_button3_click), (gpointer)main_window);
	g_signal_connect(G_OBJECT(button_4), "clicked", G_CALLBACK(level_load_button4_click), (gpointer)main_window);
	g_signal_connect(G_OBJECT(button_cancel), "clicked", G_CALLBACK(level_load_cancel_button_click), (gpointer)main_window);
	g_signal_connect(G_OBJECT(main_window->level_dialog), "delete_event", G_CALLBACK (gtk_widget_hide), NULL);
}

void level_dialog_run(GtkToolItem* tool_item, MainWindow* main_window)
{
	gtk_widget_show_all((GtkWidget *)main_window->level_dialog);
	gtk_window_set_position(GTK_WINDOW(main_window->level_dialog),GTK_WIN_POS_CENTER_ALWAYS);	
}

//********************************** Private Functions  **********************************

/*
This function enables the loading of level 1
This function also hides the level dialog once a level button is selected.
*/
static void level_load_button1_click(GtkWidget* widget, MainWindow* main_window)
{
	int check = 0;

	// Once a level is selected, hide the window
	gtk_widget_hide(main_window->level_dialog);
	
	// sending level number to the client, then through server, then main_window->level_number
	check = cph_send_level_number(1);
	if (check == 1) {
		main_window->level_number = "1";
	} else {
		fprintf(stderr, "level number was not sent through properly to the client, or server\n");
	}
}

/*
This function enables the loading of level 2
This function also hides the level dialog once a level button is selected.
*/
static void level_load_button2_click(GtkWidget* widget, MainWindow* main_window)
{
	int check = 0;

	// Once a level is selected, hide the window
	gtk_widget_hide(main_window->level_dialog);
	
	// sending level number to the client, then through server, then main_window->level_number
	check = cph_send_level_number(2);
	if (check == 1) {
		main_window->level_number = "2";
	} else {
		fprintf(stderr, "level number was not sent through properly to the client, or server\n");
	}
}

/*
This function enables the loading of level 3
This function also hides the level dialog once a level button is selected.
*/
static void level_load_button3_click(GtkWidget* widget, MainWindow* main_window)
{
	int check = 0;

	// Once a level is selected, hide the window
	gtk_widget_hide(main_window->level_dialog);
	
	// sending level number to the client, then through server, then main_window->level_number
	check = cph_send_level_number(3);
	if (check == 1) {
		main_window->level_number = "3";
	} else {
		fprintf(stderr, "level number was not sent through properly to the client, or server\n");
	}
}

/*
This function enables the loading of level 4
This function also hides the level dialog once a level button is selected.
*/
static void level_load_button4_click(GtkWidget* widget, MainWindow* main_window)
{
	int check = 0;

	// Once a level is selected, hide the window
	gtk_widget_hide(main_window->level_dialog);
	
	// sending level number to the client, then through server, then main_window->level_number
	check = cph_send_level_number(4);
	if (check == 1) {
		main_window->level_number = "4";
	} else {
		fprintf(stderr, "level number was not sent through properly to the client, or server\n");
	}
}

/*
This function allows the user to cancel, and leave hte level loading dialog window
*/
static void level_load_cancel_button_click(GtkWidget* widget, MainWindow* main_window)
{
	gtk_widget_hide(main_window->level_dialog);
}