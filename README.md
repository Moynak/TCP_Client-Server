# TCP_Client-Server
Basic TCP Client Server Implementation in C
This is a simple chat program developed at the transport layer.

Users can login and talk to each other using their usernames.
The server keeps track of who's logging in and routes the message to the correct user

How to run?

Here's an example of how to run on localhost.


1. compile both the programs:
	gcc -pthread -o client client.c
	gcc -pthread -o server server.c

2. run the server at the desired port (say 6000)
	./server 6000

3. now run the clients; and enter information as prompted. 
For instance ,
./client 6000 127.0.0.1

Example : 
When a client joins:
```
client socket created
connected to server---
:::Your No. 0
```
In Server it logs:
```
server: waiting for connections...
server: got connection from 127.0.0.1
Its CLIENT[0]
```
When Client[0] talks to Client[1], it starts its message starting with recipients index number.
```
1 Hello
```
When a client presses Ctrl+C it is disconnected.

