#include "RSAVerifyManager.h"

RSAVerifyManager::RSAVerifyManager(const char *key){
	
	this->mdctx = EVP_MD_CTX_new();
	//size_t key_hmac_size = sizeof(KEY_HMAC);


  	if(!EVP_VerifyInit(mdctx,EVP_sha256())){
  		perror("Error In RSA_Init_ex");
  	}

}

bool RSAVerifyManager::RSAUpdate(encryptedChunk &ec){

    if(!EVP_VerifyUpdate(mdctx, (unsigned char*) ec.ciphertext,ec.size)){
    	perror("Error in RSA_VerifyUpdate");
    	return false;
    }

    return true;

}

bool RSAVerifyManager::RSAUpdate(chunk &ec){

    if(!EVP_VerifyUpdate(mdctx, (unsigned char*) ec.plaintext,ec.size)){
      perror("Error in RSA_VerifyUpdate");
    	return false;
    }

    return true;

}

char* RSAVerifyManager::RSAFinal(){
	//int hash_size = EVP_MD_size(EVP_sha256());
	char *signature = new char [EVP_PKEY_size(this->prvkey)];
  unsigned int signature_len;

	if(!EVP_VerifyFinal(mdctx,(unsigned char*)signature, signature_len, this->prvkey)){
		perror("Error in RSA_Final");
		return NULL;
	}

  //distruggere chiave privata

	EVP_MD_CTX_free(mdctx);
	return signature;
}

