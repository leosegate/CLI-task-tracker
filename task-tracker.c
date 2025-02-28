#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h> 
#include <conio2.h>
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
  char *dateTime = malloc(100 * sizeof(char));
  strcpy(dateTime, text);
  printf("%s\n", dateTime);
  return dateTime;
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

cJSON* createTask(char *desc, int id, char *dateTime) {
  cJSON *task = cJSON_CreateObject();

  cJSON_AddNumberToObject(task, "id", id);
  cJSON_AddStringToObject(task, "description", desc);
  cJSON_AddStringToObject(task, "status", "to-do");
  cJSON_AddStringToObject(task, "createdAt", dateTime);
  cJSON_AddStringToObject(task, "updatedAt", "");
  
  return task;
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

void updateTasksIDs() {
  int tasksAmount = 0, i = 0;
  char *stringJson = NULL;
  FILE *data = fopen("data.json", "r");
  char buffer[1024];
  int len = fread(buffer, 1, sizeof(buffer), data);
  tasksAmount = tasksLength(data);
  fclose(data);
  cJSON *task;
  cJSON *aux;
  cJSON *json = cJSON_Parse(buffer);
  cJSON *array = cJSON_GetObjectItemCaseSensitive(json, "tasks");

  for(i=0; i <= tasksAmount; i++) {
    task = cJSON_DetachItemFromArray(array, i);
    stringJson = cJSON_Print(task);
    aux = cJSON_CreateNumber(i+1);
    cJSON_ReplaceItemInObject(task, "id", aux);
    stringJson = cJSON_Print(task);
  
    cJSON_InsertItemInArray(array, i, task);
    cJSON_free(task);
    cJSON_free(aux);
  }

  stringJson = cJSON_Print(json);
  data = fopen("data.json", "w");
  fputs(stringJson, data);

  cJSON_Delete(json);
  cJSON_free(array);
  cJSON_free(stringJson);
} 

void deleteTask(int id) {
  char *stringJson = NULL;
  FILE *data = fopen("data.json", "r");
  
  if (data == NULL) { 
    printf("Error: Unable to open the file.\n");  
  } else if(isEmpty(data)) {
    printf("There are no tasks to delete!\n");
  } else {
    char buffer[1024]; 
    int len = fread(buffer, 1, sizeof(buffer), data);
    fclose(data);

    cJSON *json = cJSON_Parse(buffer);
    cJSON *array = cJSON_GetObjectItemCaseSensitive(json, "tasks");
    cJSON_DeleteItemFromArray(array, id-1);
  
    stringJson = cJSON_Print(json);
    data = fopen("data.json", "w");
	  fputs(stringJson, data);

    cJSON_Delete(json);
    cJSON_free(array);
    cJSON_free(stringJson);
    fclose(data);
    updateTasksIDs();
  }
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
  char *date = actualDate();

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
	  cJSON_AddItemToArray(array, createTask(description, tasksLength(data), date));
    
    stringJson = cJSON_Print(json);
    data = fopen("data.json", "w");
	  fputs(stringJson, data);

    cJSON_Delete(json);
    cJSON_free(stringJson);
    fclose(data);
    free(date);
  }
}

void verifyInput(char string[100]) {
  char *str1 = NULL, *str2 = NULL, *str3 = NULL;
  char commands[1][10] = {"add\0"};
  
  splitString(string, &str1, &str2, &str3);
  
  if(strcmp(str1, "add") == 0) {
    addTask(str2);
  }
  if(strcmp(str1, "delete") == 0) {
    deleteTask(atoi(str2));
  }
}

int main() { 
  char input[100];
  gets(input);
  verifyInput(input);
  
  /* for tests
  addTask("teste 1");
  addTask("teste 2");
  addTask("teste 3");
  addTask("teste 4");
  addTask("teste 5");
  addTask("teste 6");
  addTask("teste 7");
  */

  getch();
  return 0;
}
