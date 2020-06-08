#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "header.h"

char OTAN_SPELL[26][OTAN_SPELL_MAXLENGTH] = {"ALPHA", "BRAVO", "CHARLIE", "DELTA", "ECHO", "FOXTROT", "GOLF", "HOTEL", "INDIA", "JULIETT", "KILO", "LIMA", "MIKE", "NOVEMBRE", "OSCAR", "PAPA", "QUEBEC", "ROMEO", "SIERRA", "TANGO", "UNIFORM", "VICTOR", "WHISKY", "X-RAY", "YANKEE", "ZOULOU"};
char *runways[4] = {"33L", "33R", "15L", "15R"};
char *visibility[2] = {"MAUVAISE", "CAVOK"};

void initializeData()
{
    for(int i =0; i<NB_EUROPEAN_COUTRIES;i++)
        strcpy(EuropeanCountries[i].registration_prefix,"");

    strcpy(EuropeanCountries[FRANCE].airport_prefix, "LF");
    strcpy(EuropeanCountries[FRANCE].registration_prefix, "F");

    strcpy(EuropeanCountries[UK].airport_prefix, "EG");
    strcpy(EuropeanCountries[UK].registration_prefix, "G");

    strcpy(EuropeanCountries[NETHERLANDS].airport_prefix, "EH");
    strcpy(EuropeanCountries[NETHERLANDS].registration_prefix, "PH");

    strcpy(EuropeanCountries[GERMANY].airport_prefix, "ED");
    strcpy(EuropeanCountries[GERMANY].registration_prefix, "D");

    strcpy(EuropeanCountries[SPAIN].airport_prefix, "LE");
    strcpy(EuropeanCountries[SPAIN].registration_prefix, "EC");

    strcpy(EuropeanCountries[TURKEY].airport_prefix, "LT");
    strcpy(EuropeanCountries[TURKEY].registration_prefix, "TC");

    strcpy(EuropeanCountries[RUSSIA].airport_prefix, "UU");
    strcpy(EuropeanCountries[RUSSIA].registration_prefix, "RA");

    strcpy(EuropeanCountries[ITALY].airport_prefix, "LI");
    strcpy(EuropeanCountries[ITALY].registration_prefix, "I");

    strcpy(BSL.iata, "BSL");
    strcpy(BSL.fullname, "Bale-Mulhouse");
    strcpy(BSL.oaci_suffix, "SB");
    BSL.distance = 0;
    BSL.host_country = EuropeanCountries[FRANCE];

    strcpy(BSL_reporting_points[0].id, "E");
    BSL_reporting_points[0].pref_alt = 2500;
    BSL_reporting_points[0].nb_aircrafts = 0;

    strcpy(BSL_reporting_points[1].id, "N");
    BSL_reporting_points[1].pref_alt = 2500;
    BSL_reporting_points[1].nb_aircrafts = 0;
    
    strcpy(BSL_reporting_points[2].id, "NE");
    BSL_reporting_points[2].pref_alt = 2500;
    BSL_reporting_points[2].nb_aircrafts = 0;

    strcpy(BSL_reporting_points[3].id, "S");
    BSL_reporting_points[3].pref_alt = 3000;
    BSL_reporting_points[3].nb_aircrafts = 0;

    strcpy(BSL_reporting_points[4].id, "W");
    BSL_reporting_points[4].pref_alt = 3000;
    BSL_reporting_points[4].nb_aircrafts = 0;

    strcpy(BSL_reporting_points[5].id, "WA");
    BSL_reporting_points[5].pref_alt = 3000;
    BSL_reporting_points[5].nb_aircrafts = 0;

    strcpy(BSL_reporting_points[6].id, "WB");
    BSL_reporting_points[6].pref_alt = 2500;
    BSL_reporting_points[6].nb_aircrafts = 0;

    BSL_pref_route[NORTH].point_indexes[0] = 0;
    BSL_pref_route[NORTH].point_indexes[1] = 2;
    BSL_pref_route[NORTH].point_indexes[2] = 1;
    BSL_pref_route[SOUTH].point_indexes[0] = 0;
    BSL_pref_route[SOUTH].point_indexes[1] = 3;
    BSL_pref_route[SOUTH].point_indexes[2] = -1;
    BSL_pref_route[WEST].point_indexes[0] = 6;
    BSL_pref_route[WEST].point_indexes[1] = 5;
    BSL_pref_route[WEST].point_indexes[2] = 4;

    BSL.prefered_route = BSL_pref_route[NORTH]; //we consider that local flights are do to and from west

    strcpy(ground.id, "");
    ground.nb_aircrafts = 0;
    ground.pref_alt = BSL_ALTITUDE;
}