#include <wiringPi.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/un.h>

#include "../inc/cJSON.h"
#include "../inc/gpio.h"
#include "../inc/jsonFile.h"
#include "../inc/dht.h"

#define IP "192.168.0.53"
#define PORTA 10124
#define MAX_SIZE  5000

pthread_t SOCKET_GET, SOCKET_SEND;

char *message, *data;
int temperature = 0, humidity = 0, total_people = 0, toggle_value = 0;
int sd; /*global file descriptor*/


void dist_socket();
void get_request();
void send_data();

int main () {
  wiringPiSetup();
  dist_socket();

  return 0;
}

void dist_socket() {
	struct  sockaddr_in ladoServ; /* contem dados do servidor 	*/
  unsigned short serverPort;    /* contem porta do server */
  char *IP_Server;              /* contem IP do server    */
  IP_Server = inet_addr(IP);
  serverPort = PORTA;
  message = calloc(MAX_SIZE, sizeof(char));
  data = calloc(MAX_SIZE, sizeof(char));

  /* Cria socket */
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		fprintf(stderr, "Criacao do socket falhou!\n");
		exit(1); 
  }

  ladoServ.sin_family      = AF_INET; /* config. socket p. internet*/
	ladoServ.sin_addr.s_addr = IP_Server;
	ladoServ.sin_port        = htons(serverPort);

  /* Conecta socket ao servidor definido */
 while (connect(sd, (struct sockaddr *)&ladoServ, sizeof(ladoServ)) < 0);
  while (1) {
    pthread_create(&SOCKET_GET, NULL, get_request, NULL);
    pthread_join(SOCKET_GET, NULL);
    pthread_create(&SOCKET_SEND, NULL, send_data, NULL);
    pthread_join(SOCKET_SEND, NULL);
  }
}

void get_request() {
  int i;
  char num[5];
  recv(sd, data, MAX_SIZE, 0);
  for (i = 0; data[i] != '\0'; i++)
    if (i > 3){
        break;

    }
  if (i < 3) {
    printf("Recebi porta: %d", atoi(num));
    toggle_value = 1;
    strcpy(num, data);
    int value = read_gpio(atoi(num));
    if (value == 0) {
      printf("Ligando %d\n", atoi(num));
      turn_on(atoi(num));
    }
    else {
      printf("Desligando%d\n", atoi(num));
      turn_off(atoi(num));
    }
  }
  else {
    toggle_value = 0;
    strcpy(message, data);
  }
  sleep(1);
}

void send_data() {
  if (strlen(message) > 0 && !toggle_value) {
    char *final = malloc(MAX_SIZE);
    Server *server_config = malloc(sizeof(Server));
    parse_json_string(message, server_config);
    read_dht_data(&temperature, &humidity, 0);
    final_json(&final, server_config, temperature, humidity, &total_people);
    send(sd, final, MAX_SIZE, 0);
    free(final);
    free(server_config);
  }
  sleep(1);
}