#ifndef CLIENTWORLD_H
#define CLIENTWORLD_H

#include <glib.h>

typedef struct ClientWorld ClientWorld;

ClientWorld* client_world_get();
ClientWorld* client_world_new();
void client_world_destroy(ClientWorld* world);

GList* client_world_get_strokes();

void client_world_new_stroke(int id, double initial_x, double initial_y, char type, char fill, double width, double r, double g, double b);
void client_world_add_to_stroke(int id, double x, double y);
void client_world_finish_stroke(int id);
void client_world_update_stroke(int id, double x, double y, double theta);
void client_world_delete_stroke(int id);
void client_world_clear();

#endif