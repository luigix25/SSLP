//Libreria Send e Receive senza dimensione
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <string.h>
#include<string>

#define true 1
#define false 0

extern const char *commands_list[5];

#define MAX_CHUNK_SIZE 256

enum file_status{NO_ERRORS,OUT_OF_BOUND,FILE_NOT_EXISTS,FILE_ERROR_OPEN,FILE_ERROR_SIZE};
enum commands {HELP_COMMAND,LIST_COMMAND,GET_COMMAND};

struct chunk{
	int size;
	char plaintext[MAX_CHUNK_SIZE];
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
		bool sendData(const char *,int);

		char* recvData(int&);
		bool recvInt(int&);

		void closeConnection();


};







