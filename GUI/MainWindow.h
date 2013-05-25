/*
MainView.h

Public Header File

Joyce Chung
Jacob Weiss

The Main View is the Main GTK view window
*/
	
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "GameView.h"

typedef struct mainwindow MainWindow;
struct mainwindow
{
	GtkWidget* window;
	GameView* game_view;
	
	// About Dialog Members
	GtkWidget* about_dialog;
	
	// Chat Dialog Members
	GtkWidget* chat_dialog;
	
	// Color Dialog Members
	GtkWidget* color_dialog;
	GdkColor color;
	
	// Level Dialog Members
	GtkWidget* level_dialog;
	const char* level_number;
	
	// Startup Dialog Members
	GtkWidget* startup_dialog;
	GtkWidget* login_username_entry;
	GtkWidget* server_port_entry;
	GtkWidget* server_address_entry;
	
	// Win Dialog Members
	GtkWidget* win_dialog;
};

//********************************** Public Function Prototypes **********************************

MainWindow* main_window_new();

GameView* main_window_get_game_view(MainWindow* window);

void main_window_begin(MainWindow* window);

#endif