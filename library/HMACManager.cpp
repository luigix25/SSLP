#include "HMACManager.h"


uint32_t HMACManager::nonce[] = {0,0};

HMACManager::HMACManager(){
  this->mdctx = HMAC_CTX_new();
 // size_t key_hmac_size = HMAC_KEY_SIZE;


    if(!HMAC_Init_ex(mdctx, this->hmac_key, HMAC_KEY_SIZE, EVP_sha256(), NULL)){
      perror("Error In HMAC_Init_ex");
    }

}

HMACManager::HMACManager(const char *_key) : HMACManager(){
	memcpy(this->hmac_key,_key,16);

}

bool HMACManager::HMACUpdate(encryptedChunk &ec){

    if(!HMAC_Update(mdctx, (unsigned char*) ec.ciphertext,ec.size)){
    	perror("Error in EVP_DecryptUpdate");
    	return false;
    }

    return true;

}

bool HMACManager::HMACUpdate(chunk &ec){

    if(!HMAC_Update(mdctx, (unsigned char*) ec.plaintext,ec.size)){
    	perror("Error in EVP_DecryptUpdate");
    	return false;
    }

    return true;

}

bool HMACManager::HMACUpdate(const char *pt,int len){

    if(!HMAC_Update(mdctx, (unsigned char*) pt,len)){
      perror("Error in EVP_DecryptUpdate");
      return false;
    }

    return true;

}

char* HMACManager::HMACFinal(enum_nonce en){
	int hash_size = EVP_MD_size(EVP_sha256());
	char *digest = new char [HASH_SIZE];

  if(nonce != 0){
    chunk c;
    c.size = sizeof(uint32_t);
    c.plaintext = (char*)&nonce[en];

    if(!HMACUpdate(c)){
      return NULL;
    }

    nonce[en]++;
  }
  
	if(!HMAC_Final(mdctx, (unsigned char*)digest, (unsigned int*)&hash_size)){
		perror("Error in HMAC_Final");
		return NULL;
	}

//	HMAC_CTX_free(mdctx);
	return digest;
}

HMACManager::~HMACManager(){

  HMAC_CTX_free(mdctx);

}

bool HMACManager::setLocalNonce(uint32_t new_nonce){
  if(/*nonce[LOCAL_NONCE] != 0 || */new_nonce == 0)
    return false;

  nonce[LOCAL_NONCE] = new_nonce;

  return true;
}

bool HMACManager::setRemoteNonce(uint32_t new_nonce){
  if(/*nonce[REMOTE_NONCE] != 0 ||*/ new_nonce == 0)
    return false;

  nonce[REMOTE_NONCE] = new_nonce;

  return true;
}

