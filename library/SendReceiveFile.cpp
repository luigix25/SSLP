#include "SendReceiveFile.h"

bool SendFile(string& path,NetSocket& receiverSocket,const char* filename,const char *key_path){
	cout<<"USO CHIAVE "<<key_path<<endl;


	if(filename == NULL){
		return false;
	}
	
	path+= filename;
	ReadFileManager fm(path);
	uint64_t size_64 = fm.size_file();			//32 bit ok
	cout<<"File size: "<<size_64<<endl;	

	if(size_64 >= UINT32_MAX){
		//Bigger than 4GiBs
		cout<<"File bigger than 4GiBs"<<endl;
		return false;
	}

	uint32_t size = (uint32_t)size_64;

	//check for overflow!!!


	if(!receiverSocket.sendInt(size,true)) return false;		//32 bit ok

	if(size == 0){				//file non esistente
		return false;
	}

	chunk c;
	file_status status;
	bool last = false;

	EncryptManager em(KEY_AES,AES_IV);

	int conta = 0;
 	char* digest;
 
	//HMACManager full_hmac(KEY_HMAC);
	RSASignManager sign(key_path);


	while(true){
	
		status = fm.read(&c);

		if(status == FILE_ERROR){
			cout<<"FILE ERROR"<<endl;
			return false;
		} else if(status == END_OF_FILE){
			cout<<"EOF"<<endl;
			last = true;
		}

		char *ciphertext = new char[c.size + 16];

		encryptedChunk ec;
		ec.ciphertext = ciphertext;

		if(!em.EncyptUpdate(ec,c)){
			cout<<"HANDLE ERROR"<<endl;
		}	

		if(!sign.RSAUpdate(c)){						//hash on plaintext
			cout<<"ERORR"<<endl;
		}

		if(last){
			if(!em.EncyptFinal(ec)){
				//handle
			}
		}


		HMACManager hmac(KEY_HMAC);


		if(!hmac.HMACUpdate(ec)){
			cout<<"ERROR"<<endl;
		}	


		digest = hmac.HMACFinal(LOCAL_NONCE);
		if(digest == NULL){
				cout << "digest NULL" << endl;				//HANDLE ERROR
		}

		char* msg_serialized = serialization(ec.ciphertext, digest, ec.size);

		//cout<<"INVIO: "<<ec.size + HASH_SIZE<<endl;

		if(!receiverSocket.sendDataHMAC(msg_serialized,ec.size + HASH_SIZE)){			//aggiorno il nonce
			cout<<"ERRORE SEND"<<endl;
			return false;
		} 
		
		delete[] c.plaintext;
		//delete[] ec.ciphertext;
		delete[] msg_serialized;
		//delete[] digest;
		c.size = 0;

	//	conta++;

		if(last || conta == 2)
			break;

	}

	uint32_t sign_len;

	digest = sign.RSAFinal(sign_len);
	if(digest == NULL){
		cout<<"ERRORE SignFinal"<<endl;
		return false;
	}	

	if(!receiverSocket.sendDataHMAC(digest,sign_len)){			//valutare nonce
		cout<<"ERRORE SEND"<<endl;
		delete[] digest;
		return false;
	}

	delete[] digest;

	return true;
}

bool ReceiveFile(string & path, const char* filename, NetSocket & senderSocket,const char *key_path){

	cout<<"USO CHIAVE "<<key_path<<endl;

	//handle get
	path += filename;
	cout<<"Scrivo: "<<path<<endl;

	uint32_t file_size;
	if(!senderSocket.recvInt((int32_t&)file_size,true)) return false;

	if(file_size == 0){				//file non esistente
		cout<<"File does not exist"<<endl;
		return false;
	}

	char *recvd_data,*digest;
	int len = 0;
	file_status status;

	cout<<"File Size: "<<file_size<<endl;
	WriteFileManager fm(path,file_size);
	DecryptManager dm(KEY_AES,AES_IV);
	//HMACManager full_hmac(KEY_HMAC);
	RSAVerifyManager verify(key_path);

	while(true){

		recvd_data = senderSocket.recvDataHMAC(len);
		
		//cout<<"Ricevuti "<<len<<endl;

		if(recvd_data == NULL){
			cout << "recvd_data NULL" <<endl;
			return false;
		} 

		encryptedChunk ec;

		char* recvd_hmac = new char[HASH_SIZE];
		unserialization(recvd_data,len,ec,recvd_hmac);

		HMACManager hmac(KEY_HMAC);


		if(!hmac.HMACUpdate(ec)){
			cout<<"ERROR"<<endl;
		}	

		digest = hmac.HMACFinal(REMOTE_NONCE);
			if(digest == NULL){
				cout << "digest NULL" << endl;
		}

		if(memcmp(digest,recvd_hmac,HASH_SIZE)){
			cout << "HASH DIVERSI" << endl;
			//invio stop comunicazione
			delete[] ec.ciphertext;
			delete[] recvd_hmac;
			delete[] digest;

			return false;
		}

		delete[] digest;

		char *plaintext = new char[ec.size + AES_BLOCK];

		chunk c;
		c.plaintext = plaintext;

		if(!dm.DecryptUpdate(c,ec)){
			//handle error
		}

		file_size-= (c.size);

		if(file_size < AES_BLOCK){			//ultimo chunk

			dm.DecryptFinal(c);
		}

		if(!verify.RSAUpdate(c)){
			cout<<"ERROR"<<endl;
		}	

		status = fm.write(&c);

		delete[] recvd_hmac;
		delete[] c.plaintext;
		delete[] ec.ciphertext;

		if(status == END_OF_FILE){
			fm.finalize();
			cout<<"FINITO"<<endl;
			break;
		} else if(status == FILE_ERROR){
			cout << "FILE_ERROR" << endl;
			return false;
		}
	}

	len = 0;
	char *recvd_digest = senderSocket.recvDataHMAC(len);	

	int result;
	result = verify.RSAFinal(recvd_digest);

	if(result != 1){
		cout<<"ERRORE verifyFinal"<<endl;
		delete[] recvd_digest;
		return false;
	} else {
		cout<<"verify OK"<<endl;
	}
	
	delete[] recvd_digest;
	//delete[] digest;

	return true;

}