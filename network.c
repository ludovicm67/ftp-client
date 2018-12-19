#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "network.h"
#include "ftp.h"
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

void init_sockets() {
  struct sockaddr * s;
  struct sockaddr_in s4;
  struct sockaddr_in6 s6;
  int len;
  int control_fd = socket(ftp_state.infos->ai_family, SOCK_STREAM, 0);

  if (ftp_state.infos->ai_family == AF_INET6) {
    s6.sin6_family = AF_INET6;
    s6.sin6_addr = in6addr_any;
    s6.sin6_port = htons(0);
    s6.sin6_flowinfo = 0;
    s = (struct sockaddr *) &s6;
    len = sizeof(struct sockaddr_in6);
  } else {
    s4.sin_family = AF_INET;
    s4.sin_addr.s_addr = INADDR_ANY;
    s4.sin_port = htons(0);
    s = (struct sockaddr *) &s4;
    len = sizeof(struct sockaddr_in);
  }

  if (bind(control_fd, s, len) < 0) {
    perror("bind");
    return;
  }

  listen(control_fd, 5);

  // @TODO: do something
  close(control_fd);
}
