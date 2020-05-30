#include <time.h>

#define OTAN_SPELL_MAXLENGTH 9
#define REGISTRATION_SUFFIX_MAXLENGTH 4 //max length of an aircraft registration suffix (F-___)
#define REGISTRATION_PREFIX_MAXLENGTH 2 //max length of an aircraft registration (_-HATF)
#define OACI_PREFIX_MAXLENGTH 2         //max length of the OACI prefix of an airport (LF__)
#define OACI_SUFFIX_MAXLENGTH 2         //max length of the OACI suffix of an airport (__PG)
#define REPORT_POINT_ID_MAXLENGTH 2     //max length of a reporting point identifier
#define NB_REPORT_POINTS 7              //number of reporting points for BSL
#define ROUTE_MAXLENGTH 3               //max number of reporting points for a route
#define AIRPORT_NAME_MAXLENGTH 30       //max length of an airport name
#define IATA_MAXLENGTH 3                //max length of an IATA code

#define EURFILE "AeroportsEurope.csv" //csv list of european airports
#define FRAFILE "AeroportsFrance.csv" //csv list of french airports

#define DEFAULT_NB_AVION 20         //default number of aircrafts to generate
#define DEFAULT_CAPACITE_PARKING 10 //default max number of aircrafts in BSL parking

#define NB_AIRPORTS_FRANCE 20 //BSL excluded
#define NB_AIRPORTS_EUROPE 10 //BSL excluded

#define BSL_ALTITUDE 885 //altitude of BSL airport, used to compute the difference QNH-QFE

/* Roles for display on terminal */
#define MAIN 0  //black
#define AVION 1 //blue
#define ATIS 2  //magenta
#define GND 3   //green
#define APP 4   //yellow
#define TWR 5   //red

/* BSL runways */
#define RWAY_33L 0
#define RWAY_33R 1
#define RWAY_15L 2
#define RWAY_15R 3

/* Magnetic orientation of parallel runways 33 */
#define QFU 333

/* Definition des conditions meteo */
#define CAVOK 1  //Ceiling and visibility OK
#define CAVNOK 0 //Ceiling and visibility NOK
#define MIN_TEMP -5
#define MAX_TEMP 20

#define MAX_WIND 64 // maximum possible wind on Beaufort scale

#define True 1
#define False 0

/* Used countries */
#define FRANCE 0
#define UK 1
#define NETHERLANDS 2
#define GERMANY 3
#define SPAIN 4
#define TURKEY 5
#define RUSSIA 6
#define ITALY 7

/* Types of aircraft */
#define LIGHT 0
#define MEDIUM 1
#define BIG 2

/* States of aircraft */
#define GROUND 0
#define AIR 1
#define DISTRESS 2 //triggered by MAYDAY MAYDAY MAYDAY call (SIGUSR1 signal)

#define KMTONM 0.539957 //convert factor kilometers -> nautic miles

/* Types */
typedef int bool;
typedef struct
{
  char registration_prefix[REGISTRATION_PREFIX_MAXLENGTH + 1]; //prefix of an aircraft which has this country as departure/arrival country
  char airport_prefix[OACI_PREFIX_MAXLENGTH + 1];              //OACI prefix of an airport hosted by this country
} country;

typedef struct
{
  char id[REPORT_POINT_ID_MAXLENGTH + 1];
  int pref_alt;     //in ft
  int nb_aircrafts; //number of aircrafts having this reporting point as their last known position
} report_pt;

typedef struct
{
  int point_indexes[ROUTE_MAXLENGTH]; //contains the indexes of reporting points belonging to the route inside BSL_report_points
} route;

typedef struct
{
  country host_country; //country hosting the airport
  char fullname[AIRPORT_NAME_MAXLENGTH + 1];
  char iata[IATA_MAXLENGTH + 1];
  char oaci_suffix[OACI_SUFFIX_MAXLENGTH + 1];
  route prefered_route; //prefered route to join the airport from BSL
  int distance;         //distance from BSL (in NM)
} airport;

typedef struct
{
  char registration_suffix[REGISTRATION_SUFFIX_MAXLENGTH + 1]; //suffix of the registration code (preceded by the prefix of country hosting its airport below)
  airport dep_arr;                                             //departure or arrival airport, can be BSL in case of a local flight
  int extern_airport_type;                                     //0 if airport above is arrival one (departure is BSL), 1 otherwise
  int aircraft_type;                                           //LIGHT,MEDIUM or BIG
  int state;                                                   //GROUND, AIR or DISTRESS
  report_pt last_pos;                                          //last known position of aircraft,
  int squawk;                                                  //squawk displayed by aircraft
  int fuel;                                                    //fuel amount in aircraft
  int fuel_threshold;                                          //threshold that triggers fuel distress if reached from superior value
  int cruising_speed;                                          //in kt
  struct tm ETA;                                               //estimated time arrival, computed from cruising speed above
} aircraft;

typedef struct
{
  char id;                   //id of the ATIS message (Alpha...Zoulou)
  struct tm rec_stamp;       //record timestamp
  int runway_LM, runway_big; //runway in service for light and medium aircrafts, then for big ones
  int wind_dir;              //wind direction
  int wind_speed;            //inferior to MAX_WIND
  bool visibility;
  int QNH, QFE;  //atmospheric pressure at sea level between 950 and 1050, atmospheric pressure at BSL ground level
  int temp, dew; //outside temp, dew point, both in centigrade
} condAtis;

/* Global constants */
country EuropeanCountries[8];
airport FrenchAirports[20];
airport EuropeanAirports[10];
airport BSL; //main airport. Fullname : BASEL EUROAIRPORT, IATA : BSL, OACI : LFSB
char OTAN_SPELL[26][OTAN_SPELL_MAXLENGTH]; //international OTAN spelling alphabet
char *runways[4];
char *visibility[2];
report_pt BSL_reporting_points[7];
report_pt ground; //used to localize aircrafts in BSL maneuvering area (parking and taxiways)
route BSL_pref_route[3];

/* Global functions */
void ColorVerbose(int role, bool header, bool jump, char *fmt, ...);
void printAtis();
void generateAtis();
void initializeData();
int ReadAirports();
void PrintAeroportData(airport Aeroport);
void PrintRoute(route Route);

/* Global Variables */
int nbAircrafts, parkingCapacity;
condAtis CurrentATIS;
