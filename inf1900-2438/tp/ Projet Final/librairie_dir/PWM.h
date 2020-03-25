#pragma once

#define F_CPU 8000000 
#include <avr/io.h>

enum Direction { avance, recule, droite, gauche };
void ajustementPWM ( uint8_t pwmA, uint8_t pwmB, Direction direction);
void arreterPWM();

void set();
void arreter();
void tournerDroite(uint8_t intensiteDroite, uint8_t intensiteGauche);
void avancer(uint8_t intensiteDroite, uint8_t intensiteGauche);
void tournerGauche(uint8_t intensiteDroite, uint8_t intensiteGauche);
void reculer(uint8_t intensiteDroite, uint8_t intensiteGauche);
