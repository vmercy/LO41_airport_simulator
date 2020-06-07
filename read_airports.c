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
    int row_count = 0, total_readed = 0;
    int field_count = 0;
    while (fgets(buf, 1024, FichierEurope))
    {
        field_count = 0;

        char *field = strtok(buf, ";");
        while (field)
        {
            switch (field_count)
            {
            case 0: //fullname
                strcpy(EuropeanAirports[row_count].fullname, field);
                //printf("Nom complet : %s | ", EuropeanAirports[row_count].nom);
                break;
            case 1: //IATA
                strcpy(EuropeanAirports[row_count].iata, field);
                //printf("IATA : %s | ", EuropeanAirports[row_count].iata);
                break;
            case 2: //OACI suffix
                strcpy(EuropeanAirports[row_count].oaci_suffix, field);
                //printf("Suffixe OACI : %s\n", EuropeanAirports[row_count].suffixe_oaci);
                break;
            case 3: //country index
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
            case 5: //route
            {
                EuropeanAirports[row_count].prefered_route = BSL_pref_route[atoi(field)];
                break;
            }
            }
            //printf("%s\n", field);
            field = strtok(NULL, ";");

            field_count++;
        }
        row_count++;
    }
    total_readed+=row_count;
    fclose(FichierEurope);

    FILE *FichierFrance = fopen(FRAFILE, "r");

    if (!FichierFrance)
    {
        ColorVerbose(MAIN, False, True, "Erreur ouverture fichier %s\n",FRAFILE);
        return 0;
    }

    row_count = 0;
    field_count = 0;
    while (fgets(buf, 1024, FichierFrance))
    {
        field_count = 0;

        char *field = strtok(buf, ";");
        while (field)
        {
            switch (field_count)
            {
            case 0: //fullname
                strcpy(FrenchAirports[row_count].fullname, field);
                //printf("Nom complet : %s | ", EuropeanAirports[row_count].nom);
                break;
            case 1: //IATA
                strcpy(FrenchAirports[row_count].iata, field);
                //printf("IATA : %s | ", EuropeanAirports[row_count].iata);
                break;
            case 2: //OACI suffix
                strcpy(FrenchAirports[row_count].oaci_suffix, field);
                //printf("Suffixe OACI : %s\n", EuropeanAirports[row_count].suffixe_oaci);
                break;
            case 3: //distance
            {
                int dist = atoi(field);
                FrenchAirports[row_count].distance = dist;
                break;
            }
            case 4: //route
            {
                FrenchAirports[row_count].prefered_route = BSL_pref_route[atoi(field)];
                break;
            }
            }
            //printf("%s\n", field);
            field = strtok(NULL, ";");

            field_count++;
        }
        FrenchAirports[row_count].host_country = EuropeanCountries[FRANCE];
        row_count++;
    }
    total_readed+=row_count;
    fclose(FichierFrance);
    return total_readed;
}

void PrintAeroportData(airport Aeroport)
{
    ColorVerbose(MAIN,False,False,"Nom : %s | IATA : %s | OACI : %s%s | Itineraire preferentiel : ", Aeroport.fullname, Aeroport.iata, Aeroport.host_country.airport_prefix, Aeroport.oaci_suffix);
    PrintRoute(Aeroport.prefered_route);
    ColorVerbose(MAIN,False,False," | Distance BSL : %i\n",Aeroport.distance);
}