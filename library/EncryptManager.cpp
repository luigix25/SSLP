#include "EncryptManager.h"

EncryptManager::EncryptManager(const char *key, const char *IV){
	memcpy(this->key,key,16);
	memcpy(this->IV,IV,16);
	
  	this->ctx = EVP_CIPHER_CTX_new();
  	if(!EVP_EncryptInit(ctx, EVP_aes_128_cbc(), this->key, this->IV)){
  		perror("Error In EVP_EncryptInit");
  	}

}				//KEY AND IV

bool EncryptManager::EncryptUpdate(encryptedChunk& ec, chunk& c){

    if(!EVP_EncryptUpdate(this->ctx, (unsigned char*)ec.ciphertext, &ec.size, (unsigned char*)c.plaintext, c.size)){
    	perror("Error in EVP_EncryptUpdate");
    	return false;
    }

    return true;

}

bool EncryptManager::EncryptFinal(encryptedChunk& ec){
	int len;
	
	if(!EVP_EncryptFinal(ctx, (unsigned char*)ec.ciphertext + ec.size, &len)){
		perror("Error in EVP_EncryptFinal");
		return false;
	}
  	ec.size += len;
	//EVP_CIPHER_CTX_free(ctx);
	return true;
}

EncryptManager::~EncryptManager(){
	EVP_CIPHER_CTX_free(ctx);
}
