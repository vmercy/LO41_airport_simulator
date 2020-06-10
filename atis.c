#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"
#include <time.h>

void generateAtis(){
    time_t now = time(NULL);
    if(CurrentATIS.id < 'A' || CurrentATIS.id > 'Z') CurrentATIS.id = 'A'; else CurrentATIS.id = 'A'+(CurrentATIS.id-64)%26;
    CurrentATIS.rec_stamp = *localtime(&now);
    CurrentATIS.wind_dir = rand()%360;
    CurrentATIS.wind_speed = rand()%(MAX_WIND+1);
    if(CurrentATIS.wind_dir>=(QFU-90)%360 || CurrentATIS.wind_dir<(QFU+90)%360){
        CurrentATIS.runway_LM = RWAY_15R;
        CurrentATIS.runway_big = RWAY_15L;
    }
    else{
        CurrentATIS.runway_LM = RWAY_33L;
        CurrentATIS.runway_big = RWAY_33R;
    }
    CurrentATIS.temp = MIN_TEMP+rand()%(MAX_TEMP-MIN_TEMP+1);
    CurrentATIS.dew = CurrentATIS.temp - rand()%10; //le point de rosee est toujours <= temperature de l'air, ici je choisis arbitrairement un ecart max de 10 DEG C
    CurrentATIS.visibility = rand()%2;
    CurrentATIS.QNH = 950+rand()%101;
    CurrentATIS.QFE = CurrentATIS.QNH-BSL_ALTITUDE/28; //gradient de pression en basse altitude : -1 hPa/28ft
}

void printAtis(){
    ColorVerbose(ATIS,True,True, True,
    "Ici Bale-Mulhouse, information %s enregistree a %s:%s UTC \n\
    Piste en service legers et moyens porteurs : %s\n\
    Piste en service gros porteurs : %s\n\
    Vent du %i DEG %i KT\n\
    Visibilite %s\n\
    Temperature %i DEG CELSIUS\n\
    Point de rosee %i DEG CELSIUS\n\
    Calage altimetrique QNH %i QFE %i\n\
    Informez %s recue au premier contact\n",OTAN_SPELL[CurrentATIS.id-65],zfill(2,CurrentATIS.rec_stamp.tm_hour),zfill(2,CurrentATIS.rec_stamp.tm_min),runways[CurrentATIS.runway_LM],runways[CurrentATIS.runway_big],CurrentATIS.wind_dir, CurrentATIS.wind_speed, visibility[CurrentATIS.visibility], CurrentATIS.temp, CurrentATIS.dew, CurrentATIS.QNH,CurrentATIS.QFE,OTAN_SPELL[CurrentATIS.id-65]);
}