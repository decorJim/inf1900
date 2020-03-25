/*

Nom : El Khiati 
Prenom : Azzahrae 
Matricule : 1886602

Nom : Kim
Prenom : Victor
Matricule : 1954607

Section : 01

Broche : 3 et 4 du PORTA

+---------------+--------+--------------+--------------+
| Etat pressent | Entree | Etat suivant |    Sortie    |
+---------------+--------+--------------+--------------+
| etat1         |      0 | etat1        | sans couleur |
| etat1         |      1 | etat2        | sans couleur |
| etat2         |      1 | etat3        | sans couleur |
| etat2         |      0 | etat2        | sans couleur |
| etat3         |      0 | etat3        | sans couleur |
| etat3         |      1 | etat4        | sans couleur |
| etat4         |      1 | etat5        | sans couleur |
| etat4         |      0 | etat6        | sans couleur |
| etat5         |      0 | etat5        | sans couleur |
| etat5         |      1 | etat6        | sans couleur |
| etat6         |      1 | etat1        | rouge        |
| etat6         |      0 | etat1        | rouge        |
+---------------+--------+--------------+--------------+

*/

#include "fonction_utile.h"

int main() 
{

  DDRA=SORTIE;
  bool appui = false;
  Etat etat = etat1;
    
    for(;;)
    {
      //verifier si le bouton est maintenu (s'il est relacher appui = false)
      if(!appuyer())
        {
          appui = false;
        }
          switch(etat)
            {
              case etat1:
                PORTA = sans_couleur;
                if(appuyer() && appui==false)
                {
                  etat = etat2;
                  appui = true;
                }
                break;

              case etat2:
                if(appuyer() && appui==false)
                {
                  etat = etat3;
                  appui = true;
                }
                break;

              case etat3:
                if(appuyer() && appui==false)
                {
                  etat=etat4;
                  appui = true;
                }
                break;

              case etat4:
                if(appuyer() && appui==false)
                {
                  etat=etat5;
                  appui = true;
                }
                break;

              case etat5:
                if(appuyer() && appui==false)
                {
                  etat =etat6;
                  appui = true;
                }
                break;

              case etat6:
                PORTA= RED;
                _delay_ms(one_second);
                etat= etat1;
                break;
            }
            _delay_ms(anti_rebond);
      }
  return 0; 
}

