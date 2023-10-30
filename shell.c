#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define MAX_INPUT_LENGTH 50
#define MAX_NUM_COMMANDS 100
int CURR_NUM_FILES = 0;

char* commands[MAX_NUM_COMMANDS];
int num_commands = 0;

struct FileAccessInfo {
  char *filename;
  int readers;
  sem_t lock;
  sem_t wrt;
};


struct FileAccessInfo files[16];

int file_index(char *file_name){
  for (int i = 0; i < CURR_NUM_FILES; i++)
  {
    if(strncmp(files[i].filename, file_name, strlen(file_name))==0){
      return i;
    }
  }
  return CURR_NUM_FILES;
}

void rwlock_init(struct FileAccessInfo *f){
  f->readers=0;
  sem_init(&f->lock, 0, 1);
  sem_init(&f->wrt, 0, 1);
}

void* execute_read_command_thread(void* input) {
    char* read_cmd = (char*)input;
    execute_read_command(read_cmd);
    pthread_exit(NULL);
}

void* execute_write1_command_thread(void* input) {
    char* cd_cmd = (char*)input;
    execute_write1_command(cd_cmd);
    pthread_exit(NULL);
}

void* execute_write2_command_thread(void* input) {
    char* cd_cmd = (char*)input;
    execute_write2_command(cd_cmd);
    pthread_exit(NULL);
}
void rwlock_acquire_readlock(struct FileAccessInfo *f){
  sem_wait(&f->lock);
  f->readers++;
  if(f->readers==1){
    sem_wait(&f->wrt);
  }
  sem_post(&f->lock);
}
void rwlock_release_readlock(struct FileAccessInfo *f){
  sem_wait(&f->lock);
  f->readers--;
  if(f->readers==0){
    sem_post(&f->wrt);
  }
  sem_post(&f->lock);
}
void rwlock_acquire_writelock(struct FileAccessInfo *f){
  sem_wait(&f->wrt);
}
void rwlock_release_writelock(struct FileAccessInfo *f){
  sem_post(&f->wrt);
}
void execute_read_command(char *file_name){
  FILE *file = fopen(file_name, "rb"); // Open the file in binary read mode

  // Seek to the end of the file and get the file size
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fclose(file);
  int ind = file_index(file_name);
  

  rwlock_acquire_readlock(&files[ind]);
  
  printf("read %s of %lu bytes with %d readers and %d writers present\n", file_name, size, files[ind].readers, 0);
  
  rwlock_release_readlock(&files[ind]);
  return;
}

void execute_write1_command(char *file_names){
  printf("file_names: %s\n", file_names);
  char *filename1 = strtok(file_names, " ");
  char *filename2 = strtok(NULL, "");
  // printf("filename1: %s\n", filename1);
  // printf("filename2: %s\n", filename2);
  int i = file_index(filename1);
  int j = file_index(filename2);
  
  rwlock_acquire_writelock(&files[i]);
  rwlock_acquire_readlock(&files[j]);
  FILE* file1 = fopen(filename1, "a");  // Open the first file in write mode
  FILE* file2 = fopen(filename2, "r");  // Open the second file in read mode
  fseek(file1, 0, SEEK_END);
  if(ftell(file1) > 0){
    fprintf(file1, "\n"); // Add a new line if the file is not empty
  }

  int c;
  while ((c = fgetc(file2)) != EOF) {
    fputc(c, file1);
  }

  fclose(file1);
  fclose(file2);
  file2 = fopen(filename2, "rb");
  fseek(file2, 0, SEEK_END);
  long size = ftell(file2);
  fclose(file2);
 

  printf("writing to %s added %lu bytes with %d readers and %d writers present\n", filename1, size, files[i].readers, 1);
  
  rwlock_release_readlock(&files[j]);
  rwlock_release_writelock(&files[i]);
  return;
}
void execute_write2_command(char* cmd){
  char* filename = strtok(cmd, " ");  // Get the filename
  char* text = strtok(NULL, "");  // Get the text to write to the file
  printf("filename: %s\n", filename);
  printf("text: %s\n", text);
  long size = strlen(text);
  int ind = file_index(filename);
  
  rwlock_acquire_writelock(&files[ind]);
  FILE* file = fopen(filename, "a");
  fseek(file, 0, SEEK_END);  // Go to end of file
  if(ftell(file) > 0){
    fprintf(file, "\n"); // Add a new line if the file is not empty
  }
  fprintf(file, "%s", text);
  fclose(file);
  printf("writing to %s added %lu bytes with %d readers and %d writers present\n", filename, size, files[ind].readers, 1);
  
  rwlock_release_writelock(&files[ind]);
  return;
}


