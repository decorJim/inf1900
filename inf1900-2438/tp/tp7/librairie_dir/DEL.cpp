#include "DEL.h"


// indiquer quel port sera utilise
DEL::DEL(PORT port){
  port_ = port;
}

// met le port qu<on utilise
void DEL::setPort(PORT port){
  port_ = port;
}

//allume del vert
void DEL::allumeDELVert(uint8_t duree){
  switch (port_)
  {
    case (A):
      PORTA = VERT;
      break;
    case (B):
      PORTB = VERT;
      break;
    case (C):
      PORTC = VERT;
      break;  
    default:
      PORTD = VERT;
  }
  for(uint16_t i = 0; i < duree; i++) {
    _delay_ms(1);
  }
  if (duree != 0)
    eteindreDel();
}

// allume del rouge
// * par contre, quand la duree est finie, eteint la del
// meme principe pour les autres couleurs

void DEL::allumeDELRouge(uint8_t duree){
  switch (port_)
  {
    case (A):
      PORTA = ROUGE;
      break;
    case (B):
      PORTB = ROUGE;
      break;
    case (C):
      PORTC = ROUGE;
      break;  
    default:
      PORTD = ROUGE;
  }
  for(uint16_t i = 0; i < duree; i++) {
    _delay_ms(1);
  }
  if (duree != 0)
    eteindreDel();
  
}


//Alummer del ambree
void DEL::allumeDELAmbree(uint8_t duree){
  for(uint16_t i = 0; i < duree; i++) {
    switch (port_)
    {
      case (A):
        PORTA = ROUGE;
        _delay_ms(4);
        PORTA = VERT;
        _delay_ms(6);
        break;
      case (B):
        PORTB = ROUGE;
        _delay_ms(4);
        PORTB = VERT;
        _delay_ms(6);
        break;
      case (C):
        PORTC = ROUGE;
        _delay_ms(4);
        PORTC = VERT;
        _delay_ms(6);
        break;  
      default:
        PORTD = ROUGE;
        _delay_ms(4);
        PORTD = VERT;
        _delay_ms(6);
    }
  }
  eteindreDel();
}

// eteindre la DEL
void DEL::eteindreDel(){
  switch (port_)
  {
    case (A):
      PORTA = 0;
      break;
    case (B):
      PORTB = 0;
      break;
    case (C):
      PORTC = 0;
      break;  
    default:
      PORTD = 0;
  }
}