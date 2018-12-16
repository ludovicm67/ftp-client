#ifndef __FTP_H
#define __FTP_H

#include <stdbool.h>

#define BUFF_SIZE 1024
#define STR_EQ(str1, str2) !strcmp(str1, str2)

struct ftp_state {
  char input_buffer[BUFF_SIZE];
  bool debug;
  bool loop;
};
struct ftp_state ftp_state;

#endif
