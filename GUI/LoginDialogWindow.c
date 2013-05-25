#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../Client/ClientPacketHandler.h"

#include "MainWindow.h"
#include "LoginDialogWindow.h"

#define USERNAME_MAX_LENGTH 30

//********************************** Public Functions  **********************************

/*
This function creates the login dialog pop-up window, and passes back 
the username that the player provides. This username can then be passed
through to a struct in core, such that it can be used for chatting purposes.
*/
void login_dialog_create(MainWindow* main_window)
{
	GtkWidget* login_dialog_area;
	GtkWidget* username_label;
	GtkWidget* username_hbox;
	GtkWidget* vbox;
	
	// Initialize Login Dialog Items in Main Window Struct
	main_window->login_dialog = gtk_dialog_new_with_buttons("Login", GTK_WINDOW(main_window), 
														GTK_DIALOG_MODAL,
														GTK_STOCK_OK, 
														GTK_RESPONSE_OK, 
														GTK_STOCK_CANCEL, 
														GTK_RESPONSE_CANCEL, 
														NULL);
	main_window->login_username_entry = gtk_entry_new();
	
	// Initialize Other Login Dialog Items
	login_dialog_area = gtk_dialog_get_content_area(GTK_DIALOG(main_window->login_dialog));
	username_label = gtk_label_new("Create Username: ");
	username_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	
	// Dialog item settings
	gtk_entry_set_max_length(GTK_ENTRY(main_window->login_username_entry), USERNAME_MAX_LENGTH);
	
	// Pack Login Dialog Items
	gtk_box_pack_start(GTK_BOX(username_hbox), username_label, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(username_hbox), main_window->login_username_entry, TRUE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), username_hbox, FALSE, FALSE, 5);
	gtk_container_add(GTK_CONTAINER(login_dialog_area), vbox);
}

void login_dialog_run(MainWindow* main_window)
{
	int username_text_length;
	char username_text[USERNAME_MAX_LENGTH];
	GtkResponseType response;
	
	gtk_widget_show_all((GtkWidget *)main_window->login_dialog);

	// Run dialog window
	response = gtk_dialog_run(GTK_DIALOG(main_window->login_dialog));

	// check response, here is where the username is obtained
	// and can be used later for the chat window
	if (response == GTK_RESPONSE_OK) {
		
		// get username as long as something is in the username_entry box
		if (gtk_entry_get_text_length(GTK_ENTRY(main_window->login_username_entry)) > 0) {
			strcpy(username_text, gtk_entry_get_text(GTK_ENTRY(main_window->login_username_entry)));
			//printf("Hi %s! We've successfully created your username.\n", username_text);
		} else {
			printf("error\n");
			strcpy(username_text, "anonymous");
			printf("username is now %s\n", username_text);
		}
		// Return login-name for chat window purposes
		username_text_length = strlen(username_text);
		username_text[username_text_length] = '\0';
		username_text_length++;
		//printf("Username text length is %i and it is: %s!\n", username_text_length, username_text);
		
		// Call username sending function from packet handler
		cph_send_username(username_text);
		
	} else if (response == GTK_RESPONSE_CANCEL) {
		
		// This part is not working right now
		printf("Cancelling Login Process\n");
	}
	
	// Hide dialog window once someone has logged in
	gtk_widget_hide((GtkWidget *)main_window->login_dialog);
}

/*
This function allows a player to logout and re-login with a new username.
A player cannot commence forward into the program without successfully logging
back in, once they have logged out.
*/
void logout_dialog_run(MainWindow* main_window)
{
	//printf("hello");
	login_dialog_run(main_window);
}