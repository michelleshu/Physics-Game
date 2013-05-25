#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../Client/ClientPacketHandler.h"
#include "../Client/Client.h"

#include "MainWindow.h"
#include "ChatDialogWindow.h"

typedef struct 
{
	GtkWidget* chat_view;
	GtkTextBuffer* chat_view_buffer;
	GtkTextIter chat_view_iter;
} _Chat;

//********************************** Private Function Prototypes **********************************

// Chat singleton struct allocation, initialization, creation, destruction
Chat* chat_get();
static _Chat* chat_init(_Chat* chat);
static _Chat* chat_alloc();

// Toolbar Item Callback Functions
static void 	chat_send_button_clicked	(GtkWidget *widget, gpointer data);
static void 	chat_exit_button_clicked	(GtkWidget* widget, gpointer data);
static void 	chat_update_buffer_username	(GtkTextBuffer* buffer, GtkTextIter iter, char username[30]);
static void 	chat_update_buffer_message	(GtkTextBuffer* buffer, GtkTextIter iter, char* message);

//********************************** Public Functions **********************************

/*
This function allocates space for and initializes a dialog window that pops up,
is not modal or transient to the main window screen, and thus can be shown throughout 
the game. There will be a scrollable display of the chat log, and a entry box below where
the user can type in their message. Chat texts are then sent to the client via a function call.
*/
void chat_dialog_create(MainWindow* main_window)
{
	GtkWidget* chat_dialog_area;
	GtkWidget* chat_scrolled_window;
	GtkWidget* hbox1;
	GtkWidget* hbox2;
	GtkWidget* hbox3;
	GtkWidget* vbox;
	GtkWidget* hbox1_label;
	GtkWidget* hbox3_label;
	GtkWidget* hbox3_entry;
	GtkWidget* send_button;
	GtkWidget* exit_button;
	GtkWidget* chat_view;
	GtkTextBuffer* buffer;
	GtkTextIter iter;
	PangoFontDescription *font_description;
	
	// Get chat buffer
	_Chat* chat = (_Chat*)chat_get();
	
	// Initialize main_window struct items
	main_window->chat_dialog = gtk_dialog_new();
	
	// Initialize chat dialog items
	chat_dialog_area = gtk_dialog_get_content_area(GTK_DIALOG(main_window->chat_dialog));
	chat_scrolled_window = gtk_scrolled_window_new(NULL, NULL);	
	hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	hbox3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	hbox1_label = gtk_label_new("Message Log:");
	hbox3_label = gtk_label_new("Enter text:");
	hbox3_entry = gtk_entry_new();
	chat_view = (GtkWidget* )chat->chat_view;
	buffer = (GtkWidget* )chat->chat_view_buffer;
	iter = chat->chat_view_iter;
	send_button = gtk_button_new_with_label("Send");
	exit_button = gtk_button_new_with_label("Exit");

	// text view settings
	gtk_text_view_set_editable(GTK_TEXT_VIEW(chat_view), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(chat_view), TRUE);

	// text buffer font settings and creation of tags for tag table
	gtk_text_buffer_create_tag(buffer, "blue_fg", "foreground", "blue", NULL);
	gtk_text_buffer_create_tag(buffer, "green_fg", "foreground", "green", NULL);
	gtk_text_buffer_create_tag(buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
	font_description = pango_font_description_from_string("Serif 10");
	gtk_widget_modify_font(chat_view, font_description);
	pango_font_description_free(font_description);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(chat_view), 5);
	
	// packing boxes
	gtk_box_pack_start(GTK_BOX(hbox1), hbox1_label, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox2), chat_scrolled_window, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox3), hbox3_label, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox3), hbox3_entry, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(hbox3), send_button, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox3, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), exit_button, TRUE, FALSE, 5);
	
	// adding items to containers
	gtk_container_add(GTK_CONTAINER(chat_dialog_area), vbox);
	gtk_container_add(GTK_CONTAINER(chat_scrolled_window), chat_view);
	
	// scrolled window settings
	gtk_widget_set_size_request(chat_dialog_area, 150, 400);
	gtk_widget_set_size_request(chat_scrolled_window, 150, 200);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(chat_scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(chat_scrolled_window), GTK_SHADOW_IN);
	
	// setting for message entry box so that cursor is automatically in the box for typing
	gtk_widget_grab_focus(hbox3_entry);
	
	// callback functions for sending message (with send button and carriage return key)
	g_signal_connect(G_OBJECT(send_button), "clicked", G_CALLBACK(chat_send_button_clicked), (gpointer)hbox3_entry);
	g_signal_connect(G_OBJECT(hbox3_entry), "activate", G_CALLBACK(chat_send_button_clicked), (gpointer)hbox3_entry);

	// callback function for closing chat dialog window
	g_signal_connect(G_OBJECT(exit_button), "clicked", G_CALLBACK(chat_exit_button_clicked), (gpointer)main_window->chat_dialog);
	g_signal_connect(G_OBJECT(main_window->chat_dialog), "delete_event", G_CALLBACK (gtk_widget_hide), NULL);
}

