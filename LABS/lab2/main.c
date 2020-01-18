/* 7 На вход программе подается название 2-ух файлов.
Необходимо отсортировать оба файла 
(каждый в отдельном процессе)
произвольной сортировкой(на усмотрение студента).
Содержимое обоих файлов вывести в стандартный поток
вывода родительским процессом. */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define READ 0
#define WRITE 1

void Swapping(char** str1, char** str2);
void SortingFile(const char* filename, int* tube);
int Checker(const char* filename);
int getStr(int* tube);

int main(int argc, char* argv[]) {
    int pipe1[2];
    int pipe2[2];
    pid_t proc1;
    pid_t proc2;
    int status;

    if (argc < 3) {
        printf("Usage: %s <file_1> <file_2>\n", argv[0]);
        exit(-1);
    }

    if (!Checker(argv[1]) || !Checker(argv[2])) {
        exit(EXIT_FAILURE);
    }
 
    if (pipe(pipe1) == -1) {
        printf("ERROR: Can't make pipe 1\n");
        exit(-1);
    }

    proc1 = fork();
    if (proc1 == 0) {
        close(pipe1[READ]);
        SortingFile(argv[1], pipe1);
        close(pipe1[WRITE]);
        exit(0);
    } else if (proc1 == -1) {
        perror("Can't make 1 child proccess\n");
        exit(-1);
    } else if (proc1 != 0) {
        if (pipe(pipe2) == -1) {
            printf("ERROR: Can't make pipe 2\n");
            exit(-1);
        }
        proc2 = fork();
        if (proc2 == 0) {
            close(pipe2[READ]);
            SortingFile(argv[2], pipe2);
            close(pipe2[WRITE]);
            exit(0);
        } else if (proc2 == -1) {
            perror("ERROR: Can't make 2 child proccess\n");
            exit(-1);
        }
    }
    if (waitpid(proc1, &status, 0) == -1) {
        perror("Waitpid\n");
    }
    if (waitpid(proc2, &status, 0) == -1) {
        perror("Waitpid\n");
    }

    close(pipe1[WRITE]);
    close(pipe2[WRITE]);
    while (getStr(pipe1));
    while (getStr(pipe2));
    close(pipe1[READ]);
    close(pipe2[READ]);
    
    return 0;
}


int Checker(const char* filename) {
    int file = open(filename, O_RDONLY);
    if (file == -1) {
        perror("ERROR:");
        printf("Can't open file '%s'\n", filename);
        return 0;
    }
    close(file);

    return 1;
}

void Swapping(char** str1, char** str2) {
    char* str = *str1;
    *str1 = *str2;
    *str2 = str;
}

void SortingFile(const char* filename, int* tube) {
    char symb;
    int i, j;
    int size = 0;
    int cnt = 0;
    char** str = NULL;
    int file = open(filename, O_RDONLY);
    
    lseek(file, 0, SEEK_SET);
    while((read(file, &symb, 1)) != 0) {
        if (symb == '\n') {
            size++;
        }
    }


    if (lseek(file, sizeof(size), SEEK_SET) > 0) {
        lseek(file, lseek(file, sizeof(size), SEEK_SET) - 1, SEEK_SET);
        read(file, &symb, 1);
        if (symb == '\n') {
            size--;
        }
    }
    if (size == 0) {
        close(file);
        printf("File is empty!\n");
        return;
    }

    str = (char**)malloc(sizeof(char*) * size);

    lseek(file, 0, SEEK_SET);

    for (i = 0; i < size; ++i) {
        cnt = 0;
        while (1) {
            read(file, &symb, 1);
            if (symb == '\n' || symb == EOF) {
                str[i] = (char*)malloc(sizeof(char) * (cnt + 1));
                break;
            }
            cnt++;
        }
    }

    lseek(file, 0, SEEK_SET);

    for (i = 0; i < size; ++i) {
        j = 0;
        while (1) {
            read(file, &symb, 1);
            if (symb == '\n' || symb == EOF) {
                break;
            }
            str[i][j++] = symb;
        }
        str[i][j] = '\0';
    }

    for (i = 0; i < size; ++i) {
        for (j = 0; j < size - i - 1; ++j) {
            if (strcmp(str[j], str[j + 1]) > 0) {
                Swapping(&str[j], &str[j + 1]);
            }
        }
    }

    for (i = 0; i < size; ++i) {
        cnt = strlen(str[i]) + 1;
        write(tube[WRITE], &cnt, sizeof(cnt));
        write(tube[WRITE], str[i], cnt);
        free(str[i]);
    }

    free(str);
    close(file);
}

int getStr(int* tube) {
    char* buf = NULL;
    int size = 0;
    
    if (read(tube[READ], &size, sizeof(size)) == 0)
        return 0;
    
    buf = (char*)malloc(sizeof(char) * size);
    read(tube[READ], buf, size);
    printf("%s\n", buf);
    
    free(buf);
    
    return 1;
}
