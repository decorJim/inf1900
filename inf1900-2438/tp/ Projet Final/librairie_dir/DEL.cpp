#include "DEL.h"


// indiquer quel port sera utilise
DEL::DEL(PORT port){
  port_ = port;
}

// met le port quon utilise
void DEL::setPort(PORT port){
  port_ = port;
}

// fonction eteindre la DEL
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

//fonction allumer del vert
// *** par contre, quand la duree est finie, eteint la del
// meme principe pour les autres couleurs
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
  for(uint16_t i = 0; i < duree*100; i++) {
    _delay_ms(10);
  }
  if (duree != 0)
    eteindreDel();
}

// allume del rouge
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
  for(uint16_t i = 0; i < duree*100; i++) {
    _delay_ms(10);
  }
  if (duree != 0)
    eteindreDel();
  
}


//Alummer del ambree
void DEL::allumeDELAmbree(uint8_t duree){
  for(uint16_t i = 0; i < duree*100; i++) {
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
