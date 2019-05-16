#include "EncryptManager.h"


EncryptManager::EncryptManager(){

  	this->ctx = EVP_CIPHER_CTX_new();
  	if(!EVP_EncryptInit(ctx, EVP_aes_128_cbc(), this->key, this->IV)){
  		perror("Error In EVP_EncryptInit");
  	}

}


EncryptManager::EncryptManager(const char *_key, const char *_IV) : EncryptManager() {
	setAESKey(_key);
	setAESIV(_IV);
}				//KEY AND IV

bool EncryptManager::EncryptUpdate(char *& ciphertext ,int32_t& ciphertext_len, const char* plaintext ,int32_t plaintext_len){

	ciphertext = new char[plaintext_len + AES_BLOCK];
 	
 	if(!EVP_EncryptUpdate(this->ctx, (unsigned char*)ciphertext, &ciphertext_len, (unsigned char*)plaintext, plaintext_len)){
    	perror("Error in EVP_EncryptUpdate");
    	return false;
    }

    return true;

}


bool EncryptManager::EncryptUpdate(encryptedChunk& ec, chunk& c){

	return EncryptUpdate(ec.ciphertext,ec.size,c.plaintext,c.size);

}

bool EncryptManager::EncryptFinal(encryptedChunk& ec){

	return EncryptFinal(ec.ciphertext,ec.size);	
}

bool EncryptManager::EncryptFinal(char* ciphertext,int32_t &len){

	int local_len;
	
	if(!EVP_EncryptFinal(ctx, (unsigned char*)ciphertext + len, &local_len)){
		perror("Error in EVP_EncryptFinal");
		return false;
	}
  	len += local_len;
	//EVP_CIPHER_CTX_free(ctx);
	return true;

}


EncryptManager::~EncryptManager(){
	EVP_CIPHER_CTX_free(ctx);
}