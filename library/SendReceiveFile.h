#ifndef SEND_RECEIVE_FILE
#define SEND_RECEIVE_FILE
#include "library.h"
#include "EncryptManager.h"
#include "HMACManager.h"
#include "ReadFileManager.h"
#include "WriteFileManager.h"
#include "DecryptManager.h"
#include "RSASignManager.h"

bool SendFile(string & ,NetSocket &,char* );
bool ReceiveFile(string & ,char *, NetSocket &);


#endif