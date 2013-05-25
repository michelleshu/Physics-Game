/*
LoginDialogWindow.h

Public Header File for Login Dialog Functions

Joyce Chung

*/
	
#ifndef LOGINDIALOGWINDOW_H
#define LOGINDIALOGWINDOW_H

#include "MainWindow.h"

//********************************** Public Function Prototypes **********************************

void login_dialog_create(MainWindow* main_window);
void login_dialog_run(MainWindow* main_window);
void logout_dialog_run(GtkToolItem* tool_item, MainWindow* main_window);

#endif