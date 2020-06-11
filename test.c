#include <stdio.h>          /* printf()                 */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <errno.h>          /* errno, ECHILD            */
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>          /* O_CREAT, O_EXEC          */
#include <signal.h>
#include <time.h>

void signal_handler(int num){
    printf("\nHello World!\n");
    exit(EXIT_SUCCESS);
}


int main (int argc, char **argv){
    sem_t *sem;

    sem = sem_open ("pSem", O_CREAT | O_EXCL, 0644, 0); 

    signal(SIGUSR1,signal_handler);

    pid_t pid = fork();

    if(!pid){ //child
        //sem_wait(sem);
        printf("\nDebloque\n");
        while (1);
    }

    else
    {
        printf("\n Main \n");
        sleep(3);
        kill(pid,SIGUSR1);
        wait(NULL);
        sem_close(sem);
    }
    

    return 0;
}