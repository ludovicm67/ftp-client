#include "network.h"
#include "ftp.h"
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void fetch_addr_infos(char *host) {
  struct addrinfo hints, *res;

  ftp_state.hostname = host;
  ftp_state.infos = NULL;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags |= AI_CANONNAME;

  if (getaddrinfo(host, NULL, &hints, &res) != 0) {
    perror("getaddrinfo");
    return;
  }

  if (!res) {
    printf("error: unable to fetch '%s'\n", host);
    return;
  }

  if (res->ai_canonname && strlen(res->ai_canonname))
    ftp_state.hostname = res->ai_canonname;

  ftp_state.infos = res;
}

void handle_answer() {
  char read_buf[BUFF_SIZE];
  bzero(read_buf, BUFF_SIZE);

  // get answer
  if (recv(ftp_state.control_fd, read_buf, BUFF_SIZE, 0) < 0) {
    printf("cannot read socket\n");
    return;
  }

  // print it if we are in debug mode
  if (ftp_state.debug)
    printf("%s", read_buf);

  ftp_state.last_code = atoi(read_buf);
}

bool send_control(char *cmd, char *str) {
  char write_buf[BUFF_SIZE];
  char read_buf[BUFF_SIZE];
  bzero(write_buf, BUFF_SIZE);
  bzero(read_buf, BUFF_SIZE);

  if (ftp_state.control_fd < 0)
    return false;

  strcat(write_buf, cmd);
  if (strlen(cmd) && strlen(str))
    strcat(write_buf, " ");
  strcat(write_buf, str);
  strcat(write_buf, "\r\n");

  // submitting the command
  if (send(ftp_state.control_fd, write_buf, strlen(write_buf), 0) < 0) {
    printf("send error\n");
    return false;
  }

  handle_answer();

  return true;
}

void handle_user() {
  // ask for name
  printf("Name: ");
  if (read_user_input() != 1)
    return;
  if (!send_control("USER", ftp_state.input) || ftp_state.last_code != 331)
    return;

  // ask for password if required
  printf("Password: ");
  if (read_user_input() != 1)
    return;
  if (!send_control("PASS", ftp_state.input))
    return;
  if (ftp_state.last_code == 230)
    printf("user authenticated successfully!\n");
  else if (ftp_state.last_code == 530)
    printf("bad credentials!\n");
  if (ftp_state.last_code != 332)
    return;

  // ask for account if required
  printf("Account: ");
  if (read_user_input() != 1)
    return;
  send_control("ACCT", ftp_state.input);
}

void init_sockets(int port) {
  struct sockaddr *s;
  struct sockaddr_in s4;
  struct sockaddr_in6 s6;
  int len;
  bool asked_exit = false;
  bool loop = true;

  // create control socket
  ftp_state.control_fd = socket(ftp_state.infos->ai_family, SOCK_STREAM, 0);
  if (ftp_state.control_fd < 0) {
    perror("control socket");
    return;
  }

  // create data socket
  ftp_state.data_fd = socket(ftp_state.infos->ai_family, SOCK_STREAM, 0);
  if (ftp_state.data_fd < 0) {
    perror("data socket");
    return;
  }

  // complete the struct to connect to the server
  if (ftp_state.infos->ai_family == AF_INET6) {
    s6.sin6_family = AF_INET6;
    s6.sin6_addr = ((struct sockaddr_in6 *)ftp_state.infos->ai_addr)->sin6_addr;
    s6.sin6_port = htons(port);
    s6.sin6_flowinfo = 0;
    s = (struct sockaddr *)&s6;
    len = sizeof(struct sockaddr_in6);
  } else {
    s4.sin_family = AF_INET;
    s4.sin_addr.s_addr =
        ((struct sockaddr_in *)ftp_state.infos->ai_addr)->sin_addr.s_addr;
    s4.sin_port = htons(port);
    s = (struct sockaddr *)&s4;
    len = sizeof(struct sockaddr_in);
  }

  // connect to the server
  if (connect(ftp_state.control_fd, s, len) < 0) {
    perror("connect");
    return;
  }

  // display welcome message
  handle_answer();

  // ask user credentials
  handle_user();

  while (loop) {
    PROMPT_STR(ftp_state.hostname);

    if (read_user_input() != 1) {
      asked_exit = true;
      loop = false;
      break;
    }

    if (!ftp_state.input)
      continue;

    if (STR_EQ(ftp_state.input, "exit")) {
      asked_exit = true;
      loop = false;
      break;
    } else if (STR_EQ(ftp_state.input, "ciao")) {
      loop = false;
      break;
    } else if (STR_EQ(ftp_state.input, "open")) {
      printf(
          "connection is already started. please use the 'ciao' command before "
          "login to another server.\n");
    } else if (STR_EQ(ftp_state.input, "dir")) {
      send_control("LIST", "");
    } else if (STR_EQ(ftp_state.input, "user")) {
      handle_user();
    }

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

  send_control("QUIT", "");

  // close sockets
  close(ftp_state.control_fd);
  close(ftp_state.data_fd);
  ftp_state.control_fd = -1;
  ftp_state.data_fd = -1;

  if (asked_exit)
    handle_exit();
}
