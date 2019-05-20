#include "PublicKey.h"

PublicKey::PublicKey(EVP_PKEY *pkey){
	this->key = pkey;
}

PublicKey::PublicKey(){
	this->key = NULL;
}

void PublicKey::setKey(EVP_PKEY *_key){
	destroyKey();
	this->key = _key;
}

EVP_PKEY* PublicKey::getKey(){
	return this->key;
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