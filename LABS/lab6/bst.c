#include "bst.h"

BST* Insert(BST* root, client* person) {
    if (root == NULL) {
        BST* node = (BST*) malloc(sizeof(BST));//goto
        if (!node) {
            fprintf(stderr, "ERROR: bad allocation!\n");
            exit(FAILURE);
        }
        node->left = NULL;
        node->right = NULL;
        node->key.id = person->id;
        node->key.cash = person->cash;
        
        return node;
    }
    
    if (person->id == root->key.id) {
        return root;
    } else if (root->key.id < person->id) {
        root->left = Insert(root->left, person);
    } else if (root->key.id > person->id) {
        root->right = Insert(root->right, person);
    }
    
    return root;
}

BST* Search(BST* root, int32_t clientId) {
    if (!root) {
        return root;
    }
    if (root->key.id == clientId) {
        return root;
    } else if (root->key.id < clientId) {
        return Search(root->left, clientId);
    } else if (root->key.id > clientId) {
        return Search(root->right, clientId);
    }
    
    return NULL;
}

BST* Remove(BST* root, int32_t clientId) {
    if (!root) {
        return root;
    }
    
    BST* tmp;
    if (root->key.id == clientId) {
        if (!root->left) {
            tmp = root->right;
            free(root);
            root = NULL;
            return tmp;
        } else if (!root->right) {
            tmp = root->left;
            free(root);
            root = NULL;
            return tmp;
        }
        tmp = root->right;
        
        while (tmp->left) {
            tmp = tmp->left;
        }
        
        root->right = Remove(root->right, clientId);
    } else if (root->key.id < clientId) {
        root->right = Remove(root->right, clientId);
    } else if (root->key.id > clientId) {
        root->left = Remove(root->left, clientId);
    }
    
    return root;
}
void Inorder(BST* root) {
    if (root != NULL) {
        Inorder(root->left);
        printf("%d %d|", root->key.id, root->key.cash);
        Inorder(root->right);
    }
}

void Clear(BST* root) {
    if (root != NULL) {
        Clear(root->left);
        Clear(root->right);
        free(root);
        root = NULL;
    }
}

void Save(BST* root, FILE* file) {
    if (root != NULL) {
        char tmp[64];
        sprintf(tmp, "%d %d ", root->key.id, root->key.cash);
        fwrite(tmp, sizeof(char), strlen(tmp), file);
        Save(root->left, file);
        Save(root->right, file);
    }
}

void Load(BST** root, FILE* file) {
    client tmp;
    while (fscanf(file, "%d %d ", &tmp.id, &tmp.cash) == 2) {
        *root = Insert(*root, &tmp);
    }
}
