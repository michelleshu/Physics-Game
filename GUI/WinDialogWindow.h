/*
WinDialogWindow.h

Public Header File for Win Dialog Functions

Joyce Chung

*/
	
#ifndef WINDIALOGWINDOW_H
#define WINDIALOGWINDOW_H

#include <gtk/gtk.h>
#include "MainWindow.h"


typedef struct Status Status;

//********************************** Public Function Prototypes **********************************
void win_dialog_create(MainWindow* main_window);
void win_dialog_run(GtkToolItem* tool_item, MainWindow* main_window);
void win_get_status();

#endif