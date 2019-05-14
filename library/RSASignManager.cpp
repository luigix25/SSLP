#include "RSASignManager.h"

RSASignManager::RSASignManager(const char *key_path){
	
  this->mdctx = NULL;
  this->prvkey = NULL;

	this->mdctx = EVP_MD_CTX_new();
	//size_t key_hmac_size = sizeof(KEY_HMAC);
  this->privkeyPath = string(key_path);
  
  if(!EVP_SignInit(mdctx,EVP_sha256())){
  		perror("Error In RSA_Init_ex");
  }

}

bool RSASignManager::RSAUpdate(encryptedChunk &ec){

    if(!EVP_SignUpdate(mdctx, (unsigned char*) ec.ciphertext,ec.size)){
    	perror("Error in RSA_SignUpdate");
    	return false;
    }

    return true;

}

bool RSASignManager::RSAUpdate(chunk &ec){

    if(!EVP_SignUpdate(mdctx, (unsigned char*) ec.plaintext,ec.size)){
      perror("Error in RSA_SignUpdate");
    	return false;
    }

    return true;

}

char* RSASignManager::RSAFinal(uint32_t& len){

  FILE* prvkey_file = fopen(this->privkeyPath.c_str(), "r");
  if(!prvkey_file){ cerr << "Error: cannot open file '" << this->privkeyPath << "' (missing?)\n"; exit(1); }
  this->prvkey = PEM_read_PrivateKey(prvkey_file, NULL, NULL, NULL);
  fclose(prvkey_file);
  if(!this->prvkey){ cerr << "Error: PEM_read_PrivateKey returned NULL\n"; exit(1); }
  

	char *signature = new char [EVP_PKEY_size(this->prvkey)];
  unsigned int signature_len;


	if(!EVP_SignFinal(mdctx,(unsigned char*)signature, &signature_len, this->prvkey)){
		perror("Error in RSA_Final");
		return NULL;
	}

  len = signature_len;

  //distruggere chiave privata
  EVP_PKEY_free(this->prvkey);
  this->prvkey = NULL;

	return signature;
}

RSASignManager::~RSASignManager(){
  if(this->prvkey != NULL){
    EVP_PKEY_free(this->prvkey);
  }

  if(this->mdctx != NULL){
    EVP_MD_CTX_free(this->mdctx);
  }

  CRYPTO_cleanup_all_ex_data();
  
}

