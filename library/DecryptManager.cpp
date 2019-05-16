#include "DecryptManager.h"

DecryptManager::DecryptManager(){
	
	this->ctx = EVP_CIPHER_CTX_new();
  	if(!EVP_DecryptInit(ctx, EVP_aes_128_cbc(), this->key, this->IV)){
  		perror("Error In EVP_DecryptInit");
  	}

}

DecryptManager::DecryptManager(const char *_key, const char *_IV) : DecryptManager(){
	setAESKey(_key);
	setAESIV(_IV);

}

bool DecryptManager::DecryptUpdate(chunk &c, encryptedChunk& ec){

    if(!EVP_DecryptUpdate(this->ctx, (unsigned char*)c.plaintext, &c.size, (unsigned char*)ec.ciphertext, ec.size)){
    	perror("Error in EVP_DecryptUpdate");
    	return false;
    }

    return true;

}

bool DecryptManager::DecryptFinal(chunk &c){
	int len;
	
	if(!EVP_DecryptFinal(ctx, (unsigned char*)c.plaintext + c.size, &len)){
		perror("Error in EVP_DecryptFinal");
		return false;
	}
  	c.size += len;
	//EVP_CIPHER_CTX_free(ctx);
	return true;
}

DecryptManager::~DecryptManager(){
	EVP_CIPHER_CTX_free(ctx);
}
