#include "SendReceiveFile.h"


bool checkValidityFilename(string filename){
	if(filename.find('/') != string::npos){
		cout << "filename not correct " << endl;
		return false;
	}
	else{
		cout << "filename correct " << endl;
		return true;
	}
}
bool SendFile(string& path,NetSocket& receiverSocket,string &filename,const char *key_path,bool progressBar){

	if(filename.size() == 0){
		return false;
	}

	uint32_t size;

	bool return_value = checkValidityFilename(filename);
	if(!return_value){
		size = 0;
		sendIntHMAC(receiverSocket,size);
		return false;	
	}

	path+= filename;
	ReadFileManager fm(path);
	uint64_t size_64 = fm.size_file();			//32 bit ok
	cout<<"File size: "<<size_64<<endl;	


	if(size_64 > UINT32_MAX){					//check for overflow
		//Bigger than 4GiBs
		cout<<"File bigger than 4GiBs"<<endl;
		size = 0;
	} else {
		size = (uint32_t)size_64;
	}

	if(!sendIntHMAC(receiverSocket,size)) return false;		//32 bit ok

	if(size == 0){				//file non esistente
		return true;
	}

	//For the progressbar
	struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    float currentPercentage = 0;
    uint32_t sentBytes = 0;
    float percentageStep = 100.0f / w.ws_col;
    float lastPercentage = 0.0f;
    uint32_t completeFileSize;

	completeFileSize = size;


	Chunk c;
	file_status status;
	bool last = false;

	EncryptManager em;

 	char* digest;
 
	RSASignManager sign(key_path);

	while(true){
	
		status = fm.read(c);

		if(status == FILE_ERROR){
			cout<<"[ERROR]File"<<endl;
			return false;
		} else if(status == END_OF_FILE){
			cout<<"EOF"<<endl;
			last = true;
		}


		EncryptedChunk ec;

		if(!em.EncryptUpdate(ec,c)){
			//cout<<"HANDLE ERROR"<<endl;
			return false;
		}	


		if(last){
			if(!em.EncryptFinal(ec)){
				return false;
			}


		}

		if(!sign.RSAUpdate(ec)){						//hash on plaintext
			return false;
		}


		if(!sendDataHMAC(receiverSocket,ec.getCipherText(),ec.size)){			//aggiorno il nonce
			cout<<"[ERROR]"<<endl;
			return false;
		}

		sentBytes += c.size;
		c.size = 0;

		currentPercentage = (double)sentBytes/(double)completeFileSize * 100;

		if(progressBar){
			while(lastPercentage + percentageStep < currentPercentage){
				cout << "\u25A0"<<flush;
				lastPercentage+=percentageStep;
			}

			/*int col,row;
			cout << "\033[" << col << ";" << row << "H";*/

			if(currentPercentage  >= 100){
				cout << endl;
			}
		}
		
		//delete[] ec.ciphertext;


		if(last)
			break;

	}

	uint32_t sign_len;

	digest = sign.RSAFinal(sign_len);

	if(digest == NULL){
		cout<<"[ERROR] SignFinal"<<endl;
		return false;
	}	

	if(!sendDataHMAC(receiverSocket,digest,sign_len)){		
		cout<<"[ERROR]"<<endl;
		delete[] digest;
		return false;
	}

	delete[] digest;

	return true;
}

bool ReceiveFile(string & path, string& filename, NetSocket & senderSocket,PublicKey &key,bool progressBar){

	//handle get
	if(filename.size() == 0)
		return false;


	path += filename;
	cout<<"[LOG] File Path: "<<path<<endl;

	uint32_t file_size;
	uint32_t completeFileSize;
	if(!recvIntHMAC(senderSocket,(int32_t&)file_size)) return false;

	if(file_size == 0){				//file non esistente
		cout<<"[LOG] File does not exist or too big"<<endl;
		return true;
	}

	completeFileSize = file_size;

	char *recvd_data;
	int len = 0;
	file_status status;

	cout<<"[LOG] File Size: "<<file_size<<endl;
	WriteFileManager fm(path,file_size);
	DecryptManager dm;
	RSAVerifyManager verify(key);


	struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    float currentPercentage = 0;
    uint32_t receivedBytes = 0;
    float percentageStep = 100.0f / w.ws_col;
    float lastPercentage = 0.0f;

    while(true){

		recvd_data = recvDataHMAC(senderSocket,len);
		
		if(recvd_data == NULL){
			cout << "recvd_data NULL" <<endl;
			fm.finalize(true);
			return false;
		} 

		EncryptedChunk ec;
		ec.size  = len;
		ec.setCipherText(recvd_data);

		Chunk c;

		if(!dm.DecryptUpdate(c,ec)){
			cout<<"DecryptUpdate error"<<endl;
			fm.finalize(true);
			return false;		
		}

		file_size-= (c.size);


		if(file_size < AES_BLOCK){			//ultimo chunk

			if(!dm.DecryptFinal(c)){
				cout<<"DecryptFinal error"<<endl;
				fm.finalize(true);
				return false;
			}

		}

		receivedBytes += c.size;

		currentPercentage = (double)receivedBytes/(double)completeFileSize * 100;

		if(progressBar){

			while(lastPercentage + percentageStep < currentPercentage){
				cout << "\u25A0"<<flush;
				lastPercentage+=percentageStep;
			}

			/*int col,row;
			cout << "\033[" << col << ";" << row << "H";*/

			if(currentPercentage  >= 100){
				cout << endl;
			}
		}

		if(!verify.RSAUpdate(ec)){
			cout<<"[ERROR]"<<endl;
			fm.finalize(true);


			return false;
		}	

		status = fm.write(c);

		if(status == END_OF_FILE){
			cout<<"[LOG] Transfer Completed"<<endl;
			break;
		} else if(status == FILE_ERROR){
			cout << "FILE_ERROR" << endl;
			fm.finalize(true);
			return false;
		}
	}

	len = 0;
	char *recvd_digest = recvDataHMAC(senderSocket,len);	

	int result;
	result = verify.RSAFinal(recvd_digest);

	if(result != 1){
		fm.finalize(true);						//error
		cout<<"[ERROR] File Verification"<<endl;
		delete[] recvd_digest;
		return false;
	} else {
		cout<<"[LOG] File Verification OK"<<endl;
		fm.finalize(false);
	}
	
	delete[] recvd_digest;

	return true;

}