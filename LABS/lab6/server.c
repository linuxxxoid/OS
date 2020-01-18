#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/file.h>
#include <signal.h>

#include "/usr/local/include/zmq.h"
#include "bst.h"
#include "message.h"

volatile sig_atomic_t flag = 0;//единственным подходящим способом получить доступ к разделяемому объекту из обработчика сигнала было чтение или запись в переменные типа volatile sig_atomic_t

void unblock_func(int sig) {
    flag = 1;
}

int main(int argc, char *argv[]) {
    MSG* md;
    BST* root = NULL;
    BST* node = NULL;
    BST* friend = NULL;
    char info[LENGTH];
    client tmp;
    ID remCash;
    zmq_msg_t message;
    zmq_msg_t reply;

    void* context = zmq_ctx_new();//создает новый контекст. Он является потоко-безопасным.
    void* respondSocket = zmq_socket(context, ZMQ_REP);//создает сокет типа ZMQ_REP из контекста context
    char filename[27];
    char address[25];
    printf("Enter bank's address: ");
    ID bank;
    scanf("%d", &bank);
    sprintf(address, "%s%d", "tcp://*:", bank);//~printf, вывод в массив address

    int rc = zmq_bind(respondSocket, address);//связывает сокет с локальной конечной точкой и затем принимает входящие соединения на этой конечной точке
    if (rc != 0) {
        perror("zmq_bind\n");
        exit(FAILURE);
    }
    sprintf(filename, "db%d", bank);
    FILE* file = fopen(filename, "a+");
    fseek(file, 0, SEEK_SET);
    printf("~~~Loading DataBase~~~\n");
    Load(&root, file);
    fclose(file);
    printf("DataBase was loaded:\n");
    Inorder(root);
    putchar('\n');
    
    signal(SIGINT, unblock_func);
    while(!flag) {
        zmq_msg_init(&message);//инициализирует сообщ message как пустой объект

        zmq_msg_recv(&message, respondSocket, 0);//получает сообщ из respondSocket в message с параметрами flag (0 right now), возвращ кол-во получ байт, если ошибка то -1

        md = (MSG*)zmq_msg_data(&message);
        if (md->flagus != 1) {
            if ((node = Search(root, md->clientId)) == NULL) {
                printf("Recieved client id %d cash %d\n", md->clientId, md->cash);
                printf("Adding a new client %d\n", md->clientId);
                tmp.id = md->clientId;
                tmp.cash = md->cash;
                root = Insert(root, &tmp);
                printf("inorder of bst:\n");
                Inorder(root);
                putchar('\n');
                strcpy(info, "A new client was added\0");
            }
            md->flagus = 1;
        }
        switch (md->action) {
            case 1: {
                printf("Put money into the account id: %d\n", md->clientId);
                node->key.cash += md->cash;
                Inorder(root);
                strcpy(info, "Operation was completed successfully\0");
                break;
            }
            case 2: {
                printf("Get money from the account id: %d\n", md->clientId);
                remCash = md->cash;
                if (remCash <= node->key.cash) {
                    node->key.cash -= remCash;
                    printf("Success\n");
                    Inorder(root);
                    strcpy(info, "Operation was completed successfully\0");
                } else {
                    printf("Not enough money\n");
                    strcpy(info, "Not enough money\0");
                }
                break;
            }
            case 3: {
                printf("Send money from account id: %d to account id: %d\n", md->clientId, md->receiverClient);
                remCash = md->cash;
                friend = Search(root, md->receiverClient);
                if (friend) {
                    if (remCash <= node->key.cash) {
                        node->key.cash -= remCash;
                        friend->key.cash += remCash;
                        printf("Success\n");
                        Inorder(root);
                        strcpy(info, "Operation was completed successfully\0");
                    } else {
                        printf("Not enough money\n");
                        strcpy(info, "Not enough money\0");
                    }
                } else {
                    printf("Receiver is not bank client\n");
                    strcpy(info, "Receiver is not a client of bank\0");
                }
                break;
            }
            case 4: {
                printf("Check account id: %d\n", md->clientId);
                node = Search(root, md->clientId);
                if (node) {
                    printf("Client balance is %d\n", node->key.cash);
                    sprintf(info, "ID %d CASH %d\n", node->key.id, node->key.cash);
                } else {
                    printf("%d is not a bank client\n", md->clientId);
                    strcpy(info, "You aren't client of bank\0");
                }
                break;
            }
            case 11: {
                printf("Turn off the server\n");
                strcpy(info, "Server is turned off\0");
                md->requester = respondSocket;
                printf("Saving TreeDataBase\n");
                Inorder(root);
                putchar('\n');
                printf("Saved TreeDataBase\n");
                flag = 1;
                break;
            }
            case 12: {
                break;
            }
            default: {
                break;
            }
        }
        zmq_msg_close(&message);//очищает содержимое message, аналог free для сообщений zmq, 0 luck, -1 err
        zmq_msg_init_size(&reply, strlen(info)+1);
        memcpy(zmq_msg_data(&reply), info, strlen(info)+1);
        //zmq_msg_data - получить указатель на содержимое сообщения
        zmq_msg_send(&reply, respondSocket, 0);//отправляет содержимое reply в сокет respondSocket с параметрами flags, возвр кол-во отправл байт, в случае ошибки -1
        zmq_msg_close(&reply);
    }
    FILE* f = fopen(filename, "w+");
    Save(root, file);
    fclose(f);
    zmq_close(respondSocket);//закрывает сокет respondSocket,возвращ 0 в случае успеха и -1 в случае неудачи
    zmq_ctx_destroy(context);//разрушает контекст context, блокирует доступ всем операциям кроме zmq close,все сообщ либо физич отправлены, либо висят.
    Clear(root);
    printf("~~~Finished work~~~\n");
    
    return 0;
}
