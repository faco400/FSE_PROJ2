#include <wiringPi.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define IP "192.168.0.53"
#define PORTA 10124
#define MAX_SIZE  5000

pthread_t SOCKET_GET, SOCKET_SEND;

char *message, *data;
int temperature = 0, humidity = 0, total_people = 0, toggle_value = 0;

void dist_socket();

int main () {
  wiringPiSetup();
  dist_socket();

  return 0;
}

void dist_socket() {
	struct  sockaddr_in ladoServ; /* contem dados do servidor 	*/
  unsigned short serverPort;    /* contem porta do server */
  char *IP_Server;              /* contem IP do server    */
  int     sd;          	      /* socket descriptor        */
  IP_Server = inet_addr(IP);
  serverPort = PORTA;

  if ((sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Socket");
      exit(0);
  }
  ladoServ.sin_family      = AF_INET; /* config. socket p. internet*/
	ladoServ.sin_addr.s_addr = IP_Server;
	ladoServ.sin_port        = htons(serverPort);

  /* Conecta socket ao servidor definido */
  if (connect(sd, (struct sockaddr *)&ladoServ, sizeof(ladoServ)) < 0) {
		fprintf(stderr,"Tentativa de conexao falhou!\n");
		exit(1); 
  }
  fprintf(stderr,"Conectou!!\n");
}