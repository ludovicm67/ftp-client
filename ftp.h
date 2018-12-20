#ifndef __FTP_H
#define __FTP_H

#include <stdbool.h>

#define BUFF_SIZE 2048
#define STR_EQ(str1, str2) !strcmp(str1, str2)
#define PROMPT() printf("ftp> ")
#define PROMPT_STR(str) printf("ftp:%s> ", str)

struct ftp_state {
  char input_buffer[BUFF_SIZE];
  char read_buf[BUFF_SIZE];
  char *hostname;
  char *input;
  bool debug;
  bool loop;
  bool is_passive;
  struct addrinfo *infos;
  int control_fd;
  int data_fd;
  int last_code;
};
struct ftp_state ftp_state;

void init();
int read_user_input();
void handle_exit();
void handle_open();
void handle_debugon();
void handle_debugoff();
void handle_debug();

#endif
