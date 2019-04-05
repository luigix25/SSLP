CFLAGS= -Wall -std=c++14 -g
LIBFLG = -lcrypto
LIBDIP = library.o library/library.h filemanager.o
LIBFLS = library.o filemanager.o enc_dec.o

all: client server

client.o: client/client.h client/client.cpp $(LIBDIP)
	g++ $(CFLAGS) client/client.cpp -c -o client.o 

server.o: server/server.h server/server.cpp $(LIBDIP)
	g++ $(CFLAGS) server/server.cpp -c -o server.o 

library.o: library/library.h library/library.cpp
	g++ $(CFLAGS) library/library.cpp -c -o library.o 

filemanager.o: library/library.h library/FileManager.cpp
	g++ $(CFLAGS) library/FileManager.cpp -c -o filemanager.o 

enc_dec.o: library/library.h library/enc_dec.cpp 
	g++ $(CFLAGS) library/enc_dec.cpp -c -o enc_dec.o

client: client.o library.o filemanager.o enc_dec.o
	g++ $(CFLAGS) -o client_bin client.o $(LIBFLS) $(LIBFLG) 

server: server.o library.o filemanager.o enc_dec.o
	g++ $(CFLAGS) -o server_bin server.o  $(LIBFLS) $(LIBFLG) 

clean:
	rm -f *.o server_bin client_bin
