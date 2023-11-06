#ifndef _SUBCONTRACT_H
#define _SUBCONTRACT_H
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>

#define CONNECT_OK 100
#define NO_CONNECT 120
#define WAIT_CONNECT 110

void UdpServer1_th();
void UdpServer2_th();
void wait_server();
#endif // !_SUBCONTRACT_H