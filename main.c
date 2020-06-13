#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#include "header.h"

void handler()
{
    
}

void setup(int argc, char *argv[])
{
    main_pid = getpid();

    printf("\nMain PID : %i\n", main_pid);

    //TODO: ask if user wants to connect control console or not

    printf("Attente demarrage interface de controle ...\n");
    sigset_t set;

    sigemptyset(&set);

    if (sigaddset(&set, SIGUSR1) == -1)
    {
        perror("Sigaddset error");
        exit(EXIT_FAILURE);
    }

    int sig = SIGUSR1;
    int *sig_ptr = &sig;

    sigwait(&set, sig_ptr);

    IPCCleaned = False;
    srand(time(0));

    key_t shmkey;
    shmkey = ftok(argv[0], 5);
    shmid = shmget(shmkey, NB_INT * sizeof(int), 0644 | IPC_CREAT);
    if (shmid < 0)
    { /* shared memory error check */
        perror("shmget\n");
        exit(EXIT_FAILURE);
    }

    if (MIN_WAIT > MAX_WAIT)
    {
        printf("\nErreur : le temps d'attente minimum (MIN_WAIT=%i) est superieur au temps d'attente maximum (MAX_WAIT=%i)\n", MIN_WAIT, MAX_WAIT);
        exit(EXIT_FAILURE);
    }

    NbParking = (int *)shmat(shmid, NULL, 0);
    NbAttenteDecollage2500 = NbParking + 1;
    NbAttenteAtterrissage2500 = NbParking + 2;
    NbAttenteDecollage4000 = NbParking + 3;
    NbAttenteAtterrissage4000 = NbParking + 4;
    NbAtterris2500 = NbParking + 5;
    NbDecolles2500 = NbParking + 6;
    NbAtterris4000 = NbParking + 7;
    NbDecolles4000 = NbParking + 8;
    NbEmergency2500 = NbParking + 9;
    NbEmergency4000 = NbParking + 10;

    *NbParking = 0;
    *NbAttenteDecollage2500 = 0;
    *NbAttenteAtterrissage2500 = 0;
    *NbAttenteDecollage4000 = 0;
    *NbAttenteAtterrissage4000 = 0;

    *NbAtterris2500 = 0;
    *NbDecolles2500 = 0;
    *NbAtterris4000 = 0;
    *NbDecolles4000 = 0;

    *NbEmergency2500 = 0;
    *NbEmergency4000 = 0;

    print = sem_open("printfSem", SEMFLAGS, PERM, 1);

    initializeData();

    if (argc != 3)
    {
        nbAircrafts = DEFAULT_NB_AVION;
        parkingCapacity = DEFAULT_CAPACITE_PARKING;
        ColorVerbose(MAIN, False, True, False, "\nInitialisation faite avec les valeurs par defaut : NB_AVIONS = %i et CAPACITE_PARKING = %i \n", DEFAULT_NB_AVION, DEFAULT_CAPACITE_PARKING);
    }
    else
    {
        nbAircrafts = atoi(argv[1]);
        parkingCapacity = atoi(argv[2]);
        ColorVerbose(MAIN, False, True, False, "\nInitialisation faite avec les valeurs personnalisees : NB_AVIONS = %i et CAPACITE_PARKING = %i \n", nbAircrafts, parkingCapacity);
    }

    MutexNbParking = sem_open("MutexParking", SEMFLAGS, PERM, 1);
    MutexNbAttenteDecollage2500 = sem_open("MutexNbAttenteDecollage2500", SEMFLAGS, PERM, 1);
    MutexNbAttenteAtterrissage2500 = sem_open("MutexNbAttenteAtterrissage2500", SEMFLAGS, PERM, 1);
    MutexNbAttenteDecollage4000 = sem_open("MutexNbAttenteDecollage4000", SEMFLAGS, PERM, 1);
    MutexNbAttenteAtterrissage4000 = sem_open("MutexNbAttenteAtterrissage4000", SEMFLAGS, PERM, 1);

    Piste2500 = sem_open("SemPiste2500", SEMFLAGS, PERM, 1);
    Piste4000 = sem_open("SemPiste4000", SEMFLAGS, PERM, 1);
    AutoDecollage2500 = sem_open("SemAutoDecollage2500", SEMFLAGS, PERM, 0);
    AutoDecollage4000 = sem_open("SemAutoDecollage4000", SEMFLAGS, PERM, 0);

    Parking = sem_open("SemParking", SEMFLAGS, PERM, parkingCapacity);

    if (InitializeSignal() == EXIT_FAILURE)
    {
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

    if (createPlanesProcesses() < nbAircrafts)
    {
        ColorVerbose(MAIN, True, True, False, "Erreur lors de la creation des processus avions");
    }
}

int main(int argc, char *argv[])
{
    signal(SIGUSR1, handler);

    setup(argc, argv);

    waitForPlanes();

    if (!IPCCleaned)
        CleanIPCs();

    return 0;
}
