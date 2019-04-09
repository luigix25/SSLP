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

	cout<<"Files Available for downloading fron Server:"<<endl;
	for(int i= 0;i<number_files;i++){
		cout<<files_list[i]<<'\t';
	}
	cout<<endl;

	cout<<"Files Available for uploading to Server:"<<endl;

	vector<string> files_client = get_file_list("client/database/");

	for(uint i= 0;i<files_client.size();i++){
		cout<<files_client[i]<<'\t';
	}
	cout << endl;

}

void print_hex(unsigned char* buff, unsigned int size)
{
    printf("Printing %d bytes\n", size);
    for(unsigned int i=0; i<size; i++)
        printf("%02hx", *((unsigned char*)(buff + i)));
    printf("\n");
}

void cmd_upload(){
	string filename;
	cin >> filename;
	string path("client/database/");
	fstream tmp(path + filename);
	if(!tmp.good()){
		cout << "il file non esiste" << endl;
		return;
	}
	else
		cout << "il file esiste" << endl;
	if(!server_socket.sendInt(UPLOAD_COMMAND)) return;

	if(!server_socket.sendData((const char*)filename.c_str(),filename.length()+1)) return;

	
	if(!SendFile(path,server_socket,(char *)filename.c_str()))
		cout << "sendFile fallita" << endl;
	else
		cout << "sendFile corretta" << endl;
}


void cmd_get(){

	if(!server_socket.sendInt(GET_COMMAND)) return;
	string filename;
	cin >> filename;
	string path("client/database/");

	int32_t length = filename.length()+1;

	if(!server_socket.sendData((const char*)filename.c_str(),length)) return;
	if(!ReceiveFile(path,( char*)filename.c_str(),server_socket))
		cout << "ReceiveFile ERRATA" << endl;
	else
		cout << "ReceiveFile CORRETTA" << endl;
	
	//WARING SECURECODING
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
