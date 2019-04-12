#ifndef MAC_MNG

#define MAC_MNG
#include "library.h"

class HMACManager{
	private:
		unsigned char key[16];
		HMAC_CTX* mdctx;

	public:
		HMACManager(const char *);							//KEY
		bool HMACUpdate(encryptedChunk &);
		bool HMACUpdate(chunk &);

		char* HMACFinal();

};

#endif