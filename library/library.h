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

//enum commands {HELP_COMMAND,WHO_COMMAND,CONNECT_COMMAND,CONNECT_ANSWER,QUIT_COMMAND,LOGIN_COMMAND,DISCONNECT_COMMAND};
//enum protocol_login {LOGIN_OK,LOGIN_FAIL};
//enum protocol_connect {CONNECT_NOUSER,CONNECT_BUSY,CONNECT_REFUSED,CONNECT_OK,CONNECT_REQ,CONNECT_DATA,CONNECT_ACPT,CONNECT_RFSD};


int sendInt(int,int);
int sendData(int,const char *,int);

char* recvData(int,int&);
int recvInt(int,int*);




