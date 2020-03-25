#include "PWM.h"

void ajustementPWM ( uint8_t pwmA, uint8_t pwmB, Direction direction) {
    switch(direction){
        case avance:
            PORTD = 0xff;
            break;
        case recule:
            PORTD = 0x00;
            break;
        case droite:
            PORTD = 0x40;
            break;
        case gauche:
            PORTD = 0x80;
            break;
    }


    /*Commentaires fournis par le prof
     mise à un des sorties OC1A et OC1B sur comparaison
     réussie en mode PWM 8 bits, phase correcte
     et valeur de TOP fixe à 0xFF (mode #1 de la table 17-6
     page 177 de la description technique du ATmega324PA)*/
    OCR1A = 255*pwmA/100;
    OCR1B = 255*pwmB/100;
    
    //Setup des timers du PWM
    TCCR1A = (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1);
    TCCR1B = (1 << CS11);
    TCCR1C = 0;
}