#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
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
    sleep(MIN_WAIT + rand() % (MAX_WAIT-MIN_WAIT));

    //attach process to shared memory
    NbParking = (int *) shmat (shmid,NULL,0);
    NbAttenteDecollage2500=NbParking+1;
    NbAttenteAtterrissage2500=NbParking+2;
    NbAttenteDecollage4000=NbParking+3;
    NbAttenteAtterrissage4000=NbParking+4;

    aircraft Plane;
    Plane.pid = getpid();
    strcpy(Plane.registration_suffix, randomizeRegistrationSuffix());
    //Plane.aircraft_type = rand() % 3; //TODO: reactive randomization
    Plane.aircraft_type = LIGHT;
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
    Plane.squawk = 7000;
    Plane.ETA.tm_hour = 0;
    Plane.ETA.tm_min = 0;
    Plane.state = NORMAL;
    switch (Plane.aircraft_type == LIGHT || Plane.aircraft_type == MEDIUM)
    {
    case 1:
        Plane.UsedRunway = CurrentATIS.runway_LM;
        break;

    default:
        Plane.UsedRunway = CurrentATIS.runway_big;
        break;
    }
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
    ColorVerbose(MAIN, True, True, True, "Avion initialise | PID : %i | Immatriculation : %s-%s\n", Plane.pid, Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix);
    Plane.extern_airport_type = TO;
    sem_wait(MutexNbParking);
    if ((*NbParking) > 0)
        Plane.extern_airport_type = rand() % 2; //If parking is not empty, then we can generate outgoing flights
    sem_post(MutexNbParking);

    switch (Plane.extern_airport_type)
    {
    case FROM:
    {
        Plane.last_pos = ground;
        //sem_wait(print);
        ColorVerbose(PLANE, True, True, True, " \
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
        //sem_post(print);
        //DEBUT ALGORITHME
        switch ((Plane.UsedRunway == RWAY_15L) || (Plane.UsedRunway == RWAY_33R))
        {
        case True: //long runway, length 4000m
        {

            break;
        }

        default: //short runway, length 2500m
        {
            sem_wait(MutexNbAttenteDecollage2500);
            (*NbAttenteDecollage2500)++;
            sem_post(MutexNbAttenteDecollage2500);
            sem_wait(MutexNbAttenteAtterrissage2500);
            if (*NbAttenteAtterrissage2500 > 0)
            {
                sem_post(MutexNbAttenteAtterrissage2500);
                ColorVerbose(TWR, True, True, True, "%s-%s : Autres appareils en cours d'atterrissage sur la piste %s, attendez avant decollage\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
                sem_wait(AutoDecollage2500);
            }
            else
            {
                sem_post(MutexNbAttenteAtterrissage2500);
            }
            sem_wait(Piste2500);
            sem_wait(print);
            ColorVerbose(TWR, True, True, False, "%s-%s : Autorise decollage piste %s, alignez-vous\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix,runways[Plane.UsedRunway]);
            ColorVerbose(PLANE, True, True, False, "%s-%s : Autorise decollage, je m'aligne et decolle piste %s\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix,runways[Plane.UsedRunway]);
            sem_post(print);
            sem_post(Piste2500);
            ColorVerbose(SUCCESS, True, True, True, "%s-%s : Decollage effectue, piste %s liberee\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            sem_post(Parking);
            sem_wait(MutexNbParking);
            (*NbParking)--;
            ColorVerbose(MAIN, True, True, True, "Places de parking occupees : %i/%i\n", *NbParking, parkingCapacity);
            sem_post(MutexNbParking);
            sem_wait(MutexNbAttenteDecollage2500);
            (*NbAttenteDecollage2500)--;
            sem_post(MutexNbAttenteDecollage2500);
            sem_wait(MutexNbAttenteAtterrissage2500);
            if (*NbAttenteAtterrissage2500 == 0)
            {
                sem_post(MutexNbAttenteAtterrissage2500);
                sem_wait(MutexNbAttenteDecollage2500);
                if (*NbAttenteDecollage2500 > 0)
                {
                    sem_post(MutexNbAttenteDecollage2500);
                    sem_post(AutoDecollage2500);
                }
                else
                {
                    sem_post(MutexNbAttenteDecollage2500);
                }
            }
            else
            {
                sem_post(MutexNbAttenteAtterrissage2500);
            }
            break;
        }
        }
        //FIN ALGORITHME

        report_pt lastPos = ground;
        for (int i = 0; i < CountReportingPoints(Plane.dep_arr.prefered_route); i++)
        {
            lastPos = ReportPointAtIndex(i, Plane.dep_arr.prefered_route);
            ColorVerbose(SUCCESS, False, True, True, "%s-%s : Je passe le point %s \n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, lastPos.id);
            sleep(1); //TODO: set sleep duration depending on cruising speed
        }
        Plane.last_pos = out;

        break;
    }
    case TO:
    {
        report_pt lastPos = out;
        route myRoute = reverseRoute(Plane.dep_arr.prefered_route);

        for (int i = 0; i < CountReportingPoints(myRoute); i++)
        {
            lastPos = ReportPointAtIndex(i, myRoute);
            ColorVerbose(SUCCESS, False, True, True, "%s-%s : Je passe le point %s \n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, lastPos.id);
            sleep(1); //TODO: set sleep duration depending on cruising speed
        }
        Plane.last_pos = lastPos;
        ColorVerbose(PLANE, True, True, True, " \
            Bale-Mulhouse de %s-%s bonjour \n\
            Avion %s \n\
            En provenance de %s \n\
            A destination de vos installations \n\
            Avec l'information %s \n\
            Vitesse de croisiere %i \n\
            Passant le point %s \n\
            Demandons integration pour atterrissage complet sur piste %s\n\
            ",
                     Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, aircraftCompleteType[Plane.aircraft_type], Plane.dep_arr.fullname, OTAN_SPELL[CurrentATIS.id - 65], Plane.cruising_speed, Plane.last_pos.id, runways[Plane.UsedRunway]);

        //DEBUT ALGORITHME
        switch ((Plane.UsedRunway == RWAY_15L) || (Plane.UsedRunway == RWAY_33R))
        {
        case True: //long runway, length 4000m
        {

            break;
        }

        default: //short runway, length 2500m
        {
            int ValSemParking;
            sem_getvalue(Parking, &ValSemParking);
            if (ValSemParking <= 0)
            {
                sem_wait(print);
                ColorVerbose(TWR, True, True, False, "%s-%s : Parking plein, executez deroutement ou mettez-vous en attente\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix);
                ColorVerbose(PLANE, True, True, False, "%s-%s : Parking plein, j'attends...\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix);
                sem_post(print);
            }
            sem_wait(Parking);
            sem_wait(MutexNbAttenteAtterrissage2500);
            (*NbAttenteAtterrissage2500)++;
            sem_post(MutexNbAttenteAtterrissage2500);
            int PisteOccupee;
            sem_getvalue(Piste2500, &PisteOccupee);
            if (PisteOccupee <= 0)
            {
                sem_wait(print);
                ColorVerbose(TWR, True, True, False, "%s-%s : Piste %s occupee, executez tour d'attente. Vous etes N°%i sur la sequence d'atterrissage.\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix,runways[Plane.UsedRunway],*NbAttenteAtterrissage2500);
                ColorVerbose(PLANE, False, True, False, "%s-%s : La piste %s est occupee, j attends sa liberation...\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
                sem_post(print);
            }
            sem_wait(Piste2500);
            sem_wait(print);
            ColorVerbose(TWR, True, True, False, "%s-%s : Autorise atterrissage immediat piste %s \n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            ColorVerbose(PLANE, False, True, False, "%s-%s : Autorise atterrissage piste %s\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            sem_post(print);
            sem_post(Piste2500);
            ColorVerbose(SUCCESS, False, True, False, "%s-%s : Atterrissage effectue, piste %s degagee, je roule pour le parking... Aurevoir \n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            sem_wait(MutexNbAttenteAtterrissage2500);
            (*NbAttenteAtterrissage2500)--;
            if (*NbAttenteAtterrissage2500 == 0)
            {
                sem_post(MutexNbAttenteAtterrissage2500);
                sem_wait(MutexNbAttenteDecollage2500);
                if (*NbAttenteDecollage2500 > 0)
                    sem_post(AutoDecollage2500);
                sem_post(MutexNbAttenteDecollage2500);
            }
            else
            {
                sem_post(MutexNbAttenteAtterrissage2500);
            }
            sem_wait(MutexNbParking);
            (*NbParking)++;
            ColorVerbose(MAIN, True, True, True, "Places de parking occupees : %i/%i\n", *NbParking, parkingCapacity);
            sem_post(MutexNbParking);

            break;
        }
        }
        //FIN ALGORITHME
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
        if (pid < 0)
        {
            ColorVerbose(MAIN, True, True, True, "Erreur de fork\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
            break;
    }
    if (pid == 0)
    {
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