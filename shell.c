#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_INPUT_LENGTH 50
int CURR_NUM_FILES = 0;

struct FileAccessInfo {
  char *filename;
  int readers;
  int writers;
};

struct FileAccessInfo files[16];

int update_writers(char *file_name){
  for (int i = 0; i < CURR_NUM_FILES; i++)
  {
    if(files[i].filename==file_name){
      files[i].writers++;
      return i;
    }
  }
  return CURR_NUM_FILES;
}

int update_readers(char *file_name){
  for (int i = 0; i < CURR_NUM_FILES; i++)
  {
    if(files[i].filename==file_name){
      files[i].readers++;
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
  int ind = update_readers(file_name);
  fclose(file);

  if(ind==CURR_NUM_FILES)
  {
    files[CURR_NUM_FILES].filename=file_name;
    files[CURR_NUM_FILES].readers=1;
    files[CURR_NUM_FILES].writers=0;
    CURR_NUM_FILES++;
  }
  printf("read %s of %lu bytes with %d readers and %d writers present\n", file_name, size, files[ind].readers, files[ind].writers);
  
  return;
}

void execute_write1_command(char *file_names){
  char *filename1;  
  char *filename2;  
  sscanf(file_names, "%s %s", filename1, filename2);
  FILE* file1 = fopen(filename1, "w");  // Open the first file in write mode
  FILE* file2 = fopen(filename2, "r");  // Open the second file in read mode

  int c;
  while ((c = fgetc(file2)) != EOF) {
    fputc(c, file1);
  }

  fclose(file1);
  fclose(file2);

  FILE *file = fopen(filename2, "rb"); // Open the file in binary read mode
  // Seek to the end of the file and get the file size
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fclose(file);

  int ind = update_writers(filename1);
  if(ind==CURR_NUM_FILES)
  {
    files[CURR_NUM_FILES].filename=filename1;
    files[CURR_NUM_FILES].readers=0;
    files[CURR_NUM_FILES].writers=1;
    CURR_NUM_FILES++;
  }
  printf("writing to %s added %lu bytes with %d readers and %d writers present\n", filename1, size, files[ind].readers, files[ind].writers);
  
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