CFLAGS= -Wall -Wextra -pedantic -std=c++14 -g
LIBFLG = -lcrypto
LIBDIP = library/library.h library/ReadFileManager.h library/WriteFileManager.h library/FileManager.h library/EncryptManager.h library/DecryptManager.h library/HMAC.h library/SendReceiveFile.h
LIBFLS = library.o filemanager.o writefilemanager.o readfilemanager.o encryptmanager.o decryptmanager.o hmac.o rsav.o rsas.o sendreceivefile.o certm.o rsae.o rsad.o dh.o hash.o


all: client server

client.o: library/library.h client_files/client.h client_files/client.cpp 
	g++ $(CFLAGS) client_files/client.cpp -c -o client.o 

server.o: library/library.h server_files/server.h server_files/server.cpp
	g++ $(CFLAGS) server_files/server.cpp -c -o server.o 

library.o: library/library.h library/library.cpp
	g++ $(CFLAGS) library/library.cpp -c -o library.o 

filemanager.o: library/library.h library/FileManager.cpp library/FileManager.h
	g++ $(CFLAGS) library/FileManager.cpp -c -o filemanager.o 

readfilemanager.o: library/library.h library/ReadFileManager.h library/ReadFileManager.cpp library/FileManager.h
	g++ $(CFLAGS) library/ReadFileManager.cpp -c -o readfilemanager.o 

writefilemanager.o: library/library.h library/WriteFileManager.h library/WriteFileManager.cpp library/FileManager.h
	g++ $(CFLAGS) library/WriteFileManager.cpp -c -o writefilemanager.o 

encryptmanager.o: library/library.h library/EncryptManager.h library/EncryptManager.cpp
	g++ $(CFLAGS) library/EncryptManager.cpp -c -o encryptmanager.o 

decryptmanager.o: library/library.h library/DecryptManager.h library/DecryptManager.cpp
	g++ $(CFLAGS) library/DecryptManager.cpp -c -o decryptmanager.o 

hmac.o: library/library.h library/HMACManager.h library/HMACManager.cpp
	g++ $(CFLAGS) library/HMACManager.cpp -c -o hmac.o 

hash.o: library/library.h library/HashManager.h library/HashManager.cpp
	g++ $(CFLAGS) library/HashManager.cpp -c -o hash.o 

rsas.o: library/library.h library/RSASignManager.h library/RSASignManager.cpp
	g++ $(CFLAGS) library/RSASignManager.cpp -c -o rsas.o 

rsav.o: library/library.h library/RSAVerifyManager.h library/RSAVerifyManager.cpp
	g++ $(CFLAGS) library/RSAVerifyManager.cpp -c -o rsav.o 

rsae.o: library/library.h library/RSAEncryptManager.h library/RSAEncryptManager.cpp
	g++ $(CFLAGS) library/RSAEncryptManager.cpp -c -o rsae.o 

rsad.o: library/library.h library/RSADecryptManager.h library/RSADecryptManager.cpp
	g++ $(CFLAGS) library/RSADecryptManager.cpp -c -o rsad.o 
	
certm.o: library/library.h library/CertificateManager.h library/CertificateManager.cpp
	g++ $(CFLAGS) library/CertificateManager.cpp -c -o certm.o 

dh.o: library/library.h library/DHManager.h library/DHManager.cpp
	g++ $(CFLAGS) library/DHManager.cpp -c -o dh.o 

sendreceivefile.o: library/library.h library/SendReceiveFile.h library/SendReceiveFile.cpp
	g++ $(CFLAGS) library/SendReceiveFile.cpp -c -o sendreceivefile.o

client: client.o $(LIBFLS)
	g++ $(CFLAGS) -o client client.o $(LIBFLS) $(LIBFLG) 

server: server.o $(LIBFLS)
	g++ $(CFLAGS) -o server server.o $(LIBFLS) $(LIBFLG) 


clean:
	rm -f *.o server client
