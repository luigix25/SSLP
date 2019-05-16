#ifndef HAS_MNG

#define HAS_MNG
#include "library.h"

class HashManager{
	private:
		EVP_MD_CTX* mdctx;
	public:
		HashManager();							//KEY
		~HashManager();

		bool HashUpdate(char*,int);
		char* HashFinal();

};

#endif