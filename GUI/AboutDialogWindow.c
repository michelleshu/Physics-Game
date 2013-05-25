#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>

#include "MainWindow.h"
#include "AboutDialogWindow.h"

//********************************** Public Functions  **********************************

/*
This function creates a pop-up dialog about window, which contains instructions on how
to play our game, as well as citations for graphics and a little blurb about the project
*/
void about_dialog_create(MainWindow* main_window)
{
	GdkPixbuf *pixbuf;
	
	// Initialize main_window struct about_dialog member
	main_window->about_dialog = gtk_about_dialog_new();
	gtk_window_set_modal(GTK_WINDOW(main_window->about_dialog), TRUE);
	
	// Set logo of about dialog
	pixbuf = gdk_pixbuf_new_from_file_at_size("icons/about_image.png", 300, 200, NULL);
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(main_window->about_dialog), pixbuf);
	g_object_unref(pixbuf), pixbuf = NULL;
	
	// Set attributes of about dialog
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(main_window->about_dialog), "Team 5: Screamin* Green");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(main_window->about_dialog), "(c) Screamin* Green");
	
	// set comments of about dialog to show credits
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(main_window->about_dialog), 
	"CS 50 Winter 2013 Final Project\nAuthors:\nJoyce Chung\nEliza Hanson\nMichelle Shu\nJacob Weiss\n\nCredits:\nHamster graphics: Molang (c)FEELBug.Yoon\nIcon graphics: http://freebiesbooth.com/wp-content/uploads/downloads/2011/06/025_hand_drawn_icons.zip");

	// run about dialog, then hide
	gtk_dialog_run(GTK_DIALOG (main_window->about_dialog));
	gtk_widget_hide(main_window->about_dialog);
}