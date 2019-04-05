CFLAGS= -Wall -std=c++14 -g
LIBFLG = -lcrypto
LIBDIP = library/library.h library/ReadFileManager.h library/WriteFileManager.h library/FileManager.h
LIBFLS = library.o filemanager.o writefilemanager.o readfilemanager.o

all: client server

client.o: client/client.h client/client.cpp 
	g++ $(CFLAGS) client/client.cpp -c -o client.o 

server.o: server/server.h server/server.cpp
	g++ $(CFLAGS) server/server.cpp -c -o server.o 

library.o: library/library.h library/library.cpp
	g++ $(CFLAGS) library/library.cpp -c -o library.o 

filemanager.o: library/library.h library/FileManager.cpp library/FileManager.h
	g++ $(CFLAGS) library/FileManager.cpp -c -o filemanager.o 

readfilemanager.o: library/library.h library/ReadFileManager.h library/ReadFileManager.cpp library/FileManager.h
	g++ $(CFLAGS) library/ReadFileManager.cpp -c -o readfilemanager.o 

writefilemanager.o: library/library.h library/WriteFileManager.h library/WriteFileManager.cpp library/FileManager.h
	g++ $(CFLAGS) library/WriteFileManager.cpp -c -o writefilemanager.o 

client: client.o $(LIBFLS)
	g++ $(CFLAGS) -o client_bin client.o $(LIBFLS) $(LIBFLG) 

server: server.o $(LIBFLS)
	g++ $(CFLAGS) -o server_bin server.o $(LIBFLS) $(LIBFLG) 

clean:
	rm -f *.o server_bin client_bin
