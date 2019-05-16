#include "DHManager.h"


DHManager::DHManager(const char *path){

	FILE* dh_params = fopen(path, "r");
	if(!dh_params){ cerr << "Error: cannot open file '" << path << "' (missing?)\n"; exit(1); }
	this->session = PEM_read_DHparams(dh_params, NULL, NULL, NULL);
	fclose(dh_params);
	if(!this->session){ cerr << "Error: PEM_read_DHparams returned NULL\n"; exit(1); }

	int res;

	if(DH_check(this->session,&res) != 1){
		cout<<"Errore"<<endl;
	} 

	if(res != 0){
		cerr<<"Error in DH params"<<endl;
		exit(-1);
	}

}

char * DHManager::generatePublicKey(int &len){
	if(DH_generate_key(this->session) != 1){
		cerr<<"Error in DH_generate_key"<<endl;
		return NULL;
	}

	BIGNUM *pub_key;
	DH_get0_key(this->session,(const BIGNUM**)&pub_key,NULL);

	unsigned char *return_value = new unsigned char[BN_num_bytes(pub_key)];


	len = BN_bn2bin(pub_key,return_value);
	if(len <= 0){
		delete[] return_value;
		return NULL;
	}

	return (char*)return_value;

}

char * DHManager::computeSimmetricKey(const char *received_data,uint32_t len,int &key_len){

	BIGNUM *opponent_pubkey = BN_bin2bn((const unsigned char*)received_data,len,NULL);
	unsigned char* key = new unsigned char[DH_size(this->session)];
	key_len = DH_compute_key(key,opponent_pubkey,this->session);

	if(key_len <=0)
		return NULL;

	return (char*)key;

}
