#ifndef MAC_MNG

#define MAC_MNG
#include "library.h"
#include "KeyManager.h"

class HMACManager : public KeyManager{
	private:
		HMAC_CTX* mdctx;
		static uint32_t nonce[2];
	public:
		HMACManager();							
		HMACManager(const char *);							//KEY
		~HMACManager();

		bool HMACUpdate(EncryptedChunk &);
		bool HMACUpdate(Chunk &);
		bool HMACUpdate(const char *,int);

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