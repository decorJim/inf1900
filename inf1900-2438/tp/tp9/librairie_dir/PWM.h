#pragma once

#define F_CPU 8000000 
#include <avr/io.h>

enum Direction { avance, recule, droite, gauche };
void ajustementPWM ( uint8_t pwmA, uint8_t pwmB, Direction direction);
