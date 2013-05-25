/*
LevelDialogWindow.h

Public Header File for Level Dialog Functions

Joyce Chung

*/
	
#ifndef LEVELDIALOGWINDOW_H
#define LEVELDIALOGWINDOW_H

#include <gtk/gtk.h>
#include "MainWindow.h"

//********************************** Public Function Prototypes **********************************

void level_dialog_create(MainWindow* main_window);
void level_dialog_run(GtkToolItem* tool_item, MainWindow* main_window);

#endif