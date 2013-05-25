/*
ServerDialogWindow.h

Public Header File for Server Dialog Functions

Joyce Chung

*/
	
#ifndef SERVERDIALOGWINDOW_H
#define SERVERDIALOGWINDOW_H

#include <gtk/gtk.h>
#include "MainWindow.h"

//********************************** Public Function Prototypes **********************************

void server_dialog_create(MainWindow* main_window);
void server_dialog_run(GtkToolItem* tool_item, MainWindow* main_window);

#endif