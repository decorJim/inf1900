#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

#define RED 0b0000'0100
#define GREEN 0b0000'1000
#define SORTIE 0b1111'1111
#define one_second 1000
#define delay_ambre 1
#define sans_couleur 0b0000'0000

void ambre() 
{
        PORTA = RED;
        _delay_ms(delay_ambre);
        PORTA = GREEN;
        _delay_ms(delay_ambre);
}

int main()
{

  DDRA = SORTIE;

  for (;;) // boucle sans fin
  {
    while (PIND & 0x04)
    {
        _delay_ms(10);
        while(PIND & 0x04)
        {
        ambre();
        }
    }
    PORTA = sans_couleur;
    }
  return 0;
}
