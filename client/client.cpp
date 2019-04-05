#include "../library/library.h"
#include "../library/ReadFileManager.h"				//library è qui dentro
#include "../library/WriteFileManager.h"				//library è qui dentro

#include <iostream>
#include <vector>


using namespace std;

fd_set master;
int socket_tcp;
struct sockaddr_in opponent;

NetSocket server_socket;

/*void handle_receive_data(int);
void cmd_show();
void protocol_error(int);
*/

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
			"username --> avvia una partita con l'utente username",
			"--> disconnette il client dal server","effe\n"};

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


	/*printf("\nClient disconnesso correttamente\n");
	if(!sendInt(sock,QUIT_COMMAND))		return;
	close(sock);
	exit(1);*/
}

void cmd_disconnect(int sock){
	/*printf("Disconnessione avvenuta con successo: TI SEI ARRESO\n");

	if(!sendInt(sock,DISCONNECT_COMMAND))		return;*/

}

void cmd_list(){


	/*printf("Disconnessione avvenuta con successo: TI SEI ARRESO\n");*/

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
	//WARING SECURECODING

	if(!server_socket.sendInt(GET_COMMAND)) return;

	int32_t length = filename.length()+1;

	if(!server_socket.sendData(filename.c_str(),length)) return;

	//handle get
	string full_name = "client/database/";
	full_name += filename;
	cout<<"Scrivo: "<<full_name<<endl;

	int32_t file_size;
	if(!server_socket.recvInt(file_size)) return;

	char *recvd_data;
	int len;
	file_status status;

	cout<<"File Size: "<<file_size;
	WriteFileManager fm(full_name,file_size);
	
	//EVP_CIPHER_CTX* ctx = decrypt_INIT((unsigned char*)KEY_AES,(unsigned char*)IV);
	//int plaintext_len = 0;

	while(true){

		recvd_data = server_socket.recvData(len);

		if(recvd_data == NULL) return;

		cout<<"Ricevuti "<<len<<endl;
		print_hex((unsigned char*)recvd_data,len);

		BIO_dump_fp (stdout, (const char *)recvd_data, len);

		char *plaintext = (char*)malloc(len);

		char *key = (char*)malloc(50);
		strcpy(key,(const char*)"panuozzopanuozzpanuozzopanuozz");

		int plaintext_len =  decrypt((unsigned char*)recvd_data, len, (unsigned char*)KEY_AES, (unsigned char*)IV, (unsigned char*)plaintext);
		BIO_dump_fp (stdout, (const char *)plaintext, plaintext_len);


		/*int abc_enc_len =  encrypt((unsigned char*)abc, abc_len, (unsigned char*)KEY_AES, NULL, (unsigned char*)abc_enc);

		BIO_dump_fp (stdout, (const char *)abc_enc, abc_enc_len);
		cout<<"DECIFRO"<<endl;

		char *plaintext = (char*)malloc(25+16);

		int plaintext_len =  decrypt((unsigned char*)abc_enc, abc_enc_len, (unsigned char*)KEY_AES, NULL, (unsigned char*)abc);

		BIO_dump_fp (stdout, (const char *)abc, plaintext_len);*/

		//int pt = decrypt((unsigned char*)recvd_data, len,(unsigned char*)KEY_AES, (unsigned char*)IV, (unsigned char*)plaintext);


		//BIO_dump_fp (stdout, (const char *)plaintext, plaintext_len);
		break;


		
	/*	encryptedChunk c;
		c.size = len;
		c.ciphertext = recvd_data;


		decrypt_UPDATE(ctx,(unsigned char*)c.ciphertext,c.size,(unsigned char*)plaintext,plaintext_len);

		cout<<"Plaintext LEN : "<<plaintext_len<<endl;
		cout<<"Plaintext : "<<plaintext<<endl;

		file_size-= plaintext_len;

		if(file_size < AES_BLOCK){			//ultimo chunk
			decrypt_FINAL(ctx,(unsigned char*)plaintext+plaintext_len, plaintext_len);
			file_size-= plaintext_len;
			cout<<plaintext_len<<endl;
			break;
		}
*/


		//memcpy(c.plaintext,recvd_data,len);
		//free(recvd_data);


	/*	status = fm.write(&c);
		if(status == END_OF_FILE){
			cout<<"FINITO"<<endl;
			break;
		} else if(status == FILE_ERROR){
			return;
		}*/

		//free(c.ciphertext);

	}			

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
		cout<<"Comando non riconosciuto"<<endl;
		pulisci_buff();
	}


}

void read_input(){

	string buffer;
	fflush(stdout);
	cin>>buffer;
	//scanf("%ms",&buffer);

	select_command(buffer);
	//free(buffer);

}

void wait_for_opponent(short cmd){			//disabilita tastiera

/*	waiting = cmd;
	if(cmd){
		FD_CLR(0,&master);
	} else {
		FD_SET(0,&master);
	}*/
}



void protocol_error(int sock_tcp){

	close(sock_tcp);
	exit(-1);

}

void select_command_server(int socket,int cmd){

	/*switch(cmd){
		case CONNECT_REQ:
			handle_connection_request(socket);
			break;
		case CONNECT_ACPT:
			handle_connection_accepted(socket);
			break;
		case CONNECT_REFUSED:
			printf("L'utente ha rifiutato la tua partita\n");
			return;
			break;
		case CONNECT_NOUSER:
			printf("Utente non esistente\n");
			break;
		case CONNECT_BUSY:
			printf("L'utente e' impegnato in un'altra partita\n");
			break;
		case CONNECT_DATA:
			handle_receive_data(socket);
			break;
		case WON_RETIRED:
			printf("Complimenti hai vinto! Il tuo avversario si e' ritirato\n");
			wait_for_opponent(false);
			in_game = false;
			break;
		case OPP_DISCONNECTED_TCP:
			printf("Il tuo avversario non e' più connesso con il server di gioco, Hai vinto!\n");
			in_game = false;
			wait_for_opponent(false);
			break;
		case YOU_TIMEOUT:
			printf("Timeout, hai perso!\n");
			in_game = false;
			wait_for_opponent(false);
			break;

	}*/

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

	/*const char *abc = "Uno due tre stella";
	int lun = strlen(abc)+1;

	printf("Invio: %s\n",abc);
	//sendData(socket_tcp,abc,lun);
*/
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
