#pragma once 
#ifndef F_CPU
#define F_CPU 8000000
#endif
#include <util/delay.h>
#include <avr/io.h>

enum Couleur{
    ROUGE = 0xAA, VERT = 0x55, AMBREE
};

enum PORT{
  A, B, C, D
};

class DEL{
    
public:
    DEL(PORT port);
    void setPort(PORT port);
    void allumeDELRouge(uint8_t duree, uint8_t timer);
    void allumeDELVert(uint8_t duree, uint8_t timer);
    void allumeDELAmbree(uint8_t duree, uint8_t timer);
    void eteindreDel();
private:
    PORT port_;
};
