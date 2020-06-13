#include <stdio.h>     /* printf()                 */
#include <stdlib.h>    /* exit(), malloc(), free() */
#include <sys/types.h> /* key_t, sem_t, pid_t      */
#include <sys/shm.h>   /* shmat(), IPC_RMID        */
#include <errno.h>     /* errno, ECHILD            */
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h> /* sem_open(), sem_destroy(), sem_wait().. */
#include <signal.h>
#include <fcntl.h> /* O_CREAT, O_EXEC          */

void handler()
{
    printf("\nJe suis reveille !");
}

int main(int argc, char **argv)
{
    signal(SIGUSR1, handler);
    pid_t myPid = getpid();
    printf("\nMon PID : %i", myPid);

    /* sigset_t masque;
    sigemptyset(&masque);
    sigaddset(&masque, SIGUSR1);

    if (sigprocmask(SIG_BLOCK, &masque, NULL) < 0)
    {
        printf("\nErreur SIGPROCMASK");
        exit(EXIT_FAILURE);
    } */

    printf("\nJe commence a dormir...");

    sleep(20);

    return 0;
}

//A tester : valeur de retour lors d'un envoi de signal a un processus qui a exclu le signal avec sigprocmask