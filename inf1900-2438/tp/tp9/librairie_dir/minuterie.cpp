#include "minuterie.h"


void Timer0(uint8_t duree) {
// mode CTC du timer 1 avec horloge divisée par 1024
// interruption après la durée spécifiée
    
    TCNT0 = 0x00; //compteur

    OCR0A = duree; //OCIE1A: Timer/Counter1, Output Compare A Match Interrupt Enable

    TCCR0A = (1 << COM0A0) | (1 << WGM01);

    TCCR0B = (1 << CS02) | (1 << CS00); //prescale a 1024  //tccr -1- c pour cs -1- 2

    TIMSK0 = 1 << OCIE0A; //Le timer a atteint la duree 
}


void Timer1(uint16_t duree) {
 
    TCNT1 = 0x00;
    
    OCR1A = duree; 
    
    TCCR1A = 0;
    
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); 
    
    TCCR1C = 0;
    
    TIMSK1 = 1 << OCIE1A; // pour activer l interruption
}


void Timer2(uint8_t duree) {
 
    TCNT2 = 0x00; 
    
    OCR2A = duree; 
    
    TCCR2A = 0;
    
    TCCR2B = (1 << WGM21) | (1 << CS22) | (1 << CS21) | (1 << CS20);
        
    TIMSK2 = 1 << OCIE2A;
    
}