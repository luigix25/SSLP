#include "CertificateManager.h"

CertificateManager::CertificateManager(const char *ca_cert_path, const char *ca_crl_path){

	int ret;
	
	//Load ca_cert
	FILE* cacert_file = fopen(ca_cert_path, "r");
	if(!cacert_file){ cerr << "Error: cannot open file '" << ca_cert_path << "' (missing?)\n"; exit(1); }
	X509* cacert = PEM_read_X509(cacert_file, NULL, NULL, NULL);
	fclose(cacert_file);
	if(!cacert){ cerr << "Error: PEM_read_X509 returned NULL\n"; exit(1); }

	//Load ca_crl
	FILE* crl_file = fopen(ca_crl_path, "r");
	if(!crl_file){ cerr << "Error: cannot open file '" << ca_crl_path << "' (missing?)\n"; exit(1); }
	X509_CRL* crl = PEM_read_X509_CRL(crl_file, NULL, NULL, NULL);
	fclose(crl_file);
	if(!crl){ cerr << "Error: PEM_read_X509_CRL returned NULL\n"; exit(1); }
	
	//Creating the store
	this->store = X509_STORE_new();
	if(!this->store) { cerr << "Error: X509_STORE_new returned NULL\n" << ERR_error_string(ERR_get_error(), NULL) << "\n"; exit(1); }
	ret = X509_STORE_add_cert(this->store, cacert);
	if(ret != 1) { cerr << "Error: X509_STORE_add_cert returned " << ret << "\n" << ERR_error_string(ERR_get_error(), NULL) << "\n"; exit(1); }
	ret = X509_STORE_add_crl(this->store, crl);
	if(ret != 1) { cerr << "Error: X509_STORE_add_crl returned " << ret << "\n" << ERR_error_string(ERR_get_error(), NULL) << "\n"; exit(1); }
	ret = X509_STORE_set_flags(this->store, X509_V_FLAG_CRL_CHECK);
	if(ret != 1) { cerr << "Error: X509_STORE_set_flags returned " << ret << "\n" << ERR_error_string(ERR_get_error(), NULL) << "\n"; exit(1); }

	X509_free(cacert);
	X509_CRL_free(crl);
}					

CertificateManager::~CertificateManager(){

	X509_STORE_free(this->store);				

}

bool CertificateManager::verifyCertificate(X509* certificate){

	if(!certificate)
		return false;

	int ret;

	X509_STORE_CTX* certvfy_ctx = X509_STORE_CTX_new();
	if(!certvfy_ctx) { cerr << "Error: X509_STORE_CTX_new returned NULL\n" << ERR_error_string(ERR_get_error(), NULL) << "\n"; exit(1); }
	ret = X509_STORE_CTX_init(certvfy_ctx, store, certificate, NULL);
	if(ret != 1) { cerr << "Error: X509_STORE_CTX_init returned " << ret << "\n" << ERR_error_string(ERR_get_error(), NULL) << "\n"; exit(1); }
	ret = X509_verify_cert(certvfy_ctx);
	
	X509_STORE_CTX_free(certvfy_ctx);

	if(ret != 1) { 
		cerr << "Error: X509_verify_cert returned " << ret << "\n" << ERR_error_string(ERR_get_error(), NULL) << endl; 
		return false;
	} else {
		return true;
	}


}

bool CertificateManager::extractCommonName(X509* certificate,string &string_name){

	if(!certificate)
		return false;

	X509_NAME* name = X509_get_subject_name(certificate);
	int len;
	
	len = X509_NAME_get_text_by_NID(name,NID_commonName,NULL,0);				//i get the length of the commonname
	if(len == -1)
		return false;

	len++;																		//terminator char

	char *tmp = new char[len];

	if(X509_NAME_get_text_by_NID(name,NID_commonName,tmp,len) == -1)
		return false;

	string_name = (string)tmp;
	delete[] tmp;

	return true;

}


EVP_PKEY * CertificateManager::extractPubKey(X509* certificate){

	return X509_get_pubkey(certificate);										//doesn't need to be freed

}