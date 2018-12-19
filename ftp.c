#include "ftp.h"
#include "network.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init() {
  ftp_state.debug = true;
  ftp_state.loop = true;
  ftp_state.control_fd = -1;
  ftp_state.data_fd = -1;
}

int read_user_input() {
  int n;
  if ((n = scanf(" %[^\n]s", ftp_state.input_buffer)) == 1) {
    ftp_state.input = strtok(ftp_state.input_buffer, " ");
  }
  return n;
}

void handle_exit() { ftp_state.loop = false; }

void handle_open() {
  char *hostname;
  int port = 21;

  // get hostname
  ftp_state.input = strtok(NULL, " ");
  if (!ftp_state.input) {
    printf("(to) ");
    read_user_input();
  }

  // if it was empty, show usage and return
  if (!ftp_state.input) {
    printf("usage: open host-name [port]\n");
    return;
  }

  hostname = ftp_state.input;

  // get port
  ftp_state.input = strtok(NULL, " ");
  if (ftp_state.input) {
    port = atoi(ftp_state.input);
  }

  if (ftp_state.debug) {
    printf("DEBUG: opening connection on %s:%d...\n", hostname, port);
  }

  fetch_addr_infos(hostname);
  if (!ftp_state.infos)
    return;
  init_sockets(port);
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
    PROMPT();

    if (read_user_input() != 1)
      handle_exit();

    if (!ftp_state.input)
      continue;

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
