#ifndef RSA_CERT

#define RSA_CERT
#include "library.h"

class CertificateManager{
	private:
		X509_STORE* store;
		
	public:
		CertificateManager(const char *,const char*);							//CACert and CRL
		~CertificateManager();

		bool verifyCertificate(X509*);
		char* extractCommonName(X509*);
		EVP_PKEY* extractPubKey(X509*);


};

#endif