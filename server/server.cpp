#include "../library/library.h"
#include "../library/ReadFileManager.h"				//library è qui dentro
#include "server.h"


fd_set master;
NetSocket client_socket;

chunk test;
encryptedChunk encryptedtest;



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

void cmd_list(){
	vector <string> files;
	files = get_file_list();
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

	//SECURE CODING

	int len;

	char *filename;

	filename = client_socket.recvData(len);
	if(filename == NULL){
		return;
	}

	cout<<"Client vuole leggere il file: "<<filename<<endl;

	string full_name = "server/database/";
	full_name += filename;

	ReadFileManager fm(full_name);
	free(filename); 							//non mi serve più

	uint32_t size = (uint32_t)fm.size_file();			//fix
	cout<<"File size: "<<size<<endl;	

	if(!client_socket.sendInt(size)) return;

	chunk c;
	file_status status;
	bool last = false;

	//creo contesto;
	EVP_CIPHER_CTX* ctx = encrypt_INIT((unsigned char*)KEY_AES,(unsigned char*)IV);
	int ciphertext_len = 0;


	while(true){
	
		status = fm.read(&c);

		if(status == FILE_ERROR){
			cout<<"FILE ERROR"<<endl;
			return;
		} else if(status == END_OF_FILE){
			cout<<"EOF"<<endl;
			last = true;
		}


		char *ciphertext = (char*)malloc(c.size+16);

	/*	int ciphertext_len = encrypt((unsigned char*)c.plaintext, c.size,(unsigned char*)KEY_AES, NULL, (unsigned char*)ciphertext);
		BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);

		print_hex((unsigned char*)ciphertext,ciphertext_len);

		cout<<ciphertext_len<<" "<<KEY_AES<<endl; 

		int pt = decrypt((unsigned char*)ciphertext, ciphertext_len,(unsigned char*)KEY_AES, (unsigned char*)IV, (unsigned char*)pt_dec);
		BIO_dump_fp (stdout, (const char *)c.plaintext, c.size);

*/
//		BIO_dump_fp (stdout, (const char *)c.plaintext, c.size);


		encrypt_UPDATE(ctx,(unsigned char*)ciphertext,ciphertext_len,(unsigned char*)c.plaintext,c.size);

		if(last){
			encrypt_FINAL(ctx,(unsigned char*)ciphertext, ciphertext_len);
		}

		BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);

		if(!client_socket.sendData(ciphertext,ciphertext_len)) return;
		free(c.plaintext);
		free(ciphertext);

		if(last)
			break;

	}



}



void select_command(int cmd){

	switch (cmd){
		case LIST_COMMAND:
			cmd_list();
			break;
		case GET_COMMAND:
			cmd_get();
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

void test_hash(){

	test.size = sizeof("cifra sto cazzo");
	test.plaintext = (char *)malloc(test.size);
	memcpy(test.plaintext,"cifra sto cazzo",test.size);
	cout << "stampo plaintext " << test.plaintext << endl;
	encryptChunk(test, encryptedtest);

	BIO_dump_fp(stdout,encryptedtest.ciphertext,encryptedtest.size);
	decryptChunk(encryptedtest, test);

	free(test.plaintext);
	free(encryptedtest.ciphertext);

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
