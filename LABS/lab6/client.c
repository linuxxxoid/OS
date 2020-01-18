#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/types.h>


#include "/usr/local/include/zmq.h"
#include "message.h"

void menuAdmin() {
    printf("1) Turn off the server\n");
    printf("2) Exit\n");
}

void menuClient() {
    printf("1) Add  money to your account\n");
    printf("2) Get money from your accout\n");
    printf("3) Send money to another account\n");
    printf("4) Check the balance\n");
    printf("5) Exit\n");
}

int main(int argc, char *argv[]) {
    char info[256];
    zmq_msg_t clientReq;
    zmq_msg_t reply;
    MSG md;
    ID admin = 0;
    int flag = 0;
    void* context = zmq_ctx_new();
    
    char address[25];
    printf("Enter a bank's address: ");
    ID bank;
    scanf("%d", &bank);
    sprintf(address, "%s%d", "tcp://localhost:", bank);
    printf("tcp://localhost:%d\n", bank);

    void* sendSocket = zmq_socket(context, ZMQ_REQ);
    int rc = zmq_connect(sendSocket, address);

    if (rc == 0) {
        printf("Sucessfully connected\n");
    } else {
        perror("zmq_connect\n");
        zmq_close(sendSocket);
        zmq_ctx_destroy(context);
        exit(FAILURE);
    }

    if (argc == 2 && strcmp(argv[1], "admin") == 0) {
        admin = 1;
    } else {
        printf("Enter your client id:\n");
        scanf("%d", &md.clientId);
    }
    
    if (admin == 1) {
        int act = 0;
        menuAdmin();
        do {
            scanf("%d", &act);
            MSG md;
            md.action = act + 10;
            md.flagus = 1;
            switch (act) {
                case 1: {
                    md.requester = sendSocket;
                    break;
                }
                case 2: {
                    break;
                }
                default: {
                    printf("ERROR: Inccorect command\n");
                    break;
                }
            }
            zmq_msg_init_size(&clientReq, sizeof(MSG));
            memcpy(zmq_msg_data(&clientReq), &md, sizeof(MSG));
            zmq_msg_send(&clientReq, md.requester, 0);
            zmq_msg_close(&clientReq);
            
            zmq_msg_init(&reply);
            zmq_msg_recv(&reply, sendSocket, 0);//receive a message part from a socket
            strcpy(info, (char*)zmq_msg_data(&reply));
            if (strcmp(info, "Server is turned off") == 0) {
                printf("Good bye\n");
                break;
            }
            zmq_msg_close(&reply);
        } while (act != 2);
     } else {
         zmq_msg_init_size(&clientReq, sizeof(MSG));
         memcpy(zmq_msg_data(&clientReq), &md, sizeof(MSG));
         printf("Checking for user %d\n", md.clientId);
         zmq_msg_send(&clientReq, sendSocket, 0);
         zmq_msg_close(&clientReq);
         
         zmq_msg_init(&reply);
         zmq_msg_recv(&reply, sendSocket, 0);//receive a message part from a socket
         strcpy(info, (char*)zmq_msg_data(&reply));
         
         if (strcmp(info, "A new client was added") == 0) {
             printf("Operation went sucessfully\n");
         }
         zmq_msg_close(&reply);
         
         flag = 1;
         ID remCash, friendId;
         int act;
        while(1) {
            menuClient();
            scanf("%d", &act);
            md.action = act;
            if (act == 1) {
                printf("Enter the sum: ");
                scanf("%d", &remCash);
                md.cash = remCash;
                md.requester = sendSocket;
            } else if (act == 2) {
                printf("Enter the sum: ");
                scanf("%d", &remCash);
                md.cash = remCash;
                md.requester = sendSocket;
            } else if (act == 3) {
                printf("Enter receiver id: ");
                scanf("%d", &friendId);
                printf("Enter the sum: ");
                scanf("%d", &remCash);
                md.cash = remCash;
                md.receiverClient = friendId;
                md.requester = sendSocket;
            } else if (act == 4) {
                md.requester = sendSocket;
            } else if (act == 5) {
                break;
            } else {
                printf("ERROR: Inccorect command\n");
                continue;
            }
      
            zmq_msg_init_size(&clientReq, sizeof(MSG));
            memcpy(zmq_msg_data(&clientReq), &md, sizeof(MSG));
            zmq_msg_send(&clientReq, md.requester, 0);
            zmq_msg_close(&clientReq);
            
            zmq_msg_init(&reply);
            zmq_msg_recv(&reply, sendSocket, 0);//receive a message part from a socket
            strcpy(info, (char*)zmq_msg_data(&reply));

            if (strcmp(info, "Operation was completed successfully") == 0) {
                printf("Operation went sucessfully\n");
            }
            else if (strncmp(info, "ID", 2) == 0) {
                sscanf(info, "ID %d CASH %d", &friendId, &remCash);
                printf("Your stats are:\n id - %d\tcash amount - %d\n", friendId, remCash);
            } else if (strcmp(info, "Not enough money") == 0) {
                printf("You don't have enough money\n");
            } else if (strcmp(info, "Receiver is not a client of bank") == 0) {
                printf("This client doesn't use options of our bank\n");
            } else if (strcmp(info, "You aren't client of bank") == 0) {
                printf("You are not a client of our bank\n");
                exit(FAILURE);
            } else {
                printf("Zzz...\n");
            }
            zmq_msg_close(&reply);
        }
    }
    printf("~~~Thanks for using our bank!~~~\n");
    zmq_close(sendSocket);
    zmq_ctx_destroy(context);
    return 0;
}


