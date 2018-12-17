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
  int control_fd = socket(ftp_state.infos->ai_family, SOCK_STREAM, 0);
  // @TODO: do something
  close(control_fd);
}
