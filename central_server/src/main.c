#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define MAX_SIZE 5000
#define PORT 10224
#define IP "192.168.0.53"

int id_file;

void central_socket(int );
int menu ();


int main () {
  int op;
  system("clear");
  while(1) {
    printf("\tDigite zero(0) para monitorar o terro\n\tDigite dois(1) para monitorar o primeiro andar\n");
    
    scanf("%d", &op);
    if(op != 1 && op != 0) {
      printf("\n\tOpcao invalida. Tente novamente\n");
    }

    central_socket(op);
  }
  return 0;
}

void central_socket(int id_file) {
  struct sockaddr_in endServ;  /* endereco do servidor   */
  struct sockaddr_in endCli;   /* endereco do cliente    */
  int     sd;          	      /* socket descriptor       */
  int option = 1;
  unsigned int clienteLength;

  /* Cria socket */
  sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sd < 0) {
    fprintf(stderr, "Falha ao criar socket!\n");
    exit(1); 
  }

  memset((char *)&endServ,0,sizeof(endServ)); /* limpa variavel endServ    */
  endServ.sin_family 		= AF_INET;           	/* familia TCP/IP   */
  endServ.sin_addr.s_addr 	= inet_addr(IP); 	/* endereco IP      */
  endServ.sin_port 		= htons(PORT);               /* PORTA	    */

  /* liga socket a porta e ip */
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); // Estudar esse comando
  if (bind(sd, (struct sockaddr *)&endServ, sizeof(endServ)) < 0) {
    fprintf(stderr,"Ligacao Falhou!\n");
    exit(1); 
  }


  /* Ouve porta */
  if (listen(sd, 10) < 0) {
    fprintf(stderr,"Falhou ouvindo porta!\n");
    exit(1); 
  }
  printf("Servidor ouvindo no IP %s, na porta %d ...\n\n", IP, PORT);
  close(sd);
}