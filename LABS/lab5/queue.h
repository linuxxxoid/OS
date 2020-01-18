#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static const int STR = 12;
static const int MIN = -2147483648;
static const int MAX = 2147483647;

typedef struct ItemQueue {
    int data;
    struct ItemQueue* next;
} ItemQueue;

typedef struct Queue {
    ItemQueue* head;
    ItemQueue* tail;
    int size;
} Queue;

    extern void Create(Queue*);
    extern void Push(Queue*, long);
    extern void Pop(Queue*);
    extern int Front(Queue*);
    extern bool Empty(Queue*);
    extern int Size(Queue*);
    extern void Print(Queue*);
    extern void Clear(Queue*);

#endif
