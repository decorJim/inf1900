#include "PWM.h"

void ajustementPWM ( uint8_t pwmA, uint8_t pwmB, Direction direction) {

    switch(direction){
        case droite:
            PORTD = 0x40;  //0100 0000  0x40
            break;
        case gauche:
            PORTD =0x80 ;  //1000 0000
            break;
        case avance:
            PORTD =0x30;  //0011 0000
            break;
        case recule:
            PORTD = 0xf0;   //1111 0000
            break;
    }
    
    OCR1A = 255*pwmA/100;
    OCR1B = 255*pwmB/100;
    
    //Setup des timers du PWM
    TCCR1A = (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1);
    TCCR1B = (1 << CS11);
    TCCR1C = 0;
}

void arreterPWM(){
    ajustementPWM(0,0,avance);
}

void tournerGauche(uint8_t intensiteDroite, uint8_t intensiteGauche)
{
    arreter();
    set();
    DDRD |= 0xF0;
    PORTD &= (0xCF);
    PORTD |= (0x10);
    OCR2A = 0xff*intensiteDroite/100.0;
    OCR2B = 0xff*intensiteGauche/100.0;
}

void tournerDroite(uint8_t intensiteDroite, uint8_t intensiteGauche)
{
    arreter();
    set();
    DDRD |= 0xF0;
    PORTD &= (0xCF);
    PORTD |= (0x20);
    OCR2A = 0xff*intensiteDroite/100.0;
    OCR2B = 0xff*intensiteGauche/100.0;
}

void avancer(uint8_t intensiteDroite, uint8_t intensiteGauche)
{
    arreter();
    set();
    DDRD |= 0xF0;
    PORTD &= (0xCF);
    OCR2A = 0xff*intensiteDroite/100.0;
    OCR2B = 0xff*intensiteGauche/100.0;
}

void reculer(uint8_t intensiteDroite, uint8_t intensiteGauche)
{
    arreter();
    set();
    DDRD |= 0xF0;
    PORTD |= 0x30; 
    OCR2A = 0xff*intensiteDroite/100.0;
    OCR2B = 0xff*intensiteGauche/100.0;
}

void arreter(){

    TIMSK2 = 0;
    TCCR2A = 0;
    TCCR2B = 0;
    OCR2A  = 0;
    OCR2B  = 0;

}
void set(){
    TCCR2A |= (1 << WGM20) | (1<<COM2A1) | (1<<COM2B1);
    TCCR2B |= (1 << CS11);
}