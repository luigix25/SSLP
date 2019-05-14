#ifndef ENCRYPT

#define ENCRYPT
#include "library.h"

class EncryptManager{
	private:
		unsigned char key[16];
		unsigned char IV[16];
		EVP_CIPHER_CTX *ctx;

	public:
		EncryptManager(const char *, const char *);				//KEY AND IV
		~EncryptManager();
		bool EncyptUpdate(encryptedChunk&, chunk&);
		bool EncyptFinal(encryptedChunk&);

};

#endif