#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>

#include "header.h"

void setup(int argc, char *argv[]){
    srand(time(0));

    key_t shmkey;
    shmkey = ftok (argv[0], 5);
    shmid = shmget (shmkey, 3*sizeof (int), 0644 | IPC_CREAT);
    if (shmid < 0){                           /* shared memory error check */
        perror ("shmget\n");
        exit (EXIT_FAILURE);
    }
    NbParking = (int *) shmat (shmid,NULL,0);
    NbAttenteDecollage=NbParking+1;
    NbAttenteAtterrissage=NbAttenteDecollage+1;

    *NbParking = 0;
    *NbAttenteDecollage = 0;
    *NbAttenteAtterrissage = 0;

    print = sem_open("printfSem",O_CREAT | O_EXCL,PERM,1);

    initializeData();
    
    if (argc != 3)
    {
        nbAircrafts = DEFAULT_NB_AVION;
        parkingCapacity = DEFAULT_CAPACITE_PARKING;
        ColorVerbose(MAIN,False,True,False,"\nInitialisation faite avec les valeurs par defaut : NB_AVIONS = %i et CAPACITE_PARKING = %i \n",DEFAULT_NB_AVION,DEFAULT_CAPACITE_PARKING);
    }
    else
    {
        nbAircrafts = atoi(argv[1]);
        parkingCapacity = atoi(argv[2]);
        ColorVerbose(MAIN,False,True,False,"\nInitialisation faite avec les valeurs personnalisees : NB_AVIONS = %i et CAPACITE_PARKING = %i \n",nbAircrafts,parkingCapacity);
    }

    MutexNbParking = sem_open("MutexParking",O_CREAT | O_EXCL,PERM,1);
    MutexNbAttenteDecollage = sem_open("MutexNbAttenteDecollage",O_CREAT | O_EXCL,PERM,1);
    MutexNbAttenteAtterrissage = sem_open("MutexNbAttenteAtterrissage",O_CREAT | O_EXCL,PERM,1);

    Piste = sem_open("SemPiste",O_CREAT | O_EXCL,PERM,1);
    AutoDecollage = sem_open("SemAutoDecollage",O_CREAT | O_EXCL,PERM,0);
    Parking = sem_open("SemParking",O_CREAT | O_EXCL,PERM,parkingCapacity);

    main_pid = getpid();

    printf("\nMain PID : %i",main_pid);

    /* if ((semid = initsem(SKEY,MAIN)) < 0) //Création d'un ensemble de sémaphore
    {
        printf("\nErreur lors de l'initialisation des semaphores");
        exit(EXIT_FAILURE);
    } */

    
    


    if(InitializeSignal()==EXIT_FAILURE){
        exit(EXIT_FAILURE);
    }
    
    
    generateAtis();
    printAtis();

    int airportsReaded = ReadAirports();
    if (airportsReaded > NB_AIRPORTS_EUROPE + NB_AIRPORTS_FRANCE)
    {
        ColorVerbose(MAIN, False, True, False, "Erreur : le nombre total d'aeroports importes (%i) depuis %s et %s est superieur a NB_AIRPORTS_FRANCE(%i) + NB_AIRPORTS_EUROPE(%i) = (%i)", airportsReaded, EURFILE, FRAFILE, NB_AIRPORTS_FRANCE, NB_AIRPORTS_EUROPE, NB_AIRPORTS_EUROPE + NB_AIRPORTS_FRANCE);
        exit(EXIT_FAILURE);
    }
    
    //tower();

    if(createPlanesProcesses()<nbAircrafts){
        ColorVerbose(MAIN,True,True, False, "Erreur lors de la creation des processus avions");
    }
}

int main(int argc, char *argv[])
{
    setup(argc,argv);

    for(int i =0; i<nbAircrafts;i++){
        wait(NULL);
    }

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
    shmdt(NbAttenteAtterrissage);
    shmdt(NbAttenteDecollage);

    CleanIPCs();

    return 0;
}
