/*
StartupDialogWindow.h

Public Header File for Startup Dialog Functions

Joyce Chung

*/
	
#ifndef STARTUPDIALOGWINDOW_H
#define STARTUPDIALOGWINDOW_H

#include <gtk/gtk.h>
#include "MainWindow.h"

//********************************** Public Function Prototypes **********************************

void startup_dialog_create(MainWindow* main_window);
void startup_dialog_run(GtkToolItem* tool_item, MainWindow* main_window);
void logout_dialog_run(GtkToolItem* tool_item, MainWindow* main_window);

#endif