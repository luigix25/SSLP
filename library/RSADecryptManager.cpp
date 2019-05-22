#include "RSADecryptManager.h"

RSADecryptManager::RSADecryptManager(const char *privkey_path,uint32_t key_len){
  
  this->mdctx = NULL;

  this->mdctx = EVP_CIPHER_CTX_new();
  //size_t key_hmac_size = sizeof(KEY_HMAC);
  
  FILE* privkey_file = fopen(privkey_path, "r");
  if(!privkey_file){ 
    cerr << "Error: cannot open file '" << privkey_path << "' (missing?)\n"; 
    exit(1); 
  }

  this->privkey = PEM_read_PrivateKey(privkey_file, NULL, NULL, NULL);
  fclose(privkey_file);

  if(!this->privkey){ 
    cerr << "Error: PEM_read_PUBKEY returned NULL\n"; 
    exit(1); 
  }

  this->key = new unsigned char[key_len];
  this->iv = new unsigned char[EVP_CIPHER_iv_length(EVP_aes_128_cbc())];

  if(!EVP_OpenInit(this->mdctx,EVP_aes_128_cbc(),key,key_len,iv,this->privkey)){
      perror("Error In RSA_Init_ex");
  }

}

bool RSADecryptManager::RSAUpdate(Chunk &c, EncryptedChunk &ec){

    if(!EVP_OpenUpdate(this->mdctx, (unsigned char*)ec.getCipherText(), &ec.size, (unsigned char*)c.getPlainText(), c.size)){
      perror("Error in RSA_Update");
      return false;
    }

    return true;

}


bool RSADecryptManager::RSAFinal(Chunk& c){

  int len;

  if(!EVP_OpenFinal(this->mdctx, (unsigned char*)c.getPlainText() + c.size, &len)){
    perror("Error in RSA_Final");
    return false;
  }

  c.size += len;
  return true;
}

RSADecryptManager::~RSADecryptManager(){

  EVP_CIPHER_CTX_free(this->mdctx);
  
}

