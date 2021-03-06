#include "server.h"

fd_set master;
NetSocket client_socket;
int server_socket;
PublicKey public_key_rsa;

void close_handler(int s){
	cout<<endl<<"Terminating.."<<endl;
	client_socket.closeConnection();						
	KeyManager::destroyKeys();
	close(server_socket);
	exit(s);
}


bool isClientAuthorized(string& clientName, const char* filePath){

	ifstream file(filePath);
	if(!file){
		cerr<<"Error opening file"<<endl;
		return false;
	}

	while(!file.eof()){
		string name;
		file>>name;
		if(name == clientName){
			file.close();
			return true;
		}

	}

	file.close();
	return false;

}


bool cmd_list(){
	vector <string> files;
	files = get_file_list(SERVER_PATH);
	int number;

	number = (int)files.size();

	string concatenated = files[0];

	for(uint16_t i =1;i<number;i++){
		concatenated += " "+files[i];
	}

	const char *str = concatenated.c_str();
	int len = concatenated.size()+1;

	EncryptManager em;
	Chunk c;
	c.setPlainText((char*)str,false);		//i don't need to free memory
	c.size = len;

	EncryptedChunk ec;

	if(!em.EncryptUpdate(ec,c)) return false;
	if(!em.EncryptFinal(ec))	return false;


	//delete[] c.plaintext;

	//cout << "lista che invio da server: \n" << ec.ciphertext<<endl;
	if(!sendDataHMAC(client_socket,ec.getCipherText(),ec.size))
		return false;
	return true;
	

}

void print_hex(unsigned char* buff, unsigned int size)
{
    printf("Printing %d bytes\n", size);
    for(unsigned int i=0; i<size; i++)
        printf("%02hx", *((unsigned char*)(buff + i)));
    printf("\n");
}

bool cmd_get(){

	int len;

	char *encryptedData = recvDataHMAC(client_socket,len);
	if(encryptedData == NULL)
		return false;

	EncryptedChunk ec;
	ec.setCipherText(encryptedData);				//now in charge of memory management
	ec.size = len;

	Chunk c;

	DecryptManager dm;
	if(!dm.DecryptUpdate(c,ec))			return false;
	if(!dm.DecryptFinal(c))				return false;

	string filename(c.getPlainText());

	if(filename.size() >= MAX_FILENAME_LENGTH){
		return false;
	}

	string path(SERVER_PATH);
	cout<<path<<endl;
	if(!SendFile(path,client_socket,filename,SERVER_PRIVKEY_PATH,false)){
		cout << "[ERROR] sendFile failed" << endl;
		return false;
	}
	else{
		cout << "[LOG] sendFile correct" << endl;
		return true;
	}
}

bool cmd_upload(){
	int len;

	char* encryted_data = recvDataHMAC(client_socket,len);
	if(encryted_data == NULL)
		return false;


	EncryptedChunk ec;
	ec.setCipherText(encryted_data);				//now in charge of memory management
	ec.size = len;

	Chunk c;

	DecryptManager dm;
	if(!dm.DecryptUpdate(c,ec))			return false;
	if(!dm.DecryptFinal(c))				return false;

	string filename(c.getPlainText());

	string path(SERVER_PATH);
	if(!ReceiveFile(path,filename,client_socket,public_key_rsa,false)){
		cout << "[ERROR]" << endl;
		return false;
	}

	return true;

}



bool select_command(int cmd){

	switch (cmd){
		case LIST_COMMAND:
			return cmd_list();
		case GET_COMMAND:
			return cmd_get();
		case UPLOAD_COMMAND:
			return cmd_upload();
		default:
			return false;
		}


}

int initialize_server(int port){
	
	int listener;
	int status;

	if(port <= 0){
		cout<<"Port error"<<endl;
		exit(-1);
	}

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if(listener < 0){
		cout<<"[Error] socket"<<endl;
		exit(-1);
	}

	struct sockaddr_in listenerAddress;
	memset(&listenerAddress,0,sizeof(listenerAddress));
	
	listenerAddress.sin_port = htons(port);
	listenerAddress.sin_family = AF_INET;
	inet_pton(AF_INET,"0.0.0.0",&listenerAddress.sin_addr);


	status = bind(listener, (struct sockaddr*)& listenerAddress, sizeof(listenerAddress));
	if(status < 0){
		perror("[Error] bind\n");
		exit(-1);
	}

	status = listen(listener,10);
	if(status < 0){
		perror("[Error] listen\n");
		exit(-1);
	}


	return listener;

}

