#include "server.h"


fd_set master;
NetSocket client_socket;

chunk test;
encryptedChunk encryptedtest;

void cmd_list(){
	vector <string> files;
	files = get_file_list("server/database/");
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

/*	//SECURE CODING

	int len;

	char *filename;

	filename = client_socket.recvData(len);
	if(filename == NULL){
		return;
	}

	cout<<"Client vuole leggere il file: "<<filename<<endl;

	string full_name = "server/database/";
	full_name += filename;
	free(filename); 							//non mi serve più

	ReadFileManager fm(full_name);

	uint32_t size = (uint32_t)fm.size_file();			//fix
	cout<<"File size: "<<size<<endl;	

	if(!client_socket.sendInt(size)) return;

	if(size == 0){				//file non esistente
		return;
	}

	chunk c;
	file_status status;
	bool last = false;

	EncryptManager em(KEY_AES,AES_IV);
	HMACManager hmac(KEY_HMAC);

	int conta = 0;


 	char* digest;

	while(true){
	
		status = fm.read(&c);

		if(status == FILE_ERROR){
			cout<<"FILE ERROR"<<endl;
			return;
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

		if(!client_socket.sendData(ec.ciphertext,ec.size)){
			cout<<"ERRORE SEND"<<endl;
			return;
		} 
		
		free(c.plaintext);
		free(ec.ciphertext);
		c.size = 0;

	//	conta++;

		if(last || conta == 2)
			break;

	}

	if(!client_socket.sendData((const char*)digest,HASH_SIZE)) {
			cout<<"ERRORE SEND"<<endl;
			return;
	}

	free(digest);
*/
	char *filename;
	int len;

	filename = client_socket.recvData(len);
	string path("server/database/");
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
	string path("server/database/");
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
