#ifndef TINYWEB_H
#define TINYWEB_H

#include <netinet/in.h>

typedef struct sockaddr SA;

int open_listenfd(int port);

char *process(int fd, struct sockaddr_in *clientaddr);

#endif