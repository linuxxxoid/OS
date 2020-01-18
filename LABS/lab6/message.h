#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#define _XOPEN_SOURCE 700
#include <inttypes.h>

#define LENGTH 256
#define SUCCESS 0
#define FAILURE 1

typedef int32_t ID;

typedef struct _msg {
    ID clientId;
    int cash;
    int action;
    ID receiverClient;
    void *requester;
    int flagus;
} MSG;

#endif
