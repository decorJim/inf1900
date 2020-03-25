#define F_CPU 8000000
#include "memoire_24.h"
#include "boutonPoussoir.h"
#include "can.h"
#include "DEL.h"
#include "PWM.h"
#include "UART.h"
#include "minuterie.h"
#include "son.h"
#include <avr/interrupt.h>
#include "lcm_so1602dtr_m_fw.h"
#include "customprocs.h"
#include <stdlib.h>
#include <avr/io.h>
#include <stdio.h>

#define ECRAN_DDR DDRC
#define ECRAN_PORT PORTC

enum etat
{
        Boucle,
        Couloir,
        Coupures,
        Mur,
        TransitionMurBoucle,
        TransitionCouloirMur
};
etat etatRobot;
char buffer[10];
bool gaucheADroite = false;
static volatile int pulsation = 0;
uint16_t valeurDebordee = 0;
static volatile int i = 0;
int coupure = 0;

bool CAP1 = false;
bool CAP2 = false;
bool CAP3 = false;
bool CAP4 = false;
bool CAP5 = false;

bool bougeGauche = false;

bool partir1 = true;
bool partir5 = true;

bool aSwitch = false;

bool startCompteur = false;

bool choix;

bool flag = true;
bool transitionFini = false;
int transitionBoucleFini=0;

bool startDetect = false;

int etatBoucle = 0;
int compteurgd = 0;
int compteurCouloir = 0;
int compteurCAP5 = 0;
int valeurCompare = 0;
int choixCommencement = 0;
int compteur2 = 0;
int etatCouloir = 0;

int tempsDeJoindreExtremite = 0;

const uint8_t DEF_DROITE = 65;
const uint8_t DEF_GAUCHE = 63;

bool avancerCoupure = false;
uint8_t avanceD = 0;
uint8_t avanceG = 0;
LCM disp(&ECRAN_DDR, &ECRAN_PORT);

//Detecter les capteurs
void detecter()
{
        CAP1 = PINA & 0b00000001;
        CAP2 = PINA & 0b00000010;
        CAP4 = PINA & 0b00000100;
        CAP3 = PINA & 0b00001000;
        CAP5 = PINA & 0b00010000;
}

void initialisationSonar(void)
{

        cli();
        //Mettre le portA en sortie
        DDRA = 0b1100'0000;

        //Mettre broche 3 portB en mode entree et les autres en sortie
        DDRB = 0b1111'1011;
        EIMSK |= (1 << INT2);
        EICRA |= (1 << ISC20);
        TCCR0B = 0;
        sei();
}

//Interruptions
void initBoutonInterrupt(void)
{
        cli();
        DDRD = 0x00;
        EIMSK |= (1 << INT0);
        EICRA |= (1 << ISC00);
        sei();
}

//Bouton Interrupt
ISR(INT0_vect)
{
        _delay_ms(30);
        if (PIND & 0x04)
        {

                _delay_ms(2000);
                choix = false;
        }
        EIFR |= (1 << INTF0);
}

ISR(INT2_vect)
{

        if (i == 1)
        {
                TCCR0B = 0;
                TIMSK0 &= ~(1 << TOIE0);
                pulsation = TCNT0 + (256 * valeurDebordee);
                TCNT0 = 0;
                i = 0;
        }

        if (i == 0)
        {
                valeurDebordee = 0;
                TCCR0B |= (1 << CS01);
                TIMSK0 |= (1 << TOIE0);
                i = 1;
        }
}

ISR(TIMER0_OVF_vect)
{
        ++valeurDebordee;
}

bool BoutonBreadboard()
{
        // pour trouver le bon pin on fait (numero du port ou fil bleu est branche)^3
        // dans notre cas 3^2 donc !(PIND & 0x08) et ! pour bouton poussoir externe
        if (!(PIND & 0x08))
        {
                _delay_ms(10);
                if (!(PIND & 0x08))
                {
                        return true;
                }
        }
        return false;
}

//Le suiveur de ligne
void suivreLigne()
{
        const uint8_t DEF_DROITED = 65;
        const uint8_t DEF_GAUCHEG = 63;

        if (CAP1)
        {
                avancer(73, 36);
        }

        if (CAP2)
        {
                avancer(68, 46);
        }

        if (CAP3)
        {
                avancer(60, 56);
        }

        if (CAP4)
        {
                avancer(48, 68);
        }

        if (CAP5)
        {
                avancer(35, 76);
        }
}

