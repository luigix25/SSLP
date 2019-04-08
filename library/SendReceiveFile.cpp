#include "SendReceiveFile.h"

bool SendFile(string& path,NetSocket& receiverSocket){
	int len;

	char *filename;

	filename = receiverSocket.recvData(len);
	if(filename == NULL){
		return false;
	}

	//cout<<"Client vuole leggere il file: "<<filename<<endl;

	path+= filename;
	free(filename); 							//non mi serve più

	ReadFileManager fm(path);

	uint32_t size = (uint32_t)fm.size_file();			//fix
	cout<<"File size: "<<size<<endl;	

	if(!receiverSocket.sendInt(size)) return false;

	if(size == 0){				//file non esistente
		return false;
	}

	chunk c;
	file_status status;
	bool last = false;

	EncryptManager em(KEY_AES,AES_IV);
	HMACManager hmac(KEY_HMAC);

	int conta = 0;

	/*HMAC_CTX* mdctx;
	mdctx = HMAC_CTX_new();
	size_t key_hmac_size = sizeof(KEY_HMAC);

	//HMAC_Init_ex(mdctx, KEY_HMAC, key_hmac_size, EVP_sha256(), NULL);*/
 	char* digest;

	while(true){
	
		status = fm.read(&c);

		if(status == FILE_ERROR){
			cout<<"FILE ERROR"<<endl;
			return false;
		} else if(status == END_OF_FILE){
			cout<<"EOF"<<endl;
			last = true;
		}


		char *ciphertext = (char*)malloc(c.size + 16);

		//BIO_dump_fp(stdout,msg_serialized,c.size+32);
		//BIO_dump_fp(stdout,(const char*)c.plaintext,c.size);

		encryptedChunk ec;
		ec.ciphertext = ciphertext;

		if(!em.EncyptUpdate(ec,c)){
			cout<<"HANDLE ERROR"<<endl;
		}

		if(!hmac.HMACUpdate(c)){
			cout<<"ERROR"<<endl;
		}		


		if(last){
			if(!em.EncyptFinal(ec)){
				//handle
			}

			digest = hmac.HMACFinal();
			if(digest == NULL){
				//handle
			}

		}

		cout<<"INVIO: "<<ec.size<<endl;

		if(!receiverSocket.sendData(ec.ciphertext,ec.size)){
			cout<<"ERRORE SEND"<<endl;
			return false;
		} 
		
		free(c.plaintext);
		free(ec.ciphertext);
		c.size = 0;

	//	conta++;

		if(last || conta == 2)
			break;

	}

	if(!receiverSocket.sendData((const char*)digest,HASH_SIZE)) {
			cout<<"ERRORE SEND"<<endl;
			return false;
	}

	free(digest);
	return true;
}

bool ReceiveFile(string & path, string & filename, NetSocket & senderSocket){
if(!senderSocket.sendInt(GET_COMMAND)) return false;

	int32_t length = filename.length()+1;

	if(!senderSocket.sendData(filename.c_str(),length)) return false;

	//handle get
	path += filename;
	cout<<"Scrivo: "<<path<<endl;

	int32_t file_size;
	if(!senderSocket.recvInt(file_size)) return false;

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
	HMACManager hmac(KEY_HMAC);


	while(true){

		recvd_data = senderSocket.recvData(len);
		
		cout<<"Ricevuti "<<len<<endl;

		if(recvd_data == NULL){
			cout << "recvd_data NULL" <<endl;
			return false;
		} 


		char *plaintext = (char*)malloc(len + AES_BLOCK);

		chunk c;
		c.plaintext = plaintext;

		encryptedChunk ec;
		ec.size = len;
		ec.ciphertext = recvd_data;

		cout << "stampo ec.size: " << ec.size << endl;

		if(!dm.DecyptUpdate(c,ec)){
			//handle error
		}

		file_size-= (c.size);

		if(file_size < AES_BLOCK){			//ultimo chunk
		
			dm.DecyptFinal(c);
		}

		if(!hmac.HMACUpdate(c)){
			//handle
		}


		if(file_size < AES_BLOCK){			//ultimo chunk
			digest = hmac.HMACFinal();
			if(digest == NULL){
				//handle
			}
		}

		status = fm.write(&c);

		free(c.plaintext);
		free(ec.ciphertext);

		if(status == END_OF_FILE){
			cout<<"FINITO"<<endl;
			break;
		} else if(status == FILE_ERROR){
			cout << "FILE_ERROR" << endl;
			return false;
		}
	}		

	char *MAC_rcvd = senderSocket.recvData(len);
	
	if(!memcmp(digest,MAC_rcvd,32)){
		cout<<"MAC UGUALI"<<endl;
	} else{
		cout<<"MAC DIVERSI"<<endl;
	}

	free(digest);
	free(MAC_rcvd);
	return true;

}
