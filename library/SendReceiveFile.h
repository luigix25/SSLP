#ifndef SEND_RECEIVE_FILE
#define SEND_RECEIVE_FILE
#include "library.h"
#include "EncryptManager.h"
#include "HMACManager.h"
#include "ReadFileManager.h"
#include "WriteFileManager.h"
#include "DecryptManager.h"


bool SendFile(string & ,NetSocket & );
bool ReceiveFile(string & ,string &, NetSocket &);

#endif