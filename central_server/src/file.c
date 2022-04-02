#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_file(int path)
{
  char *fp = path == 1 ? "../config/configuracao_andar_1_test.json" : "../config/configuracao_andar_terreo_test.json";
  long length;
  FILE *file = fopen(fp, "r");
  if (file)
  {
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    if (buffer)
    {
      fread(buffer, 1, length, file);
    }
    fclose(file);
    return buffer;
  }
  else
  {
    exit(0);
  }
}