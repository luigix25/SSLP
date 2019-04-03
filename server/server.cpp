//#include "../library/library.h"
#include "../library/FileManager.h"				//library è qui dentro
#include "server.h"
#include <iostream>
#include <vector>


using namespace std;

fd_set master;
NetSocket client_socket;


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

	FileManager fm(full_name);
	free(filename); 							//non mi serve più

	uint64_t size = fm.size_file();
	cout<<"File size: "<<size<<endl;

	chunk c;

	//aggiungere while

	fm.read(&c,0);
	cout<<c.plaintext<<endl;
	cout<<c.size<<endl;

	if(!client_socket.sendData(c.plaintext,c.size)) return;




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
