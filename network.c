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

  ftp_state.infos = res;
}

bool send_control(char *cmd, char *str) {
  char write_buf[2048];
  char read_buf[2048];
  bzero(write_buf, 2048);
  bzero(read_buf, 2048);

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

  // just print answer
  if (recv(ftp_state.control_fd, read_buf, 2048, 0) < 0) {
    printf("cannot read socket\n");
    return false;
  }
  printf("%s", read_buf);

  ftp_state.last_code = atoi(read_buf);

  return true;
}

void init_sockets(int port) {
  struct sockaddr *s;
  struct sockaddr_in s4;
  struct sockaddr_in6 s6;
  int len;
  int control_fd = socket(ftp_state.infos->ai_family, SOCK_STREAM, 0);
  if (control_fd < 0) {
    perror("socket");
    return;
  }

  ftp_state.control_fd = control_fd;

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

  if (connect(control_fd, s, len) < 0) {
    perror("connect");
    return;
  }

  bool asked_exit = false;
  bool loop = true;
  char read_buf[2048];
  char write_buf[2048];

  bzero(read_buf, 2048);
  bzero(write_buf, 2048);

  // just print welcome message
  if (recv(control_fd, read_buf, 2048, 0) < 0) {
    printf("cannot read socket\n");
    return;
  }
  printf("%s", read_buf);

  printf("Name: ");
  if (read_user_input() != 1)
    return;
  if (send_control("USER", ftp_state.input) && ftp_state.last_code == 331) {
    printf("Password: ");
    if (read_user_input() != 1)
      return;
    if (send_control("PASS", ftp_state.input) && ftp_state.last_code == 332) {
      printf("Account: ");
      if (read_user_input() != 1)
        return;
      send_control("ACCT", ftp_state.input);
    }
  }

  while (loop) {
    PROMPT();

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
      printf("connection is already set. please use the 'ciao' command before "
             "login to another server.\n");
      break;
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

  close(control_fd);
  ftp_state.control_fd = -1;

  if (asked_exit)
    handle_exit();
}
