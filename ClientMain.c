#include "./GUI/MainWindow.h"
#include "./Client/Client.h"
#include "./Client/ClientPacketHandler.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{			    
	gtk_init(&argc, &argv);
				
	MainWindow* main_window = main_window_new();
	
	g_timeout_add_full(G_PRIORITY_DEFAULT, 10, (GSourceFunc)cph_process_queue, NULL, NULL);
	
	// Start the main run loop		
	main_window_begin(main_window);
    	
	return EXIT_SUCCESS;
}