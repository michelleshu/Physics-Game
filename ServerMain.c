#include <glib.h>

#define __USE_BSD
#define _BSD_SOURCE
//#define D_POSIX_C_SOURCE

#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>

#include "./Core/level.h"
#include "./WorldPhysics/WorldPhysics.h"
#include "./Chipmunk-Mac/chipmunk.h" // LOCAAAALL
#include "./World/World.h"
#include "./Server/Server.h"
#include "./Server/ServerPacketHandler.h"

int main(int argc, char* argv[])
{			    
	// For now, the file level.lvl will be automatically loaded.
	// This will change when we start adding more levels and a level selector
	// to our project next week.
	char* file_name = "./Core/level.lvl";
	
	// These functions allocate space and initializes the Level structure
	// and passes the structure to the World for the world to be initialized
    Level* l = level_new(file_name);
    cpSpace* physics_world = world_physics_new(l);
	level_destroy(l);
	
	//Create a world (this is what holds strokes which are what are actually represented graphically, and load the physical world into it.  All world components are affiliated with a physics body or shape as well.
	World* world = world_new();			
	world_load_space(world, physics_world);
		
	// Start server thread
	server_start();
	
	// Set up signalling so that the server terminates gracefully.
 	sigset_t signal_set;
	sigemptyset(&signal_set);
	sigaddset(&signal_set, SIGINT);
	sigprocmask(SIG_BLOCK, &signal_set, NULL);
	
	// Begin time management loop
	const struct timeval physics_step = {0, 16667}; // represents 1/60 of a second
	struct timeval physics_time;
	gettimeofday(&physics_time, NULL);
	struct timeval current_time;
	struct timeval result;
	int did_step;
	while (1)
	{
		while (1)
		{
			//Keep track of the difference in timesteps between the actually time and the location of the physics world.  If real time has gotten too far ahead, take two physics steps in order to make up for it and catch up to real time.
			gettimeofday(&current_time, NULL);
			timersub(&current_time, &physics_time, &result);
			if (timercmp(&result, &physics_step, <))
			{
				break;
			}
			else
			{
				world_physics_step(world_get_space(world));
				timeradd(&physics_time, &physics_step, &result);
				physics_time = result;
				did_step = TRUE;
			}
		}
		//If a step occured, update the graphical representations of the world by sending all the clients updates.
		if (did_step)
		{
			sph_send_world_updates(world);
			did_step = FALSE;
		}
		
		//Check for an proccess data coming out of the recieve queue from the server thread
		sph_process_queue((gpointer)world);
		
		//Handle quitting cleanly.
		sigpending(&signal_set);		
		if (sigismember(&signal_set, SIGINT))
		{
			break;
		}
	}
	 
	server_destroy(); 
	

	printf("Exited Cleanly\n");
	
	return EXIT_SUCCESS;

}

