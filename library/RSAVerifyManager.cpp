#include "RSAVerifyManager.h"

RSAVerifyManager::RSAVerifyManager(const char *pubkey_path){
	
	this->mdctx = EVP_MD_CTX_new();
  if(!this->mdctx){
    perror("ERRORE new");
  }

	//size_t key_hmac_size = sizeof(KEY_HMAC);
  FILE* pubkey_file = fopen(pubkey_path, "r");
  if(!pubkey_file){ cerr << "Error: cannot open file '" << pubkey_path << "' (missing?)\n"; exit(1); }
  this->pubkey = PEM_read_PUBKEY(pubkey_file, NULL, NULL, NULL);
  fclose(pubkey_file);
  if(!this->pubkey){ cerr << "Error: PEM_read_PUBKEY returned NULL\n"; exit(1); }


  	if(!EVP_VerifyInit(this->mdctx,EVP_sha256())){
  		perror("Error In RSA_Init_ex");
  	}

}

RSAVerifyManager::RSAVerifyManager(EVP_PKEY* public_key){
  
  this->mdctx = EVP_MD_CTX_new();
  if(!this->mdctx){
    perror("ERRORE new");
  }

  this->pubkey = public_key;
  if(!EVP_VerifyInit(this->mdctx,EVP_sha256())){
      perror("Error In RSA_Init_ex");
    }

}

bool RSAVerifyManager::RSAUpdate(encryptedChunk &ec){

    if(!EVP_VerifyUpdate(this->mdctx, (unsigned char*) ec.ciphertext,ec.size)){
    	perror("Error in RSA_VerifyUpdate");
    	return false;
    }

    return true;

}

bool RSAVerifyManager::RSAUpdate(const char* data,int32_t len){
   
  if(!EVP_VerifyUpdate(this->mdctx, (unsigned char*) data,len)){
    perror("Error in RSA_VerifyUpdate");
    return false;
  }

  return true;
}


bool RSAVerifyManager::RSAUpdate(chunk &ec){

    if(!EVP_VerifyUpdate(this->mdctx, (unsigned char*) ec.plaintext,ec.size)){
      perror("Error in RSA_VerifyUpdate");
    	return false;
    }

    return true;

}

int RSAVerifyManager::RSAFinal(char *signature){
	//int hash_size = EVP_MD_size(EVP_sha256());
  uint32_t len = EVP_PKEY_size(this->pubkey);

	//char *signature = new char [len];
  //unsigned int signature_len;
  int ret;

	ret = EVP_VerifyFinal(this->mdctx,(unsigned char*)signature, len, this->pubkey);

  //distruggere chiave pubblica
 // EVP_PKEY_free(this->pubkey);
	//EVP_MD_CTX_free(mdctx);
	return ret;
}

RSAVerifyManager::~RSAVerifyManager(){
  EVP_PKEY_free(this->pubkey);
  EVP_MD_CTX_free(mdctx);
  CRYPTO_cleanup_all_ex_data();
}

