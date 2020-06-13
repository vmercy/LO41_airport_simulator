#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>
#include "header.h"
#include <signal.h>

#define DOMESTIC 1
#define LOCAL 1

char *aircraftCompleteType[3] = {"LEGER", "MOYEN PORTEUR", "GROS PORTEUR"};

aircraft Plane;

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

void showFinalState()
{
    if (!strcmp(Plane.last_pos.id, "UKN"))
    { //plane position is unknown (before first radio communication)
        ColorVerbose(PLANE, False, True, True, "%s-%s : Interruption du vol | Etat : avant premier contact radio\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix);
    }
    else
    {
        ColorVerbose(PLANE, False, True, True, "%s-%s : Interruption du vol | Derniere position : %s\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, Plane.last_pos.id);
    }
    exit(EXIT_SUCCESS);
}

void emergencyLanding()
{
    sem_wait(print);
    ColorVerbose(PLANE, True, True, False, "%s-%s : MAYDAY MAYDAY MAYDAY panne moteur - demandons atterrissage d'urgence piste %s", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
    ColorVerbose(TWR, True, True, False, "%s-%s : MAYDAY recu, je libere la piste %s", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
    sem_post(print);

    switch ((Plane.UsedRunway == RWAY_15L) || (Plane.UsedRunway == RWAY_33R))
    {
    case True: //long runway, length 4000m
    {
        (*NbEmergency4000)++;
        sem_wait(Piste4000);
        sem_wait(print);
        ColorVerbose(TWR, True, True, False, "%s-%s : Autorise atterrissage d'urgence immediat piste %s\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
        ColorVerbose(PLANE, True, True, False, "%s-%s : Autorise atterrissage d'urgence piste %s\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
        sem_post(print);
        sleep(EMERGENCY_LANDING_DURATION);
        ColorVerbose(SUCCESS, True, True, True, "%s-%s : Atterrissage urgent piste %s termine - piste degagee\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
        sem_post(Piste4000);
        (*NbEmergency4000)--;
    }
    case False: //short runway, length 2500m
    {
        (*NbEmergency2500)++;
        sem_wait(Piste2500);
        sem_wait(print);
        ColorVerbose(TWR, True, True, False, "%s-%s : Autorise atterrissage d'urgence immediat piste %s\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
        ColorVerbose(PLANE, True, True, False, "%s-%s : Autorise atterrissage d'urgence piste %s\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
        sem_post(print);
        sleep(EMERGENCY_LANDING_DURATION);
        ColorVerbose(SUCCESS, True, True, True, "%s-%s : Atterrissage urgent piste %s termine - piste degagee\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
        sem_post(Piste2500);
        (*NbEmergency2500)--;
    }
    }

    exit(EXIT_FAILURE);
}

void planeProcess()
{
    signal(SIGINT, showFinalState);
    signal(SIGUSR1, emergencyLanding);

    srand(time(0) ^ getpid());

    //attach process to shared memory
    NbParking = (int *)shmat(shmid, NULL, 0);
    NbWaitForTO2500 = NbParking + 1;
    NbWaitForLand2500 = NbParking + 2;
    NbWaitForTO4000 = NbParking + 3;
    NbWaitForLand4000 = NbParking + 4;
    NbLanded2500 = NbParking + 5;
    NbTO2500 = NbParking + 6;
    NbLanded4000 = NbParking + 7;
    NbTO4000 = NbParking + 8;
    NbEmergency2500 = NbParking + 9;
    NbEmergency4000 = NbParking + 10;

    Plane.pid = getpid();
    strcpy(Plane.registration_suffix, randomizeRegistrationSuffix());
    Plane.aircraft_type = rand() % 3;
    Plane.last_pos = unknown;
    Plane.squawk = 7000+rand()%1000;
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
    int waitBeforeContact = MIN_WAIT + (MAX_WAIT - MIN_WAIT)*Plane.dep_arr.distance/maxDistance;
    ColorVerbose(MAIN, True, True, True, "Avion initialise | PID : %i | Immatriculation : %s-%s | Premier contact radio dans %i sec\n", Plane.pid, Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix,waitBeforeContact);
    sleep(waitBeforeContact);
    Plane.extern_airport_type = TO;
    sem_wait(MutexNbParking);
    if ((*NbParking) > 0)
        Plane.extern_airport_type = rand() % 2; //If parking is not empty, then we can generate outgoing flights
    sem_post(MutexNbParking);

    switch (Plane.extern_airport_type)
    {

    case FROM:
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);

        if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
        {
            printf("\nErreur SIGPROCMASK");
            exit(EXIT_FAILURE);
        }

        Plane.last_pos = ground;
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
        //DEBUT ALGORITHME
        switch ((Plane.UsedRunway == RWAY_15L) || (Plane.UsedRunway == RWAY_33R))
        {
        case True: //long runway, length 4000m
        {
            sem_wait(MutexNbWaitForTO4000);
            (*NbWaitForTO4000)++;
            sem_post(MutexNbWaitForTO4000);
            sem_wait(MutexNbWaitForLand4000);
            if (*NbWaitForLand4000 > 0)
            {
                sem_post(MutexNbWaitForLand4000);
                ColorVerbose(TWR, True, True, True, "%s-%s : Autres appareils en cours d'atterrissage sur la piste %s, attendez avant decollage\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
                sem_wait(AutoDecollage4000);
            }
            else
            {
                sem_post(MutexNbWaitForLand4000);
            }
            if(*NbEmergency4000>0){
                ColorVerbose(TWR,True,True,True,"%s-%s : Atterrissage d'urgence en cours, attendez avant decollage\n",Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix);
                while(*NbEmergency4000>0){}; //active waiting
            }
            sem_wait(Piste4000);
            sem_wait(print);
            ColorVerbose(TWR, True, True, False, "%s-%s : Autorise decollage piste %s, alignez-vous\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            ColorVerbose(PLANE, True, True, False, "%s-%s : Autorise decollage, je m'aligne et decolle piste %s\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            sem_post(print);
            sem_post(Piste4000);
            ColorVerbose(SUCCESS, True, True, True, "%s-%s : Decollage effectue, piste %s liberee\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            (*NbTO4000)++;
            sem_post(Parking);
            sem_wait(MutexNbParking);
            (*NbParking)--;
            ColorVerbose(MAIN, True, True, True, "Places de parking occupees : %i/%i\n", *NbParking, parkingCapacity);
            sem_post(MutexNbParking);
            sem_wait(MutexNbWaitForTO4000);
            (*NbWaitForTO4000)--;
            sem_post(MutexNbWaitForTO4000);
            sem_wait(MutexNbWaitForLand4000);
            if (*NbWaitForLand2500 == 0)
            {
                sem_post(MutexNbWaitForLand4000);
                sem_wait(MutexNbWaitForTO4000);
                if (*NbWaitForTO4000 > 0)
                {
                    sem_post(MutexNbWaitForTO4000);
                    sem_post(AutoDecollage4000);
                }
                else
                {
                    sem_post(MutexNbWaitForTO4000);
                }
            }
            else
            {
                sem_post(MutexNbWaitForLand4000);
            }
            break;
        }

        default: //short runway, length 2500m
        {
            sem_wait(MutexNbWaitForTO2500);
            (*NbWaitForTO2500)++;
            sem_post(MutexNbWaitForTO2500);
            sem_wait(MutexNbWaitForLand2500);
            if (*NbWaitForLand2500 > 0)
            {
                sem_post(MutexNbWaitForLand2500);
                ColorVerbose(TWR, True, True, True, "%s-%s : Autres appareils en cours d'atterrissage sur la piste %s, attendez avant decollage\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
                sem_wait(AutoDecollage2500);
            }
            else
            {
                sem_post(MutexNbWaitForLand2500);
            }
            if(*NbEmergency2500>0){
                ColorVerbose(TWR,True,True,True,"%s-%s : Atterrissage d'urgence en cours, attendez avant decollage\n",Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix);
                while(*NbEmergency2500>0){}; //active waiting
            }
            sem_wait(Piste2500);
            sem_wait(print);
            ColorVerbose(TWR, True, True, False, "%s-%s : Autorise decollage piste %s, alignez-vous\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            ColorVerbose(PLANE, True, True, False, "%s-%s : Autorise decollage, je m'aligne et decolle piste %s\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            sem_post(print);
            sem_post(Piste2500);
            ColorVerbose(SUCCESS, True, True, True, "%s-%s : Decollage effectue, piste %s liberee\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            (*NbTO2500)++;
            sem_post(Parking);
            sem_wait(MutexNbParking);
            (*NbParking)--;
            ColorVerbose(MAIN, True, True, True, "Places de parking occupees : %i/%i\n", *NbParking, parkingCapacity);
            sem_post(MutexNbParking);
            sem_wait(MutexNbWaitForTO2500);
            (*NbWaitForTO2500)--;
            sem_post(MutexNbWaitForTO2500);
            sem_wait(MutexNbWaitForLand2500);
            if (*NbWaitForLand2500 == 0)
            {
                sem_post(MutexNbWaitForLand2500);
                sem_wait(MutexNbWaitForTO2500);
                if (*NbWaitForTO2500 > 0)
                {
                    sem_post(MutexNbWaitForTO2500);
                    sem_post(AutoDecollage2500);
                }
                else
                {
                    sem_post(MutexNbWaitForTO2500);
                }
            }
            else
            {
                sem_post(MutexNbWaitForLand2500);
            }
            break;
        }
        }
        //FIN ALGORITHME

        for (int i = 0; i < CountReportingPoints(Plane.dep_arr.prefered_route); i++)
        {
            Plane.last_pos = ReportPointAtIndex(i, Plane.dep_arr.prefered_route);
            ColorVerbose(SUCCESS, False, True, True, "%s-%s : Je passe le point %s (sens : sortie)\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, Plane.last_pos.id);  //TODO: set altitude
            sleep(WAIT_INSIDE_ROUTE_DEPARTURE);
        }
        Plane.last_pos = out;

        break;
    }
    case TO:
    {
        Plane.last_pos = out;
        route myRoute = reverseRoute(Plane.dep_arr.prefered_route);

        for (int i = 0; i < CountReportingPoints(myRoute); i++)
        {
            Plane.last_pos = ReportPointAtIndex(i, myRoute);
            ColorVerbose(SUCCESS, False, True, True, "%s-%s : Je passe le point %s (sens : entree) altitude \n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, Plane.last_pos.id); //TODO: set altitude
            sleep(WAIT_INSIDE_ROUTE_ARRIVAL);
        }
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);

        if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
        {
            printf("\nErreur SIGPROCMASK");
            exit(EXIT_FAILURE);
        }
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
            sem_wait(MutexNbWaitForLand4000);
            (*NbWaitForLand4000)++;
            sem_post(MutexNbWaitForLand4000);
            int PisteOccupee;
            sem_getvalue(Piste4000, &PisteOccupee);
            if (PisteOccupee <= 0)
            {
                sem_wait(print);
                ColorVerbose(TWR, True, True, False, "%s-%s : Piste %s occupee, executez tour d'attente. Vous etes N°%i sur la sequence d'atterrissage.\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway], *NbWaitForLand2500);
                ColorVerbose(PLANE, False, True, False, "%s-%s : La piste %s est occupee, j attends sa liberation...\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
                sem_post(print);
            }
            if(*NbEmergency4000>0){
                ColorVerbose(TWR,True,True,True,"%s-%s : Atterrissage d'urgence en cours, attendez avant atterrissage\n",Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix);
                while(*NbEmergency4000>0){}; //active waiting
            }
            sem_wait(Piste4000);
            sem_wait(print);
            ColorVerbose(TWR, True, True, False, "%s-%s : Autorise atterrissage immediat piste %s \n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            ColorVerbose(PLANE, False, True, False, "%s-%s : Autorise atterrissage piste %s\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            sem_post(print);
            sem_post(Piste4000);
            ColorVerbose(SUCCESS, False, True, False, "%s-%s : Atterrissage effectue, piste %s degagee, je roule pour le parking... Aurevoir \n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            (*NbLanded4000)++;
            sem_wait(MutexNbWaitForLand4000);
            (*NbWaitForLand4000)--;
            if (*NbWaitForLand4000 == 0)
            {
                sem_post(MutexNbWaitForLand4000);
                sem_wait(MutexNbWaitForTO4000);
                if (*NbWaitForTO4000 > 0)
                    sem_post(AutoDecollage4000);
                sem_post(MutexNbWaitForTO4000);
            }
            else
            {
                sem_post(MutexNbWaitForLand4000);
            }
            sem_wait(MutexNbParking);
            (*NbParking)++;
            ColorVerbose(MAIN, True, True, True, "Places de parking occupees : %i/%i\n", *NbParking, parkingCapacity);
            sem_post(MutexNbParking);

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
            sem_wait(MutexNbWaitForLand2500);
            (*NbWaitForLand2500)++;
            sem_post(MutexNbWaitForLand2500);
            int PisteOccupee;
            sem_getvalue(Piste2500, &PisteOccupee);
            if (PisteOccupee <= 0)
            {
                sem_wait(print);
                ColorVerbose(TWR, True, True, False, "%s-%s : Piste %s occupee, executez tour d'attente. Vous etes N°%i sur la sequence d'atterrissage.\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway], *NbWaitForLand2500);
                ColorVerbose(PLANE, False, True, False, "%s-%s : La piste %s est occupee, j attends sa liberation...\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
                sem_post(print);
            }
            if(*NbEmergency2500>0){
                ColorVerbose(TWR,True,True,True,"%s-%s : Atterrissage d'urgence en cours, attendez avant atterrissage\n",Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix);
                while(*NbEmergency2500>0){}; //active waiting
            }
            sem_wait(Piste2500);
            sem_wait(print);
            ColorVerbose(TWR, True, True, False, "%s-%s : Autorise atterrissage immediat piste %s \n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            ColorVerbose(PLANE, False, True, False, "%s-%s : Autorise atterrissage piste %s\n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            sem_post(print);
            sem_post(Piste2500);
            ColorVerbose(SUCCESS, False, True, False, "%s-%s : Atterrissage effectue, piste %s degagee, je roule pour le parking... Aurevoir \n", Plane.dep_arr.host_country.registration_prefix, Plane.registration_suffix, runways[Plane.UsedRunway]);
            (*NbLanded2500)++;
            sem_wait(MutexNbWaitForLand2500);
            (*NbWaitForLand2500)--;
            if (*NbWaitForLand2500 == 0)
            {
                sem_post(MutexNbWaitForLand2500);
                sem_wait(MutexNbWaitForTO2500);
                if (*NbWaitForTO2500 > 0)
                    sem_post(AutoDecollage2500);
                sem_post(MutexNbWaitForTO2500);
            }
            else
            {
                sem_post(MutexNbWaitForLand2500);
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