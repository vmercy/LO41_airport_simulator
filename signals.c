#include <stdlib.h>
#include <stdio.h>
#include "header.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>

//sigterm and sigint handlers to delete IPC with semctl( semid, 1, IPC_RMID );

void CleanIPCs(){

    ColorVerbose(MAIN,True,True,True,"EXECUTION TERMINEE - RAPPORT : %i avion(s) a/ont decolle (Piste courte : %i, Piste longue : %i), %i a/ont atterri (Piste courte : %i, Piste longue : %i), %i est/sont au parking\n",*NbTO2500+*NbTO4000,*NbTO2500,*NbTO4000,*NbLanded2500+*NbLanded4000,*NbLanded2500,*NbLanded4000,*NbParking);

    sem_unlink("printfSem");
    sem_close(print);

    sem_unlink("MutexParking");
    sem_close(MutexNbParking);

    sem_unlink("MutexNbAttenteDecollage2500");
    sem_close(MutexNbWaitForTO2500);

    sem_unlink("MutexNbAttenteAtterrissage2500");
    sem_close(MutexNbWaitForLand2500);

    sem_unlink("SemPiste2500");
    sem_close(Piste2500);

    sem_unlink("SemAutoDecollage2500");
    sem_close(AutoDecollage2500);

    sem_unlink("MutexNbAttenteDecollage4000");
    sem_close(MutexNbWaitForTO4000);

    sem_unlink("MutexNbAttenteAtterrissage4000");
    sem_close(MutexNbWaitForLand4000);

    sem_unlink("SemPiste4000");
    sem_close(Piste4000);

    sem_unlink("SemAutoDecollage4000");
    sem_close(AutoDecollage4000);

    sem_unlink("SemParking");
    sem_close(Parking);
    shmdt(NbParking);
    shmdt(MutexNbWaitForTO2500);
    shmdt(MutexNbWaitForLand2500);
    shmdt(MutexNbWaitForTO4000);
    shmdt(MutexNbWaitForLand4000);
    shmdt(NbLanded2500);
    shmdt(NbTO2500);
    shmdt(NbLanded4000);
    shmdt(NbTO4000);
    shmdt(NbEmergency2500);
    shmdt(NbEmergency4000);
    
    shmctl(shmid, IPC_RMID, 0);

    shmdt(NbPlanesPerReportPT);
    shmctl(shmid2, IPC_RMID, 0);

    IPCCleaned = True;
}

int InitializeSignal(){
    if (signal(SIGINT, CleanIPCs) == SIG_ERR) {
        ColorVerbose(MAIN,True,False,False,"Erreur lors de l'affectation du signal SIGINT\n", stderr);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}