#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "../Client/ClientPacketHandler.h"

#include "MainWindow.h"
#include "ServerDialogWindow.h"

//********************************** Private Function Prototypes **********************************

static void show_error(GtkWidget *widget);

//********************************** Public Functions  **********************************

/*
This function creates the server dialog pop-up window, and should be updated
to return data, or update a struct.
*/
void server_dialog_create(MainWindow* main_window)
{
	GtkWidget* server_dialog_area;
	GtkWidget* server_port_label;
	GtkWidget* server_address_label;
	GtkWidget* server_status_label;
	GtkWidget* hbox_port;
	GtkWidget* hbox_address;
	GtkWidget* hbox_status;
	GtkWidget* vbox;
	
	// Initialize Server Dialog items in Main Window Struct
	main_window->server_dialog = gtk_dialog_new_with_buttons("Enter Server Address and Port", 
												GTK_WINDOW(main_window), GTK_DIALOG_MODAL, 
												GTK_STOCK_OK, GTK_RESPONSE_OK, 
												GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
	main_window->server_port_entry = gtk_entry_new();
	main_window->server_address_entry = gtk_entry_new();
								
	// Initialize Other Server Dialog Items
	server_dialog_area = gtk_dialog_get_content_area(GTK_DIALOG(main_window->server_dialog));
	server_port_label = gtk_label_new("Server Port: ");
	server_address_label = gtk_label_new("Server Address: ");
	server_status_label = gtk_label_new("Connection Status: ");
	hbox_port = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	hbox_address = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	hbox_status = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	
	// Pack Server Dialog Items
	gtk_box_pack_start(GTK_BOX(hbox_port), server_port_label, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(hbox_port), main_window->server_port_entry, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(hbox_address), server_address_label, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(hbox_address), main_window->server_address_entry, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(hbox_status), server_status_label, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(vbox), hbox_port, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox_address, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox_status, FALSE, FALSE, 5);
	gtk_container_add(GTK_CONTAINER(server_dialog_area), vbox);

}	

/*
This function runs the server dialog pop-up window
*/
void server_dialog_run(GtkToolItem* tool_item, MainWindow* main_window)
{
	GtkResponseType response;
	const char* server_port_text;
	const char* server_address_text;
	
	gtk_widget_show_all((GtkWidget *)main_window->server_dialog);

	// Run Server dialog window
	response = gtk_dialog_run(GTK_DIALOG(main_window->server_dialog));

	// check response, and this is where the server port and address entry is sent out
	if (response == GTK_RESPONSE_OK) {

		server_port_text = gtk_entry_get_text(GTK_ENTRY(main_window->server_port_entry));
		server_address_text = gtk_entry_get_text(GTK_ENTRY(main_window->server_address_entry));
		
		// TESTING to see if server connection works based on entered port and address
		int port;
		char ip_address[40];
		bool status;
		
		port = atoi(server_port_text);
		strcpy(ip_address, server_address_text);
		status = client_start(port, ip_address);
		
		if (status == TRUE) {
			//printf("Established a connection and client created!\n");
		} else {
			show_error(main_window->server_dialog);
			fprintf(stderr, "Could not connect to server. Please try again.\n");
		}
		
	} else if (response == GTK_RESPONSE_CANCEL) {
		fprintf(stderr, "Cancelled attempt at server connection.\n");
	}
	
	// Hide server dialog window once someone has logged in
	// Do we want the option of disconnecting a server connection and changing to a different port
	// in the middle of the game?
	gtk_widget_hide(main_window->server_dialog);
	
}

//********************************** Private Functions  **********************************

/*
This function creates the popup error message that indicates to the user whether or not
the server connection has been established
*/
static void show_error(GtkWidget *widget)
{
  	// dialog items for popups from server_select_dialog
	GtkWidget* popup_dialog;

  	popup_dialog = gtk_message_dialog_new(GTK_WINDOW(widget),
    	GTK_DIALOG_DESTROY_WITH_PARENT,
    	GTK_MESSAGE_ERROR,
    	GTK_BUTTONS_OK,
    	"Error establishing a connection. Please try again.");

  	gtk_window_set_title(GTK_WINDOW(popup_dialog), "Error Message");
  	gtk_dialog_run(GTK_DIALOG(popup_dialog));
  	gtk_widget_destroy(popup_dialog);
}