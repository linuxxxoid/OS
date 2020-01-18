#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "queue.h"

int main(int argc, const char * argv[]) {
    Queue q;
    Create(&q);
    int choice;
    long value;
    bool flag = true;
    char str[STR];
    printf("=^-^= COMPILE-TIME LINKING =^-^=\n");
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
                                Push(&q, value);
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
                Pop(&q);
                break;
            }
            case 3: {
                Size(&q);
                break;
            }
            case 4: {
                Print(&q);
                break;
            }
            case 5: {
                Front(&q);
                break;
            }
            case 6: {
                Clear(&q);
                break;
            }
            case 0: {
                Clear(&q);
                break;
            }
            default: {
                printf("Invalid parameter!\n");
                break;
            }
        }
    } while (choice != 0);
    return 0;
}
