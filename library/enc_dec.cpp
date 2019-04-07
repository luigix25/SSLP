#include "library.h"

/*
void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  abort();
}*/

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;

  int len;
  int ciphertext_len;

  // Create and initialise the context 
  ctx = EVP_CIPHER_CTX_new();

  // Encrypt init
  EVP_EncryptInit(ctx, EVP_aes_128_ecb(), key, iv);

  // Encrypt Update: one call is enough because our mesage is very short.
  EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
  ciphertext_len = len;

  //Encrypt Final. Finalize the encryption and adds the padding
  EVP_EncryptFinal(ctx, ciphertext + len, &len);
  ciphertext_len += len;

  // MUST ALWAYS BE CALLED!!!!!!!!!!
  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}

EVP_CIPHER_CTX* encrypt_INIT(unsigned char *key,unsigned char *iv){
  EVP_CIPHER_CTX *ctx;
  ctx = EVP_CIPHER_CTX_new();
  EVP_EncryptInit(ctx, EVP_aes_128_ecb(), key, iv);
  return ctx;

}

void encrypt_UPDATE(EVP_CIPHER_CTX* ctx, unsigned char *ciphertext, int &ciphertext_len, unsigned char *plaintext, int plaintext_len){
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &ciphertext_len, plaintext, plaintext_len)){
      ERR_print_errors_fp(stderr);
    }
   // ciphertext_len = len;
}

void encrypt_FINAL(EVP_CIPHER_CTX *ctx, unsigned char *ciphertext, int &ciphertext_len){
  int len;
   if(1 != EVP_EncryptFinal(ctx, ciphertext + ciphertext_len, &len)){
    ERR_print_errors_fp(stderr);
   }
  ciphertext_len += len;

  // MUST ALWAYS BE CALLED!!!!!!!!!!
  EVP_CIPHER_CTX_free(ctx);
}

EVP_CIPHER_CTX* decrypt_INIT(unsigned char *key,unsigned char *iv){
  EVP_CIPHER_CTX *ctx;
  ctx = EVP_CIPHER_CTX_new();
  EVP_DecryptInit(ctx, EVP_aes_128_ecb(), key, iv);
  return ctx;

}

void decrypt_UPDATE(EVP_CIPHER_CTX* ctx, unsigned char *ciphertext, int ciphertext_len, unsigned char *plaintext, int &plaintext_len){
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &plaintext_len, ciphertext, ciphertext_len))
    cout << "ERRORE EVP_DecryptUpdate" << endl;   // ciphertext_len = len;
}

void decrypt_FINAL(EVP_CIPHER_CTX *ctx, unsigned char *plaintext, int &plaintext_len){
  int len;
  if(1 != EVP_DecryptFinal(ctx, plaintext + plaintext_len, &len)){
    cout << "ERRORE EVP_DecryptFinal" << endl;
  }
  plaintext_len += len;

  // MUST ALWAYS BE CALLED!!!!!!!!!!
  EVP_CIPHER_CTX_free(ctx);
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,  unsigned char *iv, unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int plaintext_len;

  // Create and initialise the context 
  ctx = EVP_CIPHER_CTX_new();

  // Decrypt Init
  EVP_DecryptInit(ctx, EVP_aes_128_ecb(), key, iv);

  // Decrypt Update: one call is enough because our mesage is very short.
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)){
    cout << "ERRORE EVP_DecryptUpdate" << endl;
    ERR_print_errors_fp(stderr);
  }
    //handleErrors();
  plaintext_len = len;

  // Decryption Finalize
  if(1 != EVP_DecryptFinal(ctx, plaintext + len, &len)){
    cout << "DECRIPT ERRORE EVP_DecryptFinal" << endl;
    ERR_print_errors_fp(stderr);
  } //handleErrors();
  plaintext_len += len;

  // Clean the context!
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}






