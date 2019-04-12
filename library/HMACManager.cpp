#include "HMACManager.h"

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
	int hash_size = EVP_MD_size(EVP_sha256());
	char *digest = (char*)malloc(HASH_SIZE);

	if(!HMAC_Final(mdctx, (unsigned char*)digest, (unsigned int*)&hash_size)){
		perror("Error in HMAC_Final");
		return NULL;
	}

	HMAC_CTX_free(mdctx);
	return digest;
}

