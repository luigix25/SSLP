#include "library.h"
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
	status = sendto(this->socket, &tosend, sizeof(uint32_t), 0,NULL,0);
	if(status < (int)sizeof(uint32_t))	{
		perror("[Error] send");
		return 0;
	}

	return (status==sizeof(uint32_t));

}

bool NetSocket::sendData(const char *buffer,int len){
	int ret;

	if(!sendInt(len))
		return false;
	
	ret = sendto(this->socket,buffer,len,0,NULL,0);
	if(ret < len){
		perror("[Error] send");
		return 0;
	} 
	
	return (len == ret);

}


char* NetSocket::recvData(int &len){
	int ret;

	if(!recvInt(len)){
		perror("[Error] recv");
		return NULL;
	}
	char *buffer = (char *)malloc(len);

	//ret = recv(sd,buffer,len,MSG_WAITALL);
	ret = recvfrom(this->socket,buffer,len,0,NULL,0);
	if(ret < len){
		perror("[Error] recv");
		return NULL;	
	}
	return buffer;
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

	char* serialized = ( char *)malloc(sizeof(int) + size + HASH_SIZE);
	*(int* )serialized = size;
	memcpy(&serialized[4],plaintext,size);
	memcpy(&serialized[4 + size],hmac,HASH_SIZE);
	return (char* )serialized;
}

void unserialization(char* serialized, int serialized_len, chunk &c, char* hmac){

	c.size = *(int* )serialized;
	memcpy(c.plaintext,&serialized[4],c.size);
	memcpy(hmac,&serialized[4 + c.size],HASH_SIZE);
	free(serialized);
}