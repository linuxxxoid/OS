#include "queue.h"

void Create(Queue* q) {
    q->head = (ItemQueue*)malloc(sizeof(ItemQueue));
    q->tail = q->head;
    q->size = 0;
}

void Pop(Queue* q) {
    if (q->head == q->tail) { printf("Queue is empty!\n"); }
    else if (q->size != 0) {
        ItemQueue* tmp = q->head;
        q->head = q->head->next;
        --q->size;
        free(tmp);
    }
}

void Push(Queue* q, long value) {
    if (Empty(q)) {
        Create(q);
    }
    ItemQueue* tmp = (ItemQueue*)malloc(sizeof(ItemQueue));
    q->tail->next = tmp;
    q->tail->data = value;
    q->tail = q->tail->next;
    ++q->size;
    q->tail->next = NULL;
}

int Front(Queue* q) {
    if (q->head != q->tail) {
        return printf("FRONT ELEMENT: %d\n",q->head->data);
    }
    else return EXIT_FAILURE;
}

int Size(Queue* q) { return printf("SIZE: %d\n", q->size); }

bool Empty(Queue* q) {
    return q->head == q->tail;
}

void Print(Queue* q) {
    if (q->size != 0) {
        ItemQueue* cur = q->head;
        printf("QUEUE:\n");
        while (cur != q->tail) {
            printf("%d\n", cur->data);
            cur = cur->next;
        }
    }
    else { printf("Queue is empty!\n"); }
}

void Clear(Queue* q) {
    while (q->size != 0) {
        Pop(q);
    }
    free(q->head);
    q->head = q->tail = NULL;
}



