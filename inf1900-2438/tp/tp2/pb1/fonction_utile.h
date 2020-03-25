#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#define RED 0b0000'0100
#define GREEN 0b0000'1000
#define SORTIE 0b1111'1111
#define ENTREE 0b0000'0000
#define one_second 1000
#define delay_ambre 1
#define sans_couleur 0b0000'0000
#define bouton_poussoir 0x04
#define anti_rebond 10
enum Etat {etat1, etat2, etat3, etat4, etat5, etat6};

//fonction qui alterne rouge et vert rapidement pour donner la couleur ambre  
void ambre() 
{
        PORTA = RED;
        _delay_ms(delay_ambre);
        PORTA = GREEN;
        _delay_ms(delay_ambre);
}

bool appuyer(){
    return (PIND & bouton_poussoir);
}
