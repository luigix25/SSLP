#ifndef RSAS_MNG

#define RSAS_MNG
#include "library.h"

class RSASignManager{
	private:
		EVP_MD_CTX* mdctx;
		EVP_PKEY *prvkey;
		string privkeyPath;

	public:
		RSASignManager(const char *);							//KEY
		~RSASignManager();
		bool RSAUpdate(EncryptedChunk &);
		bool RSAUpdate(Chunk &);
		bool RSAUpdate(const char*,int32_t);


		char* RSAFinal(uint32_t&);

};

#endif