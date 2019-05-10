#ifndef RSAV_MNG

#define RSAV_MNG
#include "library.h"

class RSAVerifyManager{
	private:
		EVP_MD_CTX* mdctx;
		EVP_PKEY *pubkey;

	public:
		RSAVerifyManager(const char *);							//KEY
		bool RSAUpdate(encryptedChunk &);
		bool RSAUpdate(chunk &);

		int RSAFinal(char *signature);

};

#endif