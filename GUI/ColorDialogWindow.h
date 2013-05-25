/*
ColorDialogWindow.h

Public Header File for Color Dialog Functions

Joyce Chung

*/
	
#ifndef COLORDIALOGWINDOW_H
#define COLORDIALOGWINDOW_H

#include <gtk/gtk.h>
#include "MainWindow.h"

//********************************** Public Function Prototypes **********************************

void color_dialog_create(MainWindow* main_window);
void color_dialog_run(GtkToolItem* tool_item, MainWindow* main_window);

#endif