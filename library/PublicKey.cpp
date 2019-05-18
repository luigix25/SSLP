#include "PublicKey.h"

PublicKey::PublicKey(EVP_PKEY *pkey){
	this->key = pkey;
}

PublicKey::PublicKey(){
	this->key = NULL;
}

void PublicKey::destroyKey(){
	if(this->key != NULL){
		EVP_PKEY_free(this->key);
		this->key = NULL;
	}
}

PublicKey::~PublicKey(){
	if(this->key != NULL)
		EVP_PKEY_free(this->key);
}