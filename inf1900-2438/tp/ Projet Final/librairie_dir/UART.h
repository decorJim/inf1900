#pragma once
#include <avr/io.h>

void initialisationUART(void);
void transmissionUART(uint8_t donnee);
uint8_t lireUART();