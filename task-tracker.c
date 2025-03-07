#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h> 
#include <conio2.h>
#include "cJSON.h"
#include "cJSON.c"

int getFileSize(FILE *data) {
  fseek(data, 0, SEEK_END);
  int fileSize = ftell(data);
  fseek(data, 0, SEEK_SET);

  return fileSize;
}

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
  
  return dateTime;
}

int tasksLength(FILE *file) {
  int fileSize = getFileSize(file);
  char buffer[fileSize];
  
  fseek(file, 0, SEEK_SET);
  
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
  cJSON_AddStringToObject(task, "status", "todo");
  cJSON_AddStringToObject(task, "createdAt", dateTime);
  cJSON_AddStringToObject(task, "updatedAt", "not updated yet");
  
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

void updateTime(int taskID) {
  taskID--;
  char *stringJson = NULL;
  cJSON *task;
  cJSON *insert;
  char *dateTime = actualDate();
  FILE *data = fopen("data.json", "r");
  int fileSize = getFileSize(data);
  char buffer[fileSize];
  int len = fread(buffer, 1, sizeof(buffer), data);
  fclose(data);

  cJSON *json = cJSON_Parse(buffer);
  cJSON *array = cJSON_GetObjectItemCaseSensitive(json, "tasks");

  task = cJSON_DetachItemFromArray(array, taskID);
  stringJson = cJSON_Print(task);
  insert = cJSON_CreateString(dateTime);
  cJSON_ReplaceItemInObject(task, "updatedAt", insert);
  cJSON_InsertItemInArray(array, taskID, task);
  
  stringJson = cJSON_Print(json);
  data = fopen("data.json", "w");
  fputs(stringJson, data);

  cJSON_Delete(json);
  cJSON_free(array);
  cJSON_free(stringJson);
  fclose(data);
}

char* getOption(char* string) {
  int i, j;
  int len = strlen(string);
  for (i = 5, j = 0; i < len; i++, j++) {
    string[j] = string[i];
  }
  string[j++] = '\0';
  return string;
}

void updateStatus(char *input, int taskID) {
  taskID--;
  char *status = getOption(input);
  char *stringJson = NULL;
  cJSON *task;
  cJSON *newStatus;
  FILE *data = fopen("data.json", "r");
  int fileSize = getFileSize(data);
  char buffer[fileSize];
  fread(buffer, 1, sizeof(buffer), data);
  fclose(data);

  cJSON *json = cJSON_Parse(buffer);
  cJSON *array = cJSON_GetObjectItemCaseSensitive(json, "tasks");

  task = cJSON_DetachItemFromArray(array, taskID);
  newStatus = cJSON_CreateString(status);
  cJSON_ReplaceItemInObject(task, "status", newStatus);
  cJSON_InsertItemInArray(array, taskID, task);
  
  stringJson = cJSON_Print(json);
  data = fopen("data.json", "w");
  fputs(stringJson, data);

  cJSON_Delete(json);
  cJSON_free(array);
  cJSON_free(stringJson);
  fclose(data);
  taskID++;
  updateTime(taskID);
}

void updateTasksIDs() {
  int tasksAmount = 0, i = 0;
  char *stringJson = NULL;
  FILE *data = fopen("data.json", "r");
  int fileSize = getFileSize(data);
  char buffer[fileSize];
  int len = fread(buffer, 1, sizeof(buffer), data);
  tasksAmount = tasksLength(data) - 1;
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
  }

  stringJson = cJSON_Print(json);
  data = fopen("data.json", "w");
  fputs(stringJson, data);

  cJSON_free(task);
  cJSON_free(aux);
  cJSON_Delete(json);
  cJSON_free(array);
  cJSON_free(stringJson);
  fclose(data);
} 

