#include "client.h"

fd_set master;
int socket_tcp;
struct sockaddr_in opponent;

NetSocket server_socket;

void pulisci_buff(){
	char c;
	while((c = getchar()) != '\n' && c != EOF)
		;

}

void print_hex(unsigned char* buff, unsigned int size)
{
    printf("Printing %d bytes\n", size);
    for(unsigned int i=0; i<size; i++)
        printf("%02hx", *((unsigned char*)(buff + i)));
    printf("\n");
}

struct sockaddr_in setup_sockaddr(char *ip,int port){

	struct sockaddr_in sv_addr;

	memset(&sv_addr,0,sizeof(sv_addr));

	sv_addr.sin_family = AF_INET;
	sv_addr.sin_port = htons(port);
	inet_pton(AF_INET,ip,&sv_addr.sin_addr);
	return sv_addr;

}


void cmd_help(){

	const char *desc[5] = {"--> show available command",
			"--> show file saved on server and client",
			"filename --> download from server filename",
			"filename --> upload to server filename",
			"--> disconnect from server\n"};

	int i;
	cout<<endl;
	cout<<"Available command:"<<endl;
	for(i=0;i<5;i++){
		cout<<commands_list[i]<<" "<<desc[i]<<endl;
	}

}


void cmd_quit(){

	cout<<"Closing Connection"<<endl;
	server_socket.closeConnection();
	exit(0);

}

bool send_command(uint32_t command, const char *key_aes/*, const char* key_hmac*/){

	EncryptManager em(key_aes,AES_IV);

	chunk c;
	c.plaintext = (char*)&command;
	c.size = 4;

	encryptedChunk ec;
	ec.ciphertext = new char[c.size + AES_BLOCK];

	if(!em.EncyptUpdate(ec,c)) 	return false;
	if(!em.EncyptFinal(ec))		return false;

	if(!sendDataHMAC(server_socket,ec.ciphertext,ec.size)){			//aggiorno il nonce
		cout<<"ERRORE SEND"<<endl;
		return false;
	}

	delete[] ec.ciphertext; 

	return true;
}

void cmd_list(){

	if(!send_command(LIST_COMMAND,KEY_AES/*,KEY_HMAC*/)){
		cout<<"Error in send command"<<endl;
		return;
	}
	
	char *recvd;
	int len;
	recvd = recvDataHMAC(server_socket,len);
	if(recvd == NULL) return;

	encryptedChunk ec;
	ec.ciphertext = recvd;
	
	ec.size = len;

	chunk c;
	c.plaintext = new char[ec.size+AES_BLOCK];

	DecryptManager dm(KEY_AES,AES_IV);
	dm.DecryptUpdate(c,ec);
	dm.DecryptFinal(c);

	string concatenated(c.plaintext);
	delete[] recvd;
	delete[] c.plaintext;

	vector<string> files_list = split(concatenated,string(" "));

	cout<<"Files Available for downloading from Server:"<<endl;
	for(uint i= 0;i<files_list.size();i++){
		cout<<files_list[i]<<'\t';
	}
	cout<<endl;

	cout<<"Files Available for uploading to Server:"<<endl;

	vector<string> files_client = get_file_list(CLIENT_PATH);

	for(uint i= 0;i<files_client.size();i++){
		cout<<files_client[i]<<'\t';
	}
	cout << endl;

}

void cmd_upload(){
	string filename;
	cin >> filename;
	string path(CLIENT_PATH);
	fstream tmp(path + filename);
	if(!tmp.good()){
		cout << "il file non esiste" << endl;
		return;
	}
	else
		cout << "il file esiste" << endl;
	if(!send_command(UPLOAD_COMMAND,KEY_AES/*,KEY_HMAC*/)) return;

	if(!sendDataHMAC(server_socket,(const char*)filename.c_str(),filename.length()+1)) return;		//vanno cifrati

	
	if(!SendFile(path,server_socket,(char *)filename.c_str(),CLIENT_PRIVKEY_PATH))
		cout << "sendFile fallita" << endl;
	else
		cout << "sendFile corretta" << endl;
}


void cmd_get(){

	//if(!server_socket.sendInt(GET_COMMAND)) return;
	if(!send_command(GET_COMMAND,KEY_AES/*,KEY_HMAC*/)) return;
	string filename;
	cin >> filename;
	string path(CLIENT_PATH);

	int32_t length = filename.length()+1;

	if(!sendDataHMAC(server_socket,(const char*)filename.c_str(),length)) return;			//va cifrato
	if(!ReceiveFile(path,( char*)filename.c_str(),server_socket,SERVER_PUBKEY_PATH))
		cout << "ReceiveFile ERRATA" << endl;
	else
		cout << "ReceiveFile CORRETTA" << endl;
	
	//WARING SECURECODING
}

