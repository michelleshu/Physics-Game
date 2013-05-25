#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../Client/ClientPacketHandler.h"

#include "MainWindow.h"
#include "WinDialogWindow.h"
#include "LevelDialogWindow.h"

#define WIDTH 200
#define HEIGHT 200
#define WINDOWWIDTH 200
#define WINDOWHEIGHT 150

typedef struct 
{
	int status_is_win;
} _Status;

//********************************** Private Function Prototypes **********************************

// Status singleton struct allocation, initialization, creation, destruction
Status* status_get();
static _Status* status_init(_Status* status);
static _Status* status_alloc();

// Level Dialog Callback Functions
static void		button_restart_level_click	(GtkWidget* widget, MainWindow* main_window);
static void		button_new_level_click		(GtkWidget* widget,	MainWindow* main_window);
static void 	button_cancel_click			(GtkWidget* widget, MainWindow* main_window);

//********************************** Public Functions  **********************************
/*
This function creates the popup dialog window that indicates to the players that someone
has won; the dialog also give players the option of restarting a level, or selecting a 
new one.
*/
void win_dialog_create(MainWindow* main_window)
{
	GtkWidget* win_dialog_area;
	GtkWidget* hbox;
	GtkWidget* vbox;
	GtkWidget* button_restart_level;
	GtkWidget* button_new_level;
	GtkWidget* button_cancel;
	
	// Initialize Level Dialog Items in Main Window Struct
	main_window->win_dialog = gtk_dialog_new();
	
	// Initialize Other Level Dialog Items
	win_dialog_area = gtk_dialog_get_content_area(GTK_DIALOG(main_window->win_dialog));
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	button_restart_level = gtk_button_new();
	button_new_level = gtk_button_new();
	button_cancel = gtk_button_new();
	
	// Button Settings
	gtk_button_set_relief(GTK_BUTTON(button_restart_level), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(button_restart_level), 0);

	gtk_button_set_relief(GTK_BUTTON(button_new_level), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(button_new_level), 0);

	gtk_button_set_relief(GTK_BUTTON(button_cancel), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(button_cancel), 0);
	
	// Load Level Window Settings
	gtk_window_set_modal(GTK_WINDOW(main_window->win_dialog), TRUE);
	gtk_widget_set_size_request(win_dialog_area, WINDOWWIDTH, WINDOWHEIGHT);
	
	// Load Level Button Images
	GdkPixbuf* pixbuf_1 = gdk_pixbuf_new_from_file_at_size("icons/you_won_label.png", 300, 300, NULL);
	GtkWidget* image_1 = gtk_image_new_from_pixbuf(pixbuf_1);
	
	GdkPixbuf* pixbuf_2 = gdk_pixbuf_new_from_file_at_size("icons/restart_level.png", WIDTH, HEIGHT, NULL);
	GtkWidget* image_2 = gtk_image_new_from_pixbuf(pixbuf_2);
	gtk_button_set_image(GTK_BUTTON(button_restart_level), image_2);
	
	GdkPixbuf* pixbuf_3 = gdk_pixbuf_new_from_file_at_size("icons/new_level.png", WIDTH, HEIGHT, NULL);
	GtkWidget* image_3 = gtk_image_new_from_pixbuf(pixbuf_3);
	gtk_button_set_image(GTK_BUTTON(button_new_level), image_3);
	
	GdkPixbuf* pixbuf_4 = gdk_pixbuf_new_from_file_at_size("icons/bullet_deny.png", 50, 50, NULL);
	GtkWidget* image_4 = gtk_image_new_from_pixbuf(pixbuf_4);
	gtk_button_set_image(GTK_BUTTON(button_cancel), image_4);

	// Pack buttons to vbox
	gtk_box_pack_start(GTK_BOX(vbox), image_1, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), button_restart_level, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), button_new_level, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), button_cancel, FALSE, FALSE, 5);
	gtk_container_add(GTK_CONTAINER(win_dialog_area), vbox);
	
	// callback functions for each button
	g_signal_connect(G_OBJECT(button_restart_level), "clicked", G_CALLBACK(button_restart_level_click), (gpointer)main_window);
	g_signal_connect(G_OBJECT(button_new_level), "clicked", G_CALLBACK(button_new_level_click), (gpointer)main_window);
	g_signal_connect(G_OBJECT(button_cancel), "clicked", G_CALLBACK(button_cancel_click), (gpointer)main_window);
	g_signal_connect(G_OBJECT(main_window->win_dialog), "delete_event", G_CALLBACK (gtk_widget_hide), NULL);
}

/*
This function runs the win dialog window, and should be called when someone wins
*/
void win_dialog_run(GtkToolItem* tool_item, MainWindow* main_window)
{
	_Status* status = (_Status*)status_get();
	
	if (status->status_is_win == 1) {
		gtk_widget_show_all((GtkWidget *)main_window->win_dialog);
		gtk_window_set_position(GTK_WINDOW(main_window->win_dialog),GTK_WIN_POS_CENTER_ALWAYS);
		status->status_is_win = 0;
	} else {
		//printf("no one won yet...\n");
	}
}

/*
This function can be called by client packet handler to let the GUI know that someone has won
*/
void win_get_status()
{
	_Status* status = (_Status*)status_get();
	status->status_is_win = 1;
}
//********************************** Private Functions  **********************************

/*
Using Singleton design pattern to maintain status properties in a place easily
accessible to other functions.
*/
Status* status_get() 
{
	static _Status* status = NULL;
	if (status == NULL)
	{
		status = status_init(status_alloc());
	}
	return (Status*)status;
}

/*
Allocate space for a new status struct
*/
static _Status* status_alloc()
{
	_Status* status = (_Status*)malloc(sizeof(_Status));
	assert(status);
	return status;
}

/*
Initialize the chat struct
*/
static _Status* status_init(_Status* status)
{
	status->status_is_win = 0;
	return status;
}

/*
Function for restarting level
*/
static void	button_restart_level_click(GtkWidget* widget, MainWindow* main_window)
{
	const char *level_number;
	int num;
	int check = 0;

  	// Get level number from label of button and store in main_window's level_number member
	level_number = main_window->level_number;
	num = atoi(level_number);

	// sending level number to the client --who sends it to the server BEFORE assigning main_window->level_number to level_number. 
	check = cph_send_level_number(num);
	if (check == 1) {
		main_window->level_number = level_number;
	} else {
		fprintf(stderr, "level number was not sent through properly to the client, or server\n");
	}
	
	// Once a level is selected, hide the window
	gtk_widget_hide(main_window->win_dialog);
}

static void	button_new_level_click(GtkWidget* widget,	MainWindow* main_window)
{
	gtk_widget_hide(main_window->win_dialog);
	level_dialog_run(NULL, main_window);
}

static void button_cancel_click(GtkWidget* widget, MainWindow* main_window)
{
	gtk_widget_hide(main_window->win_dialog);
}