/*
This function runs the chat dialog
*/
void chat_dialog_run(GtkToolItem* tool_item, MainWindow* main_window)
{
	// show widget and run it
	gtk_widget_show_all(main_window->chat_dialog);
	gtk_window_move(GTK_WINDOW(main_window->chat_dialog), 100, 100);	

}

/*
This function can be called by the Packet Handler to update the GUI with username information
*/
void chat_username_send(char* username)
{
	//printf("In chat_username_send\n");
	
	_Chat* chat = (_Chat*)chat_get();
	
	// will change all of the 30 character limits to #define
	char name[30];
	strcpy(name, username);
	chat_update_buffer_username(chat->chat_view_buffer, chat->chat_view_iter, name);
}

/*
This function can be called by the Packet Handler to update the GUI with message information
*/
void chat_message_send(char* message)
{
	//printf("In chat_message_send\n");
	
	_Chat* chat = (_Chat*)chat_get();
	chat_update_buffer_message(chat->chat_view_buffer, chat->chat_view_iter, message);
}

/*
Destroy the chat struct
*/
void chat_destroy()
{
	_Chat* chat = (_Chat*)chat_get(-1);
	{
		free(chat);
	}
}

//********************************** Private Functions **********************************

/*
Using Singleton design pattern to maintain chat properties in a place easily
accessible to other functions.
*/
Chat* chat_get() 
{
	static _Chat* chat = NULL;
	if (chat == NULL)
	{
		chat = chat_init(chat_alloc());
		//printf("Chat created.\n");
	}
	return (Chat*)chat;
}

/*
Allocate space for a new chat struct
*/
static _Chat* chat_alloc()
{
	_Chat* chat = (_Chat*)malloc(sizeof(_Chat));
	assert(chat);
	return chat;
}

/*
Initialize the chat struct
*/
static _Chat* chat_init(_Chat* chat)
{
	chat->chat_view = gtk_text_view_new();
	chat->chat_view_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat->chat_view));
	gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(chat->chat_view_buffer), &(chat->chat_view_iter), 0);
	return chat;
}

/*
This function gets the message that the user wishes to send, and a function should be called so that the client
can get the message, to send to the server. It also clears the message entry box when SEND is hit.
*/
static void chat_send_button_clicked(GtkWidget* widget, gpointer data)
{
    const char *message_text = gtk_entry_get_text(GTK_ENTRY((GtkWidget *)data));
	
	// debugging  message
	//printf("The message to be sent is: %s\n", message_text);
	
	// call to packet handler message sending function
	cph_send_message(message_text);
	
	gtk_entry_set_text(GTK_ENTRY((GtkWidget *)data), "");
}

/*
This function hides the chat dialog when the user clicks cancel
*/
static void chat_exit_button_clicked(GtkWidget* widget, gpointer data)
{
	gtk_widget_hide((GtkWidget *)data);
}

/*
This function updates the chat buffer and should be called every time someone sends a message through the server
*/
static void chat_update_buffer_username(GtkTextBuffer* buffer, GtkTextIter iter, char username[30])
{	
	// add text to buffer!
	gtk_text_buffer_get_end_iter(buffer, &iter);
	gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, username, -1, "bold", "blue_fg",  NULL);
	gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, ": ", -1, "bold", "blue_fg",  NULL);
}

/*
This function updates the chat buffer and should be called every time someone sends a message through the server
*/
static void chat_update_buffer_message(GtkTextBuffer* buffer, GtkTextIter iter, char* message)
{	
	// add text to buffer!
	gtk_text_buffer_get_end_iter(buffer, &iter);
	gtk_text_buffer_insert(buffer, &iter, message, -1);
	gtk_text_buffer_insert(buffer, &iter, "\n", -1);
}
