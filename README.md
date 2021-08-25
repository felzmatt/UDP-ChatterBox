# UDP-ChatterBox
ChatterBox to allow mesage passing between remote hosts 
# Server Description
UDP-ChatterBox is a server to allow message passing between hosts that are not connected each other.
The communication is based on UDP, this means that not exists a client-server dedicated connection handled by a thread in the server process, instead the chatterbox sees only a stream of datagrams containing a sender id, a receiver id, and the actual message. Is job is delivery the message to the receiver.
Authentication is mandatory. 
This means that the server holds a file structured as follow:
| Username | Password |
| -------- | -------- |
| user1    | pwd1     |
| ...      | ...      |
| userN    | pwdN     |

So when server boots the first thing it is request to do is to read the users.db file and parsing it in order to create a RegisteredUsers list where used only to keep track of users registered.

Then the server create an empty list with connected users that holds connection details of authenticated clients.

The server is sigle thread.
It has onlt to loop on the same loop that is designed is a request-response manner.
This means that when the server reads an incoming message, any type it is, a response will happen.
While ON:
  read a packet from socket
  execute something depending on packet type
  write a packet on socket

Possible scenarios:
[1] Subscribe
  [a] Parse the packet that holds SUBSCRIBE <username> <password>
  [b] Append user to registered users list
  [c] send SUBSCRIBE_OK to client
[2] Login
  [a] Parse the packet that holds LOGIN <username> <password>
  [b] Evaluate user authentication status
  [c] send the evaluation to client


# Client Description
Client is the user interface to communicate with server.
It offers a set of commands and related arguments to the user.
When boot up a client create a UDP socket and ask the user to choose between Subscribe or Login.
This first phase is blocking the user input so single thread managed.
After Login is succesful the user can choose between PrivateMessage OnlineList ShowPendingMessages Disconnect.
Client needs to be dual-thread because the enviroment is asynchronous and there are many clients, so is it not know where a message will be received and it such moment could happen that the user is typing.
So a thread is needed to read the socket and store the messages incoming into a dedicated buffer, and the other thread is needed to deal with the user interactivity.
One of the interactivity of the user is ShowPendingMessages that has a side effect on the message buffer, that is often managed by the incoming message thread, this means a synhronization method is needed.
