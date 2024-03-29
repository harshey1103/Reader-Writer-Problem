#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#define MAX_INPUT_LENGTH 50
int CURR_NUM_FILES = 0;

struct FileAccessInfo {
  char *filename;
  int readers;
  int rl;
  int wl;
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

void execute_read_command(char *file_name){
  FILE *file = fopen(file_name, "rb"); // Open the file in binary read mode

  // Seek to the end of the file and get the file size
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fclose(file);
  int ind = file_index(file_name);

  if(ind==CURR_NUM_FILES)
  { 
    files[CURR_NUM_FILES].filename = (char*)malloc(strlen(file_name)+1);
    strcpy(files[CURR_NUM_FILES].filename, file_name);
    files[CURR_NUM_FILES].readers=1;
    CURR_NUM_FILES++;
  }else{
    files[ind].readers++;
  }
  printf("read %s of %lu bytes with %d readers and %d writers present\n", file_name, size, files[ind].readers, 0);
  //printf("%s", files[ind].filename);
  return;
}

void execute_write1_command(char *file_names){
  char *filename1 = strtok(file_names, " ");
  char *filename2 = strtok(NULL, "");  
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

  FILE *file = fopen(filename2, "rb"); // Open the file in binary read mode
  // Seek to the end of the file and get the file size
  long size = ftell(file);
  fclose(file);

  int ind = update_writers(filename1);
  if(ind==CURR_NUM_FILES)
  {
    // files[CURR_NUM_FILES].filename=filename1;
    files[CURR_NUM_FILES].filename = (char*)malloc(strlen(filename1)+1);
    strcpy(files[CURR_NUM_FILES].filename, filename1);
    files[CURR_NUM_FILES].readers=0;
    CURR_NUM_FILES++;
  }
  printf("writing to %s added %lu bytes with %d readers and %d writers present\n", filename1, size, files[ind].readers, 1);
  
  return;
}
void execute_write2_command(char* cmd){
  char* filename = strtok(cmd, " ");  // Get the filename
  char* text = strtok(NULL, "");  // Get the text to write to the file
  long size = strlen(text);
  FILE* file = fopen(filename, "a");
  fseek(file, 0, SEEK_END);  // Go to end of file
  if(ftell(file) > 0){
    fprintf(file, "\n"); // Add a new line if the file is not empty
  }
  fprintf(file, "%s", text);
  fclose(file);

  int ind = file_index(filename);
  if(ind==CURR_NUM_FILES)
  {
    files[CURR_NUM_FILES].filename = (char*)malloc(strlen(filename)+1);
    strcpy(files[CURR_NUM_FILES].filename, filename);
    files[CURR_NUM_FILES].readers=0;
    CURR_NUM_FILES++;
  }
  printf("writing to %s added %lu bytes with %d readers and %d writers present\n", filename, size, files[ind].readers, 1);
  
  return;
}


int main() {
  char input[MAX_INPUT_LENGTH];

  while (1) {
    printf("Reader-Writer > ");
    fgets(input, sizeof(input), stdin);

    // Remove the newline character from input
    input[strcspn(input, "\n")] = '\0';

    if (strcmp(input, "exit") == 0) {
      break; // Exit the shell if user enters "exit"
    } else if (strncmp(input, "read", 4) == 0) {
      char * read_cmd = input + 5;
      execute_read_command(read_cmd);
    } else if (strncmp(input, "write 1", 7) == 0) {
      char * cd_cmd = input + 8;
      // execute_read_command(cd_cmd);
      //printf("cd_cmd: %s\n", cd_cmd);
      execute_write1_command(cd_cmd);
    } else if (strncmp(input, "write 2", 7) == 0) {
      char * cd_cmd = input + 8;
      // execute_read_command(cd_cmd);
      execute_write2_command(cd_cmd);
    }
  }

  return 0;
}