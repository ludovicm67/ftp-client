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

void read_user_input() {
  scanf(" %[^\n]s", ftp_state.input_buffer);
  ftp_state.input = strtok(ftp_state.input_buffer, " ");
}

void handle_exit() { ftp_state.loop = false; }

void handle_open() {
  ftp_state.input = strtok(NULL, " ");
  if (!ftp_state.input) {
    printf("(to) ");
    read_user_input();
  }
  printf("@TODO: open %s:21\n", ftp_state.input);
}

void handle_debugon() {
  ftp_state.debug = true;
  printf("Debugging on (debug=1).\n");
}

void handle_debugoff() {
  ftp_state.debug = false;
  printf("Debugging off (debug=0).\n");
}

void handle_debug() {
  if (ftp_state.debug)
    handle_debugoff();
  else
    handle_debugon();
}

int main() {
  init();

  printf("Welcome to FTP Client!\n");

  while (ftp_state.loop) {
    printf("ftp> "); // displaying prompt

    read_user_input();

    if (STR_EQ(ftp_state.input, "exit"))
      handle_exit();
    else if (STR_EQ(ftp_state.input, "open"))
      handle_open();
    else if (STR_EQ(ftp_state.input, "debugon"))
      handle_debugon();
    else if (STR_EQ(ftp_state.input, "debugoff"))
      handle_debugoff();
    else if (STR_EQ(ftp_state.input, "debug"))
      handle_debug();
    else {
      printf("?Invalid command\n");
    }
  }

  return EXIT_SUCCESS;
}
