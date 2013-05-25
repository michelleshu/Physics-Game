/*
ChatDialogWindow.h

Public Header File for Chat Dialog Functions

Joyce Chung

*/
	
#ifndef CHATDIALOGWINDOW_H
#define CHATDIALOGWINDOW_H

#include <gtk/gtk.h>
#include "MainWindow.h"

typedef struct Chat Chat;

//********************************** Public Function Prototypes **********************************

void chat_dialog_create(MainWindow* main_window);
void chat_dialog_run(GtkToolItem* tool_item, MainWindow* main_window);
void chat_username_send(char* username);
void chat_message_send(char* message);
void chat_destroy();

#endif