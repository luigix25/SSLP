CFLAGS=-Wall

client.o: client/client.h library.o library/library.h
	g++ client/client.cpp -c -o client.o $(CFLAGS)

server.o: library.o library/library.h
	g++ server/server.cpp -c -o server.o $(CFLAGS)

library.o: library/library.h
	gcc library/library.c -c -o library.o $(CFLAGS)

client: client.o library.o
	g++ -o client client.o library.o -lcrypto $(CFLAGS)
  
server: server.o posit.o
	g++ -o server server.o library.o -lcrypto $(CFLAGS)

clean:
	rm -f *.o server client
