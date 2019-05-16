#ifndef DECRYPT

#define DECRYPT
#include "library.h"
#include "KeyManager.h"

class DecryptManager : public KeyManager{
	private:
		EVP_CIPHER_CTX *ctx;

	public:
		DecryptManager();
		DecryptManager(const char *, const char *);				//KEY AND IV
		~DecryptManager();
		bool DecryptUpdate(chunk &,encryptedChunk&);
		bool DecryptFinal(chunk &);

};

#endif