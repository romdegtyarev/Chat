#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <sys/msg.h>
#include <pthread.h> 
#include <netdb.h>
#include <string.h>
#include <sys/select.h>


#define LEN       1024
#define h_addr    h_addr_list[0]
#define RED       "\033[31m"
#define GREEN     "\033[32m"
#define WHITE     "\033[0m"


struct message {
    char    type[256];
    char    data[LEN];
};


void serverFunction(int);


