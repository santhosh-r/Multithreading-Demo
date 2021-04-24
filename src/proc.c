//
//  proc.c
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
#include <pthread.h>
#include <ctype.h>
#include <semaphore.h>
#include "queue.h"

int bufferSize = 10;

sem_t semaphores[3];
Queue *queues[3];
// done_flags used to indicate when a thread has finished executing.
// prevent a situation that some of thread been stucked. and writer empty the queue
// program will destroy the queue and finish processing
int done_flags[4] = { 0 };
long linesProcessed[4] = { 0 };

char *inputFilename;
char outputFilename[255];
char replacement;

//methods for threads
void *reader(void *);
void *character(void *);
void *toUpper(void *);
void *writer(void *);
void *(*threadRoutines[4])(void *) = {reader, character, toUpper, writer};


int main(int argc, char *argv[]) {
    // demo mode shows lines processed for each thread
    int i, demo_flag = (argc > 3 && !strcmp(argv[3], "--demo"));
    
    inputFilename = argv[1];
    strcpy(outputFilename, argv[1]);
    strcat(outputFilename, ".output");
    replacement = argv[2][0];

    if (demo_flag && argc == 6 && !strcmp(argv[4], "--buffersize") && atoi(argv[5]) > 0)
        bufferSize = atoi(argv[5]);

    for (i = 0; i < 3; i++) {
        sem_init(&semaphores[i], 0, 1);
        // moved queue creation here to avoid address boundary error
        // by making sure they are created before queue status checks in threads
        queues[i] = createQueue(bufferSize);
    }
    
    pthread_t threads[4];
    for (i = 0; i < 4; i++)
        pthread_create(&threads[i], NULL, threadRoutines[i], NULL);
    // demo
    if (demo_flag) {
        while (!done_flags[3]) {
            printf("\033[K\e[?25l\rLines processed - reader: %ld, character: %ld, toUpper: %ld, writer: %ld\e[?25h",
                linesProcessed[0], linesProcessed[1], linesProcessed[2], linesProcessed[3]);
            if (!done_flags[0] && !done_flags[1] && !done_flags[2])
                printf("\e[?25l | Queues: %d, %d, %d\e[?25h", queues[0]->size, queues[1]->size, queues[2]->size);
        }
        printf("\n");    
    }
    
    for (i = 0; i < 4; i++)
        pthread_join(threads[i], NULL);
    
    for (i = 0; i < 3; i++)
        sem_destroy(&semaphores[i]);
    
    printf("%s",outputFilename);
    exit(0);
}



void *reader(void *arg) {
    char *line;
    int i;
    size_t len;
    FILE *fptr = fopen(inputFilename, "r");
    
    if (fptr) {
        while (getline(&line, &len, fptr) != -1) {
            while (isQueueFull(queues[0]));
            sem_wait(&semaphores[0]);
            // add line into queue
            enqueue(queues[0], &line);
            
            sem_post(&semaphores[0]);
            // for demo purpose
            linesProcessed[0]++;
        }
        done_flags[0] = 1;
    }
    fclose(fptr);
    return 0;
}



void *character(void *arg) {    
    char *line;
    int i;

    while (!done_flags[0] || !isQueueEmpty(queues[0])) {
        if (!isQueueEmpty(queues[0]) && !isQueueFull(queues[1])) {
            sem_wait(&semaphores[0]);
            // dequeue the queue and process line replacing character
            line = dequeue(queues[0]);
            for (i = 0; line[i] != 0; i++) {
                if (line[i] == ' ') {
                    line[i] = replacement;
                }
            }
            sem_post(&semaphores[0]);
            
            sem_wait(&semaphores[1]);
            // add repleced line to a new queue
            enqueue(queues[1], &line);
            
            free(line);
            
            sem_post(&semaphores[1]);
            linesProcessed[1]++;
        } 
    }
    done_flags[1] = 1;
    return 0;
}



void *toUpper(void *arg) {
    char *line;
    int i;

    while (!done_flags[1] || !isQueueEmpty(queues[1])) {
        if (!isQueueEmpty(queues[1]) && !isQueueFull(queues[2])) {
            sem_wait(&semaphores[1]);
            // dequeue and upcase all the letters
            line = dequeue(queues[1]);
            for (i = 0; line[i] != 0; i++)
                line[i] = toupper(line[i]);
            
            sem_post(&semaphores[1]);
            
            sem_wait(&semaphores[2]);
            // add the processed line to a new queue
            enqueue(queues[2], &line);
            free(line);
            
            sem_post(&semaphores[2]);
            linesProcessed[2]++;
        }
    }
    done_flags[2] = 1;
    return 0;
}



void *writer(void *arg) {
    char *line;
    int i;
    FILE *fp = fopen(outputFilename, "w");

    while (!done_flags[2] || !isQueueEmpty(queues[2])) {
        if (!isQueueEmpty(queues[2])) {
            sem_wait(&semaphores[2]);
            // dequeue and write processed content to output file
            line = dequeue(queues[2]);
            fputs(line, fp);
            free(line);
            
            sem_post(&semaphores[2]);
            
            linesProcessed[3]++;
        }
    }
    fclose(fp);
    // destroy queue
    for (i = 0; i < 3; i++)
        destroyQueue(queues[i]);
    done_flags[3] = 1;
    return 0;
}
