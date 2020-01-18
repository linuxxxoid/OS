/* 7 На вход программе подается название 2-ух файлов.
Необходимо отсортировать оба файла
(каждый в отдельном процессе)
произвольной сортировкой(на усмотрение студента).
Содержимое обоих файлов вывести в стандартный поток
вывода родительским процессом. 

Составить и отладить программу на языке Си, 
осуществляющую работу с процессами и взаимодействие
между ними в одной из двух операционных систем.
В результате работы программа (основной процесс)
должен создать для решение задачи один или несколько
дочерних процессов. Взаимодействие между процессами
осуществляется через системные сигналы/события и/или
через отображаемые файлы (memory-mapped files).
Необходимо обрабатывать системные ошибки,
которые могут возникнуть в результате работы.*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>


#define handle_error(msg)                             \
do { perror(msg); exit(EXIT_FAILURE); } while (0)

void Swapping(char** str1, char** str2);
void Sorting(int fd, int size, int line);
void Printer(int fd, int size);

int main(int argc, char* argv[]) {
    pid_t proc1, proc2;
    int status;
    int file1, file2;
    struct stat statbuf1;
    struct stat statbuf2;
    char *first, *second;

    if (argc != 3) {
        printf("Usage: %s <file_1> <file_2>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((file1 = open(argv[1], O_RDWR)) == -1) {
        perror("ERROR:");
        printf("Can't open file '%s'\n", argv[1]);
    }
    if (fstat(file1, &statbuf1) < 0 ) { // Define file size
        perror("fstat1 error");
        exit(EXIT_FAILURE);
    }
    int file_size1 = statbuf1.st_size;

    if ((file2 = open(argv[2], O_RDWR)) == -1) {
        perror("ERROR:");
        printf("Can't open file '%s'\n", argv[2]);
    }
    if (fstat(file2, &statbuf2) < 0 ) {
        perror("fstat2 error");
        exit(EXIT_FAILURE);
    }
    int file_size2 = statbuf2.st_size;

    first = (char*)mmap(NULL, file_size1, PROT_READ | PROT_WRITE, MAP_SHARED, file1, 0);
    if (first == MAP_FAILED) {
        handle_error("mmap");
    }

    int count1 = file_size1 / sizeof(char); // Number of elements in file
    int linecnt1 = 0;
    for (int i = 0; i < count1; ++i) {
    	if (first[i] == '\n') {
    		++linecnt1;
    	}
    }
    munmap(first, file_size1);

    proc1 = fork();
    if (proc1 == 0) {
        Sorting(file1, file_size1, linecnt1);
        exit(EXIT_SUCCESS);
    } else if (proc1 == -1) {
        perror("Can't make 1 child proccess\n");
        exit(EXIT_FAILURE);
    } else if (proc1 != 0) {
        second = (char*)mmap(NULL, file_size2, PROT_READ | PROT_WRITE, MAP_SHARED, file2, 0);
        if (second == MAP_FAILED) {
            handle_error("mmap");
        }

        int count2 = file_size2 / sizeof(char);
        int linecnt2 = 0;
        for (int i = 0; i < count2; ++i) {
            if (second[i] == '\n') {
                ++linecnt2;
            }
        }
        munmap(second, file_size2);
        proc2 = fork();
        if (proc2 == 0) {
            Sorting(file2, file_size2, linecnt2);
            exit(EXIT_SUCCESS);
        } else if (proc2 == -1) {
            perror("ERROR: Can't make 2 child proccess\n");
            exit(EXIT_FAILURE);
        }
    }

    if (waitpid(proc1, &status, 0) == -1) {
        perror("Waitpid\n");
    }
    if (waitpid(proc2, &status, 0) == -1) {
        perror("Waitpid\n");
    }

    Printer(file1, file_size1);
    close(file1);
    Printer(file2, file_size2);
    close(file2);

    return 0;
}

void Swapping(char** str1, char** str2) {
    char *str = *str1;
    *str1 = *str2;
    *str2 = str;
}

void Printer(int fd, int size) {
    int offset = 0;
    char *tmp = (char*)mmap(NULL, (size_t)size, PROT_READ, MAP_PRIVATE, fd, offset);
    if (tmp == MAP_FAILED) {
        handle_error("mmap");
    }

    for (int i = 0; i < size; ++i) {
        putchar(tmp[i]);
    }

    if (munmap(tmp, size) == -1) {
       handle_error("munmap");
    }   
}

void Sorting(int fd, int size, int line) {
    int offset = 0;
    int cnt = 0;
    char **buf = (char**)malloc(sizeof(char*) * line);
    char *tmp = (char*)mmap(NULL, (size_t)size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
    if (tmp == MAP_FAILED) {
        handle_error("mmap");
    }

    for (int j = 0; j < line; ++j) {
        for (int i = 0; i < size; ++i) {
            if (tmp[i] == '\n' || tmp[i] == EOF) {
                buf[j] = (char*)malloc(sizeof(char) * (cnt + 1));
                break;
            }
            cnt++;
        }
    }

    int k;
    int n = -1;
    for (int j = 0; j < line; ++j) { 
        k = 0;     
        while(n < size) {
            ++n;
            if (tmp[n] == '\n' || tmp[n] == EOF) {
                break;
            }
            buf[j][k++] = tmp[n];
        }
        buf[j][k] = '\0';
    }

    for (int i = 0; i < line; ++i) {
        for (int j = 0; j < line - i - 1; ++j) {
            if (strcmp(buf[j], buf[j + 1]) > 0) {
                Swapping(&buf[j], &buf[j + 1]);
            }
        }
    }

    int m = 0;
    for (int j = 0; j < line; ++j) { 
        k = 0;
        while (k < strlen(buf[j]) && m < size) {
            tmp[m] = buf[j][k];
            ++m;
            ++k;
        }
       tmp[m++] = '\n';
    }

    if (msync(tmp, size, MS_SYNC) == -1) {
        handle_error("msync");
    }

    if (munmap(tmp, size) == -1) {
       handle_error("munmap");
    }

    for (int i = 0; i < line; ++i) {
        free(buf[i]);
    }

    free(buf);
}
