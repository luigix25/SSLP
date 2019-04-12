#ifndef SEND_RECEIVE_FILE
#define SEND_RECEIVE_FILE
#include "library.h"
#include "EncryptManager.h"
#include "HMACManager.h"
#include "ReadFileManager.h"
#include "WriteFileManager.h"
#include "DecryptManager.h"

bool SendFile(string & ,NetSocket &,char* );
bool ReceiveFile(string & ,char *, NetSocket &);

bool SendFileHMACchunk(string & ,NetSocket &,char* );
bool ReceiveFileHMACchunk(string & ,char *, NetSocket &);

#endif