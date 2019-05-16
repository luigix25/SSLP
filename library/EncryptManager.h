#ifndef ENCRYPT

#define ENCRYPT
#include "library.h"
#include "KeyManager.h"

class EncryptManager : public KeyManager{
	private:
		//unsigned char key[16];
		//unsigned char IV[16];
		EVP_CIPHER_CTX *ctx;


	public:
		EncryptManager();				//KEY AND IV
		EncryptManager(const char *, const char *);				//KEY AND IV

		~EncryptManager();
		bool EncryptUpdate(encryptedChunk&, chunk&);
		bool EncryptFinal(encryptedChunk&);



};

#endif