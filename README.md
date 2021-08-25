# UDP-ChatterBox
ChatterBox to allow mesage passing between remote hosts 
# Desctiption
UDP-ChatterBox is a server to allow message passing between hosts that are not connected each other.
The communication is based on UDP, this means that not exists a client-server dedicated connection handled by a thread in the server process, instead the chatterbox sees only a stream of datagrams containing a sender id, a receiver id, and the actual message. Is job is delivery the message to the receiver.
Authentication is mandatory. This means that the server holds a file structured as follow:
| Username | Password |
| -------- | -------- |
| user1    | pwd1     |
| ...      | ...      |
|  userN   |  pwdN    |
