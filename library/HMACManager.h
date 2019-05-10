#ifndef MAC_MNG

#define MAC_MNG
#include "library.h"

class HMACManager{
	private:
		unsigned char key[16];
		HMAC_CTX* mdctx;
		static uint32_t nonce[4];
	public:
		HMACManager(const char *);							//KEY
		bool HMACUpdate(encryptedChunk &);
		bool HMACUpdate(chunk &);
		char* HMACFinal(enum_nonce);
		//char* HMACFinal();

		static bool setLocalNonce(uint32_t);
		static bool setRemoteNonce(uint32_t);

		//static uint32_t getLocalNonce();
		//static uint32_t getRemoteNonce();

		/*static uint32_t local_nonce;
		static uint32_t remote_nonce;*/

};

#endif