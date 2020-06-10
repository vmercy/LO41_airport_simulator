#include <stdlib.h>
#include <stdio.h>
#include "header.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>

//sigterm and sigint handlers to delete IPC with semctl( semid, 1, IPC_RMID );

void CleanIPCs(){
    sem_unlink("printfSem");
    sem_close(print);

    sem_unlink("MutexParking");
    sem_close(MutexNbParking);

    sem_unlink("MutexNbAttenteDecollage");
    sem_close(MutexNbAttenteDecollage);

    sem_unlink("MutexNbAttenteAtterrissage");
    sem_close(MutexNbAttenteAtterrissage);

    sem_unlink("SemPiste");
    sem_close(Piste);

    sem_unlink("SemAutoDecollage");
    sem_close(AutoDecollage);

    sem_unlink("SemParking");
    sem_close(Parking);
    shmdt(NbParking);
    shmdt(NbAttenteDecollage);
    shmdt(NbAttenteAtterrissage);
    shmctl(shmid, IPC_RMID, 0);
}

int InitializeSignal(){
    if (signal(SIGINT, CleanIPCs) == SIG_ERR) {
        ColorVerbose(MAIN,True,False,False,"Erreur lors de l'affectation du signal SIGINT\n", stderr);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}