bool receive_command(int &command){

	int len;
	char *raw_data = recvDataHMAC(client_socket,len);

	if(raw_data == NULL){
		cout<<"[Error] receive command"<<endl;
		return false;
	}

	EncryptedChunk ec;
	ec.size = len;
	ec.setCipherText(raw_data);

	Chunk c;

	DecryptManager dm;
	dm.DecryptUpdate(c,ec);
	dm.DecryptFinal(c);

	//il plaintext è un numero

	command = c.getInt();

	//delete[] c.plaintext;			//non mi serve più
	//delete[] ec.ciphertext;

	return true;


}


bool initial_protocol(NetSocket &client_socket){

	int cert_size;

	if(!client_socket.recvInt(cert_size)) 		return false;

	if(cert_size < 0 || cert_size > MAX_CERT_LENGTH)
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

	string name;

	if(!cm.extractCommonName(client_cert,name)){
		X509_free(client_cert);
		return false;
	}


	public_key_rsa.setKey(cm.extractPubKey(client_cert));

	if(!isClientAuthorized(name,CLIENTS_LIST)){
		cout<<"Client not authorized"<<endl;
		X509_free(client_cert);
		return false;
	}

	cout<<"Connection established with: "<<name<<endl;

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

	if(!client_socket.sendInt(cert_size)){
		delete[] cert_buf;
		return false;					
	}

	if(!client_socket.sendData((const char*)cert_buf,cert_size)){
		delete[] cert_buf;
		return false;
	}

	OPENSSL_free(cert_buf);
	X509_free(server_cert);

	RSAVerifyManager verify(public_key_rsa);

	DHManager dh(DH_PARAMS_PATH);
	int len;
	char *pub_key = dh.generatePublicKey(len);
	if(pub_key == NULL){
		exit(-1);
	}

	if(!verify.RSAUpdate((const char*)&len,sizeof(int32_t)))		return false; //check

	if(!client_socket.sendInt(len)){
		delete[] pub_key;
		return false;
	}

	if(!verify.RSAUpdate(pub_key,len))		return false; //check

	if(!client_socket.sendData(pub_key,len)){
		delete[] pub_key; 	
		return false;
	}

	delete[] pub_key;
		
	char *opponent_pub_key;
	int opponent_pub_key_len;


	if(!client_socket.recvInt(opponent_pub_key_len))	return false;			//add check to int received
	if(!verify.RSAUpdate((const char*)&opponent_pub_key_len,sizeof(int32_t)))		return false; //check

	opponent_pub_key = client_socket.recvData(opponent_pub_key_len);
	if(!verify.RSAUpdate(opponent_pub_key,opponent_pub_key_len))		return false; //check


	if(opponent_pub_key == NULL)
		return false;


	int key_length;

	char *simmetric_key = dh.computeSimmetricKey(opponent_pub_key,opponent_pub_key_len,key_length);
	delete[] opponent_pub_key;

	if(simmetric_key == NULL){
		return false;
	}

  	HashManager keys;
  	if(!keys.HashUpdate(simmetric_key,key_length)){
  		memset_s(simmetric_key,0,key_length);
  		delete[] simmetric_key;
  		return false;
  	}

	char* digest_keys = keys.HashFinal();

	memset_s(simmetric_key,0,key_length);
	delete[] simmetric_key;

	if(digest_keys == NULL)								return false;


	char AES_symmetric_key[AES_KEY_SIZE];
	memcpy(AES_symmetric_key,digest_keys,AES_KEY_SIZE);

	char HMAC_key[HMAC_KEY_SIZE];
	memcpy(HMAC_key,&digest_keys[HMAC_KEY_SIZE],HMAC_KEY_SIZE);


	memset_s(digest_keys,0,HASH_SIZE);

	delete[] digest_keys;

	char IV[AES_BLOCK];

	RAND_poll();
	RAND_bytes((unsigned char*)&IV,AES_BLOCK);

	uint32_t local_nonce = 0;
	RAND_bytes((unsigned char*)&local_nonce,sizeof(uint32_t));


	KeyManager::setAESKey(AES_symmetric_key);
	KeyManager::setAESIV(IV);

	KeyManager::setHMACKey(HMAC_key);

	if(!verify.RSAUpdate(IV,AES_BLOCK))		return false; //check

	if(!client_socket.sendData(IV,AES_BLOCK))	return false;

	Chunk c;
 	c.setInt(local_nonce);
	c.size = sizeof(uint32_t);


	EncryptedChunk ec;

	EncryptManager em;
	if(!em.EncryptUpdate(ec,c)){
		return false;
	}

	if(!em.EncryptFinal(ec)){ 
		return false;
	}

	if(!verify.RSAUpdate((const char*)&ec.size,sizeof(int32_t)))		return false; //check

	if(!client_socket.sendInt(ec.size)){
		return false;
	} 

	if(!verify.RSAUpdate((const char*)&local_nonce,sizeof(int32_t)))		return false; //check


	if(!client_socket.sendData(ec.getCipherText(),ec.size)){
		return false;
	}


	int nonce_cipher_size;

	if(!client_socket.recvInt(nonce_cipher_size)) return false;
	if(!verify.RSAUpdate((const char*)&nonce_cipher_size,sizeof(int32_t)))		return false; //check

	char *recv_data = client_socket.recvData(nonce_cipher_size);
	if(recv_data == NULL)	return false;

	ec.setCipherText(recv_data);
	ec.size = nonce_cipher_size;

	DecryptManager dm;
	if(!dm.DecryptUpdate(c,ec)){			
		return false;
	}

	if(!dm.DecryptFinal(c)){				
		return false;
	}

	int remote_nonce = c.getInt();
	if(!verify.RSAUpdate((const char*)&remote_nonce,sizeof(int32_t)))		return false; //check

	HMACManager::setRemoteNonce(remote_nonce);
	HMACManager::setLocalNonce(local_nonce);

	memset_s(AES_symmetric_key,0,AES_KEY_SIZE);
	memset_s(HMAC_key,0,HMAC_KEY_SIZE);


	int received_signature_len;
	char *received_signature = recvDataHMAC(client_socket,received_signature_len);

	int value = verify.RSAFinal(received_signature);

	RSASignManager sign(SERVER_PRIVKEY_PATH);
	if(!sign.RSAUpdate(received_signature,received_signature_len)){
		delete[] received_signature;
		return false;
	}

	delete[] received_signature;


	uint32_t sign_final_len;
	char* sign_final = sign.RSAFinal(sign_final_len);
	if(sign_final == NULL){
		return false;
	}

	if(!sendDataHMAC(client_socket,sign_final,sign_final_len)){
		delete[] sign_final;
		return false;
	}

	delete[] sign_final;

    return value;


	//mancano free
}

