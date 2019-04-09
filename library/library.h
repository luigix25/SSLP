//Libreria Send e Receive senza dimensione
#ifndef LIBRARY
	#define LIBRARY

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/err.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <dirent.h>



#define true 1
#define false 0
#define HASH_SIZE 32
#define AES_BLOCK 16
#define KEY_AES "panuozzopanuozz"
#define KEY_HMAC "bombabombabomba"
#define AES_IV "cornettonebomba"

#define CLIENT_PATH "client_files/database/"
#define SERVER_PATH "server_files/database/"

using namespace std;

#define MAX_CHUNK_SIZE 256 *1024

extern const char *commands_list[5];

enum file_status{NO_ERRORS,OUT_OF_BOUND,FILE_ERROR,END_OF_FILE};
enum commands {HELP_COMMAND,LIST_COMMAND,GET_COMMAND,UPLOAD_COMMAND};

struct chunk{
	int size;
	char* plaintext;
};

struct encryptedChunk{
	int size;
	char* ciphertext;
};



//enum protocol_login {LOGIN_OK,LOGIN_FAIL};
//enum protocol_connect {CONNECT_NOUSER,CONNECT_BUSY,CONNECT_REFUSED,CONNECT_OK,CONNECT_REQ,CONNECT_DATA,CONNECT_ACPT,CONNECT_RFSD};


class NetSocket{

	int socket;

	public:
		NetSocket();
		NetSocket(int);

		void setSocket(int);

		bool sendInt(int);							//socket value
		bool sendData(const char *,int32_t);

		char* recvData(int32_t&);
		bool recvInt(int&);

		void closeConnection();


};


char* serialization(char*, char*, int);
void unserialization(char* ,int, chunk &, char*);


vector<string> get_file_list(const char*);

#endif