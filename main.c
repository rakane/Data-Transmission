/*
 * File:   main.c
 * Author: Ryan Kane and Isaiah Plummer
 * Version: 1.0
 */

#include <xc.h>


#define BAUD_RATE 200
#define MESSAGE_LENGTH 5
#define TIMER_SCALAR 0x05


#define TRANSMIT 0  // 1 for transmitter, 0 for reciever


/* Function: delay
 * --------------------------------------------------------
 * Creates delay of length ms
 * 
 * @param ms
 *  length of delay in ms, as timer clock period is 1.02 ms
 */
void delay(int ms){
    TCNT0 = 0x00;   //reset the counter to 0
    OCR0A = ms; //length of delay
    TIFR0 = 0x02;
    while(!(TIFR0 & 0x02)){}
    return;
}

/* Function: waitStart
 * --------------------------------------------------------
 * Waits for start bit from the transmitter, then returns
 */
void waitStart() {
    int prev = 1;
    int new;
    
    new = PINC & 0x01;
    delay(1000 / BAUD_RATE);
    
    // waits for start signal (transition from idle to 0, or stop bit to 0)
    while (!(new == 0 && prev == 1)) {
        prev = new;
        new = PINC;
        delay(1000 / BAUD_RATE);
    }
    
    return;
}

/* Function: sendChar
 * --------------------------------------------------------
 * Sends integer c in form of 8-bit binary number using transmission protocol 
 * 
 * @param ms
 *  length of delay in ms, as timer clock period is 1.02 ms
 */
void sendChar(int c) {
    int tx_bit;
    
    // Send start bit
    PORTD = 0x00;
    delay(1000 / BAUD_RATE);
    
    // loops through each bit in the integer byte
    for(int bit_idx = 0; bit_idx < 8; bit_idx++) {
        tx_bit = c & (0x80 >> bit_idx); 
        
        // if bit is a 1, transmit high
        // else transmit low 
        if(tx_bit) {
            PORTD = 0x01;
        } else {
            PORTD = 0x00;
        }
        delay(1000 / BAUD_RATE);
    }
    
    // Send 2 stop bits
    PORTD = 0x01;
    delay((1000 / BAUD_RATE) * 2);

}

/* Function: readChar
 * --------------------------------------------------------
 * Receives 8-bit binary number using transmission protocol, and returns it
 * 
 * @return
 *  number received from transmitter
 */
int readChar() {
    int input_bit;
    int data_byte = 0;
    int scalar = 128;
    
    waitStart();
    for(int bit_idx = 0; bit_idx < 8; bit_idx++) {
        input_bit = PINC;
        data_byte += scalar * input_bit;
        scalar = scalar / 2;
        delay(1000 / BAUD_RATE);
    }
    // delay during stop signal for right now, instead of detecting
    delay((1000 / BAUD_RATE) * 2);
    
    return data_byte;
}


/* Function: calculateEvenParity
 * --------------------------------------------------------
 * Takes in an integer as a parameter, and returns 1 if it has odd parity,
 * and 0 if it has even parity
 * 
 * @param c
 *  integer parameter, parity bit is calculated for this integer
 * 
 * @return 
 *  1 if parity is odd, 0 if parity is even
 */
int calculateEvenParity(int c) {
    int parity = 0;
    
    while (c) {
       parity = !parity;
       c = c & (c - 1);
    }
    
    return parity;
}

int main(void) {
    // Set GPIO port directions
    DDRB = 0xFF;
    DDRC = 0x00;
    DDRD = 0xFF;
    
    // timer pre-scalar set to 1024
    // Creates a 976.5 Hz clock, or 1.02 ms period
    TCCR0B = TIMER_SCALAR;
    
    if(TRANSMIT) {
        // Transmitter code
        int transmitArray[] = {'H', 'E', 'L', 'L', 'O'};
        while(1){
            PORTD = 0x01;
            delay(255);
            delay(255);
            delay(255);
            delay(255);
            for(int i = 0; i < 5; i++) {
                sendChar(transmitArray[i]);
            }
        }
    } else {
        // Receiver code
        int receiveArray[MESSAGE_LENGTH] = { 0 };
        
        while(1) {
            for(int i = 0; i < 5; i++) {
                receiveArray[i] = readChar();
               // delay((1000 / BAUD_RATE) * 2);
            }
            
            // display message on PORTB
            for(int c = 0; c < MESSAGE_LENGTH; c++) {
                PORTB = receiveArray[c];
                delay(100);
            }
        }
    }
    
    return 0;
}