int main(int argc,char **argv){


	if(argc != 2){
		cout<<"[Error] missing port!"<<endl;
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
		
	if(port <= 0 || port >= USHRT_MAX){
		cout<<"[Error] Invalid Port"<<endl;
		exit(-1);
	}


	memset(&clientAddress,0,sizeof(clientAddress));

	server_socket = initialize_server(port);

	cout<<"[LOG] Waiting connections on port "<<port<<endl;

	
	FD_ZERO(&master);	
	FD_ZERO(&read_fds);
	FD_SET(server_socket,&master);
	fdmax = server_socket;

	client_socket = NetSocket();

	bool alreadyConnected = false;


	while(true){
		read_fds = master;
		if(select(fdmax+1,&read_fds,NULL,NULL,NULL) <=0){
			perror("[Error] Select");
			exit(-1);
		}

		for(i = 0; i <= fdmax; i++){
			if(FD_ISSET(i,&read_fds)){	
				if(i == server_socket){ 						//new connection
					memset(&clientAddress,0,sizeof(clientAddress));
					addrlen = sizeof(clientAddress);
					new_sock = accept(server_socket,(struct sockaddr*)&clientAddress,&addrlen);
					
					if(new_sock < 0){
						perror("[Error] accept\n");
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

					if(!initial_protocol(client_socket)){
						client_socket.closeConnection();
						KeyManager::destroyKeys();
						alreadyConnected = false;
						FD_CLR(new_sock,&master);
					}

					continue;


				} else {

					status = receive_command(cmd);
					if(!status){
						cout<<"[LOG] Client Disconnected"<<endl;
						client_socket.closeConnection();
						KeyManager::destroyKeys();
						alreadyConnected = false;
						FD_CLR(i,&master);							//remove socket from select

					} else {
						if(!select_command(cmd)){
							client_socket.closeConnection();
							KeyManager::destroyKeys();
							alreadyConnected = false;
							FD_CLR(i,&master);
						}
					}

				}
			}

		}	

	}
	
	//close(server_socket);											//no more clients allowed

	return 0;

}
