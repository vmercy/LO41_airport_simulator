#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "header.h"

char OTAN_SPELL[26][OTAN_SPELL_MAXLENGTH] = {"ALPHA", "BRAVO", "CHARLIE", "DELTA", "ECHO", "FOXTROT", "GOLF", "HOTEL", "INDIA", "JULIETT", "KILO", "LIMA", "MIKE", "NOVEMBRE", "OSCAR", "PAPA", "QUEBEC", "ROMEO", "SIERRA", "TANGO", "UNIFORM", "VICTOR", "WHISKY", "X-RAY", "YANKEE", "ZOULOU"};
char *runways[4] = {"33L", "33R", "15L", "15R"};
char *visibility[2] = {"MAUVAISE", "CAVOK"};

void initializeData()
{
    maxDistance = 0;

    sprintf(EuropeanCountries[FRANCE].airport_prefix, "LF");
    sprintf(EuropeanCountries[FRANCE].registration_prefix, "F");

    sprintf(EuropeanCountries[UK].airport_prefix, "EG");
    sprintf(EuropeanCountries[UK].registration_prefix, "G");

    sprintf(EuropeanCountries[NETHERLANDS].airport_prefix, "EH");
    sprintf(EuropeanCountries[NETHERLANDS].registration_prefix, "PH");

    sprintf(EuropeanCountries[GERMANY].airport_prefix, "ED");
    sprintf(EuropeanCountries[GERMANY].registration_prefix, "D");

    sprintf(EuropeanCountries[SPAIN].airport_prefix, "LE");
    sprintf(EuropeanCountries[SPAIN].registration_prefix, "EC");

    sprintf(EuropeanCountries[TURKEY].airport_prefix, "LT");
    sprintf(EuropeanCountries[TURKEY].registration_prefix, "TC");

    sprintf(EuropeanCountries[RUSSIA].airport_prefix, "UU");
    sprintf(EuropeanCountries[RUSSIA].registration_prefix, "RA");

    sprintf(EuropeanCountries[ITALY].airport_prefix, "LI");
    sprintf(EuropeanCountries[ITALY].registration_prefix, "I");

    sprintf(BSL.iata, "BSL");
    sprintf(BSL.fullname, "Bale-Mulhouse");
    sprintf(BSL.oaci_suffix, "SB");
    BSL.distance = 0;
    BSL.host_country = EuropeanCountries[FRANCE];

    sprintf(BSL_reporting_points[0].id, "E");
    BSL_reporting_points[0].pref_alt = 2500;

    sprintf(BSL_reporting_points[1].id, "N");
    BSL_reporting_points[1].pref_alt = 2500;

    sprintf(BSL_reporting_points[2].id, "NE");
    BSL_reporting_points[2].pref_alt = 2500;

    sprintf(BSL_reporting_points[3].id, "S");
    BSL_reporting_points[3].pref_alt = 3000;

    sprintf(BSL_reporting_points[4].id, "W");
    BSL_reporting_points[4].pref_alt = 3000;

    sprintf(BSL_reporting_points[5].id, "WA");
    BSL_reporting_points[5].pref_alt = 3000;

    sprintf(BSL_reporting_points[6].id, "WB");
    BSL_reporting_points[6].pref_alt = 2500;

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

    sprintf(unknown.id,"UKN");
    unknown.pref_alt = BSL_ALTITUDE;

    sprintf(ground.id,"SOL");
    ground.pref_alt = BSL_ALTITUDE;

    sprintf(out.id,"SOL");
    ground.pref_alt = BSL_ALTITUDE;
}