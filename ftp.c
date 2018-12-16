#include "ftp.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

void init() {
  ftp_state.debug = true;
  ftp_state.loop = true;
}

int main() {
  init();

  printf("Welcome to FTP Client!\n");

  while (ftp_state.loop) {
    printf("ftp> "); // displaying prompt
    scanf(" %[^\n]s", ftp_state.input_buffer);
    if (STR_EQ(ftp_state.input_buffer, "exit")) {
      ftp_state.loop = false;
    } else if (STR_EQ(ftp_state.input_buffer, "debugon")) {
      ftp_state.debug = true;
      printf("Debugging on (debug=1).\n");
    } else if (STR_EQ(ftp_state.input_buffer, "debugoff")) {
      ftp_state.debug = false;
      printf("Debugging off (debug=0).\n");
    } else if (STR_EQ(ftp_state.input_buffer, "debug")) {
      if (ftp_state.debug) {
        ftp_state.debug = false;
        printf("Debugging off (debug=0).\n");
      } else {
        ftp_state.debug = true;
        printf("Debugging on (debug=1).\n");
      }
    } else {
      printf("?Invalid command\n");
    }
  }

  return 0;
}
