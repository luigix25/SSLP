#include "../library/library.h"
#include "server.h"
#include <iostream>
#include <vector>

using namespace std;

fd_set master;


vector<string> get_file_list(){

	DIR* folder = opendir("server/database");
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

void cmd_list(int socket){
	vector <string> files;
	files = get_file_list();
	int status,number;

	number = (int)files.size();

	if(!sendInt(socket,number))
		return;

	for(uint16_t i =0;i<number;i++){
		const char *str = files[i].c_str();
		int len = strlen(str)+1;

		//cout<<"Invio: "<<str<<endl;

		if(!sendData(socket,str,len))
			return;
	}


}


void select_command(int cmd,int socket){

	switch (cmd){
		case LIST_COMMAND:
			cmd_list(socket);
			break;
		default:
			//handle error
			break;
		}


}

int initialize_server(int port){
	
	int listener;
	int status;

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if(listener < 0){
		printf("[Errore] socket\n");
		exit(-1);
	}

	struct sockaddr_in listenerAddress;
	memset(&listenerAddress,0,sizeof(listenerAddress));
	
	listenerAddress.sin_port = htons(port);
	listenerAddress.sin_family = AF_INET;
	inet_pton(AF_INET,"0.0.0.0",&listenerAddress.sin_addr);


	status = bind(listener, (struct sockaddr*)& listenerAddress, sizeof(listenerAddress));
	if(status < 0){
		perror("[Errore] bind\n");
		exit(-1);
	}

	status = listen(listener,10);
	if(status < 0){
		perror("[Errore] listen\n");
		exit(-1);
	}


	return listener;

}

int main(int argc,char **argv){


	if(argc != 2){
		printf("[Errore] parametri errati\n");
		exit(-1);
	}


	struct sockaddr_in clientAddress;

	int port = atoi(argv[1]);
	int fdmax,server_socket;
	unsigned int addrlen;
	int i,status,new_sock;
	int cmd;
	fd_set read_fds;
		
	/*if(porta < 0){
		pritnf("[Errore] porta errata");
		exit(-1);
	}*/


	memset(&clientAddress,0,sizeof(clientAddress));

	server_socket = initialize_server(port);

	printf("[LOG] Attendo connessioni sulla porta %d\n",port);

	
	FD_ZERO(&master);	
	FD_ZERO(&read_fds);
	FD_SET(server_socket,&master);

	fdmax = server_socket;
	
	while(true){
		read_fds = master;
		if(select(fdmax+1,&read_fds,NULL,NULL,NULL) <=0){
			perror("[Errore] Select");
			exit(-1);
		}

		for(i = 0; i <= fdmax; i++){
			if(FD_ISSET(i,&read_fds)){			
				if(i==server_socket){			//qualcuno si vuole connettere

					memset(&clientAddress,0,sizeof(clientAddress));
					addrlen = sizeof(clientAddress);
					new_sock = accept(server_socket,(struct sockaddr*)&clientAddress,&addrlen);
					if(new_sock < 0){
						perror("[Errore] accept\n");
						continue;
					}					
					FD_SET(new_sock,&master);
					if(new_sock > fdmax) 
						fdmax = new_sock;
					
					printf("Connessione stabilita con il client\n");
					continue;

				} else {				//qualcuno vuole scrivere
					status = recvInt(i,&cmd);
					if(!status){
						printf("Client Disconnesso\n");
						close(server_socket);
						return -1;
					}
					select_command(cmd,i);

					//printf("%d\n",cmd);

					
				}
			}

		}	

	}
	
	close(server_socket);

	return 0;

}
