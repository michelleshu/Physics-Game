/*
GameView.h

Public Header File

Jacob Weiss
*/

#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <gtk/gtk.h>
	
typedef struct GameView GameView;

/*
Make a new game view
*/
GameView* game_view_new();

/*
Get the widget associated with game view
*/
GtkWidget* game_view_get_widget(GameView* view);

void game_view_set_color_pointer(GameView* view, GdkColor* color);

/*
Deallocates a game view, but not the widget.
*/
void game_view_destroy(GameView* view);

#endif