int main() {
  char input[MAX_INPUT_LENGTH];
  //take input from an input file and store each line in arr command
  FILE *fp;
  char line[MAX_INPUT_LENGTH];
  fp = fopen("input.txt", "r");
  if (fp == NULL)
      exit(EXIT_FAILURE);
  while (fgets(line, sizeof(line), fp) != NULL) {
      //printf("Line: %s", line);
      commands[num_commands] = (char*)malloc(strlen(line)+1);
      strcpy(commands[num_commands], line);
      num_commands++;
  }
  fclose(fp);
  // for(int i=0;i<num_commands;i++){
  //   printf("%s", commands[i]);
  // }
  // while (1) {
  //   //printf("Reader-Writer > ");
  //   fgets(input, sizeof(input), stdin);
  //   // Remove the newline character from input
  //   input[strcspn(input, "\n")] = '\0';
  //   if (strcmp(input, "exit") == 0) {
  //     break; // Exit the shell if user enters "exit"
  //   } else if (strncmp(input, "read", 4) == 0) {
  //     char * read_cmd = input + 5;
  //     execute_read_command(read_cmd);
  //   } else if (strncmp(input, "write 1", 7) == 0) {
  //     char * cd_cmd = input + 8;
  //     // execute_read_command(cd_cmd);
  //     //printf("cd_cmd: %s\n", cd_cmd);
  //     execute_write1_command(cd_cmd);
  //   } else if (strncmp(input, "write 2", 7) == 0) {
  //     char * cd_cmd = input + 8;
  //     // execute_read_command(cd_cmd);
  //     execute_write2_command(cd_cmd);
  //   }
  // }
  for(int i=0;i<num_commands;i++){
    char *input = commands[i];
    
    input[strcspn(input, "\n")] = '\0';
    if (strcmp(input, "exit") == 0) {
      break; // Exit the shell if user enters "exit"
    } else if (strncmp(input, "read", 4) == 0) {
      char * read_cmd = input + 5;
      // printf("read_cmd: %s\n", read_cmd);
      int ind = file_index(read_cmd);
      if(ind==CURR_NUM_FILES)
      { 
        files[CURR_NUM_FILES].filename = (char*)malloc(strlen(read_cmd)+1);
        strcpy(files[CURR_NUM_FILES].filename, read_cmd);
        rwlock_init(&files[CURR_NUM_FILES]);
        CURR_NUM_FILES++;
      }
    } else if (strncmp(input, "write 1", 7) == 0) {
      char * cd_cmd = input + 8;
       char *filename1 = strtok(cd_cmd, " ");
       char *filename2 = strtok(NULL, "");
      int i = file_index(filename1);
      if(i==CURR_NUM_FILES)
      { 
        files[CURR_NUM_FILES].filename = (char*)malloc(strlen(filename1)+1);
        strcpy(files[CURR_NUM_FILES].filename, filename1);
        rwlock_init(&files[CURR_NUM_FILES]);
        CURR_NUM_FILES++;
      }
      int j = file_index(filename2);
      if(j==CURR_NUM_FILES)
      { 
        files[CURR_NUM_FILES].filename = (char*)malloc(strlen(filename2)+1);
        strcpy(files[CURR_NUM_FILES].filename, filename2);
        rwlock_init(&files[CURR_NUM_FILES]);
        CURR_NUM_FILES++;
      }
    } else if (strncmp(input, "write 2", 7) == 0) {
      char * cd_cmd = input + 8;
      char* filename = strtok(cd_cmd, " ");
      int ind = file_index(filename);
      if(ind==CURR_NUM_FILES)
      { 
        files[CURR_NUM_FILES].filename = (char*)malloc(strlen(filename)+1);
        strcpy(files[CURR_NUM_FILES].filename, filename);
        rwlock_init(&files[CURR_NUM_FILES]);
        CURR_NUM_FILES++;
      }
    }
  }


  pthread_t thread_ids[num_commands];
  int num_threads = 0;
  
  for(int i=0;i<num_commands;i++){
    char *input = commands[i];
   // printf("input: %s\n", input);
    input[strcspn(input, "\n")] = '\0';
    if (strcmp(input, "exit") == 0) {
      break; // Exit the shell if user enters "exit"
    } else if (strncmp(input, "read", 4) == 0) {
      char * read_cmd = input + 5;
      //printf("read_cmd: %s\n", read_cmd);
      pthread_create(&thread_ids[num_threads], NULL, execute_read_command_thread, (void *)read_cmd);
    } else if (strncmp(input, "write 1", 7) == 0) {
      char * cd_cmd = input + 8;
      printf("cd_cmd: %s", cd_cmd);
      pthread_create(&thread_ids[num_threads], NULL, execute_write1_command_thread, (void *)cd_cmd);
      
    } else if (strncmp(input, "write 2", 7) == 0) {
      char * cd_cmd = input + 8;
      //printf("cd_cmd: %s\n", cd_cmd);
      pthread_create(&thread_ids[num_threads], NULL, execute_write2_command_thread, (void *)cd_cmd);
      
    }
    num_threads++;
  }
  
  for(int i=0;i<num_threads;i++){
    pthread_join(thread_ids[i], NULL);
  }
  
  return 0;
}