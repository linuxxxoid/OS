#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dlfcn.h>
#include "queue.h"

int main(int argc, const char * argv[]) {
    char* error;
    
    void (*Create)(Queue* q);
    void (*Push)(Queue* q, long value);
    void (*Pop)(Queue* q);
    int (*Front)(Queue* q);
    bool (*Empty)(Queue* q);
    int (*Size)(Queue* q);
    void (*Print)(Queue* q);
    void (*Clear)(Queue* q);
    
    void *libHandle = dlopen("libqueue.so", RTLD_LAZY);
    if (!libHandle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    
    Create = dlsym(libHandle, "Create");
    Push = dlsym(libHandle, "Push");
    Pop = dlsym(libHandle, "Pop");
    Front = dlsym(libHandle, "Front");
    Empty = dlsym(libHandle, "Empty");
    Size = dlsym(libHandle, "Size");
    Print = dlsym(libHandle, "Print");
    Clear = dlsym(libHandle, "Clear");
    
    if(error = dlerror()) {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }
    Queue q;
    (*Create)(&q);
    int choice;
    long value;
    bool flag = true;
    char str[STR];
    printf("=^-^= RUNTIME LINKING =^-^=\n");
    do {
        printf("You're gonna work with QUEUE!\n");
        printf("Choose a parameter: \n");
        printf("1) INSERT AN ELEMENT\n");
        printf("2) DELETE THE ELEMENT\n");
        printf("3) SIZE OF A QUEUE\n");
        printf("4) PRINT THE QUEUE\n");
        printf("5) GET THE FIRST ELEMENT\n");
        printf("6) CLEAR THE QUEUE\n");
        printf("0) Exit\n");
        scanf("%d", &choice);
        switch (choice) {
            case 1: {
                do {
                    printf("Enter the value: \n");
                    scanf("%s", str);
                    int length = strlen(str);
                    flag = true;
                    if (length > 0 && length < STR) {
                        for (int i = 0; i < length; ++i) {
                            if (str[i] == '-') continue;
                            if (!isdigit(str[i])) {
                                flag = false;
                                printf("Invalid input. Try again!\n");
                                break;
                            }
                        }
                        if (flag) {
                            value = atol(str);
                            if (value >= MIN && value <= MAX) {
                                (*Push)(&q, value);
                            }
                            else {
                                printf("Too big for INT. Enter another number!\n");
                                flag = false;
                            }
                        }
                    }
                    else {
                        printf("Too big for INT. Enter another number!\n");
                        flag = false;
                    }
                } while(!flag);
                break;
            }
            case 2: {
                (*Pop)(&q);
                break;
            }
            case 3: {
                (*Size)(&q);
                break;
            }
            case 4: {
                (*Print)(&q);
                break;
            }
            case 5: {
                (*Front)(&q);
                break;
            }
            case 6: {
                (*Clear)(&q);
                break;
            }
            case 0: {
                (*Clear)(&q);
                break;
            }
            default: {
                printf("Invalid parameter!\n");
                break;
            }
        }
    } while (choice != 0);
    
    dlclose(libHandle);
    return 0;
}
