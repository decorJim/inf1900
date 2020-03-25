/*

Nom : El Khiati 
Prenom : Azzahrae 
Matricule : 1886602

Nom : Kim
Prenom : Victor
Matricule : 1954607

Section : 01

Broche : 3 et 4 du PORTA
*/

#define RED 0b0000'0100
#define GREEN 0b0000'1000
#define SORTIE 0b1111'1111
#define ENTREE 0b0000'0000
#define delay_ambre 1
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/io.h>
#include "can.h"

int main()
{
  DDRA = ENTREE;
  DDRB = SORTIE;
  can convertisseur;
  
  uint8_t luminosite; // valeur lue par le capteur 
  uint8_t max = 62; // valeur de transition exact trouver par tatonnement 
  uint8_t min = 49;
  for (;;)
  {
    luminosite = convertisseur.lecture(0);
    luminosite = luminosite >> 2;

    if (luminosite >= max)
      PORTB = RED;

    else if (luminosite > min && luminosite < max)
    {
      PORTB = GREEN;
      _delay_ms(delay_ambre);
      PORTB = RED;
      _delay_ms(delay_ambre);
    }
    else if (luminosite <= max)
    {
      PORTB = GREEN;
    }
  }
}