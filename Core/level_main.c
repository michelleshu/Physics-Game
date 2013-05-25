// This is top-level main.c file that verifies that .lvl files are loaded properly,
// loads the information from level.lvl into a Level structure, and intializes the world
// by passing the Level structure to it.
// 
// GTK_main is also contained in this main.c file ...
//
//
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "level.h"

int main (int argc, char *argv[])
{
	// Declaring Level and cpSpace structures
    Level* l;
    //cpSpace* world;
    
	//get filename
	const char* level_number = "1";
	char* file_name = level_file_name(level_number);
	
	printf("Filename is: %s\n", file_name);
	// These functions allocate space and initializes the Level structure
	// and passes the structure to the World for the world to be initialized
    l = level_new(file_name);
    //world = world_physics_new(l);
    
	// This function frees memory allocated for the Level structure and the sub-structures
	// contained within it
    level_destroy(l);
	
	return EXIT_SUCCESS;
}