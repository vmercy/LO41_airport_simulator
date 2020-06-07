#include <stdlib.h>
#include <stdio.h>
#include "header.h"
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

#define DOMESTIC 1
#define LOCAL 1

char *aircraftCompleteType[3] = {"LEGER", "MOYEN PORTEUR", "GROS PORTEUR"};

char randomizeCapLetter()
{
    return (char)(65 + rand() % 26);
}

char *randomizeRegistrationSuffix()
{
    char *ret = (char *)malloc(sizeof(char));
    sprintf(ret, "%c%c%c%c", randomizeCapLetter(), randomizeCapLetter(), randomizeCapLetter(), randomizeCapLetter());
    return ret;
}

bool randomizeProb(int prob)
{
    int random = rand() % 101;
    if (random < prob)
        return True;
    return False;
}

void planeProcess()
{
    srand(time(0)^getpid());
    aircraft Plane;
    strcpy(Plane.registration_suffix, randomizeRegistrationSuffix());
    Plane.aircraft_type = rand() % 3;
    switch (Plane.aircraft_type)
    {
    case LIGHT:
        Plane.cruising_speed = 100 + rand() % 51;
        break;

    case MEDIUM:
        Plane.cruising_speed = 400 + rand() % 51;
        break;

    case JUMBO:
        Plane.cruising_speed = 450 + rand() % 51;
        break;
    }
    Plane.extern_airport_type = rand()%2; //50 percent chance that Plane is travelling from/to BSL airport
    switch (randomizeProb(DOMESTIC_PROBABILITY))
    {
    case DOMESTIC: //France to France flight
        switch (randomizeProb(LOCAL_PROBABILITY))
        {
        case LOCAL:
            Plane.dep_arr = BSL;
            break;

        default: //non local
            Plane.dep_arr = FrenchAirports[rand()%NB_AIRPORTS_FRANCE];
            break;
        }
        break;
    default: //extern flight
        Plane.dep_arr = EuropeanAirports[rand()%NB_AIRPORTS_EUROPE];
        break;
    }
    switch (Plane.extern_airport_type)
    {
    case FROM:
        Plane.last_pos = ground;
        break;
    
    default: //TO
        Plane.last_pos = out;
        break;
    }
    Plane.squawk = 7000;
    fflush(stdout);
    //printf("\nJe suis %i et je veux entrer dans la section critique\n",getpid());
    P(SEM_PRINTF);
    //printf("\nValeur apres P : %i",getVal(SEM_PRINTF));
    fflush(stdout);
    ColorVerbose(PLANE, True, True,
            "PID : %i \n\
            Bale-Mulhouse de %s-%s bonjour \n\
            Pays de prov-dep : %s\n\
            Avion %s \n\
            En provenance de BSL \n\
            A destination de %s \n\
            Avec l'information %s \n\
            Vitesse de croisiere %i \n\
            ",
                 getpid(),Plane.dep_arr.host_country.registration_prefix,Plane.registration_suffix, aircraftCompleteType[Plane.aircraft_type], Plane.dep_arr.fullname, OTAN_SPELL[CurrentATIS.id - 65], Plane.cruising_speed);
    //TODO: cut preceeding verbose in two parts
    printf("\nJe suis %i et je suis en section critique\n",getpid());
    V(SEM_PRINTF);
    //printf("\nJe suis %i et je suis sorti de la section critique\n",getpid());
    fflush(stdout);
    //printf("\nValeur apres V : %i",getVal(SEM_PRINTF));
}

int createPlanesProcesses()
{
    int i;
    for (i = 0; i < nbAircrafts; i++)
    {
        if (getpid() == main_pid)
        {
            if (!fork())
            {
                planeProcess();
                exit(0);
            }
        }
    }
    return i + 1;
}

void waitForPlanes()
{
    for (int i = 0; i < nbAircrafts; i++)
    {
        wait(NULL);
    }
}