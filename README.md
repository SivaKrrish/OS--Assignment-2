# OS--Assignmnet-2

The goal of this homework is to become familiar with concurrent processing in the Unix/Linux using shared memory.
You will write a program that uses multiple processes to simulate a swim mill to show the behavior of a sh swimming
upstream. The swim mill is represented as a one-dimensional array such that each element indicates the distance
from the shore. The sh will occupy one and only one of these elements. The presence of a sh is indicated by
changing the integer in the specied element. For example, we can represent the sh by the character F. By default,
we will have the sh swimming right in the middle of the stream.
Somewhere upstream, a pellet is dropped into the stream. Our sh sees this pellet traveling towards it and moves
sideways to enable its capture. If the pellet and the sh are in the same position at any point, the sh is said to have
eaten the pellet. Of course, it is possible for our sh to miss the pellet if the sh cannot move close to it in time.
Dierent types of processes needed for this project are as follows:
 A set of pellet processes: Each pellet process drops a pellet at a random distance from the sh. We will call
this process pellet. The pellet will start moving towards the sh with the 
ow of the river. For simplicity, we
will assume that the pellet moves in a straight line downstream.
 A process called fish: the fish sees the pellet as it is dropped and moves one unit left or right to start aligning
itself with the pellet. As the pellet moves towards the sh and the sh is at the same distance from the shore
as the pellet, it stops changing its sideways position and devours the pellet when they are coincident. After
eating the pellet, or if the pellet is missed, the sh returns to swimming midstream.
 A coordinator process: It is responsible for creating the fish process, the pellet processes, and coordinating
the termination of pellets. We could have more than one pellet at any time. Note that the position of pellet
and sh are maintained by their respective processes.
The coordinator process also sets a timer at the start of computation to 30 seconds. If computation has not
nished by this time, coordinator kills all the children and grandchildren, and then exits. Make sure that you
print appropriate message(s).
In addition, coordinator should print a message when an interrupt signal (^C) is received. Make sure that
all the children/grandchildren are killed by coordinator when this happens, and all the shared memory is
deallocated. The grandchildren kill themselves upon receiving interrupt signal but print a message on stderr
to indicate that they are dying because of an interrupt, along with the identication information. Make sure
that the processes handle multiple interrupts correctly. As a precaution, add this feature only after your
program is well debugged.
