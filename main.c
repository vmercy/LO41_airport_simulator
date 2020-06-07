#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#include "header.h"

int main(int argc, char *argv[])
{
    main_pid = getpid();

    printf("\nMain PID : %i",main_pid);

    if ((semid = initsem(SKEY)) < 0) /* Création d'un ensemble de sémaphore */
    {
        printf("\nErreur lors de l'initialisation des semaphores");
    }
    initializeSemaphores();

    initializeData();

    srand(time(0));
    
    if (argc != 2)
    {
        printf("\nParametres NB_AVIONS et CAPACITE_PARKING initialises a 20 et 10\n");
        nbAircrafts = DEFAULT_NB_AVION;
        parkingCapacity = DEFAULT_CAPACITE_PARKING;
    }
    else
    {
        nbAircrafts = atoi(argv[1]);
        parkingCapacity = atoi(argv[2]);
    }
    generateAtis();
    printAtis();

    int airportsReaded = ReadAirports();
    if (airportsReaded > NB_AIRPORTS_EUROPE + NB_AIRPORTS_FRANCE)
    {
        ColorVerbose(MAIN, False, True, "Erreur : le nombre total d'aeroports importes (%i) depuis %s et %s est superieur a NB_AIRPORTS_FRANCE(%i) + NB_AIRPORTS_EUROPE(%i) = (%i)", airportsReaded, EURFILE, FRAFILE, NB_AIRPORTS_FRANCE, NB_AIRPORTS_EUROPE, NB_AIRPORTS_EUROPE + NB_AIRPORTS_FRANCE);
        exit(EXIT_FAILURE);
    }

    /* ColorVerbose(MAIN,False,True,"Aeroports EUROPE : \n");

    for(int i = 0;i<NB_AIRPORTS_EUROPE;i++){
        EuropeanAirports[i].prefered_route=reverseRoute(EuropeanAirports[i].prefered_route);
        PrintAeroportData(EuropeanAirports[i]);
    }

    ColorVerbose(MAIN,False,True,"Aeroports FRANCE : \n");

    for(int i = 0;i<NB_AIRPORTS_FRANCE;i++){
        FrenchAirports[i].prefered_route=reverseRoute(FrenchAirports[i].prefered_route);
        PrintAeroportData(FrenchAirports[i]);
    } */

    
    for(int i = 0;i<NB_EUROPEAN_COUTRIES;i++){
        ColorVerbose(MAIN,False,True,"%i : %s",i,EuropeanCountries[i].registration_prefix);
    }
    
    if(createPlanesProcesses()<nbAircrafts){
        ColorVerbose(MAIN,True,True,"Erreur lors de la creation des processus avions");
    }

    for(int i =0; i<nbAircrafts;i++){ //TODO: add 1 for tower process once created
        wait(NULL);
    }

    printf("\nValeur finale semaphore : %i",getVal(SEM_PRINTF));

    return 0;
}
