#include "son.h"

#define numNoteRef 69
#define FrequenceRef 440
#define FrequenceDemiTonBase 2
#define FrequenceDemiTonExp 12
#define CPU 8000000
#define prescale 1024




void ajustementSon ( uint8_t note) {

     
    
    DDRB |= (1 << PB3) | (1 << PB2); //port B est associe a OCR0 aka le timer 0
    
     float frequenceNote=FrequenceDemiTonBase^((note-numNoteRef)/FrequenceDemiTonExp)*FrequenceRef; // formule pour trouver la frequence de la note a partir de son numero 

     float duree=(CPU/prescale)/(frequenceNote/2)-1; // formule pour trouver la duree a mettre dans le registre de comparaison OCR0A pour produire le son de la note

     
    /*Commentaires fournis par le prof
     mise à un des sorties OC1A et OC1B sur comparaison
     réussie en mode PWM 8 bits, phase correcte
     et valeur de TOP fixe à 0xFF (mode #1 de la table 17-6
     page 177 de la description technique du ATmega324PA)*/
    OCR0A = duree;  // met la duree calcule dans le registre de comparaison
    TCNT0 = 0;
    
    //Setup des timers du PWM
    TCCR0A |= (1 << WGM01) | (1 << COM0A0);  //set pour que ca soit en mode ctc pour faire varier la frequence pour le son COM0A0=1,WGM01=1 pour que ca soit en mode toggle
    TCCR0B |=  (1 << CS02); // set le prescale pour 1024 tant que ca depasse pas le size de float
   
}
