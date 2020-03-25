#define F_CPU 8000000
#include <util/delay.h>
#include "UART.h"
#include "memoire_24.h"








int main(){
    initialisationUART();
    Memoire24CXXX memoire;
    uint16_t adresse = 0; //car 1ere donne a l adresse 0

    while (true){ //etre constamment en train d ecrire dans la memoire
        uint8_t donne = lireUART();

        memoire.ecriture(adresse, donne);
        adresse++;
    }

}