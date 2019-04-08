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

struct sockaddr_in setup_sockaddr(char *ip,int port){

	struct sockaddr_in sv_addr;

	memset(&sv_addr,0,sizeof(sv_addr));

	sv_addr.sin_family = AF_INET;
	sv_addr.sin_port = htons(port);
	inet_pton(AF_INET,ip,&sv_addr.sin_addr);
	return sv_addr;

}


void cmd_help(){

	const char *desc[5] = {"--> mostra l'elenco dei comandi disponibili",
			"--> mostra l'elenco dei client connessi al server",
			"nomefile --> avvia una partita con l'utente nomefile",
			"--> disconnette il client dal server","padding\n"};

	int i;
	cout<<endl;
	cout<<"Sono disponibili i seguenti comandi:"<<endl;
	for(i=0;i<5;i++){
		cout<<commands_list[i]<<" "<<desc[i]<<endl;
	}

}


void cmd_quit(){

	cout<<"Closing Connection"<<endl;
	server_socket.closeConnection();
	exit(0);

}


void cmd_list(){



	if(!server_socket.sendInt(LIST_COMMAND))		return;
	
	int number_files;

	if(!server_socket.recvInt(number_files))		return;

	vector<string> files_list(number_files);

	for(int i=0;i<number_files;i++){
		int32_t len;
		char *str;

		str = server_socket.recvData(len);
		if(str == NULL) return;
		files_list[i] = str;
		free(str);

	}

	cout<<"Files Available:"<<endl;
	for(int i= 0;i<number_files;i++){
		cout<<files_list[i]<<'\t';
	}
	cout<<endl;

}

void print_hex(unsigned char* buff, unsigned int size)
{
    printf("Printing %d bytes\n", size);
    for(unsigned int i=0; i<size; i++)
        printf("%02hx", *((unsigned char*)(buff + i)));
    printf("\n");
}

void cmd_get(){

	string filename;
	cin >> filename;
	string path("client/database/");
	if(!ReceiveFile(path,filename,server_socket))
		cout << "ReceiveFile ERRATA" << endl;
	else
		cout << "ReceiveFile CORRETTA" << endl;
	
	//WARING SECURECODING

	/*if(!server_socket.sendInt(GET_COMMAND)) return;

	int32_t length = filename.length()+1;

	if(!server_socket.sendData(filename.c_str(),length)) return;

	//handle get
	string full_name = "client/database/";
	full_name += filename;
	cout<<"Scrivo: "<<full_name<<endl;

	int32_t file_size;
	if(!server_socket.recvInt(file_size)) return;

	if(file_size == 0){				//file non esistente
		cout<<"File does not exist"<<endl;
		return;
	}

	char *recvd_data,*digest;
	int len = 0;
	file_status status;

	cout<<"File Size: "<<file_size<<endl;
	WriteFileManager fm(full_name,file_size);
	DecryptManager dm(KEY_AES,AES_IV);
	HMACManager hmac(KEY_HMAC);


	while(true){

		recvd_data = server_socket.recvData(len);
		
		cout<<"Ricevuti "<<len<<endl;

		if(recvd_data == NULL){
			cout << "recvd_data NULL" <<endl;
			return;
		} 


		char *plaintext = (char*)malloc(len + AES_BLOCK);

		chunk c;
		c.plaintext = plaintext;

		encryptedChunk ec;
		ec.size = len;
		ec.ciphertext = recvd_data;

		cout << "stampo ec.size: " << ec.size << endl;

		if(!dm.DecyptUpdate(c,ec)){
			//handle error
		}

		file_size-= (c.size);

		if(file_size < AES_BLOCK){			//ultimo chunk
		
			dm.DecyptFinal(c);
		}

		if(!hmac.HMACUpdate(c)){
			//handle
		}


		if(file_size < AES_BLOCK){			//ultimo chunk
			digest = hmac.HMACFinal();
			if(digest == NULL){
				//handle
			}
		}

		status = fm.write(&c);

		free(c.plaintext);
		free(ec.ciphertext);

		if(status == END_OF_FILE){
			cout<<"FINITO"<<endl;
			break;
		} else if(status == FILE_ERROR){
			cout << "FILE_ERROR" << endl;
			return;
		}
	}		

	char *MAC_rcvd = server_socket.recvData(len);
	
	if(!memcmp(digest,MAC_rcvd,32)){
		cout<<"MAC UGUALI"<<endl;
	} else{
		cout<<"MAC DIVERSI"<<endl;
	}

	free(digest);
	free(MAC_rcvd);
*/
}

void select_command(string buffer){


	if(buffer.compare("!help") == 0){
		cmd_help();
	} else if(buffer.compare("!list") == 0){
		cmd_list();
	} else if(buffer.compare("!quit") == 0){
		cmd_quit();
	} else if(buffer.compare("!get") == 0){
		cmd_get();
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
	//free(buffer);

}

void protocol_error(int sock_tcp){

	close(sock_tcp);
	exit(-1);

}

void select_command_server(int socket,int cmd){



}

int main(int argc,char **argv){

	if(argc < 3){
		cout<<"[Errore] ip e porta necessari"<<endl;
		exit(-1);
	}

	int status,portServer,cmd;
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
					if(!server_socket.recvInt(cmd)){
						cout<<"Connessione Persa"<<endl;
						return -1;
					}			
					select_command_server(i,cmd);	
				} 

			}

		}
		
	}

	
	return 0;

}
