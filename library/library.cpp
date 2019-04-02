#include "library.h"
const char *commands_list[5] = {"!help","!list","!get","!upload","!quit"};

int sendInt(int sd,int value){
	int status;
	uint32_t tosend;

	tosend = htonl(value);
	//status = send(sd, &tosend, sizeof(uint32_t), 0);
	status = sendto(sd, &tosend, sizeof(uint32_t), 0,NULL,0);
	if(status < (int)sizeof(uint32_t))	{
		perror("[Error] send");
		return 0;
	}

	return (status==sizeof(uint32_t));

}

int sendData(int sd,const char *buffer,int len){
	int ret;

	if(!sendInt(sd,len))
		return false;
	
	ret = sendto(sd,buffer,len,0,NULL,0);
	if(ret < len){
		perror("[Error] send");
		return 0;
	} 
	
	return (len == ret);

}


char* recvData(int sd, int &len){
	int ret;

	if(!recvInt(sd,&len)){
		perror("[Error] recv");
		return NULL;
	}
	char *buffer = (char *)malloc(len);

	//ret = recv(sd,buffer,len,MSG_WAITALL);
	ret = recvfrom(sd,buffer,len,0,NULL,0);
	if(ret < len){
		perror("[Error] recv");
		return NULL;	
	}
	return buffer;
}

int recvInt(int sd,int* val){

	int ret,tmp;

	ret = recvfrom(sd,&tmp,sizeof(uint32_t),0,NULL,0);
	if(ret < (int)sizeof(uint32_t)){
		perror("[Error] recv");
		return 0;
	}

	*val = ntohl(tmp);

	return (ret == (int)sizeof(uint32_t));

}
