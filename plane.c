#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include "header.h"

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
    srand(time(0) ^ getpid());
    aircraft Plane;
    Plane.pid = getpid();
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
    Plane.extern_airport_type = rand() % 2; //50 percent chance that Plane is travelling from/to BSL airport
    switch (randomizeProb(DOMESTIC_PROBABILITY))
    {
    case DOMESTIC: //France to France flight
        switch (randomizeProb(LOCAL_PROBABILITY))
        {
        case LOCAL:
            Plane.dep_arr = BSL;
            break;

        default: //non local
            Plane.dep_arr = FrenchAirports[rand() % NB_AIRPORTS_FRANCE];
            break;
        }
        break;
    default: //extern flight
        Plane.dep_arr = EuropeanAirports[rand() % NB_AIRPORTS_EUROPE];
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
    Plane.ETA.tm_hour = 0;
    Plane.ETA.tm_min = 0;
    Plane.state = NORMAL;
    P(SEM_PRINTF);
    ColorVerbose(PLANE, True, True, "Avion initialise | PID : %i | Immatriculation : %s-%s\n", Plane.pid,Plane.dep_arr.host_country.registration_prefix,Plane.registration_suffix);
    //fflush(stdout);
    V(SEM_PRINTF);
    switch (Plane.extern_airport_type)
    {
    case FROM:
    {
        switch (Plane.aircraft_type==LIGHT||Plane.aircraft_type==MEDIUM)
        {
        case 1:
            Plane.UsedRunway =  CurrentATIS.runway_LM;
            break;
        
        default:
            Plane.UsedRunway =  CurrentATIS.runway_big;
            break;
        }
        P(SEM_PRINTF);
        ColorVerbose(PLANE, True, True," \
            PID %i\n\
            Bale-Mulhouse de %s-%s bonjour \n\
            Avion %s \n\
            A destination de %s \n\
            Avec l'information %s \n\
            Vitesse de croisiere %i \n\
            Transpondeur %i \n\
            Actuellement au parking \n\
            Demandons roulage pour piste %s\n\
            ",Plane.pid,Plane.dep_arr.host_country.registration_prefix,Plane.registration_suffix, aircraftCompleteType[Plane.aircraft_type], Plane.dep_arr.fullname, OTAN_SPELL[CurrentATIS.id - 65], Plane.cruising_speed,Plane.squawk,runways[Plane.UsedRunway]);
        //fflush(stdout);
        V(SEM_PRINTF);

        break;

        //afficher circuit de depart complet
    }
    case TO:
    {
        report_pt lastPos = out;
        route myRoute = reverseRoute(Plane.dep_arr.prefered_route);

        P(SEM_PRINTF);
        for(int i = 0; i<CountReportingPoints(myRoute);i++){
            lastPos = ReportPointAtIndex(i,myRoute);
            ColorVerbose(PLANE,False,True,"%s-%s : Je passe le point %s \n",Plane.
            dep_arr.host_country.registration_prefix,Plane.registration_suffix,lastPos.id);
            //fflush(stdout);
            sleep(1); //sleep depending of cruising speed
        }
        Plane.last_pos = lastPos;
        ColorVerbose(PLANE, True, True," \
            Bale-Mulhouse de %s-%s bonjour \n\
            Avion %s \n\
            En provenance de %s \n\
            A destination de vos installations \n\
            Avec l'information %s \n\
            Vitesse de croisiere %i \n\
            Passant le point %s \n\
            Demandons integration pour atterrissage complet\n\
            ",Plane.dep_arr.host_country.registration_prefix,Plane.registration_suffix, aircraftCompleteType[Plane.aircraft_type], Plane.dep_arr.fullname, OTAN_SPELL[CurrentATIS.id - 65], Plane.cruising_speed,Plane.last_pos.id);
        //fflush(stdout);
        V(SEM_PRINTF);
        break;
    }
    }
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