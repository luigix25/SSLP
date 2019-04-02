CFLAGS= -Wall -std=c++14 -g
LIBFLG = -lcrypto
LIBDIP = library.o library/library.h

all: client server

client.o: client/client.h client/client.cpp $(LIBDIP)
	g++ $(CFLAGS) client/client.cpp -c -o client.o 

server.o: server/server.h server/server.cpp $(LIBDIP)
	g++ $(CFLAGS) server/server.cpp -c -o server.o 

library.o: library/library.h library/library.cpp
	g++ $(CFLAGS) library/library.cpp -c -o library.o 

client: client.o library.o
	g++ $(CFLAGS) -o client_bin client.o library.o $(LIBFLG) 

server: server.o library.o
	g++ $(CFLAGS) -o server_bin server.o library.o $(LIBFLG) 

clean:
	rm -f *.o server_bin client_bin