//Detecter la ligne apres le mur
void MurSon()
{
        detecter();
        while (flag)
        {
                detecter();
                suivreLigne();
                _delay_ms(5);
                if (!CAP1 && !CAP2 && !CAP3 && !CAP4 && !CAP5)
                {
                        flag = false;
                }
        }
        DDRA = 0x00;
        detecter();
        if (CAP1 || CAP2 || CAP3 || CAP4 || CAP5)
        {
                disp.clear();
                etatRobot = TransitionMurBoucle;
                avancer(0, 0);
        }

        initialisationSonar();
        int CompteurDistance = 0;
        DDRB |= _BV(DDB0);
        DDRB &= ~_BV(DDB2);
        PORTB |= 0b0000'0011;
        _delay_us(15);
        PORTB |= 0b0000'0000;
        PORTB |= _BV(PINB0);
        _delay_us(15);
        PORTB &= ~_BV(PINB0);
        CompteurDistance = pulsation / 58;
        _delay_ms(50);
        avancer(57, 57);

        // si proche du mur, tourne droite
        if (CompteurDistance < 13)
        {
                avancer(19, 52);
                _delay_ms(5);
                // allume DEL rouge
                // met un 1 au pin 7 du port A
                PORTA |= (1 << PA6);
                // met un 0 au pin 8 du port A
                PORTA &= ~(1 << PA7);
        }

        else if (CompteurDistance > 17)
        {
                // si loin du mur, tourne gauche
                avancer(48, 19);
                _delay_ms(5);
                // allume DEL rouge
                // met un 1 au pin 7 du port A
                PORTA |= (1 << PA6);
                // met un 0 au pin 8 du port A
                PORTA &= ~(1 << PA7);
        }

        //si bonne intervalle, avancer droit
        else if ((CompteurDistance >= 11) & (CompteurDistance <= 19))
        {
                avancer(50, 50);
                //allume DEL vert
                // met un 0 au pin 7 du port A
                PORTA |= (1 << PA7);
                // met un 1 au pin 8 du port A
                PORTA &= ~(1 << PA6);
                _delay_ms(0.5);
        }
        _delay_ms(5);
}

//Fonction eviter Ligne
void eviteLigne()
{
        detecter();

        sprintf(buffer, "%d", compteur2);
        disp.write(buffer, 16);

        if (bougeGauche)
        {
                avancer(0, 0);
                const uint8_t tempD2 = 0;
                const uint8_t tempG2 = 56;
                avancer(tempD2, tempG2);
                _delay_ms(5);
                avancer(-tempD2, -tempG2);
        }
        else
        {
                const uint8_t tempD = 56;
                const uint8_t tempG = 0;
                avancer(0, 0);
                avancer(tempD, tempG);
                _delay_ms(5);
                avancer(-tempD, -tempG);
        }

        if (CAP1)
        {
                bougeGauche = true;
                if (aSwitch)
                {
                        compteur2++;
                        aSwitch = false;
                }
        }

        if (CAP5)
        {
                bougeGauche = false;
                if (aSwitch == false)
                {
                        compteur2++;
                        aSwitch = true;
                }
        }
}

