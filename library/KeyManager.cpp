#include "KeyManager.h"

unsigned char KeyManager::key[16];
unsigned char KeyManager::IV[16];
unsigned char KeyManager::hmac_key[16];

void KeyManager::setAESKey(const char* _key){
	memcpy(key,_key,16);
}

void KeyManager::setAESIV(const char* _iv){
	memcpy(IV,_iv,16);
}

void KeyManager::setHMACKey(const char* _hmac){
	memcpy(hmac_key,_hmac,16);
}

void KeyManager::destroyKeys(){
	memset_s(key,0,16);
	memset_s(hmac_key,0,16);

}
