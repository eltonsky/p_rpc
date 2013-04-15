p_rpc
=====

A efficient rpc framework. This is used as RPC componenet in yadfs. It's implemented as a standalone framework, so it's
easy to plug into your application.

There are quite a few existing rpc frameworks written in c++. However,in order to get full control, I decide to write my
own. Also, I see this is a good project to practice my multi-threading and networking in c++. 

There are 2 major parts : server and client. 

Server side is a pipeline with 4 stages :

  Listener: accept connections from client, add socket to one of the readers queue when the sock is ready to read
  
  V
  
  Reader(s): read a call from client socket and add the call to one of the handlers' queue.
  
  V
  
  Handler: execute the call and add the result to responder queue
  
  V
  
  Responder: send the result to client
  
  There are only one Listener and Responder thread respectively, due to they are invariant of calls. On the contrary, 
  there are multiple Readers and Handlers, because each call is different in size and complexity. 
  A client's connection will stay open for more calls from client. A connection will be closed if no call received in a
  configured timeout.
  
Client side is simpler. A client object keeps a list of conections. Each connection object holds info for the actual
connection to a server. A connection object has a list of calls made to that server, it also has another thread for receving result. In another word, each connection has a separate response queue, which avoids race condition. 
Also, a connection ensure calls are sent synchronously to server. This is to archieve overall best throughput on server side.


The reflection part in handler is cheeky :P . There's no easy way to do reflection in c++, so I just use a big if...else for now. Anyway the point of this project is not that. I might add reflection some time.

Elton




  
  
