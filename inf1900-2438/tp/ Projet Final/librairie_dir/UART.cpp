#include "UART.h"

void initialisationUART ( void ) {

// 2400 bauds. Nous vous donnons la valeur des deux
// premier registres pour vous éviter des complications

UBRR0H = 0;
UBRR0L = 0xCF;

// permettre la réception et la transmission par le UART0
UCSR0A = 0;

UCSR0B = (1 << RXEN0) | (1 << TXEN0); 
//Writing this bit to one enables the USART Receiver. The Receiver will override normal port operation for the RxDn pin when enabled. Disabling the Receiver will flush the receive buffer invalidating the FEn, DORn, and UPEn Flags.
//Writing this bit to one enables the USART Transmitter. The Transmitter will override normal port operation for the TxDn pin when enabled. The disabling of the Transmitter (writing TXENn to zero) will not become effective until ongoing and pending transmissions are completed, i.e., when the Transmit Shift Register and Transmit Buffer Register do not contain data to be transmitted. When disabled, the Transmitter will no longer override the TxDn port.

// Format des trames: 8 bits, 1 stop bits, none parity

UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); //Mettre a 8 bit

}

void transmissionUART ( uint8_t donnee ) {

  //Attend pour empty transmit buffer
  while (!( UCSR0A & (1 << UDRE0)))  //p.174
  /*Met data dans le buffer, sends the data
  The UDREn Flag indicates if the transmit buffer (UDRn) is ready to receive new data.
  If UDREn is one, the buffer is empty, and therefore ready to be written. 
  The UDREn Flag can generate a Data Register Empty interrupt (see description of the UDRIEn bit).UDREn is set after a reset to indicate that the Transmitter is ready*/
	
	UDR0 = donnee;

}

uint8_t lireUART() {
   
    /* Wait for data to be received */
    while ( !(UCSR0A & (1<<RXC0)) )
        ;
    /* Get and return received data from buffer */
    return UDR0;
    
}