#include "HMACManager.h"

uint32_t HMACManager::local_nonce = 0;
uint32_t HMACManager::remote_nonce = 0;

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

char* HMACManager::HMACFinal(){
  uint32_t a = 0;           //giusto per farlo compilare
  return HMACFinal(a);
}


char* HMACManager::HMACFinal(uint32_t &nonce){
	int hash_size = EVP_MD_size(EVP_sha256());
	char *digest = new char [HASH_SIZE];

  if(nonce != 0){
    chunk c;
    c.size = sizeof(uint32_t);
    c.plaintext = (char*)&nonce;

    if(!HMACUpdate(c)){
      return NULL;
    }
    cout<<"LOCAL NONCE: "<<local_nonce<<endl;
    cout<<"REMOTE NONCE: "<<remote_nonce<<endl;
    nonce++;
  }

	if(!HMAC_Final(mdctx, (unsigned char*)digest, (unsigned int*)&hash_size)){
		perror("Error in HMAC_Final");
		return NULL;
	}

	HMAC_CTX_free(mdctx);
	return digest;
}

bool HMACManager::setLocalNonce(uint32_t nonce){
  if(local_nonce != 0 || nonce == 0)
    return false;

  local_nonce = nonce;
  return true;
}

bool HMACManager::setRemoteNonce(uint32_t nonce){
  if(remote_nonce != 0 || nonce == 0)
    return false;

  remote_nonce = nonce;
  return true;
}

uint32_t HMACManager::getLocalNonce(){
  return local_nonce;
}

uint32_t HMACManager::getRemoteNonce(){
  return remote_nonce;
}

