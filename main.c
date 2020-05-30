#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>

#include "header.h"

int main(int argc, char *argv[])
{
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

    printf("\nAeroportsEurope lus : %i\n",ReadAirports());

    for(int i = 0;i<NB_AIRPORTS_EUROPE;i++){
        EuropeanAirports[i].prefered_route = BSL_pref_route[rand()%3];
        PrintAeroportData(EuropeanAirports[i]);
    }
 
    return 0;
}