// Code pour la boucle
void boucle()
{
        const uint8_t puissanceD = 48;
        const uint8_t puissanceG = 51;
        const uint8_t puissanceDLeft = 49;
        _delay_ms(5);

        if (etatBoucle == 0)
        {

                suivreLigne();
                if (CAP1 && CAP2)
                {
                        avancer(0, 0);
                        etatBoucle = 1;
                }
        }
        if (etatBoucle == 1)
        {

                avancer(puissanceD, puissanceG);
                if (!CAP1)
                {
                        avancer(0, 0);
                        etatBoucle = 2;
                }
        }
        if (etatBoucle == 2)
        {

                suivreLigne();
                if (CAP1 && CAP2)
                {
                        avancer(0, 0);
                        etatBoucle = 3;
                }
        }
        if (etatBoucle == 3)
        {

                avancer(puissanceD, puissanceG);
                if (!CAP1)
                {
                        avancer(0, 0);
                        etatBoucle = 4;
                }
        }
        if (etatBoucle == 4)
        {
                suivreLigne();
                if (CAP1)
                {
                        _delay_ms(5);
                        etatBoucle = 42;
                }
        }
        if (etatBoucle == 41)
        {
                avancer(puissanceD, puissanceG);
                if (!CAP1)
                {
                        _delay_ms(250);
                        if (!CAP1)
                        {
                                etatBoucle = 42;
                                _delay_ms(5);
                        }
                }
        }
        if (etatBoucle == 42)
        {
                avancer(0, 0);
                detecter();
                suivreLigne();
                if (CAP1)
                {
                        etatBoucle = 43;
                }
        }
        if (etatBoucle == 43)
        {
                avancer(puissanceD, puissanceG);
                if (!CAP1)
                {
                        _delay_ms(250);
                        if (!CAP1)
                        {
                                etatBoucle = 5;
                                _delay_ms(5);
                        }
                }
        }
        if (etatBoucle == 5)
        {

                const uint8_t tempD = puissanceD - 8;
                const uint8_t tempG = 0;
                avancer(tempD, tempG);
                _delay_ms(5);
                detecter();
                if (CAP1 && !CAP2)
                {
                        etatBoucle = 6;
                }
        }
        if (etatBoucle == 6)
        {
                _delay_ms(5);

                suivreLigne();
                detecter();
                if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                {
                        etatBoucle = 7;
                }
        }
        if (etatBoucle == 7)
        {

                const uint8_t tempD = puissanceD - 8;
                const uint8_t tempG = 0;
                avancer(tempD, tempG);
                _delay_ms(5);
                detecter();
                if (CAP1 && !CAP2)
                {
                        etatBoucle = 8;
                }
        }
        if (etatBoucle == 8)
        {

                suivreLigne();
                if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                {
                        _delay_ms(50);
                        if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                        {

                                etatBoucle = 9;
                        }
                }
        }
        if (etatBoucle == 9)
        {

                const uint8_t tempD = puissanceD - 8;
                const uint8_t tempG = 0;
                avancer(tempD, tempG);
                _delay_ms(5);
                detecter();
                if (CAP1 && !CAP2)
                {
                        etatBoucle = 10;
                }
        }
        if (etatBoucle == 10)
        {

                suivreLigne();
                if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                {
                        _delay_ms(50);
                        if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                        {

                                etatBoucle = 11;
                        }
                }
        }

        if (etatBoucle == 11)
        {

                avancer(puissanceD, puissanceG);
                if (!CAP1)
                {
                        avancer(0, 0);
                        etatBoucle = 12;
                }
        }

        if (etatBoucle == 12)
        {

                suivreLigne();
                if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                {
                        _delay_ms(400);
                        avancer(puissanceD+15, puissanceG);
                        if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                        {

                                etatBoucle = 13;
                                avancer(puissanceD+12, puissanceG);
                        }
                }
        }

        if (etatBoucle == 13)
        {
                detecter();
                const uint8_t tempD = puissanceD-9;
                const uint8_t tempG = 0;
                avancer(tempD, tempG);
                _delay_ms(5);

                if (CAP1)
                {
                        _delay_ms(10);
                        if (CAP1)
                        {
                                etatBoucle = 14;
                        }
                }
        }

        if (etatBoucle == 14)
        {

                suivreLigne();
                if (CAP1)
                {

                        etatBoucle = 15;
                }
        }

        if (etatBoucle == 15)
        {
                const uint8_t tempDD = puissanceD;
                avancer(100, 0);
                if (!CAP1)
                {
                        avancer(0, 0);
                        etatBoucle = 16;
                }
        }

        if (etatBoucle == 16)
        {

                suivreLigne();
                if (CAP1)
                {
                        avancer(0, 0);
                        etatBoucle = 17;
                         const uint8_t tempD = puissanceD + 15;
                        const uint8_t tempG = 0;
                        avancer(tempD, tempG);
                }
        }

        if (etatBoucle == 17)
        {

                const uint8_t tempD = puissanceD + 2;
                const uint8_t tempG = 0;
                avancer(tempD, tempG);
                if (CAP1)
                {
                        avancer(0, 0);
                        etatBoucle = 19;
                }
        }

        if (etatBoucle == 19)
        {

                suivreLigne();
                if (CAP1)
                {
                        avancer(0, 0);

                        etatBoucle = 191;
                        _delay_ms(1000);
                }
        }

        if (etatBoucle == 191)
        {

                avancer(puissanceD, puissanceG);
                if (!CAP1)
                {
                        _delay_ms(50);
                        if (!CAP1)
                        {

                                avancer(0, 0);
                                etatBoucle = 20;
                        }
                }
        }

        if (etatBoucle == 20)
        {

                avancer(puissanceD, 0);
                if (CAP1)
                {
                        avancer(0, 0);
                        etatBoucle = 21;
                }
        }

        if (etatBoucle == 21)
        {

                suivreLigne();
                if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                {
                        _delay_ms(10);
                        if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                        {

                                etatBoucle = 22;
                        }
                }
        }

        if (etatBoucle == 22)
        {

                avancer(puissanceD - 8, 0);
                if (CAP1)
                {
                        avancer(0, 0);
                        etatBoucle = 23;
                }
        }

        if (etatBoucle == 23)
        {

                suivreLigne();
                if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                {
                        _delay_ms(10);
                        if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                        {

                                etatBoucle = 24;
                        }
                }
        }

        if (etatBoucle == 24)
        {

                avancer(puissanceD - 5, 0);
                if (CAP1)
                {
                        avancer(0, 0);
                        etatBoucle = 25;
                }
        }

        if (etatBoucle == 25)
        {

                ;
                suivreLigne();
                if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                {
                        _delay_ms(10);
                        if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                        {

                                etatBoucle = 26;
                        }
                }
        }

        if (etatBoucle == 26)
        {

                avancer(puissanceD - 5, 0);
                if (CAP1)
                {
                        avancer(0, 0);
                        etatBoucle = 27;
                }
        }

        if (etatBoucle == 27)
        {

                ;
                suivreLigne();
                if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                {
                        _delay_ms(10);
                        if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                        {

                                etatBoucle = 28;
                        }
                }
        }

        if (etatBoucle == 28)
        {

                avancer(puissanceD - 0, 0);
                if (CAP2)
                {
                        avancer(0, 0);
                        etatBoucle = 29;
                }
        }

        if (etatBoucle == 29)
        {

                _delay_ms(1000);
                choixCommencement++;
                disp.clear();
                disp.write("Coupures");
                etatRobot = Coupures;
        }
}

