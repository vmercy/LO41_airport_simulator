#include <stdlib.h>
#include <stdio.h>
#include "header.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>

//sigterm and sigint handlers to delete IPC with semctl( semid, 1, IPC_RMID );

void CleanIPCs(){
    semctl(semid, 1, IPC_RMID);
    shmctl(shmid, IPC_RMID, NULL);
}

int InitializeSignal(){
    if (signal(SIGINT, CleanIPCs) == SIG_ERR) {
        ColorVerbose(MAIN,True,False,"Erreur lors de l'affectation du signal SIGINT\n", stderr);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}