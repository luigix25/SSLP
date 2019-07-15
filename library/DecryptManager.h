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
		bool DecryptUpdate(Chunk &,EncryptedChunk&);
		bool DecryptUpdate(char *&, int32_t&, const char*, int32_t);

		bool DecryptFinal(Chunk &);
		bool DecryptFinal(char*, int32_t &);

};

#endif