void select_command(string &buffer){


	if(buffer.compare("!help") == 0){
		cmd_help();
	} else if(buffer.compare("!list") == 0){
		cmd_list();
	} else if(buffer.compare("!quit") == 0){
		cmd_quit();
	} else if(buffer.compare("!get") == 0){
		cmd_get();
	} else if (buffer.compare("!upload") == 0){
		cmd_upload();
	} else {
		cout<<">"<<buffer<<"<"<<endl;
		cout<<"Comando non riconosciuto"<<endl;
	//	exit(-1);
		pulisci_buff();
	}


}

void read_input(){

	string buffer;
	fflush(stdout);
	cin>>buffer;

	select_command(buffer);

}

void protocol_error(int sock_tcp){

	close(sock_tcp);
	exit(-1);

}



bool initial_protocol(NetSocket &server_socket){

	//read my certificate from file
	FILE* cacert_file = fopen(CERT_CLIENT_PATH, "r");
	if(!cacert_file){ cerr << "Error: cannot open file '" << CERT_CLIENT_PATH << "' (missing?)\n"; exit(1); }
	X509* client_cert = PEM_read_X509(cacert_file, NULL, NULL, NULL);
	fclose(cacert_file);
	if(!client_cert){ cerr << "Error: PEM_read_X509 returned NULL\n"; exit(1); }

	//serialize it

	unsigned char* cert_buf= NULL; 
	int cert_size = i2d_X509(client_cert, &cert_buf); 
	if(cert_size< 0) { /* handle error */ } 
	
	//send through the socket

	if(!server_socket.sendInt(cert_size)) 			return false;					//chiedere perazzo
	if(!server_socket.sendData((const char*)cert_buf,cert_size)) return false;

	OPENSSL_free(cert_buf);
	X509_free(client_cert);

	//I receive the server's certificate

	if(!server_socket.recvInt(cert_size)) 		return false;

	if(cert_size < 0)
		return false;

	char *server_cert_data = server_socket.recvData(cert_size);
	if(server_cert_data == NULL)
		return false;

	char *old_ptr = server_cert_data;

	X509 *server_cert = d2i_X509(NULL,(const unsigned char **)&server_cert_data,cert_size);

	if(!server_cert)
		return false;

	CertificateManager cm(CERT_CA_PATH,CERT_CA_CRL_PATH);
	cm.verifyCertificate(server_cert);
	char *name = cm.extractCommonName(server_cert);

	cout<<"Connessione stabilita con il server "<<name<<endl;

	delete[] old_ptr;
	delete[] name;

	X509_free(server_cert);

	HMACManager::setLocalNonce(CLIENT_NONCE);
	HMACManager::setRemoteNonce(SERVER_NONCE);

	return true;

}

int main(int argc,char **argv){

	if(argc < 3){
		cout<<"[Errore] ip e porta necessari"<<endl;
		exit(-1);
	}

	signal(SIGPIPE, SIG_IGN);


	int status,portServer;
	struct sockaddr_in serverAddress;

	int i,fdmax;
	fd_set read_fds;

	portServer = atoi(argv[2]);

	socket_tcp = socket(AF_INET,SOCK_STREAM,0);
	if(socket_tcp < 0){
		perror("[Errore] socket\n");
		return -1;
	}

	memset(&serverAddress,0,sizeof(serverAddress));
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portServer);
	inet_pton(AF_INET,argv[1],&serverAddress.sin_addr);
	

	status = connect(socket_tcp, (struct sockaddr*)&serverAddress,sizeof(serverAddress));
	if(status < 0){
		perror("[Errore] connect\n");
		return -1;
	}
	
	cout<<endl<<"Connessione al server "<<argv[1]<<" (port "<<portServer<<" effettuata con successo"<<endl;
	server_socket = NetSocket(socket_tcp);

	if(!initial_protocol(server_socket)){
		//handle error
	}

	cmd_help();

	FD_ZERO(&master);	
	FD_ZERO(&read_fds);


	FD_SET(socket_tcp,&master);
	FD_SET(0,&master);		// 0 è stdin

	fdmax = socket_tcp;

	while(true){

	
		read_fds = master;
		if(select(fdmax+1,&read_fds,NULL,NULL,NULL) <=0){
			perror("Errore select");
			exit(-1);
		}


		for(i = 0; i <= fdmax; i++){
			if(FD_ISSET(i,&read_fds)){
				if(i == 0){			//stdin
					read_input();				//keyboard			
					//continue;
				} else if(i == socket_tcp) {			//server tcp
					/*if(!server_socket.recvInt(cmd)){
						cout<<"Connessione Persa"<<endl;
						return -1;
					}			
					select_command_server(i,cmd);*/
					//non dovrei mai arrivare qui	
					cout<<"ERRORE PROTOCOLLARE!"<<endl;
					server_socket.closeConnection();
					exit(-1);
				} 

			}

		}
		
	}

	
	return 0;

}
