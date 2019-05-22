#ifndef RSAENCRYPT_MNG

#define RSAENCRYPT_MNG
#include "library.h"

class RSAEncryptManager{
	private:
		EVP_CIPHER_CTX* mdctx;
		EVP_PKEY *pubkey;
		string pubkeyPath;

	public:
		RSAEncryptManager(EVP_PKEY *);							//KEY
		~RSAEncryptManager();
		bool RSAUpdate(EncryptedChunk &,Chunk&);
		bool RSAFinal(EncryptedChunk&);

};

#endif