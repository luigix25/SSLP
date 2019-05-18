#ifndef SEND_RECEIVE_FILE
#define SEND_RECEIVE_FILE
#include "library.h"
#include "EncryptManager.h"
#include "HMACManager.h"
#include "ReadFileManager.h"
#include "WriteFileManager.h"
#include "DecryptManager.h"
#include "RSASignManager.h"
#include "RSAVerifyManager.h"
#include "PublicKey.h"

bool SendFile(string & ,NetSocket &,const char* ,const char *);
bool ReceiveFile(string & ,const char *, NetSocket &,PublicKey&);


#endif