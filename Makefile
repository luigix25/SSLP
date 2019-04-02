CFLAGS=-Wall
LIB = -lcrypto

all: client server

client.o: client/client.h library.o library/library.h
	g++ client/client.cpp -c -o client.o $(CFLAGS)

server.o: server/server.h library.o library/library.h
	g++ server/server.cpp -c -o server.o $(CFLAGS)

library.o: library/library.h
	gcc library/library.c -c -o library.o $(CFLAGS)

client: client.o library.o
	g++ -o client_bin client.o library.o $(LIB) $(CFLAGS)

server: server.o library.o
	g++ -o server_bin server.o library.o $(LIB) $(CFLAGS)

clean:
	rm -f *.o server_bin client_bin
