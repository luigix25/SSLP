#include "HashManager.h"

HashManager::HashManager(){

	this->mdctx = EVP_MD_CTX_new();


	if(!EVP_DigestInit(this->mdctx,EVP_sha256())){
		perror("Error In EVP_DigestInit");
	}

}


bool HashManager::HashUpdate(char* plaintext,int len){

    if(!EVP_DigestUpdate(mdctx, (unsigned char*) plaintext,len)){
    	perror("Error in EVP_DigestUpdate");
    	return false;
    }

    return true;

}

char* HashManager::HashFinal(){
  unsigned int hash_size = EVP_MD_size(EVP_sha256());
  char *digest = new char [HASH_SIZE];

  if(!EVP_DigestFinal(mdctx, (unsigned char*) digest,&hash_size)){
    perror("Error in EVP_DigestFinal");
    return NULL;
  }

  return digest;

}


HashManager::~HashManager(){

  EVP_MD_CTX_free(mdctx);

}