void couloir()
{
        const uint8_t gauchere = 10;
        const uint8_t droitere = 55;

        while (true)
        {
                //suivre la ligne avant l"arrivage des lignes paralleles
                detecter();
                suivreLigne();
                _delay_ms(5);
                //si les 3 capteurs du milieu ne sont pas allumes, avance pendant 200ms
                if (!CAP1 && !CAP2 && !CAP3 && !CAP4 && !CAP5)
                {
                        _delay_ms(30);
                        detecter();
                        if (!CAP1 && !CAP2 && !CAP3 && !CAP4 && !CAP5)
                        {
                                avancer(gauchere, droitere);
                                _delay_ms(200);

                                break;
                        }
                }
        }
        //rester entre les lignes paralleles
        while (true)
        {
                detecter();
                if (compteur2 < 15)
                {
                        eviteLigne();
                        _delay_ms(5);
                }
                if (compteur2 >= 15)
                {

                        avancer(0, 0);
                        _delay_ms(1000);
                        while (true)
                        {
                                _delay_ms(5);
                                avancer(droitere - 5, gauchere);
                                detecter();
                                if (CAP2 || CAP3)
                                {

                                        avancer(0, 0);
                                        _delay_ms(2000);
                                        break;
                                }
                        }
                        break;
                }
                avancer(0, 0);
        }

        avancer(0, 0);

        while (true)
        {
                detecter();
                suivreLigne();
                _delay_ms(5);
                if (!CAP1 && !CAP2 && !CAP3 && !CAP4 && !CAP5)
                {
                        break;
                }
        }

        while (true)
        {
                detecter();
                avancer(droitere, 0);
                _delay_ms(5);
                detecter();
                if (CAP2 || CAP3)
                {
                        avancer(0, 0);
                        choixCommencement++;
                        _delay_ms(1000);
                        disp.clear();
                        etatRobot = Mur;
                        break;
                }
        }
}

