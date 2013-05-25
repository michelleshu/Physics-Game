#include "./GUI/MainWindow.h"
#include "./Stroke/Stroke.h"
#include "./Core/level.h"
#include "./WorldPhysics/WorldPhysics.h"
#include "./Chipmunk-Mac/chipmunk.h" // LOCAAAALL
#include "./World/World.h"
#include "./GUI/GameView.h"

int main(int argc, char* argv[])
{	
	gtk_init (&argc, &argv);
		
	MainWindow* main_window = main_window_new();
	
	// Declaring Level and cpSpace structures
    Level* l;
    cpSpace* physics_world;
    
	// function that returns a filename depending on the level number selected
	char* file_name = level_file_name(main_window->level_number);
	
	// These functions allocate space and initializes the Level structure
	// and passes the structure to the World for the world to be initialized
    l = level_new(file_name);
    physics_world = world_physics_new(l);
	
	World* world = game_view_get_world(main_window_get_game_view(main_window));
	
	world_physics_add_box(physics_world, .5, .5, .1, .1, 0);
		
	world_load_space(world, physics_world);
		
	main_window_begin(main_window);
    
	// This function frees memory allocated for the Level structure and the sub-structures
	// contained within it
    level_destroy(l);
	
	return EXIT_SUCCESS;

}