#include "DecryptManager.h"

DecryptManager::DecryptManager(){
	
	this->ctx = EVP_CIPHER_CTX_new();
  	if(!EVP_DecryptInit(ctx, EVP_aes_128_cbc(), this->key, this->IV)){
  		perror("Error In EVP_DecryptInit");
  	}

	incrementIV();

}

DecryptManager::DecryptManager(const char *_key, const char *_IV) : DecryptManager(){
	setAESKey(_key);
	setAESIV(_IV);

}

bool DecryptManager::DecryptUpdate(Chunk &c, EncryptedChunk& ec){
	
	c.setPlainText(new char[ec.size + AES_BLOCK]);


    if(!EVP_DecryptUpdate(this->ctx, (unsigned char*)c.getPlainText(), &c.size, (unsigned char*)ec.getCipherText(), ec.size)){
    	perror("Error in EVP_DecryptUpdate");
    	return false;
    }

    return true;


}

bool DecryptManager::DecryptUpdate(char *& plaintext,int32_t& plaintext_len,const char* ciphertext,int ciphertext_len){
    plaintext = new char[ciphertext_len + AES_BLOCK];


    if(!EVP_DecryptUpdate(this->ctx, (unsigned char*)plaintext, &plaintext_len, (unsigned char*)ciphertext, ciphertext_len)){
    	perror("Error in EVP_DecryptUpdate");
    	return false;
    }

    return true;
}


bool DecryptManager::DecryptFinal(char* plaintext,int32_t &plaintext_len){
	int len;
	
	if(!EVP_DecryptFinal(ctx, (unsigned char*)plaintext + plaintext_len, &len)){
		perror("Error in EVP_DecryptFinal");
		return false;
	}
  	
  	plaintext_len += len;
	//EVP_CIPHER_CTX_free(ctx);
	return true;
}


bool DecryptManager::DecryptFinal(Chunk &c){


	return DecryptFinal(c.getPlainText(),c.size);
}

DecryptManager::~DecryptManager(){
	EVP_CIPHER_CTX_free(ctx);
}
