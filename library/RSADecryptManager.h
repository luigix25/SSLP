#ifndef RSADECRYPT_MNG

#define RSADECRYPT_MNG
#include "library.h"

class RSADecryptManager{
	private:
		EVP_CIPHER_CTX* mdctx;
		EVP_PKEY *privkey;
		unsigned char* iv;
		unsigned char* key;

	public:
		RSADecryptManager(const char *,uint32_t);							//KEY
		~RSADecryptManager();
		bool RSAUpdate(chunk&,encryptedChunk&);
		bool RSAFinal(chunk&);

};

#endif