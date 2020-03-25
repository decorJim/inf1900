#include "boutonPoussoir.h"

// Permet de savoir si le bouton est clique ou non, incluant anti-rebond
bool cliquer(){
  return (PIND & 0x04);
}



