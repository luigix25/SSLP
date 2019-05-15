#include "RSAEncryptManager.h"

RSAEncryptManager::RSAEncryptManager(EVP_PKEY *pubkey){
  
  this->mdctx = NULL;

  this->mdctx = EVP_CIPHER_CTX_new();
  //size_t key_hmac_size = sizeof(KEY_HMAC);
  this->pubkey = pubkey;
  
  unsigned char* key = new unsigned char[EVP_PKEY_size(pubkey)];
  unsigned char *iv = new unsigned char[EVP_CIPHER_iv_length(EVP_aes_128_cbc())];

  int key_len;


  if(!EVP_SealInit(mdctx,EVP_aes_128_cbc(),&key,&key_len,iv,&pubkey,1)){
      perror("Error In RSA_Init_ex");
  }

}

bool RSAEncryptManager::RSAUpdate(encryptedChunk &ec,chunk& c){

    if(!EVP_SealUpdate(this->mdctx, (unsigned char*)ec.ciphertext, &ec.size, (unsigned char*)c.plaintext, c.size)){
      perror("Error in RSA_Update");
      return false;
    }

    return true;

}


bool RSAEncryptManager::RSAFinal(encryptedChunk& ec){

  int len;

  if(!EVP_SealFinal(this->mdctx, (unsigned char*)ec.ciphertext + ec.size, &len)){
    perror("Error in RSA_Final");
    return false;
  }

  ec.size += len;
  return true;
}

RSAEncryptManager::~RSAEncryptManager(){

  EVP_CIPHER_CTX_free(this->mdctx);
  
}

