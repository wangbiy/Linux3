cc=g++
flags=-g -lpthread -std=c++11
.PHONY:all
all:Client Server
Client:tcpClient.cc
	$(cc) -o $@ $^ 
Server:tcpServer.cc
	$(cc) -o $@ $^ $(flags)
.PHONY:clean
clean:
	rm -f Client Server
