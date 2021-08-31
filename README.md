# UDP-ChatterBox
A simple server-client architecture to allow message passing between hosts trough sockets.
# How to install
1) Clone this repository
2) make server
3) make client
# How to run
1) Open a terminal on a machine in your subnet and take note of its IP Address
2) In machine of point 1, simply launch ./server in the directory where users.db lays
3) Open terminals on same machine, or other in the same subnet and type ./client "server-ip-address"
# On client
Client is dual thread:
one thread is dealing with reception of messages from server
one thread is listening for user interaction
the interface is self explainatory, SPM is the command to see the received messages, and is used also to see the messages of error sent from server
