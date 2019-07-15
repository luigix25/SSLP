#ifndef RSAV_MNG

#define RSAV_MNG
#include "library.h"
#include "PublicKey.h"

class RSAVerifyManager{
	private:
		EVP_MD_CTX* mdctx;
		EVP_PKEY *pubkey;
		bool shouldFree;

	public:
		RSAVerifyManager(const char *);							//KEY
		RSAVerifyManager(PublicKey &);

		~RSAVerifyManager();

		bool RSAUpdate(EncryptedChunk &);
		bool RSAUpdate(Chunk &);
		bool RSAUpdate(const char*,int32_t);

		int RSAFinal(char *signature);

};

#endif