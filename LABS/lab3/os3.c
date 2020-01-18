#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
//#include <malloc.h>

pthread_mutex_t mutex;

typedef struct {
    int pow1;
    int pow2;
    int* array1;
    int* array2;
    int* result;
} plnmData;

void* threadFunc(void* thread_data) {
    plnmData *data = (plnmData*) thread_data;
    pthread_mutex_lock(&mutex);

    for(int i = 0; i < data->pow1 + 1; ++i) {
        for(int j = 0; j < data->pow2 + 1; ++j) {
            data->result[i + j] += data->array1[i] * data->array2[j];
        }
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, char* argv[]) {
    int power, power2, quantity, i, j;
    int *res;
    int* polinom;
    int flag = 0;

    if (argc != 2) {
        printf("Usage: %s <quantity of polinoms>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    quantity = atoi(argv[1]);
    if (quantity <= 1) {
        printf("ERROR: Enter quantity of polinoms should be > 1\n");
        exit(EXIT_FAILURE);
    }

    int thrd = quantity - 1;
    printf("Quantity of threads = %d\n", thrd);

    pthread_t* threads = (pthread_t*)malloc(quantity * sizeof(pthread_t));
    plnmData* threadData = (plnmData*)malloc(quantity * sizeof(plnmData));

    printf("Enter power: ");
    scanf("%d", &power);
    polinom = (int*) malloc((power + 1) * sizeof(int));
    printf("\nEnter coefficients: ");
    for (i = power; i >= 0; --i) {
        scanf("%d", &polinom[i]);
    }
    printf("\n");
    for (i = power; i >= 0; --i) {
        printf("%d", polinom[i]);
        if (i == 0) {
            printf("x^%d", i);
            break;
        }
        printf("x^%d + ", i);
    }
    printf("\n");
    pthread_mutex_init(&mutex, NULL);
    for (j = 0; j < thrd; ++j) {
        printf("\nEnter power: ");
        scanf("%d", &power2);
        int* polinom2 = (int*) malloc((power2 + 1) * sizeof(int));
        printf("\nEnter coefficients: ");
        for (i = power2; i >= 0; --i) {
            scanf("%d", &polinom2[i]);
        }
        printf("\n");
        for (i = power2; i >= 0; --i) {
            printf("%d", polinom2[i]);
            if (i == 0) {
                printf("x^%d", i);
                break;
            }
            printf("x^%d + ", i);
        }
        int size = power + power2;
        if (flag == 0) {
            res = (int*) malloc((size + 1) * sizeof(int));
            flag = 1;
        }
        else {
            int* tmp = (int*) realloc(res, (size + 1) * sizeof(int));
            res = tmp;
        }
        for (i = size; i >= 0; --i) {
            res[i] = 0;
        }

        threadData[j].pow1 = power;
        threadData[j].pow2 = power2;
        threadData[j].array1 = polinom;
        threadData[j].array2 = polinom2;
        threadData[j].result = res;

        pthread_create(&(threads[j]), NULL, threadFunc, &threadData[j]);
        pthread_join(threads[j], NULL);

        power = power + power2;
        int* tmp = (int*) realloc(polinom, (power + 1) * sizeof(int));
        polinom = tmp;
        for (i = power; i >= 0; --i) {
            polinom[i] = threadData[j].result[i];
        }
        free(polinom2);
    }
    pthread_mutex_destroy(&mutex);
    printf("\n\nAnswer:\n\n");
    for (i = power; i >= 0; --i) {
        printf("%d", polinom[i]);
        if (i == 0) {
            printf("x^%d", i);
            break;
        }
        printf("x^%d + ", i);
    }
    printf("\n");
    free(threads);
    free(threadData);

    return 0;
}
