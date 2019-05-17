#ifndef RSAV_MNG

#define RSAV_MNG
#include "library.h"

class RSAVerifyManager{
	private:
		EVP_MD_CTX* mdctx;
		EVP_PKEY *pubkey;

	public:
		RSAVerifyManager(const char *);							//KEY
		RSAVerifyManager(EVP_PKEY*);

		~RSAVerifyManager();

		bool RSAUpdate(encryptedChunk &);
		bool RSAUpdate(chunk &);
		bool RSAUpdate(const char*,int32_t);

		int RSAFinal(char *signature);

};

#endif