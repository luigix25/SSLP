#ifndef RSAS_MNG

#define RSAS_MNG
#include "library.h"

class RSASignManager{
	private:
		EVP_MD_CTX* mdctx;
		EVP_PKEY *prvkey;

	public:
		RSASignManager(const char *);							//KEY
		bool RSAUpdate(encryptedChunk &);
		bool RSAUpdate(chunk &);

		char* RSAFinal();

};

#endif