#ifndef KEY_M
#define KEY_M

#include "library.h"

class KeyManager {
	protected:
		static unsigned char key[16];
		static unsigned char IV[16];
		static unsigned char hmac_key[16];

	public:
		static void setAESKey(const char*);
		static void setAESIV(const char*);
		static void setHMACKey(const char*);
		static void getIV(char*);
		static void incrementIV();
		static void destroyKeys();

};

#endif