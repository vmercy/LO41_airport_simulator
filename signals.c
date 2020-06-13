#include <stdlib.h>
#include <stdio.h>
#include "header.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>

//sigterm and sigint handlers to delete IPC with semctl( semid, 1, IPC_RMID );

void CleanIPCs(){

    ColorVerbose(MAIN,True,True,True,"EXECUTION TERMINEE - RAPPORT : %i avion(s) a/ont decolle (Piste courte : %i, Piste longue : %i), %i a/ont atterri (Piste courte : %i, Piste longue : %i), %i est/sont au parking\n",*NbDecolles2500+*NbDecolles4000,*NbDecolles2500,*NbDecolles4000,*NbAtterris2500+*NbAtterris4000,*NbAtterris2500,*NbAtterris4000,*NbParking);

    sem_unlink("printfSem");
    sem_close(print);

    sem_unlink("MutexParking");
    sem_close(MutexNbParking);

    sem_unlink("MutexNbAttenteDecollage2500");
    sem_close(MutexNbAttenteDecollage2500);

    sem_unlink("MutexNbAttenteAtterrissage2500");
    sem_close(MutexNbAttenteAtterrissage2500);

    sem_unlink("SemPiste2500");
    sem_close(Piste2500);

    sem_unlink("SemAutoDecollage2500");
    sem_close(AutoDecollage2500);

    sem_unlink("MutexNbAttenteDecollage4000");
    sem_close(MutexNbAttenteDecollage4000);

    sem_unlink("MutexNbAttenteAtterrissage4000");
    sem_close(MutexNbAttenteAtterrissage4000);

    sem_unlink("SemPiste4000");
    sem_close(Piste4000);

    sem_unlink("SemAutoDecollage4000");
    sem_close(AutoDecollage4000);

    sem_unlink("SemParking");
    sem_close(Parking);
    shmdt(NbParking);
    shmdt(NbAttenteDecollage2500);
    shmdt(NbAttenteAtterrissage2500);
    shmdt(NbAttenteDecollage4000);
    shmdt(NbAttenteAtterrissage4000);
    shmctl(shmid, IPC_RMID, 0);

    IPCCleaned = True;
}

int InitializeSignal(){
    if (signal(SIGINT, CleanIPCs) == SIG_ERR) {
        ColorVerbose(MAIN,True,False,False,"Erreur lors de l'affectation du signal SIGINT\n", stderr);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}