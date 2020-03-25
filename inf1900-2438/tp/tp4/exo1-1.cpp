#define F_CPU 8000000
/*#define vert 1
#define rouge 2
#define etein 0*/
#include <avr/io.h> 
#include <util/delay.h>
#include <avr/interrupt.h>

volatile bool appuye = false;
volatile int state = 0;
enum STATES {INIT, STATE0, STATE1, STATE2, STATE3, STATE4};
volatile int currentState = INIT;

void rouge(){
    PORTB = 0b11111110;
}

void ombre(){
    PORTB=0b11111100;
}

void vert(){
    PORTB = 0b11111101;
}
void ambre(){
    rouge(); _delay_ms(10); vert();_delay_ms(10);
}

// placer le bon type de signal d'interruption
// à prendre en charge en argument
ISR (INT0_vect) {
// laisser un delai avant de confirmer la réponse du
// bouton-poussoir: environ 30 ms (anti-rebond)
_delay_ms (30);
currentState++;
if (currentState > 4)
{
    currentState = 0;
}
// Voir la note plus bas pour comprendre cette instruction et son rôle
EIFR |= (1 << INTF0) ;
}



void initialisation (void) {
// cli est une routine qui bloque toutes les interruptions.
// Il serait bien mauvais d'être interrompu alors que
// le microcontroleur n'est pas prêt...
cli ();
// configurer et choisir les ports pour les entrées
// et les sorties. DDRx... Initialisez bien vos variables
DDRD=0x00; //D en entree
DDRB=0xff; //B en sortie
// cette procédure ajuste le registre EIMSK
// de l’ATmega324PA pour permettre les interruptions externes
EIMSK |= (1 << INT0);
// il faut sensibiliser les interruptions externes aux
// changements de niveau du bouton-poussoir
// en ajustant le registre EICRA
EICRA |= (1<<ISC00);
// sei permet de recevoir à nouveau des interruptions.
sei ();
}

int main(){
initialisation();
while(true){
switch(currentState){
        case INIT: 
            rouge();
        break;
        case STATE0:
            ambre();
        break;

        case STATE1:
            vert();
        break;

        case STATE2:
            rouge();
        break;

        case STATE3:
            ombre();
        break;

        case STATE4:
            vert();
        break;
        }
}

return 0;
}