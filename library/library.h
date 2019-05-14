//Libreria Send e Receive senza dimensione
#ifndef LIBRARY
	#define LIBRARY

#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include <openssl/conf.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/x509_vfy.h>
#include <openssl/err.h>
#include <openssl/crypto.h>

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <stdint.h>
#include <signal.h>

#define true 1
#define false 0
#define HASH_SIZE 32
#define AES_BLOCK 16
#define KEY_AES "panuozzopanuozz"
#define KEY_HMAC "bombabombabomba"
#define AES_IV "cornettonebomba"

#define SERVER_NONCE 	12345
#define CLIENT_NONCE 	54321
#define NONCE_SIZE 		4


#define CLIENT_PATH "client_files/database/"
#define SERVER_PATH "server_files/database/"

#define CLIENT_PRIVKEY_PATH "keys/client_privkey.pem"
#define CLIENT_PUBKEY_PATH 	"keys/client_pubkey.pem"
#define SERVER_PRIVKEY_PATH "keys/server_privkey.pem"
#define SERVER_PUBKEY_PATH 	"keys/server_pubkey.pem"

#define CERT_CA_PATH		"certificates/SSLPCA_cert.pem"
#define CERT_CA_CRL_PATH	"certificates/SSLPCA_crl.pem"

#define CLIENT_NAME			"SSLPClient"
#define SERVER_NAME			"SSLPServer"


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

enum enum_nonce{LOCAL_NONCE, REMOTE_NONCE};


class NetSocket{

	int socket;

	private:

	bool utilitySend(const char*,uint32_t);
	bool utilityRecv(char *,uint32_t);
	bool isClosed;


	public:
		NetSocket();
		NetSocket(int);

		void setSocket(int);

		bool sendInt(int);	
		bool sendData(const char *,uint32_t);

		char* recvData(uint32_t);
		bool recvInt(int&);

		void closeConnection();


};


bool sendIntHMAC(NetSocket&,int32_t);
bool recvIntHMAC(NetSocket&,int32_t&);

bool sendDataHMAC(NetSocket&,const char *,int32_t);
char* recvDataHMAC(NetSocket&,int32_t&);

vector<string> get_file_list(const char*);
vector<string> split (string, string);
#endif