//Libreria Send e Receive senza dimensione
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <string.h>

#define true 1
#define false 0

extern const char *commands_list[5];

enum commands {HELP_COMMAND,LIST_COMMAND};
//enum protocol_login {LOGIN_OK,LOGIN_FAIL};
//enum protocol_connect {CONNECT_NOUSER,CONNECT_BUSY,CONNECT_REFUSED,CONNECT_OK,CONNECT_REQ,CONNECT_DATA,CONNECT_ACPT,CONNECT_RFSD};


class NetSocket{

	int socket;

	public:
		NetSocket();
		NetSocket(int);

		void setSocket(int);

		int sendInt(int);							//socket value
		int sendData(const char *,int);

		char* recvData(int&);
		int recvInt(int&);

		void closeConnection();


};







