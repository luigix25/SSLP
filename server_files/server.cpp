#include "server.h"


fd_set master;
NetSocket client_socket;

chunk test;
encryptedChunk encryptedtest;

void cmd_list(){
	vector <string> files;
	files = get_file_list(SERVER_PATH);
	int number;

	number = (int)files.size();

	if(!client_socket.sendInt(number))
		return;

	for(uint16_t i =0;i<number;i++){
		const char *str = files[i].c_str();
		int len = strlen(str)+1;

		//cout<<"Invio: "<<str<<endl;

		if(!client_socket.sendData(str,len))
			return;
	}


}

void print_hex(unsigned char* buff, unsigned int size)
{
    printf("Printing %d bytes\n", size);
    for(unsigned int i=0; i<size; i++)
        printf("%02hx", *((unsigned char*)(buff + i)));
    printf("\n");
}

void cmd_get(){

	char *filename;
	int len;

	filename = client_socket.recvData(len);
	string path(SERVER_PATH);
	cout<<path<<endl;
	if(!SendFile(path,client_socket,filename))
		cout << "sendFile fallita" << endl;
	else
		cout << "sendFile corretta" << endl;

	free(filename);
}

void cmd_upload(){
	char *filename;
	int len;

	filename = client_socket.recvData(len);
	string path(SERVER_PATH);
	if(!ReceiveFile(path,filename,client_socket))
		cout << "cmd_upload fallita" << endl;
	else
		cout << "cmd_upload corretta" << endl;

	free(filename);
}



void select_command(int cmd){

	switch (cmd){
		case LIST_COMMAND:
			cmd_list();
			break;
		case GET_COMMAND:
			cmd_get();
			break;
		case UPLOAD_COMMAND:
			cmd_upload();
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
		cout<<"[Errore] socket"<<endl;
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
		cout<<"[Errore] parametri errati"<<endl;
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

	cout<<"[LOG] Attendo connessioni sulla porta "<<port<<endl;

	
	FD_ZERO(&master);	
	FD_ZERO(&read_fds);
	//FD_SET(server_socket,&master);

	memset(&clientAddress,0,sizeof(clientAddress));
	addrlen = sizeof(clientAddress);
	new_sock = accept(server_socket,(struct sockaddr*)&clientAddress,&addrlen);
	if(new_sock < 0){
		perror("[Errore] accept\n");
		return -1;
	}

	FD_SET(new_sock,&master);
	//fdmax = server_socket;

	//if(new_sock > fdmax) 
	fdmax = new_sock;

	client_socket = NetSocket(new_sock);
	cout<<"Connessione stabilita con il client"<<endl;
	close(server_socket);											//no more clients allowed

	//test_hash();
	
	while(true){
		read_fds = master;
		if(select(fdmax+1,&read_fds,NULL,NULL,NULL) <=0){
			perror("[Errore] Select");
			exit(-1);
		}

		for(i = 0; i <= fdmax; i++){
			if(FD_ISSET(i,&read_fds)){			
				/*if(i==server_socket){			//qualcuno si vuole connettere
					cout<<"NO"<<endl;
										
					//continue;

				} else {	*/			//qualcuno vuole scrivere
					status = client_socket.recvInt(cmd);
					if(!status){
						cout<<"Client Disconnesso"<<endl;
						client_socket.closeConnection();
						return -1;
					}
					select_command(cmd);

					//printf("%d\n",cmd);

					
				//}
			}

		}	

	}
	
	return 0;

}
