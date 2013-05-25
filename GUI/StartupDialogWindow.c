#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "../Client/Client.h"
#include "../Client/ClientPacketHandler.h"

#include "MainWindow.h"

#define MAX_LENGTH 40

//********************************** Private Function Prototypes **********************************

static void show_error(GtkWidget *widget);

//********************************** Public Functions  **********************************

/*
This function creates the login dialog pop-up window, and passes back 
the username that the player provides. This username can then be passed
through to a struct in core, such that it can be used for chatting purposes.
*/
void startup_dialog_create(MainWindow* main_window)
{
	GtkWidget* startup_dialog_area;
	GtkWidget* startup_vbox;
	GtkWidget* login_label;
	GtkWidget* server_port_label;
	GtkWidget* server_address_label;
	GtkWidget* login_hbox;
	GtkWidget* server_port_hbox;
	GtkWidget* server_address_hbox;
	
	// Initialize Startup Dialog Items in Main Window Struct
	main_window->startup_dialog = gtk_dialog_new_with_buttons("Startup Dialog", GTK_WINDOW(main_window->window), 
														GTK_DIALOG_MODAL,
														GTK_STOCK_OK, 
														GTK_RESPONSE_OK, 
														GTK_STOCK_CANCEL, 
														GTK_RESPONSE_CANCEL, 
														NULL);
	main_window->login_username_entry = gtk_entry_new();
	main_window->server_port_entry = gtk_entry_new();
	main_window->server_address_entry = gtk_entry_new();
	
	// Initialize Other Startup Dialog Items
	startup_dialog_area = gtk_dialog_get_content_area(GTK_DIALOG(main_window->startup_dialog));
	startup_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	
	login_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	server_port_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	server_address_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	
	login_label = gtk_label_new("Create Username: ");
	server_port_label = gtk_label_new("Server Port: ");
	server_address_label = gtk_label_new("Server Address: ");
	
	// Dialog item settings
	gtk_entry_set_max_length(GTK_ENTRY(main_window->login_username_entry), MAX_LENGTH);
	
	// Pack Startup Dialog Items
	gtk_box_pack_start(GTK_BOX(login_hbox), login_label, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(login_hbox), main_window->login_username_entry, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(server_port_hbox), server_port_label, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(server_port_hbox), main_window->server_port_entry, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(server_address_hbox), server_address_label, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(server_address_hbox), main_window->server_address_entry, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(startup_vbox), login_hbox, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(startup_vbox), server_port_hbox, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(startup_vbox), server_address_hbox, FALSE, FALSE, 5);
	gtk_container_add(GTK_CONTAINER(startup_dialog_area), startup_vbox);

}

void startup_dialog_run(GtkToolItem* tool_item, MainWindow* main_window)
{
	int username_text_length;
	char username_text[MAX_LENGTH];
	const char* server_port_text;
	const char* server_address_text;
	int port;
	char ip_address[MAX_LENGTH];
	bool status;
	GtkResponseType response;
	
	gtk_widget_show_all((GtkWidget *)main_window->startup_dialog);
	gtk_window_set_position(GTK_WINDOW(main_window->startup_dialog),GTK_WIN_POS_CENTER_ALWAYS);	

	// Run dialog window
	response = gtk_dialog_run(GTK_DIALOG(main_window->startup_dialog));

	// check response, here is where the username is obtained
	// and can be used later for the chat window
	if (response == GTK_RESPONSE_OK) {
		
		// get username as long as something is in the username_entry box
		if (gtk_entry_get_text_length(GTK_ENTRY(main_window->login_username_entry)) > 0) {
			strcpy(username_text, gtk_entry_get_text(GTK_ENTRY(main_window->login_username_entry)));

		} else {
			//printf("error\n");
			strcpy(username_text, "anonymous");
			//printf("username is now %s\n", username_text);
		}
		// Return login-name for chat window purposes
		username_text_length = strlen(username_text);
		username_text[username_text_length] = '\0';
		username_text_length++;
		//printf("Username text length is %i and it is: %s!\n", username_text_length, username_text);
		
		server_port_text = gtk_entry_get_text(GTK_ENTRY(main_window->server_port_entry));
		server_address_text = gtk_entry_get_text(GTK_ENTRY(main_window->server_address_entry));
		
		// Test to see if server connection works based on entered port and address
		
		port = atoi(server_port_text);
		strcpy(ip_address, server_address_text);
		status = client_start(port, ip_address);
		
		if (status == TRUE) {
			printf("Established a connection and client created!\n");
			
			// Call username sending function from packet handler now that server connection has been established
			cph_send_username(username_text);
			
		} else {
			show_error(main_window->startup_dialog);
			//printf("Could not connect to server. Please try again.\n");
		}
		
	} else if (response == GTK_RESPONSE_CANCEL) {
		//printf("Cancelling login process and attempt at server connection.\n");
	}
	
	// Hide dialog window once someone has logged in
	gtk_widget_hide((GtkWidget *)main_window->startup_dialog);
}

/*
This function allows a player to logout and re-login with a new username and server
connection. A player cannot commence forward into the program without successfully logging
back in with a username and server connection, once they have logged out.
*/
void logout_dialog_run(GtkToolItem* tool_item, MainWindow* main_window)
{
	startup_dialog_run(NULL, main_window);
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