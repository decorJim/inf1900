#include "DEL.h"
#include "memoire_24.h"
#include "boutonPoussoir.h"
#include "can.h"
#include "PWM.h"
#include "UART.h"
#include "minuterie.h"
#include <avr/interrupt.h>


int main(){
    DDRA = 0x00; //en entree
    DDRB = 0xff; // en sortie
    DDRD = 0x00;

    DEL delTest(A);
    delTest.allumeDELVert(1000);

    
    Memoire24CXXX mem; //objet memoire

    can convertion; //objet can

    uint8_t message[8] = {'L', 'I', 'B', '2', '4', '3', '8', '\0'};

    uint8_t RecuMess[8];

    while(true)
    {
        if (cliquer()){
            
            _delay_ms(10);
            if (cliquer()){

                DDRD = 0xff; // en sortie
                ajustementPWM(0, 0, avance);
                _delay_ms(2000);
                ajustementPWM(50, 50, recule);
                _delay_ms(2000);
                ajustementPWM(50, 50, gauche);
                _delay_ms(2000);
                ajustementPWM(50, 50, droite);
                _delay_ms(2000);
                ajustementPWM(50, 50, avance);
                _delay_ms(2000);

                DDRD = 0x00;
                PORTD = 0;

                delTest.allumeDELVert(1);

                mem.ecriture(0x00, message, 8);
                _delay_ms(5);
                
                initialisationUART();
                
                mem.lecture(0x00, RecuMess, 8);
                for(uint8_t i = 0; i < 8; i++)
                    transmissionUART(RecuMess[i]);
            }
        } else {
    
            uint16_t converted = (convertion.lecture(0) >> 2);
        
            if (converted >= 240)
            {
                delTest.allumeDELRouge(0);
            } 
            else if (converted >= 200 && converted < 240)
            {
                delTest.allumeDELAmbree(1);
            } 
            else
            {
                delTest.allumeDELVert(0);
            }
        }
    }
}
