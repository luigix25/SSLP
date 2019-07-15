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

		bool EncryptUpdate(EncryptedChunk&, Chunk&);
		bool EncryptUpdate(char *&,int32_t&,const char*,int32_t);

		bool EncryptFinal(EncryptedChunk&);
		bool EncryptFinal(char*,int32_t&);



};

#endif