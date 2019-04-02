#include "library.h"

const char *commands_list[4] = {"!help","!who","!connect","!quit"};
const char *commands_list_game[4] = {"!help","!disconnect","!shot","!show"};

int sendInt(int sd,int value){
	int status;
	uint32_t tosend;

	tosend = htonl(value);
	//status = send(sd, &tosend, sizeof(uint32_t), 0);
	status = sendto(sd, &tosend, sizeof(uint32_t), 0,NULL,0);
	if(status < sizeof(uint32_t))	{
		perror("[Errore] send");
	}

	return (status==sizeof(uint32_t));

}

int sendData(int sd,char *buffer,int len){
	int ret;

	if(!sendInt(sd,len))
		return false;
	
	//ret = send(sd,buffer,len,0);
	ret = sendto(sd,buffer,len,0,NULL,0);
	if(ret < len){
		perror("[Errore] send");
	} 
	
	return (len == ret);

}


char* recvData(int sd, int &len){
	int ret;

	if(!recvInt(sd,&len)){
		perror("[Errore] recv");
		return NULL;
	}
	char *buffer = (char *)malloc(len);

	//ret = recv(sd,buffer,len,MSG_WAITALL);
	ret = recvfrom(sd,buffer,len,0,NULL,0);
	if(ret < len){
		perror("[Errore] recv");
		return NULL;	
	}
	return buffer;
}

int recvInt(int sd,int* val){

	int ret,tmp;

	ret = recvfrom(sd,&tmp,sizeof(uint32_t),0,NULL,0);
	if(ret < sizeof(uint32_t)){
		perror("[Errore] recv");
	}

	*val = ntohl(tmp);

	return (ret == sizeof(uint32_t));

}
