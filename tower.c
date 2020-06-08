#include <stdlib.h>
#include <stdio.h>
#include "header.h"
#include <unistd.h>

void tower(){
    if(!fork()){
        ColorVerbose(TWR,True,True,"BALE TOUR bonjour");
        exit(0);
    }
}