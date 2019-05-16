#ifndef DH_MNG

#define DH_MNG
#include "library.h"

class DHManager{
	private:
		DH *session;
	public:
		DHManager(const char *);							//DH Param Path
		char * generatePublicKey(int &);
		char * computeSimmetricKey(const char *,uint32_t,int &);

		//~DHManager();

		//char* HMACFinal();

};

#endif