void digest_message(const char *message, int message_len,  char *digest, int *digest_len)
{
	EVP_MD_CTX *mdctx;

	mdctx = EVP_MD_CTX_new();

	EVP_DigestInit(mdctx, EVP_sha256());
printf("\nok\n");
	EVP_DigestUpdate(mdctx, (unsigned char*)message, message_len);
printf("\nok2\n");	
	
	//*digest = (unsigned char *)malloc(1000);
//printf("\nok3\n");
	EVP_DigestFinal(mdctx, (unsigned char*)digest, (unsigned int*)digest_len);
	
	// Redirect our digest to the terminal
	printf("Digest is:\n");
	BIO_dump_fp (stdout, (const char *)digest, *digest_len);

	EVP_MD_CTX_destroy(mdctx);
}



 char* computeHMAC(char* plaintext){
  //create key

  cout << "eseguo funzione HMAC" <<endl;
  size_t key_hmac_size = sizeof(KEY_HMAC);
  //declaring the hash function we want to use
  const EVP_MD* md = EVP_sha256();
  int hash_size; //size of the digest
  hash_size = EVP_MD_size(md);
  //create a buffer for our digest
  //unsigned char* hash_buf; //buffer containing digest
  unsigned char* digest = (unsigned char*)malloc(hash_size); 
  //create message digest context
  HMAC_CTX* mdctx;
  mdctx = HMAC_CTX_new();
  //Init,Update,Finalise digest
  // << "eseguo INIT" <<endl;
  HMAC_Init_ex(mdctx, KEY_HMAC, key_hmac_size, md, NULL);
  //cout << "eseguo UPDATE" <<endl;

  HMAC_Update(mdctx, (unsigned char*) plaintext, sizeof(plaintext));
  //cout << "eseguo FINAL" <<endl;
 
  HMAC_Final
  (mdctx, digest, (unsigned int*) &hash_size);

  //cout << "stampo hash_size: " << hash_size << endl;

  //BIO_dump_fp (stdout, (const char *)digest, hash_size);

  //Delete context
  //cout << "eseguo FREE" <<endl;

  HMAC_CTX_free(mdctx);

  //cout << "ho eseguito la FREE" << endl;

  return (char*)digest;
}
  

void encryptChunk(chunk &c, encryptedChunk &ec){

	//int digest_len;
	//digest_message(c.plaintext, c.size,digest,&digest_len);
  cout << "eseguo memcpy in encryptChunk" << endl;
  /*ec.size = c.size;
  ec.ciphertext = (char *)malloc(c.size);
  memcpy(ec.ciphertext,c.plaintext,c.size);
  cout << "eseguo computeHMAC" << endl;*/

  char* hmac = (char*)computeHMAC(c.plaintext);
  //BIO_dump_fp (stdout, (const char *)hmac, 32);

  char* serialized = serialization((char*)c.plaintext,hmac,c.size);
  int serialized_len = sizeof(int) + c.size + HASH_SIZE;

  char* ciphertext = (char* )malloc(serialized_len + 16); 
  int cipher_len = encrypt((unsigned char*) serialized,serialized_len,(unsigned char *)KEY_AES,NULL,(unsigned char*)ciphertext);
  cout << cipher_len <<endl;
  //BIO_dump_fp (stdout, (const char *)ciphertext, cipher_len);

  ec.ciphertext = (char* ) malloc(cipher_len);
  ec.size = cipher_len;

  memcpy(ec.ciphertext,ciphertext,cipher_len);

  free(serialized);
  free(ciphertext);
  free(hmac);

}

void decryptChunk(encryptedChunk &ec, chunk &c){
  char* chunkplaintext = (char*)malloc(ec.size);
  int chunkplaintext_len = decrypt((unsigned char*)ec.ciphertext, ec.size,(unsigned char *)KEY_AES,NULL,(unsigned char*)chunkplaintext);
  char* hmac = (char *)malloc(HASH_SIZE);
  unserialization(chunkplaintext,chunkplaintext_len, c,hmac);
  cout <<"stampo plaintext in decryptChunk: " << c.plaintext << endl;
  char* myHMAC = computeHMAC(c.plaintext);
  if(!memcmp(myHMAC,hmac,HASH_SIZE))
    cout << "Le HASH corrispondono" << endl;

  free(hmac);
  free(myHMAC);

}