/*
 =============variant: 25
 ====internal storage: 3 - Binary Tree, where the key - client id
 ============key type: 1 - int 32-bit
 =additional features: 1 - Saving data on customer accounts when server shutdown and renewal
 Сохранение данных о счетах клиентов при завершении работы сервера и возобновлении*/

#ifndef BST_H
#define BST_H

#define LENGTH 256
#define SUCCESS 0
#define FAILURE 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>


typedef struct _client {
    int32_t id;
    int cash;
} client;

typedef struct _bst {
    struct _bst* left;
    struct _bst* right;
    client key;
} BST;

extern BST* Insert(BST* root, client* person);
extern BST* Search(BST* root, int32_t clientId);
extern BST* Remove(BST* root, int32_t clientId);
extern void Inorder(BST* root);
extern void Clear(BST* root);
extern void Save(BST* root, FILE* file);
extern void Load(BST** root, FILE* file);

#endif
