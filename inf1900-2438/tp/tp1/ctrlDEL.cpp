#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

#define RED 0b0000'0100
#define GREEN 0b0000'1000
#define SORTIE 0b1111'1111
#define one_second 1000
#define delay_ambre 3

int main()
{

  DDRA = SORTIE;

  for (;;) // boucle sans fin
  {
    PORTA = RED;
    _delay_ms(one_second);
    PORTA = GREEN;
    _delay_ms(one_second);

    //boucle for qui alterne rouge vert rapidement pour donner ambre 

    for (int i = 0; i < 300; i++)
    {
      PORTA = 0b0000'0100;
      _delay_ms(delay_ambre);
      PORTA = 0b0000'1000;
      _delay_ms(delay_ambre);
    }
  }
  return 0;
}
