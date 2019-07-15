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

bool SendFile(string & ,NetSocket &,string &,const char *,bool);
bool ReceiveFile(string & ,string&, NetSocket &,PublicKey&,bool);


#endif