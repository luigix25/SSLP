#ifndef PUBKEY
#define PUBKEY
#include "library.h"

class PublicKey{
	public:
		PublicKey();
		PublicKey(EVP_PKEY *);
		~PublicKey();

		void destroyKey();
		EVP_PKEY *key;

};

#endif