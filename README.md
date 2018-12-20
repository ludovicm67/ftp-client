FTP client
==========

Developped by Ludovic Muller.

This FTP client supports IPv4 and IPv6 connections.

Supported commands:
  - `debug` : turn on or off server answers
  - `debugon` : turn on server answers
  - `debugoff` : turn off server answers
  - `exit` : close sockets and quit the program
  - `open` : open to a ftp server
  - `open [server]` : open to a ftp server
  - `open [server] [port]` : open to a ftp server on a specific port
  - `user` : prompt for user credntials; supports USER, PASS and ACC.
  - `passiveon` : connection to passive mode
  - `passiveoff` : connection to active mode
  - `ciao` : close the current connection
  - `dir` : list current directory content
