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

#include "cJSON.h"
#include "file.h"

#define MAX_SIZE 5000
#define PORT 10124
#define IP "192.168.0.53"
#define true 1
#define false 0

pthread_t SOCKET_GET, SOCKET_SEND, MENU_ID;
int socketCli,id_file;
char *file, *json_string, toggle_gpio_value = false;

void central_socket(int );
void print_menu(char *);
void get_data();
void send_data();
void handle_signal(int );

int main () {
  int op;
  system("clear");
  while(1) {
    printf("\t<<--MENU-->>\n");
    printf("\tDigite zero(0) para monitorar o terro\n\tDigite um(1) para monitorar o primeiro andar\n");
    
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
  signal(SIGQUIT, handle_signal);
  signal(SIGTSTP, handle_signal);

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
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
  if (bind(sd, (struct sockaddr *)&endServ, sizeof(endServ)) < 0) {
    fprintf(stderr,"Ligacao Falhou!\n");
    exit(1); 
  }


  /* Ouve porta */
  if (listen(sd, 10) < 0) {
    fprintf(stderr,"Falhou ouvindo porta!\n");
    exit(1); 
  }
  // printf("Servidor ouvindo no IP %s, na porta %d ...\n\n", IP, PORT);
  file = malloc(MAX_SIZE);
  json_string = malloc(MAX_SIZE);
  strcpy(file, read_file(id_file));

  /* Aceita conexoes */
  clienteLength = sizeof(endCli);
  /* espera nova conexao de um processo cliente ... */
  if ((socketCli = accept(sd, (struct sockaddr *)&endCli,&clienteLength)) < 0){
    fprintf(stderr,"Falha na conexao\n");
    exit(1); 
  }

  while (1) {
    pthread_create(&MENU_ID, NULL, print_menu, json_string);
    pthread_join(MENU_ID, NULL);
    pthread_create(&SOCKET_GET, NULL, get_data, NULL);
    pthread_create(&SOCKET_SEND, NULL, send_data, NULL);
    pthread_join(SOCKET_GET, NULL);
    pthread_join(SOCKET_SEND, NULL);
    sleep(1);
  }
  
  close(sd);
}

void print_menu(char *file) {
  if (strlen(file) > 0) {
    cJSON *json = cJSON_Parse(file);
    system("clear");
    int temp = cJSON_GetObjectItemCaseSensitive(json, "temperature")->valueint;
    int hum = cJSON_GetObjectItemCaseSensitive(json, "humidity")->valueint;
    int total_people = cJSON_GetObjectItemCaseSensitive(json, "total_people")->valueint;
    int *nome = cJSON_GetObjectItemCaseSensitive(json, "nome")->valuestring;

    cJSON *outputs = cJSON_GetObjectItemCaseSensitive(json, "output");
    cJSON *inputs = cJSON_GetObjectItemCaseSensitive(json, "input");
    cJSON *pin = NULL;
    printf("Monitorando: %s\n", nome);
    printf("Temperatura: %d⁰C | Úmidade: %d%% | Pessoas no Prédio: %d\n", temp, hum, total_people);
    printf("\nOutput:\n\nPin  Valor  Tag\n");
    cJSON_ArrayForEach(pin, outputs) {
      int gpio = cJSON_GetObjectItemCaseSensitive(pin, "gpio")->valueint;
      char *tag = cJSON_GetObjectItemCaseSensitive(pin, "tags")->valuestring;
      int value = cJSON_GetObjectItemCaseSensitive(pin, "value")->valueint;
      char *status = value ? "ON " : "OFF";
      printf("| %02d | %s | %s |\n", gpio, status, tag);
    }
    printf("\nInput:\n\nPin  Valor  Tag\n");
    cJSON_ArrayForEach(pin, inputs) {
      int gpio = cJSON_GetObjectItemCaseSensitive(pin, "gpio")->valueint;
      char *tag = cJSON_GetObjectItemCaseSensitive(pin, "tags")->valuestring;
      int value = cJSON_GetObjectItemCaseSensitive(pin, "value")->valueint;
      char *status = value ? "ON " : "OFF";
      printf("| %02d | %s | %s |\n", gpio, status, tag);
    }
  }
}

void get_data() {
  char *data = malloc(MAX_SIZE);
  int buffer_size = recv(socketCli, data, 5000, 0);
  if (buffer_size) {
    strcpy(json_string, data);
  }
  free(data);
  sleep(1);
}

void send_data() {
  int value = 0;
  int file_size = strlen(file);
  if (file_size > 0) {
    if (!toggle_gpio_value){
      send(socketCli, file, file_size, 0);
    }
    else {
      char *pin[5];
      printf("\nQual pino você deseja mudar o valor: ");
      scanf("%s", pin);
      send(socketCli, pin, sizeof(char) * 5, 0);
      toggle_gpio_value = false;
    }
  }
  sleep(1);
}

void handle_signal(int signal) {
  printf("\nAguarde...\n");
  if (signal == SIGQUIT) {
    toggle_gpio_value = true;
  }
  if (signal == SIGTSTP) {
    id_file = id_file == 1 ? 2 : 1;
    strcpy(file, read_file(id_file));
  }
}