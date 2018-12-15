#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUFF_SIZE 1024

int main() {
  char input_buffer[BUFF_SIZE];
  bool loop = true;

  printf("Welcome to FTP Client!\n");

  while (loop) {
    printf("ftp> "); // displaying prompt
    scanf(" %s", input_buffer);
    if (!strcmp(input_buffer, "exit")) {
      loop = false;
    } else {
      printf("?Invalid command\n");
    }
  }

  return 0;
}