#include "client.h"

fd_set master;
int socket_tcp;
struct sockaddr_in opponent;
PublicKey public_key_rsa;

NetSocket server_socket;

void close_handler(int s){
	cout<<endl<<"Terminating.."<<endl;
	server_socket.closeConnection();
	exit(s);

}

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

	const char *desc[5] = {"--> show available commands",
			"--> show files saved both on server and client",
			"filename --> download from server filename",
			"filename --> upload to server filename",
			"--> disconnect from server\n"};

	int i;
	cout<<endl;
	cout<<"Available commands:"<<endl;
	for(i=0;i<5;i++){
		cout<<commands_list[i]<<" "<<desc[i]<<endl;
	}

}


void cmd_quit(){

	cout<<"Closing Connection"<<endl;
	server_socket.closeConnection();
	exit(0);

}

bool send_command(uint32_t command){

	EncryptManager em;

	chunk c;
	c.plaintext = (char*)&command;
	c.size = 4;

	encryptedChunk ec;

	if(!em.EncryptUpdate(ec,c)) 	return false;
	if(!em.EncryptFinal(ec))		return false;

	if(!sendDataHMAC(server_socket,ec.ciphertext,ec.size)){			//aggiorno il nonce
		cout<<"ERRORE SEND"<<endl;
		return false;
	}

	delete[] ec.ciphertext; 

	return true;
}

