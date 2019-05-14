#include "server.h"


fd_set master;
NetSocket client_socket;
int server_socket;
chunk test;
encryptedChunk encryptedtest;


void close_handler(int s){
	cout<<endl<<"Terminating.."<<endl;
	client_socket.closeConnection();
	close(server_socket);
	exit(s);

}

void cmd_list(){
	vector <string> files;
	files = get_file_list(SERVER_PATH);
	int number;

	number = (int)files.size();
/*
	if(!client_socket.sendInt(number))
		return;
*/

	string concatenated = files[0];

	for(uint16_t i =1;i<number;i++){
		concatenated += " "+files[i];
	}

	const char *str = concatenated.c_str();
	int len = concatenated.size()+1;

	EncryptManager em(KEY_AES,AES_IV);
	chunk c;
	c.plaintext = (char*)str;
	c.size = len;

	encryptedChunk ec;
	ec.ciphertext = new char[c.size+AES_BLOCK];

	em.EncryptUpdate(ec,c);
	em.EncryptFinal(ec);


	//delete[] c.plaintext;

	//cout << "lista che invio da server: \n" << ec.ciphertext<<endl;
	if(!sendDataHMAC(client_socket,ec.ciphertext,ec.size))
		return;
	
	delete[] ec.ciphertext;


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

	filename = recvDataHMAC(client_socket,len);
	string path(SERVER_PATH);
	cout<<path<<endl;
	if(!SendFile(path,client_socket,filename,SERVER_PRIVKEY_PATH))
		cout << "sendFile fallita" << endl;
	else
		cout << "sendFile corretta" << endl;

	delete[] filename ;
}

void cmd_upload(){
	char *filename;
	int len;

	filename = recvDataHMAC(client_socket,len);
	string path(SERVER_PATH);
	if(!ReceiveFile(path,filename,client_socket,CLIENT_PUBKEY_PATH)){
		cout << "cmd_upload fallita" << endl;
	}
	else
		cout << "cmd_upload corretta" << endl;

	delete[] filename;
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

bool receive_command(int &command,const char *key_aes/*, const char* key_hmac*/){

	int len;
	char *raw_data = recvDataHMAC(client_socket,len);

	if(raw_data == NULL){
		cout<<"Errore receive command"<<endl;
		return false;
	}

	encryptedChunk ec;
	ec.size = len;
	ec.ciphertext = raw_data;

	//char* recvd_hmac = new char[HASH_SIZE];
	//unserialization(raw_data,len,ec,recvd_hmac);

	chunk c;
	c.plaintext = new char[ec.size+AES_BLOCK];

	DecryptManager dm(key_aes,AES_IV);
	dm.DecryptUpdate(c,ec);
	dm.DecryptFinal(c);

	//il plaintext è un numero

	int *p = (int*)c.plaintext;
	command = *p;

	delete[] c.plaintext;			//non mi serve più
	delete[] ec.ciphertext;

	return true;


}


bool initial_protocol(NetSocket &client_socket){

	int cert_size;

	if(!client_socket.recvInt(cert_size)) 		return false;

	if(cert_size < 0)
		return false;

	char *client_cert_data = client_socket.recvData(cert_size);
	if(client_cert_data == NULL)
		return false;

	char *old_ptr = client_cert_data;

	X509 *client_cert = d2i_X509(NULL,(const unsigned char **)&client_cert_data,cert_size);
	delete[] old_ptr;

	if(!client_cert)
		return false;

	CertificateManager cm(CERT_CA_PATH,CERT_CA_CRL_PATH);
	if(!cm.verifyCertificate(client_cert)){
		X509_free(client_cert);
		return false;
	}

	char *name = cm.extractCommonName(client_cert);

	cout<<"Connessione stabilita con il client "<<name<<endl;

	delete[] name;

	X509_free(client_cert);

	//read my certificate from file
	FILE* cacert_file = fopen(CERT_SERVER_PATH, "r");
	if(!cacert_file){ cerr << "Error: cannot open file '" << CERT_SERVER_PATH << "' (missing?)\n"; exit(1); }
	X509* server_cert = PEM_read_X509(cacert_file, NULL, NULL, NULL);
	fclose(cacert_file);
	if(!server_cert){ cerr << "Error: PEM_read_X509 returned NULL\n"; exit(1); }

	//serialize it

	unsigned char* cert_buf= NULL; 
	cert_size = i2d_X509(server_cert, &cert_buf); 
	if(cert_size< 0) { /* handle error */ } 
	
	//send through the socket

	if(!client_socket.sendInt(cert_size)) 			return false;					//chiedere perazzo
	if(!client_socket.sendData((const char*)cert_buf,cert_size)) return false;

	OPENSSL_free(cert_buf);
	X509_free(server_cert);



	HMACManager::setRemoteNonce(CLIENT_NONCE);
	HMACManager::setLocalNonce(SERVER_NONCE);

	return true;

}

int main(int argc,char **argv){


	if(argc != 2){
		cout<<"[Errore] parametri errati"<<endl;
		exit(-1);
	}

	signal(SIGPIPE, SIG_IGN);					//ignoro sigpipe
	signal (SIGINT,close_handler);				//gestisco i ctrl-c


	struct sockaddr_in clientAddress;

	int port = atoi(argv[1]);
	int fdmax;
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
	FD_SET(server_socket,&master);
	fdmax = server_socket;

	client_socket = NetSocket();

	bool alreadyConnected = false;


	while(true){
		read_fds = master;
		if(select(fdmax+1,&read_fds,NULL,NULL,NULL) <=0){
			perror("[Errore] Select");
			exit(-1);
		}

		for(i = 0; i <= fdmax; i++){
			if(FD_ISSET(i,&read_fds)){	
				if(i == server_socket){ 						//new connection
					memset(&clientAddress,0,sizeof(clientAddress));
					addrlen = sizeof(clientAddress);
					new_sock = accept(server_socket,(struct sockaddr*)&clientAddress,&addrlen);
					
					if(new_sock < 0){
						perror("[Errore] accept\n");
						continue;
					}

					if(alreadyConnected){					
						close(new_sock);						//refuse new connections
						continue;
					}

					FD_SET(new_sock,&master);
					if(new_sock > fdmax) 
						fdmax = new_sock;
					
					client_socket.setSocket(new_sock);
					alreadyConnected = true;

					initial_protocol(client_socket);

					continue;


				} else {

					status = receive_command(cmd,KEY_AES/*,KEY_HMAC*/);
					if(!status){
						cout<<"Client Disconnesso"<<endl;
						client_socket.closeConnection();
						alreadyConnected = false;
						FD_CLR(i,&master);							//remove socket from select

					} else {
						select_command(cmd);
					}

				}
			}

		}	

	}
	
	//close(server_socket);											//no more clients allowed

	return 0;

}
