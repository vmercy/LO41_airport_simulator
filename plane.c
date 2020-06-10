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
    Plane.extern_airport_type = TO;
    sem_wait(MutexNbParking);
    ColorVerbose(TWR,False,True,True,"NbParking = %i\n",*NbParking);
    if (*NbParking > 0)
        Plane.extern_airport_type = rand() % 2; //If parking is not empty, then we can generate outgoing flights
    *NbParking++;
    sem_post(MutexNbParking);
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
    //P(SEM_PRINTF);
    sem_wait(print);
    ColorVerbose(PLANE, True, True, False, "Avion initialise | PID : %i | Immatriculation : %s-%s\n", Plane.pid, Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix);
    sem_post(print);
    //V(SEM_PRINTF);
    switch (Plane.aircraft_type == LIGHT || Plane.aircraft_type == MEDIUM)
    {
    case 1:
        Plane.UsedRunway = CurrentATIS.runway_LM;
        break;

    default:
        Plane.UsedRunway = CurrentATIS.runway_big;
        break;
    }
    switch (Plane.extern_airport_type)
    {
    case FROM:
    {
        sem_wait(print);
        ColorVerbose(PLANE, True, True, False, " \
            PID %i\n\
            Bale-Mulhouse de %s-%s bonjour \n\
            Avion %s \n\
            A destination de %s \n\
            Avec l'information %s \n\
            Vitesse de croisiere %i \n\
            Transpondeur %i \n\
            Actuellement au parking \n\
            Demandons roulage pour piste %s\n\
            ",
                     Plane.pid, Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, aircraftCompleteType[Plane.aircraft_type], Plane.dep_arr.fullname, OTAN_SPELL[CurrentATIS.id - 65], Plane.cruising_speed, Plane.squawk, runways[Plane.UsedRunway]);
        //fflush(stdout);

        //P(SEM_2500_DEC);
        ColorVerbose(TWR, True, True, False, "\
        BSL Tour - %s-%s bonjour \n\
        Autorise decollage piste %s\n\
        ",
                     Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);

        //afficher circuit de depart complet
        report_pt lastPos = ground;
        for (int i = 0; i < CountReportingPoints(Plane.dep_arr.prefered_route); i++)
        {
            lastPos = ReportPointAtIndex(i, Plane.dep_arr.prefered_route);
            ColorVerbose(PLANE, False, True, False, "%s-%s : Je passe le point %s \n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, lastPos.id);
            //fflush(stdout);
            //sleep(1); //TODO: sleep duration depending of cruising speed
        }
        Plane.last_pos = out;
        sem_post(print);
        //DEBUT ALGORITHME
        
        //FIN ALGORITHME
        break;
    }
    case TO:
    {
        report_pt lastPos = out;
        route myRoute = reverseRoute(Plane.dep_arr.prefered_route);

        sem_wait(print);
        for (int i = 0; i < CountReportingPoints(myRoute); i++)
        {
            lastPos = ReportPointAtIndex(i, myRoute);
            ColorVerbose(PLANE, False, True, False, "%s-%s : Je passe le point %s \n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, lastPos.id);
            //fflush(stdout);
            //sleep(1); //TODO: sleep duration depending of cruising speed
        }
        Plane.last_pos = lastPos;
        ColorVerbose(PLANE, True, True, False, " \
            Bale-Mulhouse de %s-%s bonjour \n\
            Avion %s \n\
            En provenance de %s \n\
            A destination de vos installations \n\
            Avec l'information %s \n\
            Vitesse de croisiere %i \n\
            Passant le point %s \n\
            Demandons integration pour atterrissage complet\n\
            ",
                     Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, aircraftCompleteType[Plane.aircraft_type], Plane.dep_arr.fullname, OTAN_SPELL[CurrentATIS.id - 65], Plane.cruising_speed, Plane.last_pos.id);
        //fflush(stdout);
        sem_post(print);

        //DEBUT ALGORITHME
        /* sem_wait(MutexNbParking);
        if(*NbParking==parkingCapacity){
            ColorVerbose(PLANE,False,True,True,"%s-%s : Parking plein, je me deroute \n",Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix);
        }
        sem_post(MutexNbParking); */
        int ValSemParking;
        sem_getvalue(Parking,&ValSemParking);
        if(ValSemParking<=0)
            ColorVerbose(PLANE,True,True,True,"%s-%s : Parking plein, j'attends...\n",Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix);
        sem_wait(Parking);
        sem_wait(MutexNbAttenteAtterrissage);
        *NbAttenteAtterrissage--;
        sem_post(MutexNbAttenteAtterrissage);
        int PisteOccupee;
        sem_getvalue(Piste,&PisteOccupee);
        if(PisteOccupee<=0){
            ColorVerbose(PLANE,False,True,True,"%s-%s : La piste est occupee, j attends sa liberation...",Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix);
        }
        sem_wait(Piste);
        ColorVerbose(PLANE,False,True,True,"%s-%s : J'atterris \n",Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix);
        sem_post(Piste);
        sem_wait(MutexNbAttenteAtterrissage);
        *NbAttenteAtterrissage--;
        if(*NbAttenteAtterrissage==0){
            sem_post(MutexNbAttenteAtterrissage);
            sem_wait(MutexNbAttenteDecollage);
            if(*NbAttenteDecollage>0)
                sem_post(AutoDecollage);
            sem_post(MutexNbAttenteDecollage);
        }
        else
        {
            sem_post(MutexNbAttenteAtterrissage);
        }
        sem_wait(MutexNbParking);
        *NbParking++;
        sem_post(MutexNbParking);
        //FIN ALGORITHME
        sem_getvalue(Parking,&ValSemParking);
        ColorVerbose(MAIN,True,True,True,"Valeur semaphore parking : %i | Capacite parking : %i\n",ValSemParking,parkingCapacity);
        break;
    }
    }
}

int createPlanesProcesses()
{
    int i;
    pid_t pid;
    for (i = 0; i < nbAircrafts; i++)
    {
        pid = fork();
        if(pid<0){
            ColorVerbose(MAIN,True,True,True,"Erreur de fork\n");
            exit(EXIT_FAILURE);
        }
        else if(pid==0)
            break;
    }
    if(pid==0){
        sleep(rand()%3); //define MAX_WAIT
        planeProcess();
        exit(EXIT_SUCCESS);
    }
    else
    return i + 1;
}

void waitForPlanes()
{
    for (int i = 0; i < nbAircrafts; i++)
    {
        wait(NULL);
    }
}