#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "cJSON.c"

void splitString(char string[100], char **str1, char **str2, char **str3) {
  int count = 0;
  char *aux = NULL;
  char space[2] = " ";
  char character[2] = "\"";
  
  aux = strtok(string, space);
  *str1 = aux;
  while(aux != NULL && count < 3) {
    if(count == 1) {
      *str2 = aux;
    } else if(count == 2) {
      *str3 = aux;
	  }
    count++;

	if(aux[strlen(aux)+1] == '"')
	  aux = strtok(NULL, character);
	else
	  aux = strtok(NULL, space);
  }
}

char* actualDate(void) {
  char text[100];
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  strftime(text, sizeof(text)-1, "%d/%m/%Y - %H:%M", t);
  return text;
}

int tasksLength(FILE *file) {
  fseek(file, 0, SEEK_SET);
  
  char buffer[1024]; 
  int len = fread(buffer, 1, sizeof(buffer), file); 
  cJSON *json = cJSON_Parse(buffer);
  int length = (cJSON_GetArraySize(cJSON_GetObjectItemCaseSensitive(json, "tasks")) + 1);
  
  cJSON_Delete(json);
  return length;
}

cJSON* createTask(char *desc, int id) {
  char *string = NULL;
  cJSON *task = cJSON_CreateObject();

  cJSON_AddNumberToObject(task, "id", id);
  cJSON_AddStringToObject(task, "description", desc);
  cJSON_AddStringToObject(task, "status", "to-do");
  cJSON_AddStringToObject(task, "createdAt", actualDate());
  cJSON_AddStringToObject(task, "updatedAt", cJSON_CreateNull());
  
  return task;
}

void readJSON() {
  char *string = NULL;
  int nextID;
  FILE *fp = fopen("data.json", "r"); 
  if (fp == NULL) { 
    printf("Error: Unable to open the file.\n");  
  }

  char buffer[1024]; 
  int len = fread(buffer, 1, sizeof(buffer), fp); 
  fclose(fp);

  cJSON *json = cJSON_Parse(buffer); 
  if (json == NULL) { 
    const char *error_ptr = cJSON_GetErrorPtr(); 
    if (error_ptr != NULL) { 
      printf("Error: %s\n", error_ptr); 
    } 
    cJSON_Delete(json); 
    return 1; 
  }

  nextID = (cJSON_GetArraySize(cJSON_GetObjectItemCaseSensitive(json, "tasks")) + 1);
  printf("%d\n", nextID);

  printf("1");
  cJSON *task = cJSON_CreateObject();
  printf("2");
  cJSON *array = cJSON_GetObjectItemCaseSensitive(json, "tasks");
  printf("3");

  cJSON_AddNumberToObject(task, "id", nextID);
  cJSON_AddStringToObject(task, "description", "arrumar quarto");
  cJSON_AddStringToObject(task, "status", "complete");
  cJSON_AddStringToObject(task, "createdAt", actualDate());
  printf("1");
  cJSON_AddItemToArray(array, task);
  printf("2");

  string = cJSON_Print(json);

  fp = fopen("data.json", "w");
  if(fp == NULL) {
  	printf("ERRO");
  } else {
  	fputs(string, fp);
  	fclose(fp);
  }
}

char isEmpty(FILE *file) {
  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  fseek(file, 0, SEEK_SET);
  if(size == 0) {
    return 1;
  }
  return 0;
}

void createJSONStructure(void) {
  FILE *data = fopen("data.json", "w");
  char *json = NULL;

  cJSON *object = cJSON_CreateObject();
  cJSON *allTasks = cJSON_CreateArray();
  cJSON_AddItemToObject(object, "tasks", allTasks);

  json = cJSON_Print(object);
  fputs(json, data);
  cJSON_free(json); 
  cJSON_Delete(object);
  fclose(data);
}

void addTask(char *description) {
  char *stringJson = NULL;
  FILE *data = fopen("data.json", "r");
  if (data == NULL) { 
    printf("Error: Unable to open the file.\n");  
  } else {
    if(isEmpty(data)) 
      createJSONStructure();

    char buffer[1024]; 
    int len = fread(buffer, 1, sizeof(buffer), data); 
    
    cJSON *json = cJSON_Parse(buffer);
    cJSON *array = cJSON_GetObjectItemCaseSensitive(json, "tasks");
	  cJSON_AddItemToArray(array, createTask(description, tasksLength(data)));
    
    stringJson = cJSON_Print(json);
    data = fopen("data.json", "w");
	  fputs(stringJson, data);

    cJSON_Delete(json);
    cJSON_free(stringJson);
    fclose(data);
  }
}

void verifyInput(char string[100]) {
  char *str1 = NULL, *str2 = NULL, *str3 = NULL;
  char commands[1][10] = {"add\0"};
  
  splitString(string, &str1, &str2, &str3);
  
  if(strcmp(str1, "add") == 0) {
    addTask(str2);
  }
}

int main() { 
  char input[100];
  gets(&input);
  verifyInput(input);
  //char *desc = "lavar louca";
  //char *aux = createTask(desc);
  //readJSON();


  //printf("%s", aux);
  return 0;
}