void Coupure()
{

        const uint8_t droitereValeur = 65;
        const uint8_t gauchereValeur = 63;
        const uint8_t droitereValeurD = 58;
        const uint8_t gauchereValeurG = 56;
        if (coupure == 0)
        {
                suivreLigne();
                _delay_ms(5);
                if (avancerCoupure == false)
                {
                        detecter();
                        if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                        {
                                _delay_ms(50);
                                detecter();
                                if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                                {

                                        avancerCoupure = true;
                                        sonOff();
                                }
                        }
                }
                if (avancerCoupure)
                {
                        const uint8_t gauchere = 0;
                        const uint8_t droitere = droitereValeurD;
                        avancer(gauchere, droitere);
                        ajustementSon(110); //SON -------------------------------- AIGU
                        _delay_ms(5);
                        detecter();
                        if (CAP1 || CAP2 || CAP3 || CAP4 || CAP5)
                        {
                                _delay_ms(5);
                                detecter();
                                if (CAP1 || CAP2 || CAP3 || CAP4 || CAP5)
                                {
                                        coupure = 2;
                                        avancerCoupure = false;

                                        sonOff();
                                        arreter();
                                        _delay_ms(500);
                                }
                        }
                }
        }
        if (coupure == 2)
        {
                suivreLigne();
                _delay_ms(5);
                if (avancerCoupure == false)
                {
                        detecter();
                        if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                        {
                                _delay_ms(50);
                                detecter();
                                if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                                {

                                        avancerCoupure = true;
                                }
                        }
                }
                if (avancerCoupure)
                {
                        const uint8_t gauchere = gauchereValeurG;
                        const uint8_t droitere = 0;
                        avancer(gauchere, droitere);
                        ajustementSon(0); //SON -------------------------------- GRAVE
                        _delay_ms(5);
                        detecter();
                        if (CAP1 || CAP2 || CAP3 || CAP4 || CAP5)
                        {
                                _delay_ms(5);
                                detecter();
                                if (CAP1 || CAP2 || CAP3 || CAP4 || CAP5)
                                {
                                        coupure = 3;
                                        avancerCoupure = false;

                                        sonOff();
                                        arreter();
                                        _delay_ms(500);
                                }
                        }
                }
        }
        if (coupure == 3)
        {
                suivreLigne();
                _delay_ms(5);
                if (avancerCoupure == false)
                {
                        detecter();
                        if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                        {
                                _delay_ms(50);
                                detecter();
                                if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                                {

                                        avancerCoupure = true;
                                }
                        }
                }
                if (avancerCoupure)
                {
                        const uint8_t gauchere = 0;
                        const uint8_t droitere = droitereValeurD;
                        avancer(gauchere, droitere);
                        ajustementSon(110); //SON -------------------------------- AIGU
                        _delay_ms(5);
                        detecter();
                        if (CAP1 || CAP2 || CAP3 || CAP4 || CAP5)
                        {
                                _delay_ms(5);
                                detecter();
                                if (CAP1 || CAP2 || CAP3 || CAP4 || CAP5)
                                {
                                        sonOff();
                                        coupure = 4;
                                        avancerCoupure = false;

                                        arreter();
                                        _delay_ms(500);
                                }
                        }
                }
        }
        if (coupure == 4)
        {
                suivreLigne();
                _delay_ms(5);
                if (avancerCoupure == false)
                {
                        detecter();
                        if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                        {
                                _delay_ms(80);
                                detecter();
                                if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                                {

                                        avancerCoupure = true;
                                }
                        }
                }
                if (avancerCoupure)
                {
                        const uint8_t gauchere = gauchereValeurG;
                        const uint8_t droitere = 0;
                        avancer(gauchere, droitere);
                        ajustementSon(0); //SON -------------------------------- GRAVE
                        _delay_ms(5);
                        detecter();
                        if (CAP1 || CAP2 || CAP3 || CAP4 || CAP5)
                        {
                                _delay_ms(5);
                                detecter();
                                if (CAP1 || CAP2 || CAP3 || CAP4 || CAP5)
                                {
                                        sonOff();
                                        coupure = 5;
                                        avancerCoupure = false;

                                        arreter();
                                        _delay_ms(500);
                                }
                        }
                }
        }
        if (coupure == 5)
        {
                suivreLigne();
                _delay_ms(5);
                if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                {
                        _delay_ms(50);
                        detecter();
                        if (!CAP1 & !CAP2 & !CAP3 & !CAP4 & !CAP5)
                        {

                                coupure = 6;
                        }
                }
        }

        if (coupure == 6)
        {
                detecter();
                const uint8_t puissanceD = 0;
                const uint8_t puissanceG = 50;
                avancer(puissanceG, puissanceD);
                if (CAP3)
                {
                        avancer(0, 0);
                        _delay_ms(1000);
                        coupure = 7;
                }
        }
        if (coupure == 7)
        {
                choixCommencement++;
                disp.clear();
                disp.write("Couloir");
                etatRobot = Couloir;
        }
}

