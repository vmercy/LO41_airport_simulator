#include <time.h>
#include <sys/types.h>
#include <semaphore.h>

/* Semaphores */
#define PERM 0644
#define SEMFLAGS (O_CREAT | O_EXCL)

/* Shared memory */
#define NB_INT 5

/* Parameters */
#define MIN_WAIT 5 //seconds. When a plane process is generated, it will inevitably wait WAIT_FIW before starting its activity.
#define MAX_WAIT 25 //seconds. When a plane process is generated, it will randomly wait a certain amount of time before starting its activity, with a limit of MAX_WAIT.

#define EURFILE "AeroportsEurope.csv" //csv list of european airports
#define FRAFILE "AeroportsFrance.csv" //csv list of french airports

#define DOMESTIC_PROBABILITY 50 //probability (in percent) that generated plane is affected to a domestic flight (BSL->France or France->BSL)
#define LOCAL_PROBABILITY 50 //probability of generating a local flight (BSL->BSL), knowing that this aircraft is affected to a domestic flight

#define NB_AIRPORTS_FRANCE 20 //BSL excluded
#define NB_AIRPORTS_EUROPE 10 //BSL excluded
/* End of parameters */

#define DEFAULT_NB_AVION 5         //default number of aircrafts to generate //TODO: set to 20
#define DEFAULT_CAPACITE_PARKING 3 //default max number of aircrafts in BSL parking

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

#define BSL_ALTITUDE 885 //altitude of BSL airport, used to compute the difference QNH-QFE
#define SEPARATION 

/* Roles for display on terminal */
#define MAIN 0  //black
#define PLANE 1 //blue
#define ATIS 2  //magenta
#define SUCCESS 3   //green
#define TWR 4   //red

/* BSL runways */
#define RWAY_33L 0 //length 2500
#define RWAY_33R 1 //length 4000
#define RWAY_15L 2 //length 4000
#define RWAY_15R 3 //length 2500

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
#define NB_EUROPEAN_COUTRIES 8

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
#define JUMBO 2 //big heavy plane

/* States of aircraft */
#define NORMAL 0
#define DISTRESS 1 //triggered by MAYDAY MAYDAY MAYDAY call (SIGUSR1 signal)
#define PAN 2

/* Types of travels relative to BSL airport */
#define FROM 0
#define TO 1

/* Preferential Routes */
#define NORTH 0
#define SOUTH 1
#define WEST 2

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
  int point_indexes[ROUTE_MAXLENGTH]; //contains the indexes of reporting points belonging to the route inside BSL_report_points, with a -1 padding if necessary
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
  pid_t pid;
  char registration_suffix[REGISTRATION_SUFFIX_MAXLENGTH + 1]; //suffix of the registration code (preceded by the prefix of country hosting its airport below)
  airport dep_arr;                                             //departure or arrival airport, can be BSL in case of a local flight
  bool extern_airport_type;                                     //FROM if airport above is arrival one (departure is BSL), TO otherwise. If airport above is BSL (local flight) then extern_airport_type determines whether the aircraft is leaving or arriving (FROM/TO)
  int aircraft_type;                                           //LIGHT,MEDIUM or BIG
  int state;                                                   //GROUND, AIR or DISTRESS
  report_pt last_pos;                                          //last known position of aircraft,
  int squawk;                                                  //squawk displayed by aircraft
  int fuel;                                                    //fuel amount in aircraft
  int fuel_threshold;                                          //threshold that triggers fuel distress if reached from superior value
  int cruising_speed;                                          //in kt
  struct tm ETA;                                               //estimated time arrival, computed from cruising speed above
  int UsedRunway;
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
country EuropeanCountries[NB_EUROPEAN_COUTRIES];
airport FrenchAirports[NB_AIRPORTS_FRANCE];
airport EuropeanAirports[NB_AIRPORTS_EUROPE];
airport BSL; //main airport. Fullname : BASEL EUROAIRPORT, IATA : BSL, OACI : LFSB
char OTAN_SPELL[26][OTAN_SPELL_MAXLENGTH]; //international OTAN spelling alphabet
char *runways[4];
char *visibility[2];
char *aircraftCompleteType[3];
report_pt BSL_reporting_points[7];
report_pt ground; //used to localize aircrafts in BSL parking
report_pt out; //used to localize aircrafts that are outside BSL region
route BSL_pref_route[3];

/* Global functions */
void ColorVerbose(int role, bool header, bool jump, bool protected, char *fmt, ...);
void printAtis();
void generateAtis();
void initializeData();
int ReadAirports();
void PrintAeroportData(airport Aeroport);
void PrintRoute(route Route);
char* zfill(int length, int value);
int createPlanesProcesses();
void waitForPlanes();
route reverseRoute(route Route);
void initializeSemaphores();
void V(int semnum);
void P(int semnum);
int initsem(key_t semkey, int role);
void initializeSemaphores();
int getVal(int semnum);
int InitializeSignal();
void tower();
void CleanIPCs();
int CountReportingPoints(route Route);
report_pt ReportPointAtIndex(int point_index, route Route);

/* Global Variables */
pid_t main_pid;
int semid, shmid;
int nbAircrafts, parkingCapacity;
condAtis CurrentATIS;
sem_t *print;
sem_t *MutexNbParking;
sem_t *MutexNbAttenteDecollage2500;
sem_t *MutexNbAttenteAtterrissage2500;
sem_t *MutexNbAttenteDecollage4000;
sem_t *MutexNbAttenteAtterrissage4000;
sem_t *Piste2500;
sem_t *Piste4000;
sem_t *AutoDecollage2500;
sem_t *AutoDecollage4000;
sem_t *Parking;

int *NbParking;
int *NbAttenteDecollage2500;
int *NbAttenteAtterrissage2500;
int *NbAttenteDecollage4000;
int *NbAttenteAtterrissage4000;