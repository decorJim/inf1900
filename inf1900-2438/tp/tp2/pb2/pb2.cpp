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
| etat1         |      0 | etat1        | rouge        |
| etat1         |      1 | etat2        | ambre        |
| etat2         |      1 | etat2        | ambre        |
| etat2         |      0 | etat3        | vert         |
| etat3         |      0 | etat3        | vert         |
| etat3         |      1 | etat4        | rouge        |
| etat4         |      1 | etat4        | rouge        |
| etat4         |      0 | etat5        | sans couleur |
| etat5         |      0 | etat5        | sans couleur |
| etat5         |      1 | etat6        | vert         |
| etat6         |      1 | etat6        | vert         |
| etat6         |      0 | etat1        | rouge        |
+---------------+--------+--------------+--------------
*/

#include <fonction_utile.h>


int main()
{

  DDRA = SORTIE;
  Etat etat = etat1;
  for (;;) // boucle sans fin
  {
    //switch qui permet de suivre le diagramjme a etat au dessus 
    switch (etat)
    {
        case etat1 :
            PORTA = RED;
            if (appuyer())
            {
                _delay_ms(anti_rebond);
                if (appuyer())
                {
                    etat= etat2;    
                }      
            }
            break;

        case etat2 : 
            while (appuyer())
            {
                ambre();
            }
            etat = etat3;    
            break;

        case etat3 :
            PORTA = GREEN; 
            if (appuyer())
            {
                _delay_ms(anti_rebond);
                if (appuyer())
                {
                    etat=etat4;
                }
            }
            break;

        case etat4 :
            while (appuyer())
            {
                PORTA = RED;
            }
            etat = etat5;
            break ;

        case etat5 :        
            PORTA = sans_couleur;
            if (appuyer())
            {
                _delay_ms(anti_rebond);
                if (appuyer())
                {
                    etat=etat6;
                }
            }
            break;

        case etat6 : 
            PORTA = GREEN;
            if (!appuyer())
            {
                etat = etat1; 
            }         
            break;
    }
            
  }
  return 0;
}
