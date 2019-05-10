#include "library.h"
#include "HMACManager.h"

const char *commands_list[5] = {"!help","!list","!get","!upload","!quit"};


NetSocket::NetSocket(int socket){
	this->socket = socket;
}

NetSocket::NetSocket(){
	this->socket = -1;

}

void NetSocket::setSocket(int socket){
	this->socket = socket;
}


void NetSocket::closeConnection(){
	close(this->socket);
}

bool NetSocket::sendInt(int value,bool hmac){
	bool result =  sendInt(value);
	if(!result)
		return false;

	int status;

	if(hmac){
		HMACManager hm(KEY_HMAC);

		chunk c;
		c.plaintext = (char*)&value;
		c.size = sizeof(uint32_t);
		hm.HMACUpdate(c);

		char *digest = hm.HMACFinal(LOCAL_NONCE);
		status = sendto(this->socket,digest,HASH_SIZE,0,NULL,0);
		result = (status == HASH_SIZE);

		delete[] digest;

	}
	return result;
}


bool NetSocket::sendInt(int value){
	int status;
	uint32_t tosend;

	tosend = htonl(value);
	//status = send(sd, &tosend, sizeof(uint32_t), 0);
	status = sendto(this->socket, &tosend, sizeof(int32_t), 0,NULL,0);
	if(status < (int)sizeof(int32_t))	{
		perror("[Error] send");
		return false;
	}

	return (status==sizeof(uint32_t));

}

bool NetSocket::sendDataHMAC(const char *buffer,int32_t len){
	return wrapperSendData(buffer,len,true);
}

bool NetSocket::wrapperSendData(const char *buffer,int32_t len,bool hmac){

	int ret;

	if(!sendInt(len,hmac)){
		cout << "sendInt di sendData fallita" << endl;
		return false;
	}
	
	int inviati = 0;

	while(inviati < len){

		ret = sendto(this->socket,buffer+inviati,len-inviati,0,NULL,0);
		if(ret == -1){
			perror("[Error] send");
			return false;
		} 
		inviati += ret;
	}
	
	return (inviati == len);

}


bool NetSocket::sendData(const char *buffer,int32_t len){
	return wrapperSendData(buffer,len,false);

}

char* NetSocket::wrapperRecvData(int32_t &len,bool hmac){
	int ret;

	if(!recvInt(len,hmac)){
		perror("[Error] recv");
		return NULL;
	}
	char *buffer = new char [len];

	int ricevuti = 0;

	while(ricevuti < len){ 										//posso leggere meno byte
		ret = recvfrom(this->socket,buffer+ricevuti,len-ricevuti,0,NULL,0);
		if(ret == -1){
			perror("[Error] recv");
			return NULL;	
		}
		ricevuti += ret;
	}

	return buffer;
}

char* NetSocket::recvDataHMAC(int32_t &len){
	return wrapperRecvData(len,true);
}

char* NetSocket::recvData(int32_t &len){
	return wrapperRecvData(len,false);
}

bool NetSocket::recvInt(int &val, bool hmac){
	bool result = recvInt(val);
	if(!result){
		return false;
	}
	//I expect HASH_SIZE bytes of hmac
	int32_t ret;


	if(hmac){
		char *data = new char[HASH_SIZE];
		ret = recvfrom(this->socket,data,HASH_SIZE,0,NULL,0);
		if(ret != HASH_SIZE){
			delete[] data;
			return false;
		}

		HMACManager hm(KEY_HMAC);
		chunk c;
		c.size = sizeof(uint32_t);
		c.plaintext = (char*)&val;

		hm.HMACUpdate(c);
		char *digest = hm.HMACFinal(REMOTE_NONCE);

		if(digest == NULL){
			return false;
		}

		if(memcmp(data,digest,HASH_SIZE)){
			cout<<"HASH DIVERSI"<<endl;
			cout<<"Ricevuto:"<<endl;
			BIO_dump_fp (stdout, (const char *)data, HASH_SIZE);
			cout<<endl<<"Calcolato:"<<endl;

			BIO_dump_fp (stdout, (const char *)digest, HASH_SIZE);
			cout<<endl;
			//close connection
			result = false;
		} else {
			cout<<"HASH SENDINT OK"<<endl;
			result = true;
		}

		delete[] digest;
		delete[] data;

	}

	return result;

}

bool NetSocket::recvInt(int &val){

	int ret,tmp;

	ret = recvfrom(this->socket,&tmp,sizeof(uint32_t),0,NULL,0);
	if(ret < (int)sizeof(uint32_t)){
		perror("[Error] recv");
		return 0;
	}

	val = ntohl(tmp);

	return (ret == (int)sizeof(uint32_t));

}


/*char* serialization(char* plaintext, char* hmac, int size){

	char* serialized = new char * [sizeof(int) + size + HASH_SIZE];
	//cout << "stampo size: " << size << endl;
	*(int* )serialized = size;
	//cout << "in serialization stampo cosa ci metto in serialized come size: " << ((int*)serialized)[0] << endl;
	memcpy(&serialized[4],plaintext,size);
	memcpy(&serialized[4 + size],hmac,HASH_SIZE);

	return serialized;
}*/

char* serialization(char* plaintext, char* hmac, int size){

	char* serialized = new char [size + HASH_SIZE];
	//cout << "stampo size: " << size << endl;
	//*(int* )serialized = size;
	//cout << "in serialization stampo cosa ci metto in serialized come size: " << ((int*)serialized)[0] << endl;
	memcpy(&serialized[0],plaintext,size);
	memcpy(&serialized[size],hmac,HASH_SIZE);

	delete[] plaintext;
	delete[] hmac;
	return serialized;
}

/*void unserialization(char* serialized, int serialized_len, chunk &c, char* hmac){
	BIO_dump_fp (stdout, (const char *)serialized, serialized_len);

	c.size = *(int* )serialized;
	cout << "stampo serialized size: " << serialized_len << endl;
	cout << "stampo c.size in unserialization: " << c.size << endl;

	c.plaintext= new char *[c.size];
	memcpy(c.plaintext,&serialized[4],c.size);
	cout << "prima memcpy in unserialization fatta " << endl;


	memcpy(hmac,&serialized[4 + c.size],HASH_SIZE);
	cout << "seconda memcpy in unserialization fatta " << endl;

	delete[] serialized;
}*/

void unserialization(char* serialized, int serialized_len, encryptedChunk &ec, char* hmac){
	//BIO_dump_fp (stdout, (const char *)serialized, serialized_len);

	ec.size =  serialized_len - HASH_SIZE; // *(int* )serialized;

//	cout << "stampo serialized size: " << serialized_len << endl;
//	cout << "stampo c.size in unserialization: " << ec.size << endl;

	ec.ciphertext = new char [ec.size];
	memcpy(ec.ciphertext,&serialized[0],ec.size);
//	cout << "prima memcpy in unserialization fatta " << endl;

	memcpy(hmac,&serialized[ec.size],HASH_SIZE);
//	cout << "seconda memcpy in unserialization fatta " << endl;

	delete[] serialized;
}

vector<string> get_file_list(const char* path){

	DIR* folder = opendir(path);
	struct dirent* dp;
	vector <string> result;

	while( (dp = readdir(folder)) != NULL){						//WARNING SICUREZZA
		char *filename = dp->d_name;
		if(filename[0] == '.')
			continue;
		result.push_back(filename);
	}

	closedir(folder);
	return result;

}

vector<string> split (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}