void transitionMurBoucle()
{
        uint8_t tempD = 58;
        uint8_t tempG = 58;
        const uint8_t puissanceD = 48;
        const uint8_t puissanceG = 50;

        if (transitionFini == false)
        {

                avancer(tempD, tempG);
                if (!CAP1 && !CAP2 && !CAP3 && !CAP4 && !CAP5)
                {
                        _delay_ms(50);
                        if (!CAP1 && !CAP2 && !CAP3 && !CAP4 && !CAP5)
                        {
                                avancer(0, 0);
                                _delay_ms(1000);

                                transitionFini = true;
                        }
                }
        }

        if (transitionFini)
        {
                avancer(0, tempG-8);
                detecter();
                if (transitionBoucleFini==0) {
                        if (CAP4)
                        {
                        transitionBoucleFini=1;
                        }
                }

                if(transitionBoucleFini==1) {
                detecter();
                suivreLigne();
                _delay_ms(5);
                        if (!CAP1 && !CAP2 && !CAP3 && !CAP4 && !CAP5)
                        {
                                _delay_ms(50);
                                if (!CAP1 && !CAP2 && !CAP3 && !CAP4 && !CAP5)
                                        {
                                                avancer(puissanceD-10, puissanceG-10);
                                                _delay_ms(100);
                                                avancer(0, 0);
                                                _delay_ms(5);
                                                transitionBoucleFini=2;
                                        }
                                }
                }
                        
                        if (transitionBoucleFini == 2)
                        {
                                avancer(tempD-9, 0);
                                _delay_ms(5);
                                detecter();
                                if (CAP3)
                                {
                                        avancer(0, 0);
                                        arreter();
                                        choixCommencement++;
                                        disp.clear();
                                        etatRobot = Boucle;
                                }
                        }
                
        }
}

bool poussoir()
{
        if (PIND & 0x04)
        {
                _delay_ms(10);
                if (PIND & 0X04)
                {
                        return true;
                }
        }
}

void transitionCouloirMur()
{
        const uint8_t gaucheree = 60;
        const uint8_t droiteree = 0;
        avancer(droiteree, gaucheree);
        _delay_ms(5);
        if (CAP1 || CAP2 || CAP3 || CAP4 || CAP5)
        {
                etatRobot = Mur;
        }
}

int main()
{
        initBoutonInterrupt();

        int compteur = 0;
        uint8_t tempD = 55;
        uint8_t tempG = 55;
        int j = 1;
        choix = true;
        //tempD = tempD*j;
        //tempG = tempG*j;
        //DDRA = 0b1100'0000;
        //PORTA = 0x00;
        etatRobot = Mur;

        while (choix)
        {
                switch (etatRobot)
                {
                case Mur:
                        disp.write("Mur");
                        break;

                case Couloir:
                        disp.write("Couloir");
                        break;

                case Coupures:
                        disp.write("Coupure");
                        break;

                case Boucle:
                        disp.write("Boucle");
                        break;
                }
                if ((BoutonBreadboard()))
                {
                        if (etatRobot == Mur)
                        {
                                disp.clear();
                                etatRobot = Boucle;
                        }
                        else if (etatRobot == Boucle)
                        {
                                disp.clear();
                                etatRobot = Coupures;
                        }
                        else if (etatRobot == Coupures)
                        {
                                disp.clear();
                                etatRobot = Couloir;
                        }
                        else if (etatRobot == Couloir)
                        {
                                disp.clear();
                                etatRobot = Mur;
                        }
                        _delay_ms(200);
                }
        }

        while (true)
        {
                if (choixCommencement == 4)
                {
                        disp.clear();
                        disp.write("fin");
                        while (true)
                        {
                        }
                }

                //suivreLigne();
                detecter();
                switch (etatRobot)
                {
                case Mur:
                        disp.write("Mur");
                        MurSon();
                        break;

                case Couloir:
                        disp.write("Couloir");
                        couloir();
                        break;

                case Coupures:
                        disp.write("Coupures");
                        Coupure();
                        break;

                case Boucle:
                        disp.write("Boucle");
                        boucle();
                        break;

                case TransitionMurBoucle:
                        disp.write("Mur");
                        transitionMurBoucle();
                        break;

                case TransitionCouloirMur:
                        disp.write("Couloir");
                        transitionCouloirMur();
                        break;
                }
        }
}
