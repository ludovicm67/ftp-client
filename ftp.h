#ifndef __FTP_H
#define __FTP_H

#include <stdbool.h>

#define BUFF_SIZE 1024
#define STR_EQ(str1, str2) !strcmp(str1, str2)

struct ftp_state {
  char input_buffer[BUFF_SIZE];
  char *input;
  bool debug;
  bool loop;
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
