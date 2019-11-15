cc=g++
flags=-g -lpthread
.PHONY:all
all:client server
client:client.cc
	$(cc) -o $@ $^ 
server:server.cc
	$(cc) -o $@ $^ $(flags)
.PHONY:clean
clean:
	rm -f client server
