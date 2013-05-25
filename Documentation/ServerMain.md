#ServerMain

####Back To [Overview](./Overview.html)

##Overview

ServerMain initializes the [server](./Server.html) and [world](./World.html). After these initializations, the main run loop starts. The run loop does several things.

1. Step the physics world
2. Send updates to the clients
3. Process incoming packets

##Time Management

In order to maintain a somewhat accurate sense of time, the cpSpace must be stepped exactly 60 times every second. Using sleep is one option, however it is not very accurate. Instead of sleep, I chose to use the gettimeofday function supplied in time.h. The process for keeping an accurate time step are as follows.

###Initialization
1. Store the current time to both current\_time and physics\_time.

###Run Loop
1. Get the current time and store it to current\_time.
2. Compare the current\_time with the physics\_time. If the result is greater than 1/60 of a second, then do a physics step and add 1/60 of a second to physics\_time. This step is repeated until current\_time - physics\_time > 1/60 of a second. In this manner, if the physics falls behind, it will step enough times to catch up.
3. If step 2 happened (if the physics world was updated), then send the updated information to the clients.
4. Process the incoming packets on the queue.
5. Check to see if an interrupt signal has been sent and exit gracefully if so.