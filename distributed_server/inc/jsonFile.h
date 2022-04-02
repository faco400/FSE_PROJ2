#ifndef JSONFILE_H_
#define JSONFILE_H_

#include "cJSON.h"

typedef struct Server {
    int porta;
    char *ip, *nome;
    cJSON *outputs, *inputs;
} Server;

void parse_json_string(char *json_string, Server *server);
void final_json(char **json_string, Server *server, int temperature, int humidity, int *total_people);

#endif /* JSONFILE_H_ */