void deleteTask(int id) {
  char *stringJson = NULL;
  FILE *data = fopen("data.json", "r");
  
  if (data == NULL) { 
    printf("Error: Unable to open the file.\n");  
  } else if(isEmpty(data)) {
    printf("There are no tasks to delete!\n");
  } else {
    int fileSize = getFileSize(data);
    char buffer[fileSize];
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

    int fileSize = getFileSize(data);
    char buffer[fileSize]; 
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

char* formatString(char* string) {
  int i, j;
  int len = strlen(string);
  char ch = '"';
  for (i = j = 0; i < len; i++) {
    if (string[i] != ch) {
      string[j++] = string[i];
    }
  }
  string[j] = '\0';
  return string;
}

void listAllTasks() {
  FILE *data = fopen("data.json", "r");
  if (data == NULL) { 
    printf("Error: Unable to open the file.\n");  
  } else {
    if(isEmpty(data)) 
      printf("There is no tasks to list");
    else {
      int fileSize = getFileSize(data);
    char buffer[fileSize];
      char *stringJson = NULL;
      fread(buffer, 1, sizeof(buffer), data); 
      int i;
      int tasksAmount = tasksLength(data) - 1;

      cJSON *task, *taskCopy;
      cJSON *taskID, *taskDescription, *taskStatus, *taskCreated, *taskUpdated;
      cJSON *json = cJSON_Parse(buffer);
      cJSON *array = cJSON_GetObjectItemCaseSensitive(json, "tasks");

      for(i=0; i <= tasksAmount-1; i++) {
        task = cJSON_DetachItemFromArray(array, 0);
        
        taskID = cJSON_DetachItemFromObject(task, "id");
        stringJson = cJSON_PrintUnformatted(taskID);
        formatString(stringJson);
        printf("Task ID = %s\n", stringJson);

        taskDescription = cJSON_DetachItemFromObject(task, "description");
        stringJson = cJSON_PrintUnformatted(taskDescription);
        formatString(stringJson);
        printf("Description: %s\n", stringJson);

        taskStatus = cJSON_DetachItemFromObject(task, "status");
        stringJson = cJSON_PrintUnformatted(taskStatus);
        formatString(stringJson);
        printf("Status: %s\n", stringJson);

        taskCreated = cJSON_DetachItemFromObject(task, "createdAt");
        stringJson = cJSON_PrintUnformatted(taskCreated);
        formatString(stringJson);
        printf("Created at: %s\n", stringJson);

        taskUpdated = cJSON_DetachItemFromObject(task, "updatedAt");
        stringJson = cJSON_PrintUnformatted(taskUpdated);
        formatString(stringJson);
        printf("Updated at: %s\n", stringJson);

        printf("===================\n");
      }
      cJSON_Delete(json);
      cJSON_free(stringJson);
    }   
  }
  printf("Press any key to continue!");
  getch();
  system("cls");
  fclose(data);
}

void listTasksByStatus(char *status) {
  FILE *data = fopen("data.json", "r");
  if (data == NULL) { 
    printf("Error: Unable to open the file.\n");  
  } else {
    if(isEmpty(data)) 
      printf("There is no tasks to list");
    else {
      int fileSize = getFileSize(data);
      char buffer[fileSize];
      char *stringJson = NULL;
      fread(buffer, 1, sizeof(buffer), data); 
      int i;
      int tasksAmount = tasksLength(data) - 1;

      cJSON *task;
      cJSON *taskID, *taskDescription, *taskStatus, *taskCreated, *taskUpdated;
      cJSON *json = cJSON_Parse(buffer);
      cJSON *array = cJSON_GetObjectItemCaseSensitive(json, "tasks");

      for(i=0; i <= tasksAmount-1; i++) {
        task = cJSON_DetachItemFromArray(array, 0);
        taskStatus = cJSON_DetachItemFromObject(task, "status");
        stringJson = cJSON_PrintUnformatted(taskStatus);
        cJSON_AddItemToObject(task, "status", taskStatus);
        formatString(stringJson);

        if(strcmp(stringJson, status) == 0) {
          taskID = cJSON_DetachItemFromObject(task, "id");
          stringJson = cJSON_PrintUnformatted(taskID);
          formatString(stringJson);
          printf("Task ID: %s\n", stringJson);

          taskDescription = cJSON_DetachItemFromObject(task, "description");
          stringJson = cJSON_PrintUnformatted(taskDescription);
          formatString(stringJson);
          printf("Description: %s\n", stringJson);

          taskStatus = cJSON_DetachItemFromObject(task, "status");
          stringJson = cJSON_PrintUnformatted(taskStatus);
          formatString(stringJson);
          printf("Status: %s\n", stringJson);

          taskCreated = cJSON_DetachItemFromObject(task, "createdAt");
          stringJson = cJSON_PrintUnformatted(taskCreated);
          formatString(stringJson);
          printf("Created at: %s\n", stringJson);

          taskUpdated = cJSON_DetachItemFromObject(task, "updatedAt");
          stringJson = cJSON_PrintUnformatted(taskUpdated);
          formatString(stringJson);
          printf("Updated at: %s\n", stringJson);

          printf("===================\n");
        }
      }
      cJSON_Delete(json);
      cJSON_free(stringJson);
    }   
  }
  printf("Press any key to continue!");
  getch();
  system("cls");
  fclose(data);
}

char verifyDigit(char *input) {
  int i, len = strlen(input);
  char isDigit = '1';

  for(i = 0; i < len; i++) {
    if(input[i] > '9' || input[i] < '0')
      isDigit = '1';
    else
      isDigit = '0';
      break;
  }
  return isDigit;
}

void updateDescription(int taskID, char *description) {
  taskID--;
  char *stringJson = NULL;
  cJSON *task;
  cJSON *newDescription;
  FILE *data = fopen("data.json", "r");
  int fileSize = getFileSize(data);
  char buffer[fileSize];
  fread(buffer, 1, sizeof(buffer), data);
  fclose(data);

  cJSON *json = cJSON_Parse(buffer);
  cJSON *array = cJSON_GetObjectItemCaseSensitive(json, "tasks");

  task = cJSON_DetachItemFromArray(array, taskID);
  newDescription = cJSON_CreateString(description);
  cJSON_ReplaceItemInObject(task, "description", newDescription);
  cJSON_InsertItemInArray(array, taskID, task);
  
  stringJson = cJSON_Print(json);
  data = fopen("data.json", "w");
  fputs(stringJson, data);

  cJSON_Delete(json);
  cJSON_free(array);
  cJSON_free(stringJson);
  fclose(data);
  taskID++;
  updateTime(taskID);
}

void verifyInput(char string[100]) {
  char *str1 = NULL, *str2 = NULL, *str3 = NULL;
  char commands[1][10] = {"add\0"};
  
  splitString(string, &str1, &str2, &str3);
  
  if(strcmp(str1, "add") == 0) {
    addTask(str2);
    printf("Task added! Press any key to continue.");
    getch();
    system("cls");
  }
  if(strcmp(str1, "delete") == 0 && verifyDigit(str2)) {
    deleteTask(atoi(str2));
    printf("Task deleted! Press any key to continue.");
    getch();
    system("cls");
  }
  if(strcmp(str1, "list") == 0 && str2 == NULL) {
    listAllTasks();
  }
  if(strcmp(str1, "list") == 0 && str2 != NULL) {
    if(strcmp(str2, "todo") == 0 || strcmp(str2, "done") == 0 || strcmp(str2, "in-progress") == 0)
      listTasksByStatus(str2);
    else {
      printf("%s isn't an option! Press any key to continue.", str2);
      getch();
      system("cls");
    }
  }
  if((strcmp(str1, "mark-done") == 0 || strcmp(str1, "mark-in-progress") == 0) && verifyDigit(str2)) {
    updateStatus(str1, atoi(str2));
    printf("Status updated! Press any key to continue.");
    getch();
    system("cls");
  }

  if(strcmp(str1, "update") == 0 && verifyDigit(str2) && str3 != NULL) {
    updateDescription(atoi(str2), str3);
    printf("Description updated! Press any key to continue.");
    getch();
    system("cls");
  }
}

int main() { 
  char input[100];

  while (1) {
    printf(">");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    system("cls");
    if (input[0] == 27) {
      break;
    }
    verifyInput(input);
  }  
  return 0;
}
