#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "header.h"

int kmToNM(int km){return KMTONM*km;}

int ReadAirports()
{
    FILE *FichierEurope = fopen(EURFILE, "r");

    if (!FichierEurope)
    {
        ColorVerbose(MAIN, False, True, "Erreur ouverture fichier %s\n",EURFILE);
        return 0;
    }

    char buf[1024];
    int row_count = 0, total_readed;
    int field_count = 0;
    while (fgets(buf, 1024, FichierEurope))
    {
        field_count = 0;

        char *field = strtok(buf, ";");
        while (field)
        {
            switch (field_count)
            {
            case 0: //nom complet
                strcpy(EuropeanAirports[row_count].fullname, field);
                //printf("Nom complet : %s | ", EuropeanAirports[row_count].nom);
                break;
            case 1: //IATA
                strcpy(EuropeanAirports[row_count].iata, field);
                //printf("IATA : %s | ", EuropeanAirports[row_count].iata);
                break;
            case 2: //sufixe OACI
                strcpy(EuropeanAirports[row_count].oaci_suffix, field);
                //printf("Suffixe OACI : %s\n", EuropeanAirports[row_count].suffixe_oaci);
                break;
            case 3: //index pays
            {
                int index = atoi(field);
                EuropeanAirports[row_count].host_country = EuropeanCountries[index];
                break;
            }
            case 4: //distance
            {
                EuropeanAirports[row_count].distance = kmToNM(atoi(field));
                break;
            }
            }
            //printf("%s\n", field);
            field = strtok(NULL, ";");

            field_count++;
        }
        row_count++;
    }
    fclose(FichierEurope);

    FILE *FichierFrance = fopen(EURFILE, "r");

    if (!FichierEurope)
    {
        ColorVerbose(MAIN, False, True, "Erreur ouverture fichier %s\n",EURFILE);
        return 0;
    }

    int row_count = 0;
    int field_count = 0;
    while (fgets(buf, 1024, FichierEurope))
    {
        field_count = 0;

        char *field = strtok(buf, ";");
        while (field)
        {
            switch (field_count)
            {
            case 0: //nom complet
                strcpy(EuropeanAirports[row_count].fullname, field);
                //printf("Nom complet : %s | ", EuropeanAirports[row_count].nom);
                break;
            case 1: //IATA
                strcpy(EuropeanAirports[row_count].iata, field);
                //printf("IATA : %s | ", EuropeanAirports[row_count].iata);
                break;
            case 2: //sufixe OACI
                strcpy(EuropeanAirports[row_count].oaci_suffix, field);
                //printf("Suffixe OACI : %s\n", EuropeanAirports[row_count].suffixe_oaci);
                break;
            case 3: //index pays
            {
                int index = atoi(field);
                EuropeanAirports[row_count].host_country = EuropeanCountries[index];
                break;
            }
            case 4: //distance
            {
                int dist = atoi(field);
                EuropeanAirports[row_count].distance = dist;
                break;
            }
            }
            //printf("%s\n", field);
            field = strtok(NULL, ";");

            field_count++;
        }
        row_count++;
    }
    fclose(FichierEurope);


    return total_readed;
}

int CountReportingPoints(route Route)
{
    int i = 0;
    while(i<3&&Route.point_indexes[i]!=-1) i++;
    /* printf("\033[31mNb Points Report : %i\033[0m", i); */
    return i;
}

void PrintRoute(route Route)
{
    int nb_pts = CountReportingPoints(Route);
    for (int i = 0; i < nb_pts; i++)
    {
        printf("%s", BSL_reporting_points[Route.point_indexes[i]].id);
        if (i != (nb_pts - 1))
            printf("-");
    }
}

void PrintAeroportData(airport Aeroport)
{
    printf("Nom : %s | IATA : %s | OACI : %s%s | Itineraire preferentiel : ", Aeroport.fullname, Aeroport.iata, Aeroport.host_country.airport_prefix, Aeroport.oaci_suffix);
    PrintRoute(Aeroport.prefered_route);
    printf(" | Distance BSL : %i\n",Aeroport.distance);
}