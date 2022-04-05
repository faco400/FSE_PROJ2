#include "jsonFile.h"
#include "gpio.h"
#include <string.h>

void parser_json(char *json_string, Server *server)
{
    cJSON *json = cJSON_Parse(json_string);
    cJSON *porta, *ip, *outputs, *output, *inputs, *input, *nome;
    server->nome = malloc(sizeof(char)*20);
    strcpy(server->nome, cJSON_GetObjectItemCaseSensitive(json, "nome")->valuestring);
    server->outputs = cJSON_GetObjectItemCaseSensitive(json, "outputs");
    server->inputs = cJSON_GetObjectItemCaseSensitive(json, "inputs");
}

void end_json(char **json_string, Server *server, int temperature, int humidity, int *total_people)
{
    cJSON *gpio_object = cJSON_CreateObject();
    int count = 2;
    for (int i = 0; i < count; i++)
    {
        char *gpio_type = i ? "output" : "input";
        cJSON *gpio = i ? server->outputs : server->inputs;

        cJSON *gpio_array = cJSON_CreateArray();
        int len = cJSON_GetArraySize(gpio);
        for (int j = 0; j < len; j++)
        {
            cJSON *item = cJSON_GetArrayItem(gpio, j);
            char *type = cJSON_GetObjectItemCaseSensitive(item, "type")->valuestring;
            if (strcmp(type, "contagem") != 0)
            {
                char *tag = cJSON_GetObjectItemCaseSensitive(item, "tag")->valuestring;
                int gpio = cJSON_GetObjectItemCaseSensitive(item, "gpio")->valueint;
                int value = read_gpio(gpio);
                cJSON *array_item = cJSON_CreateObject();
                cJSON_AddNumberToObject(array_item, "gpio", gpio);
                cJSON_AddStringToObject(array_item, "type", type);
                cJSON_AddStringToObject(array_item, "tags", tag);
                cJSON_AddNumberToObject(array_item, "value", value);
                cJSON_AddItemToArray(gpio_array, array_item);
            }
            else if(strcmp(type, "contagem") == 0){
                int gpio = cJSON_GetObjectItemCaseSensitive(item, "gpio")->valueint;
                if(gpio == 23){
                    *total_people += 1;
                } else {
                    *total_people -= 1;
                }
            }
        }
        cJSON_AddItemToObject(gpio_object, gpio_type, gpio_array);
    }
    cJSON_AddStringToObject(gpio_object,"nome", server->nome);
    cJSON_AddNumberToObject(gpio_object, "temperature", temperature);
    cJSON_AddNumberToObject(gpio_object, "humidity", humidity);
    cJSON_AddNumberToObject(gpio_object, "total_people", *total_people);
    *json_string = malloc(5000);
    strcpy(*json_string, cJSON_Print(gpio_object));
}