void cmd_list(){

	if(!send_command(LIST_COMMAND)){
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

	DecryptManager dm;
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
	if(!send_command(UPLOAD_COMMAND)) return;

	if(!sendDataHMAC(server_socket,(const char*)filename.c_str(),filename.length()+1)) return;		//vanno cifrati

	
	if(!SendFile(path,server_socket,(char *)filename.c_str(),CLIENT_PRIVKEY_PATH,true))
		cout << "sendFile fallita" << endl;
	else
		cout << "sendFile corretta" << endl;
}


void cmd_get(){

	//if(!server_socket.sendInt(GET_COMMAND)) return;
	if(!send_command(GET_COMMAND)) return;
	string filename;
	cin >> filename;
	string path(CLIENT_PATH);

	int32_t length = filename.length()+1;

	if(!sendDataHMAC(server_socket,(const char*)filename.c_str(),length)) return;			//va cifrato
	if(!ReceiveFile(path,( char*)filename.c_str(),server_socket,public_key_rsa,true))
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
	delete[] old_ptr;


	if(!server_cert)
		return false;


	CertificateManager cm(CERT_CA_PATH,CERT_CA_CRL_PATH);
	if(!cm.verifyCertificate(server_cert)){
		cout<<"errore verifica certificato"<<endl;
		X509_free(server_cert);
		return false;
	}

	string name;
	if(!cm.extractCommonName(server_cert,name)){
		X509_free(server_cert);
		cout<<"errore extractCommonName"<<endl;
		return false;
	}

	if(name != SERVER_NAME){
		X509_free(server_cert);
		return false;
	}

	public_key_rsa.setKey(cm.extractPubKey(server_cert));


	cout<<"Connessione stabilita con il server "<<name<<endl;

	DHManager dh(DH_PARAMS_PATH);
	int pub_key_len;
	char *pub_key = dh.generatePublicKey(pub_key_len);
	if(pub_key == NULL){
		exit(-1);
	}

	char *opponent_pub_key;
	int opponent_pub_key_len;

	RSASignManager sign(CLIENT_PRIVKEY_PATH);


	if(!server_socket.recvInt(opponent_pub_key_len)) 	return false;

	if(!sign.RSAUpdate((const char*)&opponent_pub_key_len,sizeof(uint32_t)))			return false;


	//add check to int received
	opponent_pub_key = server_socket.recvData(opponent_pub_key_len);
	if(opponent_pub_key == NULL)
		return false;

	if(!sign.RSAUpdate((const char*)opponent_pub_key,opponent_pub_key_len))			return false;

	if(!sign.RSAUpdate((const char*)&pub_key_len,sizeof(uint32_t)))			return false;
	if(!server_socket.sendInt(pub_key_len)) 			return false;

	if(!sign.RSAUpdate((const char*)pub_key,pub_key_len))			return false;
	if(!server_socket.sendData(pub_key,pub_key_len)) 	return false;

	delete[] pub_key;

	int key_length;
	char *simmetric_key = dh.computeSimmetricKey(opponent_pub_key,opponent_pub_key_len,key_length);

	delete[] opponent_pub_key;



	HashManager keys;
	keys.HashUpdate(simmetric_key,key_length);
	char* digest_keys = keys.HashFinal();

	char AES_symmetric_key[AES_KEY_SIZE];
	memcpy(AES_symmetric_key,digest_keys,AES_KEY_SIZE);

	char HMAC_key[HMAC_KEY_SIZE];
	memcpy(HMAC_key,&digest_keys[HMAC_KEY_SIZE],HMAC_KEY_SIZE);

	X509_free(server_cert);


	KeyManager::setAESKey(AES_symmetric_key);
	KeyManager::setHMACKey(HMAC_key);

	memset(AES_symmetric_key,0,AES_KEY_SIZE);
	memset(HMAC_key,0,HMAC_KEY_SIZE);

	char *revc_IV = server_socket.recvData(AES_BLOCK);
	if(revc_IV == NULL){
		//delete robbba
		return false;
	}
	if(!sign.RSAUpdate((const char*)revc_IV,AES_BLOCK))			return false;



	KeyManager::setAESIV(revc_IV);
	delete[] revc_IV;

	RAND_poll();

	uint32_t local_nonce = 0;
	RAND_bytes((unsigned char*)&local_nonce,sizeof(uint32_t));

	chunk c;
	encryptedChunk ec;

	int remote_nonce_size; 
	if(!server_socket.recvInt(remote_nonce_size)) return false;

	if(!sign.RSAUpdate((const char*)&remote_nonce_size,sizeof(uint32_t)))			return false;


	char *recv_data = server_socket.recvData(remote_nonce_size);

	if(recv_data == NULL){
		//delete robba
		return false;
	}

	ec.ciphertext = recv_data;
	ec.size = remote_nonce_size;

	DecryptManager dm;
	if(!dm.DecryptUpdate(c,ec))			return false;
	if(!dm.DecryptFinal(c))				return false;

	int remote_nonce = *((int*)c.plaintext);
	if(!sign.RSAUpdate((const char*)&remote_nonce,sizeof(uint32_t)))			return false;

	delete[] c.plaintext;
	delete[] ec.ciphertext;

 	c.plaintext = (char*)&local_nonce;
	c.size = sizeof(uint32_t);

	EncryptManager em;
	if(!em.EncryptUpdate(ec,c)){
		return false;
	}

	if(!em.EncryptFinal(ec)) return false;

	if(!server_socket.sendInt(ec.size)) return false;
	
	if(!sign.RSAUpdate((const char*)&ec.size,sizeof(uint32_t)))			return false;
	if(!sign.RSAUpdate((const char*)&local_nonce,sizeof(uint32_t)))			return false;


	if(!server_socket.sendData(ec.ciphertext,ec.size)){
		//delete robba
		return false;
	}

	delete[] ec.ciphertext;

	uint32_t sign_final_len;
	char* sign_final = sign.RSAFinal(sign_final_len);
	if(sign_final == NULL) return false;

	HMACManager::setLocalNonce(local_nonce);
	HMACManager::setRemoteNonce(remote_nonce);

	if(!sendDataHMAC(server_socket,sign_final,sign_final_len))			return false;

	int32_t received_sign_len;
	char *received_sign = recvDataHMAC(server_socket,received_sign_len);
	if(received_sign == NULL)
		return false;

	RSAVerifyManager verify(public_key_rsa);
	if(!verify.RSAUpdate(sign_final,sign_final_len)){
		delete[] sign_final;
		return false;
	}

	int result = verify.RSAFinal(received_sign);

	delete[] received_sign;

	delete[] sign_final;

	delete[] simmetric_key;
	delete[] digest_keys;

	//destroy "old" key?

	return result;

}

int main(int argc,char **argv){

	if(argc < 3){
		cout<<"[Errore] ip e porta necessari"<<endl;
		exit(-1);
	}

	signal(SIGPIPE, SIG_IGN);
	signal (SIGINT,close_handler);

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
		cout<<"error"<<endl;
		exit(-1);
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
				if(i == 0){						//stdin
					read_input();				//keyboard			
					//continue;
				} else if(i == socket_tcp) {			//server tcp
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
