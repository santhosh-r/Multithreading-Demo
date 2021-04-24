//
//  cons.c
//  
//
//  Created by Jinyu T. and Santhosh R.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define CHUNK 1024

int main(int argc, char *argv[]) {
    // using pipe comunicates between processes
    int fd[2];
    pid_t p;
    char buf[CHUNK];
    size_t nread;
    FILE *fptr;
    // get filename and its filepath
    char *fname = argv[1];
    char *path = realpath(fname,NULL);

    fptr = fopen(fname,"r");
    if (argc < 3) {
        fprintf(stderr,"ERROR, a character needed\n");
        exit(EXIT_FAILURE);
    }
    
    if (pipe(fd) == -1) {
        fprintf(stderr, "Pipe Failed");
        exit(EXIT_FAILURE);
    }
    
    printf("\nfile name is: %s,\nfile path is: %s\n replace character is: %s \n\n",fname,path,argv[2]);
    
    p = fork();
    // use child process to exec producer file
    
    if (p == 0){
        close(fd[0]);
        // redirect STDOUT_FILENO to fd[1], that contains results from producer
        dup2(fd[1], STDOUT_FILENO);
        execl("proc", "proc", path, argv[2], NULL);
        perror("exec failed");
        exit(EXIT_FAILURE);
    }
    
    //use parent process to get result form producer file and print
    wait(NULL);
    close(fd[1]);
    dup2(fd[0], STDIN_FILENO);
    fgets(buf, CHUNK, stdin);
    if ((fptr = fopen(buf, "r")) == NULL) {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }
    // writting to file
    while ((nread = fread(buf,1,CHUNK,fptr)) > 0)
        fwrite(buf, 1, nread, stdout);
    exit(0);
}
