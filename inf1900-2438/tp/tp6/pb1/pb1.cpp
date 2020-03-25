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
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#define RED 0b0000'0100
#define GREEN 0b0000'1000
#define sans_couleur 0b0000'0000
#define SORTIE 0b1111'1111
#define one_second 1000
#define delay_ambre 3

volatile uint8_t etat=1;
volatile uint8_t boutonPoussoir=0;
volatile uint8_t minuterieExpiree;


void initialisation(void)
{
  cli();

  DDRA=0xff;
  DDRD=0x00;

  EIMSK |= (1 << INT0);
  EICRA |= (1 << ISC00);

  sei();
}

ISR(INT0_vect) {

 _delay_ms(30);
  if (!(PIND & 0x04))
  {

    boutonPoussoir = 1;
  }
  else
  {
    boutonPoussoir = 0;
  }
  EIFR |= (1 << INTF0);
}

int main()
{
  initialisation();
  int compteur = 0;

  for(;;)
  {
    switch (etat)
    {

    case 1:

        while ((boutonPoussoir) && compteur < 120)
        {
            compteur = compteur+10;
            _delay_ms(100);
            if ((boutonPoussoir) && compteur == 120)
            {
                EIMSK |= (0 << INT0); // on desactive les interruptions du bouton externe
                etat++;
            }
        }
        break;

    case 2:

      PORTA = GREEN;
      _delay_ms(500);

      PORTA = sans_couleur;
      _delay_ms(2000);
      etat++;
      break;

    case 3:

      for (int i = 0; i < (compteur / 2); i++)
      {
        PORTA=RED;
        _delay_ms(500);

        PORTA=sans_couleur;
        _delay_ms(450);
      }
      etat++;
      break;

    case 4:

      PORTA = GREEN;
      _delay_ms(1000);

      PORTA = sans_couleur;

      boutonPoussoir=0;
      compteur=0;
      etat=0;
      EIMSK|=(1 << INT0); // on reactive les interruptions du bouton externe
      break;
    }
  }
}