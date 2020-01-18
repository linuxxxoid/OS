#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <inttypes.h>
#include <errno.h>

#define BUFSIZE 256

int main(int argc, const char * argv[]) {
    const char *str = "Nihao!Hello!Chao!";
    char buf[BUFSIZE];
    char bufTrunc[BUFSIZE];
    int fd;
    ssize_t wr, rd;
    
    fd = creat("file1", O_RDWR); //create a file called file1 for reading&writing
    if (fd == -1)
        perror("Error CREAT!\n"); //if file descriptor return -1 then it's error
    else {
        fprintf(stdout, "file1: read/write\n");
        fprintf(stdout, "file1 is empty\n");
    }
    if (fsync(fd) == -1) // fsync - Synchronize a file's in-core state with storage
        fprintf(stdout, "Fail file1 synchronize\n");
    else fprintf(stdout, "Synchronize file1 was succesfull\n");
    
    //write in file1
    if ((wr = write(fd, str, strlen(str))) == -1)
        perror("Cannot write in 'file1'\n");
    
    if (lseek(fd, 0, SEEK_SET) == -1)
        perror("Error lseek\n"); //изменение позиции ptr файлового дескриптора
    if (close(fd) == -1)
        perror("Error closing 'file1'\n");
    
    fd = open("file1", O_RDWR);
    if (read(fd, buf, strlen(str)) == strlen(str)) { 
    /*read() пытается записать strlen(str) байтов
     файлового описателя fd в буфер,
      адрес которого начинается с buf. */
        fprintf(stdout, "%s", buf);
        fprintf(stdout, "was succesfully written\n");
    }
    else
        perror("Cannot read 'file1'\n");
    
    if (truncate("file1", 4) == -1) { //укорачивает файл до указанной длины
        perror("Error truncate\n");
    }
    
    fd = open("file1", O_RDONLY);
    rd = read(fd, bufTrunc, BUFSIZE);
    if (rd == -1 && errno == EINTR) {
        perror("Error read");
    }
    fprintf(stdout, "String after trunc: ");
    fprintf(stdout, "%s\n", bufTrunc);
    if (close(fd) == -1) {
        perror("Error close");
    }
    
    int type;
    struct stat st;
    fd = open("file1", O_RDONLY);
    //stat возвращаeт информацию об указанном файле
    if (stat("file1", &st) < 0) {
        fprintf(stdout, "%s doesn't exist\n", "file1");
        return 0;
    }
    printf("file1 has %d names\n", st.st_nlink);
    switch(type = (st.st_mode & S_IFMT)){
        case S_IFREG:
            fprintf(stdout, "Size of ordinary file %lld bytes\n", st.st_size );
            break;
        case S_IFDIR:
            fprintf(stdout, "Directory\n" );
            break;
        case S_IFCHR:   // байтоориентированное
        case S_IFBLK:   //блочноориентированное
            fprintf(stdout, "system/device\n" );
            break;
        case S_IFIFO:
            fprintf(stdout, "FIFO-file\n" );
            break;
        default:
            fprintf(stdout, "Another type\n" );
            break;
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error fork!\n");
    }
    if (pid == 1) {
        fprintf(stdout, "PARENT: I'm a parent proccess!\n");
        fprintf(stdout, "PARENT: MY PID = %d\n", getpid());
        fprintf(stdout, "PARENT: PID of my child = %d\n", getppid());
        wait(NULL);
        fprintf(stdout, "PARENT: Exit!\n");
    }
    if (pid == 0) {
        fprintf(stdout, "CHILD: It's a child process!\n");
        fprintf(stdout, "CHILD: My PID = %d\n", getpid());
        fprintf(stdout, "CHILD: PID of my parent =  %d\n", getppid());
    }

//born 3 proccess which are calling 'echo' program using execl
    
    if (fork() == 0) {
        execl("/bin/echo", "echo", "this is", "message one", (char *) 0);
        perror("exec one failed\n");
        exit(1);
    }
    if (fork() == 0) {
        execl("/bin/echo", "echo", "this is", "message two", (char *) 0);
        perror("exec two failed\n");
        exit(2);
    }
    if (fork() == 0) {
        execl("/bin/echo", "echo", "this is", "message three", (char *) 0);
        perror("exec three failed\n");
        exit(3);
    }
    printf("Parent program ending\n");

/* read&write or create 'newfile' if it doesn't exist
0644 = user can + group can + others can
user can: 00400 read     +      00200 write
        S_IRUSR (S_IREAD)       S_IWUSR (S_IWRITE)
group can: 00040 read + 00020 write
            S_IRGRP     S_IWGRP
others can: 00004 read only (S_IROTH) */
    const char *line = "Hello there =^-^=\n";
    if ((fd = open("/Users/linuxoid/Desktop/OS1/new", O_RDWR | O_CREAT, 0644)) == -1) {
        perror("Error opening\n");
    }
    fprintf(stdout, "...Making creation of a new file...\n");
    fd = creat("new", O_RDWR);
    if (write(fd, line, strlen(line)) == strlen(line))
        fprintf(stdout, "Succesfully written: '%s'\n", line);
    close(fd);

    return 0;
}
