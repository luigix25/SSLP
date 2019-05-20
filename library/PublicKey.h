#ifndef PUBKEY
#define PUBKEY
#include "library.h"

class PublicKey{
	private:
		EVP_PKEY *key;

	public:
		PublicKey();
		PublicKey(EVP_PKEY *);
		~PublicKey();

		void setKey(EVP_PKEY *);
		EVP_PKEY* getKey();

		void destroyKey();

};

#endif