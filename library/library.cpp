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

bool NetSocket::utilitySend(const char* buffer,uint32_t len){

	uint32_t inviati = 0;
	int32_t ret =  0;

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

bool NetSocket::utilityRecv(char* buffer,uint32_t len){
	uint32_t ricevuti = 0;
	int ret = 0;

	while(ricevuti < len){ 										//posso leggere meno byte
		ret = recvfrom(this->socket,buffer+ricevuti,len-ricevuti,0,NULL,0);
		if(ret == -1){
			perror("[Error] recv");
			return false;	
		}
		ricevuti += ret;
	}

	return true;
}


bool NetSocket::sendData(const char *buffer,uint32_t len){

	return utilitySend(buffer,len);


}


char* NetSocket::recvData(uint32_t len){

	char *buffer = new char[len];
	if(buffer == NULL)
		return NULL;

	if(utilityRecv(buffer,len))
		return buffer;
	else
		return NULL;
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



char* serialization(char* plaintext, char* hmac, int size){

	char* serialized = new char [size + HASH_SIZE];
	memcpy(&serialized[0],plaintext,size);
	memcpy(&serialized[size],hmac,HASH_SIZE);

	delete[] plaintext;
	delete[] hmac;
	return serialized;
}

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

bool sendIntHMAC(NetSocket& sender_socket,int32_t value){

	if(!sender_socket.sendInt(value))
		return false;

	HMACManager hm(KEY_HMAC);

	chunk c;
	c.plaintext = (char*)&value;
	c.size = sizeof(uint32_t);
	hm.HMACUpdate(c);

	char *digest = hm.HMACFinal(LOCAL_INT_NONCE);
	if(digest == NULL) return false;

	bool return_value;

	return_value = sender_socket.sendData(digest,HASH_SIZE);

	delete[] digest;

	return return_value;

}


bool recvIntHMAC(NetSocket& receiver_socket,int32_t& value){

	if(!receiver_socket.recvInt(value))
		return false;

	char *received_digest = receiver_socket.recvData(HASH_SIZE);
	if(received_digest == NULL)
		return false;


	HMACManager hm(KEY_HMAC);
	chunk c;
	c.size = sizeof(uint32_t);
	c.plaintext = (char*)&value;

	hm.HMACUpdate(c);
	char *digest = hm.HMACFinal(REMOTE_INT_NONCE);

	if(digest == NULL){
		return false;
	}

	bool result;

	if(memcmp(received_digest,digest,HASH_SIZE)){
		cout<<"HASH DIVERSI"<<endl;
		cout<<"Ricevuto:"<<endl;
		//close connection
		result = false;
	} else {
		result = true;
	}

	delete[] digest;
	delete[] received_digest;
	return result;

}

bool sendDataHMAC(NetSocket& sender_socket,const char *data,int32_t length){

	if(!sendIntHMAC(sender_socket,length+HASH_SIZE))
		return false;

	if(!sender_socket.sendData(data,length)){
		return false;
	}

	HMACManager hmac(KEY_HMAC);
	chunk c;
	c.size = length;
	c.plaintext = (char*)data;

	if(!hmac.HMACUpdate(c)) return false;

	char *digest = hmac.HMACFinal(LOCAL_NONCE);

	bool ret = sender_socket.sendData(digest,HASH_SIZE);

	delete[] digest;
	return ret;

}

char* recvDataHMAC(NetSocket& receiver_socket,int32_t &length){

	if(!recvIntHMAC(receiver_socket,length))
		return false;

	length -= HASH_SIZE;

	char *data = receiver_socket.recvData(length);

	chunk c;
	c.size = length;
	c.plaintext = data;

	HMACManager hmac(KEY_HMAC);
	if(!hmac.HMACUpdate(c)) return NULL;

	char *digest = hmac.HMACFinal(REMOTE_NONCE);
	if(digest == NULL) return NULL;

	char *received_digest = receiver_socket.recvData(HASH_SIZE);

	if(received_digest == NULL){
		delete[] digest;
		return NULL;
	}

	if(memcmp(received_digest,digest,HASH_SIZE)){
		cout<<"HASH DIVERSI"<<endl;
		delete[] digest;
		delete[] received_digest;
		return NULL;
	}

	delete[] digest;
	delete[] received_digest;
	return data;


}