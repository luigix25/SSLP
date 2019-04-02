#include "../library/library.h"

fd_set master;
int socket_tcp;
struct sockaddr_in opponent;

void handle_connection_request(int);
void handle_connection_accepted(int);
void handle_receive_data(int);
void cmd_show();
void wait_for_opponent(short);
void protocol_error(int);


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

	char *desc[4] = {"--> mostra l'elenco dei comandi disponibili",
			"--> mostra l'elenco dei client connessi al server",
			"username --> avvia una partita con l'utente username",
			"--> disconnette il client dal server\n"};

	int i;
	printf("\n");	
	printf("Sono disponibili i seguenti comandi:\n");
	for(i=0;i<4;i++){
		printf("%s\n",desc[i]);
	}

}


void cmd_quit(int sock){

	printf("\nClient disconnesso correttamente\n");
	if(!sendInt(sock,QUIT_COMMAND))		return;
	close(sock);
	exit(1);
}

void cmd_disconnect(int sock){
	printf("Disconnessione avvenuta con successo: TI SEI ARRESO\n");

	if(!sendInt(sock,DISCONNECT_COMMAND))		return;

}

void select_command(int sock,char *buffer){
/*
	if(strcmp("!help",buffer) == 0){
		cmd_help();
	} else if(strcmp("!who",buffer) == 0){
		cmd_who(sock);
	} else if(strcmp("!quit",buffer) == 0){
		cmd_quit(sock);
	} else if(strcmp("!connect",buffer) == 0){
		cmd_connect(sock);
	} else {
		printf("Comando non riconosciuto\n");
		pulisci_buff();
	}*/

	free(buffer);

}

void read_input(int sock){

	char *buffer = 0;
	fflush(stdout);
	scanf("%ms",&buffer);

	select_command(sock,buffer);

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

	printf("Errore del gioco. Termino\n");
	close(sock_tcp);
	exit(-1);

}

void select_command_udp(int sock,int cmd,int socket_tcp){

	/*switch(cmd){
		case SHOT_DATA:
			handle_data_shot(socket_tcp);
			break;
		case RESPONSE_SHOT:
			handle_response_shot();
			break;
		case YOU_WON:
			handle_win();
			break;
	}*/

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
		printf("[Errore] ip e porta necessari\n");
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
	
	printf("\nConnessione al server %s (port %d) effettuata con successo\n",argv[1],portServer);

	
	//cmd_help();

	FD_ZERO(&master);	
	FD_ZERO(&read_fds);


	FD_SET(socket_tcp,&master);
	FD_SET(0,&master);		// 0 è stdin

	fdmax = socket_tcp;

	char *abc = "Uno due tre stella";
	int lun = strlen(abc)+1;

	printf("Invio: %s\n",abc);
	sendData(socket_tcp,abc,lun);

	/*while(true){

	
		read_fds = master;
		if(select(fdmax+1,&read_fds,NULL,NULL,NULL) <=0){
			perror("Errore select");
			exit(-1);
		}


		for(i = 0; i <= fdmax; i++){
			if(FD_ISSET(i,&read_fds)){
				if(i == 0){			//stdin
					read_input(socket_tcp);					
					//continue;
				} else if(i == socket_tcp) {			//server tcp
					if(!recvInt(i,&cmd))			return -1;
					select_command_server(i,cmd);	
				} 

			}

		}
		
		//read_input(socket_server);

	}	*/

	
	return 0;

}
