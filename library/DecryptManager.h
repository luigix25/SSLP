#ifndef DECRYPT

#define DECRYPT
#include "library.h"

class DecryptManager{
	private:
		unsigned char key[16];
		unsigned char IV[16];
		EVP_CIPHER_CTX *ctx;

	public:
		DecryptManager(const char *, const char *);				//KEY AND IV
		bool DecyptUpdate(chunk &,encryptedChunk&);
		bool DecyptFinal(chunk &);

};

#endif