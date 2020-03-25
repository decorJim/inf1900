#include "memoire_24.h"
#include "boutonPoussoir.h"
#include "can.h"
#include "DEL.h"
#include "PWM.h"
#include "UART.h"
#include "minuterie.h"
#include "son.h"
#include <avr/interrupt.h>
#include "son.h"


enum instructions{

    DBT = 0x01, // debuter
    ATT = 0x02, // attendre
    DAL = 0x44, // allumer DEL
    DET = 0x45, // eteindre DEL
    SGO = 0x48, // jouer Sonorite
    SAR = 0x09, // arreter Sonorite
    MAR1 = 0x60, // arreter premier Moteur
    MAR2 = 0x61, // arreter deuxieme Moteur
    MAV = 0x62, // avancer
    MRE = 0x63, // reculer
    TRD = 0x64, // tourner droite
    TRG = 0x65, // tourner gauche
    DBC = 0xC0, // debut de la boucle
    FBC = 0xC1, // fin de la boucle
    FIN = 0xFF, // fin
};


int main(){

    DEL del(A); // DEL au port A
    uint8_t memLu;
    uint8_t nbInstruction;
    uint8_t commande;
    uint8_t operande;
    Memoire24CXXX mem; //objet memoire

    initialisationUART(); // initialiser UART
    DDRA = 0xff; //en mode sortie
    DDRB = 0xff; 
    DDRD = 0xff;

    uint8_t addresse = 0x02;
    uint8_t compteur = 0;
    uint8_t addresseBoucle = 0;
    uint16_t nbInstructionBoucle = 0;

    
    //On stock la valeur lue de 8 bits a l'adresse 0x00 dans la variable memoireLu, puis on stock memoireLu dans nbInstruction en decalant de 8 bits
    //pour additionner ensuite la valeur lu de 8 bits egalement a l'adresse 0x01 pour avoir le nb d'octets contenu dans le fichier binaire.
    //Puis on soustrait les 2 premiers octets (qui donne une information sur le nb d'octet dans le fichier) a nbInstruction puis on divise par 2 pour avoir ne pas compter le nb d'operande aussi
    mem.lecture(0x00, &memLu); 
    nbInstruction = memLu << 8; 
    mem.lecture(0x01, &memLu); 
    nbInstruction += memLu;
    nbInstruction = (nbInstruction-2)/2; 


    bool debut = false;
    // Boucle pour lire les lignes qui restent (toutes les instructions/operandes)
    for (uint16_t i =0; i< nbInstruction; i++) {
        mem.lecture(addresse++, &commande); // lire le 1er octet de la ligne i+1 (instruction)
        mem.lecture(addresse++, &operande); // lire le 2e octet de la ligne i+1 (operande)


        // Code detecte que le 1ere commande est DBT, il allume la DEL en vert
        if (commande == DBT && !debut){
            debut = true;
            del.allumeDELVert(1,0);
        }

        if (debut){
            switch (commande){
                case ATT:
                    for(uint8_t k = 0; k < operande; k++) {
                        _delay_ms(25); //operande 10 = 1/4 de seconde (25ms)
                    }
                    break;
                case DAL:
                    del.allumeDELRouge(1,0);
                    break;
                case DET:
                    del.eteindreDel();
                    break;
                case SGO:
                    if (operande >= 45 && operande <= 81){
                        ajustementSon(operande);
                    }
                    break;
                case SAR:
                    TCCR0A = 0;
                    break;

                case MAR1:
                case MAR2:
                    ajustementPWM(0, 0, avance); // il avance, mais les deux roues sont  a 0,0 --­­­> donc arrete
                    break;
                case MAV:
                    ajustementPWM(operande/255.0 * 100, operande/255.0 * 100, avance); // avancer
                    break;
                case MRE:
                    ajustementPWM(operande/255.0 * 100, operande/255.0 * 100, recule); // reculer
                    break;
                case TRD:
                    ajustementPWM(100, 100, droite); // tourner a droite
                    _delay_ms(385);
                    ajustementPWM(0, 0, droite);
                    break;
                case TRG:
                    ajustementPWM(100, 100, gauche); // tourner a gauche
                    _delay_ms(385);
                    ajustementPWM(0, 0, gauche);
                    break;
                case DBC: // debut de la boucle
                    if (compteur == 0){
                        addresseBoucle = addresse;
                        compteur = operande;
                        nbInstructionBoucle = i;
                    }
                    break;
                case FBC: // fin de la boucle
                    if(compteur != 0){
                        addresse = addresseBoucle;
                        compteur--;
                        i = nbInstructionBoucle;
                    }
                    break;
                case FIN: // fin
                    
                    TCCR0A = 0;
                    ajustementPWM(0, 0, avance);
                    del.allumeDELRouge(1,0);
                    debut = false;
                    break;
            }
        }

        
    }

}
