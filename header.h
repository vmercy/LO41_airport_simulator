#include <time.h>

#define OTAN_SPELL_MAXLENGTH 9
#define LONGMAX_SUFFIXE_IMMAT 4
#define LONGMAX_PREFIXE_IMMAT 2 //longueur maximum du prefixe de l'immatriculation d'un avion (celles-ci dépendent des pays où sont immmatriculés les avions)
#define LONGMAX_PREFIXE_OACI 2 //longueur maximum du prefixe du code OACI d'un aéroport
#define LONGMAX_SUFFIXE_OACI 2 //longueur maximum du suffixe du code OACI d'un aéroport
#define LONGMAX_INDICATIF_PT_REPORT 2 //longueur maximum d'un indicatif de point de report
#define NB_PTS_REPORT 7 //nombre de points de report autour de BSL
#define LONGMAX_ITINERAIRE 3 //nombre maximum de points de report pour un itineraire
#define LONGMAX_NOM_AEROPORT 30 //longueur maximum du nom d'un aeroport 
#define LONGMAX_IATA 3 //longueur max d'un code IATA

#define DEFAULT_NB_AVION 20
#define DEFAULT_CAPACITE_PARKING 10

/* Definition des roles pour la coloration sur terminal */
#define MAIN 0 //noir
#define AVION 1 //bleu
#define ATIS 2 //violet
#define GND 3 //vert
#define APP 4 //jaune
#define TWR 5 //rouge

#define True 1
#define False 0

/* Types */
typedef int bool;
typedef struct {
  char prefixe_immat[LONGMAX_PREFIXE_IMMAT+1];  //prefixe de l'immatriculation d'un avion en provenance/a destination du pays
  char prefixe_aeroport[LONGMAX_PREFIXE_OACI+1]; //prefixe du code OACI d'un aeroport heberge par le pays
}pays;

typedef struct {
  char indicatif[LONGMAX_INDICATIF_PT_REPORT+1];
  int alt_pref; //en pieds
  int nb_avions; //nombre d'avions ayant le point de report comme derniere position
}pt_report;

typedef struct {
  pt_report points[LONGMAX_ITINERAIRE];
}itineraire;

typedef struct {
  pays implantation; //pays ou est implante l'aeroport
  char nom[LONGMAX_NOM_AEROPORT+1]; //nom complet de l'aeroport
  char iata[LONGMAX_IATA+1];
  char suffixe_oaci[LONGMAX_SUFFIXE_OACI+1];
  itineraire itin_pref; //itineraire preferentiel pour rejoindre l'aeroport dans le sens BSL->exterieur
  int distance; //distance par rapport a BSL, exprimee en miles
}aeroport;

typedef struct{
  char suffixe_immat[LONGMAX_SUFFIXE_IMMAT+1]; //suffixe de l'immatriculation de l'avion (prefixe donne par le pays de l'aeroport ci-dessous)
  aeroport dest_arr; //pays de destination ou d'arrivee, peut etre BSL (cas du vol local BSL->BSL)
  int type_aero_ext; //vaut 0 si l'aeroport ci-dessus est celui d'arrivee (depart = BSL), 1 si c'est l'inverse
  int type_avion; //0 pour un léger, 1 pour un moyen porteur, 2 pour un gros porteur
  int etat; //0 : au sol, 1 : en vol, 2 : en détresse
  pt_report dern_pos; //dernier point de report passe par l'avion, vide s'il est au sol
  int code_transp; //code transpondeur affiche par l'avion
  int qte_carbu; //quantite de carburant actuellement dans l'avion
  int seuil_panne_carbu; //seuil a partir duquel le signal de panne de carburant sera envoye
  int vitesse_croisiere; //en noeuds
  struct tm ETA; //heure d'arrivee estimee en fonction de la distance de l'aeroport et de la vitesse de croisiere
} avion;

typedef struct{
  //temperature, pt rosee, pistes en service, QNH ,etc...
}conditionsAtis;

/* Constantes globales */
aeroport AeroportsFrance[20];
aeroport AeroportsEurope[20];
char OTAN_SPELL[26][OTAN_SPELL_MAXLENGTH]; //alphabet d'epellation international de l'OTAN

/* Fonctions */
void ColorVerbose(int role, bool header, bool saut, char *fmt, ...);

/* Variables globales */
int nbAvions, capaParking;