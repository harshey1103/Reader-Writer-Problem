#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_INPUT_LENGTH 50
#define CURR_NUM_FILES 0

struct FileAccessInfo {
    char filename[50];
    int readers ;
    int writers;
};

struct FileAccessInfo files[16];

void execute_read_command(const char *file_name){
  FILE *file = fopen(file_name, "rb"); // Open the file in binary read mode

  if (file == NULL) {
    // The file doesn't exist, so create it
    file = fopen(file_name, "wb");
    if (file == NULL) {
      perror("Error creating the file");
      return; // Return -1 to indicate an error
    }
  }

  // Seek to the end of the file and get the file size
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  // for (int i = 0; i < CURR_NUM_FILES; i++)
  // {
  //   if()
  // }
  
  printf("read %s of %lu\n", file_name, size);
  
  fclose(file);
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
    } else if (strncmp(input, "write 2", 7) == 0) {
      char * cd_cmd = input + 8;
      // execute_read_command(cd_cmd);
    }
  }

  return 0;
}