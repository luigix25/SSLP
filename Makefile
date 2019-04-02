CFLAGS=-Wall
LIBFLG = -lcrypto
LIBDIP = library.o library/library.h

all: client server

client.o: client/client.h $(LIBDIP)
	g++ client/client.cpp -c -o client.o $(CFLAGS)

server.o: server/server.h $(LIBDIP)
	g++ server/server.cpp -c -o server.o $(CFLAGS)

library.o: library/library.h
	gcc library/library.c -c -o library.o $(CFLAGS)

client: client.o library.o
	g++ -o client_bin client.o library.o $(LIBFLG) $(CFLAGS)

server: server.o library.o
	g++ -o server_bin server.o library.o $(LIBFLG) $(CFLAGS)

clean:
	rm -f *.o server_bin client_bin
