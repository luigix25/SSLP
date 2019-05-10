#include "HMACManager.h"


uint32_t HMACManager::nonce[] = {0,0,0,0};

HMACManager::HMACManager(const char *key){
	memcpy(this->key,key,16);

	this->mdctx = HMAC_CTX_new();
	size_t key_hmac_size = sizeof(KEY_HMAC);


  	if(!HMAC_Init_ex(mdctx, this->key, key_hmac_size, EVP_sha256(), NULL)){
  		perror("Error In HMAC_Init_ex");
  	}

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

/*char* HMACManager::HMACFinal(){
  uint32_t a = 0;           //giusto per farlo compilare
  return HMACFinal(a);
}*/


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
   // cout<<"LOCAL NONCE: "<<nonce[LOCAL_NONCE]<<endl;
   // cout<<"REMOTE NONCE: "<<nonce[REMOTE_NONCE]<<endl;
    nonce[en]++;
  }

	if(!HMAC_Final(mdctx, (unsigned char*)digest, (unsigned int*)&hash_size)){
		perror("Error in HMAC_Final");
		return NULL;
	}

	HMAC_CTX_free(mdctx);
	return digest;
}

bool HMACManager::setLocalNonce(uint32_t new_nonce){
  if(nonce[LOCAL_NONCE] != 0 || new_nonce == 0)
    return false;

  nonce[LOCAL_NONCE] = new_nonce;
  nonce[LOCAL_INT_NONCE] = new_nonce + NONCE_OFFSET;

  return true;
}

bool HMACManager::setRemoteNonce(uint32_t new_nonce){
  if(nonce[REMOTE_NONCE] != 0 || new_nonce == 0)
    return false;

  nonce[REMOTE_NONCE] = new_nonce;
  nonce[REMOTE_INT_NONCE] = new_nonce + NONCE_OFFSET;

  return true;
}

/*
uint32_t HMACManager::getLocalNonce(){
  return nonce[LOCAL_NONCE];
}

uint32_t HMACManager::getRemoteNonce(){
  return nonce[REMOTE_NONCE];
}*/

